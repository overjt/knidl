#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1d9c8.c (0x0801D9C8-0x0801E177, issue #84).
 *
 * The probes src/terrain_1bcac.c's entry points run for a box in the air
 * (gUnk_03005530.unk6 == 0): the right and left wall probes sub_0801d9c8 /
 * sub_0801dc88 (top, middle and, in the moving direction, bottom corner of
 * the box edge) and the landing probe sub_0801dee8.
 * 
 * Matching notes: a tile attribute the ROM tests with `cmp #1` and then ANDs
 * with a flag is `(flags & 1)`, the constant: cse knows the attribute
 * register holds 1 and substitutes it for the constant, which keeps the AND
 * in place on that register; writing `a & flags` lets cse swap the operands
 * instead.  The landing probe's two side tests each end with their own
 * `gUnk_03005570 += ...; gUnk_03005530.unk1++;` (lesson 3.430). */

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
extern s8 gUnk_08732EF0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087339F0[];
extern u16 gUnk_03005508;           /* pixel offset inside the queried cell */
extern s16 gUnk_0300550C;           /* box left (room-relative) */
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;           /* Task.unk58 */
extern s16 gUnk_0300551C;           /* box left offset */
extern s16 gUnk_03005520;           /* actor y (room-relative) */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_0300557C;           /* box top offset */
extern s16 gUnk_03005584;           /* box bottom offset */
extern s16 gUnk_03005590;           /* box right (room-relative) */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_0300559C;           /* box right offset */
extern s16 gUnk_030055A4;           /* box top (room-relative) */
extern s16 gUnk_030055B0;           /* box bottom (room-relative) */
extern u16 gUnk_08735098[];

s32 sub_08021634(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021990(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);

/* Right wall probe of a box in the air (gUnk_03005530.unk6 == 0), the
   mirror image of sub_0801dc88: the wall cell's attribute (1) goes to
   gUnk_03005530.unk0. */
void sub_0801d9c8(void)
{
    s32 d;
    s32 a;
    s32 a2;
    s32 a3;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1
        && (gUnk_087339F0[gUnk_03005594] == 0 || gUnk_087336F0[gUnk_03005594] != 0))
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0 && (gUnk_03005520 & 0xFFF0) == (gUnk_03005570 & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 1;
            return;
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (a2 = gUnk_08732EF0[gUnk_03005578]) == 1
        && (gUnk_087339F0[gUnk_03005594] == 0 || gUnk_087336F0[gUnk_03005594] != 0))
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0
            && (gUnk_030055A4 & 0xFFF0) <= ((gUnk_03005570 + gUnk_0300557C) & 0xFFF0)
            && ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0) > (gUnk_03005590 & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 1;
            return;
        }
    }
    if (gUnk_03005598 > 0
        && sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (a3 = gUnk_08732EF0[gUnk_03005578]) == 1
        && (gUnk_087339F0[gUnk_03005594] == 0
            || (gUnk_087336F0[gUnk_03005594] != 0 && (gUnk_03005530.unkB & 1) == 0)))
    {
        d = sub_080219f0(gUnk_03005578);
        if (d != 0
            && ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0) > (gUnk_03005590 & 0xFFF0)
            && (gUnk_030055B0 & 0xFFF0) >= ((gUnk_03005570 + gUnk_03005584) & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 1;
        }
    }
}

/* Left wall probe of a box in the air (gUnk_03005530.unk6 == 0): the top,
   middle and (when moving left) bottom of the box's left edge, each pushing
   the probe x out of a wall cell and setting gUnk_03005530.unk0 = 2. */
void sub_0801dc88(void)
{
    s32 d;
    s32 a;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && gUnk_08732EF0[gUnk_03005578] == 1
        && (gUnk_087339F0[gUnk_03005510] == 0 || gUnk_087336F0[gUnk_03005510] != 0))
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0 && (gUnk_03005520 & 0xFFF0) == (gUnk_03005570 & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
            return;
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && gUnk_08732EF0[gUnk_03005578] == 1
        && (gUnk_087339F0[gUnk_03005510] == 0 || gUnk_087336F0[gUnk_03005510] != 0))
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0
            && (gUnk_030055A4 & 0xFFF0) <= ((gUnk_03005570 + gUnk_0300557C) & 0xFFF0)
            && ((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) < (gUnk_0300550C & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
            return;
        }
    }
    if (gUnk_03005598 < 0
        && sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (a = gUnk_08732EF0[gUnk_03005578]) == 1
        && (gUnk_087339F0[gUnk_03005510] == 0
            || (gUnk_087336F0[gUnk_03005510] != 0 && (gUnk_03005530.unkB & 1) == 0)))
    {
        d = sub_080219d0(gUnk_03005578);
        if (d != 0
            && ((gUnk_03005560 + gUnk_0300551C) & 0xFFF0) < (gUnk_0300550C & 0xFFF0)
            && (gUnk_030055B0 & 0xFFF0) >= ((gUnk_03005570 + gUnk_03005584) & 0xFFF0))
        {
            gUnk_03005560 += d;
            gUnk_03005530.unk0 = 2;
        }
    }
}

/* Landing probe of a box in the air (gUnk_03005530.unk6 == 0): land on the
   floor cell under the probe point (or the slope tile gUnk_08735098 maps
   the cell's byte 2 to) and move the probe y onto it, else land one of the
   box's bottom corners on a floor edge. */
void sub_0801dee8(void)
{
    s32 slope;
    u16 *p;
    u16 *tbl;
    u16 t;
    s8 *q;

    slope = 0;
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) == 0)
    {
        tbl = gUnk_08735098;
        p = &tbl[gUnk_03005574];
        if (*p == 0)
            goto side;
        slope = (gUnk_03005574 & 1) ? 1 : 2;
        t = *p;
        q = gUnk_087328F0[t];
        if (q[gUnk_03005508] == 0)
            goto side;
        if (gUnk_03005514 >= 0)
            return;
        if (slope == 1)
        {
            if ((gUnk_03005560 & 0xFFF0) == ((gUnk_03005560 + gUnk_0300551C) & 0xFFF0))
                return;
        }
        else if (slope == 2)
        {
            if ((gUnk_03005560 & 0xFFF0) == ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0))
                return;
        }
        gUnk_03005530.unk1++;
        gUnk_03005530.unk4 = gUnk_08732CF0[t];
        gUnk_03005570 += sub_08021990(t);
        return;
    }
    if (gUnk_087336F0[gUnk_03005578] == 0)
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
side:
    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (gUnk_087339F0[gUnk_03005510] == 0 || gUnk_087336F0[gUnk_03005510] != 0)
        && slope == 0)
    {
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        gUnk_03005530.unk1++;
        return;
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_0300557C) != 0
        && gUnk_08732CF0[gUnk_03005578] == 0
        && gUnk_087336F0[gUnk_03005578] == 0
        && (gUnk_087339F0[gUnk_03005594] == 0 || gUnk_087336F0[gUnk_03005594] != 0)
        && slope == 0)
    {
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        gUnk_03005530.unk1++;
    }
}
