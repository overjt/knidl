/* game_code_and_rodata 0x0808AA68-0x0808CCE8 (issue #80, module M23 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808AA68 0x0808CCE8 src/enemy_8aa68.c --newpb
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
extern u32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);

void Task_Blipper(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gBlipperFrames;
    gCurTask->onGround = 0;
    if (gCurTask->variant == BLIPPER_VARIANT_IDLE)
        BlipperIdle();
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
        CallTableEntry(gCurTask->variant, 6, gBlipperVariants);
    sub_0808b5b4();
}

void BlipperChaseInit(void)
{
    gCurTask->updateCallback = (u32)BlipperChaseUpdate;
    gCurTask->onGround = 0;
    gCurTask->blipperChaseTimer = 0;
    TaskFaceNearestPlayer();
    ActorSetState(BLIPPER_CHASE_STATE_CHASE);
    CallTableEntry(gCurTask->state, 2, gBlipperChaseStates);
}

void BlipperChaseEnterState(void)
{
    gCurTask->updateCallback = (u32)BlipperChaseUpdate;
    CallTableEntry(gCurTask->state, 2, gBlipperChaseStates);
}

void BlipperChaseUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gBlipperChaseStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void BlipperChase(void)
{
    gCurTask->updateState = BLIPPER_CHASE_STATE_CHASE;
    while (gCurTask->blipperChaseTimer <= 0x6FF)
    {
        if (gCurTask->blipperStroking == 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(8);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
        }
    }
    ActorSetState(BLIPPER_CHASE_STATE_1);
    TaskSleepForever();
}

void sub_0808abf4(void)
{
    if (gCurTask->state != BLIPPER_CHASE_STATE_CHASE)
    {
        TaskSetEntry(BlipperChaseEnterState, gCurTaskIdx);
        return;
    }
    gCurTask->blipperChaseTimer++;
    if ((gCurTask->blipperChaseTimer & 7) == 0)
    {
        u8 c = TaskGetAngleToNearestPlayer(1) + 1;
        u16 d = (c & 0xF) >> 1;

        TaskAccelerateInDir(gUnk_087428E8[gCurTask->actorSpawnArg],
                     gUnk_087428E8[gCurTask->actorSpawnArg + 4], d);
        if (gUnk_030023B4 > 0)
            gCurTask->facing = 1;
        if (gUnk_030023B4 < 0)
            gCurTask->facing = -1;
        gCurTask->blipperStroking = gUnk_030023B4 | gUnk_030023D4;
    }
    if (IsWaterAtPixel(gCurTask->pixelX, (u16)gCurTask->pixelY - 8) != 1)
    {
        if (gCurTask->velY < 0)
            gCurTask->velY = -gCurTask->velY;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
    }
}

void sub_0808acdc(void)
{
    gCurTask->updateState = BLIPPER_CHASE_STATE_1;
    while (1)
    {
        if (gCurTask->blipperStroking == 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(6);
            TaskSetFrame(8);
            TaskYieldTrampoline(6);
        }
    }
}

void sub_0808ad20(void)
{
    gCurTask->blipperChaseTimer++;
    if ((gCurTask->blipperChaseTimer & 7) == 0)
    {
        u8 c = TaskGetAngleToNearestPlayer(1) + 9;
        u16 d = (c & 0xF) >> 1;

        TaskAccelerateInDir(gUnk_087428E8[gCurTask->actorSpawnArg],
                     gUnk_087428E8[gCurTask->actorSpawnArg + 4], d);
        if (gCurTask->velX > 0)
            gCurTask->facing = 1;
        if (gCurTask->velX < 0)
            gCurTask->facing = -1;
        gCurTask->blipperStroking = gUnk_030023B4 | gUnk_030023D4;
    }
    if (IsWaterAtPixel(gCurTask->pixelX, (u16)gCurTask->pixelY - 8) != 1)
    {
        if (gCurTask->velY < 0)
            gCurTask->velY = -gCurTask->velY;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
    }
}

void BlipperWaveInit(void)
{
    gCurTask->updateCallback = (u32)BlipperWaveUpdate;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    ActorSetState(BLIPPER_WAVE_STATE_WAVE);
    CallTableEntry(gCurTask->state, 1, gBlipperWaveStates);
}

void BlipperWaveEnterState(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)BlipperWaveUpdate;
    CallTableEntry(t->state, 1, gBlipperWaveStates);
}

void BlipperWaveUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gBlipperWaveStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void BlipperWave(void)
{
    gCurTask->updateState = BLIPPER_WAVE_STATE_WAVE;
    TaskStop();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08742894);
    while (1)
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFFC000;
        TaskYieldTrampoline(10);
    }
}

void sub_0808aeec(void)
{
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
    if (gCurTask->variant == 3 || gCurTask->variant == 4)
        sub_0808b4d0();
}

void sub_0808af34(void)
{
    s16 *p;

    gCurTask->updateCallback = (u32)sub_0808afd0;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    gCurTask->blipperSavedFacing = gCurTask->facing;
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08742910);
}

void sub_0808afac(void)
{
    gCurTask->updateCallback = (u32)sub_0808afd0;
    CallTableEntry(gCurTask->state, 2, gUnk_08742910);
}

void sub_0808afd0(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08742918);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808b008(void)
{
    gCurTask->updateState = 0;
    gCurTask->facing = gCurTask->blipperSavedFacing;
    gCurTask->velY = 0xFFFF8000;
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087428A8);
    gCurTask->blipperLoopCount = 0;
    do
    {
        gCurTask->velX = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFFC000;
        TaskYieldTrampoline(15);
        gCurTask->blipperLoopCount++;
    } while ((s16)gCurTask->blipperLoopCount <= 2);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0808b094(void)
{
    if (gCurTask->state == 0)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX,
                             (u16)gCurTask->pixelY - 8) != 0)
            goto anim;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
        ActorSetState(1);
    }
    TaskSetEntry(sub_0808afac, gCurTaskIdx);
    return;
anim:
    if (gCurTask->u8C.actor->animScript != 0)
    {
        if (gCurTask->actorAnimDelay == 0)
            gCurTask->actorAnimDelay = ActorStepAnim();
        gCurTask->actorAnimDelay--;
    }
}

void sub_0808b120(void)
{
    gCurTask->updateState = 1;
    gCurTask->facing = gCurTask->blipperSavedFacing;
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskSetFrame(12);
    TaskYieldTrampoline(6);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(19);
    TaskYieldTrampoline(119);
    TaskStop();
    TaskSetFrame(13);
    TaskYieldTrampoline(6);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0808b1a8(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_0808afac, gCurTaskIdx);
}

void BlipperLeapInit(void)
{
    gCurTask->updateCallback = (u32)BlipperLeapUpdate;
    gCurTask->blipperCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    gCurTask->blipperLeapTimer = 1;
    ActorSetState(BLIPPER_LEAP_STATE_0);
    CallTableEntry(gCurTask->state, 2, gBlipperLeapStates);
}

void BlipperLeapEnterState(void)
{
    gCurTask->updateCallback = (u32)BlipperLeapUpdate;
    CallTableEntry(gCurTask->state, 2, gBlipperLeapStates);
}

void BlipperLeapUpdate(void)
{
    if (gCurTask->blipperCollideTerrain != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 2, gBlipperLeapStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 2, gBlipperLeapStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808b28c(void)
{
    gCurTask->updateState = BLIPPER_LEAP_STATE_0;
    gCurTask->blipperTurnTimer = 192;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->accelY = 0xFFFFCD00;
    gCurTask->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(5);
        TaskSetFrame(8);
        TaskYieldTrampoline(5);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
    }
}

void sub_0808b2fc(void)
{
    if (gCurTask->velY < 0
        && (u8)IsWaterAtPixel(gCurTask->pixelX,
                            (u16)gCurTask->pixelY - 8) == 0)
    {
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
        ActorSetState(BLIPPER_WAVE_STATE_WAVE);
        TaskSetEntry(BlipperWaveEnterState, gCurTaskIdx);
    }
    else
    {
        sub_0808b4d0();
    }
}

void BlipperLeap(void)
{
    gCurTask->updateState = BLIPPER_LEAP_STATE_LEAP;
    if (gCurTask->variant == 3)
        gCurTask->blipperCollideTerrain = 0;
    else
        gCurTask->blipperCollideTerrain = 1;
    TaskFaceNearestPlayer();
    gCurTask->velX = 0;
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(2);
    gCurTask->blipperLeapTimer = 90;
    TaskSetMotionXFacing(gUnk_08742930[gCurTask->blipperLeapSpeedIndex], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFC0000, 0x1500, 0x30000);
    while (gCurTask->velY < 0)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(3);
        TaskSetFrame(10);
        TaskYieldTrampoline(3);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(22);
    TaskSleepForever();
}

void sub_0808b468(void)
{
    if (gCurTask->blipperLeapTimer > 0 && gCurTask->velY > 0
        && (u8)IsWaterAtPixel(gCurTask->pixelX,
                            (u16)gCurTask->pixelY
                                + ((s8 *)gCurTask->u8C.actor->terrainBox)[2]) != 0)
    {
        gCurTask->velY >>= 2;
        ActorSetState(BLIPPER_LEAP_STATE_0);
        TaskSetEntry(BlipperLeapEnterState, gCurTaskIdx);
    }
}

void sub_0808b4d0(void)
{
    if (--gCurTask->blipperTurnTimer <= 0)
    {
        gCurTask->blipperTurnTimer = 192;
        TaskTurnAroundAndReverseX();
    }
    if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
    {
        ActorSetState(BLIPPER_LEAP_STATE_LEAP);
        TaskSetEntry(BlipperLeapEnterState, gCurTaskIdx);
        return;
    }
    if (abs(TaskGetNearestPlayerDx()) > 64)
        return;
    if (--gCurTask->blipperLeapTimer > 0)
        return;
    if (RandomRange(3) == 0)
    {
        gCurTask->blipperLeapTimer = 16;
        return;
    }
    if (TaskGetNearestPlayerDx() < 0)
    {
        if (-TaskGetNearestPlayerDx() <= 32)
            goto one;
        goto zero;
    }
    if (TaskGetNearestPlayerDx() > 32)
        goto zero;
one:
    gCurTask->blipperLeapSpeedIndex = 1;
    goto done;
zero:
    gCurTask->blipperLeapSpeedIndex = 0;
done:
    ActorSetState(BLIPPER_LEAP_STATE_LEAP);
    TaskSetEntry(BlipperLeapEnterState, gCurTaskIdx);
}

void sub_0808b5b4(void)
{
    gCurTask->updateCallback = (u32)sub_0808b8c4;
    TaskStop();
    if (gCurTask->onGround == 0)
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(20);
        TaskSetMotionY(0, 0x2500, 0x30000);
        TaskSleepForever();
    }
    if (RandomRange(8) == 0)
        gCurTask->facing = -gCurTask->facing;
    switch (gUnk_08742938[RandomRange(8)])
    {
    case 0:
        sub_0808b650();
        break;
    case 1:
        sub_0808b704();
        break;
    case 2:
        sub_0808b79c();
        break;
    case 3:
        sub_0808b830();
        break;
    }
}

void sub_0808b650(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFC0000, 0x4000, 0x30000);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(21);
    TaskYieldTrampoline(3);
    CreateBlipperDroplet(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(21);
    TaskYieldTrampoline(3);
    CreateBlipperDroplet(3);
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    CreateBlipperDroplet(2);
    TaskSetFrame(21);
    TaskYieldTrampoline(4);
    CreateBlipperDroplet(3);
    TaskSetFrame(18);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    TaskSetFrame(19);
    TaskSleepForever();
}

void sub_0808b704(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    CreateBlipperDroplet(0);
    TaskSetFrame(22);
    TaskYieldTrampoline(4);
    CreateBlipperDroplet(1);
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    CreateBlipperDroplet(0);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(23);
    TaskSleepForever();
}

void sub_0808b79c(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskSleepForever();
}

void sub_0808b830(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(18);
    TaskSleepForever();
}

void sub_0808b8c4(void)
{
    if ((u8)IsWaterAtPixel(gCurTask->pixelX,
                         (u16)gCurTask->pixelY
                             + ((s8 *)gCurTask->u8C.actor->terrainBox)[2]) != 0)
    {
        switch (gCurTask->variant)
        {
        case 0:
            TaskStop();
            TaskSetEntry(BlipperChaseInit, gCurTaskIdx);
            break;
        case 1:
            TaskStop();
            TaskSetEntry(BlipperWaveInit, gCurTaskIdx);
            break;
        case 2:
            TaskStop();
            TaskSetEntry(sub_0808af34, gCurTaskIdx);
            break;
        case 3:
        case 4:
            gCurTask->velY >>= 2;
            gCurTask->blipperLeapTimer = 90;
            ActorSetState(0);
            gCurTask->updateCallback = (u32)BlipperLeapUpdate;
            TaskSetEntry(sub_0808b28c, gCurTaskIdx);
            break;
        }
    }
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void CreateBlipperDroplet(s32 a)
{
    gCurTask->blipperDropletSlot = CreateChildTaskHere(TASK_BLIPPER_DROPLET, 1);
    gTasks[gCurTask->blipperDropletSlot].variant = a;
}

void Task_BlipperDroplet(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 12;
    gCurTask->frameTable = gBlipperDropletFrames;
    gCurTask->facing = TaskGetParentFacing();
    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        t->posX = (t->pixelX - t->facing * 12) << 16;
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE0000;
        gCurTask->accelY = 0x8000;
        break;
    case 1:
        t->posX = (t->pixelX + t->facing * 12) << 16;
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE0000;
        gCurTask->accelY = 0x8000;
        break;
    case 2:
        t->posX = (t->pixelX - t->facing * 8) << 16;
        t->posY = (t->pixelY - 12) << 16;
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFD8000;
        gCurTask->accelY = 0x8000;
        break;
    case 3:
        t->posX = (t->pixelX + t->facing * 8) << 16;
        t->posY = (t->pixelY - 12) << 16;
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFD8000;
        gCurTask->accelY = 0x8000;
        break;
    }
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void BlipperIdle(void)
{
    gCurTask->updateCallback = (u32)BlipperIdleUpdate;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSleepForever();
}

void BlipperIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 BlipperLand(void)
{
    if (gCurTask->variant != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
        {
            TaskSetEntry(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
        switch (gCurTask->variant)
        {
        case 0:
            gCurTask->velY = -gCurTask->velY;
        case 1:
            return 0;
        case 2:
            ActorSetState(0);
            TaskSetEntry(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            gCurTask->velY = 0;
            return 0;
        }
    }
}

s32 BlipperStartFall(void)
{
    if (gCurTask->variant != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
        {
            TaskSetEntry(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
    }
}

s32 BlipperHitWall(void)
{
    if (gCurTask->variant != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
            goto stop;
        switch (gCurTask->variant)
        {
        case 0:
            gCurTask->velX = -gCurTask->velX;
        case 2:
        zero:
            return 0;
        case 3:
            gCurTask->blipperTurnTimer = 192;
        case 1:
        stop:
            TaskTurnAroundAndReverseX();
            goto zero;
        case 4:
            if (gCurTask->state == 0)
                gCurTask->blipperTurnTimer = 192;
            TaskTurnAroundAndReverseX();
            return 0;
        }
    }
}

s32 BlipperHitCeiling(void)
{
    u8 r;

    if (gCurTask->variant != 5)
    {
        r = IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY);
        if (r == 0)
        {
            gCurTask->velY = r;
            return 0;
        }
        switch (gCurTask->variant)
        {
        case 0:
            gCurTask->velY = -gCurTask->velY;
        case 1:
            return 0;
        case 2:
            ActorSetState(1);
            TaskSetEntry(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            TaskSetEntry(BlipperWaveInit, gCurTaskIdx);
            return 1;
        }
    }
}

void Task_Gip(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gGipFrames;
    CallTableEntry(gCurTask->variant, 2, gGipVariants);
}

void GipInit(void)
{
    gCurTask->updateCallback = (u32)GipUpdate;
    gCurTask->gipShootTimer = 120;
    if (GipTrySnapToWall() != 0)
        ActorSetState(GIP_STATE_WAIT);
    else
        ActorSetState(GIP_STATE_1);
    CallTableEntry(gCurTask->state, 12, gGipStates);
}

void GipEnterState(void)
{
    CallTableEntry(gCurTask->state, 12, gGipStates);
}

void GipUpdate(void)
{
    if (gCurTask->gipCollideTerrain != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 8, gGipStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 8, gGipStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void GipWalk(void)
{
    gCurTask->updateState = 0;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    if (gCurTask->state == GIP_STATE_2)
        gCurTask->facing = -gCurTask->facing;
    ActorSetState(GIP_STATE_0);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->gipTurnTimer = 16;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void GipWalkUpdate(void)
{
    if (--gCurTask->gipShootTimer == 0)
    {
        ActorSetState(GIP_STATE_SHOOT);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
        return;
    }
    if (--gCurTask->gipTurnTimer != 0)
        return;
    if (TaskGetNearestPlayerDx() < 0)
    {
        if (-TaskGetNearestPlayerDx() <= 63)
            goto near;
        goto far;
    }
    if (TaskGetNearestPlayerDx() > 63)
        goto far;
near:
    TaskFaceNearestPlayer();
    gCurTask->facing = -gCurTask->facing;
    TaskUpdateFlip();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    goto done;
far:
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
done:
    gCurTask->gipTurnTimer = 16;
}

void GipWait(void)
{
    gCurTask->updateState = 1;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(29);
        GipPickNextState();
    } while (gCurTask->state == GIP_STATE_WAIT);
    TaskSleepForever();
}

void GipWaitUpdate(void)
{
    if (gCurTask->state != GIP_STATE_WAIT)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
}

void GipClimbUp(void)
{
    gCurTask->updateState = 2;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        gCurTask->gipLoopCount = 0;
        do
        {
            gCurTask->velY = 0xFFFF0000;
            TaskSetFrame(8);
            TaskYieldTrampoline(3);
            TaskSetFrame(9);
            TaskYieldTrampoline(3);
            TaskSetFrame(10);
            TaskYieldTrampoline(4);
            gCurTask->velY = 0xFFFF8000;
            TaskSetFrame(11);
            TaskYieldTrampoline(3);
            TaskSetFrame(12);
            TaskYieldTrampoline(3);
            gCurTask->velY = 0xFFFFC000;
            TaskSetFrame(11);
            TaskYieldTrampoline(3);
            TaskSetFrame(10);
            TaskYieldTrampoline(3);
            TaskStop();
            TaskSetFrame(9);
            TaskYieldTrampoline(6);
            gCurTask->gipLoopCount++;
        } while ((s16)gCurTask->gipLoopCount <= 2);
        GipPickNextState();
    } while (gCurTask->state == GIP_STATE_4);
    TaskSleepForever();
}

void GipClimbUpFromFloor(void)
{
    ActorSetState(GIP_STATE_4);
    gCurTask->updateState = 2;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    gCurTask->gipLoopCount = 0;
    do
    {
        gCurTask->velY = 0xFFFE0000;
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        TaskSetFrame(9);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF8000;
        TaskSetFrame(12);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFFC000;
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(9);
        TaskYieldTrampoline(2);
        gCurTask->gipLoopCount++;
    } while ((s16)gCurTask->gipLoopCount <= 2);
    ActorSetState(GIP_STATE_WAIT);
    TaskSleepForever();
}

void GipClimbUpUpdate(void)
{
    if (gCurTask->state != GIP_STATE_4)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
    if (gCurTask->facing == 1)
    {
        if (GipGetWallDistRight(12, ((s8 *)gCurTask->u8C.actor->terrainBox)[3]) < 0)
        {
            ActorSetState(GIP_STATE_8);
            TaskSetEntry(GipEnterState, gCurTaskIdx);
        }
    }
    else if (GipGetWallDistLeft(12, ((s8 *)gCurTask->u8C.actor->terrainBox)[3]) < 0)
    {
        ActorSetState(GIP_STATE_8);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
    }
}

void GipClimbDown(void)
{
    gCurTask->updateState = 3;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        gCurTask->gipLoopCount = 0;
        do
        {
            gCurTask->velY = 0x10000;
            TaskSetFrame(16);
            TaskYieldTrampoline(4);
            TaskSetFrame(13);
            TaskYieldTrampoline(6);
            gCurTask->velY = 0x8000;
            TaskSetFrame(16);
            TaskYieldTrampoline(4);
            TaskSetFrame(13);
            TaskYieldTrampoline(2);
            gCurTask->velY = 0x4000;
            TaskSetFrame(13);
            TaskYieldTrampoline(4);
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            TaskStop();
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            TaskSetFrame(13);
            TaskYieldTrampoline(4);
            gCurTask->gipLoopCount++;
        } while ((s16)gCurTask->gipLoopCount <= 2);
        GipPickNextState();
    } while (gCurTask->state == GIP_STATE_5);
    TaskSleepForever();
}

void GipClimbDownFromLedge(void)
{
    ActorSetState(GIP_STATE_5);
    gCurTask->updateState = 3;
    gCurTask->gipCollideTerrain = 0;
    gCurTask->onGround = 0;
    TaskStop();
    gCurTask->facing = -gCurTask->facing;
    gCurTask->velY = 0x20000;
    TaskSetFrame(16);
    TaskYieldTrampoline(2);
    gCurTask->gipCollideTerrain = 1;
    gCurTask->gipLoopCount = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskSetFrame(16);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x10000;
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x8000;
        TaskSetFrame(16);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x4000;
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        gCurTask->gipLoopCount++;
    } while ((s16)gCurTask->gipLoopCount <= 2);
    ActorSetState(GIP_STATE_WAIT);
    TaskSleepForever();
}

void GipClimbDownUpdate(void)
{
    if (gCurTask->state != GIP_STATE_5)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
    if (gCurTask->facing == 1)
    {
        if (GipGetWallDistRight(12, ((s8 *)gCurTask->u8C.actor->terrainBox)[3]) < 0)
        {
            ActorSetState(GIP_STATE_9);
            TaskSetEntry(GipEnterState, gCurTaskIdx);
        }
    }
    else if (GipGetWallDistLeft(12, ((s8 *)gCurTask->u8C.actor->terrainBox)[3]) < 0)
    {
        ActorSetState(GIP_STATE_9);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
    }
}

void GipClimbOverTop(void)
{
    gCurTask->updateState = 4;
    gCurTask->gipCollideTerrain = 0;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskSetFrame(4);
    TaskYieldTrampoline(10);
    ActorSetState(GIP_STATE_1);
    TaskSleepForever();
}

void GipClimbOverTopUpdate(void)
{
    if (gCurTask->state != GIP_STATE_8)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
}

void GipLetGo(void)
{
    gCurTask->updateState = 5;
    gCurTask->gipCollideTerrain = 1;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(4);
    gCurTask->facing = -gCurTask->facing;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    TaskSleepForever();
}

void GipLetGoUpdate(void)
{
}

void GipJump(void)
{
    gCurTask->updateState = 6;
    gCurTask->gipCollideTerrain = 0;
    gCurTask->onGround = 0;
    TaskStop();
    if (TaskGetFacingTowardNearestPlayer() == gCurTask->facing)
    {
        ActorSetState(GIP_STATE_WAIT);
        TaskSleepForever();
    }
    TaskSetFrame(8);
    gCurTask->gipLoopCount = 0;
    do
    {
        TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        gCurTask->gipLoopCount++;
    } while ((s16)gCurTask->gipLoopCount <= 7);
    gCurTask->gipCollideTerrain = 1;
    gCurTask->facing = -gCurTask->facing;
    GipSetLeapVelocity();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
    }
}

void GipJumpUpdate(void)
{
    if (gCurTask->state != GIP_STATE_JUMP)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
}

void GipShoot(void)
{
    gCurTask->updateState = 7;
    gCurTask->gipCollideTerrain = 0;
    gCurTask->onGround = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(14);
    TaskYieldTrampoline(16);
    PlaySfx(233);
    CreateGipStar();
    TaskSetFrame(15);
    TaskYieldTrampoline(12);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
    gCurTask->gipShootTimer = 120;
    ActorSetState(GIP_STATE_1);
    TaskSleepForever();
}

void GipShootUpdate(void)
{
    if (gCurTask->state != GIP_STATE_SHOOT)
        TaskSetEntry(GipEnterState, gCurTaskIdx);
}

void GipIdle(void)
{
    gCurTask->updateCallback = (u32)GipIdleUpdate;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void GipIdleUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 GipLand(void)
{
    if (gCurTask->variant != 1)
    {
        ActorSetState(GIP_STATE_0);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 GipStartFall(void)
{
    if (gCurTask->variant != 1)
    {
        ActorSetState(GIP_STATE_7);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 GipEnterWater(void)
{
    if (gCurTask->variant != 1)
    {
        ActorStartDrown(-2);
        return 1;
    }
}

s32 GipHitWall(void)
{
    if (gCurTask->variant != 1)
    {
        switch (gCurTask->state)
        {
        case GIP_STATE_0:
            ActorSetState(GIP_STATE_6);
            TaskSetEntry(GipEnterState, gCurTaskIdx);
            return 1;
        case GIP_STATE_9:
        case GIP_STATE_JUMP:
            ActorSetState(GIP_STATE_WAIT);
            TaskSetEntry(GipEnterState, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 GipHitCeiling(void)
{
    if (gCurTask->variant != 1)
    {
        if (gCurTask->state != GIP_STATE_4)
        {
            gCurTask->velY = 0;
            return 0;
        }
        ActorSetState(GIP_STATE_JUMP);
        TaskSetEntry(GipEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 GipTrySnapToWall(void)
{
    s32 r;

    r = GipGetWallDistRight(12, 0);
    if (r >= 0)
    {
        gCurTask->pixelX = (u16)gCurTask->pixelX
            + (r - ((s8 *)gCurTask->u8C.actor->terrainBox)[5]);
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
        gCurTask->facing = 1;
        return 1;
    }
    r = GipGetWallDistLeft(12, 0);
    if (r >= 0)
    {
        gCurTask->pixelX = (u16)gCurTask->pixelX
            - (((s8 *)gCurTask->u8C.actor->terrainBox)[4] + r);
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
        gCurTask->facing = -1;
        return 1;
    }
    return 0;
}

void GipPickNextState(void)
{
    PickWeightedRandomIndex(gUnk_08742998[(u16)TaskGetAngleToNearestPlayer(1)], 100);
    switch (gUnk_030023D4)
    {
    case 0:
        ActorSetState(GIP_STATE_4);
        break;
    case 1:
        ActorSetState(GIP_STATE_5);
        break;
    case 2:
        ActorSetState(GIP_STATE_9);
        break;
    case 3:
        ActorSetState(GIP_STATE_JUMP);
        break;
    case 4:
        ActorSetState(GIP_STATE_WAIT);
        break;
    }
}

void CreateGipStar(void)
{
    struct ActorSpawn sp;
    u8 zero;

    sp.subtype = 34;
    sp.taskType = TASK_GIP_STAR;
    sp.variant = gCurTask->variant;
    sp.spawnArg = gCurTask->actorSpawnArg;
    zero = 0;
    sp.x = 6;
    sp.y = -6;
    sp.checkTerrain = zero;
    gCurTask->gipStarSlot = CreateActorFromDescAtOffsetFacing(&sp, 0);
}

void GipSetLeapVelocity(void)
{
    s32 a;
    s32 d;

    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    a = abs(TaskGetNearestPlayerDx()) >> 1;
    d = -TaskGetNearestPlayerDy() + 32;
    if (d < 0)
        d = -d;
    if (a == 0)
        a = 1;
    d = Div(d << 16, a);
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        d = -d;
    d -= a << 16;
    if (d > 0)
        d = 0;
    if (d < (s32)0xFFFD0000)
        d = 0xFFFD0000;
    TaskSetMotionY(d, 0x2000, 0x30000);
}

s32 GipGetWallDistRight(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gCurTask;
    pa = &t->pixelX;
    x = (s8)a;
    p = (u16)*pa + x;
    q = (u16)t->pixelY + (s8)b;
    i = GetCollisionTileAtPixel(p, q);
    if (gUnk_087339F0[i] == 0 || gCollisionTileOneWay[i] != 0
     || gUnk_087337F0[i] != 0 || gCollisionTileSlope[i] != 0
     || gCollisionTileDamaging[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gCollisionTilePushLeft[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

s32 GipGetWallDistLeft(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gCurTask;
    pa = &t->pixelX;
    x = (s8)a;
    p = (u16)*pa - x;
    q = (u16)t->pixelY + (s8)b;
    i = GetCollisionTileAtPixel(p, q);
    if (gUnk_087339F0[i] == 0 || gCollisionTileOneWay[i] != 0
     || gUnk_087337F0[i] != 0 || gCollisionTileSlope[i] != 0
     || gCollisionTileDamaging[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gCollisionTilePushRight[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

void Task_ChillyFreeze(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 12;
    t = gCurTask;
    t->frameTable = gChillyFreezeFrames;
    t->tileWord = (t->tileWord & 0xFFF) | 0xF000;
    t->updateCallback = (u32)ChillyFreezeUpdate;
    t->chillyFreezeSparkleTimer = 0;
    t->chillyFreezeSfxTimer = 0;
    t->chillyFreezeLoopCount = 0;
    do
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(-1);
        TaskYieldTrampoline(1);
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetFrame(-1);
        TaskYieldTrampoline(1);
        gCurTask->chillyFreezeLoopCount++;
    } while ((s16)gCurTask->chillyFreezeLoopCount <= 59);
    ActorDestroy();
}

void ChillyFreezeUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gCurTask->chillyFreezeSfxTimer <= 0)
    {
        PlaySfx(185);
        gCurTask->chillyFreezeSfxTimer = 5;
    }
    t = gCurTask;
    t->chillyFreezeSfxTimer--;
    u = &gTasks[t->parent];
    if (u->hitKind != HIT_KIND_NONE || u->onGround == 0)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        return;
    }
    n = t->chillyFreezeSparkleTimer;
    if (n == 0)
    {
        gCurTask->chillyFreezeSparkleSlot = CreateChildTaskHere(TASK_CHILLY_FREEZE_SPARKLE, 1);
        gTasks[gCurTask->chillyFreezeSparkleSlot].state = 0;
        gCurTask->chillyFreezeSparkleTimer++;
    }
    else if (n == 10)
    {
        gCurTask->chillyFreezeSparkleSlot = CreateChildTaskHere(TASK_CHILLY_FREEZE_SPARKLE, 1);
        gTasks[gCurTask->chillyFreezeSparkleSlot].state = 1;
        gCurTask->chillyFreezeSparkleTimer++;
    }
    else if (n > 19)
        t->chillyFreezeSparkleTimer = 0;
    else
        t->chillyFreezeSparkleTimer = n + 1;
    ActorCheckHits();
    ActorReactToHit();
}
