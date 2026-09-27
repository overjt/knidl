#include "gba/gba.h"
#include "global.h"

/* VBlank interrupt handler (0x080010CC-0x080011AB, issue #32 batch A1).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Pointer-called entry: runs the sound driver's vsync/main, flushes the OAM
 * and palette shadows via CopyOamAndPalette, pumps the copy queue, and clears the
 * frame flag gWaitingForVBlank that EndFrame spins on. */

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

void VBlankIntr(void)
{
    if (gSoundDriverOn != 0)
        m4aSoundVSync();

    if (gLinkPlayerCount != 1)
    {
        if (gUnk_03004D30 != 0)
        {
            gUnk_03004D30--;
            if (gUnk_03004D30 == 0)
                gUnk_0200EBA0 = 1;
        }
    }

    if (gUnk_0200EBA0 == 1)
        LinkVSync();

    if (gUnk_03000FA4 != 0)
        gUnk_03000FA4();

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
        if (gUnk_030004A0 != 0)
            gUnk_030004A0();
    }

    if (gSoundDriverOn != 0)
        m4aSoundMain();

    if (gUnk_03000AF4 != 0)
        gUnk_03000AF4();

    gWaitingForVBlank = 0;
}
