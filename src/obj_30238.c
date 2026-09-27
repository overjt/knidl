#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "camera.h"
#include "effect.h"

/* obj_30238.c (0x08030238-0x080306B3, issue #86).
 *
 * Task type #236 (class 4): Task_StageEffect dispatches on Task.state into the
 * six bodies of the anchor table gUnk_087328D8 (CreateStageEffect spawns it).
 * They are short sprite animations that step Task.frame, the frame of the
 * Task.frameTable graphics table, every one to four frames; sub_08030404 also
 * falls (Task.accelY = -0x400) and sub_080304ec rises (Task.velY =
 * -0x10000). */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));

void Task_StageEffect(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_087328D8);
}

void sub_08030254(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_0874CD54;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_080302cc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_0874CD68;
    TaskStop();
    gCurTask->frame = 0;
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
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    v = gCurTask;
    v->unk18 = 1;
    v->frame++;
    TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_08030404(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gUnk_0874CD68;
    TaskStop();
    v = gCurTask;
    v->accelY = -0x400;
    v->frame = 18;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_080304ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_08752548;
    TaskStop();
    v = gCurTask;
    v->velY = -0x10000;
    v->frame = 0;
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
    TaskExitTrampoline();
}

void sub_08030580(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_0874CDE0;
    u->frame = 0;
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
    TaskExitTrampoline();
}

void sub_08030604(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_0874C804;
    u->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
