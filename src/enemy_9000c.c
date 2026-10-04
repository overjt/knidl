/* game_code_and_rodata 0x0809000C-0x0809113C (issue #67, module M25 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0809000C 0x0809113C src/enemy_9000c.c --newpb
 *
 * The first of M25's four boss scripts, dispatched through the 23-entry
 * anchor table at 0x08743848.  Task_Bonkers is the task entry: it installs
 * ActorMove as the draw hook (Task.moveCallback) and sub_08065438 as the
 * per-frame hook (Task.drawCallback), points Task.frameTable at the graphics block
 * gBonkersFrames, counts the boss into gUnk_02007D00[0], spawns its helper
 * task with CreateChildTaskHere(177, 1) and hands Task.variant to CallTableEntry, which
 * jumps into the table.  BonkersUpdate is the per-frame body: it runs down
 * Task.unk18, asks sub_0806acf8 / ActorCollideTerrain whether the player interrupted,
 * dispatches Task.updateState through the same table, reloads the graphics through
 * ActorFlashPalette / ActorClearPaletteOverride and drives the three animation calls from the
 * per-frame row gUnk_087437D0[Task.frame].
 *
 * States 0-10 then follow as <body, guard> pairs (BonkersState0 /
 * sub_08090270, BonkersWalk / BonkersWalkUpdate, ...): the body is a run of
 * TaskYieldTrampoline waits that steps Task.frame, clears and then waits on
 * Task.onGround (set when the boss lands) and pushes 16.16 velocities through
 * TaskSetMotionXFacing / TaskSetMotionY, and the guard re-arms BonkersEnterState through
 * TaskSetEntry whenever Task.state leaves the state.  State 4 aims with
 * Div(|TaskGetNearestPlayerDx()|, 3), state 5 spawns the actors 8 and 145, and state 10
 * is the defeat sequence (sub_0806684c, CreateStarFlash, sub_0806ad18).
 *
 * The tail holds the pieces the states share - BonkersCreateSlamStar (fire a shot at
 * the boss's own position through CreateInhalableStar), BonkersChooseNextState (advance the
 * animation from gUnk_08743744[Task.unk28]), sub_08090e9c (the hover loop),
 * the BonkersReactToDamage / BonkersReactToDefeat / sub_08090f4c hit hooks, the companion
 * task Task_BonkersHammerHitBox / BonkersHammerHitBoxUpdate that mirrors the boss's position while
 * gTaskSlotTypes[Task.parent] says the boss is alive - and Task_PoppyBrosSr, the
 * entry of the second boss, whose states live in src/enemy_9113c.c.
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
extern void ActorSetState(u16 v);
extern u8 sub_0806acf8(void);
extern u8 ActorCollideTerrain(void);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern void sub_08068f68(void);
extern s32 ActorReactToHit(void);
extern void PlaySfx(s32 id);
extern void RequestScreenShake(s32 a);
extern void TaskSetEntry(void *fn, s32 i);
extern u32 RandomRange(u32 range);
extern s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8);
extern void ActorSetHitReactions(u32 *p);
extern u32 ActorCheckHitsWithBox(s32 a);

void Task_Bonkers(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gBonkersFrames;
    gUnk_02007D00[0]++;
    gCurTask->unk46 = CreateChildTaskHere(177, 1);
    if (sub_08067060() == 1)
        gCurTask->unk18 = 24;
    else
        gCurTask->unk18 = 0;
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gBonkersVariants);
}

void BonkersInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)BonkersUpdate;
    t->unk28 = 4;
    t->unk2C = 2;
    t->unk30 = 0;
    t->unk34 = 0;
    sub_080666cc(gUnk_08743758);
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 11, gBonkersStates);
}

void BonkersEnterState(void)
{
    CallTableEntry(gCurTask->state, 11, gBonkersStates);
}

void BonkersUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk18 != 0)
    {
        t->unk18--;
        CallTableEntry(t->updateState, 11, gBonkersStateUpdates);
    }
    else if (sub_0806acf8() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 11, gBonkersStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 11, gBonkersStateUpdates);
    }
    u = gCurTask;
    if (u->unk30 != 0)
    {
        if (u->hitTimer != 0)
            ActorFlashPalette(&gUnk_0826A668, 16);
        else
        {
            u->unk30 = 0;
            ActorClearPaletteOverride();
        }
    }
    ActorSetAttackBox(gUnk_087437F4[gUnk_087437D0[gCurTask->frame]]);
    sub_080639f0(gUnk_08743810[gUnk_087437D0[gCurTask->frame]]);
    sub_08063a00(gUnk_0874382C[gUnk_087437D0[gCurTask->frame]]);
    sub_08068f68();
    ActorReactToHit();
}

void BonkersState0(void)
{
    struct Task *t;
    u8 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    if (sub_08067060() == 1)
    {
        gCurTask->onGround = zero;
        TaskSetFrame(13);
        TaskSetMotionY(0, 5376, 196608);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        PlaySfx(0x1F7);
        TaskStop();
        RequestScreenShake(2);
        sub_08066580();
        gCurTask->frame--;
        TaskYieldTrampoline(30);
    }
    else
    {
        sub_08066580();
    }
    BonkersChooseNextState();
    TaskSleepForever();
}

void sub_08090270(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersWalk(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(gUnk_08743734[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetFrame(7);
    gCurTask->unk1C = (RandomRange(3) + 2) * 4;
    gCurTask->unk6C = zero;
    while ((s16)gCurTask->unk6C < gCurTask->unk1C)
    {
        TaskFaceNearestPlayer();
        TaskUpdateFlip();
        u = gCurTask;
        if (--u->frame <= 3)
            u->frame = 11;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    }
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    TaskSetFrame(9);
    TaskYieldTrampoline(2);
    TaskSetFrame(23);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetFrame(9);
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(20);
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersWalkUpdate(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersJump(void)
{
    struct Task *t;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(12);
    TaskYieldTrampoline(8);
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(gUnk_0874373C[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFA0000, 20480, 196608);
    gCurTask->frame++;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    CreateLandingDust(16, 6);
    gCurTask->frame--;
    TaskYieldTrampoline(30);
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersJumpUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersHop(void)
{
    struct Task *t;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 3;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk1C = gUnk_0874374E[RandomRange(2)];
    gCurTask->unk6C = zero;
    while ((s16)gCurTask->unk6C < gCurTask->unk1C)
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(8);
        gCurTask->onGround = 0;
        TaskSetMotionY(0xFFFB0000, 20480, 196608);
        gCurTask->frame++;
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        CreateLandingDust(16, 6);
        TaskFaceNearestPlayer();
        gCurTask->unk6C++;
    }
    gCurTask->frame--;
    TaskYieldTrampoline(30);
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersHopUpdate(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersDash(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *y;
    s32 n;
    s32 zero;

    t = gCurTask;
    t->updateState = 4;
    u = gCurTask;
    if (TaskGetNearestPlayerDx() < 0)
        u->unk20 = -TaskGetNearestPlayerDx();
    else
        u->unk20 = TaskGetNearestPlayerDx();
    w = gCurTask;
    if (w->unk20 > 256)
    {
        w->unk1C = 256;
    }
    else
    {
        n = w->unk20 - 36;
        w->unk20 = n;
        if (n <= 0)
        {
            w->unk20 = 1;
        }
        else
        {
            if (w->unk74 == 0)
                gCurTask->unk1C = Div(n, 3) * 2;
            else
                w->unk1C = n >> 1;
            if (gCurTask->unk1C <= 0)
                gCurTask->unk1C = 1;
        }
    }
    PlaySfx(502);
    gCurTask->unk20 = ActorStartAnim(gUnk_087437C8[gCurTask->unk74]);
    TaskSetMotionXFacing(gUnk_08743750[gCurTask->unk74], 0x5A5A5A5A);
    y = gCurTask;
    zero = 0;
    y->unk24 = zero;
    y->unk6C = zero;
    while ((s16)gCurTask->unk6C < gCurTask->unk1C)
    {
        TaskYieldTrampoline(1);
        if (--gCurTask->unk20 == 0)
        {
            if ((++gCurTask->unk24 & 1) != 0)
                CreateDustTrail(1, 1, -16, 8);
            gCurTask->unk20 = ActorStepAnim();
        }
        gCurTask->unk6C++;
    }
    if (gCurTask->unk2C == 0)
        ActorSetState(8);
    else if (TaskGetNearestPlayerDy() < -40)
        ActorSetState(7);
    else
        ActorSetState(6);
    TaskSleepForever();
}

void BonkersDashUpdate(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersThrow(void)
{
    struct Task *t;
    struct Task *z;
    struct ActorSpawn spawn;

    t = gCurTask;
    t->updateState = 5;
    TaskStop();
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDx()) <= 47)
    {
        TaskSetFrame(27);
        gCurTask->onGround = 0;
        TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
        TaskSetMotionY(0xFFFE0000, 8192, 196608);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        CreateLandingDust(16, 6);
    }
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(27);
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    } while ((s16)++gCurTask->unk6C <= 3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    spawn.subtype = 8;
    spawn.taskType = 110;
    spawn.variant = 0;
    spawn.spawnArg = 0;
    spawn.x = 24;
    spawn.y = 0;
    spawn.tileWord = gCurTask->u8C.actor->savedTileWord;
    spawn.checkTerrain = 1;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 1);
    PlaySfx(0x1FB);
    z = gCurTask;
    CreateChildTask(145, (s16)(z->pixelX - z->facing * 16), (s16)(z->pixelY + 8), 0);
    gCurTask->frame++;
    TaskYieldTrampoline(44);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersThrowUpdate(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersSlam(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 6;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(20);
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlaySfx(520);
    RequestScreenShake(2);
    BonkersCreateSlamStar();
    gCurTask->frame++;
    TaskYieldTrampoline(32);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(15);
    if (RandomRange(4) == 0)
    {
        ActorSetState(8);
    }
    else
    {
        u = gCurTask;
        u->unk2C--;
        BonkersChooseNextState();
    }
    TaskSleepForever();
}

void BonkersSlamUpdate(void)
{
    if (gCurTask->state != 6)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersJumpSlam(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 7;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFC0000, 9472, 196608);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    gCurTask->frame++;
    TaskYieldTrampoline(21);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(2);
    PlaySfx(520);
    PlaySfx(0x1F7);
    BonkersCreateSlamStar();
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFDC000, 9472, 196608);
    gCurTask->frame--;
    TaskYieldTrampoline(7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(13);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(15);
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersJumpSlamUpdate(void)
{
    if (gCurTask->state != 7)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersTripleSlam(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 8;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(20);
    TaskYieldTrampoline(16);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        RequestScreenShake(2);
        PlaySfx(520);
        if ((s16)gCurTask->unk6C == 0)
            BonkersCreateSlamStar();
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->unk6C <= 2);
    TaskYieldTrampoline(11);
    gCurTask->unk2C = 2;
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersTripleSlamUpdate(void)
{
    if (gCurTask->state != 8)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersBounceOffWall(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 9;
    RequestScreenShake(2);
    PlaySfx(0x1F7);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 9472, 196608);
    TaskSetFrame(31);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(2);
    sub_08090e9c();
    BonkersChooseNextState();
    TaskSleepForever();
}

void BonkersBounceOffWallUpdate(void)
{
    if (gCurTask->state != 9)
        TaskSetEntry(BonkersEnterState, gCurTaskIdx);
}

void BonkersDefeat(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 10;
    if (--gUnk_02007D00[0] == 0)
        sub_0806684c();
    sub_080667c0(1, 32);
    CreateStarFlash(1, 0, 0);
    TaskStop();
    TaskSetFrame(32);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 6656, 196608);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(33);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    CreateStarFlash(0, 0, 10);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 16, 8);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    TaskStopX();
    TaskYieldTrampoline(170);
    CreateStarFlash(1, 0, 0);
    sub_0806ad18();
    gCurTask->unk34 = 2;
    TaskSleepForever();
}

void BonkersDefeatUpdate(void)
{
    ActorFlashPalette(&gUnk_0826A668, 16);
    if (gCurTask->unk34 == 2)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void BonkersCreateSlamStar(void)
{
    struct Task *t;
    s16 x;
    s16 y;

    t = gCurTask;
    x = t->pixelX + t->facing * 40;
    y = t->pixelY + 8;
    CreateInhalableStar(x, y, 0, 2);
}

void BonkersChooseNextState(void)
{
    struct Task *t;

    ActorSetState(gUnk_08743744[gCurTask->unk28]);
    if (gCurTask->state == 2)
        gCurTask->state += RandomRange(2);
    t = gCurTask;
    if (--t->unk28 < 0)
        t->unk28 = 4;
}

void sub_08090e9c(void)
{
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->onGround = 0;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 65536;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 9);
    TaskStopY();
}

s32 BonkersReactToDamage(void)
{
    gCurTask->unk30 = 1;
    CreateStarFlash(1, 0, 0);
    RequestScreenShake(2);
    return 0;
}

s32 BonkersReactToDefeat(void)
{
    ActorSetHitReactions(gUnk_087440DC);
    gCurTask->unk34 = 1;
    ActorSetState(10);
    TaskSetEntry(BonkersEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_08090f4c(void)
{
    struct Task *t;
    s32 r;

    r = 0;
    switch (gCurTask->state)
    {
    case 1:
    case 6:
    case 8:
        t = gCurTask;
        TaskSetMotionX(-t->velX, -t->accelX, t->speedLimitX);
        break;
    case 4:
        ActorSetState(9);
        r = 1;
        break;
    case 2:
    case 3:
    case 5:
    case 7:
    case 9:
    case 10:
        TaskStopX();
        break;
    }
    return r;
}

void Task_BonkersHammerHitBox(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)BonkersHammerHitBoxUpdate;
    TaskSleepForever();
}

void BonkersHammerHitBoxUpdate(void)
{
    struct Task *u;
    s32 i;
    u16 d;

    if ((s16)gTaskSlotTypes[gCurTask->parent] != -1)
    {
        u = &gTasks[gCurTask->parent];
        if (u->unk76 == 0 && u->unk34 == 0)
        {
            d = u->frame - 16;
            if (d <= 6)
            {
                i = (s16)u->frame - 16;
                gCurTask->unk28 = i;
                gCurTask->pixelX = u->pixelX + gBonkersHammerHitBoxOffsetsX[i] * (u16)u->facing;
                gCurTask->pixelY = u->pixelY + gBonkersHammerHitBoxOffsetsY[gCurTask->unk28];
                ActorCheckHitsWithBox(gBonkersHammerHitBoxes[gCurTask->unk28]);
            }
        }
        else
        {
            TaskFree(gCurTaskIdx);
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_PoppyBrosSr(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065350;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrFrames;
    gUnk_02007D00[0]++;
    if (sub_08067060() == 1)
        gCurTask->unk18 = 24;
    else
        gCurTask->unk18 = 0;
    TaskFaceNearestPlayer();
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gPoppyBrosSrVariants);
}
