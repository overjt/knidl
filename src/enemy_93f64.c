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
extern void sub_080639f0(void *p);
extern void sub_08063a00(void *p);
extern void AngleToVector(s16 t, s16 mag);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 sub_0806acf8(void);

void sub_08093f64(void)
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
    u->unk28 = gUnk_087441BC[u->variant] * (s8)u->facing + 384;
    u->unk2C = zero;
    u->unk30 = 384;
    u->unk34 = 2;
    u->variant = zero;
    CallTableEntry(gCurTask->variant, 1, gUnk_087441CC);
}

void sub_08093fe0(void)
{
    gCurTask->updateCallback = (u32)sub_08094010;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087441D0);
}

void sub_08094010(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087441D4);
    if (gCurTask->unk2C == 1)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_08094040(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gCurTask->updateState = 0;
    TaskSetFrame(4);
    PlaySfx(506);
    sub_08094164();
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do {
        sub_08094164();
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk30 -= 48;
        t->unk6C++;
    } while ((s16)t->unk6C <= 7);
    t->velX = 0;
    t->velY = 0;
    t->unk2C = 1;
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
    gCurTask->unk6C = 0;
    do {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    v = gCurTask;
    v->velY = 0;
    TaskSetMotionXFacing(gUnk_087441C4[v->unk74], 0x5A5A5A5A);
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

void sub_08094144(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34--;
    if (t->unk34 == 0) {
        t->unk34 = 2;
        t->frame ^= 1;
    }
}

void sub_08094164(void)
{
    struct Task *t;

    t = gCurTask;
    AngleToVector(t->unk28, t->unk30);
    if (gCurTask->unk74 != 0)
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
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    gCurTask->frameTable = gGrandWheelieFrames;
    sub_08066088(0);
    gUnk_02007D00[8]++;
    gCurTask->unk34 = -1;
    TaskFaceNearestPlayer();
    ActorCollideTerrain();
    sub_080666cc(gUnk_0874433C);
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gGrandWheelieVariants);
}

void GrandWheelieInit(void)
{
    struct Task *t;
    struct Task *u;

    if (sub_08067060() != 0) {
        t = gCurTask;
        t->updateCallback = (u32)sub_080942b4;
        t->onGround = 0;
        ActorSetState(0);
        CallTableEntry(gCurTask->state, 11, gGrandWheelieStates);
    } else {
        u = gCurTask;
        u->updateCallback = (u32)GrandWheelieUpdate;
        u->onGround = 1;
        ActorSetState(1);
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
    sub_08068f68();
    ActorReactToHit();
}

void GrandWheelieUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk20 > 0) {
        t->unk20--;
        ActorFlashPalette(gUnk_0826F170, 16);
    } else {
        sub_08066468();
    }
    if (sub_0806acf8() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 11, gGrandWheelieStateUpdates);
    } else {
        CallTableEntry(gCurTask->updateState, 11, gGrandWheelieStateUpdates);
    }
    sub_08094358();
    sub_08068f68();
    ActorReactToHit();
}

void sub_08094358(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gCurTask;
    if (t->state == 10) {
        if (t->unk2C == 0 && t->velY < 0) {
            v = t->frame;
            if (v >= 4 && v <= 9) {
                ActorSetAttackBox(gUnk_08744A20);
                sub_08063a00(gUnk_08744A04);
            } else if (v >= 10 && v <= 19) {
                ActorSetAttackBox(gUnk_08744A74);
                sub_08063a00(gUnk_08744A58);
            } else if (v >= 20 && v <= 27) {
                ActorSetAttackBox(gUnk_08744AC8);
                sub_08063a00(gUnk_08744AAC);
            } else if (v >= 28 && v <= 31) {
                ActorSetAttackBox(gUnk_08744B54);
                sub_08063a00(gUnk_08744B38);
            } else if (v >= 32 && v <= 35) {
                ActorSetAttackBox(gUnk_08744BA8);
                sub_08063a00(gUnk_08744B8C);
            } else if (v >= 36 && v <= 47) {
                ActorSetAttackBox(gUnk_08744C34);
                sub_08063a00(gUnk_08744C18);
            } else {
                ActorSetAttackBox(gUnk_08744C88);
                sub_08063a00(gUnk_08744C6C);
            }
        } else {
            u = gCurTask;
            v = u->frame;
            if (v >= 20 && v <= 27) {
                ActorSetAttackBox(gUnk_08744AE4);
                sub_08063a00(gUnk_08744B00);
            } else if (v >= 32 && v <= 35) {
                ActorSetAttackBox(gUnk_08744BC4);
                sub_08063a00(gUnk_08744BE0);
            } else {
                ActorSetAttackBox(gUnk_08744CA4);
                sub_08063a00(gUnk_08744CC0);
            }
        }
    } else {
        v = t->frame;
        if (v >= 4 && v <= 9) {
            ActorSetAttackBox(gUnk_087449E8);
            sub_08063a00(gUnk_08744A04);
            sub_080639f0(gUnk_0874531C);
        } else if (v >= 10 && v <= 19) {
            ActorSetAttackBox(gUnk_08744A3C);
            sub_08063a00(gUnk_08744A58);
            sub_080639f0(gUnk_08745324);
        } else if (v >= 20 && v <= 27) {
            ActorSetAttackBox(gUnk_08744A90);
            sub_08063a00(gUnk_08744AAC);
            sub_080639f0(gUnk_0874532C);
        } else if (v >= 28 && v <= 31) {
            ActorSetAttackBox(gUnk_08744B1C);
            sub_08063a00(gUnk_08744B38);
            sub_080639f0(gUnk_08745334);
        } else if (v >= 32 && v <= 35) {
            ActorSetAttackBox(gUnk_08744B70);
            sub_08063a00(gUnk_08744B8C);
            sub_080639f0(gUnk_0874533C);
        } else if (v >= 36 && v <= 47) {
            ActorSetAttackBox(gUnk_08744BFC);
            sub_08063a00(gUnk_08744C18);
            sub_080639f0(gUnk_08745344);
        } else {
            ActorSetAttackBox(gUnk_08744C50);
            sub_08063a00(gUnk_08744C6C);
            sub_080639f0(gUnk_0874534C);
        }
    }
}

void sub_080945fc(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 0;
    v = sub_080956c8(gUnk_0874433C);
    t = gCurTask;
    t->unk28 = v;
    t->accelY = 9472;
    t->speedLimitY = 0x30000;
    TaskYieldTrampoline(24);
    gCurTask->updateCallback = (u32)GrandWheelieUpdate;
    TaskSleepForever();
}

void sub_08094640(void)
{
    gCurTask->unk28 = sub_08095794(gCurTask->unk28);
}

void sub_0809465c(void)
{
    gCurTask->updateState = 1;
    sub_08066580();
    ActorSetState(2);
    TaskSleepForever();
}

void sub_0809467c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 1) {
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    } else {
        gCurTask->unk28 = sub_08095794(t->unk28);
    }
}

void sub_080946b0(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    if (sub_08094810() != 0) {
        ActorSetState(8);
        TaskSleepForever();
    }
    t = gCurTask;
    t->unk2C = t->pixelX;
    switch (sub_080947cc()) {
    case 0:
        gCurTask->unk30 = gUnk_030023D4;
        gCurTask->unk28 = sub_080956c8(gUnk_0874433C);
        TaskSleepForever();
        break;
    case 1:
        gCurTask->unk30 = gUnk_030023D4;
        gCurTask->unk28 = sub_080956c8(gUnk_087443A8);
        while (1) {
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08094758(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 x;

    t = gCurTask;
    if (t->state != 2)
        goto rearm;
    x = sub_08095794(t->unk28);
    u = gCurTask;
    u->unk28 = x;
    u->unk30--;
    if (u->unk30 != 0)
        return;
    TaskStopX();
    v = gCurTask;
    v->posX = v->unk2C << 16;
    if (RandomRange(4) == 0)
        goto quiet;
    ActorSetState(4);
rearm:
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    return;
quiet:
    ActorSetState(3);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 sub_080947cc(void)
{
    TaskFaceNearestPlayer();
    gUnk_030023D4 = gUnk_0874449C[gCurTask->unk74 * 2 + (gFrameCount & 1)];
    return gFrameCount & 1;
}

s32 sub_08094810(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk34 = (t->unk34 + 1) & 3;
    if (t->unk34 == 3) {
        if (t->unk18 > 3)
            t->unk18 = 0;
        u = gCurTask;
        if (u->unk18 == 0)
            return 1;
        u->unk18++;
    }
    return 0;
}

void sub_08094844(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    if (sub_08094810() != 0) {
        ActorSetState(8);
        TaskSleepForever();
    }
    TaskFaceNearestPlayer();
    gCurTask->unk28 = sub_080956c8(gUnk_08744360);
    sub_08094908();
    gCurTask->unk2C = gUnk_030023D4;
    sub_08094894();
}

void sub_08094894(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 0) {
        t->unk2C--;
        t->onGround = 0;
        TaskSetMotionY(-0x50000, 0x4A00, 0x70000);
        TaskSleepForever();
    }
    ActorSetState(4);
    TaskSleepForever();
}

void sub_080948d4(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->state != 3)
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

    gCurTask->updateState = 4;
    TaskStop();
    if (sub_08094810() != 0) {
        ActorSetState(8);
        TaskSleepForever();
    }
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = t->facing;
    gUnk_02007D00[1] = PlaySfx(0x209);
    if (gCurTask->unk8C->animScript != gUnk_08744384)
        gCurTask->unk28 = sub_080956c8(gUnk_08744384);
    TaskYieldTrampoline(45);
    gCurTask->unk2C = sub_08094b94();
    gCurTask->unk46 = sub_0806e6f8(-10, 5);
    TaskSetMotionXFacing(gUnk_087444A4[gCurTask->unk74], 0x5A5A5A5A);
    gCurTask->unk1C = -1;
    TaskSleepForever();
}

void sub_080949e0(void)
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

    x = sub_08095794(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->state != 4)
        goto rearm;
    if (t->velX == 0)
        return;
    sub_08094bbc();
    if (gCurTask->facing != TaskGetFacingTowardNearestPlayer())
        goto other;
    if (gCurTask->unk2C != 0)
        return;
    y = sub_08094b94();
    gCurTask->unk2C = y;
    if (y == 0)
        return;
    if (RandomRange(3) != 0)
        return;
    v = gCurTask;
    if (v->unk18 != 0)
        return;
    v->unk18 = 1;
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    if (gTaskSlotTypes[gCurTask->unk46] == 167)
        TaskFree(gCurTask->unk46);
    ActorSetState(7);
rearm:
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    return;
other:
    w = gCurTask;
    if (w->unk2C < 0) {
        a = w->pixelX;
        p = &w->pixelY;
        switch (GetCollisionTileAtPixel(a, ((s8 *)w->unk8C->terrainBox)[3] + *p)) {
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
        if (gTaskSlotTypes[gCurTask->unk46] == 167)
            TaskFree(gCurTask->unk46);
        ActorSetState(5);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    }
    z = sub_08094b94();
    gCurTask->unk2C = z;
    if (z == 1)
        return;
    gCurTask->unk2C = -1;
    q = RandomRange(3);
    if (q != 0)
        return;
    gCurTask->unk18 = q;
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    if (gTaskSlotTypes[gCurTask->unk46] == 167)
        TaskFree(gCurTask->unk46);
    ActorSetState(6);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 sub_08094b94(void)
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
    if (t->unk1C == (s8)r)
        return;
    gCurTask->unk1C = (s8)v;
    switch ((s8)v) {
    case 0:
        if (gCurTask->unk8C->animScript != gUnk_08744360)
            gCurTask->unk28 = sub_080956c8(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444A4[gCurTask->unk74], 0x5A5A5A5A);
        break;
    case 1:
        if (gCurTask->unk8C->animScript != gUnk_08744360)
            gCurTask->unk28 = sub_080956c8(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444AC[gCurTask->unk74], 0x5A5A5A5A);
        break;
    case 2:
        if (gCurTask->unk8C->animScript != gUnk_08744360)
            gCurTask->unk28 = sub_080956c8(gUnk_08744360);
        TaskSetMotionXFacing(gUnk_087444B4[gCurTask->unk74], 0x5A5A5A5A);
        break;
    case 3:
        if (gCurTask->unk8C->animScript != gUnk_0874433C)
            gCurTask->unk28 = sub_080956c8(gUnk_0874433C);
        TaskSetMotionXFacing(gUnk_087444BC[gCurTask->unk74], 0x5A5A5A5A);
        break;
    case 4:
        if (gCurTask->unk8C->animScript != gUnk_08744384)
            gCurTask->unk28 = sub_080956c8(gUnk_08744384);
        TaskSetMotionXFacing(gUnk_087444C4[gCurTask->unk74], 0x5A5A5A5A);
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

void sub_08094da4(void)
{
    struct Task *t;

    gCurTask->updateState = 5;
    t = gCurTask;
    t->unk30 = 60;
    t->unk2C = t->facing;
    t->unk1C = 0;
    gCurTask->unk28 = sub_080956c8(gUnk_087443D0);
    PlaySfx(0x20A);
    TaskSleepForever();
}

void sub_08094dec(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = sub_08095794(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->unk8C->animScript == 0) {
        TaskTurnAround();
        gCurTask->unk28 = sub_080956c8(gUnk_0874433C);
    }
    if (sub_08094e88() != 0) {
        gCurTask->velX = 0;
        ActorSetState(2);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return;
    }
    u = gCurTask;
    u->unk30--;
    if (u->unk30 >= 0)
        return;
    if ((gFrameCount & 1) != 0) {
        ActorSetState(4);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return;
    }
    ActorSetState(2);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 sub_08094e88(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    if (gCurTask->unk2C == 1) {
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
    v->unk1C++;
    if (v->unk1C == 8) {
        if (v->unk2C == v->facing)
            CreateDustTrail(1, 1, -4, 12);
        else
            CreateDustTrail(1, 1, -4, 12);
        gCurTask->unk1C = 0;
    }
    return 0;
}

void sub_08094f28(void)
{
    struct Task *t;
    s32 zero;
    s32 v;

    gCurTask->updateState = 6;
    zero = 0;
    v = sub_080956c8(gUnk_087443BC);
    t = gCurTask;
    t->unk28 = v;
    t->unk2C = t->facing;
    t->unk1C = zero;
    PlaySfx(0x20A);
    TaskSleepForever();
}

void sub_08094f68(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk20 > 0)
        return;
    gCurTask->unk28 = sub_08095794(t->unk28);
    if (sub_08094e88() != 0) {
        gCurTask->velX = 0;
        ActorSetState(2);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
    }
}

void sub_08094fb0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;
    s32 one;
    s32 v;

    gCurTask->updateState = 7;
    zero = 0;
    gCurTask->unk18 = one = 1;
    v = sub_080956c8(gUnk_087443D0);
    t = gCurTask;
    t->unk28 = v;
    t->unk2C = t->facing;
    t->unk30 = zero;
    t->unk1C = zero;
    PlaySfx(0x20A);
    TaskYieldTrampoline(30);
    gCurTask->unk28 = sub_080956c8(gUnk_08744384);
    gCurTask->unk46 = sub_0806e6f8(-10, 5);
    TaskSetMotionXFacing(gUnk_087444A4[0], 0x5A5A5A5A);
    gCurTask->unk30 = one;
    gUnk_02007D00[1] = PlaySfx(0x209);
    TaskYieldTrampoline(30);
    if (gTaskSlotTypes[gCurTask->unk46] == 167)
        TaskFree(gCurTask->unk46);
    gCurTask->unk28 = sub_080956c8(gUnk_087443D0);
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    u = gCurTask;
    u->unk2C = u->facing;
    u->unk30 = zero;
    u->unk1C = zero;
    PlaySfx(0x20A);
    TaskYieldTrampoline(30);
    gCurTask->unk30 = 2;
    TaskSleepForever();
}

void sub_080950b4(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    v = t->unk30;
    switch (v) {
    case 0:
        gCurTask->unk28 = sub_08095794(t->unk28);
        if (gCurTask->unk8C->animScript == 0) {
            TaskTurnAround();
            gCurTask->unk28 = sub_080956c8(gUnk_0874433C);
        }
        if (sub_08094e88() != 0)
            gCurTask->velX = v;
        break;
    case 1:
        gCurTask->unk28 = sub_08095794(t->unk28);
        sub_08094bbc();
        break;
    case 2:
        ActorSetState(4);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        break;
    }
}

void sub_0809513c(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;
    s32 zero;

    gCurTask->updateState = 8;
    zero = 0;
    TaskStop();
    sub_080956c8(gUnk_087443E4);
    TaskSetMotionY(-0x30000, 0x3500, 0x30000);
    gCurTask->onGround = zero;
    gCurTask->unk2C = zero;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk2C == 0);
    TaskSetMotionY(-0x18000, 0x3500, 0x30000);
    gCurTask->onGround = 0;
    gCurTask->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk2C == 0);
    TaskYieldTrampoline(16);
    PlaySfx(506);
    t = gCurTask;
    a = t->unk8C;
    sp.subtype = 12;
    sp.taskType = 114;
    sp.variant = t->variant;
    sp.spawnArg = t->unk74;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 1;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
    TaskYieldTrampoline(20);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_08095220(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->state != 8)
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

void sub_08095254(void)
{
    struct Task *t;

    gCurTask->updateState = 9;
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
    sub_080956c8(gUnk_08744408);
    gCurTask->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk2C == 0);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x2500, 0x30000);
    gCurTask->onGround = 0;
    sub_080956c8(gUnk_0874441C);
    gCurTask->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk2C == 0);
    TaskYieldTrampoline(30);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_0809532c(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->state != 9)
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

void sub_08095360(void)
{
    struct Task *t;
    s32 z1;
    s32 z2;

    gCurTask->updateState = 10;
    z1 = 0;
    if (gTaskSlotTypes[gCurTask->unk46] == 167)
        TaskFree(gCurTask->unk46);
    ActorSetHitReactions(gUnk_0874541C);
    gCurTask->unk2C = z1;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 32);
    CreateChildTaskHere(142, 0);
    gCurTask->unk28 = sub_080956c8(gUnk_08744408);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x1A00, 0x30000);
    gCurTask->onGround = z1;
    while (gCurTask->unk2C == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateChildTaskHere(141, 0);
    sub_080956c8(gUnk_0874441C);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    RequestScreenShake(2);
    gCurTask->unk1C = z2 = 0;
    TaskYieldTrampoline(32);
    TaskStop();
    t = gCurTask;
    t->unk2C = 2;
    t->unk30 = z2;
    CreateChildTaskHere(141, 0);
    TaskYieldTrampoline(170);
    sub_0806ad18();
    gCurTask->unk30 = 1;
    TaskSleepForever();
}

void sub_08095484(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk28 = sub_08095794(gCurTask->unk28);
    ActorFlashPalette(gUnk_0826F170, 16);
    t = gCurTask;
    if (t->unk2C == 1) {
        t->unk1C++;
        if (t->unk1C == 16) {
            CreateDustTrail(0, 1, 8, 10);
            gCurTask->unk1C = 0;
        }
    }
    u = gCurTask;
    if (u->unk2C == 2 && u->unk30 != 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

s32 sub_080954f0(void)
{
    switch (gCurTask->state) {
    case 0:
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(504);
        ActorSetState(1);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case 3:
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(0x1F7);
        if (gCurTask->unk8C->animScript != gUnk_08744384)
            gCurTask->unk28 = sub_080956c8(gUnk_08744384);
        TaskSetEntry(sub_08094894, gCurTaskIdx);
        return 1;
    case 8:
    case 9:
    case 10:
        TaskStop();
        if (gCurTask->state != 10)
            PlaySfx(504);
        gCurTask->unk2C = 1;
        break;
    }
    return 0;
}

s32 sub_080955a8(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->state) {
    case 4:
        StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
        if (gTaskSlotTypes[gCurTask->unk46] == 167)
            TaskFree(gCurTask->unk46);
        ActorSetState(9);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case 5:
    case 6:
    case 10:
        TaskStopX();
        break;
    case 7:
        if (gCurTask->unk30 == 0) {
            TaskStopX();
            break;
        }
        TaskStop();
        StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
        if (gTaskSlotTypes[gCurTask->unk46] == 167)
            TaskFree(gCurTask->unk46);
        ActorSetState(9);
        TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
        return 1;
    case 8:
    case 9:
        break;
    }
    return 0;
}

void sub_08095674(void)
{
    CreateChildTaskHere(142, 0);
    RequestScreenShake(2);
    gCurTask->unk20 = 32;
}

void sub_08095694(void)
{
    ActorFaceHitter();
    StopSfxOnPlayer(gUnk_02007D00[1], 0x209);
    ActorSetState(10);
    TaskSetEntry(GrandWheelieEnterState, gCurTaskIdx);
}

s32 sub_080956c8(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    a->animScript = p;
    a->animScriptPos = 0;
    return sub_080956e4();
}

s32 sub_080956e4(void)
{
    struct Actor *a;
    struct AnimCmd *c;
    s32 r;

    a = gCurTask->unk8C;
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

s32 sub_08095768(s32 a)
{
    if (gCurTask->unk8C->animScript != 0) {
        if (a <= 0) {
            TaskFaceNearestPlayer();
            a = sub_080956e4();
        }
        a--;
    }
    return a;
}

s32 sub_08095794(s32 a)
{
    if (gCurTask->unk8C->animScript != 0) {
        if (a <= 0)
            a = sub_080956e4();
        a--;
    }
    return a;
}
