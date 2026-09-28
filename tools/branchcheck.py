#!/usr/bin/env python3
"""Relative branches the shift test cannot see (issue #36, docs/data.md 8.4).

The shift test compares 4-aligned words whose value looks like a ROM address;
a PC-relative branch has no such value, so a branch that crosses an insertion
point and is written as raw bytes (`.short 0xEAFC`, `.incbin`) keeps its old
offset in the shifted image, jumps pad bytes too far or too short, and is
counted nowhere.  The boot test found three of them (TaskSwitchTrampoline,
TaskYieldTrampoline and TaskExitTrampoline in asm/sdk_libc.s, ARM `b` after
`bx pc`, emitted as a Thumb stmia plus a raw halfword).

This scan decodes, in every code segment of docs/analysis/segments.txt, each
ARM B/BL word (ARM segments and the ARM islands after `bx pc`), each Thumb
BL pair and each Thumb B / B<cond> whose target is in a code segment, keeps those whose instruction and target
lie on different sides of the insertion point, and checks that the shifted
image's bytes branch to the target's new address.  It prints the ones that
do not; exit status 1 if there are any.

  python3 tools/branchcheck.py --rom knidl.gba --shifted build/boottest/agb_init.gba --point 0x08000310
"""

import argparse
import struct
import sys

ROM_BASE = 0x08000000
CODE_KINDS = ("arm_code", "thumb_code", "c_code")


def parse_segments(path):
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            p = line.split()
            segs.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
    return sorted(segs)


def sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def branches(img, start, end, kind):
    """(address, target, what) of each branch-shaped instruction in [start, end)."""
    out = []
    if kind == "arm_code":
        for a in range((start + 3) & ~3, end - 3, 4):
            w = struct.unpack_from("<I", img, a - ROM_BASE)[0]
            if (w >> 25) & 7 == 5 and (w >> 28) != 0xF:
                out.append((a, a + 8 + (sext(w & 0xFFFFFF, 24) << 2), "arm b/bl"))
        return out
    a = (start + 1) & ~1
    while a < end - 1:
        h = struct.unpack_from("<H", img, a - ROM_BASE)[0]
        if h == 0x4778 and a + 8 <= end:
            # bx pc: an ARM island at the next word boundary
            arm = (a + 4) & ~3
            w = struct.unpack_from("<I", img, arm - ROM_BASE)[0]
            if (w >> 25) & 7 == 5:
                out.append((arm, arm + 8 + (sext(w & 0xFFFFFF, 24) << 2),
                            "arm b/bl after bx pc"))
        if (h & 0xF800) == 0xF000 and a + 4 <= end:
            h2 = struct.unpack_from("<H", img, a + 2 - ROM_BASE)[0]
            if (h2 & 0xF800) == 0xF800:
                off = (sext(h & 0x7FF, 11) << 12) | ((h2 & 0x7FF) << 1)
                out.append((a, a + 4 + off, "thumb bl"))
                a += 4
                continue
        if (h & 0xF800) == 0xE000:
            out.append((a, a + 4 + (sext(h & 0x7FF, 11) << 1), "thumb b"))
        elif (h & 0xF000) == 0xD000 and (h >> 8) & 0xF < 0xE:
            out.append((a, a + 4 + (sext(h & 0xFF, 8) << 1), "thumb b<cond>"))
        a += 2
    return out


def decode_at(img, a, what):
    if what.startswith("arm"):
        w = struct.unpack_from("<I", img, a - ROM_BASE)[0]
        if (w >> 25) & 7 != 5:
            return None
        return a + 8 + (sext(w & 0xFFFFFF, 24) << 2)
    h = struct.unpack_from("<H", img, a - ROM_BASE)[0]
    if what == "thumb bl":
        h2 = struct.unpack_from("<H", img, a + 2 - ROM_BASE)[0]
        if (h & 0xF800) != 0xF000 or (h2 & 0xF800) != 0xF800:
            return None
        return a + 4 + ((sext(h & 0x7FF, 11) << 12) | ((h2 & 0x7FF) << 1))
    if what == "thumb b":
        return a + 4 + (sext(h & 0x7FF, 11) << 1)
    return a + 4 + (sext(h & 0xFF, 8) << 1)


def check(a, b, point, pad, segs):
    """(checked, bad) for images a (reference) and b (pad bytes at point);
    bad: [(address, target, what, segment, target decoded from b)]."""
    end = ROM_BASE + len(a)

    def moved(x):
        return x + pad if point <= x < end else x

    code = [(s, e) for s, e, k, _n in segs if k in CODE_KINDS]

    def in_code(x):
        return any(s <= x < e for s, e in code)

    bad = []
    checked = 0
    for s, e, kind, name in segs:
        if kind not in CODE_KINDS:
            continue
        for addr, tgt, what in branches(a, s, e, kind):
            # a branch lands in code; pool data that decodes as one does not
            if not in_code(tgt):
                continue
            if (addr >= point) == (tgt >= point):
                continue
            checked += 1
            got = decode_at(b, moved(addr), what)
            if got != moved(tgt):
                bad.append((addr, tgt, what, name, got))
    return checked, bad


def report(point, checked, bad):
    print("branches crossing 0x%08X: %d checked, %d not moved with their target"
          % (point, checked, len(bad)))
    for addr, tgt, what, name, got in bad:
        print("  0x%08X %-22s -> 0x%08X (%s); shifted image branches to %s"
              % (addr, what, tgt, name,
                 "0x%08X" % got if got is not None else "(not a branch)"))


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="knidl.gba")
    ap.add_argument("--shifted", required=True)
    ap.add_argument("--point", required=True, help="insertion point VMA")
    ap.add_argument("--pad", default="0x1000")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    args = ap.parse_args()
    point = int(args.point, 0)
    pad = int(args.pad, 0)
    a = open(args.rom, "rb").read()
    b = open(args.shifted, "rb").read()
    checked, bad = check(a, b, point, pad, parse_segments(args.segments))
    report(point, checked, bad)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
