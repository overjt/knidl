
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gViewRect[];
extern u32 gUnk_08747AA4[];
extern u32 gUnk_08747AB4[];
extern u32 gUnk_08747AC8[];
extern u32 gUnk_08747D5C[];
extern u32 gUnk_08747EF4[];
extern u32 gUnk_08753204[];
extern u32 gUnk_08753510[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void TaskStop(void);
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern void sub_0809cb90(void);
extern void sub_0809cf04(void);
extern void sub_0809d13c(void);
extern void sub_0809dc7c(void);
extern void sub_0809e824(void);
extern void sub_0809f61c(void);
extern void sub_0809f7e4(void);
extern void sub_0809f818(s32 v);
extern void TaskFaceScreenCenter(void);
extern void sub_0809f90c(void);
extern void sub_0809f91c(void);
extern void sub_0809f930(void);
extern void sub_0809f960(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

/* Defined below */
void sub_0809c490(void);
void sub_0809c528(void);
void sub_0809c5a4(void);
void sub_0809c880(void);
void sub_0809c938(void);

void sub_0809c0a8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *q;
    u8 *p42;
    struct Task *b0;
    struct Task *b1;
    struct Task *b2;
    struct Task *b3;
    struct Task *b4;
    struct Task *b5;
    struct Task *b6;
    struct Task *b7;
    struct Task *b8;
    struct Task *b9;
    struct Task *b10;
    struct Task *b11;
    s32 e;
    s32 f;
    s32 k;
    s32 m;
    s32 h;
    s32 a24;
    s32 a25;
    s32 a26;
    s32 a27;
    s32 v33;

    t = gCurTask;
    t->unk40 |= 128 << 4;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    p42 = &t->unk42;
    *p42 = e = 11;
    u = gCurTask;
    u->unk38 = gUnk_08753510;
    u->unk4A -= 3;
    u->unk50 = u->unk4A << 16;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(40);
    b0 = gCurTask;
    b0->unk54 = 0xFFFFD800;
    b0->unk3C = 1;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(1);
    b1 = gCurTask;
    b1->unk54 = 0xFFFF9800;
    b1->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(3);
    b2 = gCurTask;
    b2->unk54 = 0;
    b2->unk3C = 6;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 9;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = e;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(3);
    b3 = gCurTask;
    b3->unk54 = 192 << 9;
    b3->unk5C = f = 0xFFFF8000;
    b3->unk3C = 13;
    TaskYieldTrampoline(5);
    TaskStopX();
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(2);
    b4 = gCurTask;
    b4->unk54 = f;
    b4->unk3C = 15;
    TaskYieldTrampoline(3);
    b5 = gCurTask;
    b5->unk54 = 128 << 8;
    b5->unk3C = 16;
    TaskYieldTrampoline(3);
    b6 = gCurTask;
    b6->unk54 = 0;
    b6->unk3C = 17;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 19;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 20;
    TaskYieldTrampoline(3);
    b7 = gCurTask;
    b7->unk54 = f;
    b7->unk3C = 21;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 22;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 23;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFA000, 0, k = 0x5A5A5A5A, m = 128 << 10, h = 0xFFFF0000, k);
    b8 = gCurTask;
    a24 = 24;
    b8->unk3C = a24;
    TaskYieldTrampoline(3);
    b9 = gCurTask;
    b9->unk58 = m;
    b9->unk60 = h;
    b9->unk3C = a25 = 25;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, k, m, h, k);
    b10 = gCurTask;
    a26 = 26;
    b10->unk3C = a26;
    TaskYieldTrampoline(3);
    b11 = gCurTask;
    b11->unk58 = m;
    b11->unk60 = h;
    b11->unk3C = a27 = 27;
    TaskYieldTrampoline(3);
    TaskStopY();
    gCurTask->unk3C = a24;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = a25;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = a26;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = a27;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = a24;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 29;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 30;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->unk58 = 0;
    w->unk60 = 0xFFFFD800;
    w->unk6C = 0;
    do
    {
        x = gCurTask;
        x->unk3C = 33;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 34;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 1);
    y = gCurTask;
    y->unk60 = 192 << 9;
    y->unk3C = 33;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 35;
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->unk3C = 36;
    TaskYieldTrampoline(1);
    z = gCurTask;
    z->unk3C = 37;
    z->unk58 = 0xFFFA0000;
    while (gCurTask->unk4A > gViewRect[2] - 8)
        TaskYieldTrampoline(1);
    q = gCurTask;
    q->unk00 = 0;
    q->unk0C = 0;
}

void sub_0809c404(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk42 = 11;
    u = gCurTask;
    a = u->unk8C;
    a->unk22 = 0xF000 & u->unk40;
    sub_0809f818(1);
    switch (gCurTask->unk74)
    {
    case 0:
        sub_0809c490();
        break;
    case 1:
        sub_0809d13c();
        break;
    case 2:
        sub_0809dc7c();
        break;
    case 3:
        sub_0809e824();
        break;
    case 4:
        sub_0809f61c();
        break;
    case 5:
        sub_0809f7e4();
        break;
    }
}

void sub_0809c490(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk38 = gUnk_08753204;
    ActorLoadDef((u32)gUnk_08747D5C);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->unk73, 4, gUnk_08747AA4);
}

void sub_0809c4d0(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    t->unk04 = (u32)sub_0809c528;
    t->unk15 = 3;
    gCurTask->unk30 = 0;
    TaskFaceScreenCenter();
    u = gCurTask;
    u->unk60 = 148 << 6;
    u->unk68 = 192 << 10;
    u->unk7A = 0;
    sub_0809c5a4();
}

void sub_0809c50c(void)
{
    CallTableEntry(gCurTask->unk14, 5, gUnk_08747AB4);
}

void sub_0809c528(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 5, gUnk_08747AC8);
    t = gCurTask;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0809c570(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk2C = 90;
    sub_0809f90c();
    sub_0809f91c();
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    sub_0809c5a4();
}

void sub_0809c5a4(void)
{
    while (1)
    {
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(10);
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk3C++;
        TaskYieldTrampoline(6);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(6);
        gCurTask->unk3C--;
        TaskYieldTrampoline(8);
    }
}

void sub_0809c638(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    if (sub_0809f994() != 0)
        sub_0809f930();
    t = gCurTask;
    if (t->unk30 > 0)
    {
        t->unk30--;
        return;
    }
    if (abs(TaskGetNearestPlayerDx()) <= 39
     && abs(TaskGetNearestPlayerDy()) <= 16)
    {
        gCurTask->unk30 = 90;
        ActorSetState(1);
        goto install;
    }
    u = gCurTask;
    if (--u->unk2C > 0)
        return;
    n = 90;
    u->unk30 = n;
    if (TaskGetNearestPlayerDy() < 0 && RandomRange(3) == 0)
        goto stumble;
    if (TaskGetNearestPlayerDy() > 47)
    {
        v = gCurTask;
        v->unk2C = n;
        v->unk30 = 0;
        return;
    }
    TaskGetNearestPlayerDx();
    if (TaskGetNearestPlayerDx() > 59)
    {
        ActorSetState(2);
        TaskSetEntry(sub_0809c880, gCurTaskIdx);
        return;
    }
    if (RandomRange(3) != 0)
        ActorSetState(2);
    else
        ActorSetState(4);
install:
    TaskSetEntry(sub_0809c50c, gCurTaskIdx);
    return;
stumble:
    ActorSetState(3);
    TaskSetEntry(sub_0809c50c, gCurTaskIdx);
}

void sub_0809c74c(void)
{
    struct Task *t;
    s32 k;
    s16 a;
    s16 b;
    s16 c;
    s16 d;
    s16 e;

    gCurTask->unk15 = 1;
    TaskStop();
    sub_0809f90c();
    t = gCurTask;
    a = 25;
    t->unk3C = a;
    TaskYieldTrampoline(10);
    t = gCurTask;
    b = 26;
    t->unk3C = b;
    TaskYieldTrampoline(2);
    PlaySfx(196);
    TaskSetMotionXFacing(128 << 11, k = 0x5A5A5A5A);
    t = gCurTask;
    c = 22;
    t->unk3C = c;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(192 << 10, k);
    t = gCurTask;
    d = 23;
    t->unk3C = d;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 10, k);
    t = gCurTask;
    e = 24;
    t->unk3C = e;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(16);
    gCurTask->unk3C = e;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = d;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = c;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = b;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = a;
    TaskYieldTrampoline(10);
    sub_0809f930();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809c840(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_0809c50c, gCurTaskIdx);
    if ((u16)(gCurTask->unk3C - 22) <= 1)
        ActorCheckHitsWithBox((s32)gUnk_08747EF4);
}

void sub_0809c880(void)
{
    gCurTask->unk15 = 2;
    TaskStop();
    gCurTask->unk18 = 0;
    sub_0809f90c();
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(10);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(2);
    sub_0809cf04();
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(60);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809c8f8(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_0809c50c, gCurTaskIdx);
    if (gCurTask->unk18 != 0)
        TaskSetEntry(sub_0809c938, gCurTaskIdx);
}

void sub_0809c938(void)
{
    struct Task *t = gCurTask;

    t->unk18 = 0;
    t->unk3C = 9;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(30);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809c984(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;
    s16 v;

    t = gCurTask;
    z = 0;
    t->unk15 = 3;
    TaskStop();
    sub_0809f90c();
    TaskSetMotionY(0xFFFD0000, 148 << 6, 192 << 10);
    gCurTask->unk7A = z;
    gCurTask->unk3C = 6;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(2);
    u = gCurTask;
    v = 12;
    u->unk3C = v;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(11);
    gCurTask->unk3C = v;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(2);
    sub_0809cf04();
    gCurTask->unk3C = 14;
    TaskSleepForever();
}

void sub_0809ca0c(void)
{
}

void sub_0809ca10(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;
    s16 v;
    s16 w;

    gCurTask->unk15 = 4;
    TaskStop();
    sub_0809f960();
    t = gCurTask;
    z = 0;
    t->unk3C = 6;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(2);
    u = gCurTask;
    v = 12;
    u->unk3C = v;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(11);
    gCurTask->unk3C = v;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(2);
    sub_0809cf04();
    u = gCurTask;
    w = 14;
    u->unk3C = w;
    TaskYieldTrampoline(26);
    TaskSetMotionY(0xFFFD0000, 148 << 6, 192 << 10);
    gCurTask->unk7A = z;
    gCurTask->unk3C = w;
    TaskYieldTrampoline(12);
    TaskTurnAroundAndReverseX();
    TaskSleepForever();
}

void sub_0809caac(void)
{
}

void sub_0809cab0(void)
{
    struct Task *t;

    gCurTask->unk04 = (u32)sub_0809cb90;
    TaskFaceScreenCenter();
    while (1)
    {
        TaskStop();
        t = gCurTask;
        t->unk3C = 25;
        TaskYieldTrampoline(10);
        t = gCurTask;
        t->unk3C = 26;
        TaskYieldTrampoline(2);
        PlaySfx(196);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        t = gCurTask;
        t->unk3C = 22;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
        t = gCurTask;
        t->unk3C = 23;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
        t = gCurTask;
        t->unk3C = 24;
        TaskYieldTrampoline(2);
        TaskStop();
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(16);
        gCurTask->unk3C = 24;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 26;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 25;
        TaskYieldTrampoline(10);
    }
}
