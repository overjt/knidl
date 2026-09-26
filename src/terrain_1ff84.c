#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1ff84.c (0x0801FF84-0x0802069B, issue #84).
 *
 * The probe set of the sixth entry point (src/terrain_1bcac.c's
 * sub_0801c230): a wall probe in the moving direction that steps the box up
 * gUnk_087338F0's step tiles and remembers passable wall tiles in
 * gUnk_03005530.unkB bits 1/2, a ceiling and a floor probe while the box
 * moves vertically, and the on-floor bookkeeping (unkB bit 0, the floor row
 * unkC); plus sub_08020698, an empty dead export.
 * 
 * Matching note: a tile attribute read for a test (`gUnk_08732CF0[t] != 0 &&
 * (gUnk_08732CF0[t] & 1)`) is written as two table reads, not a local: the
 * local's AND is tied in place by regmove, the cse'd read is not. */

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
extern s8 gUnk_087339F0[];
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;           /* Task.unk58 */
extern s16 gUnk_0300551C;           /* box left offset */
extern s16 gUnk_03005520;           /* actor y (room-relative) */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_0300557C;           /* box top offset */
extern s16 gUnk_03005584;           /* box bottom offset */
extern u16 gUnk_03005588;           /* cell below: tile set */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_0300559C;           /* box right offset */

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021990(u16 a);
s32 sub_080219b0(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);

/* The probe set of the sixth entry point (sub_0801c230): a wall probe in
   the moving direction that steps the box onto gUnk_087338F0's step tiles
   and remembers passable wall tiles in gUnk_03005530.unkB bits 1 and 2,
   then a ceiling probe and a floor probe while the box moves vertically,
   and the on-floor bookkeeping (unkB bit 0, the floor row unkC). */
void sub_0801ff84(void)
{
    u8 b;
    u16 t;
    s32 u;

    if (gUnk_03005598 != 0)
    {
        if (gUnk_03005598 > 0)
        {
            if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) == 0)
            {
                gUnk_03005530.unkB &= 0xFB;
                if (gUnk_087336F0[gUnk_03005578] != 0
                    && gUnk_08732CF0[gUnk_03005578] != 0 && (gUnk_08732CF0[gUnk_03005578] & 1) != 0)
                {
                    gUnk_03005530.unkB |= 4;
                    goto vertical;
                }
                if (gUnk_087336F0[gUnk_03005510] == 0)
                    goto vertical;
                if (gUnk_08732CF0[gUnk_03005510] == 0)
                    goto vertical;
                if ((gUnk_08732CF0[gUnk_03005510] & 1) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gUnk_03005510] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                    goto vertical;
                gUnk_03005530.unkB |= 4;
                goto vertical;
            }
            if (gUnk_087339F0[gUnk_03005578] == 1 && (gUnk_03005520 & 0xFFF0) != (gUnk_03005570 & 0xFFF0))
                goto vertical;
            if (gUnk_087336F0[gUnk_03005578] != 0)
            {
                if (gUnk_08732CF0[gUnk_03005578] == 0)
                    goto vertical;
                if ((gUnk_03005530.unkB & 4) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                    goto vertical;
                u = gUnk_087338F0[gUnk_03005578];
            }
            else
            {
                u = gUnk_03005578;
            }
            gUnk_03005530.unk0 = 1;
            gUnk_03005560 += sub_080219f0(u);
            if ((gUnk_08732CF0[gUnk_03005578] == 0 || (gUnk_08732CF0[gUnk_03005578] & 1) == 0)
                && sub_08021634(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) != 0)
            {
                    if (gUnk_087336F0[gUnk_03005578] != 0)
                {
                    if (gUnk_08732CF0[gUnk_03005578] == 0)
                        goto vertical;
                    if ((gUnk_03005530.unkB & 4) == 0)
                        goto vertical;
                    if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                        goto vertical;
                    u = gUnk_087338F0[gUnk_03005578];
                }
                else
                {
                    u = gUnk_03005578;
                }
                gUnk_03005560 += sub_080219f0(u);
            }
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        }
        else
        {
            if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) == 0)
            {
                gUnk_03005530.unkB &= 0xFD;
                if (gUnk_087336F0[gUnk_03005578] != 0
                    && gUnk_08732CF0[gUnk_03005578] != 0 && (gUnk_08732CF0[gUnk_03005578] & 1) == 0)
                {
                    gUnk_03005530.unkB |= 2;
                    goto vertical;
                }
                if (gUnk_087336F0[gUnk_03005594] == 0)
                    goto vertical;
                if (gUnk_08732CF0[gUnk_03005594] == 0)
                    goto vertical;
                if ((gUnk_08732CF0[gUnk_03005594] & 1) != 0)
                    goto vertical;
                if ((gUnk_08732DF0[gUnk_03005594] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                    goto vertical;
                gUnk_03005530.unkB |= 2;
                goto vertical;
            }
            if (gUnk_087339F0[gUnk_03005578] == 1 && (gUnk_03005520 & 0xFFF0) != (gUnk_03005570 & 0xFFF0))
                goto vertical;
            if (gUnk_087336F0[gUnk_03005578] != 0)
            {
                if (gUnk_08732CF0[gUnk_03005578] == 0)
                    goto vertical;
                if ((gUnk_03005530.unkB & 2) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                    goto vertical;
                u = gUnk_087338F0[gUnk_03005578];
            }
            else
            {
                u = gUnk_03005578;
            }
            gUnk_03005530.unk0 = 2;
            gUnk_03005560 += sub_080219d0(u);
            if ((gUnk_08732CF0[gUnk_03005578] != 0 || (gUnk_08732CF0[gUnk_03005578] & 1) == 0)
                && sub_08021634(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) != 0)
            {
                    if (gUnk_087336F0[gUnk_03005578] != 0)
                {
                    if (gUnk_08732CF0[gUnk_03005578] == 0)
                        goto vertical;
                    if ((gUnk_03005530.unkB & 2) == 0)
                        goto vertical;
                    if ((gUnk_08732DF0[gUnk_03005578] & 0xCF) == 0x83 && (gUnk_03005570 & 15) > 7)
                        goto vertical;
                    u = gUnk_087338F0[gUnk_03005578];
                }
                else
                {
                    u = gUnk_03005578;
                }
                gUnk_03005560 += sub_080219d0(u);
            }
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        }
    }
vertical:
    if (gUnk_03005514 != 0)
    {
        if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) != 0
            && gUnk_087336F0[gUnk_03005578] == 0)
        {
            gUnk_03005530.unk1 = 1;
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005570 += sub_08021990(gUnk_03005578);
            if ((gUnk_08732DF0[gUnk_03005578] & 0x40) == 0
                && sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) != 0)
            {
                gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
                gUnk_03005570 += sub_08021990(gUnk_03005578);
            }
            gUnk_03005530.unkB = 0;
        }
        if (sub_080216d8(gUnk_03005560, gUnk_03005584 + gUnk_03005570) == 0)
            goto floor;
        if (gUnk_087336F0[gUnk_03005578] != 0)
        {
            if ((gUnk_03005530.unkB & 1) == 0)
                goto off;
            if (gUnk_03005530.unkC > (gUnk_03005570 + gUnk_03005584) >> 4)
                goto off;
        }
        gUnk_03005530.unk2 = 1;
        gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
        gUnk_03005570 += sub_080219b0(gUnk_03005578);
        if ((gUnk_08732DF0[gUnk_03005578] & 0x80) == 0
            && sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        {
            t = gUnk_03005578;
            if (gUnk_087336F0[t] != 0)
            {
                if ((gUnk_03005530.unkB & 1) == 0)
                    goto done;
                if ((gUnk_03005570 + gUnk_03005584) >> 4 < gUnk_03005530.unkC)
                    goto done;
            }
            gUnk_03005530.unk4 = gUnk_08732CF0[t];
            gUnk_03005570 += sub_080219b0(t);
        }
    done:
        gUnk_03005530.unk6 = 1;
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
        return;
    }
off:
    gUnk_03005530.unkB &= 0xFE;
}

/* An empty function: nothing in the ROM calls it or points at it. */
void sub_08020698(void)
{
}
