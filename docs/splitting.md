# Splitting ROM ranges into labeled assembly

`tools/split.py` (issue #23) converts verbatim `.incbin` blob segments into
labeled assembler files that re-assemble **byte-for-byte**, giving every
function in the range a symbol. This is the classic pret "split" step: once a
range is split, `build/knidl.map` shows every function (so `asmdiff.sh`,
`asm-differ`, and link-time references work), and C migration can replace
functions one by one.

All SDK/ARM segments around the code region are split (issues #23/#24), and
the whole Thumb game-code region was split into per-function chunks (issue
#25) before it became C. Since issue #36 every **data** segment is
configured too, and emitted by a separate structure-only emitter into
`data/<segment>.s`: labels, symbolic pointer words and `.incbin` slices of
`baserom.gba`, never ROM values (the data policy; rules, config keys and
metrics in [`docs/data.md`](data.md)). The code segments configured today,
all of them asm by design (`docs/audit.md` §2):

| segment              | range                        | contents                          |
| -------------------- | ---------------------------- | --------------------------------- |
| `task_switch_helpers`| `0x08000234-0x080002E8`      | cooperative task switch (ARM, 4 helpers) |
| `task_literals`      | `0x080002E8-0x08000310`      | their literal pools (10 words, all named cells) |
| `m4a_1`              | `0x080CD89C-0x080CE4B8`      | the m4a sound engine's hand-written asm core (`docs/analysis/rom-map.md` §8.2) |
| `sdk_swi_wrappers`   | `0x080CFA4C-0x080CFA80`      | 11 Thumb SWI thunks (svc wrappers)|
| `sdk_reset_helper`   | `0x080CFA80-0x080CFA9C`      | `SoftReset` (Thumb) + its pool |
| `sdk_libc`           | `0x080CFC30-0x080CFDDC`      | `_call_via_r0..lr`, division/modulo, task trampolines |
| `interworking_veneer`| `0x080CFDDC-0x080CFDE4`      | ARM `ldr ip,[pc]; bx ip` -> `0x08005654\|1` |

History: two chunked game-code segments were configured until the code
became C (#25-#35): `game_code_early` (`0x080008E8-0x08007300`, 2 chunks
in `asm/game_code_early/`; its old `0x080006FF` start split the `bx r0`
return and the literal pool of `AgbInit` in half, corrected in #28) and
`game_code_and_rodata` (`0x080075B8-0x080CFA4C`, ~5,000 functions in 14
chunks of ~64 KiB in `asm/game_code_and_rodata/`; its end moved from
`0x080CFA40` in #29, where the old boundary cut the m4a XCMD handler
`ply_xswee` in half).  Every C file now has its own `c_code` row in
`docs/analysis/segments.txt`, landed by `tools/carve.py`.

The six data segments this table used to list (the veneer's literal word,
the IRQ handler table, `lib_misc`, `lib_rodata_fir_tables`, the m4a engine
rodata and the m4a song table) were value lists in `asm/` until #36, which
moved them to `data/` with every value replaced by an `.incbin` slice or a
symbol (`lib_misc` and `lib_rodata_fir_tables` are `sram_id_string` and
`air_grind_rodata` since #36 phase 2, `docs/data.md` §4.1).

## Chunked segments (issue #25)

No segment uses chunking today (the two chunked game-code segments became
C); the mechanism stays in `tools/split.py`.

Segments configured with a `"chunk_bytes"` value are cut at EVEN function
boundaries roughly that many bytes apart and emitted as one file per chunk,
`asm/<segment>/<segment>_NN.s`:

* Every chunk re-uses the segment's section name (`.<segment>`), so
  `linker.ld` needs no edit: ld concatenates same-named input sections in
  command-line order and the Makefile globs them with zero-padded suffixes
  so alphabetical order equals address order.
* Only the FIRST chunk defines the `.global <segment>` anchor label;
  later chunks get file-local `<segment>_NN:` marker labels.
* Branches between chunks resolve through global `loc_XXXXXXXX` labels:
  every non-function branch/adr target inside the segment gets one,
  defined by its owning chunk. Within a chunk, targets keep the legacy
  file-local `.L_XXXXXXXX` labels. Flat segments behave exactly as before
  issue #25.
* Cuts land on word-aligned function boundaries because gas aligns
  instructions (2 for Thumb, 4 for ARM) within a section and pads its size
  to its alignment, and ld aligns each input section to its own
  `sh_addralign`: a chunk that started or ended off its alignment would
  gain padding and shift everything after it (segment boundaries, below).
* Two objdump→gas round-trip hazards are repaired automatically (see
  lessons-learned §4): halfwords in undefined-decode spaces print as
  later-architecture mnemonics that arm7tdmi gas rejects, and unified-
  syntax aliases (`lsls rd, rm, #0` printed as `movs rd, rm`) re-encode to
  the canonical form. Both are detected by feeding assembler errors /
  verification byte-diffs back into the emitter and forcing the affected
  single instructions to raw `.short` bytes (<0.5% of the region).

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
  * `data_symbols`: `{value: name}` — word values emitted symbolically
    wherever they appear as code pool words (the task system's IWRAM
    cells); a RAM/I-O value's definition is appended to `asm/rom_syms.s`,
    while a ROM value inside a data segment becomes a real label in its
    `data/<segment>.s` (#36);
  * `pointer_tables`, `not_pointers` and a segment's `"asset": true`
    drive the data emitter ([`docs/data.md`](data.md) §3-§4).
* `docs/analysis/segments.txt` — segment boundaries and kinds (the single
  source of truth; the config only selects segments by name);
* `docs/analysis/symbols.csv` — the function database (issue #22).

Hand names must always go through the config, never by editing generated
files: CI re-runs `make split` and fails on any diff in `asm/` or `data/`,
so committed outputs must equal regeneration byte-for-byte.

and regenerates:

* `asm/<segment>.s` — one file per flat configured segment, or
  `asm/<segment>/<segment>_NN.s` per chunk for chunked segments
  (committed);
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
  An ARM load whose pool word lies in ANOTHER segment (the task switch
  helpers and `task_literals`, the interworking veneer and the data word
  after it) is written `ldr rN, [pc, #:pc_g0:(<segment> + <off> - 8)]`, an
  `R_ARM_LDR_PC_G0` relocation ld resolves, so it stays right when the two
  sections move apart (#170, lessons 4.153 and 4.155).
* Branches are re-written to labels: in-file function names, local `.L_XXXXXXXX`
  labels for intra-file targets without database entries, or database names
  for external targets. A `@ 0x........` comment preserves the target address.
* Non-function pointer words are symbolic when the value is a
  `data_symbols` cell (`.word gUnk_04000208`, `.word gSoundMainRAM_Buffer+1`
  for the Thumb entry of RAM-copied code); a word that is a constant, not
  an address, is a `raw_words` entry and carries a same-line `@ raw:
  <reason>` (docs/data.md 3.5).

### Data segments (issue #36)

A `data`-kind segment never goes through the rules above. Its file is
`data/<segment>.s` (or, for the pieces of a zone cut around C runs, the
zone's file, below), and it holds only `.global` labels (every ROM
`data_symbols`/`extra_labels` address in it), `.word <symbol>` lines for
proven pointers (function entries outside asset segments, and the words of
consumer-proven `pointer_tables`) and `.incbin "baserom.gba", <offset>,
<length>` for everything else. It is verified in the same group link as the
code files, cannot fall back to raw bytes, and is checked by `make
check-data`. See [`docs/data.md`](data.md).

### Segment boundaries

gas aligns every instruction (2 for Thumb, 4 for ARM) and pads a
section's **size** up to its alignment (4 once it holds an ARM
instruction or a literal pool), and ld aligns each section's start.  A
code segment must therefore start and end on its alignment: `make split`
stops with an error naming the segment otherwise, and the fix is to move
the boundary to an instruction in `docs/analysis/segments.txt`.

The ROM had two boundaries inside an instruction
(`task_switch_helpers`/`task_literals` at `0x080002E5`, which cut the ARM
`b .` at `0x080002E4` in two, and `sdk_swi_wrappers`/`sdk_reset_helper`
at `0x080CFA7F`, which cut SoundDriverVSyncOff's `bx lr`).  Until #37
the splitter handled them with two fallbacks: odd trailing bytes parked
in a separate alignment-1 section `.segment.tail` that every `linker.ld`
block collected with a `KEEP(*(.segment.tail))` pattern, and an odd
segment start emitted as raw `.short`/`.byte` data with labels but no
instruction text.  #37 moved both boundaries to the instruction
boundaries `0x080002E8` and `0x080CFA80` (SoftReset is decoded as Thumb
since), and #167 removed both fallbacks and the `.tail` patterns.

### One data file per zone (#167)

A data segment's config entry may name its `"zone"`, the data file it is
written to.  `tools/carve_data.py` cuts a data segment around the C runs
it carves (`<zone>`, then `<zone>_<start>` after each run) and gives the
pieces after the first one `"zone": "<zone>"`, so a zone cut into 140
pieces (`actor_rodata`, around the 165 C runs of
`src/data/actor_records.c`, `actor_tables.c` and `actor_handlers.c`)
stays one file, `data/<zone>.s`, with one `.section .<piece>` per piece:

```
@ Zone actor_rodata: 0x0873EEA0-0x0874C418, 140 data pieces (0x94FC bytes) and
@ 164 C run(s) between them.  ...
@ Piece actor_rodata: 0x0873EEA0-0x0873F2B8 (data, 0x418 bytes)
	.section .actor_rodata, "a"
	.global	actor_rodata
actor_rodata:
	...
@ 0x0873F2B8-0x0873F4C8: C, row actor_rec_0873f2b8 (src/data/actor_records.c section .actor_rec_0873f2b8)

@ Piece actor_rodata_0873f4c8: 0x0873F4C8-0x0873F5FC (data, 0x134 bytes)
	.section .actor_rodata_0873f4c8, "a"
	...
```

Every piece keeps its `segments.txt` row and its section name, so
`tools/ldgroup.py` lists the pieces and the C runs in order inside one
output section (docs/data.md 5.2), the C runs show as comments where
they sit, and `make split` removes a generated data file that no zone
writes any more (the 34 `actor_rodata_<addr>.s` files run 3 left were
folded into `data/actor_rodata.s`, and a segment a carve consumes
entirely, `frame_tables` or `room_bg_anim_lists`, loses its file).  The
zones since #167: `room_bg_anims` (40 pieces), `room_data` (333),
`game_rodata` (21), `actor_rodata` (140) and `late_game_rodata` (22).
`make datastats`, `make check-data` and `make audit` read a data file
section by section.

### Code of the other ISA inside a function

`"isa_ranges": [{"start", "end", "isa", "why"}]` (docs/data.md 3.5)
declares an ARM island inside a Thumb function (or the reverse): the bytes
after `adr rN, <label>; bx rN` up to the `adr r0, <label>+1; bx r0` that
returns.  The emitter splits the function into runs, writes `.arm` or
`.thumb` before each run's labels, decodes each run in its own ISA and
writes every `add rd, pc, #imm` as `adr rd, <label>` (`<label>+1` for a
Thumb target of an ARM `adr`).  Labels inside a function come from
`extra_labels` (m4a_1 uses pokeemerald's `__umul3232H32`,
`SoundMainRAM_Reverb`, `SoundMainRAM_NoReverb`, `SoundMainRAM_ChanLoop`,
...).  A whole ARM function after `bx pc` (the task trampolines) is an
`ARM_ENTRIES` row of tools/symdb.py instead (lesson 4.142).

## Byte-identity verification

Each run assembles and links **all configured segments as a group** before
writing anything (a split segment may reference labels that another split
segment defines, e.g. `sdk_libc`'s trampolines branching to the task
switcher in `task_switch_helpers`):

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

`make compare` remains the authoritative end-to-end check. A segment
must start and end on its alignment (above). A stress run over
`task_switch_helpers` (ARM), the former `agb_init` split (1 KB of Thumb
with branches and pools; since decompiled, #28) and the pre-#28
`game_code_early` (162 functions) verified the paths, before #167 removed
the odd-boundary fallbacks.

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
2. Run `make split` (writes `asm/<segment>.s` or `asm/<segment>/<segment>_NN.s`,
   deletes
   `data/<segment>.s`, regenerates `asm/rom_syms.s`).
3. Run `make compare` — it must report `knidl.gba: OK`.
4. Check `build/knidl.map` (lesson 1.3 in `docs/lessons-learned.md`): the
   section must come from `build/asm/<segment>.o`, not a `.incbin` object,
   e.g.

       .sdk_swi_wrappers
                       0x080cfa4c       0x32 build/asm/sdk_swi_wrappers.o
                       0x080cfa4c                DummyFunc
                       0x080cfa50                ArcTan2
                       ...

No `linker.ld` edit is needed: every segment rule already names its section
(and, since #36 phase 2 run 2, asserts its address in matching builds
instead of pinning it; docs/data.md section 8).

To later decompile a split function to C, give it a real definition
(removing the name from `external_defined` if it was listed there), rerun
`make split` so `rom_syms.s` drops the absolute alias, and follow the
per-zone compiler recipe in `docs/research/compiler-validation.md`.
