#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_2136c.c (0x0802136C-0x080214DF, issue #84).
 *
 * The room probe of the non-player entry points (src/terrain_1bcac.c,
 * terrain_1c30c.c): src/terrain_21130.c's sub_08021130 without its tile-set
 * special case in front and its gUnk_08733AF0 copy behind. */

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

extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;           /* probe x */
extern s16 gUnk_03005570;           /* probe y */
extern u16 gUnk_03005578;           /* queried cell: tile set */
extern s16 gUnk_0300557C;           /* box top offset */
extern s16 gUnk_03005584;           /* box bottom offset */
extern s16 gUnk_0300561C;           /* map height in cells */

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);

/* Room probe: sub_08021130 without its tile-set special case in front
   and the gUnk_08733AF0 copy behind.  Classifies the cells at the probe
   point (bit 7 of the tile set) into the flags gUnk_03005530.unk7 and the
   cell boundary gUnk_03005530.unk8 (0xFFFF when there is none).  prev2 is
   u32 here: the u8 copy of sub_08021130 lets the 0x80 mask register win
   r8 over &gUnk_03005570 (global-alloc priority 0.1333 vs 0.1324). */
void sub_0802136c(void)
{
    u8 prev;
    u32 prev2;
    u32 zero;
    s32 y;
    s32 h;
    u16 f;
    u8 v;

    sub_080216d8(gUnk_03005560, gUnk_03005570);
    prev = gUnk_03005530.unk7;
    prev2 = prev;
    gUnk_03005530.unk8 = 0xFFFF;
    gUnk_03005530.unk7 = 0;
    zero = 0;
    y = gUnk_03005570;
    h = gUnk_0300561C << 4;
    if (y >= h)
    {
        sub_08021634(gUnk_03005560, h - 16);
        if (gUnk_03005578 & 0x80)
            gUnk_03005530.unk7 = 11;
    }
    else
    {
        f = gUnk_03005578 & 0x80;
        if (f)
        {
            gUnk_03005530.unk7 = 1;
            sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C);
            if (gUnk_03005578 & 0x80)
            {
                v = gUnk_03005530.unk7 | 2 | zero;
                gUnk_03005530.unk7 = v;
                if ((prev & 2) == 0)
                {
                    gUnk_03005530.unk7 = v | 0x80;
                    gUnk_03005530.unk8 = (gUnk_03005570 + gUnk_0300557C) & 0xFFF0;
                }
                else
                {
                    gUnk_03005530.unk7 = v | 8;
                }
            }
            else
            {
                gUnk_03005530.unk7 |= 0x48;
                gUnk_03005530.unk8 = ((gUnk_03005570 + gUnk_0300557C) & 0xFFF0) + 16;
            }
        }
        else
        {
            gUnk_03005530.unk7 = f;
            sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
            if (gUnk_03005578 & 0x80)
            {
                gUnk_03005530.unk7 |= 0x48;
                gUnk_03005530.unk8 = (gUnk_03005570 + gUnk_03005584) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gUnk_03005530.unk7 |= 0x80;
                gUnk_03005530.unk8 = ((gUnk_03005570 + gUnk_03005584) & 0xFFF0) + 16;
            }
        }
    }
}
