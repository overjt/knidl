/* game_code_and_rodata 0x0809000C-0x0809113C (issue #67, module M25 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0809000C 0x0809113C src/enemy_9000c.c --newpb
 *
 * The first of M25's four boss scripts, dispatched through the 23-entry
 * anchor table at 0x08743848.  sub_0809000c is the task entry: it installs
 * sub_080656b4 as the draw hook (Task.unk00) and sub_08065438 as the
 * per-frame hook (Task.unk0C), points Task.unk38 at the graphics block
 * gUnk_08752ED8, counts the boss into gUnk_02007D00[0], spawns its helper
 * task with sub_08064d34(177, 1) and hands Task.unk73 to sub_08002e98, which
 * jumps into the table.  sub_080900f4 is the per-frame body: it runs down
 * Task.unk18, asks sub_0806acf8 / sub_080692fc whether the player interrupted,
 * dispatches Task.unk15 through the same table, reloads the graphics through
 * sub_080663f4 / sub_08066468 and drives the three animation calls from the
 * per-frame row gUnk_087437D0[Task.unk3C].
 *
 * States 0-10 then follow as <body, guard> pairs (sub_080901e0 /
 * sub_08090270, sub_08090298 / sub_080903c8, ...): the body is a run of
 * TaskYieldTrampoline waits that steps Task.unk3C, clears and then waits on
 * Task.unk7A (set when the boss lands) and pushes 16.16 velocities through
 * sub_080061c0 / sub_0800622c, and the guard re-arms sub_080900d8 through
 * sub_08006148 whenever Task.unk14 leaves the state.  State 4 aims with
 * Div(|sub_08063cd0()|, 3), state 5 spawns the actors 8 and 145, and state 10
 * is the defeat sequence (sub_0806684c, sub_0806caa0, sub_0806ad18).
 *
 * The tail holds the pieces the states share - sub_08090e18 (fire a shot at
 * the boss's own position through sub_08067120), sub_08090e54 (advance the
 * animation from gUnk_08743744[Task.unk28]), sub_08090e9c (the hover loop),
 * the sub_08090ef0 / sub_08090f14 / sub_08090f4c hit hooks, the companion
 * task sub_08090fc0 / sub_08090fe0 that mirrors the boss's position while
 * gUnk_03004CA0[Task.unk44] says the boss is alive - and sub_080910c0, the
 * entry of the second boss, whose states live in src/enemy_9113c.c.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u32 gUnk_02007D00[];
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern u16 gUnk_08743744[];
extern u32 gUnk_08743734[];
extern u32 gUnk_087440DC[];
extern u16 gUnk_087438A4[];
extern u16 gUnk_087438B2[];
extern u32 gUnk_087438C0[];
extern u32 gUnk_08743984[];
extern u32 gUnk_087530F8[];
extern u32 gUnk_0874373C[];
extern u8 gUnk_0874374E[];
extern u32 gUnk_08743750[];
extern struct AnimCmd *gUnk_087437C8[];
extern u8 gUnk_087437D0[];
extern u32 gUnk_087437F4[];
extern u32 gUnk_08743810[];
extern u32 gUnk_0874382C[];
extern u32 gUnk_08743848[];
extern struct AnimCmd gUnk_08743758[];
extern u32 gUnk_0874384C[];
extern u32 gUnk_08743878[];
extern u32 gUnk_08752ED8[];
extern void *gUnk_0826A668;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u16 sub_08066088(u32 mode);
extern void sub_08066ae0(void);
extern void sub_08065438(void);
extern void sub_080656b4(void);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u8 sub_08067060(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_0806395c(u16 v);
extern u8 sub_0806acf8(void);
extern u8 sub_080692fc(void);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_080639b4(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern void sub_08068f68(void);
extern s32 sub_08069b44(void);
extern void sub_08006338(s32 a);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080031b8(s32 id);
extern void sub_080062c4(void);
extern void sub_080261d4(s32 a);
extern void sub_08066580(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_08063e14(void);
extern void sub_080061c0(s32 a, s32 b);
extern u32 sub_08002ee8(u32 range);
extern void sub_08006304(void);
extern void sub_0806684c(void);
extern void sub_080667c0(u8 a, u16 b);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern void sub_08006244(void);
extern void sub_08006214(void);
extern void sub_0806ad18(void);
extern void sub_0806a344(void);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_080639a4(u32 *p);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern u32 sub_08068cf8(s32 a);
extern void sub_08005654(s32 id);
extern void sub_08065350(void);
extern void sub_0806cffc(s16 dx, s16 dy);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 Div(s32 numerator, s32 denominator);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);

/* Defined below */
void sub_080900d8(void);
void sub_080900f4(void);
void sub_08090e18(void);
void sub_08090e9c(void);
void sub_08090fe0(void);
void sub_08090e54(void);

void sub_0809000c(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752ED8;
    gUnk_02007D00[0]++;
    gUnk_03002490->unk46 = sub_08064d34(177, 1);
    if (sub_08067060() == 1)
        gUnk_03002490->unk18 = 24;
    else
        gUnk_03002490->unk18 = 0;
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08743848);
}

void sub_08090090(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080900f4;
    t->unk28 = 4;
    t->unk2C = 2;
    t->unk30 = 0;
    t->unk34 = 0;
    sub_080666cc(gUnk_08743758);
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0874384C);
}

void sub_080900d8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0874384C);
}

void sub_080900f4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk18 != 0)
    {
        t->unk18--;
        sub_08002e98(t->unk15, 11, gUnk_08743878);
    }
    else if (sub_0806acf8() == 0)
    {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08743878);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08743878);
    }
    u = gUnk_03002490;
    if (u->unk30 != 0)
    {
        if (u->unk75 != 0)
            sub_080663f4(&gUnk_0826A668, 16);
        else
        {
            u->unk30 = 0;
            sub_08066468();
        }
    }
    sub_080639b4(gUnk_087437F4[gUnk_087437D0[gUnk_03002490->unk3C]]);
    sub_080639f0(gUnk_08743810[gUnk_087437D0[gUnk_03002490->unk3C]]);
    sub_08063a00(gUnk_0874382C[gUnk_087437D0[gUnk_03002490->unk3C]]);
    sub_08068f68();
    sub_08069b44();
}

void sub_080901e0(void)
{
    struct Task *t;
    u8 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    if (sub_08067060() == 1)
    {
        gUnk_03002490->unk7A = zero;
        sub_08006338(13);
        sub_0800622c(0, 5376, 196608);
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080031b8(0x1F7);
        sub_080062c4();
        sub_080261d4(2);
        sub_08066580();
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(30);
    }
    else
    {
        sub_08066580();
    }
    sub_08090e54();
    sub_08006138();
}

void sub_08090270(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_08090298(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 1;
    sub_080062c4();
    sub_08063e14();
    sub_080061c0(gUnk_08743734[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08006338(7);
    gUnk_03002490->unk1C = (sub_08002ee8(3) + 2) * 4;
    gUnk_03002490->unk6C = zero;
    while ((s16)gUnk_03002490->unk6C < gUnk_03002490->unk1C)
    {
        sub_08063e14();
        sub_08006304();
        u = gUnk_03002490;
        if (--u->unk3C <= 3)
            u->unk3C = 11;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    }
    sub_08063e14();
    sub_080061c0(-32768, 0x5A5A5A5A);
    sub_08006338(9);
    TaskYieldTrampoline(2);
    sub_08006338(23);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    sub_08006338(9);
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(20);
    sub_08090e54();
    sub_08006138();
}

void sub_080903c8(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_080903f0(void)
{
    struct Task *t;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 2;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(12);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk7A = zero;
    sub_080061c0(gUnk_0874373C[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_0800622c(0xFFFA0000, 20480, 196608);
    gUnk_03002490->unk3C++;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(0x1F7);
    sub_080261d4(2);
    sub_0806cffc(16, 6);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(30);
    sub_08090e54();
    sub_08006138();
}

void sub_080904ac(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_080904d4(void)
{
    struct Task *t;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 3;
    sub_080062c4();
    sub_08063e14();
    gUnk_03002490->unk1C = gUnk_0874374E[sub_08002ee8(2)];
    gUnk_03002490->unk6C = zero;
    while ((s16)gUnk_03002490->unk6C < gUnk_03002490->unk1C)
    {
        sub_08006338(12);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk7A = 0;
        sub_0800622c(0xFFFB0000, 20480, 196608);
        gUnk_03002490->unk3C++;
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080062c4();
        sub_080031b8(0x1F7);
        sub_080261d4(2);
        sub_0806cffc(16, 6);
        sub_08063e14();
        gUnk_03002490->unk6C++;
    }
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(30);
    sub_08090e54();
    sub_08006138();
}

void sub_080905b0(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_080905d8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *y;
    s32 n;
    s32 zero;

    t = gUnk_03002490;
    t->unk15 = 4;
    u = gUnk_03002490;
    if (sub_08063cd0() < 0)
        u->unk20 = -sub_08063cd0();
    else
        u->unk20 = sub_08063cd0();
    w = gUnk_03002490;
    if (w->unk20 > 256)
    {
        w->unk1C = 256;
    }
    else
    {
        n = w->unk20 - 36;
        w->unk20 = n;
        if (n <= 0)
        {
            w->unk20 = 1;
        }
        else
        {
            if (w->unk74 == 0)
                gUnk_03002490->unk1C = Div(n, 3) * 2;
            else
                w->unk1C = n >> 1;
            if (gUnk_03002490->unk1C <= 0)
                gUnk_03002490->unk1C = 1;
        }
    }
    sub_080031b8(502);
    gUnk_03002490->unk20 = sub_080640dc(gUnk_087437C8[gUnk_03002490->unk74]);
    sub_080061c0(gUnk_08743750[gUnk_03002490->unk74], 0x5A5A5A5A);
    y = gUnk_03002490;
    zero = 0;
    y->unk24 = zero;
    y->unk6C = zero;
    while ((s16)gUnk_03002490->unk6C < gUnk_03002490->unk1C)
    {
        TaskYieldTrampoline(1);
        if (--gUnk_03002490->unk20 == 0)
        {
            if ((++gUnk_03002490->unk24 & 1) != 0)
                sub_0806cc90(1, 1, -16, 8);
            gUnk_03002490->unk20 = sub_080640fc();
        }
        gUnk_03002490->unk6C++;
    }
    if (gUnk_03002490->unk2C == 0)
        sub_0806395c(8);
    else if (sub_08063d2c() < -40)
        sub_0806395c(7);
    else
        sub_0806395c(6);
    sub_08006138();
}

void sub_08090724(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_0809074c(void)
{
    struct Task *t;
    struct Task *z;
    struct ActorSpawn spawn;

    t = gUnk_03002490;
    t->unk15 = 5;
    sub_080062c4();
    sub_08063e14();
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 47)
    {
        sub_08006338(27);
        gUnk_03002490->unk7A = 0;
        sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
        sub_0800622c(0xFFFE0000, 8192, 196608);
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080062c4();
        sub_080031b8(0x1F7);
        sub_080261d4(2);
        sub_0806cffc(16, 6);
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(27);
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    spawn.unk00 = 8;
    spawn.unk04 = 110;
    spawn.unk08 = 0;
    spawn.unk09 = 0;
    spawn.unk0C = 24;
    spawn.unk0E = 0;
    spawn.unk10 = gUnk_03002490->unk8C->unk20;
    spawn.unk0A = 1;
    gUnk_03002490->unk46 = sub_08064b5c(&spawn, 1);
    sub_080031b8(0x1FB);
    z = gUnk_03002490;
    sub_08064c1c(145, (s16)(z->unk48 - z->unk43 * 16), (s16)(z->unk4A + 8), 0);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(44);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    sub_08090e54();
    sub_08006138();
}

void sub_080908ec(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_08090914(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 6;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(20);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080031b8(520);
    sub_080261d4(2);
    sub_08090e18();
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(15);
    if (sub_08002ee8(4) == 0)
    {
        sub_0806395c(8);
    }
    else
    {
        u = gUnk_03002490;
        u->unk2C--;
        sub_08090e54();
    }
    sub_08006138();
}

void sub_080909ac(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_080909d4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 7;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(14);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFC0000, 9472, 196608);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(21);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080261d4(2);
    sub_080031b8(520);
    sub_080031b8(0x1F7);
    sub_08090e18();
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFDC000, 9472, 196608);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_08006338(13);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(15);
    sub_08090e54();
    sub_08006138();
}

void sub_08090b18(void)
{
    if (gUnk_03002490->unk14 != 7)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_08090b40(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 8;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(20);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080261d4(2);
        sub_080031b8(520);
        if ((s16)gUnk_03002490->unk6C == 0)
            sub_08090e18();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
    } while ((s16)++gUnk_03002490->unk6C <= 2);
    TaskYieldTrampoline(11);
    gUnk_03002490->unk2C = 2;
    sub_08090e54();
    sub_08006138();
}

void sub_08090bf0(void)
{
    if (gUnk_03002490->unk14 != 8)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_08090c18(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 9;
    sub_080261d4(2);
    sub_080031b8(0x1F7);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 9472, 196608);
    sub_08006338(31);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080261d4(2);
    sub_08090e9c();
    sub_08090e54();
    sub_08006138();
}

void sub_08090ca8(void)
{
    if (gUnk_03002490->unk14 != 9)
        sub_08006148(sub_080900d8, gCurTaskIdx);
}

void sub_08090cd0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 10;
    if (--gUnk_02007D00[0] == 0)
        sub_0806684c();
    sub_080667c0(1, 32);
    sub_0806caa0(1, 0, 0);
    sub_080062c4();
    sub_08006338(32);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 6656, 196608);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_08006338(33);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_0806caa0(0, 0, 10);
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_0806cc90(0, 4, 16, 8);
    sub_08006244();
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    sub_08006214();
    TaskYieldTrampoline(170);
    sub_0806caa0(1, 0, 0);
    sub_0806ad18();
    gUnk_03002490->unk34 = 2;
    sub_08006138();
}

void sub_08090de4(void)
{
    sub_080663f4(&gUnk_0826A668, 16);
    if (gUnk_03002490->unk34 == 2)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_08090e18(void)
{
    struct Task *t;
    s16 x;
    s16 y;

    t = gUnk_03002490;
    x = t->unk48 + t->unk43 * 40;
    y = t->unk4A + 8;
    sub_08067120(x, y, 0, 2);
}

void sub_08090e54(void)
{
    struct Task *t;

    sub_0806395c(gUnk_08743744[gUnk_03002490->unk28]);
    if (gUnk_03002490->unk14 == 2)
        gUnk_03002490->unk14 += sub_08002ee8(2);
    t = gUnk_03002490;
    if (--t->unk28 < 0)
        t->unk28 = 4;
}

void sub_08090e9c(void)
{
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 65536;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 9);
    sub_08006244();
}

s32 sub_08090ef0(void)
{
    gUnk_03002490->unk30 = 1;
    sub_0806caa0(1, 0, 0);
    sub_080261d4(2);
    return 0;
}

s32 sub_08090f14(void)
{
    sub_080639a4(gUnk_087440DC);
    gUnk_03002490->unk34 = 1;
    sub_0806395c(10);
    sub_08006148(sub_080900d8, gCurTaskIdx);
    return 1;
}

s32 sub_08090f4c(void)
{
    struct Task *t;
    s32 r;

    r = 0;
    switch (gUnk_03002490->unk14)
    {
    case 1:
    case 6:
    case 8:
        t = gUnk_03002490;
        sub_080061a8(-t->unk54, -t->unk5C, t->unk64);
        break;
    case 4:
        sub_0806395c(9);
        r = 1;
        break;
    case 2:
    case 3:
    case 5:
    case 7:
    case 9:
    case 10:
        sub_08006214();
        break;
    }
    return r;
}

void sub_08090fc0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08090fe0;
    sub_08006138();
}

void sub_08090fe0(void)
{
    struct Task *u;
    s32 i;
    u16 d;

    if ((s16)gUnk_03004CA0[gUnk_03002490->unk44] != -1)
    {
        u = &gUnk_03002790[gUnk_03002490->unk44];
        if (u->unk76 == 0 && u->unk34 == 0)
        {
            d = u->unk3C - 16;
            if (d <= 6)
            {
                i = (s16)u->unk3C - 16;
                gUnk_03002490->unk28 = i;
                gUnk_03002490->unk48 = u->unk48 + gUnk_087438A4[i] * (u16)u->unk43;
                gUnk_03002490->unk4A = u->unk4A + gUnk_087438B2[gUnk_03002490->unk28];
                sub_08068cf8(gUnk_087438C0[gUnk_03002490->unk28]);
            }
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

void sub_080910c0(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065350;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087530F8;
    gUnk_02007D00[0]++;
    if (sub_08067060() == 1)
        gUnk_03002490->unk18 = 24;
    else
        gUnk_03002490->unk18 = 0;
    sub_08063e14();
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08743984);
}
