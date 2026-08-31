/* game_code_and_rodata 0x0806A344-0x0806AD18 (issue #64, module M18 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806A344 0x0806AD18 src/actor_6a344.c --newpb
 *
 * Class-1 task bodies for the "player is out of the level" states: the
 * warp-star / door exit animation driver (sub_0806a3dc + its per-exit
 * entry points sub_0806a500/530/568), the level-clear dance
 * (sub_0806a7a0/sub_0806a7f4 and the sub_0806a8d8 state dispatcher), the
 * death sequence (sub_0806a980/sub_0806a8f4), and a family of one-shot
 * bodies that re-arm the actor and hand control to sub_0806d65c /
 * sub_0806d730.  Every function here runs as gUnk_03002490 (the current
 * task), so almost all of them are a run of `gUnk_03002490->field = K`
 * statements interleaved with TaskYieldTrampoline() waits.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0873E5BC[];
extern u32 gUnk_0873E5E8[];
extern u16 gUnk_0873E610[];
extern s16 gUnk_0873E5F8[];
extern u32 gUnk_0874CB7C[];
extern s16 gUnk_0300244C;
extern struct ActorDef gUnk_0873F6BC;
extern struct PlayerState gUnk_03002170[];
extern u32 gUnk_0873E670[];
extern u32 gUnk_0873E67C[];
extern vs16 gUnk_03004CA0[];
extern u8 gUnk_0873F880[];
extern u32 gUnk_0874C9D8[];
extern u32 gUnk_0873E688[];
extern u16 gUnk_03001270;
extern vu16 gUnk_03001ED8;
extern u16 gUnk_0873E698[];
extern struct ActorDef gUnk_0873F6E8;
extern u8 gUnk_0873F81C[];
extern struct ActorDef gUnk_0873F704;
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08005654(s32 a);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_08063fe0(void);
extern void sub_080656b4(void);
extern void sub_0806d4e4(s32 a, s32 b);
extern void sub_0806a0cc(void);
extern void sub_0806df28(s32 a, s32 b);
extern void sub_0806a488(void);
extern void sub_0806a524(void);
extern void sub_0806a55c(void);
extern void sub_0806a594(void);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern void sub_0806395c(u8 v);
extern void sub_08063908(u32 def);
extern void sub_0806523c(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_0806a8d8(void);
extern u8 sub_0806437c(s32 a);
extern u32 sub_08002ee8(s32 a);
extern void sub_0806a7f4(void);
extern void sub_0801a828(u8 a, s16 b, s16 c, void *d);
extern void sub_08068e04(void);
extern s32 sub_08069b44(void);
extern void sub_08006138(void);
extern void sub_080031b8(s32 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0806d65c(void);
extern void sub_0806d730(void);
extern void sub_08065e1c(s32 a, s32 b);
extern void sub_0806aa40(void);
extern void sub_080670f0(u16 *p);
extern void sub_080261d4(s32 a);
extern void sub_0806aba4(void);
extern void sub_0806ac48(void);
extern void sub_0806acc4(void);
extern void TaskYieldTrampoline(u32 a);
extern s16 sub_0806a03c(void);
extern void sub_0806421c(s32 a, s32 b);

void sub_0806a344(void)
{
    struct Task *t;
    struct Actor *a;
    s8 *p;

    a = gUnk_03002490->unk8C;
    p = (s8 *)a->unk5C;
    a->unk05 = 2;
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk04 = 0;
    t->unk08 = 0;
    sub_080062c4();
    sub_08006338(0);
    if (p != NULL)
        sub_08002e98(p[1], 11, gUnk_0873E5BC);
    if (a->unk10 != -1)
    {
        sub_08005654(a->unk10);
        a->unk10 = 0xFFFF;
    }
    sub_08063fe0();
}

void sub_0806a3ac(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk82 > 3)
        t->unk82 = 0;
    sub_08002e98(gUnk_03002490->unk82, 4, gUnk_0873E5E8);
}

void sub_0806a3dc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;
    s32 i;
    u16 *p;

    sub_080062c4();
    sub_08006338(0);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    a = t->unk8C;
    a->unk22 = t->unk40 & 0xF000;
    t->unk2C = t->unk4A;
    t->unk30 = 14;
    t->unk34 = 14;
    t->unk24 = 0;
    p = gUnk_0873E610;
    for (i = 0; i < 8; i++)
    {
        u = gUnk_03002490;
        u->unk4A = u->unk2C + p[i];
        u->unk50 = u->unk4A << 16;
        TaskYieldTrampoline(1);
    }
    v = gUnk_03002490;
    v->unk50 = v->unk2C << 16;
    sub_0806421c(sub_0806a03c(), 512);
    w = gUnk_03002490;
    w->unk54 = gUnk_030023B4;
    w->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(12);
}

void sub_0806a488(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk30 > 0)
    {
        t->unk30--;
        if ((t->unk30 & 1) == 0)
            t->unk40 = (t->unk40 & 0xFFF) | 0xF000;
        else
            t->unk40 = (t->unk40 & 0xFFF) | t->unk8C->unk22;
    }
    u = gUnk_03002490;
    if (u->unk34 <= 0)
    {
        if (u->unk24 == 0)
        {
            sub_0806d4e4(3, 6);
            gUnk_03002490->unk24 = 1;
        }
    }
    else
    {
        u->unk34--;
    }
}

void sub_0806a500(void)
{
    gUnk_03002490->unk04 = (u32)sub_0806a524;
    sub_0806a3dc();
    sub_080062c4();
    sub_0806a0cc();
}

void sub_0806a524(void)
{
    sub_0806a488();
}

void sub_0806a530(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0806a55c;
    sub_0806df28(t->unk82, 0);
    sub_0806a3dc();
    sub_080062c4();
    sub_0806a0cc();
}

void sub_0806a55c(void)
{
    sub_0806a488();
}

void sub_0806a568(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0806a594;
    sub_0806df28(t->unk82, 0);
    sub_0806a3dc();
    sub_080062c4();
    sub_0806a0cc();
}

void sub_0806a594(void)
{
    sub_0806a488();
}

void sub_0806a5a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    s32 n;
    u32 a;
    s16 *tbl;

    t = gUnk_03002490;
    i = t->unk24 >> 1;
    if (i <= 5)
    {
        if (t->unk20 <= 0)
        {
            t->unk24 = t->unk24 + 1;
            if (t->unk1C != 0)
                t->unk3C = 0;
            else
                t->unk3C = 0xFFFF;
            u = gUnk_03002490;
            tbl = gUnk_0873E5F8;
            n = u->unk1C;
            a = n << 1;
            a += i << 2;
            a += (u32)tbl;
            u->unk20 = *(s16 *)a;
            n ^= 1;
            u->unk1C = n;
        }
        v = gUnk_03002490;
        v->unk20--;
    }
    else
    {
        t->unk38 = gUnk_0874CB7C;
        t->unk40 = 0;
        if (gUnk_0300244C != 0)
            t->unk8C->unk22 = 0;
        w = gUnk_03002490;
        if (w->unk3C == -1)
            w->unk3C = 4;
    }
}

void sub_0806a638(void)
{
    struct Task *t;
    struct Actor *a;
    void (*fn)(void);
    s32 v;
    s32 z;
    u8 zero;

    a = gUnk_03002490->unk8C;
    v = sub_08064d34(171, 0);
    t = gUnk_03002490;
    t->unk46 = v;
    z = 0;
    zero = 0;
    t->unk18 = z;
    t->unk1C = z;
    t->unk20 = z;
    t->unk24 = z;
    t->unk28 = a->unk0E;
    sub_0806df28(3, 0);
    gUnk_03002490->unk80 = zero;
    gUnk_03002490->unk7A = zero;
    sub_0806395c(0);
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    fn = a->unk40;
    sub_08063908((u32)&gUnk_0873F6BC);
    a->unk40 = fn;
}

void sub_0806a6a0(void)
{
    if ((u8)(gUnk_03002170[gUnk_03002490->unk28].unk0D - 13) > 1)
    {
        sub_0806395c(2);
        sub_08006148(sub_0806a8d8, gCurTaskIdx);
    }
}

void sub_0806a6e0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task **g;
    s8 *pb;
    u32 addr;
    u32 h;
    s32 one;
    u8 v;

    g = &gUnk_03002490;
    t = *g;
    if ((s8)t->unk7C != 7)
        return;
    pb = &t->unk7E;
    addr = *pb * 144;
    addr += (u32)gUnk_03002790;
    addr += 118;
    h = *(u16 *)addr;
    one = 1;
    h |= one;
    *(u16 *)addr = h;
    v = sub_0806437c(*pb);
    switch (v)
    {
    case 8:
        goto set_one;
    case 0:
        if (sub_08002ee8(2) != 0)
            goto set_one;
    case 4:
        (*g)->unk43 = -1;
        break;
    set_one:
        (*g)->unk43 = one;
        break;
    }
    u = gUnk_03002490;
    u->unk44 = u->unk7E;
    u->unk88 = &gUnk_03002170[u->unk44];
    sub_0806395c(1);
    sub_08006148(sub_0806a8d8, gCurTaskIdx);
}

void sub_0806a7a0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = 0;
    t->unk46 = 0xFFFF;
    sub_08006338(0);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk04 = (u32)sub_0806a7f4;
    sub_080062c4();
    gUnk_03002490->unk42 = 7;
    sub_0806a638();
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0873E670);
}

void sub_0806a7f4(void)
{
    struct Task *t;
    struct Actor *a;
    s32 r;

    gUnk_03002490->unk7A = 0;
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0873E67C);
    sub_0806a5a0();
    t = gUnk_03002490;
    if (t->unk14 == 1)
    {
        if ((s8)t->unk7C != 0)
        {
            sub_0806395c(2);
            sub_08006148(sub_0806a8d8, gCurTaskIdx);
        }
        else
        {
            sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F880);
        }
    }
    else if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_0806a6e0();
        r = sub_08069b44();
        if (r == 0)
        {
            gUnk_03002490->unk7C = r;
            if (gUnk_03002490->unk14 != 2)
                sub_0806a6a0();
        }
        else
        {
            a = gUnk_03002490->unk8C;
            if (a->unk10 != -1)
            {
                sub_08005654(a->unk10);
                a->unk10 = 0xFFFF;
            }
        }
    }
}

void sub_0806a8d8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0873E670);
}

void sub_0806a8f4(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(2);
        t = gUnk_03002490;
        t->unk6C = t->unk6C + 1;
    } while ((s16)t->unk6C <= 7);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(120);
    sub_0806395c(2);
    sub_08006138();
}

void sub_0806a958(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0806a8d8, gCurTaskIdx);
}

void sub_0806a980(void)
{
    struct Task *t;
    struct Actor *a;
    u8 one;

    t = gUnk_03002490;
    a = t->unk8C;
    one = 1;
    t->unk15 = one;
    gUnk_03002490->unk12 = one;
    if (a->unk10 != -1)
    {
        sub_08005654(a->unk10);
        a->unk10 = 0xFFFF;
    }
    sub_080062c4();
    sub_080031b8(229);
    sub_080061c0(0x40000, 0x5A5A5A5A);
    sub_08006138();
}

void sub_0806a9d4(void)
{
}

void sub_0806a9d8(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk04 = zero;
    sub_080031b8(242);
    sub_080062c4();
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C9D8;
    u->unk40 = zero;
    sub_0806d65c();
    sub_08063fe0();
}

void sub_0806aa0c(void)
{
}

void sub_0806aa10(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk82 > 3)
        t->unk82 = 0;
    sub_08002e98(gUnk_03002490->unk82, 4, gUnk_0873E688);
}

void sub_0806aa40(void)
{
    struct Task *t;

    sub_080062c4();
    t = gUnk_03002490;
    if (t->unk72 == 0)
    {
        t->unk04 = 0;
        sub_08006338(0);
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk78 = 127;
    sub_08065e1c(189, 0);
    sub_0806d730();
}

void sub_0806aa80(void)
{
    sub_0806aa40();
}

void sub_0806aa8c(void)
{
    sub_0806aa40();
}

void sub_0806aa98(void)
{
    sub_0806a7a0();
}

void sub_0806aaa4(void)
{
    struct Task *t;
    u16 x;

    x = gUnk_03001270;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0(gUnk_0873E698);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0(&x);
        TaskYieldTrampoline(2);
        t = gUnk_03002490;
        t->unk6C = t->unk6C + 1;
    } while ((s16)t->unk6C <= 2);
    sub_080670f0(&x);
}

void sub_0806ab34(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;
    u8 zb;

    t = gUnk_03002490;
    a = t->unk8C;
    zero = 0;
    t->unk04 = zero;
    t->unk88 = (struct PlayerState *)zero;
    sub_08006338(0);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk12 = 1;
    u = gUnk_03002490;
    zb = 0;
    u->unk78 = 127;
    u->unk04 = (u32)sub_0806aba4;
    a->unk48 = (u32)&gUnk_0873F6E8;
    u->unk80 = zb;
    v = gUnk_03002490;
    v->unk2C = zero;
    sub_08064d34(163, 1);
    sub_080261d4(2);
    sub_08065e1c(189, 0);
    sub_0806d730();
}

void sub_0806aba4(void)
{
    struct Task *u;

    if (gUnk_03002490->unk2C <= 15)
    {
        sub_08068e04();
        sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873F81C);
        u = gUnk_03002490;
        u->unk2C = u->unk2C + 1;
    }
}

void sub_0806abec(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gUnk_03002490;
    a = t->unk8C;
    zero = 0;
    t->unk04 = zero;
    sub_08006338(0);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk12 = 1;
    u = gUnk_03002490;
    u->unk78 = 127;
    u->unk04 = (u32)sub_0806ac48;
    a->unk48 = (u32)&gUnk_0873F704;
    u->unk2C = zero;
    sub_080261d4(2);
    sub_08065e1c(189, 0);
    sub_0806d730();
}

void sub_0806ac48(void)
{
    struct Task *u;

    if (gUnk_03002490->unk2C <= 15)
    {
        sub_08068e04();
        u = gUnk_03002490;
        u->unk2C = u->unk2C + 1;
    }
}

void sub_0806ac6c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    if (t->unk72 == 0)
    {
        t->unk04 = 0;
        sub_08006338(0);
        TaskYieldTrampoline(1);
    }
    u = gUnk_03002490;
    u->unk78 = 127;
    zero = 0;
    u->unk04 = (u32)sub_0806acc4;
    sub_08065e1c(109, 0);
    v = gUnk_03002490;
    v->unk38 = gUnk_0874C9D8;
    v->unk40 = zero;
    sub_0806d65c();
}

void sub_0806acc4(void)
{
}

void sub_0806acc8(void)
{
    struct Task *t;

    sub_080062c4();
    sub_08006338(0);
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    sub_080031b8(125);
    sub_0806d65c();
}

u32 sub_0806acf8(void)
{
    if (gUnk_03002490->unk8C->unk1A == -1)
        return 0;
    return 1;
}
