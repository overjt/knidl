"""Pointer-census evidence: the sprite frame network (issue #36 phase 2 run 2,
tools/ptrcensus.py provider).

The sprite frame network: the frame tables at 0x0874C44C-0x08756000 (seg 18
tail), the struct TaskGfx records they point at (seg 12, sprite_sheets_2, seg 18,
seg 19 head) and the OAM template streams, tile streams and palettes those
records and tables name.

    provide(rom, cfg, segs) -> {"coincidence": [...], "pointer": [...]}

Everything is derived from the ROM and the config's labels; the constants
below are consumer facts, cited next to them.

Formats (all from decompiled consumers):
  * frame table: an array of pointers indexed by Task.frame
    (Task.frameTable, include/task.h; read by TaskDrawWorld and friends,
    src/task_draw_world.c:85, and the actor draws, src/actor_helpers.c:104/133),
    or by a direct index (QueueSprite(..., gUnk_X[i], ...)).  Each entry is
    either an OAM template stream (the table's consumer hands it to
    QueueSprite) or a struct TaskGfx * (its consumer reads the record).
  * OAM template stream: BuildOam (src/main_build_oam.c) reads 8-byte entries
    {attr0, attr1, attr1_flipped, attr2} up to and including the one whose
    attr0 has bit 12 set.  No pointers.
  * struct TaskGfx {u32 oamTemplate; u16 *palette; u16 *tiles}
    (include/task.h): TaskLoadFrameTiles (src/task_frame_tiles.c:82-107),
    sub_08065470 (src/actor_helpers.c:89-121), sub_0801a310
    (src/cutscene_fountain_kirby_draw.c:135).  PlayerLoadFrameTilesAndPalette
    (src/player_helpers.c:479-541) adds the tagged variant: bit 0 of
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
# (src/cutscene_nightmare_power_orb_escape.c:70 `t->frameTable = gUnk_0874C44C`) follows the Whispy
# Woods behaviour tables (agent C / B); 0x08756000 is agent C's zone.  Every
# label in between names a frame table (consumer list: REPORT.md section 1).
FT_LO, FT_HI = 0x0874C44C, 0x08756000

# Frame tables whose entries are struct TaskGfx *, with the draw callback
# that reads them (start -> (tagged, consumer)).  A table not listed here
# holds OAM template pointers.
GFX_TABLES = {
    0x0874CFEC: (True, "Task_Player src/player_task.c:80, draw sub_0803ddc0 -> PlayerLoadFrameTilesAndPalette"),
    0x087519E8: (True, "src/player_object_laser_beam.c:50, draw sub_0803dfc8 -> PlayerLoadFrameTilesAndPalette"),
    0x08751A28: (True, "src/player_object_laser_beam.c:192, draw sub_0803dfc8"),
    0x08751B40: (True, "src/player_object_ice_breath.c:59, draw sub_0803dfc8"),
    0x08751C44: (True, "src/effect_skid_dust.c:71, draw sub_08053c1c -> sub_0803dfc8"),
    0x08751CBC: (False, "src/effect_fire_breath_spark_aura.c:145, draw TaskDrawWorldLoadTiles/TilesLoaded"),
    0x08751D80: (True, "src/effect_burning_flames_wheel.c:293, draw sub_0805af80 -> sub_0803dfc8"),
    0x08751DBC: (True, "src/effect_mike_attack.c:397, draw sub_0803dfc8"),
    0x08751ECC: (True, "src/effect_hi_jump_ball.c:274, draw sub_0803dfc8 / TaskDrawWorldTilesLoaded"),
    0x08752090: (True, "src/effect_crash_blast.c:510, draw sub_0805af80 -> sub_0803dfc8"),
    0x08752ED8: (False, "gBonkersFrames src/enemy_bonkers.c:73, draw sub_08065438 -> sub_08065470"),
    0x08752F74: (False, "gGrandWheelieFrames src/enemy_grand_wheelie.c:159, draw sub_08065438"),
    0x08753090: (False, "gMrFrostyFrames src/enemy_mr_frosty.c:340, draw sub_08065438"),
    0x08753180: (False, "gMrTickTockFrames src/enemy_mr_tick_tock.c:401, draw sub_08065438"),
    0x087535FC: (False, "gBugzzyFrames src/enemy_poppy_bros_sr.c:743, draw sub_080653ec -> sub_08065470"),
    0x08753718: (False, "gFireLionFrames src/enemy_fire_lion.c:42, draw sub_08065438"),
    0x087537FC: (False, "gPhanPhanFrames src/enemy_phan_phan.c:44, draw sub_08065438"),
    0x087538E0: (False, "gKingDededeFrames src/enemy_king_dedede_damage.c:69, draw sub_08065438"),
    0x08753994: (False, "gPaintRollerFrames src/enemy_nightmare_wizard_heavy_mole.c:1963, draw sub_08065438"),
    0x08753BB4: (False, "gMetaKnightFrames src/enemy_mr_shine_and_mr_bright.c:3507, draw sub_080653ec"),
    0x08754180: (False, "src/enemy_mr_shine_and_mr_bright.c:2685, draw sub_080a488c (struct TaskGfx *)"),
    0x087541B0: (False, "src/enemy_mr_shine_and_mr_bright.c:2736, draw sub_080a488c"),
    0x087541E0: (False, "src/enemy_mr_shine_and_mr_bright.c:2733, draw sub_080a488c"),
    0x08754568: (False, "src/enemy_nightmare_wizard_heavy_mole.c:998, draw sub_080a9ed8 (struct TaskGfx *)"),
    0x0875456C: (False, "gNightmareWizardFrames src/enemy_kracko_cloud_lightning.c:661, draw sub_080a9ed8"),
    0x087546D0: (False, "src/enemy_kracko_cloud_lightning.c:581 `(struct TaskGfx *)gUnk_087546D0[...]`"),
    0x087546F8: (False, "src/enemy_nightmare_wizard_heavy_mole.c:1476, draw sub_080aa16c (struct TaskGfx *)"),
    0x08754708: (False, "src/enemy_nightmare_wizard_heavy_mole.c:1233, draw sub_080aa16c"),
    0x08754718: (False, "src/enemy_nightmare_wizard_heavy_mole.c:1334, draw sub_080aa16c"),
    0x08755068: (True, "src/cutscene_fountain_kirby_king_dedede.c:18, draw sub_0801a1ec -> PlayerLoadFrameTilesAndPalette"),
    0x08755378: (False, "src/cutscene_fountain_kirby_king_dedede.c:623, draw sub_0801a310"),
    0x087553FC: (False, "src/cutscene_fountain_power_orb_star_rod.c:271, draw sub_0801a310"),
}
# gUnk_08751F84: entries 0-1 are TaskGfx records (sub_08059d7c,
# src/effect_hi_jump_ball.c:333, draw sub_0803dfc8), entries 2-17 OAM streams.
GFX_PREFIX = {0x08751F84: 2}
# Frame tables whose entries are neither OAM streams nor TaskGfx: their
# words are pointers, their targets have another format.
OTHER_TABLES = {
    0x087555FC: "struct GfxHeader * (include/hud.h:87; Task_IntroStoryPicture src/hud_init_counters.c:35)",
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


# ---- the strict parse: BuildOam's flipped-x pair --------------------------------
# BuildOam (src/main_build_oam.c:66-81) reads an 8-byte entry {attr0, attr1,
# attr1 for an x-flipped sprite, attr2}.  The exporter writes the flipped word
# as attr1 with the h-flip bit toggled and x mirrored across the sprite's
# width: (attr1 ^ 0x1000) & 0xFE00 == attr1_flipped & 0xFE00 and x + x_flipped
# + width == 0 (mod 512).  48,293 of the 48,298 entries of the consumer-proven
# streams satisfy it (the other five were one palette, 0x085F5034, that the
# chained rule below accepted before it required this parse); random bytes pass with a probability near 1e-7 per entry.
OBJ_WIDTH = {(0, 0): 8, (0, 1): 16, (0, 2): 32, (0, 3): 64,
             (1, 0): 16, (1, 1): 32, (1, 2): 32, (1, 3): 64,
             (2, 0): 8, (2, 1): 8, (2, 2): 16, (2, 3): 32}


def _entry_ok(rom, a):
    a0, a1, a1f = _u16(rom, a), _u16(rom, a + 2), _u16(rom, a + 4)
    shape = a0 >> 14
    if shape == 3 or a0 & 0x2F00:   # no affine/disable, mode or 256-colour bits
        return False
    if (a1f ^ 0x1000) & 0xFE00 != a1 & 0xFE00:
        return False
    return (a1 + a1f + OBJ_WIDTH[(shape, a1 >> 14)]) & 0x1FF == 0


def strict_oam_len(rom, a, lim=None):
    """Bytes of the OAM stream at a when every entry passes the flipped-x
    check (and the stream ends before lim), else None."""
    if a & 1 or not ROM_BASE <= a < ROM_BASE + len(rom) - 8:
        return None
    p = a
    for _ in range(128):
        if lim is not None and p + 8 > lim:
            return None
        if not _entry_ok(rom, p):
            return None
        p += 8
        if _u16(rom, p - 8) & 0x1000:
            return p - a
    return None


def provide(rom, cfg, segs):
    labels = set(int(k, 16) for k in cfg.get("data_symbols", {}))
    labels |= set(int(k, 16) for k in cfg.get("extra_labels", {}))
    bounds = sorted(a for a in labels if FT_LO <= a < FT_HI) + [FT_HI]
    if not bounds or bounds[0] != FT_LO:
        raise ValueError("frame-table labels missing from the config")
    pointer = {}
    rec_why = {}
    oam_direct = {}   # stream start -> table start (consumer-proven frames)
    oam_rec = set()   # .oamTemplate streams of records
    tiles = set()
    pals = set()

    for a, b in zip(bounds, bounds[1:]):
        ngfx = GFX_PREFIX.get(a, (b - a) // 4 if a in GFX_TABLES else 0)
        tagged, cons = GFX_TABLES.get(a, (True, "sub_08059d7c src/effect_hi_jump_ball.c:333"))
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
                                "(TaskLoadFrameTiles src/task_frame_tiles.c:96)"))
    for p in sorted(pals):
        e = p + palette_len(rom, p)
        if clean(p, e):
            coincidence.append((p, e, "palette", "TaskGfx palette {u16 byteCount; colours} "
                                "(src/task_frame_tiles.c:74, src/player_helpers.c:499)"))
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
        # a stream accepted by chaining alone must also pass the strict
        # parse: 0x085F5034, a palette of TransferNode 0x087316E8, chains
        # with its neighbours but is no OAM stream
        if o not in oam_rec and strict_oam_len(rom, o) != e - o:
            continue
        if clean(o, e):
            coincidence.append((o, e, "oam", "BuildOam template stream (src/main_build_oam.c), %s"
                                % ("the .oamTemplate of a TaskGfx record" if o in oam_rec
                                   else "frame table 0x%08X, chained" % oam_direct[o])))
    fmt_ptr, fmt_coin, sheet_hdrs = _frame_lists(rom, cfg, segs, labels, pointer,
                                                 oam_rec, oam_direct, rec_why)
    hp, hc = _sheet_headers(rom, sheet_hdrs)
    fmt_ptr.update(hp)
    fmt_coin.extend(hc)
    for a, why in fmt_ptr.items():
        pointer.setdefault(a, why)
    for s, e, k, why in fmt_coin:
        if clean(s, e):
            coincidence.append((s, e, k, why))
    return {
        "coincidence": coincidence,
        "pointer": sorted(pointer.items()),
    }


# ---- round 2: the per-sheet frame lists (format only, no code reads them) --
#
# sprite_frame_lists (0x087E2570, docs/data.md 4) and its twin at the head of
# seg 19, from the end of the player's tagged record block (the last record
# 0x0876923C, 20 bytes) to the next label gUnk_0876B1FC.  Each list is named
# by exactly one trailer word: +0x10 of a GfxHeader-shaped sheet header
# {u16 banks, u16 tileCount, u16 frameCount, u16 flag, palette, tiles} whose
# frameCount is the list length (OAM sheets), or the word after a u32 count
# that follows the sheet's struct TaskGfx records (record sheets).  An entry
# is a pointer when its target is a consumer-proven frame (above) or, if it
# has no label, when it is a frame of the sheet's block: the block chains
# byte for byte from the list's first consumer-proven frame to the header
# (OAM) or the count word (records).  Other entries (two point at the header
# itself) stay unknown.
LIST_TWIN_LO = 0x08769250
LIST_TWIN_HI = 0x0876B1FC  # gUnk_0876B1FC, the next table (labelled since phase 1)
# frames that code names directly (no frame table): gUnk_0824A9CC is the
# QueueSprite/DrawAffineSprite template of src/cutscene_warp_star.c:70/78
CODE_FRAMES = (0x0824A9CC,)


def _frame_lists(rom, cfg, segs, labels, pointer, oam_rec, oam_direct, rec_why):
    zones = [(s, e) for s, e, _k, n in segs if n == "sprite_frame_lists"]
    zones.append((LIST_TWIN_LO, LIST_TWIN_HI))
    data_segs = [(s, e) for s, e, k, _n in segs if k == "data"]
    holders = {}
    for s, e in data_segs:
        for w in range((s + 3) & ~3, e - 3, 4):
            v = _u32(rom, w)
            if any(a <= v < b for a, b in zones):
                holders.setdefault(v, []).append(w)
    proven_oam = set(oam_direct) | set(oam_rec) | set(CODE_FRAMES)
    proven_rec = set(rec_why)
    ptrs = {}
    coin = []
    sheet_hdrs = []
    starts = sorted(holders)
    for i, s in enumerate(starts):
        if len(holders[s]) != 1:
            raise ValueError("frame list 0x%08X has %d trailers" % (s, len(holders[s])))
        h = holders[s][0]
        zone = [z for z in zones if z[0] <= s < z[1]][0]
        nxt = [x for x in starts[i + 1:] if zone[0] <= x < zone[1]]
        e = nxt[0] if nxt else zone[1]
        vals = [_u32(rom, x) for x in range(s, e, 4)]
        n = len(vals)
        oam_sheet = _in_rom(rom, _u32(rom, h - 4)) and _u16(rom, h - 0x10) <= 16
        if oam_sheet:
            end_blk = h - 0x10
            if _u16(rom, end_blk + 4) != n:
                raise ValueError("0x%08X: frame count != list length" % h)
        else:
            end_blk = h - 4
            if _u32(rom, end_blk) != n:
                raise ValueError("0x%08X: record count != list length" % h)
        if not all(b > a for a, b in zip(vals, vals[1:])):
            raise ValueError("frame list 0x%08X is not increasing" % s)
        proven = proven_oam if oam_sheet else proven_rec
        prov = [v for v in vals if v in proven]
        block = {}
        if prov:
            p = min(prov)
            while p < end_blk:
                ln = oam_len(rom, p) if oam_sheet else None
                if not oam_sheet:
                    f = taskgfx(rom, p, False)
                    ln = 4 * len(f) if f else None
                if not ln:
                    break
                block[p] = ln
                p += ln
            if p != end_blk:
                block = {}
        tag = ("format only: frame list 0x%08X (%d entries, trailer 0x%08X: count "
               "== length, increasing, %d of %d consumer-proven)"
               % (s, n, h, len(prov), n))
        kept = 0
        for i2, v in enumerate(vals):
            # a frame of the block that had no label before round 2 is
            # format-proven; a labelled non-frame (the header) is not in it
            if v in proven or v in block:
                ptrs[s + 4 * i2] = tag
                kept += 1
                if not oam_sheet and v not in proven:
                    for j, (slot, sv) in enumerate(taskgfx(rom, v, False)):
                        if sv:
                            ptrs[slot] = tag + ", struct TaskGfx field +0x%X" % (4 * j)
        if kept and oam_sheet:
            sheet_hdrs.append(end_blk)
        if kept:
            ptrs[h] = ("format only: trailer word of the sheet ending 0x%08X, "
                       "the start of frame list 0x%08X (%d entries = its count)"
                       % (end_blk, s, n))
        for p, ln in block.items():
            if oam_sheet:
                coin.append((p, p + ln, "oam", tag + ": the sheet's chained OAM block"))
            else:
                f = [sv for _s, sv in taskgfx(rom, p, False)]
                o = f[0] & ~1
                coin.append((o, o + oam_len(rom, o), "oam", tag + ": record .oamTemplate"))
                if f[1]:
                    coin.append((f[1], f[1] + palette_len(rom, f[1]), "palette",
                                 tag + ": record .palette"))
                coin.append((f[2], f[2] + tiles_len(rom, f[2]), "tiles",
                             tag + ": record .tiles"))
    return ptrs, coin, sheet_hdrs


# ---- round 3: the sheet headers' palette/tiles fields (format only) -------
#
# The 85 trailer-bearing headers of the OAM sheets above, shaped like
# struct Unk0873EEA0 (include/enemy.h:13) {u16 paletteBankCount; u16
# tileCount; u16 frameCount; u16 tilesCompressed; palette; tiles}.  A field
# is a pointer when its target parses exactly:
#   .palette: paletteBankCount * 32 bytes that end exactly at .tiles;
#   .tiles (tilesCompressed != 0): a BIOS LZ77 (type 0x10) stream whose
#     header size and decoded length are both tileCount * 32.
# Raw tiles (tilesCompressed == 0) are not accepted: in all four such sheets
# the tileCount * 32 bytes are followed by tileCount * 32 zero bytes before
# the frame block, so they do not end at the next proven object.


def lz77_len(rom, a):
    """(stream bytes, decoded bytes) of the LZ77 stream at a, or None."""
    o = a - ROM_BASE
    if not 0 <= o < len(rom) - 4 or rom[o] != 0x10:
        return None
    size = rom[o + 1] | rom[o + 2] << 8 | rom[o + 3] << 16
    p = o + 4
    out = 0
    while out < size:
        if p >= len(rom):
            return None
        flags = rom[p]
        p += 1
        for bit in range(8):
            if out >= size:
                break
            if flags & (0x80 >> bit):
                b0, b1 = rom[p], rom[p + 1]
                p += 2
                if ((b0 & 0xF) << 8 | b1) + 1 > out:
                    return None
                out += (b0 >> 4) + 3
            else:
                p += 1
                out += 1
    if out != size:
        return None
    return p - o, size


def _sheet_headers(rom, hdrs):
    ptrs = {}
    coin = []
    for hdr in sorted(set(hdrs)):
        banks, tcount = _u16(rom, hdr), _u16(rom, hdr + 2)
        comp = _u16(rom, hdr + 6)
        pal, til = _u32(rom, hdr + 8), _u32(rom, hdr + 12)
        tag = "format only: sheet header 0x%08X (trailer-bearing)" % hdr
        if pal and banks and pal + banks * 32 == til:
            ptrs[hdr + 8] = tag + ": .palette = %d banks * 32 bytes ending at .tiles" % banks
            coin.append((pal, til, "palette", tag + ": palette banks"))
        if comp and tcount:
            r = lz77_len(rom, til)
            if r is not None and r[1] == tcount * 32:
                ptrs[hdr + 12] = (tag + ": .tiles = LZ77 stream (%d bytes) decoding "
                                  "to tileCount %d * 32 bytes" % (r[0], tcount))
                coin.append((til, til + r[0], "lz77", tag + ": LZ77 tile stream"))
    return ptrs, coin


