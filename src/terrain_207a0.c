#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_207a0.c (0x080207A0-0x0802112F, issue #84).
 *
 * The probes of two more entry points: sub_080207a0, the slope-follow probe
 * of src/terrain_1c30c.c's sub_0801c30c, and sub_08020b38, the tile-edge
 * probe of src/terrain_1c444.c's sub_0801c444.
 * 
 * Matching note: both are a goto dispatch - the velocity-sign tests first,
 * then the bodies in the order right, left, up, down - which is the only
 * spelling that gives the ROM's layout (the first jump pass moves the first
 * block of each body reached by an unconditional goto into its place). */

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
extern s8 gUnk_087336F0[];
extern s8 gUnk_087338F0[];
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;           /* Task.unk58 */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s32 gUnk_03005598;           /* Task.unk54 */

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021990(u16 a);
s32 sub_080219b0(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);

/* Slope-follow probe of sub_0801c30c (src/terrain_1c30c.c): when the pixel
   under the probe point is solid, step the point along its cell -
   horizontally by the sign of Task.unk54 (gUnk_03005598), else vertically
   by the sign of Task.unk58 (gUnk_03005514) - up to two cells, mapping a
   passable cell to its step tile (gUnk_087338F0) while unkB bit 0 is set;
   otherwise remember in unkB bit 0 whether the cell below is passable. */
void sub_080207a0(void)
{
    s32 t;

    if (sub_080216d8(gUnk_03005560, gUnk_03005570) == 0)
        goto none;
    if (gUnk_03005598 == 0)
        goto vert;
    if (gUnk_03005598 > 0)
        goto right;
    goto left;
vert:
    if (gUnk_03005514 == 0)
        return;
    if (gUnk_03005514 > 0)
        goto down;
    goto up;
none:
    if (gUnk_087336F0[gUnk_03005588] != 0)
        gUnk_03005530.unkB |= 1;
    else
        gUnk_03005530.unkB &= 0xFE;
    return;
right:
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if (!(gUnk_03005530.unkB & 1))
            return;
        t = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        t = gUnk_03005578;
    }
    gUnk_03005530.unk0 = 1;
    gUnk_03005530.unk4 = gUnk_08732CF0[t];
    gUnk_03005560 += sub_080219f0(t);
    if (gUnk_08732CF0[gUnk_03005578] != 0 && (gUnk_08732CF0[gUnk_03005578] & 1))
        return;
    if (sub_08021634(gUnk_03005560, gUnk_03005570) == 0)
        return;
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if (!(gUnk_03005530.unkB & 1))
            return;
        t = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        t = gUnk_03005578;
    }
    gUnk_03005530.unk4 = gUnk_08732CF0[t];
    gUnk_03005560 += sub_080219f0(t);
    return;
left:
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if (!(gUnk_03005530.unkB & 1))
            return;
        t = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        t = gUnk_03005578;
    }
    gUnk_03005530.unk0 = 2;
    gUnk_03005530.unk4 = gUnk_08732CF0[t];
    gUnk_03005560 += sub_080219d0(t);
    if (gUnk_08732CF0[gUnk_03005578] != 0 && !(gUnk_08732CF0[gUnk_03005578] & 1))
        return;
    if (sub_08021634(gUnk_03005560, gUnk_03005570) == 0)
        return;
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if (!(gUnk_03005530.unkB & 1))
            return;
        t = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        t = gUnk_03005578;
    }
    gUnk_03005530.unk4 = gUnk_08732CF0[t];
    gUnk_03005560 += sub_080219d0(t);
    return;
up:
    if (sub_08021634(gUnk_03005560, gUnk_03005570) == 0)
        return;
    if (gUnk_087336F0[gUnk_03005578] != 0)
        return;
    gUnk_03005530.unk1 = 1;
    gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
    gUnk_03005570 += sub_08021990(gUnk_03005578);
    if (!(gUnk_08732DF0[gUnk_03005578] & 0x40)
        && sub_08021634(gUnk_03005560, gUnk_03005570) != 0)
    {
        gUnk_03005570 += sub_08021990(gUnk_03005578);
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
    }
    return;
down:
    if (gUnk_087336F0[gUnk_03005578] == 0 || (gUnk_03005530.unkB & 1))
    {
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unk6 = 1;
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        if (!(gUnk_08732DF0[gUnk_03005578] & 0x80)
            && sub_08021634(gUnk_03005560, gUnk_03005570) != 0
            && (gUnk_087336F0[gUnk_03005578] == 0 || (gUnk_03005530.unkB & 1)))
        {
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005570 += sub_080219b0(gUnk_03005578);
        }
    }
}

/* The tile-edge probe of sub_0801c444 (src/terrain_1c444.c): with the box
   moving sideways, a wall probe at the probe point that steps onto
   gUnk_087338F0's step tiles and remembers passable wall tiles in
   gUnk_03005530.unkB bits 1/2; with it moving vertically, a ceiling or
   floor probe that sets unkB to the side (2 = left, 4 = right) on which
   the tile edge it stopped at continues. */
void sub_08020b38(void)
{
    s32 u;

    if (gUnk_03005598 == 0)
        goto vert;
    if (gUnk_03005598 > 0)
        goto right;
    goto left;
vert:
    if (gUnk_03005514 == 0)
        return;
    if (gUnk_03005514 > 0)
        goto down;
    goto up;
right:
    if (sub_080217dc(gUnk_03005560, gUnk_03005570) == 0)
    {
        gUnk_03005530.unkB &= 0xF9;
        if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
            && (gUnk_08732CF0[gUnk_03005578] & 1) != 0)
        {
            gUnk_03005530.unkB |= 4;
            return;
        }
        if (gUnk_087336F0[gUnk_03005510] == 0)
            return;
        if (gUnk_08732CF0[gUnk_03005510] == 0)
            return;
        if ((gUnk_08732CF0[gUnk_03005510] & 1) == 0)
            return;
        if ((gUnk_08732DF0[gUnk_03005510] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
            return;
        gUnk_03005530.unkB |= 4;
        return;
    }
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if ((gUnk_03005530.unkB & 4) == 0)
            return;
        u = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        u = gUnk_03005578;
    }
    gUnk_03005530.unk0 = 1;
    gUnk_03005530.unk4 = gUnk_08732CF0[u];
    gUnk_03005560 += sub_080219f0(u);
    if ((gUnk_08732CF0[gUnk_03005578] == 0 || (gUnk_08732CF0[gUnk_03005578] & 1) == 0)
        && sub_08021634(gUnk_03005560, gUnk_03005570) != 0)
    {
        if (gUnk_087336F0[gUnk_03005578] != 0)
        {
            if ((gUnk_03005530.unkB & 4) == 0)
                return;
            u = gUnk_087338F0[gUnk_03005578];
        }
        else
        {
            u = gUnk_03005578;
        }
        gUnk_03005530.unk4 = gUnk_08732CF0[u];
        gUnk_03005560 += sub_080219f0(u);
    }
    gUnk_03005530.unkB = 0;
    if (gUnk_03005530.unk4 != 0)
        gUnk_03005560++;
    return;
left:
    if (sub_080217dc(gUnk_03005560, gUnk_03005570) == 0)
    {
        gUnk_03005530.unkB &= 0xFD;
        if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
            && (gUnk_08732CF0[gUnk_03005578] & 1) == 0)
        {
            gUnk_03005530.unkB |= 2;
            return;
        }
        if (gUnk_087336F0[gUnk_03005594] == 0)
            return;
        if (gUnk_08732CF0[gUnk_03005594] == 0)
            return;
        if ((gUnk_08732CF0[gUnk_03005594] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gUnk_03005594] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
            return;
        gUnk_03005530.unkB |= 2;
        return;
    }
    if (gUnk_087336F0[gUnk_03005578] != 0)
    {
        if ((gUnk_03005530.unkB & 2) == 0)
            return;
        u = gUnk_087338F0[gUnk_03005578];
    }
    else
    {
        u = gUnk_03005578;
    }
    gUnk_03005530.unk0 = 2;
    gUnk_03005530.unk4 = gUnk_08732CF0[u];
    gUnk_03005560 += sub_080219d0(u);
    if ((gUnk_08732CF0[gUnk_03005578] == 0 || (gUnk_08732CF0[gUnk_03005578] & 1) != 0)
        && sub_08021634(gUnk_03005560, gUnk_03005570) != 0)
    {
        if (gUnk_087336F0[gUnk_03005578] != 0)
        {
            if ((gUnk_03005530.unkB & 2) == 0)
                return;
            u = gUnk_087338F0[gUnk_03005578];
        }
        else
        {
            u = gUnk_03005578;
        }
        gUnk_03005530.unk4 = gUnk_08732CF0[u];
        gUnk_03005560 += sub_080219d0(u);
    }
    gUnk_03005530.unkB = 0;
    if (gUnk_03005530.unk4 != 0)
        gUnk_03005560--;
    return;
up:
    if (sub_080217dc(gUnk_03005560, gUnk_03005570) == 0)
        return;
    if (gUnk_087336F0[gUnk_03005578] != 0)
        return;
    gUnk_03005530.unk1 = 1;
    gUnk_03005570 += sub_08021990(gUnk_03005578);
    if ((gUnk_08732DF0[gUnk_03005578] & 0x40) == 0
        && sub_08021634(gUnk_03005560, gUnk_03005570) != 0)
        gUnk_03005570 += sub_08021990(gUnk_03005578);
    gUnk_03005530.unkB = 0;
    gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
    if (gUnk_03005530.unk4 == 0)
        return;
    gUnk_03005570--;
    if ((gUnk_03005530.unk4 & 1) != 0)
    {
        if (gUnk_087336F0[gUnk_03005594] == 0)
            return;
        if (gUnk_08732CF0[gUnk_03005594] == 0)
            return;
        if ((gUnk_08732CF0[gUnk_03005594] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gUnk_03005594] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
            return;
        gUnk_03005530.unkB = 2;
        return;
    }
    if (gUnk_087336F0[gUnk_03005510] == 0)
        return;
    if (gUnk_08732CF0[gUnk_03005510] == 0)
        return;
    if ((gUnk_08732CF0[gUnk_03005510] & 1) == 0)
        return;
    if ((gUnk_08732DF0[gUnk_03005510] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
        return;
    gUnk_03005530.unkB = 4;
    return;
down:
    if (sub_080217dc(gUnk_03005560, gUnk_03005570) == 0)
        return;
    gUnk_03005530.unk2 = 1;
    gUnk_03005570 += sub_080219b0(gUnk_03005578);
    if ((gUnk_08732DF0[gUnk_03005578] & 0x80) == 0
        && sub_08021634(gUnk_03005560, gUnk_03005570) != 0)
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
    gUnk_03005530.unkB = 0;
    gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
    if (gUnk_03005530.unk4 == 0)
        return;
    gUnk_03005570++;
    if ((gUnk_03005530.unk4 & 1) != 0)
    {
        if (gUnk_087336F0[gUnk_03005594] == 0)
            return;
        if (gUnk_08732CF0[gUnk_03005594] == 0)
            return;
        if ((gUnk_08732CF0[gUnk_03005594] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gUnk_03005594] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 6)
            return;
        gUnk_03005530.unkB = 2;
        return;
    }
    if (gUnk_087336F0[gUnk_03005510] == 0)
        return;
    if (gUnk_08732CF0[gUnk_03005510] == 0)
        return;
    if ((gUnk_08732CF0[gUnk_03005510] & 1) == 0)
        return;
    if ((gUnk_08732DF0[gUnk_03005510] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
        return;
    gUnk_03005530.unkB = 4;
}
