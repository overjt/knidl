#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b6474.c (0x080B6474-0x080B6A8F, issue #94).
 *
 * Three more HBlank wavy-scroll table drivers, entries of the table
 * gUnk_08756198 that src/save_b6a90.c runs once per frame.
 * sub_080b6474 is src/save_b6154.c's band effect with the unshifted lines
 * cleared and the BLDALPHA shadows left alone: rows of 8 lines alternately
 * 256 - 16 * frame and 256 + 16 * frame, 16 frames long.
 * sub_080b6570 and sub_080b67dc are a 24-frame pair on a two-halfword-per-
 * line table (BG1HOFS, BG1VOFS) with the blend fading: the bands (when
 * gUnk_02016860 is 4, resp. 6; otherwise the table is cleared), whose loop
 * over lines 136-151 also sets window 1 through sub_08010048 (its bottom edge
 * moves with the frame), and a split that opens lines 112-159 by the ramp
 * gUnk_087561C4.  sub_080b6570 plays the bands first and the split last,
 * sub_080b67dc the other way round.
 *
 * Matching notes: plain subscripts with the frame counter read inline at
 * every use (lesson 3.465); in the pair, one counter n for every flat loop
 * and for the band loop's outer index, and the zero of the calling loop in a
 * variable (3.471). */

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;
extern s16 gUnk_02016860;
extern vu8 gUnk_03000040;
extern vu8 gUnk_03000B08;
extern vs32 gUnk_03000FC0;
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03001EAC;
extern u8 gUnk_087561C4[];

void sub_08010048(s32 in, s32 out, s32 h, s32 v, s32 win);

s32 sub_080b6474(void)
{
    s32 i;
    s32 n;
    s32 n2;
    s32 vt;
    s32 z;
    u16 *base;
    u16 *p1;
    u16 *e;
    vs32 *pe;
    u32 *pc;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 16)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1F00;
    }
    else
    {
        pe = &gUnk_03001EE0;
        base = gUnk_020164A0;
        pc = &gUnk_0300101C;
        p1 = base;
        z = 0;
        e = base + 23;
        do
        {
            *e = z;
            e--;
        } while ((s32)e >= (s32)p1);
        for (n = 136; n < 152; n++)
            gUnk_020164A0[n] = 0;
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gUnk_020164A0[(i * 2 + 3) * 8 + n2] = 256 - (gUnk_02016494 << 4);
                gUnk_020164A0[(i * 2 + 4) * 8 + n2] = 256 + (gUnk_02016494 << 4);
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}

s32 sub_080b6570(void)
{
    s32 n;
    s32 n2;
    u16 z;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 24)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0;
        gUnk_03000FC0 = 0;
    }
    else
    {
        if (gUnk_02016494 <= 15)
        {
            if (gUnk_02016494 <= 13)
            {
                gUnk_0300118C = 66;
                gUnk_03000040 = 12;
                gUnk_03000B08 = 13 - gUnk_02016494;
                gUnk_03001EAC = 16 - gUnk_03000B08;
            }
            if (gUnk_02016860 == 4)
            {
                for (n = 0; n < 24; n++)
                {
                    gUnk_020164A0[n * 2] = -gUnk_02016494 << 4;
                    gUnk_020164A0[n * 2 + 1] = 0;
                }
                for (n = 0; n < 7; n++)
                {
                    for (n2 = 0; n2 < 8; n2++)
                    {
                        gUnk_020164A0[((n * 2 + 3) * 8 + n2) * 2] = -gUnk_02016494 << 4;
                        gUnk_020164A0[((n * 2 + 3) * 8 + n2) * 2 + 1] = 0;
                        gUnk_020164A0[((n * 2 + 4) * 8 + n2) * 2] = gUnk_02016494 << 4;
                        gUnk_020164A0[((n * 2 + 4) * 8 + n2) * 2 + 1] = 0;
                    }
                }
            }
            else
            {
                for (n = 0; n < 272; n++)
                    gUnk_020164A0[n] = 0;
            }
            z = 0;
            for (n = 0; n < 8; n++)
            {
                gUnk_020164A0[(136 + n) * 2] = z;
                gUnk_020164A0[(144 + n) * 2] = z;
                if (gUnk_02016494 <= 7)
                {
                    gUnk_020164A0[(136 + n) * 2 + 1] = 256 - gUnk_02016494;
                    gUnk_020164A0[(144 + n) * 2 + 1] = gUnk_02016494;
                    sub_08010048(60, 63, 255, gUnk_02016494 + 0x8788, 0x4000);
                }
                else
                {
                    gUnk_020164A0[(136 + n) * 2 + 1] = 248;
                    gUnk_020164A0[(144 + n) * 2 + 1] = 8;
                    sub_08010048(60, 63, 255, 0x878F, 0x4000);
                }
            }
        }
        else
        {
            gUnk_03001ED8 &= 0xBFFF;
            for (n = 0; n < 224; n++)
                gUnk_020164A0[n] = 0;
            for (n = 0; n < 24; n++)
            {
                gUnk_020164A0[(112 + n) * 2] = 0;
                gUnk_020164A0[(112 + n) * 2 + 1] = 256 - gUnk_087561C4[gUnk_02016494 - 16];
                gUnk_020164A0[(136 + n) * 2] = 0;
                gUnk_020164A0[(136 + n) * 2 + 1] = gUnk_087561C4[gUnk_02016494 - 16];
            }
        }
        gUnk_03001EE0 = gUnk_020164A0[0] << 16;
        gUnk_03000FC0 = gUnk_020164A0[1] << 16;
        gUnk_0300101C = 0x04000014;
        return 2;
    }
}

s32 sub_080b67dc(void)
{

    s32 n;
    s32 n2;
    u16 z;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 24)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0;
        gUnk_03000FC0 = 0;
        gUnk_03001ED8 &= 0xBFFF;
        gUnk_0300118C = 66;
        gUnk_03000040 = 12;
        gUnk_03000B08 = 13;
        gUnk_03001EAC = 3;
    }
    else
    {
        if (gUnk_02016494 <= 7)
        {
            for (n = 0; n < 224; n++)
                gUnk_020164A0[n] = 0;
            for (n = 0; n < 24; n++)
            {
                gUnk_020164A0[(112 + n) * 2] = 0;
                gUnk_020164A0[(112 + n) * 2 + 1] = 256 - gUnk_087561C4[7 - gUnk_02016494];
                gUnk_020164A0[(136 + n) * 2] = 0;
                gUnk_020164A0[(136 + n) * 2 + 1] = gUnk_087561C4[7 - gUnk_02016494];
            }
            if (gUnk_02016494 == 7)
            {
                gUnk_0300118C = 66;
                gUnk_03000040 = 12;
                gUnk_03000B08 = 0;
                gUnk_03001EAC = 16;
            }
        }
        else
        {
            if (gUnk_02016494 <= 21)
            {
                gUnk_03000B08 = gUnk_02016494 - 8;
                gUnk_03001EAC = 24 - gUnk_02016494;
            }
            if (gUnk_02016860 == 6)
            {
                for (n = 0; n < 24; n++)
                {
                    gUnk_020164A0[n * 2] = 256 - ((gUnk_02016494 - 8) << 4);
                    gUnk_020164A0[n * 2 + 1] = 0;
                }
                for (n = 0; n < 7; n++)
                {
                    for (n2 = 0; n2 < 8; n2++)
                    {
                        gUnk_020164A0[((n * 2 + 3) * 8 + n2) * 2] = 256 - ((gUnk_02016494 - 8) << 4);
                        gUnk_020164A0[((n * 2 + 3) * 8 + n2) * 2 + 1] = 0;
                        gUnk_020164A0[((n * 2 + 4) * 8 + n2) * 2] = 256 + ((gUnk_02016494 - 8) << 4);
                        gUnk_020164A0[((n * 2 + 4) * 8 + n2) * 2 + 1] = 0;
                    }
                }
            }
            else
            {
                for (n = 0; n < 272; n++)
                    gUnk_020164A0[n] = 0;
            }
            z = 0;
            for (n = 0; n < 8; n++)
            {
                gUnk_020164A0[(136 + n) * 2] = z;
                gUnk_020164A0[(144 + n) * 2] = z;
                if (gUnk_02016494 > 16)
                {
                    gUnk_020164A0[(136 + n) * 2 + 1] = gUnk_02016494 + 232;
                    gUnk_020164A0[(144 + n) * 2 + 1] = 24 - gUnk_02016494;
                    sub_08010048(60, 63, 255, 0x87A0 - gUnk_02016494, 0x4000);
                }
                else
                {
                    gUnk_020164A0[(136 + n) * 2 + 1] = 248;
                    gUnk_020164A0[(144 + n) * 2 + 1] = 8;
                    sub_08010048(60, 63, 255, 0x878F, 0x4000);
                }
            }
        }
        gUnk_03001EE0 = gUnk_020164A0[0] << 16;
        gUnk_03000FC0 = gUnk_020164A0[1] << 16;
        gUnk_0300101C = 0x04000014;
        return 2;
    }
}
