
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u16 gUnk_03001EA4;
extern struct Task gUnk_03002790[];
extern vu16 gUnk_03004CA0[];
extern u32 gUnk_08745CEC[];
extern u32 gUnk_08747ADC[];
extern u32 gUnk_08747AE4[];
extern u32 gUnk_08747AFC[];
extern u32 gUnk_08747B10[];
extern u32 gUnk_08747B24[];
extern u32 gUnk_08747B38[];
extern u32 gUnk_08747B58[];
extern u32 gUnk_08747B68[];
extern u32 gUnk_08747E64[];
extern u32 gUnk_08753270[];
extern u32 gUnk_08753290[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern s32 sub_080031b8(s32 id);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_08063908(u32 def);
extern void sub_0806395c(u16 v);
extern s32 sub_08063cd0(void);
extern s32 sub_08063eb0(struct PointPair *box, s32 i);
extern void sub_08063fe0(void);
extern void sub_08064a60(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern void sub_0806523c(void);
extern u32 sub_08068cf8(s32 a);
extern u32 sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void sub_0809c50c(void);
extern void sub_0809c528(void);
extern void sub_0809c5a4(void);
extern void sub_0809d994(void);
extern u8 sub_0809dbc4(void);
extern u8 sub_0809dc3c(void);
extern void sub_0809f8d4(void);
extern void sub_0809f90c(void);
extern void sub_0809f930(void);
extern void sub_0809f970(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

/* Defined below */
void sub_0809cd4c(void);
void sub_0809cec4(void);
void sub_0809cfe0(void);
void sub_0809d17c(void);
void sub_0809d1dc(void);
void sub_0809d280(void);
void sub_0809d2a4(void);
void sub_0809d6dc(void);
void sub_0809d83c(void);
void sub_0809d8f8(void);
void sub_0809d944(void);

void sub_0809cc24(void)
{
    struct Task *t;
    struct Task *u;
    s32 k;
    s32 z1;
    register s32 z2 asm("r9");
    s32 m;

    gUnk_03002490->unk04 = (u32)sub_0809cd4c;
    sub_0809f8d4();
    sub_080061c0(128 << 10, k = 0x5A5A5A5A);
    t = gUnk_03002490;
    asm("" : "=r"(z1) : "0"(0));
    z2 = 0;
    t->unk3C = 4;
    TaskYieldTrampoline(9);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    sub_080061c0(128 << 9, k);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(7);
    m = 128 << 8;
    sub_080061c0(m, k);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    sub_080061c0(128 << 7, k);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk73 = z1;
    sub_0806395c(0);
    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809c528;
    t->unk15 = z1;
    u = gUnk_03002490;
    u->unk2C = 90;
    u->unk30 = z2;
    sub_080061c0(m, k);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cd4c(void)
{
    struct Task *t;

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
}

void sub_0809cd8c(void)
{
    struct Task *t;
    struct Task *u;
    s32 k;
    s32 z1;
    register s32 z2 asm("r9");
    s32 m;

    gUnk_03002490->unk04 = (u32)sub_0809cec4;
    sub_0809f8d4();
    sub_080061c0(128 << 10, k = 0x5A5A5A5A);
    t = gUnk_03002490;
    asm("" : "=r"(z1) : "0"(0));
    asm("" : "=r"(z2) : "0"(0));
    t->unk3C = 4;
    TaskYieldTrampoline(9);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    sub_080061c0(128 << 9, k);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    m = 128 << 8;
    sub_080061c0(m, k);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    sub_080061c0(128 << 7, k);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk73 = z1;
    sub_0806395c(0);
    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809c528;
    t->unk15 = z1;
    u = gUnk_03002490;
    u->unk2C = 90;
    u->unk30 = z2;
    sub_080061c0(m, k);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cec4(void)
{
    struct Task *t;

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
}

void sub_0809cf04(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    sub_080031b8(186);
    sp.unk00 = 26;
    sp.unk04 = 129;
    sp.unk08 = (t = gUnk_03002490)->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = 20;
    sp.unk0E = 0;
    sp.unk10 = gUnk_08745CEC[0];
    sp.unk0A = 0;
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 1);
}

void sub_0809cf60(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753270;
    u->unk04 = (u32)sub_0809cfe0;
    sub_08064a60();
    sub_080061c0(224 << 10, 0xFFFFDB00);
    gUnk_03002490->unk64 = 128 << 11;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(5);
        TaskYieldTrampoline(2);
    }
}

void sub_0809cfe0(void)
{
    struct PointPair box;
    struct Task *t;
    struct Task *w;
    s32 i;

    t = gUnk_03002490;
    box.x0 = t->unk48 - 10;
    box.y0 = t->unk4A - 4;
    box.x1 = t->unk48 + 10;
    box.y1 = t->unk4A + 4;
    if (gUnk_03004CA0[i = t->unk44] == 58 && gUnk_03002790[i].unk74 == 0
        && sub_08063eb0(&box, i) != 0)
    {
        w = &gUnk_03002790[gUnk_03002490->unk44];
        w->unk18 = 1;
        sub_08063fe0();
    }
    else
    {
        sub_08068e04();
        sub_08069b44();
    }
}

u8 sub_0809d0a0(void)
{
    u8 r;

    if (gUnk_03002490->unk73 != 0)
        r = 0;
    else
    {
        sub_08006244();
        sub_0806395c(0);
        sub_08006148(sub_0809c50c, gCurTaskIdx);
        r = 1;
    }
    return r;
}

u8 sub_0809d0dc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk73 == 0 && t->unk14 == 1)
    {
        t->unk4C -= t->unk54;
        t->unk48 = t->unk4C >> 16;
        sub_08006214();
    }
    else
    {
        u = gUnk_03002490;
        if (u->unk73 != 1)
        {
            sub_0809f930();
        }
        else
        {
            u->unk4C -= u->unk54;
            u->unk48 = u->unk4C >> 16;
            sub_0809f970();
        }
    }
    return 0;
}

u8 sub_0809d138(void)
{
    return 0;
}

void sub_0809d13c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0809d17c;
    t->unk38 = gUnk_08753290;
    sub_08063908((u32)gUnk_08747E64);
    u = gUnk_03002490;
    u->unk24 = 0;
    sub_08002e98(u->unk73, 2, gUnk_08747ADC);
}

void sub_0809d17c(void)
{
    sub_0806523c();
    sub_0809d994();
}

void sub_0809d18c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809d1dc;
    t->unk28 = 3;
    sub_0809f90c();
    sub_0809d8f8();
    sub_0806395c(1);
    u = gUnk_03002490;
    u->unk15 = 0;
    sub_0809d2a4();
}

void sub_0809d1c0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08747AE4);
}

void sub_0809d1dc(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s16 *p;
    s8 *q;
    s32 lim;

    if (sub_080692fc() == 0)
    {
        if (sub_0809f994() != 0)
            sub_0809dbc4();
        t = gUnk_03002490;
        p = &t->unk4A;
        a = t->unk8C;
        q = (s8 *)a->unk50;
        lim = q[2];
        if (*p < lim)
        {
            *p = lim;
            t->unk50 = *p << 16;
            sub_0809dc3c();
        }
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_08747AFC);
    }
    u = gUnk_03002490;
    if (u->unk24 > 0)
    {
        u->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0809d25c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk28 = 3;
    sub_0806395c(1);
    sub_0809d280();
}

void sub_0809d280(void)
{
    gUnk_03002490->unk15 = 0;
    TaskYieldTrampoline(2);
    sub_0809f90c();
    sub_0809d8f8();
    sub_0809d2a4();
}

void sub_0809d2a4(void)
{
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(5);
        TaskYieldTrampoline(4);
        sub_08006338(6);
        TaskYieldTrampoline(4);
        sub_08006338(7);
        TaskYieldTrampoline(4);
        sub_08006338(8);
        TaskYieldTrampoline(4);
        sub_08006338(9);
        TaskYieldTrampoline(4);
        sub_08006338(10);
        TaskYieldTrampoline(4);
        sub_08006338(11);
        TaskYieldTrampoline(4);
    }
}

void sub_0809d308(void)
{
}

void sub_0809d30c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 1;
    sub_080062c4();
    sub_0809f90c();
    sub_0809d6dc();
    gUnk_03002490->unk7A = z;
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    u = gUnk_03002490;
    u->unk58 = 0xFFFF0000;
    u->unk60 = 128 << 5;
    sub_08006338(12);
    TaskYieldTrampoline(2);
    sub_08006338(13);
    TaskYieldTrampoline(2);
    sub_08006338(14);
    TaskYieldTrampoline(2);
    sub_08006338(15);
    TaskYieldTrampoline(8);
    sub_08006338(28);
    TaskYieldTrampoline(6);
    sub_08006338(29);
    TaskYieldTrampoline(4);
    sub_08006338(30);
    TaskYieldTrampoline(4);
    sub_08006338(31);
    TaskYieldTrampoline(4);
    sub_08006338(32);
    TaskYieldTrampoline(4);
    sub_08006338(4);
    TaskYieldTrampoline(4);
    sub_08006338(5);
    TaskYieldTrampoline(4);
    sub_08006338(6);
    TaskYieldTrampoline(4);
    sub_08006338(7);
    TaskYieldTrampoline(4);
    sub_08006338(8);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    TaskYieldTrampoline(4);
    sub_08006338(10);
    TaskYieldTrampoline(4);
    sub_08006338(11);
    TaskYieldTrampoline(3);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0809d42c(void)
{
    struct Task *t;
    u16 v;

    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0809d1c0, gCurTaskIdx);
    t = gUnk_03002490;
    v = t->unk3C;
    if ((u16)(v - 12) <= 3)
        sub_08068cf8(gUnk_08747B10[t->unk3C - 12]);
    else if ((u16)(v - 28) <= 4)
        sub_08068cf8(gUnk_08747B24[t->unk3C - 28]);
}

void sub_0809d4a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 2;
    sub_080062c4();
    sub_0809f90c();
    sub_0809d6dc();
    gUnk_03002490->unk7A = z;
    u = gUnk_03002490;
    u->unk58 = 0xFFFE0000;
    u->unk60 = 128 << 5;
    sub_08006338(4);
    TaskYieldTrampoline(6);
    sub_08006338(5);
    TaskYieldTrampoline(6);
    sub_08006338(6);
    TaskYieldTrampoline(4);
    sub_0809d944();
    sub_08006338(6);
    TaskYieldTrampoline(2);
    while (1)
    {
        sub_08006338(7);
        TaskYieldTrampoline(6);
        sub_08006338(8);
        TaskYieldTrampoline(6);
        sub_08006338(9);
        TaskYieldTrampoline(6);
        sub_08006338(10);
        TaskYieldTrampoline(6);
        sub_08006338(11);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(6);
        sub_08006338(5);
        TaskYieldTrampoline(6);
        sub_08006338(6);
        TaskYieldTrampoline(6);
    }
}

void sub_0809d568(void)
{
}

void sub_0809d56c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 3;
    sub_080062c4();
    sub_0809f90c();
    sub_0809d6dc();
    gUnk_03002490->unk7A = z;
    u = gUnk_03002490;
    u->unk58 = 0xFFFC0000;
    u->unk60 = 128 << 6;
    while (1)
    {
        sub_080031b8(231);
        sub_08006338(17);
        TaskYieldTrampoline(2);
        sub_08006338(18);
        TaskYieldTrampoline(2);
        sub_08006338(19);
        TaskYieldTrampoline(2);
        sub_08006338(20);
        TaskYieldTrampoline(2);
        sub_08006338(21);
        TaskYieldTrampoline(2);
        sub_08006338(22);
        TaskYieldTrampoline(2);
        sub_08006338(23);
        TaskYieldTrampoline(2);
        sub_08006338(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d608(void)
{
    struct Task *t;
    u16 v;

    t = gUnk_03002490;
    v = t->unk3C;
    if ((u16)(v - 17) <= 7)
        sub_08068cf8(gUnk_08747B38[t->unk3C - 17]);
}

void sub_0809d638(void)
{
    gUnk_03002490->unk15 = 4;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    sub_08006338(5);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    sub_08006338(8);
    TaskYieldTrampoline(2);
    sub_08006338(9);
    TaskYieldTrampoline(3);
    sub_08006338(10);
    TaskYieldTrampoline(3);
    sub_08006338(11);
    TaskYieldTrampoline(4);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0809d6b4(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_0809d1c0, gCurTaskIdx);
}

void sub_0809d6dc(void)
{
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(25);
        TaskYieldTrampoline(2);
        sub_08006338(8);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
}

void sub_0809d71c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 1 && (gUnk_03001EA4 & 1) != 0)
    {
        t->unk2C = 1;
        sub_0806395c(4);
        sub_08006148(sub_0809d1c0, gCurTaskIdx);
        return;
    }
    gUnk_03002490->unk2C = 0;
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) > 39)
    {
        sub_0806395c(3);
        sub_08006148(sub_0809d1c0, gCurTaskIdx);
        return;
    }
    sub_0806395c(2);
    sub_08006148(sub_0809d1c0, gCurTaskIdx);
}

void sub_0809d7a4(void)
{
    struct Task *u;

    gUnk_03002490->unk04 = (u32)sub_0809d83c;
    sub_0809f8d4();
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    u = gUnk_03002490;
    u->unk28 = 4;
    u->unk2C = 1;
    while (1)
    {
        sub_080031b8(231);
        sub_08006338(17);
        TaskYieldTrampoline(2);
        sub_08006338(18);
        TaskYieldTrampoline(2);
        sub_08006338(19);
        TaskYieldTrampoline(2);
        sub_08006338(20);
        TaskYieldTrampoline(2);
        sub_08006338(21);
        TaskYieldTrampoline(2);
        sub_08006338(22);
        TaskYieldTrampoline(2);
        sub_08006338(23);
        TaskYieldTrampoline(2);
        sub_08006338(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d83c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gUnk_03002490->unk30 = 1;
    sub_080692fc();
    if (sub_0809f994() != 0)
    {
        sub_0809dbc4();
        gUnk_03002490->unk30 = 0;
    }
    if (gUnk_03002490->unk4A <= 15)
    {
        sub_08006244();
        u = gUnk_03002490;
        u->unk50 = 128 << 13;
        u->unk4A = 16;
        if (u->unk2C == 0)
            u->unk30 = 0;
    }
    v = gUnk_03002490;
    if (v->unk30 != 0)
    {
        if (v->unk2C == 0)
            v->unk58 += 0xFFFFFB00;
        else
            v->unk58 += 160 << 3;
    }
    gUnk_03002490->unk7A = 0;
    w = gUnk_03002490;
    if (w->unk24 > 0)
    {
        w->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
    sub_08068cf8(gUnk_08747B38[gUnk_03002490->unk3C - 17]);
}

void sub_0809d8f8(void)
{
    struct Task *t;
    u32 r;

    r = (u8)sub_08002ee8(8);
    sub_080061c0(gUnk_08747B58[r >> 1], 0x5A5A5A5A);
    t = gUnk_03002490;
    t->unk58 = gUnk_08747B68[r];
    t->unk60 = 128 << 5;
    t->unk7A = 0;
}

void sub_0809d944(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    sp.unk00 = 29;
    sp.unk04 = 132;
    sp.unk08 = (t = gUnk_03002490)->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = 0xF110;
    sp.unk0A = 0;
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 1);
}
