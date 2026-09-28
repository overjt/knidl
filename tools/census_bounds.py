"""Pointer-census evidence: consumer extents and unreachable regions (issue
#36 phase 2 run 3, tools/ptrcensus.py provider): the file-select completion
pictures (seg 14), seg 18 value tables whose extent was only the next-label
span, the credits demos (replayed), and the regions nothing reads in seg 11,
seg 18 and seg 19.

provide(rom, cfg, segs) -> {"coincidence": [(start, end, kind, why)],
                            "pointer": [(addr, why)]}

Everything is derived from the ROM plus the config; every start address
below is a label or record a decompiled consumer names, cited next to it.
Kinds: "value" / "raw-tiles" are proven coincidences (a consumer reads the
bytes as numbers or tiles); "unreachable" is the class of docs/data.md 8.3
(no code reads the bytes; ptrcensus.py checks condition (a) itself).
"""
import bisect
import struct

ROM_BASE = 0x08000000


def _u8(rom, a):
    return rom[a - ROM_BASE]


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
    return labels[bisect.bisect_right(labels, a)]


# ---- 1. the file-select completion pictures (compressed_graphics) --------
#
# sub_0800bda4 (src/menu_0b920.c:212-217): for a save slot that is neither
# erased (unk04 != 0x99999999) nor extra-mode-cleared (!(milestoneFlags & 2))
# it runs IntToDigits(completionPercent[0]) and copies, with
# RequestCopy(3, ..., 0x180) (mode 3: raw copies of <= 0x200-byte chunks into
# OBJ VRAM, src/early_1518.c:244-279), the two 0x180-byte halves
# n = 2 * (tens + 10 * hundreds) and n + 1 of u8 gUnk_08551110[]
# (include/menu.h:42).  100 is a displayed value: CalcCompletionPercent
# (src/save_b79b8.c:11-14) and MergeProgressIntoSaveSlot
# (src/save_b8918.c:104-106) write exactly 100 once milestone bit
# 4 << mode is set, which the same functions set when the sum reaches 100
# (src/save_b79b8.c:31-32, src/save_b8918.c:132-133), and bit 1 (the extra
# mode's clear, src/level_242d0.c:913-914) is independent of it.  So index
# 21 is read, and the array's 22 halves (11 two-part pictures 0%, 10%, ...
# 100%) run exactly up to the next label gUnk_08553210, which the same
# function copies as the extra-mode picture.  Slots the menu shows come only
# from checksum-valid SRAM copies or new files: ReadSaveSlot checks the
# 28-word sum of both copies and InitSaveSlots clears a slot whose copies
# both fail to 0x99999999 (src/save_b77d4.c:23-99), which takes the erased
# branch, and a new file starts at 0 (InitNewSaveFile -> ResetProgress,
# src/save_b7e14.c:106-126).
DIGITS = (0x08551110, 22 * 0x180)

# ---- 2. seg 18 value tables with consumer extents (were next-label) -------

# (start, end, why); each covers the unknown words the census listed
SEG18_VALUES = [
    (0x08732428, 0x08732428 + 42 * 3,
     "s8 gUnk_08732428[level * 6 + stage][3] hub camera-pan steps: map event 5 "
     "sub_0802dcb4 (src/camtask_2d38c.c:344-365), spawned by sub_0802672c "
     "(src/stage_261c0.c:253) on a stage's first clear (gUnk_0200AF00 == 1, "
     "src/player_39c24.c:380, src/stage_413a4.c:1392), indexes it gStageIndex * 6 "
     "+ gUnk_0200001C - 1 with gStageIndex = the level (the hub's stage, "
     "src/level_242d0.c:762-763) and gUnk_0200001C = the cleared stage + 1 "
     "(src/level_242d0.c:744, the goal door kind 0x22B8); every non-boss stage of "
     "levels 0-6 has a 0x22B8 door (RoomDef.doors), stage 5 of level 6 among "
     "them, so rows 0-41 are read: 0x7E bytes, ending at gUnk_087324A6"),
    (0x087493A4 + 52, 0x087493A4 + 56,
     "s8 gUnk_087493A4[frame] entries 52-55: Nightmare Wizard state 7 "
     "(gNightmareWizardStates[7] = sub_080aa998, src/enemy_aa338.c:408-422, "
     "entered from state 4, src/enemy_aa338.c:395-396) shows frames 52-55 while "
     "its draw callback is sub_080a9ed8 (set by Task_NightmareWizard "
     "src/enemy_a93ec.c:659 and restored by every teleport-in sub_080ab670, "
     "src/enemy_aa338.c:918, which state 2 runs after each teleport-out), and "
     "sub_080a9ef4 reads gUnk_087493A4[frame] (src/enemy_a93ec.c:577)"),
    (0x087494C8 + 28, 0x087494C8 + 32,
     "s8 gUnk_087494C8[frame] entries 28-31: the same state 7 starts the "
     "animation gUnk_087492C0 {28, 29, 30, 31, loop} (src/enemy_aa338.c:428, "
     "struct AnimCmd, ActorStepAnim src/actor_63698.c:665-692), and "
     "sub_080ac530 reads gUnk_087494C8[parent->frame] for the Wizard parent "
     "(unk76 == 8, src/enemy_aa338.c:1545-1553)"),
    (0x0874B63E, 0x0874B63E + 66,
     "u8 gUnk_0874B63E[frame] entries 0-65: sub_080b1890 reads it at "
     "t->frame (src/enemy_ae3bc.c:2955); state 4 (gUnk_0874B5E4[4] = "
     "sub_080b21a0, chosen by state 1 from gUnk_0874B831 {2, 3, 4, 5}, "
     "src/enemy_ae3bc.c:3227) sets unk34 from s16 gUnk_0874B8C8[k] (facing -1, "
     "max 60) or gUnk_0874B8F8[k] (facing 1, max 120), k = 23..0 "
     "(src/enemy_ae3bc.c:3513-3530), and its update sub_080b2228 shows frames "
     "unk34 + 0..5 (src/enemy_ae3bc.c:3534-3556): index 65 is read whatever "
     "the facing"),
]

# ---- 3. the credits demos (struct LinkSave, include/save.h:25) ------------
#
# CreditsMain (src/credits_cd330.c:62-158) plays the scenes of
# gUnk_087583CC[v] with v = gUnk_030023B0, 1 or 2 (l.74-77; row 0 is never
# played), each for 15 + (length - 1) frames plus 15 before the next scene
# or 30 after the last (l.112-147), lengths u16 [v][7] from 0x0875841E + 14 v
# (include/ending.h:96).  gInputRecorderMode is only ever 0 or 3
# (src/mode_075b8.c:179, src/mode_0b44c.c:59, src/mode_08664.c:151,
# src/credits_cd330.c:86), so these ROM records are the only recordings ever
# played.  InputRecorderStart mode 3 -> sub_080b72bc (src/save_b72bc.c:10-72)
# reads the 0x12C-byte header as numbers and starts each player i at entry i
# with count 0 and end Div(0x3B6A, players) - 4; InputRecorderPlayFrame
# (src/save_b75a4.c:62-91) reads entry unk12C[pos] (key | count << 10) when
# the player's count runs out, steps pos by the player count, and stops
# everyone at key 0x3FF.  The entries read depend only on the entries
# themselves, so replaying the frame counts gives the exact extent read.
DEMO_TABLE = 0x087583CC      # u32 gUnk_087583CC[][8]
DEMO_LENGTHS = 0x0875841E    # u16 gUnk_0875841E[][7]


def demo_extent(rom, demo, frames):
    """Bytes of the struct LinkSave at `demo` that `frames` playback frames
    read: the header plus the entries up to the last one read."""
    players = _u16(rom, demo + 0x0C)
    end = 0x3B6A // players - 4
    pos = list(range(players))
    cnt = [0] * players
    last = -1
    for _f in range(frames):
        for i in range(players):
            if pos[i] >= end:
                break
            c = (cnt[i] - 1) & 0xFF
            if c >= 0x80:
                c -= 0x100
            cnt[i] = c & 0xFF
            if c <= 0:
                p = pos[i]
                pos[i] += players
                e = _u16(rom, demo + 0x12C + 2 * p)
                last = max(last, p)
                cnt[i] = e >> 10
                if e & 0x3FF == 0x3FF:
                    pos = [end] * players
                    break
    return demo + 0x12C + 2 * (last + 1)


def demos(rom):
    out = []
    for v in (1, 2):
        scenes = [_u32(rom, DEMO_TABLE + 32 * v + 4 * i) for i in range(8)]
        lengths = [_u16(rom, DEMO_LENGTHS + 14 * v + 2 * i) for i in range(7)]
        k = 0
        while k < 7 and scenes[k]:
            frames = 15 + (lengths[k] - 1) + (15 if scenes[k + 1] else 30)
            out.append((v, k, scenes[k], frames, demo_extent(rom, scenes[k], frames)))
            k += 1
    return out


# ---- 4. unreachable regions (docs/data.md 8.3) ---------------------------

# the 20-byte tagged player frame records {oam | 1, palette, tiles, palette2,
# tiles2} inside the record chain that no frame table entry, frame list or
# other word points at (the census checks it)
UNREACHED_RECORDS = [
    0x08762748, 0x0876275C, 0x08762770, 0x08763C4C, 0x0876545C, 0x087671F8,
    0x08767220, 0x08767248, 0x08767270, 0x08767298, 0x087672C0, 0x087672E8,
    0x08767310, 0x08767338,
]

UNREACHABLE = [
    (0x080DCAA8, 0x080DCC28,
     "u8 gUnk_080DCA28[] (include/player.h:104) past its four player palettes: "
     "its only readers are the two RequestCopy(2, &gUnk_080DCA28[k * 32], .., "
     "32) of sub_0803db74 (src/stage_3cd60.c:779-782, the pool words 0x0803DD88 "
     "and 0x0803DDB8, the only ROM words that point into the object), k = "
     "PlayerState.playerIndex or gUnk_02000028; playerIndex is only written as "
     "gPlayerStates[a0].playerIndex = a0 (src/stage_3cd60.c:188/263/331) and "
     "gPlayerStates holds four 0x74-byte records (0x03002170-0x03002340, "
     "gActivePlayerMask next), gUnk_02000028 = gLocalPlayer (src/mode_c6260.c:36, "
     "EWRAM cleared by AgbInit) and gLocalPlayer is 0 or the 2-bit SIOMULTICNT "
     "id (src/early_6d28.c:84/105; the demo copy src/save_b72bc.c:51 runs only "
     "with gUnk_030023B0 == 0, never in the credits): k <= 3, so bytes 0x80-0x1FF "
     "are read by nothing; the next object is gUnk_080DCC28"),
    (0x08740628, 0x08740630,
     "the two words after u8 gUnk_08740620[8] (include/cutscene.h:110): its one "
     "reader sub_0807817c (src/actor_77ae0.c:401) indexes it by Task.unk28, "
     "which its six callers loop over 0-7, 0-2, 0-3 and 3-5, the eight credits "
     "particles gUnk_03000FE0[] (src/actor_77ae0.c:617-737); next label "
     "gWaddleDeeVariants"),
    (0x0874101C, 0x08741020,
     "word 3 of the palette-variant record gUnk_08741010 (gUnk_0873EF74[] "
     "target): its readers take word paletteVariant - 1 or level - 1 with "
     "paletteVariant, level <= 3 (src/actor_653ec.c:565-575, 605-621, 813-820) "
     "and words 4-5, never word 3, and no record of either table starts where "
     "one of those indices would land on it; next record gUnk_08741028"),
    (0x08741DE8, 0x08741DEC,
     "word 3 of the palette-variant record gUnk_08741DDC (gUnk_0873EF74[] "
     "target), read by nothing as for gUnk_08741010 (src/actor_653ec.c:565-575, "
     "605-621, 813-820); next record gUnk_08741DF4"),
    (0x08745A34, 0x08745A3C,
     "a second struct ActorAux {hitDuration, altAttackBox} after the one at "
     "gUnk_08745A2C: Actor.unk60 is set only from ActorDef+0x10 or "
     "sub_080639f0's argument (src/actor_63698.c:258) and read only as "
     "->hitDuration/->altAttackBox (src/actor_653ec.c:1088/1174, "
     "src/actor_673ec.c:1313-1430, src/enemy_a1590.c:416-417), never indexed, "
     "and no word points at 0x08745A34; next label gUnk_08745A3C"),
    (0x0874B4D8, 0x0874B4E0,
     "a second struct ActorAux after the one at gUnk_0874B4D0, unreached as "
     "for 0x08745A34 (Actor.unk60 readers src/actor_673ec.c:1313-1430); next "
     "label gUnk_0874B4E0"),
    (0x08769414, 0x08769418,
     "the last entry of the frame list 0x087693F0 (10 entries, trailer "
     "0x0824F40C): no code reads the frame lists (docs/data.md 5.3); it holds "
     "the end of the sheet's chained OAM block, the header 0x0824F3FC, not a frame"),
    (0x08769524, 0x08769528,
     "the last entry of the frame list 0x087694EC (15 entries, trailer "
     "0x08251DAC): no code reads the frame lists (docs/data.md 5.3); it holds "
     "the end of the sheet's chained OAM block, the header 0x08251D9C, not a frame"),
]


def provide(rom, cfg, segs):
    labels = _labels(cfg)
    co = []
    # 1
    co.append((DIGITS[0], DIGITS[0] + DIGITS[1], "raw-tiles",
               "u8 gUnk_08551110[22 * 0x180]: the file-select completion pictures, "
               "halves n = 2 * (percent / 10) and n + 1 copied raw by RequestCopy(3, "
               ".., 0x180) (sub_0800bda4, src/menu_0b920.c:212-217); percent 100 is "
               "displayed (src/save_b79b8.c:11-14, src/save_b8918.c:104-106), so "
               "index 21 is read and the array ends at gUnk_08553210"))
    # 2
    for s, e, why in SEG18_VALUES:
        co.append((s, e, "value", why))
    # 3
    for v, k, d, frames, end in demos(rom):
        nxt = _next_label(labels, d)
        if end > nxt:
            raise ValueError("credits demo 0x%08X read past its next label" % d)
        co.append((d, d + 0x12C, "value",
                   "credits demo gUnk_087583CC[%d][%d] header (struct LinkSave, "
                   "numbers, sub_080b72bc src/save_b72bc.c:10-72)" % (v, k)))
        co.append((d + 0x12C, end, "value",
                   "credits demo gUnk_087583CC[%d][%d] input entries key | count << 10 "
                   "that %d frames of playback read (InputRecorderPlayFrame "
                   "src/save_b75a4.c:62-91, CreditsMain src/credits_cd330.c:112-147)"
                   % (v, k, frames)))
        if end < nxt:
            co.append((end, nxt, "unreachable",
                       "credits demo gUnk_087583CC[%d][%d] entries after the last one "
                       "its scene plays (%d frames: 15 + gUnk_0875841E[%d][%d] - 1 + "
                       "the fade, src/credits_cd330.c:112-147); the playback index "
                       "only grows by the player count (src/save_b75a4.c:74-78) and no "
                       "other code reads the record (the only recordings played are "
                       "these, gInputRecorderMode is 0 or 3); next label 0x%08X"
                       % (v, k, frames, v, k, nxt)))
    # 4
    for r in UNREACHED_RECORDS:
        tagged = _u32(rom, r) & 1
        co.append((r, r + (20 if tagged else 12), "unreachable",
                   "struct TaskGfx record (tagged: {oam | 1, palette, tiles, palette2, "
                   "tiles2}) in the player record chain that no frame table entry "
                   "points at: PlayerLoadFrameTilesAndPalette and sub_0803dfc8 reach a "
                   "record only through tbl[frame + a0] (src/stage_3cd60.c:492), and no "
                   "ROM word points into it"))
    for s, e, why in UNREACHABLE:
        co.append((s, e, "unreachable", why))
    return {"coincidence": co, "pointer": []}
