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
extern void BlendColors(void *src, void *dst, s32 ratio, s32 count, void *out);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void ActorSetTerrainBox(void *p);
extern void sub_080639f0(void *p);
extern void ActorSetExtraAttackBox(void *p);
extern s16 ActorComputeHealth(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern void ReleaseHeldPlayer(s32 i, s32 d);
extern u32 ActorCheckHitsWithBox(void *p);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 ActorHasExtraFrame(void);

void Task_FireLion(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->layer = 11;
    gCurTask->frameTable = gFireLionFrames;
    gUnk_02007D00[8]++;
    sub_08095834();
    ActorCollideTerrain();
    ActorIntroPoseUntilMidBossFight(gUnk_08744510);
    if (IsMidBossDroppingIn() != 0) {
        gCurTask->updateCallback = (u32)sub_0809699c;
        FireLionDropIn();
    } else {
        gCurTask->updateCallback = (u32)FireLionUpdate;
        sub_080959ec();
    }
}

void sub_08095834(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    s32 v;

    ActorInitBossGfx(1);
    gCurTask->speedLimitY = 0x30000;
    TaskFaceNearestPlayer();
    gCurTask->fireLionFlameSlot = CreateChildTaskHere(TASK_FIRE_LION_FLAME, 0);
    gTasks[gCurTask->fireLionFlameSlot].variant = 0;
    b = gCurTask;
    b->fireLionFlameSlots = b->fireLionFlameSlot;
    gCurTask->fireLionFlameSlot = CreateChildTaskHere(TASK_FIRE_LION_FLAME, 0);
    gTasks[gCurTask->fireLionFlameSlot].variant = 1;
    d = gCurTask;
    d->fireLionFlameSlots += d->fireLionFlameSlot << 8;
    gCurTask->fireLionFlameSlot = CreateChildTaskHere(TASK_FIRE_LION_FLAME, 0);
    gTasks[gCurTask->fireLionFlameSlot].variant = 2;
    f = gCurTask;
    f->fireLionFlameSlots += f->fireLionFlameSlot << 8;
    gCurTask->fireLionFlameSlot = CreateChildTaskHere(TASK_FIRE_LION_FLAME, 0);
    gTasks[gCurTask->fireLionFlameSlot].variant = 3;
    h = gCurTask;
    h->actorAnimDelay = 0;
    h->fireLionWallHit = 0;
    h->fireLionCatchActive = 0;
    h->fireLionLanded = 0;
    h->fireLionHeldPlayerSlot = -1;
    h->fireLionGlowing = 0;
    h->fireLionDustTimer = 0;
    h->fireLionFlameSlots = 0;
    h->fireLionSequencePhase = 0;
    h->fireLionPalettePhase = 0;
    MidBossResetHealth();
    gUnk_02007D00[9] = ActorComputeHealth();
}

void FireLionEnterState(void)
{
    CallTableEntry(gCurTask->state, 12, gFireLionStates);
}

void FireLionDropIn(void)
{
    struct Task *t;
    struct Task *u;

    ActorSetState(FIRE_LION_STATE_DROP_IN);
    gCurTask->updateState = 0;
    ActorSetTerrainBox(gUnk_0874530C);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->accelY = 0x5000;
    t->speedLimitY = 0x30000;
    TaskSetFrame(15);
    TaskYieldTrampoline(24);
    u = gCurTask;
    u->updateCallback = (u32)FireLionUpdate;
    if (u->fireLionLanded == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->fireLionLanded == 0);
    }
    ActorSetTerrainBox(gUnk_08745304);
    TaskSetFrame(16);
    TaskYieldTrampoline(24);
    ActorSetState(FIRE_LION_STATE_1);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionDropInUpdate(void)
{
}

void sub_080959ec(void)
{
    ActorSetState(FIRE_LION_STATE_1);
    gCurTask->updateState = 1;
    ActorSetTerrainBox(gUnk_08745304);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08744510);
    sub_08066580();
    TaskYieldTrampoline(gUnk_08744524[gCurTask->actorSpawnArg]);
    gCurTask->fireLionSequencePhase = RandomRange(8);
    ActorSetState(FIRE_LION_STATE_WAIT);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionWait(void)
{
    struct Task *t;
    s32 v;
    s32 x;

    gCurTask->updateState = 1;
    ActorSetTerrainBox(gUnk_08745304);
    TaskFaceNearestPlayer();
    TaskStop();
    x = ActorStartAnim(gUnk_08744510);
    t = gCurTask;
    t->actorAnimDelay = x;
    gUnk_030023D4 = v = t->actorSpawnArg * 2;
    if (t->health < gUnk_02007D00[9] >> 1)
        gUnk_030023D4 = v + 1;
    TaskYieldTrampoline(gUnk_08744526[gUnk_030023D4]);
    FireLionChooseNextState();
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08095ad0(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

void FireLionHop(void)
{
    s32 n;
    s32 r;

    gCurTask->updateState = 2;
    ActorSetTerrainBox(gUnk_08745304);
    TaskFaceNearestPlayer();
    TaskSetFrame(12);
    TaskYieldTrampoline(6);
    TaskSetFrame(16);
    TaskYieldTrampoline(8);
    r = RandomRange(2);
    n = 1;
    if (r != 0)
        n = 3;
    gCurTask->fireLionLoopCount = 0;
    while ((s16)gCurTask->fireLionLoopCount < n) {
        TaskSetMotionY(-0x50000, 0x5000, 0x30000);
        FireLionWaitForLanding();
        RequestScreenShake(2);
        TaskFaceNearestPlayer();
        gCurTask->fireLionLoopCount++;
    }
    ActorSetTerrainBox(gUnk_08745304);
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    TaskSetFrame(18);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
    if (gCurTask->state == FIRE_LION_STATE_3)
        ActorSetState(gUnk_08744608[RandomRange(8)]);
    else
        ActorSetState(FIRE_LION_STATE_POUNCE);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionHopUpdate(void)
{
}

void sub_08095be8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gCurTask->updateState = 3;
    ActorSetTerrainBox(gUnk_08745304);
    TaskFaceNearestPlayer();
    sub_08095e4c();
    t = gCurTask;
    t->fireLionWallHit = 0;
    t->fireLionLoopCount = 0;
    do {
        u = gCurTask;
        if (u->fireLionWallHit != 0)
            u->facing = -u->facing;
        else
            TaskFaceNearestPlayer();
        sub_08095d40();
        v = gCurTask;
        v->fireLionLoopCount++;
    } while ((s16)v->fireLionLoopCount <= 1);
    if (gCurTask->actorSpawnArg == 1 && RandomRange(2) != 0) {
        TaskFaceNearestPlayer();
        gCurTask->onGround = 0;
        TaskSetMotionY(-0x30000, 0x2000, 0x30000);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        FireLionWaitForLanding();
        ActorSetTerrainBox(gUnk_08745304);
        sub_08096924();
        if (gCurTask->state == FIRE_LION_STATE_5)
            ActorSetState(FIRE_LION_STATE_WAIT);
        else
            ActorSetState(gUnk_08744608[RandomRange(8)]);
    } else {
        gCurTask->fireLionLoopCount = 0;
        do {
            w = gCurTask;
            if (w->fireLionWallHit != 0)
                w->facing = -w->facing;
            else
                TaskFaceNearestPlayer();
            sub_08095d40();
            x = gCurTask;
            x->fireLionLoopCount++;
        } while ((s16)x->fireLionLoopCount <= 1);
        sub_08096924();
        ActorSetState(FIRE_LION_STATE_WAIT);
    }
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08095d20(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->accelX != 0 && t->velX * t->accelX >= 0)
        TaskStopX();
}

void sub_08095d40(void)
{
    struct Task *t;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->fireLionWallHit = zero;
    t->fireLionCatchActive = 1;
    TaskStop();
    TaskSetFrame(8);
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(gUnk_0874452C[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    gCurTask->onGround = zero;
    gCurTask->velY = -0x5000;
    TaskSetFrame(9);
    TaskYieldTrampoline(5);
    gCurTask->velY = 0x5000;
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x5A5A5A5A, gUnk_08744534[gCurTask->actorSpawnArg]);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    if (gCurTask->facing != TaskGetFacingTowardNearestPlayer() || gCurTask->fireLionWallHit != 0) {
        CreateDustTrail(1, 1, -4, 12);
        TaskSetFrame(16);
        TaskYieldTrampoline(15);
    } else {
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        CreateDustTrail(1, 1, -4, 12);
        TaskYieldTrampoline(7);
    }
    TaskStop();
    gCurTask->fireLionCatchActive = 0;
}

void sub_08095e4c(void)
{
    TaskStop();
    ActorStopAnim();
    gCurTask->fireLionLoopCount = 0;
    do {
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetFrame(6);
        TaskYieldTrampoline(5);
        TaskSetFrame(7);
        TaskYieldTrampoline(5);
        gCurTask->fireLionLoopCount++;
    } while ((s16)gCurTask->fireLionLoopCount <= 1);
}

void FireLionSlash(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 4;
    ActorSetTerrainBox(gUnk_08745304);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDx()) <= 47) {
        FireLionJumpBack();
        gCurTask->updateState = 4;
    } else if (abs(TaskGetNearestPlayerDx()) > 71) {
        sub_08095e4c();
        gCurTask->fireLionLoopCount = 0;
        while ((s16)gCurTask->fireLionLoopCount <= 3) {
            if (abs(TaskGetNearestPlayerDx()) <= 71)
                break;
            TaskFaceNearestPlayer();
            sub_08095d40();
            t = gCurTask;
            if (t->fireLionWallHit != 0) {
                t->facing = -t->facing;
                sub_08095d40();
                break;
            }
            t->fireLionLoopCount++;
        }
    }
    TaskFaceNearestPlayer();
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    TaskSetFrame(18);
    TaskYieldTrampoline(8);
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    TaskSetMotionY(-0x28000, 0x2000, 0x30000);
    FireLionWaitForLanding();
    ActorSetTerrainBox(gUnk_08745304);
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    gCurTask->fireLionLoopCount = 0;
    do {
        TaskSetFrame(26);
        TaskYieldTrampoline(4);
        PlaySfx(0x23B);
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        TaskSetFrame(28);
        TaskYieldTrampoline(1);
        TaskSetFrame(29);
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->fireLionLoopCount++;
    } while ((s16)u->fireLionLoopCount <= 2);
    FireLionJumpBack();
    ActorSetState(FIRE_LION_STATE_WAIT);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionSlashUpdate(void)
{
    struct Task *t;

    switch (gCurTask->frame) {
    case 26:
        ActorCheckHitsWithBox(gUnk_0874513C);
        break;
    case 27:
        ActorCheckHitsWithBox(gUnk_08745158);
        break;
    case 28:
        ActorCheckHitsWithBox(gUnk_08745174);
        break;
    }
    t = gCurTask;
    if (t->accelX != 0 && t->velX * t->accelX >= 0)
        TaskStopX();
}

void FireLionState9(void)
{
    struct Task *t;
    s32 zero;

    TaskFaceNearestPlayer();
    ActorSetTerrainBox(gUnk_08745304);
    gCurTask->fireLionGlowing = 1;
    if (abs(TaskGetNearestPlayerDx()) <= 31)
        FireLionJumpBack();
    TaskStop();
    t = gCurTask;
    zero = 0;
    t->updateState = 8;
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    TaskSetFrame(18);
    TaskYieldTrampoline(8);
    TaskSetMotionY(-0x40000, 0x2200, 0x30000);
    FireLionWaitForLanding();
    ActorSetTerrainBox(gUnk_08745304);
    TaskStop();
    gCurTask->fireLionGlowing = zero;
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    TaskSetFrame(45);
    TaskYieldTrampoline(2);
    FireLionCharge();
}

void sub_0809616c(void)
{
    if (TaskGetNearestPlayerDy() > -24) {
        gCurTask->fireLionGlowing = 0;
        TaskSetEntry(sub_080962d0, gCurTaskIdx);
    }
}

void FireLionCharge(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 6;
    PlaySfx(500);
    gCurTask->fireLionWallHit = 0;
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_0874453C);
    TaskYieldTrampoline(48);
    while (gCurTask->facing == TaskGetFacingTowardNearestPlayer()) {
        if (abs(TaskGetNearestPlayerDx()) > 48)
            break;
        TaskYieldTrampoline(10);
    }
    t = gCurTask;
    t->updateState = 7;
    ActorStopAnim();
    TaskSetMotionXFacing(0x5A5A5A5A, -0x2000);
    u = gCurTask;
    u->accelY = 0x2000;
    u->speedLimitY = 0x30000;
    TaskSetFrame(47);
    TaskYieldTrampoline(8);
    TaskSetFrame(48);
    TaskYieldTrampoline(8);
    FireLionWaitForLanding();
    sub_08096924();
    ActorSetState(FIRE_LION_STATE_WAIT);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionChargeUpdate(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->fireLionWallHit != 0)
        TaskSetEntry(FireLionBounceOffWall, gCurTaskIdx);
}

void sub_080962ac(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->fireLionWallHit != 0 || t->velX * t->accelX >= 0)
        TaskStopX();
}

void sub_080962d0(void)
{
    gCurTask->updateState = 9;
    gCurTask->fireLionWallHit = 0;
    TaskStop();
    TaskSetMotionXFacing(-0x20000, 0x2000);
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    TaskSetFrame(45);
    TaskYieldTrampoline(2);
    TaskYieldTrampoline(12);
    TaskStop();
    FireLionCharge();
}

void FireLionBounceOffWall(void)
{
    s32 zero;

    gCurTask->updateState = 9;
    zero = 0;
    gCurTask->fireLionWallHit = zero;
    PlaySfx(504);
    RequestScreenShake(2);
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x3000, 0x30000);
    TaskSetFrame(47);
    TaskYieldTrampoline(8);
    TaskSetFrame(48);
    if (gCurTask->velY < 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->velY < 0);
    }
    FireLionWaitForLanding();
    ActorSetTerrainBox(gUnk_08745304);
    sub_08096924();
    ActorSetState(FIRE_LION_STATE_WAIT);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_080963c0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->fireLionWallHit != 0 && t->velY < 0)
        t->velY = 0;
}

void FireLionPounce(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gCurTask->updateState = 5;
    ActorSetTerrainBox(gUnk_08745304);
    ActorStopAnim();
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDx()) <= 47) {
        t = gCurTask;
        goto flip;
    }
    if (TaskGetNearestPlayerDx() >= 0)
        goto poscheck;
    if (-TaskGetNearestPlayerDx() > 80)
        goto doloop;
    goto rest;
flip:
    t->facing = -t->facing;
    sub_08095d40();
    goto rest;
poscheck:
    if (TaskGetNearestPlayerDx() > 80)
        goto doloop;
    goto rest;
doloop:
    {
        TaskFaceNearestPlayer();
        sub_08095e4c();
        gCurTask->fireLionLoopCount = 0;
        do {
            sub_08095d40();
            t = gCurTask;
            if (t->fireLionWallHit != 0)
                goto flip;
            TaskFaceNearestPlayer();
            if (abs(TaskGetNearestPlayerDx()) <= 79)
                goto rest;
            t = gCurTask;
            t->fireLionLoopCount++;
        } while ((s16)t->fireLionLoopCount <= 3);
    }
rest:
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->fireLionCatchActive = 1;
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    TaskSetFrame(18);
    TaskYieldTrampoline(8);
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    TaskSetFrame(20);
    TaskYieldTrampoline(8);
    gCurTask->onGround = 0;
    v = gCurTask;
    v->fireLionWallHit = 0;
    v->fireLionLanded = 0;
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    TaskSetMotionY(-0x38000, 0x2000, 0x30000);
    TaskSetFrame(36);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    gCurTask->fireLionCatchActive = 0;
    TaskStop();
    TaskSetFrame(37);
    TaskYieldTrampoline(8);
    TaskSetFrame(38);
    TaskYieldTrampoline(8);
    w = gCurTask;
    w->velY = 0x60000;
    w->accelY = 0x100;
    if (w->fireLionLanded == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->fireLionLanded == 0);
    }
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    FireLionCreateLandingStar();
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08744550);
    ActorSetExtraAttackBox(gUnk_08745040);
    TaskYieldTrampoline(2);
    ActorSetExtraAttackBox(0);
    TaskYieldTrampoline(gUnk_08744562[gCurTask->actorSpawnArg]);
    gCurTask->fireLionLoopCount = 0;
    do {
        gCurTask->onGround = 0;
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(1);
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(1);
        x = gCurTask;
        x->fireLionLoopCount++;
    } while ((s16)x->fireLionLoopCount <= 7);
    ActorStopAnim();
    TaskStop();
    TaskSetFrame(38);
    TaskYieldTrampoline(4);
    TaskSetFrame(36);
    TaskYieldTrampoline(8);
    sub_08096924();
    ActorSetState(FIRE_LION_STATE_WAIT);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionPounceUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = ActorTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->fireLionWallHit != 0 && t->velY < 0)
        t->velY = 0;
    u = gCurTask;
    if (u->accelX != 0 && u->velX * u->accelX >= 0)
        TaskStopX();
}

void sub_08096680(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 zero;

    TaskStop();
    t = gCurTask;
    zero = 0;
    t->fireLionCatchActive = zero;
    PlaySfx(0x236);
    TaskSetFrame(34);
    u = gCurTask;
    if (u->onGround == 0) {
        u->fireLionLanded = zero;
        u->fireLionLoopCount = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gCurTask;
            v->fireLionLoopCount++;
        } while ((s16)v->fireLionLoopCount <= 3);
        TaskStopX();
        w = gCurTask;
        w->velY = 0x48000;
        if (w->fireLionLanded == 0) {
            do {
                TaskYieldTrampoline(1);
            } while (gCurTask->fireLionLanded == 0);
        }
    } else {
        u->fireLionLoopCount = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            x = gCurTask;
            x->fireLionLoopCount++;
        } while ((s16)x->fireLionLoopCount <= 3);
        TaskStopX();
    }
    gCurTask->fireLionLoopCount = 0;
    do {
        if (gLocalPlayer == gCurTask->fireLionHeldPlayerSlot)
            PlaySfx(0x237);
        CreateChildTaskAtOffsetFacing(TASK_STAR_FLASH, 24, 0, 0);
        TaskSetFrame(30);
        TaskYieldTrampoline(4);
        TaskSetFrame(31);
        TaskYieldTrampoline(4);
        TaskSetFrame(32);
        TaskYieldTrampoline(4);
        TaskSetFrame(33);
        TaskYieldTrampoline(4);
        y = gCurTask;
        y->fireLionLoopCount++;
    } while ((s16)y->fireLionLoopCount <= 5);
    SetHeldPlayerState(gCurTask->fireLionHeldPlayerSlot, 6);
    gCurTask->fireLionHeldPlayerSlot = -1;
    FireLionJumpBack();
    gCurTask->fireLionCatchActive = 0;
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void FireLionWaitForLanding(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->onGround = 0;
    t = gCurTask;
    t->fireLionWallHit = 0;
    t->fireLionLanded = 0;
    ActorStopAnim();
    ActorSetTerrainBox(gUnk_0874530C);
    TaskSetFrame(13);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(14);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    u = gCurTask;
    if (u->velY > 0 && u->fireLionLanded == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->fireLionLanded == 0);
    }
    TaskStop();
}

void FireLionJumpBack(void)
{
    gCurTask->updateState = 10;
    TaskSetMotionY(-0x20000, 0x2000, 0x30000);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    sub_080968c0();
}

void sub_080968c0(void)
{
    struct Task *t;

    gCurTask->onGround = 0;
    t = gCurTask;
    t->fireLionWallHit = 0;
    t->fireLionLanded = 0;
    ActorStopAnim();
    TaskSetFrame(23);
    TaskYieldTrampoline(8);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    TaskSetFrame(25);
    while (gCurTask->fireLionLanded == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(1);
}

void sub_08096920(void)
{
}

void sub_08096924(void)
{
    if (TaskGetFacingTowardNearestPlayer() == 1) {
        if (gCurTask->pixelX - gViewRect[0] > 80) {
            TaskFaceNearestPlayer();
            TaskSetFrame(17);
            TaskYieldTrampoline(8);
            FireLionJumpBack();
        }
    } else {
        if (gCurTask->pixelX - gViewRect[0] <= 159) {
            TaskFaceNearestPlayer();
            TaskSetFrame(17);
            TaskYieldTrampoline(8);
            FireLionJumpBack();
        }
    }
}

void sub_0809699c(void)
{
    FireLionUpdatePalette();
    CallTableEntry(gCurTask->updateState, 13, gFireLionStateUpdates);
    sub_08097024();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void FireLionUpdate(void)
{
    FireLionUpdatePalette();
    FireLionCheckCatch();
    if (ActorHasExtraFrame() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 13, gFireLionStateUpdates);
    } else {
        CallTableEntry(gCurTask->updateState, 13, gFireLionStateUpdates);
    }
    sub_08097024();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void sub_08096a28(void)
{
    TaskSetEntry(FireLionEnterState, gCurTaskIdx);
}

void FireLionCheckCatch(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;
    struct PlayerState *p;

    t = gCurTask;
    if (t->fireLionCatchActive == 0)
        return;
    switch (t->frame) {
    case 8:
    case 17:
    case 18:
    case 19:
    case 20:
        ActorCheckHitsWithBox(gUnk_08745190);
        break;
    case 9:
        ActorCheckHitsWithBox(gUnk_087451AC);
        break;
    case 10:
        ActorCheckHitsWithBox(gUnk_087451C8);
        break;
    case 11:
        ActorCheckHitsWithBox(gUnk_087451E4);
        break;
    case 36:
        ActorCheckHitsWithBox(gUnk_08745200);
        break;
    }
    u = gCurTask;
    if (u->hitKind == HIT_KIND_CATCH) {
        p = gPlayerStates;
        i = u->hitterSlot;
        if (p[i].ability != ABILITY_STONE) {
            u->fireLionHeldPlayerSlot = i;
            TaskFaceToward(i);
            HoldPlayer(gCurTask->fireLionHeldPlayerSlot, gCurTaskIdx, 5);
            TaskSetEntry(sub_08096680, gCurTaskIdx);
        }
    }
}

void FireLionDefeat(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = 11;
    zero = 0;
    ActorStopAnim();
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        EndMidBossFightWithReward();
    sub_080667c0(1, 21);
    TaskStop();
    t = gCurTask;
    t->fireLionLanded = zero;
    t->fireLionCatchActive = zero;
    t->fireLionGlowing = 1;
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    ActorSetHitReactions(gFireLionDefeatedHitReactions);
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x1A00, 0x30000);
    TaskSetFrame(21);
    ActorSetAttackBox(gUnk_08744F0C);
    ActorSetExtraAttackBox(gUnk_087446E8[gCurTask->frame]);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    ActorSetTerrainBox(gUnk_08745304);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(38);
    ActorSetAttackBox(gUnk_087450CC);
    ActorSetExtraAttackBox(gUnk_087450E8);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    while (gCurTask->fireLionLanded == 0)
        TaskYieldTrampoline(1);
    gCurTask->actorAnimDelay = ActorStartAnim(gUnk_08744550);
    TaskSetFrameByFacing(40);
    ActorSetAttackBox(gUnk_08745104);
    ActorSetExtraAttackBox(gUnk_08745120);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateChildTaskHere(TASK_STAR_FLASH, 0);
    gCurTask->fireLionDustTimer = 0;
    TaskSetMotionXFacing(-0x10000, 0x600);
    TaskYieldTrampoline(30);
    TaskStop();
    TaskYieldTrampoline(170);
    ActorShakeVertically();
    ActorDie();
}

void FireLionDefeatUpdate(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->fireLionLanded == 1 && t->velX != 0) {
        t->fireLionDustTimer++;
        if (t->fireLionDustTimer == 16) {
            CreateDustTrail(0, 1, 8, 10);
            gCurTask->fireLionDustTimer = 0;
        }
    }
}

s32 FireLionLand(void)
{
    switch (gCurTask->state) {
    case 0:
    case 3:
    case 4:
        TaskStopY();
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        gCurTask->fireLionLanded = 1;
        return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        TaskStopY();
        gCurTask->fireLionLanded = 1;
        return 0;
    case 10:
        TaskStop();
        gCurTask->fireLionLanded = 1;
        return 0;
    }
}

s32 FireLionHitWall(void)
{
    gCurTask->fireLionWallHit = 1;
    TaskStopX();
    return 0;
}

s32 FireLionReactToDamage(void)
{
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    RequestScreenShake(2);
    return 0;
}

s32 FireLionReactToDefeat(void)
{
    struct Task *t;

    ActorFaceHitter();
    t = gCurTask;
    if (t->fireLionHeldPlayerSlot >= 0) {
        ReleaseHeldPlayer(t->fireLionHeldPlayerSlot, -t->facing);
        gCurTask->fireLionHeldPlayerSlot = -1;
    }
    ActorSetState(FIRE_LION_STATE_DEFEAT);
    TaskSetEntry(FireLionEnterState, gCurTaskIdx);
    return 1;
}

void sub_08096e70(void)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < 4; i++) {
        TaskFree(gCurTask->unk24 & 0xFF);
        t = gCurTask;
        t->unk24 >>= 8;
    }
}

void FireLionUpdatePalette(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s16 *q;

    a = gCurTask->u8C.actor;
    if ((a->paletteLocked & 1) == 0) {
    a->paletteOverridden = 1;
    t = gCurTask;
    if ((t->hitTimer != 0 && t->u8C.actor->hitState != 0) || t->fireLionGlowing != 0) {
        u = gCurTask;
        q = (s16 *)&u->fireLionPalettePhase;
        if (*q <= 3) {
            BlendColors(gUnk_08744598[u->u8C.actor->paletteVariant][0],
                         gUnk_08744598[u->u8C.actor->paletteVariant][1],
                         gUnk_087445D8[*q], 16,
                         &gObjPalette[(u->tileWord >> 12) * 32]);
        } else {
            BlendColors(gUnk_08744598[u->u8C.actor->paletteVariant][0],
                         gUnk_082B07BC,
                         gUnk_087445D8[*q], 16,
                         &gObjPalette[(u->tileWord >> 12) * 32]);
        }
    } else {
        BlendColors(gUnk_08744598[t->u8C.actor->paletteVariant][0],
                     gUnk_08744598[t->u8C.actor->paletteVariant][1],
                     gUnk_087445D8[(s16)t->fireLionPalettePhase], 16,
                     &gObjPalette[(t->tileWord >> 12) * 32]);
    }
    v = gCurTask;
    v->fireLionPalettePhase++;
    if ((s16)v->fireLionPalettePhase > 7)
        v->fireLionPalettePhase = 0;
    }
}

void FireLionChooseNextState(void)
{
    struct Task *t;
    u16 v;

    t = gCurTask;
    t->fireLionSequencePhase++;
    if (t->fireLionSequencePhase > 7)
        t->fireLionSequencePhase = 0;
    v = gUnk_087445E8[gCurTask->fireLionSequencePhase + gCurTask->actorSpawnArg * 8];
    if (gUnk_087445E8[gCurTask->fireLionSequencePhase + gCurTask->actorSpawnArg * 8] == 11)
        v = gUnk_08744608[RandomRange(8)];
    ActorSetState(v);
}

void sub_08097024(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gCurTask;
    if (t->state != 10) {
        ActorSetAttackBox(gUnk_08744618[t->frame]);
        u = gCurTask;
        v = u->frame;
        if (v < 40 || v > 43)
            ActorSetExtraAttackBox(gUnk_087446E8[u->frame]);
        sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    }
}

void FireLionCreateLandingStar(void)
{
    struct Task *t;
    s32 d;
    u16 x;
    u16 y;

    d = TaskGetFacingTowardNearestPlayer();
    t = gCurTask;
    x = t->pixelX + d * 24;
    y = t->pixelY + 3;
    CreateInhalableStar(x, y, d, 3);
}
