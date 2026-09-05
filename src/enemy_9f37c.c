
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02007D00[];
extern s16 gUnk_03002158[];
extern s32 gUnk_030023B4;
extern u32 gUnk_08747C6C[];
extern u32 gUnk_087534E0[];
extern u32 gUnk_087535A8[];

/* Externals */
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern s32 sub_080031b8(s32 id);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_08064970(void);
extern void sub_08064a60(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern u32 sub_08068e04(void);
extern u32 sub_08069b44(void);
extern void sub_0809e8b0(void);
extern void sub_0809ec84(void);

/* Defined below */
void sub_0809f3e0(void);
void sub_0809f818(s32 v);
void sub_0809f874(void);
void sub_0809f90c(void);
void sub_0809f930(void);

void sub_0809f37c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    u8 *p;
    s32 z;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    p = &t->unk42;
    z = 0;
    *p = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_087534E0;
    u->unk04 = (u32)sub_0809f3e0;
    sub_08064a60();
    v = gUnk_03002490;
    v->unk28 = z;
    sub_08002e98(v->unk73, 5, gUnk_08747C6C);
    w = gUnk_03002490;
    w->unk60 = 148 << 6;
    w->unk68 = 192 << 10;
    sub_08006138();
}

void sub_0809f3e0(void)
{
    struct Task *t;
    struct Task *u;
    s32 vy;
    s32 n;
    u16 h;

    t = gUnk_03002490;
    if (t->unk4A > gUnk_03002158[2] + 196)
    {
        sub_08063fe0();
        return;
    }
    vy = t->unk58;
    if (vy < 0)
    {
        if (abs(t->unk54) >= -vy)
            sub_08006338(8);
        else
            sub_08006338(4);
    }
    else
    {
        if (abs(t->unk54) >= vy)
            sub_08006338(10);
        else
            sub_08006338(6);
    }
    u = gUnk_03002490;
    n = u->unk28 + 1;
    u->unk28 = n;
    if ((n & 2) != 0)
    {
        h = u->unk3C;
        u->unk3C = (0xFE & h) + ((h + 1) & 1);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0809f478(void)
{
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
}

void sub_0809f49c(void)
{
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
}

void sub_0809f4c0(void)
{
    sub_080061c0(208 << 9, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
}

void sub_0809f4e4(void)
{
    sub_080061c0(136 << 10, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
}

void sub_0809f508(void)
{
    sub_080061c0(128 << 8, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF0000;
}

u8 sub_0809f52c(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 1:
    case 2:
    case 3:
    zero:
        return 0;
    case 0:
        sub_08006244();
        gUnk_03002490->unk15 = 0;
        if (gUnk_03002490->unk14 == 0)
            goto zero;
        sub_0809f90c();
        gUnk_03002490->unk28 = 1;
        sub_0806395c(0);
        sub_08006148(sub_0809e8b0, gCurTaskIdx);
        return 1;
    }
}

u8 sub_0809f588(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s8 *p;
    s32 vx;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 2:
    case 3:
        p = &t->unk43;
        *p = -*p;
        u = gUnk_03002490;
        vx = -u->unk54;
        u->unk54 = vx;
        if (u->unk43 == 1 && vx > 0)
            goto zero;
        break;
    case 0:
        if (t->unk14 == 0)
            sub_0809ec84();
        else
            sub_08006214();
        return 0;
    case 1:
        sub_0809f930();
        return 0;
    default:
        goto end;
    }
    v = gUnk_03002490;
    if (v->unk43 == -1 && v->unk54 < 0)
    {
    zero:
        gUnk_03002490->unk28 = 0;
    }
    else
    {
        gUnk_03002490->unk28 = 1;
    }
    return 0;
end:
    ;
}

u8 sub_0809f618(void)
{
    return 0;
}

void sub_0809f61c(void)
{
    struct Task *t;
    struct Task *u;
    u8 *p;
    s32 z;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        TaskYieldTrampoline(30);
        break;
    case 1:
        TaskYieldTrampoline(60);
        break;
    case 2:
        TaskYieldTrampoline(120);
        break;
    case 3:
        TaskYieldTrampoline(180);
        break;
    case 4:
        TaskYieldTrampoline(240);
        break;
    case 5:
        t = gUnk_03002490;
        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_0806523c;
        p = &t->unk42;
        z = 0;
        *p = 11;
        u = gUnk_03002490;
        u->unk38 = gUnk_087535A8;
        u->unk40 = z;
        sub_080031b8(142 << 1);
        gUnk_03002490->unk3C = z;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        break;
    }
    sub_0809f818(0);
    TaskDispatchTrampoline();
}

void sub_0809f7e4(void)
{
    sub_0809f818(-1);
    TaskDispatchTrampoline();
}

void sub_0809f7f8(void)
{
    gUnk_03002490->unk24 = 18;
}

void sub_0809f808(void)
{
    sub_0809f818(0);
    sub_0809f874();
}

void sub_0809f818(s32 v)
{
    switch (gUnk_03002490->unk6E)
    {
    case 0:
        gUnk_02007D00[0] = v;
        break;
    case 1:
        gUnk_02007D00[1] = v;
        break;
    case 2:
        gUnk_02007D00[2] = v;
        break;
    case 3:
        gUnk_02007D00[3] = v;
        break;
    default:
        while (1)
            ;
    }
}

void sub_0809f874(void)
{
    switch (gUnk_03002490->unk6E)
    {
    case 0:
        gUnk_02007D00[5] |= 1;
        break;
    case 1:
        gUnk_02007D00[5] |= 2;
        break;
    case 2:
        gUnk_02007D00[5] |= 4;
        break;
    case 3:
        gUnk_02007D00[5] |= 8;
        break;
    default:
        while (1)
            ;
    }
}

void sub_0809f8d4(void)
{
    sub_08064970();
    if (gUnk_030023B4 <= 119)
        gUnk_03002490->unk43 = 1;
    else
        gUnk_03002490->unk43 = -1;
    sub_08006304();
}

void sub_0809f90c(void)
{
    sub_08063e14();
    sub_08006304();
}

void sub_0809f91c(void)
{
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
}

void sub_0809f930(void)
{
    struct Task *t;
    s8 f;

    f = gUnk_03002490->unk43;
    sub_08063e14();
    sub_08006304();
    gUnk_03002490->unk43 = -f;
    t = gUnk_03002490;
    t->unk54 = -t->unk54;
}

void sub_0809f960(void)
{
    sub_08063e14();
    sub_08063ff4();
}

void sub_0809f970(void)
{
    struct Task *t;

    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    t = gUnk_03002490;
    t->unk54 = -t->unk54;
    sub_08006304();
}

s32 sub_0809f994(void)
{
    struct Task *t;
    struct Actor *a;
    s16 *p;
    s8 *q;
    s32 h;

    t = gUnk_03002490;
    p = &t->unk48;
    h = *p;
    a = t->unk8C;
    q = (s8 *)a->unk50;
    if (h < q[4] + 24)
    {
        if (t->unk54 < 0)
            return 1;
    }
    else if (h > 288 - q[5])
    {
        if (t->unk54 > 0)
            return 1;
    }
    return 0;
}
