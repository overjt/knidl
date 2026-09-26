#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1c690.c (0x0801C690-0x0801C8DB, issue #84).
 *
 * The wall probes src/terrain_1bcac.c's entry points run for a box
 * standing on the ground (gUnk_03005530.unk6 != 0): sub_0801c690 (right
 * edge) and sub_0801c7cc (left edge) push the probe x out of a wall cell
 * (gUnk_08732EF0 attribute 1) at the box's top, else its middle corner.
 * 
 * Matching note: cse records `attribute == 1` after the test and then swaps
 * the operands of a later AND with the attribute, so the ROM's source ANDs
 * the constant (`!(gUnk_08732CF0[t] & 1)`, two table reads) or compares a
 * cast (`(s8)a == 1`), and stores the constant `1`/`2` (cse substitutes the
 * attribute register). */

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
extern u16 gUnk_03005510;
extern s16 gUnk_0300551C;           /* box left offset */
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_0300557C;           /* box top offset */
extern u16 gUnk_03005594;           /* cell to the left: tile set */
extern s16 gUnk_0300559C;           /* box right offset */

s32 sub_080217dc(u32 x, u32 y);
s32 sub_080219d0(u16 a);
s32 sub_080219f0(u16 a);

/* Right wall probe of a box standing on the ground (gUnk_03005530.unk6 !=
   0), the mirror image of sub_0801c7cc: step the probe x out of a wall
   cell at the box's top-right, else its middle-right corner. */
void sub_0801c690(void)
{
    s32 a;
    s32 d;
    s32 a2;
    s8 b;
    s32 d2;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) != 0)
    {
        if ((a = gUnk_08732EF0[gUnk_03005578]) == 1 && gUnk_087336F0[gUnk_03005578] == 0
            && (gUnk_08732EF0[gUnk_03005594] == 0
                || (gUnk_08732CF0[gUnk_03005594] != 0 && !(gUnk_08732CF0[gUnk_03005594] & 1))))
        {
            d = sub_080219f0(gUnk_03005578);
            if (d != 0)
            {
                gUnk_03005560 += d;
                gUnk_03005530.unk0 = 1;
                return;
            }
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_0300557C) != 0)
    {
        if ((a2 = gUnk_08732EF0[gUnk_03005578]) == 1 && gUnk_087336F0[gUnk_03005578] == 0)
        {
            b = gUnk_08732EF0[gUnk_03005594];
            if (b == 0 || (gUnk_087336F0[gUnk_03005594] != 0 && b == 1))
            {
                d2 = sub_080219f0(gUnk_03005578);
                if (d2 != 0)
                {
                    gUnk_03005560 += d2;
                    gUnk_03005530.unk0 = 1;
                }
            }
        }
    }
}

/* Left wall probe of a box standing on the ground (gUnk_03005530.unk6 !=
   0), the mirror image of sub_0801c690: step the probe x out of a wall
   cell at the box's top-left, else its middle-left corner. */
void sub_0801c7cc(void)
{
    s32 a;
    u8 m;
    s32 d;
    s8 a2;
    s8 b;
    s32 d2;

    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) != 0)
    {
        a = gUnk_08732EF0[gUnk_03005578];
        if ((s8)a == 1 && gUnk_087336F0[gUnk_03005578] == 0
            && (gUnk_08732EF0[gUnk_03005510] == 0
                || ((m = gUnk_08732CF0[gUnk_03005510]) != 0 && (a & m) != 0)))
        {
            d = sub_080219d0(gUnk_03005578);
            if (d != 0)
            {
                gUnk_03005560 += d;
                gUnk_03005530.unk0 = 2;
                return;
            }
        }
    }
    if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_0300557C) != 0)
    {
        a2 = gUnk_08732EF0[gUnk_03005578];
        if (a2 == 1 && gUnk_087336F0[gUnk_03005578] == 0)
        {
            b = gUnk_08732EF0[gUnk_03005510];
            if (b == 0 || (gUnk_087336F0[gUnk_03005510] != 0 && b == 1))
            {
                d2 = sub_080219d0(gUnk_03005578);
                if (d2 != 0)
                {
                    gUnk_03005560 += d2;
                    gUnk_03005530.unk0 = 2;
                }
            }
        }
    }
}
