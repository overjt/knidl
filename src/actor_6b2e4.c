/* game_code_and_rodata 0x0806B2E4-0x0806C2A4 (issue #64, module M18 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806B2E4 0x0806C2A4 pending/batch3/actor_6b2e4.c --newpb
 *
 * The "carried by / riding on another task" movement block: the per-frame
 * position integrators sub_0806b410 and sub_0806b670 that walk the two stride-5
 * offset tables at 0x0873E7C4 / 0x0873E864, the handover helpers that hand the
 * actor back to the generic task body (sub_0806b8bc), the player-record
 * bookkeeping around gUnk_03002170[] (sub_0806b9dc, sub_0806bd10, sub_0806be4c),
 * and the class-1 task bodies sub_0806bf54 / sub_0806c05c / sub_0806c158 with
 * their per-frame callbacks.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0873E78C[];
extern u32 gUnk_0874C9D8[];

extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(u32 a);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(s32 a);
extern void sub_08063fe0(void);
extern void sub_0806a0cc(void);
extern void sub_0806d65c(void);
extern void sub_080b54d0(s32 i);
extern void TaskYieldTrampoline(u32 a);
extern u32 sub_08021a40(s32 x, s32 y);
extern u32 sub_0803111c(s32 x, s32 y, s32 c, s32 d);
extern void sub_080639e0(u32 *p);

extern s16 gUnk_0873E7C4[];
extern s16 gUnk_0873E864[];
extern u16 gUnk_0873EAD8[][4];
extern u32 gUnk_0873F8B4[];
extern u32 gUnk_0873F8BC[];

void sub_0806b8bc(void);
extern void sub_0806523c(void);
extern void sub_080059d8(void);
extern void sub_0806a344(void);
extern void sub_08065640(void);
extern void sub_080639a4(u32 *p);
extern u32 gUnk_0873F92C[];
extern struct PlayerState gUnk_03002170[];
extern void sub_08065d44(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 sub_08063cbc(s32 i);
extern void sub_080649b4(s32 a, s32 b);
extern s16 gUnk_0873E7A4[];
extern s16 gUnk_0300244C;
extern u8 gUnk_02007CF4[];
extern u8 gUnk_02006178;
extern u32 gUnk_0873EAA0[];
extern u32 gUnk_0873F938[];
extern u32 gUnk_0873F8F4[];
extern void sub_080059fc(void);
extern void sub_08063990(u32 *p);
void sub_0806bfd8(void);
void sub_0806c0c0(void);
void sub_0806c148(void);
void sub_0806c1d0(void);
extern void sub_0806a158(void);
extern void sub_080656b4(void);
extern void sub_080692fc(void);
extern void sub_080261d4(u32 a);
extern void sub_0801a828(u8 a, s16 x, s16 y, u32 *p);
extern void sub_080061c0(u32 a, u32 b);
extern void sub_0800622c(u32 a, u32 b, u32 c);
extern u32 gUnk_0873F830[];
extern u32 gUnk_0873F844[];
extern u32 gUnk_0873F894[];
extern u8 gUnk_03005550[];
void sub_0806bd10(void);
void sub_0806bf38(void);
void sub_0806bc54(void);
void sub_0806befc(void);

void sub_0806b2e4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0873E78C);
}

void sub_0806b300(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08006338(*(s16 *)&gUnk_03002490->unk18);
    gUnk_03002490->unk58 = 0x4000;
    sub_08006138();
}

void sub_0806b330(void)
{
}

void sub_0806b334(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08006338(*(s16 *)&gUnk_03002490->unk18);
    TaskYieldTrampoline(30);
    sub_0806395c(2);
    sub_08006138();
}

void sub_0806b368(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0806b2e4, gCurTaskIdx);
}

void sub_0806b390(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    sub_0806a0cc();
    sub_0806d65c();
    sub_08063fe0();
}

void sub_0806b3c0(void)
{
}

void sub_0806b3c4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = 0;
    if (t->unk76 != 0)
        sub_080b54d0(gCurTaskIdx);
    TaskYieldTrampoline(1);
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C9D8;
    u->unk40 = 0;
    sub_080031b8(109);
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

    t = gUnk_03002490;
    s = &gUnk_03002790[t->unk44];
    p = t->unk88;
    if (t->unk28 != s->unk43)
        t->unk43 = -t->unk43;
    u = gUnk_03002490;
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
        v = gUnk_03002490;
        v->unk70 = -*(s8 *)&p->unk16;
        v->unk48 = v->unk20;
        v->unk4A = v->unk1C;
        k = *(s16 *)&v->unk70 - 3;
        if (v->unk72 == 1)
        {
            sub_080639e0(gUnk_0873F8BC);
            w = gUnk_03002490;
            w->unk48 += gUnk_0873EAD8[k][2] * w->unk43;
            w->unk4A += gUnk_0873EAD8[k][3];
        }
        else
        {
            sub_080639e0(gUnk_0873F8B4);
            w = gUnk_03002490;
            w->unk48 += gUnk_0873EAD8[k][0] * w->unk43;
            w->unk4A += gUnk_0873EAD8[k][1];
        }
        x = gUnk_03002490;
        x->unk4C = x->unk48 << 16;
        x->unk50 = x->unk4A << 16;
        if (sub_08021a40(x->unk48, x->unk4A) != 0)
        {
            y = gUnk_03002490;
            if (sub_0803111c(y->unk48 >> 4, y->unk4A >> 4, 3, -1) == 0)
            {
                sub_0806b8bc();
                return;
            }
        }
        sub_0806395c(6);
        sub_08006148(sub_0806bf38, gCurTaskIdx);
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
    z = gUnk_03002490;
    z->unk48 += g;
    z->unk4A += h;
    z->unk20 = z->unk48;
    z->unk1C = z->unk4A;
    sub_08006338((s16)(e + 2));
    gUnk_03002490->unk42 = b;
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

    t = gUnk_03002490;
    s = &gUnk_03002790[t->unk44];
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
            u = gUnk_03002490;
            if (sub_0803111c(u->unk48 >> 4, u->unk4A >> 4, 3, -1) == 0)
            {
                sub_0806b8bc();
                return;
            }
            v = gUnk_03002490;
            v->unk43 = s->unk43;
        }
        else
        {
            w = gUnk_03002490;
            w->unk43 = s->unk43;
        }
        sub_0806395c(2);
        sub_08006148(sub_0806bf38, gCurTaskIdx);
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
    z = gUnk_03002490;
    z->unk48 += g;
    z->unk4A += h;
    z->unk20 = z->unk48;
    z->unk1C = z->unk4A;
    sub_08006338((s16)(e + 2));
    gUnk_03002490->unk42 = b;
    if (gUnk_0873E864[n + 3] == 1)
    {
        x = gUnk_03002490;
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

    t = gUnk_03002490;
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

    t = gUnk_03002490;
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

    t = gUnk_03002490;
    if (t->unk72 == 0)
    {
        v = t->unk76;
        if (v == 17 || v == 9 || v == 0 || v == 31 || v == 32)
            gUnk_03002490->unk0C = (u32)sub_0806523c;
    }
    u = gUnk_03002490;
    u->unk00 = (u32)sub_080059d8;
    u->unk82 = 0;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_0806b938(void)
{
    struct PlayerState *p;
    u8 z;

    p = gUnk_03002490->unk88;
    if (p->unk04 == 10)
        return;
    z = 0;
    p->filler07[1] = z;
    p->filler07[0] = z;
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

    t = gUnk_03002490;
    a = t->unk8C;
    p = t->unk88;
    if (t->unk72 != 0 || t->unk76 != 40)
        p->filler07[2] = 1;
    u = gUnk_03002490;
    if (u->unk0C == (u32)sub_08065640)
        u->unk0C = (u32)sub_0806523c;
    if (gUnk_03002490->unk72 != 1)
        sub_080639a4(gUnk_0873F92C);
    v = gUnk_03002490;
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
    t = gUnk_03002490;
    t->unk44 = t->unk7E;
    t->unk88 = p = &gUnk_03002170[t->unk44];
    p->filler07[0]++;
    u = gUnk_03002490;
    s = &gUnk_03002790[u->unk44];
    u->unk28 = s->unk43;
}

void sub_0806ba34(void)
{
    struct Task *t;
    struct Actor *a;
    u32 v;
    u32 m;
    u32 z;

    t = gUnk_03002490;
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

    t = gUnk_03002490;
    s = &gUnk_03002790[t->unk44];
    t->unk4C = (t->unk48 - s->unk48) << 16;
    t->unk50 = (t->unk4A - s->unk4A) << 16;
}

s32 sub_0806baec(s32 a)
{
    if (sub_08063cbc(gUnk_03002490->unk44) < 0)
    {
        if (a > -sub_08063cbc(gUnk_03002490->unk44))
            goto yes;
        goto no;
    }
    if (a > sub_08063cbc(gUnk_03002490->unk44))
        goto yes;
    goto no;
yes:
    return 1;
no:
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
        sub_08006338(1);
        break;
    case 1:
    case 4:
        sub_08006338(2);
        break;
    }
    t = gUnk_03002490;
    w = t->unk40;
    m = 0xFFF;
    m &= w;
    t->unk40 = m | 0xF000;
}

s32 sub_0806bb7c(void)
{
    sub_0806b9dc();
    sub_0806ba34();
    switch (gUnk_03002490->unk82)
    {
    case 1:
        sub_0806b95c();
        sub_0806395c(0);
        break;
    case 2:
        sub_0806b95c();
        sub_0806bc54();
        sub_0806395c(4);
        break;
    case 3:
        sub_0806b95c();
        sub_0806bc54();
        sub_0806395c(1);
        break;
    }
    sub_08006148(sub_0806befc, gCurTaskIdx);
    return 1;
}

void sub_0806bbe8(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;

    t = gUnk_03002490;
    if (t->unk4C <= 0)
        t->unk5C = 10752;
    else
        t->unk5C = -10752;
    u = gUnk_03002490;
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

    t = gUnk_03002490;
    p = t->unk88;
    if (*(s8 *)&p->filler07[1] != 0)
    {
        t->unk18 = 1;
        sub_080062c4();
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

    t = gUnk_03002490;
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

    t = gUnk_03002490;
    p = t->unk88;
    sub_080649b4(t->unk44, 1);
    if (*(s8 *)&p->filler07[1] != 0)
    {
        gUnk_03002490->unk18 = 1;
        sub_080062c4();
    }
    else
    {
        one = 1;
        p->filler07[1] = one;
        p->filler07[0] = one;
        sub_080062c4();
    }
}

void sub_0806bcdc(void)
{
    sub_0806bbe8();
    if (*(s16 *)&gUnk_03002490->unk70 != 0)
        return;
    if (sub_0806baec(18) == 0)
        return;
    sub_0806bd10();
    gUnk_03002490->unk70 = 1;
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

    t = gUnk_03002490;
    a = t->unk8C;
    p = t->unk88;
    if ((s8)a->unk44->unk0D == 1)
    {
        if (*(s8 *)&p->filler07[0] == 1 && t->unk72 == 6 && t->unk76 != 0)
        {
            p->filler07[2] = 3;
            gUnk_03002490->unk30 = *(s8 *)&p->filler07[1];
            p->filler07[1]++;
        }
        else if ((s8)p->filler07[0] > 0)
        {
            p->filler07[0]--;
        }
    }
    else
    {
        t->unk30 = *(s8 *)&p->filler07[1];
        p->filler07[1]++;
        p->unk06 = 1;
    }
    p->filler30[1] = 0;
    if (*(s8 *)&a->unk00 == 0)
        return;
    u = gUnk_03002490;
    if (u->unk72 == 6)
    {
        f = u->unk76;
        if (f == 0)
        {
            p->filler07[4] = u->unk18;
            v = gUnk_03002490;
            p->filler07[5] = v->unk1C;
            w = gUnk_03002490;
            if (w->unk20 == w->unk44)
                p->filler30[1] = 1;
            if (gUnk_0300244C == 0)
                return;
            p->filler07[3] = f;
            gUnk_02007CF4[p->unk00] = 1;
            return;
        }
    }
    if (gUnk_0300244C == 0 || gUnk_02007CF4[p->unk00] != 1)
        p->filler07[3]++;
    if (*(s8 *)&p->filler07[4] != 0)
        return;
    p->filler07[4] = a->unk00;
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
    p->filler07[5] = q;
}

void sub_0806be4c(u32 i)
{
    struct Task *s;
    struct Actor *a;
    struct PlayerState *p;

    s = &gUnk_03002790[i];
    a = s->unk8C;
    p = s->unk88;
    if (a->unk04 == 1)
    {
        if (*(s8 *)&p->filler07[1] != 0)
        {
            p->filler07[0] = p->filler07[1];
        }
        else
        {
            p->filler07[3] = 0;
            p->filler07[2] = 0;
            p->filler07[1] = 0;
            p->filler07[0] = 0;
        }
    }
    else
    {
        p->filler07[3] = 0;
        p->filler07[2] = 0;
        p->filler07[1] = 0;
        p->filler07[0] = 0;
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
        t = gUnk_03002490;
        w = t->unk40;
        m = 0xFFF;
        m &= w;
        t->unk40 = m | t->unk8C->unk22;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (t->unk72 != 1 && t->unk72 != 6)
            sub_080639a4(gUnk_0873F938);
        sub_0806b8bc();
    }
    return gUnk_02006178;
}

void sub_0806befc(void)
{
    gUnk_03002490->unk04 = 0;
    sub_080062c4();
    gUnk_03002490->unk7A = 0;
    sub_0806bb34(gUnk_03002490->unk14);
    sub_08002e98(gUnk_03002490->unk14, 8, gUnk_0873EAA0);
    sub_08006138();
}

void sub_0806bf38(void)
{
    sub_08002e98(gUnk_03002490->unk14, 8, gUnk_0873EAA0);
}

void sub_0806bf54(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk08 = (u32)sub_0806bfd8;
    sub_08063990(gUnk_0873F8F4);
    sub_0806ba9c();
    u = gUnk_03002490;
    u->unk46 = 0;
    u->unk70 = 0;
    u->unk30 = 0;
    u->unk42 = 6;
    while (sub_0806baec(16) == 0)
    {
        sub_0806bcdc();
        TaskYieldTrampoline(1);
    }
    if (*(s16 *)&gUnk_03002490->unk70 == 0)
    {
        sub_0806bd10();
        gUnk_03002490->unk70 = 1;
    }
    gUnk_03002490->unk46 = 1;
    sub_08006138();
}

void sub_0806bfd8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk46 != 0)
    {
        if (t->unk72 == 0)
            sub_080649b4(t->unk44, 1 << t->unk30);
        else
            sub_080649b4(t->unk44, 1);
        u = gUnk_03002490;
        if (u->unk72 != 1)
        {
            if (u->unk72 == 6)
            {
                sub_0806a158();
                return;
            }
        }
        sub_08063fe0();
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

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk04 = (u32)sub_0806c0c0;
    t->unk08 = (u32)sub_0806c148;
    t->unk50 = 0;
    t->unk4C = 0;
    t->unk7C = 0;
    u = gUnk_03002490;
    u->unk20 = u->unk48;
    u->unk1C = u->unk4A;
    u->unk12 = 1;
    sub_08063990(gUnk_0873F8F4);
    gUnk_03002490->unk18 = 0;
    sub_0806bc9c();
    sub_08006138();
}

void sub_0806c0c0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk78 = 127;
    if (t->unk18 == 1)
        sub_0806b8bc();
    else
        sub_0806b670();
    sub_0806b878();
    u = gUnk_03002490;
    if (u->unk72 == 1)
        sub_0801a828((u8)gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873F844);
    else
        sub_0801a828((u8)gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873F830);
}

void sub_0806c148(void)
{
    sub_0806b848();
    sub_0806be84();
}

void sub_0806c158(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk04 = (u32)sub_0806c1d0;
    t->unk08 = 0;
    if (gUnk_0300244C != 0 && t->unk8C->unk50 == 0)
        sub_080639e0(gUnk_0873F894);
    sub_080061c0(0x38000, 0x5A5A5A5A);
    sub_0800622c(0x30000, 0x8000, 0x60000);
    gUnk_03002490->unk7C = 0;
    gUnk_03002490->unk80 = 0;
    sub_08006138();
}

void sub_0806c1d0(void)
{
    struct Task *t;

    gUnk_03002490->unk78 = 127;
    sub_080692fc();
    if ((*(u32 *)gUnk_03005550 & 0xFFFFFF) != 0)
    {
        if (gUnk_03005550[1] != 0)
            gUnk_03002490->unk30 = 0;
        if (gUnk_03005550[2] != 0)
            gUnk_03002490->unk30 = 1;
        if (gUnk_03005550[0] != 0)
            gUnk_03002490->unk30 = 2;
        sub_080031b8(179);
        sub_080261d4(2);
        sub_0806395c(3);
        sub_08006148(sub_0806bf38, gCurTaskIdx);
        return;
    }
    t = gUnk_03002490;
    if (t->unk72 == 1)
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F844);
    else
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F830);
}
