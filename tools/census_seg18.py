"""Pointer-census evidence: seg 18's behaviour tables (issue #36 phase 2
run 2, tools/ptrcensus.py provider), for (0x08730780-0x0874C000, 0x08756000-0x08759000) and
lib_rodata_fir_tables (0x080CFE20-0x080D0000).

provide(rom, cfg, segs) -> {"coincidence": [(start, end, kind, why)],
                            "pointer": [(addr, why)]}

Everything is derived from the ROM plus the config; the start addresses
below are the labels the decompiled consumers name (cited next to each).
A range is a *value field*: bytes a consumer reads as numbers.  Two grades:
  kind "value"            the consumer's index range covers the range;
  kind "value-nextlabel"  the element type is proven, the extent is the span
                          to the next label (docs/data.md 5.1's heuristic).
"""
import struct

ROM_BASE = 0x08000000


def _u32(rom, a):
    return struct.unpack_from('<I', rom, a - ROM_BASE)[0]


def _labels(cfg):
    out = set()
    for key in ("data_symbols", "extra_labels"):
        for k in cfg.get(key, {}):
            a = int(k, 16)
            if ROM_BASE <= a < 0x08800000:
                out.add(a)
    return sorted(out)


def _next_label(labels, a):
    for x in labels:
        if x > a:
            return x
    return None


# The seven per-tile collision tables (256 entries: the index is the cell's
# u8 collisionTile, include/room.h:40) and their consumers.  Every target is
# an s8[256] pixel table indexed by gTerrainPixelIndex, i.e.
# ((y & 15) << 4) + (x & 15) (src/terrain_21b18.c:365, src/terrain_214e0.c:174-199).
COLLISION_TABLES = [
    (0x087328F0, "gCollisionTileShapes, src/terrain_214e0.c:79"),
    (0x087330F0, "gUnk_087330F0, src/terrain_214e0.c:169"),
    (0x08733BF0, "gCollisionTilePushDown, src/terrain_214e0.c:180"),
    (0x08733FF0, "gCollisionTilePushUp, src/terrain_214e0.c:186"),
    (0x087343F0, "gCollisionTilePushRight, src/terrain_214e0.c:192"),
    (0x087347F0, "gCollisionTilePushLeft, src/terrain_214e0.c:198"),
    (0x08734BF0, "gCollisionTileFloorSnap, src/terrain_214e0.c:174"),
]

# struct HitBoxSet records (include/player.h:18), listed with
# their consumers in the pointer_tables entries of tools/split_config.json.
HITBOXSETS = [
    0x0873CB84, 0x0873CB94, 0x0873CBA4, 0x0873CBDC, 0x0873CBEC, 0x0873CBFC,
    0x0873CC0C, 0x0873CC1C, 0x0873CC2C, 0x0873CC3C, 0x0873CC44, 0x0873CC54,
    0x0873CC64, 0x0873CC74, 0x0873CC84, 0x0873CC94, 0x0873CCA4, 0x0873CDAC,
    0x0873CF4C, 0x0873CF5C, 0x0873CF6C, 0x0873CF7C, 0x0873CF8C, 0x0873D03C,
    0x0873F8CC, 0x0873F8DC, 0x08749B84, 0x0874B538,
    0x0874C108,  # round 2: TaskBreakBlocksNoPlayer, src/enemy_ae3bc.c:3755
]

# struct ActorDef records not reached from src/data/actor_defs.c (their
# pointer_tables entries cite the loader call): +0x00-+0x0F are health, score, ability.
ACTORDEFS = [
    0x08740C00, 0x0874183C, 0x087419F4, 0x08741A20, 0x08742A6C, 0x087433E8,
    0x08747C80, 0x08747CAC, 0x08747CD8, 0x08747D04, 0x08747D30, 0x08747D5C,
    0x08747DB4, 0x08747E0C, 0x08747E64, 0x087487BC, 0x08748B34, 0x08748B60,
    0x08748B8C, 0x0874B96C, 0x0874B998, 0x0874B9C4, 0x0874B9F0, 0x0874BA1C,
    0x0874BA48, 0x0874BA74, 0x0874BAA0,
]

# Scalar tables: (label, end or None for the next label, kind, why).
SCALAR_TABLES = [
    (0x080CFF70, 0x080CFF96, "value", "s16 divisors gUnk_080CFF70 (div = tbl + 2 read as s16, "
     "src/subgame_c4630.c:261-264) followed by u8 gUnk_080CFF76[4][4][2] OBJ {width, height} "
     "(include/subgame.h:221, src/subgame_c4d08.c:130)"),
    (0x0873231A, 0x08732320, "value", "s8 gUnk_08732302[level][6], row 4: level 4 has 7 stages (gUnk_08334EB4[4], src/level_2296c.c), so all six columns are read (src/level_242d0.c:750)"),
    (0x0873B634, 0x0873B652, "value", "u8 gUnk_0873B634[] roulette delays read until 0 (src/player_47fe8.c:116); the 0 is at +0x1E"),
    (0x0873B7C0, 0x0873B808, "value", "u16 gUnk_0873B7C0[3][2][6] 8.8 velocities (src/plobj_5239c.c:234-311)"),
    (0x0873B872, 0x0873B88A, "value", "u8 gUnk_0873B872[3][8] step counts (src/plobj_52f6c.c:65-79)"),
    (0x0873B8C6, 0x0873B926, "value", "u16 gUnk_0873B8C6[3][2][8] 8.8 velocities (src/plobj_52f6c.c:63-78)"),
    (0x0873BA4C, 0x0873BA8C, "value", "u16 gUnk_0873BA4C[][4] velocities/accelerations, rows (unk18 & 7) + 0..4 (src/effect_54a80.c:55-72)"),
    (0x0873D0F8, 0x0873D206, "value", "u16 gUnk_0873D0F8[27][5] animation ids by ability, row 26 read explicitly (src/player_337f4.c:78-80)"),
    (0x0873D210, 0x0873D2D8, "value", "s16 gUnk_0873D210[ability * 4] animation ids (src/stage_3cd60.c:88); abilities 0-24 are reached (the roulette steps through 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D3B8, 0x0873D41C, "value", "s16 gUnk_0873D3B8[ability][2] animation ids (src/player_34f8c.c:75), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D420, 0x0873D4B6, "value", "s16 gUnk_0873D420[ability][3] animation ids (src/player_34f8c.c:275), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D4BC, 0x0873D5B6, "value", "u16 gUnk_0873D4BC[ability][5] animation ids (src/player_36280.c:237), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D5CA, 0x0873D62E, "value", "s16 gUnk_0873D5CA[ability][2] animation ids (src/player_36280.c:343), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D632, 0x0873D790, "value", "u16 gPlayerDoorAnims[ability][7] animation ids (src/player_39c24.c:77-450), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873D7E4, 0x0873D87A, "value", "s16 gUnk_0873D7E4[ability][3] animation ids (src/player_34f8c.c:429), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873DA62, 0x0873DAC6, "value", "u16 gUnk_0873DA62[ability][2] animation ids (src/player_3bde8.c:72), rows 0-24 (abilities 1-24, src/player_47fe8.c:17-22,105)"),
    (0x0873DACA, 0x0873DADE, "value", "s16 gUnk_0873DACA[5][2] animation ids, rows 0-4 read explicitly (src/player_3bde8.c:108-341)"),
    (0x0873DCC0, 0x0873DCC8, "value", "s8 offsets ((s8 *)gUnk_0873DCC0)[RandomRange(8)] (src/effect_5afac.c:835)"),
    (0x0873E1B4, 0x0873E1B8, "value", "s8 gUnk_0873E1B4[gActivePlayerCount - 1] health bonus (src/actor_653ec.c:1254)"),
    (0x0873FF98, 0x08740098, "value", "s16 gUnk_0873FF98[128] affine scales: the index Task.unk18 >> 16 is clamped to 0..127 (src/ending_c6c64.c:456-469)"),
    (0x08741350, 0x08741355, "value", "u8 gUnk_08741350[5] (src/enemy_7f044.c:189): indexed like the parallel s32[5] tables gUnk_08741328 and gUnk_0874133C (20 bytes each, src/enemy_7f044.c:190-191)"),
    (0x0874202C, 0x08742030, "value", "u8 gUnk_0874202C[4] loop bounds indexed by Task.unk74 like the parallel u8[4] tables gUnk_08742020/24/28 and the s32[4] gUnk_08742010 (src/enemy_84d14.c:496-615)"),
    (0x0874324C, 0x08743251, "value", "s8 gUnk_0874324C[5] x offsets: Task.unk30 is set to 0..4 by the switch at src/enemy_8e404.c:573-602 (read at :751), and the y twin gUnk_08743251 starts at +5"),
    (0x087493A4, None, "value-nextlabel", "s8 gUnk_087493A4[frame] (src/enemy_a93ec.c:577)"),
    (0x087494C8, None, "value-nextlabel", "s8 gUnk_087494C8[frame] (src/enemy_aa338.c:1553)"),
    (0x0874B63E, None, "value-nextlabel", "u8 gUnk_0874B63E[frame] (src/enemy_ae3bc.c:2955)"),
    (0x087561CC, None, "value-nextlabel", "s8 wave table gUnk_087561CC[i >> 3] (src/save_b6b08.c:56)"),
    (0x08756570, None, "value-nextlabel", "u8 gUnk_08756570[] beat lengths (src/subgame_bda2c.c:97)"),
    (0x08756577, 0x087565E0, "value", "the three u8[35] rows gUnk_087565F4[] points at "
     "(0x08756577, 0x0875659A, 0x087565BD: 35 bytes apart, 7 groups of 5), read r[unk2C * 5 + 0..3] "
     "as numbers (src/subgame_bda2c.c:1123-1127)"),
    (0x08756610, 0x0875664F, "value", "the three u8[21] rows gUnk_08756650[level] points at, read "
     "[unk2C * 3 + gBombRallyOutCount] (src/subgame_bda2c.c:1280); they end at gUnk_08756650"),
]


def provide(rom, cfg, segs):
    labels = _labels(cfg)
    co = []

    # 1. the collision pixel tables (s8[256] each)
    targets = set()
    for base, why in COLLISION_TABLES:
        for i in range(256):
            v = _u32(rom, base + 4 * i)
            if v:
                targets.add(v)
    for t in sorted(targets):
        co.append((t, t + 256, "value",
                   "s8[256] collision pixel table indexed by gTerrainPixelIndex "
                   "((y & 15) << 4) + (x & 15), a target of the 256-entry tile "
                   "tables at 0x087328F0-0x08734FF0 (src/terrain_214e0.c:174-199, "
                   "src/terrain_21b18.c:365)"))

    # 2. HitBoxSet: +0 (u16 flags, s8 offsetX, s8 offsetY) and the boxes list
    for a in HITBOXSETS:
        co.append((a, a + 4, "value", "HitBoxSet.unk0/offsetX/offsetY "
                   "(include/player.h:18, src/block_30804.c:133,189-191)"))
        b = _u32(rom, a + 4)
        if ROM_BASE <= b < 0x08800000:
            e = b
            while rom[e - ROM_BASE] != 127:
                e += 4
            co.append((b, e + 4, "value", "HitBoxSet.boxes: s8[4] boxes up to "
                       "the box[0] == 127 end mark (src/block_30804.c:204-230)"))

    # 3. ActorDef numbers (+0x00-+0x0F)
    for a in ACTORDEFS:
        co.append((a, a + 0x10, "value", "ActorDef health[4], score, ability, "
                   "unk0D, unk0E (include/task.h:117, src/actor_63698.c:202-203)"))

    # 4. palette-variant records: +0x10 offset, +0x14 colour count
    for tbl in (0x0873EF74, 0x0873F118):
        end = _next_label(labels, tbl)
        for a in range(tbl, end, 4):
            r = _u32(rom, a)
            if r:
                co.append((r + 0x10, r + 0x18, "value", "palette-variant record "
                           "words 4-5, the bank offset and colour count "
                           "(src/actor_653ec.c:614-617)"))

    # 5. M12Fade.unk8 (s32 rate), 3 records
    for i in range(3):
        a = 0x0873B510 + 12 * i + 8
        co.append((a, a + 4, "value", "M12Fade.unk8, the blend step "
                   "(src/player_455c8.c:218)"))

    # 6. scalar tables
    for start, end, kind, why in SCALAR_TABLES:
        if end is None:
            end = _next_label(labels, start)
        co.append((start, end, kind, why))

    # 7. the per-move arrays of sub_080ad278 (src/enemy_aa338.c:2146-2158):
    # count = gUnk_08749D44[k] (u8, src/enemy_aa338.c:2140), indices 0..count-1
    for base, size, t in ((0x08749D4C, 2, "s16 frame ids"), (0x08749D70, 4, "s32 x motion"),
                          (0x08749D94, 4, "s32 y velocity"), (0x08749DB8, 4, "s32 sound ids"),
                          (0x08749DDC, 1, "u8 yield frames")):
        for k in range(9):
            n = rom[0x08749D44 - ROM_BASE + k]
            a = _u32(rom, base + 4 * k)
            if n and ROM_BASE <= a < 0x08800000:
                co.append((a, a + n * size, "value", "%s, gUnk_08749D44[%d] = %d "
                           "entries read by sub_080ad278 (src/enemy_aa338.c:2146-2158)"
                           % (t, k, n)))

    # 8. the credits demos gUnk_087583CC[n][i] points at: struct LinkSave
    # (include/save.h:25, all numeric fields; unk12C[] entries are
    # key | count << 10, src/save_b75a4.c:57), each up to the next label
    for a in range(0x087583CC, 0x0875842C, 4):
        d = _u32(rom, a)
        if ROM_BASE <= d < 0x08800000:
            co.append((d, _next_label(labels, d), "value-nextlabel",
                       "a recorded credits demo (struct LinkSave, numbers), "
                       "played through gInputRecordingPtr (src/save_b6f38.c:139)"))

    ptr = [
        # gUnk_087583CC row 2's last three scene pointers: proven like the
        # rest of the row (CreditsMain, src/credits_cd330.c:90, stores them in
        # gUnk_0200EC50, which src/save_b6f38.c:139 uses as the recording
        # pointer), but the label gUnk_0875841E (include/ending.h:96, the
        # u16 [][7] lengths indexed [1..2]) sits at 0x0875841E, inside the
        # word at 0x0875841C, so no .word can be emitted until the C indexes
        # the lengths from their real start 0x0875842C.
        (0x0875841C, "gUnk_087583CC[2][4], credits demo pointer (src/credits_cd330.c:90)"),
        (0x08758420, "gUnk_087583CC[2][5], credits demo pointer (src/credits_cd330.c:90)"),
        (0x08758424, "gUnk_087583CC[2][6], credits demo pointer (src/credits_cd330.c:90)"),
    ]
    return {"coincidence": co, "pointer": ptr}
