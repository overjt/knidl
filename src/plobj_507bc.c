#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_507bc.c (0x080507BC-0x080509EB, issue #90).
 *
 * Task type #6 (the objects the player's actions spawn through
 * CreatePlayerObject/sub_08053a44): the body and two shared callbacks.  The body
 * Task_PlayerObject links the task to its spawner (Task.unk8C =
 * &gTasks[Task.unk44], the first time only) and dispatches the
 * variant, the top byte of Task.unk18, through the 13 variant bodies
 * gPlayerObjectVariants.  sub_08050814 is the common exit most variants re-bind on
 * contact: it installs the sprite (TaskDrawScreen for PlayerState.unk37 == 2,
 * TaskDrawWorldInViewOrFree otherwise, animation table gUnk_0874C650), registers the
 * collider row Task.unk24 if there is one and steps animation frames
 * 0-10 before TaskExitTrampoline.  sub_0805091c is a second, drifting burst
 * (gUnk_0874C7CC). */

extern void (*gPlayerObjectVariants[])(void);   /* task type #6's 13 variants, indexed by Task.unk18 >> 24 */
extern u32 gUnk_0874C650[];
extern u32 gUnk_0874C7CC[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
/* task / sprite services (landed prototypes) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void TaskMove(void);
void TaskDrawScreen(void);
void TaskDrawWorldInViewOrFree(void);
void TaskSetFrameByFacing(s16 a);
void TaskStop(void);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */

void Task_PlayerObject(void)
{
    if (gCurTask->unk8C == NULL)
    {
        gCurTask->unk80 = 0;
        gCurTask->unk8C = (struct Actor *)&gTasks[gCurTask->unk44];
        gCurTask->unk78 = 1;
        gCurTask->unk24 = 0;
    }
    CallTableEntry(((u8 *)gCurTask)[27], 13, gPlayerObjectVariants);
}

void sub_08050814(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    if (t->unk88->unk37 != 2)
        t->unk0C = (u32)TaskDrawWorldInViewOrFree;
    else
        t->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = 0;
    gCurTask->unk08 = 0;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C650;
    t->unk3E = 0;
    t->unk40 = 0;
    if (t->unk24 != 0)
        sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, (void *)t->unk24);
    TaskStop();
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 6;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 9;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0805091c(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorldInViewOrFree;
    gCurTask->unk04 = 0;
    gCurTask->unk08 = 0;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C7CC;
    t->unk3E = 0;
    t->unk40 = 0;
    t->unk28 = 0;
    if (t->unk58 < 0)
        t->unk28 = 1;
    TaskStop();
    if (gCurTask->unk28 == 1)
        gCurTask->unk58 = 0x8000;
    else
        gCurTask->unk58 = -0x8000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(3);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(3);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(3);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
