#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern void sub_08065438(void);
extern void ActorMove(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(void *src, void *dst, s32 ratio, s32 count, void *out);
extern s32 PlaySfx(s32 id);
extern void TaskFree(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetFrameByFacing(s16 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void ActorSetTerrainBox(void *p);
extern void sub_080639f0(void *p);
extern void sub_08063a00(void *p);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskFaceToward(u32 i);
extern s32 TaskGetFacingTowardNearestPlayer(void);
extern void TaskFaceNearestPlayer(void);
extern void ActorStopAnim(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnim(s32 n);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s16 ActorComputeHealth(void);
extern u16 sub_08066088(u32 mode);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern void sub_08068920(s32 i, u8 c);
extern void sub_080689c8(s32 i, s32 d);
extern u32 ActorCheckHitsWithBox(void *p);
extern u32 sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern void ActorFaceHitter(void);
extern void ActorDie(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d);
extern void TaskYieldTrampoline(u32 frames);

/* Defined below */
void Task_FireLion(void);
void sub_08095834(void);
void sub_08095940(void);
void sub_0809595c(void);
void sub_080959e8(void);
void sub_080959ec(void);
void sub_08095a54(void);
void sub_08095ad0(void);
void sub_08095aec(void);
void sub_08095be4(void);
void sub_08095be8(void);
void sub_08095d20(void);
void sub_08095d40(void);
void sub_08095e4c(void);
void sub_08095eac(void);
void sub_08096058(void);
void sub_080960bc(void);
void sub_0809616c(void);
void sub_0809619c(void);
void sub_08096278(void);
void sub_080962ac(void);
void sub_080962d0(void);
void sub_08096320(void);
void sub_080963c0(void);
void sub_080963dc(void);
void sub_08096640(void);
void sub_08096680(void);
void sub_0809680c(void);
void sub_08096888(void);
void sub_080968c0(void);
void sub_08096920(void);
void sub_08096924(void);
void sub_0809699c(void);
void sub_080969c8(void);
void sub_08096a28(void);
void sub_08096a40(void);
void sub_08096b7c(void);
void sub_08096d20(void);
s32 sub_08096d64(void);
s32 sub_08096df4(void);
s32 sub_08096e0c(void);
s32 sub_08096e24(void);
void sub_08096e70(void);
void sub_08096e9c(void);
void sub_08096fc0(void);
void sub_08097024(void);
void sub_08097088(void);

void Task_FireLion(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    gCurTask->frameTable = gFireLionFrames;
    gUnk_02007D00[8]++;
    sub_08095834();
    ActorCollideTerrain();
    sub_080666cc(gUnk_08744510);
    if (sub_08067060() != 0) {
        gCurTask->updateCallback = (u32)sub_0809699c;
        sub_0809595c();
    } else {
        gCurTask->updateCallback = (u32)sub_080969c8;
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

    sub_08066088(1);
    gCurTask->speedLimitY = 0x30000;
    TaskFaceNearestPlayer();
    gCurTask->unk46 = CreateChildTaskHere(214, 0);
    gTasks[gCurTask->unk46].variant = 0;
    b = gCurTask;
    b->unk24 = b->unk46;
    gCurTask->unk46 = CreateChildTaskHere(214, 0);
    gTasks[gCurTask->unk46].variant = 1;
    d = gCurTask;
    d->unk24 += d->unk46 << 8;
    gCurTask->unk46 = CreateChildTaskHere(214, 0);
    gTasks[gCurTask->unk46].variant = 2;
    f = gCurTask;
    f->unk24 += f->unk46 << 8;
    gCurTask->unk46 = CreateChildTaskHere(214, 0);
    gTasks[gCurTask->unk46].variant = 3;
    h = gCurTask;
    h->unk28 = 0;
    h->unk2C = 0;
    h->unk30 = 0;
    h->unk34 = 0;
    h->unk18 = -1;
    h->unk1C = 0;
    h->unk20 = 0;
    h->unk24 = 0;
    h->unk6E = 0;
    h->unk70 = 0;
    sub_08066ae0();
    gUnk_02007D00[9] = ActorComputeHealth();
}

void sub_08095940(void)
{
    CallTableEntry(gCurTask->state, 12, gUnk_087444E4);
}

void sub_0809595c(void)
{
    struct Task *t;
    struct Task *u;

    ActorSetState(0);
    gCurTask->updateState = 0;
    ActorSetTerrainBox(gUnk_0874530C);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->accelY = 0x5000;
    t->speedLimitY = 0x30000;
    TaskSetFrame(15);
    TaskYieldTrampoline(24);
    u = gCurTask;
    u->updateCallback = (u32)sub_080969c8;
    if (u->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk34 == 0);
    }
    ActorSetTerrainBox(gUnk_08745304);
    TaskSetFrame(16);
    TaskYieldTrampoline(24);
    ActorSetState(1);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_080959e8(void)
{
}

void sub_080959ec(void)
{
    ActorSetState(1);
    gCurTask->updateState = 1;
    ActorSetTerrainBox(gUnk_08745304);
    gCurTask->unk28 = ActorStartAnim(gUnk_08744510);
    sub_08066580();
    TaskYieldTrampoline(gUnk_08744524[gCurTask->unk74]);
    gCurTask->unk6E = RandomRange(8);
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08095a54(void)
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
    t->unk28 = x;
    gUnk_030023D4 = v = t->unk74 * 2;
    if (t->health < gUnk_02007D00[9] >> 1)
        gUnk_030023D4 = v + 1;
    TaskYieldTrampoline(gUnk_08744526[gUnk_030023D4]);
    sub_08096fc0();
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08095ad0(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_08095aec(void)
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
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < n) {
        TaskSetMotionY(-0x50000, 0x5000, 0x30000);
        sub_0809680c();
        RequestScreenShake(2);
        TaskFaceNearestPlayer();
        gCurTask->unk6C++;
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
    if (gCurTask->state == 3)
        ActorSetState(gUnk_08744608[RandomRange(8)]);
    else
        ActorSetState(8);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08095be4(void)
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
    t->unk2C = 0;
    t->unk6C = 0;
    do {
        u = gCurTask;
        if (u->unk2C != 0)
            u->facing = -u->facing;
        else
            TaskFaceNearestPlayer();
        sub_08095d40();
        v = gCurTask;
        v->unk6C++;
    } while ((s16)v->unk6C <= 1);
    if (gCurTask->unk74 == 1 && RandomRange(2) != 0) {
        TaskFaceNearestPlayer();
        gCurTask->onGround = 0;
        TaskSetMotionY(-0x30000, 0x2000, 0x30000);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        sub_0809680c();
        ActorSetTerrainBox(gUnk_08745304);
        sub_08096924();
        if (gCurTask->state == 5)
            ActorSetState(2);
        else
            ActorSetState(gUnk_08744608[RandomRange(8)]);
    } else {
        gCurTask->unk6C = 0;
        do {
            w = gCurTask;
            if (w->unk2C != 0)
                w->facing = -w->facing;
            else
                TaskFaceNearestPlayer();
            sub_08095d40();
            x = gCurTask;
            x->unk6C++;
        } while ((s16)x->unk6C <= 1);
        sub_08096924();
        ActorSetState(2);
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
    t->unk2C = zero;
    t->unk30 = 1;
    TaskStop();
    TaskSetFrame(8);
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(gUnk_0874452C[gCurTask->unk74], 0x5A5A5A5A);
    gCurTask->onGround = zero;
    gCurTask->velY = -0x5000;
    TaskSetFrame(9);
    TaskYieldTrampoline(5);
    gCurTask->velY = 0x5000;
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x5A5A5A5A, gUnk_08744534[gCurTask->unk74]);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    if (gCurTask->facing != TaskGetFacingTowardNearestPlayer() || gCurTask->unk2C != 0) {
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
    gCurTask->unk30 = 0;
}

void sub_08095e4c(void)
{
    TaskStop();
    ActorStopAnim();
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetFrame(6);
        TaskYieldTrampoline(5);
        TaskSetFrame(7);
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
}

void sub_08095eac(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 4;
    ActorSetTerrainBox(gUnk_08745304);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDx()) <= 47) {
        sub_08096888();
        gCurTask->updateState = 4;
    } else if (abs(TaskGetNearestPlayerDx()) > 71) {
        sub_08095e4c();
        gCurTask->unk6C = 0;
        while ((s16)gCurTask->unk6C <= 3) {
            if (abs(TaskGetNearestPlayerDx()) <= 71)
                break;
            TaskFaceNearestPlayer();
            sub_08095d40();
            t = gCurTask;
            if (t->unk2C != 0) {
                t->facing = -t->facing;
                sub_08095d40();
                break;
            }
            t->unk6C++;
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
    sub_0809680c();
    ActorSetTerrainBox(gUnk_08745304);
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
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
        u->unk6C++;
    } while ((s16)u->unk6C <= 2);
    sub_08096888();
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08096058(void)
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

void sub_080960bc(void)
{
    struct Task *t;
    s32 zero;

    TaskFaceNearestPlayer();
    ActorSetTerrainBox(gUnk_08745304);
    gCurTask->unk1C = 1;
    if (abs(TaskGetNearestPlayerDx()) <= 31)
        sub_08096888();
    TaskStop();
    t = gCurTask;
    zero = 0;
    t->updateState = 8;
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    TaskSetFrame(18);
    TaskYieldTrampoline(8);
    TaskSetMotionY(-0x40000, 0x2200, 0x30000);
    sub_0809680c();
    ActorSetTerrainBox(gUnk_08745304);
    TaskStop();
    gCurTask->unk1C = zero;
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    TaskSetFrame(45);
    TaskYieldTrampoline(2);
    sub_0809619c();
}

void sub_0809616c(void)
{
    if (TaskGetNearestPlayerDy() > -24) {
        gCurTask->unk1C = 0;
        TaskSetEntry(sub_080962d0, gCurTaskIdx);
    }
}

void sub_0809619c(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 6;
    PlaySfx(500);
    gCurTask->unk2C = 0;
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    gCurTask->unk28 = ActorStartAnim(gUnk_0874453C);
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
    sub_0809680c();
    sub_08096924();
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08096278(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->unk2C != 0)
        TaskSetEntry(sub_08096320, gCurTaskIdx);
}

void sub_080962ac(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 0 || t->velX * t->accelX >= 0)
        TaskStopX();
}

void sub_080962d0(void)
{
    gCurTask->updateState = 9;
    gCurTask->unk2C = 0;
    TaskStop();
    TaskSetMotionXFacing(-0x20000, 0x2000);
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    TaskSetFrame(45);
    TaskYieldTrampoline(2);
    TaskYieldTrampoline(12);
    TaskStop();
    sub_0809619c();
}

void sub_08096320(void)
{
    s32 zero;

    gCurTask->updateState = 9;
    zero = 0;
    gCurTask->unk2C = zero;
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
    sub_0809680c();
    ActorSetTerrainBox(gUnk_08745304);
    sub_08096924();
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_080963c0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 0 && t->velY < 0)
        t->velY = 0;
}

void sub_080963dc(void)
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
        gCurTask->unk6C = 0;
        do {
            sub_08095d40();
            t = gCurTask;
            if (t->unk2C != 0)
                goto flip;
            TaskFaceNearestPlayer();
            if (abs(TaskGetNearestPlayerDx()) <= 79)
                goto rest;
            t = gCurTask;
            t->unk6C++;
        } while ((s16)t->unk6C <= 3);
    }
rest:
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk30 = 1;
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
    v->unk2C = 0;
    v->unk34 = 0;
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    TaskSetMotionY(-0x38000, 0x2000, 0x30000);
    TaskSetFrame(36);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    gCurTask->unk30 = 0;
    TaskStop();
    TaskSetFrame(37);
    TaskYieldTrampoline(8);
    TaskSetFrame(38);
    TaskYieldTrampoline(8);
    w = gCurTask;
    w->velY = 0x60000;
    w->accelY = 0x100;
    if (w->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk34 == 0);
    }
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    sub_08097088();
    gCurTask->unk28 = ActorStartAnim(gUnk_08744550);
    sub_08063a00(gUnk_08745040);
    TaskYieldTrampoline(2);
    sub_08063a00(0);
    TaskYieldTrampoline(gUnk_08744562[gCurTask->unk74]);
    gCurTask->unk6C = 0;
    do {
        gCurTask->onGround = 0;
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(1);
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(1);
        x = gCurTask;
        x->unk6C++;
    } while ((s16)x->unk6C <= 7);
    ActorStopAnim();
    TaskStop();
    TaskSetFrame(38);
    TaskYieldTrampoline(4);
    TaskSetFrame(36);
    TaskYieldTrampoline(8);
    sub_08096924();
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_08096640(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = ActorTickAnim(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->unk2C != 0 && t->velY < 0)
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
    t->unk30 = zero;
    PlaySfx(0x236);
    TaskSetFrame(34);
    u = gCurTask;
    if (u->onGround == 0) {
        u->unk34 = zero;
        u->unk6C = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gCurTask;
            v->unk6C++;
        } while ((s16)v->unk6C <= 3);
        TaskStopX();
        w = gCurTask;
        w->velY = 0x48000;
        if (w->unk34 == 0) {
            do {
                TaskYieldTrampoline(1);
            } while (gCurTask->unk34 == 0);
        }
    } else {
        u->unk6C = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            x = gCurTask;
            x->unk6C++;
        } while ((s16)x->unk6C <= 3);
        TaskStopX();
    }
    gCurTask->unk6C = 0;
    do {
        if (gLocalPlayer == gCurTask->unk18)
            PlaySfx(0x237);
        CreateChildTaskAtOffsetFacing(141, 24, 0, 0);
        TaskSetFrame(30);
        TaskYieldTrampoline(4);
        TaskSetFrame(31);
        TaskYieldTrampoline(4);
        TaskSetFrame(32);
        TaskYieldTrampoline(4);
        TaskSetFrame(33);
        TaskYieldTrampoline(4);
        y = gCurTask;
        y->unk6C++;
    } while ((s16)y->unk6C <= 5);
    sub_08068920(gCurTask->unk18, 6);
    gCurTask->unk18 = -1;
    sub_08096888();
    gCurTask->unk30 = 0;
    ActorSetState(2);
    gCurTask->updateState = 12;
    TaskSleepForever();
}

void sub_0809680c(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->onGround = 0;
    t = gCurTask;
    t->unk2C = 0;
    t->unk34 = 0;
    ActorStopAnim();
    ActorSetTerrainBox(gUnk_0874530C);
    TaskSetFrame(13);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(14);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    u = gCurTask;
    if (u->velY > 0 && u->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk34 == 0);
    }
    TaskStop();
}

void sub_08096888(void)
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
    t->unk2C = 0;
    t->unk34 = 0;
    ActorStopAnim();
    TaskSetFrame(23);
    TaskYieldTrampoline(8);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    TaskSetFrame(25);
    while (gCurTask->unk34 == 0)
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
            sub_08096888();
        }
    } else {
        if (gCurTask->pixelX - gViewRect[0] <= 159) {
            TaskFaceNearestPlayer();
            TaskSetFrame(17);
            TaskYieldTrampoline(8);
            sub_08096888();
        }
    }
}

void sub_0809699c(void)
{
    sub_08096e9c();
    CallTableEntry(gCurTask->updateState, 13, gUnk_08744564);
    sub_08097024();
    sub_08068f68();
    ActorReactToHit();
}

void sub_080969c8(void)
{
    sub_08096e9c();
    sub_08096a40();
    if (sub_0806acf8() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 13, gUnk_08744564);
    } else {
        CallTableEntry(gCurTask->updateState, 13, gUnk_08744564);
    }
    sub_08097024();
    sub_08068f68();
    ActorReactToHit();
}

void sub_08096a28(void)
{
    TaskSetEntry(sub_08095940, gCurTaskIdx);
}

void sub_08096a40(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;
    struct PlayerState *p;

    t = gCurTask;
    if (t->unk30 == 0)
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
    if (u->hitKind == 8) {
        p = gPlayerStates;
        i = u->hitterSlot;
        if (p[i].ability != 17) {
            u->unk18 = i;
            TaskFaceToward(i);
            sub_080685ec(gCurTask->unk18, gCurTaskIdx, 5);
            TaskSetEntry(sub_08096680, gCurTaskIdx);
        }
    }
}

void sub_08096b7c(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = 11;
    zero = 0;
    ActorStopAnim();
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 21);
    TaskStop();
    t = gCurTask;
    t->unk34 = zero;
    t->unk30 = zero;
    t->unk1C = 1;
    CreateChildTaskHere(142, 0);
    ActorSetHitReactions(gUnk_08745434);
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x1A00, 0x30000);
    TaskSetFrame(21);
    ActorSetAttackBox(gUnk_08744F0C);
    sub_08063a00(gUnk_087446E8[gCurTask->frame]);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    ActorSetTerrainBox(gUnk_08745304);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(38);
    ActorSetAttackBox(gUnk_087450CC);
    sub_08063a00(gUnk_087450E8);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk28 = ActorStartAnim(gUnk_08744550);
    TaskSetFrameByFacing(40);
    ActorSetAttackBox(gUnk_08745104);
    sub_08063a00(gUnk_08745120);
    sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateChildTaskHere(141, 0);
    gCurTask->unk20 = 0;
    TaskSetMotionXFacing(-0x10000, 0x600);
    TaskYieldTrampoline(30);
    TaskStop();
    TaskYieldTrampoline(170);
    sub_0806ad18();
    ActorDie();
}

void sub_08096d20(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->unk34 == 1 && t->velX != 0) {
        t->unk20++;
        if (t->unk20 == 16) {
            CreateDustTrail(0, 1, 8, 10);
            gCurTask->unk20 = 0;
        }
    }
}

s32 sub_08096d64(void)
{
    switch (gCurTask->state) {
    case 0:
    case 3:
    case 4:
        TaskStopY();
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        gCurTask->unk34 = 1;
        return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        TaskStopY();
        gCurTask->unk34 = 1;
        return 0;
    case 10:
        TaskStop();
        gCurTask->unk34 = 1;
        return 0;
    }
}

s32 sub_08096df4(void)
{
    gCurTask->unk2C = 1;
    TaskStopX();
    return 0;
}

s32 sub_08096e0c(void)
{
    CreateChildTaskHere(142, 0);
    RequestScreenShake(2);
    return 0;
}

s32 sub_08096e24(void)
{
    struct Task *t;

    ActorFaceHitter();
    t = gCurTask;
    if (t->unk18 >= 0) {
        sub_080689c8(t->unk18, -t->facing);
        gCurTask->unk18 = -1;
    }
    ActorSetState(10);
    TaskSetEntry(sub_08095940, gCurTaskIdx);
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

void sub_08096e9c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s16 *q;

    a = gCurTask->unk8C;
    if ((a->paletteLocked & 1) == 0) {
    a->paletteOverridden = 1;
    t = gCurTask;
    if ((t->hitTimer != 0 && t->unk8C->hitState != 0) || t->unk1C != 0) {
        u = gCurTask;
        q = (s16 *)&u->unk70;
        if (*q <= 3) {
            BlendColors(gUnk_08744598[u->unk8C->paletteVariant][0],
                         gUnk_08744598[u->unk8C->paletteVariant][1],
                         gUnk_087445D8[*q], 16,
                         &gObjPalette[(u->tileWord >> 12) * 32]);
        } else {
            BlendColors(gUnk_08744598[u->unk8C->paletteVariant][0],
                         gUnk_082B07BC,
                         gUnk_087445D8[*q], 16,
                         &gObjPalette[(u->tileWord >> 12) * 32]);
        }
    } else {
        BlendColors(gUnk_08744598[t->unk8C->paletteVariant][0],
                     gUnk_08744598[t->unk8C->paletteVariant][1],
                     gUnk_087445D8[(s16)t->unk70], 16,
                     &gObjPalette[(t->tileWord >> 12) * 32]);
    }
    v = gCurTask;
    v->unk70++;
    if ((s16)v->unk70 > 7)
        v->unk70 = 0;
    }
}

void sub_08096fc0(void)
{
    struct Task *t;
    u16 v;

    t = gCurTask;
    t->unk6E++;
    if (t->unk6E > 7)
        t->unk6E = 0;
    v = gUnk_087445E8[gCurTask->unk6E + gCurTask->unk74 * 8];
    if (gUnk_087445E8[gCurTask->unk6E + gCurTask->unk74 * 8] == 11)
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
            sub_08063a00(gUnk_087446E8[u->frame]);
        sub_080639f0(gUnk_087447B8[gCurTask->frame]);
    }
}

void sub_08097088(void)
{
    struct Task *t;
    s32 d;
    u16 x;
    u16 y;

    d = TaskGetFacingTowardNearestPlayer();
    t = gCurTask;
    x = t->pixelX + d * 24;
    y = t->pixelY + 3;
    sub_08067120(x, y, d, 3);
}
