
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
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
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);

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
    t->tileWord |= 128 << 4;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    p42 = &t->layer;
    *p42 = e = 11;
    u = gCurTask;
    u->frameTable = gUnk_08753510;
    u->pixelY -= 3;
    u->posY = u->pixelY << 16;
    gCurTask->frame = 0;
    TaskYieldTrampoline(40);
    b0 = gCurTask;
    b0->velX = 0xFFFFD800;
    b0->frame = 1;
    TaskYieldTrampoline(3);
    gCurTask->frame = 2;
    TaskYieldTrampoline(3);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    b1 = gCurTask;
    b1->velX = 0xFFFF9800;
    b1->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(3);
    b2 = gCurTask;
    b2->velX = 0;
    b2->frame = 6;
    TaskYieldTrampoline(3);
    gCurTask->frame = 7;
    TaskYieldTrampoline(3);
    gCurTask->frame = 8;
    TaskYieldTrampoline(3);
    gCurTask->frame = 9;
    TaskYieldTrampoline(3);
    gCurTask->frame = 10;
    TaskYieldTrampoline(3);
    gCurTask->frame = e;
    TaskYieldTrampoline(3);
    gCurTask->frame = 12;
    TaskYieldTrampoline(3);
    b3 = gCurTask;
    b3->velX = 192 << 9;
    b3->accelX = f = 0xFFFF8000;
    b3->frame = 13;
    TaskYieldTrampoline(5);
    TaskStopX();
    gCurTask->frame = 14;
    TaskYieldTrampoline(2);
    b4 = gCurTask;
    b4->velX = f;
    b4->frame = 15;
    TaskYieldTrampoline(3);
    b5 = gCurTask;
    b5->velX = 128 << 8;
    b5->frame = 16;
    TaskYieldTrampoline(3);
    b6 = gCurTask;
    b6->velX = 0;
    b6->frame = 17;
    TaskYieldTrampoline(3);
    gCurTask->frame = 18;
    TaskYieldTrampoline(3);
    gCurTask->frame = 19;
    TaskYieldTrampoline(3);
    gCurTask->frame = 20;
    TaskYieldTrampoline(3);
    b7 = gCurTask;
    b7->velX = f;
    b7->frame = 21;
    TaskYieldTrampoline(1);
    gCurTask->frame = 22;
    TaskYieldTrampoline(1);
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFA000, 0, k = 0x5A5A5A5A, m = 128 << 10, h = 0xFFFF0000, k);
    b8 = gCurTask;
    a24 = 24;
    b8->frame = a24;
    TaskYieldTrampoline(3);
    b9 = gCurTask;
    b9->velY = m;
    b9->accelY = h;
    b9->frame = a25 = 25;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, k, m, h, k);
    b10 = gCurTask;
    a26 = 26;
    b10->frame = a26;
    TaskYieldTrampoline(3);
    b11 = gCurTask;
    b11->velY = m;
    b11->accelY = h;
    b11->frame = a27 = 27;
    TaskYieldTrampoline(3);
    TaskStopY();
    gCurTask->frame = a24;
    TaskYieldTrampoline(3);
    gCurTask->frame = a25;
    TaskYieldTrampoline(3);
    gCurTask->frame = a26;
    TaskYieldTrampoline(3);
    gCurTask->frame = a27;
    TaskYieldTrampoline(3);
    gCurTask->frame = a24;
    TaskYieldTrampoline(3);
    gCurTask->frame = 28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 29;
    TaskYieldTrampoline(3);
    gCurTask->frame = 30;
    TaskYieldTrampoline(2);
    gCurTask->frame = 31;
    TaskYieldTrampoline(3);
    gCurTask->frame = 32;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->velY = 0;
    w->accelY = 0xFFFFD800;
    w->unk6C = 0;
    do
    {
        x = gCurTask;
        x->frame = 33;
        TaskYieldTrampoline(2);
        gCurTask->frame = 34;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 1);
    y = gCurTask;
    y->accelY = 192 << 9;
    y->frame = 33;
    TaskYieldTrampoline(2);
    gCurTask->frame = 35;
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->frame = 36;
    TaskYieldTrampoline(1);
    z = gCurTask;
    z->frame = 37;
    z->velY = 0xFFFA0000;
    while (gCurTask->pixelY > gViewRect[2] - 8)
        TaskYieldTrampoline(1);
    q = gCurTask;
    q->moveCallback = 0;
    q->drawCallback = 0;
}

void Task_MetaKnightsKnight(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->layer = 11;
    u = gCurTask;
    a = u->u8C.actor;
    a->savedPaletteBits = 0xF000 & u->tileWord;
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

    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gAxeKnightFrames;
    ActorLoadDef((u32)gAxeKnightDef);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->variant, 4, gAxeKnightVariants);
}

void sub_0809c4d0(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    t->updateCallback = (u32)AxeKnightUpdate;
    t->updateState = 3;
    gCurTask->unk30 = 0;
    TaskFaceScreenCenter();
    u = gCurTask;
    u->accelY = 148 << 6;
    u->speedLimitY = 192 << 10;
    u->onGround = 0;
    sub_0809c5a4();
}

void AxeKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 5, gAxeKnightStates);
}

void AxeKnightUpdate(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gAxeKnightStateUpdates);
    t = gCurTask;
    if (t->unk24 > 0)
    {
        t->unk24--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void AxeKnightWalk(void)
{
    gCurTask->updateState = 0;
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
        gCurTask->frame = 4;
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    }
}

void AxeKnightWalkUpdate(void)
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
        TaskSetEntry(AxeKnightThrow, gCurTaskIdx);
        return;
    }
    if (RandomRange(3) != 0)
        ActorSetState(2);
    else
        ActorSetState(4);
install:
    TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
    return;
stumble:
    ActorSetState(3);
    TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
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

    gCurTask->updateState = 1;
    TaskStop();
    sub_0809f90c();
    t = gCurTask;
    a = 25;
    t->frame = a;
    TaskYieldTrampoline(10);
    t = gCurTask;
    b = 26;
    t->frame = b;
    TaskYieldTrampoline(2);
    PlaySfx(196);
    TaskSetMotionXFacing(128 << 11, k = 0x5A5A5A5A);
    t = gCurTask;
    c = 22;
    t->frame = c;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(192 << 10, k);
    t = gCurTask;
    d = 23;
    t->frame = d;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 10, k);
    t = gCurTask;
    e = 24;
    t->frame = e;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 17;
    TaskYieldTrampoline(16);
    gCurTask->frame = e;
    TaskYieldTrampoline(2);
    gCurTask->frame = d;
    TaskYieldTrampoline(1);
    gCurTask->frame = c;
    TaskYieldTrampoline(2);
    gCurTask->frame = b;
    TaskYieldTrampoline(2);
    gCurTask->frame = a;
    TaskYieldTrampoline(10);
    sub_0809f930();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809c840(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
    if ((u16)(gCurTask->frame - 22) <= 1)
        ActorCheckHitsWithBox((s32)gUnk_08747EF4);
}

void AxeKnightThrow(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    gCurTask->unk18 = 0;
    sub_0809f90c();
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(3);
    gCurTask->frame = 12;
    TaskYieldTrampoline(3);
    gCurTask->frame = 11;
    TaskYieldTrampoline(10);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    CreateAxeKnightAxe();
    gCurTask->frame = 14;
    TaskYieldTrampoline(60);
    ActorSetState(0);
    TaskSleepForever();
}

void AxeKnightThrowUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
    if (gCurTask->unk18 != 0)
        TaskSetEntry(AxeKnightCatchAxe, gCurTaskIdx);
}

void AxeKnightCatchAxe(void)
{
    struct Task *t = gCurTask;

    t->unk18 = 0;
    t->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 17;
    TaskYieldTrampoline(30);
    ActorSetState(0);
    TaskSleepForever();
}

void AxeKnightJumpThrow(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;
    s16 v;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    TaskStop();
    sub_0809f90c();
    TaskSetMotionY(0xFFFD0000, 148 << 6, 192 << 10);
    gCurTask->onGround = z;
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    u = gCurTask;
    v = 12;
    u->frame = v;
    TaskYieldTrampoline(2);
    gCurTask->frame = 11;
    TaskYieldTrampoline(11);
    gCurTask->frame = v;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    CreateAxeKnightAxe();
    gCurTask->frame = 14;
    TaskSleepForever();
}

void AxeKnightJumpThrowUpdate(void)
{
}

void sub_0809ca10(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;
    s16 v;
    s16 w;

    gCurTask->updateState = 4;
    TaskStop();
    sub_0809f960();
    t = gCurTask;
    z = 0;
    t->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    u = gCurTask;
    v = 12;
    u->frame = v;
    TaskYieldTrampoline(2);
    gCurTask->frame = 11;
    TaskYieldTrampoline(11);
    gCurTask->frame = v;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    CreateAxeKnightAxe();
    u = gCurTask;
    w = 14;
    u->frame = w;
    TaskYieldTrampoline(26);
    TaskSetMotionY(0xFFFD0000, 148 << 6, 192 << 10);
    gCurTask->onGround = z;
    gCurTask->frame = w;
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

    gCurTask->updateCallback = (u32)sub_0809cb90;
    TaskFaceScreenCenter();
    while (1)
    {
        TaskStop();
        t = gCurTask;
        t->frame = 25;
        TaskYieldTrampoline(10);
        t = gCurTask;
        t->frame = 26;
        TaskYieldTrampoline(2);
        PlaySfx(196);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        t = gCurTask;
        t->frame = 22;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
        t = gCurTask;
        t->frame = 23;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
        t = gCurTask;
        t->frame = 24;
        TaskYieldTrampoline(2);
        TaskStop();
        gCurTask->frame = 17;
        TaskYieldTrampoline(16);
        gCurTask->frame = 24;
        TaskYieldTrampoline(2);
        gCurTask->frame = 23;
        TaskYieldTrampoline(1);
        gCurTask->frame = 22;
        TaskYieldTrampoline(2);
        gCurTask->frame = 26;
        TaskYieldTrampoline(2);
        gCurTask->frame = 25;
        TaskYieldTrampoline(10);
    }
}
