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

### 2.12 `make clean` deletes build/scratch mid-iteration
Scratch dirs under `build/` (function harnesses, permuter working dirs) die
with every `make clean` — and full clean rebuilds are part of the landing
checklist, so this WILL happen mid-task. Keep only regenerable state there
(tools/fnmatch.sh regenerates everything from versioned inputs), or park
long-lived experiments outside `build/`. Related: the macOS Docker daemon
can hang for ~2 minutes under rapid repeated `docker run` — back off
instead of stacking more containers.

### 2.13 SDK libraries are linked whole-object: expect DEAD exports the census cannot see

The m4a driver (issue #31) contains ten public functions with ZERO in-ROM
references (no `bl`, no pointer word): `m4aSongNumStartOrChange`,
`m4aSongNumContinue`, the `m4aMPlayContinue`/`m4aMPlayFadeOut` wrappers,
`m4aMPlayFadeOutTemporarily`, `m4aMPlayFadeIn`, `SoundClear`,
`m4aMPlayPanpotControl`, `m4aMPlayModDepthSet`, `m4aMPlayLFOSpeedSet`.
They exist because the SDK object was linked wholesale, so the evidence-driven
census (`bl` targets + ROM pointers) can NEVER discover them — when mapping an
SDK module, walk the gaps between evidenced functions and identify the dead
bodies by shape against the reference source (function ORDER inside an SDK
object is stable). `tools/symdb.py` grew a curated `EXTRA_THUMB_ENTRIES` list
(evidence kind `curated`) for exactly this case.

### 2.14 Multiboot child images embed their own library copies — grep hits in data segments are NOT the main driver

Scanning the ROM for m4a's `SOUND_INFO_PTR`/`ID_NUMBER` literals returns, besides
the real driver at `0x080CDxxx`, three clusters inside seg 19 data
(`0x08777800+`, `0x0879F2E0+`, `0x087CA834+`). Those are multiboot child-program
images (blob pointers `0x087954C0`/`0x087C0A4C` loaded by the link sender at
`0x08007C5C+`), each embedding its own driver copy. When locating a library by
literal scan, always cross-check hits against the code span and trace who
references the containing blob before concluding anything.

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

### 3.13 Never name the C main loop `main` — gcc inserts `bl __gccmain`
gcc 2.9 hard-codes a `bl __gccmain` call into the prologue of any function
literally named `main` (with `-Werror` it also aborts on the "return type
of `main` is not `int`" warning for `void main`). The ROM's function at
0x08007300 has no such call, which is byte-level proof the original source
used another name; we use `AgbMain` (SDK/pret convention, issue #33 — the
crt0 ARM entry was renamed `Start` to free the name). If a future
"function won't match: extra bl before user code" appears, check the name
first.

### 3.14 Switch dispatch: case order = layout order; the operand's address
CSE survives into early case bodies
agbcc -O2 emits a bounds-checked jump table (`cmp; bhi; lsls #2; ldr;
mov pc, r0`) for dense switches and lays case bodies out in SOURCE order,
not case-number order — recover the original order from the body VMAs
(AgbMain's is 0,1,3,4,7,5,6,8,9,10,13,19,18,17,22,14/15/16,20,21,11,12,2)
or the pool/table layout will never line up. The switch operand's symbol
address stays CSE'd in a call-clobbered register (r2 in AgbMain) and case
bodies whose first access to that global happens BEFORE any call reuse it;
after a `bl`, or at a cross-jump merge point reachable from a call path,
it is re-materialized from the pool. Plain direct accesses to the global
in natural C reproduce all of this — do not hand-cache pointers.

### 3.15 The m4a driver zone is old_agbcc -O2 — and dead lock stores still
shape register allocation
The m4a C driver (`0x080CE520+`, issue #53) is `old_agbcc -O2
-mthumb-interwork` (`fnmatch.sh --old2`), NOT the -O1 SDK-zone default that
agb_sram set: at -O1 the same source produces shift-pair AND masks
(`lsls/lsrs #1` instead of the ROM's pooled `0x7FFFFFFF`), `bics` instead of
`movs #2; negs; ands` for `& ~1`, and u16-truncating loop counters where the
ROM strength-reduces `for (i = 0; i < N; i++)` into pointer-walking
down-counters. Corollary that cost hours: pokeruby-style ident lock pairs
(`mplayInfo->ident++; ...; mplayInfo->ident = ID_NUMBER;`) are OPTIMIZED
AWAY by old_agbcc -O2 (no stores in the ROM!), but the extra references
give the ident temporary a longer live range that pushes it from r1 to r3 —
four functions (`MPlayContinue`, `MPlayFadeOut`,
`m4aMPlayFadeOutTemporarily`, `m4aMPlayFadeIn`) differ ONLY in that r1/r3
swap unless the invisible lock statements are present in the source. When a
function matches except for a systematic register swap, look for source
statements the optimizer deletes but the allocator still feels.

### 3.16 SDK constants read as symbol ADDRESSES need abs_symbols, not
data_symbols
The m4a driver reads its music-player count and max-lines limit as the
address of extern symbols (`(u16)(u32)gNumMusicPlayers` == 4,
`(u32)gMaxLines` == 0 — katam's `m4a.h` documents the idiom): the ROM pool
words are plain `0x00000004`/`0x00000000` yet the codegen (`ldr` + u16 mask
instead of `movs #4`) proves a symbol reference. Define these via
`split_config.json` `"abs_symbols"` (name → value, emitted into
`asm/rom_syms.s` only); `data_symbols` would additionally RENAME every
unrelated `.word 4`/`.word 0` pool word in split asm to the symbol name.
`fnmatch.sh` resolves `abs_symbols` and `extra_labels` as absolute
stand-ins, so candidate C referencing `gMPlayTable`/`gSongTable`/
`gNumMusicPlayers` links standalone.

### 3.17 One translation unit upstream = one compiler recipe; unit identity
beats per-issue guesses
The m4a C driver shipped as ONE `m4a.c`, so parts 1-3 (issues #53/#54/#55,
`0x080CE520-0x080CFA4B`) must all use the same recipe (`old_agbcc -O2`) even
when an issue text guesses otherwise (#55 said `-O1`; the ROM proved -O2 on
the first fnmatch). Corollaries: (a) when a recipe is in doubt, check what
the SAME source file's already-matching neighbors use before experimenting;
(b) the part-1 ident-lock disappearance (3.15) is not a zone property but a
per-function aliasing verdict: in parts 2-3 the lock stores SURVIVE in the
ROM because gcc cannot prove the loop's `track[]` writes don't alias
`mplayInfo->ident` (different pointer origins), so the same pokeruby-style
lock pairs match as written there; (c) a dead export kept by whole-object
linking can sit BETWEEN two live functions and still belongs to the same C
unit (`m4aMPlayTempoControl` at `0x080CF554`).

### 3.18 A zone can hold SEVERAL translation units — read the leaf prologue
The `game_code_early` zone (issue #32) is not one recipe. `0x080008E8-0x08001B08`
is `agbcc -O2`, but the unit starting near `0x08001CC8` is `old_agbcc -O2`, and
they sit in the same census segment with no marker between them. Fingerprint,
readable in one glance without compiling anything: **old_agbcc omits the leaf
`push {lr}`** — a run of tiny void functions ending in a bare `bx lr` is
old_agbcc, while `agbcc` unconditionally emits `push {lr}` / `pop {r0}; bx r0`
for *every* function. Under the wrong recipe the same source was 37 bytes off
with a 4-byte size overrun; under the right one it was byte-exact. So 3.17's
"one unit = one recipe" does not license "one zone = one recipe": check the
leaves of each new range, and re-test a stuck register-allocation-only diff
with the other recipe BEFORE hunting source shapes.

### 3.19 A buffer address mentioned twice may be a SYMBOL, not a constant
Extends 3.9 from single cells to buffers. Written as `(u32 *)0x03001270`, gcc
CSEs the two mentions into one pseudo and the whole entry/tail register
assignment shifts by one register — immovable through dozens of shape variants.
Declared `extern u16 gUnk_03001270[];` and used as `(u32)gUnk_03001270`, each
mention gets its own pool word and the function matches immediately.
Diagnostic: the ROM pools the same address twice, or shows an `ldr`+copy pair
where you emit a single `ldr`.

### 3.20 A void-looking function may need a dead `return`
`sub_080008e8` only matches as `u32 f(u16 steps, ...) { ...; if (0) return
steps; }`. The unreachable return keeps the parameter's pseudo alive, which is
what makes the ROM copy-then-truncate `r0` into `r5` and push one more
register. Narrow tool: apply it only when the diff is "one extra live copy of a
parameter" — the same trick moved a sibling function from 67 to 195 bytes off.
Its cousin: an unused trailing parameter (`u32 f(u32 arg) { ...; return arg; }`)
shifts a three-instruction function's whole allocation by one register.

### 3.21 Loop notes hoist; `goto` does not
The natural `do { ... } while (!(a0 & 0x1000));` made gcc run loop.c and hoist a
table address plus a sign-extension into the preheader — 500+ bytes of
divergence from a ROM that re-materializes them every iteration. Rewriting as a
labelled block plus a trailing `if (...) goto top;` removes the loop note
entirely, so nothing hoists. When a candidate's prologue holds constants the ROM
re-loads inside the loop, try the goto form BEFORE touching expressions.

### 3.22 Three codegens for one indexed load
`(tbl + 1)[i]` emits the ROM's `adds r2, #1; adds r1, r1, r2` (bump the base,
re-add the index); `tbl[i + 1]` folds the +1 into the index; hoisting `tbl` into
a local pointer emits `ldrb rX, [r1, #1]`. Same value, three different shapes —
pick by what the ROM does, not by taste.

### 3.23 Dead-store "hoist" locals defeat CSE where in-place ops cannot
3.2's in-place `w &= mask` trick fails when gcc can re-derive the value from
unchanged operands. What works instead is making the two uses *different
expressions*: `t = pal & hoistT` vs `u = t & 0xF000` forced the ROM's two
separate adds where every same-expression variant was unified. Related
allocation levers found the same way: block-scoping a temporary in its own `{}`
pins it to a different hard register, and declaration order matters *inside* a
block but is a complete no-op at function level.

### 3.24 `ldr rS,=K` + `adds rD,rS,#0` is ONE HImode move, not two insns
agbcc's `*movhi_insn` materialises any constant wider than 8 bits through a
scratch SI register and then copies it into the HImode destination (`gCell =
256` → `movs rS,#128; lsls rS,rS,#1; adds rD,rS,#0; strh rD,...`). When the
destination is a *register* rather than memory, the scratch/dest pair only
stays distinct if the value really lives in a HImode pseudo — and a plain `u16`
local does not, because `PROMOTE_MODE` widens it to SImode and you get one bare
`ldr`. Reproduce it with `register u16 v asm("rN");` (the idiom `FadeOutBody`
in `src/m4a_c1.c` already uses). **Diagnostic sign: the ROM burns an extra
callee-saved register that is written once and never read** — that "dead"
register is the movhi scratch, and it is what drags the extra push/pop into the
prologue. In `sub_08000de4` this one root cause explained all three apparent
diff clusters (prologue push, a constant's register, and a swapped pair).

Corollary: `(u16)x` on an SImode value expands to `lsls #16 / lsrs #16`, never
to `and rX, mask`, and combine will not fold `x & <register>` even when the
register provably holds `0xFFFF`. So **any `ands rX, rM` in the ROM means the
mask is a variable in the source**, not a cast.

### 3.25 An unbalanceable dispatch tree is an if/else nest, not a switch
`balance_case_nodes` puts the MIDDLE value at the root for 3 dense-ish cases,
in every compiler in this repo (agbcc -O1/-O2/-O3, old_agbcc -O1/-O2). So when
the ROM shows the root at the LOWEST case with an empty left subtree
(`cmp Klow; beq body; cmp Klow; ble default; cmp Kmid; beq; cmp Khigh; beq;
b default`), no switch spelling will reproduce it. The source is:

    if (t != Klow) { if (t > Klow) { switch (t) { case Kmid: … case Khigh: … } } }
    else { …Klow body… }

The `else` also places the Klow body last, which is where the ROM has it.

### 3.26 A 2-D array is visible in the addressing mode
`g[i + 4]` on a flat `u16 g[]` folds into the walking pointer
(`ldrh rX,[rWalk,#8]`); `g[1][i]` on `u16 g[3][4]` materialises the row base
separately (`adds r0,rBase,#0; adds r0,#8; adds r0,rIdx,r0`) and keeps `rBase`
live — enough extra pressure, in one case, to push a constant into `r8` and add
a `mov r7,r8; push {r7}` prologue. If the ROM re-derives a base you folded into
an offset, try the 2-D form.

### 3.27 `lsrs` on store + `asrs` on use means `signed short`, not a cast
ARM's `PROMOTE_MODE` forces zero-extension, so an `s16` local is stored with
`lsls #16; lsrs #16` and *sign*-extended at each use (`lsls #16; asrs #16`,
often reusing the shifted value from the preceding compare). A `u16` local with
an explicit `(s16)` cast does NOT reproduce this — combine drops the redundant
sign-extension because the result is re-truncated.

### 3.28 gcc 2.9 never promotes a stack-passed parameter to a register
`void f(…, u16 *out)` with `out++` compiles to `ldr`/`str [sp,#N]` every
iteration. A single `ldr r7,[sp,#32]` in the ROM proves the source took a local
copy (`u16 *p = out;`). Corollary: the POSITION of that `ldr` relative to the
parameter truncations tells you the parameter types — sitting *before* the
`lsls` pairs means those values are 32-bit parameters truncated by locals
(`u16 r = ratio;`), not `u16` parameters, whose truncation `assign_parms` emits
first.

### 3.29 A volatile parameter DEREF re-ties hard registers
One function differed from the ROM only by an r1/r2 swap on the address and
temporary pseudos; five rewrites of the `&=` / `|=` / compare shapes changed
nothing, but making just the first read volatile
(`gCell |= *(vu16 *)p;`) flipped the pair and matched. Making the whole
parameter `vu16 *` is wrong — it re-reads the later fields too.

### 3.30 gcc refuses to strength-reduce a small reversed clear loop
`for (i = N; i >= 0; i--) buf[i] = 0;` keeps `add rA, rIdx, rBase` in the loop
under all four compiler/opt combos. When the ROM shows the reduced form
(`adds r0,rBase,#5` … `subs r0,#1; cmp r0,rBound; bge`) with a SIGNED compare
against the base, the reproducible source is a do-while over int-cast pointers
plus a zero *variable*:

    u8 *b = buf; u8 *e = buf; u8 zero = 0; u8 *p = b + 5;
    do { *p = zero; p--; } while ((s32)p >= (s32)e);

Two separate `buf` mentions give the ROM's `ldr rBase` + `adds rBound,rBase,#0`
copy (3.19 again); the explicit `zero` local (3.10) puts `movs r2,#0` BETWEEN
the copy and `adds r0,rBase,#5`, where a hoisted invariant would land after it;
and the `(s32)` casts give `bge` where a pointer compare emits `bcs`.

### 3.31 `+ hoistZ` (dead-store zero) is the free way to keep a copy alive
To force two textually-identical subexpressions to stay separate, `u = t;` is
useless (copy-propagated) and `u = t & 0xF000` costs a real extra `ands`. But
`u = t + hoistZ;` with a dead-store `hoistZ = 0;` survives cprop as
`add rD,rS,#0` — a register COPY that CSE will not propagate away, at zero
instruction cost. Worth 22 of 35 remaining bytes in one function. Companion to
3.23 and the 3.10 zero-variable idiom.

### 3.32 agbcc canonicalizes commutative operands lowest-pseudo-first
`*dst = v + tile;` with `u16 *dst` narrows the add to HImode, which creates a
`(u16)v` temp carrying a HIGH pseudo number, so `tile` sorts first and you get
`adds r0,r2,r3`. Writing `{ u32 w = v + tile; *dst = w; }` keeps both operands
at their original pseudo numbers and reproduces the ROM's `adds r0,r3,r2`. A
swapped `adds` operand pair is therefore a statement about pseudo birth order,
not about your source's operand order.

### 3.33 CSE's canonical register is `qty_first_reg`
Two locals set to the same constant (`c = 0xFF; b = 0xFF; b &= v;`) make CSE
rewrite `b &= v` as `b = v & c`, destroying the ROM's second `movs rX,#255`.
The fix that preserves ROM statement order is to extend the LATER variable's
pseudo past the end of the CSE block with a dead store (`b = 0;` after its last
real use); `make_regs_eqv` then promotes it to canonical. Assigning `b` first
also works but emits the two `movs` in the wrong order.

### 3.34 Read the register allocator instead of guessing: `agbcc -da`
`.greg`'s "Registers to be allocated in sorted order" is literally
`floor_log2(n_refs) * n_refs / live_length` descending; `.greg`'s "Register
dispositions" plus `.lreg`'s per-pseudo reference counts let you compute
per-hard-register use totals by hand. This turns "why is this register
different" from a guessing game into arithmetic, and is the tool of choice once
a candidate is down to a handful of register-naming bytes. Note `register u16 v
asm("rN")` (3.24) also pins a short-lived CSE temp to a chosen hard register,
not just HImode constants.

Caveat found the hard way: block-scoping temporaries is NOT reliably required
(an earlier function only matched with it, a later one matched with a fully
flattened declaration list once 3.31-3.33 were applied) — and flattening is
what lets decomp-permuter run at all, since nested scopes make it emit
"X undeclared" for most candidates. In this zone the permuter also needs
`perm_refer_to_var = 0`, `perm_add_mask = 0`, `perm_cast_simple = 0`, otherwise
~98% of candidates die on "invalid operands to binary &".

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

### 4.20 A shrinking segment leaves orphan chunk files behind
`split.py` writes chunked segments into `asm/<segment>/` and purges the
directory before regenerating — but the purge was gated on the segment still
being *chunked* (2+ chunks), while the directory layout is used whenever the
segment configures `chunk_bytes` at all. Carve enough off the front of a
segment and it drops to a single chunk: it keeps its directory, stops being
`chunked`, and its old tail chunks survive regeneration. The Makefile's
`asm/*/*.s` glob then assembles the orphans alongside the new segment, which
surfaces as a baffling `relocation truncated to fit: R_ARM_THM_JUMP8 against
loc_XXXXXXXX` from a file you did not expect to exist. Fixed in issue #32 by
gating the purge on the directory layout instead. Symptom to remember: a link
error naming a chunk file whose address range you just carved away.

### 4.21 A fully-consumed segment leaves its whole asm directory orphaned
The sibling of 4.20. Carve a segment's *entire* range and the segment vanishes
from `segments.txt` and `split_config.json` — so `split.py` never visits it
again and never purges its chunk directory, which keeps feeding the Makefile's
`asm/*/*.s` glob. The symptom is a link error blaming the C module you just
landed ("multiple definition of `sub_XXXXXXXX`"), which reads like a bad carve
but is really a file nobody regenerated. `split.py` now removes every directory
under `asm/` whose name is not a configured segment.

### 4.22 An un-carved deliverable in src/ breaks the whole build
The Makefile globs `src/*.c`, so a verified-but-not-yet-carved C module dropped
into `src/` collides with the asm that still provides those bytes ("multiple
definition of sub_XXXXXXXX"). With several agents delivering in parallel this
red-lines the tree for everyone. Keep pending deliverables outside `src/` (this
repo uses a gitignored `pending/`) and move each one in as its range is carved.

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
