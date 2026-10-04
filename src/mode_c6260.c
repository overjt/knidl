#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "room.h"
#include "player.h"
#include "save.h"
#include "ending.h"

/* mode_c6260.c (0x080C6260-0x080C641F, issue #98).
 *
 * AgbMain state 11 (src/main.c calls EndingMain once and moves to
 * state 12, which runs M38's FinalResultsScreen).
 * 
 *   EndingMain   copies gLocalPlayer, gLinkIsMaster, gLinkPlayerCount and
 *       gPlayerCount into EWRAM cells, runs the SIO teardown DisconnectLink
 *       (after LinkRequestSync/LinkSyncClock when gPrevGameState is 20, the value
 *       AgbMain's state 20 leaves there), stops the sound (StopAllSound),
 *       plays the two scenes below unless gPrevGameState is 20 or
 *       gMetaKnightmareMode is 1, and ends with LoadSaveSlot(gCurSaveSlot).
 *   EndingEpilogueScene / EndingStarRodReturnScene   one scene each: preset and load a room
 *       (LoadEndingEpilogueRoom(0, 0) / LoadEndingStarRodReturnRoom(632, 248)), palette set 15 / 16
 *       (LoadBgLayout), DISPCNT BG bits 0x1D00 / 0x1C00, spawn the scene's
 *       task and run frames (RunLinkFrame) until it clears gEndingSceneActive,
 *       then tear the level down (sub_08027178).
 *   CreateEndingEpilogue / CreateEndingStarRodReturn   spawn M38's task type #100 / #101,
 *       retrying every frame until a slot is free, with Task.variant = 0. */

s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */

void EndingMain(void)
{
    gEndingLocalPlayer = gLocalPlayer;
    gEndingLinkIsMaster = gLinkIsMaster;
    gEndingLinkPlayerCount = gLinkPlayerCount;
    gEndingPlayerCount = gPlayerCount;
    if (gPrevGameState == GAME_STATE_BOSS_ENDURANCE) {
        LinkRequestSync();
        LinkSyncClock();
    }
    DisconnectLink();
    StopAllSound();
    if (gPrevGameState != GAME_STATE_BOSS_ENDURANCE && gMetaKnightmareMode != 1) {
        EndingEpilogueScene();
        EndingStarRodReturnScene();
    }
    StopAllSound();
    LoadSaveSlot(gCurSaveSlot);
}

void EndingEpilogueScene(void)
{
    gEndingSceneActive = 1;
    LoadEndingEpilogueRoom(0, 0);
    LoadBgLayout(15);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    CreateEndingEpilogue();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    while (gEndingSceneActive != 0)
        RunLinkFrame();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027178();
}

void CreateEndingEpilogue(void)
{
    s32 id;
    struct Task *t;

    while ((id = TaskCreateFrom(TASK_ENDING_EPILOGUE, 32)) == -1)
        RunLinkFrame();
    t = &gTasks[id];
    t->variant = 0;
}

void EndingStarRodReturnScene(void)
{
    gEndingSceneActive = 1;
    LoadEndingStarRodReturnRoom(632, 248);
    LoadBgLayout(16);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1C00;
    CreateEndingStarRodReturn();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    while (gEndingSceneActive != 0)
        RunLinkFrame();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027178();
}

void CreateEndingStarRodReturn(void)
{
    s32 id;
    struct Task *t;

    while ((id = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32)) == -1)
        RunLinkFrame();
    t = &gTasks[id];
    t->variant = 0;
}
