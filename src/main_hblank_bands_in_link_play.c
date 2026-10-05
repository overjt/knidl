#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "menu.h"
#include "save.h"

/* main_hblank_bands_in_link_play.c (0x080B6474-0x080B6A8F, issue #94).
 *
 * Three more HBlank wavy-scroll table drivers, entries of the table
 * gHBlankScrollEffects that src/main_room_hblank_scroll.c runs once per frame.
 * sub_080b6474 is src/main_hblank_bands_in_blend.c's band effect with the unshifted lines
 * cleared and the BLDALPHA shadows left alone: rows of 8 lines alternately
 * 256 - 16 * frame and 256 + 16 * frame, 16 frames long.
 * sub_080b6570 and sub_080b67dc are a 24-frame pair on a two-halfword-per-
 * line table (BG1HOFS, BG1VOFS) with the blend fading: the bands (when
 * gHBlankScrollEffect is 4, resp. 6; otherwise the table is cleared), whose loop
 * over lines 136-151 also sets window 1 through SetWindow (its bottom edge
 * moves with the frame), and a split that opens lines 112-159 by the ramp
 * gUnk_087561C4.  sub_080b6570 plays the bands first and the split last,
 * sub_080b67dc the other way round.
 *
 * Matching notes: plain subscripts with the frame counter read inline at
 * every use (lesson 3.465); in the pair, one counter n for every flat loop
 * and for the band loop's outer index, and the zero of the calling loop in a
 * variable (3.471). */

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

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 16)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
        gBg1ScrollX = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1F00;
    }
    else
    {
        pe = &gBg1ScrollX;
        base = gHBlankScrollTable;
        pc = &gHBlankDmaDest;
        p1 = base;
        z = 0;
        e = base + 23;
        do
        {
            *e = z;
            e--;
        } while ((s32)e >= (s32)p1);
        for (n = 136; n < 152; n++)
            gHBlankScrollTable[n] = 0;
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gHBlankScrollTable[(i * 2 + 3) * 8 + n2] = 256 - (gHBlankScrollTimer << 4);
                gHBlankScrollTable[(i * 2 + 4) * 8 + n2] = 256 + (gHBlankScrollTimer << 4);
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = REG_ADDR_BG1HOFS;
        return 1;
    }
}

s32 sub_080b6570(void)
{
    s32 n;
    s32 n2;
    u16 z;

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 24)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
        gBg1ScrollX = 0;
        gBg1ScrollY = 0;
    }
    else
    {
        if (gHBlankScrollTimer <= 15)
        {
            if (gHBlankScrollTimer <= 13)
            {
                gBldCntTarget1 = 66;
                gBldCntTarget2 = 12;
                gBldAlphaEva = 13 - gHBlankScrollTimer;
                gBldAlphaEvb = 16 - gBldAlphaEva;
            }
            if (gHBlankScrollEffect == 4)
            {
                for (n = 0; n < 24; n++)
                {
                    gHBlankScrollTable[n * 2] = -gHBlankScrollTimer << 4;
                    gHBlankScrollTable[n * 2 + 1] = 0;
                }
                for (n = 0; n < 7; n++)
                {
                    for (n2 = 0; n2 < 8; n2++)
                    {
                        gHBlankScrollTable[((n * 2 + 3) * 8 + n2) * 2] = -gHBlankScrollTimer << 4;
                        gHBlankScrollTable[((n * 2 + 3) * 8 + n2) * 2 + 1] = 0;
                        gHBlankScrollTable[((n * 2 + 4) * 8 + n2) * 2] = gHBlankScrollTimer << 4;
                        gHBlankScrollTable[((n * 2 + 4) * 8 + n2) * 2 + 1] = 0;
                    }
                }
            }
            else
            {
                for (n = 0; n < 272; n++)
                    gHBlankScrollTable[n] = 0;
            }
            z = 0;
            for (n = 0; n < 8; n++)
            {
                gHBlankScrollTable[(136 + n) * 2] = z;
                gHBlankScrollTable[(144 + n) * 2] = z;
                if (gHBlankScrollTimer <= 7)
                {
                    gHBlankScrollTable[(136 + n) * 2 + 1] = 256 - gHBlankScrollTimer;
                    gHBlankScrollTable[(144 + n) * 2 + 1] = gHBlankScrollTimer;
                    SetWindow(60, 63, 255, gHBlankScrollTimer + 0x8788, 0x4000);
                }
                else
                {
                    gHBlankScrollTable[(136 + n) * 2 + 1] = 248;
                    gHBlankScrollTable[(144 + n) * 2 + 1] = 8;
                    SetWindow(60, 63, 255, 0x878F, 0x4000);
                }
            }
        }
        else
        {
            gDispCnt &= 0xBFFF;
            for (n = 0; n < 224; n++)
                gHBlankScrollTable[n] = 0;
            for (n = 0; n < 24; n++)
            {
                gHBlankScrollTable[(112 + n) * 2] = 0;
                gHBlankScrollTable[(112 + n) * 2 + 1] = 256 - gUnk_087561C4[gHBlankScrollTimer - 16];
                gHBlankScrollTable[(136 + n) * 2] = 0;
                gHBlankScrollTable[(136 + n) * 2 + 1] = gUnk_087561C4[gHBlankScrollTimer - 16];
            }
        }
        gBg1ScrollX = gHBlankScrollTable[0] << 16;
        gBg1ScrollY = gHBlankScrollTable[1] << 16;
        gHBlankDmaDest = REG_ADDR_BG1HOFS;
        return 2;
    }
}

s32 sub_080b67dc(void)
{

    s32 n;
    s32 n2;
    u16 z;

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 24)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
        gBg1ScrollX = 0;
        gBg1ScrollY = 0;
        gDispCnt &= 0xBFFF;
        gBldCntTarget1 = 66;
        gBldCntTarget2 = 12;
        gBldAlphaEva = 13;
        gBldAlphaEvb = 3;
    }
    else
    {
        if (gHBlankScrollTimer <= 7)
        {
            for (n = 0; n < 224; n++)
                gHBlankScrollTable[n] = 0;
            for (n = 0; n < 24; n++)
            {
                gHBlankScrollTable[(112 + n) * 2] = 0;
                gHBlankScrollTable[(112 + n) * 2 + 1] = 256 - gUnk_087561C4[7 - gHBlankScrollTimer];
                gHBlankScrollTable[(136 + n) * 2] = 0;
                gHBlankScrollTable[(136 + n) * 2 + 1] = gUnk_087561C4[7 - gHBlankScrollTimer];
            }
            if (gHBlankScrollTimer == 7)
            {
                gBldCntTarget1 = 66;
                gBldCntTarget2 = 12;
                gBldAlphaEva = 0;
                gBldAlphaEvb = 16;
            }
        }
        else
        {
            if (gHBlankScrollTimer <= 21)
            {
                gBldAlphaEva = gHBlankScrollTimer - 8;
                gBldAlphaEvb = 24 - gHBlankScrollTimer;
            }
            if (gHBlankScrollEffect == 6)
            {
                for (n = 0; n < 24; n++)
                {
                    gHBlankScrollTable[n * 2] = 256 - ((gHBlankScrollTimer - 8) << 4);
                    gHBlankScrollTable[n * 2 + 1] = 0;
                }
                for (n = 0; n < 7; n++)
                {
                    for (n2 = 0; n2 < 8; n2++)
                    {
                        gHBlankScrollTable[((n * 2 + 3) * 8 + n2) * 2] = 256 - ((gHBlankScrollTimer - 8) << 4);
                        gHBlankScrollTable[((n * 2 + 3) * 8 + n2) * 2 + 1] = 0;
                        gHBlankScrollTable[((n * 2 + 4) * 8 + n2) * 2] = 256 + ((gHBlankScrollTimer - 8) << 4);
                        gHBlankScrollTable[((n * 2 + 4) * 8 + n2) * 2 + 1] = 0;
                    }
                }
            }
            else
            {
                for (n = 0; n < 272; n++)
                    gHBlankScrollTable[n] = 0;
            }
            z = 0;
            for (n = 0; n < 8; n++)
            {
                gHBlankScrollTable[(136 + n) * 2] = z;
                gHBlankScrollTable[(144 + n) * 2] = z;
                if (gHBlankScrollTimer > 16)
                {
                    gHBlankScrollTable[(136 + n) * 2 + 1] = gHBlankScrollTimer + 232;
                    gHBlankScrollTable[(144 + n) * 2 + 1] = 24 - gHBlankScrollTimer;
                    SetWindow(60, 63, 255, 0x87A0 - gHBlankScrollTimer, 0x4000);
                }
                else
                {
                    gHBlankScrollTable[(136 + n) * 2 + 1] = 248;
                    gHBlankScrollTable[(144 + n) * 2 + 1] = 8;
                    SetWindow(60, 63, 255, 0x878F, 0x4000);
                }
            }
        }
        gBg1ScrollX = gHBlankScrollTable[0] << 16;
        gBg1ScrollY = gHBlankScrollTable[1] << 16;
        gHBlankDmaDest = REG_ADDR_BG1HOFS;
        return 2;
    }
}
