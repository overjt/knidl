#include "gba/gba.h"
#include "global.h"
#include "link.h"
#include "sound.h"

/* VBlank interrupt handler (0x080010CC-0x080011AB, issue #32 batch A1).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Pointer-called entry: runs the sound driver's vsync/main, flushes the OAM
 * and palette shadows via CopyOamAndPalette, pumps the copy queue, and clears the
 * frame flag gWaitingForVBlank that EndFrame spins on. */

/* Not from main.h: this file's view of gVBlankCallback differs (lesson
   3.517). */
extern vu16 gFadeTimer;
extern vu16 gFadeInterval;
extern vu16 gFadeSteps;
extern vs16 gBrightness;
extern vs16 gFadeStep;
extern u16 *gFadeKeepMask;
extern vu32 gPaletteSource;
extern vu16 gFadeBlankAtWhite;
extern vu16 gDispCnt;
extern u16 gBgPalette[];
extern u16 gFadedPalette[];

extern vs16 gVolumeRampMode;
extern vs16 gVolumeRampLevel;
extern vu16 gVolumeRampSpeed;
extern vu16 gSoundDisabled;
extern void (*gFrameCallback)(void);
extern vu16 gUnk_03001014;
extern u32 gHBlankDmaState;
extern vu16 gWaitingForVBlank;
extern vu16 gFrameInProgress;
extern vu16 gHeldKeys;
extern vu16 gPressedKeys;
extern u16 gLinkPlayerCount;
extern vu16 gIntrMasterEnable;
extern u32 gLinkDriverMode;
extern vu16 gFrameCount;
extern u16 gPlayTime[4];
extern void (*gFrameEndCallback)(void);

extern vu16 gSoundDriverOn;
extern u32 gLinkPauseFrames;
extern void (*gVBlankCallback)(void);
extern vu16 gVBlankCount;
extern vu16 gUnk_03001008;
extern void (*gUnk_03000F90)(void);
extern void (*gBlockAnimHook)(void);
extern void (*gVBlankEndCallback)(void);

struct MusicPlayerInfo;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;
extern void m4aSoundVSync(void);
extern void SoftReset(u32 resetFlags);
extern void ReadKeys(void);
extern void FlushDisplayRegs(void);
extern void CopyOamAndPalette(void);
extern void ProcessCopyQueue(void);

void UpdateFade(void);

void VBlankIntr(void)
{
    if (gSoundDriverOn != 0)
        m4aSoundVSync();

    if (gLinkPlayerCount != 1)
    {
        if (gLinkPauseFrames != 0)
        {
            gLinkPauseFrames--;
            if (gLinkPauseFrames == 0)
                gLinkDriverMode = 1;
        }
    }

    if (gLinkDriverMode == 1)
        LinkVSync();

    if (gVBlankCallback != 0)
        gVBlankCallback();

    gVBlankCount++;

    if (gFrameInProgress != 0)
    {
        if (gUnk_03001008 != 0)
            ReadKeys();
    }
    else
    {
        ReadKeys();
        if (gUnk_03000F90 != 0)
            gUnk_03000F90();
        FlushDisplayRegs();
        CopyOamAndPalette();
        ProcessCopyQueue();
        if (gBlockAnimHook != 0)
            gBlockAnimHook();
    }

    if (gSoundDriverOn != 0)
        m4aSoundMain();

    if (gVBlankEndCallback != 0)
        gVBlankEndCallback();

    gWaitingForVBlank = 0;
}
