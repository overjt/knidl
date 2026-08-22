# Lessons learned — decompilation workflow pitfalls

Practical mistakes made and lessons extracted while decompiling the SRAM driver
(issue #8). Read this before attempting the next C module so you don't repeat
them. All examples reference the tools that exist in this repo today
(`Makefile`, `asmdiff.sh`, Docker image `knidl-builder`).

## 1. Build system

### 1.1 Per-file compiler overrides must come AFTER `BUILD_DIR` is defined
A target-specific variable rule like:

    $(BUILD_DIR)/src/foo.o: CC := old_agbcc

silently does nothing if it appears before `BUILD_DIR := build` is assigned
(the target pattern expands to a path that never matches the real object).
Put overrides below the definition (see `Makefile`). Always confirm the
override took effect by grepping the build log for the compiler name
(`old_agbcc` / `agbcc`) on the compile line.

### 1.2 Changing flags does NOT rebuild the object
The object files don't depend on the Makefile. After editing a per-file
override, `rm build/src/<file>.o` (or `make clean`) or you will diff stale
output and chase ghosts. Symptom: source changes "don't change the output".

### 1.3 Verify placement in the map file, not just the SHA-1
After linking a C module for the first time, check `build/knidl.map` shows the
section coming from the C object (e.g. `.sram_driver 0x080cfa9c ... build/src/agb_sram.o(.text)`)
and that the old `.incbin` slice no longer covers that range. This catches
duplication that `make compare` would catch only as a size/overlap error.

### 1.4 GNU ld prints long section names on TWO lines in the map
With this repo's long custom section names (`.crt0_master_isr`,
`.gap_sram_driver_fn_table_asset_metadata_index`, ...), ld emits the section
name alone on one line and `address size object` on the next. Upstream
`calcrom.pl` (katam/pret) only matches the one-line form and silently drops
those sections (progress showed 1.6 MB of "data" instead of 8.4 MB).
`tools/calcrom.pl` now handles both forms; keep that when re-vendoring.

### 1.5 Unquoted `$VAR` word-splitting differs between shells
A CI step passed `make $OBJECTS` unquoted; fine in bash (GitHub Actions) but
zsh (the local dev shell) does not word-split and passes it as one target,
`make: *** No rule to make target 'build/asm/crt0.o build/src/agb_sram.o'`.
Use bash arrays (`objects=(...); make "${objects[@]}"`) so snippets are
copy-paste safe on any shell.

## 2. Tooling pitfalls

### 2.1 The PyPI package `m2c` is NOT the m2c decompiler
`pip install m2c` installs an unrelated code generator. The real one is
`pip install git+https://github.com/matt-kempster/m2c`. Even the real m2c has
trouble with Thumb functions whose literal pools sit mid-file between other
functions (it fails to resolve branch targets across pool words). For small
SDK-shaped functions, pattern-matching by hand against a sibling project
(katam's `agb_sram.c`) was faster than fighting m2c input syntax
(no `.n` suffixes, no inline hex encodings, `.word` pools, labels on every
branch target).

### 2.2 Never build "MATCH" checks on tools that may be absent
A quick harness compared two `xxd` streams with `cmp -s`; `xxd` does not exist
in the Debian-slim container, BOTH command substitutions produced empty
output, and `cmp` reported equality — a false MATCH for every configuration.
Use `cmp file.bin <(dd if=baserom.gba bs=1 skip=N count=M 2>/dev/null)` or
`sha1sum`, and treat "everything matches" as suspicious until seen once with
real bytes. Prefer the repo's `asmdiff.sh` (raw-binary diff, same format both
sides).

### 2.3 objdump label noise
Diffing `objdump -d` output of an ELF (symbolic labels `<Fn+0x1c>`) against
`objdump -D -bbinary` of the ROM (`0x...` labels) produces spurious diff lines
even when bytes are identical. Filter to the instruction column or use
`asmdiff.sh`, which diffs two binary dumps in the same format.

### 2.4 Docker scratch files
`/tmp` inside `docker run --rm` is destroyed when the container exits. Mount a
host directory for scratch files, or run the whole pipeline (compile → link →
objcopy → compare) in ONE `bash -c` invocation.

### 2.5 Running repo scripts inside the container
`asmdiff.sh` detects the container via `INSIDE_DOCKER=1`. Invoke it as
`docker run ... -e INSIDE_DOCKER=1 knidl-builder bash -c './asmdiff.sh ...'`
or it will try to nest docker and fail with "image not found".

### 2.6 ARMv4T BL encoding: bit 0 of the suffix halfword is NOT a flag
The Thumb `bl` pair is `hw1 = 0xF000|offset[22:12]`, `hw2 = 0xF800|offset[11:1]`
— bit 0 of `hw2` stores **bit 1 of the offset**, so real BLs have even suffix
halfwords whenever `(target - site - 4) & 2 == 0` (e.g. `f000 f890` at
`0x0800115C` is a genuine `bl 0x08001280`). Filtering on `hw2 & 1` silently
drops roughly half of all call sites. Verify encodings empirically:
`echo '.code 16\nbl .+0x120' | arm-none-eabi-as -mcpu=arm7tdmi` — gas output
is ground truth for what the original toolchain emitted.

### 2.7 Never scan a Thumb ROM for ARM-mode `bl` outside the ARM zones
Any Thumb halfword pair forms a little-endian word, and pairs like
`68a0 4b01` form `0x4B0168A0` whose bits [27:24] are `1011` — a perfectly
valid ARM `bl` conditional. Scanning the whole ROM in ARM view produced
~8,500 bogus `bl` edges whose "targets" polluted the symbol census with
mid-function fragments. Restrict ARM-mode instruction decoding to the
`arm_code` segments from `docs/analysis/segments.txt`.

### 2.8 objdump's linear sweep hides real entries
`objdump -D -bbinary -Mforce-thumb` disassembles in one linear pass: data or
padding before a function (e.g. `ffff ffff` before `0x0807ED98`) can make a
32-bit decode swallow the real entry address, so the address has no line in
the dump. When an address is missing from a dump, re-disassemble just that
entry with `--start-address` (the sweep then starts exactly at the entry)
before concluding anything about the ROM.

### 2.9 Permuter compile.sh: pipefail is not optional, and diagnostics need it too
decomp-permuter invokes `compile.sh <in.c> -o <out.o>` and treats any nonzero
exit as a failed candidate. A `cpp | compiler | as` pipeline WITHOUT
`set -o pipefail` returns the assembler's status, so a crashed compiler that
still assembles an empty/garbage object produces false scores — including
false zeros. Keep `set -euo pipefail` in every compile.sh
(`tools/permuter-example/compile.sh`). Same trap while DEBUGING the permuter:
replaying captured candidates through a plain pipeline "proves" everything
compiles because bash reports only the last command's status; instrument each
stage's stderr individually before believing any failure count.

### 2.10 gas pads Thumb section tails with NOPs, not zeros
A `.s` whose last instruction ends unaligned relative to the section alignment
gets invisible padding at assembly time — gas emits Thumb NOP (`46c0`), but the
ROM uses zero halfwords between functions. A hand-built `target.s` for the
permuter must therefore emit trailing pad bytes EXPLICITLY (`.short 0x0000`),
or byte-compare vs the baserom slice fails at the tail and the scorer counts a
phantom insertion. The Makefile avoids the same problem for C objects by
appending `.align 2, 0` (explicit zero fill) after agbcc output.

### 2.11 decomp-permuter on old_agbcc: tune weights, don't set objdump_command
The default `gcc` weight set assumes a modern compiler; several randomizer
passes (`perm_inline`'s nested inline_fn chains, type randomization,
`perm_expand_expr`) generate constructs GCC 2.9 rejects, so 25-75% of
candidates die as compile errors with defaults. `[weight_overrides]` in the
function dir's `settings.toml` (see `tools/permuter-example`) disables them and
brings the error rate to ~4% with `-j4`. Conversely do NOT set
`objdump_command`: leaving it unset lets the scorer detect ARM from the ELF
header and add its own objdump flags; a bare override loses them. Some
candidate failures are inherent — treat a persistent >25% error rate as a base.c
shape problem, not a reason to re-tune forever.

## 3. Compiler / source-shape lessons (gcc 2.9 "old_agbcc")

### 3.1 Opt level is per-zone and readable from loop shape
The SDK zone (`0x080CF9xx`) is `old_agbcc -O1 -mthumb-interwork`, NOT -O2.
Diagnostic: a `while (size--)` loop compiles at -O1 to
`subs rN, r2, #1; cmp r2, #0` (counter pre-decremented into a copy, original
register kept for the test), while -O2/-O3 fold it into in-place `subs` +
compare against a materialized `-1`. When a candidate function mismatches,
try the other opt level BEFORE rewriting the source.

### 3.2 Defeating unwanted CSE of pool literals
The ROM loads the SAME pool slot twice (`ldr r3,[pool] ... ldr r1,[pool]`)
because the original source used the function address in two separate
expressions. Single-expression C forms get CSE'd into one load + register
copy. The fix is a two-statement form with an in-place XOR:

    s = (const u16 *)((u32)ReadSram_Core);
    s = (const u16 *)((u32)s ^ 1);

The `eors rN, r0` destroys the loaded value, invalidating the CSE and forcing
the second load. Conversely, a plain temp variable (`fn = ...; ... fn ...`)
still gets CSE'd — the in-place modification of the SAME lvalue is what
matters.

### 3.3 Mask literals: 16-bit, not sign-extended
`REG_WAITCNT & 0xFFFC` produces pool literal `0x0000FFFC`. Writing `~3`
yields `0xFFFFFFFC` (word) and mismatches. Keep masks in the width the
original compiler would have seen (u16 context).

### 3.4 Export ALL `_call_via_rN` aliases from the libc blob
Interworking indirect calls compile to `bl _call_via_rN` where N depends on
register allocation; the first attempt used `r4`, the final one `r3`.
`asm/sdk_libc.s` (formerly offsets into the verbatim blob in
`data/sdk_libc.s`) defines `_call_via_r0`..`_call_via_r7` — don't narrow
that set when touching it.

### 3.5 Copy-to-stack idiom (SDK SRAM pattern)
`ReadSram`/`VerifySram` copy the `_Core` function to a stack buffer
(`u16 work[64]` / `[96]`) and call `work + 1`. The size expression
`((u32)Wrapper - (u32)Core) / 2` and the cast-call through
`((u32)work + 1)` must be spelled exactly; see `src/agb_sram.c` for the
matching shapes.

### 3.6 A ~1KB function's literal pool hides behind a pool-skip branch
When a Thumb function's early `ldr rN, [pc, #off]` literals approach the
1020-byte pc-relative limit, agbcc emits the pool after the epilogue as
`bx r0; b .Lskip; .align 2, 0; <pool>; .Lskip:`. Two traps (both hit while
decompiling AgbInit, issue #28): (a) the segment map had cut the function at
0x080006FF, splitting the final `bx r0` halfword AND orphaning the pool into
the next segment; the real compiler unit runs to the end of the pool
(0x080008E8). (b) The symbol census sees the `b .Lskip` at 0x08000700 as a
"function" (`sub_08000700`, even pointer-referenced from a data table that
uses it as a tail-call thunk); it is compiler output, not source. When a
census function is a lone `b.n` right after another function's return,
suspect a pool-skip branch before assigning it a C body.

### 3.7 Volatile *indexed* stores emit a dead pre-read; direct ones don't
agbcc reads a volatile lvalue before writing it when the address is an
indexed expression — `vu8 arr[4]; arr[i] = 0;` compiles to
`ldrb rX,[rB,rI]; strb r0,[rB,rI]` (dead load), and `((vu16*)sym)[3] = 0`
likewise. A volatile store through a plain symbol (`gCell = 0`) or a
pointer deref (`*p = 0`) has no pre-read. Compound assignment doubles up:
`arr[i] |= m` on a volatile element reads twice (RMW read + store pre-read);
the ROM's single-read `|=` loops therefore operate on *non-volatile* arrays.
Match reads first, then choose volatility/indexing to reproduce them.

### 3.8 Chained assignments are visible in pool order and re-reads
`REG = shadow = value` is one statement: the OUTER address is materialized
first (its pool word precedes the inner one — pool order exposes chains),
the inner cell is stored, and — if the inner is volatile — RE-READ for the
outer store (`strh rV,[rIn]; ldrh r0,[rIn]; strh r0,[rOut]`). Non-volatile
inner cells forward the value register with no re-read. AgbInit is almost
entirely such chains; two separate statements produce the reversed pool
order and don't match.

### 3.9 RAM cells must be extern symbols, not cast constants
gcc derives nearby *constant* addresses from a live register with add/sub
chains (`adds r1,#4` between I/O register writes) but never across distinct
symbols. The ROM pools one word per IWRAM cell even for adjacent addresses
(0x03001EA0/0x03001EA8), proving the original used named globals. Reference
unnamed cells as `extern vu16 gUnk_<addr>;` with the definitions generated
into `asm/rom_syms.s` via `split_config.json` `data_symbols`; casting the
addresses inline lets CSE derive/merge them and breaks the pool layout.

### 3.10 Zero-source variables: one per region, block-local, r4
AgbInit stores dozens of zeros. The ROM materializes `movs rN, #0` once per
straight-line region and keeps it in a callee-saved register across calls —
the shape of a LOCAL VARIABLE (`zero = 0; cell = zero; ...`), not repeated
literals: literal zeros get CSE-unified across call boundaries into one
long-lived pseudo and ruin the whole register allocation downstream (the
allocator ranks pseudos by refs/live-length, so one merged zero drops to r7
and evicts everything else). One zero variable per region keeps each pseudo
block-local (local-alloc gives each r4 independently). Corollaries:
(a) a *dead store* to the variable after its last use (`zeroC = 2;`)
compiles to nothing but invalidates the compiler's reg==0 knowledge, forcing
the ROM's fresh `movs` in the next region — without it the tail reuses the
old register across three calls; (b) the same trick class as lesson 3.2
(state must be invalidated at the source level to defeat CSE).

### 3.11 Loop shapes: reversed counters and hoisted QI zeros
If a `for (i = 0; i < 4; i++)` loop uses `i` only for indexing, loop opt
strength-reduces the arrays to walking pointers and REPLACES the counter
with a down-counter (`movs r2,#3 ... subs/bge`): an ascending C loop can
match descending-counter asm. If `i` is also stored (`arr[i] = i`), the
ascending counter survives (`adds/cmp #3/ble`). Loop-invariant constants
(the store value 0, base addresses) hoist to the preheader in statement
order; with more invariants than free low registers the last one loses and
is re-materialized inside the loop (`ldr rX, =base` per iteration) — which
of them loses is allocation, not source, so match the others first.

### 3.12 SWI numbers follow the SDK order, NOT the retail BIOS order
The ROM's syscall thunks (`0x080CFA50-0x080CFA7E`) emit `svc` numbers
that differ from GBATEK's retail-BIOS table in the 0x08-0x0F band
(e.g. `svc 0x0B` is CpuSet here but Sqrt on a retail BIOS). Misreading
this silently mislabels every thunk. Identify SWI semantics from call
sites, not from the number: the AgbInit fills prove 0x080CFA54 (`svc
0x0C`) = CpuFastSet; an octant-facing helper proves 0x080CFA50 (`svc
0x0A`) = ArcTan2; the pre-reset path proves 0x080CFA7C (`svc 0x28`) =
SoundDriverVSyncOff. The full table with evidence lives in
`include/gba/syscall.h` (see also docs/header-conventions.md).

## 4. Splitting ROM ranges into asm (tools/split.py)

### 4.1 objdump text only round-trips under `.syntax unified`
objdump prints unified mnemonics (`adds r0, #1`, `lsls r1, r1, #18`,
`svc 40`). Feeding them to gas in the default *divided* syntax fails with
"instruction not supported in Thumb16 mode"; with `.syntax unified` they
encode to the exact original halfwords. Generated files therefore always
set `.syntax unified` (same choice as `asm/crt0.s`).

### 4.2 gas pads a section's SIZE to its instruction alignment
A section that contains any Thumb instruction (or even just a `.thumb`
directive) gets `sh_addralign = 2` and is padded to an even size at
assembly time; ARM content pads to 4. A split segment with an odd size
(the `sdk_swi_wrappers` boundary at `0x080CFA7F` splits a `bx lr` in
half) would silently grow a byte and push every later section.
Workaround: park the odd trailing byte(s) in a separate alignment-1
section `.segment.tail` which the linker script appends after the main
section. Data-only directives (`.byte`/`.short`/`.word`) never pad —
mid-section odd bytes are fine (e.g. the odd-start `sdk_reset_helper`
section is emitted as pure data at `0x080CFA7F`).

### 4.3 gas silently aligns Thumb *instructions* to 2
A `.byte` followed by a real instruction gains an invisible `0x00` pad
byte. Any instruction stream at odd section offsets (segments with an
odd start such as `sdk_reset_helper` / `game_code_early`) must therefore
be emitted as raw `.short` data instead of instruction text.

### 4.4 objdump comments and `<symbol>` suffixes must be stripped
`@` is a comment character for ARM gas, so objdump's annotations
(`ldr r3, [pc, #16] @ (0x80cfa94)`) are harmless to re-assemble — but
`;` is a statement separator, and branch operands carry address
references (`bl 0x80cfa54`) that gas would re-relativize against the
wrong origin. Rewrite branch/`adr` operands to labels (the split tool
resolves them from the symbol DB / rom_syms.s) or drop the instruction
to raw `.short` bytes.

### 4.5 Absolute symbols let split files reference unsplit code
`asm/rom_syms.s` defines every not-yet-labeled database function as
`name = 0xADDR` (an absolute symbol), so a split file can emit
`.word sub_08001518+1` (IRQ table) or `bl __divsi3` and the linker
resolves it to identical bytes. Symbols that already exist as real
labels (`asm/crt0.s`, `src/agb_sram.c`) must be excluded
(`external_defined` in `tools/split_config.json`) — as must every DB
function inside a configured split range, which the tool handles via its
`in_split` set — or ld fails with "multiple definition". `make split`
regenerates the file, so the exclusion set stays consistent automatically.

### 4.6 Parsing objdump output: split on tabs, not whitespace
objdump instruction lines are `addr:\t<bytes> \t<mnemonic>\t<operands>`.
A whitespace-based "bytes column" regex silently absorbs mnemonics that
consist purely of hex characters (`add`, `bcc`, ...): the byte count
comes out wrong (4+3 hex chars -> "5 bytes") and the mnemonic vanishes,
which desynchronizes the emission walk. Parse on the tabs.

### 4.7 objdump elides trailing repeated instructions as `...`
A 4-byte disassembly buffer `70 47 00 00` prints `bx lr` then `...`
instead of decoding the trailing `movs r0, r0`. Disassembling one big
buffer per segment (not per function) keeps interior zeros decodable;
only the very segment tail may elide (harmless — those bytes fall back
to raw `.short`).

### 4.8 Branch labels need an emission cursor
A backward branch (`bge .L_loop_head`) is emitted *after* the walk has
already written the label line for its target. Re-queueing the label on
every resolution leaves a stale "unplaceable" entry and needlessly
falls the whole segment back to raw bytes. Track the emission frontier
and only queue labels for addresses not yet passed. Data that merely
*decodes* like a branch (inside pools) creates harmless false-positive
`.L_` labels on halfword boundaries; splitting any 4-byte item (bl
pair, pool word) whose second halfword carries a label keeps every
label placeable.

### 4.9 Verify split segments as a group, not one by one
A split segment may reference labels that another split segment defines
(the IRQ table points into `game_code_early`). Per-segment verification
links fail with "undefined reference" even though the real build is
fine. Link all candidate objects together at their ROM VMAs (plus
rom_syms.o and `--defsym` stand-ins for C-defined symbols) and compare
each section via `objcopy --dump-section` — note that `--dump-section`
takes `section=file` as a SEPARATE argv element and exits 0 without
creating the file when the section is absent, so check for the file.

### 4.10 objdump's linear sweep desyncs on false BL pairs — budget raw halfwords
Disassembling one big Thumb buffer decodes address-stably EXCEPT at data
that pairs as a fake `bl` (lesson 4.7): the bogus pair swallows its own
4-byte window AND shifts every later entry by an odd offset, so nearby
real instructions have no entry in the per-address map (`bx pc; nop`
trampolines at the tail of `sdk_libc` lost two `bx` mnemonics this way).
The tool falls back to raw `.short` for exactly those items, keeping
bytes exact; don't chase "why isn't this line an instruction" — check the
neighbouring fake-BL window first.

### 4.11 Hand labels must flow through the split config, never edit generated asm
CI re-runs `make split` and fails on any diff under `asm/`/`data/`, so
hand-edited names would be reverted (or break CI). Everything nameable is
config-driven: DB names come from `tools/symdb.py` → symbols.csv;
non-DB function labels go in `extra_labels`; named non-ROM word values
(IWRAM cells) go in `data_symbols` with definitions auto-appended to
`asm/rom_syms.s`. The config loader rejects labels outside configured
segments or colliding with DB names, so mistakes fail loudly.

### 4.12 A "gap" between segments can be someone else's literal pool
The 4 bytes at `0x080CFDE4` were catalogued as veneer→IRQ-table padding.
They are actually the literal of the interworking veneer at `0x080CFDDC`
(`ldr ip,[pc]` reads `pc+0` = the NEXT segment's first word: `0x08005655`,
Thumb pointer to the task dispatcher). Segment boundaries are an analysis
grid, not compiler output — pools may straddle them, so before labelling
a gap "padding", decode the preceding ARM/Thumb stream for a pc-relative
load aimed at it.

### 4.13 segments.txt ends are EXCLUSIVE — annotation edits must not shift them
`segments.txt` ranges are half-open (`end` = first byte NOT in the segment);
prose (issues, rom-map rows) often quotes them inclusively. While adding
inline annotations (#24), the `sdk_libc` end got "corrected" from
`0x080CFDDC` to the prose's `0x080CFDDB`, orphaning one byte between two
sections; ld filled it with `00` and only a from-scratch `make clean &&
make compare` caught it — incremental builds kept linking the stale
pre-edit object. Rules: never retype numbers while annotating; after any
boundary edit run a CLEAN full build, not just `make compare` on top of
stale objects.

### 4.14 ld aligns each INPUT section at ABSOLUTE addresses
Inside an output section pinned at an odd VMA, an input section whose
`sh_addralign` is 2 is placed at the next even address — ld silently
inserts a pad byte and shifts everything after it (verified empirically,
issue #25). Consequences: (a) a range starting at an odd address can never
contain real instructions; emit its leading bytes as a data-only prefix;
(b) when splitting one segment across several objects that share the
section name, cut at EVEN function boundaries so every piece starts even
and no padding ever appears; (c) same-named input sections concatenate in
command-line order, so the build must enumerate chunk objects sorted by
address (zero-padded suffixes make `$(wildcard)` order equal address
order).

### 4.15 Disassemble blobs with `-marmv4t`, never plain `-marm`
With `-b binary -marm`, objdump decodes undefined-space halfwords using
the toolchain's newest architecture and prints Thumb-2-only mnemonics
(`rev`, `cbnz`, `it`, `blx reg`, NEON/FPA junk) that arm7tdmi gas then
rejects — every such line would force a whole chunk into raw fallback.
`-marmv4t` (as used by `asmdiff.sh`) fixes most but NOT all: a few
undefined-space halfwords still print later-arch mnemonics, so the split
tool additionally feeds gas error line numbers back into the emitter and
re-emits exactly those addresses as raw `.short`.

### 4.16 objdump's byte column is ENDIAN-SWAPPED per unit
For `-bbinary` blobs objdump prints halfword/word VALUES, not file-order
bytes: ROM bytes `70 47` show as `4770`. To validate that a disassembly
entry really describes the bytes at its address (needed because the
linear sweep desyncs on fake-BL pairs, lesson 4.10), reverse each token's
bytes before comparing against the ROM slice.

### 4.17 Unified-syntax ALIASES re-encode silently
objdump prints `lsls rd, rm, #0` as the alias `movs rd, rm`; gas accepts
that happily but encodes the canonical form (`adds rd, rm, #0`,
`0x1Cxx`) instead of the original `0x00xx` halfword — NO assembler error,
wrong bytes found only at final comparison. The fix in tools/split.py is
generic: after assembling, diff the linked section against baserom and
feed the first differing address back into the emitter, forcing that one
instruction to raw bytes (a handful of addresses per segment). Any future
alias of this kind is caught automatically by the same loop.

### 4.18 `--defsym` stand-ins carry no ARM/Thumb marker
The split verification link used to provide C-defined symbols (`ReadSram`,
...) via `--defsym=...=0xADDR`. Absolutes have no ISA bit, so a `bl` from
split code into one makes ld inject a 16-byte interworking veneer into the
layout (`.__stub` appeared right after `game_code_and_rodata`, shifting
every later section and failing the byte compare). Fix: define each
external symbol as a LABEL in a zero-size section pinned at its real VMA
with the correct `.thumb`/`.arm` directive — same address, right ISA, no
bytes contributed.

### 4.19 Thumb 0xDExx is UDF, not `b<cond>`
`thumb_decode` originally treated `0xD000-0xDFFF` as conditional branches;
`0xDE00-0xDEFF` is actually the permanently-undefined (UDF) space, and
rewriting those operands to labels produced `udf loc_...`, which gas
rejects ("constant expression required"). Branch decoding stops at
`0xDDFF`; `svc` (0xDFxx) was already excluded.

## 5. Workflow that worked

The canonical per-function loop (pick → m2c first pass → asmdiff iterate →
permuter escalation → land + verify) is documented in
[`docs/decomp-loop.md`](decomp-loop.md); follow it for new modules and keep
that doc's subagent handoff contract when delegating. The original SRAM-driver
run that produced these lessons:

1. Disassemble the range from `baserom.gba` (objdump in Docker), identify
   function boundaries from the fn-pointer tables + `push` prologues.
2. Diff against a sibling project's equivalent file (katam) for naming and
   structure; confirm with xref scan (search the whole ROM for word pointers
   to each function entry, odd = Thumb).
3. Write C, wire per-file compiler override + linker section, iterate with
   `asmdiff.sh <start> <len>` on just the module's range (fast), not the
   whole ROM.
4. Finish with a full `make clean && make compare` from scratch, plus a
   `build/knidl.map` sanity check.
5. Update `docs/analysis/rom-map.md`, `docs/analysis/segments.txt`, and this
   file if something new was learned.
