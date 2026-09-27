#include "gba/gba.h"
#include "global.h"

/* Palette fade engine (0x080008E8-0x08000DE3, issue #32 batch A1).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Matching notes (docs/lessons-learned.md):
 *  - BeginFade needs the `if (0) return steps;` dead return: without it
 *    the u16 parameter is truncated in place (r0) instead of into the copy
 *    r5 the ROM keeps, and the function pushes one register less.
 *  - UpdateFade: the fade source/destination buffers 0x03001270 and
 *    0x03001A90 are SYMBOLS (gBgPalette / gFadedPalette), not address
 *    literals.  Written as literals, gcc CSEs the two mentions of each
 *    address into one pseudo and the whole entry/tail register assignment
 *    shifts by one; as extern arrays each mention is its own pool word,
 *    exactly as the ROM has it. */

extern vu16 gFadeTimer;
extern vu16 gFadeInterval;
extern vu16 gFadeSteps;
extern vs16 gBrightness;
extern vu16 gFadeStep;
extern u16 *gFadeKeepMask;
extern vu32 gPaletteSource;
extern vu16 gUnk_03000048;
extern vu16 gDispCnt;
extern u16 gBgPalette[];
extern u16 gFadedPalette[];

extern vs16 gVolumeRampMode;
extern vs16 gVolumeRampLevel;
extern vu16 gVolumeRampSpeed;
extern vu16 gSoundDisabled;
extern void (*gFrameCallback)(void);
extern vu16 gUnk_03001014;
extern u32 gUnk_03000B74;
extern vu16 gWaitingForVBlank;
extern vu16 gFrameInProgress;
extern vu16 gHeldKeys;
extern vu16 gPressedKeys;
extern vu16 gLinkPlayerCount;
extern vu16 gIntrMasterEnable;
extern vu32 gUnk_0200EBA0;
extern vu16 gFrameCount;
extern u16 gPlayTime[4];
extern void (*gFrameEndCallback)(void);

extern vu16 gSoundDriverOn;
extern u32 gUnk_03004D30;
extern void (*gUnk_03000FA4)(void);
extern vu16 gVBlankCount;
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
extern void StopAllSound(void);
extern void sub_08004734(void);
extern void LinkVSync(void);
extern void ReadKeys(void);
extern void FlushDisplayRegs(void);
extern void CopyOamAndPalette(void);
extern void ProcessCopyQueue(void);

void UpdateFade(void);

u32 BeginFade(u16 steps, u16 delta, u16 *mask)
{
    gFadeTimer = gFadeInterval = 1;
    gFadeSteps = steps;
    gBrightness += (gFadeStep = delta);
    gFadeKeepMask = mask;
    if (0)
        return steps;
}

void UpdateFade(void)
{
    u32 *src;
    u32 *dst;
    s32 rAbs, gAbs, bAbs;
    s32 i;
    u32 word;
    u16 v;
    s32 rv, gv, bv;
    u32 hv;

    gPaletteSource = (u32)gBgPalette;
    v = gFadeSteps;

    if (v != 0)
    {
        gFadeTimer--;
        if ((s16)gFadeTimer <= 0)
        {
            gBrightness += gFadeStep;
            gFadeSteps--;
            if ((s16)gFadeSteps > 0)
                gFadeTimer = gFadeInterval;
            else
                gFadeStep = gFadeTimer = gFadeInterval = gFadeSteps = 0;
        }
    }

    if (gBrightness > 31)
    {
        gBrightness = 31;
        gFadeStep = gFadeTimer = gFadeInterval = gFadeSteps = 0;
    }

    if (gBrightness < -31)
    {
        gBrightness = -31;
        gFadeStep = gFadeTimer = gFadeInterval = gFadeSteps = 0;
    }

    if (gBrightness != 0)
    {
        src = (u32 *)gBgPalette;
        dst = (u32 *)gFadedPalette;

        if (gBrightness < 0)
            rAbs = -gBrightness;
        else
            rAbs = gBrightness;

        if (gBrightness < 0)
            gAbs = -(gBrightness << 5);
        else
            gAbs = gBrightness << 5;

        if (gBrightness < 0)
            bAbs = -(gBrightness << 10);
        else
            bAbs = gBrightness << 10;

        if (gFadeKeepMask == 0)
        {
            if (gBrightness < 0)
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
            if (gBrightness < 0)
            {
                for (i = 0; i < 256; i++)
                {
                    word = *src++;
                    hv = word >> 16;
                    if (((gFadeKeepMask[i >> 3] >> ((i * 2 + 1) & 15)) & 1) == 0)
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
                    if (((gFadeKeepMask[i >> 3] >> ((i * 2) & 15)) & 1) == 0)
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
                    if (((gFadeKeepMask[i >> 3] >> ((i * 2 + 1) & 15)) & 1) == 0)
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
                    if (((gFadeKeepMask[i >> 3] >> ((i * 2) & 15)) & 1) == 0)
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

        gPaletteSource = (u32)gFadedPalette;

        if (gUnk_03000048 != 0)
        {
            if (gBrightness == 31)
                gDispCnt |= 0x80;
            else
                gDispCnt &= 0xFF7F;
        }
    }
}
