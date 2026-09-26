#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1c930.c (0x0801C930-0x0801D393, issue #84).
 *
 * The floor probe sub_0801bde0 runs for a box standing on the ground
 * (gUnk_03005530.unk6 != 0): a wall step in the moving direction first,
 * then the floor under the box (the step and slope attribute tables
 * gUnk_087338F0/gUnk_087337F0/gUnk_087334F0 and gUnk_08735018), the result
 * flags gUnk_03005530.unkD/unk5/unkE, and the ledge counter
 * gUnk_03005530.unk10 when the probe finds no floor. */

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

extern u8 gUnk_08732CF0[];
extern s8 gUnk_08732EF0[];
extern u8 gUnk_087334F0[];
extern s8 gUnk_087336F0[];
extern u8 gUnk_087337F0[];
extern s8 gUnk_087338F0[];
extern u16 gUnk_08735018[];         /* indexed by the cell's byte 2 */
extern u16 gUnk_03005508;           /* pixel offset inside the queried cell */
extern u16 gUnk_03005510;
extern s16 gUnk_0300551C;           /* box left offset */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern u8 gUnk_03005564;            /* Task.unk43 */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_03005584;           /* box bottom offset */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_0300559C;           /* box right offset */
extern u16 gUnk_030055AC;           /* cell below: byte 2 */
extern u8 gUnk_02005574[];
extern u8 gUnk_03005568;

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021970(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);
void sub_08021a10(u16 a);
s32 sub_08021ab4(u32 x, u32 y);
s32 sub_08021b18(u16 x, u16 y);

/* Floor probe of a box standing on the ground (gUnk_03005530.unk6 != 0),
   sub_0801bde0's counterpart of sub_0801d394: a wall step into the moving
   direction first, the flags gUnk_03005530.unkD/unk5/unkE on top, and the
   ledge counter gUnk_03005530.unk10 when the probe finds no floor. */
void sub_0801c930(void)
{
    s32 d;
    s32 hit;
    s32 dir;
    s32 tile;
    s32 tile2;

    gUnk_03005530.unk2++;
    gUnk_03005530.unkB = 0;
    if (gUnk_02005574[0] == 0 && (gUnk_03005568 & 4)
        && sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
    {
        if (gUnk_03005598 == 0)
            d = (s8)gUnk_03005564;
        else
        {
            d = -1;
            if (gUnk_03005598 > 0)
                d = 1;
        }
        if (d == 1)
        {
            if (sub_08021ab4(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) == 0)
                goto next;
            gUnk_03005530.unk0 = d;
            if (gUnk_087336F0[gUnk_03005578] != 0)
            {
                tile = gUnk_087338F0[gUnk_03005578];
                goto right;
            }
            /* The ROM places this arm's `tile = t` and its steps after
               the left arm (the gotos reproduce that layout). */
            goto right_t;
        }
        if (sub_08021ab4(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0)
        {
            gUnk_03005530.unk0 = 2;
            if (gUnk_087336F0[gUnk_03005578] != 0)
                tile = gUnk_087338F0[gUnk_03005578];
            else
                tile = gUnk_03005578;
            gUnk_03005560 += sub_080219d0(tile);
            if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
            {
                if (gUnk_087336F0[gUnk_03005578] != 0)
                    tile = gUnk_087338F0[gUnk_03005578];
                else
                    tile = gUnk_03005578;
                gUnk_03005560 += sub_080219d0(tile);
            }
        }
        goto next;
    right_t:
        tile = gUnk_03005578;
    right:
        gUnk_03005560 += sub_080219f0(tile);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            if (gUnk_087336F0[gUnk_03005578] != 0)
                tile = gUnk_087338F0[gUnk_03005578];
            else
                tile = gUnk_03005578;
            gUnk_03005560 += sub_080219f0(tile);
        }
    }
next:
    sub_080216d8(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
    if (!(gUnk_03005530.unkD & 0x10))
    {
        if (gUnk_03005578 == 48 || gUnk_03005578 == 49)
            gUnk_03005530.unkD = 1;
        if (gUnk_03005588 == 49)
            gUnk_03005530.unkD = 2;
    }
    else
    {
        gUnk_03005530.unkD |= 1;
        if (gUnk_03005578 == 48 || gUnk_03005578 == 49)
        {
            gUnk_03005530.unkD |= 2;
            goto clear;
        }
        gUnk_03005530.unkD = 0;
    }
    if (gUnk_08732EF0[gUnk_03005578] != 0)
    {
        if (gUnk_087336F0[gUnk_03005578] == 0)
            goto floor;
        gUnk_03005530.unkB |= 1;
        if (gUnk_08732CF0[gUnk_03005578] != 0)
            goto floor;
        if (gUnk_03005530.unkC <= (gUnk_03005570 + gUnk_03005584) >> 4)
            goto floor;
    }
    if (gUnk_08735018[gUnk_03005574] != 0)
    {
        hit = 0;
        if (gUnk_03005574 & 1)
        {
            dir = 1;
            if (((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) < (gUnk_03005560 & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gUnk_03005560 & 0xFFF0) < ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0))
                hit = 1;
        }
        if (hit != 0 || gUnk_03005508 + 16 <= 255)
            goto slope;
    }
    if (gUnk_08732EF0[gUnk_03005588] != 0)
    {
        if (gUnk_087336F0[gUnk_03005588] != 0)
            gUnk_03005530.unkB |= 1;
    }
    else
    {
        if (gUnk_08735018[gUnk_030055AC] == 0)
            goto edges;
        hit = 0;
        if (gUnk_030055AC & 1)
        {
            dir = 1;
            if (((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) < (gUnk_03005560 & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gUnk_03005560 & 0xFFF0) < ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0))
                hit = 1;
        }
        if (hit == 0)
            goto edges;
        goto slope_below;
    }
    sub_08021a10(gUnk_03005588);
    gUnk_03005570 += sub_08021970(gUnk_03005588) + 16;
    goto check;

floor:
    sub_08021a10(gUnk_03005578);
    gUnk_03005570 += sub_08021970(gUnk_03005578);
check:
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0 || gUnk_08732CF0[gUnk_03005578] != 0
            || gUnk_03005530.unkC <= (gUnk_03005570 + gUnk_03005584) >> 4))
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        sub_08021a10(gUnk_03005578);
    }
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0)
        hit = (s8)gUnk_087337F0[gUnk_03005578];
    else
        hit = (s8)gUnk_087337F0[gUnk_03005588];
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0)
        hit &= (s8)gUnk_087337F0[gUnk_03005578];
    else
        hit &= (s8)gUnk_087337F0[gUnk_03005588];
    gUnk_03005530.unk5 = hit;
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    return;

slope:
    tile = gUnk_08735018[gUnk_03005574];
    if (dir == 1)
    {
        if (hit != 0)
        {
            sub_08021a10(tile);
            gUnk_03005570 += sub_08021970(tile);
        }
        if (gUnk_03005598 > 0)
            gUnk_03005530.unk3 = dir;
    }
    else if (dir == 2)
    {
        if (hit != 0)
        {
            sub_08021a10(tile);
            gUnk_03005570 += sub_08021970(tile);
        }
        if (gUnk_03005598 < 0)
            gUnk_03005530.unk3 = 1;
    }
    goto sides;

slope_below:
    tile = gUnk_08735018[gUnk_030055AC];
    if (dir == 1)
    {
        sub_08021a10(tile);
        gUnk_03005570 += sub_08021970(tile) + 16;
        if (gUnk_03005598 > 0)
            gUnk_03005530.unk3 = dir;
    }
    else if (dir == 2)
    {
        sub_08021a10(tile);
        gUnk_03005570 += sub_08021970(tile) + 16;
        if (gUnk_03005598 < 0)
            gUnk_03005530.unk3 = 1;
    }
sides:
    if (sub_08021ab4(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) == 0)
        dir &= ~1;
    if (sub_08021ab4(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) == 0)
        dir &= ~2;
    if (dir == 0)
        goto clear;
    if (gUnk_087336F0[tile] != 0)
    {
        if (dir == 1)
            gUnk_03005530.unkB = 16;
        else
            gUnk_03005530.unkB = 32;
    }
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    return;

edges:
    dir = 0;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0 || gUnk_08732CF0[gUnk_03005578] != 0
            || (gUnk_03005570 + gUnk_03005584) >> 4 >= gUnk_03005530.unkC))
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        tile = gUnk_03005578;
    }
    else
    {
        tile = gUnk_03005588;
    }
    if (gUnk_08732EF0[tile] != 0)
        dir += 2;
    hit = (s8)gUnk_087337F0[tile];
    if (gUnk_087336F0[tile] != 0)
        gUnk_03005530.unkB |= 2;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0 || gUnk_08732CF0[gUnk_03005578] != 0
            || (gUnk_03005570 + gUnk_03005584) >> 4 >= gUnk_03005530.unkC))
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        tile2 = gUnk_03005578;
    }
    else
    {
        tile2 = gUnk_03005588;
    }
    if (gUnk_08732EF0[tile2] != 0)
        dir++;
    hit &= (s8)gUnk_087337F0[tile2];
    if (gUnk_087336F0[tile2] != 0)
        gUnk_03005530.unkB |= 4;
    gUnk_03005530.unk5 = hit;
    if (dir == 0)
        goto clear;
    if (dir <= 2 && gUnk_03005598 != 0)
    {
        if (gUnk_03005598 < 0)
        {
            if (dir == 1)
                gUnk_03005530.unk3++;
        }
        else if (gUnk_03005598 > 0 && dir == 2)
        {
            gUnk_03005530.unk3++;
        }
    }
    gUnk_03005530.unkE = gUnk_087334F0[tile] | gUnk_087334F0[tile2];
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    return;

clear:
    gUnk_03005530.unk5 = 0;
    gUnk_03005530.unk6 = 0;
    gUnk_03005530.unk3 = 0;
    gUnk_03005530.unk2 = 0;
    sub_080217dc(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
    if (gUnk_08732EF0[gUnk_03005578] == 0 && gUnk_03005574 == 0 && gUnk_03005598 != 0)
    {
        if (gUnk_03005598 > 0)
        {
            if (gUnk_08732EF0[gUnk_03005510] == 0)
            {
                tile = sub_08021b18(gUnk_03005560 + 16, gUnk_03005570 + gUnk_03005584 + 1);
                if (gUnk_08732EF0[tile] == 1)
                    gUnk_03005530.unk10++;
            }
        }
        else
        {
            if (gUnk_08732EF0[gUnk_03005594] == 0)
            {
                tile = sub_08021b18(gUnk_03005560 - 16, gUnk_03005570 + gUnk_03005584 + 1);
                if (gUnk_08732EF0[tile] == 1)
                    gUnk_03005530.unk10++;
            }
        }
    }
}
