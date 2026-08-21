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
`data/sdk_libc.s` defines `_call_via_r0`..`_call_via_r7` as offsets into the
verbatim blob — don't narrow that set when touching it.

### 3.5 Copy-to-stack idiom (SDK SRAM pattern)
`ReadSram`/`VerifySram` copy the `_Core` function to a stack buffer
(`u16 work[64]` / `[96]`) and call `work + 1`. The size expression
`((u32)Wrapper - (u32)Core) / 2` and the cast-call through
`((u32)work + 1)` must be spelled exactly; see `src/agb_sram.c` for the
matching shapes.

### 3.6 SWI numbers follow the SDK order, NOT the retail BIOS order
The ROM's syscall thunks (`0x080CFA50-0x080CFA7E`) emit `svc` numbers
that differ from GBATEK's retail-BIOS table in the 0x08-0x0F band
(e.g. `svc 0x0B` is CpuSet here but Sqrt on a retail BIOS). Misreading
this silently mislabels every thunk. Identify SWI semantics from call
sites, not from the number: the AgbInit fills prove 0x080CFA54 (`svc
0x0C`) = CpuFastSet; an octant-facing helper proves 0x080CFA50 (`svc
0x0A`) = ArcTan2; the pre-reset path proves 0x080CFA7C (`svc 0x28`) =
SoundDriverVSyncOff. The full table with evidence lives in
`include/gba/syscall.h` (see also docs/header-conventions.md).

## 4. Workflow that worked

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
