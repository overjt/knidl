/* game_code_and_rodata 0x0809113C-0x08091E18 (issue #67, module M25 batch 2a).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0809113C 0x08091E18 src/enemy_9113c.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u32 gUnk_02007D00[];
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern struct AnimCmd gUnk_0874397C[];
extern u32 gUnk_08743988[];
extern u32 gUnk_087439A4[];
extern u32 gUnk_087440F4[];
extern struct AnimCmd *gUnk_08743A00[];
extern u32 gUnk_08743ADC[];
extern u32 gUnk_087535FC[];
extern u32 gUnk_08753128[];
extern u32 gUnk_08753148[];
extern u8 gUnk_087438DC[];
extern u32 gUnk_087438E4[];
extern u32 gUnk_087438EC[];
extern u32 gUnk_0874391C[];
extern u32 gUnk_0874394C[];
extern struct GfxHeader gUnk_0827565C;
extern u32 gUnk_08275670;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_08064a60(void);
extern u16 sub_08066630(u16 a);
extern void sub_080651b4(void);
extern void sub_080653ec(void);
extern void sub_080656b4(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_080031b8(s32 id);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern void sub_08063e14(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066480(struct GfxHeader *h, u32 src, u32 size);
extern void sub_080664cc(struct GfxHeader *h);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 sub_08068cf8(s32 a);
extern u8 sub_080692fc(void);
extern s32 sub_08069b44(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern void sub_0806a344(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern s32 Div(s32 numerator, s32 denominator);

/* Defined below */
void sub_0809118c(void);
void sub_080911a8(void);
void sub_08091954(void);
void sub_08091a30(void);
void sub_08091a98(void);
void sub_08091d24(void);
void sub_08091e18(void);

void sub_0809113c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080911a8;
    t->unk2C = 0;
    t->unk30 = 0;
    t->unk34 = 0;
    gUnk_03002490->unk46 = sub_08064d34(179, 1);
    sub_080666cc(gUnk_0874397C);
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08743988);
}

void sub_0809118c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08743988);
}

void sub_080911a8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk18 != 0)
    {
        t->unk18--;
        sub_08002e98(t->unk15, 7, gUnk_087439A4);
    }
    else if (sub_0806acf8() == 0)
    {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 7, gUnk_087439A4);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 7, gUnk_087439A4);
    }
    u = gUnk_03002490;
    if (u->unk34 != 0)
    {
        if (u->unk75 != 1)
            sub_08066480(&gUnk_0827565C, (u32)&gUnk_08275670, 16);
        else
        {
            u->unk34 = 0;
            sub_080664cc(&gUnk_0827565C);
        }
    }
    sub_080639b4(gUnk_087438EC[gUnk_03002490->unk3C]);
    sub_080639f0(gUnk_0874391C[gUnk_03002490->unk3C]);
    sub_08063a00(gUnk_0874394C[gUnk_03002490->unk3C]);
    sub_08068f68();
    sub_08069b44();
}

void sub_0809128c(void)
{
    struct Task *t;
    u8 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    if (sub_08067060() == 1)
    {
        gUnk_03002490->unk7A = zero;
        sub_08006338(4);
        sub_0800622c(0, 5376, 196608);
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080031b8(528);
        sub_080062c4();
    }
    sub_08066580();
    sub_0806395c(1);
    sub_08006138();
}

void sub_080912f8(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_08091320(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk20 = zero;
    gUnk_03002490->unk1C =
        gUnk_087438DC[sub_08002ee8(4) + gUnk_03002490->unk74 * 4];
    sub_08091954();
    sub_0806395c(2);
    sub_08006138();
}

void sub_08091368(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_08091390(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *p2;
    s32 zero;
    s32 n;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 2;
    gUnk_03002490->unk7A = zero;
    sub_0800622c(0xFFFD0000, 4096, 196608);
    if ((sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 63)
        gUnk_03002490->unk1C = 1;
    else
        gUnk_03002490->unk1C = 0;
    gUnk_03002490->unk20 = -1;
    p = &gUnk_03002790[(s16)sub_08064d34(180, 1)];
    p->unk18 = gUnk_03002490->unk1C;
    gUnk_03002490->unk30 = 2;
    sub_08006338(5);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C -= 1;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(4);
    v = gUnk_03002490;
    if (v->unk1C == 0)
    {
        v->unk20 = 4;
    }
    else
    {
        p2 = v;
        p2->unk24 = (sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0());
        w = gUnk_03002490;
        n = w->unk24;
        if (n <= 47)
            w->unk20 = 64;
        else if (n <= 79)
            w->unk20 = 1;
        else
            w->unk20 = 36;
    }
    while (1)
    {
        if (gUnk_03002490->unk20-- == 0)
            break;
        TaskYieldTrampoline(1);
    }
    sub_08006338(4);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C -= 2;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(528);
    sub_0806395c(3);
    sub_08006138();
}

void sub_08091558(void)
{
    struct Task *t;

    if (gUnk_03002490->unk20 != 0)
    {
        sub_08063e14();
        sub_08006304();
    }
    t = gUnk_03002490;
    if (t->unk58 > 0 && t->unk30 == 2)
        t->unk30 = 3;
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_080915a4(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 3;
    u = gUnk_03002490;
    u->unk20 = zero;
    u->unk1C = 2;
    sub_08091954();
    sub_0806395c(4);
    sub_08006138();
}

void sub_080915d0(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_080915f8(void)
{
    struct Task *t;
    u8 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 4;
    sub_08063e14();
    gUnk_03002490->unk7A = zero;
    sub_080061c0(81920, 0x5A5A5A5A);
    sub_0800622c(0xFFFEC000, 4096, 196608);
    gUnk_03002490->unk24 = 6;
    sub_08091a30();
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080031b8(528);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFEC000, 0x5A5A5A5A);
    sub_0800622c(0xFFFE8000, 4096, 196608);
    sub_08091a98();
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(528);
    sub_0806395c(5);
    sub_08006138();
}

void sub_080916c4(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_080916ec(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 5;
    gUnk_03002490->unk7A = zero;
    sub_080061c0(163840, 0x5A5A5A5A);
    sub_0800622c(0xFFFE4000, 4096, 196608);
    gUnk_03002490->unk24 = 7;
    sub_08091a30();
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080031b8(528);
    gUnk_03002490->unk7A = 0;
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 55)
    {
        sub_080061c0(0xFFFD8000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFE4000;
    }
    else
    {
        sub_08006214();
        u = gUnk_03002490;
        u->unk58 = 0xFFFE0000;
        u->unk24 = 10;
    }
    sub_08091a98();
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(528);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080917fc(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_0809118c, gCurTaskIdx);
}

void sub_08091824(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 6;
    if (--gUnk_02007D00[0] == 0)
        sub_0806684c();
    sub_080667c0(0, 10);
    sub_0806caa0(1, 0, 0);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 6656, 196608);
    sub_08006338(10);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080261d4(2);
    sub_080031b8(0x1F7);
    sub_0806cc90(0, 4, 16, 4);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    sub_0800622c(0xFFFF0000, 32768, 196608);
    sub_08006338(11);
    TaskYieldTrampoline(32);
    sub_080062c4();
    TaskYieldTrampoline(170);
    sub_08006244();
    sub_0806ad18();
    gUnk_03002490->unk2C = 2;
    sub_08006138();
}

void sub_0809191c(void)
{
    sub_08066480(&gUnk_0827565C, (u32)&gUnk_08275670, 16);
    if (gUnk_03002490->unk2C == 2)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_08091954(void)
{
    struct Task *u;
    struct Task *v;

    while (gUnk_03002490->unk1C-- > 0)
    {
        sub_08063e14();
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 39)
            gUnk_03002490->unk20 = 1;
        gUnk_03002490->unk7A = 0;
        sub_080061c0(gUnk_087438E4[gUnk_03002490->unk20], 0x5A5A5A5A);
        sub_0800622c(0xFFFEE000, 4096, 196608);
        u = gUnk_03002490;
        u->unk24 = 3;
        if (u->unk20 == 0)
            sub_08091a30();
        else
            sub_08091a98();
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        v = gUnk_03002490;
        v->unk20 ^= 1;
        sub_080031b8(528);
    }
    sub_080062c4();
}

void sub_08091a30(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gUnk_03002490;
    t->unk30 = 0;
    sub_08006338(4);
    TaskYieldTrampoline(gUnk_03002490->unk24);
    u = gUnk_03002490;
    u->unk3C += 2;
    TaskYieldTrampoline(u->unk24);
    v = gUnk_03002490;
    v->unk3C += 2;
    TaskYieldTrampoline(v->unk24);
    w = gUnk_03002490;
    w->unk3C += 1;
    TaskYieldTrampoline(w->unk24);
    x = gUnk_03002490;
    x->unk3C -= 2;
    TaskYieldTrampoline(x->unk24);
    y = gUnk_03002490;
    y->unk3C -= 2;
    TaskYieldTrampoline(y->unk24);
}

void sub_08091a98(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gUnk_03002490;
    t->unk30 = 1;
    sub_08006338(5);
    TaskYieldTrampoline(gUnk_03002490->unk24);
    u = gUnk_03002490;
    u->unk3C += 2;
    TaskYieldTrampoline(u->unk24);
    v = gUnk_03002490;
    v->unk3C += 2;
    TaskYieldTrampoline(v->unk24);
    w = gUnk_03002490;
    w->unk3C -= 1;
    TaskYieldTrampoline(w->unk24);
    x = gUnk_03002490;
    x->unk3C -= 2;
    TaskYieldTrampoline(x->unk24);
    y = gUnk_03002490;
    y->unk3C -= 2;
    TaskYieldTrampoline(y->unk24);
}

s32 sub_08091b00(void)
{
    gUnk_03002490->unk34 = 1;
    sub_0806caa0(1, 0, -8);
    sub_080261d4(2);
    return 0;
}

s32 sub_08091b24(void)
{
    sub_08006338(10);
    sub_080639a4(gUnk_087440F4);
    gUnk_03002490->unk2C = 1;
    sub_0806395c(6);
    sub_08006148(sub_0809118c, gCurTaskIdx);
    return 1;
}

s32 sub_08091b60(void)
{
    sub_08006214();
    return 0;
}

void sub_08091b6c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *p;
    struct Task *q;
    struct ActorSpawn spawn;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk00 = zero;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 14;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753148;
    u->unk04 = (u32)sub_08091d24;
    sub_08064a60();
    v = gUnk_03002490;
    v->unk2C = 0x100000;
    v->unk30 = zero;
    v->unk34 = 0x40000;
    v->unk1C = zero;
    spawn.unk00 = 9;
    spawn.unk04 = 111;
    spawn.unk08 = v->unk18;
    spawn.unk09 = v->unk74;
    spawn.unk0C = zero;
    spawn.unk0E = zero;
    spawn.unk0A = 1;
    spawn.unk10 = sub_08066630(1);
    gUnk_03002490->unk46 = sub_08064b5c(&spawn, 1);
    gUnk_03002490->unk28 = 0x30000;
    gUnk_03002490->unk30 = 0xFFFA0000;
    sub_08006338(5);
    gUnk_03002490->unk6C = zero;
    do
    {
        x = gUnk_03002490;
        x->unk28 += 0xFFFF7000;
        x->unk30 += 0x8000;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 15);
    y = gUnk_03002490;
    y->unk28 = 0;
    y->unk30 = 0;
    p = gUnk_03002490;
    while ((q = &gUnk_03002790[p->unk44])->unk20 != 0)
    {
        TaskYieldTrampoline(1);
        p = gUnk_03002490;
    }
    z = gUnk_03002490;
    z->unk28 = 0x88000;
    z->unk30 = 0xFFFC0000;
    z->unk6C = 0;
    do
    {
        a = gUnk_03002490;
        a->unk28 += 0xFFFF0000;
        a->unk30 += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    b = gUnk_03002490;
    b->unk1C++;
    sub_080031b8(0x20F);
    sub_08006338(1);
    gUnk_03002490->unk6C = 0;
    do
    {
        c = gUnk_03002490;
        c->unk28 += 0xFFFF0000;
        c->unk30 += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    d = gUnk_03002490;
    d->unk28 = 0xFFFF0000;
    d->unk30 = 0xFFFF0000;
    sub_08006338(3);
    TaskYieldTrampoline(6);
    TaskDispatchTrampoline();
}

void sub_08091d24(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 m;

    if ((s16)gUnk_03004CA0[gUnk_03002490->unk44] != -1)
    {
        t = gUnk_03002490;
        u = &gUnk_03002790[t->unk44];
        if (u->unk76 == 1 && u->unk2C == 0)
        {
            t->unk43 = u->unk43;
            sub_08006304();
            v = gUnk_03002490;
            v->unk2C += v->unk28;
            v->unk34 += v->unk30;
            v->unk48 = u->unk48 + (v->unk2C * u->unk43 >> 16);
            m = ((s16 *)v)[27];
            v->unk4A = u->unk4A + m;
        }
        else
        {
            sub_08005654(gCurTaskIdx);
        }
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_08091ddc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753128;
    u->unk04 = (u32)sub_08091e18;
    u->unk30 = -1;
    sub_08006138();
}

