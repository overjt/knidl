#!/usr/bin/env python3
"""Data-structure metrics (issue #36, docs/data.md).

Prints two reproducible numbers that track how much of the ROM's data is
structure (labels and symbolic pointers) rather than anonymous bytes:

  1. ROM data symbols still defined by ABSOLUTE address: every name in
     tools/split_config.json "data_symbols" whose value lies in the ROM and
     which asm/rom_syms.s still defines as `name = 0x08......` (target 0:
     each one should be a real label inside its data segment's file);
  2. pointer-like words inside data segments: 4-aligned words whose value
     lies in 0x08000000-0x08800000, split into the ones the committed files
     already emit symbolically (`.word <symbol>`) and the ones still inside
     `.incbin` slices (or emitted as numbers), the latter split again into
     "points at code" (a code or literal-pool segment) and "points at data".

Pointer-LIKE is not pointer: most of the remaining "points at data" words
sit in graphics, samples and songs, where a 0x08xxxxxx value is as likely
to be pixels as an address.  The counts measure the work left, not proven
pointers; the rules that prove a pointer are in docs/data.md.

The data segments are the `data`-kind rows of docs/analysis/segments.txt.
Their committed files are found by section name (`.section .<segment>`)
under asm/ and data/, and are parsed directive by directive: `.incbin`,
`.word`, `.short`, `.byte` and labels, which covers both the structure-only
emitter's output and the older value-list files, so the same tool measures
a tree before and after a data run.

Run inside the knidl-builder image via `make datastats`, or directly:

  python3 tools/datastats.py --rom baserom.gba
"""

import argparse
import glob
import json
import os
import re
import struct
import sys

ROM_BASE = 0x08000000
PTR_LO = 0x08000000
PTR_HI = 0x08800000

CODE_KINDS = ("arm_code", "thumb_code", "c_code", "pool")

# The cartridge header is hand-written source (asm/rom_header.s, patched by
# tools/gbafix.py), not ROM data: only its logo is extracted from baserom.
NOT_MEASURED = ("rom_header",)

SECTION_RE = re.compile(r'^\s*\.section\s+\.([A-Za-z0-9_]+)(?:\.tail)?\b')
INCBIN_RE = re.compile(
    r'^\s*\.incbin\s+"([^"]+)"\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*'
    r'(0x[0-9A-Fa-f]+|\d+)\s*$'
)
DIRECTIVE_RE = re.compile(r'^\s*\.(word|short|hword|byte)\s+(.*)$')
LABEL_RE = re.compile(r'^\s*([A-Za-z_.$][A-Za-z0-9_.$]*):\s*$')
NUMBER_RE = re.compile(r'^(?:0x[0-9A-Fa-f]+|-?\d+)$')
ABS_RE = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*$')


def parse_segments(path):
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            p = line.split()
            segs.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
    return segs


def segment_of(segs, addr):
    for s, e, k, n in segs:
        if s <= addr < e:
            return (n, k)
    return (None, None)


def segment_files(data_names):
    """{segment name: [file, ...]} for every committed file that opens one
    of the given sections (asm/*.s, asm/*/*.s, data/*.s)."""
    files = {}
    paths = sorted(
        glob.glob("asm/*.s") + glob.glob("asm/*/*.s") + glob.glob("data/*.s")
    )
    for path in paths:
        with open(path) as f:
            for line in f:
                m = SECTION_RE.match(line)
                if m and m.group(1) in data_names:
                    files.setdefault(m.group(1), []).append(path)
                    break
    return files


def walk_file(path, start):
    """Yield (addr, size, symbolic) for every byte-producing directive of a
    segment file, plus the addresses of its labels.

    `symbolic` is True for a `.word` whose operand is not a plain number.
    Addresses advance from `start`; a `.tail` section continues the count
    (it is linked right after the main section).
    """
    items = []
    labels = []
    addr = start
    with open(path) as f:
        for raw in f:
            line = raw.split("@", 1)[0].rstrip()
            if not line.strip():
                continue
            m = INCBIN_RE.match(line)
            if m:
                length = int(m.group(3), 0)
                items.append((addr, length, False, "incbin"))
                addr += length
                continue
            m = DIRECTIVE_RE.match(line)
            if m:
                kind, operands = m.group(1), m.group(2)
                size = {"word": 4, "short": 2, "hword": 2, "byte": 1}[kind]
                for op in operands.split(","):
                    op = op.strip()
                    symbolic = kind == "word" and not NUMBER_RE.match(op)
                    items.append((addr, size, symbolic, kind))
                    addr += size
                continue
            m = LABEL_RE.match(line)
            if m:
                labels.append((addr, m.group(1)))
    return items, labels, addr


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--config", default="tools/split_config.json")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--rom-syms", default="asm/rom_syms.s")
    ap.add_argument("--verbose", action="store_true",
                    help="per-segment breakdown")
    args = ap.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    rom_end = ROM_BASE + len(rom)
    segs = parse_segments(args.segments)
    with open(args.config) as f:
        cfg = json.load(f)
    data_symbols = dict(
        (int(a, 16), n) for a, n in cfg.get("data_symbols", {}).items()
    )
    rom_data_names = set(
        n for a, n in data_symbols.items() if ROM_BASE <= a < rom_end
    )

    # ---- metric 1: ROM data symbols defined by absolute address ----------
    absolute = set()
    with open(args.rom_syms) as f:
        for line in f:
            m = ABS_RE.match(line.strip())
            if m and m.group(1) in rom_data_names:
                absolute.add(m.group(1))

    # ---- metric 2: pointer-like words in data segments -------------------
    data_segs = [
        (s, e, k, n) for s, e, k, n in segs
        if k == "data" and n not in NOT_MEASURED
    ]
    files = segment_files(set(n for _s, _e, _k, n in data_segs))
    total = {"symbolic": 0, "code": 0, "data": 0}
    rows = []
    labels_total = 0
    incbins_total = 0
    missing = []
    for s, e, _k, name in data_segs:
        paths = files.get(name)
        if not paths:
            missing.append(name)
            continue
        symbolic_at = set()
        covered = 0
        for path in paths:
            items, labels, _end = walk_file(path, s)
            labels_total += sum(
                1 for _a, n in labels if n != name
            )
            for addr, size, symbolic, kind in items:
                covered += size
                if kind == "incbin":
                    incbins_total += 1
                if symbolic:
                    symbolic_at.add(addr)
        if covered != e - s:
            sys.exit(
                "error: %s: files cover 0x%X bytes, segment is 0x%X"
                % (name, covered, e - s)
            )
        row = {"symbolic": 0, "code": 0, "data": 0}
        a = (s + 3) & ~3
        while a + 4 <= e:
            v = struct.unpack_from("<I", rom, a - ROM_BASE)[0]
            if a in symbolic_at:
                if PTR_LO <= v < PTR_HI:
                    row["symbolic"] += 1
            elif PTR_LO <= v < PTR_HI:
                _tn, tk = segment_of(segs, v & ~1)
                row["code" if tk in CODE_KINDS else "data"] += 1
            a += 4
        for key in row:
            total[key] += row[key]
        rows.append((name, row))
    if missing:
        sys.exit("error: no committed file for data segment(s): %s"
                 % ", ".join(missing))

    print("ROM data symbols defined by absolute address: %d of %d"
          % (len(absolute), len(rom_data_names)))
    print("ROM data labels in data segment files: %d (%d .incbin slices)"
          % (labels_total, incbins_total))
    print("pointer-like words in data segments (4-aligned, 0x%08X-0x%08X):"
          % (PTR_LO, PTR_HI))
    print("    %d symbolic" % total["symbolic"])
    print("    %d not symbolic, points at code" % total["code"])
    print("    %d not symbolic, points at data" % total["data"])
    if args.verbose:
        print()
        print("%-46s %9s %9s %9s" % ("segment", "symbolic", "code", "data"))
        for name, row in rows:
            print("%-46s %9d %9d %9d"
                  % (name, row["symbolic"], row["code"], row["data"]))


if __name__ == "__main__":
    main()
