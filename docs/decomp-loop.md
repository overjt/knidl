# The standard decompilation loop — one function from asm to matching C

This is the repeatable workflow for turning one ROM function from labeled asm
(issue #25 split) into matching C that links back into its pinned section.
It is written for AI agents executing decompilation issues: every step is a
command that runs inside the `knidl-builder` Docker image, and §8 defines the
subagent handoff contract.

Read [`docs/lessons-learned.md`](lessons-learned.md) before your first module;
the steps below reference those lessons by number. Tool references:
[`docs/diffing.md`](diffing.md), [`docs/splitting.md`](splitting.md).

> [!IMPORTANT]
> Everything runs in Docker (`make image` once). Host-side wrappers pass
> `INSIDE_DOCKER=1` automatically (lesson 2.5); raw `docker run` invocations in
> this doc already include it where needed. Scratch work lives under `build/`
> (gitignored): `/tmp` inside a container dies with it (lesson 2.4).

---

## 0. Inputs you need before starting

| Input | Where |
|-------|-------|
| Function address + size | `docs/analysis/symbols.csv` (e.g. `0x080CFA9C,0x24,thumb,rom-pointer,ReadSram_Core`) |
| Its current asm | `asm/<segment>/<segment>_NN.s` chunk files (find the chunk via `docs/analysis/segments.txt` boundaries); functions are labeled, literal pools included |
| Symbol names it calls/references | `docs/analysis/callgraph.csv`, `asm/rom_syms.s` absolute symbols |
| Compiler recipe for its zone | `Makefile` per-file overrides + `docs/research/compiler-validation.md`: game code = `agbcc -O2 -mthumb-interwork`; SDK zone (`0x080CF9xx`) = `old_agbcc -O1 -mthumb-interwork`; ARM units = `agbcc_arm` |

## 1. Pick the function and set up scratch space

Choose ONE function (smallest first; leaf functions before callers). Cut its
range at symbol boundaries only — never retype addresses by hand (lesson 4.13;
ends in `segments.txt` are exclusive).

```sh
mkdir -p build/scratch/fn_<name>
```

Extract the target disassembly for reference (read-only; do not edit generated
asm, lesson 4.11):

```sh
./asmdiff.sh 0x0<ADDR> 0x<SIZE>        # shows baserom vs current build (identical pre-work)
```

## 2. First-pass C with m2c

Feed the function's asm (objdump text of exactly its range) to m2c:

```sh
pip install git+https://github.com/matt-kempster/m2c   # NOT PyPI 'm2c' (lesson 2.1)
m2c --target gba-gcc-c < fn.asm > base.c
```

Known m2c limits (lesson 2.1): Thumb literal pools mid-file confuse branch
resolution; for small SDK-shaped functions, hand-port from a sibling project
(katam) instead — often faster than fixing m2c input. Make the C self-contained
for now (inline `typedef u8/u16/u32;` or use repo headers later when linking).

## 3. Iterate with asmdiff.sh on the function's range only

Wire the file into the build first so there is something to diff:

1. Put the C in `src/<name>.c`.
2. Add a per-file override AFTER `BUILD_DIR := build` in the `Makefile`
   (lesson 1.1) with the zone's recipe, e.g.:

   ```make
   $(BUILD_DIR)/src/<name>.o: CC := old_agbcc
   $(BUILD_DIR)/src/<name>.o: CFLAGS := -O1 -mthumb-interwork
   ```

   For a new pinned section, also add a section to `linker.ld` and remove the
   covering `.incbin`/chunk slice — see step 5; during iteration keep the asm
   in place and just diff.
3. Rebuild and diff ONLY the function range:

   ```sh
   rm -f build/src/<name>.o          # flag changes don't trigger rebuilds (lesson 1.2)
   make && ./asmdiff.sh 0x0<ADDR> 0x<SIZE>
   ```

Empty diff = done with this stage. While iterating,
`python3 tools/asm-differ/diff.py -mwo <symbol>` gives colored instruction
diffs (see `docs/diffing.md` §2).

Source-shape fixes live in `docs/lessons-learned.md` §3 (opt level per zone,
CSE defeat, mask widths, stack-copy idioms). Try the other opt level BEFORE
rewriting source (lesson 3.1).

## 4. Escalate to decomp-permuter (close-but-not-equal)

When the diff is small and stable but nonzero, let
[`tools/decomp-permuter`](tools/decomp-permuter) (vendored from
simonlindholm/decomp-permuter@`2795247304ec4798459b9bc865314e64e5182bf9`,
MIT LICENSE kept intact) search semantically-equivalent source shapes for you.
It needs no baserom — only the compiler pipeline and target asm.

### 4.1 Directory layout (one dir per function)

A working example lives in [`tools/permuter-example/`](../tools/permuter-example);
copy it and adapt. Required files:

```
build/scratch/fn_<name>/
├── base.c        # your current best C (self-contained: typedefs or -I include)
├── target.s      # EXACT ROM bytes of the function as gas (.syntax unified,
│                 #   .thumb_func, pools as .word/.short data, trailing pad
│                 #   halfwords emitted explicitly — gas pads with NOPs, not zeros)
├── target.o      # arm-none-eabi-as -mcpu=arm7tdmi -o target.o target.s
├── compile.sh    # zone pipeline, contract: ./compile.sh <in.c> -o <out.o>
└── settings.toml # func_name, compiler_type, weight_overrides
```

Verify `target.o` is really the ROM truth before trusting any score:

```sh
arm-none-eabi-objcopy -O binary --only-section=.text target.o /tmp/t.bin
dd if=baserom.gba bs=1 skip=$((ADDR & 0x1FFFFFFF)) count=$((SIZE)) of=/tmp/r.bin status=none
cmp /tmp/t.bin /tmp/r.bin     # must be silent (lesson 2.2: never trust empty-tool checks)
```

### 4.2 compile.sh per zone

Game-code zone (default recipe; drop `-Werror` — permutations legitimately
warn, and warnings must not abort candidates):

```bash
#!/bin/bash
set -euo pipefail                     # pipefail is MANDATORY: without it a crashed
                                      # compiler + succeeding assembler scores 0
INPUT="$1"; OUTPUT="$3"
cpp -P -I include "$INPUT" \
  | agbcc -O2 -mthumb-interwork -fhex-asm \
  | { cat; printf '.text\n\t.align\t2, 0\n'; } \
  | arm-none-eabi-as -mcpu=arm7tdmi -o "$OUTPUT"
```

SDK zone: same with `old_agbcc -O1 -mthumb-interwork` (see
`tools/permuter-example/compile.sh`). `-I include` lets base.c use repo headers.

### 4.3 settings.toml

```toml
func_name = "<Name>"
compiler_type = "gcc"

# old_agbcc rejects many modern-GCC randomizer passes; disabling them keeps
# candidate success high (~95%+ parallel). Tune further if errors dominate.
[weight_overrides]
perm_inline = 0
perm_var_cond_block = 0
perm_randomize_internal_type = 0
perm_randomize_external_type = 0
perm_randomize_function_type = 0
perm_expand_expr = 0
perm_long_chain_assignment = 0
```

Do **not** set `objdump_command`: the scorer auto-detects ARM objects and adds
its own objdump flags.

### 4.4 Run

```sh
# Sanity: score the CURRENT base.c without permuting (fast, deterministic)
docker run --rm -v "$(pwd):/src" -w /src knidl-builder bash -c '
  cd build/scratch/fn_<name> && python3 /src/tools/decomp-permuter/permuter.py . --debug'
# -> "[<Name>] base score = N"  (0 means already matching)

# Permute (-j workers; improvements land in output-<score>-<n>/source.c)
docker run --rm -v "$(pwd):/src" -w /src knidl-builder bash -c '
  cd build/scratch/fn_<name> &&
  timeout 1800 python3 /src/tools/decomp-permuter/permuter.py . -j 4 --seed 1'
```

Interpretation: score decreases toward 0; a candidate reaching 0 IS the missing
source shape — port it back into `src/`, then re-verify through the real build.
Expect some `errors, score = inf` lines: rejected candidates are normal; a rate
above ~25% means your weights/base.c fight old_agbcc (check `--show-errors`).
The permuter explores *equivalent-form* mutations around your code; if the base
shape is structurally wrong, fix structure by hand (steps 2–3) first.

Useful flags: `--stop-on-zero` (halt on match), `--better-only`,
`--print-diffs` (show what changed per improvement), `--debug` (dumps compiled
base object). Full CLI: `tools/decomp-permuter/USAGE.md`.

## 5. Land it: link the C object in its pinned section

1. `linker.ld`: pin the new section at the function's VMA
   (`.name ADDR : { build/src/<name>.o(.text) })` style, mirroring existing
   entries) and delete the replaced asm slice/chunk coverage for that range.
2. Remove the function's body from the `asm/` chunk (or regenerate split
   outputs if config-driven — never hand-edit generated asm, lesson 4.11).
3. Full clean verification — incremental builds hide stale objects (lesson 4.13):

   ```sh
   make clean && make compare          # SHA-1 must pass
   grep -A2 '<section name>' build/knidl.map   # section from src/<name>.o at right VMA,
                                               # no leftover duplicate (lesson 1.3)
   ```

4. `make progress` to record the byte shift.

## 6. Update the maps

- `docs/analysis/rom-map.md`: mark the function/range as decompiled.
- `docs/analysis/segments.txt`: only if a whole segment converted (keep ends
  exclusive, lesson 4.13).
- `docs/lessons-learned.md`: add anything NEW you learned (source shapes → §3,
  tooling → §2). No duplicates of existing lessons.
- If symbols changed: rerun `make split`/`make symbols` flow so CI's
  regeneration checks stay green.

## 7. Definition of done (per function)

- [ ] `make clean && make compare` passes from scratch.
- [ ] `build/knidl.map` shows the C object owning exactly the assigned range.
- [ ] No edits outside the assigned range (see §8 rule below).
- [ ] rom-map/lessons updated; new gotchas recorded.

## 8. Subagent handoff contract

Decompilation issues are executed by AI agents, possibly several in parallel.
To keep parallel agents from colliding, every delegation MUST use this contract.

### Coordinator → subagent inputs (all mandatory)

1. **Assignment**: exactly ONE function — name, `[start, end)` ROM range
   (half-open, from `symbols.csv`/`segments.txt`) and its containing segment.
2. **Current asm**: path(s) to the chunk file(s) holding the function plus the
   function's label inside them.
3. **Symbol context**: known names the function calls/references (from
   `callgraph.csv` / `rom_syms.s`) and whether each is C-defined yet.
4. **Compiler recipe**: the zone verdict (`agbcc -O2` vs `old_agbcc -O1` /
   `agbcc_arm`) and the matching `compile.sh` template from §4.2.
5. **Scratch root**: `build/scratch/fn_<name>/` — pre-created by the
   coordinator so concurrent agents never share directories.

### Subagent → coordinator outputs (all mandatory)

1. `src/<name>.c` compiling to a byte-exact match of the range under the given
   recipe (proof: `asmdiff.sh` empty on the range + permuter `--debug` score 0
   or final full-build diff).
2. Header additions (declarations) if the function is referenced elsewhere.
3. Exact deltas needed to land: `Makefile` override block, `linker.ld` section
   entry, list of asm lines/files to delete. As unified diffs against the
   current tree — the COORDINATOR applies them.
4. A short report: lessons learned (for §6), open questions, any mismatching
   bytes that could indicate wrong assumptions about the assignment itself.

### Hard rules

- A subagent NEVER modifies ranges outside its assignment: no other `src/`
  files, no shared headers except pure additions, no `linker.ld`/`Makefile`
  edits beyond reporting proposed deltas, no `asm/` deletions (report instead).
- A subagent never commits. It returns diffs + evidence; the coordinator
  integrates serially and runs the full `make clean && make compare`.
- If the assignment looks wrong (size mismatch, overlapping pool, mislabeled
  boundary), STOP and report back — do not silently widen the range
  (lesson 4.13 exists because someone did).
