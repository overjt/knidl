# The Pi agent harness

A project-local harness that lets the [Pi coding agent](https://github.com/earendil-works/pi)
work this repository under the rules in [`AGENTS.md`](../AGENTS.md), either
interactively or unattended.

It is three things: a **deterministic oracle** that answers "what is left and
what is next" from repository metadata, a **guard** that refuses actions the
contract forbids, and an **autopilot** that drives the module queue without a
human.

```sh
./tools/pi-knidl.sh                  # interactive session
./tools/pi-knidl.sh start M27        # interactive, aimed at one module
./tools/pi-knidl.sh autopilot        # unattended run of the whole queue
```

## Layout

| Path | Role |
|------|------|
| `tools/pi_harness.py` | Oracle: queue, assignments, bookkeeping, verification, merge gate |
| `tools/knidl-rules.mjs` | Guard policy as pure functions, so CI can test it |
| `tools/knidl-rules.test.mjs` | Assertions for those rules (`node tools/knidl-rules.test.mjs`) |
| `tools/pi-knidl.sh` | Launcher: hooks, session store, autopilot switches |
| `.githooks/pre-commit` | Runs the merge gate |
| `.pi/extensions/knidl-guard.ts` | Wires the rules into Pi, registers `/knidl-*` |
| `.pi/extensions/knidl-autopilot.ts` | The unattended loop |
| `.pi/skills/knidl-decomp/SKILL.md` | The workflow the agent follows |
| `.pi/prompts/*.md` | Prompt templates for common tasks |
| `.pi/sessions/`, `.pi/state/` | Local transcripts and bookkeeping (gitignored) |

## Module identity

`docs/analysis/module-map.csv` numbers its rows **positionally**, so those ids
shift every time a range lands as C — after M25, M36 and M27 landed, the CSV
row called `M27` was `0x0806D22C`, while issue #68's M27 is `0x080988F8`.

The harness therefore speaks only the frozen numbering of
[`module-map.md`](analysis/module-map.md) section 4, which is hand-maintained
and is what every issue and document uses, and it always prints the address
range beside the id. `assignment` also accepts a start address or a range.
`pi_harness.py selftest` asserts this, with M27 as the named regression case.

Whether a module is *done* is never taken from the map's prose: it is computed
from `docs/analysis/segments.txt`, the same ownership record the linker uses.

## Commands

```sh
python3 tools/pi_harness.py status      # repo, ROM, Docker, hook, queue size
python3 tools/pi_harness.py preflight   # prerequisites, with warnings
python3 tools/pi_harness.py next        # next module in the recommended order
python3 tools/pi_harness.py plan        # the whole remaining queue
python3 tools/pi_harness.py assignment M27
python3 tools/pi_harness.py verify --full
python3 tools/pi_harness.py quota       # plan usage and time to reset
python3 tools/pi_harness.py publish     # push the branch, ensure one PR tracks it
python3 tools/pi_harness.py selftest    # metadata contract; no ROM, no Docker
python3 tools/pi_harness.py unpark M27  # requeue a parked module
```

`status`, `next`, `plan` and `assignment` take `--json` for machine use; that
is how the autopilot reads state.

## The autopilot loop

The loop is deliberately **not** model-driven:

1. The agent is briefed on one module and works it through the normal
   `fnmatch` → `carve` → `compare` → commit loop.
2. When the agent goes quiet (`agent_settled`), the autopilot re-runs
   `assignment --json` and reads `uncovered_bytes`.
3. `uncovered_bytes == 0` → the module is recorded as landed and the next one
   begins, after a compaction that keeps the lessons and drops the finished
   module's per-function detail.
4. Otherwise the agent is nudged with the measured delta. Real progress resets
   the attempt budget; `KNIDL_MAX_ATTEMPTS` settles without measurable progress
   park the module and the queue moves on.

So a model that claims a module is finished cannot advance the queue, and a
model that quietly gives up cannot stall it. Parking is the point: with a
smaller model most modules will land and some will not, and the run should
still make progress overnight.

### Quota

The z.ai coding plan meters a rolling 5-hour token window and a weekly one.
`quota` reads the plan's monitoring endpoint, which costs no tokens:

```
z.ai coding plan (lite)
  5-hour  used 30%   resets 2026-08-31 20:59:50 (in 51 min)
```

The autopilot asks before sending every prompt. When a window is exhausted it
sleeps until the reset and then sends the same prompt — **without consuming an
attempt**, because hitting a quota wall looks exactly like a model that stopped
cooperating, and without this the loop would park perfectly good modules. A
monitoring outage never stops the queue: an unreadable quota is treated as
"carry on".

The credential is taken from `ZAI_GLM_API_KEY`, `ZAI_API_KEY`,
`ZAI_CODING_API_KEY` or `GLM_API_KEY`, and otherwise from
`pi auth print-api-key`.

### Delivery

After every landed module the autopilot runs `publish`: it pushes the branch
and, the first time, opens one PR against `master`. Later pushes update that
same PR, so an unattended run needs no human step between modules. The branch
accumulates deliberately — a chain of per-module branches would conflict in
`segments.txt`, `linker.ld`, `split_config.json` and `symbols.csv`.

Publishing never merges: CI and the owner remain the merge authority.

| Variable | Meaning |
|----------|---------|
| `KNIDL_MAX_MODULES` | Stop after N modules land (0 = until the queue empties) |
| `KNIDL_MAX_ATTEMPTS` | Settles without progress before parking (default 3) |
| `KNIDL_PROVIDER` / `KNIDL_MODEL` | Pass `--provider` / `--model` to Pi |
| `KNIDL_THINKING` | Reasoning level (default `high`) |

The autopilot extension stays dormant unless `KNIDL_AUTOPILOT=1`, which only
`./tools/pi-knidl.sh autopilot` sets, so interactive sessions are never driven.

## The merge gate

`tools/pi-knidl.sh` points `core.hooksPath` at `.githooks`, and
`.githooks/pre-commit` runs `pi_harness.py merge-gate`. The gate refuses a
commit unless the branch is a feature branch based on `master`, the tree has no
unstaged or untracked leftovers, nothing forbidden is staged, and
`make symbols`, `make split`, `make modmap`, `make check-headers`, the report
check and `make clean && make compare` all pass on the staged snapshot.

Two consequences worth knowing before you use it:

- a commit takes minutes, so commit at **batch boundaries**, not per edit;
- the gate runs `make clean`, which deletes `build/scratch/`, so nothing
  irreplaceable may live there.

`core.hooksPath` is set to the **relative** value `.githooks`, which Git
resolves against each working tree's own root. An absolute path here is shared
through `.git/config` with every other worktree of the repository, which made
unrelated worktrees fail every commit with `can't open file
tools/pi_harness.py`. The hook also exits 0 when the harness is absent, so a
worktree on another branch is never blocked.

The gate proves local byte-exactness. It is not the merge authority: the
required **Build and verify** CI run and the owner's review still are.

## Trust and isolation

Pi loads `.pi/` resources only in a trusted project, and the launcher passes
`--approve` to trust this checkout for the run. Read the extensions before you
trust them; they run with your permissions.

Pi has no sandbox, and its own documentation recommends a container for
unattended work. This harness is written for a **dedicated worktree on the
host**, with the guard as the only barrier, because the build itself needs the
`knidl-builder` Docker image. If you move it into a container, mount the
workspace and the ROM, and expect to pass the Docker socket through.

## Testing the harness

Three checks run in CI on every push and need neither the ROM nor Docker:

```sh
python3 tools/pi_harness.py selftest    # metadata contract
node tools/knidl-rules.test.mjs         # guard policy
node --input-type=module -e "import('./.pi/extensions/knidl-guard.ts')"
```
