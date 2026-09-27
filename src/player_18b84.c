#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "cutscene.h"
#include "player.h"
#include "effect.h"

s32 PlaySfx(s32 id);

void sub_08018b84(void)
{
    struct Task *t;
    s32 v;

    v = gCurTask->unk20;
    if (v < 0)
        return;
    if (v == 0)
        PlaySfx(0x111);
    t = gCurTask;
    v = t->unk20 + 1;
    t->unk20 = v;
    if (v > 2)
        t->unk20 = 0;
}

void sub_08018bb8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->layer = 10;
    gCurTask->frameTable = gUnk_0874C600;
    gCurTask->tileWord = 0;
    while (1)
    {
        gCurTask->frame = 0xFFFF;
        if (gTasks[gCurTask->parent].unk28 == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gTasks[gCurTask->parent].unk28 == 0);
        }
        u = gCurTask;
        u->posX = gTasks[u->parent].pixelX << 16;
        u->posY = (gTasks[u->parent].pixelY + 24) << 16;
        TaskSetMotion(0xFFFD0000, 0, 0x5A5A5A5A, 0, 0xFFFFE000, 0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        CreateCutsceneActor(54, 32);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        TaskStop();
        v = gCurTask;
        v->posX = gTasks[v->parent].pixelX << 16;
        v->posY = (gTasks[v->parent].pixelY + 24) << 16;
        TaskSetMotion(0xFFFDC000, 0xFFFFC000, 0x5A5A5A5A, 192 << 5, 0xFFFFE000,
                     0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
    }
}

void sub_08018d7c(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gUnk_0874C600;
    u->posX = gTasks[u->parent].pixelX << 16;
    u->posY = gTasks[u->parent].pixelY << 16;
    u->accelX = 0x4000;
    u->accelY = 0xFFFFE000;
    u->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
