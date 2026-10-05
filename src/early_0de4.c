#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "link.h"
#include "sound.h"

/* Main per-frame driver / VBlank waiter (0x08000DE4-0x080010CB, issue #32).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Runs the palette fade step (UpdateFade), advances the BGM/SE volume ramp
 * state machine (gVolumeRampMode mode, gVolumeRampLevel volume 0..256,
 * gVolumeRampSpeed delta), calls the per-frame hook gFrameCallback, spins on the
 * VBlank flag gWaitingForVBlank (cleared by the handler in src/early_10cc.c),
 * handles the A+B+Start+Select soft-reset combo, ticks the play-time clock
 * gPlayTime[] (frames/seconds/minutes/hours, 59 rollovers, hour cap 998)
 * and finally calls the post-frame hook gFrameEndCallback.
 *
 * Matching note: the clock counters are pre-incremented in their tests
 * (`if (++gPlayTime[1] > 59)`): the HImode increment's zero-extension
 * is what gives the ROM's `ldr r5, =0xFFFF; adds r2, r5, #0` mask and the
 * `ands r0, r2` truncations.
 */

struct MusicPlayerInfo;
extern void m4aSoundVSync(void);
extern void SoftReset(u32 resetFlags);

void EndFrame(void)
{
    s32 i;
    u16 keys;
    UpdateFade();

    switch (gVolumeRampMode)
    {
    case 0:
        break;
    case 1:
        gVolumeRampLevel += gVolumeRampSpeed;
        if (gVolumeRampLevel > 255)
        {
            gVolumeRampLevel = 256;
            gVolumeRampSpeed = 0;
            gVolumeRampMode = 0;
        }
        if (gSoundDisabled == 0)
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFF, gVolumeRampLevel);
        break;
    case 2:
        gVolumeRampLevel += gVolumeRampSpeed;
        if (gVolumeRampLevel <= 0)
        {
            gVolumeRampLevel = 0;
            gVolumeRampSpeed = 0;
            gVolumeRampMode = 0;
            if (gSoundDisabled == 0)
                StopAllSound();
        }
        if (gSoundDisabled == 0)
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFF, gVolumeRampLevel);
        break;
    case 3:
        gVolumeRampLevel += gVolumeRampSpeed;
        if (gVolumeRampLevel > 255)
        {
            gVolumeRampLevel = 256;
            gVolumeRampSpeed = 0;
            gVolumeRampMode = 0;
        }
        if (gSoundDisabled == 0)
        {
            m4aMPlayVolumeControl(&gMPlayInfo_SE1, 0xFF, gVolumeRampLevel);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFF, gVolumeRampLevel);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, 0xFF, gVolumeRampLevel);
        }
        break;
    case 4:
        gVolumeRampLevel += gVolumeRampSpeed;
        if (gVolumeRampLevel <= 0)
        {
            gVolumeRampLevel = 0;
            gVolumeRampSpeed = 0;
            gVolumeRampMode = 0;
        }
        if (gSoundDisabled == 0)
        {
            m4aMPlayVolumeControl(&gMPlayInfo_SE1, 0xFF, gVolumeRampLevel);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFF, gVolumeRampLevel);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, 0xFF, gVolumeRampLevel);
        }
        break;
    }

    if (gFrameCallback != 0)
    {
        gUnk_03001014 = 0;
        gFrameCallback();
        gHBlankDmaState |= 1;
    }

    gWaitingForVBlank = 1;
    gFrameInProgress = 0;

    if (REG_IME & 1)
    {
        while (gWaitingForVBlank != 0)
            ;
    }

    keys = gHeldKeys & 15;
    if (keys == 15 && gFadeSteps == 0 && (keys & gPressedKeys) != 0 && gLinkPlayerCount == 1)
    {
        gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
        SoundDriverVSyncOff();
        m4aSoundVSync();
        for (i = 0x4000; i != 0; i--)
            ;
        SoftReset(0x1C);
    }

    gFrameInProgress = 1;

    if (gLinkDriverMode == 2)
        LinkBlockMain();

    gFrameCount++;

    if (gPlayTime[3] <= 998)
    {
        if (++gPlayTime[0] > 59)
        {
            gPlayTime[0] = 0;
            if (++gPlayTime[1] > 59)
            {
                gPlayTime[1] = 0;
                if (++gPlayTime[2] > 59)
                {
                    gPlayTime[2] = 0;
                    gPlayTime[3]++;
                }
            }
        }
    }

    if (gFrameEndCallback != 0)
        gFrameEndCallback();
}
