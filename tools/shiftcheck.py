#!/usr/bin/env python3
"""The shift test (issue #36, docs/data.md section 8): is the ROM movable?

Links the ROM a second time with N bytes of padding inserted at a section
boundary P and compares the two images word by word.  Every 4-aligned word
of image A whose value points into the moved part (value & ~1 in
[P, end of ROM)) must read value + N in image B:

  relocated     B == value + N: the word is a symbol, the linker moved it;
  unrelocated   B == value: the word is a number (inside an .incbin slice or
                a raw constant).  Either a real pointer that a shifted ROM
                would break, or a coincidence (pixels, a value field) that
                only looks like an address; tools/ptrcensus.py tells them
                apart;
  broken        anything else (a symbol+offset whose base and target lie on
                different sides of P, say).

It also counts the words whose target lies BEFORE P but that changed
anyway (a symbol that moved although its target did not), and the bytes
that differ without belonging to a relocated aligned word (relative
branches across P, unaligned pointer operands such as the m4a GOTO/PATT
targets once they are symbols).

No emulator is needed and the test is exact about what moved: the same
objects are linked twice, and only the linker decides.  How the padding is
inserted depends on linker.ld: every section whose pinned address is at or
after P gets the pin + N, and a `. = . + N;` statement goes in front of the
section at P for the sections that follow each other without pins
(docs/data.md section 8.2).  The MEMORY region is widened to 32 MiB (the
GBA maximum) so the padded image fits, and `--defsym MATCHING=0` turns the
compare-mode address assertions off.

Run inside the knidl-builder image via `make shifttest` (after a build), or:

  python3 tools/shiftcheck.py --elf build/knidl.elf --objs build/...o ...

`--at` takes section names or addresses (a section start); the default is
a fixed set of insertion points: the start of AgbInit (everything after
crt0 moves), right after the code, between the data zones and inside the
song zone.  `--json FILE` writes every unrelocated word of the LOWEST
insertion point (the full set; a higher point tests a subset of the same
words) for tools/ptrcensus.py.
"""

import argparse
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
from array import array

ROM_BASE = 0x08000000

# (label, section): the default insertion points.
DEFAULT_POINTS = [
    ("after crt0 (all code and data move)", "agb_init"),
    ("right after the code", "level_object_tables"),
    ("level data (rooms)", "room_bg_anims"),
    ("compressed graphics", "compressed_graphics"),
    ("inside the song zone", "m4a_songs_2"),
    ("behaviour tables (seg 18)", "gap_sram_driver_fn_table_asset_metadata_index"),
    ("song tail (seg 19)", "song_tail_misc_audio"),
]

SECTION_LINE_RE = re.compile(
    r'^(\s*)\.([A-Za-z0-9_]+)(\s+)(0x[0-9A-Fa-f]+)?(\s*):(\s*)\{')
MEMORY_RE = re.compile(r'(ROM\s*:\s*ORIGIN\s*=\s*0x08000000\s*,\s*LENGTH\s*=\s*)8M')


def run(cmd):
    p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                       universal_newlines=True)
    if p.returncode:
        sys.stderr.write(p.stdout + p.stderr)
        sys.exit("error: %s failed" % cmd[0])
    return p.stdout


def elf_sections(elf):
    """[(name, vma, size)] of the ELF's allocated PROGBITS sections, by VMA."""
    out = run(["arm-none-eabi-readelf", "-W", "-S", elf])
    secs = []
    for line in out.splitlines():
        m = re.match(r'\s*\[\s*\d+\]\s+\.(\S+)\s+PROGBITS\s+([0-9a-f]+)\s+'
                     r'[0-9a-f]+\s+([0-9a-f]+)\s+\S+\s+(\S*A\S*)', line)
        if m:
            secs.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    secs.sort(key=lambda s: s[1])
    return secs


def parse_segments(path):
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            p = line.split()
            segs.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
    segs.sort()
    return segs


def shifted_script(text, at_section, pad):
    """linker.ld text with `pad` bytes inserted in front of `.at_section`."""
    out = []
    seen = False
    for line in text.splitlines(True):
        m = SECTION_LINE_RE.match(line)
        if m:
            name = m.group(2)
            if name == at_section:
                seen = True
                out.append("%s. = . + 0x%X;\n" % (m.group(1), pad))
            if seen and m.group(4):
                addr = int(m.group(4), 16) + pad
                line = "%s.%s%s0x%08X%s:%s{%s" % (
                    m.group(1), name, m.group(3), addr, m.group(5),
                    m.group(6), line[m.end():])
        out.append(line)
    if not seen:
        sys.exit("error: section .%s not found in the linker script"
                 % at_section)
    text = "".join(out)
    text, n = MEMORY_RE.subn(r'\g<1>32M', text)
    if n != 1:
        sys.exit("error: the ROM MEMORY region is not the expected 8M one")
    return text


def link(script_text, objs, tmpdir, tag):
    lpath = os.path.join(tmpdir, tag + ".ld")
    epath = os.path.join(tmpdir, tag + ".elf")
    bpath = os.path.join(tmpdir, tag + ".gba")
    with open(lpath, "w") as f:
        f.write(script_text)
    rsp = os.path.join(tmpdir, tag + ".objs")
    with open(rsp, "w") as f:
        f.write("\n".join(objs) + "\n")
    run(["arm-none-eabi-ld", "--defsym", "MATCHING=0", "-T", lpath,
         "-o", epath, "@" + rsp])
    run(["arm-none-eabi-objcopy", "-O", "binary", epath, bpath])
    with open(bpath, "rb") as f:
        return f.read()


def seg_index(segs):
    starts = [s for s, _e, _k, _n in segs]
    import bisect

    def find(addr):
        i = bisect.bisect_right(starts, addr) - 1
        if i >= 0 and segs[i][0] <= addr < segs[i][1]:
            return segs[i]
        return None
    return find


def compare(a, b, p_off, pad):
    """Word-by-word verdicts for one insertion point (offsets, not VMAs)."""
    end = ROM_BASE + len(a)
    p_vma = ROM_BASE + p_off
    # B with the padding cut out lines up with A byte for byte
    bb = b[:p_off] + b[p_off + pad:]
    wa = array("I")
    wa.frombytes(a[:len(a) & ~3])
    wb = array("I")
    wb.frombytes(bb[:len(a) & ~3])
    if sys.byteorder != "little":
        wa.byteswap()
        wb.byteswap()
    relocated = []
    unrelocated = []
    broken = []
    moved_wrongly = []
    accounted = set()
    for i, v in enumerate(wa):
        w = wb[i]
        t = v & ~1
        if p_vma <= t < end:
            if w == (v + pad) & 0xFFFFFFFF:
                relocated.append(i * 4)
                accounted.add(i)
            elif w == v:
                unrelocated.append(i * 4)
            else:
                broken.append((i * 4, v, w))
                accounted.add(i)
        elif w != v and ROM_BASE <= t < end:
            moved_wrongly.append((i * 4, v, w))
            accounted.add(i)
    # bytes that differ outside the aligned words accounted for above
    other = 0
    block = 4096
    for base in range(0, len(a), block):
        if a[base:base + block] == bb[base:base + block]:
            continue
        for off in range(base, min(base + block, len(a))):
            if a[off] != bb[off] and (off // 4) not in accounted:
                other += 1
    return {
        "relocated": relocated,
        "unrelocated": unrelocated,
        "broken": broken,
        "moved_wrongly": moved_wrongly,
        "other_bytes": other,
    }


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--elf", default="build/knidl.elf")
    ap.add_argument("--script", default="linker.ld")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--pad", default="0x1000",
                    help="bytes to insert (a multiple of 4; default 0x1000)")
    ap.add_argument("--at", action="append", default=[],
                    help="insertion point: a section name or its start "
                         "address (repeatable; default: a fixed set)")
    ap.add_argument("--json", help="write the lowest point's unrelocated "
                                   "words (and per-point counts) here")
    ap.add_argument("--by-target", action="store_true",
                    help="also list the unrelocated words by the segment "
                         "they point into")
    ap.add_argument("--objs", nargs="+", required=True,
                    help="the link's objects, in the Makefile's order")
    args = ap.parse_args()
    pad = int(args.pad, 0)
    if pad <= 0 or pad % 4:
        sys.exit("error: --pad must be a positive multiple of 4")

    secs = elf_sections(args.elf)
    by_name = dict((n, (v, s)) for n, v, s in secs)
    by_addr = dict((v, n) for n, v, s in secs if s)
    segs = parse_segments(args.segments)
    find_seg = seg_index(segs)
    with open(args.script) as f:
        script = f.read()

    points = []
    for spec in (args.at or [s for _l, s in DEFAULT_POINTS]):
        label = dict((s, l) for l, s in DEFAULT_POINTS).get(spec, "")
        if re.match(r'^(0x)?[0-9A-Fa-f]{8}$', spec):
            addr = int(spec, 16)
            if addr not in by_addr:
                sys.exit("error: 0x%08X is not the start of a section" % addr)
            name = by_addr[addr]
        else:
            name = spec.lstrip(".")
            if name not in by_name:
                sys.exit("error: no section .%s in %s" % (name, args.elf))
        points.append((by_name[name][0], name, label))
    points.sort()

    with tempfile.TemporaryDirectory() as tmp:
        a = link(script, args.objs, tmp, "a")
        results = []
        for vma, name, label in points:
            b = link(shifted_script(script, name, pad), args.objs, tmp,
                     "b_" + name)
            if len(b) != len(a) + pad:
                sys.exit("error: shifted image is 0x%X bytes, expected 0x%X"
                         % (len(b), len(a) + pad))
            r = compare(a, b, vma - ROM_BASE, pad)
            results.append((vma, name, label, r))

    print("shift test: 0x%X bytes inserted at each point; 4-aligned words "
          "whose value points at or after the point" % pad)
    print()
    print("%-10s %-44s %10s %11s %7s %7s %7s"
          % ("point", "section", "relocated", "unrelocated", "broken",
             "wrong", "other"))
    for vma, name, label, r in results:
        print("0x%08X %-44s %10d %11d %7d %7d %7d"
              % (vma, name, len(r["relocated"]), len(r["unrelocated"]),
                 len(r["broken"]), len(r["moved_wrongly"]), r["other_bytes"]))
        if label:
            print("%-10s   (%s)" % ("", label))
    print()
    print("relocated: moved by the linker; unrelocated: still a number "
          "(a real pointer or a coincidence, see tools/ptrcensus.py);")
    print("broken: neither; wrong: pointed before the point but moved; "
          "other: differing bytes outside those words (branches, unaligned "
          "operands).")

    # per holding segment, for the lowest point
    vma0, name0, _l, r0 = results[0]
    rows = {}
    for key in ("relocated", "unrelocated"):
        for off in r0[key]:
            seg = find_seg(ROM_BASE + off)
            sname = seg[3] if seg else "?"
            row = rows.setdefault(sname, {"relocated": 0, "unrelocated": 0,
                                          "start": seg[0] if seg else 0})
            row[key] += 1
    print()
    print("by holding segment, insertion at 0x%08X (.%s):" % (vma0, name0))
    print("%-46s %10s %11s" % ("segment", "relocated", "unrelocated"))
    code_kinds = ("arm_code", "thumb_code", "c_code", "pool")
    code_row = {"relocated": 0, "unrelocated": 0}
    for sname, row in sorted(rows.items(), key=lambda kv: kv[1]["start"]):
        seg = find_seg(row["start"])
        if seg and seg[2] in code_kinds:
            code_row["relocated"] += row["relocated"]
            code_row["unrelocated"] += row["unrelocated"]
            continue
        print("%-46s %10d %11d" % (sname, row["relocated"], row["unrelocated"]))
    print("%-46s %10d %11d" % ("(all code segments)", code_row["relocated"],
                               code_row["unrelocated"]))

    if args.by_target:
        tgt = {}
        wa = struct.unpack_from("<%dI" % (len(a) // 4), a)
        for off in r0["unrelocated"]:
            seg = find_seg(wa[off // 4] & ~1)
            sname = seg[3] if seg else "?"
            tgt.setdefault(sname, [0, seg[0] if seg else 0])[0] += 1
        print()
        print("unrelocated words by the segment they point into:")
        for sname, (n, _s) in sorted(tgt.items(), key=lambda kv: kv[1][1]):
            print("%-46s %11d" % (sname, n))

    if args.json:
        wa = struct.unpack_from("<%dI" % (len(a) // 4), a)
        out = {
            "pad": pad,
            "points": [
                {"vma": "0x%08X" % vma, "section": name,
                 "relocated": len(r["relocated"]),
                 "unrelocated": len(r["unrelocated"]),
                 "broken": len(r["broken"]),
                 "moved_wrongly": len(r["moved_wrongly"]),
                 "other_bytes": r["other_bytes"]}
                for vma, name, _l, r in results
            ],
            "lowest": "0x%08X" % vma0,
            "unrelocated": [
                ["0x%08X" % (ROM_BASE + off), "0x%08X" % wa[off // 4]]
                for off in r0["unrelocated"]
            ],
            "broken": [["0x%08X" % (ROM_BASE + o), "0x%08X" % v, "0x%08X" % w]
                       for o, v, w in r0["broken"]],
            "moved_wrongly": [["0x%08X" % (ROM_BASE + o), "0x%08X" % v,
                               "0x%08X" % w]
                              for o, v, w in r0["moved_wrongly"]],
        }
        with open(args.json, "w") as f:
            json.dump(out, f, indent=0)
            f.write("\n")
    bad = sum(len(r["broken"]) + len(r["moved_wrongly"])
              for _v, _n, _l, r in results)
    if bad:
        print()
        print("note: %d broken or wrongly moved word(s); see --json" % bad)


if __name__ == "__main__":
    main()
