/* game_code_and_rodata 0x0807D3B0-0x0807F044 (issue #77, module M20 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807D3B0 0x0807F044 src/enemy_rocky.c --newpb
 *
 * The last third of enemy/object behaviour bank 1 (see src/enemy_sparky.c for
 * the bank's three-table script pattern).  Enemies (Task_Rocky,
 * Task_SirKibble, Task_Cappy, Task_Gordo, Task_CoolSpook and the head of
 * Task_Kabu) and their helper tasks (#155 run 2; an older reading called them
 * moving scenery): scripts that drive Task.velX/velY and Task.accelY from ROM
 * tables and wait on Task.onGround:
 *   * task types #216 and #217 (`Task_PengyIceBreathPuff`, `Task_PengyIceBreathSparkle`, the latter a
 *     seventeen-step cue script over `0x087410C0`), the effect tasks Pengy's
 *     breath state PengyShoot spawns with CreateChildTaskAtOffsetFacing;
 *   * Task_Rocky's (#9) row 0 `RockyWalkInit`+`RockyWalkUpdate`, whose per-frame
 *     handlers `RockyWalkState0Update` / `RockyWalkState1Update` re-centre on the nearest
 *     player when `|TaskGetNearestPlayerDx()| <= 49` and `|TaskGetDyTo()| <= 15`;
 *   * the class-3 three-way branch pair `RockyLand` / `RockyStartFall`
 *     (`switch (Task.variant)` with an empty `case 1`);
 *   * Task_SirKibble's `0x08741220` function-pointer table the three
 *     `SirKibbleShootUpdate` / `SirKibbleJumpUpdate` hooks dispatch through;
 *   * Sir Kibble's jump-and-throw state `SirKibbleJump` (rows 0/1, state 2)
 *     and the capless Cappy's hop `CappyCaplessHop` (row 1, state 0), which
 *     spawn a companion with `CreateActorFromDescAtOffsetFacing` and then
 *     bounce between velocity presets until Task.onGround fires;
 *   * Gordo's four movement states (`GordoBob`, `GordoBounceVertical`,
 *     `GordoBounceHorizontal`, `GordoSweep`) and Cool Spook's float `sub_0807ef7c`,
 *     each an infinite eight-step velocity ramp.
 *
 * `RockyIdleEnterState`, `SirKibbleIdleEnterState`, `CappyCappedEnterState`, `CappyStandEnterState`,
 * `CoolSpookFlyEnterState` and `CoolSpookBobEnterState` are dead exports: each is a copy of its
 * host's tail dispatch that nothing in the ROM references (curated in
 * tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 TaskGetDxTo(s32 i);
extern s32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern u32 ActorCheckHitsWithBox(void *p);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *fn, u32 i);
extern void RequestScreenShake(s32 a);
extern void ActorLoadDef(u32 *def);
extern void ActorSetState(u32 v);
extern void ActorSetAttackBox(u32 *p);

void SwordAndBladeKnightSlashUpdate(void)
{
    s32 v;

    if (gTaskSlotTypes[gCurTask->parent] == -1)
        goto kill1;
    v = (u8)TaskHasSameSerial(gCurTask->parent);
    if (v != 1)
        goto kill1;
    {
        struct Task *t = gCurTask;
        struct Task *o = &gTasks[t->parent];
        s16 *s;

        if ((u16)(o->u76.subtype - 28) > 1)
            goto kill2;
        s = &o->pixelX;
        t->pixelX = o->swordAndBladeKnightLungeDir * 20 + *s;
        t->pixelY = o->pixelY;
        if (t->swordAndBladeKnightSlashStruck != 0)
        {
            ActorCheckHitsWithBox(gUnk_08741174);
            return;
        }
    }
    if (ActorCheckHitsWithBox(gUnk_08741158) != 0)
    {
        if (gCurTask->hitKind == HIT_KIND_NO_DAMAGE)
        {
            PlaySfx(243);
            gCurTask->swordAndBladeKnightSlashStruck = v;
        }
    }
    return;

kill2:
    TaskFree(gCurTaskIdx);
    return;

kill1:
    TaskFree(gCurTaskIdx);
}

void Task_PengyIceBreathPuff(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gPengyIceBreathFrames;
    t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0x30000, -0x5000);
    {
        struct Task *u = gCurTask;

        u->accelY = -0x4000;
        u->frame = 2;
    }
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}

void Task_PengyIceBreathSparkle(void)
{
    u32 a;
    u32 b;
    s32 i;
    s32 j;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    {
        struct Task *t = gCurTask;

        t->frameTable = gPengyIceBreathFrames;
        t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    }
    TaskFaceLikeParent();
    i = gCurTask->pengyIceBreathSparkleIndex;
    j = i * 2;
    a = gUnk_087410C0[j];
    b = gUnk_087410C0[j + 1];
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = -1;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = gUnk_087410D8[t->pengyIceBreathSparkleIndex];
        t->frame--;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void Task_Rocky(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gRockyFrames;
    CallTableEntry(gCurTask->variant, 3, gRockyVariants);
}

void RockyWalkInit(void)
{
    gCurTask->updateCallback = (u32)RockyWalkUpdate;
    TaskFaceNearestPlayer();
    gCurTask->rockyLeapTimer = 0;
    ActorSetState(ROCKY_WALK_STATE_WALK);
    CallTableEntry(gCurTask->state, 5, gRockyWalkStates);
}

void RockyWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 5, gRockyWalkStates);
}

void RockyWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gRockyWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void RockyWalk(void)
{
    gCurTask->updateState = ROCKY_WALK_STATE_WALK;
    gCurTask->rockyLoopCount = 0;
    do
    {
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x5800, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->frame++;
        }
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x4800, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->frame++;
        }
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->rockyLoopCount <= 1);
    gCurTask->velX = 0;
    ActorSetState(ROCKY_WALK_STATE_1);
    TaskSleepForever();
}

void RockyWalkState0Update(void)
{
    gCurTask->rockyPlayerSlot = TaskFindNearestPlayer();
    if (abs(TaskGetDxTo(gCurTask->rockyPlayerSlot)) <= 49)
    {
        if (abs(TaskGetDyTo(gCurTask->rockyPlayerSlot)) <= 15)
        {
            struct Task *t = gCurTask;
            s32 n = --t->rockyLeapTimer;

            if (n <= 0)
            {
                t->unk30 = n < 0 ? 4 : 3;
                if (RandomRange(gCurTask->unk30) == 0)
                {
                    TaskFaceToward(gCurTask->rockyPlayerSlot);
                    ActorSetState(ROCKY_WALK_STATE_JUMP);
                }
                gCurTask->rockyLeapTimer = 60;
            }
        }
    }
    if (gCurTask->state != ROCKY_WALK_STATE_WALK)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void RockyWalkState1(void)
{
    gCurTask->updateState = ROCKY_WALK_STATE_1;
    sub_0807dd10();
    ActorSetState(ROCKY_WALK_STATE_WALK);
    TaskSleepForever();
}

void RockyWalkState1Update(void)
{
    gCurTask->rockyPlayerSlot = TaskFindNearestPlayer();
    if (abs(TaskGetDxTo(gCurTask->rockyPlayerSlot)) <= 49)
    {
        if (abs(TaskGetDyTo(gCurTask->rockyPlayerSlot)) <= 15)
            TaskFaceToward(gCurTask->rockyPlayerSlot);
    }
    if (gCurTask->state != ROCKY_WALK_STATE_1)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void RockyWalkJump(void)
{
    gCurTask->onGround = 0;
    gCurTask->updateState = ROCKY_WALK_STATE_JUMP;
    TaskSetMotionXFacing(0x18000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x2E800;
        t->accelY = 0x2000;
    }
    TaskSetFrame(14);
    TaskYieldTrampoline(24);
    TaskSetFrame(12);
    TaskStop();
    TaskYieldTrampoline(16);
    ActorSetState(ROCKY_WALK_STATE_FALL);
    TaskSleepForever();
}

void RockyWalkJumpUpdate(void)
{
    if (gCurTask->state != ROCKY_WALK_STATE_JUMP)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void RockyWalkState3(void)
{
    gCurTask->updateState = ROCKY_WALK_STATE_3;
    TaskSetFrame(12);
    TaskStop();
    TaskYieldTrampoline(16);
    ActorSetState(ROCKY_WALK_STATE_FALL);
    TaskSleepForever();
}

void RockyWalkState3Update(void)
{
    if (gCurTask->state != ROCKY_WALK_STATE_3)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void RockyWalkFall(void)
{
    gCurTask->updateState = ROCKY_WALK_STATE_FALL;
    gCurTask->velY = 0x80000;
    TaskSetFrame(12);
    TaskSleepForever();
}

void RockyWalkFallUpdate(void)
{
}

void RockyIdleInit(void)
{
    gCurTask->updateCallback = (u32)RockyIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(ROCKY_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gRockyIdleStates);
}

void RockyIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gRockyIdleStates);
}

void RockyIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gRockyIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void RockyIdle(void)
{
    gCurTask->updateState = ROCKY_IDLE_STATE_IDLE;
    while (1)
        sub_0807dd10();
}

void RockyIdleState0Update(void)
{
}

void RockyStandInit(void)
{
    gCurTask->updateCallback = (u32)RockyStandUpdate;
    TaskFaceNearestPlayer();
    gCurTask->rockyLeapTimer = 0;
    ActorSetState(ROCKY_STAND_STATE_0);
    CallTableEntry(gCurTask->state, 3, gRockyStandStates);
}

void RockyStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gRockyStandStates);
}

void RockyStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gRockyStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void RockyStandState0(void)
{
    gCurTask->updateState = ROCKY_STAND_STATE_0;
    {
        struct Task *t = gCurTask;

        t->velX = 0;
        t->rockyLoopCount = 0;
    }
    do
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->rockyLoopCount <= 1);
    ActorSetState(ROCKY_STAND_STATE_1);
    TaskSleepForever();
}

void RockyStandState0Update(void)
{
    if (gCurTask->state != ROCKY_STAND_STATE_0)
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
}

void RockyStandState1(void)
{
    gCurTask->updateState = ROCKY_STAND_STATE_1;
    sub_0807dd10();
    ActorSetState(ROCKY_STAND_STATE_0);
    TaskSleepForever();
}

void RockyStandState1Update(void)
{
    if (gCurTask->state != ROCKY_STAND_STATE_1)
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
}

void RockyStandFall(void)
{
    gCurTask->updateState = ROCKY_STAND_STATE_FALL;
    gCurTask->velY = 0x80000;
    TaskSetFrame(12);
    TaskSleepForever();
}

void RockyStandFallUpdate(void)
{
}

void sub_0807dd10(void)
{
    TaskSetFrame(12);
    TaskYieldTrampoline(100);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame--;
    TaskYieldTrampoline(60);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame--;
    TaskYieldTrampoline(18);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
}

s32 RockyLand(void)
{
    s32 r;

    {
        struct Task *t = gCurTask;

        t->velX = 0;
        t->velY = 0;
    }
    RequestScreenShake(1);
    PlaySfx(163);
    r = 0;
    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(1);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(1);
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 RockyStartFall(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(4);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 RockyHitCeiling(void)
{
    s32 r = 0;

    if (gCurTask->variant == 0)
    {
        ActorSetState(ROCKY_WALK_STATE_3);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 RockyHitWall(void)
{
    struct Task *t = gCurTask;

    if (t->state == 2)
        t->velX = 0;
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 RockyEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_SirKibble(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSirKibbleFrames;
    TaskFaceNearestPlayer();
    {
        struct Task *t = gCurTask;

        t->unk2C = 0;
        CallTableEntry(t->variant, 3, gSirKibbleVariants);
    }
}

void SirKibbleStandInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleStandUpdate;
    ActorSetState(SIR_KIBBLE_STAND_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gSirKibbleStandStates);
}

void SirKibbleStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gSirKibbleStandStates);
}

void SirKibbleStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gSirKibbleStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SirKibbleWait(void)
{
    struct Task *t;

    gCurTask->updateState = SIR_KIBBLE_STAND_STATE_WAIT;
    t = gCurTask;
    t->sirKibbleStateTimer = gSirKibbleStateTimes[t->actorSpawnArg];
    sub_0807e484();
}

void SirKibbleWaitUpdate(void)
{
    struct Task *t = gCurTask;

    if (--t->sirKibbleStateTimer == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (RandomRange(gSirKibbleJumpOdds[gCurTask->actorSpawnArg]) == 0)
                ActorSetState(SIR_KIBBLE_STAND_STATE_JUMP);
            else
                ActorSetState(SIR_KIBBLE_STAND_STATE_SHOOT);
        }
        else
        {
            ActorSetState(SIR_KIBBLE_STAND_STATE_SHOOT);
        }
        gCurTask->velX = 0;
        TaskSetEntry(SirKibbleStandEnterState, gCurTaskIdx);
    }
}

void SirKibbleWalkInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleWalkUpdate;
    ActorSetState(SIR_KIBBLE_WALK_STATE_WALK);
    CallTableEntry(gCurTask->state, 3, gSirKibbleWalkStates);
}

void SirKibbleWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gSirKibbleWalkStates);
}

void SirKibbleWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gSirKibbleWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SirKibbleWalk(void)
{
    struct Task *t;

    gCurTask->updateState = SIR_KIBBLE_WALK_STATE_WALK;
    t = gCurTask;
    t->sirKibbleStateTimer = gSirKibbleStateTimes[t->actorSpawnArg];
    TaskSetMotionXFacing(gUnk_08741218[t->actorSpawnArg], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(12);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        TaskSetFrame(8);
        TaskYieldTrampoline(12);
        TaskSetFrame(6);
        TaskYieldTrampoline(12);
    }
}

void SirKibbleWalkState0Update(void)
{
    struct Task *t = gCurTask;

    if (--t->sirKibbleStateTimer == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (RandomRange(gSirKibbleJumpOdds[gCurTask->actorSpawnArg]) == 0)
                ActorSetState(SIR_KIBBLE_WALK_STATE_JUMP);
            else
                ActorSetState(SIR_KIBBLE_WALK_STATE_SHOOT);
        }
        else
        {
            ActorSetState(SIR_KIBBLE_WALK_STATE_SHOOT);
        }
        gCurTask->velX = 0;
        TaskSetEntry(SirKibbleWalkEnterState, gCurTaskIdx);
    }
}

void SirKibbleShoot(void)
{
    struct ActorSpawn spawn;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(9);
    TaskYieldTrampoline(40);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    spawn.subtype = 3;
    spawn.taskType = TASK_SIR_KIBBLE_CUTTER;
    spawn.variant = SIR_KIBBLE_CUTTER_VARIANT_INIT;
    spawn.spawnArg = 0;
    spawn.x = 16;
    spawn.y = 0;
    spawn.checkTerrain = 0;
    {
        struct Task *t;
        s32 id = CreateActorFromDescAtOffsetFacing(&spawn, 0);

        t = gCurTask;
        t->sirKibbleCutterSlot = id;
        t->sirKibbleStateTimer = 88;
    }
    do
        TaskYieldTrampoline(1);
    while (gCurTask->sirKibbleStateTimer != 0);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(12);
    ActorSetState(0);
    TaskSleepForever();
}

void SirKibbleShootUpdate(void)
{
    {
        struct Task *t = gCurTask;

        if (t->onGround != 0 && t->sirKibbleStateTimer > 0)
            t->sirKibbleStateTimer--;
    }
    {
        struct Task *t = gCurTask;

        if (t->state != 1)
            TaskSetEntry((void *)gSirKibbleEnterStates[t->variant], gCurTaskIdx);
    }
}

void SirKibbleJump(void)
{
    struct ActorSpawn spawn;

    gCurTask->updateState = 2;
    {
        struct Task *t;
        s32 r = TaskGetFacingTowardNearestPlayer();

        t = gCurTask;
        t->sirKibbleSavedFacing = r;
        if (r == 1)
            t->facing = 255;
        else
            t->facing = 1;
    }
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(9);
    TaskYieldTrampoline(40);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    spawn.subtype = 3;
    spawn.taskType = TASK_SIR_KIBBLE_CUTTER;
    spawn.variant = SIR_KIBBLE_CUTTER_VARIANT_INIT;
    spawn.spawnArg = 1;
    spawn.x = 16;
    spawn.y = 0;
    spawn.checkTerrain = 0;
    gCurTask->sirKibbleCutterSlot = CreateActorFromDescAtOffsetFacing(&spawn, 0);
    TaskYieldTrampoline(30);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;

        if (t->onGround != 0)
        {
            struct Task *u;

            t->onGround = 0;
            u = gCurTask;
            u->velY = -0x40000;
            u->accelY = 0x4000;
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
        }
    }
    {
        struct Task *t = gCurTask;

        t->velY = 0;
        t->accelY = 0;
        t->facing = t->sirKibbleSavedFacing;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void SirKibbleJumpUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 2)
        TaskSetEntry((void *)gSirKibbleEnterStates[t->variant], gCurTaskIdx);
}

void SirKibbleIdleInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(SIR_KIBBLE_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gSirKibbleIdleStates);
}

void SirKibbleIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gSirKibbleIdleStates);
}

void SirKibbleIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gSirKibbleIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SirKibbleIdle(void)
{
    gCurTask->updateState = SIR_KIBBLE_IDLE_STATE_IDLE;
    sub_0807e484();
}

void SirKibbleIdleState0Update(void)
{
}

void sub_0807e484(void)
{
    TaskSetFrame(5);
    while (1)
    {
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(16);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    }
}

s32 sub_0807e4cc(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    TaskSetFrame((s16)gCurTask->frame);
    t = gCurTask;
    TaskSetMotionX(-t->velX, -t->accelX, t->speedLimitX);
    return 0;
}

s32 SirKibbleStartFall(void)
{
    TaskSetMotionY(0, 0x2500, 0x30000);
    return 0;
}

s32 SirKibbleLand(void)
{
    TaskStopY();
    return 0;
}

s32 SirKibbleEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Cappy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    CallTableEntry(gCurTask->variant, 3, gCappyVariants);
}

void CappyCappedInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CappyCappedUpdate;
    t->frameTable = gCappyFrames;
    ActorSetState(CAPPY_CAPPED_STATE_HOP);
    CallTableEntry(gCurTask->state, 1, gCappyCappedStates);
}

void CappyCappedEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gCappyCappedStates);
}

void CappyCappedUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gCappyCappedStateUpdates);
    ActorCheckHits();
    {
        struct Task *t = gCurTask;

        if (t->hitKind == HIT_KIND_INHALE)
        {
            gCurTask->cappyCaplessSlot = CreateActorByKind(ACTOR_KIND_ENEMY, 8, CAPPY_VARIANT_CAPLESS, 0, t->pixelX, t->pixelY,
                                                t->tileWord);
            TransferRoomObject(gCurTaskIdx, gCurTask->cappyCaplessSlot);
        }
    }
    ActorReactToHit();
}

void CappyCappedHop(void)
{
    gCurTask->updateState = CAPPY_CAPPED_STATE_HOP;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->cappyHopCount = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        while (--gCurTask->cappyHopCount >= 0)
        {
            gCurTask->onGround = 0;
            {
                struct Task *u = gCurTask;

                u->velY = -0x10000;
                u->accelY = 0x1000;
            }
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
            gCurTask->spriteFlags ^= SPRITE_FLAG_FLIP_X;
        }
    }
}

void CappyCappedHopUpdate(void)
{
}

void CappyCaplessInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CappyCaplessUpdate;
    t->frameTable = gCappyCaplessFrames;
    ActorLoadDef(gCappyCaplessDef);
    ActorSetState(CAPPY_CAPLESS_STATE_JUMP);
    CallTableEntry(gCurTask->state, 2, gCappyCaplessStates);
}

void CappyCaplessEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gCappyCaplessStates);
}

void CappyCaplessUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gCappyCaplessStateUpdates);
    if (gCurTask->updateState != CAPPY_CAPLESS_STATE_JUMP)
        ActorCheckHits();
    ActorReactToHit();
}

void CappyCaplessHop(void)
{
    gCurTask->updateState = CAPPY_CAPLESS_STATE_HOP;
    TaskSetFrame(4);
    while (1)
    {
        gCurTask->cappyHopCount = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        TaskSetFrame((s16)gCurTask->frame);
        while (--gCurTask->cappyHopCount >= 0)
        {
            gCurTask->onGround = 0;
            {
                struct Task *u = gCurTask;

                u->velY = -0x10000;
                u->accelY = 0x1000;
            }
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
            if ((s16)gCurTask->frame == 6)
                TaskSetFrame(4);
            else
                TaskSetFrame(6);
        }
    }
}

void CappyCaplessHopUpdate(void)
{
}

void CappyCaplessJump(void)
{
    gCurTask->updateState = CAPPY_CAPLESS_STATE_JUMP;
    TaskFaceNearestPlayer();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(-0x8000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x20000;
        t->accelY = 0x2000;
    }
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
}

void CappyCaplessJumpUpdate(void)
{
    if (gCurTask->onGround != 0)
    {
        ActorSetState(CAPPY_CAPLESS_STATE_HOP);
        TaskSetEntry(CappyCaplessEnterState, gCurTaskIdx);
    }
}

void CappyStandInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CappyStandUpdate;
    t->frameTable = gCappyFrames;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(CAPPY_STAND_STATE_HOP);
    CallTableEntry(gCurTask->state, 1, gCappyStandStates);
}

void CappyStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gCappyStandStates);
}

void CappyStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gCappyStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void CappyStandHop(void)
{
    gCurTask->updateState = CAPPY_STAND_STATE_HOP;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->onGround = 0;
        {
            struct Task *t = gCurTask;

            t->velY = -0x10000;
            t->accelY = 0x1000;
        }
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        gCurTask->spriteFlags ^= SPRITE_FLAG_FLIP_X;
    }
}

void CappyStandHopUpdate(void)
{
}

s32 CappyBounceOffWall(void)
{
    struct Task *t = gCurTask;

    t->velX = -t->velX;
    return 0;
}

s32 CappyEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Gordo(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    {
        struct Task *t = gCurTask;

        t->frameTable = gGordoFrames;
        t->onGround = 0;
    }
    TaskFaceNearestPlayer();
    gCurTask->actorAnimDelay = ActorStartAnim(gGordoAnim);
    CallTableEntry(gCurTask->variant, 4, gGordoVariants);
}

void GordoBobInit(void)
{
    gCurTask->updateCallback = (u32)GordoBobUpdate;
    ActorSetState(GORDO_BOB_STATE_BOB);
    CallTableEntry(gCurTask->state, 1, gGordoBobStates);
}

void GordoBobUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gGordoBobStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GordoBob(void)
{
    gCurTask->updateState = GORDO_BOB_STATE_BOB;
    while (1)
    {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
    }
}

void GordoBobState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void GordoBounceVerticalInit(void)
{
    gCurTask->updateCallback = (u32)GordoBounceVerticalUpdate;
    ActorSetState(GORDO_BOUNCE_VERTICAL_STATE_BOUNCE_VERTICAL);
    CallTableEntry(gCurTask->state, 1, gGordoBounceVerticalStates);
}

void GordoBounceVerticalUpdate(void)
{
    if ((u8)ActorCollideTerrainCeilingAndFloor() == 1)
    {
        struct Task *t = gCurTask;

        t->velY = -t->velY;
        t->onGround = 0;
    }
    CallTableEntry(gCurTask->updateState, 1, gGordoBounceVerticalStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GordoBounceVertical(void)
{
    gCurTask->updateState = GORDO_BOUNCE_VERTICAL_STATE_BOUNCE_VERTICAL;
    {
        struct Task *t = gCurTask;

        t->velY = gUnk_08741298[t->actorSpawnArg];
    }
    while (1)
    {
        gCurTask->velX = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void GordoBounceVerticalState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void GordoBounceHorizontalInit(void)
{
    gCurTask->updateCallback = (u32)GordoBounceHorizontalUpdate;
    ActorSetState(GORDO_BOUNCE_HORIZONTAL_STATE_BOUNCE_HORIZONTAL);
    CallTableEntry(gCurTask->state, 1, gGordoBounceHorizontalStates);
}

void GordoBounceHorizontalUpdate(void)
{
    if ((u8)ActorCollideTerrainWalls() == 1)
    {
        struct Task *t = gCurTask;

        t->velX = -t->velX;
    }
    CallTableEntry(gCurTask->updateState, 1, gGordoBounceHorizontalStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GordoBounceHorizontal(void)
{
    gCurTask->updateState = GORDO_BOUNCE_HORIZONTAL_STATE_BOUNCE_HORIZONTAL;
    {
        struct Task *t = gCurTask;

        t->velX = gUnk_087412A0[t->actorSpawnArg];
    }
    while (1)
    {
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void GordoBounceHorizontalState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void GordoSweepInit(void)
{
    gCurTask->updateCallback = (u32)GordoSweepUpdate;
    ActorSetState(GORDO_SWEEP_STATE_SWEEP);
    CallTableEntry(gCurTask->state, 1, gGordoSweepStates);
}

void GordoSweepUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gGordoSweepStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GordoSweep(void)
{
    gCurTask->updateState = GORDO_SWEEP_STATE_SWEEP;
    while (1)
    {
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(96);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0xC000;
        TaskYieldTrampoline(96);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
    }
}

void GordoSweepState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void Task_CoolSpook(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gCoolSpookFrames;
    AcquirePaletteAnim(0, 0);
    gCurTask->facing = 255;
    gCurTask->actorAnimDelay = ActorStartAnim(gCoolSpookAnim);
    CallTableEntry(gCurTask->variant, 2, gCoolSpookVariants);
}

void CoolSpookFlyInit(void)
{
    gCurTask->updateCallback = (u32)CoolSpookFlyUpdate;
    ActorSetState(COOL_SPOOK_FLY_STATE_FLY);
    CallTableEntry(gCurTask->state, 1, gCoolSpookFlyStates);
}

void CoolSpookFlyEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gCoolSpookFlyStates);
}

void CoolSpookFlyUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gCoolSpookFlyStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void CoolSpookFly(void)
{
    gCurTask->updateState = COOL_SPOOK_FLY_STATE_FLY;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_0807ef7c();
}

void CoolSpookFlyState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void CoolSpookBobInit(void)
{
    gCurTask->updateCallback = (u32)CoolSpookBobUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(COOL_SPOOK_BOB_STATE_BOB);
    CallTableEntry(gCurTask->state, 1, gCoolSpookBobStates);
}

void CoolSpookBobEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gCoolSpookBobStates);
}

void CoolSpookBobUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gCoolSpookBobStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void CoolSpookBob(void)
{
    gCurTask->updateState = COOL_SPOOK_BOB_STATE_BOB;
    sub_0807ef7c();
}

void CoolSpookBobState0Update(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void sub_0807ef7c(void)
{
    while (1)
    {
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
    }
}

void CoolSpookTeardown(void)
{
    gPaletteAnimRefCounts[0]--;
}

void Task_Kabu(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gKabuFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 4, gKabuVariants);
}
