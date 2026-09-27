
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02007D00[];
extern u32 gUnk_02020000[];
extern u32 gUnk_03001570[];
extern u32 gUnk_0300158E[];
extern u32 gUnk_030015A0[];
extern u32 gUnk_030015CE[];
extern u32 gUnk_030015EC[];
extern struct Task gTasks[];
extern u32 gUnk_0827AC64[];
extern u32 gUnk_0827AC78[];
extern u32 gUnk_0827AC7C[];
extern u32 gUnk_0827B8F8[];
extern u32 gUnk_0827B90C[];
extern u32 gUnk_0827B914[];
extern u32 gUnk_0827CA48[];
extern u32 gUnk_0827CA5C[];
extern u32 gUnk_0827CA60[];
extern u32 gUnk_0827D808[];
extern u32 gUnk_0827D81C[];
extern u32 gUnk_0827D820[];
extern u32 gUnk_08745B0C[];
extern u32 gUnk_08745B20[];
extern u32 gUnk_08745B28[];
extern u32 gUnk_08745CEC[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BlendColors(void *src, void *dst, s32 ratio, s32 count, void *out);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskStop(void);
extern void TaskSetFrameNoFlip(s32 a);
extern void HudAnimateTaskHpBar(void);
extern void HudRemoveHpBar(void);
extern void sub_080262e8(u8 a);
extern void ActorSetState(u16 v);
extern void ActorDestroy(void);
extern s32 CreateActor(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y, u32 prio);
extern void sub_08066544(void);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern u32 ActorCheckHits(void);
extern u8 sub_0806951c(void);
extern u32 ActorReactToHit(void);
extern void sub_0809c028(void);
extern void sub_0809c0a8(void);
extern void sub_0809fbd0(void);
extern void sub_0809fc08(void);

/* Defined below */
void sub_0809ba94(void);
void sub_0809baec(void);
void sub_0809bc1c(void);
void sub_0809bf2c(void);
void sub_0809bfac(void);

void sub_0809ba44(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0809ba94;
    t->unk2C = -gTasks[t->unk44].facing;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08745B20);
}

void sub_0809ba94(void)
{
    if (sub_0806951c() == 0)
    {
        CallTableEntry(gCurTask->unk15, 2, gUnk_08745B28);
    }
    else
    {
        ActorSetState(1);
        TaskSetEntry(sub_0809baec, gCurTaskIdx);
    }
    if (gCurTask->unk14 != 1)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_0809baec(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08745B20);
}

void sub_0809bb08(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk28 = 0;
    t->unk54 = gUnk_08745B0C[t->unk74] * t->unk2C;
    t->unk58 = 0xFFFD0000;
    t->unk60 = 0xC0 << 6;
    t->unk68 = 0xC0 << 10;
    TaskSetFrameNoFlip(4);
    TaskYieldTrampoline(6);
    TaskSetFrameNoFlip(5);
    TaskSleepForever();
}

void sub_0809bb68(void)
{
}

void sub_0809bb6c(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    gCurTask->unk7A = 0;
    gCurTask->unk28 = 0;
    TaskStop();
    t = gCurTask;
    t->unk58 = 0xFFFD0000;
    t->unk60 = 0xC0 << 6;
    t->unk68 = 0xC0 << 10;
    t->frame = 6;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame += 2;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame += 1;
    TaskYieldTrampoline(2);
    ActorDestroy();
    TaskSleepForever();
}

void sub_0809bbd4(void)
{
}

void sub_0809bbd8(void)
{
    struct Task *t;

    sub_0809c028();
    sub_0809fbd0();
    sub_08067108();
    sub_08066544();
    sub_0809c0a8();
    sub_0809fc08();
    sub_0809bfac();
    sub_08067114();
    t = gCurTask;
    t->unk04 = (u32)sub_0809bc1c;
    t->unk08 = (u32)sub_0809bf2c;
    TaskSleepForever();
}

void sub_0809bc1c(void)
{
    struct Task *t;
    struct Task *t2;
    struct Task *u;
    struct Task *u2;
    struct Task *v;
    struct Task *v2;
    struct Task *w;
    struct Task *w2;
    struct Task *x;
    u8 *q;
    s16 *p;
    s16 *p2;
    s32 r;
    s32 n;

    if (gUnk_02007D00[0] == 0)
    {
        t = gCurTask;
        q = (u8 *)(t->unk28 + (t->unk18 << 3));
        r = CreateActor(3, 9, 58, q[1], q[0], ((s16 *)q)[1], ((s16 *)q)[2],
                         gUnk_08745CEC[q[0]]);
        t2 = gCurTask;
        p2 = &t2->unk46;
        *p2 = r;
        ((struct Task *)(*p2 * 144 + (s32)gTasks))->unk6E = 0;
        t2->unk18++;
    }
    if (gUnk_02007D00[1] == 0)
    {
        u = gCurTask;
        q = (u8 *)(u->unk2C + (u->unk1C << 3));
        r = CreateActor(3, 9, 58, q[1], q[0], ((s16 *)q)[1], ((s16 *)q)[2],
                         gUnk_08745CEC[q[0]]);
        u2 = gCurTask;
        p2 = &u2->unk46;
        *p2 = r;
        ((struct Task *)(*p2 * 144 + (s32)gTasks))->unk6E = 1;
        u2->unk1C++;
    }
    if (gUnk_02007D00[2] == 0)
    {
        v = gCurTask;
        q = (u8 *)(v->unk30 + (v->unk20 << 3));
        r = CreateActor(3, 9, 58, q[1], q[0], ((s16 *)q)[1], ((s16 *)q)[2],
                         gUnk_08745CEC[q[0]]);
        v2 = gCurTask;
        p2 = &v2->unk46;
        *p2 = r;
        ((struct Task *)(*p2 * 144 + (s32)gTasks))->unk6E = 2;
        v2->unk20++;
    }
    if (gUnk_02007D00[3] == 0)
    {
        w = gCurTask;
        q = (u8 *)(w->unk34 + (w->unk24 << 3));
        r = CreateActor(3, 9, 58, q[1], q[0], ((s16 *)q)[1], ((s16 *)q)[2],
                         gUnk_08745CEC[q[0]]);
        w2 = gCurTask;
        p2 = &w2->unk46;
        *p2 = r;
        ((struct Task *)(*p2 * 144 + (s32)gTasks))->unk6E = 3;
        w2->unk24++;
    }
    if ((s16)gCurTask->unk6C > 63)
    {
        BlendColors(gUnk_0827AC78, gUnk_0827AC7C,
                     (u16)(abs(72 - (s16)gCurTask->unk6C) * 255 / 8), 1, gUnk_0300158E);
        BlendColors(gUnk_0827B90C, gUnk_0827B914,
                     (u16)(abs(72 - (s16)gCurTask->unk6C) * 255 / 8), 3, gUnk_030015A0);
        BlendColors(gUnk_0827CA5C, gUnk_0827CA60,
                     (u16)(abs(72 - (s16)gCurTask->unk6C) * 255 / 8), 1, gUnk_030015CE);
        BlendColors(gUnk_0827D81C, gUnk_0827D820,
                     (u16)(abs(72 - (s16)gCurTask->unk6C) * 255 / 8), 1, gUnk_030015EC);
    }
    x = gCurTask;
    p = &x->unk6C;
    *p = *p + 1;
    if ((s16)*p > 80)
        *p = 0;
    if (gUnk_02007D00[0] < 0 && gUnk_02007D00[1] < 0 && gUnk_02007D00[2] < 0
        && gUnk_02007D00[3] < 0)
    {
        sub_080262e8(gCurTask->unk73);
        HudRemoveHpBar();
        ActorDestroy();
    }
}

void sub_0809bf2c(void)
{
    if (gUnk_02007D00[5] & 1)
        gCurTask->unk78 -= gUnk_02007D00[4];
    if (gUnk_02007D00[5] & 2)
        gCurTask->unk78 -= gUnk_02007D00[4];
    if (gUnk_02007D00[5] & 4)
        gCurTask->unk78 -= gUnk_02007D00[4];
    if (gUnk_02007D00[5] & 8)
        gCurTask->unk78 -= gUnk_02007D00[4];
    if (gUnk_02007D00[5] != 0)
        HudAnimateTaskHpBar();
    gUnk_02007D00[5] = 0;
}

void sub_0809bfac(void)
{
    struct GfxHeader *g;
    u16 *p;

    RequestCopy(4, (u32)gUnk_02020000, 0x06010000, 240 << 6);
    g = (struct GfxHeader *)gUnk_0827AC64;
    RequestCopy(2, (u32)g->unk08, (u32)(p = (u16 *)gUnk_03001570), g->unk00 << 5);
    g = (struct GfxHeader *)gUnk_0827B8F8;
    RequestCopy(2, (u32)g->unk08, (u32)(p + 16), g->unk00 << 5);
    g = (struct GfxHeader *)gUnk_0827CA48;
    RequestCopy(2, (u32)g->unk08, (u32)(p + 32), g->unk00 << 5);
    g = (struct GfxHeader *)gUnk_0827D808;
    RequestCopy(2, (u32)g->unk08, (u32)(p + 48), g->unk00 << 5);
}
