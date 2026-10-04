#!/usr/bin/env python3
"""How much of the code the boot test runs (issue #168, docs/data.md 8.4).

`knidl-boottest --coverage FILE` runs the reference one instruction at a
time and writes the address ranges it executed (ROM and IWRAM).  This script
maps those ranges onto the functions of docs/analysis/symbols.csv and prints,
per coverage file and for all of them together, how many functions ran at
least one instruction and how many of their bytes ran.

IWRAM ranges are mapped back to the ROM code copied there (COPIES below), so
the ISR and the sound mixer count as the functions they are.  What runs in
IWRAM outside those copies is code on the stack: the SRAM driver
(src/agb_sram.c) copies ReadSram_Core and VerifySram_Core into a local
buffer and calls it there; it is reported as a byte count.  The coverage
files are build products (build/boottest/coverage-*.txt): never committed.

  python3 tools/boottest/coverage.py --syms build/boottest/syms.txt \\
      build/boottest/coverage-input.txt ...
"""

import argparse
import bisect
import csv
import os
import sys

# Code copied from ROM into IWRAM and run there: (ROM symbol, IWRAM symbol or
# address, bytes).  src/agb_init.c copies the master ISR (CpuSet of 160
# halfwords from 0x08000108 to IWRAM_START + 0x1030) and BuildOam (0x100
# halfwords to IWRAM_START + 0x1F40); src/m4a_c1.c copies SoundMainRAM into
# gSoundMainRAM_Buffer.
COPIES = [
    ("MasterIsr", 0x03001030, 320),
    ("BuildOam", 0x03001F40, 0x200),
    ("SoundMainRAM", "gSoundMainRAM_Buffer", None),
]


def load_functions(path):
    funcs = []
    with open(path) as f:
        for row in csv.DictReader(f):
            funcs.append((int(row["vma"], 16), int(row["size"], 16), row["name"]))
    funcs.sort()
    return funcs


def load_syms(path):
    syms = {}
    with open(path) as f:
        for line in f:
            parts = line.split()
            if len(parts) == 3:
                syms[parts[2]] = int(parts[0], 16)
    return syms


def load_ranges(path):
    rom, iwram = [], []
    with open(path) as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            a, b = (int(x, 16) for x in line.split())
            (rom if a >= 0x08000000 else iwram).append((a, b))
    return rom, iwram


def iwram_to_rom(iwram, copies):
    """Map executed IWRAM ranges into the ROM images they were copied from."""
    out, unmapped = [], 0
    for a, b in iwram:
        hit = False
        for rom_vma, ram, size in copies:
            lo, hi = max(a, ram), min(b, ram + size)
            if lo < hi:
                out.append((rom_vma + lo - ram, rom_vma + hi - ram))
                hit = True
        if not hit:
            unmapped += b - a
    return out, unmapped


def measure(funcs, ranges):
    """Functions with at least one executed byte, and executed function bytes."""
    starts = [v for v, _s, _n in funcs]
    hit = set()
    covered = {}
    for a, b in ranges:
        i = max(bisect.bisect_right(starts, a) - 1, 0)
        while i < len(funcs) and funcs[i][0] < b:
            v, s, _n = funcs[i]
            lo, hi = max(a, v), min(b, v + s)
            if lo < hi:
                hit.add(i)
                covered.setdefault(i, []).append((lo, hi))
            i += 1
    nbytes = 0
    for spans in covered.values():
        spans.sort()
        end = 0
        for lo, hi in spans:
            lo = max(lo, end)
            if lo < hi:
                nbytes += hi - lo
            end = max(end, hi)
    return hit, nbytes


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--symbols", default="docs/analysis/symbols.csv")
    ap.add_argument("--syms", required=True, help="nm output (for COPIES)")
    ap.add_argument("coverage", nargs="+")
    args = ap.parse_args()

    funcs = load_functions(args.symbols)
    syms = load_syms(args.syms)
    copies = []
    for rom_name, ram, size in COPIES:
        if rom_name not in syms or (isinstance(ram, str) and ram not in syms):
            sys.exit("error: %s: no symbol for the IWRAM copy %s" % (args.syms, rom_name))
        rom_vma = syms[rom_name] & ~1
        if isinstance(ram, str):
            ram = syms[ram]
        if size is None:
            size = next(s for v, s, _n in funcs if v == rom_vma)
        copies.append((rom_vma, ram, size))
    total_bytes = sum(s for _v, s, _n in funcs)

    def line(label, hit, nbytes, unmapped):
        print("%-36s %5d of %d functions (%5.1f%%), %7d of %d bytes (%5.1f%%)%s"
              % (label, len(hit), len(funcs), 100.0 * len(hit) / len(funcs), nbytes,
                 total_bytes, 100.0 * nbytes / total_bytes,
                 ", %d IWRAM bytes on the stack" % unmapped if unmapped else ""))

    union_hit, union_ranges, union_unmapped = set(), [], 0
    for path in args.coverage:
        rom, iwram = load_ranges(path)
        mapped, unmapped = iwram_to_rom(iwram, copies)
        hit, nbytes = measure(funcs, rom + mapped)
        line(os.path.basename(path), hit, nbytes, unmapped)
        union_hit |= hit
        union_ranges += rom + mapped
        union_unmapped = max(union_unmapped, unmapped)
    if len(args.coverage) > 1:
        hit, nbytes = measure(funcs, union_ranges)
        line("all scripts", hit, nbytes, 0)


if __name__ == "__main__":
    main()
