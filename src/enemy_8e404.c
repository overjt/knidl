/* game_code_and_rodata 0x0808E404-0x0808F41C (issue #70, module M24 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808E404 0x0808F41C src/enemy_8e404.c --newpb
 *
 * See src/enemy_8cce8.c for the three-table pattern all of M24 is built on.
 *
 * This batch holds:
 *   * script 2's rows `LaserBallInit` / `LaserBallIdleInit` (bodies `0x087431EC`
 *     (4) and `0x0874320C` (1), guards `0x087431FC` (4) and `0x08743210`),
 *     with `LaserBallApproach` / `LaserBallHover` / `LaserBallShoot` / `LaserBallRetreat`
 *     as the bodies and `LaserBallApproachUpdate` / `LaserBallHoverUpdate` / `LaserBallShootUpdate` /
 *     `LaserBallRetreatUpdate` as their guards;
 *   * script 3: entry `Task_Coconut` (Task.variant -> `0x08743224`, 3 rows),
 *     rows `CoconutInit` / `CoconutIdleInit`, bodies `0x08743230` (3) and
 *     `0x08743240`, guards `0x0874323C` and `0x08743244`;
 *   * the class-3 hook row `0x087434FC` — `ShotzoLand`, `ShotzoStartFall`,
 *     `ShotzoEnterWater` and `ShotzoHitWall`, every one returning s32;
 *   * the module's shared aiming library: `ShotzoTargetNearestPlayer` classifies the
 *     direction to the target into 16 sectors ((u16)ArcTan2 >> 12) and stores
 *     it in Task.unk30 with the parity in Task.unk20, `ShotzoStepBarrel` turns
 *     Task.unk34 one notch towards it, `ShotzoTestBarrelOnTarget` reports arrival in
 *     Task.unk1C, `ShotzoSetRecoilVelocity` converts the heading into an aim angle for
 *     AngleToVector, `CreateShotzoCannonball` / `CreateShotzoFixedCannonball` fire actor 109 through
 *     CreateActorFromDescAtOffsetFacing + CreateChildTaskAtOffsetFacing, and `ShotzoAimBarrel` / `ShotzoCheckShoot` are the
 *     per-frame drivers;
 *   * script 4's entry `Task_Shotzo` (Task.variant -> `0x08743284`, 6 rows) and
 *     its three <body, guard> table pairs `0x0874329C`/`0x087432A8`,
 *     `0x087432B4`/`0x087432C0` and `0x087432CC`/`0x087432D8`.  Its bodies
 *     continue in src/enemy_8f41c.c.
 *
 * `sub_0808ed0c` is a dead export: a byte-for-byte twin of `ParasolShotzoReactToDefeat`
 * that no ROM word points at (lesson 4.30, curated in tools/symdb.py).
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
extern void TaskSetEntry(void *a, u32 i);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void AngleToVector(s32 a, s32 b);
extern void sub_08066b34(u32 *p);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069888(void);
extern u32 ActorReactToHit(void);

void LaserBallInit(void)
{
    gCurTask->onGround = 0;
    gCurTask->updateCallback = (u32)LaserBallUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 4, gLaserBallStates);
}

void LaserBallUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gLaserBallStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void LaserBallEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gLaserBallStates);
}

void LaserBallApproach(void)
{
    gCurTask->updateState = 0;
    gCurTask->laserBallSteerTimer = 16;
    TaskStop();
    while (1)
    {
        switch (gCurTask->laserBallPlayerSide = TaskGetXDirBitToNearestPlayer())
        {
        case 8:
            if (gCurTask->laserBallTurnPending != 0)
            {
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->laserBallTurnPending = 0;
            }
            TaskSetFrameFlip(4);
            TaskYieldTrampoline(8);
            break;
        case 4:
            if (gCurTask->laserBallTurnPending != 0)
            {
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->laserBallTurnPending = 0;
            }
            TaskSetFrameNoFlip(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void LaserBallApproachUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->laserBallPlayerSide != TaskGetXDirBitToNearestPlayer())
        gCurTask->laserBallTurnPending = 1;
    LaserBallSetTargetX();
    LaserBallCheckShoot();
    u = gCurTask;
    u->laserBallSteerTimer--;
    LaserBallReaim();
    LaserBallAccelerateInMoveDir();
}

void LaserBallHover(void)
{
    gCurTask->updateState = 2;
    gCurTask->laserBallHoverTimer = 0;
    TaskStop();
    while (1)
    {
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->velY = 0xFFFFF000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFFF800;
        TaskYieldTrampoline(16);
        gCurTask->velY = 128 << 4;
        TaskYieldTrampoline(8);
        gCurTask->velY = 128 << 5;
        TaskYieldTrampoline(8);
        gCurTask->velY = 128 << 4;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0xFFFFF800;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0xFFFFF000;
        TaskYieldTrampoline(8);
    }
}

void LaserBallHoverUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    t->laserBallHoverTimer++;
    t->pixelX = t->laserBallTargetX;
    if (t->laserBallHoverTimer > 5 && RandomRange(2) != 0)
    {
        ActorSetState(1);
        TaskSetEntry(LaserBallEnterState, gCurTaskIdx);
    }
}

void LaserBallShoot(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    gCurTask->laserBallShotDone = 0;
    TaskStop();
    t = gCurTask;
    switch (t->facing)
    {
    case 1:
        t->laserBallLaserDir = 0;
        break;
    case -1:
        t->laserBallLaserDir = 1;
        break;
    }
    gCurTask->laserBallWindUpCount = 0;
    do
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->laserBallWindUpCount++;
    } while ((s16)u->laserBallWindUpCount <= 7);
    gCurTask->laserBallShotCount = 0;
    while ((s16)gCurTask->laserBallShotCount < RandomRange(3) + 1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        CreateLaserBallLaser();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        w = gCurTask;
        w->laserBallShotCount++;
    }
    gCurTask->laserBallShotDone = 1;
    TaskSleepForever();
}

void LaserBallShootUpdate(void)
{
    if (gCurTask->laserBallShotDone != 0)
    {
        ActorSetState(3);
        TaskSetEntry(LaserBallEnterState, gCurTaskIdx);
    }
}

void LaserBallRetreat(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = 3;
    t = gCurTask;
    t->laserBallTurnPending = 1;
    t->accelY = 0xFFFFF000;
    while (1)
    {
        switch (TaskGetXDirBitToNearestPlayer())
        {
        case 4:
            AngleToVector(160 << 1, 102);
            u = gCurTask;
            u->velX = gUnk_030023B4;
            if (u->laserBallTurnPending != 0)
            {
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->laserBallTurnPending = 0;
            }
            TaskSetFrameFlip(4);
            TaskYieldTrampoline(8);
            break;
        case 8:
            AngleToVector(224 << 1, 102);
            v = gCurTask;
            v->velX = gUnk_030023B4;
            if (v->laserBallTurnPending != 0)
            {
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->laserBallTurnPending = 0;
            }
            TaskSetFrameNoFlip(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void LaserBallRetreatUpdate(void)
{
}

void LaserBallIdleInit(void)
{
    gCurTask->updateCallback = (u32)LaserBallIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gLaserBallIdleStates);
}

void LaserBallIdleUpdate(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 1, gLaserBallIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void LaserBallIdle(void)
{
    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    gCurTask->onGround = 1;
    TaskStop();
    TaskSetFrame(4);
    TaskSleepForever();
}

void LaserBallIdleState0Update(void)
{
}

s32 sub_0808e8a4(void)
{
    ActorSetState(2);
    TaskSetEntry(CoconutEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808e8c4(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Coconut(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gCoconutFrames;
    CallTableEntry(u->variant, 3, gCoconutVariants);
}

void CoconutInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)CoconutUpdate;
    switch (t->variant)
    {
    case 0:
        t->facing = 1;
        break;
    case 1:
        t->facing = 255;
        break;
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gCoconutStates);
}

void CoconutUpdate(void)
{
    if (sub_08069888() == 0)
        CallTableEntry(gCurTask->updateState, 1, gCoconutStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void CoconutEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gCoconutStates);
}

void CoconutWait(void)
{
    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetFrame(5);
    TaskSleepForever();
}

void CoconutWaitUpdate(void)
{
    s32 v;

    v = TaskGetNearestPlayerDx();
    if (abs(v) <= 7)
    {
        ActorSetState(1);
        TaskSetEntry(CoconutEnterState, gCurTaskIdx);
    }
}

void CoconutFall(void)
{
    struct Task *t;

    t = gCurTask;
    t->accelY = gUnk_08743214[t->actorSpawnArg];
    t->speedLimitY = gUnk_0874321C[t->actorSpawnArg];
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
    }
}

void CoconutExplode(void)
{
    ActorSetHitReactions(gUnk_08743558);
    ActorDie();
}

void CoconutIdleInit(void)
{
    gCurTask->updateCallback = (u32)CoconutIdleUpdate;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gCoconutIdleStates);
}

void CoconutIdleUpdate(void)
{
    if (sub_08069888() == 0)
        CallTableEntry(gCurTask->updateState, 1, gCoconutIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void CoconutIdle(void)
{
    gCurTask->updateState = 0;
    gCurTask->onGround = 1;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void CoconutIdleState0Update(void)
{
}

s32 ShotzoStartFall(void)
{
    switch (gCurTask->variant)
    {
    default:
        ActorSetState(1);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
        return 1;
    case 0:
        ActorSetState(1);
        TaskSetEntry(ShotzoAimEnterState, gCurTaskIdx);
        return 1;
    case 4:
        ActorSetState(2);
        TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 ShotzoLand(void)
{
    switch (gCurTask->variant)
    {
    default:
        ActorSetState(0);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
        return 1;
    case 0:
        ActorSetState(0);
        TaskSetEntry(ShotzoAimEnterState, gCurTaskIdx);
        return 1;
    case 4:
        sub_08066c3c(gShotzoDef);
        ActorSetState(0);
        TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 ShotzoHitWall(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant == 4 && t->state == 2)
        sub_08066b70();
    return 0;
}

s32 ShotzoEnterWater(void)
{
    if (gCurTask->variant == 4)
        sub_08066c08(gShotzoDef, 0);
    ActorStartDrown(-2);
    return 1;
}

s32 ParasolShotzoReactToDefeat(void)
{
    sub_08066c08(gShotzoDef, 0);
    ActorSetState(2);
    TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808ed0c(void)
{
    sub_08066c08(gShotzoDef, 0);
    ActorSetState(2);
    TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
    return 1;
}

void ShotzoTargetNearestPlayer(void)
{
    s32 dx;
    s32 dy;

    dx = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->pixelX - (u16)gCurTask->pixelX);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->pixelY - (u16)gCurTask->pixelY);
    switch (gCurTask->shotzoTargetBarrelDir = (u16)ArcTan2(dx, dy) >> 12)
    {
    case 0:
    case 15:
        gCurTask->shotzoTargetBarrelDir = 0;
        gCurTask->shotzoTargetInArc = 1;
        break;
    case 1:
    case 2:
        gCurTask->shotzoTargetBarrelDir = 0;
        gCurTask->shotzoTargetInArc = 0;
        break;
    case 3:
    case 4:
        gCurTask->shotzoTargetBarrelDir = 2;
        gCurTask->shotzoTargetInArc = 0;
        break;
    case 5:
    case 6:
        gCurTask->shotzoTargetBarrelDir = 4;
        gCurTask->shotzoTargetInArc = 0;
        break;
    case 7:
    case 8:
        gCurTask->shotzoTargetBarrelDir = 4;
        gCurTask->shotzoTargetInArc = 1;
        break;
    case 9:
    case 10:
        gCurTask->shotzoTargetBarrelDir = 3;
        gCurTask->shotzoTargetInArc = 1;
        break;
    case 11:
    case 12:
        gCurTask->shotzoTargetBarrelDir = 2;
        gCurTask->shotzoTargetInArc = 1;
        break;
    case 13:
    case 14:
        gCurTask->shotzoTargetBarrelDir = 1;
        gCurTask->shotzoTargetInArc = 1;
        break;
    }
}

void ShotzoStepBarrel(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->shotzoBarrelDir > t->shotzoTargetBarrelDir)
    {
        if (t->shotzoBarrelDir > 0)
        {
            t->shotzoPrevBarrelDir = t->shotzoBarrelDir;
            t->shotzoBarrelDir--;
        }
        else
        {
            t->shotzoPrevBarrelDir = t->shotzoBarrelDir;
        }
    }
    else if (t->shotzoBarrelDir < t->shotzoTargetBarrelDir)
    {
        if (t->shotzoBarrelDir <= 3)
        {
            t->shotzoPrevBarrelDir = t->shotzoBarrelDir;
            t->shotzoBarrelDir++;
        }
        else
        {
            t->shotzoPrevBarrelDir = t->shotzoBarrelDir;
        }
    }
    else if (t->shotzoBarrelDir == t->shotzoTargetBarrelDir)
    {
        t->shotzoPrevBarrelDir = t->shotzoBarrelDir;
        t->shotzoBarrelOnTarget = 1;
    }
}

void ShotzoTestBarrelOnTarget(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->shotzoBarrelDir > t->shotzoTargetBarrelDir)
        t->shotzoBarrelOnTarget = 0;
    else if (t->shotzoBarrelDir < t->shotzoTargetBarrelDir)
        t->shotzoBarrelOnTarget = 0;
    else if (t->shotzoBarrelDir == t->shotzoTargetBarrelDir)
        t->shotzoBarrelOnTarget = 1;
}

void ShotzoSetRecoilVelocity(s32 a)
{
    struct Task *t;
    u16 b;

    b = a;

    switch (gCurTask->variant)
    {
    case 0:
    case 4:
        switch (gCurTask->shotzoBarrelDir)
        {
        case 0:
            AngleToVector(0, (s16)b);
            break;
        case 1:
            AngleToVector(224 << 1, (s16)b);
            break;
        case 2:
            AngleToVector(192 << 1, (s16)b);
            break;
        case 3:
            AngleToVector(160 << 1, (s16)b);
            break;
        case 4:
            AngleToVector(128 << 1, (s16)b);
            break;
        }
        break;
    case 1:
        AngleToVector(192 << 1, (s16)b);
        break;
    case 2:
        AngleToVector(224 << 1, (s16)b);
        break;
    case 3:
        AngleToVector(160 << 1, (s16)b);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    t = gCurTask;
    t->velX = gUnk_030023B4;
    t->velY = gUnk_030023D4;
}

void ShotzoInitBarrel(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    switch (TaskGetXDirBitToNearestPlayer())
    {
    default:
        t = gCurTask;
        t->shotzoTargetBarrelDir = 0;
        t->shotzoBarrelDir = 0;
        break;
    case 4:
        u = gCurTask;
        u->shotzoTargetBarrelDir = 0;
        u->shotzoBarrelDir = 0;
        TaskSetFrameNoFlip(4);
        break;
    case 8:
        v = gCurTask;
        v->shotzoTargetBarrelDir = 4;
        v->shotzoBarrelDir = 4;
        TaskSetFrameFlip(4);
        break;
    }
}

void CreateShotzoCannonball(void)
{
    struct ActorSpawn sp;

    sp.subtype = 7;
    sp.taskType = 109;
    sp.variant = 0;
    sp.spawnArg = gCurTask->shotzoSpeedLevel;
    sp.x = gUnk_0874324C[gCurTask->shotzoTargetBarrelDir];
    sp.y = gUnk_08743251[gCurTask->shotzoTargetBarrelDir];
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    gCurTask->shotzoSmokeRingSlot = CreateChildTaskAtOffsetFacing(172, gUnk_0874324C[gCurTask->shotzoTargetBarrelDir], gUnk_08743251[gCurTask->shotzoTargetBarrelDir], 0);
}

void CreateShotzoFixedCannonball(void)
{
    struct ActorSpawn sp;

    sp.subtype = 7;
    sp.taskType = 109;
    sp.variant = 0;
    sp.spawnArg = 4;
    sp.x = gUnk_08743256[gCurTask->shotzoBarrelDir];
    sp.y = gUnk_08743251[gCurTask->shotzoBarrelDir];
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    gCurTask->shotzoSmokeRingSlot = CreateChildTaskAtOffsetFacing(172, gUnk_08743256[gCurTask->shotzoBarrelDir], gUnk_08743251[gCurTask->shotzoBarrelDir], 0);
}

void ShotzoAimBarrel(void)
{
    ShotzoTargetNearestPlayer();
    ShotzoStepBarrel();
    switch (gCurTask->shotzoBarrelDir)
    {
    case 0:
        TaskSetFrameNoFlip(4);
        break;
    case 1:
        if (gCurTask->shotzoPrevBarrelDir == 2)
        {
            TaskSetFrameNoFlip(6);
            TaskYieldTrampoline(4);
        }
        TaskSetFrameNoFlip(5);
        break;
    case 2:
        switch (gCurTask->shotzoPrevBarrelDir)
        {
        case 1:
            TaskSetFrameNoFlip(6);
            TaskYieldTrampoline(4);
            break;
        case 3:
            TaskSetFrameFlip(6);
            TaskYieldTrampoline(4);
            break;
        }
        TaskSetFrameNoFlip(7);
        break;
    case 3:
        if (gCurTask->shotzoPrevBarrelDir == 2)
        {
            TaskSetFrameFlip(6);
            TaskYieldTrampoline(4);
        }
        TaskSetFrameFlip(5);
        break;
    case 4:
        TaskSetFrameFlip(4);
        break;
    }
    TaskYieldTrampoline(gUnk_08743248[gCurTask->shotzoSpeedLevel]);
}

void ShotzoCheckShoot(u16 a, void *b)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->shotzoAimTimer <= 0)
    {
        t->shotzoAimTimer = gUnk_08743248[t->shotzoSpeedLevel];
        ShotzoTargetNearestPlayer();
        ShotzoTestBarrelOnTarget();
        u = gCurTask;
        if (u->shotzoBarrelOnTarget != 0)
        {
            u->shotzoBarrelOnTarget = 0;
            if (u->shotzoArmed != 0)
            {
                if (u->shotzoTargetInArc != 0)
                {
                    ActorSetState(a);
                    TaskSetEntry(b, gCurTaskIdx);
                }
            }
            else
            {
                u->shotzoArmed = 1;
            }
        }
    }
}

void Task_Shotzo(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gShotzoFrames;
    u->u8C.actor->extraFrame = 4;
    CallTableEntry(u->variant, 6, gShotzoVariants);
}

void ShotzoAimInit(void)
{
    gCurTask->updateCallback = (u32)ShotzoAimUpdate;
    ActorSetState(0);
    ShotzoInitBarrel();
    CallTableEntry(gCurTask->state, 3, gShotzoAimStates);
}

void ShotzoAimUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gShotzoAimStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ShotzoFixedInit(void)
{
    gCurTask->updateCallback = (u32)ShotzoFixedUpdate;
    ActorSetState(0);
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 3, gShotzoFixedStates);
}

void ShotzoFixedUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gShotzoFixedStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ParasolShotzoInit(void)
{
    gCurTask->updateCallback = (u32)ParasolShotzoUpdate;
    ActorSetState(0);
    ShotzoInitBarrel();
    sub_08066b34(gParasolShotzoDef);
    CallTableEntry(gCurTask->state, 3, gParasolShotzoStates);
}

void ParasolShotzoUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gParasolShotzoStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ShotzoAimEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gShotzoAimStates);
}

void ShotzoFixedEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gShotzoFixedStates);
}

void ParasolShotzoEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gParasolShotzoStates);
}

void ShotzoAim(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->shotzoAimTimer = gUnk_08743248[t->shotzoSpeedLevel];
    TaskStopY();
    while (1)
        ShotzoAimBarrel();
}

void ShotzoAimState0Update(void)
{
    if (sub_08069888() == 0)
        ShotzoCheckShoot(2, ShotzoAimEnterState);
}
