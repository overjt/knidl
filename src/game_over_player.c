#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "room.h"
#include "ending.h"

/* game_over_player.c (0x080CB64C-0x080CBED3, issue #100).
 *
 * The game-over screen, task type #264 variant 0: the player character.
 * Its sub-states gGameOverPlayerStates[Task.state] and per-frame handlers
 * gGameOverPlayerStateUpdates[Task.updateState] (GameOverPlayer starts it, GameOverPlayerEnterState re-enters
 * it with Task.unk24 = 1):
 *   GameOverPlayerWait / GameOverPlayerWaitUpdate   sub-state 0, the idle loop; once re-entered
 *       (Task.unk24) the handler counts Task.unk20 down and then ends the
 *       screen (gGameOverDone = 1) with game state 1.
 *   GameOverPlayerContinue / GameOverPlayerContinueUpdate   sub-state 1, the "continue" animation, which
 *       ends the screen with game state 5 (back into the game).
 *   GameOverPlayerGiveUp / GameOverPlayerGiveUpUpdate   sub-state 2, the "give up" animation (it
 *       spawns variants 3 and 4); its handler re-enters sub-state 0 with a
 *       120-frame count and spawns variant 2. */

s32 PlaySfx(s32 id);                                    /* play a sound effect */
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetEntry(void *a, u32 i);

/* Task type #264 variant 0, sub-state 0. */
void GameOverPlayerWait(void)
{
    gCurTask->frameTable = gGameOverPlayerFrames;
    gCurTask->updateState = 0;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 129 << 16;
    for (;;) {
        TaskSetFrame(0);
        TaskYieldTrampoline(16);
        gCurTask->frame++;
        TaskYieldTrampoline(16);
        gCurTask->frame++;
        TaskYieldTrampoline(20);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        if (gCurTask->gameOverPlayerLooped != 0)
            PlaySfx(180);
        gCurTask->gameOverPlayerLooped = 1;
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        TaskSetFrame(3);
        TaskYieldTrampoline(48);
    }
}

/* Task type #264 variant 0, handler 0. */
void GameOverPlayerWaitUpdate(void)
{
    if (gCurTask->gameOverPlayerReentered != 0) {
        if (gCurTask->gameOverPlayerEndTimer <= 0) {
            gGameOverDone = 1;
            gGameState = GAME_STATE_BOOT_LOGO;
        }
        gCurTask->gameOverPlayerEndTimer--;
    }
}

/* Task type #264 variant 0, sub-state 1. */
void GameOverPlayerContinue(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 9;
    TaskYieldTrampoline(2);
    gCurTask->frame = 12;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x27000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(2);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    gCurTask->frame = 11;
    TaskYieldTrampoline(2);
    gCurTask->frame = 6;
    TaskYieldTrampoline(4);
    gCurTask->frame = 5;
    TaskYieldTrampoline(26);
    gCurTask->frame = 7;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    gCurTask->frame = 5;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x14000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(19);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(11);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->frame = 8;
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->frame = 8;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->frame = 24;
    TaskSetMotion(-0x4000, 0, 0x5A5A5A5A, -0x18000, 0x3000, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->frame = 17;
    TaskSetMotion(0x8000, 0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskSetMotion(0x18000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame = 14;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gGameOverDone = 1;
    gGameState = GAME_STATE_HUB;
    TaskSleepForever();
}

void GameOverPlayerContinueUpdate(void)
{
}

/* Task type #264 variant 0, sub-state 2. */
void GameOverPlayerGiveUp(void)
{
    gCurTask->updateState = 2;
    gCurTask->frameTable = gUnk_087548B8;
    TaskStop();
    gCurTask->frame = 0;
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    TaskYieldTrampoline(12);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    TaskYieldTrampoline(12);
    gCurTask->velX = 0xC000;
    gCurTask->accelX = -0x4000;
    TaskYieldTrampoline(5);
    gCurTask->velX = -0xC000;
    gCurTask->accelX = 0x4000;
    TaskYieldTrampoline(5);
    gCurTask->velX = 0xC000;
    gCurTask->accelX = -0x4000;
    TaskYieldTrampoline(5);
    gCurTask->velX = -0xC000;
    gCurTask->accelX = 0x4000;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    gCurTask->velX = 0;
    gCurTask->accelX = 0;
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(30);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(20);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame = 7;
    gCurTask->velY = 0x14000;
    gCurTask->accelY = -0x4000;
    TaskYieldTrampoline(6);
    gCurTask->gameOverPlayerSfxPlayer = PlaySfx(103);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->gameOverPlayerLoopCount = 0;
    do {
        gCurTask->frame = 9;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->gameOverPlayerLoopCount++;
    } while ((s16)gCurTask->gameOverPlayerLoopCount <= 12);
    StopSfxOnPlayer(gCurTask->gameOverPlayerSfxPlayer, 0x67);
    PlaySfx(104);
    CreateGameOverObject(3);
    gCurTask->frame++;
    gCurTask->velX = -0x40000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x60000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x30000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    gCurTask->velX = 0;
    TaskYieldTrampoline(2);
    CreateGameOverObject(4);
    gCurTask->frame = 11;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 11;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    TaskSetMotion(0x38000, -0x10000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(20);
    PlaySfx(113);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->facing = -1;
    gCurTask->state = 0;
    TaskSleepForever();
}

/* Task type #264 variant 0, handler 2. */
void GameOverPlayerGiveUpUpdate(void)
{
    if (gCurTask->state != 2) {
        gCurTask->gameOverPlayerEndTimer = 120;
        CreateGameOverObject(2);
        TaskSetEntry(GameOverPlayerEnterState, gCurTaskIdx);
    }
}
