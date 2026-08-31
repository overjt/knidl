/* game_code_and_rodata 0x0806D22C-0x0806E0F0 (issue #64, module M18 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806D22C 0x0806E0F0 src/actor_6d22c.c --newpb
 *
 * Class-1 task bodies for a family of scripted set-piece actors: the
 * three-stage entrance at sub_0806d22c (three sub_080061c0 sweeps with the
 * position recomputed from the parent task each time), the four short
 * animation-table players sub_0806d65c/6e4/730/77c and their dispatch
 * wrappers sub_0806d554/564/574/5a4/5b8/5cc, the sub_08064d34 spawner
 * helpers sub_0806d4e4/d928/da3c, the eight-way "carried" body
 * sub_0806d7ec, the gUnk_03002790[].unk73-keyed body sub_0806daec (with its
 * per-frame mover sub_0806da74), the two-sprite draw callback sub_0806dca0,
 * and the two random-walk bodies sub_0806dd90 and sub_0806df98.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0874CB90[];
extern u32 gUnk_0874C9D8[];
extern u32 gUnk_0874CA78[];
extern u32 gUnk_0874CA1C[];
extern u32 gUnk_0874CAD8[];
extern u16 gUnk_0873E620[];
extern u16 gUnk_0873E634[];
extern u16 gUnk_0873E700[];
extern u16 gUnk_0873E640[];
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_0874C500[];
extern s16 gUnk_0873EB40[];
extern s16 gUnk_0873EB60[];
extern s16 gUnk_0873EB80[];
extern u32 gUnk_0874CC60[];
extern u32 gUnk_0873EBA0[];
extern u32 gUnk_0873EC20[];
extern s32 gUnk_0873ECA0[];
extern s32 gUnk_0873ECC0[];
extern u32 gUnk_08752E48[];
extern u32 gUnk_08752D20[];
extern u16 gUnk_0873ECD0[];
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern u32 gUnk_0874CC84[];
extern u32 gUnk_0874CC48[];

extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_080656b4(void);
extern void sub_0806523c(void);
extern void sub_08064a60(void);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800617c(u32 a);
extern void sub_0806cd30(void);
extern void sub_080062c4(void);
extern u8 sub_08065f2c(s32 i);
extern void sub_08005654(s32 i);
extern s32 sub_08064d34(u32 a, u32 b);
extern void sub_080059fc(void);
extern void sub_080651b4(void);
extern void sub_080059d8(void);
extern s32 sub_08002ee8(s32 a);
extern void sub_08006138(void);
extern void sub_08063fe0(void);
extern void sub_0806af78(void);
extern void sub_0806aaa4(void);
extern u8 sub_08065160(void);
extern s32 sub_08005acc(void);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern s32 sub_080064ac(s32 a, s32 b, s32 c);
extern void sub_08005a74(void);
extern void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_0806e0f0(void);

void sub_0806d49c(void);
void sub_0806d5e0(void);
void sub_0806d65c(void);
void sub_0806d6e4(void);
void sub_0806d77c(void);
void sub_0806d9d4(void);
void sub_0806dca0(void);
void sub_0806de18(void);

void sub_0806d22c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB90;
    u->unk04 = (u32)sub_0806d49c;
    sub_08064a60();

    v = gUnk_03002490;
    v->unk4C = (gUnk_03002790[v->unk44].unk48 + -v->unk43 * v->unk24) << 16;
    v->unk50 = (gUnk_03002790[v->unk44].unk4A + v->unk20) << 16;
    sub_080061c0(0xFFFD0000, 0);
    w = gUnk_03002490;
    w->unk58 = 0;
    w->unk60 = -0x2000;
    sub_0800617c(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    sub_0806cd30();
    sub_0800617c(4);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(1);
    sub_080062c4();

    x = gUnk_03002490;
    x->unk4C = (gUnk_03002790[x->unk44].unk48 + -x->unk43 * x->unk24) << 16;
    x->unk50 = (gUnk_03002790[x->unk44].unk4A + x->unk20) << 16;
    sub_080061c0(0xFFFDC000, 0x1000);
    w = gUnk_03002490;
    w->unk58 = -0x4000;
    w->unk60 = -0x2000;
    sub_0800617c(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(1);
    sub_080062c4();

    y = gUnk_03002490;
    y->unk4C = (gUnk_03002790[y->unk44].unk48 + -y->unk43 * y->unk24) << 16;
    y->unk50 = (gUnk_03002790[y->unk44].unk4A + y->unk20) << 16;
    sub_080061c0(0xFFFEE000, 0x1800);
    w = gUnk_03002490;
    w->unk58 = -0x4000;
    w->unk60 = -0x2000;
    sub_0800617c(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    sub_0800617c(6);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0806d49c(void)
{
    if (gUnk_03004CA0[gUnk_03002490->unk44] == -1
     || sub_08065f2c(gUnk_03002490->unk44) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806d4e4(u32 a, s32 b)
{
    s32 i;
    struct Task *p;

    switch (a)
    {
    case 1:
        i = sub_08064d34(148, 0);
        break;
    case 0:
        i = sub_08064d34(149, 0);
        break;
    case 2:
        i = sub_08064d34(150, 0);
        break;
    case 4:
        i = sub_08064d34(151, 0);
        break;
    case 3:
        i = sub_08064d34(152, 0);
        break;
    case 5:
        i = sub_08064d34(153, 0);
        break;
    }
    if (i != -1 && b > 0)
    {
        p = &gUnk_03002790[i];
        p->unk24 = b;
    }
}

void sub_0806d554(void)
{
    sub_0806d6e4();
    TaskDispatchTrampoline();
}

void sub_0806d564(void)
{
    sub_0806d65c();
    TaskDispatchTrampoline();
}

void sub_0806d574(void)
{
    sub_0806d77c();
    TaskDispatchTrampoline();
}

void sub_0806d584(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk50 = 0;
    t->unk4C = 0;
    t->unk04 = (u32)sub_0806d5e0;
}

void sub_0806d5a4(void)
{
    sub_0806d584();
    sub_0806d6e4();
    TaskDispatchTrampoline();
}

void sub_0806d5b8(void)
{
    sub_0806d584();
    sub_0806d65c();
    TaskDispatchTrampoline();
}

void sub_0806d5cc(void)
{
    sub_0806d584();
    sub_0806d77c();
    TaskDispatchTrampoline();
}

void sub_0806d5e0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk24 <= 0)
    {
        t->unk04 = 0;
        t->unk00 = (u32)sub_080656b4;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
    }
    else
    {
        if (gUnk_03004CA0[t->unk44] == -1 || sub_08065f2c(t->unk44) != 1)
            sub_08005654(gCurTaskIdx);
    }
}

void sub_0806d65c(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874C9D8;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk40 = 0;
    u->unk3C = 0;
    TaskYieldTrampoline(2);
    for (i = 0; i < 10; i++)
    {
        gUnk_03002490->unk3C = gUnk_0873E620[i];
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(4);
    for (i = 0; i < 6; i++)
    {
        gUnk_03002490->unk3C = gUnk_0873E634[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d6e4(void)
{
    struct Task *t;
    s32 i;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CA78;
    t->unk42 = 10;
    gUnk_03002490->unk40 = 0;
    for (i = 0; i < 23; i++)
    {
        gUnk_03002490->unk3C = gUnk_0873E700[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d730(void)
{
    struct Task *t;
    s32 i;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CA1C;
    t->unk42 = 4;
    gUnk_03002490->unk40 = 0;
    for (i = 0; i < 23; i++)
    {
        gUnk_03002490->unk3C = gUnk_0873E640[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d77c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CAD8;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk40 = 0;
    u->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
}

void sub_0806d7ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 k;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_0874C500;
    n = sub_08002ee8(8);
    u = gUnk_03002490;
    u->unk28 = n;
    u->unk4C = (u->unk48 + gUnk_0873EB40[n]) << 16;
    k = 4;
    u->unk50 = (u->unk4A + ((gUnk_0873EB40 + 8)[n] + k)) << 16;
    a = gUnk_0873EB60[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->unk54 = b;
    a = (gUnk_0873EB60 + 8)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->unk58 = b;
    u->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    v = gUnk_03002490;
    a = gUnk_0873EB80[v->unk28];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk54 = b;
    a = (gUnk_0873EB80 + 8)[v->unk28];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk58 = b;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    w = gUnk_03002490;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_0806d928(void)
{
    s32 i;
    s32 j;
    struct Task *p;

    for (i = 0; i < 8; i++)
    {
        j = sub_08064d34(155, 0);
        if (j != -1)
        {
            p = &gUnk_03002790[j];
            p->unk73 = i;
        }
    }
}

void sub_0806d95c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 k;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CC60;
    u->unk04 = (u32)sub_0806d9d4;
    u->unk24 = 2;
    u->unk3C = 0;
    k = u->unk73 * 4;
    for (i = 0; i < 4; i++)
    {
        v = gUnk_03002490;
        v->unk54 = gUnk_0873EBA0[k + i];
        v->unk58 = gUnk_0873EC20[k + i];
        TaskYieldTrampoline(6);
    }
    sub_08006138();
}

void sub_0806d9d4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk24 <= 0)
    {
        if (t->unk3C > 7)
        {
            sub_08063fe0();
        }
        else
        {
            t->unk3C++;
            t->unk24 = 2;
        }
    }
    else
    {
        t->unk24--;
    }
}

void sub_0806da04(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    sub_0806af78();
    TaskDispatchTrampoline();
}

void sub_0806da20(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    sub_0806aaa4();
    TaskDispatchTrampoline();
}

void sub_0806da3c(u32 a, u32 b)
{
    s32 i;
    struct Task *p;

    i = sub_08064d34(156, 0);
    if (i != -1)
    {
        p = &gUnk_03002790[i];
        p->unk73 = a;
        p->unk74 = b;
    }
}

void sub_0806da74(void)
{
    struct Task *t;
    s32 m;
    s32 j;

    t = gUnk_03002490;
    if (t->unk73 != 0)
    {
        m = t->unk74;
        j = m * 2;
        t->unk48 += gUnk_0873ECA0[m * 2];
        t->unk4A += gUnk_0873ECA0[j + 1];
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (t->unk73 == 1)
        {
            t->unk54 = gUnk_0873ECC0[m * 2];
            t->unk58 = gUnk_0873ECC0[j + 1];
        }
    }
}

void sub_0806daec(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_08752E48;
    t->unk42 = 10;
    gUnk_03002490->unk40 = 0;
    sub_0806da74();
    u = gUnk_03002490;
    switch (u->unk73)
    {
    case 0:
        u->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        break;
    case 1:
        u->unk6E = 0;
        do
        {
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            } while (++*(s16 *)&gUnk_03002490->unk6C <= 6);
        } while (++*(s16 *)&gUnk_03002490->unk6E <= 8);
        break;
    case 2:
        u->unk3C = 33;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        break;
    case 3:
        u->unk3C = 24;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_0806dca0(void)
{
    struct Task *p;
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08065160() == 0)
        return;
    if (sub_08005acc() == 0)
        return;
    t = gUnk_03002490;
    tbl = t->unk38;
    sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40,
                 t->unk48 - gUnk_03002348,
                 (s16)(t->unk4A - gUnk_030023E4));
    u = gUnk_03002490;
    sub_08001a94(u->unk42 - 1, tbl[7], u->unk3E, u->unk40,
                 u->unk48 + u->unk24 - gUnk_03002348,
                 (s16)(u->unk4A + u->unk20 - gUnk_030023E4));
}

void sub_0806dd90(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806dca0;
    t->unk38 = gUnk_08752D20;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk04 = (u32)sub_0806de18;
    u->unk40 = 0;
    u->unk24 = 0;
    u->unk20 = 0;
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            gUnk_03002490->unk3C = gUnk_0873ECD0[i];
            TaskYieldTrampoline(3);
            gUnk_03002490->unk24 = (u16)sub_080064ac(-12, 1, 24);
            gUnk_03002490->unk20 = (u16)sub_080064ac(-12, 1, 24);
        }
    }
}

void sub_0806de18(void)
{
    if (gUnk_03004CA0[gUnk_03002490->unk44] == -1
     || sub_08065f2c(gUnk_03002490->unk44) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806de60(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CC84;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk40 = 0;
    u->unk4C = gUnk_03002790[u->unk44].unk4C;
    u->unk50 = gUnk_03002790[u->unk44].unk50;
    u->unk3C = 0;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0806dedc(void)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    if (a->unk10 == -1)
        return;
    if (*(s16 *)&a->unk12 == -2)
        return;
    if (*(s16 *)&a->unk12 <= 0)
    {
        sub_08005654(a->unk10);
        a->unk10 = 0xFFFF;
        a->unk12 = 0xFFFE;
    }
    a->unk12--;
}

void sub_0806df28(s32 a, s32 b)
{
    struct Actor *p;
    s32 c;

    p = gUnk_03002490->unk8C;
    if (p->unk10 != -1)
    {
        sub_08005654(p->unk10);
        p->unk10 = 0xFFFF;
    }
    switch (a)
    {
    case 1:
        c = sub_08064d34(160, 0);
        break;
    case 2:
        c = sub_08064d34(161, 0);
        break;
    case 3:
        c = sub_08064d34(159, 0);
        break;
    case 0:
    default:
        c = -1;
        break;
    }
    p->unk10 = c;
    if (b == 1)
        p->unk12 = 60;
    else
        p->unk12 = 0xFFFE;
}

void sub_0806df98(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CC48;
    t->unk42 = 6;
    u = gUnk_03002490;
    u->unk04 = (u32)sub_0806e0f0;
    u->unk40 = 0;
    u->unk6E = 0;
    do
    {
        gUnk_03002490->unk4C = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        sub_0800625c(0x4000, 0xFFFFF900, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 4);
        gUnk_03002490->unk4C = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        sub_0800625c(0xFFFFC000, 0x700, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 4);
    } while (++*(s16 *)&gUnk_03002490->unk6E <= 1);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}
