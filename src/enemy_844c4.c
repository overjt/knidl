/* game_code_and_rodata 0x080844C4-0x08084D14 (issue #69, module M22 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080844C4 0x08084D14 src/enemy_844c4.c --newpb
 *
 * The body of the class-3 task #10 script whose entry (`Task_Noddy`) is at
 * the end of src/enemy_82e68.c: `NoddyInit` / `NoddyEnterState` are the two
 * unk73 rows of `0x08741F70`, the six coroutine bodies hang off `0x08741F78`
 * and the six per-frame guards off `0x08741F90`, and `NoddyUpdate` is the
 * per-frame hook - it keeps the low half of Task.unk24 (the 16.16 vertical
 * offset the draw helper reads) while Task.onGround bit 0 says the object is
 * still attached, and or-s in 0x10000 while `gTerrainResult[4]` (the room's
 * kind) is outside 1-4.
 *
 * `NoddyLand` / `NoddyStartFall` / `NoddyEnterWater` / `NoddyBounceOffWall` plus the
 * shared `sub_08084c84` are the four class-3 hook rows at `0x08742CF0` /
 * `0x08742D00`: each returns 1 when it has handed the task to a new state and
 * 0 otherwise, and all four open with the same `Task.variant == 1` bail-out.
 * `Task_Chilly` is the class-3 task #14 entry; its script is in
 * src/enemy_84d14.c.
 *
 * `NoddySleepFallUpdate` is a leaf the census could not propose (no `push`, lesson
 * 4.30): the anchor-table word at `0x08741FA0` points at it and it counts the
 * <Task.unk30, Task.unk34> pair down, restoring both when unk34 underflows.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(s32 a);
extern void ActorSetAttackBox(u32 *p);
extern u8 ActorStepBackFromSlope(void);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern void ActorReactToHit(void);

void NoddyInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)NoddyUpdate;
    t->onGround = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 6, gNoddyStates);
}

void NoddyEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)NoddyUpdate;
    CallTableEntry(t->state, 6, gNoddyStates);
}

void NoddyUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = ActorCollideTerrain();
    t = gCurTask;
    if ((t->onGround & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            t->actorFlatGroundY = (u16)t->actorFlatGroundY | 0x10000;
        if ((gCurTask->onGround & 1) != 0)
            goto skip;
    }
    u = gCurTask;
    u->actorFlatGroundY = (u16)u->actorFlatGroundY;
skip:
    if (r == 0)
    {
        sub_08084c84();
        CallTableEntry(gCurTask->updateState, 6, gNoddyStateUpdates);
    }
    v = gCurTask;
    v->actorFlatGroundY = (v->actorFlatGroundY & 0xFFFF0000) | v->pixelY;
    ActorCheckHits();
    ActorReactToHit();
}

void NoddyWalk(void)
{
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 0;
    TaskStop();
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->noddyWalkTimer = gUnk_08741FA8[t->actorSpawnArg];
    TaskSetMotionXFacing(gUnk_08741FAC[t->actorSpawnArg], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(9);
        TaskYieldTrampoline(gUnk_08741FB4[gCurTask->actorSpawnArg]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u1->actorSpawnArg]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u2->actorSpawnArg]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u3->actorSpawnArg]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u4->actorSpawnArg]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u5->actorSpawnArg]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u6->actorSpawnArg]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u7->actorSpawnArg]);
    }
}

void NoddyWalkUpdate(void)
{
    if (--gCurTask->noddyWalkTimer == 0)
    {
        ActorSetState(1);
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
    }
}

void NoddyState5(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 5;
    TaskStop();
    t = gCurTask;
    if (t->actorSpawnArg != 0)
    {
        if (t->actorSpawnArg == 1)
            t->velY = -0x30000;
    }
    u = gCurTask;
    u->accelY = 0x1500;
    u->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(9);
        TaskYieldTrampoline(gUnk_08741FB4[gCurTask->actorSpawnArg]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u1->actorSpawnArg]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u2->actorSpawnArg]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u3->actorSpawnArg]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08741FB4[u4->actorSpawnArg]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u5->actorSpawnArg]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u6->actorSpawnArg]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08741FB4[u7->actorSpawnArg]);
    }
}

void NoddyState5Update(void)
{
}

void NoddyState1(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(10);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(20);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(40);
    ActorSetState(2);
    TaskSleepForever();
}

void NoddyState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
}

void NoddySleep(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gCurTask->updateState = 2;
    TaskStop();
    t = gCurTask;
    t->noddySleepTimer = 224;
    t->noddySleepPeriodCount = 1;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(26);
        u = gCurTask;
        u->frame++;
        TaskYieldTrampoline(22);
        gCurTask->noddyBubbleSlot = CreateChildTaskAtOffsetFacing(194, 12, 0, 1);
        gCurTask->frame++;
        TaskYieldTrampoline(26);
        w = gCurTask;
        w->frame--;
        TaskYieldTrampoline(22);
    }
}

void NoddySleepUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if (--t->noddySleepTimer == 0)
        t->noddySleepPeriodCount--;
    u = gCurTask;
    if (u->noddySleepPeriodCount == 0 && u->noddySleepTimer == 0)
    {
        ActorSetState(3);
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
    }
    v = gCurTask;
    if (v->actorSpawnArg != 0 && v->noddySleepPeriodCount != 0 && v->noddySleepTimer <= 119 && TaskGetNearestPlayerDistSq() <= 0xFFF)
    {
        ActorSetState(3);
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
    }
}

void NoddyState3(void)
{
    struct Task *t;

    gCurTask->updateState = 3;
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(10);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(20);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(40);
    ActorSetState(0);
    TaskSleepForever();
}

void NoddyState3Update(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
}

void NoddySleepFall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gCurTask->updateState = 4;
    TaskStop();
    t = gCurTask;
    t->accelY = 0x1500;
    t->speedLimitY = 0x30000;
    t->noddySleepTimer = 224;
    t->noddySleepPeriodCount = 1;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(26);
        u = gCurTask;
        u->frame++;
        TaskYieldTrampoline(22);
        gCurTask->noddyBubbleSlot = CreateChildTaskAtOffsetFacing(194, 12, 0, 1);
        gCurTask->frame++;
        TaskYieldTrampoline(26);
        w = gCurTask;
        w->frame--;
        TaskYieldTrampoline(22);
    }
}

void NoddySleepFallUpdate(void)
{
    struct Task *t;
    s32 a;
    s32 b;

    t = gCurTask;
    a = t->noddySleepTimer;
    t->noddySleepTimer = a - 1;
    if (t->noddySleepTimer == 0)
    {
        b = t->noddySleepPeriodCount;
        t->noddySleepPeriodCount = b - 1;
        if (t->noddySleepPeriodCount < 0)
        {
            t->noddySleepTimer = a;
            t->noddySleepPeriodCount = b;
        }
    }
}

void NoddyVariant1(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gCurTask->updateCallback = (u32)sub_08084ae8;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(26);
        u = gCurTask;
        u->frame++;
        TaskYieldTrampoline(22);
        gCurTask->noddyBubbleSlot = CreateChildTaskAtOffsetFacing(194, 12, 0, 1);
        gCurTask->frame++;
        TaskYieldTrampoline(26);
        w = gCurTask;
        w->frame--;
        TaskYieldTrampoline(22);
    }
}

void sub_08084ae8(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void Task_NoddyBubble(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gNoddyBubbleFrames;
    t->layer = 10;
    u = gCurTask;
    u->updateCallback = (u32)NoddyBubbleUpdate;
    u->facing = (gTasks + (s16)u->parent)->facing;
    TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
    gCurTask->velY = -0x4000;
    TaskSetFrame(0);
    TaskYieldTrampoline(48);
    TaskExitTrampoline();
}

void NoddyBubbleUpdate(void)
{
    struct Task *t;

    t = gTasks + (s16)gCurTask->parent;
    if (t->variant != 1 && t->state != 2 && t->state != 4)
        TaskFree(gCurTaskIdx);
}

u8 NoddyLand(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    if (t->variant != 1)
    {
        switch (t->state)
        {
        case 5:
            v = 0;
            break;
        case 4:
            v = 2;
            break;
        default:
            goto def;
        }
        ActorSetState(v);
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
        return 1;
def:
        TaskStop();
    }
    return 0;
}

u8 NoddyStartFall(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    if (t->variant != 1)
    {
        switch (t->state)
        {
        case 0:
        case 3:
            v = 5;
            break;
        case 1:
        case 2:
            v = 4;
            break;
        default:
            goto out;
        }
        ActorSetState(v);
        TaskSetEntry(NoddyEnterState, gCurTaskIdx);
        return 1;
    }
out:
    return 0;
}

u8 NoddyEnterWater(void)
{
    if (gCurTask->variant == 1)
        return 0;
    ActorStartDrown(-2);
    return 1;
}

s32 sub_08084c84(void)
{
    struct Task *t;
    u8 r;

    t = gCurTask;
    if (t->variant == 1)
        return 0;
    if (t->velX != 0)
    {
        r = ActorStepBackFromSlope();
        if (r != 0)
        {
            TaskTurnAroundAndReverseX();
            return 0;
        }
    }
}

s32 NoddyBounceOffWall(void)
{
    if (gCurTask->variant != 1)
        TaskTurnAroundAndReverseX();
    return 0;
}

void Task_Chilly(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gChillyFrames;
    CallTableEntry(u->variant, 2, gChillyVariants);
}
