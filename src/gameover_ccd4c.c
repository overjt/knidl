#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "ending.h"

/* gameover_ccd4c.c (0x080CCD4C-0x080CD32F, issue #100).
 *
 * The game-over screen, task type #264 variants 2-5.
 *   sub_080ccd4c / sub_080cce98   variant 2, a sprite that follows variant 0
 *       (gGameOverPlayerTask) through three motion sets gUnk_087582F4[] and ends
 *       once variant 0 leaves sub-state 0.
 *   sub_080ccec8   variant 3: sub-state gUnk_08758324[Task.state] (1 when the
 *       cursor gGameOverCursor is set), handlers gUnk_0875832C[Task.updateState]
 *       (sub_080ccf10): sub_080ccf2c / sub_080cd0c8 and sub_080cd0cc /
 *       sub_080cd248, two scripted sprites with empty handlers.
 *   sub_080cd24c / sub_080cd2f8   variants 4 and 5. */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
s32 PlaySfx(s32 id);                                    /* play a sound effect */
void TaskFree(s32 id);                                   /* kill task */
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskSetFrame(s32 a);
void sub_080cce98(void);
void sub_080ccf10(void);

/* Task type #264 variant 2. */
void sub_080ccd4c(void)
{
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_087549FC;
    gCurTask->updateCallback = (u32)sub_080cce98;
    gCurTask->facing = gTasks[gGameOverPlayerTask].facing;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(64);
    gCurTask->unk24 = 0;
    for (;;) {
        gCurTask->posX = (gCurTask->facing << 19) + (120 << 16);
        gCurTask->posY = 129 << 16;
        TaskSetMotionXFacing(gUnk_087582F4[gCurTask->unk24], gUnk_087582F4[gCurTask->unk24 + 1]);
        gCurTask->velY = gUnk_087582F4[gCurTask->unk24 + 2];
        gCurTask->accelY = gUnk_087582F4[gCurTask->unk24 + 3];
        gCurTask->unk24 += 4;
        if (gCurTask->unk24 > 11)
            gCurTask->unk24 = 0;
        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        TaskSetFrame(3);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        TaskStop();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(64);
    }
}

/* Task type #264 variant 2's callback: end once the variant-0 task leaves
   sub-state 0. */
void sub_080cce98(void)
{
    struct Task *t = &gTasks[gGameOverPlayerTask];

    if (t->state != 0)
        TaskFree(gCurTaskIdx);
}

/* Task type #264 variant 3: sub-state gUnk_08758324[Task.state], 1 when
   gGameOverCursor is set, per-frame handlers gUnk_0875832C[Task.updateState]
   (sub_080ccf10). */
void sub_080ccec8(void)
{
    gCurTask->updateCallback = (u32)sub_080ccf10;
    if (gGameOverCursor != 0)
        gCurTask->state = 1;
    else
        gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 2, gUnk_08758324);
    TaskSleepForever();
}

/* Task type #264 variant 3's per-frame hook: handler gUnk_0875832C[Task.updateState]. */
void sub_080ccf10(void)
{
    CallTableEntry(gCurTask->updateState, 2, gUnk_0875832C);
}

/* Task type #264 variant 3, sub-state 0. */
void sub_080ccf2c(void)
{
    gCurTask->updateState = 0;
    gCurTask->frameTable = gUnk_08754984;
    gCurTask->layer = 8;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 129 << 16;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, -0x2B00, 0x5A5A5A5A, -0x10000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(6);
    gCurTask->frame = 2;
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskSetMotion(0x5A5A5A5A, -0x22B00, 0x5A5A5A5A, -0x70000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskSetMotion(0x5A5A5A5A, 0x20000, 0x5A5A5A5A, -0x40000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    gCurTask->accelX = -0x2B00;
    TaskYieldTrampoline(2);
    TaskSetMotion(-0x30000, 0, 0x5A5A5A5A, 0x5A5A5A5A, 0x4000, 0x5A5A5A5A);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskSleepForever();
}

void sub_080cd0c8(void)
{
}

/* Task type #264 variant 3, sub-state 1. */
void sub_080cd0cc(void)
{
    gCurTask->updateState = 1;
    gCurTask->frameTable = gUnk_08754908;
    gCurTask->layer = 5;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 129 << 16;
    gCurTask->frame = 0;
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
    gCurTask->velX = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x88000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotion(0x38000, -0x10000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(22);
    gCurTask->frame++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x10000, 0x4000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080cd248(void)
{
}

/* Task type #264 variant 4. */
void sub_080cd24c(void)
{
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_087548B8;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 129 << 16;
    gCurTask->frame = 19;
    gCurTask->velY = -0x38000;
    gCurTask->accelY = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->frame = 0xFFFF;
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 19;
    gCurTask->velY = -0x38000;
    gCurTask->accelY = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->frame = -1;
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    TaskYieldTrampoline(10);
    gCurTask->frame = 19;
    gCurTask->velX = 0x38000;
    gCurTask->accelX = -0x10000;
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

/* Task type #264 variant 5. */
void sub_080cd2f8(void)
{
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_087548A8;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 120 << 16;
    gCurTask->frame = gPlayerCount - 1;
    TaskSleepForever();
}
