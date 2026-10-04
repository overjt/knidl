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
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void AngleToVector(s16 t, s16 mag);

void sub_08088000(void)
{
    gCurTask->updateCallback = (u32)sub_08088024;
    CallTableEntry(gCurTask->state, 6, gUnk_0874266C);
}

void sub_08088024(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 6, gUnk_08742684);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088054(void)
{
    gCurTask->updateState = 0;
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
        ActorSetState(1);
        break;
    case 2:
    case 3:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        ActorSetState(1);
        break;
    case 4:
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        ActorSetState(1);
        break;
    case 5:
        ActorSetState(2);
        break;
    case 6:
    case 7:
        ActorSetState(3);
        break;
    }
    TaskSleepForever();
}

void sub_080880fc(void)
{
    s32 v;

    if (gCurTask->state != 0)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
    v = gUnk_0874269C[gCurTask->actorSpawnArg];
    if (v > abs(TaskGetNearestPlayerDx()))
    {
        v = gUnk_087426A4[gCurTask->actorSpawnArg];
        if (v > abs(TaskGetNearestPlayerDy()))
        {
            ActorSetState(4);
            TaskSetEntry(sub_08088000, gCurTaskIdx);
        }
    }
}

void sub_080881a0(void)
{
    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    if (RandomRange(2) != 0)
        TaskTurnAroundAndReverseX();
    TaskSetMotionY(0xFFFE8000, 0x4000, 0x30000);
    TaskSleepForever();
}

void sub_080881e0(void)
{
}

void sub_080881e4(void)
{
    gCurTask->updateState = 2;
    TaskYieldTrampoline(32);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088208(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
}

void sub_08088230(void)
{
    gCurTask->updateState = 3;
    TaskYieldTrampoline(64);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088254(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
}

void sub_0808827c(void)
{
    gCurTask->updateState = 4;
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

void sub_080882f0(void)
{
    if (gCurTask->variant != 7)
    {
        gCurTask->updateCallback = 0;
        TaskSetEntry(Task_Twizzy, gCurTaskIdx);
    }
}

void sub_08088320(void)
{
    gCurTask->updateState = 5;
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

void sub_0808835c(void)
{
}

void TwizzyHoverInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyHoverUpdate;
    gCurTask->onGround = 0;
    ActorSetState(0);
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
    gCurTask->updateState = 0;
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

void sub_08088478(void)
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
    ActorSetAttackBox((u32)gUnk_0873F500);
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

s32 sub_080884e4(void)
{
    if (gCurTask->variant != 9)
    {
        switch (gCurTask->variant)
        {
        case 6:
            ActorSetState(0);
            TaskSetEntry(sub_08087e60, gCurTaskIdx);
            return 1;
        case 7:
            ActorSetState(0);
            TaskSetEntry(sub_08088000, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08088540(void)
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
                ActorSetState(5);
                TaskSetEntry(sub_08088000, gCurTaskIdx);
                return 1;
            }
            return 0;
        }
        return 0;
    }
}

s32 sub_08088590(void)
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

s32 sub_080885c0(void)
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
        ActorSetState(3);
        gCurTask->onGround = 0;
        CallTableEntry(gCurTask->state, 5, gSquishyWalkStates);
    }
    ActorSetState(0);
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
    gCurTask->updateState = 0;
    gCurTask->unk34 = 100;
    TaskSetMotionXFacing(gUnk_087426EC[gCurTask->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2 + 1]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2]);
        gCurTask->frame++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2 + 1]);
    }
}

void SquishyWalkState0Update(void)
{
    s32 n = gCurTask->unk34 - 1;

    gCurTask->unk34 = n;
    switch (n)
    {
    case 20:
    case 40:
    case 60:
    case 80:
        if (RandomRange(4) == 0)
        {
            ActorSetState(1);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        }
        break;
    case 0:
        ActorSetState(1);
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        break;
    }
}

void SquishyWalkState1(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk28 = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(24);
    PlaySfx(188);
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void SquishyWalkState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
}

void SquishyWalkFall(void)
{
    gCurTask->updateState = 2;
    gCurTask->unk28 = 0;
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    TaskSetFrame(6);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void SquishyWalkFallUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
}

void SquishyWalkState3(void)
{
    gCurTask->updateState = 3;
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
    gCurTask->updateState = 4;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void SquishyWalkState4Update(void)
{
    if (gCurTask->state != 4)
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
    if (gCurTask->unk28 != 0)
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
    gCurTask->unk28 = 0;
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
    gCurTask->unk28 = 0;
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
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_08088ca0(void)
{
}

void sub_08088ca4(void)
{
    gCurTask->updateState = 2;
    gCurTask->unk28 = 0;
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

void SquishyVariant2(void)
{
    gCurTask->updateCallback = (u32)sub_08088d7c;
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
    CallTableEntry(gCurTask->state, 3, gUnk_0874271C);
}

void sub_08088d58(void)
{
    gCurTask->updateCallback = (u32)sub_08088d7c;
    CallTableEntry(gCurTask->state, 3, gUnk_0874271C);
}

void sub_08088d7c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_08742728);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088dac(void)
{
    struct Task *t;
    struct Task *u;
    s32 a, d;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    u = &gTasks[TaskFindNearestPlayer()];
    t = gCurTask;
    a = t->posY;
    d = (a >> 16) - (u->posY >> 16);
    if (d > 0)
        t->unk34 = 37;
    else if (d < 0)
        t->unk34 = 43;
    else if ((a & 0xFF) - (u->posY & 0xFF) >= 0
                 ? (a & 0xFF) - (u->posY & 0xFF) <= 15
                 : (u->posY & 0xFF) - (a & 0xFF) <= 15)
        gCurTask->unk34 = 37;
    else
        gCurTask->unk34 = 40;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(gUnk_08742734[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2000, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08088e78(void)
{
    if (--gCurTask->unk34 == 0)
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
}

void sub_08088ea4(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    gCurTask->variant = 0;
    TaskSleepForever();
}

void sub_08088ed4(void)
{
    if (gCurTask->variant != 2)
        TaskSetEntry(Task_Squishy, gCurTaskIdx);
}

void sub_08088efc(void)
{
    gCurTask->updateState = 1;
    gCurTask->velY = 0;
    TaskYieldTrampoline(24);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088f24(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
}

void SquishyIdle(void)
{
    gCurTask->updateCallback = (u32)SquishyIdleUpdate;
    ActorSetAttackBox((u32)gUnk_0873F500);
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

s32 sub_08088fc0(void)
{
    if (gCurTask->variant != 3)
    {
        switch (gCurTask->variant)
        {
        case 0:
            gCurTask->unk28 = 1;
            return 0;
        case 1:
            ActorSetState(2);
            TaskSetEntry(sub_08088aec, gCurTaskIdx);
            return 1;
        case 2:
            ActorSetState(2);
            TaskSetEntry(sub_08088d58, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08089024(void)
{
    if (gCurTask->variant != 3)
    {
        switch (gCurTask->variant)
        {
        default:
            return 0;
        case 0:
            ActorSetState(2);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        }
    }
}

s32 sub_08089064(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->u8C.actor->hitReactions = (u32)gUnk_08742E5C;
        switch (gCurTask->variant)
        {
        case 0:
            ActorSetState(3);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        case 2:
            gCurTask->variant = 0;
            ActorSetState(3);
            TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080890d4(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->u8C.actor->hitReactions = (u32)gUnk_08742E50;
        if (gCurTask->variant != 0)
            return 0;
        ActorSetState(4);
        TaskSetEntry(SquishyWalkEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 sub_08089120(void)
{
    if (gCurTask->variant != 3)
    {
        TaskTurnAroundAndReverseX();
        return 0;
    }
}

s32 sub_0808913c(void)
{
    if (gCurTask->variant != 3)
    {
        gCurTask->velY = 0;
        if (gCurTask->variant != 2)
            return 0;
        ActorSetState(1);
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
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
    ActorSetState(0);
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
        sub_08089808(gCurTask->bubblesRollPhase);
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

void BubblesState1(void)
{
    gCurTask->updateState = 1;
    gCurTask->moveCallback = 0;
    gCurTask->onGround = 0;
    gCurTask->facing = -gCurTask->facing;
    if (gCurTask->facing == 1)
        gCurTask->spriteFlags = gCurTask->spriteFlags & 0x7FFF;
    else
        gCurTask->spriteFlags = gCurTask->spriteFlags | 0x8000;
    ActorSetAttackBox((u32)gUnk_08742CBC);
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
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->bubblesRollPhase);
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

void BubblesState1Update(void)
{
}

void BubblesState2(void)
{
    gCurTask->updateState = 1;
    gCurTask->moveCallback = 0;
    ActorSetAttackBox((u32)gUnk_08742C84);
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
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->bubblesRollPhase);
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
    ActorSetAttackBox((u32)gUnk_08742CA0);
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
    ActorSetState(0);
    gCurTask->moveCallback = (u32)ActorMove;
    ActorSetAttackBox((u32)gUnk_08742C14);
    TaskSleepForever();
}

void BubblesLandUpdate(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
}

void BubblesFall(void)
{
    gCurTask->updateState = 4;
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->bubblesRollPhase);
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
    ActorSetAttackBox((u32)gUnk_0873F500);
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
        if (gCurTask->state == 3)
            return 0;
        ActorSetState(3);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808972c(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->state != 3)
            return 0;
        ActorSetState(4);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808976c(void)
{
    if (gCurTask->variant != 1)
    {
        ActorStartDrown(-2);
        return 1;
    }
}

s32 sub_0808978c(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->velX != 0 && gCurTask->velY != 0)
        {
            ActorSetState(1);
            TaskSetEntry(BubblesEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080897d0(void)
{
    if (gCurTask->variant != 1)
    {
        gCurTask->velY = -gCurTask->velY;
        ActorSetState(2);
        TaskSetEntry(BubblesEnterState, gCurTaskIdx);
        return 1;
    }
}

void sub_08089808(u8 a)
{
    gCurTask->frame = gUnk_08742778[a * 2];
    if (gUnk_08742778[a * 2 + 1] != 0)
        gCurTask->spriteFlags = gCurTask->spriteFlags | 0x8000;
    else
        gCurTask->spriteFlags = gCurTask->spriteFlags & 0x7FFF;
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
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gGlunkStates);
}

void GlunkEnterState(void)
{
    gCurTask->updateCallback = (u32)GlunkUpdate;
    CallTableEntry(gCurTask->state, 2, gGlunkStates);
}

void GlunkUpdate(void)
{
    if ((u8)sub_08069888() == 0)
        CallTableEntry(gCurTask->updateState, 2, gGlunkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void GlunkWait(void)
{
    gCurTask->updateState = 0;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_087427B0[gCurTask->unk74])
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
        gCurTask->unk6C++;
    }
    ActorSetState(1);
    TaskSleepForever();
}

void GlunkWaitUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(GlunkEnterState, gCurTaskIdx);
}

void GlunkShoot(void)
{
    struct ActorSpawn sp;
    u8 zero;

    gCurTask->updateState = 1;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_087427B2[gCurTask->unk74])
    {
        if (gCurTask->unk74 == 1)
        {
            sp.subtype = 5;
            sp.taskType = 107;
            sp.variant = zero = 0;
            sp.spawnArg = gCurTask->unk74;
            sp.x = zero;
            sp.y = -8;
            sp.checkTerrain = 1;
            PlaySfx(195);
            gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
            CreateChildTaskHere(219, 1);
        }
        gCurTask->frame = 4;
        TaskYieldTrampoline(12);
        gCurTask->unk6C++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void GlunkShootUpdate(void)
{
    if (gCurTask->state != 1)
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
    ActorSetAttackBox((u32)gUnk_0873F500);
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
    sub_08069888();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_08089bf0(void)
{
    if (gCurTask->variant != 1)
    {
        TaskStop();
        return 0;
    }
}

s32 sub_08089c0c(void)
{
    if (gCurTask->variant != 1)
    {
        gCurTask->accelY = 0x1500;
        gCurTask->speedLimitY = 0x30000;
        return 0;
    }
}

s32 sub_08089c30(void)
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
        ActorSetState(6);
    }
    else
    {
        ActorSetState(0);
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
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        gCurTask->onGround = 0;
        break;
    }
    ActorCheckHits();
    ActorReactToHit();
}

void SlippyState0(void)
{
    gCurTask->updateState = 0;
    gCurTask->slippyCollideTerrain = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(gUnk_08742814[gCurTask->actorSpawnArg]);
    SlippyPickMove(15);
    switch (gUnk_030023D4)
    {
    case 0:
        ActorSetState(2);
        TaskSleepForever();
        break;
    case 1:
        ActorSetState(3);
        TaskSleepForever();
        break;
    case 2:
        ActorSetState(2);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 3:
        ActorSetState(3);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 4:
        ActorSetState(2);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        TaskTurnAroundAndReverseX();
        TaskSleepForever();
        break;
    case 5:
        ActorSetState(1);
        break;
    }
    TaskSleepForever();
}

void SlippyState0Update(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyState1(void)
{
    gCurTask->updateState = 1;
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
                if (gCurTask->spriteFlags & 0x8000)
                    gCurTask->spriteFlags = gCurTask->spriteFlags & 0x7FFF;
                else
                    gCurTask->spriteFlags = gCurTask->spriteFlags | 0x8000;
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
            ActorSetState(2);
            TaskSleepForever();
            break;
        case 1:
            ActorSetState(3);
            TaskSleepForever();
            break;
        case 2:
            ActorSetState(2);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 3:
            ActorSetState(3);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 4:
            ActorSetState(2);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskTurnAroundAndReverseX();
            TaskSleepForever();
            break;
        case 5:
            break;
        }
    }
}

void SlippyState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyState2(void)
{
    gCurTask->updateState = 2;
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

void SlippyState2Update(void)
{
}

void SlippyState3(void)
{
    gCurTask->updateState = 3;
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

void SlippyState3Update(void)
{
}

void SlippyState4(void)
{
    gCurTask->updateState = 4;
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
    TaskSetMotionXFacing(gUnk_08742818[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    TaskSetFrame(8);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void SlippyState4Update(void)
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
    gCurTask->updateState = 5;
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
    ActorSetState(6);
    TaskSleepForever();
}

void SlippyState5Update(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
}

void SlippyState6(void)
{
    gCurTask->updateState = 6;
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
        ActorSetState(7);
        break;
    case 1:
        ActorSetState(8);
        break;
    case 2:
        ActorSetState(9);
        break;
    }
    TaskSleepForever();
}

void SlippyState6Update(void)
{
    if (gCurTask->state != 6)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState7(void)
{
    gCurTask->updateState = 7;
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
    ActorSetState(6);
    TaskSleepForever();
}

void SlippyState7Update(void)
{
    if (gCurTask->state != 7)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState8(void)
{
    gCurTask->updateState = 8;
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
            ActorSetState(6);
            TaskSleepForever();
        }
    }
}

void SlippyState8Update(void)
{
    if (gCurTask->state != 8)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyState9(void)
{
    gCurTask->updateState = 9;
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
        ActorSetState(6);
    else
        ActorSetState(8);
    TaskSleepForever();
}

void SlippyState9Update(void)
{
    if (gCurTask->state != 9)
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    if (sub_08021c14(gCurTask->pixelX,
                     (u16)gCurTask->pixelY - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(SlippyEnterState, gCurTaskIdx);
    }
}

void SlippyFall(void)
{
    gCurTask->updateState = 10;
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
    ActorSetAttackBox((u32)gUnk_0873F500);
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
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gCurTask->slippySwimAngle = 512 - gCurTask->slippySwimAngle;
            gCurTask->velY = -gCurTask->velY;
            return 0;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 10:
            ActorSetState(0);
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
        case 0:
        case 1:
            ActorSetState(10);
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
        ActorSetState(5);
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
