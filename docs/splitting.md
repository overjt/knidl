# Splitting ROM ranges into labeled assembly

`tools/split.py` (issue #23) converts verbatim `.incbin` blob segments into
labeled assembler files that re-assemble **byte-for-byte**, giving every
function in the range a symbol. This is the classic pret "split" step: once a
range is split, `build/knidl.map` shows every function (so `asmdiff.sh`,
`asm-differ`, and link-time references work), and C migration can replace
functions one by one.

All SDK/ARM segments around the code region are split (issues #23/#24);
`data/` incbin slices remain only for bulk asset zones. The currently
configured segments (`tools/split_config.json`):

| segment              | range                        | contents                          |
| -------------------- | ---------------------------- | --------------------------------- |
| `task_switch_helpers`| `0x08000234-0x080002E5`      | cooperative task switch (ARM, 4 helpers + 1-byte tail) |
| `task_literals`      | `0x080002E5-0x08000310`      | their literal pools (10 words, all named cells) |
| `sdk_swi_wrappers`   | `0x080CFA40-0x080CFA7F`      | 11 Thumb SWI thunks (svc wrappers)|
| `sdk_reset_helper`   | `0x080CFA7F-0x080CFA9C`      | odd-start reset helper tail + pool|
| `sdk_libc`           | `0x080CFC30-0x080CFDDC`      | `_call_via_r0..lr`, division/modulo, task trampolines |
| `interworking_veneer`| `0x080CFDDC-0x080CFDE4`      | ARM `ldr ip,[pc]; bx ip` -> `0x08005654\|1` |
| `gap_interworking_veneer_irq_handler_table_14` | `0x080CFDE4-0x080CFDE8` | the veneer's literal word (not padding) |
| `irq_handler_table_14` | `0x080CFDE8-0x080CFE20`    | 14-entry IRQ handler pointer table|
| `lib_misc`           | `0x080CFE20-0x080CFF00`      | SRAM id string + sound-driver coefficient windows |
| `lib_rodata_fir_tables` | `0x080CFF00-0x080D0000`   | FIR/envelope-style coefficient tables |

## Usage

```sh
make split      # runs tools/split.py inside the knidl-builder image
make compare    # must stay byte-identical (SHA-1 vs knidl.sha1)
```

`make split` reads three committed inputs:

* `tools/split_config.json` — which segments to split, plus:
  * `external_defined`: symbols that already exist as real labels elsewhere
    (`asm/crt0.s`, `src/agb_sram.c`) and must not be redefined as absolute
    symbols;
  * `extra_labels`: `{address: name}` — force a real `.global` label at an
    address inside a configured segment, for functions absent from
    symbols.csv because nothing `bl`s them (the libgcc `_call_via_r4..lr`
    half of lesson 3.4's family) or for named items inside data segments
    (`gSramIdString`);
  * `data_symbols`: `{value: name}` — non-ROM word values emitted
    symbolically wherever they appear as pool/data words (the task system's
    IWRAM cells); definitions are appended to `asm/rom_syms.s`.
* `docs/analysis/segments.txt` — segment boundaries and kinds (the single
  source of truth; the config only selects segments by name);
* `docs/analysis/symbols.csv` — the function database (issue #22).

Hand names must always go through the config, never by editing generated
files: CI re-runs `make split` and fails on any diff in `asm/` or `data/`,
so committed outputs must equal regeneration byte-for-byte.

and regenerates:

* `asm/<segment>.s` — one file per configured segment (committed);
* `asm/rom_syms.s` — absolute symbols (`name = 0xADDR`) for every database
  function not defined by a real label, plus the `data_symbols` definitions
  (committed);
* it deletes the obsolete `data/<segment>.s` incbin slice.

The tool verifies each output itself before writing it (see below), so
`make split` never leaves a non-matching file behind. `make compare` remains
the authoritative check.

## How a segment is emitted

* Ranges covered by a database function become code: `.thumb`/`.arm` mode,
  `.global` with the database name (`Div`, `sub_080cfa60`, ...), and
  `.thumb_func` for Thumb entries.
* Every other range becomes labeled data: `gUnk_XXXXXXXX` labels over
  `.word`/`.short`/`.byte` directives.
* Literal pools are detected by decoding pc-relative `ldr` targets from the
  ROM bytes (not by heuristics on disassembly text) and emitted as `.word`,
  symbolically when the value is a known function pointer:
  `.word sub_08001518+1` (Thumb pointer), `.word MasterIsr` (ARM pointer).
  `asm/rom_syms.s` provides the absolute symbol when the target is not yet
  split, so references resolve at link time with identical bytes.
* Branches are re-written to labels: in-file function names, local `.L_XXXXXXXX`
  labels for intra-file targets without database entries, or database names
  for external targets. A `@ 0x........` comment preserves the target address.
* Non-function pointer words (MMIO/IWRAM addresses, plain rodata) stay
  numeric: `.word 0x04000208`.

### Odd boundaries

gas silently pads a section's **size** up to its alignment (2 for a section
containing Thumb instructions, 4 for ARM) and aligns every Thumb instruction
to 2. Two segment-boundary oddities in this ROM therefore need explicit
handling (both occur in the demo set):

* **Odd trailing byte** (`sdk_swi_wrappers` ends at `0x080CFA7F`; the final
  `bx lr` of `SoundDriverVSyncOff` is split across the segment boundary):
  the trailing byte(s) are parked in a separate alignment-1 section
  `.segment.tail`, and every segment rule in `linker.ld` carries a
  `KEEP(*(.segment.tail))` pattern that appends it after the main content.
* **Odd segment start** (`sdk_reset_helper` begins at `0x080CFA7F`, the
  second half of the previous segment's `bx lr`; `game_code_early` at
  `0x080006FF` is the same shape): real instructions cannot be placed at odd
  section offsets (gas would pad), so such segments are emitted from raw
  `.short`/`.byte`/`.word` data with labels but no instruction text. Pure
  data directives do not pad, so the bytes stay exact.

## Byte-identity verification

Each run assembles and links **all configured segments as a group** before
writing anything (a split segment may reference labels that another split
segment defines, e.g. the IRQ table pointing into `game_code_early`):

1. `arm-none-eabi-as -mcpu=arm7tdmi` assembles every candidate file
   (per segment, real instructions first, raw fallback if it will not
   assemble);
2. `arm-none-eabi-ld` links all candidate objects together with a
   throwaway linker script that pins every section at its ROM VMA, plus
   `rom_syms.o` and `--defsym` stand-ins for symbols that compiled C
   defines (`ReadSram`, ...);
3. `arm-none-eabi-objcopy --dump-section` extracts each section and the
   bytes are compared with the original `baserom.gba` slice;
4. segments that do not match are re-emitted from raw `.short`/`.byte`
   (a verbatim byte copy) and the group is verified again — so output is
   always byte-identical; the fallback only costs readability and is
   flagged in the `make split` log. Instructions that cannot be resolved
   individually (e.g. a branch to an address with no database entry) fall
   back to raw halfwords one instruction at a time.

`make compare` remains the authoritative end-to-end check. Note that a
segment whose *start* is odd (e.g. `game_code_early` at `0x080006FF`) is
always emitted as labeled raw data — gas would silently pad real
instructions to even offsets — and that odd *trailing* bytes live in the
`.tail` section described above. A stress run over `task_switch_helpers`
(ARM), `agb_init` (1 KB of Thumb with branches and pools) and
`game_code_early` (odd start, 162 functions) verified all three paths.

Notes on round-tripping objdump text (validated empirically, see
`docs/lessons-learned.md` §4):

* objdump prints **unified** mnemonics (`adds r0, #1`, `lsls`, `svc 40`);
  they only assemble under `.syntax unified` — divided syntax rejects them;
* objdump's `@ ...` comments and `<symbol>` suffixes must be stripped;
  `@` is a comment character for ARM gas, but `;` is a statement separator,
  so leftover fragments would silently change the bytes;
* branch/`adr` operands print as absolute addresses (`bl 0x80cfa54`), which
  gas would re-relativize against the wrong origin — they must be rewritten
  to labels (or the instruction falls back to raw bytes).

## Wiring a new segment into the build

1. Add `{"name": "<segment>"}` to `tools/split_config.json`.
2. Run `make split` (writes `asm/<segment>.s`, deletes
   `data/<segment>.s`, regenerates `asm/rom_syms.s`).
3. Run `make compare` — it must report `knidl.gba: OK`.
4. Check `build/knidl.map` (lesson 1.3 in `docs/lessons-learned.md`): the
   section must come from `build/asm/<segment>.o`, not a `.incbin` object,
   e.g.

       .sdk_swi_wrappers
                       0x080cfa40       0x3e build/asm/sdk_swi_wrappers.o
                       0x080cfa40                gUnk_080cfa40
                       0x080cfa4c                DummyFunc
                       ...

No `linker.ld` edit is needed: every segment rule already pins its section
by name and collects an optional `.tail`.

To later decompile a split function to C, give it a real definition
(removing the name from `external_defined` if it was listed there), rerun
`make split` so `rom_syms.s` drops the absolute alias, and follow the
per-zone compiler recipe in `docs/research/compiler-validation.md`.
