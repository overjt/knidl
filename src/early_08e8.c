#include "gba/gba.h"
#include "global.h"

/* Palette fade engine (0x080008E8-0x08000DE3, issue #32 batch A1).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Matching notes (docs/lessons-learned.md):
 *  - sub_080008e8 needs the `if (0) return steps;` dead return: without it
 *    the u16 parameter is truncated in place (r0) instead of into the copy
 *    r5 the ROM keeps, and the function pushes one register less.
 *  - sub_08000934: the fade source/destination buffers 0x03001270 and
 *    0x03001A90 are SYMBOLS (gUnk_03001270 / gUnk_03001A90), not address
 *    literals.  Written as literals, gcc CSEs the two mentions of each
 *    address into one pseudo and the whole entry/tail register assignment
 *    shifts by one; as extern arrays each mention is its own pool word,
 *    exactly as the ROM has it. */

extern vu16 gUnk_03001174;
extern vu16 gUnk_03001E98;
extern vu16 gUnk_03001E90;
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_030004A4;
extern u16 *gUnk_03001ED4;
extern vu32 gUnk_03000FB0;
extern vu16 gUnk_03000048;
extern vu16 gUnk_03001ED8;
extern u16 gUnk_03001270[];
extern u16 gUnk_03001A90[];

extern vs16 gUnk_03000B0C;
extern vs16 gUnk_03000FBC;
extern vu16 gUnk_03000FCC;
extern vu16 gUnk_03000AF8;
extern void (*gUnk_0300003C)(void);
extern vu16 gUnk_03001014;
extern u32 gUnk_03000B74;
extern vu16 gUnk_03001EC4;
extern vu16 gUnk_03000FD0;
extern vu16 gUnk_03001EF4;
extern vu16 gUnk_03000038;
extern vu16 gUnk_0300243C;
extern vu16 gUnk_03001EF8;
extern vu32 gUnk_0200EBA0;
extern vu16 gUnk_03001EA4;
extern u16 gUnk_03000498[4];
extern void (*gUnk_03000014)(void);

extern vu16 gUnk_03001EE4;
extern u32 gUnk_03004D30;
extern void (*gUnk_03000FA4)(void);
extern vu16 gUnk_03000FAC;
extern vu16 gUnk_03001008;
extern void (*gUnk_03000F90)(void);
extern void (*gUnk_030004A0)(void);
extern void (*gUnk_03000AF4)(void);

struct MusicPlayerInfo;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;
extern void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
extern void m4aSoundVSync(void);
extern void m4aSoundMain(void);
extern void SoundDriverVSyncOff(void);
extern void SoftReset(u32 resetFlags);
extern void sub_08003484(void);
extern void sub_08004734(void);
extern void sub_08006cd4(void);
extern void sub_080011dc(void);
extern void sub_08001280(void);
extern void sub_080011ac(void);
extern void sub_080013f8(void);

void sub_08000934(void);

u32 sub_080008e8(u16 steps, u16 delta, u16 *mask)
{
    gUnk_03001174 = gUnk_03001E98 = 1;
    gUnk_03001E90 = steps;
    gUnk_03000FB8 += (gUnk_030004A4 = delta);
    gUnk_03001ED4 = mask;
    if (0)
        return steps;
}

void sub_08000934(void)
{
    u32 *src;
    u32 *dst;
    s32 rAbs, gAbs, bAbs;
    s32 i;
    u32 word;
    u16 v;
    s32 rv, gv, bv;
    u32 hv;

    gUnk_03000FB0 = (u32)gUnk_03001270;
    v = gUnk_03001E90;

    if (v != 0)
    {
        gUnk_03001174--;
        if ((s16)gUnk_03001174 <= 0)
        {
            gUnk_03000FB8 += gUnk_030004A4;
            gUnk_03001E90--;
            if ((s16)gUnk_03001E90 > 0)
                gUnk_03001174 = gUnk_03001E98;
            else
                gUnk_030004A4 = gUnk_03001174 = gUnk_03001E98 = gUnk_03001E90 = 0;
        }
    }

    if (gUnk_03000FB8 > 31)
    {
        gUnk_03000FB8 = 31;
        gUnk_030004A4 = gUnk_03001174 = gUnk_03001E98 = gUnk_03001E90 = 0;
    }

    if (gUnk_03000FB8 < -31)
    {
        gUnk_03000FB8 = -31;
        gUnk_030004A4 = gUnk_03001174 = gUnk_03001E98 = gUnk_03001E90 = 0;
    }

    if (gUnk_03000FB8 != 0)
    {
        src = (u32 *)gUnk_03001270;
        dst = (u32 *)gUnk_03001A90;

        if (gUnk_03000FB8 < 0)
            rAbs = -gUnk_03000FB8;
        else
            rAbs = gUnk_03000FB8;

        if (gUnk_03000FB8 < 0)
            gAbs = -(gUnk_03000FB8 << 5);
        else
            gAbs = gUnk_03000FB8 << 5;

        if (gUnk_03000FB8 < 0)
            bAbs = -(gUnk_03000FB8 << 10);
        else
            bAbs = gUnk_03000FB8 << 10;

        if (gUnk_03001ED4 == 0)
        {
            if (gUnk_03000FB8 < 0)
            {
                for (i = 0; i < 256; i++)
                {
                    word = *src++;
                    hv = word >> 16;
                    rv = hv & 31;
                    gv = hv & 0x3E0;
                    bv = hv & 0x7C00;
                    rv -= rAbs;
                    if (rv < 0)
                        rv = 0;
                    gv -= gAbs;
                    if (gv <= 31)
                        gv = 0;
                    bv -= bAbs;
                    if (bv <= 0x3E0)
                        bv = 0;
                    hv = bv | gv | rv;
                    rv = word & 31;
                    gv = word & 0x3E0;
                    bv = word & 0x7C00;
                    rv -= rAbs;
                    if (rv < 0)
                        rv = 0;
                    gv -= gAbs;
                    if (gv <= 31)
                        gv = 0;
                    bv -= bAbs;
                    if (bv <= 0x3E0)
                        bv = 0;
                    *dst++ = (hv << 16) | bv | gv | rv;
                }
            }
            else
            {
                for (i = 0; i < 256; i++)
                {
                    word = *src++;
                    hv = word >> 16;
                    rv = hv & 31;
                    gv = hv & 0x3E0;
                    bv = hv & 0x7C00;
                    rv += rAbs;
                    if (rv > 31)
                        rv = 31;
                    gv += gAbs;
                    if (gv > 0x3E0)
                        gv = 0x3E0;
                    bv += bAbs;
                    if (bv > 0x7C00)
                        bv = 0x7C00;
                    hv = bv | gv | rv;
                    rv = word & 31;
                    gv = word & 0x3E0;
                    bv = word & 0x7C00;
                    rv += rAbs;
                    if (rv > 31)
                        rv = 31;
                    gv += gAbs;
                    if (gv > 0x3E0)
                        gv = 0x3E0;
                    bv += bAbs;
                    if (bv > 0x7C00)
                        bv = 0x7C00;
                    *dst++ = (hv << 16) | bv | gv | rv;
                }
            }
        }
        else
        {
            if (gUnk_03000FB8 < 0)
            {
                for (i = 0; i < 256; i++)
                {
                    word = *src++;
                    hv = word >> 16;
                    if (((gUnk_03001ED4[i >> 3] >> ((i * 2 + 1) & 15)) & 1) == 0)
                    {
                        rv = hv & 31;
                        gv = hv & 0x3E0;
                        bv = hv & 0x7C00;
                        rv -= rAbs;
                        if (rv < 0)
                            rv = 0;
                        gv -= gAbs;
                        if (gv <= 31)
                            gv = 0;
                        bv -= bAbs;
                        if (bv <= 0x3E0)
                            bv = 0;
                        hv = bv | gv | rv;
                    }
                    if (((gUnk_03001ED4[i >> 3] >> ((i * 2) & 15)) & 1) == 0)
                    {
                        rv = word & 31;
                        gv = word & 0x3E0;
                        bv = word & 0x7C00;
                        rv -= rAbs;
                        if (rv < 0)
                            rv = 0;
                        gv -= gAbs;
                        if (gv <= 31)
                            gv = 0;
                        bv -= bAbs;
                        if (bv <= 0x3E0)
                            bv = 0;
                        word = bv | gv | rv;
                    }
                    *dst++ = (hv << 16) | word;
                }
            }
            else
            {
                for (i = 0; i < 256; i++)
                {
                    word = *src++;
                    hv = word >> 16;
                    word &= 0xFFFF;
                    if (((gUnk_03001ED4[i >> 3] >> ((i * 2 + 1) & 15)) & 1) == 0)
                    {
                        rv = hv & 31;
                        gv = hv & 0x3E0;
                        bv = hv & 0x7C00;
                        rv += rAbs;
                        if (rv > 31)
                            rv = 31;
                        gv += gAbs;
                        if (gv > 0x3E0)
                            gv = 0x3E0;
                        bv += bAbs;
                        if (bv > 0x7C00)
                            bv = 0x7C00;
                        hv = bv | gv | rv;
                    }
                    if (((gUnk_03001ED4[i >> 3] >> ((i * 2) & 15)) & 1) == 0)
                    {
                        rv = word & 31;
                        gv = word & 0x3E0;
                        bv = word & 0x7C00;
                        rv += rAbs;
                        if (rv > 31)
                            rv = 31;
                        gv += gAbs;
                        if (gv > 0x3E0)
                            gv = 0x3E0;
                        bv += bAbs;
                        if (bv > 0x7C00)
                            bv = 0x7C00;
                        word = bv | gv | rv;
                    }
                    *dst++ = (hv << 16) | word;
                }
            }
        }

        gUnk_03000FB0 = (u32)gUnk_03001A90;

        if (gUnk_03000048 != 0)
        {
            if (gUnk_03000FB8 == 31)
                gUnk_03001ED8 |= 0x80;
            else
                gUnk_03001ED8 &= 0xFF7F;
        }
    }
}
