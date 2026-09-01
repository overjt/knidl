/* game_code_and_rodata 0x08084D14-0x080860F8 (issue #69, module M22 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08084D14 0x080860F8 src/enemy_84d14.c --newpb
 *
 * Three more scripts in the same three-table shape as src/enemy_82e68.c:
 *   * class-3 task #14 (entry `sub_08084cd4` in the previous file): unk73 rows
 *     `0x08741FB8`, bodies `0x08741FC0`, guards `0x08741FD4`, per-frame hook
 *     `sub_08084d90`;
 *   * class-3 task #17 (`sub_08085450`): `0x08741FE8` / `0x08741FF8` /
 *     `0x08742004`, per-frame hook `sub_08085500`;
 *   * the `sub_08085858` script: `0x08742030` / `0x08742040`, per-frame hook
 *     `sub_080858cc`;
 *   * class-3 task #20 (`sub_08086090` / `sub_080860d8`), whose seven unk73
 *     rows at `0x08742064` all point INTO module M23 - the first cross-module
 *     dispatch found in the behaviour banks.
 *
 * `sub_08085390` / `sub_080853c8` / `sub_08085404` / `sub_0808542c` and
 * `sub_08085e74` / `sub_08085ef0` / `sub_08085fa0` / `sub_08086024` /
 * `sub_08085fec` are the class-3 hook rows at `0x08742D0C` / `0x08742D1C` and
 * `0x08742D28` / `0x08742D38` / `0x08742D40`; the second group switches on
 * Task.unk73 (0 = plain, 1 = riding a carrier, 2-3 = ignore) instead of only
 * bailing out on 1.
 *
 * `sub_08085fec` is the second leaf the prologue scan missed (lesson 4.30):
 * the table word at `0x08742D40` points at it and it clamps Task.unk58 (the
 * 16.16 vertical velocity) at zero.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u32 gUnk_087523A0[];
extern s32 gUnk_08742010[];
extern u8 gUnk_08742020[];
extern u8 gUnk_08742028[];
extern struct AnimCmd gUnk_08742050[];
extern u32 gUnk_08742A40[];
extern u32 gUnk_08742064[];
extern u32 gUnk_0875240C[];
extern u32 gUnk_08742030[];
extern u32 gUnk_08742040[];
extern u32 gUnk_08742A6C[];
extern u8 gUnk_0874202C[];
extern u8 gUnk_08742024[];
extern u32 gUnk_08741FE8[];
extern u32 gUnk_08741FF8[];
extern u32 gUnk_08742004[];
extern u32 gUnk_08741FC0[];
extern u32 gUnk_08741FD4[];

/* Externals */
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_080031b8(s32 a);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(s32 a);
extern void sub_080639b4(u32 *p);
extern void sub_08063e14(void);
extern s32 sub_08064188(s32 n);
extern void sub_08063ff4(void);
extern void sub_08066b34(u32 *p);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void sub_080640c8(void);
extern void sub_08066c3c(u32 *p);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066b70(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
extern void sub_080656b4(void);
extern void sub_0806523c(void);
extern s32 sub_08063d2c(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_08064b28(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08021a40(s32 x, s32 y);
extern s32 sub_08063df4(void);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern void sub_08069b44(void);

/* Defined below */
void sub_08084d90(void);
void sub_0808537c(void);
void sub_08085500(void);
void sub_080858cc(void);
void sub_08085cc4(void);
void sub_08085e60(void);

void sub_08084d14(void)
{
    u32 v;

    gUnk_03002490->unk04 = (u32)sub_08084d90;
    v = sub_08002ee8(4);
    switch (v)
    {
    case 0:
    case 1:
        sub_0806395c(0);
        break;
    case 2:
        sub_0806395c(2);
        break;
    case 3:
        sub_0806395c(3);
        break;
    }
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_08741FC0);
}

void sub_08084d6c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08084d90;
    sub_08002e98(t->unk14, 5, gUnk_08741FC0);
}

void sub_08084d90(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_08741FD4);
    sub_08068e04();
    sub_08069b44();
}

void sub_08084dc0(void)
{
    struct Task *t;
    u32 zero;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = zero;
    sub_080062c4();
    sub_08063e14();
    gUnk_03002490->unk6C = zero;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(1);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(2);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(2);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(8);
        u4 = gUnk_03002490;
        u4->unk3C--;
        TaskYieldTrampoline(2);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(2);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk43 = -gUnk_03002490->unk43;
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    sub_0806395c(1);
    sub_08006138();
}

void sub_08084e74(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08084d6c, gCurTaskIdx);
}

void sub_08084e9c(void)
{
    s32 a;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    gUnk_03002490->unk15 = 1;
    while (1)
    {
        a = gUnk_03002490->unk43;
        if (a == sub_08063df4()
            && (sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 32)
        {
            sub_0806395c(2);
            sub_08006138();
        }
        gUnk_03002490->unk43 = -gUnk_03002490->unk43;
        sub_08006338(4);
        TaskYieldTrampoline(1);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(2);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(2);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(8);
        u4 = gUnk_03002490;
        u4->unk3C--;
        TaskYieldTrampoline(2);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(2);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(1);
    }
}

void sub_08084f50(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08084d6c, gCurTaskIdx);
}

void sub_08084f78(void)
{
    struct Task *t;
    u16 zero;

    gUnk_03002490->unk15 = 2;
    t = gUnk_03002490;
    zero = 0;
    if (t->unk74 == 0)
    {
        t->unk6C = zero;
        do
        {
            sub_08063e14();
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_080061c0(0x30000, 0x5A5A5A5A);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_080061c0(0x20000, 0x5A5A5A5A);
            sub_08006338(6);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            sub_08006338(6);
            TaskYieldTrampoline(2);
            sub_080061c0(0x8000, 0x5A5A5A5A);
            sub_08006338(6);
            TaskYieldTrampoline(4);
            sub_08006338(7);
            TaskYieldTrampoline(18);
            sub_08006214();
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(6);
            TaskYieldTrampoline(6);
            sub_08006338(5);
            TaskYieldTrampoline(5);
            sub_08006338(4);
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 3);
    }
    else
    {
        t->unk6C = zero;
        do
        {
            sub_08063e14();
            sub_08006338(4);
            TaskYieldTrampoline(3);
            sub_080061c0(0x40000, 0x5A5A5A5A);
            sub_08006338(5);
            TaskYieldTrampoline(3);
            sub_080061c0(0x20000, 0x5A5A5A5A);
            sub_08006338(5);
            TaskYieldTrampoline(1);
            sub_08006338(6);
            TaskYieldTrampoline(2);
            sub_080061c0(0x10000, 0x5A5A5A5A);
            sub_08006338(6);
            TaskYieldTrampoline(3);
            sub_080061c0(0x8000, 0x5A5A5A5A);
            sub_08006338(6);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(19);
            sub_08006214();
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(6);
            TaskYieldTrampoline(5);
            sub_08006338(5);
            TaskYieldTrampoline(4);
            sub_08006338(4);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
    }
    sub_08063e14();
    sub_0806395c(3);
    sub_08006138();
}

void sub_08085158(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_08084d6c, gCurTaskIdx);
}

void sub_08085180(void)
{
    struct Task *t;
    struct Task *u;
    struct ActorSpawn sp;
    u16 zero1;
    u8 zero2;

    t = gUnk_03002490;
    zero1 = 0;
    t->unk15 = 3;
    sub_080062c4();
    sub_08006338(10);
    gUnk_03002490->unk6C = zero1;
    do
    {
        sub_080061c0(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        u = gUnk_03002490;
        u->unk6C++;
    } while ((s16)u->unk6C <= 14);
    sp.unk00 = 2;
    sp.unk04 = 104;
    sp.unk08 = zero2 = 0;
    sp.unk09 = u->unk74;
    sp.unk0A = zero2;
    gUnk_03002490->unk46 = sub_08064b28(&sp, 0);
    gUnk_03002490->unk6C = zero2;
    do
    {
        gUnk_03002490->unk54 = -0x10000;
        sub_08006338(11);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = -0x10000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 63);
    sub_080062c4();
    sub_0806395c(0);
    sub_08006138();
}

void sub_08085274(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_08084d6c, gCurTaskIdx);
}

void sub_0808529c(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk60 = 0x2500;
    t->unk68 = 0x30000;
    sub_08006138();
}

void sub_080852c8(void)
{
}

void sub_080852cc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808537c;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(5);
        TaskYieldTrampoline(2);
        sub_08006338(5);
        TaskYieldTrampoline(2);
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(6);
        TaskYieldTrampoline(4);
        sub_08006338(7);
        TaskYieldTrampoline(18);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        sub_08006338(6);
        TaskYieldTrampoline(6);
        sub_08006338(5);
        TaskYieldTrampoline(5);
        sub_08006338(4);
        TaskYieldTrampoline(4);
    }
}

void sub_0808537c(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

u8 sub_08085390(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk73 == 1)
        return 0;
    sub_0806395c((u16)t->unk28);
    sub_08006148(sub_08084d6c, gCurTaskIdx);
    return 1;
}

u8 sub_080853c8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk73 == 1)
        return 0;
    t->unk28 = t->unk14;
    sub_0806395c(4);
    sub_08006148(sub_08084d6c, gCurTaskIdx);
    return 1;
}

u8 sub_08085404(void)
{
    if (gUnk_03002490->unk73 == 1)
        return 0;
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808542c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk73 != 1 && t->unk14 == 2)
        sub_08063ff4();
    return 0;
}

void sub_08085450(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087523A0;
    u->unk8C->unk1A = 4;
    sub_08002e98(u->unk73, 4, gUnk_08741FE8);
}

void sub_08085498(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08085500;
    t->unk28 = 15;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741FF8);
}

void sub_080854d0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08085500;
    if (t->unk14 == 0)
        t->unk28 = 80;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741FF8);
}

void sub_08085500(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742004);
    sub_08068e04();
    sub_08069b44();
}

void sub_08085530(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk15 = 0;
    sub_080061c0(gUnk_08742010[gUnk_03002490->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(gUnk_08742020[gUnk_03002490->unk74]);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[u1->unk74]);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[u2->unk74]);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[u3->unk74]);
        u4 = gUnk_03002490;
        u4->unk3C++;
        TaskYieldTrampoline(gUnk_08742020[u4->unk74]);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[u5->unk74]);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[u6->unk74]);
        u7 = gUnk_03002490;
        u7->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[u7->unk74]);
    }
}

void sub_08085608(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gUnk_03002490;
    if (t->unk2C == 0 && --t->unk28 == 0)
    {
        v = sub_08002ee8(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gUnk_03002490->unk28 = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        sub_0806395c(n);
        sub_08006148(sub_080854d0, gCurTaskIdx);
    }
}

void sub_08085660(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 0;
    sub_0800622c(-0x28000, 0x1500, 0x30000);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(8);
    sub_08006138();
}

void sub_080856dc(void)
{
}

void sub_080856e0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 2;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk6C = zero;
    while ((s16)gUnk_03002490->unk6C < gUnk_08742028[gUnk_03002490->unk74])
    {
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(11);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    }
    v = gUnk_03002490;
    zero2 = 0;
    v->unk34 = zero2;
    sub_080031b8(110);
    sub_08006338(12);
    gUnk_03002490->unk6C = zero2;
    while ((s16)gUnk_03002490->unk6C < gUnk_0874202C[gUnk_03002490->unk74])
    {
        sp.unk00 = 4;
        sp.unk04 = 106;
        sp.unk08 = 0;
        w = gUnk_03002490;
        sp.unk09 = w->unk74;
        sp.unk0C = 8;
        sp.unk0E = 3;
        sp.unk0A = 1;
        if (sub_08021a40(w->unk48 + (w->unk43 << 3), w->unk4A + 3) == 0)
            gUnk_03002490->unk46 = sub_08064b5c(&sp, 0);
        TaskYieldTrampoline(2);
        x = gUnk_03002490;
        x->unk34++;
        if ((s16)x->unk3C == 12)
            sub_08006338(13);
        else
            sub_08006338(12);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(0);
    sub_08006138();
}

void sub_0808582c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C == 0 && t->unk14 != 2)
        sub_08006148(sub_080854d0, gCurTaskIdx);
}

void sub_08085858(void)
{
    struct Task *t;

    gUnk_03002490->unk04 = (u32)sub_080858cc;
    sub_08066b34(gUnk_08742A6C);
    gUnk_03002490->unk28 = 15;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08742030);
}

void sub_0808589c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080858cc;
    if (t->unk14 == 0)
        t->unk28 = 80;
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08742030);
}

void sub_080858cc(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 4, gUnk_08742040);
    sub_08068e04();
    sub_08069b44();
}

void sub_080858fc(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk15 = 0;
    sub_080061c0(gUnk_08742010[1], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(gUnk_08742020[1]);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u4 = gUnk_03002490;
        u4->unk3C++;
        TaskYieldTrampoline(gUnk_08742020[1]);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u7 = gUnk_03002490;
        u7->unk3C--;
        TaskYieldTrampoline(gUnk_08742024[1]);
    }
}

void sub_08085998(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gUnk_03002490;
    if (t->unk2C == 0 && t->unk8C->unk1A == -1 && --t->unk28 == 0)
    {
        v = sub_08002ee8(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gUnk_03002490->unk28 = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        sub_0806395c(n);
        sub_08006148(sub_0808589c, gCurTaskIdx);
    }
}

void sub_08085a04(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 0;
    sub_0800622c(-0x28000, 0x1500, 0x30000);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(2);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(8);
    sub_08006138();
}

void sub_08085a80(void)
{
}

void sub_08085a84(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gUnk_03002490;
    zero = 0;
    t->unk15 = 2;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk6C = zero;
    while ((s16)gUnk_03002490->unk6C < gUnk_08742028[1])
    {
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(11);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    }
    v = gUnk_03002490;
    zero2 = 0;
    v->unk34 = zero2;
    sub_080031b8(110);
    sub_08006338(12);
    gUnk_03002490->unk6C = zero2;
    while ((s16)gUnk_03002490->unk6C < gUnk_0874202C[gUnk_03002490->unk74])
    {
        sp.unk00 = 4;
        sp.unk04 = 106;
        sp.unk08 = 0;
        sp.unk09 = 1;
        sp.unk0C = 8;
        sp.unk0E = 3;
        sp.unk0A = 1;
        w = gUnk_03002490;
        if (sub_08021a40(w->unk48 + (w->unk43 << 3), w->unk4A + 3) == 0)
            gUnk_03002490->unk46 = sub_08064b5c(&sp, 0);
        TaskYieldTrampoline(2);
        x = gUnk_03002490;
        x->unk34++;
        if ((s16)x->unk3C == 12)
            sub_08006338(13);
        else
            sub_08006338(12);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(0);
    sub_08006138();
}

void sub_08085bb8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk2C == 0 && t->unk14 != 2)
        sub_08006148(sub_0808589c, gCurTaskIdx);
}

void sub_08085be4(void)
{
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk30 = sub_080640dc(gUnk_08742050);
    sub_08066ba8();
    while (1)
    {
        sub_08066bdc();
        TaskYieldTrampoline(8);
    }
}

void sub_08085c10(void)
{
    gUnk_03002490->unk30 = sub_08064188(gUnk_03002490->unk30);
}

void sub_08085c2c(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk04 = (u32)sub_08085cc4;
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(10);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(7);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(7);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(7);
        u4 = gUnk_03002490;
        u4->unk3C++;
        TaskYieldTrampoline(10);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(7);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(7);
        u7 = gUnk_03002490;
        u7->unk3C--;
        TaskYieldTrampoline(7);
    }
}

void sub_08085cc4(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_08085cd8(void)
{
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk04 = (u32)sub_08085e60;
    sub_08063e14();
    while (1)
    {
        sub_080062c4();
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(4);
            TaskYieldTrampoline(10);
            u1 = gUnk_03002490;
            u1->unk3C++;
            TaskYieldTrampoline(7);
            u2 = gUnk_03002490;
            u2->unk3C++;
            TaskYieldTrampoline(7);
            u3 = gUnk_03002490;
            u3->unk3C++;
            TaskYieldTrampoline(7);
            u4 = gUnk_03002490;
            u4->unk3C++;
            TaskYieldTrampoline(10);
            u5 = gUnk_03002490;
            u5->unk3C--;
            TaskYieldTrampoline(7);
            u6 = gUnk_03002490;
            u6->unk3C--;
            TaskYieldTrampoline(7);
            u7 = gUnk_03002490;
            u7->unk3C--;
            TaskYieldTrampoline(7);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        sub_08006338(6);
        TaskYieldTrampoline(32);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(6);
            TaskYieldTrampoline(2);
            sub_08006338(11);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 15);
        gUnk_03002490->unk34 = 0;
        sub_080031b8(110);
        sub_08006338(12);
        gUnk_03002490->unk6C = 0;
        do
        {
            sp.unk00 = 4;
            sp.unk04 = 106;
            sp.unk08 = 0;
            sp.unk09 = 0;
            sp.unk0C = 8;
            sp.unk0E = 3;
            sp.unk0A = 1;
            w = gUnk_03002490;
            if (sub_08021a40(w->unk48 + (w->unk43 << 3), w->unk4A + 3) == 0)
                gUnk_03002490->unk46 = sub_08064b5c(&sp, 0);
            TaskYieldTrampoline(2);
            x = gUnk_03002490;
            x->unk34++;
            if ((s16)x->unk3C == 12)
                sub_08006338(13);
            else
                sub_08006338(12);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 15);
    }
}

void sub_08085e60(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

u8 sub_08085e74(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->unk2C = 0;
        sub_08006244();
        sub_0806395c(0);
        sub_08006148(sub_080854d0, gCurTaskIdx);
        return 1;
    case 1:
        sub_080640c8();
        sub_08066c3c(gUnk_08742A40);
        sub_08006244();
        sub_0806395c(0);
        sub_08006148(sub_0808589c, gCurTaskIdx);
        return 1;
    }
}

u8 sub_08085ef0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->unk2C = 1;
        t->unk60 = 0x1500;
        t->unk68 = 0x30000;
        sub_0806395c(0);
        sub_08006148(sub_080854d0, gCurTaskIdx);
        return 1;
    case 1:
        if (t->unk8C->unk1A == -1)
        {
            t->unk2C = 1;
            t->unk60 = 0x1500;
            t->unk68 = 0x30000;
            sub_0806395c(0);
            sub_08006148(sub_0808589c, gCurTaskIdx);
        }
        else
        {
            sub_0806395c(3);
            sub_08006148(sub_08085be4, gCurTaskIdx);
        }
        return 1;
    }
}

u8 sub_08085fa0(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        sub_0806a0f0(-2);
        return 1;
    case 1:
        sub_08066c08(gUnk_08742A40, 0);
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_08085fec(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->unk58 = 0;
        return 0;
    case 1:
        if (t->unk58 < 0)
            t->unk58 = 0;
        return 0;
    }
}

s32 sub_08086024(void)
{
    struct Task *t;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        sub_08063ff4();
        return 0;
    case 1:
        if (t->unk14 == 3)
            sub_08066b70();
        else
            sub_08063ff4();
        return 0;
    }
}

void sub_0808606c(void)
{
    sub_08066c08(gUnk_08742A40, 0);
    sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_08086090(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875240C;
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk73, 7, gUnk_08742064);
}

void sub_080860d8(void)
{
    sub_08002e98(gUnk_03002490->unk73, 7, gUnk_08742064);
}
