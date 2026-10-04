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
    ActorSetState(0);
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

    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08742088[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        t = gCurTask;
        t->velY = gUnk_08742090[0];
        t->accelY = -gUnk_08742098[0];
    }
    else
    {
        t = gCurTask;
        t->velY = -gUnk_08742090[0];
        t->accelY = gUnk_08742098[0];
    }
    t = gCurTask;
    t->unk30 = 4;
    t->unk34 = 40;
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

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->accelY < 0)
    {
        if (n < 0)
            return;
        t->velY = -gUnk_08742090[0];
        t->accelY = gUnk_08742098[0];
    }
    else
    {
        t->velY = gUnk_08742090[0];
        t->accelY = -gUnk_08742098[0];
    }
    gCurTask->unk34 = 40;
}

void BrontoBurtVariant1(void)
{
    gCurTask->updateCallback = (u32)sub_08086320;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087420A0);
}

void sub_080862fc(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_08086320;
    CallTableEntry(t->state, 1, gUnk_087420A0);
}

void sub_08086320(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087420A4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086344(void)
{
    struct Task *t;
    s16 *p;

    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08742088[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        t = gCurTask;
        t->velY = gUnk_08742090[1];
        t->accelY = -gUnk_08742098[1];
    }
    else
    {
        t = gCurTask;
        t->velY = -gUnk_08742090[1];
        t->accelY = gUnk_08742098[1];
    }
    gCurTask->unk34 = 40;
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

    if (--gCurTask->unk34 == 20)
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
    if (gCurTask->unk34 != 0)
        return;
    if (gCurTask->accelY < 0)
    {
        gCurTask->velY = -gUnk_08742090[1];
        gCurTask->accelY = gUnk_08742098[1];
    }
    else
    {
        gCurTask->velY = gUnk_08742090[1];
        gCurTask->accelY = -gUnk_08742098[1];
    }
    gCurTask->unk34 = 40;
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
    gCurTask->unk34 = 0;
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
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0x8000;
    k = gCurTask->unk74;
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

void sub_080867b8(void)
{
    if (gCurTask->variant != 2)
    {
        TaskSetEntry(BrontoBurtEnterVariant, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
    if (gCurTask->unk34 != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->unk34 = 1;
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
    gCurTask->unk2C = (k << 7) + 64;
    AngleToVector(gCurTask->unk2C,
                 gUnk_087420F4[gCurTask->unk74] << 8 >> 16);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    gCurTask->unk34 = 0;
    ActorSetState(0);
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
    if (sub_08086f54() != 0)
    {
        AngleToVector((s16)gCurTask->unk2C,
                     gUnk_087420F4[gCurTask->unk74] << 8 >> 16);
        v = gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        if (v > 0)
            gCurTask->facing = 1;
        else if (v < 0)
            gCurTask->facing = -1;
        gCurTask->unk34 = 20;
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    }
    gCurTask->onGround = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void BrontoBurtDiagonal(void)
{
    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    while (1)
    {
        if (gCurTask->unk34 == 0)
            gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
        TaskYieldTrampoline(1);
    }
}

void BrontoBurtDiagonalState0Update(void)
{
    gCurTask->unk34--;
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void BrontoBurtChaseInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtChaseUpdate;
    ActorSetState(0);
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
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 384;
    do
    {
        switch (gCurTask->unk34)
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
    } while (gCurTask->unk30 != 0);
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
    if (gCurTask->unk30 != 0)
    {
        gCurTask->unk30--;
        TaskFaceNearestPlayer();
        gCurTask->unk28++;
        if (gCurTask->unk28 == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_0874210C[gCurTask->unk74],
                         gUnk_0874210C[gCurTask->unk74 + 4]);
            gCurTask->unk34 = gUnk_030023D4;
            gCurTask->unk28 = 0;
        }
    }
}

void BrontoBurtTakeOffInit(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtTakeOffUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gBrontoBurtTakeOffStates);
}

void BrontoBurtTakeOffEnterState(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtTakeOffUpdate;
    CallTableEntry(gCurTask->state, 3, gBrontoBurtTakeOffStates);
}

void BrontoBurtTakeOffUpdate(void)
{
    if (gCurTask->unk34 != 0)
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
    gCurTask->updateState = 0;
    gCurTask->unk34 = 1;
    gCurTask->onGround = 1;
    gCurTask->unk28 = ActorStartAnim(gUnk_08742144);
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
    ActorSetState(1);
    TaskSleepForever();
}

void BrontoBurtTakeOffWaitUpdate(void)
{
    if (gCurTask->state != 0)
    {
        TaskSetEntry(BrontoBurtTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void BrontoBurtTakeOff(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk34 = 0;
    PlaySfx(187);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    ActorSetState(2);
    TaskSleepForever();
}

void BrontoBurtTakeOffState1Update(void)
{
    if (gCurTask->state != 1)
    {
        TaskSetEntry(BrontoBurtTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void BrontoBurtTakeOffState2(void)
{
    gCurTask->updateState = 2;
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    switch (gCurTask->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
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
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
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
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void BrontoBurtIdle(void)
{
    gCurTask->updateCallback = (u32)BrontoBurtIdleUpdate;
    ActorSetAttackBox((u32)gUnk_0873F500);
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

s32 sub_08086f54(void)
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
    ActorSetState(0);
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

    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_087425B8[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        gCurTask->velY = gUnk_087425C0[0];
        gCurTask->accelY = -gUnk_087425C8[0];
    }
    else
    {
        gCurTask->velY = -gUnk_087425C0[0];
        gCurTask->accelY = gUnk_087425C8[0];
    }
    gCurTask->unk30 = 4;
    gCurTask->unk34 = 40;
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

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->accelY < 0)
    {
        if (n < 0)
            return;
        t->velY = -gUnk_087425C0[0];
        t->accelY = gUnk_087425C8[0];
    }
    else
    {
        t->velY = gUnk_087425C0[0];
        t->accelY = -gUnk_087425C8[0];
    }
    gCurTask->unk34 = 40;
}

void TwizzyVariant1(void)
{
    gCurTask->updateCallback = (u32)sub_080872bc;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087425D0);
}

void sub_08087298(void)
{
    gCurTask->updateCallback = (u32)sub_080872bc;
    CallTableEntry(gCurTask->state, 1, gUnk_087425D0);
}

void sub_080872bc(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087425D4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080872e0(void)
{
    s16 *p;

    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_087425B8[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
    {
        gCurTask->velY = gUnk_087425C0[1];
        gCurTask->accelY = -gUnk_087425C8[1];
    }
    else
    {
        gCurTask->velY = -gUnk_087425C0[1];
        gCurTask->accelY = gUnk_087425C8[1];
    }
    gCurTask->unk34 = 40;
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

    if (--gCurTask->unk34 == 20)
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
    if (gCurTask->unk34 != 0)
        return;
    if (gCurTask->accelY < 0)
    {
        gCurTask->velY = -gUnk_087425C0[1];
        gCurTask->accelY = gUnk_087425C8[1];
    }
    else
    {
        gCurTask->velY = gUnk_087425C0[1];
        gCurTask->accelY = -gUnk_087425C8[1];
    }
    gCurTask->unk34 = 40;
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
    gCurTask->unk34 = 0;
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
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->velY = 0x8000;
    k = gCurTask->unk74;
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

void sub_08087724(void)
{
    if (gCurTask->variant != 2)
    {
        TaskSetEntry(TwizzyEnterVariant, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
    if (gCurTask->unk34 != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->unk34 = 1;
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
    gCurTask->unk2C = (k << 7) + 64;
    AngleToVector(gCurTask->unk2C,
                 gUnk_08742600[gCurTask->unk74] << 8 >> 16);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    gCurTask->unk34 = 0;
    ActorSetState(0);
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
    if (sub_08086f54() != 0)
    {
        AngleToVector((s16)gCurTask->unk2C,
                     gUnk_08742600[gCurTask->unk74] << 8 >> 16);
        v = gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
        if (v > 0)
            gCurTask->facing = 1;
        else if (v < 0)
            gCurTask->facing = -1;
        gCurTask->unk34 = 20;
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    }
    gCurTask->onGround = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void TwizzyDiagonal(void)
{
    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    while (1)
    {
        if (gCurTask->unk34 == 0)
            gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
        TaskYieldTrampoline(1);
    }
}

void TwizzyDiagonalState0Update(void)
{
    gCurTask->unk34--;
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void TwizzyChaseInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyChaseUpdate;
    ActorSetState(0);
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
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 384;
    do
    {
        switch (gCurTask->unk34)
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
    } while (gCurTask->unk30 != 0);
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
    if (gCurTask->unk30 != 0)
    {
        gCurTask->unk30--;
        TaskFaceNearestPlayer();
        gCurTask->unk28++;
        if (gCurTask->unk28 == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_08742614[gCurTask->unk74],
                         gUnk_08742614[gCurTask->unk74 + 4]);
            gCurTask->unk34 = gUnk_030023D4;
            gCurTask->unk28 = 0;
        }
    }
}

void TwizzyTakeOffInit(void)
{
    gCurTask->updateCallback = (u32)TwizzyTakeOffUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gTwizzyTakeOffStates);
}

void TwizzyTakeOffEnterState(void)
{
    gCurTask->updateCallback = (u32)TwizzyTakeOffUpdate;
    CallTableEntry(gCurTask->state, 3, gTwizzyTakeOffStates);
}

void TwizzyTakeOffUpdate(void)
{
    if (gCurTask->unk34 != 0)
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
    gCurTask->updateState = 0;
    gCurTask->unk34 = 1;
    gCurTask->onGround = 1;
    gCurTask->unk28 = ActorStartAnim(gUnk_08742634);
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
    ActorSetState(1);
    TaskSleepForever();
}

void TwizzyTakeOffWaitUpdate(void)
{
    if (gCurTask->state != 0)
    {
        TaskSetEntry(TwizzyTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void TwizzyTakeOff(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk34 = 0;
    PlaySfx(187);
    gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    ActorSetState(2);
    TaskSleepForever();
}

void TwizzyTakeOffState1Update(void)
{
    if (gCurTask->state != 1)
    {
        TaskSetEntry(TwizzyTakeOffEnterState, gCurTaskIdx);
        return;
    }
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void TwizzyTakeOffState2(void)
{
    gCurTask->updateState = 2;
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    switch (gCurTask->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
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
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
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
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void TwizzyVariant6(void)
{
    gCurTask->updateCallback = (u32)sub_08087e84;
    gCurTask->onGround = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_08742654);
}

void sub_08087e60(void)
{
    gCurTask->updateCallback = (u32)sub_08087e84;
    CallTableEntry(gCurTask->state, 3, gUnk_08742654);
}

void sub_08087e84(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_08742660);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08087eb4(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08087ef0(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_08087e60, gCurTaskIdx);
}

void sub_08087f18(void)
{
    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    PlaySfx(187);
    TaskSetMotionY(0xFFFE0000, 0x800, 0x30000);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 3);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08087f88(void)
{
}

void sub_08087f8c(void)
{
    gCurTask->updateState = 2;
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

void sub_08087fc8(void)
{
}

void TwizzyVariant7(void)
{
    gCurTask->updateCallback = (u32)sub_08088024;
    gCurTask->onGround = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 6, gUnk_0874266C);
}
