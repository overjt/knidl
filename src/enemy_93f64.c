#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern s32 gUnk_030023D4;
extern vu16 gUnk_03004CA0[];
extern vu16 gUnk_03001EA4;
extern s32 gUnk_030023B4;
extern u8 gUnk_03005550[];

/* ROM tables */
extern u32 gUnk_087536FC[];
extern u32 gUnk_0826F170[];
extern u32 gUnk_087441BC[];
extern u32 gUnk_087441C4[];
extern u32 gUnk_087441CC[];
extern u32 gUnk_087441D0[];
extern u32 gUnk_087441D4[];
extern struct AnimCmd gUnk_0874433C[];
extern struct AnimCmd gUnk_08744360[];
extern struct AnimCmd gUnk_08744384[];
extern struct AnimCmd gUnk_087443A8[];
extern struct AnimCmd gUnk_087443BC[];
extern struct AnimCmd gUnk_087443D0[];
extern struct AnimCmd gUnk_087443E4[];
extern struct AnimCmd gUnk_08744408[];
extern struct AnimCmd gUnk_0874441C[];
extern u32 gUnk_08744440[];
extern u32 gUnk_08744444[];
extern u32 gUnk_08744470[];
extern u16 gUnk_0874449C[];
extern u32 gUnk_087444A4[];
extern u32 gUnk_087444AC[];
extern u32 gUnk_087444B4[];
extern u32 gUnk_087444BC[];
extern u32 gUnk_087444C4[];
extern u32 gUnk_087444CC[];
extern u32 gUnk_087449E8[];
extern u32 gUnk_08744A04[];
extern u32 gUnk_08744A20[];
extern u32 gUnk_08744A3C[];
extern u32 gUnk_08744A58[];
extern u32 gUnk_08744A74[];
extern u32 gUnk_08744A90[];
extern u32 gUnk_08744AAC[];
extern u32 gUnk_08744AC8[];
extern u32 gUnk_08744AE4[];
extern u32 gUnk_08744B00[];
extern u32 gUnk_08744B1C[];
extern u32 gUnk_08744B38[];
extern u32 gUnk_08744B54[];
extern u32 gUnk_08744B70[];
extern u32 gUnk_08744B8C[];
extern u32 gUnk_08744BA8[];
extern u32 gUnk_08744BC4[];
extern u32 gUnk_08744BE0[];
extern u32 gUnk_08744BFC[];
extern u32 gUnk_08744C18[];
extern u32 gUnk_08744C34[];
extern u32 gUnk_08744C50[];
extern u32 gUnk_08744C6C[];
extern u32 gUnk_08744C88[];
extern u32 gUnk_08744CA4[];
extern u32 gUnk_08744CC0[];
extern u32 gUnk_0874531C[];
extern u32 gUnk_08745324[];
extern u32 gUnk_0874532C[];
extern u32 gUnk_08745334[];
extern u32 gUnk_0874533C[];
extern u32 gUnk_08745344[];
extern u32 gUnk_0874534C[];
extern u32 gUnk_0874541C[];
extern u32 gUnk_08752F74[];

/* Externals */
extern void sub_0806523c(void);
extern void sub_08065438(void);
extern void sub_080656b4(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern s32 sub_080031b8(s32 id);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_0800634c(s32 a);
extern void sub_08006364(s32 a);
extern s32 sub_08021b18(u16 x, u16 y);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(void *p);
extern void sub_080639b4(void *p);
extern void sub_080639f0(void *p);
extern void sub_08063a00(void *p);
extern s32 sub_08063cd0(void);
extern s32 sub_08063df4(void);
extern void sub_08063e14(void);
extern void sub_08064038(void);
extern void sub_0806421c(s16 t, s16 mag);
extern u8 sub_08064358(void);
extern void sub_08064a60(void);
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
extern u32 sub_08068e04(void);
extern u32 sub_08068f68(void);
extern u8 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void sub_0806a008(void);
extern void sub_0806a344(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 sub_0806e6f8(s16 x, s16 y);
extern void TaskYieldTrampoline(u32 frames);

/* Defined below */
void sub_08093f64(void);
void sub_08093fe0(void);
void sub_08094010(void);
void sub_08094040(void);
void sub_08094144(void);
void sub_08094164(void);
void sub_080941ac(void);
void sub_08094220(void);
void sub_08094290(void);
void sub_080942b4(void);
void sub_080942dc(void);
void sub_08094358(void);
void sub_080945fc(void);
void sub_08094640(void);
void sub_0809465c(void);
void sub_0809467c(void);
void sub_080946b0(void);
void sub_08094758(void);
s32 sub_080947cc(void);
s32 sub_08094810(void);
void sub_08094844(void);
void sub_08094894(void);
void sub_080948d4(void);
void sub_08094908(void);
void sub_08094930(void);
void sub_080949e0(void);
s32 sub_08094b94(void);
void sub_08094bbc(void);
s32 sub_08094d10(void);
void sub_08094da4(void);
void sub_08094dec(void);
s32 sub_08094e88(void);
void sub_08094f28(void);
void sub_08094f68(void);
void sub_08094fb0(void);
void sub_080950b4(void);
void sub_0809513c(void);
void sub_08095220(void);
void sub_08095254(void);
void sub_0809532c(void);
void sub_08095360(void);
void sub_08095484(void);
s32 sub_080954f0(void);
s32 sub_080955a8(void);
void sub_08095674(void);
void sub_08095694(void);
s32 sub_080956c8(struct AnimCmd *p);
s32 sub_080956e4(void);
s32 sub_08095768(s32 a);
s32 sub_08095794(s32 a);

void sub_08093f64(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    zero = 0;
    gUnk_03002490->unk38 = gUnk_087536FC;
    sub_08064a60();
    u = gUnk_03002490;
    u->unk28 = gUnk_087441BC[u->unk73] * (s8)u->unk43 + 384;
    u->unk2C = zero;
    u->unk30 = 384;
    u->unk34 = 2;
    u->unk73 = zero;
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_087441CC);
}

void sub_08093fe0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08094010;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087441D0);
}

void sub_08094010(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087441D4);
    if (gUnk_03002490->unk2C == 1)
        sub_08068e04();
    sub_08069b44();
}

void sub_08094040(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gUnk_03002490->unk15 = 0;
    sub_08006338(4);
    sub_080031b8(506);
    sub_08094164();
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08094164();
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk30 -= 48;
        t->unk6C++;
    } while ((s16)t->unk6C <= 7);
    t->unk54 = 0;
    t->unk58 = 0;
    t->unk2C = 1;
    sub_08063e14();
    sub_08006304();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(4);
        u = gUnk_03002490;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    v = gUnk_03002490;
    v->unk58 = 0;
    sub_080061c0(gUnk_087441C4[v->unk74], 0x5A5A5A5A);
    while (1) {
        if (sub_08064358() == 1) {
            w = gUnk_03002490;
            w->unk58 += 0x800;
            if (w->unk58 > 0x10000)
                w->unk58 = 0x10000;
        } else {
            x = gUnk_03002490;
            x->unk58 += -0x800;
            if (x->unk58 < -0x10000)
                x->unk58 = -0x10000;
        }
        TaskYieldTrampoline(1);
    }
}

void sub_08094144(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk34--;
    if (t->unk34 == 0) {
        t->unk34 = 2;
        t->unk3C ^= 1;
    }
}

void sub_08094164(void)
{
    struct Task *t;

    t = gUnk_03002490;
    sub_0806421c(t->unk28, t->unk30);
    if (gUnk_03002490->unk74 != 0)
        gUnk_030023B4 += gUnk_030023B4 >> 1;
    t = gUnk_03002490;
    t->unk54 = gUnk_030023B4;
    t->unk58 = gUnk_030023D4;
}

void sub_080941ac(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752F74;
    sub_08066088(0);
    gUnk_02007D00[8]++;
    gUnk_03002490->unk34 = -1;
    sub_08063e14();
    sub_080692fc();
    sub_080666cc(gUnk_0874433C);
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08744440);
}

void sub_08094220(void)
{
    struct Task *t;
    struct Task *u;

    if (sub_08067060() != 0) {
        t = gUnk_03002490;
        t->unk04 = (u32)sub_080942b4;
        t->unk7A = 0;
        sub_0806395c(0);
        sub_08002e98(gUnk_03002490->unk14, 11, gUnk_08744444);
    } else {
        u = gUnk_03002490;
        u->unk04 = (u32)sub_080942dc;
        u->unk7A = 1;
        sub_0806395c(1);
        sub_08002e98(gUnk_03002490->unk14, 11, gUnk_08744444);
    }
}

void sub_08094290(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080942dc;
    sub_08002e98(t->unk14, 11, gUnk_08744444);
}

void sub_080942b4(void)
{
    sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08744470);
    sub_08094358();
    sub_08068f68();
    sub_08069b44();
}

void sub_080942dc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk20 > 0) {
        t->unk20--;
        sub_080663f4(gUnk_0826F170, 16);
    } else {
        sub_08066468();
    }
    if (sub_0806acf8() == 0) {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08744470);
    } else {
        sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08744470);
    }
    sub_08094358();
    sub_08068f68();
    sub_08069b44();
}

void sub_08094358(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gUnk_03002490;
    if (t->unk14 == 10) {
        if (t->unk2C == 0 && t->unk58 < 0) {
            v = t->unk3C;
            if (v >= 4 && v <= 9) {
                sub_080639b4(gUnk_08744A20);
                sub_08063a00(gUnk_08744A04);
            } else if (v >= 10 && v <= 19) {
                sub_080639b4(gUnk_08744A74);
                sub_08063a00(gUnk_08744A58);
            } else if (v >= 20 && v <= 27) {
                sub_080639b4(gUnk_08744AC8);
                sub_08063a00(gUnk_08744AAC);
            } else if (v >= 28 && v <= 31) {
                sub_080639b4(gUnk_08744B54);
                sub_08063a00(gUnk_08744B38);
            } else if (v >= 32 && v <= 35) {
                sub_080639b4(gUnk_08744BA8);
                sub_08063a00(gUnk_08744B8C);
            } else if (v >= 36 && v <= 47) {
                sub_080639b4(gUnk_08744C34);
                sub_08063a00(gUnk_08744C18);
            } else {
                sub_080639b4(gUnk_08744C88);
                sub_08063a00(gUnk_08744C6C);
            }
        } else {
            u = gUnk_03002490;
            v = u->unk3C;
            if (v >= 20 && v <= 27) {
                sub_080639b4(gUnk_08744AE4);
                sub_08063a00(gUnk_08744B00);
            } else if (v >= 32 && v <= 35) {
                sub_080639b4(gUnk_08744BC4);
                sub_08063a00(gUnk_08744BE0);
            } else {
                sub_080639b4(gUnk_08744CA4);
                sub_08063a00(gUnk_08744CC0);
            }
        }
    } else {
        v = t->unk3C;
        if (v >= 4 && v <= 9) {
            sub_080639b4(gUnk_087449E8);
            sub_08063a00(gUnk_08744A04);
            sub_080639f0(gUnk_0874531C);
        } else if (v >= 10 && v <= 19) {
            sub_080639b4(gUnk_08744A3C);
            sub_08063a00(gUnk_08744A58);
            sub_080639f0(gUnk_08745324);
        } else if (v >= 20 && v <= 27) {
            sub_080639b4(gUnk_08744A90);
            sub_08063a00(gUnk_08744AAC);
            sub_080639f0(gUnk_0874532C);
        } else if (v >= 28 && v <= 31) {
            sub_080639b4(gUnk_08744B1C);
            sub_08063a00(gUnk_08744B38);
            sub_080639f0(gUnk_08745334);
        } else if (v >= 32 && v <= 35) {
            sub_080639b4(gUnk_08744B70);
            sub_08063a00(gUnk_08744B8C);
            sub_080639f0(gUnk_0874533C);
        } else if (v >= 36 && v <= 47) {
            sub_080639b4(gUnk_08744BFC);
            sub_08063a00(gUnk_08744C18);
            sub_080639f0(gUnk_08745344);
        } else {
            sub_080639b4(gUnk_08744C50);
            sub_08063a00(gUnk_08744C6C);
            sub_080639f0(gUnk_0874534C);
        }
    }
}

void sub_080945fc(void)
{
    struct Task *t;
    s32 v;

    gUnk_03002490->unk15 = 0;
    v = sub_080956c8(gUnk_0874433C);
    t = gUnk_03002490;
    t->unk28 = v;
    t->unk60 = 9472;
    t->unk68 = 0x30000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk04 = (u32)sub_080942dc;
    sub_08006138();
}

void sub_08094640(void)
{
    gUnk_03002490->unk28 = sub_08095794(gUnk_03002490->unk28);
}

void sub_0809465c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_08066580();
    sub_0806395c(2);
    sub_08006138();
}

void sub_0809467c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 1) {
        sub_08006148(sub_08094290, gCurTaskIdx);
    } else {
        gUnk_03002490->unk28 = sub_08095794(t->unk28);
    }
}

void sub_080946b0(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    if (sub_08094810() != 0) {
        sub_0806395c(8);
        sub_08006138();
    }
    t = gUnk_03002490;
    t->unk2C = t->unk48;
    switch (sub_080947cc()) {
    case 0:
        gUnk_03002490->unk30 = gUnk_030023D4;
        gUnk_03002490->unk28 = sub_080956c8(gUnk_0874433C);
        sub_08006138();
        break;
    case 1:
        gUnk_03002490->unk30 = gUnk_030023D4;
        gUnk_03002490->unk28 = sub_080956c8(gUnk_087443A8);
        while (1) {
            sub_080061c0(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            sub_080061c0(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08094758(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 x;

    t = gUnk_03002490;
    if (t->unk14 != 2)
        goto rearm;
    x = sub_08095794(t->unk28);
    u = gUnk_03002490;
    u->unk28 = x;
    u->unk30--;
    if (u->unk30 != 0)
        return;
    sub_08006214();
    v = gUnk_03002490;
    v->unk4C = v->unk2C << 16;
    if (sub_08002ee8(4) == 0)
        goto quiet;
    sub_0806395c(4);
rearm:
    sub_08006148(sub_08094290, gCurTaskIdx);
    return;
quiet:
    sub_0806395c(3);
    sub_08006148(sub_08094290, gCurTaskIdx);
}

s32 sub_080947cc(void)
{
    sub_08063e14();
    gUnk_030023D4 = gUnk_0874449C[gUnk_03002490->unk74 * 2 + (gUnk_03001EA4 & 1)];
    return gUnk_03001EA4 & 1;
}

s32 sub_08094810(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk34 = (t->unk34 + 1) & 3;
    if (t->unk34 == 3) {
        if (t->unk18 > 3)
            t->unk18 = 0;
        u = gUnk_03002490;
        if (u->unk18 == 0)
            return 1;
        u->unk18++;
    }
    return 0;
}

void sub_08094844(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    if (sub_08094810() != 0) {
        sub_0806395c(8);
        sub_08006138();
    }
    sub_08063e14();
    gUnk_03002490->unk28 = sub_080956c8(gUnk_08744360);
    sub_08094908();
    gUnk_03002490->unk2C = gUnk_030023D4;
    sub_08094894();
}

void sub_08094894(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C != 0) {
        t->unk2C--;
        t->unk7A = 0;
        sub_0800622c(-0x50000, 0x4A00, 0x70000);
        sub_08006138();
    }
    sub_0806395c(4);
    sub_08006138();
}

void sub_080948d4(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk14 != 3)
        sub_08006148(sub_08094290, gCurTaskIdx);
}

void sub_08094908(void)
{
    if (sub_08002ee8(2) != 0)
        gUnk_030023D4 = 1;
    else
        gUnk_030023D4 = 3;
}

void sub_08094930(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    if (sub_08094810() != 0) {
        sub_0806395c(8);
        sub_08006138();
    }
    sub_08063e14();
    t = gUnk_03002490;
    t->unk30 = t->unk43;
    gUnk_02007D00[1] = sub_080031b8(0x209);
    if (gUnk_03002490->unk8C->unk2C != gUnk_08744384)
        gUnk_03002490->unk28 = sub_080956c8(gUnk_08744384);
    TaskYieldTrampoline(45);
    gUnk_03002490->unk2C = sub_08094b94();
    gUnk_03002490->unk46 = sub_0806e6f8(-10, 5);
    sub_080061c0(gUnk_087444A4[gUnk_03002490->unk74], 0x5A5A5A5A);
    gUnk_03002490->unk1C = -1;
    sub_08006138();
}

void sub_080949e0(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 x;
    s32 y;
    s32 z;
    s32 q;
    s16 *p;
    u16 a;

    x = sub_08095794(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk14 != 4)
        goto rearm;
    if (t->unk54 == 0)
        return;
    sub_08094bbc();
    if (gUnk_03002490->unk43 != sub_08063df4())
        goto other;
    if (gUnk_03002490->unk2C != 0)
        return;
    y = sub_08094b94();
    gUnk_03002490->unk2C = y;
    if (y == 0)
        return;
    if (sub_08002ee8(3) != 0)
        return;
    v = gUnk_03002490;
    if (v->unk18 != 0)
        return;
    v->unk18 = 1;
    sub_080034f0(gUnk_02007D00[1], 0x209);
    if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
        sub_08005654(gUnk_03002490->unk46);
    sub_0806395c(7);
rearm:
    sub_08006148(sub_08094290, gCurTaskIdx);
    return;
other:
    w = gUnk_03002490;
    if (w->unk2C < 0) {
        a = w->unk48;
        p = &w->unk4A;
        switch (sub_08021b18(a, ((s8 *)w->unk8C->unk50)[3] + *p)) {
        case 2:
            if (gUnk_03002490->unk43 == -1)
                return;
            break;
        case 3:
            if (gUnk_03002490->unk43 == 1)
                return;
            break;
        }
        sub_080034f0(gUnk_02007D00[1], 0x209);
        if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
            sub_08005654(gUnk_03002490->unk46);
        sub_0806395c(5);
        sub_08006148(sub_08094290, gCurTaskIdx);
    }
    z = sub_08094b94();
    gUnk_03002490->unk2C = z;
    if (z == 1)
        return;
    gUnk_03002490->unk2C = -1;
    q = sub_08002ee8(3);
    if (q != 0)
        return;
    gUnk_03002490->unk18 = q;
    sub_080034f0(gUnk_02007D00[1], 0x209);
    if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
        sub_08005654(gUnk_03002490->unk46);
    sub_0806395c(6);
    sub_08006148(sub_08094290, gCurTaskIdx);
}

s32 sub_08094b94(void)
{
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        return 1;
    return 0;
}

void sub_08094bbc(void)
{
    struct Task *t;
    u8 v;
    s32 r;

    t = gUnk_03002490;
    r = sub_08094d10();
    v = r;
    if (t->unk1C == (s8)r)
        return;
    gUnk_03002490->unk1C = (s8)v;
    switch ((s8)v) {
    case 0:
        if (gUnk_03002490->unk8C->unk2C != gUnk_08744360)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_08744360);
        sub_080061c0(gUnk_087444A4[gUnk_03002490->unk74], 0x5A5A5A5A);
        break;
    case 1:
        if (gUnk_03002490->unk8C->unk2C != gUnk_08744360)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_08744360);
        sub_080061c0(gUnk_087444AC[gUnk_03002490->unk74], 0x5A5A5A5A);
        break;
    case 2:
        if (gUnk_03002490->unk8C->unk2C != gUnk_08744360)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_08744360);
        sub_080061c0(gUnk_087444B4[gUnk_03002490->unk74], 0x5A5A5A5A);
        break;
    case 3:
        if (gUnk_03002490->unk8C->unk2C != gUnk_0874433C)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_0874433C);
        sub_080061c0(gUnk_087444BC[gUnk_03002490->unk74], 0x5A5A5A5A);
        break;
    case 4:
        if (gUnk_03002490->unk8C->unk2C != gUnk_08744384)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_08744384);
        sub_080061c0(gUnk_087444C4[gUnk_03002490->unk74], 0x5A5A5A5A);
        break;
    }
}

s32 sub_08094d10(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if ((t->unk7A & 1) != 0) {
        if ((u8)(gUnk_03005550[4] - 1) <= 3) {
            switch (gUnk_03005550[4]) {
            case 3:
                if (t->unk43 == 1)
                    return 1;
                return 2;
            case 1:
                if (t->unk43 == 1)
                    return 3;
                return 4;
            case 4:
                if (t->unk43 == 1)
                    return 2;
                return 1;
            case 2:
                if (t->unk43 == 1)
                    return 4;
                return 3;
            }
        } else {
            return 0;
        }
    }
    return -1;
}

void sub_08094da4(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 5;
    t = gUnk_03002490;
    t->unk30 = 60;
    t->unk2C = t->unk43;
    t->unk1C = 0;
    gUnk_03002490->unk28 = sub_080956c8(gUnk_087443D0);
    sub_080031b8(0x20A);
    sub_08006138();
}

void sub_08094dec(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;

    x = sub_08095794(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk8C->unk2C == 0) {
        sub_08064038();
        gUnk_03002490->unk28 = sub_080956c8(gUnk_0874433C);
    }
    if (sub_08094e88() != 0) {
        gUnk_03002490->unk54 = 0;
        sub_0806395c(2);
        sub_08006148(sub_08094290, gCurTaskIdx);
        return;
    }
    u = gUnk_03002490;
    u->unk30--;
    if (u->unk30 >= 0)
        return;
    if ((gUnk_03001EA4 & 1) != 0) {
        sub_0806395c(4);
        sub_08006148(sub_08094290, gCurTaskIdx);
        return;
    }
    sub_0806395c(2);
    sub_08006148(sub_08094290, gCurTaskIdx);
}

s32 sub_08094e88(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    if (gUnk_03002490->unk2C == 1) {
        a = sub_08094d10();
        t = gUnk_03002490;
        t->unk54 -= gUnk_087444CC[a];
        if (t->unk54 < 0)
            return 1;
    } else {
        b = sub_08094d10();
        u = gUnk_03002490;
        u->unk54 += gUnk_087444CC[b];
        if (u->unk54 > 0)
            return 1;
    }
    v = gUnk_03002490;
    v->unk1C++;
    if (v->unk1C == 8) {
        if (v->unk2C == v->unk43)
            sub_0806cc90(1, 1, -4, 12);
        else
            sub_0806cc90(1, 1, -4, 12);
        gUnk_03002490->unk1C = 0;
    }
    return 0;
}

void sub_08094f28(void)
{
    struct Task *t;
    s32 zero;
    s32 v;

    gUnk_03002490->unk15 = 6;
    zero = 0;
    v = sub_080956c8(gUnk_087443BC);
    t = gUnk_03002490;
    t->unk28 = v;
    t->unk2C = t->unk43;
    t->unk1C = zero;
    sub_080031b8(0x20A);
    sub_08006138();
}

void sub_08094f68(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk20 > 0)
        return;
    gUnk_03002490->unk28 = sub_08095794(t->unk28);
    if (sub_08094e88() != 0) {
        gUnk_03002490->unk54 = 0;
        sub_0806395c(2);
        sub_08006148(sub_08094290, gCurTaskIdx);
    }
}

void sub_08094fb0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;
    s32 one;
    s32 v;

    gUnk_03002490->unk15 = 7;
    zero = 0;
    gUnk_03002490->unk18 = one = 1;
    v = sub_080956c8(gUnk_087443D0);
    t = gUnk_03002490;
    t->unk28 = v;
    t->unk2C = t->unk43;
    t->unk30 = zero;
    t->unk1C = zero;
    sub_080031b8(0x20A);
    TaskYieldTrampoline(30);
    gUnk_03002490->unk28 = sub_080956c8(gUnk_08744384);
    gUnk_03002490->unk46 = sub_0806e6f8(-10, 5);
    sub_080061c0(gUnk_087444A4[0], 0x5A5A5A5A);
    gUnk_03002490->unk30 = one;
    gUnk_02007D00[1] = sub_080031b8(0x209);
    TaskYieldTrampoline(30);
    if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
        sub_08005654(gUnk_03002490->unk46);
    gUnk_03002490->unk28 = sub_080956c8(gUnk_087443D0);
    sub_080034f0(gUnk_02007D00[1], 0x209);
    u = gUnk_03002490;
    u->unk2C = u->unk43;
    u->unk30 = zero;
    u->unk1C = zero;
    sub_080031b8(0x20A);
    TaskYieldTrampoline(30);
    gUnk_03002490->unk30 = 2;
    sub_08006138();
}

void sub_080950b4(void)
{
    struct Task *t;
    s32 v;

    t = gUnk_03002490;
    v = t->unk30;
    switch (v) {
    case 0:
        gUnk_03002490->unk28 = sub_08095794(t->unk28);
        if (gUnk_03002490->unk8C->unk2C == 0) {
            sub_08064038();
            gUnk_03002490->unk28 = sub_080956c8(gUnk_0874433C);
        }
        if (sub_08094e88() != 0)
            gUnk_03002490->unk54 = v;
        break;
    case 1:
        gUnk_03002490->unk28 = sub_08095794(t->unk28);
        sub_08094bbc();
        break;
    case 2:
        sub_0806395c(4);
        sub_08006148(sub_08094290, gCurTaskIdx);
        break;
    }
}

void sub_0809513c(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;
    s32 zero;

    gUnk_03002490->unk15 = 8;
    zero = 0;
    sub_080062c4();
    sub_080956c8(gUnk_087443E4);
    sub_0800622c(-0x30000, 0x3500, 0x30000);
    gUnk_03002490->unk7A = zero;
    gUnk_03002490->unk2C = zero;
    do {
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk2C == 0);
    sub_0800622c(-0x18000, 0x3500, 0x30000);
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk2C == 0);
    TaskYieldTrampoline(16);
    sub_080031b8(506);
    t = gUnk_03002490;
    a = t->unk8C;
    sp.unk00 = 12;
    sp.unk04 = 114;
    sp.unk08 = t->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = a->unk20;
    sp.unk0A = 1;
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 1);
    TaskYieldTrampoline(20);
    sub_0806395c(2);
    sub_08006138();
}

void sub_08095220(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk14 != 8)
        sub_08006148(sub_08094290, gCurTaskIdx);
}

void sub_08095254(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 9;
    t = gUnk_03002490;
    if (t->unk54 > 0)
        t->unk43 = 1;
    else
        t->unk43 = -1;
    sub_080031b8(0x1F7);
    sub_080261d4(2);
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x30000, 0x2500, 0x30000);
    gUnk_03002490->unk7A = 0;
    sub_080956c8(gUnk_08744408);
    gUnk_03002490->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk2C == 0);
    sub_080061c0(-0x8000, 0x5A5A5A5A);
    sub_0800622c(-0x20000, 0x2500, 0x30000);
    gUnk_03002490->unk7A = 0;
    sub_080956c8(gUnk_0874441C);
    gUnk_03002490->unk2C = 0;
    do {
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk2C == 0);
    TaskYieldTrampoline(30);
    sub_0806395c(2);
    sub_08006138();
}

void sub_0809532c(void)
{
    struct Task *t;
    s32 x;

    x = sub_08095794(gUnk_03002490->unk28);
    t = gUnk_03002490;
    t->unk28 = x;
    if (t->unk14 != 9)
        sub_08006148(sub_08094290, gCurTaskIdx);
}

void sub_08095360(void)
{
    struct Task *t;
    s32 z1;
    s32 z2;

    gUnk_03002490->unk15 = 10;
    z1 = 0;
    if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
        sub_08005654(gUnk_03002490->unk46);
    sub_080639a4(gUnk_0874541C);
    gUnk_03002490->unk2C = z1;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 32);
    sub_08064d34(142, 0);
    gUnk_03002490->unk28 = sub_080956c8(gUnk_08744408);
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    sub_0800622c(-0x30000, 0x1A00, 0x30000);
    gUnk_03002490->unk7A = z1;
    while (gUnk_03002490->unk2C == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_08064d34(141, 0);
    sub_080956c8(gUnk_0874441C);
    sub_080061c0(-0x8000, 0x5A5A5A5A);
    sub_080261d4(2);
    gUnk_03002490->unk1C = z2 = 0;
    TaskYieldTrampoline(32);
    sub_080062c4();
    t = gUnk_03002490;
    t->unk2C = 2;
    t->unk30 = z2;
    sub_08064d34(141, 0);
    TaskYieldTrampoline(170);
    sub_0806ad18();
    gUnk_03002490->unk30 = 1;
    sub_08006138();
}

void sub_08095484(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk28 = sub_08095794(gUnk_03002490->unk28);
    sub_080663f4(gUnk_0826F170, 16);
    t = gUnk_03002490;
    if (t->unk2C == 1) {
        t->unk1C++;
        if (t->unk1C == 16) {
            sub_0806cc90(0, 1, 8, 10);
            gUnk_03002490->unk1C = 0;
        }
    }
    u = gUnk_03002490;
    if (u->unk2C == 2 && u->unk30 != 0)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

s32 sub_080954f0(void)
{
    switch (gUnk_03002490->unk14) {
    case 0:
        sub_08006244();
        sub_080261d4(2);
        sub_080031b8(504);
        sub_0806395c(1);
        sub_08006148(sub_08094290, gCurTaskIdx);
        return 1;
    case 3:
        sub_08006244();
        sub_080261d4(2);
        sub_080031b8(0x1F7);
        if (gUnk_03002490->unk8C->unk2C != gUnk_08744384)
            gUnk_03002490->unk28 = sub_080956c8(gUnk_08744384);
        sub_08006148(sub_08094894, gCurTaskIdx);
        return 1;
    case 8:
    case 9:
    case 10:
        sub_080062c4();
        if (gUnk_03002490->unk14 != 10)
            sub_080031b8(504);
        gUnk_03002490->unk2C = 1;
        break;
    }
    return 0;
}

s32 sub_080955a8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk14) {
    case 4:
        sub_080034f0(gUnk_02007D00[1], 0x209);
        if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
            sub_08005654(gUnk_03002490->unk46);
        sub_0806395c(9);
        sub_08006148(sub_08094290, gCurTaskIdx);
        return 1;
    case 5:
    case 6:
    case 10:
        sub_08006214();
        break;
    case 7:
        if (gUnk_03002490->unk30 == 0) {
            sub_08006214();
            break;
        }
        sub_080062c4();
        sub_080034f0(gUnk_02007D00[1], 0x209);
        if (gUnk_03004CA0[gUnk_03002490->unk46] == 167)
            sub_08005654(gUnk_03002490->unk46);
        sub_0806395c(9);
        sub_08006148(sub_08094290, gCurTaskIdx);
        return 1;
    case 8:
    case 9:
        break;
    }
    return 0;
}

void sub_08095674(void)
{
    sub_08064d34(142, 0);
    sub_080261d4(2);
    gUnk_03002490->unk20 = 32;
}

void sub_08095694(void)
{
    sub_0806a008();
    sub_080034f0(gUnk_02007D00[1], 0x209);
    sub_0806395c(10);
    sub_08006148(sub_08094290, gCurTaskIdx);
}

s32 sub_080956c8(struct AnimCmd *p)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    a->unk2C = p;
    a->unk09 = 0;
    return sub_080956e4();
}

s32 sub_080956e4(void)
{
    struct Actor *a;
    struct AnimCmd *c;
    s32 r;

    a = gUnk_03002490->unk8C;
    c = a->unk2C;
    c += a->unk09;
    if (c->unk00 == -3) {
        a->unk09 = 0;
        c = a->unk2C;
    } else if (c->unk00 == -2) {
        r = c->unk00;
        a->unk2C = 0;
        goto tail;
    }
    if (gUnk_03002490->unk43 == 1) {
        if ((c->unk00 & 1) != 0)
            sub_08006364(c->unk00);
        else
            sub_0800634c(c->unk00);
    } else {
        if ((c->unk00 & 1) != 0)
            sub_0800634c(c->unk00);
        else
            sub_08006364(c->unk00);
    }
    r = c->unk02;
tail:
    a->unk09++;
    return r;
}

s32 sub_08095768(s32 a)
{
    if (gUnk_03002490->unk8C->unk2C != 0) {
        if (a <= 0) {
            sub_08063e14();
            a = sub_080956e4();
        }
        a--;
    }
    return a;
}

s32 sub_08095794(s32 a)
{
    if (gUnk_03002490->unk8C->unk2C != 0) {
        if (a <= 0)
            a = sub_080956e4();
        a--;
    }
    return a;
}
