
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u16 gFrameCount;
extern struct Task gTasks[];
extern vu16 gTaskSlotTypes[];
extern u32 gUnk_08745CEC[];
extern u32 gUnk_08747ADC[];
extern u32 gUnk_08747AE4[];
extern u32 gUnk_08747AFC[];
extern u32 gUnk_08747B10[];
extern u32 gUnk_08747B24[];
extern u32 gUnk_08747B38[];
extern u32 gUnk_08747B58[];
extern u32 gUnk_08747B68[];
extern u32 gUnk_08747E64[];
extern u32 gUnk_08753270[];
extern u32 gUnk_08753290[];

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
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
extern void ActorDestroy(void);
extern void TaskFaceLikeParent(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern void ActorDrawWorldInViewOrDestroy(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern void sub_0809c50c(void);
extern void sub_0809c528(void);
extern void sub_0809c5a4(void);
extern void sub_0809d994(void);
extern u8 sub_0809dbc4(void);
extern u8 sub_0809dc3c(void);
extern void TaskFaceScreenCenter(void);
extern void sub_0809f90c(void);
extern void sub_0809f930(void);
extern void sub_0809f970(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

/* Defined below */
void sub_0809cd4c(void);
void sub_0809cec4(void);
void sub_0809cfe0(void);
void sub_0809d17c(void);
void sub_0809d1dc(void);
void sub_0809d280(void);
void sub_0809d2a4(void);
void sub_0809d6dc(void);
void sub_0809d83c(void);
void sub_0809d8f8(void);
void sub_0809d944(void);

void sub_0809cc24(void)
{
    struct Task *t;

    gCurTask->unk04 = (u32)sub_0809cd4c;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    t = gCurTask;
    t->unk3C = 4;
    TaskYieldTrampoline(9);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(7);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk73 = 0;
    ActorSetState(0);
    t = gCurTask;
    t->unk04 = (u32)sub_0809c528;
    t->unk15 = 0;
    t = gCurTask;
    t->unk2C = 90;
    t->unk30 = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cd4c(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && sub_0809f994() != 0)
        sub_0809f930();
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

void sub_0809cd8c(void)
{
    struct Task *t;

    gCurTask->unk04 = (u32)sub_0809cec4;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    t = gCurTask;
    t->unk3C = 4;
    TaskYieldTrampoline(9);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    gCurTask->unk73 = 0;
    ActorSetState(0);
    t = gCurTask;
    t->unk04 = (u32)sub_0809c528;
    t->unk15 = 0;
    t = gCurTask;
    t->unk2C = 90;
    t->unk30 = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->unk3C--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cec4(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && sub_0809f994() != 0)
        sub_0809f930();
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

void sub_0809cf04(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    PlaySfx(186);
    sp.unk00 = 26;
    sp.unk04 = 129;
    sp.unk08 = (t = gCurTask)->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = 20;
    sp.unk0E = 0;
    sp.unk10 = gUnk_08745CEC[0];
    sp.unk0A = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void sub_0809cf60(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 11;
    u = gCurTask;
    u->unk38 = gUnk_08753270;
    u->unk04 = (u32)sub_0809cfe0;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(224 << 10, 0xFFFFDB00);
    gCurTask->unk64 = 128 << 11;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
    }
}

void sub_0809cfe0(void)
{
    struct PointPair box;
    struct Task *t;
    struct Task *w;
    s32 i;

    t = gCurTask;
    box.x0 = t->unk48 - 10;
    box.y0 = t->unk4A - 4;
    box.x1 = t->unk48 + 10;
    box.y1 = t->unk4A + 4;
    if (gTaskSlotTypes[i = t->unk44] == 58 && gTasks[i].unk74 == 0
        && TaskIsInRectSlot(&box, i) != 0)
    {
        w = &gTasks[gCurTask->unk44];
        w->unk18 = 1;
        ActorDestroy();
    }
    else
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

u8 sub_0809d0a0(void)
{
    u8 r;

    if (gCurTask->unk73 != 0)
        r = 0;
    else
    {
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(sub_0809c50c, gCurTaskIdx);
        r = 1;
    }
    return r;
}

u8 sub_0809d0dc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk73 == 0 && t->unk14 == 1)
    {
        t->unk4C -= t->unk54;
        t->unk48 = t->unk4C >> 16;
        TaskStopX();
    }
    else
    {
        u = gCurTask;
        if (u->unk73 != 1)
        {
            sub_0809f930();
        }
        else
        {
            u->unk4C -= u->unk54;
            u->unk48 = u->unk4C >> 16;
            sub_0809f970();
        }
    }
    return 0;
}

u8 sub_0809d138(void)
{
    return 0;
}

void sub_0809d13c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk0C = (u32)sub_0809d17c;
    t->unk38 = gUnk_08753290;
    ActorLoadDef((u32)gUnk_08747E64);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->unk73, 2, gUnk_08747ADC);
}

void sub_0809d17c(void)
{
    ActorDrawWorldInViewOrDestroy();
    sub_0809d994();
}

void sub_0809d18c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = (u32)sub_0809d1dc;
    t->unk28 = 3;
    sub_0809f90c();
    sub_0809d8f8();
    ActorSetState(1);
    u = gCurTask;
    u->unk15 = 0;
    sub_0809d2a4();
}

void sub_0809d1c0(void)
{
    CallTableEntry(gCurTask->unk14, 6, gUnk_08747AE4);
}

void sub_0809d1dc(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s16 *p;
    s8 *q;
    s32 lim;

    if (ActorCollideTerrain() == 0)
    {
        if (sub_0809f994() != 0)
            sub_0809dbc4();
        t = gCurTask;
        p = &t->unk4A;
        a = t->unk8C;
        q = (s8 *)a->unk50;
        lim = q[2];
        if (*p < lim)
        {
            *p = lim;
            t->unk50 = *p << 16;
            sub_0809dc3c();
        }
        CallTableEntry(gCurTask->unk15, 5, gUnk_08747AFC);
    }
    u = gCurTask;
    if (u->unk24 > 0)
    {
        u->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0809d25c(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk28 = 3;
    ActorSetState(1);
    sub_0809d280();
}

void sub_0809d280(void)
{
    gCurTask->unk15 = 0;
    TaskYieldTrampoline(2);
    sub_0809f90c();
    sub_0809d8f8();
    sub_0809d2a4();
}

void sub_0809d2a4(void)
{
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        TaskSetFrame(8);
        TaskYieldTrampoline(4);
        TaskSetFrame(9);
        TaskYieldTrampoline(4);
        TaskSetFrame(10);
        TaskYieldTrampoline(4);
        TaskSetFrame(11);
        TaskYieldTrampoline(4);
    }
}

void sub_0809d308(void)
{
}

void sub_0809d30c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 1;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->unk7A = z;
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    u = gCurTask;
    u->unk58 = 0xFFFF0000;
    u->unk60 = 128 << 5;
    TaskSetFrame(12);
    TaskYieldTrampoline(2);
    TaskSetFrame(13);
    TaskYieldTrampoline(2);
    TaskSetFrame(14);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(8);
    TaskSetFrame(28);
    TaskYieldTrampoline(6);
    TaskSetFrame(29);
    TaskYieldTrampoline(4);
    TaskSetFrame(30);
    TaskYieldTrampoline(4);
    TaskSetFrame(31);
    TaskYieldTrampoline(4);
    TaskSetFrame(32);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(10);
    TaskYieldTrampoline(4);
    TaskSetFrame(11);
    TaskYieldTrampoline(3);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809d42c(void)
{
    struct Task *t;
    u16 v;

    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
    t = gCurTask;
    v = t->unk3C;
    if ((u16)(v - 12) <= 3)
        ActorCheckHitsWithBox(gUnk_08747B10[t->unk3C - 12]);
    else if ((u16)(v - 28) <= 4)
        ActorCheckHitsWithBox(gUnk_08747B24[t->unk3C - 28]);
}

void sub_0809d4a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 2;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->unk7A = z;
    u = gCurTask;
    u->unk58 = 0xFFFE0000;
    u->unk60 = 128 << 5;
    TaskSetFrame(4);
    TaskYieldTrampoline(6);
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    sub_0809d944();
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(8);
        TaskYieldTrampoline(6);
        TaskSetFrame(9);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(6);
        TaskSetFrame(11);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
    }
}

void sub_0809d568(void)
{
}

void sub_0809d56c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 3;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->unk7A = z;
    u = gCurTask;
    u->unk58 = 0xFFFC0000;
    u->unk60 = 128 << 6;
    while (1)
    {
        PlaySfx(231);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        TaskSetFrame(20);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        TaskSetFrame(22);
        TaskYieldTrampoline(2);
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d608(void)
{
    struct Task *t;
    u16 v;

    t = gCurTask;
    v = t->unk3C;
    if ((u16)(v - 17) <= 7)
        ActorCheckHitsWithBox(gUnk_08747B38[t->unk3C - 17]);
}

void sub_0809d638(void)
{
    gCurTask->unk15 = 4;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809d6b4(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
}

void sub_0809d6dc(void)
{
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(25);
        TaskYieldTrampoline(2);
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 3);
}

void sub_0809d71c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 1 && (gFrameCount & 1) != 0)
    {
        t->unk2C = 1;
        ActorSetState(4);
        TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
        return;
    }
    gCurTask->unk2C = 0;
    if (abs(TaskGetNearestPlayerDx()) > 39)
    {
        ActorSetState(3);
        TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
        return;
    }
    ActorSetState(2);
    TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
}

void sub_0809d7a4(void)
{
    struct Task *u;

    gCurTask->unk04 = (u32)sub_0809d83c;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    u = gCurTask;
    u->unk28 = 4;
    u->unk2C = 1;
    while (1)
    {
        PlaySfx(231);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        TaskSetFrame(20);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        TaskSetFrame(22);
        TaskYieldTrampoline(2);
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d83c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->unk30 = 1;
    ActorCollideTerrain();
    if (sub_0809f994() != 0)
    {
        sub_0809dbc4();
        gCurTask->unk30 = 0;
    }
    if (gCurTask->unk4A <= 15)
    {
        TaskStopY();
        u = gCurTask;
        u->unk50 = 128 << 13;
        u->unk4A = 16;
        if (u->unk2C == 0)
            u->unk30 = 0;
    }
    v = gCurTask;
    if (v->unk30 != 0)
    {
        if (v->unk2C == 0)
            v->unk58 += 0xFFFFFB00;
        else
            v->unk58 += 160 << 3;
    }
    gCurTask->unk7A = 0;
    w = gCurTask;
    if (w->unk24 > 0)
    {
        w->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
    ActorCheckHitsWithBox(gUnk_08747B38[gCurTask->unk3C - 17]);
}

void sub_0809d8f8(void)
{
    struct Task *t;
    u32 r;

    r = (u8)RandomRange(8);
    TaskSetMotionXFacing(gUnk_08747B58[r >> 1], 0x5A5A5A5A);
    t = gCurTask;
    t->unk58 = gUnk_08747B68[r];
    t->unk60 = 128 << 5;
    t->unk7A = 0;
}

void sub_0809d944(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    sp.unk00 = 29;
    sp.unk04 = 132;
    sp.unk08 = (t = gCurTask)->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = 0xF110;
    sp.unk0A = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
}
