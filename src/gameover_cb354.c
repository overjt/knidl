#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "ending.h"

/* gameover_cb354.c (0x080CB354-0x080CB64B, issue #100).
 *
 * The game-over screen's object task types, all class 4.
 *   Task_GameOverSprite   #260, a still sprite picked by Task.unk18.
 *   Task_GameOverCursor   #261, the cursor, its frame following gGameOverCursor.
 *   Task_GameOverPalette   #262, a palette effect: fade the blend in, then cycle four
 *       colours between two rows of gUnk_08584BB0.
 *   Task_HalveScore   #263, halve this player's score (rounded down to a
 *       multiple of ten) and count the displayed score down to it.
 *   Task_GameOverObject   #264: six variants gGameOverObjectVariants[Task.variant]; variant 0
 *       (GameOverPlayer, the player character) is a small state machine of
 *       sub-states gGameOverPlayerStates[Task.state] and per-frame handlers
 *       gGameOverPlayerStateUpdates[Task.updateState] (GameOverPlayerUpdate), re-entered through
 *       GameOverPlayerEnterState. */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);                                     /* end the running task */

/* Task type #260 (class 4): a still sprite, frame and position picked by
   Task.unk18. */
void Task_GameOverSprite(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 9;
    gCurTask->frameTable = gGameOverScreenFrames;
    gCurTask->frame = gCurTask->gameOverSpriteIndex;
    gCurTask->posX = gUnk_08758274[gCurTask->gameOverSpriteIndex] << 16;
    gCurTask->posY = gUnk_08758284[gCurTask->gameOverSpriteIndex] << 16;
    TaskSleepForever();
}

/* Task type #261 (class 4): the cursor; its frame follows gGameOverCursor. */
void Task_GameOverCursor(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 8;
    gCurTask->frameTable = gGameOverScreenFrames;
    if (gMetaKnightmareMode == 0) {
        gCurTask->posX = 184 << 16;
        gCurTask->posY = 94 << 16;
    } else {
        gCurTask->posX = 184 << 16;
        gCurTask->posY = 96 << 16;
    }
    for (;;) {
        gCurTask->frame = gGameOverCursor + 8;
        TaskYieldTrampoline(1);
    }
}

/* Task type #262 (class 4): fade the blend in over 32 frames, then cycle
   four colours between two palettes of gUnk_08584BB0. */
void Task_GameOverPalette(void)
{
    gCurTask->gameOverPaletteFadeTimer = 0;
    gCurTask->gameOverPaletteFrom = 0;
    gCurTask->gameOverPaletteTo = 1;
    gCurTask->gameOverPaletteBlend = 0;
    gBldCntTarget1 = 66;
    gBldCntTarget2 = 12;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    for (;;) {
        if (gCurTask->gameOverPaletteFadeTimer < 32) {
            gCurTask->gameOverPaletteFadeTimer++;
            if (gCurTask->gameOverPaletteFadeTimer == 32) {
                gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
            } else {
                gBldAlphaEva = gCurTask->gameOverPaletteFadeTimer >> 1;
                gBldAlphaEvb = 16 - gBldAlphaEva;
            }
        }
        if (gCurTask->gameOverPaletteBlend == 256) {
            gCurTask->gameOverPaletteFrom ^= 1;
            gCurTask->gameOverPaletteTo ^= 1;
            gCurTask->gameOverPaletteBlend = 0;
        }
        gCurTask->gameOverPaletteBlend += 4;
        if (gCurTask->gameOverPaletteBlend > 256)
            gCurTask->gameOverPaletteBlend = 256;
        BlendColors(gUnk_08584BB0[gCurTask->gameOverPaletteFrom], gUnk_08584BB0[gCurTask->gameOverPaletteTo],
            (u16)gCurTask->gameOverPaletteBlend, 4, gBgPaletteBank9);
        TaskYieldTrampoline(1);
    }
}

/* Task type #263 (class 4): halve this player's score, round it down to a
   multiple of 10 and count the displayed score down to it. */
void Task_HalveScore(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    gCurTask->halveScoreShown = gPlayerScores[gLocalPlayer];
    gPlayerScores[gLocalPlayer] >>= 1;
    gCurTask->halveScoreRemainder = Mod(gPlayerScores[gLocalPlayer], 10);
    gPlayerScores[gLocalPlayer] -= gCurTask->halveScoreRemainder;
    while (gCurTask->halveScoreShown != gPlayerScores[gLocalPlayer]) {
        gCurTask->halveScoreShown -= 10;
        DrawScoreToBgMap(gCurTask->halveScoreShown, 22, 18);
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task type #264 (class 4): six variants, gGameOverObjectVariants[Task.variant]. */
void Task_GameOverObject(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->tileWord = 0x4800;
    CallTableEntry(gCurTask->variant, 6, gGameOverObjectVariants);
    TaskSleepForever();
}

/* Task type #264 variant 0: sub-states gGameOverPlayerStates[Task.state], per-frame
   handlers gGameOverPlayerStateUpdates[Task.updateState] (GameOverPlayerUpdate). */
void GameOverPlayer(void)
{
    gCurTask->updateCallback = (u32)GameOverPlayerUpdate;
    gCurTask->layer = 8;
    gCurTask->frameTable = gGameOverPlayerFrames;
    gCurTask->unk24 = 0;
    gCurTask->unk18 = 0;
    gCurTask->facing = 1;
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 3, gGameOverPlayerStates);
    TaskSleepForever();
}

/* Task type #264 variant 0's per-frame hook: handler gGameOverPlayerStateUpdates[Task.updateState]. */
void GameOverPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gGameOverPlayerStateUpdates);
}

/* Re-enter task type #264 variant 0 (TaskSetEntry installs this as its
   body): Task.unk24 = 1, then sub-state gGameOverPlayerStates[Task.state]. */
void GameOverPlayerEnterState(void)
{
    gCurTask->unk24 = 1;
    CallTableEntry(gCurTask->state, 3, gGameOverPlayerStates);
}
