#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "ending.h"

/* gameover_cbed4.c (0x080CBED4-0x080CCD4B, issue #100).
 *
 * The game-over screen, task type #264 variant 1: the cursor.  Its
 * sub-states gUnk_087582C4[Task.state] and per-frame handlers
 * gUnk_087582DC[Task.updateState] (GameOverChoice starts it, GameOverChoiceEnterState re-enters
 * it), plus the helpers the whole screen uses:
 *   CreateGameOverObject   spawn task type #264 with Task.variant = variant.
 *   sub_080cbf68 / sub_080cbfac   re-enter variant 0 (its task index is
 *       gGameOverPlayerTask) in sub-state 2 or 1 by this cursor's Task.unk24.
 *   sub_080cbfe4 / sub_080cc024   sub-state 0, the cursor at rest; up or down
 *       (GameOverIsUpDownPressed) moves it (sub-state 1), A or START picks: with
 *       Task.unk24 set sub-state 4, otherwise task type #263 and sub-state 2.
 *   sub_080cc0a4 / sub_080cc14c   sub-state 1, the move, which flips
 *       gGameOverCursor.
 *   sub_080cc180 ... sub_080cc768 / sub_080ccd10   sub-states 2-5, the cursor's
 *       animations after a choice; handlers 3 and 5 hand over to variant 0. */

void TaskYieldTrampoline(s32 frames);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetEntry(void *a, u32 i);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
u8 GameOverIsUpDownPressed(void);
void GameOverPlayerEnterState(void);
void GameOverChoiceUpdate(void);

/* Task type #264 variant 1: sub-states gUnk_087582C4[Task.state], per-frame
   handlers gUnk_087582DC[Task.updateState] (GameOverChoiceUpdate). */
void GameOverChoice(void)
{
    gCurTask->updateCallback = (u32)GameOverChoiceUpdate;
    gCurTask->layer = 6;
    gCurTask->frameTable = gUnk_087549B0;
    gCurTask->unk24 = 0;
    gCurTask->unk20 = 0;
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 6, gUnk_087582C4);
    TaskSleepForever();
}

/* Task type #264 variant 1's per-frame hook: handler gUnk_087582DC[Task.updateState]. */
void GameOverChoiceUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gUnk_087582DC);
}

/* Spawn task type #264 with Task.variant = variant. */
void CreateGameOverObject(u8 variant)
{
    s32 id = TaskCreateFrom(264, 32);
    struct Task *t;

    if (id != -1) {
        t = &gTasks[id];
        t->variant = variant;
    }
}

/* Re-enter task type #264 variant 0 (task gGameOverPlayerTask) in sub-state 2
   when this task's Task.unk24 is set, else 1. */
void sub_080cbf68(void)
{
    struct Task *t = &gTasks[gGameOverPlayerTask];

    if (gCurTask->unk24 != 0)
        t->state = 2;
    else
        t->state = 1;
    TaskSetEntry(GameOverPlayerEnterState, gGameOverPlayerTask);
}

/* Re-enter variant 0, clear Task.unk20 and spawn variant 3. */
void sub_080cbfac(void)
{
    sub_080cbf68();
    gCurTask->unk20 = 0;
    CreateGameOverObject(3);
}

/* Re-enter task type #264 variant 1: sub-state gUnk_087582C4[Task.state]. */
void GameOverChoiceEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_087582C4);
}

/* Task type #264 variant 1, sub-state 0. */
void sub_080cbfe4(void)
{
    gCurTask->updateState = 0;
    if (gCurTask->unk24 != 0) {
        gCurTask->posX = 128 << 16;
        gCurTask->posY = 110 << 16;
    } else {
        gCurTask->posX = 128 << 16;
        gCurTask->posY = 85 << 16;
    }
    gCurTask->frame = 0;
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 0. */
void sub_080cc024(void)
{
    if (GameOverIsUpDownPressed()) {
        gCurTask->unk24 ^= 1;
        gCurTask->state = 1;
    } else if (gPlayerPressedKeys[0] & 9) {
        PlaySfx(102);
        if (gCurTask->unk24 != 0) {
            gCurTask->state = 4;
        } else {
            TaskCreateFrom(263, 32);
            gCurTask->state = 2;
        }
    }
    if (gCurTask->state != 0)
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 1. */
void sub_080cc0a4(void)
{
    gCurTask->updateState = 1;
    if (gCurTask->unk24 != 0) {
        TaskSetMotion(-0xA000, 0x1000, 0x5A5A5A5A, 0, 0x5000, 0x5A5A5A5A);
        TaskYieldTrampoline(9);
        gCurTask->accelY = -0x5000;
        TaskYieldTrampoline(9);
    } else {
        TaskSetMotion(-0xA000, 0x1000, 0x5A5A5A5A, 0, -0x5000, 0x5A5A5A5A);
        TaskYieldTrampoline(9);
        gCurTask->accelY = 0x5000;
        TaskYieldTrampoline(9);
    }
    TaskStop();
    TaskYieldTrampoline(5);
    gCurTask->state = 0;
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 1. */
void sub_080cc14c(void)
{
    if (gCurTask->state != 1) {
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
        gGameOverCursor ^= 1;
    }
}

/* Task type #264 variant 1, sub-state 2. */
void sub_080cc180(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    gCurTask->frame = 1;
    gCurTask->velX = -0xA000;
    gCurTask->accelX = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velX = 0x18000;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskSetMotion(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskStop();
    gCurTask->frame = 0;
    TaskYieldTrampoline(10);
    gCurTask->frame = 1;
    TaskSetMotion(-0x20000, 0x1000, 0x5A5A5A5A, 0, 0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(15);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gCurTask->frame = 3;
    gCurTask->accelY = -0x800;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->state = 3;
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 2. */
void sub_080cc2b8(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 3. */
void sub_080cc2e0(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    gCurTask->frame = 4;
    TaskSetMotion(0x20000, 0, 0x5A5A5A5A, 0x20000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->unk20 = 1;
    gCurTask->frame++;
    TaskSetMotion(0, -0x1000, 0x5A5A5A5A, -0x30000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    PlaySfx(280);
    gCurTask->frame++;
    TaskSetMotion(-0x10000, -0x8000, 0x5A5A5A5A, -0x30000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotion(-0x10000, 0x1000, 0x5A5A5A5A, 0, 0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(12);
    TaskSetMotion(-0x4100, 0xA00, 0x5A5A5A5A, 0x5A5A5A5A, 0x1D00, 0x5A5A5A5A);
    TaskYieldTrampoline(12);
    TaskSetMotion(0, 0xA00, 0x5A5A5A5A, 0x16000, -0xC00, 0x5A5A5A5A);
    TaskYieldTrampoline(24);
    TaskSetMotion(0x10000, 0x1C00, 0x5A5A5A5A, 0x4000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = 8;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    PlaySfx(279);
    gCurTask->frame = 4;
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(10);
    PlaySfx(279);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(10);
    PlaySfx(279);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(6);
    gCurTask->accelX = 0x1000;
    TaskYieldTrampoline(16);
    TaskSetMotion(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    TaskSetMotion(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskSetMotion(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    TaskSetMotion(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskSetMotion(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    TaskSetMotion(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gCurTask->accelX = -0x2000;
    TaskYieldTrampoline(8);
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 3. */
void sub_080cc5d4(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
    if (gCurTask->unk20 != 0)
        sub_080cbfac();
}

/* Task type #264 variant 1, sub-state 4. */
void sub_080cc608(void)
{
    gCurTask->updateState = 4;
    TaskStop();
    gCurTask->frame = 1;
    gCurTask->velX = -0xA000;
    gCurTask->accelX = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velX = 0x18000;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskSetMotion(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskStop();
    gCurTask->frame = 0;
    TaskYieldTrampoline(10);
    gCurTask->frame = 1;
    TaskSetMotion(-0x20000, 0xC00, 0x5A5A5A5A, 0, 0x800, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(15);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(20);
    TaskSetMotion(0x20000, 0, 0x5A5A5A5A, 0x10000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->state = 5;
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 4. */
void sub_080cc740(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 5. */
void sub_080cc768(void)
{
    gCurTask->updateState = 5;
    TaskStop();
    PlaySfx(278);
    gCurTask->frame = 2;
    TaskSetMotion(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(7);
    gCurTask->unk20 = 1;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    PlaySfx(278);
    gCurTask->frame = 2;
    TaskSetMotion(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->frame = 0;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(20);
    TaskSetMotion(-0x18C00, 0xC00, 0x5A5A5A5A, -0x6000, 0x200, 0x5A5A5A5A);
    TaskYieldTrampoline(33);
    gCurTask->frame = 1;
    TaskSetMotion(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(7);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = 1;
    TaskSetMotion(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame = 2;
    TaskYieldTrampoline(16);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskSetMotion(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(7);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = 1;
    TaskSetMotion(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame = 2;
    TaskYieldTrampoline(16);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskSetMotion(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 10;
    TaskYieldTrampoline(7);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = 1;
    TaskSetMotion(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame = 2;
    TaskYieldTrampoline(11);
    gCurTask->frame = 1;
    TaskSetMotion(0x10000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 11;
    TaskYieldTrampoline(5);
    TaskSetMotion(-0x18000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    TaskSetMotion(0x1C000, -0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    TaskSetMotion(-0xA000, 0, 0x5A5A5A5A, -0x28000, 0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    TaskSetMotion(-0x4000, 0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskSetMotion(-0x8000, 0x1000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskSetMotion(-0x10000, 0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 13;
        TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->frame = 12;
    TaskSetMotion(0x30000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 125 << 16;
    gCurTask->frame = 17;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x60000, 0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0x60000, -0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = -1;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame = 17;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x60000, 0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0x60000, -0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = -1;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 133 << 16;
    gCurTask->frame = 18;
    TaskSetMotion(0x60000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotion(-0x60000, 0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame = -1;
    TaskStop();
    TaskSleepForever();
}

/* Task type #264 variant 1, handler 5. */
void sub_080ccd10(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(GameOverChoiceEnterState, gCurTaskIdx);
    if (gCurTask->unk20 != 0) {
        sub_080cbf68();
        gCurTask->unk20 = 0;
    }
}
