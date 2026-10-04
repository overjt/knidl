#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern s32 GetCollisionTileAtPixel(u16 x, u16 y);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void ActorSetAux(void *p);
extern void ActorSetExtraAttackBox(void *p);
extern void AngleToVector(s16 t, s16 mag);
extern u32 ActorCheckHits(void);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 ActorHasExtraFrame(void);

void Task_BugzzyLadybug(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    gCurTask->frameTable = gUnk_087536FC;
    TaskFaceLikeParent();
    u = gCurTask;
    u->bugzzyLadybugAngle = gUnk_087441BC[u->variant] * (s8)u->facing + 384;
    u->bugzzyLadybugHitsActive = zero;
    u->bugzzyLadybugSpeed = 384;
    u->bugzzyLadybugFrameTimer = 2;
    u->variant = zero;
    CallTableEntry(gCurTask->variant, 1, gBugzzyLadybugVariants);
}

void BugzzyLadybugInit(void)
{
    gCurTask->updateCallback = (u32)BugzzyLadybugUpdate;
    ActorSetState(BUGZZY_LADYBUG_STATE_0);
    CallTableEntry(gCurTask->state, 1, gBugzzyLadybugStates);
}

void BugzzyLadybugUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBugzzyLadybugStateUpdates);
    if (gCurTask->bugzzyLadybugHitsActive == 1)
        ActorCheckHits();
    ActorReactToHit();
}

void BugzzyLadybugState0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gCurTask->updateState = BUGZZY_LADYBUG_STATE_0;
    TaskSetFrame(4);
    PlaySfx(506);
    BugzzyLadybugSetVelocity();
    TaskYieldTrampoline(8);
    gCurTask->bugzzyLadybugLoopCount = 0;
    do {
        BugzzyLadybugSetVelocity();
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->bugzzyLadybugSpeed -= 48;
        t->bugzzyLadybugLoopCount++;
    } while ((s16)t->bugzzyLadybugLoopCount <= 7);
    t->velX = 0;
    t->velY = 0;
    t->bugzzyLadybugHitsActive = 1;
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
    gCurTask->bugzzyLadybugLoopCount = 0;
    do {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->bugzzyLadybugLoopCount++;
    } while ((s16)u->bugzzyLadybugLoopCount <= 1);
    v = gCurTask;
    v->velY = 0;
    TaskSetMotionXFacing(gUnk_087441C4[v->actorSpawnArg], 0x5A5A5A5A);
    while (1) {
        if (TaskGetYDirBitToNearestPlayer() == 1) {
            w = gCurTask;
            w->velY += 0x800;
            if (w->velY > 0x10000)
                w->velY = 0x10000;
        } else {
            x = gCurTask;
            x->velY += -0x800;
            if (x->velY < -0x10000)
                x->velY = -0x10000;
        }
        TaskYieldTrampoline(1);
    }
}

void BugzzyLadybugState0Update(void)
{
    struct Task *t;

    t = gCurTask;
    t->bugzzyLadybugFrameTimer--;
    if (t->bugzzyLadybugFrameTimer == 0) {
        t->bugzzyLadybugFrameTimer = 2;
        t->frame ^= 1;
    }
}

void BugzzyLadybugSetVelocity(void)
{
    struct Task *t;

    t = gCurTask;
    AngleToVector(t->bugzzyLadybugAngle, t->bugzzyLadybugSpeed);
    if (gCurTask->actorSpawnArg != 0)
        gUnk_030023B4 += gUnk_030023B4 >> 1;
    t = gCurTask;
    t->velX = gUnk_030023B4;
    t->velY = gUnk_030023D4;
}

void Task_GrandWheelie(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->layer = 11;
    gCurTask->frameTable = gGrandWheelieFrames;
    ActorInitBossGfx(0);
    gUnk_02007D00[8]++;
    gCurTask->grandWheelieSummonPhase = -1;
    TaskFaceNearestPlayer();
    ActorCollideTerrain();
    ActorIntroPoseUntilMidBossFight(gUnk_0874433C);
    MidBossResetHealth();
    CallTableEntry(gCurTask->variant, 1, gGrandWheelieVariants);
}

void GrandWheelieInit(void)
{
    struct Task *t;
    struct Task *u;

    if (IsMidBossDroppingIn() != 0) {
        t = gCurTask;
        t->updateCallback = (u32)sub_080942b4;
        t->onGround = 0;
        ActorSetState(GRAND_WHEELIE_STATE_FALL);
        CallTableEntry(gCurTask->state, 11, gGrandWheelieStates);
    } else {
        u = gCurTask;
        u->updateCallback = (u32)GrandWheelieUpdate;
        u->onGround = 1;
        ActorSetState(GRAND_WHEELIE_STATE_1);
        CallTableEntry(gCurTask->state, 11, gGrandWheelieStates);
    }
}

void GrandWheelieEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)GrandWheelieUpdate;
    CallTableEntry(t->state, 11, gGrandWheelieStates);
}

void sub_080942b4(void)
{
    CallTableEntry(gCurTask->updateState, 11, gGrandWheelieStateUpdates);
    sub_08094358();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void GrandWheelieUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->grandWheelieFlashTimer > 0) {
        t->grandWheelieFlashTimer--;
        ActorFlashPalette(gUnk_0826F170, 16);
    } else {
        ActorClearPaletteOverride();
    }
    if (ActorHasExtraFrame() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 11, gGrandWheelieStateUpdates);
    } else {
        CallTableEntry(gCurTask->updateState, 11, gGrandWheelieStateUpdates);
    }
    sub_08094358();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void sub_08094358(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gCurTask;
    if (t->state == GRAND_WHEELIE_STATE_DEFEAT) {
        if (t->grandWheelieDefeatPhase == 0 && t->velY < 0) {
            v = t->frame;
            if (v >= 4 && v <= 9) {
                ActorSetAttackBox(gUnk_08744A20);
                ActorSetExtraAttackBox(gUnk_08744A04);
            } else if (v >= 10 && v <= 19) {
                ActorSetAttackBox(gUnk_08744A74);
                ActorSetExtraAttackBox(gUnk_08744A58);
            } else if (v >= 20 && v <= 27) {
                ActorSetAttackBox(gUnk_08744AC8);
                ActorSetExtraAttackBox(gUnk_08744AAC);
            } else if (v >= 28 && v <= 31) {
                ActorSetAttackBox(gUnk_08744B54);
                ActorSetExtraAttackBox(gUnk_08744B38);
            } else if (v >= 32 && v <= 35) {
                ActorSetAttackBox(gUnk_08744BA8);
                ActorSetExtraAttackBox(gUnk_08744B8C);
            } else if (v >= 36 && v <= 47) {
                ActorSetAttackBox(gUnk_08744C34);
                ActorSetExtraAttackBox(gUnk_08744C18);
            } else {
                ActorSetAttackBox(gUnk_08744C88);
                ActorSetExtraAttackBox(gUnk_08744C6C);
            }
        } else {
            u = gCurTask;
            v = u->frame;
            if (v >= 20 && v <= 27) {
                ActorSetAttackBox(gUnk_08744AE4);
                ActorSetExtraAttackBox(gUnk_08744B00);
            } else if (v >= 32 && v <= 35) {
                ActorSetAttackBox(gUnk_08744BC4);
                ActorSetExtraAttackBox(gUnk_08744BE0);
            } else {
                ActorSetAttackBox(gUnk_08744CA4);
                ActorSetExtraAttackBox(gUnk_08744CC0);
            }
        }
    } else {
        v = t->frame;
        if (v >= 4 && v <= 9) {
            ActorSetAttackBox(gUnk_087449E8);
            ActorSetExtraAttackBox(gUnk_08744A04);
            ActorSetAux(gUnk_0874531C);
        } else if (v >= 10 && v <= 19) {
            ActorSetAttackBox(gUnk_08744A3C);
            ActorSetExtraAttackBox(gUnk_08744A58);
            ActorSetAux(gUnk_08745324);
        } else if (v >= 20 && v <= 27) {
            ActorSetAttackBox(gUnk_08744A90);
            ActorSetExtraAttackBox(gUnk_08744AAC);
            ActorSetAux(gUnk_0874532C);
        } else if (v >= 28 && v <= 31) {
            ActorSetAttackBox(gUnk_08744B1C);
            ActorSetExtraAttackBox(gUnk_08744B38);
            ActorSetAux(gUnk_08745334);
        } else if (v >= 32 && v <= 35) {
            ActorSetAttackBox(gUnk_08744B70);
            ActorSetExtraAttackBox(gUnk_08744B8C);
            ActorSetAux(gUnk_0874533C);
        } else if (v >= 36 && v <= 47) {
            ActorSetAttackBox(gUnk_08744BFC);
            ActorSetExtraAttackBox(gUnk_08744C18);
            ActorSetAux(gUnk_08745344);
        } else {
            ActorSetAttackBox(gUnk_08744C50);
            ActorSetExtraAttackBox(gUnk_08744C6C);
            ActorSetAux(gUnk_0874534C);
        }
    }
}

void GrandWheelieFall(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = GRAND_WHEELIE_STATE_FALL;
    v = GrandWheelieStartAnim(gUnk_0874433C);
    t = gCurTask;
    t->actorAnimDelay = v;
    t->accelY = 9472;
    t->speedLimitY = 0x30000;
    TaskYieldTrampoline(24);
    gCurTask->updateCallback = (u32)GrandWheelieUpdate;
    TaskSleepForever();
}

void GrandWheelieFallUpdate(void)
{
    gCurTask->actorAnimDelay = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
}

void GrandWheelieState1(void)
{
    gCurTask->updateState = GRAND_WHEELIE_STATE_1;
    sub_08066580();
    ActorSetState(GRAND_WHEELIE_STATE_2);
    TaskSleepForever();
}

void GrandWheelieState1Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != GRAND_WHEELIE_STATE_1) {
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    } else {
        gCurTask->actorAnimDelay = GrandWheelieTickAnim(t->actorAnimDelay);
    }
}

void GrandWheelieState2(void)
{
    struct Task *t;

    gCurTask->updateState = GRAND_WHEELIE_STATE_2;
    if (GrandWheelieCheckSummon() != 0) {
        ActorSetState(GRAND_WHEELIE_STATE_SUMMON);
        TaskSleepForever();
    }
    t = gCurTask;
    t->grandWheelieSavedPixelX = t->pixelX;
    switch (sub_080947cc()) {
    case 0:
        gCurTask->grandWheelieStandTimer = gUnk_030023D4;
        gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_0874433C);
        TaskSleepForever();
        break;
    case 1:
        gCurTask->grandWheelieStandTimer = gUnk_030023D4;
        gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_087443A8);
        while (1) {
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
        }
    }
}

void GrandWheelieState2Update(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 x;

    t = gCurTask;
    if (t->state != GRAND_WHEELIE_STATE_2)
        goto rearm;
    x = GrandWheelieTickAnim(t->actorAnimDelay);
    u = gCurTask;
    u->actorAnimDelay = x;
    u->grandWheelieStandTimer--;
    if (u->grandWheelieStandTimer != 0)
        return;
    TaskStopX();
    v = gCurTask;
    v->posX = v->grandWheelieSavedPixelX << 16;
    if (RandomRange(4) == 0)
        goto quiet;
    ActorSetState(GRAND_WHEELIE_STATE_CHARGE);
rearm:
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    return;
quiet:
    ActorSetState(GRAND_WHEELIE_STATE_HOP);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 sub_080947cc(void)
{
    TaskFaceNearestPlayer();
    gUnk_030023D4 = gUnk_0874449C[gCurTask->actorSpawnArg * 2 + (gFrameCount & 1)];
    return gFrameCount & 1;
}

s32 GrandWheelieCheckSummon(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->grandWheelieSummonPhase = (t->grandWheelieSummonPhase + 1) & 3;
    if (t->grandWheelieSummonPhase == 3) {
        if (t->grandWheelieRushCooldown > 3)
            t->grandWheelieRushCooldown = 0;
        u = gCurTask;
        if (u->grandWheelieRushCooldown == 0)
            return 1;
        u->grandWheelieRushCooldown++;
    }
    return 0;
}

void GrandWheelieHop(void)
{
    gCurTask->updateState = GRAND_WHEELIE_STATE_HOP;
    TaskStop();
    if (GrandWheelieCheckSummon() != 0) {
        ActorSetState(GRAND_WHEELIE_STATE_SUMMON);
        TaskSleepForever();
    }
    TaskFaceNearestPlayer();
    gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744360);
    sub_08094908();
    gCurTask->grandWheelieHopsLeft = gUnk_030023D4;
    sub_08094894();
}

void sub_08094894(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->grandWheelieHopsLeft != 0) {
        t->grandWheelieHopsLeft--;
        t->onGround = 0;
        TaskSetMotionY(-0x50000, 0x4A00, 0x70000);
        TaskSleepForever();
    }
    ActorSetState(GRAND_WHEELIE_STATE_CHARGE);
    TaskSleepForever();
}

void GrandWheelieHopUpdate(void)
{
    struct Task *t;
    s32 x;

    x = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->state != GRAND_WHEELIE_STATE_HOP)
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

void sub_08094908(void)
{
    if (RandomRange(2) != 0)
        gUnk_030023D4 = 1;
    else
        gUnk_030023D4 = 3;
}

void GrandWheelieCharge(void)
{
    struct Task *t;

    gCurTask->updateState = GRAND_WHEELIE_STATE_CHARGE;
    TaskStop();
    if (GrandWheelieCheckSummon() != 0) {
        ActorSetState(GRAND_WHEELIE_STATE_SUMMON);
        TaskSleepForever();
    }
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = t->facing;
    gUnk_02007D00[1] = PlaySfx(0x209);
    if (gCurTask->u8C.actor->animScript != gUnk_08744384)
        gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744384);
    TaskYieldTrampoline(45);
    gCurTask->grandWheelieRushRange = GrandWheelieIsNearPlayer();
    gCurTask->grandWheelieFlameSlot = CreateDashFlame(-10, 5);
    TaskSetMotionXFacing(gUnk_087444A4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    gCurTask->grandWheelieGroundClass = -1;
    TaskSleepForever();
}

void GrandWheelieChargeUpdate(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 x;
    s32 y;
    s32 z;
    s32 q;
    s16 *p;
    u16 a;

    x = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->state != GRAND_WHEELIE_STATE_CHARGE)
        goto rearm;
    if (t->velX == 0)
        return;
    sub_08094bbc();
    if (gCurTask->facing != TaskGetFacingTowardNearestPlayer())
        goto other;
    if (gCurTask->grandWheelieRushRange != 0)
        return;
    y = GrandWheelieIsNearPlayer();
    gCurTask->grandWheelieRushRange = y;
    if (y == 0)
        return;
    if (RandomRange(3) != 0)
        return;
    v = gCurTask;
    if (v->grandWheelieRushCooldown != 0)
        return;
    v->grandWheelieRushCooldown = 1;
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
        TaskFree(gCurTask->grandWheelieFlameSlot);
    ActorSetState(GRAND_WHEELIE_STATE_7);
rearm:
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    return;
other:
    w = gCurTask;
    if (w->grandWheelieRushRange < 0) {
        a = w->pixelX;
        p = &w->pixelY;
        switch (GetCollisionTileAtPixel(a, ((s8 *)w->u8C.actor->terrainBox)[3] + *p)) {
        case 2:
            if (gCurTask->facing == -1)
                return;
            break;
        case 3:
            if (gCurTask->facing == 1)
                return;
            break;
        }
        StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
        if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
            TaskFree(gCurTask->grandWheelieFlameSlot);
        ActorSetState(GRAND_WHEELIE_STATE_5);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    }
    z = GrandWheelieIsNearPlayer();
    gCurTask->grandWheelieRushRange = z;
    if (z == 1)
        return;
    gCurTask->grandWheelieRushRange = -1;
    q = RandomRange(3);
    if (q != 0)
        return;
    gCurTask->grandWheelieRushCooldown = q;
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
        TaskFree(gCurTask->grandWheelieFlameSlot);
    ActorSetState(GRAND_WHEELIE_STATE_6);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 GrandWheelieIsNearPlayer(void)
{
    if (abs(TaskGetNearestPlayerDx()) <= 63)
        return 1;
    return 0;
}

void sub_08094bbc(void)
{
    struct Task *t;
    u8 v;
    s32 r;

    t = gCurTask;
    r = sub_08094d10();
    v = r;
    if (t->grandWheelieGroundClass == (s8)r)
        return;
    gCurTask->grandWheelieGroundClass = (s8)v;
    switch ((s8)v) {
    case 0:
        if (gCurTask->u8C.actor->animScript != gUnk_08744360)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444A4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        break;
    case 1:
        if (gCurTask->u8C.actor->animScript != gUnk_08744360)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444AC[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        break;
    case 2:
        if (gCurTask->u8C.actor->animScript != gUnk_08744360)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444B4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        break;
    case 3:
        if (gCurTask->u8C.actor->animScript != gUnk_0874433C)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_0874433C);
        TaskSetMotionXFacing(gUnk_087444BC[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        break;
    case 4:
        if (gCurTask->u8C.actor->animScript != gUnk_08744384)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744384);
        TaskSetMotionXFacing(gUnk_087444C4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
        break;
    }
}

s32 sub_08094d10(void)
{
    struct Task *t;

    t = gCurTask;
    if ((t->onGround & 1) != 0) {
        if ((u8)(gTerrainResult[4] - 1) <= 3) {
            switch (gTerrainResult[4]) {
            case 3:
                if (t->facing == 1)
                    return 1;
                return 2;
            case 1:
                if (t->facing == 1)
                    return 3;
                return 4;
            case 4:
                if (t->facing == 1)
                    return 2;
                return 1;
            case 2:
                if (t->facing == 1)
                    return 4;
                return 3;
            }
        } else {
            return 0;
        }
    }
    return -1;
}

void GrandWheelieState5(void)
{
    struct Task *t;

    gCurTask->updateState = GRAND_WHEELIE_STATE_5;
    t = gCurTask;
    t->grandWheelieSkidTimer = 60;
    t->grandWheelieSkidDir = t->facing;
    t->grandWheelieDustTimer = 0;
    gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_087443D0);
    PlaySfx(0x20A);
    TaskSleepForever();
}

void GrandWheelieState5Update(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->u8C.actor->animScript == 0) {
        TaskTurnAround();
        gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_0874433C);
    }
    if (GrandWheelieBrake() != 0) {
        gCurTask->velX = 0;
        ActorSetState(GRAND_WHEELIE_STATE_2);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return;
    }
    u = gCurTask;
    u->grandWheelieSkidTimer--;
    if (u->grandWheelieSkidTimer >= 0)
        return;
    if ((gFrameCount & 1) != 0) {
        ActorSetState(GRAND_WHEELIE_STATE_CHARGE);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return;
    }
    ActorSetState(GRAND_WHEELIE_STATE_2);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 GrandWheelieBrake(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    if (gCurTask->grandWheelieSkidDir == 1) {
        a = sub_08094d10();
        t = gCurTask;
        t->velX -= gUnk_087444CC[a];
        if (t->velX < 0)
            return 1;
    } else {
        b = sub_08094d10();
        u = gCurTask;
        u->velX += gUnk_087444CC[b];
        if (u->velX > 0)
            return 1;
    }
    v = gCurTask;
    v->grandWheelieDustTimer++;
    if (v->grandWheelieDustTimer == 8) {
        if (v->grandWheelieSkidDir == v->facing)
            CreateDustTrail(1, 1, -4, 12);
        else
            CreateDustTrail(1, 1, -4, 12);
        gCurTask->grandWheelieDustTimer = 0;
    }
    return 0;
}

void GrandWheelieState6(void)
{
    struct Task *t;
    s32 zero;
    s32 v;

    gCurTask->updateState = GRAND_WHEELIE_STATE_6;
    zero = 0;
    v = GrandWheelieStartAnim(gUnk_087443BC);
    t = gCurTask;
    t->actorAnimDelay = v;
    t->grandWheelieSkidDir = t->facing;
    t->grandWheelieDustTimer = zero;
    PlaySfx(0x20A);
    TaskSleepForever();
}

void GrandWheelieState6Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->grandWheelieFlashTimer > 0)
        return;
    gCurTask->actorAnimDelay = GrandWheelieTickAnim(t->actorAnimDelay);
    if (GrandWheelieBrake() != 0) {
        gCurTask->velX = 0;
        ActorSetState(GRAND_WHEELIE_STATE_2);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    }
}

void GrandWheelieState7(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;
    s32 one;
    s32 v;

    gCurTask->updateState = GRAND_WHEELIE_STATE_7;
    zero = 0;
    gCurTask->grandWheelieRushCooldown = one = 1;
    v = GrandWheelieStartAnim(gUnk_087443D0);
    t = gCurTask;
    t->actorAnimDelay = v;
    t->grandWheelieSkidDir = t->facing;
    t->grandWheelieRushPhase = zero;
    t->grandWheelieDustTimer = zero;
    PlaySfx(0x20A);
    TaskYieldTrampoline(30);
    gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744384);
    gCurTask->grandWheelieFlameSlot = CreateDashFlame(-10, 5);
    TaskSetMotionXFacing(gUnk_087444A4[0], 0x5A5A5A5A);
    gCurTask->grandWheelieRushPhase = one;
    gUnk_02007D00[1] = PlaySfx(0x209);
    TaskYieldTrampoline(30);
    if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
        TaskFree(gCurTask->grandWheelieFlameSlot);
    gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_087443D0);
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    u = gCurTask;
    u->grandWheelieSkidDir = u->facing;
    u->grandWheelieRushPhase = zero;
    u->grandWheelieDustTimer = zero;
    PlaySfx(0x20A);
    TaskYieldTrampoline(30);
    gCurTask->grandWheelieRushPhase = 2;
    TaskSleepForever();
}

void GrandWheelieState7Update(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    v = t->grandWheelieRushPhase;
    switch (v) {
    case 0:
        gCurTask->actorAnimDelay = GrandWheelieTickAnim(t->actorAnimDelay);
        if (gCurTask->u8C.actor->animScript == 0) {
            TaskTurnAround();
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_0874433C);
        }
        if (GrandWheelieBrake() != 0)
            gCurTask->velX = v;
        break;
    case 1:
        gCurTask->actorAnimDelay = GrandWheelieTickAnim(t->actorAnimDelay);
        sub_08094bbc();
        break;
    case 2:
        ActorSetState(GRAND_WHEELIE_STATE_CHARGE);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        break;
    }
}

void GrandWheelieSummon(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;
    s32 zero;

    gCurTask->updateState = GRAND_WHEELIE_STATE_SUMMON;
    zero = 0;
    TaskStop();
    GrandWheelieStartAnim(gUnk_087443E4);
    TaskSetMotionY(-0x30000, 0x3500, 0x30000);
    gCurTask->onGround = zero;
    gCurTask->grandWheelieLanded = zero;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->grandWheelieLanded == 0);
    TaskSetMotionY(-0x18000, 0x3500, 0x30000);
    gCurTask->onGround = 0;
    gCurTask->grandWheelieLanded = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->grandWheelieLanded == 0);
    TaskYieldTrampoline(16);
    PlaySfx(506);
    t = gCurTask;
    a = t->u8C.actor;
    sp.subtype = 12;
    sp.taskType = TASK_GRAND_WHEELIE_MINI_WHEELIE;
    sp.variant = t->variant;
    sp.spawnArg = t->actorSpawnArg;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 1;
    gCurTask->grandWheelieMiniWheelieSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
    TaskYieldTrampoline(20);
    ActorSetState(GRAND_WHEELIE_STATE_2);
    TaskSleepForever();
}

void GrandWheelieSummonUpdate(void)
{
    struct Task *t;
    s32 x;

    x = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->state != GRAND_WHEELIE_STATE_SUMMON)
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

void GrandWheelieBounceOffWall(void)
{
    struct Task *t;

    gCurTask->updateState = GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL;
    t = gCurTask;
    if (t->velX > 0)
        t->facing = 1;
    else
        t->facing = -1;
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    gCurTask->onGround = 0;
    GrandWheelieStartAnim(gUnk_08744408);
    gCurTask->grandWheelieLanded = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->grandWheelieLanded == 0);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x2500, 0x30000);
    gCurTask->onGround = 0;
    GrandWheelieStartAnim(gUnk_0874441C);
    gCurTask->grandWheelieLanded = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->grandWheelieLanded == 0);
    TaskYieldTrampoline(30);
    ActorSetState(GRAND_WHEELIE_STATE_2);
    TaskSleepForever();
}

void GrandWheelieBounceOffWallUpdate(void)
{
    struct Task *t;
    s32 x;

    x = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    t = gCurTask;
    t->actorAnimDelay = x;
    if (t->state != GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL)
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

void GrandWheelieDefeat(void)
{
    struct Task *t;
    s32 z1;
    s32 z2;

    gCurTask->updateState = GRAND_WHEELIE_STATE_DEFEAT;
    z1 = 0;
    if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
        TaskFree(gCurTask->grandWheelieFlameSlot);
    ActorSetHitReactions(gGrandWheelieDefeatedHitReactions);
    gCurTask->grandWheelieDefeatPhase = z1;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        EndMidBossFightWithReward();
    MidBossStartDefeat(1, 32);
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744408);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x1A00, 0x30000);
    gCurTask->onGround = z1;
    while (gCurTask->grandWheelieDefeatPhase == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateChildTaskHere(TASK_STAR_FLASH, 0);
    GrandWheelieStartAnim(gUnk_0874441C);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    RequestScreenShake(2);
    gCurTask->grandWheelieDustTimer = z2 = 0;
    TaskYieldTrampoline(32);
    TaskStop();
    t = gCurTask;
    t->grandWheelieDefeatPhase = 2;
    t->grandWheelieDefeatDone = z2;
    CreateChildTaskHere(TASK_STAR_FLASH, 0);
    TaskYieldTrampoline(170);
    ActorShakeVertically();
    gCurTask->grandWheelieDefeatDone = 1;
    TaskSleepForever();
}

void GrandWheelieDefeatUpdate(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->actorAnimDelay = GrandWheelieTickAnim(gCurTask->actorAnimDelay);
    ActorFlashPalette(gUnk_0826F170, 16);
    t = gCurTask;
    if (t->grandWheelieDefeatPhase == 1) {
        t->grandWheelieDustTimer++;
        if (t->grandWheelieDustTimer == 16) {
            CreateDustTrail(0, 1, 8, 10);
            gCurTask->grandWheelieDustTimer = 0;
        }
    }
    u = gCurTask;
    if (u->grandWheelieDefeatPhase == 2 && u->grandWheelieDefeatDone != 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

s32 GrandWheelieLand(void)
{
    switch (gCurTask->state) {
    case GRAND_WHEELIE_STATE_FALL:
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(504);
        ActorSetState(GRAND_WHEELIE_STATE_1);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case GRAND_WHEELIE_STATE_HOP:
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(0x1F7);
        if (gCurTask->u8C.actor->animScript != gUnk_08744384)
            gCurTask->actorAnimDelay = GrandWheelieStartAnim(gUnk_08744384);
        TaskSetEntry(sub_08094894, gCurTaskIdx);
        return 1;
    case GRAND_WHEELIE_STATE_SUMMON:
    case GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL:
    case GRAND_WHEELIE_STATE_DEFEAT:
        TaskStop();
        if (gCurTask->state != GRAND_WHEELIE_STATE_DEFEAT)
            PlaySfx(504);
        gCurTask->grandWheelieLanded = 1;
        break;
    }
    return 0;
}

s32 GrandWheelieHitWall(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->state) {
    case GRAND_WHEELIE_STATE_CHARGE:
        StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
        if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
            TaskFree(gCurTask->grandWheelieFlameSlot);
        ActorSetState(GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case GRAND_WHEELIE_STATE_5:
    case GRAND_WHEELIE_STATE_6:
    case GRAND_WHEELIE_STATE_DEFEAT:
        TaskStopX();
        break;
    case GRAND_WHEELIE_STATE_7:
        if (gCurTask->grandWheelieRushPhase == 0) {
            TaskStopX();
            break;
        }
        TaskStop();
        StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
        if (gTaskSlotTypes[gCurTask->grandWheelieFlameSlot] == TASK_DASH_FLAME)
            TaskFree(gCurTask->grandWheelieFlameSlot);
        ActorSetState(GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case GRAND_WHEELIE_STATE_SUMMON:
    case GRAND_WHEELIE_STATE_BOUNCE_OFF_WALL:
        break;
    }
    return 0;
}

void GrandWheelieReactToDamage(void)
{
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    RequestScreenShake(2);
    gCurTask->grandWheelieFlashTimer = 32;
}

void GrandWheelieReactToDefeat(void)
{
    ActorFaceHitter();
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    ActorSetState(GRAND_WHEELIE_STATE_DEFEAT);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 GrandWheelieStartAnim(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    a->animScript = p;
    a->animScriptPos = 0;
    return GrandWheelieStepAnim();
}

s32 GrandWheelieStepAnim(void)
{
    struct Actor *a;
    struct AnimCmd *c;
    s32 r;

    a = gCurTask->u8C.actor;
    c = a->animScript;
    c += a->animScriptPos;
    if (c->frame == -3) {
        a->animScriptPos = 0;
        c = a->animScript;
    } else if (c->frame == -2) {
        r = c->frame;
        a->animScript = 0;
        goto tail;
    }
    if (gCurTask->facing == 1) {
        if ((c->frame & 1) != 0)
            TaskSetFrameFlip(c->frame);
        else
            TaskSetFrameNoFlip(c->frame);
    } else {
        if ((c->frame & 1) != 0)
            TaskSetFrameNoFlip(c->frame);
        else
            TaskSetFrameFlip(c->frame);
    }
    r = c->delay;
tail:
    a->animScriptPos++;
    return r;
}

s32 GrandWheelieTickAnimFacingNearestPlayer(s32 a)
{
    if (gCurTask->u8C.actor->animScript != 0) {
        if (a <= 0) {
            TaskFaceNearestPlayer();
            a = GrandWheelieStepAnim();
        }
        a--;
    }
    return a;
}

s32 GrandWheelieTickAnim(s32 a)
{
    if (gCurTask->u8C.actor->animScript != 0) {
        if (a <= 0)
            a = GrandWheelieStepAnim();
        a--;
    }
    return a;
}
