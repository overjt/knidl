/* game_code_and_rodata 0x080820B8-0x08082E68 (issue #71, module M21 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080820B8 0x08082E68 src/enemy_820b8.c --newpb
 *
 * Last third of enemy/object behaviour bank 2 (pattern: src/enemy_7f044.c).
 * It holds:
 *   * task #42's dispatcher `sub_080826d8` (`0x08741640`) and task #43's
 *     `sub_08082de4` (`0x0874176C`), plus the unk73 quartets at
 *     `0x08741C44` (`sub_08082678` / `sub_080826a0` / `sub_080826bc` /
 *     `sub_080826c8`) and `0x08741C64` (`sub_08082d14` / `sub_08082d4c` /
 *     `sub_08082db0` / `sub_08082dd4`);
 *   * five scripts in the entry/hook shape: `sub_080820b8`+`sub_08082108`
 *     (`0x08741610`/`0x08741614`; its hook is the one that spawns the
 *     class-0 sub-actors 30/31/32 through sub_08064fc4 and hands them to
 *     sub_080b5540), `sub_080822b0`+`sub_08082300` (`0x08741618`),
 *     `sub_0808248c`+`sub_080824ec` (`0x08741620`), `sub_08082718`+
 *     `sub_0808276c` (`0x0874164C`/`0x08741664`, six states) and
 *     `sub_08082bb8`+`sub_08082c18` (`0x0874167C`);
 *   * `sub_08082554` / `sub_080825ec`, the hop cycle that drives the actor
 *     record's unk16/unk18/unk1A/unk1E straight from the task, and
 *     `sub_08082cc4`, the four-step Task.unk2C-scaled animation loop;
 *   * `sub_08082c5c`, the "turn around once every 30 frames if the player is
 *     behind and within 31 units" probe shared by the bank's walkers.
 *
 * `sub_080820ec`, `sub_080822e4`, `sub_080824d0` and `sub_08082bfc` are dead
 * exports; `sub_08082458` is a pointer-referenced leaf the census originally
 * missed (both curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u8 gUnk_02006178;
extern s32 gUnk_030023B4;
extern s16 gUnk_0300244C;
extern struct Task *gUnk_03002490;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern s32 gUnk_087415E4[];
extern u8 gUnk_087415F0[];
extern s32 gUnk_087415F4[];
extern s32 gUnk_087415FC[];
extern u32 gUnk_08741610[];
extern u32 gUnk_08741614[];
extern u32 gUnk_08741618[];
extern u32 gUnk_0874161C[];
extern u32 gUnk_08741620[];
extern u32 gUnk_08741624[];
extern s32 gUnk_08741628[];
extern s32 gUnk_08741630[];
extern u8 gUnk_08741638[];
extern u8 gUnk_0874163B[];
extern u32 gUnk_08741640[];
extern u32 gUnk_0874164C[];
extern u32 gUnk_08741664[];
extern u32 gUnk_0874167C[];
extern u32 gUnk_08741680[];
extern u32 gUnk_0874176C[];
extern u32 gUnk_087528D0[];
extern u32 gUnk_08752A8C[];
extern u32 gUnk_08752AB4[];

/* Externals */
extern s32 sub_08002ee8(s32 a);
extern s32 sub_08063d2c(void);
extern s32 sub_08063df4(void);
extern s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 sub_08064314(s32 prec);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u32 v);
extern void sub_080639b4(u32 *p);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_0806a0f0(s32 a);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_080b5540(s32 a, s32 b);

/* Defined below */
void sub_08082108(void);
void sub_08082554(void);
void sub_080825ec(void);
void sub_08082300(void);
void sub_080824ec(void);
void sub_0808276c(void);
void sub_08082c5c(void);
void sub_08082cc4(void);
void sub_08082c18(void);

void sub_080820b8(void)
{
    gUnk_03002490->unk04 = (u32)sub_08082108;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741610);
}

void sub_080820ec(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741610);
}

void sub_08082108(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741614);
    sub_08068e04();
    t = gUnk_03002490;
    switch (t->unk7C)
    {
    case 3:
    case 4:
        t->unk0C = (u32)sub_0806523c;
        if (t->unk28 == 0)
            gUnk_03002490->unk46 =
                sub_08064fc4(0, 31, 1, t->unk74, t->unk48, t->unk4A, t->unk40);
        else
            gUnk_03002490->unk46 =
                sub_08064fc4(0, 32, 1, t->unk74, t->unk48, t->unk4A, t->unk40);
        sub_080b5540(gCurTaskIdx, gUnk_03002490->unk46);
        u = gUnk_03002490;
        u->unk40 = u->unk2C;
        if (gUnk_0300244C != 0)
            u->unk38 = gUnk_087528D0;
        sub_08006338(1);
        break;
    case 1:
        if (gUnk_02006178 == 1)
        {
            t->unk8C->unk0D = 1;
            gUnk_03002490->unk8C->unk1A = 8;
        }
        else
        {
            gUnk_03002490->unk46 =
                sub_08064fc4(0, 30, 1, t->unk74, t->unk48, t->unk4A - 14, t->unk2C);
        }
        sub_080b5540(gCurTaskIdx, gUnk_03002490->unk46);
        break;
    }
    sub_08069b44();
}

void sub_08082270(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080061c0(gUnk_087415E4[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08082554();
}

void sub_080822a4(void)
{
    sub_080825ec();
}

void sub_080822b0(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08082300;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk24 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741618);
}

void sub_080822e4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741618);
}

void sub_08082300(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874161C);
    if (gUnk_03002490->unk24 == 2)
        sub_08068e04();
    sub_08069b44();
}

void sub_08082338(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 n;
    s32 *p;
    struct Task **g;

    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    t = gUnk_03002490;
    if (t->unk43 == 1)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    n = sub_08064314(3);
    u = gUnk_03002490;
    u->unk18 = ((u16)n + 256) & 511;
    sub_0806421c(u->unk18, 128);
    gUnk_03002490->unk54 = gUnk_030023B4;
    sub_08006338(4);
    if (gUnk_03002490->unk24 != 2)
    {
        g = &gUnk_03002490;
        p = gUnk_087415F4;
        do
        {
            (*g)->unk7A = 0;
            v = *g;
            sub_0800622c(p[v->unk24], gUnk_087415FC[v->unk24], 0x30000);
            if ((*g)->unk7A == 0)
            {
                do
                {
                    TaskYieldTrampoline(1);
                } while (gUnk_03002490->unk7A == 0);
            }
            w = *g;
        } while (++w->unk24 != 2);
    }
    sub_080062c4();
    x = gUnk_03002490;
    if (x->unk28 == 1)
    {
        gUnk_03002490->unk46 = sub_08064fc4(6, 2, 0, 0, x->unk48, x->unk4A, 0);
        sub_080b5540(gCurTaskIdx, gUnk_03002490->unk46);
        sub_08063fe0();
    }
    sub_08006138();
}

void sub_08082458(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk24 != 2)
    {
        if ((++t->unk1C & 3) == 0)
        {
            t->unk1C = 0;
            if (++t->unk3C > 11)
                t->unk3C = 4;
        }
    }
}

void sub_0808248c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_080824ec;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741620);
}

void sub_080824d0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741620);
}

void sub_080824ec(void)
{
    struct Task *t;

    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741624);
    sub_08068e04();
    t = gUnk_03002490;
    switch (t->unk7C)
    {
    case 3:
    case 4:
        t->unk0C = (u32)sub_0806523c;
        t->unk40 = t->unk2C;
        sub_08006338(1);
        break;
    }
    sub_08069b44();
}

void sub_0808253c(void)
{
    sub_08082554();
}

void sub_08082548(void)
{
    sub_080825ec();
}

void sub_08082554(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    sub_08006338(9);
    gUnk_03002490->unk20 = gUnk_087415F0[gUnk_03002490->unk74];
    while (1)
    {
        t = gUnk_03002490;
        t->unk34 = 0;
        t->unk18 = -1;
        t->unk1C = 7;
        TaskYieldTrampoline(4);
        u = gUnk_03002490;
        u->unk18 = 0xFFFE8000;
        u->unk1C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk1C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk1C--;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk1C++;
        TaskYieldTrampoline(6);
        v = gUnk_03002490;
        v->unk1C++;
        if (v->unk34 != 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gUnk_03002490->unk34 != 0);
        }
    }
}

void sub_080825ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s16 h;

    t = gUnk_03002490;
    if (--t->unk20 == 0)
    {
        t->unk20 = gUnk_087415F0[t->unk74];
        if (++t->unk3C > 16)
            t->unk3C = 9;
    }
    u = gUnk_03002490;
    if (u->unk18 != -1)
    {
        u->unk18 += 0x1800;
        u->unk34 += u->unk18;
        if (u->unk34 > 0)
            u->unk34 = 0;
    }
    v = gUnk_03002490;
    a = v->unk8C;
    a->unk16 = 0;
    h = ((s16 *)&v->unk34)[1];
    v->unk8C->unk18 = h - 16;
    v->unk8C->unk1E = v->unk2C;
    v->unk8C->unk1A = v->unk1C;
    v->unk8C->unk03 = -1;
}

s32 sub_08082678(void)
{
    if (gUnk_03002490->unk73 == 0)
        sub_0800622c(0, 0x1500, 0x30000);
    return 0;
}

s32 sub_080826a0(void)
{
    if (gUnk_03002490->unk73 == 0)
        sub_08006244();
    return 0;
}

s32 sub_080826bc(void)
{
    sub_08063ff4();
    return 0;
}

s32 sub_080826c8(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_080826d8(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752A8C;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_08741640);
}

void sub_08082718(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808276c;
    sub_08063e14();
    gUnk_03002490->unk28 = 30;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_0874164C);
}

void sub_08082750(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_0874164C);
}

void sub_0808276c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08741664);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808279c(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    sub_080061c0(gUnk_08741628[gUnk_03002490->unk74], 0x5A5A5A5A);
    *(s16 *)&gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    } while (++*(s16 *)&gUnk_03002490->unk6C <= 3);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08082818(void)
{
    sub_08082c5c();
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08082750, gCurTaskIdx);
}

void sub_08082844(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080061c0(gUnk_08741630[gUnk_03002490->unk74], 0x5A5A5A5A);
    gUnk_03002490->unk2C = 1;
    sub_08082cc4();
}

void sub_0808287c(void)
{
    sub_08082c5c();
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08082750, gCurTaskIdx);
}

void sub_080828a8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    t = gUnk_03002490;
    t->unk30 = 42;
    t->unk34 = 0;
    while (1)
    {
        sub_0806cc90(1, 1, -4, 6);
        sub_08006338(8);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_08082908(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    if (t->unk34 == 0)
    {
        if (t->unk54 > 0)
            t->unk54 = t->unk54 + 0xFFFFF400;
        else if (t->unk54 < 0)
            t->unk54 = t->unk54 + 0xC00;
        u = gUnk_03002490;
        if (u->unk7A == 0)
        {
            sub_08006214();
            gUnk_03002490->unk34 = 1;
        }
    }
    v = gUnk_03002490;
    if (--v->unk30 == 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_08082750, gCurTaskIdx);
    }
}

void sub_08082980(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gUnk_03002490->unk15 = 3;
    sub_08006214();
    t = gUnk_03002490;
    if (t->unk43 == 1)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    n = sub_08002ee8(3);
    u = gUnk_03002490;
    u->unk30 = gUnk_08741638[n];
    u->unk2C = 2;
    sub_08082cc4();
}

void sub_080829d4(void)
{
    struct Task *t = gUnk_03002490;

    if (--t->unk30 == 0)
    {
        t->unk28 = 30;
        sub_0806395c(1);
        sub_08006148(sub_08082750, gCurTaskIdx);
    }
}

void sub_08082a08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    sub_080261d4(1);
    sub_080031b8(197);
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFD0000, 0x2500, 0x30000);
    t = gUnk_03002490;
    t->unk30 = 0;
    t->unk34 = 3;
    sub_08006338(gUnk_0874163B[0]);
    if (gUnk_03002490->unk7A == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
            u = gUnk_03002490;
            if (--u->unk34 == 0)
            {
                if (++u->unk30 > 3)
                    u->unk30 = 0;
                sub_08006338(gUnk_0874163B[gUnk_03002490->unk30]);
                gUnk_03002490->unk34 = 3;
            }
        } while (gUnk_03002490->unk7A == 0);
    }
    sub_080062c4();
    sub_08006338(8);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    v = gUnk_03002490;
    if (v->unk43 == 1)
        v->unk43 = -1;
    else
        v->unk43 = 1;
    sub_0806395c(1);
    sub_08006138();
}

void sub_08082aec(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_08082750, gCurTaskIdx);
}

void sub_08082b14(void)
{
    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk30 = 0;
    sub_0800622c(0, 0x2500, 0x30000);
    gUnk_03002490->unk2C = 1;
    sub_08082cc4();
}

void sub_08082b48(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk30 != 0)
    {
        if (--t->unk30 == 0)
        {
            t->unk4C = t->unk34;
            t->unk50 = t->unk24;
        }
    }
    if (gUnk_03002490->unk7A != 0)
    {
        sub_08006244();
        if (sub_08063df4() != gUnk_03002490->unk43)
            sub_0806395c(2);
        else
            sub_0806395c(1);
        sub_08006148(sub_08082750, gCurTaskIdx);
    }
}

void sub_08082bb8(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08082c18;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874167C);
}

void sub_08082bfc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874167C);
}

void sub_08082c18(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741680);
    sub_08068e04();
    sub_08069b44();
}

void sub_08082c3c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk2C = 1;
    sub_08082cc4();
}

void sub_08082c58(void)
{
}

void sub_08082c5c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk73 != 0)
        return;
    if (--t->unk28 != 0)
        return;
    t->unk28 = 30;
    if (sub_08002ee8(2) == 0)
        return;
    if (sub_08063df4() == gUnk_03002490->unk43)
        return;
    if ((sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 31)
        sub_0806395c(2);
}

void sub_08082cc4(void)
{
    struct Task *t;

    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(gUnk_03002490->unk2C * 2);
        sub_08006338(4);
        t = gUnk_03002490;
        TaskYieldTrampoline(t->unk2C * 3);
        sub_08006338(6);
        TaskYieldTrampoline(gUnk_03002490->unk2C * 2);
        sub_08006338(5);
        t = gUnk_03002490;
        TaskYieldTrampoline(t->unk2C * 3);
    }
}

s32 sub_08082d14(void)
{
    s32 r = 0;

    switch (gUnk_03002490->unk14)
    {
    case 0:
    case 1:
    case 2:
        sub_0806395c(5);
        sub_08006148(sub_08082750, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08082d4c(void)
{
    s32 r = 0;
    struct Task *t;

    switch (gUnk_03002490->unk14)
    {
    case 0:
    case 1:
        sub_0806395c(4);
        sub_08006148(sub_08082750, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        sub_08063ff4();
        t = gUnk_03002490;
        t->unk30 = 2;
        t->unk34 = t->unk4C;
        t->unk24 = t->unk50;
        break;
    case 2:
        sub_08006214();
        gUnk_03002490->unk34 = 1;
        break;
    }
    return r;
}

s32 sub_08082db0(void)
{
    if (gUnk_03002490->unk14 == 2)
    {
        sub_08006214();
        gUnk_03002490->unk34 = 1;
    }
    return 0;
}

s32 sub_08082dd4(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08082de4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752AB4;
    switch (u->unk73)
    {
    case 0:
        u->unk28 = 20;
        break;
    case 1:
        u->unk73 = 0;
        v = gUnk_03002490;
        if (++v->unk74 > 2)
            gUnk_03002490->unk74 = 2;
        gUnk_03002490->unk28 = 128;
        break;
    case 2:
        break;
    }
    w = gUnk_03002490;
    w->unk2C = 0;
    w->unk30 = 0;
    w->unk34 = 1;
    sub_08002e98(w->unk73, 3, gUnk_0874176C);
}
