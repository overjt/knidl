#!/usr/bin/env python3
"""ROM-wide function/symbol database generator (issue #22).

Emits one record per identified function plus a caller->callee call graph,
using the census heuristics validated in docs/analysis/rom-map.md section 1
(dual-view disassembly technique, re-implemented here as direct Thumb/ARM
decoding of baserom.gba so the generator is deterministic and fast):

  - BL-target census: every Thumb `bl` pair whose site lies in the code span
    0x080000C0-0x080CFFFF contributes its target as a function entry; the
    ARM `bl` census covers only the arm_code segments (rom-map.md section 3).
  - ROM-pointer census: every word-aligned word in the ROM whose value points
    into the code span (bit 0 set = Thumb entry) contributes a function entry.
  - Prologue plausibility: a candidate must start with `push {.., lr}`, be an
    immediate terminator (bx rN / mov pc, lr / pop {.., pc} / b), or (BL
    targets only) reach a terminator in a short forward sweep.

Outputs (deterministic, sorted; commit the results):
  docs/analysis/symbols.csv   vma,size,isa,evidence,name
  docs/analysis/callgraph.csv caller,callee,kind,site,count

`size` is the entry->next-entry distance capped at 4 KiB (as in rom-map.md
section 3): an upper bound that includes trailing literal pools / padding.

Run inside the knidl-builder image via `make symbols`, or directly:
  python3 tools/symdb.py --rom baserom.gba
"""

import argparse
import bisect
import os
import struct
import sys

ROM_BASE = 0x08000000

# Code span used by the BL/pointer census (rom-map.md section 3: restricted to
# 0x080000C0-0x080CFFFF; rounded up to the 0x080D0000 code|data boundary).
CODE_SPAN_START = 0x080000C0
CODE_SPAN_END = 0x080D0000

# Entry->next-entry size cap (rom-map.md section 3).
MAX_SIZE = 0x1000

# Forward-sweep budget for the prologue plausibility check of BL targets
# (must reach past __divsi3's first unconditional branch at +0x30).
SWEEP_LIMIT = 0x100

# ARM-mode functions. The three ARM zones of rom-map.md section 3
# (crt0+master ISR, task-switch helpers, interworking veneer) are split by
# hand-validated boundaries; entry evidence (bl-target / rom-pointer) is
# still derived automatically from the censuses. Names marked None stay
# sub_XXXXXXXX until they are identified from sibling projects.
ARM_ENTRIES = [
    (0x080000C0, 0x48, "Start"),  # asm/crt0.s (ROM entry via header branch)
    (0x08000108, 0x108, "MasterIsr"),  # asm/crt0.s (copied to 0x03001030)
    (0x08000234, 0x24, None),  # task/context-switch helper 1 (stmfd sp!,{lr})
    (0x08000258, 0x30, None),  # helper 2: save sp/lr/r0, restore task sp
    (0x08000288, 0x20, None),  # helper 3: call task fn via ARM veneer
    (0x080002A8, 0x40, None),  # helper 4: task-done check + hang loop
    (0x080CFDDC, 0x08, None),  # ARM interworking veneer -> 0x08005655
]

# Canonical names, all previously validated in this repo (asm/crt0.s,
# src/agb_sram.c, asm/sdk_libc.s) or canonical SDK/BIOS-syscall names from
# sibling pret projects (katam / pokeemerald; SWI numbering per GBATEK).
KNOWN_SYMBOLS = {
    0x08000310: "AgbInit",  # rom-map.md section 2 seg 5 (crt0 literal 0x08000311)
    0x08007300: "AgbMain",  # rom-map.md section 4 (crt0 literal 0x08007301); the
    # ROM has no __gccmain call in its prologue, so the original source did
    # NOT name it `main` — AgbMain per SDK/pret convention (issue #33).
    # SDK SWI thunks (rom-map.md section 2 seg 8 / GBATEK SWI numbering)
    0x080CFA4C: "DummyFunc",  # bare `bx lr` (m4a-style no-op)
    0x080CFA50: "ArcTan2",  # svc 0x0A
    0x080CFA54: "CpuFastSet",  # svc 0x0C
    0x080CFA58: "CpuSet",  # svc 0x0B
    0x080CFA5C: "Div",  # svc 0x06
    0x080CFA60: "Mod",  # svc 0x06 + mov r0,r1 (returns remainder; syscall.h)
    0x080CFA68: "HuffUnComp",  # svc 0x13 (SDK order; verified insn bytes, #29)
    0x080CFA6C: "LZ77UnCompVram",  # svc 0x12
    0x080CFA70: "LZ77UnCompWram",  # svc 0x11
    0x080CFA74: "MultiBoot",  # r1=1; svc 0x25
    0x080CFA7C: "SoundDriverVSyncOff",  # svc 0x28
    # m4a/mp2k XCMD (extended command 0xCD) handlers, issue #29.  Evidence:
    # the 12-entry Thumb-pointer table at 0x0860A3E8 matches gXcmdTable of
    # katam (src/m4a_tables.c) / pokeemerald one-for-one — ply_xxx fills
    # indices 0 AND 3 in both — and each handler stores its argument byte
    # to exactly the MusicPlayerTrack field its name implies
    # (m4a_internal.h offsets: pseudoEchoVolume 0x1E, pseudoEchoLength
    # 0x1F, instrument.type 0x24, .length 0x26, .pan_sweep 0x27, .wav
    # 0x28, .attack 0x2C, .decay 0x2D, .sustain 0x2E, .release 0x2F;
    # track->cmdPtr 0x40).  None is ever BL-called (dispatched through the
    # table only), and most start with `ldr r0, [r1, #0x40]`, so the
    # strict pointer-candidate prologue filter would reject them — the
    # KNOWN_SYMBOLS bypass in build() accepts curated entries instead.
    0x080CF94C: "ply_xxx",    # xcmd 0x00/0x03: gMPlayJumpTable dispatch
    0x080CF960: "ply_xwave",  # xcmd 0x01: assemble instrument.wav pointer
    0x080CF9A8: "ply_xtype",  # xcmd 0x02: instrument.type
    0x080CF9BC: "ply_xatta",  # xcmd 0x04: instrument.attack
    0x080CF9D0: "ply_xdeca",  # xcmd 0x05: instrument.decay
    0x080CF9E4: "ply_xsust",  # xcmd 0x06: instrument.sustain
    0x080CF9F8: "ply_xrele",  # xcmd 0x07: instrument.release
    0x080CFA0C: "ply_xiecv",  # xcmd 0x08: pseudoEchoVolume
    0x080CFA18: "ply_xiecl",  # xcmd 0x09: pseudoEchoLength
    0x080CFA24: "ply_xleng",  # xcmd 0x0A: instrument.length
    0x080CFA38: "ply_xswee",  # xcmd 0x0B: instrument.pan_sweep.  Its tail
    # (0x080CFA40-0x080CFA4B) was the former sdk_swi_wrappers
    # gUnk_080cfa40: the old 0x080CFA40 segment boundary cut this handler
    # in half; issue #29 moved it to 0x080CFA4C.
    # SRAM driver (decompiled in src/agb_sram.c, issue #8)
    0x080CFA9C: "ReadSram_Core",
    0x080CFAC0: "ReadSram",
    0x080CFB24: "WriteSram",
    0x080CFB64: "VerifySram_Core",
    0x080CFB94: "VerifySram",
    0x080CFBF8: "WriteSramEx",
    # SDK libc (asm/sdk_libc.s; agbcc libgcc division helpers)
    0x080CFC30: "_call_via_r0",
    0x080CFC34: "_call_via_r1",
    0x080CFC38: "_call_via_r2",
    0x080CFC3C: "_call_via_r3",
    0x080CFC40: "_call_via_r4",
    0x080CFC44: "_call_via_r5",
    0x080CFC48: "_call_via_r6",
    0x080CFC4C: "_call_via_r7",
    0x080CFC6C: "__divsi3",
    0x080CFD00: "_div0",
    0x080CFD04: "__umodsi3",
}

EVIDENCE_KINDS = ("bl-target", "rom-pointer", "prologue-scan")


def u16(rom, off):
    return struct.unpack_from("<H", rom, off)[0]


def u32(rom, off):
    return struct.unpack_from("<I", rom, off)[0]


def push_lr(hw):
    """Thumb `push {rList, lr}` (0xB500-0xB5FF)."""
    return 0xB500 <= hw <= 0xB5FF


def thumb_terminator(hw):
    """Thumb instructions that unconditionally end a function body:
    bx rN (incl. bx lr / bx pc), mov pc, lr, pop {.., pc}, b (uncond.)."""
    if hw & 0xFF87 == 0x4700:  # bx rN
        return True
    if hw == 0x46F7:  # mov pc, lr
        return True
    if hw & 0xFF00 == 0xBD00:  # pop {.., pc}
        return True
    if hw & 0xF800 == 0xE000:  # b (unconditional; cond fields 0xDxxx excluded)
        return True
    return False


def parse_segments(path):
    """Return [(start, end, kind, name)] from docs/analysis/segments.txt."""
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            segs.append((int(parts[0], 16), int(parts[1], 16), parts[2], parts[3]))
    return segs


def bl_census(rom, arm_ranges):
    """Scan for BL instructions.

    Thumb: sources restricted to the code span (rom-map.md section 3); the
    BL prefix/suffix pair is decoded directly. ARM: only the arm_code
    segments are scanned — decoding the Thumb-dominated rest of the ROM in
    ARM view yields huge amounts of bogus `bl` words (rom-map.md section 3
    reports exactly one ARM bl in the whole ROM, at 0x08000290).

    Returns [(site, target)] with absolute VMAs; targets may lie outside the
    census span and are filtered by the caller.
    """
    edges = []
    end = min(len(rom), CODE_SPAN_END - ROM_BASE) - 4
    for off in range(CODE_SPAN_START - ROM_BASE, end, 2):
        hw1 = u16(rom, off)
        if not 0xF000 <= hw1 <= 0xF7FF:  # BL prefix (11110 offset_high[10:0])
            continue
        hw2 = u16(rom, off + 2)
        # BL suffix: bits [15:11] = 11111 (0xF800-0xFFFF). The offset field is
        # hw1[10:0]:hw2[10:0]:0 (23 bits, sign at bit 22); BLX(2) suffixes
        # (0xE800-0xEFFF) do not exist on ARMv4T and are excluded.
        if not 0xF800 <= hw2 <= 0xFFFF:
            continue
        offset = ((hw1 & 0x7FF) << 12) | ((hw2 & 0x7FF) << 1)
        if offset & 0x400000:  # sign-extend from bit 22
            offset -= 0x800000
        target = ROM_BASE + off + 4 + offset
        edges.append((ROM_BASE + off, target))

    for start, stop in arm_ranges:
        for off in range(start - ROM_BASE, stop - ROM_BASE - 3, 4):
            w = u32(rom, off)
            if (w & 0x0F000000) != 0x0B000000 or (w & 0xF0000000) == 0xF0000000:
                continue
            imm = w & 0xFFFFFF
            if imm & 0x800000:
                imm -= 0x1000000
            edges.append((ROM_BASE + off, ROM_BASE + off + 8 + (imm << 2)))
    return edges


def pointer_census(rom, arm_ranges):
    """Scan every word-aligned word for pointers into the code span.

    Odd pointers are Thumb entry candidates; even pointers are accepted only
    when they land exactly on an ARM zone AND the referencing word itself
    sits inside the code span (ARM entries here are only referenced from
    code literal pools; even words in the far data zones matching ARM
    addresses are coincidences). Returns [(site, target, is_thumb_ptr)].
    """
    refs = []
    for off in range(0, len(rom) - 3, 4):
        w = u32(rom, off)
        if not CODE_SPAN_START <= w < CODE_SPAN_END:
            continue
        if w & 1:
            refs.append((ROM_BASE + off, w & ~1, True))
        elif (
            CODE_SPAN_START <= ROM_BASE + off < CODE_SPAN_END
            and any(start <= w < end for start, end in arm_ranges)
        ):
            refs.append((ROM_BASE + off, w, False))
    return refs


def plausible_thumb_entry(rom, vma, hard_end, strict):
    """Prologue plausibility check for a Thumb candidate at `vma` (a file
    offset relative to ROM_BASE). `hard_end` stops the sweep at the next
    known candidate entry.

    strict (pointer-only candidates): require `push {.., lr}` or an immediate
    terminator (`bx rN` thunk / `bx lr` leaf such as the default IRQ handler
    / `bx pc` veneer) — odd pointers into the rodata interleaved with the
    code are common and only instruction-shaped entries keep precision.
    non-strict (BL targets): the call itself is strong evidence, so any
    body reaching an unconditional terminator within SWEEP_LIMIT passes.
    """
    off = vma - ROM_BASE
    hw0 = u16(rom, off)
    if push_lr(hw0) or thumb_terminator(hw0):
        return True
    if strict:
        return False
    limit = min(off + SWEEP_LIMIT, hard_end, len(rom) - 2)
    off += 2
    while off < limit:
        if thumb_terminator(u16(rom, off)):
            return True
        off += 2
    return False


def build(rom, segments):
    arm_ranges = [(s, e) for s, e, kind, _ in segments if kind == "arm_code"]

    bl_edges = bl_census(rom, arm_ranges)
    ptr_refs = pointer_census(rom, arm_ranges)

    bl_targets = {}
    for site, target in bl_edges:
        bl_targets.setdefault(target, []).append(site)
    # Pointer targets, split by the interworking bit: bit 0 set references
    # Thumb entries, bit 0 clear references ARM entries. A mismatched parity
    # (e.g. a coincidental odd data word equal to an ARM entry address) is
    # ignored for both evidence and call-graph edges.
    ptr_targets = {}
    for site, target, is_thumb_ptr in ptr_refs:
        if not is_thumb_ptr:
            continue
        ptr_targets.setdefault(target, []).append(site)
    ptr_targets_arm = {}
    for site, target, is_thumb_ptr in ptr_refs:
        if is_thumb_ptr:
            continue
        ptr_targets_arm.setdefault(target, []).append(site)

    # Thumb candidates: BL targets union bit0-set pointer targets, restricted
    # to the code span, excluding the ARM zones, validated by prologue shape.
    candidates = {}
    for target in set(bl_targets) | set(ptr_targets):
        if not CODE_SPAN_START <= target < CODE_SPAN_END:
            continue
        if target & 1 or any(s <= target < e for s, e in arm_ranges):
            continue
        if target - ROM_BASE + 1 >= len(rom):
            continue
        candidates[target] = None
    order = sorted(candidates)
    for i, target in enumerate(order):
        nxt = order[i + 1] if i + 1 < len(order) else CODE_SPAN_END
        strict = target not in bl_targets
        # Curated identifications (KNOWN_SYMBOLS) are accepted directly:
        # the m4a XCMD handlers are table-dispatched only and open with
        # `ldr r0, [r1, #0x40]`, which no generic prologue filter admits.
        if target in KNOWN_SYMBOLS or plausible_thumb_entry(
            rom, target, nxt - ROM_BASE, strict
        ):
            candidates[target] = nxt
    thumb_entries = {t: n for t, n in candidates.items() if n is not None}

    # ARM entries: curated boundaries; evidence merged from the censuses.
    arm_entries = {}
    for vma, size, _ in ARM_ENTRIES:
        arm_entries[vma] = vma + size

    # Boundaries for every accepted entry, used for size computation and for
    # attributing call-graph sites to the containing function. Sizes run to
    # the next ACCEPTED entry (rejected candidates must not truncate sizes).
    all_entries = sorted(list(thumb_entries) + list(arm_entries))
    sizes = {}
    for i, vma in enumerate(all_entries):
        nxt = all_entries[i + 1] if i + 1 < len(all_entries) else CODE_SPAN_END
        if vma in arm_entries:
            end = arm_entries[vma]
        else:
            end = min(nxt, vma + MAX_SIZE)
        sizes[vma] = max(0, min(end, vma + MAX_SIZE) - vma)

    def containing(site):
        i = bisect.bisect_right(all_entries, site) - 1
        if i < 0:
            return None
        vma = all_entries[i]
        return vma if site < vma + sizes[vma] else None

    # ---- symbol records ----------------------------------------------------
    symbols = []
    for vma in all_entries:
        is_arm = vma in arm_entries
        ev = []
        if vma in bl_targets:
            ev.append("bl-target")
        if vma in (ptr_targets_arm if is_arm else ptr_targets):
            ev.append("rom-pointer")
        if not ev:
            ev.append("prologue-scan")
        name = KNOWN_SYMBOLS.get(vma)
        if is_arm:
            name = dict((a, n) for a, s, n in ARM_ENTRIES).get(vma, name)
        if name is None:
            name = "sub_%08x" % vma
        isa = "arm" if is_arm else "thumb"
        symbols.append((vma, sizes[vma], isa, "+".join(ev), name))

    # ---- call graph --------------------------------------------------------
    edges = {}

    def add_edge(caller, callee, kind, site):
        key = (caller or 0, callee, kind)
        if key not in edges:
            edges[key] = [site, 0]
        edges[key][1] += 1

    for site, target in bl_edges:
        if target not in sizes:
            continue
        add_edge(containing(site), target, "bl", site)
    for site, target, is_thumb_ptr in ptr_refs:
        if target not in sizes:
            continue
        if is_thumb_ptr == (target in arm_entries):
            continue  # interworking-bit / ISA mismatch: coincidence
        add_edge(containing(site), target, "ptr", site)

    callgraph = [
        (caller, callee, kind, site, count)
        for (caller, callee, kind), (site, count) in sorted(
            edges.items(), key=lambda kv: (kv[0][2], kv[0][0] or 0, kv[0][1])
        )
    ]
    return symbols, callgraph, thumb_entries, arm_entries


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", default="baserom.gba")
    parser.add_argument(
        "--segments", default="docs/analysis/segments.txt",
        help="machine-readable segment table (kinds arm_code etc.)",
    )
    parser.add_argument("--out-dir", default="docs/analysis")
    args = parser.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    if len(rom) & 3:
        sys.exit("error: ROM size is not word-aligned")

    segments = parse_segments(args.segments)

    symbols, callgraph, thumb_entries, arm_entries = build(rom, segments)

    sym_path = os.path.join(args.out_dir, "symbols.csv")
    with open(sym_path, "w") as f:
        f.write("vma,size,isa,evidence,name\n")
        for vma, size, isa, ev, name in symbols:
            f.write("0x%08X,0x%X,%s,%s,%s\n" % (vma, size, isa, ev, name))

    graph_path = os.path.join(args.out_dir, "callgraph.csv")
    with open(graph_path, "w") as f:
        f.write("caller,callee,kind,site,count\n")
        for caller, callee, kind, site, count in callgraph:
            f.write(
                "0x%08X,0x%08X,%s,0x%08X,%d\n" % (caller, callee, kind, site, count)
            )

    def count_ev(pred):
        return sum(1 for _, _, _, ev, _ in symbols if pred(ev))

    print("wrote %s (%d functions: %d thumb, %d arm)" % (
        sym_path, len(symbols), len(thumb_entries), len(arm_entries)))
    print("  evidence: %d bl-target, %d rom-pointer, %d prologue-scan" % (
        count_ev(lambda e: "bl-target" in e),
        count_ev(lambda e: "bl-target" not in e and "rom-pointer" in e),
        count_ev(lambda e: e == "prologue-scan")))
    print("wrote %s (%d edges: %d bl, %d ptr)" % (
        graph_path, len(callgraph),
        sum(1 for e in callgraph if e[2] == "bl"),
        sum(1 for e in callgraph if e[2] == "ptr")))


if __name__ == "__main__":
    main()
