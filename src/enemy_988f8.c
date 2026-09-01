/* game_code_and_rodata 0x080988F8-0x08099B20 (issue #68, module M27 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080988F8 0x08099B20 src/enemy_988f8.c --newpb
 *
 * M27's first mid-boss script, built exactly like M25's bosses (rom-map section 9).
 * sub_08098e64 is the task entry: it installs sub_080656b4 as the draw hook
 * (Task.unk00) and sub_08065438 as the per-frame hook (Task.unk0C), points
 * Task.unk38 at the graphics block gUnk_08753090, counts the enemy into
 * gUnk_02007D00[0], loads the animation script gUnk_08745624 and hands
 * Task.unk73 to sub_08002e98 with the one-word table gUnk_08745630, whose only
 * entry is sub_08098ed4.
 *
 * sub_08098ed4 installs sub_08098f38 as the per-frame body and dispatches
 * Task.unk14 through the 19-word guard table gUnk_08745634; sub_08098f38
 * re-uploads (sub_080663f4) or drops (sub_08066468) the 16-byte graphics
 * record gUnk_08274840 while Task.unk18 is set, dispatches Task.unk15 through
 * the 19-word body table gUnk_08745680 that follows it, and finishes with the
 * animation-row selector sub_08098de4 plus sub_08068f68 / sub_08069b44.
 * sub_08098fb0 is the re-arm hook every guard installs through
 * sub_08006148(fn, gCurTaskIdx).
 *
 * The rest are the states.  sub_080988f8 / sub_08098a04 are the two jump-table
 * dispatchers that turn Task.unk14 into the next animation, sub_08098afc frees
 * the helper task recorded in Task.unk46 once gUnk_03004CA0[] says its type is
 * 143 and gUnk_03002790[] says this task is its parent, sub_08098b60 walks the
 * gUnk_08745618 / gUnk_0874561F rows with the decimal-digit buffer
 * gUnk_03001F08[1] as the index, sub_08098c54 fires the timed
 * sub_08002ee8-gated transitions at Task.unk30 == 120 / 60 / 45, sub_08098d58
 * spawns the actor 13 through sub_08064b5c and sub_08098da4 is the "close
 * enough" probe (|sub_08063cbc(Task.unk1C)| <= 10).  sub_080992a8 and
 * sub_08099a0c are empty state handlers, and sub_08099ad0 is the timer leaf
 * the guard table word at 0x087456C8 points at.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern u8 gUnk_03001F08[6];
extern vu16 gUnk_03004CA0[];

/* ROM tables */
extern u8 gUnk_08745618[];
extern u8 gUnk_0874561F[];
extern struct AnimCmd gUnk_08745624[];
extern u32 gUnk_08745630[];
extern u32 gUnk_08745634[];
extern u32 gUnk_08745680[];
extern u32 gUnk_08745868[];
extern u32 gUnk_08745884[];
extern u32 gUnk_087458A0[];
extern u32 gUnk_087458BC[];
extern u32 gUnk_087458F4[];
extern u32 gUnk_08745910[];
extern u32 gUnk_0874592C[];
extern u32 gUnk_08745948[];
extern u32 gUnk_08745A0C[];
extern u32 gUnk_08745A14[];
extern u32 gUnk_08745A80[];
extern u32 gUnk_08753090[];
extern void *gUnk_08274840;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_08002f14(s16 n);
extern void sub_08005654(s32 id);
extern void sub_08006148(void *fn, s32 i);
extern void sub_08006244(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_080639e0(u32 *p);
extern void sub_08063a00(u32 *p);
extern s32 sub_08063cbc(s32 i);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_08066580(void);
extern void sub_08066ae0(void);
extern void sub_08065438(void);
extern void sub_080656b4(void);
extern void sub_080261d4(s32 a);
extern void sub_0806ee2c(void);
extern void sub_080062c4(void);
extern void sub_08063e14(void);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern u8 sub_08067060(void);
extern void sub_08068f68(void);
extern u8 sub_080692fc(void);
extern s32 sub_08069b44(void);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006138(void);
extern void sub_080031b8(s32 id);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_08064038(void);
extern void sub_0806684c(void);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806a344(void);
extern void sub_0806ad18(void);

/* Defined below */
void sub_08098f38(void);
void sub_0809a1f4(void);
void sub_08098fb0(void);
void sub_0809a080(s32 a);

u8 sub_080988f8(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 0:
    case 1:
        sub_0809a080(1);
        gUnk_03002490->unk28 = 1;
        break;
    case 4:
        sub_0809a080(1);
        gUnk_03002490->unk7A = 0;
        sub_080639e0(gUnk_08745A14);
        sub_0806395c(5);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 5:
        gUnk_03002490->unk58 = -65536;
        break;
    case 13:
        sub_0809a080(0);
        sub_080639e0(gUnk_08745A14);
        sub_0806395c(14);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 16:
        sub_0809a080(1);
        sub_0806395c(0);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        sub_08006338(19);
        break;
    case 18:
        sub_0809a080(0);
        sub_08066580();
        sub_0806395c(0);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08098a04(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 2:
        sub_0806395c(7);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 3:
        sub_0806395c(4);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 5:
        gUnk_03002490->unk7A = 1;
        sub_08006244();
        sub_0806395c(6);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 4:
    case 13:
        gUnk_03002490->unk54 = 0;
        return 0;
    case 14:
        sub_0806395c(15);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08098aa0(void)
{
    sub_080639a4(gUnk_08745A80);
    gUnk_03002490->unk18 = 0;
    sub_0806395c(13);
    sub_08006148(sub_08098fb0, gCurTaskIdx);
    return 1;
}

u8 sub_08098ad8(void)
{
    gUnk_03002490->unk2C = 32;
    sub_08064d34(142, 0);
    sub_080261d4(4);
    return 0;
}

void sub_08098afc(void)
{
    if ((s16)gUnk_03004CA0[gUnk_03002490->unk46] != -1
        && gUnk_03004CA0[gUnk_03002490->unk46] == 143
        && gUnk_03002790[gUnk_03002490->unk46].unk44 == gCurTaskIdx)
    {
        sub_08005654(gUnk_03002490->unk46);
        gUnk_03002490->unk46 = 0;
    }
}

void sub_08098b60(void)
{
    struct Task *t;
    struct Task *u;

    sub_08002f14((s16)sub_08002ee8(70));
    switch (gUnk_08745618[(s8)gUnk_03001F08[1]])
    {
    case 0:
        gUnk_03002490->unk34 = 2;
        sub_0806395c(2);
        break;
    case 1:
        sub_08002f14((s16)sub_08002ee8(20));
        t = gUnk_03002490;
        t->unk30 = gUnk_0874561F[(s8)gUnk_03001F08[1]];
        t->unk34 = 2;
        sub_0806395c(1);
        break;
    case 2:
        u = gUnk_03002490;
        if (--u->unk34 != 0)
        {
            sub_0806395c(8);
            break;
        }
        switch (sub_08002ee8(2))
        {
        case 0:
            sub_0806395c(2);
            break;
        case 1:
            sub_08002f14((s16)sub_08002ee8(20));
            gUnk_03002490->unk30 = gUnk_0874561F[(s8)gUnk_03001F08[1]];
            sub_0806395c(1);
            break;
        default:
            sub_0806ee2c();
            break;
        }
        break;
    default:
        sub_0806ee2c();
        break;
    }
    sub_08006148(sub_08098fb0, gCurTaskIdx);
}

void sub_08098c54(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (--t->unk30)
    {
    case 45:
        if (t->unk74 == 1 && sub_08002ee8(4) == 0)
        {
            gUnk_03002490->unk54 = 0;
            sub_0806395c(0);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
        break;
    case 60:
        if (t->unk74 == 0 && sub_08002ee8(2) == 0)
        {
            gUnk_03002490->unk54 = 0;
            sub_0806395c(0);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
        break;
    case 120:
        if (t->unk74 == 1 && sub_08002ee8(4) == 0)
        {
            gUnk_03002490->unk54 = 0;
            sub_0806395c(0);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
        break;
    }
}

void sub_08098cf4(void)
{
    sub_08002f14((s16)sub_08002ee8(30));
    switch ((s8)gUnk_03001F08[1])
    {
    case 0:
        sub_0806395c(9);
        break;
    case 1:
        sub_0806395c(10);
        break;
    case 2:
        sub_0806395c(11);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    sub_08006148(sub_08098fb0, gCurTaskIdx);
}

void sub_08098d58(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    sp.unk00 = 13;
    sp.unk04 = 115;
    sp.unk08 = 0;
    sp.unk09 = t->unk43;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = a->unk20;
    sp.unk0A = 1;
    gUnk_03002490->unk1C = sub_08064b5c(&sp, 1);
}

u8 sub_08098da4(void)
{
    s32 v;

    v = sub_08063cbc(gUnk_03002490->unk1C);
    if (v < 0)
        v = -v;
    if (v <= 10)
    {
        sub_0806395c(17);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08098de4(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 13:
    case 14:
    case 15:
        sub_080639b4(gUnk_087458F4);
        sub_08063a00(gUnk_08745910);
        break;
    case 8:
        sub_080639b4(gUnk_087458A0);
        sub_08063a00(gUnk_087458BC);
        break;
    case 1:
        sub_080639b4(gUnk_0874592C);
        sub_08063a00(gUnk_08745948);
    default:
        sub_080639b4(gUnk_08745868);
        sub_08063a00(gUnk_08745884);
        break;
    }
}

void sub_08098e64(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    sub_08066088(0);
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    zero = 0;
    gUnk_03002490->unk38 = gUnk_08753090;
    gUnk_02007D00[0]++;
    sub_080666cc(gUnk_08745624);
    u = gUnk_03002490;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08745630);
}

void sub_08098ed4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08098f38;
    t->unk30 = 90;
    t->unk34 = 2;
    if (sub_08067060() != 0)
    {
        u = gUnk_03002490;
        u->unk20 = 0;
        u->unk7A = 0;
        sub_0806395c(18);
    }
    else
    {
        v = gUnk_03002490;
        v->unk20 = 1;
        sub_08066580();
        sub_0806395c(0);
    }
    sub_08002e98(gUnk_03002490->unk14, 19, gUnk_08745634);
}

void sub_08098f38(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk18 != 0)
    {
        if (t->unk2C > 0)
        {
            t->unk2C--;
            sub_080663f4(&gUnk_08274840, 16);
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
            sub_08002e98(gUnk_03002490->unk15, 19, gUnk_08745680);
    }
    else
    {
        sub_08002e98(u->unk15, 19, gUnk_08745680);
    }
    sub_08098de4();
    sub_08068f68();
    sub_08069b44();
}

void sub_08098fb0(void)
{
    sub_08098afc();
    sub_08002e98(gUnk_03002490->unk14, 19, gUnk_08745634);
}

void sub_08098fd0(void)
{
    struct Task *t;

    sub_080062c4();
    t = gUnk_03002490;
    t->unk28 = 1;
    t->unk15 = 0;
    sub_08063e14();
    while (1)
    {
        sub_08006338(25);
        TaskYieldTrampoline(4);
        sub_08006338(4);
        TaskYieldTrampoline(16);
    }
}

void sub_08099004(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
        sub_08098b60();
}

void sub_08099020(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk7A = zero;
    v = gUnk_03002490;
    v->unk28 = zero;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x70000;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
    }
}

void sub_08099080(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk28 != 0)
    {
        if (--t->unk30 == 0)
        {
            t->unk30 = 30;
            sub_0806395c(0);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
        else
        {
            sub_0806395c(1);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080990d4(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    t->unk30 = 160;
    zero = 0;
    t->unk15 = 2;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk28 = zero;
    switch (u->unk74)
    {
    case 0:
        sub_080061c0(-24576, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(-32768, 0x5A5A5A5A);
        break;
    }
    while (1)
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(1);
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(1);
        }
        while ((s16)++gUnk_03002490->unk6C <= 3);
        gUnk_03002490->unk28 = 1;
    }
}

void sub_08099180(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_080991ac(void)
{
    struct Task *t;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 3;
    gUnk_03002490->unk46 = sub_0806cc90(1, 10, -8, 24);
    sub_080031b8(502);
    v = gUnk_03002490;
    switch (v->unk74)
    {
    case 0:
        sub_080061c0(0x18000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0x20000, 0x5A5A5A5A);
        break;
    }
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
    }
}

void sub_08099238(void)
{
    sub_08098c54();
}

void sub_08099244(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 4;
    u = gUnk_03002490;
    u->unk7A = zero;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk60 = 0x2500;
    v->unk68 = 0x30000;
    sub_080061c0(-49152, 0x5A5A5A5A);
    w = gUnk_03002490;
    w->unk58 = -196608;
    sub_080261d4(2);
    sub_080031b8(0x1F7);
    sub_08006338(24);
    sub_08006138();
}

void sub_080992a8(void)
{
}

void sub_080992ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk30 = 32;
    t->unk15 = 5;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk60 = 0x8000;
    u->unk68 = 0x30000;
    sub_080061c0(-32768, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -65536;
    sub_080261d4(2);
    gUnk_03002490->unk46 = sub_0806cc90(0, 4, 8, 24);
    sub_0806caa0(0, 0, 24);
    sub_08006338(24);
    sub_08006138();
}

void sub_0809931c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_08006244();
        sub_0806395c(6);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099350(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk30 = 20;
    t->unk15 = 6;
    sub_080062c4();
    sub_08006338(24);
    while (1)
    {
        u = gUnk_03002490;
        u->unk7A = 0;
        v = gUnk_03002490;
        v->unk58 = -65536;
        TaskYieldTrampoline(2);
        w = gUnk_03002490;
        w->unk58 = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_08099394(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        t->unk7A = 1;
        u = gUnk_03002490;
        u->unk30 = 30;
        sub_080639e0(gUnk_08745A0C);
        sub_0806395c(0);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_080993dc(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 7;
    u = gUnk_03002490;
    u->unk28 = zero;
    while (1)
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(1);
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(1);
        }
        while ((s16)++gUnk_03002490->unk6C <= 2);
        gUnk_03002490->unk28 = 1;
    }
}

void sub_08099448(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099474(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 8;
    u = gUnk_03002490;
    u->unk30 = 44;
    sub_08063e14();
    while (1)
    {
        sub_08006338(23);
        TaskYieldTrampoline(1);
        sub_08064038();
        sub_08006338(21);
        TaskYieldTrampoline(1);
        sub_08064038();
        sub_08006338(22);
        TaskYieldTrampoline(4);
        sub_08064038();
        sub_08006338(21);
        TaskYieldTrampoline(1);
        sub_08064038();
        sub_08006338(23);
        TaskYieldTrampoline(1);
        sub_08006338(21);
        TaskYieldTrampoline(1);
        sub_08064038();
        sub_08006338(22);
        TaskYieldTrampoline(4);
        sub_08064038();
        sub_08006338(21);
        TaskYieldTrampoline(1);
    }
}

void sub_08099508(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
        sub_08098cf4();
}

void sub_08099524(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 9;
    u = gUnk_03002490;
    u->unk30 = 48;
    u->unk7A = 0;
    sub_080061c0(-32768, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -131072;
    sub_08098d58();
    while (1)
    {
        sub_08006338(10);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        sub_08006338(11);
        gUnk_03002490->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        sub_08006338(12);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0;
        TaskYieldTrampoline(8);
    }
}

void sub_080995b8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        if (sub_08098da4() == 0)
        {
            sub_0806395c(12);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080995f4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 10;
    u = gUnk_03002490;
    u->unk30 = 48;
    u->unk7A = 0;
    sub_080031b8(506);
    sub_08098d58();
    sub_08064038();
    sub_080061c0(0x8000, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -131072;
    while (1)
    {
        sub_08006338(16);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        sub_08006338(17);
        gUnk_03002490->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        sub_08006338(18);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk60 = 0;
        TaskYieldTrampoline(8);
    }
}

void sub_08099690(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_08064038();
        if (sub_08098da4() == 0)
        {
            sub_0806395c(12);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080996d0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 11;
    u = gUnk_03002490;
    u->unk30 = 48;
    sub_080061c0(-32768, 0x5A5A5A5A);
    sub_080031b8(506);
    sub_08098d58();
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
    }
}

void sub_08099734(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        if (sub_08098da4() == 0)
        {
            sub_0806395c(12);
            sub_08006148(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_08099770(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 12;
    u = gUnk_03002490;
    u->unk30 = 18;
    sub_080062c4();
    while (1)
    {
        sub_080061c0(0x60000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006338(13);
        TaskYieldTrampoline(3);
        sub_08006338(14);
        TaskYieldTrampoline(1);
        sub_080062c4();
        sub_08006338(14);
        TaskYieldTrampoline(2);
        sub_08006338(15);
        TaskYieldTrampoline(25);
        sub_080062c4();
    }
}

void sub_080997e4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        t->unk30 = 30;
        sub_0806395c(0);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099818(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 13;
    sub_080639a4(gUnk_08745A80);
    u = gUnk_03002490;
    u->unk7A = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
    sub_080667c0(1, 24);
    sub_080061c0(-65536, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_0806caa0(0, -10, 24);
    sub_08006338(24);
    sub_08006138();
}

void sub_08099890(void)
{
    sub_080663f4(&gUnk_08274840, 16);
}

void sub_080998a4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 14;
    u = gUnk_03002490;
    u->unk30 = 32;
    sub_0806caa0(1, 0, 0);
    gUnk_03002490->unk46 = sub_0806cc90(0, 4, 8, 24);
    sub_080062c4();
    sub_080061c0(-32768, 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_08006338(24);
    sub_08006138();
}

void sub_08099908(void)
{
    struct Task *t;

    sub_080663f4(&gUnk_08274840, 16);
    t = gUnk_03002490;
    if (--t->unk30 < 0)
    {
        sub_0806395c(15);
        sub_08006148(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099944(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 15;
    u = gUnk_03002490;
    u->unk30 = zero;
    sub_0806caa0(1, 0, 0);
    sub_080062c4();
    sub_08006338(24);
    TaskYieldTrampoline(170);
    v = gUnk_03002490;
    v->unk20 = zero;
    sub_0806ad18();
    w = gUnk_03002490;
    w->unk30 = 1;
    sub_08006138();
}

void sub_0809998c(void)
{
    struct Task *t;

    sub_080663f4(&gUnk_08274840, 16);
    t = gUnk_03002490;
    if (t->unk30 != 0)
    {
        t->unk20 = 1;
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
}

void sub_080999c4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk60 = 0x5000;
    t->unk15 = 16;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
    }
}

void sub_08099a0c(void)
{
}

void sub_08099a10(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 17;
    sub_080062c4();
    sub_08006338(12);
    TaskYieldTrampoline(1);
    sub_08006338(8);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    TaskYieldTrampoline(36);
    sub_0806395c(3);
    sub_08006138();
}

void sub_08099a54(void)
{
    if (gUnk_03002490->unk14 != 17)
        sub_08006148(sub_08098fb0, gCurTaskIdx);
}

void sub_08099a7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 18;
    u = gUnk_03002490;
    u->unk30 = 24;
    u->unk60 = 0x5000;
    u->unk68 = 0x70000;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(1);
    }
}

void sub_08099ad0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk30 <= 0)
        t->unk20 = 1;
    u = gUnk_03002490;
    u->unk30--;
}

u8 sub_08099aec(void)
{
    if (gUnk_03002490->unk14 == 0)
    {
        sub_0806395c(22);
        sub_08006148(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}
