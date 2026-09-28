"""Pointer-census evidence: the sprite sheets of level_graphics_palettes (seg 12)
and m4a_songs (issue #36 phase 2 run 3, tools/ptrcensus.py provider).

census_sprites.py proves the sprite frame network that the frame tables and
the frame lists reach.  This provider covers what those walks leave in the
two zones, from consumers first and from formats only where no consumer
exists:

  1. consumer-read sources outside the frame network: LZ77 streams (the
     picture screens, the stage GfxHeaders, TransferNode mode 8, direct
     loads), palettes and raw tile blocks whose size the consumer gives, and
     the four raw-tiles sheet headers the code reads (their .tiles words are
     consumer-proven pointers, raw in the ROM before this run);
  2. the OAM template streams of two QueueSprite tables (consumer);
  3. format only: struct TaskGfx record blocks {records; u32 count; u32
     trailer} whose count word equals the number of records, some records of
     which no frame table reaches (their fields are format pointers), and
     runs of OAM template streams (most of them reached by nothing) that
     tile the bytes between two proven objects exactly, every entry checked
     against BuildOam's flipped-x pair.

    provide(rom, cfg, segs) -> {"coincidence": [...], "pointer": [...]}

Everything is derived from the ROM and the config's labels; every start
address below cites its consumer.
"""

import bisect
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import census_rooms as cr    # noqa: E402  (tools/census_rooms.py: lz77())
import census_sprites as cs  # noqa: E402  (tools/census_sprites.py)

ROM_BASE = 0x08000000
ZONES = ("level_graphics_palettes", "m4a_songs")


def _u16(rom, a):
    return struct.unpack_from("<H", rom, a - ROM_BASE)[0]


def _u32(rom, a):
    return struct.unpack_from("<I", rom, a - ROM_BASE)[0]


def _labels(cfg):
    out = set()
    for key in ("data_symbols", "extra_labels"):
        for k in cfg.get(key, {}):
            a = int(k, 16)
            if ROM_BASE <= a < 0x08800000:
                out.add(a)
    return sorted(out)


def _next_label(labels, a):
    i = bisect.bisect_right(labels, a)
    return labels[i] if i < len(labels) else None


# ---- 1. consumer-read LZ77 sources ------------------------------------------
# u32 gUnk_087319C8[][3] (include/mode.h:103): sub_08008d98 (src/gfx_08b8c.c:
# 109-111) copies [0] as a 64-byte BG palette (RequestCopy(2, [0], gBgPalette,
# 64)) and LZ77UnCompVram-s [1] (tiles) and [2] (map).  Rows: the span to the
# next label (a next-label pointer table, docs/data.md 5.1).
PICTURE_TABLE = 0x087319C8
# u32 gUnk_08731980[][2][2] (include/mode.h:101): sub_08008d10 (src/gfx_08b8c.c:
# 96-100) LZ77UnCompWram-s every non-zero element.
OBJ_LZ_TABLE = 0x08731980
# struct GfxHeader *const gUnk_08731F78[] (include/cutscene.h:62): sub_080102c0
# (src/mode_100ac.c:88-93) LZ77UnCompWram-s h->tiles and uses tileCount << 5
# bytes of it, and copies paletteBankCount << 5 bytes of h->palette.
STAGE_GFX = 0x08731F78
# struct TransferNode *gUnk_0873185C[] (include/mode.h:100, LoadGfxSet
# src/gfx_08b8c.c:67): RequestCopyList (src/early_1518.c:104) walks {cmd =
# size << 8 | mode, src, dst} until cmd == 0; mode 8 LZ77UnCompVram-s src
# (l.203-204).  census_rooms.py covers the raw-copy modes 1-5.
GFX_SETS = 0x0873185C
DIRECT_LZ77 = {
    0x085CCB58: "LZ77UnCompVram(gUnk_085CCB58, 0x06001800) sub_08008d98 src/gfx_08b8c.c:112-113",
    0x085E0090: "LZ77UnCompWram(gUnk_085E0090, ..) src/ending_c9004.c:90, sub_080102c0 src/mode_100ac.c:98-99",
    0x085E2CE0: "LZ77UnCompWram(gUnk_085E2CE0, ..) src/gameover_cacf0.c:260",
    0x085E4064: "LZ77UnCompWram(gUnk_085E4064, ..) src/gameover_cacf0.c:262",
    0x085E5BC4: "LZ77UnCompWram(gUnk_085E5BC4, ..) src/gameover_cacf0.c:265",
}


def _lz77_sources(rom, labels):
    """[(src, why, expected decoded size or None)] of the consumer tables."""
    out = []
    end = _next_label(labels, PICTURE_TABLE)
    for i, a in enumerate(range(PICTURE_TABLE, end - 11, 12)):
        for j in (1, 2):
            v = _u32(rom, a + 4 * j)
            if v:
                out.append((v, "gUnk_087319C8[%d][%d], LZ77UnCompVram sub_08008d98 src/gfx_08b8c.c:%d"
                            % (i, j, 109 + j), None))
    end = _next_label(labels, OBJ_LZ_TABLE)
    for k, a in enumerate(range(OBJ_LZ_TABLE, end, 4)):
        v = _u32(rom, a)
        if v:
            out.append((v, "gUnk_08731980[%d][%d][%d], LZ77UnCompWram sub_08008d10 src/gfx_08b8c.c:96-100"
                        % (k // 4, k // 2 % 2, k % 2), None))
    end = _next_label(labels, STAGE_GFX)
    for i, a in enumerate(range(STAGE_GFX, end, 4)):
        h = _u32(rom, a)
        if h:
            out.append((_u32(rom, h + 12), "gUnk_08731F78[%d]->tiles (GfxHeader 0x%08X), LZ77UnCompWram "
                        "sub_080102c0 src/mode_100ac.c:91" % (i, h), _u16(rom, h + 2) * 32))
    end = _next_label(labels, GFX_SETS)
    for lst in sorted(set(_u32(rom, a) for a in range(GFX_SETS, end, 4) if _u32(rom, a))):
        a = lst
        while _u32(rom, a):
            if _u32(rom, a) & 0xF == 8:
                out.append((_u32(rom, a + 4), "TransferNode 0x%08X {mode 8} source, LZ77UnCompVram "
                            "(RequestCopyList src/early_1518.c:203)" % a, None))
            a += 12
    for v, why in sorted(DIRECT_LZ77.items()):
        out.append((v, why, None))
    return out


# ---- 1b. palettes and raw tile blocks whose size the consumer gives ----------
SIZED = [
    (0x085E0070, 32, "palette", "RequestCopy(2, gUnk_085E0070, .., 32) src/ending_c9004.c:89, src/player_19000.c:490/535/575"),
    (0x085E2920, 256, "palette", "BlendColors(gUnk_085E2920, .., 128 colours) src/ending_c9004.c:428-440, src/player_19eec.c:55-70"),
    (0x085E2A20, 256, "palette", "BlendColors(.., gUnk_085E2A20, 128 colours) src/ending_c9004.c:440, src/player_19eec.c:55-63"),
    (0x085E2B20, 256, "palette", "BlendColors(.., gUnk_085E2B20, 128 colours) src/ending_c9004.c:428-434, src/player_19eec.c:70"),
    (0x085E2C20, 192, "palette", "RequestCopy(2, gUnk_085E2C20, .., 192) src/gameover_cacf0.c:258"),
    (0x081BE6BC, 512, "palette",
     "gUnk_081BE6BC[playerIndex * 128 + 0..127]: BlendColors of 16 colours from +0/+32/+64/+96 "
     "(src/player_47fe8.c:742-759, src/player_449c8.c:66-67) and RequestCopy(2, .., 32) from +0 "
     "(src/stage_3cd60.c:771); playerIndex is InitPlayerState's slot (src/stage_3cd60.c:188, callers loop "
     "i <= 3, src/mode_0b44c.c:70), gPlayerStates holds 4 records of 116 bytes (0x03002170-0x03002340)"),
    (0x081F1AE0, 512, "raw-tiles", "RequestCopy(1, gUnk_081F1AE0 + k * 128, .., 128), k = 0..3 src/player_49738.c:91-94"),
    (0x081CC328, 1024, "raw-tiles", "RequestCopy(1, gUnk_081CC328 + k * 256, .., 256), k = 0..3 src/player_49738.c:51-54"),
    (0x081AC358, 32, "palette", "RequestCopy(2, gUnk_081AC358, gObjPalette, 32) src/player_10358.c:248"),
    (0x081AC378, 512, "raw-tiles", "RequestCopy(1, gUnk_081AC378 + k * 128, .., 128), k = 0..3 src/player_10358.c:244-247"),
    (0x08334480, 64, "palette", "BlendColors(gUnk_08334480, gUnk_08334480 + 16, .., 16 colours, ..) src/enemy_a93ec.c:565/568"),
    (0x083344C0, 64, "palette", "RequestCopy(2, gUnk_083344C0 + (((gFrameCount >> 1) & 1) << 5), .., 32) src/enemy_a93ec.c:569/593/636"),
    (0x0826A668, 32, "palette",
     "ActorFlashPalette(&gUnk_0826A668, 16) (src/enemy_9000c.c:128/637) -> ActorLoadPalette(src, 16 << 1, 1) "
     "-> RequestCopy(2, src, .., 32) (src/actor_653ec.c:1033/1281)"),
]
# struct M12Fade gUnk_0873B510[] (include/player.h:47-52): BlendColors(f->unk0,
# f->unk4, .., 16 colours, ..) for f = &gUnk_0873B510[Task.unk2C]
# (src/player_455c8.c:217-221), Task.unk2C in {0, 1, 2} (l.77-125) or -1.
FADE_TABLE, FADE_ROWS = 0x0873B510, 3
# Palette-variant records {u32 pal[4]; u32 offset; u32 count;} (config entry
# 0x0873EF74's targets): sub_08065d44 (src/actor_653ec.c:584-627) copies, for
# actorKind 0, count << 1 bytes from pal[level - 1] + (offset << 1), level
# 1..3 (l.605-626).
VARIANT0_TABLE = 0x0873EF74
# actorKind 1: gUnk_0873F118[unk76] (src/actor_653ec.c:559/599/810), with the
# colour count Actor.paletteColorCount (src/actor_6b2e4.c:409), which
# ActorLoadPalette sets to the byte count >> 1 of the frame's TaskGfx palette
# (src/actor_653ec.c:1031, 1282).  Row 0 is Bonkers': for actorKind 1 the same
# unk76 picks gMidBossGfx[unk76] (src/actor_653ec.c:843-844), and
# gMidBossGfx[0] is gBonkersGfx, whose task draws gBonkersFrames
# (src/enemy_9000c.c:73).
VARIANT1_TABLE, BONKERS_ROW, BONKERS_FRAMES = 0x0873F118, 0, 0x08752ED8
MIDBOSS_GFX, BONKERS_GFX = 0x0873F0E4, 0x0826A654

# ---- 1c. the four raw-tiles sheet headers the code reads --------------------
# {u16 banks, u16 tileCount, u32, palette, tiles, trailer}; fields read ->
# (field offset, size in bytes from the header) per consumer.
RAW_HEADERS = {
    0x0824A9E4: ("struct GfxSrc gUnk_0824A9E4 (include/actor.h:16/58): RequestCopy(2, .unk08, .., unk00 << 5) "
                 "and RequestCopy(3, .unk0C, .., unk02 << 5) src/actor_6ff24.c:446-450", "banks"),
    0x082FEFF4: ("sub_080ae4c4 src/enemy_ae3bc.c:150-152: RequestCopy(4, q[3], .., q[1] << 5) and "
                 "RequestCopy(2, q[2], .., q[0] << 5)", "banks"),
    0x082FFDF0: ("sub_080b0b04 src/enemy_ae3bc.c:2306-2308: RequestCopy(4, q[3], .., q[1] << 5) and "
                 "RequestCopy(2, q[2], .., 32)", 32),
    0x08334DC0: ("sub_08066f78 src/actor_653ec.c:1670-1673: RequestCopy(4, h->tiles, .., h->tileCount << 5) "
                 "(its palette comes from gUnk_0873E264[gLevelIndex])", None),
}
# The palette entry says how many bytes of .palette the consumer copies:
# "banks" = paletteBankCount << 5, a number, or None (.palette not read).

# ---- 2. OAM streams of QueueSprite tables -----------------------------------
# u32 [] tables whose element QueueSprite stores as the OAM template stream
# BuildOam reads (src/early_1518.c:361, src/early_1b08.c:65).  Both are read
# with the same index d: 0, then 2 or 3 by facing, then + 2 while d <= 9
# (src/player_3bde8.c:490-500, src/player_109c8.c:39/61-64), so d <= 11.
QUEUE_TABLES = {
    0x08732104: "gUnk_08732104[d] QueueSprite source, sub_080109c8 src/player_109c8.c:71",
    0x0873A964: "gUnk_0873A964[d] QueueSprite source, src/player_3bde8.c:511",
}
QUEUE_COUNT = 12

# ---- 3. format only: BuildOam's flipped-x pair: census_sprites.strict_oam_len
strict_oam_len = cs.strict_oam_len


def provide(rom, cfg, segs):
    labels = _labels(cfg)
    zones = [(s, e) for s, e, _k, n in segs if n in ZONES]

    def inzone(a):
        return any(s <= a < e for s, e in zones)

    base = cs.provide(rom, cfg, segs)
    ptr_words = set(a for a, _w in base["pointer"])
    pointer = {}
    coin = []

    def claim(s, e, kind, why):
        # a claimed non-pointer range must not hold a proven pointer word
        bad = [w for w in range((s + 3) & ~3, e - 3, 4) if w in ptr_words or w in pointer]
        if bad:
            raise ValueError("0x%08X-0x%08X (%s) holds the pointer word 0x%08X" % (s, e, why, bad[0]))
        coin.append((s, e, kind, why))

    # 1. LZ77 sources: the stream decodes to its declared size and ends at the
    #    next label (at most 3 bytes of alignment before it), as census_rooms.py
    #    requires in seg 11/14; a GfxHeader's must also decode to tileCount * 32
    for src, why, want in _lz77_sources(rom, labels):
        if not inzone(src):
            continue
        x = cr.lz77(rom, src)
        nxt = _next_label(labels, src)
        if not x or not nxt or not nxt - 3 <= x[0] <= nxt or (want is not None and x[1] != want):
            continue
        claim(src, x[0], "lz77", "LZ77 stream (%s): decodes to its declared 0x%X bytes and ends at the "
              "next label" % (why, x[1]))
    # 1b. consumer-sized palettes and tiles
    end = _next_label(labels, PICTURE_TABLE)
    for i, a in enumerate(range(PICTURE_TABLE, end - 11, 12)):
        v = _u32(rom, a)
        if v and inzone(v):
            claim(v, v + 64, "palette", "gUnk_087319C8[%d][0], RequestCopy(2, .., gBgPalette, 64) sub_08008d98 "
                  "src/gfx_08b8c.c:109" % i)
    end = _next_label(labels, STAGE_GFX)
    for i, a in enumerate(range(STAGE_GFX, end, 4)):
        h = _u32(rom, a)
        if h and _u32(rom, h + 8) and inzone(_u32(rom, h + 8)):
            p = _u32(rom, h + 8)
            claim(p, p + 32 * _u16(rom, h), "palette", "gUnk_08731F78[%d]->palette (GfxHeader 0x%08X), "
                  "paletteBankCount << 5 bytes, sub_080102c0 src/mode_100ac.c:93" % (i, h))
    for s, n, kind, why in SIZED:
        claim(s, s + n, kind, why)
    for k in range(FADE_ROWS):
        r = FADE_TABLE + 12 * k
        for off in (0, 4):
            p = _u32(rom, r + off)
            if p and inzone(p):
                claim(p, p + 32, "palette", "gUnk_0873B510[%d].unk%d, BlendColors 16 colours "
                      "src/player_455c8.c:217-221" % (k, off))
    end = _next_label(labels, VARIANT0_TABLE)
    for a in range(VARIANT0_TABLE, end, 4):
        r = _u32(rom, a)
        if not r:
            continue
        off, n = _u32(rom, r + 16) << 1, _u32(rom, r + 20) << 1
        for k in range(3):
            p = _u32(rom, r + 4 * k)
            if p and n and inzone(p):
                claim(p + off, p + off + n, "palette",
                      "palette-variant record 0x%08X (gUnk_0873EF74 row 0x%X) pal[%d]: sub_08065d44 copies "
                      "count << 1 = %d bytes from pal[level - 1] + (offset << 1) (src/actor_653ec.c:605-627)"
                      % (r, (a - VARIANT0_TABLE) // 4, k, n))
    # Bonkers' variant palettes: every palette gBonkersFrames' records name
    # has the same byte count, so paletteColorCount << 1 is that count
    if _u32(rom, MIDBOSS_GFX + 4 * BONKERS_ROW) != BONKERS_GFX:
        raise ValueError("gMidBossGfx[%d] is not gBonkersGfx" % BONKERS_ROW)
    fend = _next_label(labels, BONKERS_FRAMES)
    counts = set()
    for a in range(BONKERS_FRAMES, fend, 4):
        g = _u32(rom, a)
        if g:
            if cs.taskgfx(rom, g, False) is None:
                raise ValueError("gBonkersFrames entry 0x%08X is no TaskGfx record" % a)
            if _u32(rom, g + 4):
                counts.add(_u16(rom, _u32(rom, g + 4)))
    if len(counts) == 1:
        n = counts.pop()
        r = _u32(rom, VARIANT1_TABLE + 4 * BONKERS_ROW)
        for k in range(3):
            p = _u32(rom, r + 4 * k)
            if p and inzone(p):
                claim(p, p + n, "palette",
                      "palette-variant record 0x%08X (gUnk_0873F118[%d], Bonkers) pal[%d]: sub_08065d44 / "
                      "ActorLoadPalette copy paletteColorCount << 1 = %d bytes (src/actor_653ec.c:1279, "
                      "src/actor_6b2e4.c:409), the byte count of every palette of gBonkersFrames' records"
                      % (r, BONKERS_ROW, k, n))
    # 1c. the raw-tiles sheet headers the code reads
    for h, (why, palsize) in sorted(RAW_HEADERS.items()):
        banks, tcount = _u16(rom, h), _u16(rom, h + 2)
        pal, til = _u32(rom, h + 8), _u32(rom, h + 12)
        if pal + banks * 32 != til:
            raise ValueError("raw sheet header 0x%08X: palette does not end at the tiles" % h)
        pointer[h + 12] = "sheet header 0x%08X .tiles, read by %s" % (h, why)
        claim(til, til + tcount * 32, "raw-tiles", "sheet header 0x%08X .tiles, tileCount %d * 32 bytes: %s"
              % (h, tcount, why))
        if palsize is not None:
            n = banks * 32 if palsize == "banks" else palsize
            pointer[h + 8] = "sheet header 0x%08X .palette, read by %s" % (h, why)
            claim(pal, pal + n, "palette", "sheet header 0x%08X .palette, %d bytes: %s" % (h, n, why))

    # 2. OAM streams of the QueueSprite tables (strict parse)
    for t, why in sorted(QUEUE_TABLES.items()):
        if _next_label(labels, t) < t + 4 * QUEUE_COUNT:
            raise ValueError("%s: the table is shorter than its index range" % why)
        for i, a in enumerate(range(t, t + 4 * QUEUE_COUNT, 4)):
            v = _u32(rom, a)
            if v and inzone(v):
                n = strict_oam_len(rom, v)
                if n is None:
                    raise ValueError("%s: entry %d (0x%08X) is no OAM stream" % (why, i, v))
                claim(v, v + n, "oam", "BuildOam template stream (src/early_1b08.c): %s, entry %d (d <= 11), "
                      "strict parse" % (why, i))

    # 3a. record blocks: a run of struct TaskGfx records followed by a u32
    #     count equal to the run's length and a trailer word (0 here; the
    #     frame-list trailers are census_sprites.py's), with at least one
    #     record a frame table reaches.  The other records' fields are format
    #     pointers and their targets format-only objects.
    rec_starts = sorted(a for a in ptr_words if inzone(a) and a % 4 == 0
                        and cs.taskgfx(rom, a, False) is not None)
    rec_set = set(rec_starts)
    seen = set()
    new_recs = []
    blocks = []
    for r in rec_starts:
        if r in seen:
            continue
        lo = r
        while cs.taskgfx(rom, lo - 12, False) is not None and inzone(lo - 12):
            lo -= 12
        hi = r
        while cs.taskgfx(rom, hi, False) is not None and inzone(hi):
            hi += 12
        n = (hi - lo) // 12
        for q in range(lo, hi, 12):
            seen.add(q)
        if _u32(rom, hi) != n or _u32(rom, hi + 4) != 0:
            continue
        missing = [q for q in range(lo, hi, 12) if q not in rec_set]
        blocks.append((lo, hi, n))
        for q in missing:
            if strict_oam_len(rom, _u32(rom, q)) is None:
                raise ValueError("record 0x%08X of block 0x%08X: .oamTemplate fails the strict parse" % (q, lo))
            new_recs.append((q, lo, n))
    for q, lo, n in new_recs:
        tag = ("format only: record %d of the struct TaskGfx block 0x%08X (%d records, then the count "
               "word %d and a zero trailer; %d of them reached by a frame table)"
               % ((q - lo) // 12, lo, n, n, len([x for x in range(lo, lo + 12 * n, 12) if x in rec_set])))
        f = [_u32(rom, q + 4 * i) for i in range(3)]
        for i, v in enumerate(f):
            if v:
                pointer[q + 4 * i] = tag + ", field +0x%X" % (4 * i)
    for q, lo, n in new_recs:
        tag = "format only: struct TaskGfx record 0x%08X (block 0x%08X, count %d)" % (q, lo, n)
        o, p, t = [_u32(rom, q + 4 * i) for i in range(3)]
        claim(o, o + strict_oam_len(rom, o), "oam", tag + ": .oamTemplate, strict BuildOam parse")
        if p:
            claim(p, p + cs.palette_len(rom, p), "palette", tag + ": .palette {u16 byteCount; colours}")
        claim(t, t + cs.tiles_len(rom, t), "tiles", tag + ": .tiles chunk stream ended by 0xFFFF")

    # 3b. chained OAM streams: from the end of a proven sprite object, strict
    #     streams tile the bytes exactly up to the start of the next proven
    #     object (at most 3 zero bytes of alignment before a 4-aligned one)
    objs = [(s, e) for s, e, _k, _w in base["coincidence"] + coin if inzone(s)]
    objs += [(lo, hi + 8) for lo, hi, _n in blocks]
    obj_starts = set(s for s, _e in objs) | rec_set
    obj_ends = sorted(set(e for _s, e in objs) | set(r + 12 for r in rec_set))
    have = sorted(objs)
    hs = [s for s, _e in have]

    def covered(a):
        i = bisect.bisect_right(hs, a) - 1
        return any(s <= a < e for s, e in have[max(0, i - 64):i + 1])

    for s in obj_ends:
        if not inzone(s) or covered(s) or s in obj_starts:
            continue
        p, streams = s, []
        while True:
            n = strict_oam_len(rom, p)
            if n is None:
                break
            streams.append((p, p + n))
            p += n
            if p in obj_starts:
                break
            pad = (4 - p % 4) % 4
            if pad and p + pad in obj_starts and not any(rom[p - ROM_BASE:p + pad - ROM_BASE]):
                break
            if covered(p):
                p = None
                break
        pad_end = p is not None and (p in obj_starts or (p + (4 - p % 4) % 4) in obj_starts)
        if not streams or not pad_end:
            continue
        for a, b in streams:
            claim(a, b, "oam", "format only: BuildOam template stream (src/early_1b08.c), strict parse; "
                  "the streams 0x%08X-0x%08X tile the bytes between two proven objects exactly" % (s, p))
    return {"coincidence": coin, "pointer": sorted(pointer.items())}
