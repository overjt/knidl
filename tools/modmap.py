#!/usr/bin/env python3
"""Subsystem clustering over the bulk game code (issue #34).

Reads the committed symbol database (`docs/analysis/symbols.csv`), the call
graph (`docs/analysis/callgraph.csv`), the segment table
(`docs/analysis/segments.txt`) and `baserom.gba`, and partitions the bulk
Thumb code region into *contiguous* candidate modules plus the evidence that
justifies each one.  Output:

  * `docs/analysis/module-map.csv`  - machine-readable module table
                                      (regeneration is checked in CI)
  * `--report`                      - full per-module evidence dump, the input
                                      for the hand-written narrative in
                                      `docs/analysis/module-map.md`
  * `--markdown`                    - the module table as a markdown table,
                                      pasted verbatim into module-map.md

Why *contiguous* modules: the original ROM was linked from translation units,
so a module is an address range, and `tools/carve.py` (the tool that lands
decompiled code) only accepts address ranges.  A clustering that produced
address-interleaved sets would not be actionable.

Method (all thresholds are module-level constants, all output is
deterministic):

 1. **Nodes.** Every `symbols.csv` function inside the requested range.  Sizes
    run to the next entry, so the nodes tile the range exactly and coverage is
    100% by construction (asserted).

 2. **Edges.** `callgraph.csv` `bl` edges, with `caller == 0` repaired by
    interval containment (the last function entry at or below the call site;
    `symdb.py` leaves the caller blank when the site is past its 4 KiB size
    cap).  `ptr` edges are NOT unioned into the same graph: a pointer table can
    point anywhere.  They are used separately, as anchors (step 4).

 3. **Boundary cost.** For each gap between two adjacent functions, the number
    of *local* `bl` edges that straddle it (`|caller - callee| <= LOCAL_WINDOW`).
    Long-range calls are excluded on purpose: they are calls into shared
    engine-wide helpers and carry no locality information.  Cheap gaps are
    translation-unit seams: 850 of the 4949 gaps between adjacent functions
    in the bulk range are straddled by no local call at all.

 4. **Anchor tables.** Runs of consecutive ROM words holding Thumb function
    pointers (from the `ptr` edge sites).  A run whose targets span less than
    `TIGHT_SPAN` bytes is a state/behaviour table belonging to one module; a
    boundary that splits such a run is charged `TABLE_PENALTY`, which keeps
    table-dispatched families whole.

 5. **Segmentation.** Exact dynamic program: minimise the total boundary cost
    subject to every module's byte size lying in `[MIN, MAX]`.  Run twice -
    coarse (`--module-max`, the child-issue granularity) and fine
    (`--block-max`, suggested batches inside an issue), with the coarse
    boundaries constrained to be a subset of the fine ones.

 6. **Evidence.** Per module: literal-pool references (decoded from every
    `ldr rN, [pc, #imm]`, which gives exact pool words rather than a guess),
    grouped into I/O register blocks, IWRAM/EWRAM cells and far ROM data
    zones; anchor tables; cross-module call traffic; calls into the already
    decompiled early subsystems (named by role, see `EARLY_ROLES`); and a
    difficulty score.

Usage:
    python3 tools/modmap.py --rom baserom.gba
    python3 tools/modmap.py --rom baserom.gba --report -   # evidence dump
"""

import argparse
import bisect
import csv
import os
import struct
import sys
from collections import Counter, defaultdict

ROM_BASE = 0x08000000

# Bulk range from issue #34.  `main` (0x08007300) and the m4a engine
# (0x080CD89C+) are inside it but already landed; they are reported as fixed
# modules and never clustered (see `split_range`).
RANGE_START = 0x08007300
RANGE_END = 0x080CFA4C

# --- clustering thresholds ------------------------------------------------
LOCAL_WINDOW = 0x8000   # calls farther than this carry no locality signal
TIGHT_SPAN = 0x6000     # a pointer run spanning less than this is one module's
TABLE_PENALTY = 20      # cost of splitting such a run
MIN_TABLE_RUN = 4       # words; shorter runs are not treated as tables
MODULE_MAX = 0x8000     # coarse granularity: one child issue (~32 KiB)
MODULE_MIN = 0x3000
BLOCK_MAX = 0x2000      # fine granularity: one decompilation batch (~8 KiB)
BLOCK_MIN = 0x0800

# ROM task-type table (rom-map section 6): 8-byte entries
# `{u8 class; u8 pad[3]; u32 entry}`.  The second word is the task body's Thumb
# entry point, not a flag word: every one of the 266 entries points at an
# accepted function in the symbol DB, and the table stops dead at the first
# word that does not (`0x00130018` at 0x08730780).  This is the game's own
# index of gameplay subsystems - see `task_types()`.
TASK_TYPE_TABLE = 0x0872FF30

# Decompilation waves (issue #34 section 5 of module-map.md), keyed by module
# start VMA.  Wave 1 is the actor API (highest fan-in: it decides how struct
# Task is spelled); wave 2 the caller-less behaviour banks (cheap, parallel);
# wave 3 the support libraries; wave 4 the level/stage engine; wave 5 the
# modes, UI, save and effect modules reachable from AgbMain.
WAVES = {
    0x08062584: 1, 0x080692FC: 1,
    0x08070EC0: 2, 0x08078B68: 2, 0x0807F044: 2, 0x08082E68: 2, 0x080860F8: 2,
    0x0808CCE8: 2, 0x0809000C: 2, 0x08093F64: 2, 0x080988F8: 2, 0x0809BA44: 2,
    0x080A1590: 2, 0x080A5644: 2, 0x080AA338: 2, 0x080AE3BC: 2, 0x080BDA2C: 2,
    0x08010358: 3, 0x08017668: 3, 0x0801A8C8: 3, 0x0803CD60: 3, 0x0805AFAC: 3,
    0x08021B18: 4, 0x080296A0: 4, 0x08030804: 4, 0x08036280: 4, 0x080449C8: 4,
    0x08047FE8: 4, 0x0804CC7C: 4, 0x08053AF4: 4,
    0x080075B8: 5, 0x0800B920: 5, 0x080B2FE8: 5, 0x080B6154: 5, 0x080B9D0C: 5,
    0x080C1FFC: 5, 0x080C6420: 5,
}

WAVE_NAMES = {
    1: "actor API (do first: every module calls it)",
    2: "behaviour banks (cheap, no callers, parallelisable)",
    3: "support libraries",
    4: "level / stage engine",
    5: "modes, UI, save, effects",
}

# --- evidence classification ---------------------------------------------
# GBA I/O register groups (0x04000000+), by subsystem.
IO_GROUPS = (
    (0x000, 0x008, "LCD ctrl"),
    (0x008, 0x010, "BG ctrl"),
    (0x010, 0x020, "BG scroll"),
    (0x020, 0x040, "BG affine"),
    (0x040, 0x050, "window"),
    (0x050, 0x056, "blend/fade"),
    (0x060, 0x0A8, "sound"),
    (0x0B0, 0x0E0, "DMA"),
    (0x100, 0x110, "timer"),
    (0x120, 0x136, "SIO"),
    (0x130, 0x134, "keys"),
    (0x200, 0x210, "IRQ"),
)

# Roles of the already-decompiled early zone (0x080008E8-0x08007300), keyed by
# the `segments.txt` segment name.  A BL from the bulk range into one of these
# is direct evidence of what the calling module does.  Sources: rom-map.md
# section 2 seg 6 and the header comment of each src/early_*.c.
EARLY_ROLES = {
    "agb_init": "boot/init",
    "early_08e8": "palette fade",
    "early_0de4": "frame tick",
    "early_10cc": "vblank handler",
    "early_11ac": "OAM/palette flush + keys + copy queue",
    "early_1518": "VRAM transfer queue + sprite buckets",
    "early_1b08": "OAM shadow builder",
    "early_1fd0": "BG/fade/blend reset",
    "early_2668": "SIO handshake",
    "early_293c": "SIO handshake",
    "early_2b04": "frame driver + RNG + blend",
    "early_3110": "sound/BGM",
    "early_3484": "sound/SE",
    "early_3964": "SIO multiboot",
    "early_4000": "SIO multiboot",
    "early_4734": "SDK multiboot client",
    "early_4d6c": "SDK multiboot client",
    "early_4fec": "task engine",
    "early_5228": "task engine",
    "early_55b0": "task engine",
    "early_58e4": "task position/draw",
    "early_5c4c": "task position/draw",
    "early_5d9c": "sprite draw/update",
    "early_6464": "SIO multi-play",
    "early_6cd4": "SIO multi-play",
    "early_6d18": "SIO multi-play",
    "early_6e8c": "SIO multi-play",
    "early_7004": "SIO multi-play",
    "main": "main dispatch loop",
    "sram_driver": "SRAM save",
}

# Helpers outside the symbol DB's naming (labels live in asm/sdk_libc.s, whose
# addresses are implicit).  `_call_via_rN` is gcc's interworking shim: a BL to
# one of these is an indirect call through a function pointer, so a high count
# means vtable/behaviour-table-driven code.  The task trampolines are the
# cooperative-scheduler entry points: a module that calls TaskYieldTrampoline
# is made of task bodies (coroutines resumed once per frame).
NAMED_HELPERS = {}
for _i, _r in enumerate(("r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
                         "r9", "sl", "fp", "ip", "sp", "lr")):
    NAMED_HELPERS[0x080CFC30 + 4 * _i] = "_call_via_" + _r
NAMED_HELPERS[0x080CFC6C] = "__divsi3"
NAMED_HELPERS[0x080CFD00] = "_div0"
NAMED_HELPERS[0x080CFD04] = "__umodsi3"
NAMED_HELPERS[0x080CFDC4] = "TaskSwitchTrampoline"
NAMED_HELPERS[0x080CFDCC] = "TaskYieldTrampoline"
NAMED_HELPERS[0x080CFDD4] = "TaskDispatchTrampoline"

# Curated module names from the triage pass (issue #34), keyed by module start
# VMA so a re-run with unchanged thresholds keeps them.  The second field is
# the one-line justification that goes into the CSV and the markdown table;
# the full reasoning per module is in docs/analysis/module-map.md.  Names
# ending in "?" are candidate identifications, not established ones.
MODULE_NAMES = {
    0x080075B8: ("game mode + screen/asset loader",
                 "class-0 task types #0-2; game-state cells; LZ77UnCompVram x21, HuffUnComp x6; called from AgbMain"),
    0x0800B920: ("menu / UI task bank",
                 "22 class-4 task types #238-259; key auto-repeat + decimal digit buffer; called from AgbMain and the save module"),
    0x08010358: ("player-character state bodies?",
                 "64/67 functions pointer-dispatched from the 71-entry table @0x08731FA8 (shared with the next module); TaskYieldTrampoline x1340, the densest in the ROM; sprite draw x261"),
    0x08017668: ("player-character driver?",
                 "task type #3; 20/23 pointer-dispatched; called by every stage module, calls the bank above"),
    0x0801A8C8: ("terrain / collision query (pure leaf)",
                 "zero outgoing calls except ArcTan2 x3; 1303 pool refs into the 0x030054E0-0x030055B0 room descriptor; 334 into the 0x100-stride index tables @0x087328F0-0x087339F0"),
    0x08021B18: ("level / room builder + tilemap upload",
                 "writes BG map bases 0x06002000-0x06008000; CpuSet+CpuFastSet x25; 15 jump tables; EWRAM working set 0x02005000-0x0200B080; indexes 0x087E1D58; drives the next module x195"),
    0x080296A0: ("camera / BG scroll + tilemap streaming",
                 "all eight BGxHOFS/VOFS 16.16 shadows; 15 class-3 task types #221-236; task position/draw x18"),
    0x08030804: ("stage manager A",
                 "task type #5 (class 1); 17 jump tables; calls the stage support library x151 and the script runner x23"),
    0x08036280: ("stage script runner",
                 "21 jump tables; 35/41 pointer-dispatched; task engine x14; calls the stage support library x266"),
    0x0803CD60: ("stage support library",
                 "fan-in from every stage module (266+171+165+159+151); hot leaves 0x08040B40 x195 / 0x080413A4 x165; link-aware SE gate 0x0803E34C; 43-entry rodata table @0x0803EC48"),
    0x080449C8: ("large actor bank A",
                 "22 functions, 21 pointer-dispatched, mean 0x275; TaskYieldTrampoline x244; sprite draw x89"),
    0x08047FE8: ("large actor bank B",
                 "27 functions, 23 pointer-dispatched, mean 0x2c2; TaskYieldTrampoline x317; palette-fade calls; level_graphics_palettes refs x24"),
    0x0804CC7C: ("stage manager B",
                 "task type #6 (class 1); anchor tables @0x0873B664 (25) and @0x0873B77C (13); calls the stage support library x159"),
    0x08053AF4: ("link multiplayer mode",
                 "SIO multi-play (early_6464) x162 - by far the heaviest link user in the bulk; task type #7 (class 1); 49-entry anchor table @0x0873B928"),
    0x0805AFAC: ("actor / effect support library B",
                 "fan-in 76+41+27+25+18 from the stage modules; TaskYieldTrampoline x1052; touches each BG shadow once"),
    0x08062584: ("struct Task field API (actor core)",
                 "confirmed by #65: 244 functions over gUnk_03002490 (current task) and the 64-entry gUnk_03002790[] table - spawn/free, 16.16 position+velocity, ArcTan2 aiming, animation-script walking (struct AnimCmd), actor graphics upload, and the 116-byte per-player record gUnk_03002170[] that Task.unk88 points at; the two 0xa04/0x710 leaders are straight-line cutscene bodies"),
    0x080692FC: ("player-state task bodies (actor core part 2)",
                 "confirmed by #64: the class-1 bodies the player's own tasks run - input probing and the six directional decoders, the carried/riding movement block, the vehicle and star-ride state machines, the screen-transition fades over the DISPCNT shadow, and the stage-enter/leave sequences dispatched from the 26-entry table @0x0873FB08; 258 functions in 30.9 KiB"),
    0x08070EC0: ("actor bank C (11 class-3 tasks)",
                 "8 anchor tables in 0x0873F-0x08740; TaskYieldTrampoline x691; sprite draw x172"),
    0x08078B68: ("enemy/object behaviour bank 1",
                 "390 functions, mean 0x42, 347 pointer-dispatched, 20 anchor tables; no BL callers at all; 21 task types"),
    0x0807F044: ("enemy/object behaviour bank 2",
                 "188 functions, mean 0x54, 179 pointer-dispatched, 10 anchor tables; 9 task types"),
    0x08082E68: ("enemy/object behaviour bank 3",
                 "119 functions, mean 0x6c, 112 pointer-dispatched, 8 anchor tables; 8 task types"),
    0x080860F8: ("enemy/object behaviour bank 4",
                 "285 functions, mean 0x60, 266 pointer-dispatched, 14 anchor tables; sprite draw x511"),
    0x0808CCE8: ("enemy/object behaviour bank 5",
                 "157 functions, mean 0x53, 131 pointer-dispatched, 8 anchor tables; 11 task types"),
    0x0809000C: ("boss behaviour bank (four scripted bosses)",
                 "confirmed by #67: four self-contained boss scripts, one per anchor table, each built the same way - an entry function that installs a draw hook (sub_080656b4 with sub_08065438/sub_08065350/sub_080653ec/sub_0806523c) and a per-frame body, a state byte in Task.unk15 that sub_08002e98 dispatches through the table, and a <body, state-guard> pair per state, the guard re-arming the body through sub_08006148(fn, gCurTaskIdx) whenever Task.unk14 leaves its state; the bodies are runs of TaskYieldTrampoline waits that drive Task.unk3C (animation frame), Task.unk7A (the flag the movement helpers set on landing) and the 16.16 velocity pair through sub_080061c0/sub_0800622c; gUnk_02007D00[] is the shared EWRAM scratch block (live-boss count, parent task id, table row, RNG slot)"),
    0x08093F64: ("enemy/object behaviour bank 7",
                 "140 functions, 100 pointer-dispatched, 4 anchor tables; sprite draw x339; 7 task types"),
    0x080988F8: ("enemy/object behaviour bank 8",
                 "140 functions, mean 0x5a, 122 pointer-dispatched; anchor tables @0x08745630 (38) and @0x0874574C (48)"),
    0x0809BA44: ("enemy/object behaviour bank 9",
                 "198 functions, 135 pointer-dispatched, 5 anchor tables; sprite draw x487"),
    0x080A1590: ("enemy/object behaviour bank 10",
                 "221 functions, mean 0x4a, 153 pointer-dispatched; 80-entry anchor table @0x08748624"),
    0x080A5644: ("enemy/object behaviour bank 11",
                 "130 functions, 110 pointer-dispatched; anchor tables @0x08748EB8 (53) and @0x08749150 (20); 12 task types"),
    0x080AA338: ("enemy/object behaviour bank 12",
                 "121 functions, 96 pointer-dispatched; 18 task types; Div x5"),
    0x080AE3BC: ("enemy/object behaviour bank 13",
                 "129 functions, 96 pointer-dispatched, 5 anchor tables; 15 task types; TaskYieldTrampoline x412"),
    0x080B2FE8: ("HUD / overlay effects?",
                 "9 task types (5 of class 4); LZ77UnCompVram x6 + CpuSet x3 + a DMA0 register pair; VRAM transfer queue x20; EWRAM x102"),
    0x080B6154: ("save file / SRAM records + options",
                 "WriteSramEx x5, ReadSram x2, sram_driver x7 - the only SRAM user in the bulk; 98 RAM cells; direct REG_BG1HOFS/REG_BG2HOFS writes"),
    0x080B9D0C: ("game-mode flow + link lobby",
                 "requested/next game state x7, link session state, link-mode flag, number of linked players; called from AgbMain; 193 small functions"),
    0x080BDA2C: ("sub-game: four-slot bomb-pass minigame",
                 "confirmed by #66: task type #95, started from the game-mode flow module and dispatched through the 41-entry anchor table @0x08756668; four players are shuffled into the four slots of gUnk_02006A10[] and a projectile is passed from slot to slot (Task.unk34 0-3, forward while Task.unk28 <= 2 and backward otherwise), the beat length comes from the seven-entry level table @0x08756570, the button window from the five-byte records @0x087565F4, the hand-off flight from the 16.16 parabola p0 + v*t + (a*t*t)/2 in sub_080c061c, and elimination order is written into gUnk_0200B044[] with the out-mask gUnk_0200AF10"),
    0x080C1FFC: ("FIR-coefficient effect engine",
                 "the consumer rom-map section 2 seg 10 predicted: 22 pool refs into lib_misc + 7 into lib_rodata_fir_tables (0x080CFF00 tables); Div x10 / Mod x4 / __divsi3 x5; EWRAM x183"),
    0x080C6420: ("intro / cutscene / ending sequences?",
                 "TaskYieldTrampoline x826; 28 compressed_graphics refs; palette fade + BLD shadows; task types #260-264; called from AgbMain and the mode manager"),
}


# --------------------------------------------------------------------------
# input
# --------------------------------------------------------------------------
def u32(rom, off):
    return struct.unpack_from("<I", rom, off)[0]


def u16(rom, off):
    return struct.unpack_from("<H", rom, off)[0]


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


def load_symbols(path, lo, hi):
    rows = []
    with open(path) as f:
        for r in csv.DictReader(f):
            rows.append((int(r["vma"], 16), int(r["size"], 16), r["isa"],
                         r["evidence"], r["name"]))
    rows.sort()
    return rows, [r for r in rows if lo <= r[0] < hi and r[2] == "thumb"]


def load_ram_annotations(src_dir):
    """{addr: semantics} for the `gUnk_<addr>` cells annotated in src/*.c.

    The early-zone decompilation (issue #32) documented what most IWRAM cells
    are for in trailing `/* ... */` comments on the `extern` declarations.
    Those semantics are the cheapest naming evidence available for the bulk
    modules: whatever cells a module pokes says what it is.
    """
    import glob
    import re
    pat = re.compile(r"gUnk_(0[0-9A-Fa-f]{7})[^;]*;\s*/\*(.*?)\*/")
    out = {}
    for path in sorted(glob.glob(os.path.join(src_dir, "*.c"))):
        with open(path) as f:
            for m in pat.finditer(f.read()):
                addr = int(m.group(1), 16)
                text = " ".join(m.group(2).split())
                # Skip struct-offset comments (`/* 0x04 */`) and stubs: they
                # carry no semantics.
                if len(text) < 4 or re.fullmatch(r"0x[0-9A-Fa-f]+", text):
                    continue
                out.setdefault(addr, text)
    return out


def load_io_names(path):
    """{offset: REG name} from include/gba/io_reg.h."""
    import re
    pat = re.compile(r"#define\s+REG_OFFSET_(\w+)\s+(0x[0-9A-Fa-f]+|\d+)")
    out = {}
    try:
        with open(path) as f:
            for m in pat.finditer(f.read()):
                out.setdefault(int(m.group(2), 0), m.group(1))
    except OSError:
        pass
    return out


def load_callgraph(path, all_addrs):
    """Return (bl_edges, ptr_edges); bl callers repaired by containment."""
    bl, ptr = [], []
    with open(path) as f:
        for r in csv.DictReader(f):
            caller = int(r["caller"], 16)
            callee = int(r["callee"], 16)
            site = int(r["site"], 16)
            count = int(r["count"])
            if caller == 0:
                i = bisect.bisect_right(all_addrs, site) - 1
                caller = all_addrs[i] if i >= 0 else 0
            (bl if r["kind"] == "bl" else ptr).append((caller, callee, site, count))
    return bl, ptr


# --------------------------------------------------------------------------
# anchors: runs of consecutive Thumb function pointers
# --------------------------------------------------------------------------
def pointer_runs(ptr_edges):
    """Group `ptr` edge sites into runs of consecutive words.

    A one-word hole is tolerated: dispatch tables commonly carry a NULL or a
    non-code word (a count, a flag) between entries.
    """
    by_site = {}
    for _caller, callee, site, _count in ptr_edges:
        by_site[site] = callee
    sites = sorted(by_site)
    runs, cur = [], [sites[0]]
    for s in sites[1:]:
        if s - cur[-1] <= 8:
            cur.append(s)
        else:
            runs.append(cur)
            cur = [s]
    runs.append(cur)
    return [(r[0], [by_site[s] for s in r]) for r in runs]


def anchor_tables(runs, lo, hi):
    """Tables with >= MIN_TABLE_RUN targets inside [lo, hi)."""
    out = []
    for site, targets in runs:
        inside = [t for t in targets if lo <= t < hi]
        if len(inside) < MIN_TABLE_RUN:
            continue
        out.append((site, len(targets), len(inside), min(inside), max(inside)))
    return out


# --------------------------------------------------------------------------
# literal pools: exact pool words via `ldr rN, [pc, #imm]`
# --------------------------------------------------------------------------
def pool_refs(rom, lo, hi):
    """Return {pool_word_vma: value} and [(site, value)] for the range.

    Decodes every Thumb `LDR rd, [pc, #imm8*4]` (0x4800-0x4FFF) in [lo, hi):
    the loaded word is at `(site + 4) & ~3 + imm*4`.  This is exact where the
    halfword really is an instruction; inside a data blob it can fire on a
    data word, which only ever adds noise to the aggregate counts below.
    """
    words, refs = {}, []
    for vma in range(lo, hi - 1, 2):
        hw = u16(rom, vma - ROM_BASE)
        if not 0x4800 <= hw <= 0x4FFF:
            continue
        target = ((vma + 4) & ~3) + (hw & 0xFF) * 4
        if not ROM_BASE <= target < ROM_BASE + len(rom) - 3:
            continue
        val = u32(rom, target - ROM_BASE)
        words[target] = val
        refs.append((vma, val))
    return words, refs


def classify_ref(val, segments, io_names=None):
    """Bucket a pool word by what it points at."""
    if 0x04000000 <= val < 0x04000400:
        off = val & 0x3FF
        if io_names is not None and off in io_names:
            return "io", "REG_" + io_names[off]
        for start, end, name in IO_GROUPS:
            if start <= off < end:
                return "io", "IO %s" % name
        return "io", "IO other"
    if 0x03000000 <= val < 0x03008000:
        return "iwram", "IWRAM"
    if 0x02000000 <= val < 0x02040000:
        return "ewram", "EWRAM"
    if 0x05000000 <= val < 0x05000400:
        return "vram", "palette RAM"
    if 0x06000000 <= val < 0x06018000:
        return "vram", "VRAM"
    if 0x07000000 <= val < 0x07000400:
        return "vram", "OAM"
    if 0x0E000000 <= val < 0x0E010000:
        return "sram", "SRAM"
    if ROM_BASE <= val < 0x08800000:
        target = val & ~1
        for start, end, kind, name in segments:
            if start <= target < end:
                return ("code" if kind.endswith("code") else "rom"), name
    return None, None


def task_types(rom, known_entries):
    """[(type, class, entry_vma)] from the ROM task-type table.

    Walks 8-byte entries until one whose second word is not a Thumb pointer to
    a known function entry; that is the table's real end.
    """
    out = []
    i = 0
    while True:
        vma = TASK_TYPE_TABLE + 8 * i
        if vma - ROM_BASE + 8 > len(rom):
            break
        cls = rom[vma - ROM_BASE]
        word = u32(rom, vma + 4 - ROM_BASE)
        if not word & 1 or (word & ~1) not in known_entries:
            break
        out.append((i, cls, word & ~1))
        i += 1
    return out


def switch_dispatches(rom, lo, hi):
    """Count computed jumps (`mov pc, rN` / `add pc, rN`): jump-table switches."""
    n = 0
    for vma in range(lo, hi - 1, 2):
        hw = u16(rom, vma - ROM_BASE)
        if hw & 0xFF87 in (0x4687, 0x4487):
            n += 1
    return n


# --------------------------------------------------------------------------
# segmentation
# --------------------------------------------------------------------------
def boundary_costs(fn_addrs, bl_edges, tables, lo, hi):
    """Cost of cutting before fn_addrs[i], for i in 1..len(fn_addrs)-1."""
    idx = {a: i for i, a in enumerate(fn_addrs)}
    n = len(fn_addrs)
    cut = [0] * (n + 1)
    for caller, callee, _site, _count in bl_edges:
        if caller == callee:
            continue
        if not (lo <= caller < hi and lo <= callee < hi):
            continue
        if abs(caller - callee) > LOCAL_WINDOW:
            continue
        i, j = sorted((idx[caller], idx[callee]))
        for b in range(i + 1, j + 1):
            cut[b] += 1
    pen = [0] * (n + 1)
    for _site, _total, _inside, tlo, thi in tables:
        if thi - tlo > TIGHT_SPAN:
            continue
        for b in range(idx[tlo] + 1, idx[thi] + 1):
            pen[b] += 1
    return [cut[b] + TABLE_PENALTY * pen[b] for b in range(n + 1)], cut, pen


def segment(sizes, cost, size_min, size_max, allowed=None):
    """Exact DP: minimise the total cut cost, module size in [min, max].

    `allowed` restricts boundaries to a set of unit indices (used to make the
    coarse boundaries a subset of the fine ones).  Returns [(i, j)] unit index
    half-open spans.
    """
    n = len(sizes)
    pref = [0]
    for s in sizes:
        pref.append(pref[-1] + s)
    inf = float("inf")
    dp = [inf] * (n + 1)
    back = [-1] * (n + 1)
    dp[0] = 0
    for j in range(1, n + 1):
        i = j - 1
        while i >= 0:
            span = pref[j] - pref[i]
            if span > size_max and i < j - 1:
                break
            if dp[i] == inf:
                i -= 1
                continue
            if span >= size_min or j == n:
                if i == 0 or allowed is None or i in allowed:
                    c = dp[i] + (cost[i] if i > 0 else 0)
                    if c < dp[j]:
                        dp[j] = c
                        back[j] = i
            i -= 1
    out, j = [], n
    while j > 0:
        i = back[j]
        assert i >= 0, "no feasible segmentation"
        out.append((i, j))
        j = i
    return list(reversed(out))


# --------------------------------------------------------------------------
# assembly of the module records
# --------------------------------------------------------------------------
def split_range(segments, lo, hi):
    """Split [lo, hi) into (clusterable, fixed) pieces per segments.txt.

    A piece is clusterable when it is still `thumb_code` awaiting C; anything
    already `c_code` (or deliberately-final asm) is reported as-is.  Carving a
    module out of the middle of the bulk splits `game_code_and_rodata` into
    `game_code_and_rodata` + `game_code_and_rodata_<addr>` tails (first hit when
    #65 landed M17), so match on the prefix — otherwise the whole tail collapses
    into one unclustered "module".
    """
    clusterable, fixed = [], []
    for start, end, kind, name in segments:
        s, e = max(start, lo), min(end, hi)
        if s >= e:
            continue
        if kind == "thumb_code" and name.startswith("game_code_and_rodata"):
            clusterable.append((s, e))
        else:
            fixed.append((s, e, kind, name))
    return clusterable, fixed


def build_modules(args):
    with open(args.rom, "rb") as f:
        rom = f.read()
    segments = parse_segments(args.segments)
    all_syms, syms = load_symbols(args.symbols, args.start, args.end)
    all_addrs = [s[0] for s in all_syms]
    bl_edges, ptr_edges = load_callgraph(args.callgraph, all_addrs)

    seg_of = {}
    for start, end, _kind, name in segments:
        seg_of[(start, end)] = name

    def segment_name(vma):
        for start, end, _kind, name in segments:
            if start <= vma < end:
                return name
        return "?"

    clusterable, fixed = split_range(segments, args.start, args.end)
    runs = pointer_runs(ptr_edges)

    # --- function nodes, tiling the clusterable pieces ---------------------
    modules = []
    blocks_all = []
    for lo, hi in clusterable:
        fns = [s for s in syms if lo <= s[0] < hi]
        fn_addrs = [s[0] for s in fns]
        sizes = [(fn_addrs[i + 1] if i + 1 < len(fn_addrs) else hi) - fn_addrs[i]
                 for i in range(len(fn_addrs))]
        assert sum(sizes) == hi - lo, "nodes do not tile %08X-%08X" % (lo, hi)
        tables = anchor_tables(runs, lo, hi)
        cost, cut, pen = boundary_costs(fn_addrs, bl_edges, tables, lo, hi)
        blocks = segment(sizes, cost, args.block_min, args.block_max)
        allowed = set(i for i, _j in blocks)
        mods = segment(sizes, cost, args.module_min, args.module_max, allowed)
        for i, j in blocks:
            blocks_all.append((fn_addrs[i],
                               fn_addrs[j] if j < len(fn_addrs) else hi, j - i))
        for i, j in mods:
            start = fn_addrs[i]
            end = fn_addrs[j] if j < len(fn_addrs) else hi
            modules.append({
                "start": start, "end": end, "fns": fns[i:j],
                "cut_in": cut[i] if i else 0, "cut_out": cut[j] if j < len(cut) else 0,
                "tables": tables, "cost": cost[i] if i else 0,
            })
    for lo, hi, kind, name in fixed:
        modules.append({"start": lo, "end": hi, "fns": [s for s in syms if lo <= s[0] < hi],
                        "fixed": name, "kind": kind, "tables": [], "cut_in": 0,
                        "cut_out": 0, "cost": 0})
    modules.sort(key=lambda m: m["start"])

    # --- coverage assertion ------------------------------------------------
    pos = args.start
    for m in modules:
        assert m["start"] == pos, "coverage hole/overlap at %08X" % pos
        pos = m["end"]
    assert pos == args.end, "coverage ends at %08X, expected %08X" % (pos, args.end)

    # --- per-module evidence ----------------------------------------------
    def module_of(vma):
        i = bisect.bisect_right([m["start"] for m in modules], vma) - 1
        if i < 0:
            return None
        return modules[i] if vma < modules[i]["end"] else None

    pool_words, refs = pool_refs(rom, args.start, args.end)
    io_names = load_io_names(args.io_header)
    ram_ann = load_ram_annotations(args.src_dir)
    for m in modules:
        m["pool_words"] = sum(1 for w in pool_words if m["start"] <= w < m["end"])
        m["refs"] = Counter()
        m["io"] = Counter()
        m["ram_sem"] = Counter()
        m["ram_cells"] = set()
        for site, val in refs:
            if not m["start"] <= site < m["end"]:
                continue
            kind, label = classify_ref(val, segments, io_names)
            if kind is None:
                continue
            if kind == "io":
                m["io"][label] += 1
                continue
            m["refs"][label] += 1
            if kind in ("iwram", "ewram"):
                m["ram_cells"].add(val)
                if val in ram_ann:
                    m["ram_sem"][ram_ann[val]] += 1
        m["switches"] = switch_dispatches(rom, m["start"], m["end"])
        m["callers"] = Counter()
        m["callees"] = Counter()
        m["early"] = Counter()
        m["helpers"] = Counter()
        m["ptr_sites"] = Counter()
        m["anchors"] = []
        for site, total, inside, tlo, thi in m["tables"]:
            if m["start"] <= tlo and thi < m["end"]:
                m["anchors"].append((site, total, inside, tlo, thi))

    tasks = task_types(rom, set(s[0] for s in all_syms))
    for m in modules:
        m["tasks"] = [(t, c) for t, c, e in tasks if m["start"] <= e < m["end"]]

    sym_names = {s[0]: s[4] for s in all_syms if not s[4].startswith("sub_")}
    idx_by_fn = {}
    for m in modules:
        for s in m["fns"]:
            idx_by_fn[s[0]] = m
    for caller, callee, _site, count in bl_edges:
        mc, me = idx_by_fn.get(caller), idx_by_fn.get(callee)
        if me is not None and mc is not me:
            me["callers"][mc["start"] if mc else segment_name(caller)] += count
        if mc is not None and mc is not me:
            if me is not None:
                mc["callees"][me["start"]] += count
            else:
                name = segment_name(callee)
                mc["callees"][name] += count
                role = EARLY_ROLES.get(name)
                if role:
                    mc["early"][role] += count
                helper = NAMED_HELPERS.get(callee) or sym_names.get(callee)
                if helper:
                    mc["helpers"][helper] += count
    for _caller, callee, site, count in ptr_edges:
        me = idx_by_fn.get(callee)
        if me is not None:
            me["ptr_sites"][segment_name(site)] += count

    # --- derived metrics ---------------------------------------------------
    for m in modules:
        fns = m["fns"]
        m["nfns"] = len(fns)
        m["bytes"] = m["end"] - m["start"]
        m["ptr_only"] = sum(1 for s in fns if s[3] == "rom-pointer")
        m["maxfn"] = max((s[1] for s in fns), default=0)
        m["meanfn"] = (sum(s[1] for s in fns) // len(fns)) if fns else 0
        m["big"] = sum(1 for s in fns if s[1] >= 0x200)
        m["pool_density"] = m["pool_words"] * 4 / m["bytes"] if m["bytes"] else 0
        m["ext_deps"] = len([k for k in m["callees"] if isinstance(k, int)])
        m["blocks"] = [b for b in blocks_all if m["start"] <= b[0] < m["end"]]
        # Difficulty 1-6: size profile, struct/RAM breadth, jump tables, pools.
        score = 0
        score += 1 if m["meanfn"] > 0xC0 else 0
        score += 1 if m["maxfn"] >= 0x600 else 0
        score += 1 if len(m["ram_cells"]) >= 40 else 0
        score += 1 if m["switches"] >= 8 else 0
        score += 1 if m["pool_density"] >= 0.14 else 0
        m["difficulty"] = 1 + score
    return modules, blocks_all, runs


# --------------------------------------------------------------------------
# output
# --------------------------------------------------------------------------
def top(counter, n, fmt=str):
    return ", ".join("%s x%d" % (fmt(k), v)
                     for k, v in sorted(counter.items(), key=lambda kv: (-kv[1], str(kv[0])))[:n])


MODULE_IDS = {}


def fmt_key(k):
    """Module starts render as their map id; everything else is a segment name."""
    if isinstance(k, int):
        return MODULE_IDS.get(k, "0x%08X" % k)
    return k


def write_csv(modules, path):
    rows = []
    for i, m in enumerate(modules, 1):
        name, evidence = MODULE_NAMES.get(m["start"], ("", ""))
        if "fixed" in m:
            name = name or m["fixed"]
            evidence = evidence or "already landed (%s)" % m["kind"]
        rows.append({
            "id": "M%02d" % i,
            "start": "0x%08X" % m["start"],
            "end": "0x%08X" % m["end"],
            "size": "0x%X" % m["bytes"],
            "functions": m["nfns"],
            "ptr_dispatched": m["ptr_only"],
            "blocks": len(m["blocks"]),
            "mean_fn": "0x%X" % m["meanfn"],
            "max_fn": "0x%X" % m["maxfn"],
            "pool_pct": "%.1f" % (100 * m["pool_density"]),
            "ram_cells": len(m["ram_cells"]),
            "jump_tables": m["switches"],
            "anchor_tables": len(m["anchors"]),
            "task_types": len(m["tasks"]),
            "ext_callers": len(m["callers"]),
            "ext_deps": m["ext_deps"],
            "difficulty": m["difficulty"],
            "name": name,
            "evidence": evidence,
        })
    fields = list(rows[0].keys())
    out = open(path, "w", newline="") if path != "-" else sys.stdout
    w = csv.DictWriter(out, fields, lineterminator="\n")
    w.writeheader()
    w.writerows(rows)
    if out is not sys.stdout:
        out.close()
    return rows


def write_details(modules, out):
    """Per-module detail blocks for module-map.md (regenerable)."""
    MODULE_IDS.update({m["start"]: "M%02d" % i for i, m in enumerate(modules, 1)})
    for i, m in enumerate(modules, 1):
        name, evidence = MODULE_NAMES.get(m["start"], ("", ""))
        if "fixed" in m:
            continue
        print("\n### M%02d `0x%08X-0x%08X` - %s\n"
              % (i, m["start"], m["end"] - 1, name or "unknown"), file=out)
        print("* **Size** %.1f KiB (`%#x`), %d functions (%d reachable only through "
              "pointer tables), mean `%#x`, largest `%#x`, pool words %.1f%% of bytes."
              % (m["bytes"] / 1024, m["bytes"], m["nfns"], m["ptr_only"], m["meanfn"],
                 m["maxfn"], 100 * m["pool_density"]), file=out)
        print("* **Difficulty** %d/6 - %d distinct RAM cells, %d jump-table dispatches, "
              "%d functions >= `0x200`." % (m["difficulty"], len(m["ram_cells"]),
                                            m["switches"], m["big"]), file=out)
        print("* **Seam cost** %d in / %d out (local `bl` edges crossing the boundary)."
              % (m["cut_in"], m["cut_out"]), file=out)
        if evidence:
            print("* **Why** %s." % evidence, file=out)
        if m["tasks"]:
            cls = Counter(c for _t, c in m["tasks"])
            print("* **Task types** %d (%s): %s."
                  % (len(m["tasks"]),
                     ", ".join("class %d x%d" % (k, v) for k, v in sorted(cls.items())),
                     ", ".join("#%d" % t for t, _c in m["tasks"])), file=out)
        if m["anchors"]:
            print("* **Anchor tables** %s%s."
                  % ("; ".join("`0x%08X` %d entries -> `0x%08X-0x%08X`"
                               % (site, tot, tlo, thi)
                               for site, tot, _ins, tlo, thi in m["anchors"][:6]),
                     " (+%d more)" % (len(m["anchors"]) - 6) if len(m["anchors"]) > 6 else ""),
                  file=out)
        if m["early"]:
            print("* **Calls into the decompiled early zone** %s." % top(m["early"], 6),
                  file=out)
        if m["helpers"]:
            print("* **Named helpers** %s." % top(m["helpers"], 6), file=out)
        if m["callers"]:
            print("* **Called from** %s." % top(m["callers"], 5, fmt_key), file=out)
        if m["callees"]:
            print("* **Depends on** %s." % top(m["callees"], 5, fmt_key), file=out)
        print("* **Pool references** %s." % (top(m["refs"], 8) or "-"), file=out)
        if m["ram_sem"]:
            print("* **Known RAM cells touched** %s." % top(m["ram_sem"], 6), file=out)
        print("* **Suggested batches** %s."
              % ", ".join("`0x%08X` (%d fns)" % (b[0], b[2]) for b in m["blocks"]),
              file=out)


def write_markdown(modules, out):
    MODULE_IDS.update({m["start"]: "M%02d" % i for i, m in enumerate(modules, 1)})
    print("| # | Range | Size | Fns | Anchors | Difficulty | Candidate subsystem |", file=out)
    print("|---|-------|------|-----|---------|------------|---------------------|", file=out)
    for i, m in enumerate(modules, 1):
        name, _ev = MODULE_NAMES.get(m["start"], ("", ""))
        diff = "*" * m["difficulty"]
        if "fixed" in m:
            name = name or "**done** - %s" % m["fixed"]
            diff = "-"
        print("| M%02d | `0x%08X-0x%08X` | %.1f KiB | %d | %d | %s | %s |"
              % (i, m["start"], m["end"] - 1, m["bytes"] / 1024, m["nfns"],
                 len(m["anchors"]), diff, name or "unknown"), file=out)


def write_report(modules, blocks, out):
    total = sum(m["bytes"] for m in modules)
    ntasks = sum(len(m["tasks"]) for m in modules)
    print("# module map evidence dump", file=out)
    print("# %d modules, %d blocks, %#x bytes" % (len(modules), len(blocks), total), file=out)
    print("# %d of the ROM task-type table's entry points land in this range\n"
          % ntasks, file=out)
    for i, m in enumerate(modules, 1):
        print("=" * 78, file=out)
        head = "M%02d  %08X-%08X  %#x bytes  %d fns" % (
            i, m["start"], m["end"] - 1, m["bytes"], m["nfns"])
        if "fixed" in m:
            head += "  [DONE: %s]" % m["fixed"]
        print(head, file=out)
        if "fixed" in m:
            continue
        print("  seam cost in/out: %d / %d   (local bl edges crossing)"
              % (m["cut_in"], m["cut_out"]), file=out)
        print("  fns: %d (%d pointer-dispatched only)  mean %#x  max %#x  >=0x200: %d"
              % (m["nfns"], m["ptr_only"], m["meanfn"], m["maxfn"], m["big"]), file=out)
        print("  pool: %d words (%.1f%% of bytes)  jump tables: %d  RAM cells: %d  difficulty %d"
              % (m["pool_words"], 100 * m["pool_density"], m["switches"],
                 len(m["ram_cells"]), m["difficulty"]), file=out)
        if m["anchors"]:
            print("  anchor tables (fully inside):", file=out)
            for site, tot, ins, tlo, thi in m["anchors"][:12]:
                print("    @%08X %3d entries (%d here) -> %08X..%08X (span %#x) [%s]"
                      % (site, tot, ins, tlo, thi, thi - tlo, "asset" if site >= 0x08730000 else "rodata"),
                      file=out)
            if len(m["anchors"]) > 12:
                print("    ... %d more" % (len(m["anchors"]) - 12), file=out)
        if m["tasks"]:
            cls = Counter(c for _t, c in m["tasks"])
            print("  task types: %d (%s) -> %s"
                  % (len(m["tasks"]),
                     " ".join("class%d x%d" % (k, v) for k, v in sorted(cls.items())),
                     ",".join("#%d" % t for t, _c in m["tasks"])), file=out)
        if m["early"]:
            print("  early-zone calls: %s" % top(m["early"], 8), file=out)
        if m["helpers"]:
            print("  named helpers: %s" % top(m["helpers"], 8), file=out)
        print("  callers: %s" % (top(m["callers"], 6, fmt_key) or "-"), file=out)
        print("  callees: %s" % (top(m["callees"], 6, fmt_key) or "-"), file=out)
        print("  pool refs: %s" % (top(m["refs"], 12) or "-"), file=out)
        if m["io"]:
            print("  IO regs: %s" % top(m["io"], 14), file=out)
        if m["ram_sem"]:
            print("  known RAM: %s" % top(m["ram_sem"], 10), file=out)
        print("  ptr sites: %s" % (top(m["ptr_sites"], 4) or "-"), file=out)
        print("  blocks: %s" % ", ".join("%08X(%d)" % (b[0], b[2]) for b in m["blocks"]), file=out)



RECIPE = """## Recipe (read before writing any C)

* **Compiler: `agbcc -O2 -mthumb-interwork -fprologue-bugfix`** (fnmatch
  `--newpb`) for the whole bulk range. `src/main.c` at `0x08007300`, the first
  bytes of the range, is verified byte-exact under it. Do not repeat #32's
  wrong turn: a missing leaf `push {lr}` looks like an `old_agbcc` fingerprint
  but is agbcc's cached `far_jump_used_p()` bug
  (`docs/lessons-learned.md` §3.75; §3.18 is kept as superseded because the
  reasoning that produced it is instructive).
* Read `docs/decomp-loop.md` (per-function loop + subagent handoff contract) and
  `docs/lessons-learned.md` **§3.19-§3.93** - 38 lessons from #32, most of which
  are properties of agbcc rather than of that zone.
* **Escalation rule that saved the most time in #32:** if a diff is
  register-naming-only and two informed shape attempts fail, go straight to
  `agbcc -da` RTL dumps or the pinned agbcc source
  (`grep -rn OLD_COMPILER` lists all nine behavioural differences between the
  two compiler binaries). Every hard function in #32 fell that way; every
  "try 60 source shapes" campaign failed.
* Per function: `./tools/fnmatch.sh <start> <end> src/<name>.c` until MATCH
  (decomp-permuter when stuck, `tools/permuter-example/`), then land the
  verified range with `python3 tools/carve.py <start> <end> <name> --write`.
  Never hand-edit generated asm (lesson 4.11).
* After each landing: `make symbols && make split`, then
  `make clean && make compare` + map check.
* A size in `symbols.csv` that does not tile with the next entry is a signal,
  not noise: #32 found 20 wrong sizes in 27 KiB (hidden dead exports, and rows
  that were not functions). Curated fixes go in `tools/symdb.py`
  (`FALSE_POSITIVES`, `EXTRA_THUMB_ENTRIES`).
* This range references the `0x04000000` I/O block only 20 times in 792 KiB:
  expect IWRAM shadow cells (`gUnk_03001ED8` = DISPCNT shadow, the eight 16.16
  BG scroll shadows, ...) rather than registers, and declare them via
  `tools/split_config.json` `data_symbols`.
"""

DONE = """## Definition of done

- [ ] every function in the range is C, `make progress` shows 0 asm code bytes
      in it
- [ ] `make clean && make compare` byte-identical
- [ ] names are evidence-based or `sub_08XXXXXX` / `gUnk_08XXXXXX`
      (AGENTS.md conventions); RAM cells via `split_config.json` `data_symbols`
- [ ] `MODULE_NAMES` in `tools/modmap.py` updated with what this module turned
      out to be, then `make modmap` (CI checks the CSV) and the §6 detail block
      in `docs/analysis/module-map.md` refreshed
- [ ] anything the module reveals about the ROM's tables/segments recorded in
      `docs/analysis/rom-map.md`; new agbcc lessons in
      `docs/lessons-learned.md`
- [ ] progress posted on #35 (bytes remaining in the module)
- [ ] PR open against `master` with CI green (do NOT merge - #35, AGENTS.md)
"""


def write_issues(modules, directory):
    """Write one child-issue file per module: line 1 = title, rest = body."""
    MODULE_IDS.update({m["start"]: "M%02d" % i for i, m in enumerate(modules, 1)})
    if not os.path.isdir(directory):
        os.makedirs(directory)
    order = sorted((m for m in modules if "fixed" not in m),
                   key=lambda m: (WAVES.get(m["start"], 9), m["difficulty"],
                                  m["ext_deps"], m["bytes"]))
    written = []
    for m in order:
        mid = MODULE_IDS[m["start"]]
        name, evidence = MODULE_NAMES.get(m["start"], ("unknown", ""))
        wave = WAVES.get(m["start"], 9)
        path = os.path.join(directory, "%s.md" % mid)
        with open(path, "w") as out:
            title_name = (name[:-1].rstrip() + " (candidate)"
                          if name.endswith("?") else name)
            print("bulk %s: decompile %s - 0x%08X-0x%08X (%.1f KiB, %d fns)"
                  % (mid, title_name, m["start"], m["end"] - 1,
                     m["bytes"] / 1024, m["nfns"]), file=out)
            print("""
Child issue of #35, generated from the module map produced by #34
(`docs/analysis/module-map.md` §6, module **%s**).

## Range

| | |
|---|---|
| VMA range | `0x%08X-0x%08X` (end exclusive `0x%08X`) |
| Size | %.1f KiB (`%#x`) |
| Functions | %d, of which %d are reachable **only** through ROM pointer tables |
| Mean / largest function | `%#x` / `%#x` |
| Literal-pool words | %d (%.1f%% of the range) |
| Jump-table dispatches | %d |
| Distinct RAM cells referenced | %d |
| Difficulty (1-6 heuristic) | %d |
| Wave | %d - %s |

The module boundaries are frozen while this issue is open: they are the keys of
`MODULE_NAMES` in `tools/modmap.py` and changing a clustering threshold re-cuts
them. Neighbours in address order: see the table in
`docs/analysis/module-map.md` §4.
""" % (mid, m["start"], m["end"] - 1, m["end"], m["bytes"] / 1024, m["bytes"],
            m["nfns"], m["ptr_only"], m["meanfn"], m["maxfn"], m["pool_words"],
            100 * m["pool_density"], m["switches"], len(m["ram_cells"]),
            m["difficulty"], wave, WAVE_NAMES.get(wave, "")), file=out)

            print("## What this module looks like (measured)\n", file=out)
            if evidence:
                print("**Candidate identification: %s** - %s.\n"
                      % (name, evidence), file=out)
            if name.endswith("?"):
                print("The name ends in `?`: it is a hypothesis, and\n"
                      "`docs/analysis/module-map.md` §7 says what would settle it.\n"
                      "Renaming it from what you find is part of this issue.\n", file=out)
            if m["tasks"]:
                cls = Counter(c for _t, c in m["tasks"])
                print("* **Task types** (ROM table `0x0872FF30`, 8-byte entries "
                      "`{u8 class; u32 entry}`; the entry word is the task body's "
                      "Thumb entry point): %d here (%s): %s."
                      % (len(m["tasks"]),
                         ", ".join("class %d x%d" % (k, v) for k, v in sorted(cls.items())),
                         ", ".join("#%d" % t for t, _c in m["tasks"])), file=out)
            if m["anchors"]:
                print("* **Anchor tables** (each gives its targets an ordinal - "
                      "the cheapest way to name functions here):", file=out)
                for site, tot, ins, tlo, thi in m["anchors"]:
                    print("  * `0x%08X`: %d entries -> `0x%08X-0x%08X`"
                          % (site, tot, tlo, thi), file=out)
            if m["early"]:
                print("* **Calls into the decompiled early zone** (what services it "
                      "uses): %s." % top(m["early"], 8), file=out)
            if m["helpers"]:
                print("* **Named helpers called**: %s." % top(m["helpers"], 8), file=out)
            if m["callers"]:
                print("* **Called from**: %s." % top(m["callers"], 6, fmt_key), file=out)
            if m["callees"]:
                print("* **Depends on**: %s." % top(m["callees"], 6, fmt_key), file=out)
            print("* **Literal-pool references**: %s." % (top(m["refs"], 10) or "-"), file=out)
            if m["io"]:
                print("* **I/O registers referenced**: %s." % top(m["io"], 8), file=out)
            if m["ram_sem"]:
                print("* **Known RAM cells touched** (semantics from #32): %s."
                      % top(m["ram_sem"], 8), file=out)
            print("* **Seam cost** %d in / %d out: local `bl` edges crossing the "
                  "module boundary. A high value means the neighbouring module is "
                  "part of the same subsystem and was split only by the 32 KiB cap."
                  % (m["cut_in"], m["cut_out"]), file=out)
            print("\n## Suggested batches\n", file=out)
            print("The fine-grained segmentation, cut at cheap seams; each is a "
                  "contiguous run `tools/carve.py` will accept, and start-adjacent "
                  "carves keep `segments.txt` tidy (#35's carve note):\n", file=out)
            for i, b in enumerate(m["blocks"], 1):
                end = m["blocks"][i][0] if i < len(m["blocks"]) else m["end"]
                print("%d. `0x%08X-0x%08X` - %d functions" % (i, b[0], end - 1, b[2]),
                      file=out)
            print("\n" + RECIPE, file=out)
            print(DONE, file=out)
            print("## References\n", file=out)
            print("#35 (parent), #34 (module map), `docs/analysis/module-map.md` "
                  "§6 module %s, `docs/analysis/rom-map.md` §2 seg 7 + §9, "
                  "`docs/analysis/symbols.csv`, `docs/analysis/callgraph.csv`, "
                  "`asm/game_code_and_rodata/`." % mid, file=out)
        written.append((mid, path))
    return written


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--symbols", default="docs/analysis/symbols.csv")
    ap.add_argument("--callgraph", default="docs/analysis/callgraph.csv")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--src-dir", default="src",
                    help="decompiled sources, mined for gUnk_* cell semantics")
    ap.add_argument("--io-header", default="include/gba/io_reg.h")
    ap.add_argument("--out", default="docs/analysis/module-map.csv",
                    help="module table CSV ('-' for stdout)")
    ap.add_argument("--report", help="write the evidence dump here ('-' for stdout)")
    ap.add_argument("--markdown", help="write the markdown table here ('-' for stdout)")
    ap.add_argument("--issues", metavar="DIR",
                    help="write one child-issue file per module into DIR "
                         "(line 1 = title, rest = body), in wave order")
    ap.add_argument("--details", action="store_true",
                    help="with --markdown, also emit the per-module detail blocks")
    ap.add_argument("--start", type=lambda x: int(x, 16), default=RANGE_START)
    ap.add_argument("--end", type=lambda x: int(x, 16), default=RANGE_END)
    ap.add_argument("--module-max", type=lambda x: int(x, 16), default=MODULE_MAX)
    ap.add_argument("--module-min", type=lambda x: int(x, 16), default=MODULE_MIN)
    ap.add_argument("--block-max", type=lambda x: int(x, 16), default=BLOCK_MAX)
    ap.add_argument("--block-min", type=lambda x: int(x, 16), default=BLOCK_MIN)
    args = ap.parse_args()

    if not os.path.exists(args.rom):
        sys.exit("%s: not found (a legally dumped baserom.gba is required)" % args.rom)
    modules, blocks, _runs = build_modules(args)
    write_csv(modules, args.out)
    if args.report:
        out = sys.stdout if args.report == "-" else open(args.report, "w")
        write_report(modules, blocks, out)
        if out is not sys.stdout:
            out.close()
    if args.markdown:
        out = sys.stdout if args.markdown == "-" else open(args.markdown, "w")
        write_markdown(modules, out)
        if args.details:
            write_details(modules, out)
        if out is not sys.stdout:
            out.close()
    if args.issues:
        written = write_issues(modules, args.issues)
        print("%d child-issue drafts -> %s" % (len(written), args.issues))
    if args.out != "-":
        print("%d modules / %d blocks over %08X-%08X -> %s"
              % (len(modules), len(blocks), args.start, args.end, args.out))


if __name__ == "__main__":
    main()
