#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1e178.c (0x0801E178-0x0801ECCF, issue #84).
 *
 * The landing probe sub_0801bde0 runs for a box in the air
 * (gUnk_03005530.unk6 == 0), the twin of sub_0801ecd0 in src/terrain_1ecd0.c
 * with a wall step at the box's bottom corner in front (the head
 * src/terrain_1c930.c's sub_0801c930 has too).  Its shared epilogue at
 * 0x0801ECBA is reached by a long `bl` from 0x0801E470 as well as by `b.n`s
 * (a far branch inside the function, lesson 4.39). */

/* The probe result block, filled by the terrain probes and mirrored into
   gUnk_03005550 by sub_0801c5c8. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

/* ROM tables indexed by tile set (0x100 entries each). */
extern s8 *const gUnk_087328F0[];   /* per-tile-set pixel attribute tables */
extern u8 gUnk_08732CF0[];
extern u8 gUnk_08732DF0[];
extern s8 gUnk_08732EF0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087338F0[];
extern s8 gUnk_087339F0[];
extern u16 gUnk_08735018[];         /* indexed by the cell's byte 2 */
extern u16 gUnk_03005508;           /* pixel offset inside the queried cell */
extern s16 gUnk_0300550C;           /* box left (room-relative) */
extern s32 gUnk_03005514;           /* Task.unk58 */
extern s16 gUnk_0300551C;           /* box left offset */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern u8 gUnk_03005564;            /* Task.unk43 */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_03005584;           /* box bottom offset */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern s16 gUnk_03005590;           /* box right (room-relative) */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_0300559C;           /* box right offset */
extern u16 gUnk_030055AC;           /* cell below: byte 2 */
extern s16 gUnk_030055B0;           /* box bottom (room-relative) */
extern u8 gUnk_02005574[];
extern u8 gUnk_03005568;

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080219b0(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);
s32 sub_08021ab4(u32 x, u32 y);

/* Landing probe of a box in the air (sub_0801bde0, on-ground flag
   gUnk_03005530.unk6 == 0), the twin of sub_0801ecd0: first step the box
   out of a wall it moves into at its bottom corner (the same head as
   sub_0801c930), then the flag gUnk_03005530.unkD, then snap the box to
   the floor under its centre; failing that, follow the cell's slope link
   (gUnk_08735018[byte 2]) and test the floor under the box's left and
   right corners, keeping the corner/slope flags in gUnk_03005530.unkB. */
void sub_0801e178(void)
{
    s32 side;
    s32 a;
    s32 u;
    s32 odd;
    u32 below;
    u32 cell;
    u32 cellBelow;
    u8 b;
    const s8 *p;
    s32 dir;
    s32 r;

    if (gUnk_02005574[0] == 0 && (gUnk_03005568 & 4)
        && sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0
            || ((gUnk_03005530.unkB & 1)
                && gUnk_03005530.unkC <= ((gUnk_03005570 + gUnk_03005584) >> 4))))
    {
        if (gUnk_03005598 == 0)
            dir = (s8)gUnk_03005564;   /* canon.h: u8; the ROM loads it with ldrsb */
        else
        {
            dir = -1;
            if (gUnk_03005598 > 0)
                dir = 1;
        }
        if (dir == 1)
        {
            if (sub_08021ab4(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) == 0)
                goto done;
            gUnk_03005530.unk0 = 1;
            /* The ROM places this arm's `u = t` and its steps after the
               left arm (the gotos reproduce that layout, as in
               sub_0801c930). */
            if (gUnk_087336F0[gUnk_03005578] == 0)
                goto rflat;
            u = gUnk_087338F0[gUnk_03005578];
            goto rmove;
        }
        if (sub_08021ab4(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0)
        {
            gUnk_03005530.unk0 = 2;
            if (gUnk_087336F0[gUnk_03005578] != 0)
                u = gUnk_087338F0[gUnk_03005578];
            else
                u = gUnk_03005578;
            gUnk_03005560 += sub_080219d0(u);
            if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
            {
                if (gUnk_087336F0[gUnk_03005578] != 0)
                    u = gUnk_087338F0[gUnk_03005578];
                else
                    u = gUnk_03005578;
                gUnk_03005560 += sub_080219d0(u);
            }
        }
        goto done;
    rflat:
        u = gUnk_03005578;
    rmove:
        gUnk_03005560 += sub_080219f0(u);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            if (gUnk_087336F0[gUnk_03005578] != 0)
                u = gUnk_087338F0[gUnk_03005578];
            else
                u = gUnk_03005578;
            gUnk_03005560 += sub_080219f0(u);
        }
    }
done:
    side = 0;
    r = sub_080216d8(gUnk_03005560, gUnk_03005584 + gUnk_03005570);
    if (!(gUnk_03005530.unkD & 0x10))
    {
        if ((u16)(gUnk_03005578 - 48) <= 1)
            gUnk_03005530.unkD = 3;
        else
            gUnk_03005530.unkD = 0;
        /* The no-floor test is written in both arms: the ROM loads the
           floor code's addresses at the end of each arm (3.430). */
        if (r == 0)
            goto floor;
    }
    else
    {
        gUnk_03005530.unkB = 1;
        if ((u16)(gUnk_03005578 - 48) <= 1)
        {
            gUnk_03005530.unkD = 3;
            return;
        }
        gUnk_03005530.unkD = 0;
        if (r == 0)
            goto floor;
    }
    {
        if (gUnk_087336F0[gUnk_03005578] != 0)
        {
            if (gUnk_03005530.unkB & 0x30)
            {
                gUnk_03005530.unkB = 1;
                if ((gUnk_03005570 & 0xFFF0) < ((gUnk_03005570 + gUnk_03005584) & 0xFFF0))
                    goto snap0;
            }
            if (!(gUnk_03005530.unkB & 1))
            {
                if (gUnk_08732EF0[gUnk_03005578] != 1)
                    goto walls;
                goto floor;
            }
            if (gUnk_08732EF0[gUnk_03005578] == 1
                && gUnk_03005530.unkC > ((gUnk_03005570 + gUnk_03005584) >> 4))
                goto floor;
        }
    snap0:
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            if (gUnk_087336F0[gUnk_03005578] == 0 || (s8)(a = gUnk_08732EF0[gUnk_03005578]) != 1
                || ((b = gUnk_03005530.unkB) & 0x30)
                || ((a & b) != 0
                    && ((gUnk_03005570 + gUnk_03005584) >> 4) >= gUnk_03005530.unkC))
            {
                gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
                gUnk_03005570 += sub_080219b0(gUnk_03005578);
            }
        }
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
        return;
    }
floor:
    if (gUnk_087336F0[gUnk_03005588] != 0)
    {
        gUnk_03005530.unkB |= 1;
        if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0)
            gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584) >> 4;
        else
            gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
    else
    {
        gUnk_03005530.unkB &= 0xFE;
    }
    u = gUnk_08735018[gUnk_03005574];
    if (u != 0)
    {
        odd = gUnk_03005574 & 1;
        side = 2;
        if (odd)
            side = 1;
        p = gUnk_087328F0[u];
        if (p[gUnk_03005508] != 0)
        {
            if (odd)
            {
                if (((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) < (gUnk_03005560 & 0xFFF0)
                    && (gUnk_087336F0[u] == 0 || (gUnk_03005530.unkB & 0x10)))
                    goto snap;
            }
            else if ((gUnk_03005560 & 0xFFF0) < ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0)
                     && (gUnk_087336F0[u] == 0 || (gUnk_03005530.unkB & 0x20)))
            {
            snap:
                gUnk_03005530.unk6 = 1;
                gUnk_03005530.unk2 = 1;
                gUnk_03005530.unk4 = gUnk_08732CF0[u];
                gUnk_03005570 += sub_080219b0(u);
                gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
                return;
            }
        }
    }
walls:
    u = gUnk_03005578;
    below = gUnk_03005588;
    cell = gUnk_03005574;
    cellBelow = gUnk_030055AC;

    /* Left corner. */
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0)
    {
        if (gUnk_08732CF0[gUnk_03005578] != 0)
        {
            if (gUnk_08732DF0[gUnk_03005578] != 0x93)
                goto slopeL;
            if (((gUnk_03005570 + gUnk_03005584) & 15) > 7)
                goto right;
            goto slopeL;
        }
        if (side == 1)
            goto right;
        if (gUnk_087339F0[u] != 0 && (gUnk_087336F0[u] == 0 || (gUnk_03005530.unkB & 1)))
            goto right;
        if (gUnk_087336F0[gUnk_03005578] != 0 && !(gUnk_03005530.unkB & 0x10))
        {
            if (!(gUnk_03005530.unkB & 2))
                goto flatL;
            if (((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) != (gUnk_0300550C & 0xFFF0))
                goto flatL;
            if (((gUnk_03005570 + gUnk_03005584) & 0xFFF0) <= (gUnk_030055B0 & 0xFFF0))
                goto flatL;
        }
        gUnk_03005530.unkB |= 1;
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
        goto right;
    }
    if (gUnk_08732CF0[gUnk_03005578] == 0)
        goto flatL;
slopeL:
    if (gUnk_03005514 >= 0 && gUnk_087336F0[gUnk_03005588] != 0
        && gUnk_08732EF0[gUnk_03005588] == 1)
    {
        if ((gUnk_08732DF0[gUnk_03005578] & 0xF0) != 0x90)
            goto clr10;
        if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83)
        {
            if (gUnk_08735018[cell] == 0)
                goto clr10;
        }
        else if (gUnk_08735018[cellBelow] == 0)
            goto clr10;
        goto set10;
    }
    goto clr12;
flatL:
    if (gUnk_03005514 >= 0 && gUnk_087336F0[gUnk_03005588] != 0
        && gUnk_08732EF0[gUnk_03005588] == 1)
    {
        if (gUnk_08732EF0[below] == 0)
        {
            gUnk_03005530.unkB |= 2;
            goto right;
        }
        goto clr2;
    }
clr12:
    gUnk_03005530.unkB &= 0xED;
    goto right;
clr2:
    gUnk_03005530.unkB &= 0xFD;
    goto right;
set10:
    gUnk_03005530.unkB |= 0x10;
    goto right;
clr10:
    gUnk_03005530.unkB &= 0xEF;
right:
    /* Right corner. */
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0)
    {
        if (gUnk_08732CF0[gUnk_03005578] != 0)
        {
            if (gUnk_08732DF0[gUnk_03005578] != 0xA3)
                goto slopeR;
            if (((gUnk_03005570 + gUnk_03005584) & 15) > 7)
                return;
            goto slopeR;
        }
        if (side == 2)
            return;
        if (gUnk_087339F0[u] != 0 && (gUnk_087336F0[u] == 0 || (gUnk_03005530.unkB & 1)))
            return;
        if (gUnk_087336F0[gUnk_03005578] != 0 && !(gUnk_03005530.unkB & 0x20))
        {
            if (!(gUnk_03005530.unkB & 4))
                goto flatR;
            if (((gUnk_03005560 + gUnk_0300559C) & 0xFFF0) != (gUnk_03005590 & 0xFFF0))
                return;
            if (((gUnk_03005570 + gUnk_03005584) & 0xFFF0) <= (gUnk_030055B0 & 0xFFF0))
                goto flatR;
        }
        gUnk_03005530.unkB |= 1;
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
        return;
    }
    if (gUnk_08732CF0[gUnk_03005578] == 0)
        goto flatR;
slopeR:
    if (gUnk_03005514 >= 0 && gUnk_087336F0[gUnk_03005588] != 0
        && gUnk_08732EF0[gUnk_03005588] == 1)
    {
        if ((gUnk_08732DF0[gUnk_03005578] & 0xF0) != 0xA0)
            goto clr20;
        if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83)
        {
            if (gUnk_08735018[cell] == 0)
                goto clr20;
        }
        else if (gUnk_08735018[cellBelow] == 0)
            goto clr20;
        goto set20;
    }
    goto clr24;
flatR:
    if (gUnk_03005514 >= 0 && gUnk_087336F0[gUnk_03005588] != 0
        && gUnk_08732EF0[gUnk_03005588] == 1)
    {
        if (gUnk_08732EF0[below] == 0)
        {
            gUnk_03005530.unkB |= 4;
            return;
        }
        goto clr4;
    }
clr24:
    gUnk_03005530.unkB &= 0xDB;
    return;
clr4:
    gUnk_03005530.unkB &= 0xFB;
    return;
set20:
    gUnk_03005530.unkB |= 0x20;
    return;
clr20:
    gUnk_03005530.unkB &= 0xDF;
}
