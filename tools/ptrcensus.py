#!/usr/bin/env python3
"""Pointer census (issue #36, docs/data.md section 8): what the shift test's
unrelocated words are.

tools/shiftcheck.py lists every 4-aligned word whose value points into the
moved part of the ROM but that the linker did not move.  This tool sorts
each of them into one of three classes, from evidence only:

  proven pointer   a consumer or a format parse says the word IS a pointer,
                   so a shifted ROM breaks it (the work left: symbolize it);
  coincidence      the word lies in bytes that a format or a consumer proves
                   hold no pointer: instructions, a record's value field, an
                   LZ77 stream, an m4a track's events, a PCM sample body,
                   raw tiles or a palette whose size the consumer gives;
  unknown          neither.  It may be either, so a zone that such a word
                   points into is not proven movable.

The evidence comes from providers.  The built-in ones read the linked ELF
(ARM/Thumb mapping symbols: a word inside an instruction stream is two
instructions, not a literal) and the config (the value fields of a
pointer_tables record, raw_ranges).  The format providers live in
tools/census_*.py; each has

    provide(rom, cfg, segs) -> {"coincidence": [(start, end, kind, why)],
                                "pointer": [(addr, why)]}

with VMAs (end exclusive).  A word is a coincidence when all four of its
bytes lie in coincidence ranges; a word that one provider proves a pointer
and another covers as a coincidence is a conflict and stops the run.

Run inside the knidl-builder image via `make shifttest` (after
shiftcheck.py wrote build/shifttest.json), or:

  python3 tools/ptrcensus.py --shift build/shifttest.json
"""

import argparse
import bisect
import glob
import importlib.util
import json
import os
import re
import struct
import subprocess
import sys

ROM_BASE = 0x08000000
CODE_KINDS = ("arm_code", "thumb_code", "c_code", "pool")


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


class Ranges(object):
    """A union of [start, end) ranges, each with the first (kind, why) that
    claimed it."""

    def __init__(self, items):
        items = sorted(items)
        self.starts = []
        self.ends = []
        self.info = []
        for s, e, kind, why in items:
            if e <= s:
                continue
            if self.ends and s <= self.ends[-1]:
                if e > self.ends[-1]:
                    self.ends[-1] = e
                continue
            self.starts.append(s)
            self.ends.append(e)
            self.info.append((kind, why))

    def covering(self, a, b):
        """(kind, why) if [a, b) lies inside the union, else None."""
        i = bisect.bisect_right(self.starts, a) - 1
        if i < 0 or self.ends[i] < b:
            return None
        return self.info[i]


def elf_mapping(elf):
    """[(vma, 'a'|'t'|'d')] of the ELF's ARM mapping symbols, by address."""
    out = subprocess.run(["arm-none-eabi-readelf", "-sW", elf],
                         stdout=subprocess.PIPE, universal_newlines=True,
                         check=True).stdout
    marks = []
    for line in out.splitlines():
        m = re.match(r'\s*\d+:\s+([0-9a-f]+)\s+0\s+NOTYPE\s+LOCAL\s+DEFAULT\s+'
                     r'\d+\s+\$([atd])(?:\.\d+)?\s*$', line)
        if m:
            marks.append((int(m.group(1), 16), m.group(2)))
    marks.sort()
    return marks


def code_provider(rom, cfg, segs, elf):
    """Instruction words of the code segments.

    A 4-aligned word that the mapping symbols place inside an ARM or Thumb
    instruction stream is instructions (a Thumb word is two of them), not a
    literal; config raw_ranges (code of the other ISA kept raw) count too.
    Literal-pool words ($d) get no verdict here."""
    marks = elf_mapping(elf)
    addrs = [a for a, _k in marks]
    out = []
    for s, e, kind, name in segs:
        if kind not in CODE_KINDS:
            continue
        i = bisect.bisect_right(addrs, s) - 1
        state = marks[i][1] if i >= 0 else "d"
        cur = s
        j = bisect.bisect_left(addrs, s)
        while cur < e:
            nxt = addrs[j] if j < len(addrs) and addrs[j] < e else e
            if state in ("a", "t") and nxt > cur:
                out.append((cur, nxt, "instructions",
                            "%s: %s code (ELF mapping symbols)"
                            % (name, "ARM" if state == "a" else "Thumb")))
            if nxt >= e:
                break
            state = marks[j][1]
            cur = nxt
            j += 1
    for r in cfg.get("raw_ranges", []):
        out.append((int(r["start"], 16), int(r["end"], 16), "instructions",
                    "raw_ranges: " + r["why"]))
    return {"coincidence": out, "pointer": []}


def record_provider(rom, cfg, segs):
    """Value fields of the records a pointer_tables entry describes.

    The consumer's struct lists the pointer fields of a record (the entry's
    "pointers", or its "targets" layout with a "size"); every other word
    of the record is a number to that consumer."""
    out = []
    ptr = []
    notp = dict((int(a, 16), r) for a, r in cfg.get("not_pointers", {}).items())
    for a, r in notp.items():
        out.append((a, a + 4, "value field", "not_pointers: " + r))

    def fields(base, size, offs, why):
        for o in range(0, size - 3, 4):
            if o in offs and base + o not in notp:
                ptr.append((base + o, why))
            elif o not in offs:
                out.append((base + o, base + o + 4, "value field", why))

    for t in cfg.get("pointer_tables", []):
        start = int(str(t["start"]), 0)
        stride = int(str(t.get("stride", 4)), 0)
        offs = set(int(str(o), 0) for o in t.get("pointers", ["0x0"]))
        if "count" in t:
            count = int(str(t["count"]), 0)
        elif "end" in t:
            count = (int(str(t["end"]), 0) - start) // stride
        else:
            count = 0  # next-label: plain pointer arrays, no value fields
        for k in range(count):
            fields(start + k * stride, stride, offs, t["why"])
        tg = t.get("targets")
        if tg and "size" in tg:
            size = int(str(tg["size"]), 0)
            toffs = set(int(str(o), 0) for o in tg.get("pointers", []))
            for k in range(count):
                for o in offs:
                    a = start + k * stride + o
                    v = struct.unpack_from("<I", rom, a - ROM_BASE)[0]
                    if v:
                        fields(v, size, toffs, tg["why"])
    return {"coincidence": out, "pointer": ptr}


def load_providers(tools_dir):
    mods = []
    for path in sorted(glob.glob(os.path.join(tools_dir, "census_*.py"))):
        name = os.path.splitext(os.path.basename(path))[0]
        spec = importlib.util.spec_from_file_location(name, path)
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        mods.append((name, mod))
    return mods


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--config", default="tools/split_config.json")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--elf", default="build/knidl.elf")
    ap.add_argument("--shift", default="build/shifttest.json",
                    help="shiftcheck.py's --json output")
    ap.add_argument("--unknown", help="write the unknown words here (JSON)")
    ap.add_argument("--by-target", action="store_true",
                    help="also count the unproven words by the segment they "
                         "point into")
    args = ap.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    with open(args.config) as f:
        cfg = json.load(f)
    segs = parse_segments(args.segments)
    starts = [s for s, _e, _k, _n in segs]

    def seg_of(a):
        i = bisect.bisect_right(starts, a) - 1
        return segs[i] if i >= 0 and a < segs[i][1] else None

    with open(args.shift) as f:
        shift = json.load(f)
    words = [(int(a, 16), int(v, 16)) for a, v in shift["unrelocated"]]

    results = [("code", code_provider(rom, cfg, segs, args.elf)),
               ("records", record_provider(rom, cfg, segs))]
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    for name, mod in load_providers(tools_dir):
        results.append((name, mod.provide(rom, cfg, segs)))

    # proven coincidences and next-label-extent ones in separate unions, so
    # a heuristic range never hides the kind of a proven one it overlaps
    coin = Ranges([c for _n, r in results for c in r["coincidence"]
                   if not c[2].endswith("-nextlabel")])
    heur = Ranges([c for _n, r in results for c in r["coincidence"]
                   if c[2].endswith("-nextlabel")])
    proven = {}
    for name, r in results:
        for a, why in r["pointer"]:
            proven.setdefault(a, "%s: %s" % (name, why))
    conflicts = [a for a in proven if coin.covering(a, a + 4)
                 and coin.covering(a, a + 4)[0] != "value field"]
    if conflicts:
        for a in sorted(conflicts)[:20]:
            sys.stderr.write("conflict: 0x%08X is a proven pointer (%s) and a "
                             "coincidence (%s)\n"
                             % (a, proven[a], coin.covering(a, a + 4)[1]))
        sys.exit("error: %d word(s) are both" % len(conflicts))

    rows = {}
    order = []
    unknown = []
    targets = {}
    kinds_total = {}
    for a, v in words:
        seg = seg_of(a)
        key = "(code segments)" if seg and seg[2] in CODE_KINDS else (
            seg[3] if seg else "?")
        if key not in rows:
            rows[key] = {"total": 0, "pointer": 0, "coincidence": 0,
                         "unknown": 0, "heuristic": 0,
                         "start": seg[0] if seg else 0}
            order.append(key)
        row = rows[key]
        row["total"] += 1
        if a in proven:
            cls = "pointer"
        else:
            c = coin.covering(a, a + 4)
            if c:
                cls = "coincidence"
                kinds_total[c[0]] = kinds_total.get(c[0], 0) + 1
            else:
                # a value table whose element type is proven but whose
                # extent is only the span to the next label (docs/data.md
                # 5.1) is counted apart and not treated as proven
                cls = "unknown"
                if heur.covering(a, a + 4):
                    row["heuristic"] += 1
                unknown.append(("0x%08X" % a, "0x%08X" % v))
        row[cls] += 1
        if cls != "coincidence":
            t = seg_of(v & ~1)
            tkey = "(code segments)" if t and t[2] in CODE_KINDS else (
                t[3] if t else "?")
            targets.setdefault(tkey, [0, 0, t[0] if t else 0])
            targets[tkey][0 if cls == "pointer" else 1] += 1

    print("pointer census of the %d unrelocated words (insertion at %s):"
          % (len(words), shift.get("lowest", "?")))
    print()
    print("%-46s %8s %8s %11s %8s %9s"
          % ("holding segment", "words", "pointer", "coincidence", "unknown",
             "(heur.)"))
    tot = {"total": 0, "pointer": 0, "coincidence": 0, "unknown": 0,
           "heuristic": 0}
    for key in sorted(order, key=lambda k: rows[k]["start"]):
        r = rows[key]
        for k in tot:
            tot[k] += r[k]
        print("%-46s %8d %8d %11d %8d %9s"
              % (key, r["total"], r["pointer"], r["coincidence"], r["unknown"],
                 r["heuristic"] or ""))
    print("%-46s %8d %8d %11d %8d %9d"
          % ("total", tot["total"], tot["pointer"], tot["coincidence"],
             tot["unknown"], tot["heuristic"]))
    print("(heur.): unknown words inside a value table whose extent is only "
          "the span to the next label")
    print()
    print("coincidences by evidence: " + ", ".join(
        "%s %d" % (k, n) for k, n in sorted(kinds_total.items(),
                                            key=lambda kv: -kv[1])))
    print("providers: " + ", ".join(n for n, _r in results))
    fmt = [t for t in cfg.get("pointer_tables", []) if t.get("proof") == "format"]
    if fmt:
        print("(%d pointer table(s) are symbolized on format-only evidence, "
              "no consumer: docs/data.md 5.3; make datastats counts their "
              "words)" % len(fmt))
    if args.by_target:
        print()
        print("proven-pointer and unknown words by the segment they point "
              "into (what keeps it from being proven movable):")
        print("%-46s %8s %8s" % ("target segment", "pointer", "unknown"))
        for key, (p, u, _s) in sorted(targets.items(), key=lambda kv: kv[1][2]):
            print("%-46s %8d %8d" % (key, p, u))
    # An insertion at P is proven safe when no proven-pointer or unknown
    # word points at or after P (each such word would keep its old value).
    unproven = sorted(int(v, 16) & ~1 for _a, v in unknown)
    unproven += sorted((v & ~1) for a, v in words if a in proven)
    unproven.sort()
    print()
    print("insertion points: proven-pointer or unknown words that point at "
          "or after each segment start")
    safe = None
    for st, _e, kind, name in reversed(segs):
        n = len(unproven) - bisect.bisect_left(unproven, st)
        if n == 0:
            safe = (st, name)
    for st, _e, kind, name in segs:
        if kind in CODE_KINDS and name not in ("agb_init",):
            continue
        n = len(unproven) - bisect.bisect_left(unproven, st)
        print("  0x%08X %-44s %8d" % (st, name, n))
    if safe:
        print("lowest proven-safe insertion point: 0x%08X (.%s)" % safe)
    else:
        print("no proven-safe insertion point before the end of the ROM")
    if args.unknown:
        with open(args.unknown, "w") as f:
            json.dump(unknown, f, indent=0)
            f.write("\n")


if __name__ == "__main__":
    main()
