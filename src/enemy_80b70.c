/* game_code_and_rodata 0x08080B70-0x080820B8 (issue #71, module M21 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08080B70 0x080820B8 src/enemy_80b70.c --newpb
 *
 * Second third of enemy/object behaviour bank 2 (see src/enemy_7f044.c for
 * the three-table pattern all of M21 is built from).  It holds:
 *   * task #175, the only class-4 type in the bank: `sub_08080c48` is the
 *     coroutine itself (it re-seats the actor next to gUnk_03002790[unk44]
 *     every cycle) with per-frame hook `sub_08080d58`, and `sub_08080b70` /
 *     `sub_08080bcc` are its unk73 handlers (`0x08741BF4`);
 *   * task #31's dispatcher `sub_08080dd0` (`0x08741544`, four rows) and
 *     task #38's `sub_080819b4` (`0x087415B8`, three rows);
 *   * six scripts in the entry/hook shape: `sub_08080e10`+`sub_08080e5c`
 *     (`0x08741554`/`0x0874156C`, six states), `sub_08081408`+`sub_0808145c`
 *     (`0x08741584`), `sub_08081614`+`sub_08081668` (`0x0874159C`),
 *     `sub_08081774`+`sub_080817d4` (`0x087415A4`), `sub_080819f4`+
 *     `sub_08081a74` (`0x087415C4`) and `sub_08081d24`+`sub_08081d84`
 *     (`0x087415DC`);
 *   * the shared helpers `sub_08080374` (drain Task.unk18 by N and fire cue
 *     192 when it runs out), `sub_08081814` (the eight-step walk animation)
 *     and `sub_08081e64` (flip Task.unk43 from the gUnk_03005550[4] input,
 *     returning 1 when the input already matches the facing).
 *
 * `sub_0808164c`, `sub_080817b8` and `sub_08081d68` are dead exports of the
 * same kind as batch 1's; `sub_08081960`, `sub_08081e40`, `sub_08081f08` and
 * `sub_08081f18` are pointer-referenced leaves the census originally missed
 * (both classes curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gUnk_03002158[];
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern vu16 gUnk_03004CA0[];
extern u8 gUnk_03005550[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern s32 gUnk_087414F8[];
extern u8 gUnk_08741500[];
extern u8 gUnk_08741502[];
extern s32 gUnk_08741504[];
extern u8 gUnk_0874150C[];
extern s32 gUnk_0874151C[];
extern s32 gUnk_08741524[];
extern u32 gUnk_08741544[];
extern u32 gUnk_08741554[];
extern u32 gUnk_0874156C[];
extern u32 gUnk_08741584[];
extern u32 gUnk_08741590[];
extern u32 gUnk_0874159C[];
extern u32 gUnk_087415A0[];
extern u32 gUnk_087415A4[];
extern u32 gUnk_087415A8[];
extern s32 gUnk_087415AC[];
extern u32 gUnk_087415B8[];
extern u32 gUnk_087415C4[];
extern u32 gUnk_087415D0[];
extern u32 gUnk_087415DC[];
extern u32 gUnk_087415E0[];
extern u32 gUnk_08741604[];
extern u32 gUnk_087419F4[];
extern u32 gUnk_08741A20[];
extern u32 gUnk_08741B3C[];
extern u32 gUnk_08741B44[];
extern u32 gUnk_0874CBD0[];
extern u32 gUnk_08752708[];
extern u32 gUnk_087528D0[];
extern u32 gUnk_087528F0[];
extern u32 gUnk_08752934[];
extern u32 gUnk_08752978[];
extern u32 gUnk_087529A8[];

/* Externals */
extern s32 sub_08002ee8(s32 a);
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 sub_08063df4(void);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 sub_08064314(s32 prec);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern u8 sub_08065f2c(u32 i);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_08063908(u32 *def);
extern void sub_0806395c(u32 v);
extern void sub_080639b4(u32 *p);
extern void sub_080639e0(u32 *p);
extern void sub_08063e14(void);
extern void sub_08063ff4(void);
extern void sub_08064a60(void);
extern void sub_0806a0f0(s32 a);
extern void sub_08065640(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_080804a4(void);
extern void sub_08080870(void);

/* Defined below */
void sub_08080d58(void);
void sub_08080e5c(void);
void sub_08081814(void);
void sub_0808145c(void);
void sub_08081668(void);
void sub_080817d4(void);
void sub_08081a74(void);
s32 sub_08081e64(void);
void sub_08081d84(void);

s32 sub_08080b70(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(2);
        sub_08006148(sub_080804a4, gCurTaskIdx);
        break;
    case 1:
        break;
    case 2:
        sub_0806395c(2);
        sub_08006148(sub_08080870, gCurTaskIdx);
        break;
    }
    return 1;
}

s32 sub_08080bcc(void)
{
    sub_08006244();
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(0);
        sub_08006148(sub_080804a4, gCurTaskIdx);
        break;
    case 1:
        break;
    case 2:
        sub_0806395c(0);
        sub_08006148(sub_08080870, gCurTaskIdx);
        break;
    }
    return 1;
}

s32 sub_08080c2c(void)
{
    sub_08063ff4();
    return 0;
}

s32 sub_08080c38(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08080c48(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *o;
    s32 n;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk04 = (u32)sub_08080d58;
    t->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_0874CBD0;
    while (1)
    {
        o = &gUnk_03002790[gUnk_03002490->unk44];
        sub_08064a60();
        u = gUnk_03002490;
        u->unk48 = *(u16 *)&o->unk48 - (u->unk43 << 2);
        u->unk4C = u->unk48 << 16;
        n = sub_08002ee8(16);
        v = gUnk_03002490;
        v->unk4A = *(u16 *)&o->unk4A + 4 - n;
        v->unk50 = v->unk4A << 16;
        sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFF8000;
        sub_08006338(18);
        TaskYieldTrampoline(2);
        sub_08006338(20);
        TaskYieldTrampoline(2);
        sub_08006338(14);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF0000;
        sub_08006338(16);
        TaskYieldTrampoline(2);
        sub_08006338(18);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF0000;
        sub_08006338(24);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(10);
    }
}

void sub_08080d58(void)
{
    struct Task *p;
    struct Actor **q;
    struct Actor *a;
    s32 n;
    s32 k;

    if (sub_08065f2c(gUnk_03002490->unk44) == 1)
    {
        p = gUnk_03002790;
        n = gUnk_03002490->unk44;
        k = n * 144;
        q = (struct Actor **)((u8 *)p + 140);
        a = *(struct Actor **)((u8 *)q + k);
        if ((s16)gUnk_03004CA0[n] == -1 || a->unk05 == 2)
            sub_08005654(gCurTaskIdx);
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_08080dd0(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752708;
    sub_08002e98(u->unk73, 4, gUnk_08741544);
}

void sub_08080e10(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08080e5c;
    t->unk28 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08741554);
}

void sub_08080e40(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08741554);
}

void sub_08080e5c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 5)
    {
        sub_08002e98(t->unk15, 6, gUnk_0874156C);
    }
    else if ((u8)sub_080692fc() == 0)
    {
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_0874156C);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08080ea4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    sub_080061c0(gUnk_087414F8[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08081814();
}

void sub_08080edc(void)
{
    gUnk_03002490->unk18 = (u16)sub_08064314(3);
    if (gUnk_03002490->unk18 == 0 || gUnk_03002490->unk18 > 255)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 47)
        {
            sub_0806395c(1);
            sub_08006148(sub_08080e40, gCurTaskIdx);
        }
    }
}

void sub_08080f34(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gUnk_03002490->unk15 = 1;
    a = gUnk_03002490;
    a->unk2C = 0;
    a->unk54 = 0;
    sub_08006338(11);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    sub_080061c0(0x20000, 0x5A5A5A5A);
    b = gUnk_03002490;
    b->unk58 = 0xFFFE0000;
    b->unk30 = 16;
    while (1)
    {
        TaskYieldTrampoline(6);
        c = gUnk_03002490;
        if (++c->unk30 > 18)
            c->unk30 = 16;
    }
}

void sub_08080fa4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;

    i = 2;
    t = gUnk_03002490;
    if ((s8)t->unk7A != 0)
        return;
    if (gUnk_03002158[i] - 9 > t->unk4A)
    {
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        sub_0806395c(2);
        sub_08006148(sub_08080e40, gCurTaskIdx);
        return;
    }
    if (sub_08063cd0() < 0)
    {
        u = gUnk_03002490;
        u->unk54 = u->unk54 + 0xFFFFE700;
    }
    else
    {
        u = gUnk_03002490;
        u->unk54 = u->unk54 + 0x1900;
    }
    v = gUnk_03002490;
    if ((v->unk54 < 0 ? -v->unk54 : v->unk54) > 0x20000)
    {
        sub_080061c0(0x20000, 0x5A5A5A5A);
    }
    else
    {
        if (v->unk54 > 0)
            v->unk43 = 1;
        else
            v->unk43 = -1;
    }
    w = gUnk_03002490;
    if ((w->unk54 < 0 ? -w->unk54 : w->unk54) <= 0x5FFF)
        sub_08006338(13);
    else
        sub_08006338(*(s16 *)&w->unk30);
}

void sub_08081084(void)
{
    struct Task *a;
    struct Task *b;

    gUnk_03002490->unk15 = 2;
    a = gUnk_03002490;
    a->unk2C++;
    sub_080062c4();
    b = gUnk_03002490;
    b->unk3C = 0xFFFF;
    b->unk30 = gUnk_08741500[b->unk74];
    sub_08006138();
}

void sub_080810c4(void)
{
    struct Task *t;
    struct Task *o;
    s32 n;
    s32 i;

    i = 2;
    t = gUnk_03002490;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (n == 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_08080e40, gCurTaskIdx);
    }
    else
    {
        if (n > gUnk_08741502[t->unk74])
        {
            o = &gUnk_03002790[sub_08063b38()];
            gUnk_03002490->unk4C = o->unk48 << 16;
        }
        gUnk_03002490->unk50 = (gUnk_03002158[i] - 9) << 16;
    }
}

void sub_08081140(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gUnk_03002490->unk15 = 3;
    sub_0800622c(0x20000, 0x1500, 0x30000);
    sub_08006338(20);
    while ((s8)gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    if (gUnk_03002490->unk2C != 0)
        sub_080261d4(1);
    gUnk_03002490->unk7A = 0;
    sub_08006338(13);
    sub_0800622c(0xFFFE0000, 0x1500, 0x30000);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    a = gUnk_03002490;
    a->unk30 = 0;
    if ((s8)a->unk7A == 0)
    {
        do
        {
            b = gUnk_03002490;
            if (--b->unk30 <= 0)
            {
                if (++b->unk3C > 15)
                    b->unk3C = 14;
                gUnk_03002490->unk30 = 4;
            }
            TaskYieldTrampoline(1);
        } while ((s8)gUnk_03002490->unk7A == 0);
    }
    sub_080062c4();
    c = gUnk_03002490;
    if (c->unk28++ == 0)
    {
        sub_08006338(13);
        TaskYieldTrampoline(60);
        sub_0806395c(0);
    }
    else
    {
        sub_0806395c(5);
    }
    sub_08006138();
}

void sub_0808124c(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_08080e40, gCurTaskIdx);
}

void sub_08081274(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 5;
    if (sub_08063df4() == 1)
        gUnk_03002490->unk43 = -1;
    else
        gUnk_03002490->unk43 = 1;
    sub_08006338(11);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x6600, 0);
    t = gUnk_03002490;
    t->unk58 = 0;
    t->unk60 = 0xFFFFE700;
    sub_08006338(13);
    sub_08006138();
}

void sub_080812ec(void)
{
}

void sub_080812f0(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gUnk_03002490->unk15 = 4;
    sub_080639e0(gUnk_08741B44);
    a = gUnk_03002490;
    a->unk5C = 0;
    a->unk58 = 0x18000;
    sub_08006338(16);
    b = gUnk_03002490;
    b->unk30 = 4;
    if ((s8)b->unk7A == 0)
    {
        do
        {
            c = gUnk_03002490;
            if (--c->unk30 == 0)
            {
                if (++c->unk3C > 18)
                    sub_08006338(16);
                gUnk_03002490->unk30 = 4;
            }
            TaskYieldTrampoline(1);
        } while ((s8)gUnk_03002490->unk7A == 0);
    }
    sub_08006244();
    sub_080061c0(0x5A5A5A5A, gUnk_08741504[gUnk_03002490->unk74]);
    sub_08006338(16);
    while (1)
    {
        sub_0806cc90(1, 1, -2, 5);
        TaskYieldTrampoline(8);
    }
}

void sub_080813a4(void)
{
    struct Task *t = gUnk_03002490;
    s32 d;

    if ((s8)t->unk7A != 0)
    {
        d = t->unk43;
        if ((d == 1 && t->unk54 <= 0) || (d == -1 && t->unk54 >= 0))
        {
            sub_08006214();
            sub_080639e0(gUnk_08741B3C);
            sub_0806395c(0);
            sub_08006148(sub_08080e40, gCurTaskIdx);
        }
    }
}

void sub_08081408(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0808145c;
    t->unk28 = 216;
    t->unk2C = 0;
    t->unk30 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741584);
}

void sub_08081440(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741584);
}

void sub_0808145c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08741590);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808148c(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    sub_080061c0(0x8000, 0x5A5A5A5A);
    sub_08081814();
}

void sub_080814b4(void)
{
    struct Task *t = gUnk_03002490;
    s32 n;

    n = t->unk28 + 1;
    t->unk28 = n;
    if (n == t->unk2C)
    {
        sub_0806395c(1);
        sub_08006148(sub_08081440, gCurTaskIdx);
    }
    else if (n == 217)
    {
        t->unk28 = 0;
        gUnk_03002490->unk2C = gUnk_0874150C[sub_08002ee8(16)];
    }
}

void sub_08081508(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk54 = 0;
    sub_08006338(11);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFE0000, 0x1000, 0x30000);
    sub_08006338(13);
    sub_08006138();
}

void sub_08081560(void)
{
    struct Task *t = gUnk_03002490;

    if ((s8)t->unk7A == 0 && t->unk58 >= 0)
    {
        sub_0806395c(2);
        sub_08006148(sub_08081440, gCurTaskIdx);
    }
}

void sub_0808159c(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    t = gUnk_03002490;
    t->unk54 = 0;
    t->unk60 = 0x1000;
    t->unk68 = 0x30000;
    while (1)
    {
        sub_08006338(14);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
    }
}

void sub_080815dc(void)
{
    if ((s8)gUnk_03002490->unk7A != 0)
    {
        sub_08006244();
        sub_0806395c(0);
        sub_08006148(sub_08081440, gCurTaskIdx);
    }
}

void sub_08081614(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08081668;
    t->unk7A = 0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874159C);
}

void sub_0808164c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874159C);
}

void sub_08081668(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087415A0);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808168c(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = 6;
    t->unk2C = 0;
    t->unk30 = 6;
    sub_080061c0(gUnk_0874151C[t->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(16);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

void sub_080816e8(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    struct Task *x;
    s32 n;
    s32 v;
    s32 w;

    n = t->unk30;
    if (n == 0)
        return;
    if (--t->unk28 != 0)
        return;
    if (++t->unk2C == 8)
    {
        t->unk2C = 0;
        t->unk30 = n - 1;
        if (t->unk30 == 0)
        {
            t->unk60 = 0xFFFFE700;
            return;
        }
    }
    v = sub_08063d2c();
    u = gUnk_03002490;
    u->unk18 = v;
    w = gUnk_08741524[u->unk2C];
    u->unk1C = w;
    if ((v < 0 ? -v : v) > 10)
    {
        if (v < 0)
        {
            if (w < 0)
                u->unk28 = 8;
            else
                u->unk28 = 6;
        }
        else
        {
            if (w > 0)
                u->unk28 = 8;
            else
                u->unk28 = 6;
        }
    }
    else
    {
        u->unk28 = 6;
    }
    x = gUnk_03002490;
    x->unk58 = x->unk1C;
}

void sub_08081774(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_080817d4;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087415A4);
}

void sub_080817b8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087415A4);
}

void sub_080817d4(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087415A8);
    sub_08068e04();
    sub_08069b44();
}

void sub_080817f8(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08081814();
}

void sub_08081810(void)
{
}

void sub_08081814(void)
{
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(8);
        TaskYieldTrampoline(6);
        sub_08006338(10);
        TaskYieldTrampoline(10);
        sub_08006338(9);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

s32 sub_08081884(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk73 == 1 && t->unk30 != 0)
        sub_08063ff4();
    return 0;
}

s32 sub_080818a8(void)
{
    s32 r = 0;
    struct Task *t = gUnk_03002490;

    switch (t->unk73)
    {
    case 0:
        if (t->unk14 == 1)
        {
            sub_0806395c(3);
            sub_08006148(sub_08080e40, gCurTaskIdx);
            r = 1;
        }
        break;
    case 1:
        t->unk58 = 0;
        sub_0806395c(2);
        sub_08006148(sub_08081440, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        break;
    }
    return r;
}

s32 sub_08081900(void)
{
    s32 r = 0;
    struct Task *t = gUnk_03002490;

    switch (t->unk73)
    {
    case 0:
        if (t->unk14 == 0 || t->unk14 == 4)
        {
            sub_0806395c(4);
            sub_08006148(sub_08080e40, gCurTaskIdx);
            r = 1;
        }
        break;
    case 1:
        t->unk30++;
        sub_0806395c(2);
        sub_08006148(sub_08081440, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        break;
    }
    return r;
}

s32 sub_08081960(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk73 == 0 && t->unk14 == 1)
        t->unk7A = 0;
    return 0;
}

s32 sub_08081984(void)
{
    s32 n = gUnk_03002490->unk73;

    if (n >= 0)
    {
        if (n <= 1)
            sub_08063ff4();
    }
    return 0;
}

s32 sub_080819a4(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_080819b4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087528D0;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_087415B8);
}

void sub_080819f4(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk04 = (u32)sub_08081a74;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk2C = 0;
    t->unk7A = 0;
    u = gUnk_03002490;
    if (u->unk73 == 0)
    {
        u->unk30 = 0;
        sub_0806395c(0);
    }
    else
    {
        u->unk73 = 0;
        gUnk_03002490->unk30 = 30;
        sub_0806395c(2);
    }
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087415C4);
}

void sub_08081a58(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087415C4);
}

void sub_08081a74(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087415D0);
    if (gUnk_03002490->unk30 == 0)
        sub_08068e04();
    sub_08069b44();
}

void sub_08081aac(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 0;
    while (1)
    {
        gUnk_03002490->unk34 = -1;
        sub_080062c4();
        sub_08006338(7);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk7A = 0;
        sub_080061c0(gUnk_087415AC[gUnk_03002490->unk74], 0x5A5A5A5A);
        t = gUnk_03002490;
        t->unk58 = 0xFFFF3300;
        t->unk34 = 0;
        t->unk24 = 6;
        sub_08006338(5);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        u = gUnk_03002490;
        u->unk3C++;
        if (u->unk7A == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gUnk_03002490->unk7A == 0);
        }
    }
}

void sub_08081b5c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk34 == -1)
        return;
    if (t->unk7A != 0)
    {
        sub_08081e64();
    }
    else if (t->unk24 != 0)
    {
        if (--t->unk24 == 0)
        {
            t->unk60 = 0x1500;
            t->unk68 = 0x30000;
        }
    }
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08081a58, gCurTaskIdx);
}

void sub_08081bc4(void)
{
    gUnk_03002490->unk15 = 1;
    sub_08006244();
    TaskYieldTrampoline(6);
    sub_080061c0(gUnk_087415AC[gUnk_03002490->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(7);
        TaskYieldTrampoline(6);
        sub_08006338(5);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
    }
}

void sub_08081c3c(void)
{
    if (gUnk_03002490->unk7A != 0 && (u8)sub_08081e64() == 0)
    {
        sub_0806395c(0);
        sub_08006148(sub_08081a58, gCurTaskIdx);
    }
}

void sub_08081c78(void)
{
    gUnk_03002490->unk15 = 2;
    sub_0800622c(0xFFFE0000, 0x1500, 0x30000);
    sub_08006338(6);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    sub_08006138();
}

void sub_08081ce0(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    if (t->unk30 != 0)
        t->unk30--;
    u = gUnk_03002490;
    if (u->unk7A != 0)
    {
        u->unk30 = 0;
        sub_0806395c(0);
        sub_08006148(sub_08081a58, gCurTaskIdx);
    }
}

void sub_08081d24(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08081d84;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087415DC);
}

void sub_08081d68(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087415DC);
}

void sub_08081d84(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087415E0);
    sub_08068e04();
    sub_08069b44();
}

void sub_08081db4(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk24 = 0;
    while (1)
    {
        sub_080062c4();
        sub_08006338(7);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk7A = 0;
        t = gUnk_03002490;
        t->unk58 = 0xFFFF3300;
        t->unk24 = 6;
        sub_08006338(5);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        u = gUnk_03002490;
        u->unk3C++;
        if (u->unk7A == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gUnk_03002490->unk7A == 0);
        }
    }
}

void sub_08081e40(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk24 != 0)
    {
        if (--t->unk24 == 0)
        {
            t->unk60 = 0x1500;
            t->unk68 = 0x30000;
        }
    }
}

s32 sub_08081e64(void)
{
    s32 r = 0;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gUnk_03002490;
    n = gUnk_03005550[4];
    t->unk1C = n;
    if (t->unk43 == 1)
    {
        if (n == 1 || n == 3)
            r = 1;
    }
    else
    {
        if (n == 2 || n == 4)
            r = 1;
    }
    if (r == 0)
    {
        u = gUnk_03002490;
        u->unk28++;
        if (u->unk2C == 0)
        {
            if ((u->unk28 & 1) == 0 && sub_08002ee8(3) == 0)
            {
                v = gUnk_03002490;
                v->unk2C = 1;
                if (v->unk43 == 1)
                    v->unk43 = -1;
                else
                    v->unk43 = 1;
            }
        }
        else
        {
            u->unk2C--;
            if (u->unk43 == 1)
                u->unk43 = -1;
            else
                u->unk43 = 1;
        }
    }
    else
    {
        sub_0806395c(1);
    }
    return r;
}

void sub_08081f08(void)
{
    gUnk_03002490->unk58 = 0;
}

s32 sub_08081f18(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 1)
    {
        t->unk60 = 0x1500;
        t->unk68 = 0x30000;
    }
    return 0;
}

s32 sub_08081f38(void)
{
    struct Task *t;

    sub_08063ff4();
    t = gUnk_03002490;
    t->unk2C = 0;
    t->unk28 = 0;
}

s32 sub_08081f50(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08081f60(void)
{
    struct Task *s;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    s = gUnk_03002490;
    s->unk00 = (u32)sub_080656b4;
    s->unk42 = 11;
    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
    case 2:
        u = gUnk_03002490;
        u->unk0C = (u32)sub_08065640;
        u->unk38 = gUnk_087528F0;
        break;
    case 1:
        t->unk0C = (u32)sub_0806523c;
        sub_08063908(gUnk_087419F4);
        gUnk_03002490->unk38 = gUnk_08752978;
        break;
    }
    v = gUnk_03002490;
    v->unk28 = 0;
    v->unk2C = v->unk40;
    v->unk40 = (v->unk40 & 0xFFF) | 0xF000;
    sub_08002e98(v->unk73, 3, gUnk_08741604);
}

void sub_08082008(void)
{
    struct Task *s;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    s = gUnk_03002490;
    s->unk00 = (u32)sub_080656b4;
    s->unk42 = 11;
    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
    case 2:
        u = gUnk_03002490;
        u->unk0C = (u32)sub_08065640;
        u->unk38 = gUnk_08752934;
        u->unk74 = 0;
        break;
    case 1:
        t->unk0C = (u32)sub_0806523c;
        sub_08063908(gUnk_08741A20);
        gUnk_03002490->unk38 = gUnk_087529A8;
        break;
    }
    v = gUnk_03002490;
    v->unk2C = v->unk40;
    v->unk28 = 1;
    v->unk40 = (v->unk40 & 0xFFF) | 0xF000;
    sub_08002e98(v->unk73, 3, gUnk_08741604);
}
