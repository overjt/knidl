/* game_code_and_rodata 0x08088000-0x0808AA68 (issue #80, module M23 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08088000 0x0808AA68 src/enemy_88000.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorCollideTerrainFloor(void);
extern u32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void AngleToVector(s16 t, s16 mag);

void TwizzyHopToChaseEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyHopToChaseUpdate;
    CallTableEntry(gCurTask->state, 6, gTwizzyHopToChaseStates);
}

void TwizzyHopToChaseUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 6, gTwizzyHopToChaseStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088054(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_0;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskYieldTrampoline(15);
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    switch (RandomRange(8))
    {
    case 0:
    case 1:
        gCurTask->velX = 0;
        ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_JUMP);
        break;
    case 2:
    case 3:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_JUMP);
        break;
    case 4:
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_JUMP);
        break;
    case 5:
        ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_WAIT_SHORT);
        break;
    case 6:
    case 7:
        ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_WAIT_LONG);
        break;
    }
    TaskSleepForever();
}

void sub_080880fc(void)
{
    s32 v;

    if (gCurTask->state != TWIZZY_HOP_TO_CHASE_STATE_0)
        TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
    v = gUnk_0874269C[gCurTask->actorSpawnArg];
    if (v > abs(TaskGetNearestPlayerDx()))
    {
        v = gUnk_087426A4[gCurTask->actorSpawnArg];
        if (v > abs(TaskGetNearestPlayerDy()))
        {
            ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_TAKE_OFF);
            TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
        }
    }
}

void TwizzyHopToChaseJump(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_JUMP;
    gCurTask->onGround = 0;
    if (RandomRange(2) != 0)
        TaskTurnAroundAndReverseX();
    TaskSetMotionY(0xFFFE8000, 0x4000, 0x30000);
    TaskSleepForever();
}

void TwizzyHopToChaseJumpUpdate(void)
{
}

void TwizzyHopToChaseWaitShort(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_WAIT_SHORT;
    TaskYieldTrampoline(32);
    ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_0);
    TaskSleepForever();
}

void TwizzyHopToChaseWaitShortUpdate(void)
{
    if (gCurTask->state != TWIZZY_HOP_TO_CHASE_STATE_WAIT_SHORT)
        TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
}

void TwizzyHopToChaseWaitLong(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_WAIT_LONG;
    TaskYieldTrampoline(64);
    ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_0);
    TaskSleepForever();
}

void TwizzyHopToChaseWaitLongUpdate(void)
{
    if (gCurTask->state != TWIZZY_HOP_TO_CHASE_STATE_WAIT_LONG)
        TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
}

void TwizzyHopToChaseTakeOff(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_TAKE_OFF;
    gCurTask->onGround = 0;
    PlaySfx(187);
    TaskStop();
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskFaceNearestPlayer();
    while (gCurTask->velY < 0)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    }
    gCurTask->variant = 4;
    TaskSleepForever();
}

void TwizzyHopToChaseTakeOffUpdate(void)
{
    if (gCurTask->variant != 7)
    {
        gCurTask->updateCallback = 0;
        TaskSetEntry(Task_Twizzy, gCurTaskIdx);
    }
}

void TwizzyHopToChaseFall(void)
{
    gCurTask->updateState = TWIZZY_HOP_TO_CHASE_STATE_FALL;
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    }
}

void TwizzyHopToChaseFallUpdate(void)
{
}

void TwizzyHoverInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyHoverUpdate;
    gCurTask->onGround = 0;
    ActorSetState(TWIZZY_HOVER_STATE_HOVER);
    CallTableEntry(gCurTask->state, 1, gTwizzyHoverStates);
}

void TwizzyHoverEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)TwizzyHoverUpdate;
    CallTableEntry(t->state, 1, gTwizzyHoverStates);
}

void TwizzyHoverUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwizzyHoverStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyHover(void)
{
    gCurTask->updateState = TWIZZY_HOVER_STATE_HOVER;
    TaskFaceNearestPlayer();
    gCurTask->twizzyFaceTimer = 8;
    while (1)
    {
        gCurTask->velY = 0xFFFF0000;
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        gCurTask->velY = 0xFFFF8000;
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x10000;
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0xFFFF8000;
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    }
}

void TwizzyHoverState0Update(void)
{
    if (--gCurTask->twizzyFaceTimer == 0)
    {
        gCurTask->twizzyFaceTimer = 8;
        TaskFaceNearestPlayer();
    }
}

void TwizzyIdle(void)
{
    gCurTask->updateCallback = (u32)TwizzyIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskSleepForever();
}

void TwizzyIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 TwizzyLand(void)
{
    if (gCurTask->variant != 9)
    {
        switch (gCurTask->variant)
        {
        case 6:
            ActorSetState(0);
            TaskSetEntry(TwizzyHopEnterState, gCurTaskIdx);
            return 1;
        case 7:
            ActorSetState(0);
            TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 TwizzyStartFall(void)
{
    u8 s;

    if (gCurTask->variant != 9)
    {
        switch (gCurTask->variant)
        {
        case 7:
            s = gCurTask->state;
            if (s == 0 || s == 2 || s == 3)
            {
                ActorSetState(TWIZZY_HOP_TO_CHASE_STATE_FALL);
                TaskSetEntry(TwizzyHopToChaseEnterState, gCurTaskIdx);
                return 1;
            }
            return 0;
        }
        return 0;
    }
}

s32 TwizzyHitWall(void)
{
    if (gCurTask->variant != 9)
    {
        switch (gCurTask->variant)
        {
        case 7:
            if (gCurTask->state == 1)
                TaskTurnAroundAndReverseX();
            return 0;
        }
        return 0;
    }
}

s32 TwizzyHitCeiling(void)
{
    if (gCurTask->variant != 9)
    {
        switch (gCurTask->variant)
        {
        case 6:
            gCurTask->velY = 0;
            return 0;
        case 7:
            if (gCurTask->state == 4)
            {
                gCurTask->variant = 4;
                TaskSetEntry(Task_Twizzy, gCurTaskIdx);
                return 1;
            }
            break;
        default:
            return 0;
        }
        return 0;
    }
}

void Task_Squishy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSquishyFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 4, gSquishyVariants);
}

void SquishyWalkInit(void)
{
    gCurTask->updateCallback = (u32)SquishyWalkUpdate;
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
    {
        ActorSetState(SQUISHY_WALK_STATE_3);
        gCurTask->onGround = 0;
        CallTableEntry(gCurTask->state, 5, gSquishyWalkStates);
    }
    ActorSetState(SQUISHY_WALK_STATE_WALK);
    CallTableEntry(gCurTask->state, 5, gSquishyWalkStates);
}

void SquishyWalkEnterState(void)
{
    gCurTask->updateCallback = (u32)SquishyWalkUpdate;
    CallTableEntry(gCurTask->state, 5, gSquishyWalkStates);
}

void SquishyWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gSquishyWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SquishyWalk(void)
{
    gCurTask->updateState = SQUISHY_WALK_STATE_WALK;
    gCurTask->squishyWalkTimer = 100;
    TaskSetMotionXFacing(gUnk_087426EC[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->actorSpawnArg * 2]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->actorSpawnArg * 2 + 1]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->actorSpawnArg * 2]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->actorSpawnArg * 2 + 1]);
    }
}

void SquishyWalkState0Update(void)
{
    s32 n = gCurTask->squishyWalkTimer - 1;

    gCurTask->squishyWalkTimer = n;
    switch (n)
    {
    case 20:
    case 40:
    case 60:
    case 80:
        if (RandomRange(4) == 0)
        {
            ActorSetState(SQUISHY_WALK_STATE_1);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        }
        break;
    case 0:
        ActorSetState(SQUISHY_WALK_STATE_1);
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        break;
    }
}

void SquishyWalkState1(void)
{
    gCurTask->updateState = SQUISHY_WALK_STATE_1;
    gCurTask->squishyLanded = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(24);
    PlaySfx(188);
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->squishyLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(SQUISHY_WALK_STATE_WALK);
    TaskSleepForever();
}

void SquishyWalkState1Update(void)
{
    if (gCurTask->state != SQUISHY_WALK_STATE_1)
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
}

void SquishyWalkFall(void)
{
    gCurTask->updateState = SQUISHY_WALK_STATE_FALL;
    gCurTask->squishyLanded = 0;
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    TaskSetFrame(6);
    while (gCurTask->squishyLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(SQUISHY_WALK_STATE_WALK);
    TaskSleepForever();
}

void SquishyWalkFallUpdate(void)
{
    if (gCurTask->state != SQUISHY_WALK_STATE_FALL)
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
}

void SquishyWalkState3(void)
{
    gCurTask->updateState = SQUISHY_WALK_STATE_3;
    TaskStop();
    gCurTask->velY = 0x4000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(96);
        gCurTask->velY = 0x8000;
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFF0000;
        gCurTask->onGround = 0;
        gCurTask->frame--;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFFC000;
        gCurTask->frame++;
        TaskYieldTrampoline(10);
    }
}

void SquishyWalkState3Update(void)
{
}

void SquishyWalkState4(void)
{
    gCurTask->updateState = SQUISHY_WALK_STATE_4;
    gCurTask->squishyLanded = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->squishyLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(SQUISHY_WALK_STATE_WALK);
    TaskSleepForever();
}

void SquishyWalkState4Update(void)
{
    if (gCurTask->state != SQUISHY_WALK_STATE_4)
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
}

void SquishyVariant1(void)
{
    gCurTask->updateCallback = (u32)sub_08088b10;
    TaskFaceNearestPlayer();
    if (gCurTask->facing == 1)
        gCurTask->posX = (gTasks[TaskFindNearestPlayer()].pixelX - 80) << 16;
    else
        gCurTask->posX = (gTasks[TaskFindNearestPlayer()].pixelX + 80) << 16;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_08742704);
}

void sub_08088aec(void)
{
    gCurTask->updateCallback = (u32)sub_08088b10;
    CallTableEntry(gCurTask->state, 3, gUnk_08742704);
}

void sub_08088b10(void)
{
    if (gCurTask->squishyCollideTerrain != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gUnk_08742710);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 3, gUnk_08742710);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088b58(void)
{
    gCurTask->updateState = 0;
    gCurTask->squishyCollideTerrain = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_08088b98(void)
{
    if (sub_08021c14(gCurTask->pixelX, gCurTask->pixelY) == 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_08088aec, gCurTaskIdx);
    }
}

void sub_08088bd8(void)
{
    gCurTask->updateState = 1;
    gCurTask->squishyCollideTerrain = 0;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    TaskSetMotionY(0x40000, 0x1500, 0x30000);
    gCurTask->onGround = 0;
    gCurTask->squishyCollideTerrain = 1;
    TaskSleepForever();
}

void sub_08088ca0(void)
{
}

void sub_08088ca4(void)
{
    gCurTask->updateState = 2;
    gCurTask->squishyCollideTerrain = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFC0000;
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08088ce4(void)
{
}

void SquishyJumpToWalkInit(void)
{
    gCurTask->updateCallback = (u32)SquishyJumpToWalkUpdate;
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
    {
        gCurTask->variant = 0;
        gCurTask->updateCallback = (u32)SquishyWalkUpdate;
        gCurTask->onGround = 0;
        ActorSetState(3);
        CallTableEntry(gCurTask->state, 5, gSquishyWalkStates);
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gSquishyJumpToWalkStates);
}

void SquishyJumpToWalkEnterState(void)
{
    gCurTask->updateCallback = (u32)SquishyJumpToWalkUpdate;
    CallTableEntry(gCurTask->state, 3, gSquishyJumpToWalkStates);
}

void SquishyJumpToWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gSquishyJumpToWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SquishyJumpToWalkJump(void)
{
    struct Task *t;
    struct Task *u;
    s32 a, d;

    gCurTask->updateState = SQUISHY_JUMP_TO_WALK_STATE_JUMP;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    u = &gTasks[TaskFindNearestPlayer()];
    t = gCurTask;
    a = t->posY;
    d = (a >> 16) - (u->posY >> 16);
    if (d > 0)
        t->squishyHopTimer = 37;
    else if (d < 0)
        t->squishyHopTimer = 43;
    else if ((a & 0xFF) - (u->posY & 0xFF) >= 0
                 ? (a & 0xFF) - (u->posY & 0xFF) <= 15
                 : (u->posY & 0xFF) - (a & 0xFF) <= 15)
        gCurTask->squishyHopTimer = 37;
    else
        gCurTask->squishyHopTimer = 40;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(gUnk_08742734[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2000, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void SquishyJumpToWalkJumpUpdate(void)
{
    if (--gCurTask->squishyHopTimer == 0)
        TaskSetEntry(SquishyJumpToWalkEnterState, gCurTaskIdx);
}

void SquishyJumpToWalkLand(void)
{
    gCurTask->updateState = SQUISHY_JUMP_TO_WALK_STATE_LAND;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    gCurTask->variant = 0;
    TaskSleepForever();
}

void SquishyJumpToWalkLandUpdate(void)
{
    if (gCurTask->variant != 2)
        TaskSetEntry(Task_Squishy, gCurTaskIdx);
}

void sub_08088efc(void)
{
    gCurTask->updateState = SQUISHY_JUMP_TO_WALK_STATE_1;
    gCurTask->velY = 0;
    TaskYieldTrampoline(24);
    ActorSetState(SQUISHY_JUMP_TO_WALK_STATE_JUMP);
    TaskSleepForever();
}

void sub_08088f24(void)
{
    if (gCurTask->state != SQUISHY_JUMP_TO_WALK_STATE_1)
        TaskSetEntry(SquishyJumpToWalkEnterState, gCurTaskIdx);
}

void SquishyIdle(void)
{
    gCurTask->updateCallback = (u32)SquishyIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
    }
}

void SquishyIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 SquishyLand(void)
{
    if (gCurTask->variant != 3)
    {
        switch (gCurTask->variant)
        {
        case 0:
            gCurTask->squishyLanded = 1;
            return 0;
        case 1:
            ActorSetState(2);
            TaskSetEntry(sub_08088aec, gCurTaskIdx);
            return 1;
        case 2:
            ActorSetState(2);
            TaskSetEntry(SquishyJumpToWalkEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 SquishyStartFall(void)
{
    if (gCurTask->variant != 3)
    {
        switch (gCurTask->variant)
        {
        default:
            return 0;
        case 0:
            ActorSetState(SQUISHY_WALK_STATE_FALL);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        }
    }
}

s32 SquishyEnterWater(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->u8C.actor->hitReactions = (u32)gSquishyInWaterHitReactions;
        switch (gCurTask->variant)
        {
        case 0:
            ActorSetState(SQUISHY_WALK_STATE_3);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        case 2:
            gCurTask->variant = 0;
            ActorSetState(SQUISHY_WALK_STATE_3);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 SquishyLeaveWater(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->u8C.actor->hitReactions = (u32)gSquishyHitReactions;
        if (gCurTask->variant != 0)
            return 0;
        ActorSetState(SQUISHY_WALK_STATE_4);
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 SquishyBounceOffWall(void)
{
    if (gCurTask->variant != 3)
    {
        TaskTurnAroundAndReverseX();
        return 0;
    }
}

s32 SquishyHitCeiling(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->velY = 0;
        if (gCurTask->variant != 2)
            return 0;
        ActorSetState(SQUISHY_JUMP_TO_WALK_STATE_1);
        TaskSetEntry(SquishyJumpToWalkEnterState, gCurTaskIdx);
        return 1;
    }
}

void Task_Bubbles(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gBubblesFrames;
    CallTableEntry(gCurTask->variant, 2, gBubblesVariants);
}

void BubblesInit(void)
{
    gCurTask->updateCallback = (u32)BubblesUpdate;
    gCurTask->bubblesRollPhase = 12;
    TaskFaceNearestPlayer();
    ActorSetState(BUBBLES_STATE_JUMP);
    CallTableEntry(gCurTask->state, 5, gBubblesStates);
}

void BubblesEnterState(void)
{
    gCurTask->updateCallback = (u32)BubblesUpdate;
    CallTableEntry(gCurTask->state, 5, gBubblesStates);
}

void BubblesUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gBubblesStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BubblesJump(void)
{
    s32 n;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    if (gCurTask->actorSpawnArg == 0 || RandomRange(2) != 0)
    {
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    }
    else
    {
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    }
    n = RandomRange(3);
    gCurTask->bubblesJumpKind = n;
    TaskSetMotionY(gUnk_0874276C[n], 0x2000, 0x30000);
    PlaySfx(193);
    while (1)
    {
        BubblesSetRollFrame(gCurTask->bubblesRollPhase);
        switch (gCurTask->bubblesJumpKind)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->facing == 1)
        {
            gCurTask->bubblesRollPhase++;
            if (gCurTask->bubblesRollPhase > 15)
                gCurTask->bubblesRollPhase = 0;
        }
        else
        {
            gCurTask->bubblesRollPhase--;
            if (gCurTask->bubblesRollPhase < 0)
                gCurTask->bubblesRollPhase = 15;
        }
    }
}

void BubblesJumpUpdate(void)
{
}

void BubblesBounceOffWall(void)
{
    gCurTask->updateState = 1;
    gCurTask->moveCallback = 0;
    gCurTask->onGround = 0;
    gCurTask->facing = -gCurTask->facing;
    if (gCurTask->facing == 1)
        gCurTask->spriteFlags = gCurTask->spriteFlags & ~SPRITE_FLAG_FLIP_X;
    else
        gCurTask->spriteFlags = gCurTask->spriteFlags | SPRITE_FLAG_FLIP_X;
    ActorSetAttackBox((u32)gBubblesBounceOffWallAttackBox);
    gCurTask->frame = 21;
    TaskYieldTrampoline(12);
    ActorSetAttackBox((u32)gUnk_08742C68);
    if (gCurTask->bubblesRollPhase <= 7)
    {
        gCurTask->frame = 20;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->frame = 21;
        TaskYieldTrampoline(3);
    }
    gCurTask->moveCallback = (u32)ActorMove;
    TaskSetMotionXFacing(abs(gCurTask->velX), 0x5A5A5A5A);
    ActorSetAttackBox((u32)gBubblesAttackBox);
    while (1)
    {
        BubblesSetRollFrame(gCurTask->bubblesRollPhase);
        switch (gCurTask->bubblesJumpKind)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->facing == 1)
        {
            gCurTask->bubblesRollPhase++;
            if (gCurTask->bubblesRollPhase > 15)
                gCurTask->bubblesRollPhase = 0;
        }
        else
        {
            gCurTask->bubblesRollPhase--;
            if (gCurTask->bubblesRollPhase < 0)
                gCurTask->bubblesRollPhase = 15;
        }
    }
}

void BubblesBounceOffWallUpdate(void)
{
}

void BubblesBounceOffCeiling(void)
{
    gCurTask->updateState = 1;
    gCurTask->moveCallback = 0;
    ActorSetAttackBox((u32)gBubblesBounceOffCeilingAttackBox);
    gCurTask->frame = 17;
    TaskYieldTrampoline(12);
    ActorSetAttackBox((u32)gUnk_08742C30);
    if (gCurTask->bubblesRollPhase <= 7)
    {
        gCurTask->frame = 15;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->frame = 13;
        TaskYieldTrampoline(3);
    }
    gCurTask->moveCallback = (u32)ActorMove;
    ActorSetAttackBox((u32)gBubblesAttackBox);
    while (1)
    {
        BubblesSetRollFrame(gCurTask->bubblesRollPhase);
        switch (gCurTask->bubblesJumpKind)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->velX > 0)
        {
            gCurTask->bubblesRollPhase++;
            if (gCurTask->bubblesRollPhase > 15)
                gCurTask->bubblesRollPhase = 0;
        }
        else
        {
            gCurTask->bubblesRollPhase--;
            if (gCurTask->bubblesRollPhase < 0)
                gCurTask->bubblesRollPhase = 15;
        }
    }
}

void sub_08089530(void)
{
    gCurTask->velY += gCurTask->accelY;
}

void BubblesLand(void)
{
    gCurTask->updateState = 3;
    gCurTask->moveCallback = 0;
    gCurTask->velY = 0;
    ActorSetAttackBox((u32)gBubblesLandAttackBox);
    gCurTask->frame = 18;
    TaskYieldTrampoline(13);
    ActorSetAttackBox((u32)gUnk_08742C4C);
    if (gCurTask->bubblesRollPhase <= 7)
    {
        gCurTask->frame = 16;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->frame = 14;
        TaskYieldTrampoline(3);
    }
    ActorSetState(BUBBLES_STATE_JUMP);
    gCurTask->moveCallback = (u32)ActorMove;
    ActorSetAttackBox((u32)gBubblesAttackBox);
    TaskSleepForever();
}

void BubblesLandUpdate(void)
{
    if (gCurTask->state != BUBBLES_STATE_LAND)
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
}

void BubblesFall(void)
{
    gCurTask->updateState = 4;
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    ActorSetAttackBox((u32)gBubblesAttackBox);
    while (1)
    {
        BubblesSetRollFrame(gCurTask->bubblesRollPhase);
        switch (gCurTask->bubblesJumpKind)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->velX > 0)
        {
            gCurTask->bubblesRollPhase++;
            if (gCurTask->bubblesRollPhase > 15)
                gCurTask->bubblesRollPhase = 0;
        }
        else
        {
            gCurTask->bubblesRollPhase--;
            if (gCurTask->bubblesRollPhase < 0)
                gCurTask->bubblesRollPhase = 15;
        }
    }
}

void BubblesFallUpdate(void)
{
}

void BubblesIdle(void)
{
    gCurTask->updateCallback = (u32)BubblesIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(20);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(4);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
    }
}

void BubblesIdleUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_080896ec(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->state == BUBBLES_STATE_LAND)
            return 0;
        ActorSetState(BUBBLES_STATE_LAND);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 BubblesStartFall(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->state != BUBBLES_STATE_LAND)
            return 0;
        ActorSetState(BUBBLES_STATE_FALL);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 BubblesEnterWater(void)
{
    if (gCurTask->variant != 1)
    {
        ActorStartDrown(-2);
        return 1;
    }
}

s32 BubblesHitWall(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->velX != 0 && gCurTask->velY != 0)
        {
            ActorSetState(BUBBLES_STATE_BOUNCE_OFF_WALL);
            TaskSetEntry(BubblesEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 BubblesHitCeiling(void)
{
    if (gCurTask->variant != 1)
    {
        gCurTask->velY = -gCurTask->velY;
        ActorSetState(BUBBLES_STATE_BOUNCE_OFF_CEILING);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

void BubblesSetRollFrame(u8 a)
{
    gCurTask->frame = gUnk_08742778[a * 2];
    if (gUnk_08742778[a * 2 + 1] != 0)
        gCurTask->spriteFlags = gCurTask->spriteFlags | SPRITE_FLAG_FLIP_X;
    else
        gCurTask->spriteFlags = gCurTask->spriteFlags & ~SPRITE_FLAG_FLIP_X;
}

void Task_Glunk(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gGlunkFrames;
    CallTableEntry(gCurTask->variant, 2, gGlunkVariants);
}

void GlunkInit(void)
{
    gCurTask->updateCallback = (u32)GlunkUpdate;
    ActorSetState(GLUNK_STATE_WAIT);
    CallTableEntry(gCurTask->state, 2, gGlunkStates);
}

void GlunkEnterState(void)
{
    gCurTask->updateCallback = (u32)GlunkUpdate;
    CallTableEntry(gCurTask->state, 2, gGlunkStates);
}

void GlunkUpdate(void)
{
    if ((u8)ActorCollideTerrainFloor() == 0)
        CallTableEntry(gCurTask->updateState, 2, gGlunkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GlunkWait(void)
{
    gCurTask->updateState = GLUNK_STATE_WAIT;
    gCurTask->glunkLoopCount = 0;
    while ((s16)gCurTask->glunkLoopCount < gUnk_087427B0[gCurTask->actorSpawnArg])
    {
        gCurTask->frame = 8;
        TaskYieldTrampoline(28);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(18);
        gCurTask->frame++;
        TaskYieldTrampoline(7);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->glunkLoopCount++;
    }
    ActorSetState(GLUNK_STATE_SHOOT);
    TaskSleepForever();
}

void GlunkWaitUpdate(void)
{
    if (gCurTask->state != GLUNK_STATE_WAIT)
        TaskSetEntry(GlunkEnterState, gCurTaskIdx);
}

void GlunkShoot(void)
{
    struct ActorSpawn sp;
    u8 zero;

    gCurTask->updateState = GLUNK_STATE_SHOOT;
    gCurTask->glunkLoopCount = 0;
    while ((s16)gCurTask->glunkLoopCount < gUnk_087427B2[gCurTask->actorSpawnArg])
    {
        if (gCurTask->actorSpawnArg == 1)
        {
            sp.subtype = 5;
            sp.taskType = TASK_GLUNK_SHOT;
            sp.variant = zero = 0;
            sp.spawnArg = gCurTask->actorSpawnArg;
            sp.x = zero;
            sp.y = -8;
            sp.checkTerrain = 1;
            PlaySfx(195);
            gCurTask->glunkShotSlot = CreateActorFromDescAtOffsetFacing(&sp, 0);
            CreateChildTaskHere(TASK_GLUNK_SHOT_SPRAY, 1);
        }
        gCurTask->frame = 4;
        TaskYieldTrampoline(12);
        gCurTask->glunkLoopCount++;
    }
    ActorSetState(GLUNK_STATE_WAIT);
    TaskSleepForever();
}

void GlunkShootUpdate(void)
{
    if (gCurTask->state != GLUNK_STATE_SHOOT)
        TaskSetEntry(GlunkEnterState, gCurTaskIdx);
}

void Task_GlunkShotSpray(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 12;
    gCurTask->frameTable = gGlunkShotFrames;
    gCurTask->facing = TaskGetParentFacing();
    gCurTask->posY = (gCurTask->pixelY - 8) << 16;
    gCurTask->velY = 0xFFFE0000;
    TaskSetFrame(6);
    TaskYieldTrampoline(6);
    TaskSetFrame(7);
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void GlunkIdle(void)
{
    gCurTask->updateCallback = (u32)GlunkIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        gCurTask->frame = 8;
        TaskYieldTrampoline(34);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(18);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
}

void GlunkIdleUpdate(void)
{
    ActorCollideTerrainFloor();
    ActorCheckHits();
    ActorReactToHit();
}

s32 GlunkLand(void)
{
    if (gCurTask->variant != 1)
    {
        TaskStop();
        return 0;
    }
}

s32 GlunkStartFall(void)
{
    if (gCurTask->variant != 1)
    {
        gCurTask->accelY = 0x1500;
        gCurTask->speedLimitY = 0x30000;
        return 0;
    }
}

s32 GlunkEnterWater(void)
{
    if (gCurTask->variant != 1)
    {
        TaskStop();
        gCurTask->velY = 0x4000;
        return 0;
    }
}

void Task_Slippy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSlippyFrames;
    CallTableEntry(gCurTask->variant, 2, gSlippyVariants);
}

void SlippyInit(void)
{
    gCurTask->updateCallback = (u32)SlippyUpdate;
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
    {
        gCurTask->slippyCollideTerrain = 0;
        TaskFaceNearestPlayer();
        if (gCurTask->facing == 1)
            gCurTask->slippySwimAngle = 0;
        else
            gCurTask->slippySwimAngle = 256;
        TaskSetFrame(7);
        ActorSetState(SLIPPY_STATE_6);
    }
    else
    {
        ActorSetState(SLIPPY_STATE_WAIT);
        gCurTask->onGround = 0;
        gCurTask->slippyCollideTerrain = 1;
    }
    CallTableEntry(gCurTask->state, 11, gSlippyStates);
}

void SlippyEnterState(void)
{
    gCurTask->updateCallback = (u32)SlippyUpdate;
    CallTableEntry(gCurTask->state, 11, gSlippyStates);
}

void SlippyUpdate(void)
{
    if (gCurTask->slippyCollideTerrain != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 11, gSlippyStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 11, gSlippyStateUpdates);
    }
    switch (gCurTask->state)
    {
    case SLIPPY_STATE_LEAP:
    case SLIPPY_STATE_5:
    case SLIPPY_STATE_6:
    case SLIPPY_STATE_7:
    case SLIPPY_STATE_8:
    case SLIPPY_STATE_9:
        gCurTask->onGround = 0;
        break;
    }
    ActorCheckHits();
    ActorReactToHit();
}

void SlippyWait(void)
{
    gCurTask->updateState = SLIPPY_STATE_WAIT;
    gCurTask->slippyCollideTerrain = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(gUnk_08742814[gCurTask->actorSpawnArg]);
    SlippyPickMove(15);
    switch (gUnk_030023D4)
    {
    case 0:
        ActorSetState(SLIPPY_STATE_JUMP);
        TaskSleepForever();
        break;
    case 1:
        ActorSetState(SLIPPY_STATE_HIGH_JUMP);
        TaskSleepForever();
        break;
    case 2:
        ActorSetState(SLIPPY_STATE_JUMP);
        TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 3:
        ActorSetState(SLIPPY_STATE_HIGH_JUMP);
        TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 4:
        ActorSetState(SLIPPY_STATE_JUMP);
        TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskTurnAroundAndReverseX();
        TaskSleepForever();
        break;
    case 5:
        ActorSetState(SLIPPY_STATE_LOOK_AROUND);
        break;
    }
    TaskSleepForever();
}

void SlippyWaitUpdate(void)
{
    if (gCurTask->state != SLIPPY_STATE_WAIT)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyLookAround(void)
{
    gCurTask->updateState = SLIPPY_STATE_LOOK_AROUND;
    gCurTask->onGround = 1;
    while (1)
    {
        gUnk_030023D4 = RandomRange(3);
        gCurTask->slippyLoopCount = 0;
        while ((s16)gCurTask->slippyLoopCount < gUnk_08742820[gCurTask->actorSpawnArg])
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            if (gUnk_030023D4 != 0)
            {
                if (gCurTask->spriteFlags & SPRITE_FLAG_FLIP_X)
                    gCurTask->spriteFlags = gCurTask->spriteFlags & ~SPRITE_FLAG_FLIP_X;
                else
                    gCurTask->spriteFlags = gCurTask->spriteFlags | SPRITE_FLAG_FLIP_X;
            }
            else
            {
                TaskSetFrame(5);
            }
            TaskYieldTrampoline(16);
            gCurTask->slippyLoopCount++;
        }
        SlippyPickMove(0);
        switch (gUnk_030023D4)
        {
        case 0:
            ActorSetState(SLIPPY_STATE_JUMP);
            TaskSleepForever();
            break;
        case 1:
            ActorSetState(SLIPPY_STATE_HIGH_JUMP);
            TaskSleepForever();
            break;
        case 2:
            ActorSetState(SLIPPY_STATE_JUMP);
            TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 3:
            ActorSetState(SLIPPY_STATE_HIGH_JUMP);
            TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 4:
            ActorSetState(SLIPPY_STATE_JUMP);
            TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskTurnAroundAndReverseX();
            TaskSleepForever();
            break;
        case 5:
            break;
        }
    }
}

void SlippyLookAroundUpdate(void)
{
    if (gCurTask->state != SLIPPY_STATE_LOOK_AROUND)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyJump(void)
{
    gCurTask->updateState = SLIPPY_STATE_JUMP;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    PlaySfx(188);
    TaskSetFrame(8);
    gCurTask->velY = 0xFFFB0000;
    gCurTask->accelY = 0x8000;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->velY < 0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void SlippyJumpUpdate(void)
{
}

void SlippyHighJump(void)
{
    gCurTask->updateState = SLIPPY_STATE_HIGH_JUMP;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    PlaySfx(188);
    TaskSetFrame(8);
    gCurTask->velY = 0xFFFA0000;
    gCurTask->accelY = 0x4000;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->velY < 0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void SlippyHighJumpUpdate(void)
{
}

void SlippyLeap(void)
{
    gCurTask->updateState = SLIPPY_STATE_LEAP;
    gCurTask->slippyCollideTerrain = 0;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(5);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(4);
    TaskFaceNearestPlayer();
    gCurTask->velY = 0xFFFB0000;
    gCurTask->accelY = 0x4000;
    TaskSetMotionXFacing(gSlippyJumpSpeeds[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    TaskSetFrame(8);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void SlippyLeapUpdate(void)
{
    u8 r;

    r = IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY);
    if (r == 0)
    {
        gCurTask->slippyCollideTerrain = 1;
        gCurTask->onGround = r;
    }
}

void SlippyState5(void)
{
    gCurTask->updateState = SLIPPY_STATE_5;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    gCurTask->velY = 0x8000;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    if (gCurTask->facing > 0)
        gCurTask->slippySwimAngle = 64;
    else
        gCurTask->slippySwimAngle = 192;
    TaskSetFrame(5);
    TaskYieldTrampoline(40);
    TaskSetFrame(7);
    ActorSetState(SLIPPY_STATE_6);
    TaskSleepForever();
}

void SlippyState5Update(void)
{
    if (gCurTask->state != SLIPPY_STATE_5)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyState6(void)
{
    gCurTask->updateState = SLIPPY_STATE_6;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    AngleToVector((s16)gCurTask->slippySwimAngle, 128);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(gUnk_08742822[gCurTask->actorSpawnArg]);
    if (RandomRange(4) != 0)
        TaskYieldTrampoline(30);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    switch (RandomRange(3))
    {
    case 0:
        ActorSetState(SLIPPY_STATE_7);
        break;
    case 1:
        ActorSetState(SLIPPY_STATE_8);
        break;
    case 2:
        ActorSetState(SLIPPY_STATE_9);
        break;
    }
    TaskSleepForever();
}

void SlippyState6Update(void)
{
    if (gCurTask->state != SLIPPY_STATE_6)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(SLIPPY_STATE_LEAP);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState7(void)
{
    gCurTask->updateState = SLIPPY_STATE_7;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    SlippySetSwimAngleToPlayer(0);
    TaskSetFrame(7);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742824[0]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742824[2]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742824[4]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    ActorSetState(SLIPPY_STATE_6);
    TaskSleepForever();
}

void SlippyState7Update(void)
{
    if (gCurTask->state != SLIPPY_STATE_7)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(SLIPPY_STATE_LEAP);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState8(void)
{
    gCurTask->updateState = SLIPPY_STATE_8;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    while (1)
    {
        TaskStop();
        TaskSetFrame(9);
        TaskYieldTrampoline(gUnk_08742830[gCurTask->actorSpawnArg]);
        SlippySetSwimAngleToPlayer(8);
        TaskSetFrame(7);
        AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742834[0]);
        gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742834[2]);
        gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742834[4]);
        gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742834[6]);
        gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        TaskYieldTrampoline(8);
        if (RandomRange(3) != 0)
        {
            ActorSetState(SLIPPY_STATE_6);
            TaskSleepForever();
        }
    }
}

void SlippyState8Update(void)
{
    if (gCurTask->state != SLIPPY_STATE_8)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(SLIPPY_STATE_LEAP);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState9(void)
{
    gCurTask->updateState = SLIPPY_STATE_9;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(gUnk_08742830[gCurTask->actorSpawnArg]);
    SlippySetSwimAngleToPlayer(16);
    TaskSetFrame(7);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742844[0]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742844[2]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742844[4]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->slippySwimAngle, gUnk_08742844[6]);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    TaskYieldTrampoline(8);
    if (RandomRange(3) != 0)
        ActorSetState(SLIPPY_STATE_6);
    else
        ActorSetState(SLIPPY_STATE_8);
    TaskSleepForever();
}

void SlippyState9Update(void)
{
    if (gCurTask->state != SLIPPY_STATE_9)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(SLIPPY_STATE_LEAP);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyFall(void)
{
    gCurTask->updateState = SLIPPY_STATE_FALL;
    gCurTask->slippyCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(4);
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    TaskSleepForever();
}

void SlippyFallUpdate(void)
{
}

void SlippyIdle(void)
{
    gCurTask->updateCallback = (u32)SlippyIdleUpdate;
    ActorSetAttackBox((u32)gIdleAttackBox);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSleepForever();
}

void SlippyIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void SlippyPickMove(u8 a)
{
    s32 n;
    u8 v;

    n = gCurTask->slippyMovePickCount + 1;
    gCurTask->slippyMovePickCount = n;
    if (a == 0)
        v = gUnk_08742854[n & 1];
    else
        v = a;
    if ((gCurTask->slippyMovePickCount & 1) == 0)
        PickWeightedRandomIndex(gUnk_08742856, v);
    else
        PickWeightedRandomIndex(gUnk_0874285C, v);
}

void PickWeightedRandomIndex(u8 *p, s32 b)
{
    s32 r;
    s32 i;

    r = RandomRange(b + 1);
    i = 0;
    while (p[i] < r)
        i++;
    gUnk_030023B4 = r;
    gUnk_030023D4 = i;
}

void SlippySetSwimAngleToPlayer(s32 a)
{
    u32 v;

    gCurTask->slippySwimAngle = v = gUnk_08742862[(u16)TaskGetAngleToNearestPlayer(0) + a];
    if (v < 128 || v > 384)
        gCurTask->facing = 1;
    else if (v > 128 && v < 384)
        gCurTask->facing = -1;
}

s32 SlippyLand(void)
{
    if (gCurTask->variant != 1)
    {
        switch (gCurTask->state)
        {
        case SLIPPY_STATE_5:
        case SLIPPY_STATE_6:
        case SLIPPY_STATE_7:
        case SLIPPY_STATE_8:
        case SLIPPY_STATE_9:
            gCurTask->slippySwimAngle = 512 - gCurTask->slippySwimAngle;
            gCurTask->velY = -gCurTask->velY;
            return 0;
        case SLIPPY_STATE_WAIT:
        case SLIPPY_STATE_LOOK_AROUND:
        case SLIPPY_STATE_JUMP:
        case SLIPPY_STATE_HIGH_JUMP:
        case SLIPPY_STATE_LEAP:
        case SLIPPY_STATE_FALL:
            ActorSetState(SLIPPY_STATE_WAIT);
            TaskSetEntry(SlippyEnterState, gCurTaskIdx);
            return 1;
        default:
            return 0;
        }
    }
}

s32 SlippyStartFall(void)
{
    if (gCurTask->variant != 1)
    {
        switch (gCurTask->state)
        {
        case SLIPPY_STATE_WAIT:
        case SLIPPY_STATE_LOOK_AROUND:
            ActorSetState(SLIPPY_STATE_FALL);
            TaskSetEntry(SlippyEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 SlippyEnterWater(void)
{
    if (gCurTask->variant != 1)
    {
        ActorSetState(SLIPPY_STATE_5);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 SlippyHitWall(void)
{
    s32 n;

    if (gCurTask->variant != 1)
    {
        switch (gCurTask->state)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            n = 768 - gCurTask->slippySwimAngle;
            gCurTask->slippySwimAngle = n;
            if (n > 0x1FF)
                gCurTask->slippySwimAngle = n - 0x200;
            /* fallthrough */
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            TaskTurnAroundAndReverseX();
            return 0;
        default:
            return 0;
        }
    }
}

s32 SlippyHitCeiling(void)
{
    if (gCurTask->variant != 1)
    {
        switch (gCurTask->state)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            gCurTask->velY = 0;
            return 0;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gCurTask->slippySwimAngle = 512 - gCurTask->slippySwimAngle;
            gCurTask->velY = -gCurTask->velY;
            return 0;
        default:
            return 0;
        }
    }
}
