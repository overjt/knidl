---
name: knidl-decomp
description: "Safely develop the Kirby: Nightmare in Dream Land matching decompilation. Use for any source, asm, map, build, validation, module-selection, commit, or PR task in this repository."
---

# KnIDL matching-decompilation workflow

This is a byte-exact ROM decompilation, not a rewrite. Treat the repository's
current source, `docs/analysis/`, and the user-provided `baserom.gba` as the
ground truth. Never copy ROM bytes, assets, or disassembly into committed C,
documentation, a commit, a PR, or chat output.

## Begin every task

1. Run `python3 tools/pi_harness.py preflight` and inspect its result.
2. Read `AGENTS.md`. Before starting a new decompilation module, also read
   `docs/decomp-loop.md` and `docs/lessons-learned.md` in full. Read the
   relevant module entry in `docs/analysis/module-map.md`.
3. Preserve unrelated working-tree changes. Do not reset, checkout, clean,
   stash, or overwrite them. A dirty tree is a reason to narrow the task or
   stop and report the conflict, not to discard work. The guard blocks those
   commands; `git checkout -b` and `git switch -c` stay available because the
   workflow needs them.
4. Work on a feature branch whose base is `master`. Never develop on, merge
   into, push to, or open a PR against `init`.

Use `python3 tools/pi_harness.py status`, `next`, `plan`, and
`assignment <module>` for deterministic local context. They only inspect
metadata and never expose ROM contents.

**Module identity.** Always name modules by the frozen ids of
`docs/analysis/module-map.md` section 4 (`M27`), which is what every issue and
document uses. The `id` column of `docs/analysis/module-map.csv` is positional
and renumbers itself every time a range lands as C, so it is not an identity;
the harness never reports it. `assignment` also accepts a start address or an
explicit range.

## Development rules

- All compile, assemble, link, match, and ROM-compare operations run through
  Docker: `make ...` host wrappers or explicit `docker run ... knidl-builder`.
  Do not install or use a native GBA compiler.
- `baserom.gba` is legal user-local input. It is gitignored and must remain
  unreadable as source material: do not commit it, stage it, upload it, print
  it, extract its assets, or add it to a patch.
- Work in one isolated function/range at a time unless the user explicitly
  authorizes a verified module batch. Put disposable experiments under
  `build/scratch/fn_<name>/`; `make clean` deletes that directory.
- Ranges are half-open and must start/end at validated function boundaries.
  Include each function's trailing literal pool before deciding it matches.
- Do not edit generated split asm by hand. A verified C range is landed only
  through `tools/carve.py`, followed by split regeneration.
- The authoritative compiler recipe is the current `Makefile`. For game code
  currently this includes `-fprologue-bugfix`; pass `--newpb` to `fnmatch.sh`
  when its output must model that recipe. SDK and m4a ranges use their explicit
  per-file recipes.

## Matching loop

Follow `docs/decomp-loop.md` exactly:

1. Pick an unresolved function from `symbols.csv`; use `assignment` to find
   its range and asm chunk.
2. Create a self-contained first-pass C candidate. Use m2c only as a first
   pass, then express the result using project headers and symbols.
3. Iterate with `tools/fnmatch.sh` using the exact range and recipe. A
   `MATCH` is required before carving. Investigate the first instruction
   divergence, not downstream register noise.
4. Use decomp-permuter only for a stable, small residual mismatch; retain its
   target under the function scratch directory and verify its candidate with
   `fnmatch.sh` again.
5. Dry-run `tools/carve.py`; only then use `--write`, add the verified source,
   and regenerate the split.

## Landing and delivery gate

Before committing any landed range, run all applicable checks from scratch:

```sh
make symbols && make split
make clean && make compare
make progress
```

Also inspect the linker map to ensure the C object owns the exact intended
range. Update `docs/analysis/rom-map.md`, and add a lesson only if it is new.
Run `python3 tools/pi_harness.py verify --full` when a complete baserom is
available; otherwise explain that byte verification could not run rather than
claiming a match.

Use concise English in code, docs, commits, and PR text. Never make a claim of
matching based solely on compilation. A deliverable branch must be based on
`master`, have a focused commit, and—when delivery is requested—an open PR
against `master` with CI green. Do not merge the PR or close its issue.

### Committing

The merge gate runs `make symbols`, `make split`, `make modmap`,
`make check-headers`, the report check, and `make clean && make compare` on
**every** commit, so a commit takes minutes and `make clean` deletes
`build/scratch/`. Commit at batch boundaries, not after every edit, and do not
keep anything you cannot regenerate under `build/`.

`tools/pi-knidl.sh` installs the repository-local pre-commit merge gate. It
requires a clean staged snapshot, valid local ROM, generated-map consistency,
header compilation, report coverage, and `make clean && make compare`. Never
bypass it with `--no-verify` or a replacement hooks path. This proves local
byte-exactness; GitHub's required **Build and verify** CI and owner review are
still the final merge authority.

## Useful commands

```sh
./tools/pi-knidl.sh                  # launch Pi with this project harness
./tools/pi-knidl.sh start M27        # launch aimed at one module
./tools/pi-knidl.sh autopilot        # unattended run of the whole queue
python3 tools/pi_harness.py status
python3 tools/pi_harness.py next     # next module in the recommended order
python3 tools/pi_harness.py plan     # the whole remaining queue
python3 tools/pi_harness.py assignment M27
python3 tools/pi_harness.py verify --full
python3 tools/pi_harness.py quota    # plan usage and time to reset
python3 tools/pi_harness.py publish  # push the branch; one PR tracks it
python3 tools/pi_harness.py selftest # metadata contract check (no ROM needed)
```

If a run stops making progress, check `quota` before concluding anything about
the code: a spent token window looks exactly like a stuck problem.

## Autopilot

`./tools/pi-knidl.sh autopilot` drives the queue without a human. After every
time you go quiet it re-reads `assignment --json` and decides from the
**measured** `uncovered_bytes` — announcing a module finished does not finish
it, and going quiet does not stall the queue. A module with no measurable
progress after `KNIDL_MAX_ATTEMPTS` settles (default 3) is parked and the loop
moves on; `python3 tools/pi_harness.py unpark M27` puts it back.

Under autopilot, prefer breadth over depth: land the functions that match and
leave the ones that do not, with a note. A parked function costs the queue
nothing; an infinite loop on one function costs it everything.
