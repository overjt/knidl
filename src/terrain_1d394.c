#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1d394.c (0x0801D394-0x0801D9C7, issue #84).
 *
 * The floor probe sub_0801bcac runs for a box standing on the ground
 * (gUnk_03005530.unk6 != 0), the simpler sibling of sub_0801c930 in
 * src/terrain_1c930.c. */

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
extern s8 gUnk_087336F0[];
extern u16 gUnk_08735018[];         /* indexed by the cell's byte 2 */
extern u16 gUnk_03005508;           /* pixel offset inside the queried cell */
extern s16 gUnk_0300551C;           /* box left offset */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_03005584;           /* box bottom offset */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_0300559C;           /* box right offset */
extern u16 gUnk_030055AC;           /* cell below: byte 2 */

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_08021970(u16 a);
s32 sub_08021ab4(u32 x, u32 y);

/* Floor probe of a box standing on the ground (gUnk_03005530.unk6 != 0). */
void sub_0801d394(void)
{
    s32 tile;
    s32 tile2;
    s32 hit;
    s32 dir;

    gUnk_03005530.unk2++;
    gUnk_03005530.unkB = 0;
    sub_080216d8(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
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
    gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005588];
    gUnk_03005570 += sub_08021970(gUnk_03005588) + 16;
    goto check;

floor:
    gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
    gUnk_03005570 += sub_08021970(gUnk_03005578);
check:
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0 || gUnk_08732CF0[gUnk_03005578] != 0
            || gUnk_03005530.unkC <= (gUnk_03005570 + gUnk_03005584) >> 4))
    {
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_08021970(gUnk_03005578);
    }
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    return;

slope:
    tile = gUnk_08735018[gUnk_03005574];
    if (dir == 1)
    {
        if (hit != 0)
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[tile];
            gUnk_03005570 += sub_08021970(tile);
        }
        if (gUnk_03005598 > 0)
            gUnk_03005530.unk3 = dir;
    }
    else if (dir == 2)
    {
        if (hit != 0)
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[tile];
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
        gUnk_03005530.unk4 = gUnk_08732CF0[tile];
        gUnk_03005570 += sub_08021970(tile) + 16;
        if (gUnk_03005598 > 0)
            gUnk_03005530.unk3 = dir;
    }
    else if (dir == 2)
    {
        gUnk_03005530.unk4 = gUnk_08732CF0[tile];
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
    if (gUnk_087336F0[tile2] != 0)
        gUnk_03005530.unkB |= 4;
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
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    return;

clear:
    gUnk_03005530.unk5 = 0;
    gUnk_03005530.unk6 = 0;
    gUnk_03005530.unk3 = 0;
    gUnk_03005530.unk2 = 0;
}
