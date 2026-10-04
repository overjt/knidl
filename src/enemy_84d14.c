/* game_code_and_rodata 0x08084D14-0x080860F8 (issue #69, module M22 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08084D14 0x080860F8 src/enemy_84d14.c --newpb
 *
 * Three more scripts in the same three-table shape as src/enemy_82e68.c:
 *   * class-3 task #14 (entry `Task_Chilly` in the previous file): unk73 rows
 *     `0x08741FB8`, bodies `0x08741FC0`, guards `0x08741FD4`, per-frame hook
 *     `ChillyUpdate`;
 *   * class-3 task #17 (`Task_WaddleDoo`): `0x08741FE8` / `0x08741FF8` /
 *     `0x08742004`, per-frame hook `WaddleDooWalkUpdate`;
 *   * the `ParasolWaddleDooInit` script: `0x08742030` / `0x08742040`, per-frame hook
 *     `ParasolWaddleDooUpdate`;
 *   * class-3 task #20 (`Task_BrontoBurt` / `BrontoBurtEnterVariant`), whose seven unk73
 *     rows at `0x08742064` all point INTO module M23 - the first cross-module
 *     dispatch found in the behaviour banks.
 *
 * `ChillyLand` / `ChillyStartFall` / `ChillyEnterWater` / `ChillyHitWall` and
 * `WaddleDooLand` / `WaddleDooStartFall` / `WaddleDooEnterWater` / `WaddleDooHitWall` /
 * `WaddleDooHitCeiling` are the class-3 hook rows at `0x08742D0C` / `0x08742D1C` and
 * `0x08742D28` / `0x08742D38` / `0x08742D40`; the second group switches on
 * Task.variant (0 = plain, 1 = riding a carrier, 2-3 = ignore) instead of only
 * bailing out on 1.
 *
 * `WaddleDooHitCeiling` is the second leaf the prologue scan missed (lesson 4.30):
 * the table word at `0x08742D40` points at it and it clamps Task.velY (the
 * 16.16 vertical velocity) at zero.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 a);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(s32 a);
extern void ActorSetAttackBox(u32 *p);
extern void sub_08066b34(u32 *p);
extern void sub_08066c3c(u32 *p);
extern void sub_08066c08(u32 *p, s32 b);
extern s32 GetShapeAtPixelIgnoringOneWay(s32 x, s32 y);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern void ActorReactToHit(void);

void ChillyInit(void)
{
    u32 v;

    gCurTask->updateCallback = (u32)ChillyUpdate;
    v = RandomRange(4);
    switch (v)
    {
    case 0:
    case 1:
        ActorSetState(0);
        break;
    case 2:
        ActorSetState(2);
        break;
    case 3:
        ActorSetState(3);
        break;
    }
    CallTableEntry(gCurTask->state, 5, gChillyStates);
}

void ChillyEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ChillyUpdate;
    CallTableEntry(t->state, 5, gChillyStates);
}

void ChillyUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gChillyStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ChillyState0(void)
{
    struct Task *t;
    u32 zero;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->chillyLoopCount = zero;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(2);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(2);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(8);
        u4 = gCurTask;
        u4->frame--;
        TaskYieldTrampoline(2);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(2);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(1);
        gCurTask->facing = -gCurTask->facing;
        gCurTask->chillyLoopCount++;
    } while ((s16)gCurTask->chillyLoopCount <= 3);
    gCurTask->facing = -gCurTask->facing;
    ActorSetState(1);
    TaskSleepForever();
}

void ChillyState0Update(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void ChillyState1(void)
{
    s32 a;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    gCurTask->updateState = 1;
    while (1)
    {
        a = gCurTask->facing;
        if (a == TaskGetFacingTowardNearestPlayer()
            && abs(TaskGetNearestPlayerDy()) <= 32)
        {
            ActorSetState(2);
            TaskSleepForever();
        }
        gCurTask->facing = -gCurTask->facing;
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(2);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(2);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(8);
        u4 = gCurTask;
        u4->frame--;
        TaskYieldTrampoline(2);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(2);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(1);
    }
}

void ChillyState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void ChillySlide(void)
{
    struct Task *t;
    u16 zero;

    gCurTask->updateState = 2;
    t = gCurTask;
    zero = 0;
    if (t->actorSpawnArg == 0)
    {
        t->chillyLoopCount = zero;
        do
        {
            TaskFaceNearestPlayer();
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
            TaskSetFrame(7);
            TaskYieldTrampoline(18);
            TaskStopX();
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(6);
            TaskYieldTrampoline(6);
            TaskSetFrame(5);
            TaskYieldTrampoline(5);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            gCurTask->chillyLoopCount++;
        } while ((s16)gCurTask->chillyLoopCount <= 3);
    }
    else
    {
        t->chillyLoopCount = zero;
        do
        {
            TaskFaceNearestPlayer();
            TaskSetFrame(4);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(19);
            TaskStopX();
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(6);
            TaskYieldTrampoline(5);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            TaskSetFrame(4);
            TaskYieldTrampoline(3);
            gCurTask->chillyLoopCount++;
        } while ((s16)gCurTask->chillyLoopCount <= 2);
    }
    TaskFaceNearestPlayer();
    ActorSetState(3);
    TaskSleepForever();
}

void ChillySlideUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void ChillyState3(void)
{
    struct Task *t;
    struct Task *u;
    struct ActorSpawn sp;
    u16 zero1;
    u8 zero2;

    t = gCurTask;
    zero1 = 0;
    t->updateState = 3;
    TaskStop();
    TaskSetFrame(10);
    gCurTask->chillyLoopCount = zero1;
    do
    {
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        u = gCurTask;
        u->chillyLoopCount++;
    } while ((s16)u->chillyLoopCount <= 14);
    sp.subtype = 2;
    sp.taskType = 104;
    sp.variant = zero2 = 0;
    sp.spawnArg = u->actorSpawnArg;
    sp.checkTerrain = zero2;
    gCurTask->chillyFreezeSlot = CreateActorFromDescHere(&sp, 0);
    gCurTask->chillyLoopCount = zero2;
    do
    {
        gCurTask->velX = -0x10000;
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        TaskYieldTrampoline(1);
        gCurTask->chillyLoopCount++;
    } while ((s16)gCurTask->chillyLoopCount <= 63);
    TaskStop();
    ActorSetState(0);
    TaskSleepForever();
}

void ChillyState3Update(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void ChillyFall(void)
{
    struct Task *t;

    gCurTask->updateState = 4;
    TaskStop();
    t = gCurTask;
    t->accelY = 0x2500;
    t->speedLimitY = 0x30000;
    TaskSleepForever();
}

void ChillyFallUpdate(void)
{
}

void ChillyIdle(void)
{
    gCurTask->updateCallback = (u32)ChillyIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(18);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
    }
}

void ChillyIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

u8 ChillyLand(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant == 1)
        return 0;
    ActorSetState((u16)t->chillySavedState);
    TaskSetEntry(ChillyEnterState, gCurTaskIdx);
    return 1;
}

u8 ChillyStartFall(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant == 1)
        return 0;
    t->chillySavedState = t->state;
    ActorSetState(4);
    TaskSetEntry(ChillyEnterState, gCurTaskIdx);
    return 1;
}

u8 ChillyEnterWater(void)
{
    if (gCurTask->variant == 1)
        return 0;
    ActorStartDrown(-2);
    return 1;
}

s32 ChillyHitWall(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant != 1 && t->state == 2)
        TaskTurnAroundAndReverseX();
    return 0;
}

void Task_WaddleDoo(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gWaddleDooFrames;
    u->u8C.actor->extraFrame = 4;
    CallTableEntry(u->variant, 4, gWaddleDooVariants);
}

void WaddleDooWalkInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)WaddleDooWalkUpdate;
    t->waddleDooPickTimer = 15;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gWaddleDooWalkStates);
}

void WaddleDooWalkEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)WaddleDooWalkUpdate;
    if (t->state == 0)
        t->waddleDooPickTimer = 80;
    CallTableEntry(gCurTask->state, 3, gWaddleDooWalkStates);
}

void WaddleDooWalkUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gWaddleDooWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void WaddleDooWalk(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_08742010[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_08742020[gCurTask->actorSpawnArg]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08742024[u1->actorSpawnArg]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08742024[u2->actorSpawnArg]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08742024[u3->actorSpawnArg]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08742020[u4->actorSpawnArg]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08742024[u5->actorSpawnArg]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08742024[u6->actorSpawnArg]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08742024[u7->actorSpawnArg]);
    }
}

void WaddleDooWalkState0Update(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gCurTask;
    if (t->waddleDooAirborne == 0 && --t->waddleDooPickTimer == 0)
    {
        v = RandomRange(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gCurTask->waddleDooPickTimer = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        ActorSetState(n);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
    }
}

void WaddleDooWalkJump(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x28000, 0x1500, 0x30000);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskSleepForever();
}

void WaddleDooWalkJumpUpdate(void)
{
}

void WaddleDooWalkShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    TaskStop();
    u = gCurTask;
    u->waddleDooLoopCount = zero;
    while ((s16)gCurTask->waddleDooLoopCount < gUnk_08742028[gCurTask->actorSpawnArg])
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->waddleDooLoopCount++;
    }
    v = gCurTask;
    zero2 = 0;
    v->waddleDooBeamStep = zero2;
    PlaySfx(110);
    TaskSetFrame(12);
    gCurTask->waddleDooLoopCount = zero2;
    while ((s16)gCurTask->waddleDooLoopCount < gUnk_0874202C[gCurTask->actorSpawnArg])
    {
        sp.subtype = 4;
        sp.taskType = 106;
        sp.variant = 0;
        w = gCurTask;
        sp.spawnArg = w->actorSpawnArg;
        sp.x = 8;
        sp.y = 3;
        sp.checkTerrain = 1;
        if (GetShapeAtPixelIgnoringOneWay(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
            gCurTask->waddleDooBeamSlot = CreateActorFromDescAtOffsetFacing(&sp, 0);
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->waddleDooBeamStep++;
        if ((s16)x->frame == 12)
            TaskSetFrame(13);
        else
            TaskSetFrame(12);
        gCurTask->waddleDooLoopCount++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void WaddleDooWalkShootUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->waddleDooAirborne == 0 && t->state != 2)
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
}

void ParasolWaddleDooInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)ParasolWaddleDooUpdate;
    sub_08066b34(gParasolWaddleDooDef);
    gCurTask->waddleDooPickTimer = 15;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 4, gParasolWaddleDooStates);
}

void ParasolWaddleDooEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ParasolWaddleDooUpdate;
    if (t->state == 0)
        t->waddleDooPickTimer = 80;
    CallTableEntry(gCurTask->state, 4, gParasolWaddleDooStates);
}

void ParasolWaddleDooUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gParasolWaddleDooStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ParasolWaddleDooWalk(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_08742010[1], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_08742020[1]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08742020[1]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
    }
}

void ParasolWaddleDooWalkUpdate(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gCurTask;
    if (t->waddleDooAirborne == 0 && t->u8C.actor->extraFrame == -1 && --t->waddleDooPickTimer == 0)
    {
        v = RandomRange(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gCurTask->waddleDooPickTimer = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        ActorSetState(n);
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
    }
}

void ParasolWaddleDooJump(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x28000, 0x1500, 0x30000);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskSleepForever();
}

void ParasolWaddleDooJumpUpdate(void)
{
}

void ParasolWaddleDooShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    TaskStop();
    u = gCurTask;
    u->waddleDooLoopCount = zero;
    while ((s16)gCurTask->waddleDooLoopCount < gUnk_08742028[1])
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->waddleDooLoopCount++;
    }
    v = gCurTask;
    zero2 = 0;
    v->waddleDooBeamStep = zero2;
    PlaySfx(110);
    TaskSetFrame(12);
    gCurTask->waddleDooLoopCount = zero2;
    while ((s16)gCurTask->waddleDooLoopCount < gUnk_0874202C[gCurTask->actorSpawnArg])
    {
        sp.subtype = 4;
        sp.taskType = 106;
        sp.variant = 0;
        sp.spawnArg = 1;
        sp.x = 8;
        sp.y = 3;
        sp.checkTerrain = 1;
        w = gCurTask;
        if (GetShapeAtPixelIgnoringOneWay(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
            gCurTask->waddleDooBeamSlot = CreateActorFromDescAtOffsetFacing(&sp, 0);
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->waddleDooBeamStep++;
        if ((s16)x->frame == 12)
            TaskSetFrame(13);
        else
            TaskSetFrame(12);
        gCurTask->waddleDooLoopCount++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void ParasolWaddleDooShootUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->waddleDooAirborne == 0 && t->state != 2)
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
}

void ParasolWaddleDooDrift(void)
{
    gCurTask->updateState = 3;
    gCurTask->actorAnimDelay30 = ActorStartAnim(gUnk_08742050);
    TaskStartParasolDrift();
    while (1)
    {
        TaskStepParasolDrift();
        TaskYieldTrampoline(8);
    }
}

void ParasolWaddleDooDriftUpdate(void)
{
    gCurTask->actorAnimDelay30 = ActorTickAnim(gCurTask->actorAnimDelay30);
}

void WaddleDooIdle(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateCallback = (u32)WaddleDooIdleUpdate;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(7);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(7);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(7);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(10);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(7);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(7);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(7);
    }
}

void WaddleDooIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void WaddleDooShoot(void)
{
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateCallback = (u32)WaddleDooShootUpdate;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskStop();
        gCurTask->waddleDooLoopCount = 0;
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(10);
            u1 = gCurTask;
            u1->frame++;
            TaskYieldTrampoline(7);
            u2 = gCurTask;
            u2->frame++;
            TaskYieldTrampoline(7);
            u3 = gCurTask;
            u3->frame++;
            TaskYieldTrampoline(7);
            u4 = gCurTask;
            u4->frame++;
            TaskYieldTrampoline(10);
            u5 = gCurTask;
            u5->frame--;
            TaskYieldTrampoline(7);
            u6 = gCurTask;
            u6->frame--;
            TaskYieldTrampoline(7);
            u7 = gCurTask;
            u7->frame--;
            TaskYieldTrampoline(7);
            gCurTask->waddleDooLoopCount++;
        } while ((s16)gCurTask->waddleDooLoopCount <= 2);
        TaskSetFrame(6);
        TaskYieldTrampoline(32);
        gCurTask->waddleDooLoopCount = 0;
        do
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetFrame(11);
            TaskYieldTrampoline(2);
            gCurTask->waddleDooLoopCount++;
        } while ((s16)gCurTask->waddleDooLoopCount <= 15);
        gCurTask->waddleDooBeamStep = 0;
        PlaySfx(110);
        TaskSetFrame(12);
        gCurTask->waddleDooLoopCount = 0;
        do
        {
            sp.subtype = 4;
            sp.taskType = 106;
            sp.variant = 0;
            sp.spawnArg = 0;
            sp.x = 8;
            sp.y = 3;
            sp.checkTerrain = 1;
            w = gCurTask;
            if (GetShapeAtPixelIgnoringOneWay(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
                gCurTask->waddleDooBeamSlot = CreateActorFromDescAtOffsetFacing(&sp, 0);
            TaskYieldTrampoline(2);
            x = gCurTask;
            x->waddleDooBeamStep++;
            if ((s16)x->frame == 12)
                TaskSetFrame(13);
            else
                TaskSetFrame(12);
            gCurTask->waddleDooLoopCount++;
        } while ((s16)gCurTask->waddleDooLoopCount <= 15);
    }
}

void WaddleDooShootUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

u8 WaddleDooLand(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->waddleDooAirborne = 0;
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
        return 1;
    case 1:
        ActorStopAnim();
        sub_08066c3c(gWaddleDooDef);
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
        return 1;
    }
}

u8 WaddleDooStartFall(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->waddleDooAirborne = 1;
        t->accelY = 0x1500;
        t->speedLimitY = 0x30000;
        ActorSetState(0);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
        return 1;
    case 1:
        if (t->u8C.actor->extraFrame == -1)
        {
            t->waddleDooAirborne = 1;
            t->accelY = 0x1500;
            t->speedLimitY = 0x30000;
            ActorSetState(0);
            TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(3);
            TaskSetEntry(ParasolWaddleDooDrift, gCurTaskIdx);
        }
        return 1;
    }
}

u8 WaddleDooEnterWater(void)
{
    switch (gCurTask->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        ActorStartDrown(-2);
        return 1;
    case 1:
        sub_08066c08(gWaddleDooDef, 0);
        ActorStartDrown(-2);
        return 1;
    }
}

s32 WaddleDooHitCeiling(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->velY = 0;
        return 0;
    case 1:
        if (t->velY < 0)
            t->velY = 0;
        return 0;
    }
}

s32 WaddleDooHitWall(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        TaskTurnAroundAndReverseX();
        return 0;
    case 1:
        if (t->state == 3)
            sub_08066b70();
        else
            TaskTurnAroundAndReverseX();
        return 0;
    }
}

void ParasolWaddleDooReactToDefeat(void)
{
    sub_08066c08(gWaddleDooDef, 0);
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_BrontoBurt(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gBrontoBurtFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 7, gBrontoBurtVariants);
}

void BrontoBurtEnterVariant(void)
{
    CallTableEntry(gCurTask->variant, 7, gBrontoBurtVariants);
}
