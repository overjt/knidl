"""Pointer-census evidence: the sprite frame network (issue #36 phase 2 run 2,
tools/ptrcensus.py provider).

The sprite frame network: the frame tables at 0x0874C44C-0x08756000 (seg 18
tail), the struct TaskGfx records they point at (seg 12, m4a_songs, seg 18,
seg 19 head) and the OAM template streams, tile streams and palettes those
records and tables name.

    provide(rom, cfg, segs) -> {"coincidence": [...], "pointer": [...]}

Everything is derived from the ROM and the config's labels; the constants
below are consumer facts, cited next to them.

Formats (all from decompiled consumers):
  * frame table: an array of pointers indexed by Task.frame
    (Task.frameTable, include/task.h; read by TaskDrawWorld and friends,
    src/early_5d9c.c:85, and the actor draws, src/actor_653ec.c:104/133),
    or by a direct index (QueueSprite(..., gUnk_X[i], ...)).  Each entry is
    either an OAM template stream (the table's consumer hands it to
    QueueSprite) or a struct TaskGfx * (its consumer reads the record).
  * OAM template stream: BuildOam (src/early_1b08.c) reads 8-byte entries
    {attr0, attr1, attr1_flipped, attr2} up to and including the one whose
    attr0 has bit 12 set.  No pointers.
  * struct TaskGfx {u32 oamTemplate; u16 *palette; u16 *tiles}
    (include/task.h): TaskLoadFrameTiles (src/early_5acc.c:82-107),
    sub_08065470 (src/actor_653ec.c:89-121), sub_0801a310
    (src/player_1a07c.c:135).  PlayerLoadFrameTilesAndPalette
    (src/stage_3cd60.c:479-541) adds the tagged variant: bit 0 of
    .oamTemplate set => 20 bytes {oam|1, palette, tiles, palette2, tiles2};
    palette, palette2 and tiles2 may be NULL.
  * tile stream: {u16 size; size bytes} chunks ended by the halfword 0xFFFF
    (the same three readers).  No pointers.
  * palette block: {u16 byteCount; byteCount bytes of colours}
    (RequestCopy(2, pal + 1, ..., *pal) in the same readers).  No pointers.
"""

import struct

ROM_BASE = 0x08000000

# The frame-table store (seg 18): the first frame table gUnk_0874C44C
# (src/actor_74c0c.c:70 `t->frameTable = gUnk_0874C44C`) follows the Whispy
# Woods behaviour tables (agent C / B); 0x08756000 is agent C's zone.  Every
# label in between names a frame table (consumer list: REPORT.md section 1).
FT_LO, FT_HI = 0x0874C44C, 0x08756000

# Frame tables whose entries are struct TaskGfx *, with the draw callback
# that reads them (start -> (tagged, consumer)).  A table not listed here
# holds OAM template pointers.
GFX_TABLES = {
    0x0874CFEC: (True, "Task_Player src/player_32688.c:80, draw sub_0803ddc0 -> PlayerLoadFrameTilesAndPalette"),
    0x087519E8: (True, "src/plobj_514f8.c:50, draw sub_0803dfc8 -> PlayerLoadFrameTilesAndPalette"),
    0x08751A28: (True, "src/plobj_514f8.c:192, draw sub_0803dfc8"),
    0x08751B40: (True, "src/plobj_5239c.c:59, draw sub_0803dfc8"),
    0x08751C44: (True, "src/effect_53af4.c:71, draw sub_08053c1c -> sub_0803dfc8"),
    0x08751CBC: (False, "src/effect_56dd4.c:145, draw TaskDrawWorldLoadTiles/TilesLoaded"),
    0x08751D80: (True, "src/effect_57494.c:293, draw sub_0805af80 -> sub_0803dfc8"),
    0x08751DBC: (True, "src/effect_57ce0.c:397, draw sub_0803dfc8"),
    0x08751ECC: (True, "src/effect_59570.c:274, draw sub_0803dfc8 / TaskDrawWorldTilesLoaded"),
    0x08752090: (True, "src/effect_5a358.c:510, draw sub_0805af80 -> sub_0803dfc8"),
    0x08752ED8: (False, "gBonkersFrames src/enemy_9000c.c:73, draw sub_08065438 -> sub_08065470"),
    0x08752F74: (False, "gGrandWheelieFrames src/enemy_93f64.c:159, draw sub_08065438"),
    0x08753090: (False, "gMrFrostyFrames src/enemy_988f8.c:340, draw sub_08065438"),
    0x08753180: (False, "gMrTickTockFrames src/enemy_99b20.c:401, draw sub_08065438"),
    0x087535FC: (False, "gBugzzyFrames src/enemy_9113c.c:743, draw sub_080653ec -> sub_08065470"),
    0x08753718: (False, "gFireLionFrames src/enemy_957bc.c:42, draw sub_08065438"),
    0x087537FC: (False, "gPhanPhanFrames src/enemy_974c8.c:44, draw sub_08065438"),
    0x087538E0: (False, "gKingDededeFrames src/enemy_9fbd0.c:69, draw sub_08065438"),
    0x08753994: (False, "gPaintRollerFrames src/enemy_aa338.c:1963, draw sub_08065438"),
    0x08753BB4: (False, "gMetaKnightFrames src/enemy_a1590.c:3507, draw sub_080653ec"),
    0x08754180: (False, "src/enemy_a1590.c:2685, draw sub_080a488c (struct TaskGfx *)"),
    0x087541B0: (False, "src/enemy_a1590.c:2736, draw sub_080a488c"),
    0x087541E0: (False, "src/enemy_a1590.c:2733, draw sub_080a488c"),
    0x08754568: (False, "src/enemy_aa338.c:998, draw sub_080a9ed8 (struct TaskGfx *)"),
    0x0875456C: (False, "gNightmareWizardFrames src/enemy_a93ec.c:661, draw sub_080a9ed8"),
    0x087546D0: (False, "src/enemy_a93ec.c:581 `(struct TaskGfx *)gUnk_087546D0[...]`"),
    0x087546F8: (False, "src/enemy_aa338.c:1476, draw sub_080aa16c (struct TaskGfx *)"),
    0x08754708: (False, "src/enemy_aa338.c:1233, draw sub_080aa16c"),
    0x08754718: (False, "src/enemy_aa338.c:1334, draw sub_080aa16c"),
    0x08755068: (True, "src/player_17668.c:18, draw sub_0801a1ec -> PlayerLoadFrameTilesAndPalette"),
    0x08755378: (False, "src/player_17668.c:623, draw sub_0801a310"),
    0x087553FC: (False, "src/player_19000.c:271, draw sub_0801a310"),
}
# gUnk_08751F84: entries 0-1 are TaskGfx records (sub_08059d7c,
# src/effect_59570.c:333, draw sub_0803dfc8), entries 2-17 OAM streams.
GFX_PREFIX = {0x08751F84: 2}
# Frame tables whose entries are neither OAM streams nor TaskGfx: their
# words are pointers, their targets have another format.
OTHER_TABLES = {
    0x087555FC: "struct GfxHeader * (include/hud.h:87; Task_IntroStoryPicture src/hud_099fc.c:35)",
}


def _u16(rom, a):
    return struct.unpack_from("<H", rom, a - ROM_BASE)[0]


def _u32(rom, a):
    return struct.unpack_from("<I", rom, a - ROM_BASE)[0]


def _in_rom(rom, a):
    return ROM_BASE <= a < ROM_BASE + len(rom)


def oam_len(rom, a, maxn=128):
    """Bytes of the BuildOam template stream at a, or None."""
    if a & 1 or not _in_rom(rom, a):
        return None
    for n in range(1, maxn + 1):
        if not _in_rom(rom, a + 8 * n):
            return None
        if _u16(rom, a + 8 * (n - 1)) & 0x1000:
            return 8 * n
    return None


def tiles_len(rom, a, maxchunks=64):
    """Bytes of the {u16 size; data} chunk stream at a (incl. 0xFFFF)."""
    if a & 1 or not _in_rom(rom, a):
        return None
    p = a
    for _ in range(maxchunks):
        n = _u16(rom, p)
        if n == 0xFFFF:
            return p + 2 - a
        if n == 0 or n % 32 or n > 0x400:
            return None
        p += 2 + n
        if not _in_rom(rom, p + 2):
            return None
    return None


def palette_len(rom, a):
    if a & 1 or not _in_rom(rom, a):
        return None
    n = _u16(rom, a)
    if n % 2 or n > 0x200:
        return None
    return 2 + n


def taskgfx(rom, r, tag_ok):
    """[(slot addr, value)] of the TaskGfx record at r, or None."""
    if r & 3 or not _in_rom(rom, r + 12):
        return None
    o = _u32(rom, r)
    tagged = bool(o & 1)
    if tagged and not tag_ok:
        return None
    f = [_u32(rom, r + 4 * i) for i in range(5 if tagged else 3)]
    if oam_len(rom, f[0] & ~1) is None:
        return None
    if f[1] and palette_len(rom, f[1]) is None:
        return None
    if tiles_len(rom, f[2]) is None:
        return None
    if tagged:
        if f[3] and palette_len(rom, f[3]) is None:
            return None
        if f[4] and tiles_len(rom, f[4]) is None:
            return None
    return [(r + 4 * i, v) for i, v in enumerate(f)]


def provide(rom, cfg, segs):
    labels = set(int(k, 16) for k in cfg.get("data_symbols", {}))
    labels |= set(int(k, 16) for k in cfg.get("extra_labels", {}))
    bounds = sorted(a for a in labels if FT_LO <= a < FT_HI) + [FT_HI]
    if not bounds or bounds[0] != FT_LO:
        raise ValueError("frame-table labels missing from the config")
    pointer = {}
    rec_why = {}
    oam_direct = {}   # stream start -> table start
    oam_rec = set()   # .oamTemplate streams of records
    tiles = set()
    pals = set()

    for a, b in zip(bounds, bounds[1:]):
        ngfx = GFX_PREFIX.get(a, (b - a) // 4 if a in GFX_TABLES else 0)
        tagged, cons = GFX_TABLES.get(a, (True, "sub_08059d7c src/effect_59570.c:333"))
        for i, w in enumerate(range(a, b, 4)):
            v = _u32(rom, w)
            if v == 0:
                continue
            if not _in_rom(rom, v):
                raise ValueError("frame table word 0x%08X is not a ROM pointer" % w)
            if i < ngfx:
                slots = taskgfx(rom, v, tagged)
                if slots is None:
                    raise ValueError("0x%08X: no TaskGfx record at 0x%08X" % (w, v))
                pointer[w] = "frame table 0x%08X entry -> struct TaskGfx (%s)" % (a, cons)
                for s, sv in slots:
                    if sv:
                        pointer[s] = ("struct TaskGfx field +0x%X of the record 0x%08X "
                                      "(frame table 0x%08X, %s)%s"
                                      % (s - v, v, a, cons,
                                         "; bit 0 is PlayerLoadFrameTilesAndPalette's tag"
                                         if s == v and sv & 1 else ""))
                f = [sv for _s, sv in slots]
                oam_rec.add(f[0] & ~1)
                if f[1]:
                    pals.add(f[1])
                tiles.add(f[2])
                if len(f) == 5:
                    if f[3]:
                        pals.add(f[3])
                    if f[4]:
                        tiles.add(f[4])
                rec_why[v] = a
            else:
                pointer[w] = ("frame table 0x%08X entry (%s)"
                              % (a, OTHER_TABLES.get(a, "OAM template pointer")))
                if a not in OTHER_TABLES:
                    if oam_len(rom, v) is None:
                        raise ValueError("0x%08X: no OAM stream at 0x%08X" % (w, v))
                    oam_direct.setdefault(v, a)

    coincidence = []

    def clean(s, e):
        # a claimed non-pointer range must not hold a proven pointer word
        return not any(w in pointer for w in range(s & ~3, e, 4))

    for t in sorted(tiles):
        e = t + tiles_len(rom, t)
        if clean(t, e):
            coincidence.append((t, e, "tiles", "TaskGfx tile stream {u16 size; data}.. 0xFFFF "
                                "(TaskLoadFrameTiles src/early_5acc.c:96)"))
    for p in sorted(pals):
        e = p + palette_len(rom, p)
        if clean(p, e):
            coincidence.append((p, e, "palette", "TaskGfx palette {u16 byteCount; colours} "
                                "(src/early_5acc.c:74, src/stage_3cd60.c:499)"))
    # OAM streams: a record's .oamTemplate is one by its consumer; a direct
    # table's target is accepted when the stream chains with another known
    # stream or record (its end is one's start, or its start is one's end),
    # the byte-exact tiling of a sheet's OAM block.
    starts = set(oam_rec) | set(oam_direct) | set(rec_why)
    ends = set(o + oam_len(rom, o) for o in set(oam_rec) | set(oam_direct))
    for o in sorted(set(oam_rec) | set(oam_direct)):
        e = o + oam_len(rom, o)
        if o not in oam_rec and not (e in starts or o in ends):
            continue
        if clean(o, e):
            coincidence.append((o, e, "oam", "BuildOam template stream (src/early_1b08.c), %s"
                                % ("the .oamTemplate of a TaskGfx record" if o in oam_rec
                                   else "frame table 0x%08X, chained" % oam_direct[o])))
    return {
        "coincidence": coincidence,
        "pointer": sorted(pointer.items()),
    }


