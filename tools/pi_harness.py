#!/usr/bin/env python3
"""Deterministic project oracle and verification entry points for the Pi harness.

Everything the autonomous loop needs to know about "what is left, what is next,
and is it really done" is answered here rather than by the model, so the same
question always gets the same answer.

This tool consumes only repository metadata and build commands. It never reads,
prints, extracts, or copies the copyrighted baserom payload.

Module identity
---------------
`docs/analysis/module-map.csv` numbers its rows positionally, so those ids shift
every time a range lands as C. Every issue and every document instead uses the
frozen numbering of `docs/analysis/module-map.md` section 4. This tool speaks
only the frozen ids and always prints the address range beside them.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
STATE_PATH = ROOT / ".pi/state/autopilot.json"
EXPECTED_ROM_SHA1 = "37a476567d133c146fee6b5e2eb0b07a215da6b0"
ASM_FOREVER = {
    "crt0_master_isr",
    "task_switch_helpers",
    "sdk_swi_wrappers",
    "sdk_reset_helper",
    "sdk_libc",
    "lib_misc",
    "m4a_1",
}
MODULE_MAP = "docs/analysis/module-map.md"

# Section 4: | M27 | `0x080988F8-0x0809BA43` | 12.3 KiB | 140 | 3 | * | name |
MODULE_ROW = re.compile(
    r"^\|\s*(M\d\d)\s*\|\s*`(0x[0-9A-Fa-f]{8})-(0x[0-9A-Fa-f]{8})`\s*\|"
    r"([^|]*)\|([^|]*)\|([^|]*)\|([^|]*)\|([^|]*)\|",
    re.M,
)
# Section 5: | 5 | #68 | M27 name | `0x080988F8-0x0809BA43` | 12.3 KiB | 2 |
QUEUE_ROW = re.compile(
    r"^\|\s*(\d+)\s*\|\s*#(\d+)\s*\|\s*(M\d\d)\s+([^|]*?)\s*\|\s*"
    r"`(0x[0-9A-Fa-f]{8})-(0x[0-9A-Fa-f]{8})`\s*\|([^|]*)\|\s*(\d+)\s*\|",
    re.M,
)
BATCHES_ROW = re.compile(r"^\*\s+\*\*Suggested batches\*\*\s+(.+)$", re.M)


def run(command: list[str], timeout: int = 30) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, text=True, capture_output=True, timeout=timeout, check=False)


def git(*args: str) -> str:
    return run(["git", *args]).stdout.strip()


def address(value: str) -> int:
    return int(value.strip(), 0)


# ---------------------------------------------------------------- segments


@dataclass(frozen=True)
class Interval:
    start: int
    end: int
    kind: str
    name: str


def segments() -> list[Interval]:
    result: list[Interval] = []
    for line in (ROOT / "docs/analysis/segments.txt").read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        fields = line.split()
        if len(fields) < 4:
            continue
        result.append(Interval(address(fields[0]), address(fields[1]), fields[2], fields[3]))
    return result


def c_intervals() -> list[Interval]:
    return [item for item in segments() if item.kind == "c_code"]


def covered(start: int, end: int) -> bool:
    return any(item.start <= start and end <= item.end for item in c_intervals())


def _spans(intervals: list[Interval], start: int, end: int) -> bool:
    cursor = start
    for item in sorted(intervals, key=lambda interval: interval.start):
        if item.end <= cursor:
            continue
        if item.start > cursor:
            return False
        cursor = max(cursor, item.end)
        if cursor >= end:
            return True
    return cursor >= end


def range_covered(start: int, end: int) -> bool:
    """Whether adjacent c_code intervals cover a whole module range."""
    return _spans(c_intervals(), start, end)


def range_owned_by_asm_forever(start: int, end: int) -> bool:
    """Whether every byte is in a deliberately permanent assembly segment."""
    return _spans([item for item in segments() if item.name in ASM_FOREVER], start, end)


def uncovered_bytes(start: int, end: int) -> int:
    """Bytes of the range that no c_code segment owns yet."""
    total = end - start
    for item in sorted(c_intervals(), key=lambda interval: interval.start):
        lo, hi = max(start, item.start), min(end, item.end)
        if hi > lo:
            total -= hi - lo
    return max(total, 0)


# ---------------------------------------------------------------- modules


@dataclass(frozen=True)
class Module:
    id: str
    start: int
    end: int
    name: str
    order: int | None = None
    issue: int | None = None

    @property
    def size(self) -> int:
        return self.end - self.start

    @property
    def range_text(self) -> str:
        return f"{self.start:#010x}-{self.end:#010x}"


def _map_text() -> str:
    return (ROOT / MODULE_MAP).read_text(encoding="utf-8")


def doc_modules() -> dict[str, Module]:
    """The frozen module numbering every issue and document refers to."""
    text = _map_text()
    modules: dict[str, Module] = {}
    for match in MODULE_ROW.finditer(text):
        identifier, start, end = match.group(1), match.group(2), match.group(3)
        name = match.group(8).strip().replace("**", "")
        # `end` in the table is inclusive; every other tool uses half-open.
        modules[identifier] = Module(identifier, address(start), address(end) + 1, name)
    return modules


def work_queue() -> list[Module]:
    """Modules in the repository's own recommended order, with their issues."""
    modules = doc_modules()
    queue: list[Module] = []
    for match in QUEUE_ROW.finditer(_map_text()):
        order, issue, identifier = int(match.group(1)), int(match.group(2)), match.group(3)
        base = modules.get(identifier)
        if base is None:
            continue
        queue.append(
            Module(base.id, base.start, base.end, base.name, order=order, issue=issue)
        )
    queue.sort(key=lambda module: module.order or 0)
    return queue


def resolve(identifier: str) -> Module | None:
    """Accept a frozen module id, a start address, or an explicit range."""
    token = identifier.strip()
    queued = {module.id: module for module in work_queue()}
    modules = doc_modules()
    upper = token.upper()
    if upper in queued:
        return queued[upper]
    if upper in modules:
        return modules[upper]
    if "-" in token:
        lo, hi = token.split("-", 1)
        try:
            start, end = address(lo), address(hi)
        except ValueError:
            return None
        for module in modules.values():
            if module.start == start:
                return module
        return Module("(range)", start, end, "explicit range")
    try:
        start = address(token)
    except ValueError:
        return None
    for module in modules.values():
        if module.start <= start < module.end:
            return module
    return None


def suggested_batches(module: Module) -> str:
    """The batch split section 6 recommends for this module, if it records one."""
    text = _map_text()
    heading = re.search(
        rf"^### {re.escape(module.id)} `0x[0-9A-Fa-f]{{8}}-0x[0-9A-Fa-f]{{8}}`.*$",
        text,
        re.M,
    )
    if heading is None:
        return ""
    body = text[heading.end() :]
    nxt = re.search(r"^### ", body, re.M)
    if nxt:
        body = body[: nxt.start()]
    found = BATCHES_ROW.search(body)
    return found.group(1).strip() if found else ""


def csv_metrics() -> dict[int, dict[str, str]]:
    """Census metrics keyed by start address, so positional ids never matter."""
    path = ROOT / "docs/analysis/module-map.csv"
    if not path.is_file():
        return {}
    with path.open(encoding="utf-8", newline="") as stream:
        return {address(row["start"]): row for row in csv.DictReader(stream)}


# ---------------------------------------------------------------- state


def read_state() -> dict:
    if not STATE_PATH.is_file():
        return {"parked": {}, "attempts": {}, "history": []}
    try:
        data = json.loads(STATE_PATH.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return {"parked": {}, "attempts": {}, "history": []}
    data.setdefault("parked", {})
    data.setdefault("attempts", {})
    data.setdefault("history", [])
    return data


def write_state(state: dict) -> None:
    STATE_PATH.parent.mkdir(parents=True, exist_ok=True)
    STATE_PATH.write_text(json.dumps(state, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def pending(state: dict | None = None, include_parked: bool = False) -> list[Module]:
    """Queued modules that still have assembly to convert."""
    state = state if state is not None else read_state()
    parked = set(state["parked"])
    result: list[Module] = []
    for module in work_queue():
        if range_covered(module.start, module.end):
            continue
        if range_owned_by_asm_forever(module.start, module.end):
            continue
        if not include_parked and module.id in parked:
            continue
        result.append(module)
    return result


def module_payload(module: Module, state: dict) -> dict:
    metrics = csv_metrics().get(module.start, {})
    return {
        "id": module.id,
        "issue": module.issue,
        "order": module.order,
        "start": f"{module.start:#010x}",
        "end": f"{module.end:#010x}",
        "size": module.size,
        "uncovered_bytes": uncovered_bytes(module.start, module.end),
        "name": module.name,
        "recipe": compiler_recipe(module.start),
        "batches": suggested_batches(module),
        "difficulty": metrics.get("difficulty"),
        "ext_deps": metrics.get("ext_deps"),
        "attempts": state["attempts"].get(module.id, 0),
        "parked": state["parked"].get(module.id),
    }


# ---------------------------------------------------------------- environment


def rom_status() -> str:
    rom = ROOT / "baserom.gba"
    if not rom.is_file():
        return "absent (byte-exact matching cannot run)"
    digest = hashlib.sha1()
    with rom.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    matches = digest.hexdigest() == EXPECTED_ROM_SHA1
    return "present; SHA-1 " + ("matches expected" if matches else "DOES NOT MATCH expected")


def docker_status() -> str:
    if shutil.which("docker") is None:
        return "unavailable"
    version = run(["docker", "image", "inspect", "knidl-builder", "--format", "{{.Id}}"])
    return "available; knidl-builder " + ("ready" if version.returncode == 0 else "not built (run: make image)")


HOOKS_RELATIVE = ".githooks"


def hook_status() -> str:
    """Report the merge gate hook.

    The launcher configures core.hooksPath RELATIVE, because Git resolves a
    relative value against each working tree's own root while an absolute one
    is shared through .git/config with every other worktree of the repository.
    An absolute value still works here, so it counts as active, but it is worth
    flagging as the form that breaks siblings.
    """
    configured = git("config", "--get", "core.hooksPath")
    if configured == HOOKS_RELATIVE:
        return "active"
    if configured == str(ROOT / HOOKS_RELATIVE):
        return "active (absolute path; rerun ./tools/pi-knidl.sh to make it worktree-safe)"
    if configured:
        return f"pointing elsewhere ({configured}); run ./tools/pi-knidl.sh to repair"
    return "inactive (launch Pi through ./tools/pi-knidl.sh)"


def ignored(path: str) -> bool:
    return run(["git", "check-ignore", "-q", path]).returncode == 0


# ---------------------------------------------------------------- reporting


def emit_status(as_json: bool) -> int:
    state = read_state()
    todo = pending(state)
    parked = pending(state, include_parked=True)
    code_left = sum(uncovered_bytes(module.start, module.end) for module in parked)
    if as_json:
        print(
            json.dumps(
                {
                    "root": str(ROOT),
                    "branch": git("branch", "--show-current") or None,
                    "dirty_paths": len(git("status", "--short").splitlines()),
                    "baserom": rom_status(),
                    "docker": docker_status(),
                    "hook": hook_status(),
                    "c_ranges": len(c_intervals()),
                    "c_bytes": sum(item.end - item.start for item in c_intervals()),
                    "pending_modules": len(todo),
                    "parked_modules": sorted(state["parked"]),
                    "asm_bytes_in_queue": code_left,
                },
                indent=2,
            )
        )
        return 0
    dirty = git("status", "--short")
    print("KnIDL Pi harness status")
    print(f"root: {ROOT}")
    print(f"branch: {git('branch', '--show-current') or 'DETACHED'}")
    print("worktree: " + ("clean" if not dirty else f"dirty ({len(dirty.splitlines())} paths)"))
    print(f"baserom: {rom_status()}")
    print(f"docker: {docker_status()}")
    print(f"merge gate hook: {hook_status()}")
    c_code = c_intervals()
    print(f"matching C: {len(c_code)} ranges, {sum(i.end - i.start for i in c_code):#x} bytes")
    print(f"queue: {len(todo)} module(s) pending, {code_left:#x} bytes of assembly left")
    if state["parked"]:
        print("parked: " + ", ".join(f"{k} ({v.get('reason', '?')})" for k, v in sorted(state["parked"].items())))
    return 0


def emit_preflight() -> int:
    required = [
        "AGENTS.md",
        "Makefile",
        "docs/decomp-loop.md",
        "docs/lessons-learned.md",
        MODULE_MAP,
        "tools/fnmatch.sh",
        "tools/carve.py",
    ]
    missing = [name for name in required if not (ROOT / name).exists()]
    branch = git("branch", "--show-current")
    print("KnIDL Pi harness preflight")
    print("repository instructions: " + ("ready" if not missing else "MISSING " + ", ".join(missing)))
    print("branch: " + (branch or "DETACHED"))
    if branch in {"master", "init"}:
        print("WARNING: create and use a feature branch before changing source.")
    print("baserom: " + rom_status())
    print("docker: " + docker_status())
    print("merge gate hook: " + hook_status())
    if hook_status() != "active":
        print("WARNING: start Pi with ./tools/pi-knidl.sh before allowing it to commit.")
    if not ignored(".pi/state/autopilot.json"):
        print("WARNING: .pi/state is not gitignored; the merge gate will refuse every commit.")
    print("mandatory reading before a new module: AGENTS.md, docs/decomp-loop.md, docs/lessons-learned.md")
    return 1 if missing else 0


def emit_next(as_json: bool) -> int:
    state = read_state()
    todo = pending(state)
    if as_json:
        print(json.dumps(module_payload(todo[0], state) if todo else None, indent=2))
        return 0
    if not todo:
        print("Queue empty: every module in the recommended order is landed or parked.")
        return 0
    head = todo[0]
    print("Next module in the repository's recommended order:")
    print(f"  {head.id} [{head.range_text}) issue #{head.issue}  {head.name}")
    print(f"  {uncovered_bytes(head.start, head.end):#x} bytes of assembly left")
    print(f"  recipe: {compiler_recipe(head.start)}")
    print()
    print("Remaining queue:")
    for module in todo[1:11]:
        print(
            f"  {module.id} [{module.range_text}) issue #{module.issue}  "
            f"{uncovered_bytes(module.start, module.end):#x} bytes  {module.name}"
        )
    if len(todo) > 11:
        print(f"  … and {len(todo) - 11} more")
    return 0


def emit_plan(as_json: bool) -> int:
    state = read_state()
    todo = pending(state)
    if as_json:
        print(json.dumps([module_payload(module, state) for module in todo], indent=2))
        return 0
    for module in todo:
        print(
            f"{module.id}\t{module.range_text}\t#{module.issue}\t"
            f"{uncovered_bytes(module.start, module.end):#x}\t{module.name}"
        )
    return 0


# ---------------------------------------------------------------- assignment


def functions_in_range(start: int, end: int) -> list[dict[str, str]]:
    with (ROOT / "docs/analysis/symbols.csv").open(encoding="utf-8", newline="") as stream:
        rows = list(csv.DictReader(stream))
    candidates = [
        row
        for row in rows
        if start <= address(row["vma"]) < end
        and not covered(address(row["vma"]), address(row["vma"]) + address(row["size"]))
    ]
    return sorted(candidates, key=lambda row: (address(row["size"]), address(row["vma"])))


def compiler_recipe(start: int) -> str:
    if 0x080CFA9C <= start < 0x080CFC30:
        return "old_agbcc -O1 -mthumb-interwork; fnmatch flag: --old"
    if 0x080CE520 <= start < 0x080CFA4C:
        return "old_agbcc -O2 -mthumb-interwork; fnmatch flag: --old2"
    return "agbcc -O2 -mthumb-interwork -fprologue-bugfix; fnmatch flag: --newpb"


def asm_files_for(symbol: str, start: int) -> list[str]:
    needles = {symbol, f"sub_{start:08x}", f"sub_{start:08X}"}
    matches: list[str] = []
    for path in ROOT.glob("asm/**/*.s"):
        try:
            data = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        if any(re.search(rf"(?m)^{re.escape(needle)}:", data) for needle in needles):
            matches.append(str(path.relative_to(ROOT)))
    return matches


def emit_assignment(identifier: str, as_json: bool) -> int:
    module = resolve(identifier)
    if module is None:
        print(f"ERROR: cannot resolve {identifier!r}; use an id from `next`, a start address, or a range.")
        return 2
    state = read_state()
    payload = module_payload(module, state)
    funcs = functions_in_range(module.start, module.end)
    payload["asm_forever"] = range_owned_by_asm_forever(module.start, module.end)
    payload["functions_left"] = len(funcs)
    payload["smallest_functions"] = [
        {
            "name": fn["name"],
            "start": f"{address(fn['vma']):#010x}",
            "end": f"{address(fn['vma']) + address(fn['size']):#010x}",
            "size": fn["size"],
            "asm": (asm_files_for(fn["name"], address(fn["vma"])) or [None])[0],
        }
        for fn in funcs[:10]
    ]
    if as_json:
        print(json.dumps(payload, indent=2))
        return 0
    print(f"{module.id} {module.name}")
    print(f"range: [{module.start:#010x}, {module.end:#010x})  ({module.size:#x} bytes)")
    if module.issue:
        print(f"issue: #{module.issue} (order {module.order} in the recommended sequence)")
    if payload["asm_forever"]:
        print("This range is intentionally permanent assembly; it is not a C-decompilation candidate.")
        return 0
    print(f"recipe: {payload['recipe']}")
    if payload["batches"]:
        print(f"suggested batches: {payload['batches']}")
    print(f"uncovered: {payload['uncovered_bytes']:#x} bytes in {len(funcs)} function(s)")
    if not funcs:
        print("No uncovered symbol rows: this module looks fully landed; re-check segments.txt.")
        return 0
    print("smallest uncovered functions:")
    for fn in payload["smallest_functions"]:
        print(f"  {fn['name']} [{fn['start']}, {fn['end']}) size {fn['size']}  {fn['asm'] or '(locate with rg in asm/)'}")
    return 0


# ---------------------------------------------------------------- bookkeeping


def emit_record(identifier: str, result: str, note: str) -> int:
    module = resolve(identifier)
    if module is None:
        print(f"ERROR: cannot resolve {identifier!r}.")
        return 2
    state = read_state()
    key = module.id
    state["attempts"][key] = state["attempts"].get(key, 0) + 1
    entry = {
        "module": key,
        "range": module.range_text,
        "result": result,
        "note": note,
        "attempt": state["attempts"][key],
        "timestamp": int(time.time()),
    }
    state["history"].append(entry)
    if result == "blocked":
        state["parked"][key] = {"reason": note or "blocked", "attempts": state["attempts"][key]}
    elif result == "landed":
        state["parked"].pop(key, None)
    write_state(state)
    print(json.dumps(entry, indent=2))
    return 0


def emit_unpark(identifier: str) -> int:
    module = resolve(identifier)
    if module is None:
        print(f"ERROR: cannot resolve {identifier!r}.")
        return 2
    state = read_state()
    removed = state["parked"].pop(module.id, None)
    state["attempts"].pop(module.id, None)
    write_state(state)
    print(f"unparked {module.id}" + ("" if removed else " (was not parked)"))
    return 0


# ---------------------------------------------------------------- verification


def emit_verify(full: bool) -> int:
    if not (ROOT / "baserom.gba").is_file():
        print("ERROR: baserom.gba is absent; refusing to claim byte-exact verification.")
        return 2
    commands = [["make", "check-headers"], ["make", "clean"], ["make", "compare"], ["make", "progress"]]
    if full:
        commands.extend([["make", "symbols"], ["make", "split"], ["make", "modmap"]])
    for command in commands:
        print("$ " + " ".join(command), flush=True)
        result = subprocess.run(command, cwd=ROOT, text=True)
        if result.returncode:
            print(f"ERROR: {' '.join(command)} failed with exit status {result.returncode}")
            return result.returncode
    if full:
        diff = run(
            [
                "git",
                "diff",
                "--exit-code",
                "--",
                "docs/analysis/symbols.csv",
                "docs/analysis/callgraph.csv",
                "docs/analysis/module-map.csv",
                "asm",
                "data",
            ]
        )
        if diff.returncode:
            print("ERROR: regeneration changed committed generated files; inspect the diff.")
            return diff.returncode
    print("Verification passed.")
    return 0


def gate_error(message: str) -> int:
    print("MERGE GATE FAILED: " + message, file=sys.stderr)
    return 1


def emit_merge_gate() -> int:
    """Enforce that a commit is a locally reproducible, reviewable snapshot."""
    branch = git("branch", "--show-current")
    if not branch or branch in {"master", "init"}:
        return gate_error("commits must be made on a feature branch based on master")
    if run(["git", "merge-base", "--is-ancestor", "master", "HEAD"]).returncode:
        return gate_error("the current branch is not based on local master")
    if not (ROOT / "baserom.gba").is_file():
        return gate_error("baserom.gba is required for byte-exact commit verification")
    if rom_status().endswith("DOES NOT MATCH expected"):
        return gate_error("the local baserom.gba SHA-1 is not the required USA A7KE image")
    if run(["git", "diff", "--quiet"]).returncode:
        return gate_error("unstaged changes exist; stage the exact reviewed snapshot before committing")
    untracked = git("ls-files", "--others", "--exclude-standard")
    if untracked:
        return gate_error("untracked non-ignored files exist; add or remove them before committing:\n" + untracked)
    staged = git("diff", "--cached", "--name-only").splitlines()
    if not staged:
        return gate_error("the commit has no staged files")
    forbidden = [
        path
        for path in staged
        if path == "baserom.gba"
        or path.endswith(".gba")
        or path == "report.json"
        or path.startswith("build/")
        or path.startswith(".pi/sessions/")
        or path.startswith(".pi/state/")
    ]
    if forbidden:
        return gate_error("forbidden staged paths: " + ", ".join(forbidden))
    whitespace = run(["git", "diff", "--cached", "--check"])
    if whitespace.returncode:
        return gate_error("staged diff has whitespace errors:\n" + whitespace.stdout + whitespace.stderr)

    # The regeneration checks duplicate the ROM-dependent CI contracts. They
    # run before the compare so the following byte-exact build uses the exact
    # committed split configuration and generated assembly.
    commands: list[list[str]] = [
        ["make", "symbols"],
        ["make", "split"],
        ["make", "modmap"],
        ["make", "check-headers"],
        ["python3", "tools/gen_report.py", "--check"],
        ["make", "clean"],
        ["make", "compare"],
    ]
    for command in commands:
        print("$ " + " ".join(command), flush=True)
        result = subprocess.run(command, cwd=ROOT, text=True)
        if result.returncode:
            return gate_error(f"{' '.join(command)} failed with exit status {result.returncode}")

    if run(["git", "diff", "--quiet"]).returncode:
        return gate_error("verification regenerated unstaged files; review and stage them, then commit again")
    if git("ls-files", "--others", "--exclude-standard"):
        return gate_error("verification left untracked non-ignored files")
    print("MERGE GATE PASSED: staged snapshot is locally byte-exact and regeneration-clean.")
    return 0


# ---------------------------------------------------------------- selftest


def emit_selftest() -> int:
    """Assertions that fail loudly when the metadata contract drifts.

    Runs without a baserom, without Docker, and without network, so CI can
    execute it on every push.
    """
    failures: list[str] = []

    def check(condition: bool, message: str) -> None:
        if not condition:
            failures.append(message)

    modules = doc_modules()
    check(len(modules) >= 37, f"module map section 4 yielded only {len(modules)} rows")
    check(len(set(modules)) == len(modules), "duplicate module ids in section 4")

    # The regression that motivated the frozen ids: module-map.csv renumbers
    # its rows as ranges land, so an id looked up there points at the wrong
    # module. M27 is the documented example (issue #68).
    m27 = modules.get("M27")
    check(m27 is not None and m27.start == 0x080988F8, "M27 must resolve to 0x080988F8 (frozen numbering)")
    check(resolve("M27") is not None and resolve("M27").start == 0x080988F8, "resolve('M27') drifted")
    check(resolve("0x080988F8") is not None and resolve("0x080988F8").id == "M27", "address lookup failed")

    queue = work_queue()
    check(len(queue) >= 30, f"work queue yielded only {len(queue)} rows")
    check(all(module.issue for module in queue), "a queue row is missing its issue number")
    check(all(module.id in modules for module in queue), "a queue row names an unknown module id")
    orders = [module.order for module in queue]
    check(orders == sorted(orders), "queue is not sorted by recommended order")

    # Ranges in section 4 must tile the code span without gaps or overlaps.
    ordered = sorted(modules.values(), key=lambda module: module.start)
    for left, right in zip(ordered, ordered[1:]):
        check(left.end == right.start, f"gap or overlap between {left.id} and {right.id}")

    check(bool(segments()), "segments.txt did not parse")
    check(range_covered(0x08007300, 0x080075B8), "known landed range reported as not covered")

    # The merge gate refuses untracked non-ignored files, so a live harness
    # depends on these two paths being ignored. A trailing comment on a
    # .gitignore line silently voids the pattern, which is how this broke.
    check(ignored(".pi/sessions/probe.jsonl"), ".pi/sessions is not gitignored")
    check(ignored(".pi/state/autopilot.json"), ".pi/state is not gitignored")

    launcher = (ROOT / "tools/pi-knidl.sh").read_text(encoding="utf-8")
    check(
        f'core.hooksPath {HOOKS_RELATIVE}' in launcher,
        "the launcher must configure core.hooksPath relative, or sibling worktrees inherit this hook",
    )
    check("absolute" in hook_status() or hook_status() in {"active", "inactive (launch Pi through ./tools/pi-knidl.sh)"}
          or "pointing elsewhere" in hook_status(), "hook_status returned an unexpected string")

    recipe = compiler_recipe(0x080988F8)
    check("--newpb" in recipe, "game-code recipe lost its -fprologue-bugfix flag")

    for message in failures:
        print("FAIL: " + message)
    if failures:
        print(f"{len(failures)} selftest failure(s)")
        return 1
    print("pi_harness selftest passed")
    return 0


# ---------------------------------------------------------------- cli


def main() -> int:
    parser = argparse.ArgumentParser(description="KnIDL Pi harness oracle")
    subparsers = parser.add_subparsers(dest="command", required=True)

    for name in ("status", "next", "plan"):
        sub = subparsers.add_parser(name)
        sub.add_argument("--json", action="store_true")
    subparsers.add_parser("preflight")
    subparsers.add_parser("selftest")
    subparsers.add_parser("merge-gate", help="strict pre-commit validation for a staged snapshot")

    assignment = subparsers.add_parser("assignment")
    assignment.add_argument("module", help="frozen module id (M27), start address, or range")
    assignment.add_argument("--json", action="store_true")

    record = subparsers.add_parser("record", help="record the outcome of a work unit")
    record.add_argument("module")
    record.add_argument("--result", required=True, choices=["landed", "blocked", "progress"])
    record.add_argument("--note", default="")

    unpark = subparsers.add_parser("unpark", help="clear a parked module and its attempt count")
    unpark.add_argument("module")

    verify = subparsers.add_parser("verify")
    verify.add_argument("--full", action="store_true", help="also regenerate and diff generated outputs")

    args = parser.parse_args()
    if args.command == "status":
        return emit_status(args.json)
    if args.command == "preflight":
        return emit_preflight()
    if args.command == "next":
        return emit_next(args.json)
    if args.command == "plan":
        return emit_plan(args.json)
    if args.command == "assignment":
        return emit_assignment(args.module, args.json)
    if args.command == "record":
        return emit_record(args.module, args.result, args.note)
    if args.command == "unpark":
        return emit_unpark(args.module)
    if args.command == "selftest":
        return emit_selftest()
    if args.command == "merge-gate":
        return emit_merge_gate()
    return emit_verify(args.full)


if __name__ == "__main__":
    raise SystemExit(main())
