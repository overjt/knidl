/* game_code_and_rodata 0x08063698-0x08063E14 (issue #65, module M17 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08063698 0x08063E14 src/actor_63698.c --newpb
 *
 * Actor lifecycle + "nearest other task" queries: binding a task to its ROM
 * descriptor (sub_08063704/sub_0806391c), resetting the actor record
 * (sub_080637E4), and the distance helpers every later module calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02006178;
extern u8 gUnk_0200D090[][64];
extern u32 gUnk_02007F50;
extern u16 gUnk_02000008;
extern u8 gUnk_03002340;
extern vs16 gUnk_03004CA0[];
extern u16 gUnk_030023AC;
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

extern struct ActorDef *gUnk_0873ECEC[];
extern struct ActorDef *gUnk_0873ED90[];
extern struct ActorDef *gUnk_0873EDB8[];
extern struct ActorDef *gUnk_0873EDDC[];
extern struct ActorDef *gUnk_0873EE70[];
extern struct ActorDef *gUnk_0873EE88[];

extern s32 sub_08005904(u32 type, s32 start, s32 end);
extern void sub_08065ce0(u32 i);
extern u16 sub_08065f5c(u32 a);
extern u16 sub_08065f74(u32 i);
extern void sub_08069ac4(u32 i);
extern void sub_080b54a4(u32 i);
extern void sub_08005654(s32 id);
extern void sub_08006338(s32 a);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void TaskDispatchTrampoline(void);
extern s32 gUnk_030026F4;
extern s16 gUnk_0872FB30[];
extern u8 gUnk_03002350;
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s16 gUnk_03002158[];
extern u16 gUnk_030023D8;
extern u16 gUnk_03001EA4;
extern s32 gUnk_0873DF14[];
extern struct Actor gUnk_0200C320[];
extern struct PlayerState gUnk_03002170[];
extern u32 gUnk_0873F198[];
extern u32 gUnk_0873F23C[];
extern u32 gUnk_0873F264[];
extern u32 gUnk_0873F288[];
extern u32 gUnk_0873F2A0[];
extern void sub_0800a04c(s32 a, u32 b);
extern s32 sub_08021a40(s16 x, s16 y);

void sub_0806391c(u32 i, struct ActorDef *d);
void sub_080639c8(u32 i, u32 v);
void sub_08063a14(u32 i, u32 v);
void sub_08063704(u32 i);
void sub_080637ac(u32 i);
void sub_080637e4(u32 i);
s32 sub_08063a9c(u32 i);
s32 sub_08063b38(void);
s32 sub_08063bd4(struct PointPair *p);
s32 sub_08063bf4(u32 i, u32 j);
s32 sub_08063c60(u32 i);
s32 sub_08063c94(u32 i, u32 j);
s32 sub_08063cbc(u32 i);
s32 sub_08063cf0(u32 i, u32 j);
s32 sub_08063d18(u32 i);
void sub_08063d4c(u32 i);
s32 sub_08063dac(u32 i);
s32 sub_08063df4(void);
s32 sub_08063eb0(struct Rect *r, u32 i);
void sub_08063f24(s32 i);
void sub_08063e14(void);
s32 sub_080640fc(void);
u16 sub_0806425c(s16 x0, s16 y0, s16 x1, s16 y1, s32 prec);
u16 sub_080642b0(u32 i, u32 j, s32 prec);
u16 sub_080642fc(u32 i, s32 prec);
u8 sub_0806433c(u32 i);
u8 sub_0806437c(u32 i);
u8 sub_080643bc(u32 i);
u16 sub_08064314(s32 prec);
void sub_08064460(s32 step, s32 limit, u8 axis);
void sub_080644c0(s32 step, s32 limit, u8 axis);
void sub_0806453c(s32 step, u8 axis);
void sub_080648a0(u32 i);
s8 sub_08064a38(void);
void sub_080636e4(u32 i);
s32 sub_08064a78(struct ActorSpawn *p);
s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
s32 sub_08064d34(u32 type, u8 keepPrio);
s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg, int prioArg,
                 int altArg);
s32 sub_08064e5c(u32 sub, u32 type, u8 p2);
s32 sub_08064f28(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio);

s32 sub_08063698(u32 type, s32 start)
{
    s32 i;
    struct Task *t;

    i = sub_08005904(type, start, 62);
    if (i == -1)
        return i;
    t = &gUnk_03002790[i];
    if (gUnk_02006178 != 1)
        return i;
    gUnk_0200D090[0][i] = gUnk_0200D090[1][i] = 0;
    t->unk13 = 15;
    return i;
}

void sub_080636e4(u32 i)
{
    sub_08063704(i);
    sub_080637e4(i);
    sub_08063a9c(i);
    sub_08069ac4(i);
}

void sub_08063704(u32 i)
{
    struct Task *t;
    struct Actor *a;

    t = &gUnk_03002790[i];
    a = t->unk8C;
    a->unk44 = NULL;
    switch (t->unk72)
    {
    case 0:
        a->unk44 = gUnk_0873ECEC[t->unk76];
        break;
    case 1:
    case 3:
        a->unk44 = gUnk_0873ED90[t->unk76];
        break;
    case 2:
        a->unk44 = gUnk_0873EDB8[t->unk76];
        gUnk_02007F50 = t->unk76;
        break;
    case 4:
        a->unk44 = gUnk_0873EDDC[t->unk76];
        break;
    case 5:
        a->unk44 = gUnk_0873EE70[t->unk76];
        break;
    default:
        a->unk44 = gUnk_0873EE88[t->unk76];
        break;
    }
}

void sub_080637ac(u32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    t->unk78 = sub_08065f74(i);
}

void sub_080637cc(u32 a)
{
    gUnk_03002490->unk78 = sub_08065f5c(a);
}

void sub_080637e4(u32 i)
{
    struct Task *t;
    struct Actor *a;
    struct ActorDef *d;

    t = &gUnk_03002790[i];
    a = t->unk8C;
    a->unk1C = 0xFFFF;
    a->unk04 = 0;
    a->unk05 = 0;
    a->unk06 = 0;
    a->unk07 = 0;
    a->unk2C = 0;
    a->unk01 = 0;
    a->unk08 = 0;
    a->unk09 = 0;
    a->unk0A = 0;
    a->unk0B = 0;
    a->unk0E = -1;
    a->unk10 = -1;
    a->unk12 = 0xFFFE;
    a->unk22 = t->unk40 & 0xF000;
    a->unk24 = 16;
    a->unk28 = 0;
    a->unk0C = t->unk73 >> 4;
    t->unk73 = t->unk73 & 15;
    if (a->unk0C != 0)
        sub_08065ce0(i);
    t->unk16 = gUnk_02000008;
    if (gUnk_02000008 > 0xFFFE)
        gUnk_02000008 = 0;
    else
        gUnk_02000008 = gUnk_02000008 + 1;
    a->unk3C = 0;
    a->unk34 = -1;
    a->unk38 = -1;
    a->unk0D = 0;
    a->unk18 = 0;
    a->unk16 = 0;
    a->unk03 = 0;
    a->unk1E = 0;
    a->unk1A = 0xFFFF;
    a->unk02 = 0;
    d = a->unk44;
    if (d != NULL)
    {
        a->unk48 = d->unk14;
        a->unk4C = 0;
        a->unk50 = d->unk18;
        a->unk58 = a->unk54 = d->unk1C;
        a->unk5C = d->unk20;
        a->unk00 = d->unk0C;
        a->unk30 = d->unk08;
        sub_080637ac(i);
        if (a->unk44->unk24 != NULL)
            a->unk44->unk24(i);
        a->unk40 = a->unk44->unk28;
        a->unk60 = a->unk44->unk10;
    }
    else
    {
        a->unk48 = 0;
        a->unk4C = 0;
        a->unk50 = 0;
        a->unk54 = 0;
        a->unk58 = 0;
        a->unk5C = 0;
        a->unk00 = 0;
        a->unk30 = 0;
        t->unk78 = 0;
        a->unk60 = 0;
        a->unk40 = 0;
    }
}

void sub_08063908(struct ActorDef *d)
{
    sub_0806391c(gCurTaskIdx, d);
}

void sub_0806391c(u32 i, struct ActorDef *d)
{
    struct Actor *a;

    a = (&gUnk_03002790[i])->unk8C;
    a->unk44 = d;
    a->unk48 = d->unk14;
    a->unk50 = d->unk18;
    a->unk58 = a->unk54;
    a->unk54 = d->unk1C;
    a->unk5C = d->unk20;
    a->unk00 = d->unk0C;
    a->unk30 = d->unk08;
    a->unk40 = d->unk28;
    sub_080637ac(i);
}

void sub_0806395c(u8 v)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk8C->unk1C = t->unk14;
    t->unk14 = v;
}

void sub_08063974(u32 i, u8 v)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    t->unk8C->unk1C = t->unk14;
    t->unk14 = v;
}

void sub_08063990(u32 v)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    a->unk58 = a->unk54;
    a->unk54 = v;
}

void sub_080639a4(u32 v)
{
    gUnk_03002490->unk8C->unk5C = v;
}

void sub_080639b4(u32 v)
{
    sub_080639c8(gCurTaskIdx, v);
}

void sub_080639c8(u32 i, u32 v)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    t->unk8C->unk48 = v;
}

void sub_080639e0(u32 v)
{
    gUnk_03002490->unk8C->unk50 = v;
}

void sub_080639f0(u32 v)
{
    gUnk_03002490->unk8C->unk60 = v;
}

void sub_08063a00(u32 v)
{
    sub_08063a14(gCurTaskIdx, v);
}

void sub_08063a14(u32 i, u32 v)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    t->unk8C->unk4C = v;
}

/* Nearest task in slots 4..15 to the running one, along X. */
/* Nearest task in slots 4..15 to the running one, along X. */
s32 sub_08063a2c(void)
{
    struct Task *o;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 i;

    best = -1;
    bestDist = 0;
    for (i = 4; i <= 15; i++)
    {
        if (gUnk_03004CA0[i] != -1)
        {
            o = &gUnk_03002790[i];
            d = gUnk_03002490->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - gUnk_03002490->unk48;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = i;
            }
        }
    }
    return best;
}

/* Same, but over the active-player mask, relative to task `i`. */
s32 sub_08063a9c(u32 i)
{
    struct Task *t;
    struct Task *o;
    struct Task *bestPtr;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 j;

    best = -1;
    bestDist = 0;
    t = &gUnk_03002790[i];
    for (j = 0; j < gUnk_030023AC; j++)
    {
        if ((gUnk_03002340 >> j) & 1)
        {
            o = &gUnk_03002790[j];
            d = t->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - t->unk48;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    t->unk80 = best;
    t->unk88 = bestPtr;
    return best;
}

s32 sub_08063b38(void)
{
    struct Task *o;
    struct Task *bestPtr;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 j;

    best = -1;
    bestDist = 0;
    for (j = 0; j < gUnk_030023AC; j++)
    {
        if ((gUnk_03002340 >> j) & 1)
        {
            o = &gUnk_03002790[j];
            d = gUnk_03002490->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - gUnk_03002490->unk48;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    gUnk_03002490->unk80 = best;
    gUnk_03002490->unk88 = bestPtr;
    return best;
}

s32 sub_08063bd4(struct PointPair *p)
{
    s16 dx;
    s16 dy;

    dx = p->x0 - p->x1;
    dy = p->y0 - p->y1;
    return dx * dx + dy * dy;
}

s32 sub_08063bf4(u32 i, u32 j)
{
    struct PointPair p;
    struct Task *a;
    struct Task *b;

    a = &gUnk_03002790[i];
    b = &gUnk_03002790[j];
    p.x1 = a->unk48;
    p.y1 = a->unk4A;
    p.x0 = b->unk48;
    p.y0 = b->unk4A;
    return sub_08063bd4(&p);
}

s32 sub_08063c60(u32 i)
{
    return sub_08063bf4(i, gCurTaskIdx);
}

s32 sub_08063c74(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_08063c60(i);
}

s32 sub_08063c94(u32 i, u32 j)
{
    struct Task *a;
    struct Task *b;

    a = &gUnk_03002790[i];
    b = &gUnk_03002790[j];
    return a->unk48 - b->unk48;
}

s32 sub_08063cbc(u32 i)
{
    return sub_08063c94(i, gCurTaskIdx);
}

s32 sub_08063cd0(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_08063cbc(i);
}

s32 sub_08063cf0(u32 i, u32 j)
{
    struct Task *a;
    struct Task *b;

    a = &gUnk_03002790[i];
    b = &gUnk_03002790[j];
    return a->unk4A - b->unk4A;
}

s32 sub_08063d18(u32 i)
{
    return sub_08063cf0(i, gCurTaskIdx);
}

s32 sub_08063d2c(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_08063d18(i);
}

void sub_08063d4c(u32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    gUnk_030023B4 = t->unk48;
    gUnk_030023D4 = t->unk4A;
}

void sub_08063d7c(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
    {
        gUnk_030023B4 = gUnk_030023D4 = 0;
    }
    else
    {
        sub_08063d4c(i);
    }
}

s32 sub_08063dac(u32 i)
{
    struct Task *t;
    s32 d;
    s32 v;

    t = &gUnk_03002790[i];
    d = t->unk48 - gUnk_03002490->unk48;
    v = 1;
    if (d < 0)
        v = -1;
    return v;
}

void sub_08063ddc(u32 i)
{
    gUnk_03002490->unk43 = sub_08063dac(i);
}

s32 sub_08063df4(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_08063dac(i);
}

void sub_08063e14(void)
{
    gUnk_03002490->unk43 = sub_08063df4();
}

u8 sub_08063e2c(struct Rect *r)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk48 > r->left && t->unk48 < r->right
        && t->unk4A > r->top && t->unk4A < r->bottom)
        return 1;
    return 0;
}

u8 sub_08063e74(struct Rect *r, s16 x, u16 y)
{
    if (x > r->left && x < r->right
        && (s16)y > r->top && (s16)y < r->bottom)
        return 1;
    return 0;
}

s32 sub_08063eb0(struct Rect *r, u32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    if (t->unk48 > r->left && t->unk48 < r->right
        && t->unk4A > r->top && t->unk4A < r->bottom)
        return 1;
    return 0;
}

s32 sub_08063f00(struct Rect *r)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_08063eb0(r, i);
}

/* Tear down task `i`: run the actor's teardown hook, release its child task
   and its slot, then either re-dispatch (if it killed itself) or free it. */
void sub_08063f24(s32 i)
{
    struct Task *t;
    struct Actor *a;

    if (i <= 31)
        return;
    t = &gUnk_03002790[i];
    a = t->unk8C;
    if (a != NULL && (u8)(t->unk72 - 7) > 3)
    {
        if (a->unk40 != NULL)
            a->unk40();
        if (a->unk10 != -1)
        {
            sub_08005654(a->unk10);
            a->unk10 = 0xFFFF;
        }
        switch (t->unk72)
        {
        case 0:
            sub_080b54a4(i);
            break;
        case 6:
            if (gUnk_03002490->unk76 != 0)
                sub_080b54a4(i);
            break;
        }
    }
    gUnk_0200D090[0][i] = gUnk_0200D090[1][i] = 0;
    if (i == gCurTaskIdx && gUnk_030026F4 == 1)
        TaskDispatchTrampoline();
    else
        sub_08005654(i);
}

void sub_08063fe0(void)
{
    sub_08063f24(gCurTaskIdx);
}

void sub_08063ff4(void)
{
    if (gUnk_03002490->unk43 == 1)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_08006338(gUnk_03002490->unk3C);
    sub_080061a8(-gUnk_03002490->unk54, -gUnk_03002490->unk5C,
                 gUnk_03002490->unk64);
}

void sub_08064038(void)
{
    if (gUnk_03002490->unk43 == 1)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_08006338(gUnk_03002490->unk3C);
}

void sub_0806406c(void)
{
    if (gUnk_03002490->unk43 == 1)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_080061a8(-gUnk_03002490->unk54, -gUnk_03002490->unk5C,
                 gUnk_03002490->unk64);
}

s32 sub_080640a8(struct AnimCmd *p)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    a->unk2C = p;
    a->unk09 = 0;
    a->unk08 = 1;
    return sub_080640fc();
}

void sub_080640c8(void)
{
    gUnk_03002490->unk8C->unk2C = NULL;
}

s32 sub_080640dc(struct AnimCmd *p)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    a->unk2C = p;
    a->unk09 = 0;
    a->unk08 = 0;
    return sub_080640fc();
}

/* Step the running actor's animation script; returns the new delay. */
/* Step the running actor's animation script; returns the new delay. */
/* Step the running actor's animation script; returns the new delay. */
s32 sub_080640fc(void)
{
    struct Actor *a;
    struct AnimCmd *p;
    s32 delay;
    s32 cmd;

    a = gUnk_03002490->unk8C;
    p = a->unk2C;
    p += a->unk09;
    cmd = p->unk00;
    if (cmd == -3)
    {
        a->unk09 = 0;
        p = a->unk2C;
    }
    else if (cmd == -2)
    {
        delay = cmd;
        a->unk2C = NULL;
        a->unk09++;
        return delay;
    }
    if (a->unk08 != 0)
        gUnk_03002490->unk3C = p->unk00;
    else
        sub_08006338(p->unk00);
    delay = p->unk02;
    a->unk09++;
    return delay;
}

s32 sub_0806415c(s32 n)
{
    if (gUnk_03002490->unk8C->unk2C != NULL)
    {
        if (n <= 0)
        {
            sub_08063e14();
            n = sub_080640fc();
        }
        n--;
    }
    return n;
}

s32 sub_08064188(s32 n)
{
    if (gUnk_03002490->unk8C->unk2C != NULL)
    {
        if (n <= 0)
            n = sub_080640fc();
        n--;
    }
    return n;
}

/* Dead export: split `mag` into the trig-table components of the angle from
   (x0, y0) to (x1, y1) and leave them in gUnk_030023B4/gUnk_030023D4. */
u32 sub_080641b0(s16 x0, s16 y0, s16 x1, s16 y1, u16 mag)
{
    u32 t;

    t = (u16)ArcTan2(x1 - x0, y1 - y0) >> 7;
    gUnk_030023B4 = *(gUnk_0872FB30 + t) * (s16)mag;
    gUnk_030023D4 = *(gUnk_0872FB30 - 128 + t) * (s16)mag;
    return t;
}

void sub_0806421c(s16 t, s16 mag)
{
    gUnk_030023B4 = *(gUnk_0872FB30 + t) * mag;
    gUnk_030023D4 = *(gUnk_0872FB30 - 128 + t) * mag;
}

/* Angle from (x0, y0) to (x1, y1), narrowed to `prec` steps of resolution. */
u16 sub_0806425c(s16 x0, s16 y0, s16 x1, s16 y1, s32 prec)
{
    u16 a;

    a = ArcTan2(x1 - x0, y1 - y0);
    switch (prec)
    {
    case 0:
        a >>= 1;
    case 1:
        a >>= 4;
    case 2:
        a >>= 1;
    case 3:
        a >>= 7;
    }
    return a;
}

u16 sub_080642b0(u32 i, u32 j, s32 prec)
{
    struct Task *a;
    struct Task *b;

    a = &gUnk_03002790[i];
    b = &gUnk_03002790[j];
    return sub_0806425c(b->unk48, b->unk4A, a->unk48, a->unk4A, prec);
}

u16 sub_080642fc(u32 i, s32 prec)
{
    return sub_080642b0(i, gCurTaskIdx, prec);
}

u16 sub_08064314(s32 prec)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_080642fc(i, prec);
}

u8 sub_0806433c(u32 i)
{
    s32 d;

    d = sub_08063d18(i);
    if (d == 0)
        return 0;
    if (d < 0)
        return 2;
    return 1;
}

u8 sub_08064358(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_0806433c(i);
}

u8 sub_0806437c(u32 i)
{
    s32 d;

    d = sub_08063cbc(i);
    if (d == 0)
        return 0;
    if (d < 0)
        return 8;
    return 4;
}

u8 sub_08064398(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_0806437c(i);
}

/* Combine the two axis signs into one of the 8 compass directions. */
u8 sub_080643bc(u32 i)
{
    u8 dir;

    dir = sub_0806433c(i);
    dir |= sub_0806437c(i);
    switch (dir)
    {
    case 5:
        dir = 1;
        break;
    case 9:
        dir = 2;
        break;
    case 6:
        dir = 0;
        break;
    case 10:
        dir = 3;
        break;
    case 1:
        dir = 5;
        break;
    case 2:
        dir = 4;
        break;
    case 4:
        dir = 7;
        break;
    case 8:
        dir = 6;
        break;
    default:
        dir = 8;
        break;
    }
    return dir;
}

u8 sub_0806443c(void)
{
    s32 i;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    return sub_080643bc(i);
}

/* Accelerate the running task towards +limit on the given axis. */
/* Accelerate the running task towards +limit on the given axis. */
void sub_08064460(s32 step, s32 limit, u8 axis)
{
    struct Task *t;

    if (axis == 0)
    {
        t = gUnk_03002490;
        if (t->unk54 < 0)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else if (t->unk54 < limit)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->unk54 = limit;
        }
    }
    else
    {
        t = gUnk_03002490;
        if (t->unk58 < 0 || t->unk58 < limit)
        {
            t->unk58 += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->unk58 = limit;
        }
    }
}

/* Accelerate the running task towards -limit on the given axis. */
/* Accelerate the running task towards -limit on the given axis. */
void sub_080644c0(s32 step, s32 limit, u8 axis)
{
    struct Task *t;
    s32 v;

    if (axis == 0)
    {
        t = gUnk_03002490;
        if (t->unk54 > 0)
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
        else if (abs(t->unk54) < limit)
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
        else
        {
            t->unk54 = -limit;
        }
    }
    else
    {
        t = gUnk_03002490;
        v = t->unk58;
        if (v > 0 || abs(v) < limit)
        {
            t->unk58 = v - step;
            gUnk_030023D4 = -1;
        }
        else
        {
            t->unk58 = -limit;
        }
    }
}

/* Decay the running task's speed towards zero on the given axis. */
void sub_0806453c(s32 step, u8 axis)
{
    struct Task *t;

    if (axis == 0)
    {
        t = gUnk_03002490;
        if (t->unk54 == 0)
            return;
        if (t->unk54 < 0)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
    }
    else
    {
        t = gUnk_03002490;
        if (t->unk58 == 0)
            return;
        if (t->unk58 < 0)
        {
            t->unk58 += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->unk58 -= step;
            gUnk_030023D4 = -1;
        }
    }
}

/* Steer the running task towards the nearest player. */
void sub_080645a4(s32 step, s32 limit)
{
    gUnk_030023B4 = gUnk_030023D4 = 0;
    switch (sub_08064314(0))
    {
    case 0:
        sub_08064460(step, limit, 0);
        sub_0806453c(step, 1);
        break;
    case 1:
        sub_08064460(step, limit, 0);
        sub_08064460(step, limit, 1);
        break;
    case 2:
        sub_0806453c(step, 0);
        sub_08064460(step, limit, 1);
        break;
    case 3:
        sub_080644c0(step, limit, 0);
        sub_08064460(step, limit, 1);
        break;
    case 4:
        sub_080644c0(step, limit, 0);
        sub_0806453c(step, 1);
        break;
    case 5:
        sub_080644c0(step, limit, 0);
        sub_080644c0(step, limit, 1);
        break;
    case 6:
        sub_0806453c(step, 0);
        sub_080644c0(step, limit, 1);
        break;
    case 7:
        sub_08064460(step, limit, 0);
        sub_080644c0(step, limit, 1);
        break;
    }
}

/* sub_080645A4 with the direction supplied by the caller. */
void sub_08064680(s32 step, s32 limit, u16 dir)
{
    gUnk_030023B4 = gUnk_030023D4 = 0;
    switch (dir)
    {
    case 0:
        sub_08064460(step, limit, 0);
        sub_0806453c(step, 1);
        break;
    case 1:
        sub_08064460(step, limit, 0);
        sub_08064460(step, limit, 1);
        break;
    case 2:
        sub_0806453c(step, 0);
        sub_08064460(step, limit, 1);
        break;
    case 3:
        sub_080644c0(step, limit, 0);
        sub_08064460(step, limit, 1);
        break;
    case 4:
        sub_080644c0(step, limit, 0);
        sub_0806453c(step, 1);
        break;
    case 5:
        sub_080644c0(step, limit, 0);
        sub_080644c0(step, limit, 1);
        break;
    case 6:
        sub_0806453c(step, 0);
        sub_080644c0(step, limit, 1);
        break;
    case 7:
        sub_08064460(step, limit, 0);
        sub_080644c0(step, limit, 1);
        break;
    }
}

/* Nearest active player whose X offset falls inside [lo, hi). */
s32 sub_08064758(u16 lo, u16 hi)
{
    s32 best;
    s32 bestDist;
    s32 d;
    s32 v;
    s32 found;
    s32 i;

    best = -1;
    found = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            sub_080648a0(i);
            v = gUnk_030023B4;
            if (v >= (s16)lo && v < (s16)hi)
            {
                found = 1;
                if (best == -1)
                {
                    best = i;
                    bestDist = sub_08063c60(best);
                }
                else
                {
                    d = sub_08063c60(i);
                    if (bestDist > d)
                    {
                        bestDist = d;
                        best = i;
                    }
                }
            }
        }
    }
    gUnk_030023D4 = best;
    return found;
}

/* Same over the Y offset. */
s32 sub_080647fc(u16 lo, u16 hi)
{
    s32 best;
    s32 bestDist;
    s32 d;
    s32 v;
    s32 found;
    s32 i;

    best = -1;
    found = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            sub_080648a0(i);
            v = gUnk_030023D4;
            if (v >= (s16)lo && v < (s16)hi)
            {
                found = 1;
                if (best == -1)
                {
                    best = i;
                    bestDist = sub_08063c60(best);
                }
                else
                {
                    d = sub_08063c60(i);
                    if (bestDist > d)
                    {
                        bestDist = d;
                        best = i;
                    }
                }
            }
        }
    }
    gUnk_030023D4 = best;
    return found;
}

/* Screen-space offset of task `i` from the camera, into gUnk_030023B4/D4. */
void sub_080648a0(u32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    if (gUnk_030023AC == 1 || gUnk_03002350 == 1)
    {
        gUnk_030023B4 = t->unk48 - gUnk_03002348;
        gUnk_030023D4 = t->unk4A - gUnk_030023E4;
    }
    else
    {
        gUnk_030023B4 = t->unk48 - gUnk_03002158[0];
        gUnk_030023D4 = t->unk4A - gUnk_03002158[2];
    }
}

s32 sub_0806493c(void)
{
    s32 i;

    i = sub_08063b38();
    if (i != -1)
        sub_080648a0(i);
    else
        gUnk_030023B4 = gUnk_030023D4 = 0;
    return i;
}

void sub_08064970(void)
{
    sub_080648a0(gCurTaskIdx);
}

s32 sub_08064984(s32 range)
{
    s32 i;
    s32 d;

    i = sub_08063b38();
    if (i == -1)
        return 0;
    d = sub_08063cbc(i);
    if (abs(d) <= range)
        return 1;
    return 0;
}

void sub_080649b4(u32 arg, s32 mul)
{
    struct Task *t;
    struct Actor *a;
    s32 v;
    u32 k;

    t = gUnk_03002490;
    a = t->unk8C;
    if (gUnk_030023D8 == 18)
        return;
    v = a->unk30;
    if (t->unk72 == 0 && t->unk76 == 37)
    {
        k = gUnk_03001EA4 & 3;
        v = gUnk_0873DF14[k];
    }
    v *= mul;
    if (gUnk_03002490->unk72 == 0 && gUnk_03002490->unk76 == 40
        && (u8)(a->unk04 - 2) <= 1)
        v = 200;
    sub_0800a04c(v, arg);
}

s8 sub_08064a38(void)
{
    struct Task *t;
    s32 i;

    i = gUnk_03002490->unk44;
    t = &gUnk_03002790[i];
    return t->unk43;
}

void sub_08064a60(void)
{
    gUnk_03002490->unk43 = sub_08064a38();
}

/* Spawn a class-4 task from a descriptor; returns its slot or -1. */
s32 sub_08064a78(struct ActorSpawn *p)
{
    struct Task *t;
    s32 i;

    if (p->unk0A == 1)
    {
        if (sub_08021a40(p->unk0C, p->unk0E) != 0)
            return -1;
    }
    i = sub_08063698(p->unk04, 32);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk72 = 4;
        t->unk76 = p->unk00;
        t->unk73 = p->unk08;
        t->unk74 = p->unk09;
        t->unk48 = p->unk0C;
        t->unk4A = p->unk0E;
        t->unk4C = p->unk0C << 16;
        t->unk50 = p->unk0E << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = p->unk10;
        t->unk8C = &gUnk_0200C320[i];
        sub_080636e4(i);
    }
    return i;
}

s32 sub_08064b28(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gUnk_03002490;
    p->unk0C = t->unk48;
    p->unk0E = t->unk4A;
    if (keepPrio == 0)
        p->unk10 = t->unk40;
    return sub_08064a78(p);
}

s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gUnk_03002490;
    p->unk0C = t->unk48 + p->unk0C * t->unk43;
    p->unk0E += t->unk4A;
    if (keepPrio == 0)
        p->unk10 = t->unk40;
    return sub_08064a78(p);
}

s32 sub_08064ba8(struct ActorSpawn *p, u8 keepPrio)
{
    if (keepPrio == 0)
        p->unk10 = gUnk_03002490->unk40;
    return sub_08064a78(p);
}

/* Cycle the running task's frame between 4 and 7 every other tick. */
/* Cycle the running task's frame between 4 and 7 every other tick. */
void sub_08064bcc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk24 <= 0)
    {
        if (t->unk43 == 1)
        {
            t->unk3C++;
            if (t->unk3C > 7)
                t->unk3C = 4;
        }
        else
        {
            t->unk3C--;
            if (t->unk3C <= 3)
                t->unk3C = 7;
        }
        gUnk_03002490->unk24 = 2;
    }
    else
    {
        t->unk24--;
    }
}

/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class.
   The three 16-bit arguments are declared `int` and narrowed into u16
   locals: sub_08064CDC passes them sign-extended, so the ROM's call site
   never converts (issue #65 lessons). */
s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg)
{
    struct Task *t;
    s32 i;
    u16 x = xArg;
    u16 y = yArg;
    u16 prio = prioArg;

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        switch (gUnk_03002490->unk72)
        {
        case 2:
        case 7:
            t->unk72 = 7;
            break;
        case 1:
        case 8:
            t->unk72 = 8;
            break;
        default:
            t->unk72 = 9;
            break;
        }
        t->unk48 = x;
        t->unk4A = y;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = prio;
        t->unk78 = 2;
        gUnk_03002790[i].unk16 = gUnk_03002790[gCurTaskIdx].unk16;
    }
    return i;
}

s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio)
{
    struct Task *t;
    s32 x;
    s32 y;
    u16 prio;

    t = gUnk_03002490;
    x = (s16)(t->unk48 + t->unk43 * dx);
    y = (s16)(dy + t->unk4A);
    if (keepPrio != 0)
        prio = t->unk40;
    else
        prio = 0;
    return sub_08064c1c(type, x, y, prio);
}

s32 sub_08064d34(u32 type, u8 keepPrio)
{
    struct Task *t;
    s32 x;
    s32 y;
    u16 prio;

    t = gUnk_03002490;
    x = t->unk48;
    y = t->unk4A;
    if (keepPrio != 0)
        prio = t->unk40;
    else
        prio = 0;
    return sub_08064c1c(type, x, y, prio);
}

s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio)
{
    s32 x = xArg;
    s32 y = yArg;
    u16 prio;

    if (keepPrio != 0)
        prio = gUnk_03002490->unk40;
    else
        prio = 0;
    return sub_08064c1c(type, x, y, prio);
}

/* Spawn a class-5/6 task; the 16-bit arguments are `int` for the same
   reason as sub_08064C1C's. */
s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg,
                 int prioArg, int altArg)
{
    struct Task *t;
    s32 i;
    u8 p2 = p2Arg;
    u16 x = xArg;
    u16 y = yArg;
    u16 prio = prioArg;
    u8 alt = altArg;

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        if (alt != 0)
            t->unk72 = 6;
        else
            t->unk72 = 5;
        t->unk76 = sub;
        t->unk73 = 0;
        t->unk74 = p2;
        t->unk48 = x;
        t->unk4A = y;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = prio;
        t->unk8C = &gUnk_0200C320[i];
        sub_080636e4(i);
    }
    return i;
}

s32 sub_08064e5c(u32 sub, u32 type, u8 p2)
{
    struct Task *t;

    t = gUnk_03002490;
    return sub_08064d9c(sub, type, p2, t->unk48, t->unk4A, 0, 1);
}

s32 sub_08064e90(u32 sub, u32 type, u8 p2, s16 xArg, s16 yArg)
{
    s32 x = xArg;
    s32 y = yArg;

    return sub_08064d9c(sub, type, p2, x, y, 0, 1);
}

s32 sub_08064eb8(u8 p2)
{
    struct Task *t;
    struct Actor *a;
    struct PlayerState *p;
    s32 i;

    i = sub_08064e5c(0, 68, p2);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk88 = p = &gUnk_03002170[gCurTaskIdx];
        a = t->unk8C;
        t->unk18 = p->unk0D;
        t->unk1C = p->unk0E;
        t->unk20 = gCurTaskIdx;
        a->unk00 = p->unk0D;
    }
    sub_08064d34(166, 0);
    return i;
}

/* Generic task spawn: every field of the new task comes from an argument. */
s32 sub_08064f28(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio)
{
    struct Task *t;
    s32 i;

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk72 = cls;
        t->unk76 = sub;
        t->unk73 = p3;
        t->unk74 = p4;
        t->unk48 = x;
        t->unk4A = y;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk40 = prio;
        t->unk8C = &gUnk_0200C320[i];
        sub_080636e4(i);
    }
    return i;
}

/* Pick the task type for `cls` out of one of five ROM tables. */
/* Pick the task type for `cls` out of one of five ROM tables. */
s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio)
{
    u32 type;

    switch (cls)
    {
    case 0:
        type = gUnk_0873F198[sub];
        break;
    case 1:
    case 3:
        type = gUnk_0873F23C[sub];
        break;
    case 2:
        type = gUnk_0873F264[sub];
        break;
    case 5:
        type = gUnk_0873F288[sub];
        break;
    case 6:
        type = gUnk_0873F2A0[sub];
        break;
    default:
        while (1)
            ;
    }
    return sub_08064f28(cls, sub, type, p3, p4, x, y, prio);
}

/* Clone the running task's class/sub into a fresh task. */
s32 sub_0806505c(u8 p3, u8 p4, u32 x, u32 y, u16 prio)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    struct Actor *b;
    s32 i;

    t = gUnk_03002490;
    a = t->unk8C;
    i = sub_08064f28(t->unk72, t->unk76, gUnk_03004CA0[gCurTaskIdx], p3, p4,
                     x, y, prio);
    if (i != -1)
    {
        u = &gUnk_03002790[i];
        b = u->unk8C;
        u->unk44 = gCurTaskIdx;
        b->unk64 = a->unk64;
    }
    return i;
}

s32 sub_08065100(s16 x, s16 y, u32 p2, u8 p3, u8 p4)
{
    struct Task *t;
    s32 i;

    i = sub_08064f28(0, 40, 48, 0, 0, x, y, 0);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk7C = p3;
        t->unk82 = p4;
        t->unk7E = p2;
    }
    return i;
}
