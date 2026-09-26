#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1f540.c (0x0801F540-0x0801FF83, issue #84).
 *
 * The probe sets of three more entry points in src/terrain_1bcac.c:
 * sub_0801bf1c's right/left wall probes sub_0801f540/sub_0801f6b0 (the
 * box's top corner through a gUnk_08732DF0 wall class, then the middle and
 * bottom corners), sub_0801c030's floor probes sub_0801f800 (land on the
 * floor, a floor-follow without slope tiles) and sub_0801f9b8 (land while
 * not moving up, tracking fall-through tiles in gUnk_03005530.unkB), and
 * sub_0801c12c's pair sub_0801fc48 (on the ground: follow the floor or
 * drop off it) / sub_0801fe2c (in the air: land). */

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
extern u8 gUnk_08732DF0[];
extern s8 gUnk_08732EF0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087339F0[];
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;           /* Task.unk58 */
extern s16 gUnk_0300551C;           /* box left offset */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_0300557C;           /* box top offset */
extern s16 gUnk_03005584;           /* box bottom offset */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern s16 gUnk_03005590;           /* box right (room-relative) */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s16 gUnk_0300559C;           /* box right offset */

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021970(u16 a);
s32 sub_08021990(u16 a);
s32 sub_080219b0(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);

/* Right wall probe of the third entry point (sub_0801bf1c). */
void sub_0801f540(void)
{
    s32 d;
    s32 x;
    s32 a;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0
            || (gUnk_08732CF0[gUnk_03005578] != 0 && (gUnk_08732DF0[gUnk_03005578] & 0xF0) == 0xA0)))
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 1;
            return;
        }
    }
    x = gUnk_03005560 + gUnk_0300559C;
    if ((x & 0xFFF0) == (gUnk_03005590 & 0xFFF0))
        return;
    if (sub_080217dc(x, gUnk_03005570 + gUnk_0300557C) != 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1
        && gUnk_087336F0[gUnk_03005578] != 0)
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = a;
            return;
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1
        && gUnk_087336F0[gUnk_03005578] != 0)
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = a;
        }
    }
}

/* Left wall probe of the third entry point, the mirror image of
   sub_0801f540. */
void sub_0801f6b0(void)
{
    s32 d;
    s32 x;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0
            || (gUnk_08732CF0[gUnk_03005578] != 0 && (gUnk_08732DF0[gUnk_03005578] & 0xF0) == 0x90)))
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
            return;
        }
    }
    x = gUnk_03005560 + gUnk_0300551C;
    if ((x & 0xFFF0) == (gUnk_03005590 & 0xFFF0))
        return;
    if (sub_080217dc(x, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_08732EF0[gUnk_03005578] == 1
        && gUnk_087336F0[gUnk_03005578] != 0)
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
            return;
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && gUnk_08732EF0[gUnk_03005578] == 1
        && gUnk_087336F0[gUnk_03005578] != 0)
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0)
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
        }
    }
}

/* Floor probe of the fourth entry point (sub_0801c030): sub_0801dee8
   without the slope-tile case. */
void sub_0801f800(void)
{
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_087336F0[gUnk_03005578] == 0)
    {
        gUnk_03005530.unk1++;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) != 0)
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005570 += sub_08021990(gUnk_03005578);
        }
        return;
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (gUnk_087339F0[gUnk_03005510] == 0 || gUnk_087336F0[gUnk_03005510] != 0))
    {
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        gUnk_03005530.unk1++;
        return;
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (gUnk_087339F0[gUnk_03005594] == 0 || gUnk_087336F0[gUnk_03005594] != 0))
    {
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        gUnk_03005530.unk1++;
    }
}

/* Floor probe (none while gUnk_03005514, Task.unk58, is negative): land
   the box's bottom on the floor cell under it, else on a slope under one
   of its bottom corners.  Bits 0-2 of gUnk_03005530.unkB record, for the
   centre and the two corners, whether the cell below is a
   gUnk_087336F0 tile, and let such a cell be landed on next time. */
void sub_0801f9b8(void)
{
    s32 a;
    s32 a2;

    if (gUnk_03005514 < 0)
    {
        gUnk_03005530.unkB = 0;
        return;
    }
    if (sub_080216d8(gUnk_03005560, gUnk_03005584 + gUnk_03005570) != 0
        && (gUnk_087336F0[gUnk_03005578] == 0 || (gUnk_03005530.unkB & 1)))
    {
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005570 += sub_080219b0(gUnk_03005578);
        }
        return;
    }
    if (gUnk_087336F0[gUnk_03005588] != 0)
        gUnk_03005530.unkB |= 1;
    else
        gUnk_03005530.unkB &= ~1;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1)
    {
        if (gUnk_087336F0[gUnk_03005578] == 0 || (gUnk_03005530.unkB & 2))
        {
            gUnk_03005570 += sub_080219b0(gUnk_03005578);
            gUnk_03005530.unk6 = 1;
            gUnk_03005530.unk2 = 1;
        }
    }
    else
    {
        if (gUnk_087336F0[gUnk_03005588] != 0 && gUnk_08732EF0[gUnk_03005588] == 1)
            gUnk_03005530.unkB |= 2;
        else
            gUnk_03005530.unkB &= ~2;
    }
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && (a2 = gUnk_08732EF0[gUnk_03005578]) == 1)
    {
        if (gUnk_087336F0[gUnk_03005578] != 0 && !(gUnk_03005530.unkB & 4))
            return;
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
    }
    if (gUnk_087336F0[gUnk_03005588] != 0 && gUnk_08732EF0[gUnk_03005588] == 1)
        gUnk_03005530.unkB |= 4;
    else
        gUnk_03005530.unkB &= ~4;
}

/* Floor probe of the fifth entry point (sub_0801c12c) for a box standing on
   the ground: keep it on the floor cell under it (or the cell below), else
   count the floor cells under its two bottom corners and clear the
   on-ground flags when there are none (the box starts to fall). */
void sub_0801fc48(void)
{
    s32 n;
    u16 t;

    gUnk_03005530.unk2++;
    gUnk_03005530.unkB = 0;
    sub_080216d8(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
    if (gUnk_08732EF0[gUnk_03005578] == 0)
    {
        if (gUnk_08732EF0[gUnk_03005588] == 0)
            goto count;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005588];
        gUnk_03005570 += sub_08021970(gUnk_03005588) + 16;
    }
    else
    {
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_08021970(gUnk_03005578);
    }
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
    {
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_08021970(gUnk_03005578);
    }
    return;
count:
    n = 0;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0)
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        t = gUnk_03005578;
    }
    else
    {
        t = gUnk_03005588;
    }
    if (gUnk_08732EF0[t] != 0)
        n += 2;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0)
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        t = gUnk_03005578;
    }
    else
    {
        t = gUnk_03005588;
    }
    if (gUnk_08732EF0[t] != 0)
        n += 1;
    if (n == 0)
        gUnk_03005530.unk2 = gUnk_03005530.unk3 = gUnk_03005530.unk6 = gUnk_03005530.unk5 = 0;
}

/* Landing probe of the fifth entry point (sub_0801c12c) for a box in the
   air: land the box's bottom on the floor cell under it, else a bottom
   corner on a floor cell, and set the on-ground flags. */
void sub_0801fe2c(void)
{
    s32 a;
    s32 a2;

    if (sub_080216d8(gUnk_03005560, gUnk_03005584 + gUnk_03005570) != 0)
    {
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005570 += sub_080219b0(gUnk_03005578);
        }
        return;
    }
    if (sub_08021634(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1)
    {
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    }
    if (sub_08021634(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && (a2 = gUnk_08732EF0[gUnk_03005578]) == 1)
    {
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk6 = 1;
        gUnk_03005530.unk2 = 1;
    }
}
