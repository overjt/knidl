/* game_code_and_rodata 0x0806C2A4-0x0806CD40 (issue #64, module M18 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806C2A4 0x0806CD40 src/actor_6c2a4.c --newpb
 *
 * Class-1 task bodies for the vehicle/ride actors: the launch-and-fall pair
 * (sub_0806c2a4 / sub_0806c930 set Task.unk54/unk58 from the sign in unk43
 * and hand control to sub_080656b4), the star-ride state machine
 * (sub_0806c4a0 / sub_0806c5d4 / sub_0806c770 - a nine-step animation
 * switch over Task.unk46, the gUnk_0873EAC0 speed table and the
 * gUnk_0873EAF0 drift table), and the short spawn-effect bodies that only
 * walk Task.unk3C through a gfx list (gUnk_0874C520 / gUnk_0874CBC8) before
 * TaskDispatchTrampoline.  sub_0806caa0 and sub_0806cc90 are the two helper
 * spawners that fix up Task.unk43 (facing) on the task they created.
 *
 * sub_0806c770 was the hardest function in M18: instruction-identical to the
 * ROM but 34 bytes of a three-way register rotation, caused by a preference
 * exclusion in the allocator rather than a wrong shape.  Its `ka`, `kb`,
 * `tbl` and `p` locals are load-bearing - see the commit message and
 * docs/lessons-learned.md.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080059fc(void);
extern void sub_0801a828(u8 a, s16 x, s16 y, void *p);
extern void sub_0806395c(u8 v);
extern void sub_08063990(u32 v);
extern void sub_080656b4(void);
extern void sub_0806b410(void);
extern void sub_0806b848(void);
extern void sub_0806b878(void);
extern void sub_0806b8bc(void);
extern void sub_0806b938(void);
extern void sub_0806ba9c(void);
extern u32 sub_0806baec(u32 a);
extern void sub_0806bc28(void);
extern void sub_0806bc9c(void);
extern void sub_0806be84(void);
extern void sub_0806bf38(void);

extern s16 gUnk_0300244C;
extern void sub_080061c0(s32 a, s32 b);
extern void sub_080639e0(u32 v);
extern s32 sub_08064d34(u32 type, u8 keepPrio);

extern u8 gUnk_03005550[];
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern u32 sub_0800641c(s16 x, s16 y);
extern u32 gUnk_0874CC84[];
extern void sub_080031b8(u32 a);
extern void sub_080261d4(u32 a);
extern void sub_08030848(void *p, s16 v);
extern void sub_080692fc(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_0806523c(void);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern u32 gUnk_0874C520[];
extern u32 gUnk_0874CBC8[];
extern vs16 gUnk_03004CA0[];
extern void sub_08064a60(void);
extern u8 sub_08065f2c(u32 i);
extern void sub_08005654(s32 i);
extern s8 gUnk_0873EB30[];
extern void sub_0800617c(u32 a);
extern void sub_080062c4(void);
extern s16 sub_080064dc(s32 a, u32 b, u32 c);
extern s32 sub_080064ac(s32 a, u32 b, u32 c);
extern u32 gUnk_0874CB90[];
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void sub_08006338(s32 a);
extern s8 gUnk_0873EB38[];

struct Unk0873EAC0
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
};

extern struct Unk0873EAC0 gUnk_0873EAC0[];
extern s16 gUnk_0873EAF0[];
extern u32 gUnk_0873F8CC[];
extern u32 gUnk_0873F8DC[];
extern u32 gUnk_0873F894[];
extern u32 gUnk_0873F858[];
extern u32 gUnk_0873F86C[];
extern u32 gUnk_0873F8F4[];

void sub_0806c30c(void);
void sub_0806c384(void);
void sub_0806c418(void);
void sub_0806c490(void);
void sub_0806c5d4(void);
void sub_0806c770(void);
void sub_0806c9e8(void);
void sub_0806cbd4(void);
void sub_0806cd30(void);

void sub_0806c2a4(void)
{
    struct Task *t;
    s32 v;
    s32 w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk04 = (u32)sub_0806c30c;
    t->unk08 = 0;
    t->unk34 = 0;
    v = t->unk54 >> 1;
    if (v < 0)
        v = -v;
    t->unk54 = t->unk43 * v;
    w = t->unk58 >> 1;
    if (w < 0)
        w = -w;
    t->unk58 = -w;
    t->unk7C = 0;
    gUnk_03002490->unk80 = 0;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk34 = 1;
    sub_08006138();
}

void sub_0806c30c(void)
{
    if (gUnk_03002490->unk34 != 0)
        sub_0806b8bc();
}

void sub_0806c324(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk08 = (u32)sub_0806c384;
    sub_08063990((u32)gUnk_0873F8F4);
    sub_0806ba9c();
    u = gUnk_03002490;
    u->unk18 = 0;
    u->unk42 = 6;
    while (sub_0806baec(18) == 0)
    {
        sub_0806bc28();
        TaskYieldTrampoline(1);
    }
    sub_0806bc9c();
    sub_0806395c(5);
    sub_08006138();
}

void sub_0806c384(void)
{
    struct Task *t;

    sub_0806b878();
    sub_0806b938();
    t = gUnk_03002490;
    if (t->unk18 == 1)
        sub_0806b8bc();
    else if (t->unk14 != 4)
        sub_08006148(sub_0806bf38, gCurTaskIdx);
    sub_0806be84();
}

void sub_0806c3c4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk04 = (u32)sub_0806c418;
    t->unk08 = (u32)sub_0806c490;
    t->unk50 = 0;
    t->unk4C = 0;
    t->unk20 = t->unk48;
    t->unk1C = t->unk4A;
    t->unk12 = 1;
    sub_08063990((u32)gUnk_0873F8F4);
    sub_08006138();
}

void sub_0806c418(void)
{
    struct Task *t;

    gUnk_03002490->unk78 = 127;
    sub_0806b878();
    sub_0806b410();
    t = gUnk_03002490;
    if (t->unk72 == 1)
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F86C);
    else
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873F858);
}

void sub_0806c490(void)
{
    sub_0806b848();
    sub_0806be84();
}

void sub_0806c4a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 i;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk04 = (u32)sub_0806c5d4;
    t->unk08 = (u32)sub_0806c770;
    t->unk42 = 11;
    if (gUnk_0300244C != 0 && gUnk_03002490->unk8C->unk50 == 0)
        sub_080639e0((u32)gUnk_0873F894);
    u = gUnk_03002490;
    i = (s16)u->unk70 - 3;
    u->unk70 = i;
    u->unk24 = 0;
    sub_080061c0(gUnk_0873EAC0[i].unk00, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = gUnk_0873EAC0[i].unk04;
    v->unk7C = 0;
    gUnk_03002490->unk80 = 0;
    w = gUnk_03002490;
    w->unk46 = 0;
    w->unk34 = 0;
    w->unk20 = 0;
    if (w->unk72 != 1)
    {
        while (1)
        {
        gUnk_03002490->unk3E |= 0x8000;
        sub_08064d34(158, 0);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3E &= 0x7FFF;
        sub_08064d34(158, 0);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3E &= 0x7FFF;
        sub_08064d34(158, 0);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3E |= 0x8000;
        sub_08064d34(158, 0);
        TaskYieldTrampoline(4);
        }
    }
    while (1)
    {
        sub_08064d34(158, 0);
        TaskYieldTrampoline(4);
    }
}

void sub_0806c5d4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk78 = 127;
    if (t->unk72 == 1)
        sub_08030848(gUnk_0873F8DC, t->unk44);
    else
        sub_08030848(gUnk_0873F8CC, t->unk44);
    sub_080692fc();
    if ((*(u32 *)gUnk_03005550 & 0x00FFFFFF) != 0)
    {
        if (gUnk_03005550[1] != 0)
            gUnk_03002490->unk30 = 0;
        if (gUnk_03005550[2] != 0)
            gUnk_03002490->unk30 = 1;
        if (gUnk_03005550[0] != 0)
            gUnk_03002490->unk30 = 2;
        if (gUnk_03005550[4] != 0)
            gUnk_03002490->unk30 = 2;
        sub_080031b8(237);
        sub_080261d4(2);
        gUnk_03002490->unk08 = 0;
        sub_0806395c(7);
        sub_08006148(sub_0806bf38, gCurTaskIdx);
    }
    else
    {
        u = gUnk_03002490;
        if ((u->unk7B & 1) != 0)
        {
            if ((u->unk54 & 0xFFFF0000) != 0)
                sub_080061c0(0x5A5A5A5A, 0xFFFFE800);
            gUnk_03002490->unk60 = 0x1000;
        }
        v = gUnk_03002490;
        if (v->unk72 != 1)
        {
            v->unk48 += gUnk_0873EAF0[v->unk24 * 2];
            v->unk4A += (&gUnk_0873EAF0[1])[v->unk24 * 2];
            v->unk24 = (v->unk24 + 1) & 15;
        }
        if (v->unk72 == 1)
            sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48,
                         gUnk_03002490->unk4A, gUnk_0873F86C);
        else
            sub_0801a828((u8)gCurTaskIdx, v->unk48, v->unk4A, gUnk_0873F858);
    }
}

void sub_0806c770(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *p;
    u32 *tbl;
    s16 dx;
    s16 dy;
    u32 k;
    u32 ka;
    u32 kb;

    switch (gUnk_03002490->unk46)
    {
    case 0:
        t = gUnk_03002490;
        dx = t->unk4C >> 16;
        dy = t->unk50 >> 16;
        if (t->unk72 == 1)
            k = 8;
        else
            k = 0;
        if (t->unk43 == 1)
            t->unk30 = dx - (ka = k + 8);
        else
            t->unk30 = dx + (kb = k + 8);
        u = gUnk_03002490;
        u->unk1C = dy;
        u->unk2C = -1;
        break;
    case 1:
        gUnk_03002490->unk2C = 1;
        break;
    case 2:
        gUnk_03002490->unk2C = 2;
        break;
    case 3:
        gUnk_03002490->unk2C = 3;
        break;
    case 4:
        gUnk_03002490->unk2C = 4;
        break;
    case 5:
        gUnk_03002490->unk2C = 5;
        break;
    case 6:
        gUnk_03002490->unk2C = 6;
        break;
    case 7:
        gUnk_03002490->unk2C = 7;
        break;
    case 8:
        v = gUnk_03002490;
        v->unk2C = -1;
        v->unk34 = 0;
        v->unk20 = 0;
        break;
    }
    w = gUnk_03002490;
    if (w->unk2C != -1)
    {
        if (w->unk46 != 0)
        {
            if (w->unk43 == 1)
                w->unk34 = w->unk34 - 2;
            else
                w->unk34 = w->unk34 + 2;
            x = gUnk_03002490;
            switch ((s16)x->unk70)
            {
            case 0:
                x->unk20 = x->unk20 + 2;
                break;
            case 2:
                x->unk20 = x->unk20 - 2;
                break;
            }
        }
        y = gUnk_03002490;
        if (sub_0800641c((s16)(y->unk30 + y->unk34),
                         (s16)(y->unk1C + y->unk20)) != 0)
        {
            tbl = gUnk_0874CC84;
            z = gUnk_03002490;
            sub_08001a94(z->unk42, tbl[z->unk2C], 0, 0,
                         z->unk30 + z->unk34 - gUnk_03002348,
                         (s16)(z->unk1C + z->unk20 - gUnk_030023E4));
        }
    }
    p = gUnk_03002490;
    p->unk46++;
    if (p->unk46 > 8)
        p->unk46 = 0;
}

void sub_0806c930(void)
{
    struct Task *t;
    s32 v;
    s32 w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk04 = (u32)sub_0806c9e8;
    t->unk08 = 0;
    t->unk34 = 0;
    v = t->unk54 >> 1;
    if (v < 0)
        v = -v;
    t->unk54 = t->unk43 * v;
    switch (t->unk30)
    {
    case 0:
        w = t->unk58 >> 1;
        if (w < 0)
            w = -w;
        t->unk58 = w;
        break;
    case 1:
        w = t->unk58 >> 1;
        if (w < 0)
            w = -w;
        t->unk58 = -w;
        break;
    case 2:
        v = t->unk54 >> 1;
        if (v < 0)
            v = -v;
        t->unk54 = -t->unk43 * v;
        sub_0800622c(0xFFFE0000, 0x4000, 0x20000);
        break;
    }
    gUnk_03002490->unk7C = 0;
    gUnk_03002490->unk80 = 0;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk34 = 1;
    sub_08006138();
}

void sub_0806c9e8(void)
{
    if (gUnk_03002490->unk34 != 0)
        sub_0806b8bc();
}

void sub_0806ca00(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C520;
    u->unk3C = 11;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

s16 sub_0806caa0(u8 kind, s32 dx, s32 dy)
{
    struct Task *t;
    s32 i;
    u16 r;

    switch (kind)
    {
    case 0:
        r = sub_08064cdc(141, (s16)dx, (s16)dy, 0);
        break;
    case 1:
        i = sub_08064d34(142, 0);
        r = i;
        if ((s16)i != -1)
        {
            t = &gUnk_03002790[(s16)i];
            t->unk18 = gUnk_03002490->unk43 * dx;
            t->unk1C = dy;
        }
        break;
    }
    return r;
}

void sub_0806cb10(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CBC8;
    sub_08064a60();
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_0806cb64(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CBC8;
    u->unk04 = (u32)sub_0806cbd4;
    sub_08064a60();
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    } while (++*(s16 *)&gUnk_03002490->unk6C <= 7);
    TaskDispatchTrampoline();
}

void sub_0806cbd4(void)
{
    struct Task *t;
    struct Task *p;
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] != -1 && sub_08065f2c(i) == 1)
    {
        t = gUnk_03002490;
        p = &gUnk_03002790[t->unk44];
        t->unk48 = p->unk48 + t->unk18
                 + gUnk_0873EB30[*(s16 *)&t->unk6C] * t->unk43;
        t->unk4A = p->unk4A + t->unk1C + gUnk_0873EB38[*(s16 *)&t->unk6C];
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d)
{
    struct Task *p;
    s32 i;
    u16 r;

    i = sub_08064cdc(143, (s16)c, (s16)d, 0);
    r = i;
    if ((s16)i != -1)
    {
        p = &gUnk_03002790[(s16)i];
        p->unk18 = vx;
        if (flag == 0)
        {
            if (gUnk_03002490->unk43 == 1)
                p->unk43 = 0xFF;
            else
                p->unk43 = 1;
        }
        else
        {
            p->unk43 = gUnk_03002490->unk43;
        }
        p->unk1C = gUnk_03002490->unk43 * c;
        p->unk20 = d;
    }
    return r;
}

void sub_0806cd30(void)
{
    sub_08064d34(144, 0);
}
