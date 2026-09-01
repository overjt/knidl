/* game_code_and_rodata 0x08099B20-0x0809BA44 (issue #68, module M27 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08099B20 0x0809BA44 src/enemy_99b20.c --newpb
 *
 * The tail of the first mid-boss script, M27's second one, and the three small
 * companion tasks that close the module.
 *
 * sub_08099b20 (22 cases) and sub_08099c4c (18) are the first script's
 * remaining jump-table dispatchers; sub_08099dec picks the next animation from
 * one of gUnk_087456D4 / gUnk_087456E4 / gUnk_087456F4 / gUnk_08745704 by
 * classifying |sub_08063cd0()| against 128 and |sub_08063d2c()| against 64;
 * sub_08099e9c and sub_08099ee4 build struct ActorSpawn records for the actors
 * 16 and 17; sub_0809a080 is the shared hit reaction (rumble sub_080261d4(2)
 * or (4), then SE 0x1F7).  sub_08099fe0 and sub_08099fe4 are two dead `bx lr`
 * state handlers nothing in the ROM points at.
 *
 * The second script starts at sub_0809a0a8 (graphics gUnk_08753180, animation
 * gUnk_08745744, one-word table gUnk_0874574C): sub_0809a118 installs
 * sub_0809a17c and dispatches Task.unk14 through the 24-word guard table
 * gUnk_08745750, sub_0809a17c dispatches Task.unk15 through the 24-word body
 * table gUnk_087457B0 that follows it, and sub_0809a1f4 is its re-arm hook.
 * States 0-23 follow as <body, guard> pairs; sub_0809b438 is the timer leaf
 * the table word at 0x0874580C points at.
 *
 * sub_0809b528, sub_0809b7f0 and sub_0809ba00 are the three companion tasks
 * (graphics gUnk_0874CB7C, gUnk_087531C4, gUnk_087531DC).  They use
 * sub_0806523c as the per-frame hook and Task.unk42 = 9; sub_0809b6ac and
 * sub_0809b964 read the parent's state out of gUnk_03002790[Task.unk44], and
 * the third one's own states live in the next module - gUnk_08745B1C points at
 * sub_0809ba44.  sub_0809b8ac is a dead copy of the gUnk_08745B04 re-arm.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern u8 gUnk_03001F08[6];
extern vu16 gUnk_03004CA0[];

/* ROM tables */
extern u8 gUnk_087456D0[];
extern u8 gUnk_087456D2[];
extern u8 gUnk_087456D4[];
extern u8 gUnk_087456E4[];
extern u8 gUnk_087456F4[];
extern u8 gUnk_08745704[];
extern u8 gUnk_08745714[];
extern u8 gUnk_087456CC[];
extern u32 gUnk_08745964[];
extern u32 gUnk_08745980[];
extern u32 gUnk_087459D4[];
extern u32 gUnk_087459F0[];
extern struct AnimCmd gUnk_08745744[];
extern u32 gUnk_0874574C[];
extern u32 gUnk_08745750[];
extern u32 gUnk_087457B0[];
extern u32 gUnk_08745A1C[];
extern u32 gUnk_08745A24[];
extern u32 gUnk_08745A98[];
extern u32 gUnk_08745AE4[];
extern u32 gUnk_08745B00[];
extern u32 gUnk_08745B04[];
extern u32 gUnk_08745B1C[];
extern u32 gUnk_08745B08[];
extern u32 gUnk_08745BD0[];
extern u32 gUnk_08745BEC[];
extern u32 gUnk_08745C08[];
extern u32 gUnk_08745C24[];
extern u32 gUnk_08745C40[];
extern u32 gUnk_08745AE8[];
extern u32 gUnk_08745AF4[];
extern u32 gUnk_0874CB7C[];
extern u32 gUnk_08753180[];
extern u32 gUnk_087531C4[];
extern u32 gUnk_087531DC[];
extern void *gUnk_082797C8;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_08002f14(s16 n);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0800634c(s32 a);
extern void sub_08006364(s32 a);
extern s32 sub_080031b8(s32 id);
extern void sub_080261d4(s32 a);
extern void sub_080034f0(s32 player, s32 songId);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern u32 sub_08021a40(s32 x, s32 y);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_080639e0(u32 *p);
extern void sub_08063a00(u32 *p);
extern s32 sub_08063cbc(s32 i);
extern void sub_08063e14(void);
extern void sub_08064038(void);
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
extern void sub_08065438(void);
extern void sub_0806523c(void);
extern u32 sub_08068e04(void);
extern void sub_0806df28(s32 a, s32 b);
extern void sub_0806d65c(void);
extern void sub_08063fe0(void);
extern void sub_080656b4(void);
extern u8 sub_08067060(void);
extern void sub_08068f68(void);
extern u8 sub_080692fc(void);
extern s32 sub_08069b44(void);
extern void sub_0806a344(void);
extern void sub_0806ad18(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern void sub_0806ee2c(void);
extern void sub_08098afc(void);
extern void sub_0809baec(void);

/* Defined below */
void sub_0809a080(u8 a);
void sub_0809a17c(void);
void sub_0809a1f4(void);
void sub_0809b5b4(void);
void sub_0809b868(void);
void sub_0809b5ec(void);

u8 sub_08099b20(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 9:
        sub_080261d4(1);
        sub_080031b8(0x1F7);
        sub_0806395c(10);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        sub_0806395c(11);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 4:
        sub_08006338(4);
        sub_0809a080(1);
        sub_0806cffc(0, 16);
        sub_080062c4();
    stop:
        gUnk_03002490->unk28 = 1;
        break;
    case 2:
        sub_08006338(4);
        sub_0809a080(1);
        sub_0806cffc(0, 16);
        goto stop;
    case 22:
        sub_0809a080(1);
        sub_0806395c(0);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 7:
        sub_0809a080(1);
        sub_0806395c(8);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 15:
        sub_0809a080(1);
        sub_0806395c(16);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 19:
        sub_0809a080(0);
        sub_0806395c(20);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 23:
        sub_0809a080(0);
        sub_08066580();
        sub_0806395c(0);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08099c4c(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 17:
        sub_0806395c(18);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 4:
        gUnk_03002490->unk54 = 0;
        return 0;
    case 5:
        sub_0806395c(6);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 6:
        sub_0806395c(15);
        sub_080639e0(gUnk_08745A24);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 3:
        gUnk_03002490->unk54 = -gUnk_03002490->unk54;
        break;
    case 12:
        sub_0806395c(15);
        sub_080639e0(gUnk_08745A24);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 16:
        sub_080062c4();
        gUnk_03002490->unk28 = 1;
        break;
    case 19:
        sub_080639e0(gUnk_08745A24);
    case 7:
    case 13:
    case 15:
        sub_08006214();
        break;
    case 20:
        sub_0806395c(21);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08099d40(void)
{
    gUnk_03002490->unk2C = 32;
    sub_08064d34(142, 0);
    sub_080261d4(4);
    return 0;
}

u8 sub_08099d64(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk14 == 11)
        sub_080034f0(t->unk1C, 0x219);
    sub_080639a4(gUnk_08745A98);
    u = gUnk_03002490;
    u->unk18 = 0;
    sub_0806395c(19);
    sub_08006148(sub_0809a1f4, gCurTaskIdx);
    return 1;
}

u8 sub_08099db0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk34 = (t->unk34 + 1) & 19;
    if (t->unk34 == 3)
    {
        sub_0806395c(9);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08099dec(void)
{
    s32 v;

    gUnk_03002490->unk1C = sub_08063d2c();
    v = sub_08063cd0();
    if (v < 0)
        v = -v;
    if (v > 128)
    {
        v = sub_08063d2c();
        if (v < 0)
            v = -v;
        if (v > 64)
            sub_0806395c(gUnk_087456D4[sub_08002ee8(16)]);
        else
            sub_0806395c(gUnk_087456E4[sub_08002ee8(16)]);
    }
    else
    {
        v = sub_08063d2c();
        if (v < 0)
            v = -v;
        if (v > 64)
            sub_0806395c(gUnk_087456F4[sub_08002ee8(16)]);
        else
            sub_0806395c(gUnk_08745704[sub_08002ee8(16)]);
    }
    if (gUnk_03002490->unk14 == 2)
        gUnk_03002490->unk30 = gUnk_087456D0[sub_08002ee8(2)];
    sub_08006148(sub_0809a1f4, gCurTaskIdx);
}

s32 sub_08099e9c(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    sp.unk00 = 16;
    sp.unk04 = 118;
    sp.unk08 = 0;
    sp.unk09 = t->unk43;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = a->unk20;
    sp.unk0A = 0;
    return sub_08064b5c(&sp, 1);
}

void sub_08099ee4(u8 a)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *act;
    s32 zero;

    switch (a)
    {
    case 0:
    case 2:
        t = gUnk_03002490;
        if (sub_08021a40(t->unk48 - ((s8)t->unk43 << 4), t->unk4A) != 0)
            return;
        break;
    case 1:
    case 3:
        t = gUnk_03002490;
        if (sub_08021a40(t->unk48 + ((s8)t->unk43 << 4), t->unk4A) != 0)
            return;
        break;
    }
    act = gUnk_03002490->unk8C;
    zero = 0;
    gUnk_03002490->unk70 = sub_08002ee8(4);
    gUnk_03002490->unk6C = (s8)gUnk_087456CC[(s16)gUnk_03002490->unk70];
    sp.unk00 = 17;
    sp.unk04 = 119;
    sp.unk08 = zero;
    sp.unk09 = a;
    sp.unk0C = gUnk_03002490->unk6C;
    sp.unk0E = 0xFFF0;
    sp.unk10 = act->unk20;
    sp.unk0A = 1;
    sub_08064b5c(&sp, 1);
}

u8 sub_08099fb4(void)
{
    s32 v;

    v = sub_08063cd0();
    if (v < 0)
        v = -v;
    if (v > 111)
        return 1;
    return 0;
}

void sub_08099fd0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk54 = -t->unk54;
}

void sub_08099fe0(void)
{
}

void sub_08099fe4(void)
{
}

void sub_08099fe8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk28)
    {
    case 0:
        t->unk58 = 0x8000;
        break;
    case 1:
        t->unk58 = 0x10000;
        break;
    }
}

void sub_0809a00c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk7A = 0;
    u = gUnk_03002490;
    switch (u->unk28)
    {
    case 0:
        u->unk58 = -32768;
        break;
    case 1:
        u->unk58 = -65536;
        break;
    }
}

void sub_0809a03c(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 19:
    case 20:
    case 21:
        sub_080639b4(gUnk_087459D4);
        sub_08063a00(gUnk_087459F0);
        break;
    default:
        sub_080639b4(gUnk_08745964);
        sub_08063a00(gUnk_08745980);
        break;
    }
}

void sub_0809a080(u8 a)
{
    if (a == 1)
        sub_080261d4(2);
    else
        sub_080261d4(4);
    sub_080031b8(0x1F7);
}

void sub_0809a0a8(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    sub_08066088(0);
    sub_08063e14();
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    zero = 0;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753180;
    gUnk_02007D00[0]++;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_080666cc(gUnk_08745744);
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_0874574C);
}

void sub_0809a118(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809a17c;
    if (sub_08067060() != 0)
    {
        u = gUnk_03002490;
        u->unk20 = 0;
        u->unk7A = 0;
        sub_0806395c(23);
    }
    else
    {
        v = gUnk_03002490;
        v->unk20 = 1;
        sub_08066580();
        sub_0806395c(23);
        sub_0806395c(0);
    }
    sub_08002e98(gUnk_03002490->unk14, 24, gUnk_08745750);
}

void sub_0809a17c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk18 != 0)
    {
        if (t->unk2C > 0)
        {
            t->unk2C--;
            sub_080663f4(&gUnk_082797C8, 16);
        }
        else
        {
            sub_08066468();
        }
    }
    u = gUnk_03002490;
    if (u->unk20 != 0)
    {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 24, gUnk_087457B0);
    }
    else
    {
        sub_08002e98(u->unk15, 24, gUnk_087457B0);
    }
    sub_0809a03c();
    sub_08068f68();
    sub_08069b44();
}

void sub_0809a1f4(void)
{
    sub_08098afc();
    sub_08002e98(gUnk_03002490->unk14, 24, gUnk_08745750);
}

void sub_0809a214(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    u = gUnk_03002490;
    u->unk28 = zero;
    u->unk30 = 120;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk6C = zero;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(15);
        sub_08006338(16);
        TaskYieldTrampoline(9);
    }
    while ((s16)++gUnk_03002490->unk6C <= 4);
    sub_08006338(4);
    sub_08006138();
}

void sub_0809a270(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a2a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk28 = zero;
    sub_080062c4();
    sub_08006138();
}

void sub_0809a2c0(void)
{
    struct Task *t;

    if (gUnk_03002490->unk28 != 0)
    {
        sub_08099dec();
    }
    else
    {
        sub_08099db0();
        t = gUnk_03002490;
        t->unk28 = 1;
    }
}

void sub_0809a2e8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 2;
    u = gUnk_03002490;
    u->unk28 = zero;
    u->unk7A = zero;
    sub_08006338(6);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x70000;
    sub_08006138();
}

void sub_0809a32c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk28 != 0)
    {
        if (--t->unk30 <= 0)
            sub_0806395c(1);
        else
            sub_0806395c(2);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a36c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 3;
    u = gUnk_03002490;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 64;
        sub_080061c0(-65536, 0x5A5A5A5A);
        while (1)
        {
            sub_08006338(11);
            TaskYieldTrampoline(7);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(7);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(7);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(7);
        }
    case 1:
        v = gUnk_03002490;
        v->unk30 = 64;
        sub_080061c0(-98304, 0x5A5A5A5A);
        while (1)
        {
            sub_08006338(11);
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(5);
        }
    }
}

void sub_0809a434(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 <= 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a464(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 4;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk30 = 16;
    u->unk7A = zero;
    v = gUnk_03002490;
    v->unk28 = zero;
    sub_08006338(4);
    TaskYieldTrampoline(8);
    sub_08006338(5);
    TaskYieldTrampoline(8);
    sub_08006338(6);
    w = gUnk_03002490;
    w->unk58 = -327680;
    w->unk60 = 0x3700;
    w->unk68 = 0x30000;
    switch (w->unk74)
    {
    case 0:
        sub_080061c0(0x10000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0x14000, 0x5A5A5A5A);
        break;
    }
    sub_08006138();
}

void sub_0809a4f0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk28 != 0)
    {
        if (--t->unk30 < 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a528(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 5;
    sub_08063e14();
    sub_080062c4();
    sub_080061c0(-32768, 0x5A5A5A5A);
    u = gUnk_03002490;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 20;
        sub_08006338(11);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        break;
    case 1:
        u->unk30 = 22;
        sub_08006338(11);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        break;
    }
    sub_08006138();
}

void sub_0809a624(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_0806395c(6);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a654(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 6;
    u = gUnk_03002490;
    u->unk28 = zero;
    gUnk_03002490->unk46 = sub_0806cc90(1, 10, -12, 16);
    sub_080062c4();
    switch (gUnk_03002490->unk74)
    {
    case 0:
        sub_080061c0(0x18000, 0x5A5A5A5A);
        sub_08006338(11);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk28 = 1;
        TaskYieldTrampoline(4);
        break;
    case 1:
        sub_080061c0(0x1C000, 0x5A5A5A5A);
        sub_08006338(11);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk28 = 1;
        TaskYieldTrampoline(2);
        break;
    }
    sub_08006338(11);
    sub_08006138();
}

void sub_0809a744(void)
{
    s32 v;

    if (gUnk_03002490->unk28 != 0)
    {
        v = sub_08063cd0();
        if (v < 0)
            v = -v;
        if (v > 32)
        {
            sub_0806395c(12);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
        else
        {
            sub_0806395c(7);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a798(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 7;
    u = gUnk_03002490;
    u->unk7A = zero;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x30000;
    sub_08006338(6);
    sub_08006138();
}

void sub_0809a7d8(void)
{
}

void sub_0809a7dc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 8;
    u = gUnk_03002490;
    u->unk30 = 120;
    gUnk_03002490->unk6C = sub_08099e9c();
    while (1)
    {
        sub_08006364(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_0800634c(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
    }
}

void sub_0809a82c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        if (sub_08099db0() == 0)
        {
            sub_0806395c(14);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a868(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 9;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk7A = zero;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk58 = -163840;
    v->unk60 = 0x3000;
    v->unk68 = 0x30000;
    sub_08006338(6);
    TaskYieldTrampoline(8);
    sub_08006138();
}

void sub_0809a8b4(void)
{
}

void sub_0809a8b8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 10;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk7A = zero;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk58 = -163840;
    v->unk60 = 0x3000;
    v->unk68 = 0x30000;
    sub_080061c0(-81920, 0x5A5A5A5A);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(6);
    sub_08006138();
}

void sub_0809a918(void)
{
}

void sub_0809a91c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 11;
    u = gUnk_03002490;
    u->unk30 = 116;
    sub_080062c4();
    gUnk_03002490->unk1C = sub_080031b8(0x219);
    while (1)
    {
        sub_08006364(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_0800634c(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
    }
}

void sub_0809a974(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (--t->unk30)
    {
    case 100:
        sub_080031b8(0x1FB);
        sub_08099ee4(0);
        break;
    case 76:
        sub_080031b8(0x1FB);
        sub_08099ee4(1);
        break;
    case 52:
        sub_080031b8(0x1FB);
        sub_08099ee4(2);
        break;
    case 28:
        sub_080031b8(0x1FB);
        sub_08099ee4(3);
        break;
    case 0:
        sub_080034f0(t->unk1C, 0x219);
        if (sub_08099db0() == 0)
        {
            sub_0806395c(14);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
        break;
    }
}

void sub_0809aa24(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 12;
    gUnk_03002490->unk30 = (s8)gUnk_087456D2[sub_08002ee8(2)];
    sub_080031b8(502);
    u = gUnk_03002490;
    switch (u->unk74)
    {
    case 0:
        TaskYieldTrampoline(4);
        while (1)
        {
            sub_08006338(11);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(6);
        }
    case 1:
        TaskYieldTrampoline(2);
        while (1)
        {
            sub_08006338(11);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
        }
    }
}

void sub_0809aaf0(void)
{
    struct Task *t;
    s32 v;

    t = gUnk_03002490;
    if (--t->unk30 == 0)
    {
        sub_0806395c(9);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
    if (gUnk_03002490->unk30 < 0)
    {
        v = sub_08063cd0();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            sub_0806395c(7);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
    else
    {
        v = sub_08063cd0();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            sub_0806395c(13);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809ab70(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 13;
    u = gUnk_03002490;
    u->unk30 = 120;
    u->unk70 = u->unk54;
    u->unk28 = zero;
    switch (u->unk74)
    {
    case 0:
        sub_080061c0(0x5A5A5A5A, -2048);
        break;
    case 1:
        sub_080061c0(0x5A5A5A5A, -1536);
        break;
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006364(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_0800634c(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
    }
    while ((s16)++gUnk_03002490->unk6C <= 5);
    gUnk_03002490->unk28 = 1;
    sub_08099e9c();
    while (1)
    {
        sub_08006364(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_0800634c(8);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
    }
}

void sub_0809ac54(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    t = gUnk_03002490;
    a = (s16)t->unk70;
    if (a < 0)
        a = -a;
    b = t->unk54;
    if (b < 0)
        b = -b;
    if (a > b)
        sub_080062c4();
    u = gUnk_03002490;
    if (u->unk28 != 0)
    {
        if (--u->unk30 <= 0)
        {
            sub_08099db0();
            sub_0806395c(14);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
    v = gUnk_03002490;
    v->unk70 = v->unk54;
}

void sub_0809acbc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 14;
    sub_08063e14();
    u = gUnk_03002490;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 64;
        sub_08006338(4);
        TaskYieldTrampoline(14);
        sub_08006338(16);
        TaskYieldTrampoline(8);
        sub_08006338(4);
        TaskYieldTrampoline(14);
        sub_08006338(16);
        TaskYieldTrampoline(8);
        sub_08006338(4);
        TaskYieldTrampoline(14);
        sub_08006338(16);
        TaskYieldTrampoline(8);
        break;
    case 1:
        u->unk30 = 36;
        sub_08006338(4);
        TaskYieldTrampoline(13);
        sub_08006338(16);
        TaskYieldTrampoline(8);
        sub_08006338(4);
        TaskYieldTrampoline(13);
        sub_08006338(16);
        TaskYieldTrampoline(8);
        break;
    }
    sub_08006138();
}

void sub_0809ad6c(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        if (sub_08099fb4() != 0)
        {
            u = gUnk_03002490;
            if (sub_08021a40(u->unk48 - ((s8)u->unk43 << 4), u->unk4A) != 0)
            {
                /* The empty asm keeps agbcc from if-converting the two
                   constant arms into a preload + conditional overwrite. */
                BLOCK_CROSS_JUMP
                v = 18;
            }
            else
            {
                v = 17;
            }
            sub_0806395c(v);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
        else
        {
            sub_0806395c(1);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809adf4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 15;
    u = gUnk_03002490;
    u->unk7A = zero;
    v = gUnk_03002490;
    v->unk54 = -65536;
    v->unk58 = -196608;
    v->unk60 = 0x2500;
    v->unk68 = 0x30000;
    sub_080261d4(2);
    sub_08006338(7);
    sub_08006138();
}

void sub_0809ae3c(void)
{
}

void sub_0809ae40(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 16;
    u = gUnk_03002490;
    u->unk30 = zero;
    u->unk28 = zero;
    sub_0806caa0(0, 0, 24);
    sub_080062c4();
    while (1)
    {
        sub_08006338(13);
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
    }
}

void sub_0809aefc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (++t->unk30)
    {
    case 32:
        t->unk28 = 1;
        break;
    case 62:
        t->unk58 = 0x10000;
        sub_080639e0(gUnk_08745A1C);
        sub_0806395c(14);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        break;
    }
}

void sub_0809af4c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 17;
    u = gUnk_03002490;
    u->unk30 = 200;
    sub_08063e14();
    sub_080061c0(-49152, 0x5A5A5A5A);
    sub_08006338(10);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    while (1)
    {
        sub_08099fd0();
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(11);
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(8);
        }
        while ((s16)++gUnk_03002490->unk6C <= 1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        sub_08099fd0();
        sub_08006338(11);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(11);
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(8);
        }
        while ((s16)++gUnk_03002490->unk6C <= 1);
    }
}

void sub_0809b09c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_0806395c(gUnk_08745714[sub_08002ee8(16)]);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
    if ((gUnk_03002490->unk30 & 1) != 0)
    {
        if (sub_08099fb4() == 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809b104(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 17;
    sub_08006338(11);
    TaskYieldTrampoline(8);
    sub_08006338(12);
    TaskYieldTrampoline(8);
    sub_08006338(11);
    TaskYieldTrampoline(8);
    sub_08006338(10);
    TaskYieldTrampoline(8);
    while (1)
    {
        sub_08099fd0();
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(11);
            TaskYieldTrampoline(8);
            sub_08006338(12);
            TaskYieldTrampoline(8);
            sub_08006338(11);
            TaskYieldTrampoline(8);
            sub_08006338(10);
            TaskYieldTrampoline(8);
        }
        while ((s16)++gUnk_03002490->unk6C <= 1);
        sub_08006338(11);
        TaskYieldTrampoline(8);
        sub_08006338(12);
        TaskYieldTrampoline(8);
        sub_08099fd0();
        sub_08006338(11);
        TaskYieldTrampoline(8);
        sub_08006338(12);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(11);
            TaskYieldTrampoline(8);
            sub_08006338(10);
            TaskYieldTrampoline(8);
            sub_08006338(11);
            TaskYieldTrampoline(8);
            sub_08006338(12);
            TaskYieldTrampoline(8);
        }
        while ((s16)++gUnk_03002490->unk6C <= 1);
    }
}

void sub_0809b210(void)
{
}

void sub_0809b214(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 19;
    sub_080639a4(gUnk_08745A98);
    sub_080639e0(gUnk_08745A24);
    u = gUnk_03002490;
    u->unk7A = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
    sub_080667c0(1, 7);
    sub_080061c0(-65536, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_0806caa0(0, -10, 24);
    sub_08006338(7);
    sub_08006138();
}

void sub_0809b298(void)
{
    sub_080663f4(&gUnk_082797C8, 16);
}

void sub_0809b2ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 20;
    u = gUnk_03002490;
    u->unk30 = 32;
    sub_0806caa0(1, 0, 0);
    gUnk_03002490->unk46 = sub_0806cc90(0, 4, 8, 24);
    sub_080062c4();
    sub_080061c0(-32768, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_08006338(14);
    sub_08006138();
}

void sub_0809b310(void)
{
    struct Task *t;

    sub_080663f4(&gUnk_082797C8, 16);
    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_0806395c(21);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809b34c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 21;
    u = gUnk_03002490;
    u->unk30 = zero;
    sub_0806caa0(1, 0, 0);
    sub_080062c4();
    sub_08006338(14);
    TaskYieldTrampoline(170);
    v = gUnk_03002490;
    v->unk20 = zero;
    sub_0806ad18();
    w = gUnk_03002490;
    w->unk30 = 1;
    sub_08006138();
}

void sub_0809b394(void)
{
    struct Task *t;

    sub_080663f4(&gUnk_082797C8, 16);
    if (gUnk_03002490->unk30 != 0)
    {
        sub_080062c4();
        t = gUnk_03002490;
        t->unk20 = 1;
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
}

void sub_0809b3d4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 22;
    u = gUnk_03002490;
    u->unk60 = 0x5000;
    u->unk68 = 0x30000;
    sub_08006338(11);
    TaskYieldTrampoline(8);
    sub_08006138();
}

void sub_0809b404(void)
{
}

void sub_0809b408(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 23;
    u = gUnk_03002490;
    u->unk30 = 24;
    u->unk60 = 0x5000;
    u->unk68 = 0x30000;
    sub_08006338(6);
    sub_08006138();
}

void sub_0809b438(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk30 <= 0)
        t->unk20 = 1;
    u = gUnk_03002490;
    u->unk30--;
}

u8 sub_0809b454(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    if (t->unk14 == 0)
    {
        sub_0806395c(2);
        sub_08006148(sub_0809b5ec, gCurTaskIdx);
        return 1;
    }
    t->unk7A = 0;
    u = gUnk_03002490;
    switch (u->unk28)
    {
    case 0:
        sub_080061c0(0x10000, 0x5A5A5A5A);
        v = gUnk_03002490;
        v->unk58 = -196608;
        v->unk60 = 0x1E00;
        break;
    case 1:
        sub_080061c0(0x2A000, 0x5A5A5A5A);
        v = gUnk_03002490;
        v->unk58 = -98304;
        v->unk60 = 0x1E00;
        break;
    }
    return 0;
}

void sub_0809b4d8(void)
{
}

u8 sub_0809b4dc(void)
{
    sub_0806395c(2);
    sub_08006148(sub_0809b5ec, gCurTaskIdx);
    return 1;
}

void sub_0809b4fc(void)
{
    s32 v;

    v = sub_08063d2c();
    if (v < 0)
        v = -v;
    if (v > 29)
        gUnk_03002490->unk28 = 0;
    else
        gUnk_03002490->unk28 = 1;
}

void sub_0809b528(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    zero = 0;
    u = gUnk_03002490;
    u->unk70 = u->unk40;
    u->unk38 = gUnk_0874CB7C;
    u->unk40 = zero;
    sub_08002e98(u->unk73, 1, gUnk_08745AE4);
}

void sub_0809b57c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809b5b4;
    t->unk43 = t->unk74;
    sub_0806395c(0);
    u = gUnk_03002490;
    sub_08002e98(u->unk14, 3, gUnk_08745AE8);
}

void sub_0809b5b4(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08745AF4);
    if (gUnk_03002490->unk14 != 2)
        sub_08068e04();
    sub_08069b44();
}

void sub_0809b5ec(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08745AE8);
}

void sub_0809b608(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    sub_0800634c(4);
    u = gUnk_03002490;
    u->unk7A = zero;
    v = gUnk_03002490;
    v->unk28 = zero;
    v->unk68 = 0x30000;
    sub_080031b8(506);
    sub_0806df28(3, 0);
    gUnk_03002490->unk58 = -262144;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -131072;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -65536;
    TaskYieldTrampoline(8);
    sub_080639b4(gUnk_08745BD0);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_0809b6ac(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk28 != 0)
    {
        t->unk2C = (gUnk_03002790 + t->unk44)->unk14;
        if (t->unk2C == 12)
        {
            sub_0806395c(1);
            sub_08006148(sub_0809b5ec, gCurTaskIdx);
        }
    }
}

void sub_0809b6f8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;

    t = gUnk_03002490;
    a = t->unk8C;
    zero = 0;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk68 = 0x30000;
    u->unk7A = zero;
    sub_080031b8(0x1FB);
    if (a->unk10 != -1)
    {
        sub_08005654(a->unk10);
        a->unk10 = 0xFFFF;
    }
    sub_0809b4fc();
    switch (gUnk_03002490->unk28)
    {
    case 0:
        sub_080061c0(0x10000, 0x5A5A5A5A);
        v = gUnk_03002490;
        v->unk58 = -196608;
        v->unk60 = 0x1E00;
        break;
    case 1:
        sub_080061c0(0x2A000, 0x5A5A5A5A);
        v = gUnk_03002490;
        v->unk58 = -98304;
        v->unk60 = 0x1E00;
        break;
    }
    sub_08006138();
}

void sub_0809b790(void)
{
}

void sub_0809b794(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gUnk_03002490;
    a = t->unk8C;
    zero = 0;
    t->unk15 = 2;
    u = gUnk_03002490;
    u->unk28 = zero;
    u->unk7A = zero;
    sub_080062c4();
    if (a->unk10 != -1)
    {
        sub_08005654(a->unk10);
        a->unk10 = 0xFFFF;
    }
    sub_0800634c(4);
    TaskYieldTrampoline(2);
    sub_0806d65c();
    sub_08063fe0();
}

void sub_0809b7ec(void)
{
}

void sub_0809b7f0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_087531C4;
    sub_08002e98(u->unk73, 1, gUnk_08745B00);
}

void sub_0809b830(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0809b868;
    t->unk43 = t->unk74;
    sub_0806395c(0);
    u = gUnk_03002490;
    sub_08002e98(u->unk14, 1, gUnk_08745B04);
}

void sub_0809b868(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08745B08);
    if ((s16)gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0809b8ac(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08745B04);
}

void sub_0809b8c8(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    sub_08006338(4);
    u = gUnk_03002490;
    u->unk7A = zero;
    gUnk_03002490->unk2C = sub_080031b8(0x219);
    while (1)
    {
        sub_080639b4(gUnk_08745BEC);
        sub_08006338(0);
        TaskYieldTrampoline(2);
        sub_08006338(1);
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08745C08);
        sub_08006338(2);
        TaskYieldTrampoline(2);
        sub_08006338(3);
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08745C24);
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08745C40);
        sub_08006338(5);
        TaskYieldTrampoline(2);
    }
}

void sub_0809b964(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk48 = (gUnk_03002790 + t->unk44)->unk48;
    t->unk28 = (gUnk_03002790 + t->unk44)->unk14;
    if (t->unk28 != 8 && t->unk28 != 13)
    {
        sub_080034f0(t->unk2C, 0x219);
        sub_08063fe0();
    }
}

u8 sub_0809b9c0(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0809baec, gCurTaskIdx);
    return 1;
}

u8 sub_0809b9e0(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0809baec, gCurTaskIdx);
    return 1;
}

void sub_0809ba00(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_087531DC;
    u->unk43 = 1;
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08745B1C);
}
