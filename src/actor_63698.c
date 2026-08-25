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

u8 sub_08063e70(struct Rect *r, s16 x, u16 y)
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
