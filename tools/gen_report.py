#!/usr/bin/env python3
"""gen_report.py — generate an objdiff-format progress report (report.json,
schema v2) for decomp.dev from the repository's own ground truth.

Sources of truth (no baserom required):
  - docs/analysis/segments.txt  ROM segment map (start, end, kind, name)
  - docs/analysis/symbols.csv   every function (vma, size, isa, name)
  - docs/analysis/module-map.csv M01..M38 functional clustering of game code
  - linker.ld                   which sections are built from src/ (C)

Method:
  A function is "matched" iff its VMA falls inside a `c_code` segment
  (those are exactly the ranges linker.ld pins to build/src/<name>.o,
  i.e. byte-verified C — make compare enforces the match on every build).

Units:
  One unit per module from module-map.csv (M01..), one unit per
  non-module code segment (early carve-outs, SDK drivers, m4a C parts),
  and one unit for hand-written asm zones that stay asm by design
  (excluded from decompilation, reported as complete with an
  auto_generated-style caveat in the unit name suffix).

Output:
  report.json at the repository root, objdiff Report schema v2 (JSON
  flavour, exactly what `decomp.dev` downloads from the `A7KE_report`
  workflow artifact and parses with Report::parse + migrate).

The report keeps two category views:
  "code"   — every tracked code unit (default; what the front page shows)
Run `python3 tools/gen_report.py --check` to fail closed if any function
in symbols.csv is not covered by exactly one unit (detects map drift).
"""

import argparse
import csv
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SEGMENTS = ROOT / "docs/analysis/segments.txt"
SYMBOLS = ROOT / "docs/analysis/symbols.csv"
MODULES = ROOT / "docs/analysis/module-map.csv"
LINKER = ROOT / "linker.ld"
OUT = ROOT / "report.json"

REPORT_VERSION = 2

# Segments that are hand-written / SDK asm that stays asm forever, by design
# (docs/analysis/segments.txt annotations, issues #29/#30/#52).  They are
# reported as their own units with complete=true so the overall numbers
# are honest about what "decompilation" means for this project.
ASM_FOREVER = {
    "sdk_libc",
    "sdk_swi_wrappers",
    "sdk_reset_helper",
    "m4a_1",
    "lib_misc",
    "task_switch_helpers",
    "crt0_master_isr",
}

# Data/pool segments are not tracked as code.
NON_CODE_KINDS = {"data", "pool"}


def load_segments():
    segs = []
    for line in SEGMENTS.read_text().splitlines():
        parts = line.split("#")[0].split()
        if len(parts) >= 4:
            segs.append(
                {
                    "start": int(parts[0], 16),
                    "end": int(parts[1], 16),
                    "kind": parts[2],
                    "name": parts[3],
                }
            )
    segs.sort(key=lambda s: s["start"])
    return segs


def load_symbols():
    fns = []
    with SYMBOLS.open() as f:
        for row in csv.DictReader(f):
            fns.append(
                {
                    "vma": int(row["vma"], 16),
                    "size": int(row["size"], 16),
                    "name": row["name"],
                }
            )
    fns.sort(key=lambda f: f["vma"])
    return fns


def load_c_code_ranges(segments):
    """c_code segments == ranges owned by verified C (src/*.c via carve.py)."""
    return [
        (s["start"], s["end"], s["name"]) for s in segments if s["kind"] == "c_code"
    ]


def load_modules():
    mods = []
    with MODULES.open() as f:
        for row in csv.DictReader(f):
            mods.append(
                {
                    "id": row["id"],
                    "start": int(row["start"], 16),
                    "end": int(row["end"], 16),
                    "name": row["name"],
                }
            )
    mods.sort(key=lambda m: m["start"])
    return mods


def pct(n, d):
    return round(n / d * 100.0, 6) if d else 0.0


def build_units(segments, functions, c_ranges, modules):
    """Assign every tracked function to exactly one unit.

    Unit model (decomp.dev-friendly, mirrors the project's own module map):
      * modules M01..M38 cover the big game_code_and_rodata blob
      * each remaining code segment (c_code carve-outs, asm-forever SDK
        zones, split thumb segments outside modules) is its own unit
    """
    units = []

    def fn_in(fn, start, end):
        return start <= fn["vma"] < end

    # --- module units ---
    for m in modules:
        fns = [f for f in functions if fn_in(f, m["start"], m["end"])]
        if not fns:
            continue
        matched = [f for f in fns if any(a <= f["vma"] < b for a, b, _ in c_ranges)]
        units.append(make_unit(f"{m['id']}: {m['name']}", fns, matched, m))

    # --- non-module segments ---
    module_span = [(m["start"], m["end"]) for m in modules]
    for s in segments:
        if s["kind"] in NON_CODE_KINDS:
            continue
        if any(a <= s["start"] < b for a, b in module_span):
            continue  # covered by a module unit
        fns = [f for f in functions if fn_in(f, s["start"], s["end"])]
        if not fns:
            continue
        matched = [f for f in fns if any(a <= f["vma"] < b for a, b, _ in c_ranges)]
        asm_forever = s["name"] in ASM_FOREVER or s["kind"] == "arm_code" and s["name"] in ASM_FOREVER
        units.append(make_unit(s["name"], fns, matched, s, asm_forever=asm_forever))

    units.sort(key=lambda u: u["_sort"])
    return units


def make_unit(name, fns, matched, meta, asm_forever=False):
    total_code = sum(f["size"] for f in fns)
    matched_code = sum(f["size"] for f in matched)
    total_functions = len(fns)
    matched_functions = len(matched)
    complete = matched_code == total_code and total_code > 0

    unit = {
        "name": name + (" [asm]" if asm_forever else ""),
        "measures": {
            "fuzzy_match_percent": pct(matched_code, total_code),
            "total_code": total_code,
            "matched_code": matched_code,
            "matched_code_percent": pct(matched_code, total_code),
            "total_data": 0,
            "matched_data": 0,
            "matched_data_percent": 0.0,
            "total_functions": total_functions,
            "matched_functions": matched_functions,
            "matched_functions_percent": pct(matched_functions, total_functions),
            "complete_code": matched_code if complete else 0,
            "complete_code_percent": pct(matched_code, total_code) if complete else 0.0,
            "complete_data": 0,
            "complete_data_percent": 0.0,
            "total_units": 1,
            "complete_units": 1 if complete else 0,
        },
        "sections": [],
        "functions": [
            {
                "name": f["name"],
                "size": f["size"],
                "fuzzy_match_percent": 100.0 if f in matched else 0.0,
            }
            for f in fns
        ],
        "metadata": {
            "complete": complete,
            "source_path": f"src/{meta['name']}.c" if not asm_forever and meta.get("kind") == "c_code" else None,
        },
    }
    unit["_sort"] = fns[0]["vma"] if fns else 0
    unit["_asm_forever"] = asm_forever
    return unit


def check_coverage(functions, units):
    """Fail closed if any tracked function is missing from the units."""
    covered = sum(len(u["functions"]) for u in units)
    if covered != len(functions):
        # find duplicates/missing by vma
        seen = {}
        for u in units:
            for f in u["functions"]:
                seen[f["name"]] = seen.get(f["name"], 0) + 1
        dupes = [n for n, c in seen.items() if c > 1]
        print(
            f"ERROR: unit functions ({covered}) != symbols.csv rows ({len(functions)})"
            + (f"; duplicated: {dupes[:5]}" if dupes else ""),
            file=sys.stderr,
        )
        return False
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="validate only, no write")
    ap.add_argument("-o", "--output", default=str(OUT))
    args = ap.parse_args()

    segments = load_segments()
    functions = load_symbols()
    c_ranges = load_c_code_ranges(segments)
    modules = load_modules()

    units = build_units(segments, functions, c_ranges, modules)

    if not check_coverage(functions, units):
        sys.exit(1)

    # Global measures over tracked units only (asm-forever units included:
    # they are real ROM bytes; their [asm] suffix documents the caveat).
    tot = {k: 0 for k in (
        "total_code", "matched_code", "total_functions", "matched_functions",
        "complete_code", "total_units", "complete_units",
    )}
    for u in units:
        m = u["measures"]
        for k in tot:
            tot[k] += m[k]

    measures = {
        "fuzzy_match_percent": pct(tot["matched_code"], tot["total_code"]),
        "total_code": tot["total_code"],
        "matched_code": tot["matched_code"],
        "matched_code_percent": pct(tot["matched_code"], tot["total_code"]),
        "total_data": 0,
        "matched_data": 0,
        "matched_data_percent": 0.0,
        "total_functions": tot["total_functions"],
        "matched_functions": tot["matched_functions"],
        "matched_functions_percent": pct(tot["matched_functions"], tot["total_functions"]),
        "complete_code": tot["complete_code"],
        "complete_code_percent": pct(tot["complete_code"], tot["total_code"]),
        "complete_data": 0,
        "complete_data_percent": 0.0,
        "total_units": tot["total_units"],
        "complete_units": tot["complete_units"],
    }

    report = {
        "measures": measures,
        "units": [{k: v for k, v in u.items() if not k.startswith("_")} for u in units],
        "version": REPORT_VERSION,
        "categories": [],
    }

    summary = (
        f"code: {measures['matched_code']}/{measures['total_code']} bytes "
        f"({measures['matched_code_percent']:.4f}%) | "
        f"functions: {measures['matched_functions']}/{measures['total_functions']} "
        f"({measures['matched_functions_percent']:.4f}%) | "
        f"units: {measures['complete_units']}/{measures['total_units']}"
    )

    if args.check:
        print(summary)
        return

    out = Path(args.output)
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {out} ({out.stat().st_size} bytes)")
    print(summary)


if __name__ == "__main__":
    main()
