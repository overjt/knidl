"""Pointer-census evidence: rooms, BG animations, GfxHeaders, ActorDefs
(issue #36 phase 2 run 2, tools/ptrcensus.py provider).

Zones: level_object_tables (seg 11), room_bg_anims, room_data,
room_metatiles, room_bg3_maps, compressed_graphics (seg 14), and the
ActorDef/ActorAux pointer fields in seg 18.  Everything is derived from the
ROM and the config; every start address listed here cites its consumer.

provide(rom, cfg, segs) -> {"coincidence": [(start, end, kind, why)],
                            "pointer": [(addr, why)]}
"""

import bisect
import struct

ROM_BASE = 0x08000000


# ---- BG animation scripts ------------------------------------------------
# BG animation scripts of room_bg_anims (src/camera_2d01c.c).
# 
# Consumer facts (src/camera_2d01c.c, include/camera.h):
# - LoadRoomBgAnims (l.176) copies gRoomBgAnimScripts[RoomDef.bgAnimSet][i]
#   (NULL-ended lists, room_bg_anim_lists) into gBgAnims[i].unk4: a script is
#   an array of 8-byte struct Unk02007D70Cmd {u16 op, u16 arg, void *ptr}.
# - UpdateBgAnims (l.205) walks it: op 0 -> sub_0802d25c(cmd->ptr), op 1 ->
#   sub_0802d278(p, cmd->ptr), op 2 wait, op 3 restart at command 0, op 5
#   sub_0802d2f0(arg >> 8, arg & 0xFF, (u16)cmd->ptr) (a VALUE), op 6
#   PlaySfx(arg), anything else stops the slot (sub_0802d32c).  So a script's
#   commands end at its first op 3 or op >= 7/op 4, and only ops 0/1 carry a
#   pointer.
# - sub_0802d25c (l.250): struct Unk0802D25C {u16 tile, u16 byteSize, tiles[]}
#   -> RequestCopy(1, tiles, 0x06004000 + tile*32, byteSize) (mode 1 = raw copy
#   of byteSize bytes, src/early_1518.c RequestCopy): a frame is 4 + byteSize
#   raw bytes.
# - sub_0802d278 (l.261): struct Unk0802D278 {u16 *from, u16 *to, u16 index,
#   u16 count, u32 rate} (16 bytes); sub_0802d294 (l.271) BlendColors(from,
#   to, t, count, &gUnk_030012B0[index]): from/to are palettes of count
#   colours (2*count bytes).

LISTS_INDEX = 0x087E1F20   # gRoomBgAnimScripts[], entries 0 and 13 NULL
LISTS_COUNT = 14


def bganim_parse(u16, u32):
    scripts = []
    for i in range(LISTS_COUNT):
        lst = u32(LISTS_INDEX + 4 * i)
        if not lst:
            continue
        j = 0
        while u32(lst + 4 * j):
            scripts.append(u32(lst + 4 * j))
            j += 1
    scripts = sorted(set(scripts))
    out = {"scripts": [], "frames": {}, "fades": {}, "palettes": {}, "op5": []}
    for s in scripts:
        c = s
        cmds = []
        while True:
            op = u16(c)
            cmds.append((c, op, u16(c + 2), u32(c + 4)))
            if op not in (0, 1, 2, 5, 6):
                break
            c += 8
        out["scripts"].append((s, len(cmds), cmds))
        for (c, op, arg, ptr) in cmds:
            if op == 0:
                out["frames"][ptr] = ptr + 4 + u16(ptr + 2)
            elif op == 1:
                out["fades"][ptr] = ptr + 16
                n = u16(ptr + 10)
                for pal in (u32(ptr), u32(ptr + 4)):
                    e = pal + 2 * n
                    out["palettes"][pal] = max(out["palettes"].get(pal, e), e)
            elif op == 5:
                out["op5"].append(c + 4)
    return out


# ---- room records ----------------------------------------------------------
# Room records (room_data, room_metatiles, room_bg3_maps and the RoomDef
# targets in compressed_graphics): proven byte extents from the consumers.
# 
# RoomDef (include/room.h, 0x58 bytes) fields and their consumers:
#   +0x08 metatileMap   src/level_242d0.c:223-231 (sub_08024698): RequestCopy(8)
#         = LZ77UnCompVram when mapsCompressed (+0x05) != 0, else CpuSet of
#         width*height*2 halfwords (4 bytes per cell, struct MapCell)
#   +0x0C blockLayer    same function: LZ77 or CpuSet of width*height halfwords
#   +0x10 unk10         the block table (struct MapTile[], src/block_30804.c:459)
#   +0x18/+0x28 palettes  src/camera_28b8c.c:301/307 LoadBg2Gfx/LoadBg3Gfx:
#         RequestCopy(2, pal + 1, dst, pal[0]) = CpuSet of pal[0] BYTES
#   +0x1C/+0x2C tiles, +0x20 metatileTiles  RequestCopy(8, ...) = LZ77
#         (src/camera_28b8c.c:300/306, src/level_242d0.c:229)
#   +0x30 bg3Map (struct BgMap {u16 size, u16 width, u16 height, u16 flag})
#         src/camera_28b8c.c:338 LoadBg3Map: LZ77 stream at +8 (size 0x1000);
#         src/bgmap_2a9cc.c:375 DrawBg3Tile: raw u16 map at +6, width*height
#   +0x44 doors: struct Door[doorCount (+0x3A)], 12 bytes (src/door_26b60.c:26)
#   +0x48 objects: 8-byte entries {s8 kind, s8 a, s8 b, s8 c, u16 x, u16 y},
#         objectCount (+0x3C) of them (src/hud_b5024.c:131-134)

ROOM_LISTS = (0x087E1F58, 0x087E2570)  # src/data/room_lists.c
POINTER_FIELDS = (0x08, 0x0C, 0x10, 0x18, 0x1C, 0x20, 0x28, 0x2C, 0x30, 0x44, 0x48)


def lz77(rom, a):
    """GBA BIOS LZ77 (type 0x10) at VMA a: (end VMA, declared size) when the
    stream decodes to its declared size with every back-reference inside the
    output, else None.  The BIOS copies a back-reference whole, so the last
    token may overshoot the size (by < 18 bytes); the stream still ends
    where that token's bytes end."""
    off = a - 0x08000000
    if off < 0 or off + 4 > len(rom) or rom[off] != 0x10:
        return None
    size = rom[off + 1] | rom[off + 2] << 8 | rom[off + 3] << 16
    if size == 0:
        return None
    p = off + 4
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
                disp = ((b0 & 0xF) << 8 | b1) + 1
                if disp > out:
                    return None
                out += (b0 >> 4) + 3
            else:
                p += 1
                out += 1
    if out - size >= 18:
        return None
    return (p + 0x08000000, size)


def room_items(rom, u8, u16, u32):
    """[(start, end, kind, why)] of every RoomDef target with a proven
    extent, plus each RoomDef's value fields; the field words themselves
    are returned as pointers."""
    rooms = sorted(set(u32(a) for a in range(ROOM_LISTS[0], ROOM_LISTS[1], 4)
                       if u32(a)))
    out = []
    ptrs = []
    seen = set()

    def add(s, e, kind, why):
        if (s, kind) not in seen:
            seen.add((s, kind))
            out.append((s, e, kind, why))

    for r in rooms:
        # value fields of the record itself
        cur = r
        for off in POINTER_FIELDS:
            if r + off > cur:
                add(cur, r + off, "roomdef-values", "RoomDef 0x%08X value fields" % r)
            cur = r + off + 4
            if u32(r + off):
                ptrs.append((r + off, "RoomDef+0x%02X (room lists targets)" % off))
        add(cur, r + 0x58, "roomdef-values", "RoomDef 0x%08X value fields" % r)
        comp = u8(r + 5)
        w, h = u16(r + 0x14), u16(r + 0x16)
        for off, bpc, name in ((0x08, 4, "metatile map"), (0x0C, 2, "block layer")):
            p = u32(r + off)
            if comp:
                x = lz77(rom, p)
                if x and x[1] == w * h * bpc:
                    add(p, x[0], "lz77", "RoomDef.%s LZ77 (%dx%d cells, %d bytes)" % (name, w, h, x[1]))
            else:
                add(p, p + w * h * bpc, "raw-map", "RoomDef.%s raw, %dx%dx%d bytes (CpuSet)" % (name, w, h, bpc))
        for off, name in ((0x18, "bg2Palette"), (0x28, "bg3Palette")):
            p = u32(r + off)
            if p:
                add(p, p + 2 + u16(p), "palette", "RoomDef.%s {u16 byteSize, colours}" % name)
        for off, name in ((0x1C, "bg2Tiles"), (0x20, "metatileTiles"), (0x2C, "bg3Tiles")):
            p = u32(r + off)
            x = lz77(rom, p) if p else None
            if x:
                add(p, x[0], "lz77", "RoomDef.%s LZ77 (%d bytes)" % (name, x[1]))
        m = u32(r + 0x30)
        if m:
            size, mw, mh = u16(m), u16(m + 2), u16(m + 4)
            x = lz77(rom, m + 8)
            if x and x[1] == size:
                add(m, x[0], "bgmap-lz77", "RoomDef.bg3Map header + LZ77 at +8 (%d bytes, LoadBg3Map)" % size)
            elif size == mw * mh * 2:
                add(m, m + 6 + size, "bgmap-raw", "RoomDef.bg3Map header + raw %dx%d u16 map at +6 (DrawBg3Tile)" % (mw, mh))
        d = u32(r + 0x44)
        if d:
            add(d, d + 12 * u16(r + 0x3A), "doors", "RoomDef.doors: struct Door[%d]" % u16(r + 0x3A))
        o = u32(r + 0x48)
        if o:
            add(o, o + 8 * u16(r + 0x3C), "objects", "RoomDef.objects: %d 8-byte entries" % u16(r + 0x3C))
    return rooms, out, ptrs


# ---- GfxHeaders ------------------------------------------------------------
# struct GfxHeader records (include/task.h) inside agent B's zones.
# 
# A GfxHeader is {u16 paletteBankCount, u16 tileCount, u32 unk04, void
# *palette, void *tiles}; every consumer below copies paletteBankCount << 5
# bytes from `palette` (RequestCopy mode 2) and LZ77-decompresses `tiles`
# (LZ77UnCompWram/Vram) before copying tileCount << 5 bytes of the result.
# The records here sit right after their own palette and LZ77 tiles; each
# stream decodes to exactly tileCount * 32 bytes (checked in records()).

# address -> (consumer, why)
HEADERS = {
    0x0854BB60: "gUnk_087555FC[0-2] (include/hud.h struct GfxHeader *[]), read by Task_IntroStoryPicture (src/hud_099fc.c:35-38)",
    0x0854D294: "gUnk_087555FC[3-5] (include/hud.h), Task_IntroStoryPicture (src/hud_099fc.c:35-38)",
    0x0854E838: "gUnk_087555FC[6-8] (include/hud.h), Task_IntroStoryPicture (src/hud_099fc.c:35-38)",
    0x085995AC: "gUnk_085995AC (include/ending.h extern struct GfxHeader), sub_080c6ca0 (src/ending_c6c64.c:57-62)",
    0x0859990C: "gUnk_0859990C (include/ending.h extern struct GfxHeader), sub_080c6ca0 (src/ending_c6c64.c:65-67)",
    0x0859A09C: "gUnk_0859A09C (include/ending.h extern struct GfxHeader), sub_080c9040 (src/ending_c9004.c:82-86)",
    0x085B9B6C: "gUnk_085B9B6C (src/effect_5afac.c:25 u32[]), sub_0805b370 (src/effect_5afac.c:276-278) reads [0] << 5 palette bytes from [2] and LZ77-decompresses [3] ([1] << 5 tile bytes)",
    0x085BC800: "gUnk_08731F78[0] (include/cutscene.h struct GfxHeader *const []), sub_080102c0 (src/mode_100ac.c:88-93)",
    0x085BEB70: "gUnk_08731F78[1] (include/cutscene.h), sub_080102c0 (src/mode_100ac.c:88-93)",
}


def gfxheader_records(rom, u16, u32):
    """[(header, palette, pal_end, tiles, tiles_end, why)], asserting the
    format cross-check (LZ77 size == tileCount * 32)."""
    out = []
    for h, why in sorted(HEADERS.items()):
        pal, til = u32(h + 8), u32(h + 12)
        x = lz77(rom, til)
        assert x and x[1] == u16(h + 2) * 32, hex(h)
        out.append((h, pal, pal + u16(h) * 32, til, x[0], why))
    return out


# ---- the provider ------------------------------------------------------------

# The six struct ActorDef * tables (src/data/actor_defs.c, consumer
# sub_08063704, src/actor_63698.c:64); the end is the next C table.
ACTOR_DEF_TABLES = (0x0873ECEC, 0x0873ED90, 0x0873EDB8, 0x0873EDDC,
                    0x0873EE70, 0x0873EE88, 0x0873EEA0)
ACTOR_DEF_FIELDS = (0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28)

# struct ActorDef records the C passes by name to ActorLoadDef /
# ActorLoadDefSlot / sub_08066b34 / sub_08066c08 (not in the six tables);
# ActorLoadDefSlot (src/actor_63698.c:190-205) reads their pointer fields.
DIRECT_ACTOR_DEFS = {
    0x0873F690: "ActorLoadDef src/actor_653ec.c:1785",
    0x0873F6BC: "ActorLoadDef src/actor_6a344.c:270",
    0x08740C00: "sub_08066b34 src/enemy_78b68.c:333",
    0x0874183C: "ActorLoadDef src/enemy_7d3b0.c:1013",
    0x087419F4: "ActorLoadDef src/enemy_80b70.c:1201",
    0x08741A20: "ActorLoadDef src/enemy_80b70.c:1234",
    0x08742A6C: "sub_08066b34 src/enemy_84d14.c:654",
    0x087433E8: "sub_08066b34 src/enemy_8e404.c:900",
    0x08747D5C: "ActorLoadDef src/enemy_9c0a8.c:248",
    0x08747DB4: "ActorLoadDef src/enemy_9da1c.c:192",
    0x08747E0C: "ActorLoadDef src/enemy_9da1c.c:754",
    0x08747E64: "ActorLoadDef src/enemy_9cc24.c:285",
    0x087487BC: "ActorLoadDefSlot src/enemy_a1590.c:457",
    0x08748B34: "ActorLoadDef src/enemy_a1590.c:3049",
    0x08748B60: "ActorLoadDef src/enemy_a1590.c:3141",
    0x08748B8C: "ActorLoadDef src/enemy_a1590.c:3217",
    0x0874B96C: "ActorLoadDef src/enemy_ae3bc.c:2407",
    0x0874B998: "ActorLoadDef src/enemy_ae3bc.c:2440",
    0x0874B9C4: "ActorLoadDef src/enemy_ae3bc.c:2464",
    0x0874B9F0: "ActorLoadDef src/enemy_ae3bc.c:2489",
    0x0874BA1C: "ActorLoadDef src/enemy_ae3bc.c:2514",
    0x0874BA48: "ActorLoadDef src/enemy_ae3bc.c:2538",
    0x0874BA74: "ActorLoadDef src/enemy_ae3bc.c:2603",
    0x0874BAA0: "ActorLoadDef src/enemy_ae3bc.c:2708",
}

# Seg 11 (level_object_tables) records whose consumer gives the extent.
SEG11 = [
    (0x080D0198, 0x080D0398, "palette",
     "gUnk_080D0198, 256 u16 BG colours copied by src/subgame_c5284.c:343-344 (for i < 256)"),
    (0x080D0398, 0x080D059A, "s16-table",
     "gUnk_080D0398, s16 sine quarter-wave read [0..0x100] by sub_080c5284 (src/subgame_c5284.c:57-63)"),
    (0x080D0760, 0x080D0768, "s16-table",
     "s16 gUnk_080D0760[] / gUnk_080D0766[] (include/subgame.h:226-227, src/subgame_c5284.c:244/464): "
     "the word at 0x080D0764 straddles two s16 objects, so it is no pointer"),
]
# ---- Round 2 (#36 phase 2 run 2): Huffman streams, TransferNode sizes, ---
# ---- consumer-sized palettes and tile blocks in seg 14 / seg 11 ----------
# GBA BIOS HuffUnComp stream parser (GBATEK "BIOS Decompression Functions"):
# u32 header {bits 0-3 data size 4/8, bits 4-7 type 2, bits 8-31 size},
# tree-size byte T, tree table of (T+1)*2 bytes from the size byte, then the
# bit stream in 32-bit LE words (bit 31 first).


def huffman(rom, a, base=0x08000000):
    """(end VMA, declared size, output bytes) of the Huffman stream at VMA a,
    or None when the header, tree or bit stream is malformed.  The stream
    ends after the last 32-bit bit-stream word the decoder reads."""
    off = a - base
    if off < 0 or off + 8 > len(rom):
        return None
    hdr = struct.unpack_from("<I", rom, off)[0]
    bits = hdr & 0xF
    if (hdr >> 4) & 0xF != 2 or bits not in (4, 8):
        return None
    size = hdr >> 8
    if size == 0:
        return None
    tree = off + 4                       # tree size byte
    root = tree + 1
    stream = tree + (rom[tree] + 1) * 2  # bit stream start
    if stream % 4 or stream > len(rom):
        return None
    need = size * 8 // bits              # data units to produce
    out = bytearray()
    acc = 0
    nacc = 0
    units = 0
    p = stream
    node = root
    word = 0
    left = 0
    while units < need:
        if left == 0:
            if p + 4 > len(rom):
                return None
            word = struct.unpack_from("<I", rom, p)[0]
            p += 4
            left = 32
        bit = (word >> 31) & 1
        word = (word << 1) & 0xFFFFFFFF
        left -= 1
        v = rom[node]
        nxt = (node & ~1) + (v & 0x3F) * 2 + 2 + bit
        if nxt >= stream:
            return None                  # child outside the tree table
        if v & (0x80 >> bit):            # bit 7: node0 is data, bit 6: node1
            d = rom[nxt]
            if d >> bits:
                return None
            acc |= d << nacc
            nacc += bits
            if nacc == 8:
                out.append(acc)
                acc = nacc = 0
            units += 1
            node = root
        else:
            node = nxt
    return (p + base, size, bytes(out))


# HuffUnComp sources (src/gfx_08b8c.c): four direct symbols and the
# u32 gUnk_08731BA0[][2] rows (include/mode.h:109, gfx_08b8c.c:189/193).
# Every stream's output is itself an LZ77 stream that the same function
# hands to LZ77UnCompVram, which huffman_items() checks as well.
HUFF_DIRECT = {
    0x0856F308: "HuffUnComp(gUnk_0856F308) src/gfx_08b8c.c:161",
    0x085707D4: "HuffUnComp(gUnk_085707D4) src/gfx_08b8c.c:163",
    0x08570B28: "HuffUnComp(gUnk_08570B28) src/gfx_08b8c.c:176",
    0x08571248: "HuffUnComp(gUnk_08571248) src/gfx_08b8c.c:191",
}
HUFF_TABLE = 0x08731BA0  # u32 [][2], rows up to the first all-NULL row


def lz77_bytes(buf, off=0):
    """Declared size when the LZ77 stream in buf decodes to it, else None."""
    if len(buf) < off + 4 or buf[off] != 0x10:
        return None
    size = buf[off + 1] | buf[off + 2] << 8 | buf[off + 3] << 16
    p, out = off + 4, 0
    while out < size:
        if p >= len(buf):
            return None
        flags = buf[p]
        p += 1
        for bit in range(8):
            if out >= size:
                break
            if flags & (0x80 >> bit):
                if p + 2 > len(buf):
                    return None
                b0, b1 = buf[p], buf[p + 1]
                p += 2
                if ((b0 & 0xF) << 8 | b1) + 1 > out:
                    return None
                out += (b0 >> 4) + 3
            else:
                p += 1
                out += 1
    return size if out - size < 18 else None


def huffman_items(rom, u32):
    srcs = dict(HUFF_DIRECT)
    i = 0
    while True:
        a, b = u32(HUFF_TABLE + 8 * i), u32(HUFF_TABLE + 8 * i + 4)
        if not a and not b:
            break
        for j, v in enumerate((a, b)):
            if v:
                srcs.setdefault(v, "gUnk_08731BA0[%d][%d], HuffUnComp src/gfx_08b8c.c:%d" % (i, j, 189 if j == 0 else 193))
        i += 1
    out = []
    for a, why in sorted(srcs.items()):
        x = huffman(rom, a)
        if x and lz77_bytes(x[2]) is not None:
            out.append((a, x[0], "huffman",
                        "Huffman stream (%s): decodes to its declared 0x%X bytes, an LZ77 stream the same loader LZ77UnCompVram-s" % (why, x[1])))
    return out


def transfer_node_items(rom, u32, labels):
    """Sources of the TransferNode lists behind gUnk_0873185C (LoadGfxSet,
    src/gfx_08b8c.c:67 -> RequestCopyList, src/early_1518.c:104): a node is
    {u32 cmd = size << 8 | mode, src, dst}, the list ends at cmd == 0; modes
    1-5 copy `size` bytes from src, mode 8 LZ77-decompresses it (mode 6's src
    is a fill value)."""
    top = 0x0873185C
    i = bisect.bisect_right(labels, top)
    end = labels[i] if i < len(labels) else top + 4
    lists = sorted(set(u32(a) for a in range(top, end, 4) if u32(a)))
    out = []
    for lst in lists:
        a = lst
        while u32(a):
            cmd, src = u32(a), u32(a + 4)
            mode, size = cmd & 0xF, cmd >> 8
            if mode in (1, 2, 3, 4, 5) and size and ROM_BASE <= src < ROM_BASE + len(rom):
                out.append((src, src + size, "raw-copy",
                            "TransferNode 0x%08X {mode %d, 0x%X bytes} source (RequestCopyList, src/early_1518.c:104)" % (a, mode, size)))
            a += 12
    return out


def consumer_sized_items(rom, u8, u32):
    out = []
    # sub_08008e1c (src/gfx_08b8c.c:118): RequestCopy(2, gUnk_08731A28[a0][0],
    # .., gUnk_08731A88[a0] << 5) with u8 gUnk_08731A88[8] (include/mode.h:105)
    for i in range(8):
        pal = u32(0x08731A28 + 12 * i)
        if pal:
            out.append((pal, pal + (u8(0x08731A88 + i) << 5), "palette",
                        "gUnk_08731A28[%d][0], gUnk_08731A88[%d] << 5 bytes (sub_08008e1c, src/gfx_08b8c.c:118)" % (i, i)))
    # sub_0800bda4 (src/menu_0b920.c:208-209): two 0x180-byte RequestCopy(3)s
    # from gUnk_08553210 and gUnk_08553210 + 0x180
    out.append((0x08553210, 0x08553510, "raw-tiles",
                "gUnk_08553210: RequestCopy(3, .., 0x180) at +0 and +0x180 (sub_0800bda4, src/menu_0b920.c:208-209)"))
    # sub_0800f2b4 (src/menutask_0f180.c:106-121): u16 gUnk_08563024[][13]
    # (include/menu.h:65) rows unk2C/unk30, set to 0/1 (l.63-64) and wrapped
    # to 0..3 (l.106-110): 4 rows of 13 colours
    out.append((0x08563024, 0x08563024 + 4 * 26, "palette",
                "u16 gUnk_08563024[4][13]: BlendColors rows 0..3 (sub_0800f2b4, src/menutask_0f180.c:106-121)"))
    return out


SEG14_RANGE = (0x083D0148, 0x085C0000)
SEG11_RANGE = (0x080D0000, 0x08120000)


def provide(rom, cfg, segs):
    def u8(a):
        return rom[a - ROM_BASE]

    def u16(a):
        return struct.unpack_from("<H", rom, a - ROM_BASE)[0]

    def u32(a):
        return struct.unpack_from("<I", rom, a - ROM_BASE)[0]

    co = []
    ptr = []

    # 1. BG animation scripts (src/camera_2d01c.c, bganim_parse above)
    P = bganim_parse(u16, u32)
    for s, n, cmds in P["scripts"]:
        for (c, op, arg, p) in cmds:
            co.append((c, c + 4, "bg-anim-cmd",
                       "struct Unk02007D70Cmd {u16 op, u16 arg} of script 0x%08X (UpdateBgAnims, src/camera_2d01c.c:205)" % s))
            if op in (0, 1):
                ptr.append((c + 4, "BG animation op %d pointer (UpdateBgAnims -> %s, src/camera_2d01c.c)"
                            % (op, "sub_0802d25c" if op == 0 else "sub_0802d278")))
            elif op == 5:
                co.append((c + 4, c + 8, "bg-anim-value",
                           "op 5's ptr word is the collision value (u16)cmd->unk4 passed to sub_0802d2f0 (src/camera_2d01c.c:235)"))
    for f, e in P["frames"].items():
        co.append((f, e, "raw-tiles",
                   "BG animation frame {u16 tile, u16 byteSize, byteSize raw tile bytes}: sub_0802d25c RequestCopy(1, tiles, ..., byteSize) (src/camera_2d01c.c:250)"))
    for f, e in P["fades"].items():
        ptr.append((f, "struct Unk0802D278.from (sub_0802d278, src/camera_2d01c.c:261)"))
        ptr.append((f + 4, "struct Unk0802D278.to (sub_0802d278, src/camera_2d01c.c:261)"))
        co.append((f + 8, e, "fade-values",
                   "struct Unk0802D278 {u16 index, u16 count, u32 rate} (sub_0802d278, src/camera_2d01c.c:261)"))
    for p, e in P["palettes"].items():
        co.append((p, e, "palette",
                   "BG animation fade palette: count colours BlendColors reads (sub_0802d294, src/camera_2d01c.c:271)"))

    # 2. RoomDef records and their targets (room_items above)
    _rooms, items, rptrs = room_items(rom, u8, u16, u32)
    for s, e, kind, why in items:
        co.append((s, e, kind, why))
    ptr.extend(rptrs)

    # 3. GfxHeaders in these zones (gfxheader_records above)
    for h, pal, pe, til, te, why in gfxheader_records(rom, u16, u32):
        co.append((h, h + 8, "gfxheader-values", "struct GfxHeader counts: " + why))
        co.append((pal, pe, "palette", "GfxHeader.palette, paletteBankCount * 32 bytes: " + why))
        co.append((til, te, "lz77", "GfxHeader.tiles LZ77, tileCount * 32 bytes: " + why))
        ptr.append((h + 8, "GfxHeader.palette: " + why))
        ptr.append((h + 12, "GfxHeader.tiles: " + why))

    # 4. ActorDef / ActorAux pointer fields (seg 18)
    recs = set()
    for i in range(6):
        for a in range(ACTOR_DEF_TABLES[i], ACTOR_DEF_TABLES[i + 1], 4):
            if u32(a):
                recs.add(u32(a))
    for r in sorted(recs):
        for off in ACTOR_DEF_FIELDS:
            if u32(r + off):
                ptr.append((r + off, "struct ActorDef+0x%02X (sub_08063704/sub_080637e4, src/actor_63698.c)" % off))
    auxes = set(u32(r + 0x10) for r in recs if u32(r + 0x10))
    for aux in sorted(auxes):
        if u32(aux + 4):
            ptr.append((aux + 4, "struct ActorAux.altAttackBox (ActorCheckHits, src/actor_673ec.c:1315)"))
    for r, site in sorted(DIRECT_ACTOR_DEFS.items()):
        for off in ACTOR_DEF_FIELDS:
            v = u32(r + off)
            if not v or (off == 0x10 and v not in auxes):
                continue  # unk10 is not read on the ActorLoadDef path
            ptr.append((r + off, "struct ActorDef+0x%02X of a record passed to %s" % (off, site)))

    # 5. Seg 11 records with a consumer extent; LZ77 streams at labels of
    #    seg 11 and seg 14 (every one decodes to its declared size and ends
    #    at the next label, at most 3 bytes of alignment before it)
    co.extend(SEG11)
    labels = sorted(set(int(k, 16) for k in cfg["data_symbols"]) |
                    set(int(k, 16) for k in cfg.get("extra_labels", {})))
    for lo, hi in (SEG11_RANGE, SEG14_RANGE):
        zl = [a for a in labels if lo <= a < hi]
        for a in zl:
            # the next label anywhere in the ROM: a stream may run past its
            # segment's end (gUnk_085BF484 ends at 0x085C122C, m4a_songs)
            k = bisect.bisect_right(labels, a)
            nxt = labels[k] if k < len(labels) else hi
            x = lz77(rom, a)
            if x and nxt - 3 <= x[0] <= nxt:
                co.append((a, x[0], "lz77",
                           "LZ77 stream at label 0x%08X: decodes to its declared 0x%X bytes and ends at the next label" % (a, x[1])))
    # 6. Round 2: Huffman streams, TransferNode sources, consumer sizes
    co.extend(huffman_items(rom, u32))
    co.extend(transfer_node_items(rom, u32, labels))
    co.extend(consumer_sized_items(rom, u8, u32))
    return {"coincidence": co, "pointer": ptr}
