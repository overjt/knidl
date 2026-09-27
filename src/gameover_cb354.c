#include "gba/gba.h"
#include "global.h"
#include "task.h"

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
 *       sub-states gUnk_087582AC[Task.state] and per-frame handlers
 *       gUnk_087582B8[Task.updateState] (GameOverPlayerUpdate), re-entered through
 *       GameOverPlayerEnterState. */

extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gLocalPlayer;           /* this player's index */
extern s32 gPlayerScores[];         /* score per player */
extern s8 gGameOverCursor;            /* game-over screen: cursor (continue = 0?) */
extern u32 gUnk_087556E0[];
extern u16 gUnk_08758274[];
extern u16 gUnk_08758284[];
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;
extern u16 gUnk_08584BB0[][4];
extern u16 gUnk_03001390[];
extern void (*gGameOverObjectVariants[])(void);
extern u32 gUnk_08754914[];
extern void (*gUnk_087582AC[])(void);
extern void (*gUnk_087582B8[])(void);

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);                                     /* end the running task */
void DrawScoreToBgMap(s32 v, s32 x, s32 y);
void GameOverPlayerUpdate(void);

/* Task type #260 (class 4): a still sprite, frame and position picked by
   Task.unk18. */
void Task_GameOverSprite(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_087556E0;
    gCurTask->frame = gCurTask->unk18;
    gCurTask->posX = gUnk_08758274[gCurTask->unk18] << 16;
    gCurTask->posY = gUnk_08758284[gCurTask->unk18] << 16;
    TaskSleepForever();
}

/* Task type #261 (class 4): the cursor; its frame follows gGameOverCursor. */
void Task_GameOverCursor(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_087556E0;
    if (gUnk_03001F30 == 0) {
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
    gCurTask->unk28 = 0;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gBldCntTarget1 = 66;
    gBldCntTarget2 = 12;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    for (;;) {
        if (gCurTask->unk28 < 32) {
            gCurTask->unk28++;
            if (gCurTask->unk28 == 32) {
                gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
            } else {
                gBldAlphaEva = gCurTask->unk28 >> 1;
                gBldAlphaEvb = 16 - gBldAlphaEva;
            }
        }
        if (gCurTask->unk34 == 256) {
            gCurTask->unk2C ^= 1;
            gCurTask->unk30 ^= 1;
            gCurTask->unk34 = 0;
        }
        gCurTask->unk34 += 4;
        if (gCurTask->unk34 > 256)
            gCurTask->unk34 = 256;
        BlendColors(gUnk_08584BB0[gCurTask->unk2C], gUnk_08584BB0[gCurTask->unk30],
            (u16)gCurTask->unk34, 4, gUnk_03001390);
        TaskYieldTrampoline(1);
    }
}

/* Task type #263 (class 4): halve this player's score, round it down to a
   multiple of 10 and count the displayed score down to it. */
void Task_HalveScore(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    gCurTask->unk28 = gPlayerScores[gLocalPlayer];
    gPlayerScores[gLocalPlayer] >>= 1;
    gCurTask->unk2C = Mod(gPlayerScores[gLocalPlayer], 10);
    gPlayerScores[gLocalPlayer] -= gCurTask->unk2C;
    while (gCurTask->unk28 != gPlayerScores[gLocalPlayer]) {
        gCurTask->unk28 -= 10;
        DrawScoreToBgMap(gCurTask->unk28, 22, 18);
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

/* Task type #264 variant 0: sub-states gUnk_087582AC[Task.state], per-frame
   handlers gUnk_087582B8[Task.updateState] (GameOverPlayerUpdate). */
void GameOverPlayer(void)
{
    gCurTask->updateCallback = (u32)GameOverPlayerUpdate;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_08754914;
    gCurTask->unk24 = 0;
    gCurTask->unk18 = 0;
    gCurTask->facing = 1;
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 3, gUnk_087582AC);
    TaskSleepForever();
}

/* Task type #264 variant 0's per-frame hook: handler gUnk_087582B8[Task.updateState]. */
void GameOverPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gUnk_087582B8);
}

/* Re-enter task type #264 variant 0 (TaskSetEntry installs this as its
   body): Task.unk24 = 1, then sub-state gUnk_087582AC[Task.state]. */
void GameOverPlayerEnterState(void)
{
    gCurTask->unk24 = 1;
    CallTableEntry(gCurTask->state, 3, gUnk_087582AC);
}
