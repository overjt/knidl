#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* This module's |x|: the ROM tests `< 0` first, unlike global.h's abs(). */
#define ABS(n) ((n) < 0 ? -(n) : (n))

/* RAM cells */
extern s32 gUnk_02007D00[];
extern s32 gUnk_030023D4;
extern u16 gUnk_03002360;
extern struct PlayerState gUnk_03002170[];
extern s16 gUnk_03002158[];
extern u8 gUnk_03001470[];

/* ROM tables */
extern u32 gUnk_082B07BC[];
extern u32 gUnk_087444E4[];
extern struct AnimCmd gUnk_08744510[];
extern u8 gUnk_08744524[];
extern u8 gUnk_08744526[];
extern u32 gUnk_0874452C[];
extern u32 gUnk_08744534[];
extern struct AnimCmd gUnk_0874453C[];
extern struct AnimCmd gUnk_08744550[];
extern u8 gUnk_08744562[];
extern u32 gUnk_08744564[];
extern void *gUnk_08744598[][4];
extern u16 gUnk_087445D8[];
extern s16 gUnk_087445E8[];
extern u16 gUnk_08744608[];
extern void *gUnk_08744618[];
extern void *gUnk_087446E8[];
extern void *gUnk_087447B8[];
extern u32 gUnk_08744F0C[];
extern u32 gUnk_08745040[];
extern u32 gUnk_087450CC[];
extern u32 gUnk_087450E8[];
extern u32 gUnk_08745104[];
extern u32 gUnk_08745120[];
extern u32 gUnk_0874513C[];
extern u32 gUnk_08745158[];
extern u32 gUnk_08745174[];
extern u32 gUnk_08745190[];
extern u32 gUnk_087451AC[];
extern u32 gUnk_087451C8[];
extern u32 gUnk_087451E4[];
extern u32 gUnk_08745200[];
extern u32 gUnk_08745304[];
extern u32 gUnk_0874530C[];
extern u32 gUnk_08745434[];
extern u32 gUnk_08753718[];

/* Externals */
extern void sub_08065438(void);
extern void sub_080656b4(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_08003014(void *src, void *dst, s32 ratio, s32 count, void *out);
extern s32 sub_080031b8(s32 id);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_0800617c(s16 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(void *p);
extern void sub_080639b4(void *p);
extern void sub_080639e0(void *p);
extern void sub_080639f0(void *p);
extern void sub_08063a00(void *p);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern void sub_08063ddc(u32 i);
extern s32 sub_08063df4(void);
extern void sub_08063e14(void);
extern void sub_080640c8(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_08064188(s32 n);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s16 sub_08065f5c(void);
extern u16 sub_08066088(u32 mode);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern void sub_08068920(s32 i, u8 c);
extern void sub_080689c8(s32 i, s32 d);
extern u32 sub_08068cf8(void *p);
extern u32 sub_08068f68(void);
extern u8 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void sub_0806a008(void);
extern void sub_0806a344(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void TaskYieldTrampoline(u32 frames);

/* Defined below */
void sub_080957bc(void);
void sub_08095834(void);
void sub_08095940(void);
void sub_0809595c(void);
void sub_080959e8(void);
void sub_080959ec(void);
void sub_08095a54(void);
void sub_08095ad0(void);
void sub_08095aec(void);
void sub_08095be4(void);
void sub_08095be8(void);
void sub_08095d20(void);
void sub_08095d40(void);
void sub_08095e4c(void);
void sub_08095eac(void);
void sub_08096058(void);
void sub_080960bc(void);
void sub_0809616c(void);
void sub_0809619c(void);
void sub_08096278(void);
void sub_080962ac(void);
void sub_080962d0(void);
void sub_08096320(void);
void sub_080963c0(void);
void sub_080963dc(void);
void sub_08096640(void);
void sub_08096680(void);
void sub_0809680c(void);
void sub_08096888(void);
void sub_080968c0(void);
void sub_08096920(void);
void sub_08096924(void);
void sub_0809699c(void);
void sub_080969c8(void);
void sub_08096a28(void);
void sub_08096a40(void);
void sub_08096b7c(void);
void sub_08096d20(void);
s32 sub_08096d64(void);
s32 sub_08096df4(void);
s32 sub_08096e0c(void);
s32 sub_08096e24(void);
void sub_08096e70(void);
void sub_08096e9c(void);
void sub_08096fc0(void);
void sub_08097024(void);
void sub_08097088(void);

void sub_080957bc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08753718;
    gUnk_02007D00[8]++;
    sub_08095834();
    sub_080692fc();
    sub_080666cc(gUnk_08744510);
    if (sub_08067060() != 0) {
        gUnk_03002490->unk04 = (u32)sub_0809699c;
        sub_0809595c();
    } else {
        gUnk_03002490->unk04 = (u32)sub_080969c8;
        sub_080959ec();
    }
}

void sub_08095834(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    s32 v;

    sub_08066088(1);
    gUnk_03002490->unk68 = 0x30000;
    sub_08063e14();
    gUnk_03002490->unk46 = sub_08064d34(214, 0);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 0;
    b = gUnk_03002490;
    b->unk24 = b->unk46;
    gUnk_03002490->unk46 = sub_08064d34(214, 0);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 1;
    d = gUnk_03002490;
    d->unk24 += d->unk46 << 8;
    gUnk_03002490->unk46 = sub_08064d34(214, 0);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 2;
    f = gUnk_03002490;
    f->unk24 += f->unk46 << 8;
    gUnk_03002490->unk46 = sub_08064d34(214, 0);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 3;
    h = gUnk_03002490;
    h->unk28 = 0;
    h->unk2C = 0;
    h->unk30 = 0;
    h->unk34 = 0;
    h->unk18 = -1;
    h->unk1C = 0;
    h->unk20 = 0;
    h->unk24 = 0;
    h->unk6E = 0;
    h->unk70 = 0;
    sub_08066ae0();
    gUnk_02007D00[9] = sub_08065f5c();
}

void sub_08095940(void)
{
    sub_08002e98(gUnk_03002490->unk14, 12, gUnk_087444E4);
}

void sub_0809595c(void)
{
    struct Task *t;
    struct Task *u;

    sub_0806395c(0);
    gUnk_03002490->unk15 = 0;
    sub_080639e0(gUnk_0874530C);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk60 = 0x5000;
    t->unk68 = 0x30000;
    sub_08006338(15);
    TaskYieldTrampoline(24);
    u = gUnk_03002490;
    u->unk04 = (u32)sub_080969c8;
    if (u->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk34 == 0);
    }
    sub_080639e0(gUnk_08745304);
    sub_08006338(16);
    TaskYieldTrampoline(24);
    sub_0806395c(1);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_080959e8(void)
{
}

void sub_080959ec(void)
{
    sub_0806395c(1);
    gUnk_03002490->unk15 = 1;
    sub_080639e0(gUnk_08745304);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08744510);
    sub_08066580();
    TaskYieldTrampoline(gUnk_08744524[gUnk_03002490->unk74]);
    gUnk_03002490->unk6E = sub_08002ee8(8);
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08095a54(void)
{
    struct Task *t;
    s32 v;
    s32 x;

    gUnk_03002490->unk15 = 1;
    sub_080639e0(gUnk_08745304);
    sub_08063e14();
    sub_080062c4();
    x = sub_080640dc(gUnk_08744510);
    t = gUnk_03002490;
    t->unk28 = x;
    gUnk_030023D4 = v = t->unk74 * 2;
    if (t->unk78 < gUnk_02007D00[9] >> 1)
        gUnk_030023D4 = v + 1;
    TaskYieldTrampoline(gUnk_08744526[gUnk_030023D4]);
    sub_08096fc0();
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08095ad0(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_08095aec(void)
{
    s32 n;
    s32 r;

    gUnk_03002490->unk15 = 2;
    sub_080639e0(gUnk_08745304);
    sub_08063e14();
    sub_08006338(12);
    TaskYieldTrampoline(6);
    sub_08006338(16);
    TaskYieldTrampoline(8);
    r = sub_08002ee8(2);
    n = 1;
    if (r != 0)
        n = 3;
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < n) {
        sub_0800622c(-0x50000, 0x5000, 0x30000);
        sub_0809680c();
        sub_080261d4(2);
        sub_08063e14();
        gUnk_03002490->unk6C++;
    }
    sub_080639e0(gUnk_08745304);
    sub_08006338(17);
    TaskYieldTrampoline(8);
    sub_08006338(18);
    TaskYieldTrampoline(8);
    sub_08006338(6);
    TaskYieldTrampoline(8);
    sub_08006338(7);
    TaskYieldTrampoline(8);
    if (gUnk_03002490->unk14 == 3)
        sub_0806395c(gUnk_08744608[sub_08002ee8(8)]);
    else
        sub_0806395c(8);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08095be4(void)
{
}

void sub_08095be8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gUnk_03002490->unk15 = 3;
    sub_080639e0(gUnk_08745304);
    sub_08063e14();
    sub_08095e4c();
    t = gUnk_03002490;
    t->unk2C = 0;
    t->unk6C = 0;
    do {
        u = gUnk_03002490;
        if (u->unk2C != 0)
            u->unk43 = -u->unk43;
        else
            sub_08063e14();
        sub_08095d40();
        v = gUnk_03002490;
        v->unk6C++;
    } while ((s16)v->unk6C <= 1);
    if (gUnk_03002490->unk74 == 1 && sub_08002ee8(2) != 0) {
        sub_08063e14();
        gUnk_03002490->unk7A = 0;
        sub_0800622c(-0x30000, 0x2000, 0x30000);
        sub_080061c0(0x10000, 0x5A5A5A5A);
        sub_0809680c();
        sub_080639e0(gUnk_08745304);
        sub_08096924();
        if (gUnk_03002490->unk14 == 5)
            sub_0806395c(2);
        else
            sub_0806395c(gUnk_08744608[sub_08002ee8(8)]);
    } else {
        gUnk_03002490->unk6C = 0;
        do {
            w = gUnk_03002490;
            if (w->unk2C != 0)
                w->unk43 = -w->unk43;
            else
                sub_08063e14();
            sub_08095d40();
            x = gUnk_03002490;
            x->unk6C++;
        } while ((s16)x->unk6C <= 1);
        sub_08096924();
        sub_0806395c(2);
    }
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08095d20(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk5C != 0 && t->unk54 * t->unk5C >= 0)
        sub_08006214();
}

void sub_08095d40(void)
{
    struct Task *t;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk2C = zero;
    t->unk30 = 1;
    sub_080062c4();
    sub_08006338(8);
    TaskYieldTrampoline(5);
    sub_080061c0(gUnk_0874452C[gUnk_03002490->unk74], 0x5A5A5A5A);
    gUnk_03002490->unk7A = zero;
    gUnk_03002490->unk58 = -0x5000;
    sub_08006338(9);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk58 = 0x5000;
    sub_08006338(10);
    TaskYieldTrampoline(3);
    sub_080061c0(0x5A5A5A5A, gUnk_08744534[gUnk_03002490->unk74]);
    sub_08006338(10);
    TaskYieldTrampoline(2);
    sub_08006338(11);
    TaskYieldTrampoline(5);
    if (gUnk_03002490->unk43 != sub_08063df4() || gUnk_03002490->unk2C != 0) {
        sub_0806cc90(1, 1, -4, 12);
        sub_08006338(16);
        TaskYieldTrampoline(15);
    } else {
        sub_08006338(8);
        TaskYieldTrampoline(2);
        sub_0806cc90(1, 1, -4, 12);
        TaskYieldTrampoline(7);
    }
    sub_080062c4();
    gUnk_03002490->unk30 = 0;
}

void sub_08095e4c(void)
{
    sub_080062c4();
    sub_080640c8();
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(4);
        TaskYieldTrampoline(5);
        sub_08006338(5);
        TaskYieldTrampoline(5);
        sub_08006338(6);
        TaskYieldTrampoline(5);
        sub_08006338(7);
        TaskYieldTrampoline(5);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
}

void sub_08095eac(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 4;
    sub_080639e0(gUnk_08745304);
    sub_08063e14();
    if (ABS(sub_08063cd0()) <= 47) {
        sub_08096888();
        gUnk_03002490->unk15 = 4;
    } else if (ABS(sub_08063cd0()) > 71) {
        sub_08095e4c();
        gUnk_03002490->unk6C = 0;
        while ((s16)gUnk_03002490->unk6C <= 3) {
            if (ABS(sub_08063cd0()) <= 71)
                break;
            sub_08063e14();
            sub_08095d40();
            t = gUnk_03002490;
            if (t->unk2C != 0) {
                t->unk43 = -t->unk43;
                sub_08095d40();
                break;
            }
            t->unk6C++;
        }
    }
    sub_08063e14();
    sub_08006338(17);
    TaskYieldTrampoline(8);
    sub_08006338(18);
    TaskYieldTrampoline(8);
    sub_08006338(19);
    TaskYieldTrampoline(8);
    sub_080061c0(0xC000, 0x5A5A5A5A);
    sub_0800622c(-0x28000, 0x2000, 0x30000);
    sub_0809680c();
    sub_080639e0(gUnk_08745304);
    sub_080062c4();
    sub_08063e14();
    sub_08006338(4);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(26);
        TaskYieldTrampoline(4);
        sub_080031b8(0x23B);
        sub_08006338(27);
        TaskYieldTrampoline(1);
        sub_08006338(28);
        TaskYieldTrampoline(1);
        sub_08006338(29);
        TaskYieldTrampoline(4);
        u = gUnk_03002490;
        u->unk6C++;
    } while ((s16)u->unk6C <= 2);
    sub_08096888();
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08096058(void)
{
    struct Task *t;

    switch (gUnk_03002490->unk3C) {
    case 26:
        sub_08068cf8(gUnk_0874513C);
        break;
    case 27:
        sub_08068cf8(gUnk_08745158);
        break;
    case 28:
        sub_08068cf8(gUnk_08745174);
        break;
    }
    t = gUnk_03002490;
    if (t->unk5C != 0 && t->unk54 * t->unk5C >= 0)
        sub_08006214();
}

void sub_080960bc(void)
{
    struct Task *t;
    s32 zero;

    sub_08063e14();
    sub_080639e0(gUnk_08745304);
    gUnk_03002490->unk1C = 1;
    if (ABS(sub_08063cd0()) <= 31)
        sub_08096888();
    sub_080062c4();
    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 8;
    sub_08006338(17);
    TaskYieldTrampoline(8);
    sub_08006338(18);
    TaskYieldTrampoline(8);
    sub_0800622c(-0x40000, 0x2200, 0x30000);
    sub_0809680c();
    sub_080639e0(gUnk_08745304);
    sub_080062c4();
    gUnk_03002490->unk1C = zero;
    sub_08006338(44);
    TaskYieldTrampoline(2);
    sub_08006338(45);
    TaskYieldTrampoline(2);
    sub_0809619c();
}

void sub_0809616c(void)
{
    if (sub_08063d2c() > -24) {
        gUnk_03002490->unk1C = 0;
        sub_08006148(sub_080962d0, gCurTaskIdx);
    }
}

void sub_0809619c(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 6;
    sub_080031b8(500);
    gUnk_03002490->unk2C = 0;
    sub_080061c0(0x30000, 0x5A5A5A5A);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_0874453C);
    TaskYieldTrampoline(48);
    while (gUnk_03002490->unk43 == sub_08063df4()) {
        if (ABS(sub_08063cd0()) > 48)
            break;
        TaskYieldTrampoline(10);
    }
    t = gUnk_03002490;
    t->unk15 = 7;
    sub_080640c8();
    sub_080061c0(0x5A5A5A5A, -0x2000);
    u = gUnk_03002490;
    u->unk60 = 0x2000;
    u->unk68 = 0x30000;
    sub_08006338(47);
    TaskYieldTrampoline(8);
    sub_08006338(48);
    TaskYieldTrampoline(8);
    sub_0809680c();
    sub_08096924();
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08096278(void)
{
    struct Task *t;
    s32 x;

    x = sub_08064188(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk2C != 0)
        sub_08006148(sub_08096320, gCurTaskIdx);
}

void sub_080962ac(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 0 || t->unk54 * t->unk5C >= 0)
        sub_08006214();
}

void sub_080962d0(void)
{
    gUnk_03002490->unk15 = 9;
    gUnk_03002490->unk2C = 0;
    sub_080062c4();
    sub_080061c0(-0x20000, 0x2000);
    sub_08006338(44);
    TaskYieldTrampoline(2);
    sub_08006338(45);
    TaskYieldTrampoline(2);
    TaskYieldTrampoline(12);
    sub_080062c4();
    sub_0809619c();
}

void sub_08096320(void)
{
    s32 zero;

    gUnk_03002490->unk15 = 9;
    zero = 0;
    gUnk_03002490->unk2C = zero;
    sub_080031b8(504);
    sub_080261d4(2);
    gUnk_03002490->unk7A = zero;
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x3000, 0x30000);
    sub_08006338(47);
    TaskYieldTrampoline(8);
    sub_08006338(48);
    if (gUnk_03002490->unk58 < 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk58 < 0);
    }
    sub_0809680c();
    sub_080639e0(gUnk_08745304);
    sub_08096924();
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_080963c0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 0 && t->unk58 < 0)
        t->unk58 = 0;
}

void sub_080963dc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gUnk_03002490->unk15 = 5;
    sub_080639e0(gUnk_08745304);
    sub_080640c8();
    sub_08063e14();
    if (ABS(sub_08063cd0()) <= 47) {
        t = gUnk_03002490;
        goto flip;
    }
    if (sub_08063cd0() >= 0)
        goto poscheck;
    if (-sub_08063cd0() > 80)
        goto doloop;
    goto rest;
flip:
    t->unk43 = -t->unk43;
    sub_08095d40();
    goto rest;
poscheck:
    if (sub_08063cd0() > 80)
        goto doloop;
    goto rest;
doloop:
    {
        sub_08063e14();
        sub_08095e4c();
        gUnk_03002490->unk6C = 0;
        do {
            sub_08095d40();
            t = gUnk_03002490;
            if (t->unk2C != 0)
                goto flip;
            sub_08063e14();
            if (ABS(sub_08063cd0()) <= 79)
                goto rest;
            t = gUnk_03002490;
            t->unk6C++;
        } while ((s16)t->unk6C <= 3);
    }
rest:
    sub_080062c4();
    sub_08063e14();
    gUnk_03002490->unk30 = 1;
    sub_08006338(17);
    TaskYieldTrampoline(8);
    sub_08006338(18);
    TaskYieldTrampoline(8);
    sub_08006338(19);
    TaskYieldTrampoline(8);
    sub_08006338(20);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk7A = 0;
    v = gUnk_03002490;
    v->unk2C = 0;
    v->unk34 = 0;
    sub_080061c0(0x20000, 0x5A5A5A5A);
    sub_0800622c(-0x38000, 0x2000, 0x30000);
    sub_08006338(36);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk30 = 0;
    sub_080062c4();
    sub_08006338(37);
    TaskYieldTrampoline(8);
    sub_08006338(38);
    TaskYieldTrampoline(8);
    w = gUnk_03002490;
    w->unk58 = 0x60000;
    w->unk60 = 0x100;
    if (w->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk34 == 0);
    }
    sub_080031b8(0x1F7);
    sub_080261d4(2);
    sub_08097088();
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08744550);
    sub_08063a00(gUnk_08745040);
    TaskYieldTrampoline(2);
    sub_08063a00(0);
    TaskYieldTrampoline(gUnk_08744562[gUnk_03002490->unk74]);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk58 = 0x20000;
        TaskYieldTrampoline(1);
        x = gUnk_03002490;
        x->unk6C++;
    } while ((s16)x->unk6C <= 7);
    sub_080640c8();
    sub_080062c4();
    sub_08006338(38);
    TaskYieldTrampoline(4);
    sub_08006338(36);
    TaskYieldTrampoline(8);
    sub_08096924();
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_08096640(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = sub_08064188(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk2C != 0 && t->unk58 < 0)
        t->unk58 = 0;
    u = gUnk_03002490;
    if (u->unk5C != 0 && u->unk54 * u->unk5C >= 0)
        sub_08006214();
}

void sub_08096680(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 zero;

    sub_080062c4();
    t = gUnk_03002490;
    zero = 0;
    t->unk30 = zero;
    sub_080031b8(0x236);
    sub_08006338(34);
    u = gUnk_03002490;
    if (u->unk7A == 0) {
        u->unk34 = zero;
        u->unk6C = zero;
        do {
            sub_080061c0(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gUnk_03002490;
            v->unk6C++;
        } while ((s16)v->unk6C <= 3);
        sub_08006214();
        w = gUnk_03002490;
        w->unk58 = 0x48000;
        if (w->unk34 == 0) {
            do {
                TaskYieldTrampoline(1);
            } while (gUnk_03002490->unk34 == 0);
        }
    } else {
        u->unk6C = zero;
        do {
            sub_080061c0(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            x = gUnk_03002490;
            x->unk6C++;
        } while ((s16)x->unk6C <= 3);
        sub_08006214();
    }
    gUnk_03002490->unk6C = 0;
    do {
        if (gUnk_03002360 == gUnk_03002490->unk18)
            sub_080031b8(0x237);
        sub_08064cdc(141, 24, 0, 0);
        sub_08006338(30);
        TaskYieldTrampoline(4);
        sub_08006338(31);
        TaskYieldTrampoline(4);
        sub_08006338(32);
        TaskYieldTrampoline(4);
        sub_08006338(33);
        TaskYieldTrampoline(4);
        y = gUnk_03002490;
        y->unk6C++;
    } while ((s16)y->unk6C <= 5);
    sub_08068920(gUnk_03002490->unk18, 6);
    gUnk_03002490->unk18 = -1;
    sub_08096888();
    gUnk_03002490->unk30 = 0;
    sub_0806395c(2);
    gUnk_03002490->unk15 = 12;
    sub_08006138();
}

void sub_0809680c(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk2C = 0;
    t->unk34 = 0;
    sub_080640c8();
    sub_080639e0(gUnk_0874530C);
    sub_08006338(13);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_08006338(14);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    u = gUnk_03002490;
    if (u->unk58 > 0 && u->unk34 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk34 == 0);
    }
    sub_080062c4();
}

void sub_08096888(void)
{
    gUnk_03002490->unk15 = 10;
    sub_0800622c(-0x20000, 0x2000, 0x30000);
    sub_080061c0(-0x20000, 0x5A5A5A5A);
    sub_080968c0();
}

void sub_080968c0(void)
{
    struct Task *t;

    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk2C = 0;
    t->unk34 = 0;
    sub_080640c8();
    sub_08006338(23);
    TaskYieldTrampoline(8);
    sub_08006338(24);
    TaskYieldTrampoline(8);
    sub_08006338(25);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(12);
    TaskYieldTrampoline(1);
}

void sub_08096920(void)
{
}

void sub_08096924(void)
{
    if (sub_08063df4() == 1) {
        if (gUnk_03002490->unk48 - gUnk_03002158[0] > 80) {
            sub_08063e14();
            sub_08006338(17);
            TaskYieldTrampoline(8);
            sub_08096888();
        }
    } else {
        if (gUnk_03002490->unk48 - gUnk_03002158[0] <= 159) {
            sub_08063e14();
            sub_08006338(17);
            TaskYieldTrampoline(8);
            sub_08096888();
        }
    }
}

void sub_0809699c(void)
{
    sub_08096e9c();
    sub_08002e98(gUnk_03002490->unk15, 13, gUnk_08744564);
    sub_08097024();
    sub_08068f68();
    sub_08069b44();
}

void sub_080969c8(void)
{
    sub_08096e9c();
    sub_08096a40();
    if (sub_0806acf8() == 0) {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 13, gUnk_08744564);
    } else {
        sub_08002e98(gUnk_03002490->unk15, 13, gUnk_08744564);
    }
    sub_08097024();
    sub_08068f68();
    sub_08069b44();
}

void sub_08096a28(void)
{
    sub_08006148(sub_08095940, gCurTaskIdx);
}

void sub_08096a40(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;
    struct PlayerState *p;

    t = gUnk_03002490;
    if (t->unk30 == 0)
        return;
    switch (t->unk3C) {
    case 8:
    case 17:
    case 18:
    case 19:
    case 20:
        sub_08068cf8(gUnk_08745190);
        break;
    case 9:
        sub_08068cf8(gUnk_087451AC);
        break;
    case 10:
        sub_08068cf8(gUnk_087451C8);
        break;
    case 11:
        sub_08068cf8(gUnk_087451E4);
        break;
    case 36:
        sub_08068cf8(gUnk_08745200);
        break;
    }
    u = gUnk_03002490;
    if (u->unk7C == 8) {
        p = gUnk_03002170;
        i = u->unk7E;
        if (p[i].unk0D != 17) {
            u->unk18 = i;
            sub_08063ddc(i);
            sub_080685ec(gUnk_03002490->unk18, gCurTaskIdx, 5);
            sub_08006148(sub_08096680, gCurTaskIdx);
        }
    }
}

void sub_08096b7c(void)
{
    struct Task *t;
    s32 zero;

    gUnk_03002490->unk15 = 11;
    zero = 0;
    sub_080640c8();
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 21);
    sub_080062c4();
    t = gUnk_03002490;
    t->unk34 = zero;
    t->unk30 = zero;
    t->unk1C = 1;
    sub_08064d34(142, 0);
    sub_080639a4(gUnk_08745434);
    gUnk_03002490->unk7A = zero;
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x30000, 0x1A00, 0x30000);
    sub_08006338(21);
    sub_080639b4(gUnk_08744F0C);
    sub_08063a00(gUnk_087446E8[gUnk_03002490->unk3C]);
    sub_080639f0(gUnk_087447B8[gUnk_03002490->unk3C]);
    sub_080639e0(gUnk_08745304);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_08006338(38);
    sub_080639b4(gUnk_087450CC);
    sub_08063a00(gUnk_087450E8);
    sub_080639f0(gUnk_087447B8[gUnk_03002490->unk3C]);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08744550);
    sub_0800617c(40);
    sub_080639b4(gUnk_08745104);
    sub_08063a00(gUnk_08745120);
    sub_080639f0(gUnk_087447B8[gUnk_03002490->unk3C]);
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_08064d34(141, 0);
    gUnk_03002490->unk20 = 0;
    sub_080061c0(-0x10000, 0x600);
    TaskYieldTrampoline(30);
    sub_080062c4();
    TaskYieldTrampoline(170);
    sub_0806ad18();
    sub_0806a344();
}

void sub_08096d20(void)
{
    struct Task *t;
    s32 x;

    x = sub_08064188(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk34 == 1 && t->unk54 != 0) {
        t->unk20++;
        if (t->unk20 == 16) {
            sub_0806cc90(0, 1, 8, 10);
            gUnk_03002490->unk20 = 0;
        }
    }
}

s32 sub_08096d64(void)
{
    switch (gUnk_03002490->unk14) {
    case 0:
    case 3:
    case 4:
        sub_08006244();
        sub_080031b8(0x1F7);
        sub_080261d4(2);
        gUnk_03002490->unk34 = 1;
        return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        sub_08006244();
        gUnk_03002490->unk34 = 1;
        return 0;
    case 10:
        sub_080062c4();
        gUnk_03002490->unk34 = 1;
        BLOCK_CROSS_JUMP
        return 0;
    }
}

s32 sub_08096df4(void)
{
    gUnk_03002490->unk2C = 1;
    sub_08006214();
    return 0;
}

s32 sub_08096e0c(void)
{
    sub_08064d34(142, 0);
    sub_080261d4(2);
    return 0;
}

s32 sub_08096e24(void)
{
    struct Task *t;

    sub_0806a008();
    t = gUnk_03002490;
    if (t->unk18 >= 0) {
        sub_080689c8(t->unk18, -t->unk43);
        gUnk_03002490->unk18 = -1;
    }
    sub_0806395c(10);
    sub_08006148(sub_08095940, gCurTaskIdx);
    return 1;
}

void sub_08096e70(void)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < 4; i++) {
        sub_08005654(gUnk_03002490->unk24 & 0xFF);
        t = gUnk_03002490;
        t->unk24 >>= 8;
    }
}

void sub_08096e9c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s16 *q;

    a = gUnk_03002490->unk8C;
    if ((a->unk0B & 1) == 0) {
    a->unk0A = 1;
    t = gUnk_03002490;
    if ((t->unk75 != 0 && t->unk8C->unk05 != 0) || t->unk1C != 0) {
        u = gUnk_03002490;
        q = (s16 *)&u->unk70;
        if (*q <= 3) {
            sub_08003014(gUnk_08744598[u->unk8C->unk0C][0],
                         gUnk_08744598[u->unk8C->unk0C][1],
                         gUnk_087445D8[*q], 16,
                         &gUnk_03001470[(u->unk40 >> 12) * 32]);
        } else {
            sub_08003014(gUnk_08744598[u->unk8C->unk0C][0],
                         gUnk_082B07BC,
                         gUnk_087445D8[*q], 16,
                         &gUnk_03001470[(u->unk40 >> 12) * 32]);
        }
    } else {
        sub_08003014(gUnk_08744598[t->unk8C->unk0C][0],
                     gUnk_08744598[t->unk8C->unk0C][1],
                     gUnk_087445D8[(s16)t->unk70], 16,
                     &gUnk_03001470[(t->unk40 >> 12) * 32]);
    }
    v = gUnk_03002490;
    v->unk70++;
    if ((s16)v->unk70 > 7)
        v->unk70 = 0;
    }
}

void sub_08096fc0(void)
{
    struct Task *t;
    u16 v;

    t = gUnk_03002490;
    t->unk6E++;
    if (t->unk6E > 7)
        t->unk6E = 0;
    v = gUnk_087445E8[gUnk_03002490->unk6E + gUnk_03002490->unk74 * 8];
    if (gUnk_087445E8[gUnk_03002490->unk6E + gUnk_03002490->unk74 * 8] == 11)
        v = gUnk_08744608[sub_08002ee8(8)];
    sub_0806395c(v);
}

void sub_08097024(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gUnk_03002490;
    if (t->unk14 != 10) {
        sub_080639b4(gUnk_08744618[t->unk3C]);
        u = gUnk_03002490;
        v = u->unk3C;
        if (v < 40 || v > 43)
            sub_08063a00(gUnk_087446E8[u->unk3C]);
        sub_080639f0(gUnk_087447B8[gUnk_03002490->unk3C]);
    }
}

void sub_08097088(void)
{
    struct Task *t;
    s32 d;
    u16 x;
    u16 y;

    d = sub_08063df4();
    t = gUnk_03002490;
    x = t->unk48 + d * 24;
    y = t->unk4A + 3;
    sub_08067120(x, y, d, 3);
}
