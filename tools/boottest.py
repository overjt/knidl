#!/usr/bin/env python3
"""Link the shifted images for the boot test (issue #36, docs/data.md 8.4).

`make shifttest` proves which words the linker moves; the boot test runs a
shifted image in an emulator to see whether the game still behaves the same.
This script builds the images: for each insertion point it links the build's
objects exactly as tools/shiftcheck.py does for its image B (linker.ld with
`--pad` bytes in front of the section, `--defsym MATCHING=0`), patches the
header checksum with tools/gbafix.py, and writes

  <out>/<section>.gba   one shifted ROM per insertion point
  <out>/roms.txt        one line per ROM, `<path>@<point VMA>+<pad>`, the
                        argument form tools/boottest/boottest.c takes

Before writing a ROM it checks, as shiftcheck.py does, that the image is the
reference plus the padding and that no word moved wrongly, and, with
tools/branchcheck.py, that every relative branch crossing the point still
reaches its target (a raw-bytes branch is invisible to the shift test's word
compare); the words that did not move (unrelocated) are the census's
business, and the emulator shows whether one of them mattered.

Run inside the knidl-builder image (`make boottest-roms`, the host side of
`make boottest`):

  python3 tools/boottest.py --elf build/knidl.elf --objs build/...o ...
"""

import argparse
import os
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import branchcheck  # noqa: E402
import shiftcheck  # noqa: E402

ROM_BASE = shiftcheck.ROM_BASE

# The default points: the first one moves everything after crt0 (the whole
# ROM but the header, crt0 and its pool), the others are shiftcheck.py's.
DEFAULT_POINTS = [s for _l, s in shiftcheck.DEFAULT_POINTS]


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--elf", default="build/knidl.elf")
    ap.add_argument("--script", default="linker.ld")
    ap.add_argument("--pad", default="0x1000")
    ap.add_argument("--at", action="append", default=[],
                    help="insertion point (section name; repeatable; "
                         "default: shiftcheck.py's points)")
    ap.add_argument("--out", default="build/boottest")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--objs", nargs="+", required=True)
    args = ap.parse_args()
    pad = int(args.pad, 0)
    if pad <= 0 or pad % 4:
        sys.exit("error: --pad must be a positive multiple of 4")

    secs = shiftcheck.elf_sections(args.elf)
    by_name = dict((n, v) for n, v, _s in secs)
    with open(args.script) as f:
        script = f.read()
    names = [s.lstrip(".") for s in (args.at or DEFAULT_POINTS)]
    for n in names:
        if n not in by_name:
            sys.exit("error: no section .%s in %s" % (n, args.elf))

    segs = shiftcheck.parse_segments(args.segments)
    os.makedirs(args.out, exist_ok=True)
    lines = []
    bad_branches = 0
    with tempfile.TemporaryDirectory() as tmp:
        a = shiftcheck.link(script, args.objs, tmp, "a")
        for name in names:
            vma = by_name[name]
            b = shiftcheck.link(shiftcheck.shifted_script(script, name, pad),
                                args.objs, tmp, "b_" + name)
            if len(b) != len(a) + pad:
                sys.exit("error: .%s: shifted image is 0x%X bytes, expected "
                         "0x%X" % (name, len(b), len(a) + pad))
            r = shiftcheck.compare(a, b, vma - ROM_BASE, pad)
            if r["broken"] or r["moved_wrongly"]:
                sys.exit("error: .%s: %d broken or wrongly moved word(s); "
                         "run make shifttest" % (name, len(r["broken"])
                                                 + len(r["moved_wrongly"])))
            checked, bad = branchcheck.check(a, b, vma, pad, segs)
            if bad:
                branchcheck.report(vma, checked, bad)
                bad_branches += len(bad)
            path = os.path.join(args.out, name + ".gba")
            with open(path, "wb") as f:
                f.write(b)
            subprocess.run([sys.executable, os.path.join(
                os.path.dirname(os.path.abspath(__file__)), "gbafix.py"), path],
                check=True, stdout=subprocess.DEVNULL)
            lines.append("%s@0x%08X+0x%X" % (path, vma, pad))
            print("%-40s 0x%08X +0x%X  %6d unrelocated word(s) at or after "
                  "the point, %d crossing branch(es) checked"
                  % (path, vma, pad, len(r["unrelocated"]), checked))
    with open(os.path.join(args.out, "roms.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    if bad_branches:
        sys.exit("error: %d relative branch(es) do not reach their target in "
                 "a shifted image (raw bytes the linker never saw); see "
                 "tools/branchcheck.py" % bad_branches)


if __name__ == "__main__":
    main()
