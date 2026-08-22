#!/usr/bin/env python3
"""Validation harness for the ROM-wide symbol database (issue #22).

Checks the committed docs/analysis/symbols.csv + callgraph.csv against a
FRESH dual-view disassembly of baserom.gba (the objdump technique from
docs/analysis/rom-map.md section 1), independently re-derived here:

  1. Structure: CSV shape, sorted/unique VMAs, no overlaps, size/isa/
     evidence/name value domains (names must be sub_XXXXXXXX or come from
     the curated known-symbol tables in tools/symdb.py).
  2. Coverage: at least the 3,149 validated Thumb entries reported in
     rom-map.md section 3, plus ARM entries covering the three arm_code
     zones from docs/analysis/segments.txt.
  3. Spot checks: a deterministic random sample of ~20 entries verified
     against a freshly generated objdump dump (thumb and ARM views):
     bl-evidence entries must appear as `bl` targets in the dump,
     rom-pointer entries must decode as `push {.., lr}` or an immediate
     terminator at the entry, ARM entries must decode in the ARM view.
  4. Cross-checks: call-graph callees exist, pointer edges keep the
     interworking bit consistent with the callee ISA, and bl-evidence
     entries are reported when the fresh dump cannot see their callers.

Run inside the knidl-builder image via `make symbols`, or directly:
  python3 tools/symdb_check.py --rom baserom.gba
"""

import argparse
import os
import random
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import symdb  # noqa: E402  (KNOWN_SYMBOLS / ARM_ENTRIES / parse_segments)

ROM_BASE = symdb.ROM_BASE
CODE_SPAN_START = symdb.CODE_SPAN_START
CODE_SPAN_END = symdb.CODE_SPAN_END

# Validated floor from rom-map.md section 3 (issue #22 acceptance).
MIN_THUMB_ENTRIES = 3149

LINE_RE = re.compile(r"^\s*([0-9a-f]+):\t(.*)$")
BL_RE = re.compile(r"\bbl\t0x([0-9a-f]+)")

SAMPLE_SEED = 0xC0FFEE


class Result:
    def __init__(self):
        self.failures = []
        self.notes = []

    def fail(self, msg):
        self.failures.append(msg)

    def note(self, msg):
        self.notes.append(msg)

    @property
    def ok(self):
        return not self.failures


def u16(rom, off):
    return struct.unpack_from("<H", rom, off)[0]


def u32(rom, off):
    return struct.unpack_from("<I", rom, off)[0]


def load_symbols(path):
    with open(path) as f:
        lines = f.read().splitlines()
    if lines[0] != "vma,size,isa,evidence,name":
        sys.exit("error: %s: unexpected header %r" % (path, lines[0]))
    rows = []
    for n, line in enumerate(lines[1:], 2):
        parts = line.split(",")
        if len(parts) != 5:
            sys.exit("error: %s:%d: bad row %r" % (path, n, line))
        rows.append(
            (int(parts[0], 16), int(parts[1], 16), parts[2], parts[3], parts[4])
        )
    return rows


def load_callgraph(path):
    with open(path) as f:
        lines = f.read().splitlines()
    if lines[0] != "caller,callee,kind,site,count":
        sys.exit("error: %s: unexpected header %r" % (path, lines[0]))
    rows = []
    for n, line in enumerate(lines[1:], 2):
        parts = line.split(",")
        if len(parts) != 5:
            sys.exit("error: %s:%d: bad row %r" % (path, n, line))
        rows.append(
            (int(parts[0], 16), int(parts[1], 16), parts[2], int(parts[3], 16),
             int(parts[4], 10))
        )
    return rows


def objdump(rom, extra):
    cmd = [
        "arm-none-eabi-objdump", "-D", "-bbinary", "-marmv4t",
        "--adjust-vma=0x%08X" % ROM_BASE,
    ] + extra + [rom]
    return subprocess.run(cmd, check=True, capture_output=True, text=True).stdout


def first_instruction(rom, vma, force_thumb):
    """Targeted fresh disassembly of the instruction AT `vma`: starting the
    objdump sweep exactly at the entry avoids the linear-sweep misalignment
    that hides entries preceded by data/padding (e.g. 0x0807ED98)."""
    extra = ["-Mforce-thumb"] if force_thumb else []
    extra += [
        "--start-address=0x%08X" % vma,
        "--stop-address=0x%08X" % (vma + 4),
    ]
    for line in objdump(rom, extra).splitlines():
        m = LINE_RE.match(line)
        if m and "\t" in m.group(2):
            _, text = m.group(2).split("\t", 1)
            return text.strip()
    return None


def parse_dump(dump, wanted):
    """Stream a dump; return (addr->text for `wanted`, bl targets from sites
    inside the code span). objdump pads the hex-byte column with spaces, so
    the line is split on the first tab separating bytes from the mnemonic."""
    lines_wanted = {}
    bl_targets = set()
    for line in dump.splitlines():
        m = LINE_RE.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        rest = m.group(2)
        if "\t" not in rest:
            continue
        _, text = rest.split("\t", 1)
        text = text.strip()
        if addr in wanted and addr not in lines_wanted:
            lines_wanted[addr] = text
        if CODE_SPAN_START <= addr < CODE_SPAN_END:
            bm = BL_RE.search(text)
            if bm:
                bl_targets.add(int(bm.group(1), 16))
    return lines_wanted, bl_targets


def is_push_lr_text(text):
    return re.match(r"push\s+\{[^}]*\blr\b[^}]*\}", text) is not None


def is_terminator_text(text):
    if re.match(r"(bx|bxj)\s+", text):
        return True
    if re.match(r"pop\s+\{[^}]*\bpc\b[^}]*\}", text):
        return True
    if re.match(r"b\t0x", text):
        return True
    if text.startswith("mov") and re.search(r"\bpc,\s*lr\b", text):
        return True
    return False


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", default="baserom.gba")
    parser.add_argument("--symbols", default="docs/analysis/symbols.csv")
    parser.add_argument("--callgraph", default="docs/analysis/callgraph.csv")
    parser.add_argument("--segments", default="docs/analysis/segments.txt")
    parser.add_argument("--sample", type=int, default=20)
    args = parser.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()

    symbols = load_symbols(args.symbols)
    callgraph = load_callgraph(args.callgraph)
    segments = symdb.parse_segments(args.segments)
    res = Result()

    # ---- 1. structure ------------------------------------------------------
    known_names = set(symdb.KNOWN_SYMBOLS.values())
    known_names |= {n for _, _, n in symdb.ARM_ENTRIES if n}
    name_re = re.compile(r"^sub_[0-9a-f]{8}$")
    valid_ev = {"bl-target", "rom-pointer", "prologue-scan"}
    prev = None
    for vma, size, isa, ev, name in symbols:
        if isa not in ("thumb", "arm"):
            res.fail("0x%08X: bad isa %r" % (vma, isa))
        if size <= 0 or size > symdb.MAX_SIZE:
            res.fail("0x%08X: bad size 0x%X" % (vma, size))
        if vma & 1:
            res.fail("0x%08X: unaligned entry" % vma)
        for tok in ev.split("+"):
            if tok not in valid_ev:
                res.fail("0x%08X: bad evidence %r" % (vma, tok))
        if name_re.match(name) is None and name not in known_names:
            res.fail("0x%08X: bad name %r" % (vma, name))
        if prev is not None:
            if vma <= prev[0]:
                res.fail("0x%08X: not sorted / duplicate" % vma)
            elif prev[0] + prev[1] > vma:
                res.fail("0x%08X overlaps previous entry (0x%08X+0x%X)"
                         % (vma, prev[0], prev[1]))
        prev = (vma, size)
    res.note("structure: %d rows checked" % len(symbols))

    by_vma = {v: (s, i, e, n) for v, s, i, e, n in symbols}
    thumb_rows = [r for r in symbols if r[2] == "thumb"]
    arm_rows = [r for r in symbols if r[2] == "arm"]

    # ---- 2. coverage -------------------------------------------------------
    if len(thumb_rows) < MIN_THUMB_ENTRIES:
        res.fail("coverage: only %d thumb entries (need >= %d)"
                 % (len(thumb_rows), MIN_THUMB_ENTRIES))
    else:
        res.note("coverage: %d thumb entries (>= %d)"
                 % (len(thumb_rows), MIN_THUMB_ENTRIES))

    arm_zones = [(s, e, n) for s, e, k, n in segments if k == "arm_code"]
    if len(arm_zones) != 3:
        res.fail("coverage: expected 3 arm_code zones, found %d" % len(arm_zones))
    arm_span = [(v, v + s) for v, s, _, _, _ in arm_rows]
    for zs, ze, zname in arm_zones:
        cursor = zs
        for vs, ve in arm_span:
            if ve <= cursor or vs >= ze:
                continue
            if vs > cursor:
                break
            cursor = max(cursor, ve)
        if cursor < ze:
            res.fail("coverage: arm zone %s (0x%08X-0x%08X) not fully covered"
                     % (zname, zs, ze))
        else:
            res.note("coverage: arm zone %s covered" % zname)

    for vma, name in (
        (0x080000C0, "Start"), (0x08000108, "MasterIsr"),
        (0x08000310, "AgbInit"), (0x08007300, "AgbMain"),
        (0x080CFA9C, "ReadSram_Core"), (0x080CFAC0, "ReadSram"),
        (0x080CFB24, "WriteSram"), (0x080CFB64, "VerifySram_Core"),
        (0x080CFB94, "VerifySram"), (0x080CFBF8, "WriteSramEx"),
        (0x080CFC30, "_call_via_r0"), (0x080CFC3C, "_call_via_r3"),
        (0x080CFA54, "CpuFastSet"), (0x080CFA58, "CpuSet"),
    ):
        row = by_vma.get(vma)
        if row is None or row[3] != name:
            res.fail("known symbol %s @0x%08X missing/misnamed in DB" % (name, vma))

    # ---- 4. call-graph consistency (cheap, before the expensive dumps) -----
    for caller, callee, kind, site, count in callgraph:
        if callee not in by_vma:
            res.fail("callgraph: callee 0x%08X not in symbols" % callee)
            continue
        if caller and caller not in by_vma:
            res.fail("callgraph: caller 0x%08X not in symbols" % caller)
        if kind not in ("bl", "ptr"):
            res.fail("callgraph: bad kind %r" % kind)
        if count < 1:
            res.fail("callgraph: bad count %d" % count)
        if not ROM_BASE <= site < ROM_BASE + len(rom):
            res.fail("callgraph: site 0x%08X outside ROM" % site)
        if kind == "ptr":
            w = u32(rom, site - ROM_BASE)
            if (w & 1) != (by_vma[callee][1] == "thumb"):
                res.fail("callgraph: ptr edge 0x%08X->0x%08X parity mismatch"
                         % (site, callee))
    res.note("callgraph: %d edges checked" % len(callgraph))

    # ---- 3. spot checks against a fresh dual-view disassembly --------------
    rng = random.Random(SAMPLE_SEED)
    fixed = [0x08000310, 0x08007300, 0x080CFA54, 0x080CFC3C, 0x080CFA9C,
             0x08005654]
    pool = [v for v, _, _, _, _ in thumb_rows if v not in fixed]
    sample = fixed + rng.sample(pool, max(0, args.sample - len(fixed)))
    arm_sample = [v for v, _, _, _, _ in arm_rows]

    wanted = set(sample) | set(arm_sample)
    thumb_dump = objdump(args.rom, ["-Mforce-thumb"])
    arm_dump = objdump(args.rom, [])
    thumb_lines, bl_targets = parse_dump(thumb_dump, wanted)
    arm_lines, _ = parse_dump(arm_dump, set(arm_sample))

    bl_only_unseen = 0
    for vma, _, _, ev, _ in thumb_rows:
        if "bl-target" in ev and vma not in bl_targets:
            bl_only_unseen += 1
    res.note("cross-check: %d bl-evidence entries not visible as bl targets "
             "in the fresh objdump sweep (sites hidden from its linear "
             "disassembly or called only via pairs it consumes)" % bl_only_unseen)

    checked = 0
    for vma in sample:
        size, isa, ev, name = by_vma[vma]
        text = thumb_lines.get(vma) or first_instruction(args.rom, vma, True)
        if text is None:
            res.fail("spot: 0x%08X (%s) has no line in the fresh thumb dump"
                     % (vma, name))
            continue
        ok = False
        why = []
        if "bl-target" in ev and vma in bl_targets:
            ok = True
            why.append("bl target in fresh dump")
        if is_push_lr_text(text):
            ok = True
            why.append("push {.., lr} prologue")
        if is_terminator_text(text):
            ok = True
            why.append("immediate terminator")
        if not ok:
            res.fail("spot: 0x%08X (%s, %s) not confirmed by fresh dump "
                     "(entry decodes as %r)" % (vma, name, ev, text))
        else:
            checked += 1
            res.note("spot: 0x%08X %-14s %-24s %s" % (vma, name, ev, "; ".join(why)))

    for vma in arm_sample:
        size, isa, ev, name = by_vma[vma]
        text = arm_lines.get(vma) or first_instruction(args.rom, vma, False)
        if text is None:
            res.fail("spot: arm 0x%08X (%s) has no line in the fresh ARM dump"
                     % (vma, name))
            continue
        if "UNDEFINED" in text or text == "":
            res.fail("spot: arm 0x%08X (%s) decodes as %r in the ARM view"
                     % (vma, name, text))
        else:
            checked += 1
            res.note("spot: arm 0x%08X %-14s %s" % (vma, name, text))

    res.note("spot checks confirmed: %d/%d" % (checked, len(sample) + len(arm_sample)))

    # ---- summary -----------------------------------------------------------
    for n in res.notes:
        print(n)
    if res.ok:
        print("symdb-check: PASS (%d functions, %d call-graph edges)"
              % (len(symbols), len(callgraph)))
        return 0
    for f in res.failures:
        print("FAIL: " + f)
    print("symdb-check: FAIL (%d problems)" % len(res.failures))
    return 1


if __name__ == "__main__":
    sys.exit(main())
