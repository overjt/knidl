#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern struct Task *gCurTask;
extern struct Task gTasks[];
extern u32 gUnk_0874C600[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 PlaySfx(s32 id);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void CreateCutsceneActor(s32 a, s32 b);

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
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)TaskDrawScreen;
    t->layer = 10;
    gCurTask->unk38 = gUnk_0874C600;
    gCurTask->unk40 = 0;
    while (1)
    {
        gCurTask->frame = 0xFFFF;
        if (gTasks[gCurTask->unk44].unk28 == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gTasks[gCurTask->unk44].unk28 == 0);
        }
        u = gCurTask;
        u->posX = gTasks[u->unk44].unk48 << 16;
        u->posY = (gTasks[u->unk44].unk4A + 24) << 16;
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
        v->posX = gTasks[v->unk44].unk48 << 16;
        v->posY = (gTasks[v->unk44].unk4A + 24) << 16;
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

    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)TaskDrawScreen;
    t->layer = 9;
    u = gCurTask;
    u->unk38 = gUnk_0874C600;
    u->posX = gTasks[u->unk44].unk48 << 16;
    u->posY = gTasks[u->unk44].unk4A << 16;
    u->unk5C = 0x4000;
    u->unk60 = 0xFFFFE000;
    u->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
