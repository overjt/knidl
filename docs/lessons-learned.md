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

### 3.18 SUPERSEDED BY 3.75 — the leaf `push {lr}` is a BUG, not an identity
This lesson originally read: "the `game_code_early` zone is not one recipe;
`0x080008E8-0x08001B08` is `agbcc -O2` but the unit near `0x08001CC8` is
`old_agbcc -O2`; old_agbcc omits the leaf `push {lr}`, so a run of tiny
functions ending in a bare `bx lr` identifies it."

**That conclusion was wrong**, and it cost several ranges a wrong Makefile
recipe — they still matched, because old_agbcc happens to reproduce the missing
push. The real cause is agbcc's `-fprologue-bugfix` flag; see 3.75. The whole
game-code zone is ONE recipe. What survives: 3.17's "one unit = one recipe"
genuinely does not license "one zone = one recipe" a priori, and a prologue
difference IS a real signal worth chasing — just not evidence of a different
compiler binary until you have ruled out a flag.
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
in `src/m4a_c1.c` already uses). **This is an `agbcc` property — see 3.47:
under `old_agbcc` a plain `u16`/`s16` local is already HImode and the
`register asm` idiom is unnecessary.** **Diagnostic sign: the ROM burns an extra
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

See 3.41: function-level declaration order DOES matter whenever two pseudos
tie on global-alloc priority.

Caveat found the hard way: block-scoping temporaries is NOT reliably required
(an earlier function only matched with it, a later one matched with a fully
flattened declaration list once 3.31-3.33 were applied) — and flattening is
what lets decomp-permuter run at all, since nested scopes make it emit
"X undeclared" for most candidates. In this zone the permuter also needs
`perm_refer_to_var = 0`, `perm_add_mask = 0`, `perm_cast_simple = 0`, otherwise
~98% of candidates die on "invalid operands to binary &".

### 3.35 A two-register swap in a two-address op is a `regmove` decision
When the only residue is "the ROM ties the result to operand 2, I tie it to
operand 1" in an `orrs`/`ands`/`adds`, stop hunting expression shapes: it is
gcc's `regmove` pass doing the two-address matching-constraint fixup. Dump it
(`old_agbcc -da -dumpbase X file.i`) and read `X.regmove` — `Fixed operand 1 of
insn N` names the operand the destination was tied to.

**regmove always prefers operand 1.** It only re-targets to operand 2 in a later
BACKWARD pass, and — verified with a four-case micro-lab — only when the root
pseudo of that chain is a copy from a HARD register: an incoming parameter or a
call return value. A chain rooted in a constant or a global load can never get
the swap, no matter how the expression is written. This makes some ROM register
assignments structurally unreachable from a given function's environment: the
identical source shape reproduces them exactly when the accumulator happens to
root in a parameter. Recognise it and take the documented exception rather than
burning days.

### 3.36 A comma expression reverses RTL operand order without reordering emission
`((var = acc), ext) | var` is the only construct that makes `ext` operand 1
while still emitting `acc` first. Reach for it whenever the ROM emits A before B
but ties the result to B.

### 3.37 HImode constants only survive inside a 16-bit store's RHS
A `|0x100`-style constant becomes the 3-insn HImode form (`movs/lsls` +
`adds rD,rS,#0`, see 3.24) only while it sits inside the RHS of a 16-bit store,
because expand narrows the whole tree and `force_reg` runs in HImode. Capture
any part of that tree in a 32-bit variable and the constant silently drops to
the 2-insn SImode form. Conversely, **any 16-bit-typed local costs 2 extra insns
on the C integral promotion** — on read for `register u16 x asm("rN")`, on write
for a plain `u16` (PROMOTE_MODE) — and combine cannot remove them once the value
flows through a paradoxical `(subreg:SI (reg:HI …))`, because `nonzero_bits`
gives up on the high half. The two requirements are mutually exclusive, which
can make a ROM shape unreachable; that is a real outcome, not a failure to try
hard enough.

### 3.38 Storing through a pointer temp loses a volatile array's dead pre-read
`arr[i++] = v` on a volatile array emits the dead pre-read `ldrh` (3.7);
`vs16 *p = &arr[i++]; *p = v;` does not. Complements 3.7's cast-literal case.

### 3.39 A lone reload destination register is `last_spill_reg` ROTATION
When the ONLY diff is the destination register of a reload (`ldr rN,[sp,#K]`),
it is almost never liveness. agbcc's `allocate_reload_reg` (gcc/reload1.c) picks
the reload register by advancing `last_spill_reg` ONE STEP through the ascending
`spill_regs` array, skipping regs occupied by live pseudos — and that state
PERSISTS ACROSS INSNS. So the fix is not "free the right register", it is "add
or remove exactly one reload allocation earlier in the function".

Method: count the reload-inserted insns (UIDs above the max UID in `.lreg`)
before the divergence. `find_dummy_reload`-supplied OUTPUT reloads do not
advance the rotation — the `.greg` dump's own `"Spilling for insn N / Spilling
reg R"` trace tells you which allocations really counted, and is far more direct
than reasoning from register dispositions. In `sub_08001b08` the ROM's last
allocation before the compare was index 4 of `spill_regs = [0,1,2,3,7]`, so the
next was index 0 = r0; our candidate had one extra allocation and landed on r1.

### 3.40 A rematerialized constant silently costs a rotation step
A mask/constant held in a LONG-LIVED local gets spilled with a `REG_EQUIV` note
and rematerialized *by reload*. The emitted instructions are byte-identical to a
plain pseudo's, so it is invisible in the diff — but it burns a `last_spill_reg`
step and shifts every later reload (3.39). Moving the constant into a
block-local `u32` temp removes the reload while keeping the same bytes:

    { u32 t2 = v & 0x1000; last = t2; }     /* not: last = v & hoistM; */

Writing `last = v & 0x1000;` directly is also wrong — it narrows the AND to
HImode and costs the extra `adds rD,rS,#0` movhi copy (3.24). This is the
counterpart to the `hoist` idiom (3.31), which survives only because
`a0 = hoist; a0 &= v;` lets `update_equiv_regs` fold the constant into `a0`'s
own def.

### 3.41 Global-alloc priority TIES are broken by declaration order
`floor_log2(refs) * refs / live_length * 10000` is integer-truncated, so
near-equal pseudos tie EXACTLY (`3*12/156 == 2*6/52 == 2307`). `allocno_compare`
then breaks the tie by pseudo number — i.e. **by declaration order at function
scope**. This corrects the earlier note under 3.34 that declaration order only
matters inside a block: at function scope it matters too, whenever two pseudos
tie, and it decided a 6-byte diff across every `mov rX,r8`/`mov rX,r9` in one
function body. Diagnostic: read `refs` and `live_length` per pseudo out of
`.greg`'s "Registers to be allocated in sorted order" header and compute the
priorities yourself.

### 3.42 Empty `case` labels are load-bearing
`balance_case_nodes` bisects the case list by cost (1 per value, 2 per range),
so the NUMBER of case nodes sets the tree root. An empty case in the left
subtree collapses to `b default`, which jump.c folds into the `ble default` the
ROM shows — invisible in the output, fatal if omitted. One function only matched
after adding `case 0x9900: break;`. Complements 3.25. Related: a switch's own
dispatch compare COUNTS as a use of the case constant when the same constant is
also stored in the bodies; that hidden extra reference is what pushed a constant
over loop.c's `combine_movables` hoist threshold
(`threshold * savings * lifetime` vs `insn_count`) in another function.

### 3.43 A negative constant in a 16-bit cell needs a SIGNED destination
`gCell = -999` with `extern vu16` folds to `0x0000FC19` at compile time; the ROM
pools the full word `0xFFFFFC19`. Only `extern vs16` keeps it. The same
declaration is what turns the READ into the ROM's `ldrh` + `lsls #16` + `asrs
#16` rather than a narrowed `ldrsh` — a volatile `s16` sign-extends explicitly,
it does not use `ldrsh`. Corollary of 3.24/3.27.

### 3.44 Two range guards must stay two `if` statements
`if (a > K) return; if (a < 0) return;` written as `if (a > K || a < 0)` lets
fold collapse the pair into one unsigned `(u32)a > K` compare, and BOTH of the
ROM's compares vanish.

### 3.45 A bound materialised before the value it bounds is a local variable
`if ((u32)(x - 100) > 478)` always emits the subtraction first. When the ROM
loads the bound first, the source held it in a local:
`limit = 478; if ((u32)(id = x - 100) > limit)`. Read the ROM per function —
in the same batch one function needed this and another needed the plain literal.

### 3.46 `MEM & reg` puts the load first only if the load has a SECOND reference
agbcc swaps the operands (a MEM cannot be the two-address target) and emits
`mov tmp,reg` / `ldrb val` / `and tmp,val` — three insns. The ROM's two-insn
`ldrb rX,[rY,#N]; ands rX,rZ` only appears when the loaded pseudo has another
reference, so combine cannot fold the load into the AND. An intervening call
does it. Making the load volatile does NOT (it only reorders the copy after the
`ldrb`), and neither does splitting the statement, caching in a local, swapping
the written operand order, De Morgan, or type juggling — all compile
byte-identically.

### 3.47 HImode locals differ by RECIPE — scope 3.24 before applying it
Under `old_agbcc`, `s16 v; v = -1;` pool-loads `0xFFFF` (a real HImode local),
while `s32 v; v = -1;` emits `movs #1; negs`. So in the old_agbcc units a plain
`u16`/`s16` local is HImode already and the `register u16 x asm("rN")` idiom of
3.24 is NOT needed. Under `agbcc`, `PROMOTE_MODE` widens the same local to
SImode and the idiom IS needed. Establish your range's recipe (3.18) before
reaching for either.

### 3.48 A volatile local defeats zero-constant CSE across a call
`u16 zero = 0; CpuSet(&zero, …); z = 0;` merges both zeros into one pseudo that
lives across the call, costing an extra callee-saved register in the prologue.
Declaring the fill source `vu16 zero;` (or storing through `u16 *p = &zero`)
gives the ROM's two independent `movs rX,#0` and drops the extra push.
Extends 3.10.

### 3.49 Volatile pins instruction ORDER, not just re-reads
For `(field & K)` in a condition, old_agbcc ALWAYS emits `movs rK` before the
`ldrb` — unless the field is volatile, in which case the load comes first.
Making two `MultiBootParam` byte fields `vu8` fixed 22 differing bytes across
three sites and was the last blocker in that function. Corollary: a
NON-volatile byte field read three times gives one CSE'd `ldrb` plus one shared
`lsls #24` zero-extension (`(x>>1)&1`, `(x>>2)&1`, `(x>>3)&1`); copying it into
a local instead makes the extension disappear.

### 3.50 The OR quirk: one constant stored to several bytes
Storing the SAME QImode constant to two or more different byte fields compiles
to `ldrb rT; orrs rT,rK; strb rT` per store, not a plain `strb`. Different
constants (e.g. `0xFF` vs a register-held `-1`) give plain `strb`s. So **a run
of plain `strb`s of one value in the ROM proves the source used a VARIABLE**,
not a repeated literal.

### 3.51 Cross-jumping needs the same hard register, so the same VARIABLE
Two identical tails only merge into the ROM's single `b` when both are written
in terms of one shared local. Writing the same statements with two different
locals leaves both tails emitted in full.

### 3.52 Two dense cases are a real `switch`
`if (x == 0) … else if (x == 0xD1)` gives an inverted-branch fallthrough; the
ROM's `beq case1; cmp K2; beq case2; b default` only comes from a `switch`.
Complements 3.25/3.42, which cover when a switch will NOT match.

### 3.53 A label INSIDE a `do/while` body is a third loop shape
Between 3.21's two options there is a third. A plain `do { } while` optimizes
everything; the `goto` form optimizes nothing BUT also drops the loop NOTES,
which costs the induction variable its loop-depth-weighted references in flow.c
and can flip its register. Putting the label INSIDE the `do/while` body gives
the loop two entry points — `.loop` prints `Loop at N ignored due to multiple
entry points` — so `scan_loop`/`strength_reduce` are skipped while the notes
survive and the depth weighting is preserved. **Diagnostic: the ROM re-adds a
base+index every iteration AND re-materialises small constants in the body, yet
still has pool addresses in its preheader.** This one change took a function
from 640 differing bytes to 1. Note the invalid loop cannot hoist its own
preheader values, so those must be hand-hoisted into a local placed exactly
where the ROM's `ldr` sits.

### 3.54 `arr[i]` can NEVER produce `adds rD, rIndex, rBase`
C's `build_binary_op` routes any pointer/int addition through
`pointer_int_sum(ptrop, intop)`, so `arr[i]`, `i[arr]` and `*(i + arr)` build
the identical tree and all emit `adds rD, rBase, rIndex`. The ROM's opposite
operand order requires the addition to happen in INTEGER type:
`*(s8 *)(i + (int)arr)`. This is a front-end canonicalisation, so — unlike 3.32
— no declaration-order or pseudo-birth-order change can fix it.

### 3.55 Read `.loop`'s life/savings arithmetic before hunting shapes
loop.c moves a movable iff `threshold * savings * lifetime >= insn_count`, and
the `.loop` dump prints all three inputs; `combine_movables` accumulates
`savings` and `lifetime` across identical loads. In this zone `threshold = 16`.
If the product is 1 and `insn_count` is 98, no source shape in that family will
ever produce the hoist — take the documented exception instead of grinding.
Related, and more terminal: **a symbol address that CSE merged across a branch
can never be loop-hoisted at all** — `scan_loop` silently omits such a set from
the movable list (`! reg_in_basic_block_p && maybe_never`), so "the movable is
not in the `.loop` list at all" is a stronger signal than "not desirable".

### 3.56 Count pool loads per pass to find which pass merged two mentions
`grep -c 'symbol_ref/u:SI ("\*\.LCn")' x.{rtl,jump,cse,gcse,loop,cse2}` pins
exactly which pass collapsed two mentions of one symbol (in one case: 5 loads in
`.rtl`, still 5 after jump, 3 after cse1). Cheap, and it turns 3.19-style
questions into arithmetic instead of guesswork.

### 3.57 The `push {lr}` fingerprint covers value-returning leaves too
3.18's recipe test is not limited to void functions: a `u32`-returning leaf also
has no prologue under old_agbcc.

### 3.58 An 8-byte STRUCT assignment is how you get the DImode block move
`*(struct { u32 a, b; } *)dst = *(struct { u32 a, b; } *)REG_ADDR;` gives the
ROM's `ldr [4]; ldr [0]; str [0]; str [4]` ("created by
thumb_load_double_from_address"). A `struct { u16 d[4]; }` of the same size
emits `ldmia`/`stmia` instead. `long long` and `double` casts behave like the
u32 pair.

### 3.59 Strength reduction needs a MIXED pointer/array loop body
An all-array loop refuses to strength-reduce; an all-pointer loop reduces but
loses the volatile store's dead pre-read (3.7). The ROM needs both: read through
a base pointer (`src[i+1]`) and write the volatile array element
(`arr[i] = …`). Extends 3.22 — and note `src = base; src[i+1]` folds the `+1`
into the offset (`ldrh [r2,#2]`) while `src = base + 1; src[i]` puts it into the
giv's initial value (`adds r2,r6,#2`). Likewise `p[i]` and `p[i-1]` in one loop
create the ROM's TWO pointer ivs, where a single pointer with `p[-1]` gives
`subs r7,r6,#2` instead.

### 3.60 A volatile cast pins evaluation ORDER no expression shape can reach
For `lo | (hi << 16)`, agbcc ALWAYS expands the shift subtree first — regardless
of operand order, `+`/`*` instead of `|`, masking, temps, or 2-D array shapes
(32 forms tested). Making the low read volatile (`*(vu16 *)&arr[i]`) is the only
thing that pins it first. Companion to 3.49.

Sharpens 3.35: `regmove` ALSO refuses operand 1 when it is a paradoxical subreg
— `(ior (subreg:SI (reg:HI 50)) (reg:SI 54))`, exactly what a volatile `u16`
read produces. So "volatile u16 operand" is a second structural reason the
operand-1 tie is unreachable, besides the hard-register-root rule. The escape
that worked: pin both operands with `register u32 x asm("rN")` AND perform the
sub-assignments inside the store expression —
`dst = (u32 *)((lo = a) | (hi = b << 16))` — which additionally keeps the LHS
address `ldr` ahead of both loads, since `expand_assignment` does the LHS first
while separate statements push it after.

### 3.61 gcc 2.9 CSE is basic-block-local — cross-block reloads are NOT volatile
Repeated `ldrh` of the same global across `bne` boundaries looks like a volatile
tell but reproduces fine non-volatile. Adding volatile there blew up register
pressure and pushed a base address into `ip`. Only reach for volatile when the
re-read is WITHIN one basic block.

### 3.62 An MMIO register can be DERIVED from another live I/O address
3.9/3.19 said RAM cells must be extern symbols rather than cast constants; the
same applies to I/O registers. `REG_IME` written as the cast literal
`0x04000208` came out as `adds r1,#252` off the still-live `0x0400010C`
(REG_TM3CNT_L) — that deletes one address pseudo and shifts the whole
function's allocation by one hard register (98 bytes off, immovable). Declaring
it `extern vu16 gUnk_04000208;` gives it its own pool word and the function
matched instantly. **Diagnostic: the ROM pools an I/O address that your
candidate derives with an `adds` off a neighbouring register.**

### 3.63 A cycle-exact delay loop is unreachable from pure C
The ROM's `subs r0,r0,r1; bgt` has no `cmp`; old_agbcc ALWAYS emits
`cmp rX,#0` after the subtract (verified across five loop spellings and both
compilers). Such a function needs inline asm, e.g.
`asm volatile("1:\n\tsub %0, %0, %2\n\tbgt 1b" : "=r"(c) : "0"(c), "r"(gap));`
with `register` pins on the operand pair. This is a legitimate exception, not a
failure to find the shape.

### 3.64 Cross-jump DIRECTION is source order
When several `return K;` sites must collapse onto ONE block placed at the END of
the function, gcc keeps the EARLIEST copy and branches backwards into it — the
opposite of what the ROM often shows. Write them as `goto done;` with
`done: return K;` last. Same lever for a shared tail call: put `return f(…)` at
the end of the first case body and `goto` it from the later ones. Companion to
3.51 (which is about needing the same variable).

### 3.65 `orr rD, rS` always ties to the SHIFTED operand in `lo | (hi << 8)`
The byte value reaches the `ior` as `(subreg:SI (reg:QI …))`, and the
`.regmove` dump only ever prints "Could fix operand 2" — regmove skips a
QI-subreg operand outright. When the ROM ties to the byte instead, the only fix
found is pinning both with `register u32 lo asm("r1"); register u32 hi
asm("r0");` in a block. Third structural exception under 3.35, alongside the
hard-register-root rule and the paradoxical-subreg case of 3.60.

### 3.66 `volatile` on a `u8 *` struct member fixes byte-read ORDER
`mp->p[0xAC] | (mp->p[0xAD] << 8)` reads 0xAD first — fold hoists the complex
operand and the scheduler reorders the two `ldrb`s. Typing the member `vu8 *`
forces source order. Related: **a local pointer to a struct byte field
manufactures a second address pseudo AND suppresses a giv** — taking
`u8 *p = &mp->field;` inside a loop while later code still says `mp->field`
reproduces a `mov ip,r2; mov r8,ip` pair, and the extra pseudo removes the free
register loop.c was using to strength-reduce an unrelated array walk.

### 3.67 A volatile switch operand is re-loaded for the table index
`switch (cell)` on a `vs32` emits an `ldr` for the range check AND a second
`ldr` before the `lsls #2`; a non-volatile operand reuses the first register.

### 3.68 A volatile 16-bit store of ALL-ONES reuses the dead pre-read
`vu16 A[]; A[i] = 0xFFFF;` emits `ldrh rX,[rP]; orrs rX,rMask; strh rX,[rP]` —
agbcc emits 3.7's dead pre-read and then ORs the all-ones constant INTO that
register instead of materialising a fresh value. `A[i] |= 0xFFFF` gives four
insns (pre-read + RMW read + orrs + strh), and a pointer/cast lvalue drops the
pre-read but lets `fold` collapse the OR into a plain store. Only the plain
`= 0xFFFF` on a volatile ARRAY produces the ROM's three.

### 3.69 Chained assignment: addresses left-to-right, stores right-to-left
`a = b = c = K` materialises `to_rtx(a)`, `to_rtx(b)`, `to_rtx(c)` and THEN
stores `c, b, a`. So a run of stores at descending offsets with their address
computations interleaved ahead of them is a chain, while `addr/store/addr/store`
pairs are separate statements. Combined with `update_equiv_regs` moving
single-use address loads down to their store, this explains most "out of order"
pool/`ldr` sequences. Sharpens 3.8.

### 3.70 An address-taken `u16` stack temp still needs `vu16` for the movhi pair
`u16 fill = 0xFFFF;` emits one `ldr rD,=0xFFFF`; `vu16 fill` emits the ROM's
`ldr rS,=0xFFFF; adds rD,rS,#0` (3.24). Complements 3.37.

### 3.71 `|= 0xFF` folds on an `s8` field but survives on a `u8` field
On `s8` it is constant-folded to a plain store; on `u8` it stays as
`movs rM,#255` + `orrs`. The same ROM byte can therefore need to be `u8` in one
function and read with an explicit `(s8)` cast in another — the cast still
produces `ldrsb`.

### 3.72 The goto-vs-loop-note lever steers ALLOCATION, not just hoisting
3.21/3.53 are about hoisting; the same lever also changes register assignment.
Rewriting an outer restart from `do {…} while (n)` to a label + `if (n) goto`
removes one loop level from every `REG_N_REFS` weight inside it, which moved
three long-lived address pseudos onto the ROM's registers. Related knob:
**merging a counter into an existing loop variable** shifts
`floor_log2(refs)*refs/live_length` across a power-of-two boundary (refs 57→63,
live_length 275→305 in one case) and was the only thing that moved a specific
pseudo onto the ROM's register.

### 3.73 A commutative SImode op with a 16-bit load: operand order proves USES
A 16-bit load feeding a commutative SImode operation becomes
`(subreg:SI (reg:HI …))`, and combine's canonicalisation ALWAYS moves the SUBREG
operand first. So `adds rD, rPtr, rLoaded` in the ROM — pointer first — **proves
the loaded value has two or more uses**: with a single use combine folds the
extension and you unavoidably get `adds rD, rLoaded, rPtr`. Verified with a
four-case micro-lab (1 use → load first in all three spellings; 2 uses → other
operand first). No single-use spelling reproduces it, including the comma
expression of 3.36, `u8 *`/`u32` temps, `q[-1]`, or const/volatile pointees.

### 3.74 Assorted levers confirmed on the task/draw helpers
- 3.50's OR quirk applies in HImode too: two or more stores of the SAME 16-bit
  constant give `ldrh/orrs/strh` per store, while exactly one such store stays a
  plain `ldr =K; strh`.
- `s32 ff = 0xFFFF; *p16 = ff;` gives `ldr rX,=0xFFFF; strh` with NO movhi
  scratch, where `*p16 = 0xFFFF` adds 3.24's `adds rD,rS,#0` and `*p16 = -1`
  gets CSE'd onto any live SImode −1.
- 3.49 extends to loop guards: with a non-volatile pointee, `while (*p != K)`
  emits `ldr rK,=K` BEFORE the guard's `ldrh`; a `vu16` pointee flips it to the
  ROM's `ldrh`-first order.
- `vs16 *slot = &arr[id]; *slot = v;` suppresses a volatile array's dead
  pre-read — 3.38 confirmed for variable-indexed arrays, not just `i++`.
- A dead `adds rN,r0,#0` after a `bl` in the ROM means the wrapper wrote
  `i = callee(x); return i;` reusing the SAME local the preceding loop used.
- Two blocks that each re-read one global pointer need TWO different locals when
  the first pointer must die before a call; one shared local pins it to a
  callee-saved register for the whole function.
- `(u16)(x + 63) > 366` reproduces `lsls #16 / adds 0x3F0000 / movs #183;
  lsls #17 / cmp / bhi` — an unsigned HImode compare shifts BOTH operands left
  16 and combine folds the addend into the shifted domain.

### 3.75 `-fprologue-bugfix` is the game-code zone's real recipe
`agbcc -O2 -mthumb-interwork -fprologue-bugfix` (fnmatch `--newpb`). The flag's
own help text is "Prevent unnecessary saving of the lr register to the stack".
Mechanism: `far_jump_used_p()` in `thumb.c` caches
`current_function_has_far_jump` unless `flag_prologue_bugfix` is set; the stale
cache makes `thumb_function_prologue`'s
`if (live_regs_mask || !leaf_function_p() || far_jump_used_p())` fire and push
LR on a leaf that only has a 2-byte branch. With the flag the cache is disabled
and the spurious push disappears.

**Everything 3.18 attributed to "old_agbcc sub-units" inside game_code_early was
this flag.** Verified by re-checking every landed module of issue #32 —
including `src/agb_init.c` from issue #28 and the twelve modules that had been
landed with `old_agbcc -O2` Makefile overrides — all 21 match byte-exact under
the single `--newpb` recipe, as does `src/main.c`. The m4a driver
(`m4a_c1/cgb/ctrl`) does NOT, so it stays genuinely `old_agbcc -O2`.

Practical rule: **before concluding that a game-code range needs a different
compiler binary, re-test it with `--newpb`.** Treat a two-recipe story inside
one zone as a flag difference until proven otherwise.

### 3.76 `ldrb; lsls #24; asrs #24; cmp rX,#K` is an agbcc-only signature
Under `old_agbcc` that sign extension is ALWAYS removed on an equality test:
`simplify_comparison`'s `ASHIFTRT` case rewrites
`(compare (ashiftrt (ashift X 24) 24) K)` to a QImode compare, and the
"widen back" block then succeeds for `EQ`/`NE` because `WORD_REGISTER_OPERATIONS`
plus `LOAD_EXTEND_OP(QI) == ZERO_EXTEND` make `nonzero_bits((mem:QI)) == 0xFF`;
old_agbcc's `s_register_operand` is plain `register_operand`, which accepts
`(subreg:SI (mem:QI))`, so `recog` succeeds. agbcc's `s_register_operand`
rejects subreg-of-mem, `recog` fails, and the shifts survive. Corollary: under
old_agbcc a signed RELATIONAL (`> 1`) keeps the shifts, since the widen block
only allows `EQ/NE/GEU/GTU/LEU/LTU`. ROM-wide there are 724 instances of this
pattern and zero in any confirmed old_agbcc range.

### 3.77 Read the compiler source — there are only nine behavioural differences
The decisive step on the hardest recipe question was not RTL dumps but cloning
the pinned `jiangzhengwenjz/agbcc` source and reading `combine.c` + `thumb.c`.
`grep -rn OLD_COMPILER` lists EVERY behavioural difference between the two
compiler binaries — there are only nine — and `flag_prologue_bugfix` plus
`s_register_operand`'s `#ifndef OLD_COMPILER` were both found within minutes.
Add this to the escalation ladder after 3.34/3.39: the dumps tell you WHICH pass
did something, the source tells you WHY.

### 3.78 `u16 t = x + K; if (t > LIMIT)` is how you get a HImode range check
`adds rX,#K; lsls rX,#16; cmp rX,rLIMIT<<16; bhi` comes from a genuine `u16`
local, not from `(u16)(x + K) > LIMIT`, which zero-extends into an SImode
compare instead.

### 3.79 Call-argument sub-expressions need their own locals to fix order
`(u32)q->tbl[q->idx]` loads the index first; `t = q->tbl; t[q->idx]` loads the
base first. Same for a call inside an argument list: `v = f(0); q = g;
h(q->a, v, …)` keeps the global reload AFTER the call, while `h(q->a, f(0), …)`
hoists it before.

### 3.80 The `push {lr}` fingerprint fails a SECOND way
Beyond the prologue bug of 3.75, agbcc also omits the leaf push for
**straight-line** leaves (one basic block, no `.L` labels). So only a leaf that
BRANCHES discriminates anything at all — and after 3.75, not even that. One
agent picked two straight-line leaves as probes and concluded "old_agbcc" for a
range that is plain agbcc. Treat the fingerprint as a hint to investigate, never
as an answer.

### 3.81 A `u16`-returning `return a + b;` narrows the PLUS and swaps operands
`convert_to_integer`'s "shorten" pushes the u16 conversion down into the
addends, so the wider operand becomes `(subreg:SI (reg:HI …))`. That flips the
commutative canonical order (3.32) AND ties the two-address result to the
parameter instead of the call-return chain — which extends the parameter's live
range, raises its local-alloc priority (refs 6→12) and shifts every
callee-saved register by one. Computing into a `u32` local first
(`u32 v = a + b; return v;`) keeps both operands plain SImode and the ROM's
`adds r0, r0, rParam` falls out. **Diagnostic: the ROM ties the add's
destination to the call-return register while you tie it to the parameter, with
byte-identical instruction counts.**

### 3.82 `(int)(signed char)x == K` never keeps its sign-extension
`fold` narrows the comparison to QImode and expand emits `ldrb; cmp`. The ROM's
three-instruction `extendqisi2` form (3.76) only survives when the extended
value has a SECOND reference — verified against 25 spellings (`volatile`,
`vs8 *`, `s32`/`s8` locals, `s32 f:8` bitfields, struct members, `switch`,
`(u32)` casts, `c-1==0`, `if (0) return c;`, `c*0`, `c&0`, `register`,
ternaries), all of which fold.

### 3.83 A packed status word must be ONE assignment expression
Splitting `g = A|B|…` into `v = …; if (c) v |= K; g = v;` makes gcc load each
byte field lazily (interleaved `ldrb`/`orr`) and emit the destination address
last. Written as a single assignment with the conditions as ternaries,
`expand_assignment` materialises the LHS address FIRST — into a callee-saved
register, which is what adds the extra `mov r7,r8 / push {r7}` — and every leaf
shift is evaluated before any branch. Worth 276 → 0 differing bytes in one
function.

### 3.84 Giv initialisations land at the END of the preheader, in discovery order
They are emitted after loop.c's hoisted movables, so the array-vs-pointer
spelling of a subscript decides where its base computation sits relative to the
hoisted invariants. `*(base + idx + i*30)` and `base[idx + i*30]` are
semantically identical but differ by 14 bytes: the pointer form makes the whole
`base+idx` the giv base, the array form splits it. Companion to 3.59.

### 3.85 Read pool words with a script, not off objdump by eye
`((u8 *)&sym)[K]` with K > 31 gives `ldr rC,=K; adds rD,rBase,rC`, and the SAME
constant mentioned twice reuses one pool word — which is how a mis-transcribed
`0x10D` as `0x10C` was caught after a wrong-field detour. Two minutes of
scripting beats re-reading hex columns.

### 3.86 `&sym[K]` is a link-time constant and behaves nothing like `ptr + K`
When the ROM shows `mov rX, rBase; adds rX, #K; adds rD, rD, rX` INSTEAD of
folding `#K` into the load's immediate offset, the source said
`(u32)&gUnk_XXXX[K]`. gcc gives the `(const (plus symbol K))` its own pseudo —
cse cannot fold it into the MEM address — and then rewrites the constant as
`base_reg + K` because a register already holds the symbol. A local pointer `p`
with `p + K` always folds into the load offset instead. This took one function
from 44 differing bytes to 16 and, as a side effect, raised block pressure
enough to move the base into `ip`, which no shape hunting had achieved.

### 3.87 `x * 2` inside an address sum is deferred; `x << 1` is not
For a constant `MULT_EXPR` in a pointer-sized sum, `expand_expr` returns an
unexpanded `(mult reg const)` under `EXPAND_SUM` and the shift is emitted later
by `force_operand` — which interleaves both operand LOADS before either shift.
Spelling the scaling as explicit shifts (`x << 1`, `((x << 4) - x) << 2`) emits
each shift right after its load.

### 3.88 A base pointer in `ip` is PRESSURE, not a `register asm` pin
ARM's `REG_ALLOC_ORDER` is `{0,1,2,3,12,14,4,…}`, so once four block-local
pseudos occupy r0-r3, global-alloc's next choice for a long-lived base is r12 —
with `mov rN, ip` reload copies at every use and no prologue push. Writing
`register T x asm("ip")` produces visibly DIFFERENT (wrong) code: it stops
offsets folding into `ldrb`/`ldrh`. Raise the pressure instead of pinning.

### 3.89 A `+=` chain pins commutative operand order in a 3-term address
Plain `A + B + C` lets gcc choose each `adds`'s operand order independently;
`t = A; t += B; t += C;` ties every destination to the accumulator and gives the
ROM's `adds rD, rD, rS`. One function matched on this after eight failed
operand-order permutations.

### 3.90 Integer-typed index arithmetic defeats the `base + i*stride` hoist
`buf[i][j] = v` hoists `base + i*stride` into the outer loop;
`*(u16 *)(i*stride + j*2 + (u32)buf) = v` — all integer, base added LAST —
reproduces the ROM's per-iteration re-add. Extends 3.54, and note the sum must
be ordered so that no two-term subexpression is loop-invariant.

### 3.91 `while (f(), cond)` puts the call AND the test at the loop bottom
`while (1) { f(); if (cond) break; body; }` emits `f()` at the top. The comma
condition emits `goto test; loop: body; test: f(); if (cond) goto loop;`.

### 3.92 `(t << 26) >> 30` and `(t >> 4) & 3` are different codegen
The shift pair is a literal `lsls`/`lsrs`; the mask form gives
`lsrs; movs #3; ands`. Bit-field extraction in this zone is written as shift
pairs.

### 3.93 A provably-false compare against an extern symbol is NOT dead code
One function contains `ldr r0,=0x53F3; cmp r1,r0` where `r1` holds a pooled
work-area address — always false. Because the work area is an EXTERN symbol,
gcc cannot fold `(u32)gUnk_XXXX == 0x53F3` and emits the compare, reproducing
the ROM exactly. Do not "clean up" such a compare; it is load-bearing. (It is
probably a bug in the original source — a session-id compare that got the wrong
operand — which is exactly the kind of thing a matching decomp preserves.)

### 3.94 `pop {r1}; bx r1` in the epilogue means the function RETURNS a value
A void agbcc function pops the return address into the first free register,
which is `r0`; a function whose result is live-out pops into `r1` instead. When
the epilogue reads `pop {r4}; pop {r1}; bx r1` but the body never sets `r0`, the
original prototype was non-void with no `return` statement — declaring it
`s32 f(void)` reserves `r0`, which shifts the whole allocation up one register
and adds the prologue/epilogue. That single change turned `sub_080684A4`
(a 26-case switch) from an 8-byte mismatch into a byte-match; nothing else about
the body changed.

### 3.95 Count the callee's argument registers before writing the call
`sub_0800A340` starts `adds r3, r0, #0; adds r4, r1, #0` — it reads TWO argument
registers. Its caller's `ldrsh r1, [r0, r2]; adds r0, r1, #0` is therefore not a
stray reload-copy but `f(v, v)`: the load lands in the second argument register
and is copied into the first. Reading the callee's prologue is much cheaper than
hunting for a source shape that reproduces a "spurious" copy — that copy is an
argument.

### 3.96 A `(u16)` cast on a multiplicand feeding a 16-bit store is free but re-allocates
`t->unk48 = u->unk48 + u->unk43 * tbl[i];` (`unk43` an `s8`, `unk48` an `s16`)
emits `muls` in place. The ROM had `adds r7, r0, #0; muls r7, r1; adds r0, r7, #0`
— the product in a callee-saved register with copies in and out. Writing
`(u16)u->unk43 * tbl[i]` reproduces it exactly: the zero-extension is dead
(only the low 16 bits reach the `strh`, and `(u16)x * y == x * y` mod 2^16), so
gcc emits no masking, but the RTL is a different expression and the product gets
its own pseudo. Same bytes, same behaviour; try it whenever a multiply is short
by exactly the two copies.

### 3.97 One local per re-read of a global pointer
`t = gUnk_03002490;` twice in a function is ONE pseudo whose live range spans
both uses, so gcc keeps it in a register across the second statement and pays a
copy (`adds r1, r2, #0`) whenever the ROM instead mutates the pointer in place
(`adds r1, #66`). Give every re-read its own local (`t`, `u`, `v`, `w`, `x`) and
the copies disappear. Conversely, when the ROM does keep a copy, merge the
locals — `sub_080665A0` needed ONE `s16` shared by both arms of an `if/else` so
the value pseudo spans the branch.

### 3.98 Re-read the global in the loop condition, not at the loop bottom
```c
while (gUnk_03002790[gUnk_03002490->unk44].unk3C == 34)
    TaskYieldTrampoline(1);
```
matches; the same loop written with `t = gUnk_03002490;` as the last statement of
the body does not — `t` is then live across the yield call, so it gets a
callee-saved register and the whole function's allocation shifts. gcc CSEs the
first iteration's read with the preceding statement anyway, so the inline form
still emits the ROM's single pre-loop load.

### 3.99 The destination address is loaded FIRST in a global-to-global store
`gUnk_03002358 = t->unk48;` with `t` already in a local emits the task load
first. The ROM loads the destination's pool word first, then the task. Writing
the first store as `gUnk_03002358 = gUnk_03002490->unk48;` and only then taking
`t = gUnk_03002490;` for the rest of the block reproduces it (gcc CSEs the second
read). Applies to every run of "copy N task fields into N globals".

### 3.100 Computed 16-bit call arguments belong in `s16` locals
For a 6-or-7 argument call, `f(a, b, c, 0, (s16)(x + y), (s16)(p + q))` evaluates
the casts late and interleaves them with the register arguments. Assigning
`s16 x = ...; s16 y = ...;` before the call reproduces the ROM's order (stack
arguments computed first, then `r0`-`r3`). Two functions in M17 (`sub_08066C74`,
`sub_08066E88`) needed exactly this.

### 3.101 `ldrsb` vs `ldrb` + `lsls #24`/`asrs #24` is register pressure, not syntax
Both read the same `s8` field. `ldrsb rd, [rn, rm]` needs a spare register for
the zero index; when none is free gcc falls back to `ldrb` plus a shift pair.
The same field in the same file compiles to both forms depending on what else is
live, so do NOT rewrite the field access to chase the opcode — fix the
surrounding allocation instead.

### 3.102 Give a subexpression shared by two switch arms its own local
`gUnk_0873E1E8[i * 2]` / `[i * 2 + 1]` in two arms of a switch left `i * 2`
recomputed inside the (cross-jumped) shared arm; the ROM had it hoisted above the
switch (`lsls r5, r0, #1`). Adding `j = i * 2;` before the switch and indexing
`[i * 2]` / `[j + 1]` reproduces the hoist and keeps `i` live for the strength-
reduced first index (`lsls r0, r0, #2`).

### 3.103 An extra pointer local can flip a whole function's register assignment
`if (gUnk_03002790[i].unk7A == 1)` and `t = &gUnk_03002790[i]; if (t->unk7A == 1)`
are the same code, but only the second reproduced the ROM's loop-invariant hoist
order in `sub_08066338`. When a function differs from the ROM by a consistent
register permutation and nothing else, adding (or removing) one user variable is
the cheapest lever — cheaper than the permuter, which failed on several of these.

### 3.104 Case labels are emitted in source order
A 4-arm switch whose blocks appear in the ROM as `case 0`'s body before
`case 9`'s must be written with `case 0:` first, even though the jump table is
index-ordered either way. Read the block order off the ROM, not the table.

### 3.105 agbcc rejects declarations in a nested block
`else { struct Task *u; ... }` produces `syntax error at end of input` reported
against a LATER function, which is very hard to read. Declare every local at the
top of the function body.

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

### 4.22b Curated symbol entries must bypass the prologue filter
`symdb.py` validated `EXTRA_THUMB_ENTRIES` candidates with
`plausible_thumb_entry(..., strict)` like any other address, so hand-verified
entries were silently DROPPED whenever they did not open with a `push` — e.g.
`0x08005A74` starts `ldr r0, [pc, #20]`. The symptom is subtle: the curated
address simply never appears in `symbols.csv` and its predecessor keeps an
oversized size, so the census looks self-consistent while being wrong. Seven of
fifteen curated additions were being lost this way before it was noticed.
Curation IS the evidence — `KNOWN_SYMBOLS` already bypassed the filter for the
m4a XCMD handlers, and `EXTRA_THUMB_ENTRIES` now does too. If you add a curated
entry, always re-grep `symbols.csv` for it rather than assuming it landed.

### 4.24 macOS is case-insensitive — scripted variant names can collide
Writing `t9C.c` silently overwrote `t9c.c` during a variant sweep and produced a
wrong conclusion before it was noticed. Name generated candidates with a scheme
that cannot collide under case folding.

### 4.23 A slept Mac can wedge the container runtime beyond `orbctl restart`
After the host sleeps mid-build, OrbStack can end up in a state where `docker
ps`, `docker version` AND `orbctl restart` all hang indefinitely (no output, no
timeout). Every agent depending on the toolchain stalls at once, which looks
like many independent failures rather than one environment fault. Recovery that
worked: `pkill -f "OrbStack Helper"; pkill -x OrbStack`, wait, `open -ga
OrbStack`, then confirm with `docker image inspect knidl-builder` and a real
`make compare` before resuming anything. Nothing is lost — `build/` is
regenerable and committed work is untouched — but re-verify the build rather
than assuming, and do not diagnose the stalled agents individually until the
daemon answers.

### 3.106 To land a commutative result in the CONSTANT's register, fold into a variable
`return r | (v & 0xFFF);` ties the `and`'s destination to `v`'s register
(`ands r2, r1`). The ROM had `ands r1, r2` — destination = the pooled constant's
register. Giving the mask its own variable and folding into it reproduces it:
```c
u32 m;
m = 0xFFF;
m &= v;
return r | m;
```
The constant's reload temp then IS the result pseudo, so no copy is needed and
the operands land ROM-side-up. Applies to any commutative operator whose ROM
destination is the constant side. (decomp-permuter found this after a dozen hand
variants failed — "assign the constant to a temp and compound-assign into it" is
the class of rewrite it is much better at than a human.)

### 3.107 Prefer repeating the global over a `T **g` local
A `struct Task **g = &gUnk_03002490;` local makes the address a *user* pseudo,
which the allocator treats differently from the compiler-generated CSE temp the
ROM has. `sub_08066754` only matched once `g` was deleted and
`gUnk_03002490->unk24` was simply written out in each arm — gcc then CSEs the
address itself, keeps it in a callee-saved register across the calls, and emits
the ROM's `adds r4, r1, #0` split. Reach for the `**g` idiom only when the plain
form is short by exactly a pool reload (it is still what `sub_080665A0` needs).

### 3.108 A per-case store beats a shared result variable when the ROM cross-jumps
`switch (...) { case 2: v = 13; break; ... } a->unk02 = v;` gives the shared
variable a hard register across the whole switch. Writing `a->unk02 = 13;` in
each arm lets gcc cross-jump the stores itself and reproduces the ROM's register
assignment (`sub_08066AE0`). The reverse is also true — `sub_080665A0` needed one
`s16` shared by both arms — so read the ROM: a value materialised per-arm into
the *same* register means one variable; a store repeated per-arm means several.

### 3.109 Taking a local's address can move a loop-invariant hoist
`sub_080668C8` hoisted its `-1` compare constant into the loop preheader BEFORE
the induction-variable initialisations; the ROM has it after, and no loop shape,
`continue`/`goto` form, declaration order, condition order, cast or `-f` flag
moved it (`-fno-schedule-insns` proved the scheduler is not involved — the order
comes straight out of `move_movables`, which always runs before
`strength_reduce`). What did work, found by decomp-permuter, is taking the
address of the one local that lives across the induction variables:
```c
u32 *p;
...
p = &cls;
cls = t->unk72;
if (*p == 6 && t->unk76 == 0)
    continue;
```
`cls` still ends up in a register — no stack slot appears — but the address-taken
pseudo changes what `move_movables` considers cheap enough to hoist first, and
the constant lands after the givs exactly as in the ROM. Keep this in the toolbox
for "one hoisted constant sits N bytes too early" diffs; it is not something a
human would guess.

### 4.25 Carving a range re-cuts every later chunk — `split.py` needs a raw-halfword escape
After `tools/carve.py` shortened `game_code_and_rodata_080653ec`, `make split`
died with `KeyError: ..._06` ("unplaceable `loc_` labels"): objdump had merged
two halfwords into one 4-byte instruction, but a cross-chunk `loc_` label had to
sit on the SECOND halfword. `emit_func_region` now emits raw halfwords when
`entry_size > size and self.label_pending_at(addr + 2)`, giving the label a
boundary. Expect this every time a carve moves a chunk edge.

### 4.26 fnmatch's pool-resolving diff is capped — fix from the top and re-run
The diff stops after a fixed number of blocks, so a function that "has no diff"
may simply be past the cap. Work top-down, re-running after each fix; the
`candidate=N bytes, target=M bytes` header and the total differing-byte count are
the honest progress metrics. A raw byte compare of `cand.bin` against the ROM
over-reports instead: `bl` targets outside the carved range are unrelocated in
the candidate, so every external call shows as a difference.

### 4.27 Long straight-line task bodies are worth transcribing mechanically
M17's two leaders (`sub_08062584` 0xA04, `sub_08062F88` 0x710) are ~230
statements of `gUnk_03002490->unk3C = K; TaskYieldTrampoline(n);` with occasional
6-argument calls. A ~90-line Python pass over the split asm — track constants in
a register map, resolve pool words from the ROM, emit one C statement per store
or call — produced both functions, and each matched on the FIRST fnmatch run.
Write the transcriber instead of the C when a function is long, regular and
literal-heavy; the residual `/*?? ... */` lines it cannot classify (about ten
here) are exactly the interesting parts.

### 3.110 No return value in the ROM means the function is `void`
The mirror of 3.94. `sub_08066FC0` was written as `s32 ... return i;`, which
costs a callee-saved register for `i` plus an `adds r0, r4, #0` before the
epilogue. The ROM leaves whatever the last call or compare put in `r0`, so the
value is never used: the function is `void`, and its only caller
(`sub_0806704C`) ignores the result. Read the epilogue before choosing a return
type — `pop {rN}; bx rN` with nothing feeding `r0` is the tell.

### 3.111 `a >= 1 && a <= 3` is always range-optimised; two signed compares mean a `switch`
`fold` rewrites every spelling of a two-sided range test — `>= 1 && <= 3`,
`> 0 && < 4`, `!(a < 1) && !(a > 3)`, `(a - 1) >= 0 && (a - 1) <= 2`, `&` instead
of `&&`, and every integer type for `a` — into `(unsigned)(a - 1) <= 2`. Nesting
the two `if`s keeps them separate but canonicalises `a >= 1` into `cmp #0; ble`.
The ROM's `cmp #1; blt` + `cmp #3; bgt` pair comes from `expand_case`, which
emits case-node compares against the literal case values and never sees `fold`:
```c
switch (v)
{
case 1:  sub_08063908(...); break;
case 2:
case 3:  sub_080639b4(...); break;
}
```
So "two signed compares against the actual bounds" is a `switch` fingerprint, not
an `if` chain.

### 3.112 A conditional expression can stop a constant living across a call
`sub_08066E88` calls `sub_08064FC4(0, 38, kind, 0, x, y, 0)` and later stores
`u->unk3C = 0`. gcc shared one zero pseudo between the seventh argument and that
store, so it survived the call in `r7` (`push {r4,r5,r6,r7,lr}`,
`movs r7,#0; str r7,[sp,#8]`, `strh r7,[r4,#60]`); the ROM materialises both
zeros separately and pushes only `{r4,r5,r6}`. Proof of causation: changing the
store to a non-zero constant made the whole function match. Nothing at the
constant end fixed it — typed zero casts, a zero variable in any subset of
arguments 1/4/7, address-taken locals, statement reordering, or the flags
`-fno-gcse -fno-cse-follow-jumps -fno-rerun-cse-after-loop
-fno-expensive-optimizations` (all four together still push `r7`, so the sharing
is not CSE). What fixed it was at the *other* end of the function: reading the
task pointer into its own local and selecting the value with a conditional
expression,
```c
s = gUnk_03002490;
v = (s->unk7F == -1) ? sub_08063b38() : s->unk7F;
```
which also restores the ROM's `movs r0,#0; ldrsb r0,[r2,r0]` (see 3.101 — the
sign-extending load needs a register that is not already the address). An
`if`/`else` with a temporary gets the prologue right but costs a second load;
the ternary keeps one load and still splits the block.

### 3.113 One local per statement group also decides WHICH register the pointer gets
3.97 says to give each re-read of a global pointer its own local. `sub_08067258`
shows the sharper version: in a 20-statement task body that re-reads
`gUnk_03002490` before every store, the statements whose stored value already
sits in a callee-saved register (`strh r5, [r0, #60]` with `r5 = 10`) load the
pointer into `r0`, while the ones that need `r0` for the value load it into
`r1`. Reusing one `t` for all of them forces an extra `adds r0, r1, #0`; giving
those groups their own locals (`u`, `v`) reproduces the ROM exactly. When a long
body is two bytes too big at one statement, split the local, not the statement.

### 3.114 An argument that is the symbol's address means the symbol is the object
`sub_08067170` passes `gUnk_0873F690` with a bare `ldr r0, =gUnk_0873F690`,
while the declaration `extern struct ActorDef *gUnk_0873F690;` compiled to
`ldr r0, =...; ldr r0, [r0]`. The extra load is the whole tell: declare the
symbol as the object (`extern struct ActorDef gUnk_0873F690;`) and pass
`&gUnk_0873F690`, or as an array when it is a table.

### 4.28 Attribute a diff only after the sizes agree
`attr.py` bucketed all 17 blocks of a mismatch into one function and hid five
others; the cap in 4.26 was only half the reason. While `candidate` and `target`
sizes differ, everything after the first size-changing diff is shifted, so both
the block addresses and their attribution are fiction. Fix size first (the
header line), re-run, and only then trust the per-function counts.

### 3.115 A "hold N extra frames" loop is a SIGNED count-up `for`, reversed by `check_dbra_loop`
Every class-1 task body in M18 has this shape, and no hand-written down-counter
reproduces it:
```
loc_TOP: ldr r4,=gUnk_03002490 ; ldr r1,[r4] ; ... ; bl TaskYieldTrampoline
         cmp r7,#0 ; beq loc_TOP        @ guard on the COUNT
         adds r5,r4,#0                  @ loop-invariant hoist: a COPY of the pool address
         adds r4,r7,#0                  @ the down-counter's initial value
loc_INNER: ldr r1,[r5] ; ... ; bl TaskYieldTrampoline
         subs r4,#1 ; cmp r4,#0 ; bne loc_INNER ; b loc_TOP
```
The two `adds` copies **in that order** are the whole tell. Write it as
```c
s32 d, n, i;
for (i = 0; i < n; i++) { gUnk_03002490->unk3C++; TaskYieldTrampoline(d); }
```
`loop.c`'s `move_movables` inserts the invariant pool-address copy immediately
before `NOTE_INSN_LOOP_BEG`, but `check_dbra_loop` — which runs later, inside
`strength_reduce` — reverses `for (i = 0; i < n; i++)` into a down-counter and
puts *its* initialisation at the END of the preheader, i.e. after the hoist
(3.84). That ordering is what keeps the outer pool pseudo's live range from
overlapping the counter, so the allocator can give both `r4` and the inner pool
pseudo `r5`. A hand-written `i = n; do {...} while (i != 0);` puts `i = n`
before the hoist, the ranges overlap, the outer pseudo is pushed to `r5`, the
copy collapses, and the function comes out 4 bytes short.
`check_dbra_loop` (`gcc/loop.c` ~7330) needs the comparison to be **`LT`
(signed)** — `u32` gives `LTU` and the reversal is silently skipped, and so does
`i != n` — plus increment `+1`, initial value 0, and a counter used only for
counting. Do not add a redundant `if (n != 0)` guard: that emits a second `cmp`.
(`sub_0806ef5c`.)

### 3.116 `break` out of a task loop ROTATES it; count the ROM's exit tests before choosing
`gcc/stmt.c:expand_end_loop` carries a CYGNUS "loop test" transformation: it
scans the first ~30 insns of the loop for a conditional jump to the loop's own
`end_label` and, finding one, moves everything from the loop top through that
test to the bottom, adding an entry `goto`. A `break` is exactly such a jump.
Getting this wrong costs 8 bytes *and* a mid-function literal pool, because the
rotated body pushes the pool out of `ldr rN,[pc,#imm]` range (3.6): 301
differing bytes in `sub_0806cd40` before the shape was right.
The decision is readable off the ROM — **count the copies of the loop's own
exit test**:
* two copies (one before the body, one after) IS the rotation, so the plain
  `while (c1 && c2) { ... break; ... }` matches (`sub_0806cd40`);
* one copy means the rotation did not happen, so spell the exits `goto done;`
  with `done:` after the loop. The jump target is then a user label, the scan
  finds nothing, and the loop NOTES survive so `move_movables` still hoists the
  pool-address copy of 3.115.

### 3.117 A pure register PERMUTATION is `regs_someone_prefers`, not a shape problem
`sub_0806c770` was instruction-identical to the ROM but 34 bytes off: ROM
`addr=r3 / k=r1 / dx=r4`, candidate `r4 / r3 / r1`. About eighty source
variants — statement order, declaration order, every integer type for every
local, `+=` vs `+`, goto forms, per-arm duplication, address-taken locals, dead
stores — all left it at exactly 34.
The cause is `gcc/global.c:find_reg`'s **pass 0**, which skips not only
conflicting registers but every register that a *lower-priority conflicting*
allocno **prefers** (`regs_someone_prefers`, built by `prune_preferences`).
Pass 1, conflicts only, runs only if pass 0 finds nothing — and pass 0
practically never fails, because r0-r3 are `call_used_regs` and are seeded into
`regs_used_so_far` unconditionally.
Here `u->unk1C = dy;` (an `s16` local into an `s32` field) expands to a pair of
block-local shift temps; `local_alloc` renumbers them and, the block's task
pointer already holding r0, one gets r1. `set_preference` then runs on the
store, takes `XEXP (src, 0)` — `dy` — and, because the destination is already
renumbered, records `dy prefers r1`. `dy` conflicts with `k` and is lower
priority (2 refs/28 = 714 against 8 refs/40 = 6000), so r1 is excluded for `k`
in pass 0; `k` takes r3, which pushes the long-lived pool-address pseudo to r4,
which pushes the `>> 16` temp to r1. One preference, a three-way rotation.
The escape hatch is in `prune_preferences`: a register the allocno **itself**
prefers is not excluded. So do not try to remove the other allocno's
preference — give the loser the same one, with an insn `(set (X) (op (k) ...))`
whose destination is block-local and lands on r1:
```c
if (t->unk43 == 1) t->unk30 = dx - (ka = k + 8);   /* not: dx - (k += 8) */
else               t->unk30 = dx + (kb = k + 8);
```
Three details are load-bearing: the assignment must stay **inside** the store
expression (hoisting it emits `adds r1,#8` before the other operand's
`lsls/asrs`, 12 bytes off); the two arms need **two different** temps, because
one shared temp is defined in two blocks, becomes a global allocno and
`set_preference` skips it; and `k + 8` without an assignment is tied back by
`regmove`. See 4.31 for how to read this out of `agbcc -da`.

### 3.118 `(x != 0)` as a VALUE only comes from `if (c) v = 1; else v = 0;`
Thumb has no `setcc`, and `expr.c:do_store_flag` bails out before reaching
`emit_store_flag`: `setcc_gen_code[NE] == CODE_FOR_nothing`, and its fallback
gate additionally wants `abs_optab` or `ffs_optab` for SImode, neither of which
Thumb has. So *every* spelling of a boolean value — `return (x & M) != 0;`,
`!!(...)`, `(...) ? 1 : 0`, a temporary, a `u8` return type — compiles to
`cmp rX,#0; beq .L; movs rX,#1; .L:`. The ROM's branchless
`negs r0,r1; orrs r0,r1; lsrs r0,r0,#31` comes from `jump.c`, which calls
`emit_store_flag` directly, with no gate, when it sees two constant stores to
the same pseudo. Write the `if`/`else` out.
(`sub_0806951c`, `sub_0806956c`, `sub_080695bc`.)

### 3.119 Two constant arms and one store: the ROM says whether it is `if`/`else` or `?:`
Both spellings are legitimate and they compile differently:
* `beq .L2; A; b .L3; .L2: B; .L3: <store>` — a branch **over** the second arm —
  is `if (c) f->x = A; else f->x = B;`, with cross-jumping merging the store
  (`sub_0806e424`, `sub_0806ed28`, `sub_0806bbe8`);
* `movs K1; cmp; beq .L; movs K2; .L: <store>` — one arm with a fall-through
  default — is the ternary `x = c ? K2 : K1;` (`sub_0806ff7c`).
Getting it backwards costs ~10 bytes each way. Two corollaries: with the
`if`/`else` form the *then* arm is emitted first, so spell the condition so the
arm the ROM lays out first is the `if` arm (3.64); and the shared-variable form
(`if (c) w = A; else w = B; t->x = w;`) is a *third* shape — "materialise B,
conditionally overwrite, one store" — which is 3.108 read from the other end.

### 3.120 Where the table base `ldr` sits picks the array spelling
Four different C spellings of "index a ROM table" put the base load in four
different places, and the ROM's placement tells you which one the source used.
Decide by what happens to the base:

| ROM | spelling |
|---|---|
| base `ldr` FIRST, before the index shifts | plain value subscript `arr[i]` |
| base `ldr` LAST, after the index arithmetic | `p = &arr[i];` (an ADDR_EXPR) |
| plain symbol kept live, two different `adds rD, rBase, #K` | 2-D array `arr[i][k]` |
| base bumped in place (`adds rBase, #K`) | `(&sym[K])[i * N]` |
| offset folded into the load (`ldrh rD,[rB,#K]`) | struct array, or a literal index |

The mechanism: a plain subscript expands the whole address under `EXPAND_SUM`,
where `force_operand` emits both operand loads before either shift (3.87); an
`&element` builds an ADDR_EXPR whose base load `update_equiv_regs` sinks down to
the add. So the choice is about the ADDR_EXPR, not about how many fields you
read — a *single* `gUnk_03002790[i].unk43 = 1;` already differs from
`p = &gUnk_03002790[i]; p->unk43 = 1;`.
In practice both forms occur in one file, and the index tells you which:
an index that comes from a **load** goes with the plain subscript (and with the
index assignment written inline inside the subscript, `arr[j = t->unk44].fld`,
which keeps the base load in front — 3.87); an index that comes from a **call
return** goes with `p = &arr[i];`. The 2-D form is the constructive companion of
3.26: it materialises the row base separately and keeps the plain symbol live,
so two column constants come out as `adds rD, rBase, #K` off one pool word,
which nothing else reproduces (fifteen other spellings of `gUnk_0873EAD8` all
sat at 50 differing bytes).
(`sub_0806e0f0` and seven clones, `sub_0806b410`, `sub_0806c5d4`,
`sub_0806cd40`, `sub_0806d1e8`.)

### 3.121 Narrow locals: where the conversion lands names the type
A 16-bit local is only right when the ROM converts at every USE; if it merely
reloads and compares, the local is 32-bit — a `u16` local costs
`adds rD, rS, #0` (PROMOTE_MODE's HImode store) even when `ldrh` already
truncated (3.24 from the other end). For a value coming out of a call, read
what sits between the `bl` and the extension pair:
* `bl; ldr r2,[r4]; lsls; asrs` — something loaded in between, so the
  conversion belongs at the use: an `s16` local, which extends on read (3.27);
* `bl; lsls; lsrs; ldr r2,[r4]` — conversion immediately after the call, i.e. a
  `u16` local truncating on write (3.37);
* conversion at the use but unsigned — an `s32` local plus an explicit `(u16)`.
Two traps. An `(s16)` cast **vanishes** inside an expression that is itself
narrowed to `s16` (`convert_to_integer`'s "shorten" drops it at the tree level),
no matter how many references the value has, so 3.82's escape does not apply —
if the ROM sign-extends an argument your candidate uses raw, the value is an
`s16` *parameter*. And you cannot infer parameter types by counting entry
truncations: combine merges an `s16` parameter's `lsls #16; lsrs #16` into the
`lsls/asrs` of its first signed use, so the entry pair survives only when that
use is far away — `sub_0806d08c(s16, s16, s16)` shows all three variants in one
prologue.
(`sub_0806ceb8`, `sub_0806cffc`, `sub_0806d08c`, `sub_0806bd10`.)

### 3.122 Reading a `switch` off the ROM: node count, root, and the condition code
Four independent tells, all of which came up in M18:
* **`bgt`/`blt` versus `bhi`/`bls` on a freshly zero-extended value.** For a
  value produced by `lsls #24; lsrs #24`, an `if` gives `bhi`: combine's
  `simplify_comparison` folds the shift pair into the comparison and, the
  operand being an LSHIFTRT, calls `unsigned_condition`. `expand_case` emits its
  compares against the index pseudo with the promoted `int` type and combine
  never sees a shift to fold, so a `switch` keeps `bgt`. An otherwise perfect
  `if` chain that differs from the ROM in exactly this condition code is telling
  you to write a `switch`.
* **The number of case NODES sets the tree root** (3.42), and `add_case_node`
  merges adjacent values that share a label. So `case 0: case 1: x = 7; break;`
  builds one RANGE node and emits a range test, while four separate bodies build
  four VALUE nodes and root at 1 — and jump.c cross-jumps the duplicate bodies
  away, so the OUTPUT is identical either way. Count the nodes in the dispatch
  tree, not the blocks in the output (`sub_080700e8`).
* **A case stacked on `default:` moves the root down.** When the ROM roots at
  the lowest case with an empty left arm, add a fourth case node below the range
  sharing the default's label (`case 0: default: c = -1; break;`);
  `balance_case_nodes` then roots one lower and jump.c folds the empty arm into
  the parent's `ble`. Writing that value as its own body instead fails whenever
  the function already materialises the same constant, because gcse shares the
  pseudo and the blocks stop being identical (3.112 family). (`sub_0806df28`.)
* **The operand keeps its own QImode type** — gcc 2.9's C front end does not
  integer-promote a switch operand, so `switch ((s8)t->unk7C)` is a faithful
  shape; `cmp rN,#K; bhi` with no subtraction means the lowest case label is 0
  (so a `case 0:` exists even with an empty body); and when the jump table's
  filler entries point at the switch's END label rather than at the `bhi`
  target there is **no `default:`** and every value is an explicit case
  (`sub_08069c8c` genuinely lists 26).

### 3.123 Write converging arms out in FULL and let cross-jumping merge them
Three separate M18 functions failed the same way: a block reached from two paths
was written once, and the ROM has its head duplicated in each predecessor.
jump.c's `do_cross_jump` merges identical tails, and 3.51 requires the arms to
use the same variables — so the source really did contain both copies.
* `sub_0806b670` loads the task pointer **in each predecessor** of a shared
  store; writing it once after an early-`return` is 4 bytes short and swaps
  `r8`/`r9` across the whole function.
* `sub_0806be4c`'s `movs r0,#0` sits in ONE predecessor of a shared four-`strb`
  clear, because the other path already knows the register is zero from the
  preceding `cmp/beq`. Duplicating the whole clear block lets cross-jumping
  merge just the four stores.
* `sub_0806f638` merges a **24-instruction** tail, `bl sub_08006338` included;
  only the argument setup is per-arm. A shared local plus one call after the
  `switch` gives a different, longer shape.
The corollary for case order: arm bodies are emitted in source order (3.104), so
the `movs r0,#K` block order in the ROM is the order the cases were written —
`sub_0806d4e4`'s is 1, 0, 2, 4, 3, 5, not the numeric order the jump table
suggests.

### 3.124 `pop {r1}; bx r1` is a statement about the function's TYPE
3.94 sharpened. `sub_0806b0f0` is a pure `switch` whose arms only call a void
function; nothing anywhere feeds `r0`. But its epilogue is `pop {r1}; bx r1` and
its switch operand lives in `r1`. Declaring it `s32 sub_0806b0f0(void)` **with
no `return` statement anywhere** matched byte-for-byte: a non-void return type
reserves `r0` and biases every allocation in the function away from it.
The mirror case is a `void` callee whose caller consumes `r0` — the original had
no prototype in scope and used K&R's implicit `int ()`. Reproducing that
literally costs `-Wimplicit -Werror`, which is on for every file; instead cast
the function **symbol**, which gcc 2.9 folds back into a direct call:
`r = ((u32 (*)(void))sub_0806a158)();` (an indirect call only appears when the
cast is applied to a pointer *value*). (`sub_08069bbc` calling `sub_0806a158`.)

### 3.125 `&=` then `|=` on one cell, with a store between, means `volatile`
```
ldrh r1,[r4] ; ands ; strh r0,[r4] ; ldrh r0,[r4] ; orrs ; strh r0,[r4]
```
A non-volatile `u16` global CSEs that pair into one load and one store
(`x = (x & M) | K`), 8 bytes shorter. `vu16` reproduces the ROM exactly, and
because this is a *plain symbol* store there is no extra dead pre-read (3.7 only
bites indexed lvalues). `gUnk_03001ED8`, the DISPCNT shadow, is written this way
all over M18's transition code. Keep in mind the mask constant is
loop-invariant and gets hoisted, so inside the loop it shows up as 3.24's
`adds rD, rInv, #0` copy before the `ands` — a copy that must NOT be removed.

### 3.126 Literal constants in a `while (1)` task body hoist THEMSELVES
`sub_0806e258` keeps `0x10000` / `0` / `0xFFFF0000` / `0xFFFF` in r9/r8/r7/r6
for a whole loop, and `sub_0806e424` keeps `5`, `6`, `0xFFFF`. These are plain
literals in the source, **not** 3.10 zero-variables: `loop.c` hoists them into
the preheader in order of first use in the body, and a constant used only once
per iteration is not hoisted at all. So the preheader's register order is a free
check on your statement order — and the "one variable per constant" reflex from
M17 is wrong inside an infinite task loop.

### 3.127 Reading an UNINITIALISED local is a real ROM shape — do not fix it
Three M18 functions do it, and each needs it to match: `sub_0806b410` reads `r8`
sixteen insns before its only write; `sub_0806caa0` returns a value the default
switch arm never sets; `sub_0806d4e4`'s cross-jumped arms leave the variable
untouched on the default path (which is *why* they merge — 3.51). Write the
local, read it, and compile with `--no-werror`; adding an initialiser costs two
bytes and shifts the tail. Same family as 3.93: the original source's bugs are
part of the target.

### 3.128 Pre-shifted 16-bit constants are the `s16`-local fingerprint
`s16 r; r += 64;` materialises the addend **already shifted** into bits 16-31
(`movs r1,#128; lsls r1,#15`); `u16 r; r += 64;` gives the shorter
`adds rD,#64` before the truncation. Same for `|` and `&`: an `s16` destination
pools `0xF0000` for `| 0xF` and `0xFFF00000` for `& 0xFFF0`. So whenever a
16-bit computation carries its constants pre-shifted left by 16, the destination
local is `s16`. To move the truncation away from the operator, split it with an
intermediate: `m = t->unk48 - 16; y = m | 0xF;` costs no instructions, while
`y = (t->unk48 - 16) | 0xF;` stays in SImode and emits `movs r1,#15; orrs`.
(`sub_0806a03c`, `sub_080699a8`.)

### 3.129 `fold_truthop` merges same-polarity bit tests only
`(f & 1) != 0 || (f & 0x40) != 0` and `(f & 1) == 0 && (f & 0x40) == 0` both
collapse into one `(f & 0x41)` test. MIXED polarity —
`(g & 1) != 0 && (g & 0x40) == 0` — never merges and stays two `ands`. So when
the ROM keeps two same-polarity tests apart, they were separate statements;
transcribe them with a label and a `goto`. Extends 3.44 from range guards to bit
tests.

### 3.130 `A + (B + K)` is re-associated; a `K` held in a local is not
Every literal spelling — `a + (b + 4)`, `a + b + 4`, `a + (4 + b)` — lets `fold`
re-associate into `(a + 4) + b` and emit `adds rD, rD, #4` right after the first
load. The ROM adds the constant to the *second* operand. Assigning the constant
to a plain local first blocks the fold at zero instruction cost — the `#4` still
ends up an immediate, and the local never gets a register. (`sub_0806d7ec`.)

### 3.131 Hoisting a load out of a call argument extends a CSE'd symbol's live range
`sub_0806ff7c`'s ROM pools `gUnk_03002490` **twice**: the CSE'd address pseudo
dies mid-function. Computing a table index into a local *before* the call keeps
one pseudo alive across all 192 insns and costs an extra callee-saved register.
Writing the index expression inline in the argument fixes it, because gcc
evaluates argument 1 (itself a call) first, so the global re-read lands after
that call — fresh load, short live range. Diagnostic: **the ROM pools the same
global twice and your candidate pools it once** (3.19 applied to a live range
rather than to a pool word); `.greg` names it in one line
(`Register 194 used 3 times across 192 insns; crosses 2 calls`).

### 3.132 `muls rD, rS` ties to the operand the source names SECOND
Written `t->unk43 * tbl[i]`, agbcc loads `unk43` first and multiplies the table
byte into it; the ROM loads the table byte first and does `muls r0, r2` with r0
holding `unk43`. Swap the source operands. The operand written first is
evaluated first, and the two-address destination ties to the second.
(`sub_0806cbd4`, 27 differing bytes to 0 with nothing else changed.)

### 3.133 `movs rN, #255` is an `s8` local holding -1
`movs r4,#255` followed later by `lsls r4,#24; asrs r4,#24` **writing back into
r4** is a plain `s8` local assigned -1, whose QI-to-SI extension happens once at
its first signed use and is reused afterwards. Spelling it `u8 sign = 255;` with
`(s8)sign` at each use creates a second pseudo, and that one extra long-lived
value pushed `sub_0806ff7c` from six to seven callee-saved registers. Contrast
3.76's genuine `u8` + per-use-cast case, where the extension does *not* write
back to the same register.

### 3.134 `bne default; b case` comes from a first case whose body is a bare `goto`
`expand_case` always emits `do_jump_if_equal` for a single-valued leaf, i.e.
`cmp K; beq case; b default`, so the ROM's `cmp #8; bne default; b body` cannot
be produced that way. It IS what you get when the case is the first arm in
source order and its body is nothing but `goto label;`: RTL order then puts the
case label immediately after `emit_jump_if_reachable(default_label)`, which
triggers jump.c's "conditional jump jumping over an unconditional jump"
(`invert_jump (insn, JUMP_LABEL (reallabelprev))`), inverting the `beq` and
deleting the `b`. (`sub_0806a6e0`.)

### 3.135 `T **g` versus repeating the global — read the diff, do not apply a rule
3.107 says prefer repeating the global to a `struct Task **g` local. M18 found
both the case that needs the opposite and the case where it does not matter.
* **Needs `**g`**: `sub_0806a6e0`'s ROM keeps ONE pool word in r4 and re-loads
  the pointer through it in three different basic blocks, then the block after
  the switch loads its own pool word. Writing every mention as the plain global
  gives three address pseudos and lets gcse hoist the tail's load into the
  switch arms — an extra pool word plus an `adds r0, r4, #0` on the fall-through
  path. Diagnostic: the ROM has FEWER pool words for the global than your
  candidate AND your candidate has a `adds rX, rPool, #0` copy on a merge path.
* **Does not matter**: `sub_0806e73c`'s setup block is byte-identical written
  either way, because CSE collapses the repeated loads into the pseudo the local
  would have created. All of `src/actor_6e0f0.c` matched with locals.
So treat this as a diagnostic, not a rule: reach for the rewrite when the diff
shows a spurious `adds rX, rY, #0` before an offset fix-up, and never churn a
file that already matches. The real lever for *which* register each group gets
is still 3.113.

### 3.136 A zero variable's `movs` lands correctly only after the first store's address
Extends 3.10. Three M18 functions put `movs rN, #0` *after* the first store's
address computation; writing the zero variables at the top of the block emits
the `movs` two instructions too early. The shape that matches puts the
declaration-initialisation after the first statement of the group:
```c
t = gUnk_03002490;
zero = 0;
t->unk18 = zero;
```
When two zero variables of different widths are needed (a `u8` for the `strb`s,
an `s32` for the `str`s, 3.50) both go there in either order — emission order is
decided by first use, not by source order.
(`sub_0806a638`, `sub_0806a9d8`, `sub_0806ac6c`.)

### 3.137 A ROM base added LAST in an integer address sum needs its own local
3.90 gets the operand order right, but the base's pool `ldr` still sinks below
the shifts, because `update_equiv_regs` moves a single-use constant load down to
its use. The ROM loads it first. Assigning the base to a local pointer restores
the order at no cost:
```c
tbl = gUnk_0873E5F8;      /* ldr r4, =0x873E5F8 lands HERE */
a = n << 1; a += i << 2; a += (u32)tbl;
```
(`sub_0806a5a0`, 23 differing bytes to 0.)

### 3.138 A commutative sum mixing a MEM and a computed value: source order picks the address
`(s16)(t->unk43 * a + t->unk48)` computes the product and then the address of
`unk48`; the ROM materialises `&t->unk48` first. Writing the field first —
`(s16)(t->unk48 + t->unk43 * a)` — reproduces it exactly and does not change the
`adds` operand order, since the product still ties the destination. A
`s16 *p48 = &t->unk48;` local works too but costs a user pseudo. Companion to
3.87/3.89.

### 3.139 `sym[K]` folds into the pool word; only a VARIABLE index keeps the base
Thumb `ldrsh` needs `reg+reg`, so `((s16 *)gUnk_03005550)[4]` is emitted as
`ldr rB, =gUnk_03005550+8` plus `movs rI,#0`. The ROM's `ldr rB, =gUnk_03005550`
plus `movs rI,#8` needs the offset to reach expand as a register. A pointer
local gives the right addressing but emits the pool load at the assignment,
before the earlier call arguments (8 bytes wrong); a plain index variable set
next to the other initialisations gives both the addressing and the emission
order, because cse folds it to 4 only after the address form has been chosen.
(All three input dispatchers in `src/actor_692fc.c`.)

### 3.140 Assorted M18 one-liners
* **Fall-through `case` chains are how a small multiply is spelled.** `unk7D * 64`
  is seven fall-through arms each doing `r += 64`. N identical blocks in a row
  whose jump-table entries point at successively later blocks is a fall-through
  chain, never a loop (`sub_0806a03c`).
* **`!= K1 && == K2` collapses** — gcc deletes the redundant first compare.
  Nesting the two `if`s keeps both, exactly as 3.44 does for range guards
  (`sub_0806bfd8`).
* **The return-value extension pair names the CALLEE's return type**: `lsls #16;
  asrs #16` after a `bl` is an `s16` return, `lsls #16; lsrs #16` a `u16` one.
  Related trap: `src/early_6464.c` declares two helpers with a `u16` first
  parameter, but every M18 caller passes a negative base, where `u16` folds the
  constant to `0xFFF4` and pools it instead of the ROM's `movs; negs`. Declare
  the parameter `s32` in the caller's file — `s16` does not work, because ARM
  `PROMOTE_MODE` zero-extends (3.27).
* **One `= 0xFFFF` store is a plain `strh`; two or more become 3.74's OR quirk.**
  Both come from the identical source line, so do not rewrite the OR-shaped one
  as `|=` — that adds a fourth instruction (`sub_0806e138` vs `sub_0806e258`).
* **Two byte-identical neighbouring functions really are two functions.**
  `sub_0806b1c4` and `sub_0806b1f4` are the same eight statements with the same
  constants and the same table. Write them out twice.

### 3.141 A `switch`'s dispatch shape is decided by the SOURCE ORDER of its arms
For `switch (n)` over {0,1}, {2}, {3}, `balance_case_nodes` always roots the
tree at `[2]` and `emit_case_nodes` always emits
`cmp #2/beq body2; cmp #2/bgt test; <left subtree>; test: <right subtree>`.
jump.c then applies its range-swap (the block commented `insn = condjump
label1; ...range1...; jump label2; label1: ...range2...; jump; label2:`),
which **inverts the condition and exchanges the two ranges** whenever
`next_label(test_label)` is what range1's trailing jump targets — that is,
whenever the FIRST case body in source order belongs to the left subtree.  So

    source order 0/1, 3, 2  ->  `ble left`  (right subtree inline)
    source order 3, 2, 0/1  ->  `bgt right` (left subtree inline)   <- the ROM

`sub_080c18c8` matched only in the second order.  When a switch is
instruction-identical except that the dispatch branch polarity and the
subtree order are flipped, permute the arms; a sweep over all orderings of
the arms plus `default:` is a couple of minutes and settles it.

### 3.142 `ptr + int` is normalised, so the pointer's register is the one reused
`build_binary_op` rewrites `int + ptr` into `ptr + int`, and local-alloc ties
an `adds` destination to its FIRST operand.  `p = base + off;` therefore always
compiles to `adds rBase, rBase, rOff`, and `p` inherits *base*'s register.
`sub_080bf0ac`'s ROM does the opposite — `adds rOff, rOff, rBase`, freeing the
pointer's register for the next load.  The fix is to drop the pointer local and
index the base array directly:

```c
v = u->unk2C * 5;
q1 = r[v + 1];  q2 = r[v + 2];  q3 = r[v + 3];   /* not: p = r + v; p[1] … */
```

### 3.143 `A - (B - C)` is re-associated; the ROM's `subs rT, rB, #C` means two statements
fold-const.c's `associate:` path splits `arg1` into var and con, so
`tbl[n * 5 - (q - 2)]` becomes `(5n + 2) - q` and emits `adds r0,#2; subs r0,r0,q`.
When the ROM computes the inner difference in its own `subs r1, r4, #2` and only
then subtracts, the original had the two halves as separate statements, in the
ROM's evaluation order:

```c
vv = u->unk2C * 5;
m  = q - 2;
t->unk28 = tbl[vv - m];
```
(`sub_080bf394`.)  The mirror of 3.130.

### 3.144 One register across two disjoint live ranges is usually ONE variable
`sub_080bf394` kept an `u8` RNG result and an `u8` animation index both in r4.
Merging the two locals into a single `u8 q` took the function from 22 differing
bytes to MATCH; no amount of declaration-order or type permutation had moved it
before.  Read it the other way too: when the ROM uses two registers for what
looks like one value, split the variable (3.117's companion at the source level).

### 3.145 A leading `ldr rX,=pool; ldr rY,[rX]` may have no local behind it
`sub_080bf394`'s prologue only matched after the `t = gUnk_03002490; …
t->unk15 = 7;` local was deleted and the two field accesses written as
`gUnk_03002490->unk44` / `->unk15`: cse builds the pseudo either way, but the
pseudo NUMBERING that results is what the ROM's register assignment needs.
Same for `sub_080c061c`, where dropping `t`/`u` moved the fourth argument out of
`ip` into a low register and removed 12 bytes of `mov ip` shuffling.  Try both
spellings whenever a function is instruction-identical but permuted; it is one
edit, and 3.135 is the same lesson seen from the `T **g` side.

### 3.146 Which loop invariant gets hoisted is arithmetic, and the loop dump prints it
loop.c moves a movable when `threshold * savings * lifetime >= insn_count`,
with `threshold = (loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (13 in the
game-code zone), `savings` counting the movables `combine_movables` merged into
it, and `threshold -= 3` after every move.  `agbcc -dL` prints exactly these:

    Insn 337: regno 126 (life 2), move-insn savings 2  moved to 431
    Insn 352: regno 129 (life 5), move-insn savings 1 not desirable

`sub_080bff28`'s loop hoists two invariants out of three; the ROM hoists the
`gUnk_08756770` base and leaves `movs r1,#3` inside, the candidate did the
reverse, and the whole 63-byte diff was that one swap.  loop.c runs twice and
the second pass sees a smaller `insn_count`, so a decision can flip between
passes — read the SECOND `Loop from …` block for the loop you care about.

### 3.147 One pointer variable shared by two loops outranks the task-pointer pseudo
Global-alloc sorts by `floor_log2(n_refs) * n_refs / live_length` (4.31), and
`n_refs` is loop-depth-weighted, so a value set in a preheader and used twice
inside scores `1 + 2*2 = 5`.  The current-task address, used three times in the
same loop, scores 7 and always wins — it takes r4 and pushes the table base to
r6.  In `sub_080bff28` the ROM has it the other way round, and the reason is
that ONE `s16 *` variable carries `gUnk_087567A0` for the second wait loop and
`gUnk_08756770` for the main loop: as a single pseudo set in two loops its
`n_refs` is 8 over both, `floor_log2` steps from 2 to 3, and it is allocated
first.  Two identical `ldr r4, =<table>` hoists in two different loops of one
function are the fingerprint — do not assume they are two variables.

### 3.148 Assorted M36 one-liners
* **`while (1) { A; if (t) break; body; }` puts an un-CSE'd copy of `A` before
  the loop and a CSE'd copy at the bottom.**  That asymmetry is gcc's loop
  exit-test duplication, not a `**g` local and not `volatile` (`sub_080bdab4`).
* **`while (cond) yield;` gets its exit test duplicated too**; the ROM's
  jump-into-the-test shape needs `goto wait; do { … wait: ; } while (cond);`
  plus hand-hoisted invariant locals, because the `goto` gives the loop two
  entries and also stops loop.c hoisting (`sub_080be1ec`).
* **A three-case switch is always root-balanced.**  A lopsided
  `cmp #1/beq/ble default` tree therefore means a FOURTH, empty case —
  `case 0: break;` (`sub_080be774`).  `use_cost_table` never helps for values
  1..3, they are all ISCNTRL.
* **When the ROM materialises an array base BEFORE the object pointer, drop the
  pointer local**: `tbl[gUnk_03002490->unk1C]`, not `t = gUnk_03002490;
  tbl[t->unk1C]` (six functions in this module).  The converse — an explicit
  `base = tbl;` local — is what forces the base out before the index
  (`sub_080c1ebc`).
* **`sym[K + i]` does NOT fold `K` into the pool word** (the complement of
  3.139): the three parallel byte tables at 0x0875665C/0x0875665F/0x08756662
  each need their own symbol.
* **A loop whose pointer walks backwards wants the offset in a second
  induction variable.**  `for (i = 3; i >= 0; i--) r[i * 15 + 6] = 0;` splits
  the `+6` into the MEM offset; `j = 51; for (i = 3; i >= 0; i--) { r[j] = 0;
  j -= 15; }` gives the ROM's `adds r0,#204` / `str r3,[r0,#0]`
  (`sub_080c1f9c`).
* **`s32 i` versus `s8 i` for an `s8` array element moves a pool load**
  (`sub_080bdd28`).

### 3.149 A variable ASSIGNED IN BOTH ARMS is not a loop movable — and that is how you keep a constant out of a register
`sub_080bff28`'s inner loop ends in

```
    bgt else ; adds r0,r2,#1 ; b join ; else: subs r0,r2,#1 ; join: movs r1,#3 ; ands r0,r1
```

Written the obvious way — `d = (m + 1) & 3;` / `d = (m - 1) & 3;` — the two
arms produce two `(set reg 3)` movables, `combine_movables` matches them
(savings 2, lifetime 2), and loop.c hoists the constant into a fourth
callee-saved register.  Padding the loop with a narrow local (`u8 d`) pushes
`insn_count` past `13 * 2 * 2` and stops the hoist (3.146), but a narrow local
makes gcc treat every def of `d` as partial: `d` becomes live across the loop's
calls, picks up a hard-r0 conflict, and lands in r1 instead of the ROM's r0.
The way out is `scan_loop`'s own rule that a register set more than once in the
loop is not a movable at all:

```c
if (u->unk28 <= 2) { n = m + 1; msk = 3; d = n & msk; }
else               { n = m - 1; msk = 3; d = n & msk; }
```

`msk` and `n` are ordinary `s32` locals assigned in *both* arms, so neither is
hoistable, `d` stays a full-width pseudo, cross-jumping still merges the
`movs`/`ands` tail, and the arms match to the byte.  Reusing `n` again for the
table value two statements later (`n = tb[d]; n -= tb[m];`) ties the `subs`
destination to the same register the ROM uses and closes the function.  Read
`;; N conflicts:` in the `.greg` dump for the hard registers (4.31): a lone
`0` in an allocno's conflict list that disappears when the narrow type does is
this exact effect.

### 4.29 Literal pools cross function boundaries — cut a prefix at the POOL
`sub_0806efec` (`symbols.csv` size `0xF6`, nominally ending at `0x0806F0E2`)
loads eight of its constants from a pool at `0x0806F158-0x0806F174`, well past
its own end. Running `tools/fnmatch.sh` on the nominal range prints a wall of
fake differences, and the tell is asymmetric: the **rom** side shows an
unresolved `ldr rN, [pc, #K] @ (0x……)` while the **candidate** side shows a
resolved `ldr rN, =VALUE`, because the pool word is outside the compared slice
and the tool could not fold it. Extend the range past the pool — usually to the
start of the next real function — and re-run. The same rule governs partial
landings: a matched *prefix* is only carvable if it ends after the pool.
Dumping every pool in a range with a script (3.85) up front is worth the two
minutes; in some ranges every pool ends exactly on the next entry, and then any
`symbols.csv` boundary is a valid cut.

### 4.30 The census has two systematic blind spots in the game-code zone — sweep for them BEFORE writing C
M18 found **seven** defects in 256 entries; batch 8 alone had five in 31. Both
classes come from the zone's own properties, so expect them in every bulk
module, and both sweeps take about a minute.

*Invented functions.* `.word 0xFFFFF000` in a literal pool always disassembles
as `bl <pc + 0xFFE>`, so it invents a function roughly 4 KiB further on: this
one constant produced `0x08063DFE` and `0x080643A2` (#65) and `0x0806F0E2` and
`0x0806FC3E` (#64). A `b.n` that skips a mid-function pool (3.6) reads the same
way (`0x0806FFF8`, `0x08070406`), as does a mid-function address a stray ROM
word happens to match (`0x080706A8`). **Sweep**: disassemble the first
instruction of every entry (`arm-none-eabi-objdump ... -Mforce-thumb` over the
baserom slice). Anything that is not a prologue is a suspect; look at what
precedes it. Preceded by a literal pool, it is a genuine straight-line leaf
(3.80); preceded by real code, the recorded size of the function *above* it is
wrong. For any `bl-target`-only symbol `S`, also check whether the word at
`S - 0x1002` is `0xFFFFF000`.

*Missed functions.* The zone is compiled `-fprologue-bugfix`, so a leaf has **no
`push {lr}`** and the prologue scan cannot propose it; unless something points
at it, it is silently merged into its predecessor's size. `0x080694E0`,
`0x0806ACF8`, `0x0806B40C`, `0x080702D8` and `0x08070454` were all recovered
this way, and three of them are live code with real callers — `0x0806ACF8` had
six. **Sweep**: `grep -o 'bl\tloc_[0-9a-f]*'` over the split asm lists every
`bl` whose target `split.py` could not name; function pointers stored into
fields (`Task.unk00`/`unk04`/`unk0C`) never appear that way, so also scan the
pool words for odd values inside your own range.

Both classes are curated in `tools/symdb.py` (`FALSE_POSITIVES` /
`EXTRA_THUMB_ENTRIES`), and both must be fixed before `tools/carve.py` will take
the range. The compact diagnostic, which generalises 4.13's "a size that does
not tile is a signal": **when the byte after a function's real epilogue is a
prologue, the census merged two functions; when the entry has no prologue and is
reached by fall-through, the census invented one.** Getting either wrong is not
a small diff — a false entry makes you write two C functions where the ROM has
one, and agbcc gives each its own literal pool, so the range can never match.

### 4.31 Read the allocator's own dump instead of guessing at register diffs
`agbcc -O2 -mthumb-interwork -fprologue-bugfix -da -o out.s in.c` writes
`<file>.greg`, and two parts of it answer almost every register-permutation
question:

* the header lists each pseudo as `Register N used X times across Y insns`
  (that is `n_refs` and `live_length`) and ends with `;; Register dispositions:`,
  the final hard register per pseudo. Allocation order is `qsort` by
  `gcc/global.c:allocno_compare`, priority
  `floor_log2(n_refs) * n_refs / live_length * 10000 * size`, highest first,
  each taking the lowest non-conflicting register;
* just above the `Spilling for insn` trace, each allocno's conflicts and
  preferences: `;; 31 conflicts: 22 29 … 0 13` (trailing small numbers are HARD
  registers) and `;; 30 preferences: 1`. Grepping
  `^;; [0-9]* preferences: 1` is a one-step oracle for "does anything still
  prefer r1?" and steers far better than the byte count.

`in.c.loop` and `in.c.cse2` show which pass created an extra reference. A full
agbcc source tree (`git clone https://github.com/jiangzhengwenjz/agbcc`, commit
`59b966e`) is worth keeping in a gitignored scratch dir for this; `grep -rn
OLD_COMPILER` still lists the nine behavioural differences between the two
compiler binaries. Two minutes of dump beat an afternoon of source shapes on
both of M18's hard functions (3.115, 3.117).

### 4.32 Grep the config before inventing a `gUnk_` name
`tools/fnmatch.sh` resolves any `gUnk_<hexaddr>` automatically, so a wrongly
named cell **matches** and only fails at the real link with "undefined reference
to `gUnk_0300248C'". Before naming a pool address, grep
`tools/split_config.json` `data_symbols` and the headers: `0x0300248C` is
`gCurTaskIdx`, and the task zone also pools `gUnk_03002490`, `gUnk_03002790` and
`gUnk_03001ED8` (the DISPCNT shadow, `vu16`) constantly. The same asymmetry
applies to every symbol fnmatch stands in for — a green `fnmatch` is necessary
but not sufficient; the landing check is `make clean && make compare`.

### 3.150 Write literal `0`s in a task body; do not invent zero variables
M17/M18 taught the zero-variable idiom (3.10, 3.136) and M25 shows its limit:
in six of its bodies the plain literal is what matches and a `zero` local is
what breaks. The mechanism is that cse already gives the repeated zero one
pseudo per MODE (one for the `strb`s, one for the `strh`s, one for the `str`s)
and places its `movs` where the ROM has it — including *before* an intervening
call, which is exactly what a hand-written variable is usually reached for.
Reach for a variable only when the ROM keeps the zero in a **high** register
(`mov r8, rN`) or when one register serves stores of two different widths;
otherwise start from literals. (`sub_0809074c`, `sub_08092198`,
`sub_08092cdc`, `sub_08091f08`, `sub_08093ac8`, `sub_08093bd4`.)

### 3.151 A `while (cond) yield;` loop has two shapes; count the exit tests
3.116 explains the rotation for `break`; the plain wait loop needs the same
count. With **two** copies of the exit test (one before the body, one after)
the natural `while (gUnk_03002490->unk7A == 0) TaskYieldTrampoline(1);`
matches, and the entry copy re-uses the pointer the preceding statement left in
a register (3.98). With **one** copy and a `b` into it, gcc's
`expand_end_loop` rotation is what you want but the plain `while` will not
produce it — write
```c
while (1) { if (cond_is_false) break; body; }
```
which rotates the same way *and* keeps the loop notes, so a loop-invariant
pool address stays hoisted. The `goto wait; do { … wait: ; } while (cond);`
form of 3.148 gives one test copy too, but kills the hoist and re-loads the
pool inside the loop — use it only when the ROM really does re-load.
(`sub_08091390` needed the `while (1)` form, `sub_080903f0` the plain one.)

### 3.152 An explicit pointer copy inside the arm that crosses a call splits the live range
`sub_08091390` reads the running task once, tests one field, and in the `else`
arm keeps the pointer across three calls. Written the obvious way the pointer
becomes one long-lived pseudo, takes a second callee-saved register and pushes
the pool address out of `r4`; the ROM instead copies it (`adds r4, r1, #0`)
*inside* the arm. Reproduce that with a second local assigned from the first
at the top of the arm:
```c
v = gUnk_03002490;
if (v->unk1C == 0) v->unk20 = 4;
else { p2 = v; p2->unk24 = abs_expr; … }
```
`v` then dies at the test and only `p2` is saved. Diagnostic: the ROM pushes
one callee-saved register fewer than your candidate and has a
`adds rN, rM, #0` copy at the top of one arm.

### 3.153 A wait loop that indexes a task table wants the address assigned inside the condition
The shape is `while (arr[p->idx].field != 0) { yield; p = g; }`. Written that
way the element address is built base-first and the offset lands in the index
register; the ROM builds it base-**last** with the field offset folded into the
load. Assigning the element address to a local *inside the condition* gives
both, at no instruction cost:
```c
p = gUnk_03002490;
while ((q = &gUnk_03002790[p->unk44])->unk20 != 0)
{
    TaskYieldTrampoline(1);
    p = gUnk_03002490;
}
```
(`sub_08091b6c`; 3.120's "`p = &arr[i]`" row, applied to a loop test.)

### 3.154 A narrow signed load whose extension must survive needs its own `s32` local
`ldrsh` in the ROM where your candidate emits `ldrh` means the sign extension
matters, but writing `(s16)` inside the expression does not help: if the result
is stored back into a 16-bit field, `convert_to_integer` shortens the whole
tree and the extension disappears (3.82's mechanism, seen from the store side).
Give the load its own statement and a 32-bit destination:
```c
m = ((s16 *)v)[27];          /* ldrsh — the high half of a 16.16 field */
v->unk4A = u->unk4A + m;
```
`update_equiv_regs` then sinks the load to its use, so the emission order is
unchanged. (`sub_08091d24`, `sub_08091ffc`.)

### 3.155 Which operand a `muls` names decides more than the multiply
3.132 says the two-address destination ties to the operand named second. In
`sub_08090fe0` that choice also decided which of two long-lived pointers got
`ip`: with `(u16)u->unk43 * gUnk_087438A4[i]` the product's temp took `r3`,
which pushed the task pointer to `r4` and the table pointer to `ip`; with the
operands swapped (`gUnk_087438A4[i] * (u16)u->unk43`) the temp took a different
register, `r3` stayed free for the element pointer and the task pointer landed
in `ip` exactly as the ROM has it. When a function is instruction-identical but
two registers are exchanged and a multiply is involved, swap the multiply
before hunting anything else.

### 3.156 Reload-scratch registers are not reachable from the source
`ldrsh`/`ldrsb` with a register offset is `*extendhisi2_insn`, whose index is a
`(clobber (match_scratch))`: reload fills it, so the register is chosen by the
`last_spill_reg` rotation (3.39) and by which hard registers the allocator has
already handed out — not by anything in the C. `sub_08091e18` matches the ROM
in every instruction and differs only in three such registers (two `ldrsh`
indices and the `subs rD, rS, #1` that decrements a field); ~40 source shapes,
both compiler binaries, and 9,400 decomp-permuter iterations all leave them
alone. Recognise the pattern early: if the diff is only the *register* fields
of insns whose registers are scratches or reload destinations, the lever is a
different number of reload allocations **earlier** in the function (3.39/3.40),
not a different expression shape — and if none of the plausible earlier changes
is byte-neutral, stop and record it, as this one is.

The dump reads this out directly and is a much better signal than the byte
count: `agbcc -da` writes `Spilling for insn N.` for every insn reload
processes and `Spilling reg R.` each time it takes a hard register for reload
use. `sub_08091e18` prints `4, 2, 4, 4` where the ROM needs `4, 5, 5, 4`, and
the choice is reload's spill-cost comparison over the pseudos currently
allocated to each candidate register (`.greg`'s header gives each one as
`Register N used X times across Y insns; crosses K calls`), not the rotation
alone. Two useful facts fell out of probing it: the sequence *is*
source-sensitive (indexing the table from the other struct moved the second
entry to `3`, hoisting the animation pointer into a local moved it to `1`),
and it is stable under everything that only renames or reorders locals. If you
need to move a scratch register, drive the search off the `Spilling reg`
sequence — one dump per variant, no linking — and stop when the sequence stops
moving.

### 3.157 One more use of a temporary can flip a register-priority tie
Global-alloc sorts by `floor_log2(refs) * refs / live_length` (4.31), so moving
a single reference across a `floor_log2` step re-orders two allocnos. In
`sub_08093cf8` the ROM has the 16.16 pair in `r1`/`r2` and the candidate had
them swapped; splitting the last use of the value into its own local
```c
y = x >> 16;      /* instead of  v->unk48 = x >> 16; */
v->unk48 = y;
```
dropped `x` from 5 refs to 4, flipped the tie and closed the function. The
mirror lever is 3.144 (merge two locals into one); read the ROM to see which
direction you need.

### 3.158 `p += 1` between two `ldrsh` reads is not the same as `p[1]`
Both spell "the next halfword", but `p[1]` puts the offset in the index
register (`movs rI,#2; ldrsh rD,[rP,rI]`) while an explicit increment bumps the
pointer and leaves the index zero (`adds rP,#2; movs rI,#0; ldrsh rD,[rP,rI]`)
— which is what the ROM does when it reads `Task.unk48` and then `Task.unk4A`
through one pointer. `*p++` and `r = p + 1` both fold back to `&u->unk4A` and
re-materialise the address from the struct base, so the two-statement form is
the only one that matches. (`sub_08093cf8`.)

### 3.159 A 2-D ROM table is still visible in the addressing mode
3.26 read this from the row-base side; M25 shows the flat/2-D distinction in a
plain lookup. `tbl[a*2 + b]` on a `u32 tbl[]` computes the index and then
scales it (`lsls #1`, `adds`, `lsls #2`); `tbl[a][b]` on a `u32 tbl[][2]`
distributes the scale into the two terms (`lsls #3` and `lsls #2`, then one
`adds` each) — which is what the ROM does whenever a table is indexed by two
fields. Declaring the extern as `[][2]` is also the honest description of the
data. (`sub_08093dcc`.)

### 3.160 A `vu8`/`vu16` cell read puts the load before the mask constant
3.49 established this for old_agbcc byte fields; it holds for agbcc globals
too. `if ((gUnk_03005550 & 2) != 0)` on a plain `u8` emits `movs rK,#2` first,
on a `vu8` the `ldrb` first — and in the same function the volatile read is
also what puts `ldrb rV; adds rT,rK,#0; ands rT,rV` in the ROM's order instead
of the copy-first form of 3.46. When two reads of one cell disagree with your
candidate only in the order of the load and the mask, make the cell volatile.
(`sub_080937d0`.)

### 3.161 Per-arm stores keep a base address out of loop.c's movable list
`sub_08092250` classifies a value into `gUnk_02007D00[6]` in six arms and then
runs a `switch` on it, all inside a loop. Written as "compute into a local,
store once after the `if`/`else` chain" the array's base becomes one movable
with `savings 4` and loop.c hoists it out of the loop — the ROM re-loads it in
every arm. Writing the store **in each arm** (3.108) makes each arm's base
pseudo live into the cross-jumped store block, so `reg_in_basic_block_p` fails
and `scan_loop` drops them from the movable list (3.55) — no hoist, four pool
words, exactly like the ROM. The companion observation: mentioning a RAM cell
three times (`gUnk_03001F2C = abs(x); if (gUnk_03001F2C <= 43) … else if
(gUnk_03001F2C <= 87)`) raises *its* address's `savings` to 3 and gets it
hoisted instead, which is what the ROM keeps in `r8`.

### 3.162 `abs()` in a condition distributes into two compares
`(f() < 0 ? -f() : f()) <= 47` compiles to `cmp #0; bge arm2; f(); negs;
cmp #47; ble body; b skip; arm2: f(); cmp #47; bgt skip;` — `do_jump` walks the
`COND_EXPR` and emits one comparison per arm, with jump.c inverting the first
arm's branch pair (3.134). Three calls to `f()` in the ROM around one compare
is therefore an `abs`-style ternary in the condition, not a value in a local;
`global.h`'s `abs()` macro has the arms the other way round (`>= 0 ? n : -n`),
which puts the *negated* arm second — read the ROM's arm order and spell the
ternary to match. M25 has eight of these. (`sub_0809074c`, `sub_08092a14`,
`sub_08091954`, …)

### 4.33 Verify a growing batch file by fnmatching the prefix, and keep a backup
`tools/fnmatch.sh` compiles the WHOLE file and links it at the given start, so
"just this function" runs report the entire file as one giant diff. Two habits
make a 60-function file tractable: (a) after adding functions, re-run fnmatch
with the range end of the **last** function written (`0x0809000C 0x08090298`,
then `…0x080903F0`, …) — every prefix that ends on a `symbols.csv` boundary is
a valid range once you have checked that no literal pool crosses it (4.29;
a five-line script over the split asm's `@ 0x…` pool comments answers that for
the whole module in one go); and (b) work on one function at a time in a
scratch copy that holds the file header plus that function, so an iteration is
3 s instead of 20 s. Splice the result back with a matcher that requires the
`name(args)\n{` form: `file.index("void " + name)` also matches the forward
declaration in the header and silently duplicates half the file — which cost
this issue a rebuild of 26 functions from the transcript.

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
