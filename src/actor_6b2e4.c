/* game_code_and_rodata 0x0806B2E4-0x0806C2A4 (issue #64, module M18 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806B2E4 0x0806C2A4 src/actor_6b2e4.c --newpb
 *
 * The "carried by / riding on another task" movement block: the per-frame
 * position integrators sub_0806b410 and sub_0806b670 that walk the two stride-5
 * offset tables at 0x0873E7C4 / 0x0873E864, the handover helpers that hand the
 * actor back to the generic task body (sub_0806b8bc), the player-record
 * bookkeeping around gPlayerStates[] (sub_0806b9dc, sub_0806bd10, sub_0806be4c),
 * and the class-1 task bodies sub_0806bf54 / sub_0806c05c / sub_0806c158 with
 * their per-frame callbacks.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0873E78C[];
extern u32 gUnk_0874C9D8[];

extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(u32 a);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(s32 a);
extern void ActorDestroy(void);
extern void sub_0806a0cc(void);
extern void sub_0806d65c(void);
extern void sub_080b54d0(s32 i);
extern void TaskYieldTrampoline(u32 a);
extern u32 sub_08021a40(s32 x, s32 y);
extern u32 CanBreakBlock(s32 x, s32 y, s32 c, s32 d);
extern void ActorSetTerrainBox(u32 *p);

extern s16 gUnk_0873E7C4[];
extern s16 gUnk_0873E864[];
extern u16 gUnk_0873EAD8[][4];
extern u32 gUnk_0873F8B4[];
extern u32 gUnk_0873F8BC[];

void sub_0806b8bc(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void TaskMove(void);
extern void ActorDie(void);
extern void sub_08065640(void);
extern void ActorSetHitReactions(u32 *p);
extern u32 gUnk_0873F92C[];
extern struct PlayerState gPlayerStates[];
extern void sub_08065d44(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 TaskGetDxTo(s32 i);
extern void ActorAwardScore(s32 a, s32 b);
extern s16 gUnk_0873E7A4[];
extern s16 gUnk_0300244C;
extern u8 gUnk_02007CF4[];
extern u8 gUnk_02006178;
extern u32 gUnk_0873EAA0[];
extern u32 gUnk_0873F938[];
extern u32 gUnk_0873F8F4[];
extern void TaskMoveRelativeToParent(void);
extern void ActorSetTerrainHandlers(u32 *p);
void sub_0806bfd8(void);
void sub_0806c0c0(void);
void sub_0806c148(void);
void sub_0806c1d0(void);
extern void sub_0806a158(void);
extern void ActorMove(void);
extern void ActorCollideTerrain(void);
extern void RequestScreenShake(u32 a);
extern void RegisterCollider(u8 a, s16 x, s16 y, u32 *p);
extern void TaskSetMotionXFacing(u32 a, u32 b);
extern void TaskSetMotionY(u32 a, u32 b, u32 c);
extern u32 gUnk_0873F830[];
extern u32 gUnk_0873F844[];
extern u32 gUnk_0873F894[];
extern u8 gTerrainResult[];
void sub_0806bd10(void);
void sub_0806bf38(void);
void sub_0806bc54(void);
void sub_0806befc(void);

void sub_0806b2e4(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_0873E78C);
}

void sub_0806b300(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->unk18);
    gCurTask->unk58 = 0x4000;
    TaskSleepForever();
}

void sub_0806b330(void)
{
}

void sub_0806b334(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->unk18);
    TaskYieldTrampoline(30);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_0806b368(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_0806b2e4, gCurTaskIdx);
}

void sub_0806b390(void)
{
    struct Task *t;

    gCurTask->unk15 = 2;
    t = gCurTask;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    sub_0806a0cc();
    sub_0806d65c();
    ActorDestroy();
}

void sub_0806b3c0(void)
{
}

void sub_0806b3c4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = 0;
    if (t->unk76 != 0)
        sub_080b54d0(gCurTaskIdx);
    TaskYieldTrampoline(1);
    u = gCurTask;
    u->unk38 = gUnk_0874C9D8;
    u->unk40 = 0;
    PlaySfx(109);
    sub_0806d65c();
}

void sub_0806b40c(void)
{
}

void sub_0806b410(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct PlayerState *p;
    struct Task *s;
    s16 e;
    s32 k;
    s16 n;
    s16 g;
    s16 h;
    u32 tb;
    u8 b;

    t = gCurTask;
    s = &gTasks[t->unk44];
    p = t->unk88;
    if (t->unk28 != s->unk43)
        t->unk43 = -t->unk43;
    u = gCurTask;
    u->unk28 = s->unk43;
    if (*(s8 *)&p->unk16 == -1)
    {
        u->unk48 = u->unk20;
        u->unk4A = u->unk1C;
        sub_0806b8bc();
        return;
    }
    if ((u8)(p->unk16 + 5) <= 2)
    {
        u->unk43 = s->unk43;
        v = gCurTask;
        v->unk70 = -*(s8 *)&p->unk16;
        v->unk48 = v->unk20;
        v->unk4A = v->unk1C;
        k = *(s16 *)&v->unk70 - 3;
        if (v->unk72 == 1)
        {
            ActorSetTerrainBox(gUnk_0873F8BC);
            w = gCurTask;
            w->unk48 += gUnk_0873EAD8[k][2] * w->unk43;
            w->unk4A += gUnk_0873EAD8[k][3];
        }
        else
        {
            ActorSetTerrainBox(gUnk_0873F8B4);
            w = gCurTask;
            w->unk48 += gUnk_0873EAD8[k][0] * w->unk43;
            w->unk4A += gUnk_0873EAD8[k][1];
        }
        x = gCurTask;
        x->unk4C = x->unk48 << 16;
        x->unk50 = x->unk4A << 16;
        if (sub_08021a40(x->unk48, x->unk4A) != 0)
        {
            y = gCurTask;
            if (CanBreakBlock(y->unk48 >> 4, y->unk4A >> 4, 3, -1) == 0)
            {
                sub_0806b8bc();
                return;
            }
        }
        ActorSetState(6);
        TaskSetEntry(sub_0806bf38, gCurTaskIdx);
        return;
    }
    n = *(s8 *)&p->unk16 * 5;
    g = (gUnk_0873E7C4[n] + u->unk34) * s->unk43;
    if (e == 1)
        h = gUnk_0873E7C4[n + 1] - u->unk30;
    else
        h = gUnk_0873E7C4[n + 1] + u->unk30;
    e = gUnk_0873E7C4[n + 2];
    b = gUnk_0873E7C4[n + 4];
    z = gCurTask;
    z->unk48 += g;
    z->unk4A += h;
    z->unk20 = z->unk48;
    z->unk1C = z->unk4A;
    TaskSetFrame((s16)(e + 2));
    gCurTask->unk42 = b;
}

void sub_0806b670(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *z;
    struct PlayerState *p;
    struct Task *s;
    s16 e;
    s16 n;
    s16 g;
    s16 h;
    u8 b;

    t = gCurTask;
    s = &gTasks[t->unk44];
    p = t->unk88;
    if (*(s8 *)&p->unk16 == -1)
    {
        t->unk48 = t->unk20;
        t->unk4A = t->unk1C;
        sub_0806b8bc();
        return;
    }
    if (*(s8 *)&p->unk16 == -2)
    {
        t->unk48 = t->unk20;
        t->unk4A = t->unk1C;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (sub_08021a40(t->unk48, t->unk4A) != 0)
        {
            u = gCurTask;
            if (CanBreakBlock(u->unk48 >> 4, u->unk4A >> 4, 3, -1) == 0)
            {
                sub_0806b8bc();
                return;
            }
            v = gCurTask;
            v->unk43 = s->unk43;
        }
        else
        {
            w = gCurTask;
            w->unk43 = s->unk43;
        }
        ActorSetState(2);
        TaskSetEntry(sub_0806bf38, gCurTaskIdx);
        return;
    }
    n = *(s8 *)&p->unk16 * 5;
    g = (gUnk_0873E864[n] + t->unk34) * s->unk43;
    if (e == 1)
        h = gUnk_0873E864[n + 1] - t->unk30;
    else
        h = gUnk_0873E864[n + 1] + t->unk30;
    e = gUnk_0873E864[n + 2];
    b = gUnk_0873E864[n + 4];
    z = gCurTask;
    z->unk48 += g;
    z->unk4A += h;
    z->unk20 = z->unk48;
    z->unk1C = z->unk4A;
    TaskSetFrame((s16)(e + 2));
    gCurTask->unk42 = b;
    if (gUnk_0873E864[n + 3] == 1)
    {
        x = gCurTask;
        if (x->unk43 == 1)
            x->unk3E |= 0x8000;
        else
            x->unk3E &= 0x7FFF;
    }
}

void sub_0806b848(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    p = t->unk88;
    t->unk48 += *(s16 *)&p->unk24 >> 8;
    t->unk20 = t->unk48;
    t->unk1C = t->unk4A;
}

void sub_0806b878(void)
{
    struct Task *t;
    u32 m;
    u32 n;
    u32 v;
    u32 w;
    u32 q;

    t = gCurTask;
    v = t->unk40;
    m = 0xF000;
    q = t->unk8C->unk22;
    m &= v;
    if (m == q)
        return;
    if (--t->unk2C >= 0)
        return;
    w = t->unk40;
    n = 0xFFF;
    n &= w;
    t->unk40 = n | t->unk8C->unk22;
}

void sub_0806b8bc(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gCurTask;
    if (t->unk72 == 0)
    {
        v = t->unk76;
        if (v == 17 || v == 9 || v == 0 || v == 31 || v == 32)
            gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    }
    u = gCurTask;
    u->unk00 = (u32)TaskMove;
    u->unk82 = 0;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_0806b938(void)
{
    struct PlayerState *p;
    u8 z;

    p = gCurTask->unk88;
    if (p->unk04 == 10)
        return;
    z = 0;
    p->unk08 = z;
    p->unk07 = z;
    sub_0806b8bc();
}

void sub_0806b95c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    struct Actor *c;
    struct PlayerState *p;
    u32 m;
    u32 w;

    t = gCurTask;
    a = t->unk8C;
    p = t->unk88;
    if (t->unk72 != 0 || t->unk76 != 40)
        p->unk09 = 1;
    u = gCurTask;
    if (u->unk0C == (u32)sub_08065640)
        u->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    if (gCurTask->unk72 != 1)
        ActorSetHitReactions(gUnk_0873F92C);
    v = gCurTask;
    c = v->unk8C;
    w = v->unk40;
    m = 0xF000;
    m &= w;
    c->unk22 = m;
    v->unk2C = 2;
    a->unk04 = v->unk82;
}

void sub_0806b9dc(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    struct Task *s;
    t = gCurTask;
    t->unk44 = t->unk7E;
    t->unk88 = p = &gPlayerStates[t->unk44];
    p->unk07++;
    u = gCurTask;
    s = &gTasks[u->unk44];
    u->unk28 = s->unk43;
}

void sub_0806ba34(void)
{
    struct Task *t;
    struct Actor *a;
    u32 v;
    u32 m;
    u32 z;

    t = gCurTask;
    a = t->unk8C;
    v = t->unk40;
    z = v >> 12;
    if (a->unk28 != 0)
        sub_08065d44(z, t->unk76, a->unk0C, a->unk24, t->unk72, a->unk28);
    else if (t->unk72 == 0 || t->unk72 == 3)
    {
        m = 0xFFF;
        m &= v;
        t->unk40 = m | a->unk22;
    }
    a->unk0A |= 1;
}

void sub_0806ba9c(void)
{
    struct Task *t;
    struct Task *s;

    t = gCurTask;
    s = &gTasks[t->unk44];
    t->unk4C = (t->unk48 - s->unk48) << 16;
    t->unk50 = (t->unk4A - s->unk4A) << 16;
}

s32 sub_0806baec(s32 a)
{
    if (a > abs(TaskGetDxTo(gCurTask->unk44)))
        return 1;
    return 0;
}

void sub_0806bb34(s32 a)
{
    struct Task *t;
    u32 m;
    u32 w;

    switch (a)
    {
    case 0:
        TaskSetFrame(1);
        break;
    case 1:
    case 4:
        TaskSetFrame(2);
        break;
    }
    t = gCurTask;
    w = t->unk40;
    m = 0xFFF;
    m &= w;
    t->unk40 = m | 0xF000;
}

s32 sub_0806bb7c(void)
{
    sub_0806b9dc();
    sub_0806ba34();
    switch (gCurTask->unk82)
    {
    case 1:
        sub_0806b95c();
        ActorSetState(0);
        break;
    case 2:
        sub_0806b95c();
        sub_0806bc54();
        ActorSetState(4);
        break;
    case 3:
        sub_0806b95c();
        sub_0806bc54();
        ActorSetState(1);
        break;
    }
    TaskSetEntry(sub_0806befc, gCurTaskIdx);
    return 1;
}

void sub_0806bbe8(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;

    t = gCurTask;
    if (t->unk4C <= 0)
        t->unk5C = 10752;
    else
        t->unk5C = -10752;
    u = gCurTask;
    v = u->unk50;
    if (v < 0)
        v = -v;
    v >>= 3;
    if (u->unk50 <= 0)
        u->unk58 = v;
    else
        u->unk58 = -v;
}

void sub_0806bc28(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    p = t->unk88;
    if (*(s8 *)&p->unk08 != 0)
    {
        t->unk18 = 1;
        TaskStop();
    }
    else
    {
        sub_0806bbe8();
    }
}

void sub_0806bc54(void)
{
    struct Task *t;
    s16 n;

    t = gCurTask;
    if (t->unk72 == 1)
    {
        n = t->unk76 * 2;
        t->unk34 = gUnk_0873E7A4[n];
        t->unk30 = gUnk_0873E7A4[n + 1];
    }
    else
    {
        t->unk30 = 0;
        t->unk34 = 0;
    }
}

void sub_0806bc9c(void)
{
    struct Task *t;
    struct PlayerState *p;
    u8 one;

    t = gCurTask;
    p = t->unk88;
    ActorAwardScore(t->unk44, 1);
    if (*(s8 *)&p->unk08 != 0)
    {
        gCurTask->unk18 = 1;
        TaskStop();
    }
    else
    {
        one = 1;
        p->unk08 = one;
        p->unk07 = one;
        TaskStop();
    }
}

void sub_0806bcdc(void)
{
    sub_0806bbe8();
    if (*(s16 *)&gCurTask->unk70 != 0)
        return;
    if (sub_0806baec(18) == 0)
        return;
    sub_0806bd10();
    gCurTask->unk70 = 1;
}

void sub_0806bd10(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;
    struct PlayerState *p;
    u32 f;
    u8 q;

    t = gCurTask;
    a = t->unk8C;
    p = t->unk88;
    if ((s8)a->unk44->unk0D == 1)
    {
        if (*(s8 *)&p->unk07 == 1 && t->unk72 == 6 && t->unk76 != 0)
        {
            p->unk09 = 3;
            gCurTask->unk30 = *(s8 *)&p->unk08;
            p->unk08++;
        }
        else if ((s8)p->unk07 > 0)
        {
            p->unk07--;
        }
    }
    else
    {
        t->unk30 = *(s8 *)&p->unk08;
        p->unk08++;
        p->unk06 = 1;
    }
    p->unk31 = 0;
    if (*(s8 *)&a->unk00 == 0)
        return;
    u = gCurTask;
    if (u->unk72 == 6)
    {
        f = u->unk76;
        if (f == 0)
        {
            p->unk0B = u->unk18;
            v = gCurTask;
            p->unk0C = v->unk1C;
            w = gCurTask;
            if (w->unk20 == w->unk44)
                p->unk31 = 1;
            if (gUnk_0300244C == 0)
                return;
            p->unk0A = f;
            gUnk_02007CF4[p->unk00] = 1;
            return;
        }
    }
    if (gUnk_0300244C == 0 || gUnk_02007CF4[p->unk00] != 1)
        p->unk0A++;
    if (*(s8 *)&p->unk0B != 0)
        return;
    p->unk0B = a->unk00;
    switch ((s8)a->unk00)
    {
    case 7:
        q = 3;
        break;
    case 11:
    case 20:
    case 21:
        q = 1;
        break;
    default:
        q = 255;
        break;
    }
    p->unk0C = q;
}

void sub_0806be4c(u32 i)
{
    struct Task *s;
    struct Actor *a;
    struct PlayerState *p;

    s = &gTasks[i];
    a = s->unk8C;
    p = s->unk88;
    if (a->unk04 == 1)
    {
        if (*(s8 *)&p->unk08 != 0)
        {
            p->unk07 = p->unk08;
        }
        else
        {
            p->unk0A = 0;
            p->unk09 = 0;
            p->unk08 = 0;
            p->unk07 = 0;
        }
    }
    else
    {
        p->unk0A = 0;
        p->unk09 = 0;
        p->unk08 = 0;
        p->unk07 = 0;
    }
}

u8 sub_0806be84(void)
{
    struct Task *t;
    u32 m;
    u32 w;

    if (gUnk_02006178 == 1)
    {
        sub_0806be4c(gCurTaskIdx);
        t = gCurTask;
        w = t->unk40;
        m = 0xFFF;
        m &= w;
        t->unk40 = m | t->unk8C->unk22;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (t->unk72 != 1 && t->unk72 != 6)
            ActorSetHitReactions(gUnk_0873F938);
        sub_0806b8bc();
    }
    return gUnk_02006178;
}

void sub_0806befc(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    gCurTask->unk7A = 0;
    sub_0806bb34(gCurTask->unk14);
    CallTableEntry(gCurTask->unk14, 8, gUnk_0873EAA0);
    TaskSleepForever();
}

void sub_0806bf38(void)
{
    CallTableEntry(gCurTask->unk14, 8, gUnk_0873EAA0);
}

void sub_0806bf54(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk08 = (u32)sub_0806bfd8;
    ActorSetTerrainHandlers(gUnk_0873F8F4);
    sub_0806ba9c();
    u = gCurTask;
    u->unk46 = 0;
    u->unk70 = 0;
    u->unk30 = 0;
    u->unk42 = 6;
    while (sub_0806baec(16) == 0)
    {
        sub_0806bcdc();
        TaskYieldTrampoline(1);
    }
    if (*(s16 *)&gCurTask->unk70 == 0)
    {
        sub_0806bd10();
        gCurTask->unk70 = 1;
    }
    gCurTask->unk46 = 1;
    TaskSleepForever();
}

void sub_0806bfd8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk46 != 0)
    {
        if (t->unk72 == 0)
            ActorAwardScore(t->unk44, 1 << t->unk30);
        else
            ActorAwardScore(t->unk44, 1);
        u = gCurTask;
        if (u->unk72 != 1)
        {
            if (u->unk72 == 6)
            {
                sub_0806a158();
                return;
            }
        }
        ActorDestroy();
        return;
    }
    if (*(s16 *)&t->unk70 != 0)
        return;
    if (sub_0806be84() != 0)
        return;
    sub_0806b938();
    sub_0806b878();
}

void sub_0806c05c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk04 = (u32)sub_0806c0c0;
    t->unk08 = (u32)sub_0806c148;
    t->unk50 = 0;
    t->unk4C = 0;
    t->unk7C = 0;
    u = gCurTask;
    u->unk20 = u->unk48;
    u->unk1C = u->unk4A;
    u->unk12 = 1;
    ActorSetTerrainHandlers(gUnk_0873F8F4);
    gCurTask->unk18 = 0;
    sub_0806bc9c();
    TaskSleepForever();
}

void sub_0806c0c0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk78 = 127;
    if (t->unk18 == 1)
        sub_0806b8bc();
    else
        sub_0806b670();
    sub_0806b878();
    u = gCurTask;
    if (u->unk72 == 1)
        RegisterCollider((u8)gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873F830);
}

void sub_0806c148(void)
{
    sub_0806b848();
    sub_0806be84();
}

void sub_0806c158(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk04 = (u32)sub_0806c1d0;
    t->unk08 = 0;
    if (gUnk_0300244C != 0 && t->unk8C->unk50 == 0)
        ActorSetTerrainBox(gUnk_0873F894);
    TaskSetMotionXFacing(0x38000, 0x5A5A5A5A);
    TaskSetMotionY(0x30000, 0x8000, 0x60000);
    gCurTask->unk7C = 0;
    gCurTask->unk80 = 0;
    TaskSleepForever();
}

void sub_0806c1d0(void)
{
    struct Task *t;

    gCurTask->unk78 = 127;
    ActorCollideTerrain();
    if ((*(u32 *)gTerrainResult & 0xFFFFFF) != 0)
    {
        if (gTerrainResult[1] != 0)
            gCurTask->unk30 = 0;
        if (gTerrainResult[2] != 0)
            gCurTask->unk30 = 1;
        if (gTerrainResult[0] != 0)
            gCurTask->unk30 = 2;
        PlaySfx(179);
        RequestScreenShake(2);
        ActorSetState(3);
        TaskSetEntry(sub_0806bf38, gCurTaskIdx);
        return;
    }
    t = gCurTask;
    if (t->unk72 == 1)
        RegisterCollider((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F830);
}
