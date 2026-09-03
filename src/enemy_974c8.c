#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* This module's |x|: the ROM tests `< 0` first, unlike global.h's abs(). */
#define ABS(n) ((n) < 0 ? -(n) : (n))

/* RAM cells */
extern s32 gUnk_02007D00[];
extern s32 gUnk_030023D4;
extern vu16 gUnk_03004CA0[];
extern u16 gUnk_03002360;
extern struct PlayerState gUnk_03002170[];
extern s16 gUnk_03002158[];

/* ROM tables */
extern u32 gUnk_082BFBA4[];
extern struct AnimCmd gUnk_08744888[];
extern u32 gUnk_0874489C[];
extern u32 gUnk_087448C0[];
extern u8 gUnk_087448E4[];
extern s16 gUnk_087448E6[];
extern s32 gUnk_087448EC[];
extern u8 gUnk_087448F4[];
extern s32 gUnk_087448F8[];
extern struct AnimCmd gUnk_08744900[];
extern u32 gUnk_08744924[];
extern u8 gUnk_0874492C[];
extern s32 gUnk_08744934[];
extern s32 gUnk_0874494C[];
extern u32 gUnk_0874521C[];
extern u32 gUnk_08745238[];
extern u32 gUnk_08745254[];
extern u32 gUnk_08745270[];
extern u32 gUnk_0874528C[];
extern u32 gUnk_087452A8[];
extern u32 gUnk_087452C4[];
extern u32 gUnk_087452E0[];
extern u32 gUnk_0874544C[];
extern struct AnimCmd gUnk_087454B8[];
extern struct AnimCmd gUnk_087454C4[];
extern s32 gUnk_087454D8[];
extern s32 gUnk_087454EC[];
extern s32 gUnk_087454F4[];
extern s32 gUnk_087454FC[];
extern s32 gUnk_08745504[];
extern struct AnimCmd gUnk_0874550C[];
extern u32 gUnk_08753054[];
extern u32 gUnk_087537FC[];
extern u32 gUnk_087538B0[];

/* Externals */
extern void sub_0806523c(void);
extern void sub_08065438(void);
extern void sub_080656b4(void);
extern void sub_08098fb0(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern s32 sub_080031b8(s32 id);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
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
extern void sub_08063a00(void *p);
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern void sub_08063ddc(u32 i);
extern void sub_08063e14(void);
extern void sub_080640c8(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_08064188(s32 n);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern void sub_08068920(s32 i, u8 c);
extern void sub_080689c8(s32 i, s32 d);
extern u32 sub_08068e04(void);
extern u32 sub_08068f68(void);
extern u32 sub_0806914c(void *p);
extern u8 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void sub_0806a008(void);
extern void sub_0806a0f0(s32 a);
extern u32 sub_0806a25c(void);
extern void sub_0806a344(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern void TaskYieldTrampoline(u32 frames);
extern s32 sub_08094d10(void);

/* Defined below */
void sub_080974c8(void);
void sub_080974f8(void);
void sub_08097580(void);
void sub_080975ac(void);
void sub_080975c8(void);
void sub_080975fc(void);
void sub_08097694(void);
void sub_08097714(void);
void sub_0809773c(void);
void sub_0809780c(void);
void sub_08097844(void);
void sub_0809794c(void);
void sub_08097a54(void);
void sub_08097a7c(void);
void sub_08097b4c(void);
void sub_08097b74(void);
void sub_08097c44(void);
void sub_08097c78(void);
void sub_08097d7c(void);
void sub_08097da4(void);
void sub_08097e68(void);
void sub_08097e90(void);
void sub_0809809c(void);
void sub_08098140(void);
void sub_0809816c(void);
void sub_08098194(void);
void sub_08098268(void);
void sub_0809829c(void);
void sub_080983a0(void);
void sub_080983a4(void);
void sub_08098450(void);
void sub_080984b4(void);
s32 sub_08098528(void);
s32 sub_08098540(void);
s32 sub_0809857c(void);
s32 sub_08098594(void);
void sub_080985e0(void);
void sub_0809869c(void);
void sub_080986ec(void);
void sub_08098708(void);
void sub_08098718(void);
void sub_08098728(void);
void sub_08098738(void);
s32 sub_08098748(void);
void sub_0809876c(void);
void sub_0809887c(void);
s32 sub_080988a4(void);
s32 sub_080988b4(void);
void sub_080988c0(void);
s32 sub_080988c4(void);

void sub_080974c8(void)
{
    if (gUnk_03004CA0[gUnk_03002490->unk44] != 55)
        sub_08005654(gCurTaskIdx);
}

void sub_080974f8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087537FC;
    gUnk_02007D00[8]++;
    sub_08066088(0);
    sub_08097580();
    sub_080692fc();
    sub_080666cc(gUnk_08744888);
    if (sub_08067060() != 0) {
        gUnk_03002490->unk04 = (u32)sub_080975c8;
        sub_08097694();
    } else {
        gUnk_03002490->unk04 = (u32)sub_080975fc;
        sub_08066580();
        sub_0806395c(1);
        sub_0809773c();
    }
}

void sub_08097580(void)
{
    struct Task *t;

    sub_08063a00(gUnk_08745238);
    sub_08063e14();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk2C = 0;
    t->unk1C = 0;
    t->unk18 = -1;
    sub_08066ae0();
}

void sub_080975ac(void)
{
    sub_08002e98(gUnk_03002490->unk14, 9, gUnk_0874489C);
}

void sub_080975c8(void)
{
    sub_08002e98(gUnk_03002490->unk15, 9, gUnk_087448C0);
    if (gUnk_03002490->unk2C != 0)
        sub_080984b4();
    sub_08068f68();
    sub_08069b44();
}

void sub_080975fc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if ((t->unk75 != 0 && t->unk8C->unk05 != 0) || t->unk14 == 8)
        sub_080663f4(gUnk_082BFBA4, 16);
    else
        sub_08066468();
    if (sub_0806acf8() == 0) {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 9, gUnk_087448C0);
    } else {
        sub_08002e98(gUnk_03002490->unk15, 9, gUnk_087448C0);
    }
    if (gUnk_03002490->unk2C != 0)
        sub_080984b4();
    sub_08068f68();
    sub_08069b44();
}

void sub_08097694(void)
{
    struct Task *t;

    sub_0806395c(0);
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk28 = 0;
    sub_08006338(18);
    gUnk_03002490->unk60 = 0x2500;
    TaskYieldTrampoline(24);
    t = gUnk_03002490;
    t->unk04 = (u32)sub_080975fc;
    if (t->unk28 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 == 0);
    }
    sub_080031b8(0x1F7);
    sub_080261d4(2);
    sub_08006338(19);
    TaskYieldTrampoline(24);
    sub_08066580();
    sub_0806395c(1);
    sub_08006138();
}

void sub_08097714(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_0809773c(void)
{
    struct Task *t;
    s32 r;
    s16 v;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk2C = 1;
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_087448E4[gUnk_03002490->unk74]) {
        sub_08063e14();
        sub_080062c4();
        v = gUnk_087448E6[gUnk_03002490->unk74];
        if (ABS(sub_08063cd0()) <= v) {
            gUnk_030023D4 = r = sub_08002ee8(8);
            if (r > 3)
                goto anim;
            sub_0809780c();
        } else {
            gUnk_030023D4 = r = sub_08002ee8(8);
            if (r > 3) {
                if (r <= 4) {
anim:
                    sub_08097844();
                } else {
                    sub_0809794c();
                }
            }
        }
        gUnk_03002490->unk6C++;
    }
    sub_080983a4();
    sub_08006138();
}

void sub_0809780c(void)
{
    sub_08006338(4);
    TaskYieldTrampoline(8);
    sub_08006338(5);
    TaskYieldTrampoline(8);
    sub_08006338(6);
    TaskYieldTrampoline(8);
    sub_08006338(7);
    TaskYieldTrampoline(8);
}

void sub_08097844(void)
{
    struct Task *t;
    struct Task *u;

    sub_080062c4();
    sub_08006338(22);
    TaskYieldTrampoline(5);
    sub_08006338(26);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk28 = 0;
    sub_080061c0(-gUnk_087448EC[t->unk74], 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x8000, 0x30000);
    sub_08006338(23);
    TaskYieldTrampoline(4);
    sub_08006338(25);
    TaskYieldTrampoline(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(24);
    TaskYieldTrampoline(5);
    sub_08006338(27);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk7A = 0;
    u = gUnk_03002490;
    u->unk28 = 0;
    sub_080061c0(-gUnk_087448EC[u->unk74], 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x8000, 0x30000);
    sub_08006338(19);
    TaskYieldTrampoline(4);
    sub_08006338(17);
    TaskYieldTrampoline(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
}

void sub_0809794c(void)
{
    struct Task *t;
    struct Task *u;

    sub_080062c4();
    sub_08006338(22);
    TaskYieldTrampoline(5);
    sub_08006338(26);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk28 = 0;
    sub_080061c0(gUnk_087448EC[t->unk74], 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x8000, 0x30000);
    sub_08006338(23);
    TaskYieldTrampoline(4);
    sub_08006338(25);
    TaskYieldTrampoline(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(24);
    TaskYieldTrampoline(5);
    sub_08006338(27);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk7A = 0;
    u = gUnk_03002490;
    u->unk28 = 0;
    sub_080061c0(gUnk_087448EC[u->unk74], 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x8000, 0x30000);
    sub_08006338(19);
    TaskYieldTrampoline(4);
    sub_08006338(17);
    TaskYieldTrampoline(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
}

void sub_08097a54(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_08097a7c(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk2C = 1;
    sub_080062c4();
    sub_08063e14();
    n = gUnk_087448F4[gUnk_03002490->unk74 * 2 + sub_08002ee8(2)];
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < n) {
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk28 = 0;
        sub_0800622c(-0x50000, 0x5000, 0x30000);
        sub_08006338(17);
        TaskYieldTrampoline(16);
        sub_08006338(18);
        TaskYieldTrampoline(14);
        sub_08006338(19);
        TaskYieldTrampoline(2);
        while (gUnk_03002490->unk28 == 0)
            TaskYieldTrampoline(1);
        sub_080031b8(0x1F7);
        sub_080261d4(2);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_08097b4c(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_08097b74(void)
{
    struct Task *t;
    s32 v;

    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk2C = 1;
    sub_080061c0(-0x30000, 0x3000);
    sub_080640c8();
    sub_08006338(11);
    TaskYieldTrampoline(5);
    sub_08006338(12);
    TaskYieldTrampoline(4);
    sub_08006338(13);
    TaskYieldTrampoline(3);
    sub_08006338(14);
    TaskYieldTrampoline(2);
    sub_08006338(15);
    TaskYieldTrampoline(2);
    sub_080062c4();
    v = sub_080640dc(gUnk_08744900);
    t = gUnk_03002490;
    t->unk24 = v;
    if (t->unk74 == 0)
        TaskYieldTrampoline(16);
    sub_080031b8(500);
    sub_0806cc90(1, 3, 8, 10);
    sub_080061c0(gUnk_087448F8[gUnk_03002490->unk74], 0x5A5A5A5A);
    TaskYieldTrampoline(48);
    sub_080062c4();
    sub_080640c8();
    sub_0806395c(1);
    sub_08006138();
}

void sub_08097c44(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 3) {
        sub_08006148(sub_080975ac, gCurTaskIdx);
    } else {
        gUnk_03002490->unk24 = sub_08064188(t->unk24);
    }
}

void sub_08097c78(void)
{
    struct Task *t;
    s32 zero;

    gUnk_03002490->unk15 = 4;
    zero = 0;
    sub_080031b8(0x1F7);
    sub_080261d4(4);
    sub_080639b4(gUnk_08745270);
    sub_08063a00(gUnk_0874528C);
    t = gUnk_03002490;
    t->unk2C = zero;
    t->unk7A = zero;
    gUnk_03002490->unk28 = zero;
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x30000, 0x2500, 0x30000);
    sub_08006338(28);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_0806cc90(0, 1, 8, 10);
    sub_080061c0(-0x18000, 0x5A5A5A5A);
    sub_08006338(29);
    TaskYieldTrampoline(2);
    sub_080062c4();
    if (gUnk_03002490->unk74 == 0)
        TaskYieldTrampoline(40);
    sub_080639b4(gUnk_0874521C);
    sub_08063a00(gUnk_08745238);
    sub_08006338(14);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08097d7c(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_08097da4(void)
{
    struct Task *t;
    s32 zero;

    gUnk_03002490->unk15 = 5;
    zero = 0;
    sub_08063e14();
    gUnk_03002490->unk7A = zero;
    t = gUnk_03002490;
    t->unk28 = zero;
    t->unk2C = 1;
    sub_080061c0(gUnk_08744924[t->unk74], 0x5A5A5A5A);
    sub_0800622c(-0x48000, 0x2500, 0x30000);
    sub_08006338(17);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_08006338(18);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080031b8(0x1F7);
    sub_080261d4(2);
    sub_0806cffc(16, 12);
    sub_080062c4();
    sub_08006338(19);
    TaskYieldTrampoline(8);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08097e68(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_08097e90(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;
    s32 d;
    s32 k;

    gUnk_03002490->unk15 = 6;
    zero = 0;
    sub_080062c4();
    gUnk_03002490->unk2C = zero;
    sub_080640c8();
    sub_08006338(40);
    t = gUnk_03002490;
    if (t->unk7A == 0) {
        t->unk28 = zero;
        sub_0800622c(0, 0x2500, 0x30000);
        gUnk_03002490->unk6C = zero;
        do {
            sub_080061c0(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            u = gUnk_03002490;
            u->unk6C++;
        } while ((s16)u->unk6C <= 3);
        sub_08006214();
        while (gUnk_03002490->unk28 == 0)
            TaskYieldTrampoline(1);
        TaskYieldTrampoline(20);
    } else {
        t->unk6C = zero;
        do {
            sub_080061c0(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gUnk_03002490;
            v->unk6C++;
        } while ((s16)v->unk6C <= 3);
        sub_08006214();
        TaskYieldTrampoline(4);
    }
    gUnk_030023D4 = d = gUnk_03002490->unk48 - gUnk_03002158[0];
    k = gUnk_03002490->unk43;
    if (120 - d >= 0)
        goto chk2;
    if (k <= 0)
        goto yes;
    goto other;
chk2:
    if (k < 0)
        goto other;
yes:
    sub_0809809c();
    sub_08068920(gUnk_03002490->unk18, 8);
    if (gUnk_03002360 == gUnk_03002490->unk18)
        sub_080031b8(0x23A);
    gUnk_03002490->unk18 = -1;
    sub_08006338(32);
    TaskYieldTrampoline(4);
    sub_08006338(39);
    TaskYieldTrampoline(8);
    goto tail;
other:
    sub_08098140();
    sub_08068920(gUnk_03002490->unk18, 9);
    if (gUnk_03002360 == gUnk_03002490->unk18)
        sub_080031b8(0x23A);
    gUnk_03002490->unk18 = -1;
    sub_08006338(43);
    TaskYieldTrampoline(4);
    sub_08006338(44);
    TaskYieldTrampoline(8);
tail:
    sub_08006338(4);
    TaskYieldTrampoline(8);
    sub_08006338(5);
    TaskYieldTrampoline(8);
    sub_08006338(6);
    TaskYieldTrampoline(8);
    sub_08006338(7);
    TaskYieldTrampoline(8);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0809809c(void)
{
    struct Task *t;

    gUnk_03002490->unk6C = 0;
    do {
        if (gUnk_03002360 == gUnk_03002490->unk18)
            sub_080031b8(0x239);
        sub_08006338(30);
        TaskYieldTrampoline(4);
        sub_08006338(34);
        TaskYieldTrampoline(2);
        sub_08006338(31);
        TaskYieldTrampoline(1);
        sub_08006338(35);
        TaskYieldTrampoline(2);
        sub_08006338(32);
        TaskYieldTrampoline(4);
        sub_08006338(36);
        TaskYieldTrampoline(4);
        sub_08006338(33);
        TaskYieldTrampoline(5);
        sub_08006338(37);
        TaskYieldTrampoline(4);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
}

void sub_08098140(void)
{
    sub_08006338(40);
    TaskYieldTrampoline(8);
    sub_08006338(41);
    TaskYieldTrampoline(8);
    sub_08006338(42);
    TaskYieldTrampoline(4);
}

void sub_0809816c(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_080975ac, gCurTaskIdx);
}

void sub_08098194(void)
{
    s32 zero;

    gUnk_03002490->unk15 = 7;
    zero = 0;
    gUnk_03002490->unk2C = zero;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(4);
    TaskYieldTrampoline(15);
    gUnk_03002490->unk7A = zero;
    gUnk_03002490->unk28 = zero;
    gUnk_03002490->unk24 = sub_080640dc(gUnk_08744900);
    sub_0800622c(-0x38000, 0x2000, 0x30000);
    TaskYieldTrampoline(gUnk_0874492C[sub_08002ee8(8)]);
    sub_080640c8();
    sub_080031b8(506);
    sub_08098450();
    sub_08006338(14);
    TaskYieldTrampoline(3);
    sub_08006338(15);
    TaskYieldTrampoline(3);
    sub_08006338(8);
    TaskYieldTrampoline(3);
    sub_08006338(18);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_08006338(19);
    TaskYieldTrampoline(8);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08098268(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 7) {
        sub_08006148(sub_080975ac, gCurTaskIdx);
    } else {
        gUnk_03002490->unk24 = sub_08064188(t->unk24);
    }
}

void sub_0809829c(void)
{
    s32 zero;

    gUnk_03002490->unk15 = 8;
    zero = 0;
    sub_080062c4();
    gUnk_03002490->unk2C = zero;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 28);
    sub_08064d34(142, 0);
    sub_080639a4(gUnk_0874544C);
    gUnk_03002490->unk7A = zero;
    gUnk_03002490->unk28 = zero;
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x30000, 0x2500, 0x30000);
    sub_08006338(28);
    sub_080639b4(gUnk_08745254);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_080639b4(gUnk_087452A8);
    sub_08063a00(gUnk_087452C4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(29);
    sub_080261d4(4);
    sub_08064d34(141, 0);
    sub_0806cc90(0, 1, -4, 12);
    TaskYieldTrampoline(170);
    sub_0806ad18();
    sub_0806a344();
}

void sub_080983a0(void)
{
}

void sub_080983a4(void)
{
    struct Task *t;
    s32 k;
    s32 r;

    t = gUnk_03002490;
    t->unk1C++;
    if (t->unk1C > 2) {
        t->unk1C = 0;
        sub_0806395c(7);
        return;
    }
    k = 0;
    gUnk_030023D4 = gUnk_03002170[sub_08063b38()].unk04;
    switch (gUnk_030023D4) {
    case 14:
        k = 2;
        break;
    case 4:
    case 5:
        k = 1;
        break;
    }
    if (gUnk_03002490->unk74 != 0)
        k += 3;
    gUnk_030023D4 = r = sub_08002ee8(8);
    if (r < gUnk_08744934[k]) {
        sub_0806395c(3);
        return;
    }
    if (r < gUnk_0874494C[k]) {
        sub_0806395c(5);
        return;
    }
    sub_0806395c(2);
}

void sub_08098450(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct ActorSpawn *p;
    s8 d;

    sp.unk00 = 35;
    sp.unk04 = 138;
    p = &sp;
    t = gUnk_03002490;
    p->unk08 = t->unk73;
    p->unk09 = t->unk74;
    p->unk10 = t->unk8C->unk20;
    p->unk0C = 12;
    p->unk0E = 8;
    p->unk0A = 1;
    d = t->unk43;
    sub_08063e14();
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 1);
    gUnk_03002490->unk43 = d;
}

void sub_080984b4(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 i;

    if (sub_0806914c(gUnk_087452E0) != 0) {
        p = gUnk_03002170;
        t = gUnk_03002490;
        i = t->unk7E;
        if (p[i].unk0D != 17 || p[i].unk04 != 13) {
            t->unk18 = i;
            sub_08063ddc(i);
            sub_080031b8(568);
            sub_080685ec(gUnk_03002490->unk18, gCurTaskIdx, 7);
            sub_0806395c(6);
            sub_08006148(sub_080975ac, gCurTaskIdx);
        }
    }
}

s32 sub_08098528(void)
{
    sub_08006244();
    gUnk_03002490->unk28 = 1;
    return 0;
}

s32 sub_08098540(void)
{
    struct Task *t;
    s32 d;

    t = gUnk_03002490;
    if (t->unk14 != 3)
        return 0;
    d = t->unk43;
    if (t->unk54 < 0) {
        if (d < 0)
            goto hit;
        goto miss;
    }
    if (d <= 0)
        goto miss;
hit:
    sub_0806395c(4);
    return 1;
miss:
    sub_08006214();
    return 0;
}

s32 sub_0809857c(void)
{
    sub_08064d34(142, 0);
    sub_080261d4(2);
    return 0;
}

s32 sub_08098594(void)
{
    struct Task *t;

    sub_0806a008();
    t = gUnk_03002490;
    if (t->unk18 >= 0) {
        sub_080689c8(t->unk18, -t->unk43);
        gUnk_03002490->unk18 = -1;
    }
    sub_0806395c(8);
    sub_08006148(sub_080975ac, gCurTaskIdx);
    return 1;
}

void sub_080985e0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809869c;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    zero = 0;
    gUnk_03002490->unk38 = gUnk_08753054;
    sub_08063e14();
    sub_080062c4();
    sub_080061c0(gUnk_087454D8[0] >> 1, 0x5A5A5A5A);
    sub_0800622c(gUnk_087454EC[gUnk_03002490->unk74], 0x3000, 0x30000);
    gUnk_03002490->unk7A = zero;
    gUnk_03002490->unk2C = zero;
    while (1) {
        u = gUnk_03002490;
        switch (u->unk2C) {
        case 0:
            if (u->unk8C->unk2C != gUnk_087454B8)
                gUnk_03002490->unk28 = sub_080640dc(gUnk_087454B8);
            break;
        case 1:
            if (u->unk8C->unk2C != gUnk_087454C4)
                gUnk_03002490->unk28 = sub_080640dc(gUnk_087454C4);
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void sub_0809869c(void)
{
    struct Task *t;
    s32 x;

    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
    x = sub_08064188(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk2C != 0 && sub_08098748() != 0)
        sub_080061c0(gUnk_087454D8[gUnk_03002490->unk1C], 0x5A5A5A5A);
}

void sub_080986ec(void)
{
    struct Task *t;

    sub_08006244();
    t = gUnk_03002490;
    t->unk1C = -1;
    t->unk2C = 1;
}

void sub_08098708(void)
{
    gUnk_03002490->unk2C = 0;
}

void sub_08098718(void)
{
    sub_0806a0f0(-2);
}

void sub_08098728(void)
{
    sub_080261d4(1);
    sub_0806a25c();
}

void sub_08098738(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk58 = -t->unk58;
}

s32 sub_08098748(void)
{
    struct Task *t;
    s32 v;

    v = sub_08094d10();
    t = gUnk_03002490;
    if (t->unk1C != (s8)v) {
        t->unk1C = (s8)v;
        return 1;
    }
    return 0;
}

void sub_0809876c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *y;
    struct Task *z;
    struct Task *q;
    s32 zero;
    s32 v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    zero = 0;
    u = gUnk_03002490;
    u->unk38 = gUnk_087538B0;
    u->unk04 = (u32)sub_0809887c;
    sub_08063e14();
    v = sub_080640dc(gUnk_0874550C);
    w = gUnk_03002490;
    w->unk28 = v;
    w->unk7A = zero;
    y = gUnk_03002490;
    y->unk34 = zero;
    sub_080061c0(gUnk_087454F4[y->unk74], 0x5A5A5A5A);
    sub_0800622c(0x2AF00, 0x3000, 0x30000);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk7A = 0;
    z = gUnk_03002490;
    z->unk34 = 0;
    sub_080061c0(gUnk_087454FC[z->unk74], 0x5A5A5A5A);
    sub_0800622c(gUnk_08745504[gUnk_03002490->unk74], 0x3000, 0x30000);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    while (1) {
        gUnk_03002490->unk7A = 0;
        q = gUnk_03002490;
        q->unk34 = 0;
        q->unk58 = -0x30000;
        while (gUnk_03002490->unk34 == 0)
            TaskYieldTrampoline(1);
    }
}

void sub_0809887c(void)
{
    struct Task *t;
    s32 x;

    x = sub_08064188(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_080988a4(void)
{
    gUnk_03002490->unk34 = 1;
    return 0;
}

s32 sub_080988b4(void)
{
    sub_0806a25c();
    return 1;
}

void sub_080988c0(void)
{
}

s32 sub_080988c4(void)
{
    if (gUnk_03002490->unk14 == 0) {
        sub_0806395c(16);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}
