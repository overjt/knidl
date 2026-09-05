
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gUnk_03002158[];
extern struct Task gUnk_03002790[];
extern u8 gUnk_03005550[];
extern u32 gUnk_08745CEC[];
extern u32 gUnk_08747BCC[];
extern u32 gUnk_08747BD8[];
extern u32 gUnk_08747BE0[];
extern u32 gUnk_08747BE8[];
extern u32 gUnk_08747BF4[];
extern u32 gUnk_08747C04[];
extern u32 gUnk_08747C14[];
extern u8 gUnk_08747C28[];
extern u32 gUnk_08747DB4[];
extern u32 gUnk_08747E0C[];
extern u32 gUnk_08753354[];
extern u32 gUnk_08753378[];
extern u32 gUnk_08753404[];
extern u32 gUnk_08753414[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern s32 sub_080031b8(s32 id);
extern void sub_080059a0(void);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_08063908(u32 def);
extern void sub_0806395c(u16 v);
extern s32 sub_08063cd0(void);
extern void sub_08063fe0(void);
extern u8 sub_08064358(void);
extern void sub_08064a60(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064ba8(struct ActorSpawn *p, u8 keepPrio);
extern void sub_0806523c(void);
extern u32 sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void sub_0809d1c0(void);
extern void sub_0809d71c(void);
extern void sub_0809f2f4(void);
extern void sub_0809f8d4(void);
extern void sub_0809f90c(void);
extern void sub_0809f930(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

/* Defined below */
void sub_0809da9c(void);
void sub_0809dd08(void);
void sub_0809ddbc(void);
void sub_0809de54(void);
void sub_0809dfc8(void);
void sub_0809e04c(void);
s32 sub_0809e214(void);
void sub_0809e2c4(void);
void sub_0809e630(void);
void sub_0809e780(void);
void sub_0809e864(void);
void sub_0809e8cc(void);
void sub_0809e914(void);
void sub_0809ebc0(void);
void sub_0809ec2c(void);
void sub_0809ec84(void);
void sub_0809ed08(void);
void sub_0809ed74(void);
void sub_0809ef98(void);
void sub_0809f0f0(void);
void sub_0809f26c(void);
s32 sub_0809f29c(s32 a);

void sub_0809da1c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753354;
    u->unk04 = (u32)sub_0809da9c;
    sub_08064a60();
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    if ((u8)sub_08064358() == 2)
        gUnk_03002490->unk58 = 0xFFFC8000;
    else
        gUnk_03002490->unk58 = 0xFFFF0000;
    gUnk_03002490->unk60 = 184 << 5;
    sub_08006138();
}

void sub_0809da9c(void)
{
    struct Task *t;
    s32 vx;
    s32 vy;

    t = gUnk_03002490;
    if (t->unk4A > gUnk_03002158[2] + 196)
    {
        sub_08063fe0();
        return;
    }
    vy = t->unk58;
    if (vy < 0)
    {
        vx = t->unk54;
        if (abs(vx) >> 2 < -vy >> 2)
            sub_08006338(4);
        else if (abs(vx) >> 1 < -vy)
            sub_08006338(5);
        else
            sub_08006338(6);
    }
    else
    {
        vx = t->unk54;
        if (abs(vx) >> 2 < vy >> 2)
            sub_08006338(8);
        else if (abs(vx) >> 1 < vy)
            sub_08006338(7);
        else
            sub_08006338(6);
    }
    sub_08068e04();
    sub_08069b44();
}

u8 sub_0809db48(void)
{
    struct Task *t;
    struct Task *u;
    u8 s;
    s32 n;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_08006244();
        t = gUnk_03002490;
        s = t->unk14;
        if (s == 1)
        {
            if (--t->unk28 > 0)
            {
                n = 1;
            }
            else
            {
                sub_0809d71c();
                return 0;
            }
        }
        else if (s == 3)
        {
            n = 0;
        }
        else if (s == 4)
        {
            n = 5;
        }
        else
        {
            break;
        }
        sub_0806395c(n);
        sub_08006148(sub_0809d1c0, gCurTaskIdx);
        return 1;
    case 1:
        sub_08006244();
        u = gUnk_03002490;
        if (u->unk2C == 1)
            u->unk30 = 0;
        return 0;
    }
}

u8 sub_0809dbc4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        if (t->unk14 == 1)
            t->unk54 = -t->unk54;
        else
            sub_08006214();
        return 0;
    case 1:
        if ((t->unk7A & 1) == 0 || (gUnk_03005550[3] & 1) == 0)
        {
            t->unk54 = -t->unk54;
            if (--t->unk28 > 0)
                break;
            t->unk28 = 4;
            t->unk2C = (t->unk2C + 1) & 1;
        }
        gUnk_03002490->unk30 = 0;
        return 0;
    }
}

u8 sub_0809dc3c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        if (t->unk14 == 3)
            t->unk58 = 0;
        return 0;
    case 1:
        sub_08006244();
        u = gUnk_03002490;
        if (u->unk2C == 0)
            u->unk30 = 0;
        return 0;
    }
}

void sub_0809dc7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_08753378;
    sub_08063908((u32)gUnk_08747DB4);
    u = gUnk_03002490;
    u->unk24 = 0;
    sub_08002e98(u->unk73, 3, gUnk_08747BCC);
}

void sub_0809dcbc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0809dd08;
    sub_0809f8d4();
    while (1)
    {
        sub_08006338(9);
        TaskYieldTrampoline(8);
        sub_08006338(10);
        TaskYieldTrampoline(12);
        sub_08006338(11);
        TaskYieldTrampoline(8);
        sub_08006338(10);
        TaskYieldTrampoline(12);
        sub_0809e04c();
    }
}

void sub_0809dd08(void)
{
    struct Task *t;
    s32 i;
    struct Task *w;
    s8 v;
    s16 *p;

    sub_080692fc();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
    v = gUnk_03002490->unk7C;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gUnk_03002490->unk46;
        if (*p != -1)
        {
            w = &gUnk_03002790[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809dd7c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0809ddbc;
    sub_0809f8d4();
    sub_0806395c(0);
    sub_0809de54();
}

void sub_0809dda0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08747BD8);
}

void sub_0809ddbc(void)
{
    struct Task *t;
    s32 i;
    struct Task *w;
    s8 v;
    s16 *p;

    if (sub_080692fc() == 0)
    {
        if (sub_0809f994() != 0)
            sub_0809f930();
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08747BE0);
    }
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
    v = gUnk_03002490->unk7C;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gUnk_03002490->unk46;
        if (*p != -1)
        {
            w = &gUnk_03002790[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809de54(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080061c0(128 << 7, 0x5A5A5A5A);
    sub_08006338(6);
    TaskYieldTrampoline(3);
    sub_08006338(5);
    TaskYieldTrampoline(5);
    sub_08006338(4);
    TaskYieldTrampoline(6);
    sub_08006338(26);
    TaskYieldTrampoline(5);
    sub_08006338(6);
    TaskYieldTrampoline(3);
    sub_08006338(7);
    TaskYieldTrampoline(5);
    sub_08006338(8);
    TaskYieldTrampoline(6);
    sub_08006338(27);
    TaskYieldTrampoline(5);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0809dee0(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0809dda0, gCurTaskIdx);
}

void sub_0809df08(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_0809e04c();
    sub_0806395c(0);
    sub_08006138();
}

void sub_0809df2c(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0809dda0, gCurTaskIdx);
}

void sub_0809df54(void)
{
    gUnk_03002490->unk04 = (u32)sub_0809dfc8;
    sub_0809f8d4();
    sub_080061c0(176 << 9, 0x5A5A5A5A);
    sub_0809e214();
    while (1)
    {
        sub_08006338(15);
        TaskYieldTrampoline(3);
        sub_08006338(13);
        TaskYieldTrampoline(4);
        sub_08006338(12);
        TaskYieldTrampoline(4);
        sub_08006338(14);
        TaskYieldTrampoline(3);
        sub_08006338(16);
        TaskYieldTrampoline(4);
        sub_08006338(12);
        TaskYieldTrampoline(4);
    }
}

void sub_0809dfc8(void)
{
    struct Task *t;
    struct Task *w;
    s16 *p;
    s8 v;

    if (sub_080692fc() == 0 && sub_0809f994() != 0)
        sub_0809f930();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
    v = gUnk_03002490->unk7C;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gUnk_03002490->unk46;
        if (*p != -1)
        {
            w = &gUnk_03002790[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809e04c(void)
{
    s32 k;

    sub_08006338(28);
    TaskYieldTrampoline(1);
    sub_08006338(29);
    TaskYieldTrampoline(2);
    sub_08006338(30);
    TaskYieldTrampoline(2);
    sub_08006338(31);
    TaskYieldTrampoline(2);
    sub_08006338(32);
    TaskYieldTrampoline(2);
    sub_08006338(33);
    TaskYieldTrampoline(3);
    sub_08006338(34);
    TaskYieldTrampoline(16);
    sub_0809e214();
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_08006338(13);
    TaskYieldTrampoline(4);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(14);
    TaskYieldTrampoline(3);
    sub_08006338(16);
    TaskYieldTrampoline(4);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_08006338(13);
    TaskYieldTrampoline(4);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(14);
    TaskYieldTrampoline(3);
    sub_08006338(16);
    TaskYieldTrampoline(4);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_08006338(13);
    TaskYieldTrampoline(4);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(14);
    TaskYieldTrampoline(3);
    sub_08006338(16);
    TaskYieldTrampoline(4);
    sub_0809f90c();
    sub_08006338(17);
    TaskYieldTrampoline(3);
    sub_08006338(18);
    TaskYieldTrampoline(3);
    sub_08006338(24);
    TaskYieldTrampoline(60);
    sub_08006338(25);
    TaskYieldTrampoline(4);
    sub_08006338(21);
    TaskYieldTrampoline(2);
    sub_080061c0(0xFFFC0000, k = 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    sub_080061c0(0xFFFE0000, k);
    sub_08006338(20);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 10, k);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 11, k);
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(10);
}

s32 sub_0809e214(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s16 *p;
    s32 z;
    s32 r;

    sp.unk00 = 27;
    sp.unk04 = 130;
    sp.unk08 = (t = gUnk_03002490)->unk73;
    z = 0;
    sp.unk09 = t->unk74;
    sp.unk0C = z;
    sp.unk0E = 0xFFF0;
    sp.unk10 = gUnk_08745CEC[2];
    sp.unk0A = 0;
    r = sub_08064ba8(&sp, 1);
    p = &gUnk_03002490->unk46;
    *p = r;
    ((struct Task *)(*p * 144 + (s32)gUnk_03002790))->unk2C = z;
}

void sub_0809e284(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_0809e2c4;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_08753404;
    sub_08064a60();
    u = gUnk_03002490;
    sub_08002e98(u->unk73, 3, gUnk_08747BE8);
}

void sub_0809e2c4(void)
{
    struct Task *t;
    s16 *q;

    sub_080059a0();
    t = gUnk_03002490;
    q = &t->unk44;
    t->unk48 = ((struct Task *)(*q * 144 + (s32)gUnk_03002790))->unk48 + (t->unk4C >> 16);
    t->unk4A = ((struct Task *)(*q * 144 + (s32)gUnk_03002790))->unk4A + (t->unk50 >> 16);
}

void sub_0809e320(void)
{
    struct Task *t;
    struct Task *u;
    s32 k;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809e630;
    t->unk42 = 10;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080031b8(213);
        sub_08006338(1);
        sub_080061c0(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk42 = 12;
        sub_08006338(3);
        sub_080061c0(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080031b8(213);
        sub_08006338(2);
        sub_080061c0(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk42 = 10;
        sub_08006338(0);
        sub_080061c0(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_080031b8(213);
    sub_08006338(1);
    sub_080061c0(128 << 12, k = 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 11, k);
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(3);
    gUnk_03002490->unk42 = 12;
    sub_08006338(3);
    sub_080061c0(0xFFFC0000, k);
    TaskYieldTrampoline(2);
    sub_080061c0(0xFFF80000, k);
    TaskYieldTrampoline(2);
    sub_080031b8(213);
    sub_08006338(2);
    sub_080061c0(0xFFF80000, k);
    TaskYieldTrampoline(2);
    sub_080061c0(0xFFFC0000, k);
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(3);
    gUnk_03002490->unk42 = 10;
    sub_08006338(0);
    sub_08064a60();
    u = gUnk_03002490;
    if (u->unk43 == 1)
        u->unk4C = 0xFFF00000;
    else
        u->unk4C = 128 << 13;
    sub_080061c0(192 << 10, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 12;
    TaskYieldTrampoline(1);
    sub_080061c0(160 << 11, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 160 << 11;
    TaskYieldTrampoline(1);
    sub_080061c0(128 << 12, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 192 << 10;
    TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(213);
    sub_080061c0(192 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080061c0(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080062c4();
    TaskYieldTrampoline(12);
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_080061c0(0xFFFD0000, 0x5A5A5A5A);
    TaskYieldTrampoline(7);
    sub_080062c4();
    TaskYieldTrampoline(2);
    gUnk_03002790[gUnk_03002490->unk44].unk46 = 0xFFFF;
    sub_08063fe0();
}

void sub_0809e630(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 0)
    {
        gUnk_03002790[t->unk44].unk46 = 0xFFFF;
        sub_08063fe0();
    }
    else
    {
        sub_08068e04();
    }
}

void sub_0809e670(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809e780;
    t->unk42 = 12;
    sub_08006338(0);
    gUnk_03002490->unk50 = 0xFFF00000;
    while (1)
    {
        gUnk_03002490->unk4C = 0;
        sub_080031b8(213);
        sub_08006338(1);
        sub_080061c0(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk42 = 12;
        sub_08006338(3);
        sub_080061c0(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080031b8(213);
        sub_08006338(2);
        sub_080061c0(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk42 = 10;
        sub_08006338(0);
        sub_080061c0(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    }
}

void sub_0809e780(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 0)
    {
        gUnk_03002790[t->unk44].unk46 = 0xFFFF;
        sub_08063fe0();
    }
    else
    {
        t->unk50 = 0xFFF00000;
        sub_08068e04();
    }
}

u8 sub_0809e7c8(void)
{
    sub_08006244();
    return 0;
}

u8 sub_0809e7d4(void)
{
    gUnk_03002490->unk60 = 148 << 6;
    return 0;
}

u8 sub_0809e7e8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        break;
    case 1:
        if (t->unk14 == 0)
            sub_0809f930();
        break;
    case 2:
        sub_0809f930();
        break;
    }
    return 0;
}

u8 sub_0809e820(void)
{
    return 0;
}

void sub_0809e824(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0809e864;
    t->unk38 = gUnk_08753414;
    sub_08063908((u32)gUnk_08747E0C);
    u = gUnk_03002490;
    u->unk24 = 0;
    sub_08002e98(u->unk73, 4, gUnk_08747BF4);
}

void sub_0809e864(void)
{
    sub_0806523c();
    sub_0809f2f4();
}

void sub_0809e874(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk04 = (u32)sub_0809e8cc;
    sub_0809f8d4();
    t = gUnk_03002490;
    t->unk2C = 60;
    t->unk28 = 0;
    t->unk7A = 0;
    u = gUnk_03002490;
    u->unk60 = 148 << 6;
    u->unk15 = 4;
    sub_0809e914();
}

void sub_0809e8b0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08747C04);
}

void sub_0809e8cc(void)
{
    struct Task *t;

    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_08747C14);
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0809e914(void)
{
    if (gUnk_03002490->unk28 == 0)
    {
        sub_080061c0(128 << 9, 0x5A5A5A5A);
        while (1)
        {
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(9);
            TaskYieldTrampoline(3);
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(10);
            TaskYieldTrampoline(3);
            sub_08006338(8);
            TaskYieldTrampoline(4);
            sub_08006338(7);
            TaskYieldTrampoline(2);
        }
    }
    else
    {
        sub_080061c0(0xFFFF4000, 0x5A5A5A5A);
        while (1)
        {
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(7);
            TaskYieldTrampoline(2);
            sub_08006338(8);
            TaskYieldTrampoline(4);
            sub_08006338(10);
            TaskYieldTrampoline(3);
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(9);
            TaskYieldTrampoline(3);
        }
    }
}

void sub_0809ea08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gUnk_03002490;
    if (t->unk2C == 61)
    {
        t->unk2C = 60;
        if (t->unk28 != 1)
        {
            t->unk28 = 1;
            sub_0806395c(0);
            sub_08006148(sub_0809e8b0, gCurTaskIdx);
        }
        return;
    }
    if (sub_0809f994() != 0)
    {
        sub_0809ec84();
        return;
    }
    u = gUnk_03002490;
    n = u->unk2C - 1;
    u->unk2C = n;
    if (n > 59)
        return;
    if (n <= 0)
    {
        sub_0809ed08();
        return;
    }
    if ((n & 7) != 7)
        return;
    if (u->unk28 != 0)
        return;
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
    {
        v = gUnk_03002490;
        v->unk28 = 1;
        sub_0806395c(0);
        sub_08006148(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809eab8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 1;
    sub_0809f90c();
    u = gUnk_03002490;
    u->unk2C = 120;
    u->unk7A = z;
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = 0xFFFD0000;
    v->unk60 = 148 << 6;
    sub_08006338(29);
    TaskYieldTrampoline(18);
    sub_08006338(21);
    sub_08006138();
}

void sub_0809eb10(void)
{
}

void sub_0809eb14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 2;
    u = gUnk_03002490;
    u->unk2C = 120;
    sub_080062c4();
    sub_0809ebc0();
    TaskYieldTrampoline(10);
    sub_0806395c(0);
    sub_0809f90c();
    v = gUnk_03002490;
    v->unk28 = 1;
    sub_08006138();
}

void sub_0809eb50(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 2)
    {
        t->unk15 = 0;
        sub_08006148(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809eb7c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 3;
    u = gUnk_03002490;
    u->unk2C = 120;
    sub_080062c4();
    gUnk_03002490->unk7A = z;
    v = gUnk_03002490;
    v->unk58 = 0xFFFD0000;
    v->unk60 = 148 << 6;
    sub_0809ec2c();
    sub_08006138();
}

void sub_0809ebbc(void)
{
}

void sub_0809ebc0(void)
{
    sub_0809f90c();
    sub_08006338(11);
    TaskYieldTrampoline(8);
    sub_08006338(12);
    TaskYieldTrampoline(3);
    sub_08006338(13);
    TaskYieldTrampoline(4);
    sub_08006338(14);
    TaskYieldTrampoline(14);
    sub_0809f29c(0);
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_08006338(27);
    TaskYieldTrampoline(1);
    sub_08006338(28);
    TaskYieldTrampoline(1);
    sub_08006338(16);
}

void sub_0809ec2c(void)
{
    sub_0809f90c();
    sub_08006338(11);
    TaskYieldTrampoline(6);
    sub_08006338(12);
    TaskYieldTrampoline(3);
    sub_08006338(13);
    TaskYieldTrampoline(3);
    sub_08006338(14);
    TaskYieldTrampoline(19);
    sub_0809f29c(0);
    sub_08006338(15);
    TaskYieldTrampoline(5);
    sub_08006338(16);
}

void sub_0809ec80(void)
{
}

void sub_0809ec84(void)
{
    if (gUnk_03002490->unk2C > 59
        || (sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) > 63)
    {
        sub_0809ed74();
    }
    else if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 31)
    {
        sub_0806395c(1);
        sub_08006148(sub_0809e8b0, gCurTaskIdx);
    }
    else
    {
        sub_0806395c(2);
        sub_08006148(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809ed08(void)
{
    gUnk_03002490->unk2C = 60;
    if (sub_08002ee8(2) == 0)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) > 63)
        {
            sub_0806395c(3);
            sub_08006148(sub_0809e8b0, gCurTaskIdx);
        }
        else
        {
            sub_0806395c(2);
            sub_08006148(sub_0809e8b0, gCurTaskIdx);
        }
    }
}

void sub_0809ed74(void)
{
    struct Task *t;
    s32 vx;
    s8 f;

    sub_0809f90c();
    t = gUnk_03002490;
    vx = -t->unk54;
    t->unk54 = vx;
    f = t->unk43;
    if ((f == 1 && vx > 0) || (f == -1 && vx < 0))
        gUnk_03002490->unk28 = 0;
    else
        gUnk_03002490->unk28 = 1;
    sub_0806395c(0);
    gUnk_03002490->unk15 = 0;
    sub_08006148(sub_0809e8b0, gCurTaskIdx);
}

void sub_0809eddc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 k;
    s32 z;
    s32 n;
    s8 *p;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809ef98;
    p = &t->unk7A;
    z = 0;
    *p = 1;
    sub_0809f8d4();
    u = gUnk_03002490;
    u->unk28 = u->unk43;
    sub_080061c0(128 << 10, k = 0x5A5A5A5A);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(9);
    TaskYieldTrampoline(2);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 9, k);
    sub_08006338(18);
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 8, k);
    sub_08006338(21);
    TaskYieldTrampoline(8);
    sub_080061c0(128 << 7, k);
    TaskYieldTrampoline(8);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk43 = v->unk28;
    sub_08006304();
    gUnk_03002490->unk2C = z;
    while (1)
    {
        sub_08006338(23);
        TaskYieldTrampoline(10);
        sub_08006338(24);
        TaskYieldTrampoline(10);
        sub_08006338(25);
        TaskYieldTrampoline(10);
        sub_08006338(23);
        TaskYieldTrampoline(10);
        sub_08006338(24);
        TaskYieldTrampoline(10);
        sub_08006338(25);
        TaskYieldTrampoline(10);
        sub_0809f90c();
        sub_08006338(11);
        TaskYieldTrampoline(8);
        sub_08006338(12);
        TaskYieldTrampoline(3);
        sub_08006338(13);
        TaskYieldTrampoline(4);
        sub_08006338(14);
        TaskYieldTrampoline(14);
        sub_0809f29c(gUnk_08747C28[gUnk_03002490->unk2C]);
        w = gUnk_03002490;
        n = w->unk2C + 1;
        w->unk2C = n;
        if (n > 6)
            w->unk2C = 0;
        sub_08006338(15);
        TaskYieldTrampoline(3);
        sub_08006338(27);
        TaskYieldTrampoline(1);
        sub_08006338(28);
        TaskYieldTrampoline(1);
        sub_08006338(16);
        TaskYieldTrampoline(10);
    }
}

void sub_0809ef98(void)
{
    struct Task *t;

    sub_080692fc();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0809efc8(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s8 *p;
    s32 k;
    s32 z;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809f0f0;
    p = &t->unk7A;
    z = 0;
    *p = 1;
    sub_0809f8d4();
    gUnk_03002490->unk28 = z;
    sub_080061c0(128 << 10, k = 0x5A5A5A5A);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(10);
    TaskYieldTrampoline(2);
    sub_08006338(8);
    TaskYieldTrampoline(3);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(9);
    TaskYieldTrampoline(2);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 9, k);
    sub_08006338(18);
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 8, k);
    sub_08006338(21);
    TaskYieldTrampoline(8);
    sub_080061c0(128 << 7, k);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk73 = z;
    sub_0806395c(0);
    v = gUnk_03002490;
    v->unk04 = (u32)sub_0809e8cc;
    v->unk15 = z;
    w = gUnk_03002490;
    w->unk2C = 60;
    sub_0809e914();
}

void sub_0809f0f0(void)
{
    struct Task *t;

    sub_080692fc();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0809f120(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s8 *p;
    s32 k;
    s32 z;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809f26c;
    p = &t->unk7A;
    z = 0;
    *p = 1;
    sub_0809f8d4();
    gUnk_03002490->unk28 = z;
    sub_080061c0(128 << 10, k = 0x5A5A5A5A);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(9);
    TaskYieldTrampoline(2);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(10);
    TaskYieldTrampoline(2);
    sub_08006338(8);
    TaskYieldTrampoline(3);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(1);
    sub_08006338(9);
    TaskYieldTrampoline(2);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_08006338(22);
    TaskYieldTrampoline(2);
    sub_080061c0(128 << 9, k);
    sub_08006338(18);
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 8, k);
    sub_08006338(21);
    TaskYieldTrampoline(8);
    sub_080061c0(128 << 7, k);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk73 = z;
    sub_0806395c(0);
    v = gUnk_03002490;
    v->unk04 = (u32)sub_0809e8cc;
    v->unk15 = z;
    w = gUnk_03002490;
    w->unk2C = 60;
    sub_0809e914();
}

void sub_0809f26c(void)
{
    struct Task *t;

    sub_080692fc();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
}

s32 sub_0809f29c(s32 a)
{
    struct ActorSpawn sp;
    s32 r;

    sub_080031b8(214);
    sp.unk00 = 28;
    sp.unk04 = 131;
    sp.unk08 = a;
    sp.unk09 = gUnk_03002490->unk74;
    sp.unk0C = 12;
    sp.unk0E = 0xFFEC;
    sp.unk10 = 0xF310;
    sp.unk0A = 0;
    r = sub_08064b5c(&sp, 1);
    gUnk_03002490->unk46 = r;
}
