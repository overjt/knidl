/* game_code_and_rodata 0x080860F8-0x08088000 (issue #80, module M23 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080860F8 0x08088000 src/enemy_860f8.c --newpb
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
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void AngleToVector(s16 t, s16 mag);

void BrontoBurtWaveInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtWaveUpdate;
    ActorSetState(BRONTO_BURT_WAVE_STATE_WAVE);
    CallTableEntry(gCurTask->state, 1, gBrontoBurtWaveStates);
}

void BrontoBurtWaveEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)BrontoBurtWaveUpdate;
    CallTableEntry(t->state, 1, gBrontoBurtWaveStates);
}

void BrontoBurtWaveUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBrontoBurtWaveStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtWave(void)
{
    struct Task *t;
    s16 *p;

    gCurTask->updateState = BRONTO_BURT_WAVE_STATE_WAVE;
    TaskStop();
    TaskSetMotionXFacing(gBrontoBurtFlySpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        t = gCurTask;
        t->velY = gBrontoBurtWaveVelY[0];
        t->accelY = -gBrontoBurtWaveAccelY[0];
    }
    else
    {
        t = gCurTask;
        t->velY = -gBrontoBurtWaveVelY[0];
        t->accelY = gBrontoBurtWaveAccelY[0];
    }
    t = gCurTask;
    t->brontoBurtTurnCount = 4;
    t->brontoBurtTurnTimer = 40;
    while (1)
    {
        if (gCurTask->accelY >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        }
    }
}

void BrontoBurtWaveState0Update(void)
{
    struct Task *t = gCurTask;
    s32 n;

    if (--t->brontoBurtTurnTimer != 0)
        return;
    n = t->brontoBurtTurnCount - 1;
    t->brontoBurtTurnCount = n;
    if (t->accelY < 0)
    {
        if (n < 0)
            return;
        t->velY = -gBrontoBurtWaveVelY[0];
        t->accelY = gBrontoBurtWaveAccelY[0];
    }
    else
    {
        t->velY = gBrontoBurtWaveVelY[0];
        t->accelY = -gBrontoBurtWaveAccelY[0];
    }
    gCurTask->brontoBurtTurnTimer = 40;
}

void BrontoBurtWeaveInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtWeaveUpdate;
    ActorSetState(BRONTO_BURT_WEAVE_STATE_WEAVE);
    CallTableEntry(gCurTask->state, 1, gBrontoBurtWeaveStates);
}

void BrontoBurtWeaveEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)BrontoBurtWeaveUpdate;
    CallTableEntry(t->state, 1, gBrontoBurtWeaveStates);
}

void BrontoBurtWeaveUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBrontoBurtWeaveStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtWeave(void)
{
    struct Task *t;
    s16 *p;

    gCurTask->updateState = BRONTO_BURT_WEAVE_STATE_WEAVE;
    TaskStop();
    TaskSetMotionXFacing(gBrontoBurtFlySpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        t = gCurTask;
        t->velY = gBrontoBurtWaveVelY[1];
        t->accelY = -gBrontoBurtWaveAccelY[1];
    }
    else
    {
        t = gCurTask;
        t->velY = -gBrontoBurtWaveVelY[1];
        t->accelY = gBrontoBurtWaveAccelY[1];
    }
    gCurTask->brontoBurtTurnTimer = 40;
    while (1)
    {
        if (gCurTask->accelY >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08086444(void)
{
    s32 d, a;

    if (--gCurTask->brontoBurtTurnTimer == 20)
    {
        d = (s16)TaskGetNearestPlayerDy();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gCurTask->accelY < 0)
                    gCurTask->accelY = 0xFFFFE000;
                else
                    gCurTask->accelY = 0xD00;
            }
            else
            {
                if (gCurTask->accelY < 0)
                    gCurTask->accelY = 0xFFFFF300;
                else
                    gCurTask->accelY = 0x2000;
            }
        }
    }
    if (gCurTask->brontoBurtTurnTimer != 0)
        return;
    if (gCurTask->accelY < 0)
    {
        gCurTask->velY = -gBrontoBurtWaveVelY[1];
        gCurTask->accelY = gBrontoBurtWaveAccelY[1];
    }
    else
    {
        gCurTask->velY = gBrontoBurtWaveVelY[1];
        gCurTask->accelY = -gBrontoBurtWaveAccelY[1];
    }
    gCurTask->brontoBurtTurnTimer = 40;
}

void BrontoBurtSwoopInit(void)
{
    struct Task *u;
    u32 r;

    gCurTask->updateCallback = (u32)BrontoBurtSwoopUpdate;
    if (TaskGetNearestPlayerDy() <= 31)
    {
        u = &gTasks[TaskFindNearestPlayer()];
        gCurTask->pixelX = (u16)u->pixelX;
        gCurTask->posX = u->posX;
    }
    else
    {
        r = RandomRange(4);
        gCurTask->pixelX =
            gUnk_087420AC[r] + (u16)gTasks[TaskFindNearestPlayer()].pixelX;
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
    }
    gCurTask->pixelY = 0;
    gCurTask->posY = 0;
    gCurTask->brontoBurtAtPlayerHeight = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, (u32 *)gBrontoBurtSwoopStates);
}

void BrontoBurtSwoopUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBrontoBurtSwoopStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtSwoop(void)
{
    u8 k;

    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->accelY = 0x2500;
    gCurTask->speedLimitY = 0x30000;
    TaskSetFrame(4);
    while (gCurTask->brontoBurtAtPlayerHeight == 0)
        TaskYieldTrampoline(1);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420C0);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->actorSpawnArg]);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0x8000;
    k = gCurTask->actorSpawnArg;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gCurTask->variant = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gCurTask->variant = 1;
    }
    TaskSleepForever();
}

void BrontoBurtSwoopState0Update(void)
{
    if (gCurTask->variant != 2)
    {
        TaskSetEntry(BrontoBurtEnterVariant, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
    if (gCurTask->brontoBurtAtPlayerHeight != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->brontoBurtAtPlayerHeight = 1;
}

void BrontoBurtDiagonalInit(void)
{
    s8 k;

    gCurTask->updateCallback = (u32)BrontoBurtDiagonalUpdate;
    k = TaskGetCompassDirToNearestPlayer();
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gCurTask->brontoBurtFlightAngle = (k << 7) + 64;
    AngleToVector(gCurTask->brontoBurtFlightAngle,
                 gBrontoBurtDiagonalSpeeds[gCurTask->actorSpawnArg] << 8 >> 16);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    gCurTask->brontoBurtBounceTimer = 0;
    ActorSetState(BRONTO_BURT_DIAGONAL_STATE_DIAGONAL);
    CallTableEntry(gCurTask->state, 1, gBrontoBurtDiagonalStates);
}

void BrontoBurtDiagonalEnterState(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtDiagonalUpdate;
    CallTableEntry(gCurTask->state, 1, gBrontoBurtDiagonalStates);
}

void BrontoBurtDiagonalUpdate(void)
{
    s32 v;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gBrontoBurtDiagonalStateUpdates);
    if (TaskReflectFlightAngle() != 0)
    {
        AngleToVector((s16)gCurTask->brontoBurtFlightAngle,
                     gBrontoBurtDiagonalSpeeds[gCurTask->actorSpawnArg] << 8 >> 16);
        v = gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        if (v > 0)
            gCurTask->facing = 1;
        else if (v < 0)
            gCurTask->facing = -1;
        gCurTask->brontoBurtBounceTimer = 20;
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420C0);
    }
    gCurTask->onGround = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtDiagonal(void)
{
    gCurTask->updateState = BRONTO_BURT_DIAGONAL_STATE_DIAGONAL;
    gCurTask->onGround = 0;
    while (1)
    {
        if (gCurTask->brontoBurtBounceTimer == 0)
            gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420D4);
        TaskYieldTrampoline(1);
    }
}

void BrontoBurtDiagonalState0Update(void)
{
    gCurTask->brontoBurtBounceTimer--;
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void BrontoBurtChaseInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtChaseUpdate;
    ActorSetState(BRONTO_BURT_CHASE_STATE_CHASE);
    CallTableEntry(gCurTask->state, 1, gBrontoBurtChaseStates);
}

void BrontoBurtChaseEnterState(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtChaseUpdate;
    CallTableEntry(gCurTask->state, 1, gBrontoBurtChaseStates);
}

void BrontoBurtChaseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBrontoBurtChaseStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtChase(void)
{
    gCurTask->updateState = BRONTO_BURT_CHASE_STATE_CHASE;
    TaskStop();
    gCurTask->brontoBurtSteerDirY = 0;
    gCurTask->brontoBurtChaseTimer = 384;
    do
    {
        switch (gCurTask->brontoBurtSteerDirY)
        {
        case 0:
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
            break;
        case 1:
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            break;
        }
    } while (gCurTask->brontoBurtChaseTimer != 0);
    TaskFaceNearestPlayer();
    TaskTurnAroundAndReverseX();
    TaskSetMotionXFacing(0x6600, 0x5A5A5A5A);
    gCurTask->accelY = 0xFFFFE700;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
    }
}

void BrontoBurtChaseState0Update(void)
{
    if (gCurTask->brontoBurtChaseTimer != 0)
    {
        gCurTask->brontoBurtChaseTimer--;
        TaskFaceNearestPlayer();
        gCurTask->brontoBurtSteerTimer++;
        if (gCurTask->brontoBurtSteerTimer == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_0874210C[gCurTask->actorSpawnArg],
                         gUnk_0874210C[gCurTask->actorSpawnArg + 4]);
            gCurTask->brontoBurtSteerDirY = gUnk_030023D4;
            gCurTask->brontoBurtSteerTimer = 0;
        }
    }
}

void BrontoBurtTakeOffInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtTakeOffUpdate;
    ActorSetState(BRONTO_BURT_TAKE_OFF_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gBrontoBurtTakeOffStates);
}

void BrontoBurtTakeOffEnterState(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtTakeOffUpdate;
    CallTableEntry(gCurTask->state, 3, gBrontoBurtTakeOffStates);
}

void BrontoBurtTakeOffUpdate(void)
{
    if (gCurTask->brontoBurtGrounded != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gBrontoBurtTakeOffStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 3, gBrontoBurtTakeOffStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtTakeOffWait(void)
{
    gCurTask->updateState = BRONTO_BURT_TAKE_OFF_STATE_WAIT;
    gCurTask->brontoBurtGrounded = 1;
    gCurTask->onGround = 1;
    gCurTask->actorAnimDelay = ActorStartAnim(gBrontoBurtTakeOffWaitAnim);
    while (gCurTask->onGround != 0)
    {
        if (TaskGetNearestPlayerDx() < 0)
        {
            if (-TaskGetNearestPlayerDx() <= 63)
                break;
        }
        else if (TaskGetNearestPlayerDx() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    ActorSetState(BRONTO_BURT_TAKE_OFF_STATE_TAKE_OFF);
    TaskSleepForever();
}

void BrontoBurtTakeOffWaitUpdate(void)
{
    if (gCurTask->state != BRONTO_BURT_TAKE_OFF_STATE_WAIT)
    {
        TaskSetEntry(BrontoBurtTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void BrontoBurtTakeOff(void)
{
    gCurTask->updateState = BRONTO_BURT_TAKE_OFF_STATE_TAKE_OFF;
    gCurTask->brontoBurtGrounded = 0;
    PlaySfx(187);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420C0);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420D4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    ActorSetState(BRONTO_BURT_TAKE_OFF_STATE_2);
    TaskSleepForever();
}

void BrontoBurtTakeOffState1Update(void)
{
    if (gCurTask->state != BRONTO_BURT_TAKE_OFF_STATE_TAKE_OFF)
    {
        TaskSetEntry(BrontoBurtTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void BrontoBurtTakeOffState2(void)
{
    gCurTask->updateState = BRONTO_BURT_TAKE_OFF_STATE_2;
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420D4);
    switch (gCurTask->actorSpawnArg)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420C0);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 1:
        TaskYieldTrampoline(32);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087420C0);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    }
}

void BrontoBurtTakeOffState2Update(void)
{
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void BrontoBurtIdle(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(18);
        TaskSetFrame(9);
        TaskYieldTrampoline(10);
    }
}

void BrontoBurtIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 TaskReflectFlightAngle(void)
{
    u8 n = 0;
    s16 v;

    if ((gCurTask->onGround & 1) == 0 && (gTerrainResult[1] & 1) == 0
     && (gTerrainResult[0] & 3) == 0)
        return 0;
    switch (gTerrainResult[4])
    {
    case 0:
        if (gCurTask->onGround & 1)
            n = n + 1;
        if ((gTerrainResult[1] & 1) == 0)
            break;
        n = n + 2;
        break;
    case 3:
        n = n + 3;
        break;
    case 4:
        n = n + 4;
        break;
    case 1:
        n = n + 5;
        break;
    case 2:
        n = n + 6;
        break;
    case 8:
        n = n + 7;
        break;
    case 7:
        n = n + 8;
        break;
    case 6:
        n = n + 9;
        break;
    case 5:
        n = n + 10;
        break;
    }
    if (gTerrainResult[0] & 1)
        n = n + 11;
    if (gTerrainResult[0] & 2)
        n = n + 22;
    v = gUnk_08742150[(n << 4) + (gCurTask->unk2C >> 5)];
    if (v < 0)
        return 0;
    gCurTask->unk2C = v;
    return 1;
}

void Task_Twizzy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gTwizzyFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 10, gTwizzyVariants);
}

void TwizzyEnterVariant(void)
{
    CallTableEntry(gCurTask->variant, 10, gTwizzyVariants);
}

void TwizzyWaveInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyWaveUpdate;
    ActorSetState(TWIZZY_WAVE_STATE_WAVE);
    CallTableEntry(gCurTask->state, 1, gTwizzyWaveStates);
}

void TwizzyWaveEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyWaveUpdate;
    CallTableEntry(gCurTask->state, 1, gTwizzyWaveStates);
}

void TwizzyWaveUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwizzyWaveStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyWave(void)
{
    s16 *p;

    gCurTask->updateState = TWIZZY_WAVE_STATE_WAVE;
    TaskStop();
    TaskSetMotionXFacing(gTwizzyFlySpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        gCurTask->velY = gTwizzyWaveVelY[0];
        gCurTask->accelY = -gTwizzyWaveAccelY[0];
    }
    else
    {
        gCurTask->velY = -gTwizzyWaveVelY[0];
        gCurTask->accelY = gTwizzyWaveAccelY[0];
    }
    gCurTask->twizzyTurnCount = 4;
    gCurTask->twizzyTurnTimer = 40;
    while (1)
    {
        if (gCurTask->accelY >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
        }
    }
}

void TwizzyWaveState0Update(void)
{
    struct Task *t = gCurTask;
    s32 n;

    if (--t->twizzyTurnTimer != 0)
        return;
    n = t->twizzyTurnCount - 1;
    t->twizzyTurnCount = n;
    if (t->accelY < 0)
    {
        if (n < 0)
            return;
        t->velY = -gTwizzyWaveVelY[0];
        t->accelY = gTwizzyWaveAccelY[0];
    }
    else
    {
        t->velY = gTwizzyWaveVelY[0];
        t->accelY = -gTwizzyWaveAccelY[0];
    }
    gCurTask->twizzyTurnTimer = 40;
}

void TwizzyWeaveInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyWeaveUpdate;
    ActorSetState(TWIZZY_WEAVE_STATE_WEAVE);
    CallTableEntry(gCurTask->state, 1, gTwizzyWeaveStates);
}

void TwizzyWeaveEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyWeaveUpdate;
    CallTableEntry(gCurTask->state, 1, gTwizzyWeaveStates);
}

void TwizzyWeaveUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwizzyWeaveStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyWeave(void)
{
    s16 *p;

    gCurTask->updateState = TWIZZY_WEAVE_STATE_WEAVE;
    TaskStop();
    TaskSetMotionXFacing(gTwizzyFlySpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        gCurTask->velY = gTwizzyWaveVelY[1];
        gCurTask->accelY = -gTwizzyWaveAccelY[1];
    }
    else
    {
        gCurTask->velY = -gTwizzyWaveVelY[1];
        gCurTask->accelY = gTwizzyWaveAccelY[1];
    }
    gCurTask->twizzyTurnTimer = 40;
    while (1)
    {
        if (gCurTask->accelY >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
        }
    }
}

void sub_080873b0(void)
{
    s32 d, a;

    if (--gCurTask->twizzyTurnTimer == 20)
    {
        d = (s16)TaskGetNearestPlayerDy();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gCurTask->accelY < 0)
                    gCurTask->accelY = 0xFFFFE000;
                else
                    gCurTask->accelY = 0xD00;
            }
            else
            {
                if (gCurTask->accelY < 0)
                    gCurTask->accelY = 0xFFFFF300;
                else
                    gCurTask->accelY = 0x2000;
            }
        }
    }
    if (gCurTask->twizzyTurnTimer != 0)
        return;
    if (gCurTask->accelY < 0)
    {
        gCurTask->velY = -gTwizzyWaveVelY[1];
        gCurTask->accelY = gTwizzyWaveAccelY[1];
    }
    else
    {
        gCurTask->velY = gTwizzyWaveVelY[1];
        gCurTask->accelY = -gTwizzyWaveAccelY[1];
    }
    gCurTask->twizzyTurnTimer = 40;
}

void TwizzySwoopInit(void)
{
    struct Task *u;
    u32 r;

    gCurTask->updateCallback = (u32)TwizzySwoopUpdate;
    if (TaskGetNearestPlayerDy() <= 31)
    {
        u = &gTasks[TaskFindNearestPlayer()];
        gCurTask->pixelX = (u16)u->pixelX;
        gCurTask->posX = u->posX;
    }
    else
    {
        r = RandomRange(4);
        gCurTask->pixelX =
            gUnk_087425DC[r] + (u16)gTasks[TaskFindNearestPlayer()].pixelX;
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
    }
    gCurTask->pixelY = 0;
    gCurTask->posY = 0;
    gCurTask->twizzyAtPlayerHeight = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, (u32 *)gTwizzySwoopStates);
}

void TwizzySwoopUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwizzySwoopStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzySwoop(void)
{
    u8 k;

    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->accelY = 0x2500;
    gCurTask->speedLimitY = 0x30000;
    TaskSetFrame(4);
    while (gCurTask->twizzyAtPlayerHeight == 0)
        TaskYieldTrampoline(1);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742598);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->actorSpawnArg]);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0x8000;
    k = gCurTask->actorSpawnArg;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gCurTask->variant = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gCurTask->variant = 1;
    }
    TaskSleepForever();
}

void TwizzySwoopState0Update(void)
{
    if (gCurTask->variant != 2)
    {
        TaskSetEntry(TwizzyEnterVariant, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
    if (gCurTask->twizzyAtPlayerHeight != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->twizzyAtPlayerHeight = 1;
}

void TwizzyDiagonalInit(void)
{
    s8 k;

    gCurTask->updateCallback = (u32)TwizzyDiagonalUpdate;
    k = TaskGetCompassDirToNearestPlayer() - 1;
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gCurTask->twizzyFlightAngle = (k << 7) + 64;
    AngleToVector(gCurTask->twizzyFlightAngle,
                 gTwizzyDiagonalSpeeds[gCurTask->actorSpawnArg] << 8 >> 16);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    gCurTask->twizzyBounceTimer = 0;
    ActorSetState(TWIZZY_DIAGONAL_STATE_DIAGONAL);
    CallTableEntry(gCurTask->state, 1, gTwizzyDiagonalStates);
}

void TwizzyDiagonalEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)TwizzyDiagonalUpdate;
    CallTableEntry(t->state, 1, gTwizzyDiagonalStates);
}

void TwizzyDiagonalUpdate(void)
{
    s32 v;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gTwizzyDiagonalStateUpdates);
    if (TaskReflectFlightAngle() != 0)
    {
        AngleToVector((s16)gCurTask->twizzyFlightAngle,
                     gTwizzyDiagonalSpeeds[gCurTask->actorSpawnArg] << 8 >> 16);
        v = gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        if (v > 0)
            gCurTask->facing = 1;
        else if (v < 0)
            gCurTask->facing = -1;
        gCurTask->twizzyBounceTimer = 20;
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742598);
    }
    gCurTask->onGround = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyDiagonal(void)
{
    gCurTask->updateState = TWIZZY_DIAGONAL_STATE_DIAGONAL;
    gCurTask->onGround = 0;
    while (1)
    {
        if (gCurTask->twizzyBounceTimer == 0)
            gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087425A4);
        TaskYieldTrampoline(1);
    }
}

void TwizzyDiagonalState0Update(void)
{
    gCurTask->twizzyBounceTimer--;
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void TwizzyChaseInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyChaseUpdate;
    ActorSetState(TWIZZY_CHASE_STATE_CHASE);
    CallTableEntry(gCurTask->state, 1, gTwizzyChaseStates);
}

void TwizzyChaseEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)TwizzyChaseUpdate;
    CallTableEntry(t->state, 1, gTwizzyChaseStates);
}

void TwizzyChaseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwizzyChaseStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyChase(void)
{
    gCurTask->updateState = TWIZZY_CHASE_STATE_CHASE;
    TaskStop();
    gCurTask->twizzySteerDirY = 0;
    gCurTask->twizzyChaseTimer = 384;
    do
    {
        switch (gCurTask->twizzySteerDirY)
        {
        case 0:
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
            break;
        case 1:
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            break;
        }
    } while (gCurTask->twizzyChaseTimer != 0);
    TaskFaceNearestPlayer();
    TaskTurnAroundAndReverseX();
    TaskSetMotionXFacing(0x6600, 0x5A5A5A5A);
    gCurTask->accelY = 0xFFFFE700;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
    }
}

void TwizzyChaseState0Update(void)
{
    if (gCurTask->twizzyChaseTimer != 0)
    {
        gCurTask->twizzyChaseTimer--;
        TaskFaceNearestPlayer();
        gCurTask->twizzySteerTimer++;
        if (gCurTask->twizzySteerTimer == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_08742614[gCurTask->actorSpawnArg],
                         gUnk_08742614[gCurTask->actorSpawnArg + 4]);
            gCurTask->twizzySteerDirY = gUnk_030023D4;
            gCurTask->twizzySteerTimer = 0;
        }
    }
}

void TwizzyTakeOffInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyTakeOffUpdate;
    ActorSetState(TWIZZY_TAKE_OFF_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gTwizzyTakeOffStates);
}

void TwizzyTakeOffEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyTakeOffUpdate;
    CallTableEntry(gCurTask->state, 3, gTwizzyTakeOffStates);
}

void TwizzyTakeOffUpdate(void)
{
    if (gCurTask->twizzyGrounded != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gTwizzyTakeOffStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 3, gTwizzyTakeOffStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyTakeOffWait(void)
{
    gCurTask->updateState = TWIZZY_TAKE_OFF_STATE_WAIT;
    gCurTask->twizzyGrounded = 1;
    gCurTask->onGround = 1;
    gCurTask->actorAnimDelay = ActorStartAnim(gTwizzyTakeOffWaitAnim);
    while (gCurTask->onGround != 0)
    {
        if (TaskGetNearestPlayerDx() < 0)
        {
            if (-TaskGetNearestPlayerDx() <= 63)
                break;
        }
        else if (TaskGetNearestPlayerDx() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    ActorSetState(TWIZZY_TAKE_OFF_STATE_TAKE_OFF);
    TaskSleepForever();
}

void TwizzyTakeOffWaitUpdate(void)
{
    if (gCurTask->state != TWIZZY_TAKE_OFF_STATE_WAIT)
    {
        TaskSetEntry(TwizzyTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void TwizzyTakeOff(void)
{
    gCurTask->updateState = TWIZZY_TAKE_OFF_STATE_TAKE_OFF;
    gCurTask->twizzyGrounded = 0;
    PlaySfx(187);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742598);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087425A4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    ActorSetState(TWIZZY_TAKE_OFF_STATE_2);
    TaskSleepForever();
}

void TwizzyTakeOffState1Update(void)
{
    if (gCurTask->state != TWIZZY_TAKE_OFF_STATE_TAKE_OFF)
    {
        TaskSetEntry(TwizzyTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void TwizzyTakeOffState2(void)
{
    gCurTask->updateState = TWIZZY_TAKE_OFF_STATE_2;
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087425A4);
    switch (gCurTask->actorSpawnArg)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742598);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 1:
        TaskYieldTrampoline(32);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742598);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    }
}

void TwizzyTakeOffState2Update(void)
{
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void TwizzyHopInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyHopUpdate;
    gCurTask->onGround = 1;
    ActorSetState(TWIZZY_HOP_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gTwizzyHopStates);
}

void TwizzyHopEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyHopUpdate;
    CallTableEntry(gCurTask->state, 3, gTwizzyHopStates);
}

void TwizzyHopUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gTwizzyHopStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyHopWait(void)
{
    gCurTask->updateState = TWIZZY_HOP_STATE_WAIT;
    TaskStop();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    ActorSetState(TWIZZY_HOP_STATE_JUMP);
    TaskSleepForever();
}

void TwizzyHopWaitUpdate(void)
{
    if (gCurTask->state != TWIZZY_HOP_STATE_WAIT)
        TaskSetEntry(TwizzyHopEnterState, gCurTaskIdx);
}

void TwizzyHopJump(void)
{
    gCurTask->updateState = TWIZZY_HOP_STATE_JUMP;
    gCurTask->onGround = 0;
    PlaySfx(187);
    TaskSetMotionY(0xFFFE0000, 0x800, 0x30000);
    gCurTask->twizzyFlapCount = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->twizzyFlapCount <= 3);
    TaskSetFrame(4);
    TaskSleepForever();
}

void TwizzyHopJumpUpdate(void)
{
}

void TwizzyHopFall(void)
{
    gCurTask->updateState = TWIZZY_HOP_STATE_FALL;
    gCurTask->accelY = 0x800;
    gCurTask->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    }
}

void TwizzyHopFallUpdate(void)
{
}

void TwizzyHopToChaseInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyHopToChaseUpdate;
    gCurTask->onGround = 1;
    ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_0);
    CallTableEntry(gCurTask->state, 6, gTwizzyHopToChaseStates);
}
