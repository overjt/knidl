#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void ActorSetExtraAttackBox(void *p);
extern void ReleaseHeldPlayer(s32 i, s32 d);
extern u32 ActorCheckHits(void);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u32 ActorCheckPlayerHitsWithBox(void *p);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 ActorHasExtraFrame(void);

void FireLionFlameCheckParent(void)
{
    if (gTaskSlotTypes[gCurTask->parent] != TASK_FIRE_LION)
        TaskFree(gCurTaskIdx);
}

void Task_PhanPhan(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->layer = 11;
    gCurTask->frameTable = gPhanPhanFrames;
    gUnk_02007D00[8]++;
    ActorInitBossGfx(0);
    sub_08097580();
    ActorCollideTerrain();
    ActorIntroPoseUntilMidBossFight(gUnk_08744888);
    if (IsMidBossDroppingIn() != 0) {
        gCurTask->updateCallback = (u32)sub_080975c8;
        PhanPhanDropIn();
    } else {
        gCurTask->updateCallback = (u32)PhanPhanUpdate;
        sub_08066580();
        ActorSetState(1);
        sub_0809773c();
    }
}

void sub_08097580(void)
{
    struct Task *t;

    ActorSetExtraAttackBox(gPhanPhanExtraAttackBox);
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->phanPhanLanded = 0;
    t->phanPhanCatchActive = 0;
    t->phanPhanMoveCount = 0;
    t->phanPhanHeldPlayerSlot = -1;
    MidBossResetHealth();
}

void PhanPhanEnterState(void)
{
    CallTableEntry(gCurTask->state, 9, gPhanPhanStates);
}

void sub_080975c8(void)
{
    CallTableEntry(gCurTask->updateState, 9, gPhanPhanStateUpdates);
    if (gCurTask->phanPhanCatchActive != 0)
        PhanPhanCheckCatch();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void PhanPhanUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if ((t->hitTimer != 0 && t->u8C.actor->hitState != 0) || t->state == 8)
        ActorFlashPalette(gUnk_082BFBA4, 16);
    else
        ActorClearPaletteOverride();
    if (ActorHasExtraFrame() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 9, gPhanPhanStateUpdates);
    } else {
        CallTableEntry(gCurTask->updateState, 9, gPhanPhanStateUpdates);
    }
    if (gCurTask->phanPhanCatchActive != 0)
        PhanPhanCheckCatch();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void PhanPhanDropIn(void)
{
    struct Task *t;

    ActorSetState(PHAN_PHAN_STATE_DROP_IN);
    gCurTask->updateState = PHAN_PHAN_STATE_DROP_IN;
    gCurTask->onGround = 0;
    gCurTask->phanPhanLanded = 0;
    TaskSetFrame(18);
    gCurTask->accelY = 0x2500;
    TaskYieldTrampoline(24);
    t = gCurTask;
    t->updateCallback = (u32)PhanPhanUpdate;
    if (t->phanPhanLanded == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->phanPhanLanded == 0);
    }
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    TaskSetFrame(19);
    TaskYieldTrampoline(24);
    sub_08066580();
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanDropInUpdate(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_DROP_IN)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void sub_0809773c(void)
{
    struct Task *t;
    s32 r;
    s16 v;

    gCurTask->updateState = PHAN_PHAN_STATE_1;
    gCurTask->phanPhanCatchActive = 1;
    gCurTask->phanPhanLoopCount = 0;
    while ((s16)gCurTask->phanPhanLoopCount < gUnk_087448E4[gCurTask->actorSpawnArg]) {
        TaskFaceNearestPlayer();
        TaskStop();
        v = gUnk_087448E6[gCurTask->actorSpawnArg];
        if (abs(TaskGetNearestPlayerDx()) <= v) {
            gUnk_030023D4 = r = RandomRange(8);
            if (r > 3)
                goto anim;
            sub_0809780c();
        } else {
            gUnk_030023D4 = r = RandomRange(8);
            if (r > 3) {
                if (r <= 4) {
anim:
                    PhanPhanHopBackward();
                } else {
                    PhanPhanHopForward();
                }
            }
        }
        gCurTask->phanPhanLoopCount++;
    }
    PhanPhanChooseNextState();
    TaskSleepForever();
}

void sub_0809780c(void)
{
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
}

void PhanPhanHopBackward(void)
{
    struct Task *t;
    struct Task *u;

    TaskStop();
    TaskSetFrame(22);
    TaskYieldTrampoline(5);
    TaskSetFrame(26);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->phanPhanLanded = 0;
    TaskSetMotionXFacing(-gPhanPhanHopSpeeds[t->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(23);
    TaskYieldTrampoline(4);
    TaskSetFrame(25);
    TaskYieldTrampoline(4);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(5);
    TaskSetFrame(27);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->phanPhanLanded = 0;
    TaskSetMotionXFacing(-gPhanPhanHopSpeeds[u->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
}

void PhanPhanHopForward(void)
{
    struct Task *t;
    struct Task *u;

    TaskStop();
    TaskSetFrame(22);
    TaskYieldTrampoline(5);
    TaskSetFrame(26);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->phanPhanLanded = 0;
    TaskSetMotionXFacing(gPhanPhanHopSpeeds[t->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(23);
    TaskYieldTrampoline(4);
    TaskSetFrame(25);
    TaskYieldTrampoline(4);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(5);
    TaskSetFrame(27);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->phanPhanLanded = 0;
    TaskSetMotionXFacing(gPhanPhanHopSpeeds[u->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
}

void PhanPhanState1Update(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_1)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void PhanPhanHop(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gCurTask->updateState = PHAN_PHAN_STATE_HOP;
    gCurTask->phanPhanCatchActive = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    n = gUnk_087448F4[gCurTask->actorSpawnArg * 2 + RandomRange(2)];
    gCurTask->phanPhanLoopCount = 0;
    while ((s16)gCurTask->phanPhanLoopCount < n) {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->phanPhanLanded = 0;
        TaskSetMotionY(-0x50000, 0x5000, 0x30000);
        TaskSetFrame(17);
        TaskYieldTrampoline(16);
        TaskSetFrame(18);
        TaskYieldTrampoline(14);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        while (gCurTask->phanPhanLanded == 0)
            TaskYieldTrampoline(1);
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        gCurTask->phanPhanLoopCount++;
    }
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanHopUpdate(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_HOP)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void PhanPhanCharge(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = PHAN_PHAN_STATE_CHARGE;
    gCurTask->phanPhanCatchActive = 1;
    TaskSetMotionXFacing(-0x30000, 0x3000);
    ActorStopAnim();
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(13);
    TaskYieldTrampoline(3);
    TaskSetFrame(14);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(2);
    TaskStop();
    v = ActorStartAnim(gPhanPhanAnim);
    t = gCurTask;
    t->actorAnimDelay24 = v;
    if (t->actorSpawnArg == 0)
        TaskYieldTrampoline(16);
    PlaySfx(500);
    CreateDustTrail(1, 3, 8, 10);
    TaskSetMotionXFacing(gUnk_087448F8[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    TaskYieldTrampoline(48);
    TaskStop();
    ActorStopAnim();
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanChargeUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != PHAN_PHAN_STATE_CHARGE) {
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
    } else {
        gCurTask->actorAnimDelay24 = ActorTickAnim(t->actorAnimDelay24);
    }
}

void PhanPhanBounceOffWall(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = PHAN_PHAN_STATE_BOUNCE_OFF_WALL;
    zero = 0;
    PlaySfx(0x1F7);
    RequestScreenShake(4);
    ActorSetAttackBox(gPhanPhanBounceOffWallAttackBox);
    ActorSetExtraAttackBox(gPhanPhanBounceOffWallExtraAttackBox);
    t = gCurTask;
    t->phanPhanCatchActive = zero;
    t->onGround = zero;
    gCurTask->phanPhanLanded = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    TaskSetFrame(28);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 1, 8, 10);
    TaskSetMotionXFacing(-0x18000, 0x5A5A5A5A);
    TaskSetFrame(29);
    TaskYieldTrampoline(2);
    TaskStop();
    if (gCurTask->actorSpawnArg == 0)
        TaskYieldTrampoline(40);
    ActorSetAttackBox(gPhanPhanAttackBox);
    ActorSetExtraAttackBox(gPhanPhanExtraAttackBox);
    TaskSetFrame(14);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanBounceOffWallUpdate(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_BOUNCE_OFF_WALL)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void PhanPhanJump(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = PHAN_PHAN_STATE_JUMP;
    zero = 0;
    TaskFaceNearestPlayer();
    gCurTask->onGround = zero;
    t = gCurTask;
    t->phanPhanLanded = zero;
    t->phanPhanCatchActive = 1;
    TaskSetMotionXFacing(gUnk_08744924[t->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-0x48000, 0x2500, 0x30000);
    TaskSetFrame(17);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(18);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    CreateLandingDust(16, 12);
    TaskStop();
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanJumpUpdate(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_JUMP)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void PhanPhanThrowPlayer(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;
    s32 d;
    s32 k;

    gCurTask->updateState = PHAN_PHAN_STATE_THROW_PLAYER;
    zero = 0;
    TaskStop();
    gCurTask->phanPhanCatchActive = zero;
    ActorStopAnim();
    TaskSetFrame(40);
    t = gCurTask;
    if (t->onGround == 0) {
        t->phanPhanLanded = zero;
        TaskSetMotionY(0, 0x2500, 0x30000);
        gCurTask->phanPhanLoopCount = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            u = gCurTask;
            u->phanPhanLoopCount++;
        } while ((s16)u->phanPhanLoopCount <= 3);
        TaskStopX();
        while (gCurTask->phanPhanLanded == 0)
            TaskYieldTrampoline(1);
        TaskYieldTrampoline(20);
    } else {
        t->phanPhanLoopCount = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gCurTask;
            v->phanPhanLoopCount++;
        } while ((s16)v->phanPhanLoopCount <= 3);
        TaskStopX();
        TaskYieldTrampoline(4);
    }
    gUnk_030023D4 = d = gCurTask->pixelX - gViewRect[0];
    k = gCurTask->facing;
    if (120 - d >= 0)
        goto chk2;
    if (k <= 0)
        goto yes;
    goto other;
chk2:
    if (k < 0)
        goto other;
yes:
    PhanPhanThrowPlayerWindUpLong();
    SetHeldPlayerState(gCurTask->phanPhanHeldPlayerSlot, 8);
    if (gLocalPlayer == gCurTask->phanPhanHeldPlayerSlot)
        PlaySfx(0x23A);
    gCurTask->phanPhanHeldPlayerSlot = -1;
    TaskSetFrame(32);
    TaskYieldTrampoline(4);
    TaskSetFrame(39);
    TaskYieldTrampoline(8);
    goto tail;
other:
    PhanPhanThrowPlayerWindUpShort();
    SetHeldPlayerState(gCurTask->phanPhanHeldPlayerSlot, 9);
    if (gLocalPlayer == gCurTask->phanPhanHeldPlayerSlot)
        PlaySfx(0x23A);
    gCurTask->phanPhanHeldPlayerSlot = -1;
    TaskSetFrame(43);
    TaskYieldTrampoline(4);
    TaskSetFrame(44);
    TaskYieldTrampoline(8);
tail:
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanThrowPlayerWindUpLong(void)
{
    struct Task *t;

    gCurTask->phanPhanLoopCount = 0;
    do {
        if (gLocalPlayer == gCurTask->phanPhanHeldPlayerSlot)
            PlaySfx(0x239);
        TaskSetFrame(30);
        TaskYieldTrampoline(4);
        TaskSetFrame(34);
        TaskYieldTrampoline(2);
        TaskSetFrame(31);
        TaskYieldTrampoline(1);
        TaskSetFrame(35);
        TaskYieldTrampoline(2);
        TaskSetFrame(32);
        TaskYieldTrampoline(4);
        TaskSetFrame(36);
        TaskYieldTrampoline(4);
        TaskSetFrame(33);
        TaskYieldTrampoline(5);
        TaskSetFrame(37);
        TaskYieldTrampoline(4);
        t = gCurTask;
        t->phanPhanLoopCount++;
    } while ((s16)t->phanPhanLoopCount <= 2);
}

void PhanPhanThrowPlayerWindUpShort(void)
{
    TaskSetFrame(40);
    TaskYieldTrampoline(8);
    TaskSetFrame(41);
    TaskYieldTrampoline(8);
    TaskSetFrame(42);
    TaskYieldTrampoline(4);
}

void PhanPhanThrowPlayerUpdate(void)
{
    if (gCurTask->state != PHAN_PHAN_STATE_THROW_PLAYER)
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
}

void PhanPhanThrowApple(void)
{
    s32 zero;

    gCurTask->updateState = PHAN_PHAN_STATE_THROW_APPLE;
    zero = 0;
    gCurTask->phanPhanCatchActive = zero;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(15);
    gCurTask->onGround = zero;
    gCurTask->phanPhanLanded = zero;
    gCurTask->actorAnimDelay24 = ActorStartAnim(gPhanPhanAnim);
    TaskSetMotionY(-0x38000, 0x2000, 0x30000);
    TaskYieldTrampoline(gUnk_0874492C[RandomRange(8)]);
    ActorStopAnim();
    PlaySfx(506);
    CreatePhanPhanApple();
    TaskSetFrame(14);
    TaskYieldTrampoline(3);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(18);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    ActorSetState(PHAN_PHAN_STATE_1);
    TaskSleepForever();
}

void PhanPhanThrowAppleUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != PHAN_PHAN_STATE_THROW_APPLE) {
        TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
    } else {
        gCurTask->actorAnimDelay24 = ActorTickAnim(t->actorAnimDelay24);
    }
}

void PhanPhanDefeat(void)
{
    s32 zero;

    gCurTask->updateState = PHAN_PHAN_STATE_DEFEAT;
    zero = 0;
    TaskStop();
    gCurTask->phanPhanCatchActive = zero;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        EndMidBossFightWithReward();
    MidBossStartDefeat(1, 28);
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    ActorSetHitReactions(gPhanPhanDefeatedHitReactions);
    gCurTask->onGround = zero;
    gCurTask->phanPhanLanded = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    TaskSetFrame(28);
    ActorSetAttackBox(gPhanPhanDefeatAttackBox);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    ActorSetAttackBox(gUnk_087452A8);
    ActorSetExtraAttackBox(gPhanPhanDefeatExtraAttackBox);
    while (gCurTask->phanPhanLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(29);
    RequestScreenShake(4);
    CreateChildTaskHere(TASK_STAR_FLASH, 0);
    CreateDustTrail(0, 1, -4, 12);
    TaskYieldTrampoline(170);
    ActorShakeVertically();
    ActorDie();
}

void PhanPhanDefeatUpdate(void)
{
}

void PhanPhanChooseNextState(void)
{
    struct Task *t;
    s32 k;
    s32 r;

    t = gCurTask;
    t->phanPhanMoveCount++;
    if (t->phanPhanMoveCount > 2) {
        t->phanPhanMoveCount = 0;
        ActorSetState(PHAN_PHAN_STATE_THROW_APPLE);
        return;
    }
    k = 0;
    gUnk_030023D4 = gPlayerStates[TaskFindNearestPlayer()].mode;
    switch (gUnk_030023D4) {
    case 14:
        k = 2;
        break;
    case 4:
    case 5:
        k = 1;
        break;
    }
    if (gCurTask->actorSpawnArg != 0)
        k += 3;
    gUnk_030023D4 = r = RandomRange(8);
    if (r < gUnk_08744934[k]) {
        ActorSetState(PHAN_PHAN_STATE_CHARGE);
        return;
    }
    if (r < gUnk_0874494C[k]) {
        ActorSetState(PHAN_PHAN_STATE_JUMP);
        return;
    }
    ActorSetState(PHAN_PHAN_STATE_HOP);
}

void CreatePhanPhanApple(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct ActorSpawn *p;
    s8 d;

    sp.subtype = 35;
    sp.taskType = TASK_PHAN_PHAN_APPLE;
    p = &sp;
    t = gCurTask;
    p->variant = t->variant;
    p->spawnArg = t->actorSpawnArg;
    p->tileWord = t->u8C.actor->savedTileWord;
    p->x = 12;
    p->y = 8;
    p->checkTerrain = 1;
    d = t->facing;
    TaskFaceNearestPlayer();
    gCurTask->phanPhanAppleSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
    gCurTask->facing = d;
}

void PhanPhanCheckCatch(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 i;

    if (ActorCheckPlayerHitsWithBox(gUnk_087452E0) != 0) {
        p = gPlayerStates;
        t = gCurTask;
        i = t->hitterSlot;
        if (p[i].ability != ABILITY_STONE || p[i].mode != 13) {
            t->phanPhanHeldPlayerSlot = i;
            TaskFaceToward(i);
            PlaySfx(568);
            HoldPlayer(gCurTask->phanPhanHeldPlayerSlot, gCurTaskIdx, 7);
            ActorSetState(PHAN_PHAN_STATE_THROW_PLAYER);
            TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
        }
    }
}

s32 PhanPhanLand(void)
{
    TaskStopY();
    gCurTask->phanPhanLanded = 1;
    return 0;
}

s32 PhanPhanHitWall(void)
{
    struct Task *t;
    s32 d;

    t = gCurTask;
    if (t->state != 3)
        return 0;
    d = t->facing;
    if (t->velX < 0) {
        if (d < 0)
            goto hit;
        goto miss;
    }
    if (d <= 0)
        goto miss;
hit:
    ActorSetState(4);
    return 1;
miss:
    TaskStopX();
    return 0;
}

s32 PhanPhanReactToDamage(void)
{
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    RequestScreenShake(2);
    return 0;
}

s32 PhanPhanReactToDefeat(void)
{
    struct Task *t;

    ActorFaceHitter();
    t = gCurTask;
    if (t->phanPhanHeldPlayerSlot >= 0) {
        ReleaseHeldPlayer(t->phanPhanHeldPlayerSlot, -t->facing);
        gCurTask->phanPhanHeldPlayerSlot = -1;
    }
    ActorSetState(PHAN_PHAN_STATE_DEFEAT);
    TaskSetEntry(PhanPhanEnterState, gCurTaskIdx);
    return 1;
}

void Task_GrandWheelieMiniWheelie(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->updateCallback = (u32)GrandWheelieMiniWheelieUpdate;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    gCurTask->frameTable = gGrandWheelieMiniWheelieFrames;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetMotionXFacing(gGrandWheelieMiniWheelieRollSpeeds[0] >> 1, 0x5A5A5A5A);
    TaskSetMotionY(gUnk_087454EC[gCurTask->actorSpawnArg], 0x3000, 0x30000);
    gCurTask->onGround = zero;
    gCurTask->grandWheelieMiniWheelieRolling = zero;
    while (1) {
        u = gCurTask;
        switch (u->grandWheelieMiniWheelieRolling) {
        case 0:
            if (u->u8C.actor->animScript != gUnk_087454B8)
                gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087454B8);
            break;
        case 1:
            if (u->u8C.actor->animScript != gUnk_087454C4)
                gCurTask->actorAnimDelay = ActorStartAnim(gUnk_087454C4);
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void GrandWheelieMiniWheelieUpdate(void)
{
    struct Task *t;
    s32 x;

    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
    x = ActorTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->grandWheelieMiniWheelieRolling != 0 && GrandWheelieMiniWheelieUpdateGroundClass() != 0)
        TaskSetMotionXFacing(gGrandWheelieMiniWheelieRollSpeeds[gCurTask->grandWheelieMiniWheelieGroundClass], 0x5A5A5A5A);
}

void GrandWheelieMiniWheelieLand(void)
{
    struct Task *t;

    TaskStopY();
    t = gCurTask;
    t->grandWheelieMiniWheelieGroundClass = -1;
    t->grandWheelieMiniWheelieRolling = 1;
}

void sub_08098708(void)
{
    gCurTask->unk2C = 0;
}

void GrandWheelieMiniWheelieEnterWater(void)
{
    ActorStartDrown(-2);
}

void GrandWheelieMiniWheelieHitWall(void)
{
    RequestScreenShake(1);
    ActorReactToDefeat();
}

void GrandWheelieMiniWheelieHitCeiling(void)
{
    struct Task *t;

    t = gCurTask;
    t->velY = -t->velY;
}

s32 GrandWheelieMiniWheelieUpdateGroundClass(void)
{
    struct Task *t;
    s32 v;

    v = GrandWheelieGetGroundClass();
    t = gCurTask;
    if (t->grandWheelieMiniWheelieGroundClass != (s8)v) {
        t->grandWheelieMiniWheelieGroundClass = (s8)v;
        return 1;
    }
    return 0;
}

void Task_PhanPhanApple(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *y;
    struct Task *z;
    struct Task *q;
    s32 zero;
    s32 v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    u = gCurTask;
    u->frameTable = gPhanPhanAppleFrames;
    u->updateCallback = (u32)PhanPhanAppleUpdate;
    TaskFaceNearestPlayer();
    v = ActorStartAnim(gPhanPhanAppleAnim);
    w = gCurTask;
    w->actorAnimDelay = v;
    w->onGround = zero;
    y = gCurTask;
    y->phanPhanAppleLanded = zero;
    TaskSetMotionXFacing(gUnk_087454F4[y->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(0x2AF00, 0x3000, 0x30000);
    while (gCurTask->phanPhanAppleLanded == 0)
        TaskYieldTrampoline(1);
    gCurTask->onGround = 0;
    z = gCurTask;
    z->phanPhanAppleLanded = 0;
    TaskSetMotionXFacing(gUnk_087454FC[z->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_08745504[gCurTask->actorSpawnArg], 0x3000, 0x30000);
    while (gCurTask->phanPhanAppleLanded == 0)
        TaskYieldTrampoline(1);
    while (1) {
        gCurTask->onGround = 0;
        q = gCurTask;
        q->phanPhanAppleLanded = 0;
        q->velY = -0x30000;
        while (gCurTask->phanPhanAppleLanded == 0)
            TaskYieldTrampoline(1);
    }
}

void PhanPhanAppleUpdate(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 PhanPhanAppleLand(void)
{
    gCurTask->phanPhanAppleLanded = 1;
    return 0;
}

s32 PhanPhanAppleHitWall(void)
{
    ActorReactToDefeat();
    return 1;
}

void sub_080988c0(void)
{
}

s32 MrFrostyStartFall(void)
{
    if (gCurTask->state == MR_FROSTY_STATE_WAIT) {
        ActorSetState(MR_FROSTY_STATE_FALL);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}
