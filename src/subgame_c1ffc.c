#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "subgame.h"

/* subgame_c1ffc.c (0x080C1FFC-0x080C243B, issue #98).
 *
 * Sub-game 2 (gUnk_02007FCC == 2), the race screen: phase 0 of M35's
 * framework (M36's AirGrindMain dispatches gSubGamePhase through
 * gAirGrindPhases; entry 0 is AirGrindRace).
 * 
 *   CreateAirGrindRacers / CreateAirGrindScenery / CreateAirGrindEffect   spawn task type #96:
 *       variant 0 once per player (Task.unk1C = the player), variant 1 (its
 *       task index kept in M37Game.unk44C) and variant 2 (Task.unk18/unk1C/
 *       unk20 from the caller).
 *   AirGrindSetupRace   the state set-up: gAirGrindPtr = &gAirGrind,
 *       gAirGrindCoursePtr = &gAirGrindCourse, the linked-player count and mode
 *       cells, the four players' course records, the per-frame hook
 *       AirGrindBuildSky (gFrameCallback, called by the frame driver EndFrame)
 *       and the VBlank hook AirGrindSkyVBlankCallback (gVBlankCallback, called by the VBlank
 *       handler), and two 16-colour rows of gUnk_08609E40.
 *   AirGrindRace   the screen's task body: waits for the scroll position
 *       gAirGrindCoursePtr->unk000 to reach the two course lines unk00C and unk010
 *       (a sign sprite and songs 0x82B/0x82A/0x82C at each), then for all
 *       four players to pass unk010, and ends the screen (Task.unk18 = 2).
 *   AirGrindRaceUpdate   its per-frame callback: the frame counter gAirGrindFrame,
 *       the script cursor, the palette fades and M35's SubGameCheckEnd. */

/* Not from main.h: this file's view of gFrameCallback differs (lesson 3.517). */
extern u16 gLinkPlayerCount;
extern u32 gFrameCallback;
extern u32 gVBlankCallback;
extern u32 gVBlankEndCallback;

void ClearHBlankIntr(void);
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskSleepForever(void);                                     /* end the running task */
void AirGrindStepPaletteFades(void);                                  /* step the four palette fades gAirGrindPaletteFades[] */
void AirGrindStepScript(void);                                  /* step the gAirGrindScript script */

void CreateAirGrindRacers(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i < 4; i++) {
        id = TaskCreateFrom(96, 0);
        if (id != -1) {
            t = &gTasks[id];
            t->unk18 = id;
            t->unk1C = i;
            t->variant = 0;
        }
    }
}

void CreateAirGrindScenery(s32 unused)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(96, 32);
    if (id != -1) {
        t = &gTasks[id];
        t->variant = 1;
        gAirGrindPtr->unk44C = id;
    }
}

void CreateAirGrindEffect(s32 a, s32 b, s32 c)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(96, 32);
    if (id != -1) {
        t = &gTasks[id];
        t->unk18 = b;
        t->unk1C = a;
        t->variant = 2;
        t->unk20 = c;
    }
}

void AirGrindSetupRace(void)
{
    s32 i;

    gAirGrindPtr = &gAirGrind;
    gAirGrindCoursePtr = &gAirGrindCourse;
    gAirGrindPtr->localPlayer = gLocalPlayer;
    gAirGrindPtr->playerCount = gLinkPlayerCount;
    gAirGrindFrame = 0;
    StopBgm();
    StopAllSfx();
    AirGrindSeedRandom();
    AirGrindClearScript();
    for (i = 0; i < 4; i++) {
        gAirGrindCoursePtr->players[i].coursePos = gAirGrindCoursePtr->scrollPos;
        gAirGrindCoursePtr->players[i].unk04 = 0;
    }
    gAirGrindPtr->unk014 = -1;
    gAirGrindPtr->frameCount = 0;
    gFrameCallback = (u32)AirGrindBuildSky;
    gVBlankCallback = (u32)AirGrindSkyVBlankCallback;
    AirGrindStopAllPaletteFades();
    for (i = 0; i < 16; i++) {
        gAirGrindPtr->unk0EC.unk76[i] = gUnk_08609E40[32 + i];
        gAirGrindPtr->unk0EC.unk96[i] = gUnk_08609E40[64 + i];
    }
}

void AirGrindRace(void)
{
    s32 n;
    s32 i;

    AirGrindSetupRace();
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->frameTable = gUnk_08755FEC;
    gCurTask->frame = 0xFFFF;
    PlayBgm(0x82B);
    CreateAirGrindRacers();
    CreateAirGrindScenery(0);
    gCurTask->updateCallback = (u32)AirGrindRaceUpdate;
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->unk00C - 240)
        TaskYieldTrampoline(1);
    AirGrindShowCourseSign(gAirGrindCoursePtr->unk00C);
    while (1) {
        if (gAirGrindCoursePtr->scrollPos >= gAirGrindCoursePtr->unk00C)
            break;
        TaskYieldTrampoline(1);
    }
    gCurTask->pixelX = 144;
    gCurTask->pixelY = 80;
    gCurTask->airGrindSignFadeSlot = AirGrindStartPaletteFade(gUnk_0860A042, 241, 10, 8, 15, 0);
    gCurTask->frame = 0;
    PlayBgm(0x82A);
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->unk00C + 240)
        TaskYieldTrampoline(1);
    gCurTask->lateUpdateCallback = 0;
    gCurTask->frame = 0xFFFF;
    AirGrindStopPaletteFade(gCurTask->airGrindSignFadeSlot);
    if (gFrameCallback != 0 && gAirGrind.level != 2) {
        while (gAirGrindPtr->frameCount <= 0x4AF)
            TaskYieldTrampoline(1);
        AirGrindStartPaletteFade(&gAirGrindPtr->unk0EC.unk76[1], 161, 256, 2, 6, 1);
    }
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->finishLine - 240)
        TaskYieldTrampoline(1);
    AirGrindShowCourseSign(gAirGrindCoursePtr->finishLine);
    while (1) {
        if (gAirGrindCoursePtr->scrollPos >= gAirGrindCoursePtr->finishLine)
            break;
        TaskYieldTrampoline(1);
    }
    while (gAirGrindCoursePtr->players[0].coursePos < gAirGrindCoursePtr->finishLine)
        TaskYieldTrampoline(1);
    gCurTask->pixelX = 112;
    gCurTask->pixelY = 80;
    gCurTask->airGrindSignFadeSlot = AirGrindStartPaletteFade(gUnk_0860A042, 241, 10, 8, 15, 0);
    gCurTask->frame = 1;
    PlayBgm(0x82C);
    while (1) {
        n = 0;
        for (i = 0; i < 4; i++)
            if (gAirGrindCoursePtr->players[i].coursePos > gAirGrindCoursePtr->finishLine)
                n++;
        if (n > 3)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(180);
    StopBgm();
    StopAllSfx();
    ClearHBlankIntr();
    gVBlankEndCallback = 0;
    gCurTask->subGameNextPhase = 2;
    TaskSleepForever();
}

void AirGrindRaceUpdate(void)
{
    gAirGrindFrame++;
    AirGrindStepScript();
    AirGrindStepPaletteFades();
    SubGameCheckEnd();
}
