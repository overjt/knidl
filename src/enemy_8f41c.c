/* game_code_and_rodata 0x0808F41C-0x0809000C (issue #70, module M24 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808F41C 0x0809000C src/enemy_8f41c.c --newpb
 *
 * See src/enemy_8cce8.c for the three-table pattern all of M24 is built on.
 *
 * This batch holds:
 *   * script 4's bodies and guards (tables `0x0874329C`/`0x087432A8`,
 *     `0x087432B4`/`0x087432C0`, `0x087432CC`/`0x087432D8` and the
 *     single-row `0x087432E4`/`0x087432E8`); the bodies walk the `s16[][4]`
 *     aim table `gUnk_0874325A` one row per Task.unk34 / Task.variant through
 *     `ShotzoSetRecoilVelocity`;
 *   * the class-3 hook row `0x08743518` — `sub_0808f9b8`, `sub_0808f978`,
 *     `sub_0808f9d8` and `sub_0808f9f8`;
 *   * script 5: entry `Task_Coner` (`0x087432F4`, 2 rows), rows
 *     `ConerInit` / `ConerIdleInit`, bodies `0x087432FC` (3) and
 *     `0x08743308` (1);
 *   * script 6: entry `Task_LaserBallLaser` (`0x08743600`, 1 row), row
 *     `LaserBallLaserInit`, bodies `0x08743604` (2), guards `0x0874360C` (2);
 *   * script 7: entry `Task_ShotzoCannonball` (`0x0874362C`, 4 identical rows), row
 *     `ShotzoCannonballInit`, body `0x0874363C` (`ShotzoCannonballState0`, the class-2 wanderer
 *     that picks its heading from gTasks[Task.parent].unk34) and guard
 *     `0x08743640` (`ShotzoCannonballState0Update`).
 *
 * `sub_0808fa04` and `ShotzoCannonballEnterState` are dead exports (twins of
 * `sub_0808f9f8` and of `ShotzoCannonballInit`'s cue call) that no ROM word points at;
 * both are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *a, u32 i);
extern void ActorSetState(u16 v);
extern void ActorSetAttackBox(void *p);
extern void AngleToVector(s32 a, s32 b);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_08069888(void);
extern u32 ActorReactToHit(void);

void ShotzoAimShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 2;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->shotzoShotDone = 0;
    t->shotzoRecoilDone = 0;
    PlaySfx(190);
    CreateShotzoCannonball();
    u = gCurTask;
    u->shotzoRecoilStep = 0;
    u->shotzoLoopCount = 0;
    do
    {
        v = gCurTask;
        ShotzoSetRecoilVelocity(gUnk_0874325A[v->shotzoBarrelDir][v->shotzoRecoilStep]);
        w = gCurTask;
        w->shotzoRecoilStep++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->shotzoLoopCount++;
    } while ((s16)x->shotzoLoopCount <= 3);
    y = gCurTask;
    y->shotzoRecoilDone = 1;
    y->shotzoShotDone = 1;
    y->shotzoArmed = 0;
    TaskSleepForever();
}

void ShotzoAimShootUpdate(void)
{
    if ((s16)gCurTask->shotzoRecoilDone != 0 && sub_08069888() == 0 && gCurTask->shotzoShotDone != 0)
    {
        ActorSetState(0);
        TaskSetEntry(ShotzoAimEnterState, gCurTaskIdx);
    }
}

void ShotzoAimFall(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    TaskSleepForever();
}

void ShotzoAimFallUpdate(void)
{
    sub_08069888();
}

void ShotzoFixedState0(void)
{
    gCurTask->updateState = 0;
    TaskStopY();
    switch (gCurTask->variant)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    TaskSleepForever();
}

void ShotzoFixedState0Update(void)
{
    struct Task *t;

    if (sub_08069888() == 0)
    {
        t = gCurTask;
        switch (t->variant)
        {
        case 1:
            t->shotzoBarrelDir = 2;
            break;
        case 2:
            t->shotzoBarrelDir = 1;
            break;
        case 3:
            t->shotzoBarrelDir = 3;
            break;
        }
        ActorSetState(2);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
    }
}

void ShotzoFixedShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->shotzoShotDone = 0;
    t->shotzoRecoilDone = 1;
    while (1)
    {
        TaskYieldTrampoline(100);
        gCurTask->shotzoLoopCount = 0;
        do
        {
            gCurTask->shotzoRecoilDone = 0;
            PlaySfx(190);
            CreateShotzoFixedCannonball();
            u = gCurTask;
            u->shotzoFixedRecoilStep = 0;
            u->shotzoFixedRecoilCount = 0;
            do
            {
                v = gCurTask;
                ShotzoSetRecoilVelocity(gUnk_0874325A[v->variant][v->shotzoFixedRecoilStep]);
                w = gCurTask;
                w->shotzoFixedRecoilStep++;
                TaskYieldTrampoline(2);
                TaskStop();
                x = gCurTask;
                x->shotzoFixedRecoilCount++;
            } while ((s16)x->shotzoFixedRecoilCount <= 3);
            gCurTask->shotzoRecoilDone = 1;
            TaskYieldTrampoline(7);
            y = gCurTask;
            y->shotzoLoopCount++;
        } while ((s16)y->shotzoLoopCount <= 2);
        gCurTask->shotzoShotDone = 1;
    }
}

void ShotzoFixedShootUpdate(void)
{
    if ((s16)gCurTask->shotzoRecoilDone != 0 && sub_08069888() == 0 && gCurTask->shotzoShotDone != 0)
    {
        TaskStop();
        ActorSetState(2);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
    }
}

void ShotzoFixedFall(void)
{
    struct Task *t;

    switch (gCurTask->variant)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    gCurTask->updateState = 1;
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    TaskSleepForever();
}

void ShotzoFixedFallUpdate(void)
{
    sub_08069888();
}

void ParasolShotzoAim(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->shotzoSpeedLevel = 1;
    t = gCurTask;
    t->shotzoAimTimer = gUnk_08743248[t->shotzoSpeedLevel];
    TaskStop();
    while (1)
        ShotzoAimBarrel();
}

void ParasolShotzoAimUpdate(void)
{
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        if (sub_08069888() == 0)
            ShotzoCheckShoot(1, ParasolShotzoEnterState);
    }
    else if (ActorCollideTerrain() == 0)
        ShotzoCheckShoot(1, ParasolShotzoEnterState);
}

void ParasolShotzoShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->shotzoShotDone = 0;
    t->shotzoRecoilDone = 0;
    PlaySfx(190);
    CreateShotzoCannonball();
    u = gCurTask;
    u->shotzoRecoilStep = 0;
    u->shotzoLoopCount = 0;
    do
    {
        v = gCurTask;
        ShotzoSetRecoilVelocity(gUnk_0874325A[v->shotzoBarrelDir][v->shotzoRecoilStep]);
        w = gCurTask;
        w->shotzoRecoilStep++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->shotzoLoopCount++;
    } while ((s16)x->shotzoLoopCount <= 3);
    y = gCurTask;
    y->shotzoRecoilDone = 1;
    y->shotzoShotDone = 1;
    y->shotzoArmed = 0;
    TaskSleepForever();
}

void ParasolShotzoShootUpdate(void)
{
    if ((s16)gCurTask->shotzoRecoilDone != 0 && sub_08069888() == 0 && gCurTask->shotzoShotDone != 0)
    {
        ActorSetState(0);
        TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
    }
}

void ParasolShotzoState2(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskStop();
        t = gCurTask;
        t->accelY = 168 << 5;
        t->speedLimitY = 192 << 10;
        TaskSleepForever();
    }
    else
    {
        TaskStartParasolDrift();
        while (1)
        {
            TaskStepParasolDrift();
            TaskYieldTrampoline(8);
        }
    }
}

void ParasolShotzoState2Update(void)
{
    ActorCollideTerrain();
}

void ShotzoIdleInit(void)
{
    gCurTask->updateCallback = (u32)ShotzoIdleUpdate;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 1, gShotzoIdleStates);
}

void ShotzoIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gShotzoIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ShotzoIdle(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrameNoFlip(6);
    TaskSleepForever();
}

void ShotzoIdleState0Update(void)
{
}

s32 sub_0808f978(void)
{
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
    {
        ActorSetState(2);
        TaskSetEntry(ConerEnterState, gCurTaskIdx);
        return 1;
    }
    else
    {
        ActorSetState(1);
        TaskSetEntry(ConerEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808f9b8(void)
{
    ActorSetState(0);
    TaskSetEntry(ConerEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9d8(void)
{
    ActorSetState(2);
    TaskSetEntry(ConerEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9f8(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_0808fa04(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void Task_Coner(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gConerFrames;
    CallTableEntry(u->variant, 2, gConerVariants);
}

void ConerInit(void)
{
    gCurTask->updateCallback = (u32)ConerUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gConerStates);
}

void ConerUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void ConerEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gConerStates);
}

void ConerWalk(void)
{
    TaskStop();
    while (1)
    {
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->actorSpawnArg][0]);
        TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->actorSpawnArg][1]);
        TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->actorSpawnArg][2]);
        gCurTask->velX = 0;
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->actorSpawnArg][3]);
    }
}

void ConerState1(void)
{
    TaskInitWaterFlags();
    gCurTask->accelY = 128 << 5;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void ConerState2(void)
{
    TaskStopY();
    gCurTask->velY = 128 << 7;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void ConerIdleInit(void)
{
    gCurTask->updateCallback = (u32)ConerIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gConerIdleStates);
}

void ConerIdleUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

void ConerIdle(void)
{
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[0][0]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][1]);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[0][2]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][3]);
    }
}

void Task_LaserBallLaser(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gLaserBallLaserFrames;
    TaskFaceLikeParent();
    gCurTask->onGround = 0;
    u = gCurTask;
    u->unk28 = 0;
    CallTableEntry(u->variant, 1, gLaserBallLaserVariants);
}

void LaserBallLaserInit(void)
{
    gCurTask->updateCallback = (u32)LaserBallLaserUpdate;
    PlaySfx(165);
    TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
    ActorSetState(1);
    CallTableEntry(gCurTask->state, 2, gLaserBallLaserStates);
}

void LaserBallLaserUpdate(void)
{
    if (sub_08069604() == 0)
        CallTableEntry(gCurTask->updateState, 2, gLaserBallLaserStateUpdates);
    else
    {
        ActorSetState(0);
        TaskSetEntry(LaserBallLaserEnterState, gCurTaskIdx);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void LaserBallLaserEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gLaserBallLaserStates);
}

void LaserBallLaserState1(void)
{
    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    gCurTask->frame = 0;
    TaskSleepForever();
}

void LaserBallLaserState1Update(void)
{
}

void LaserBallLaserState0(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    gCurTask->updateCallback = 0;
    TaskStop();
    t = gCurTask;
    t->pixelX += t->facing * 16;
    t->posX = t->pixelX << 16;
    t->frame = 1;
    TaskYieldTrampoline(2);
    ActorDestroy();
    TaskSleepForever();
}

void LaserBallLaserState0Update(void)
{
}

void Task_ShotzoCannonball(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gShotzoCannonballFrames;
    CallTableEntry(u->variant, 4, gShotzoCannonballVariants);
}

void ShotzoCannonballInit(void)
{
    gCurTask->updateCallback = (u32)ShotzoCannonballUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gShotzoCannonballStates);
}

void ShotzoCannonballUpdate(void)
{
    if (sub_08069660() == 0)
        CallTableEntry(gCurTask->updateState, 1, gShotzoCannonballStateUpdates);
    else
        TaskSetEntry(ActorDie, gCurTaskIdx);
    ActorCheckHits();
    ActorReactToHit();
}

void ShotzoCannonballEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gShotzoCannonballStates);
}

void ShotzoCannonballState0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    t = gCurTask;
    t->shotzoCannonballLifeTimer = gUnk_08743614[t->actorSpawnArg];
    switch (t->shotzoCannonballDir = (&gTasks[t->parent])->shotzoBarrelDir)
    {
    case 0:
        AngleToVector(0, gUnk_0874361A[gCurTask->actorSpawnArg]);
        break;
    case 1:
        AngleToVector(224 << 1, gUnk_0874361A[gCurTask->actorSpawnArg]);
        break;
    case 2:
        AngleToVector(192 << 1, gUnk_0874361A[gCurTask->actorSpawnArg]);
        break;
    case 3:
        AngleToVector(160 << 1, gUnk_0874361A[gCurTask->actorSpawnArg]);
        break;
    case 4:
        AngleToVector(128 << 1, gUnk_0874361A[gCurTask->actorSpawnArg]);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    u = gCurTask;
    u->velX = gUnk_030023B4;
    u->velY = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(4);
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
    }
}

void ShotzoCannonballState0Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->shotzoCannonballLifeTimer < 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}
