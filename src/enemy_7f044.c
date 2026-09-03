/* game_code_and_rodata 0x0807F044-0x08080B70 (issue #71, module M21 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807F044 0x08080B70 src/enemy_7f044.c --newpb
 *
 * M21 is enemy/object behaviour bank 2: nine ROM task types (eight class-3
 * plus the class-4 task #175) whose bodies are built from the same three-table
 * pattern as M22/M24/M25/M26 (rom-map section 9):
 *
 *   entry       -> installs Task.unk04 (the per-frame hook) and hands
 *                  Task.unk14 / Task.unk15 to sub_08002e98, which indexes the
 *                  script's tables;
 *   unk14 table -> the coroutine BODIES (each runs a chain of
 *                  TaskYieldTrampoline waits);
 *   unk15 table -> the per-frame HANDLERS;
 *   unk73 table -> the class-3 dispatch a task type's body selects with.
 *
 * This batch holds:
 *   * task #27's class-3 dispatcher `sub_0807fcbc` (`0x08741488`, two rows)
 *     and its unk73 quartet `sub_0807fbd0` / `sub_0807fc20` / `sub_0807fc70` /
 *     `sub_0807fcac` (`0x08741BB8`);
 *   * task #32's dispatcher `sub_08080400` (`0x087414B4`, three rows);
 *   * nine scripts in the entry/hook shape: `sub_0807f044`+`sub_0807f094`
 *     (`0x08741390`/`0x0874139C`), `sub_0807f380`+`sub_0807f3d4`
 *     (`0x087413A8`/`0x087413B4`), `sub_0807f88c`+`sub_0807f8d8`
 *     (`0x087413C0`/`0x087413C8`), `sub_0807fb00`+`sub_0807fb60`
 *     (`0x087413D0`/`0x087413D4`), `sub_0807fd34`+`sub_0807fd80`
 *     (`0x08741490`/`0x0874149C`), `sub_080802bc`+`sub_0808031c`
 *     (`0x087414A8`), `sub_0808044c`+`sub_080804c0` (`0x087414C0`),
 *     `sub_0808076c`+`sub_080807d8` (`0x087414D8`) and `sub_08080818`+
 *     `sub_0808088c` (`0x087414E0`);
 *   * the bank's four-corner terrain probe `sub_0807f6a8` (four
 *     sub_08021c14 samples around a box whose six signed offsets come from
 *     Actor.unk50) and the jump-table state machine `sub_0807fe18`
 *     (five states over Task.unk30);
 *   * the six-frame flap loop `sub_08080b2c` and the "spawn a puff of six
 *     class-6 actors" routines `sub_08080570` / `sub_08080930`.
 *
 * `sub_0807fb44`, `sub_08080300` and `sub_080807bc` are dead exports: each is
 * a copy of its host's tail dispatch that nothing in the ROM references
 * (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u16 gUnk_03001EA4;
extern struct PlayerState gUnk_03002170[];
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F720[];
extern u8 gUnk_08741318[];
extern u8 gUnk_0874131A[];
extern u8 gUnk_0874131C[];
extern s32 gUnk_08741328[];
extern s32 gUnk_0874133C[];
extern u8 gUnk_08741350[];
extern u8 gUnk_08741355[];
extern u8 gUnk_08741357[];
extern u8 gUnk_08741365[];
extern u8 gUnk_08741367[];
extern u8 gUnk_0874136D[];
extern s32 gUnk_08741378[];
extern u32 gUnk_08741390[];
extern u32 gUnk_0874139C[];
extern u32 gUnk_087413A8[];
extern u32 gUnk_087413B4[];
extern u32 gUnk_087413C0[];
extern u32 gUnk_087413C8[];
extern u32 gUnk_087413D0[];
extern u32 gUnk_087413D4[];
extern u8 gUnk_087413D8[];
extern u8 gUnk_087413DA[];
extern s32 gUnk_087413DC[];
extern s32 gUnk_087413E4[];
extern struct AnimCmd gUnk_087413EC[];
extern struct AnimCmd gUnk_08741420[];
extern struct AnimCmd gUnk_08741454[];
extern u32 gUnk_08741488[];
extern u32 gUnk_08741490[];
extern u32 gUnk_0874149C[];
extern u32 gUnk_087414A8[];
extern u32 gUnk_087414AC[];
extern u8 gUnk_087414B0[];
extern u32 gUnk_087414B4[];
extern u32 gUnk_087414C0[];
extern u32 gUnk_087414CC[];
extern u32 gUnk_087414D8[];
extern u32 gUnk_087414DC[];
extern u32 gUnk_087414E0[];
extern u32 gUnk_087414EC[];
extern u32 gUnk_08741ADC[];
extern u32 gUnk_08741CE0[];
extern u32 gUnk_087525E4[];
extern u32 gUnk_0875275C[];

/* Externals */
extern s32 sub_08002ee8(s32 a);
extern u16 sub_08021c14(s16 x, s16 y);
extern s32 sub_08063b38(void);
extern s32 sub_08063c74(void);
extern s32 sub_08063cbc(u32 i);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d18(u32 i);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s32 sub_08064188(s32 n);
extern s32 sub_08064b28(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
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
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_08063ddc(u32 i);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0806523c(void);
extern void sub_080656b4(void);

/* Defined below */
void sub_0807f094(void);
void sub_0807f3d4(void);
s32 sub_0807f6a8(int px, int py, s8 *p);
void sub_0807f8d8(void);
void sub_0807fb60(void);
void sub_0807fd80(void);
void sub_08080374(s32 a, s32 b);
void sub_0808031c(void);
void sub_080804c0(void);
void sub_08080b2c(void);
void sub_080807d8(void);
void sub_0808088c(void);

void sub_0807f044(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807f094;
    t->unk28 = -1;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741390);
}

void sub_0807f078(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741390);
}

void sub_0807f094(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0874139C);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807f0c4(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *j;
    struct Task *k;
    struct Task *r;
    struct Task *volatile *g;
    struct Task **h;
    s16 *p;
    u8 *tbl;
    s32 q;
    s32 n;
    s32 n2;
    s32 m;
    s32 z;

    gUnk_03002490->unk15 = 0;
    a = gUnk_03002490;
    if (a->unk28 != -1)
    {
        a->unk3C = 12;
        TaskYieldTrampoline(15);
    }
    b = gUnk_03002490;
    b->unk2C = gUnk_08741318[b->unk74];
    n = sub_08002ee8(3);
    k = gUnk_03002490;
    m = n + 1;
    k->unk30 = n;
    if (m != 0)
    {
        do
        {
            sub_08006338(4);
            TaskYieldTrampoline(gUnk_03002490->unk2C);
            *(s16 *)&gUnk_03002490->unk6C = 0;
            do
            {
                c = gUnk_03002490;
                c->unk3C++;
                TaskYieldTrampoline(c->unk2C);
            } while (++*(s16 *)&gUnk_03002490->unk6C <= 6);
        } while (gUnk_03002490->unk30-- != 0);
    }
    d = gUnk_03002490;
    d->unk3C = 12;
    TaskYieldTrampoline(gUnk_0874131A[d->unk74]);
    g = (struct Task *volatile *)&gUnk_03002490;
    tbl = gUnk_0874131C;
    h = &gUnk_03002490;
    z = 0;
    do
    {
        n2 = sub_08002ee8(8);
        e = *g;
        e->unk1C = n2;
        e->unk18 = z;
        *(s16 *)&e->unk6C = z;
    inner:
        f = *g;
        p = (s16 *)&f->unk6C;
        q = *p;
        q += f->unk74 * 5;
        f->unk18 += *(u8 *)(q + (s32)tbl);
        if (f->unk18 <= f->unk1C)
        {
            if (++*p <= 4)
                goto inner;
        }
        r = *g;
    } while (r->unk28 == *(s16 *)&r->unk6C);
    j = *h;
    j->unk28 = *(s16 *)&j->unk6C;
    sub_0806395c(1);
    sub_08006138();
}

void sub_0807f1f0(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0807f078, gCurTaskIdx);
}

void sub_0807f218(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 1;
    sub_080031b8(188);
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk2C = gUnk_08741350[u->unk28];
    sub_080061c0(gUnk_08741328[u->unk28], 0x5A5A5A5A);
    sub_0800622c(gUnk_0874133C[gUnk_03002490->unk28], 0x2000, 0x60000);
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(gUnk_03002490->unk2C);
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(t->unk2C);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 6);
    }
}

void sub_0807f2b4(void)
{
    if ((s8)gUnk_03002490->unk7A != 0)
    {
        sub_080062c4();
        sub_0806395c(0);
        sub_08006148(sub_0807f078, gCurTaskIdx);
    }
}

void sub_0807f2ec(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gUnk_03002490->unk15 = 2;
    a = gUnk_03002490;
    if (a->unk28 != -1)
        a->unk28 = 0;
    b = gUnk_03002490;
    b->unk60 = 0x2000;
    b->unk68 = 0x60000;
    b->unk2C = gUnk_08741318[b->unk74];
    while (1)
    {
        c = gUnk_03002490;
        if (++c->unk3C > 11)
            c->unk3C = 4;
        TaskYieldTrampoline(gUnk_03002490->unk2C);
    }
}

void sub_0807f348(void)
{
    if ((s8)gUnk_03002490->unk7A != 0)
    {
        sub_080062c4();
        sub_0806395c(0);
        sub_08006148(sub_0807f078, gCurTaskIdx);
    }
}

void sub_0807f380(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807f3d4;
    sub_080639a4(gUnk_08741CE0);
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087413A8);
}

void sub_0807f3b8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087413A8);
}

void sub_0807f3d4(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 2)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087413B4);
    }
    else
    {
        sub_08002e98(t->unk15, 3, gUnk_087413B4);
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08068e04();
    sub_08069b44();
}

void sub_0807f42c(void)
{
    struct Task *a;
    struct Task *b;

    gUnk_03002490->unk15 = 0;
    a = gUnk_03002490;
    a->unk28 = gUnk_08741355[a->unk74];
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(8);
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            b = gUnk_03002490;
            b->unk3C++;
            TaskYieldTrampoline(8);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 6);
    }
}

void sub_0807f488(void)
{
    struct Task *t;

    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        gUnk_03002490->unk28 = 0;
    t = gUnk_03002490;
    if (--t->unk28 <= 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0807f3b8, gCurTaskIdx);
    }
}

void sub_0807f4dc(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *o;
    s32 n;
    s32 m;

    gUnk_03002490->unk15 = 1;
    a = gUnk_03002490;
    a->unk28 = 48;
    a->unk30 = 7;
    a->unk2C = a->unk3C;
    a->unk34 = 4;
    do
    {
        b = gUnk_03002490;
        if (b->unk34-- == 0)
        {
            if (++b->unk2C > 11)
                b->unk2C = 4;
            gUnk_03002490->unk34 = 4;
        }
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk28 != 0);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(60);
    o = &gUnk_03002790[sub_08063b38()];
    *(s16 *)&gUnk_03002490->unk6C = 0;
    while (1)
    {
        TaskYieldTrampoline(1);
        n = sub_08002ee8(6);
        c = gUnk_03002490;
        c->unk30 = o->unk48 + (s8)gUnk_08741367[n];
        m = sub_08002ee8(9);
        d = gUnk_03002490;
        d->unk34 = o->unk4A + (s8)gUnk_0874136D[m];
        if (sub_0807f6a8(d->unk30, d->unk34, (s8 *)d->unk8C->unk50) != 0)
            break;
        if (++*(s16 *)&gUnk_03002490->unk6C > 59)
            break;
    }
    if (*(s16 *)&gUnk_03002490->unk6C == 60)
        sub_08063fe0();
    e = gUnk_03002490;
    e->unk4C = e->unk30 << 16;
    e->unk50 = e->unk34 << 16;
    e->unk7A = 0;
    f = gUnk_03002490;
    f->unk28 = 48;
    f->unk30 = 0;
    f->unk34 = 4;
    do
    {
        b = gUnk_03002490;
        if (b->unk34-- == 0)
        {
            if (++b->unk2C > 11)
                b->unk2C = 4;
            gUnk_03002490->unk34 = 4;
        }
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk28 != 0);
    sub_0806395c(2);
    sub_08006138();
}

void sub_0807f634(void)
{
    struct Task *t = gUnk_03002490;
    s32 n;
    s32 m;

    n = t->unk28;
    if (n != 0)
    {
        m = gUnk_08741357[(n >> 3) + t->unk30];
        t->unk18 = m;
        t->unk1C = 1 << (n & 7);
        if ((m & t->unk1C) != 0)
            sub_08006338(*(s16 *)&t->unk2C);
        else
            t->unk3C = 0xFFFF;
        gUnk_03002490->unk28--;
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0807f3b8, gCurTaskIdx);
}

s32 sub_0807f6a8(int px, int py, s8 *p)
{
    u16 x;
    u16 y;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s16 xs;
    s16 ys;
    s16 e;
    s16 f;
    s16 g;
    s16 h;

    x = px;
    y = py;
    a = p[1] + p[2];
    b = p[1] + p[3];
    if (gUnk_03002490->unk43 == 1)
    {
        c = p[0] + p[5];
        d = p[0] + p[4];
    }
    else
    {
        c = -(p[0] + p[4]);
        d = -(p[0] + p[5]);
    }
    xs = x;
    ys = y;
    e = a;
    if (sub_08021c14(xs, ys + e) != 0)
        return 0;
    f = b;
    if (sub_08021c14(xs, ys + f) != 0)
        return 0;
    g = d;
    if (sub_08021c14(xs + g, ys) != 0)
        return 0;
    h = c;
    if (sub_08021c14(xs + h, ys) != 0)
        return 0;
    return 1;
}

void sub_0807f78c(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;

    gUnk_03002490->unk15 = 2;
    a = gUnk_03002490;
    a->unk2C = 4;
    while (1)
    {
        if ((s8)gUnk_03002490->unk7A == 0)
        {
            sub_0800622c(0, 0x2000, 0x60000);
            b = gUnk_03002490;
            b->unk28 = b->unk2C;
            if ((s8)b->unk7A == 0)
            {
                do
                {
                    c = gUnk_03002490;
                    if (c->unk28-- == 0)
                    {
                        if (++c->unk3C > 11)
                            c->unk3C = 4;
                        gUnk_03002490->unk28 = gUnk_03002490->unk2C;
                    }
                    TaskYieldTrampoline(1);
                } while ((s8)gUnk_03002490->unk7A == 0);
            }
        }
        sub_08006244();
        d = gUnk_03002490;
        d->unk3C = 12;
        TaskYieldTrampoline(gUnk_08741365[d->unk74]);
        sub_08063e14();
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006338(4);
        TaskYieldTrampoline(1);
        if ((s8)gUnk_03002490->unk7A != 0)
        {
            do
            {
                e = gUnk_03002490;
                if (++e->unk3C > 11)
                    e->unk3C = 4;
                TaskYieldTrampoline(1);
            } while ((s8)gUnk_03002490->unk7A != 0);
        }
        gUnk_03002490->unk2C = 1;
    }
}

void sub_0807f888(void)
{
}

void sub_0807f88c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807f8d8;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087413C0);
}

void sub_0807f8bc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087413C0);
}

void sub_0807f8d8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 1)
    {
        sub_08002e98(t->unk15, 2, gUnk_087413C8);
    }
    else if ((u8)sub_080692fc() == 0)
    {
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087413C8);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0807f920(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    struct Task *o;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk30 = 3;
    t->unk34 = 0;
    n = sub_08063b38();
    u = gUnk_03002490;
    u->unk28 = n;
    o = &gUnk_03002790[n];
    if (o->unk4C > u->unk4C)
        u->unk2C = 0;
    else
        u->unk2C = 1;
    sub_08006214();
    while (1)
    {
        do
        {
            sub_08063ddc(gUnk_03002490->unk28);
            sub_08006338(4);
            TaskYieldTrampoline(4);
        } while (gUnk_03002490->unk3C > 10);
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
        } while (gUnk_03002490->unk3C <= 10);
    }
}

void sub_0807f9a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *o;
    s32 n;
    s32 d;

    t = gUnk_03002490;
    t->unk18 = 0;
    n = t->unk34;
    if (n != 0)
    {
        t->unk18 = 1;
        t->unk34 = n - 1;
        switch (t->unk34)
        {
        case 0:
        case 60:
        case 120:
            u = gUnk_03002490;
            u->unk30--;
            u->unk34 = 0;
            u->unk2C ^= 1;
            break;
        }
    }
    v = gUnk_03002490;
    o = &gUnk_03002790[v->unk28];
    v->unk1C = o->unk48 + gUnk_08741378[v->unk2C];
    d = v->unk1C - v->unk48;
    v->unk20 = d;
    if (v->unk18 == 0)
    {
        if ((d < 0 ? -d : d) <= 1)
            v->unk34 = 180;
    }
    w = gUnk_03002490;
    if (w->unk20 >= 0)
        w->unk54 = w->unk54 + 0x2000;
    else
        w->unk54 = w->unk54 + 0xFFFFE000;
    x = gUnk_03002490;
    if ((x->unk54 < 0 ? -x->unk54 : x->unk54) > 0x2FFFF)
    {
        if (x->unk20 >= 0)
            x->unk54 = 0x10000;
        else
            x->unk54 = 0xFFFF0000;
    }
    if (gUnk_03002490->unk30 == 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0807f8bc, gCurTaskIdx);
    }
}

void sub_0807fa98(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(15);
    sub_0806d4e4(1, 0);
    gUnk_03002490->unk58 = 0xFFFC0000;
loop:
l1:
    sub_08006338(4);
    TaskYieldTrampoline(1);
    if (gUnk_03002490->unk3C > 10)
        goto l1;
    do
    {
        t = gUnk_03002490;
        t->unk3C++;
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk3C <= 10);
    goto loop;
}

void sub_0807fafc(void)
{
}

void sub_0807fb00(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807fb60;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087413D0);
}

void sub_0807fb44(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087413D0);
}

void sub_0807fb60(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087413D4);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807fb84(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(5);
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(5);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 6);
    }
}

void sub_0807fbcc(void)
{
}

s32 sub_0807fbd0(void)
{
    struct Task *t;
    s32 r = 0;

    switch (gUnk_03002490->unk73)
    {
    case 0:
    case 2:
    case 3:
        t = gUnk_03002490;
        t->unk54 = -t->unk54;
        break;
    case 1:
        sub_080261d4(1);
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807fc20(void)
{
    s32 r = 0;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(2);
        sub_08006148(sub_0807f078, gCurTaskIdx);
        r = 1;
        break;
    case 1:
    case 2:
        sub_0800622c(0, 0x1500, 0x30000);
        break;
    case 3:
        break;
    }
    return r;
}

s32 sub_0807fc70(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
        sub_08006244();
        break;
    }
    return 0;
}

s32 sub_0807fc94(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk73 == 0)
        t->unk58 = 0;
    return 0;
}

s32 sub_0807fcac(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807fcbc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087525E4;
    sub_08063e14();
    u = gUnk_03002490;
    u->unk28 = 0;
    u->unk2C = 1;
    u->unk18 = 0;
    sub_080640dc(gUnk_087413EC);
    if (sub_08002ee8(4) != 0)
        gUnk_03002490->unk30 = 1;
    else
        gUnk_03002490->unk30 = 3;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08741488);
}

void sub_0807fd34(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807fd80;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741490);
}

void sub_0807fd64(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741490);
}

void sub_0807fd80(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0874149C);
    }
    else
    {
        sub_08002e98(t->unk15, 3, gUnk_0874149C);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0807fdc8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    sub_080639b4(gUnk_0873F720);
    t = gUnk_03002490;
    if ((s8)t->unk7A == 0)
    {
        t->unk60 = 0x1500;
        t->unk68 = 0x30000;
    }
    while (gUnk_03002490->unk30 != 4)
        TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_0807fe18(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    s32 n;
    s32 m;

    switch (gUnk_03002490->unk30)
    {
    case 0:
        a = gUnk_03002490;
        if (--a->unk34 == 0)
        {
            gUnk_03002490->unk34 = sub_080640fc();
            sub_08080374(gUnk_03002490->unk34, 10);
        }
        b = gUnk_03002490;
        if ((--b->unk2C & 28) == 0)
        {
            b->unk30++;
            b->unk2C = 23;
            b->unk34 = 1;
            sub_080640dc(gUnk_087413EC);
        }
        break;
    case 1:
        c = gUnk_03002490;
        n = c->unk2C - 1;
        c->unk2C = n;
        if (n != 0)
        {
            if (--c->unk34 > 0)
                break;
            c->unk34 = ~(n >> 3) & 3;
            sub_080640fc();
            sub_08080374(1, 10);
        }
        else
        {
            c->unk30++;
            c->unk2C = gUnk_087413D8[c->unk74];
            c->unk34 = 1;
            sub_08063e14();
        }
        break;
    case 2:
        d = gUnk_03002490;
        if (--d->unk2C != 0)
        {
            if (--d->unk34 == 0)
                gUnk_03002490->unk34 = sub_080640fc();
        }
        else
        {
            d->unk30++;
            d->unk2C = 31;
            d->unk34 = 1;
        }
        break;
    case 3:
        e = gUnk_03002490;
        m = e->unk2C - 1;
        e->unk2C = m;
        if ((28 & m) != 0)
        {
            if (--e->unk34 > 0)
                break;
            e->unk34 = m >> 3;
            sub_080640fc();
            sub_08080374(1, 10);
        }
        else
        {
            e->unk30++;
            e->unk2C = 31;
            e->unk34 = 1;
            sub_080640dc(gUnk_08741420);
        }
        break;
    case 4:
        f = gUnk_03002490;
        if (--f->unk34 == 0)
        {
            gUnk_03002490->unk34 = sub_080640fc();
            sub_08080374(gUnk_03002490->unk34, 10);
        }
        g = gUnk_03002490;
        if ((--g->unk2C & 28) == 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_0807fd64, gCurTaskIdx);
        }
        break;
    }
}

void sub_0807ffa0(void)
{
    struct Task *t;
    s32 n;

    gUnk_03002490->unk15 = 1;
    sub_080639b4(gUnk_08741ADC);
    gUnk_03002490->unk30 = -1;
    gUnk_03002490->unk34 = sub_080640dc(gUnk_08741454);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk58 = 0xFFFF8000;
    *(s16 *)&t->unk6C = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (++*(s16 *)&gUnk_03002490->unk6C <= 39);
    gUnk_03002490->unk30++;
    n = sub_08063b38();
    gUnk_03002490->unk20 = n;
    if (sub_08063cbc(n) > 0)
        gUnk_03002490->unk24 = 1;
    else
        gUnk_03002490->unk24 = 0;
    sub_08006244();
    gUnk_03002490->unk2C = 120;
    sub_08006138();
}

void sub_0808003c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *o;
    s32 n;
    s32 m;
    u16 *p;
    s32 k;

    t = gUnk_03002490;
    if (--t->unk34 == 0)
    {
        gUnk_03002490->unk34 = sub_080640fc();
        sub_08080374(gUnk_03002490->unk34, 8);
    }
    t = gUnk_03002490;
    m = t->unk30;
    if (m == -1)
        return;
    if (--t->unk2C == 0)
    {
        if (m == 2)
        {
            sub_0806395c(2);
            sub_08006148(sub_0807fd64, gCurTaskIdx);
            return;
        }
        if ((sub_08063d18(gUnk_03002490->unk20) < 0
                 ? -sub_08063d18(gUnk_03002490->unk20)
                 : sub_08063d18(gUnk_03002490->unk20))
            <= 23)
        {
            u = gUnk_03002490;
            u->unk24 ^= 1;
            u->unk30 = 2;
            u->unk2C = 120;
        }
        else
        {
            v = gUnk_03002490;
            v->unk30++;
            v->unk2C = 40;
        }
    }
    if ((gUnk_03001EA4 & 7) != 0)
        return;
    w = gUnk_03002490;
    o = &gUnk_03002790[w->unk20];
    p = (u16 *)&o->unk48;
    gUnk_03002490->unk1C =
        (u16)ArcTan2((s8)gUnk_087413DA[w->unk24] + *p - *(u16 *)&w->unk48,
                     *(u16 *)&o->unk4A - *(u16 *)&w->unk4A)
        >> 7;
    if (gUnk_03002490->unk1C > 255)
        gUnk_03002490->unk58 = 0xFFFFC000;
    else
        gUnk_03002490->unk58 = 0x4000;
    x = gUnk_03002490;
    if (x->unk1C >= 128 && x->unk1C <= 383)
    {
        x->unk1C = x->unk54;
        x->unk54 = x->unk1C - gUnk_087413DC[x->unk74];
    }
    else
    {
        x->unk1C = x->unk54;
        x->unk54 = x->unk1C + gUnk_087413DC[x->unk74];
    }
    x = gUnk_03002490;
    n = x->unk54;
    if ((n < 0 ? -n : n) >= gUnk_087413E4[x->unk74])
        x->unk54 = x->unk1C;
}

void sub_080801cc(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;

    gUnk_03002490->unk15 = 2;
    a = gUnk_03002490;
    a->unk54 = 0;
    a->unk60 = 0xFFFF1900;
    while (gUnk_03002490->unk58 > -0x20000)
        TaskYieldTrampoline(1);
    b = gUnk_03002490;
    b->unk58 = -0x20000;
    b->unk60 = 0;
    if (b->unk28++ == 0)
    {
        while (1)
        {
            c = gUnk_03002490;
            if ((u8)sub_0807f6a8(c->unk48, c->unk4A, (s8 *)c->unk8C->unk50) != 0)
                break;
            TaskYieldTrampoline(1);
        }
        d = gUnk_03002490;
        d->unk2C = 31;
        d->unk30 = 0;
        d->unk34 = 1;
        sub_080640dc(gUnk_08741420);
        sub_0806395c(0);
    }
    sub_08006138();
}

void sub_08080278(void)
{
    struct Task *t = gUnk_03002490;

    if (--t->unk34 == 0)
    {
        gUnk_03002490->unk34 = sub_080640fc();
        sub_08080374(gUnk_03002490->unk34, 8);
    }
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0807fd64, gCurTaskIdx);
}

void sub_080802bc(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0808031c;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087414A8);
}

void sub_08080300(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087414A8);
}

void sub_0808031c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087414AC);
    sub_08068e04();
    sub_08069b44();
}

void sub_08080340(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006138();
}

void sub_08080358(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_08080374(s32 a, s32 b)
{
    struct Task *t = gUnk_03002490;

    t->unk18 -= a;
    if (t->unk18 <= 0)
    {
        t->unk18 = b;
        sub_080031b8(192);
    }
}

s32 sub_08080398(void)
{
    sub_08006244();
    return 0;
}

s32 sub_080803a4(void)
{
    sub_0800622c(0, 0x1500, 0x30000);
    return 0;
}

void sub_080803bc(void)
{
    gUnk_03002490->unk58 = 0;
}

s32 sub_080803cc(void)
{
    switch (gUnk_03002490->unk14)
    {
    case 0:
        sub_0806a0f0(-2);
        break;
    case 1:
    case 2:
        sub_0806a0f0(-2);
        break;
    }
    return 1;
}

void sub_08080400(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875275C;
    sub_08064d34(175, 0);
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_087414B4);
}

void sub_0808044c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_080804c0;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk30 = 6;
    t->unk34 = -1;
    if (sub_08002ee8(4) == 0)
        sub_0806395c(1);
    else
        sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087414C0);
}

void sub_080804a4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087414C0);
}

void sub_080804c0(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087414CC);
    sub_08068e04();
    sub_08069b44();
}

void sub_080804f0(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = 90;
    t->unk2C = 0;
    sub_080061c0(0x8000, 0x5A5A5A5A);
    sub_08080b2c();
}

void sub_0808051c(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (sub_08002ee8(gUnk_087414B0[t->unk2C]) != 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_080804a4, gCurTaskIdx);
        }
        else
        {
            u = gUnk_03002490;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void sub_08080570(void)
{
    struct ActorSpawn sp;

    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    sub_080062c4();
    sub_08006338(9);
    TaskYieldTrampoline(24);
    sub_08063e14();
    if ((sub_08063c74() < 0 ? -sub_08063c74() : sub_08063c74()) <= 0x143F)
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 0;
        sp.unk09 = 0;
        sp.unk10 = (gUnk_03002490->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 0;
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            *(s16 *)&gUnk_03002490->unk6E = 0;
            do
            {
                gUnk_03002490->unk46 = sub_08064b28(&sp, 1);
                sub_08006338(10);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gUnk_03002490->unk6E <= 4);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
    }
    else
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 1;
        sp.unk09 = 0;
        sp.unk10 = (gUnk_03002490->unk40 & 0xFFF) | 0xF000;
        gUnk_03002490->unk46 = sub_08064b28(&sp, 1);
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(10);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
    }
    sub_080062c4();
    TaskYieldTrampoline(15);
    sub_0806395c(0);
    sub_08006138();
}

void sub_080806e8(void)
{
    struct PlayerState *p = &gUnk_03002170[sub_08063b38()];

    if (*(s8 *)&p->filler07[4] == 1 || p->unk0D == 1)
    {
        sub_08006338(*(s16 *)&gUnk_03002490->unk30);
        sub_0806395c(0);
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080804a4, gCurTaskIdx);
}

void sub_08080740(void)
{
    gUnk_03002490->unk15 = 2;
    sub_08006214();
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08080b2c();
}

void sub_08080768(void)
{
}

void sub_0808076c(void)
{
    struct Task *t;

    gUnk_03002490->unk04 = (u32)sub_080807d8;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk30 = 6;
    t->unk34 = -1;
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087414D8);
}

void sub_080807bc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087414D8);
}

void sub_080807d8(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087414DC);
    sub_08068e04();
    sub_08069b44();
}

void sub_080807fc(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08080b2c();
}

void sub_08080814(void)
{
}

void sub_08080818(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0808088c;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk30 = 6;
    t->unk34 = -1;
    if (sub_08002ee8(4) == 0)
        sub_0806395c(1);
    else
        sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087414E0);
}

void sub_08080870(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087414E0);
}

void sub_0808088c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087414EC);
    sub_08068e04();
    sub_08069b44();
}

void sub_080808bc(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = 90;
    t->unk2C = 0;
    sub_08080b2c();
}

void sub_080808dc(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (sub_08002ee8(gUnk_087414B0[t->unk2C]) != 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_08080870, gCurTaskIdx);
        }
        else
        {
            u = gUnk_03002490;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void sub_08080930(void)
{
    struct ActorSpawn sp;

    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    sub_080062c4();
    sub_08006338(9);
    TaskYieldTrampoline(24);
    sub_08063e14();
    if ((sub_08063c74() < 0 ? -sub_08063c74() : sub_08063c74()) <= 0x143F)
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 0;
        sp.unk09 = 0;
        sp.unk10 = (gUnk_03002490->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 0;
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            *(s16 *)&gUnk_03002490->unk6E = 0;
            do
            {
                gUnk_03002490->unk46 = sub_08064b28(&sp, 1);
                sub_08006338(10);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gUnk_03002490->unk6E <= 4);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
    }
    else
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 1;
        sp.unk09 = 0;
        sp.unk10 = (gUnk_03002490->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 1;
        gUnk_03002490->unk46 = sub_08064b28(&sp, 1);
        *(s16 *)&gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(10);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gUnk_03002490->unk6C <= 5);
    }
    sub_080062c4();
    TaskYieldTrampoline(15);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08080aa8(void)
{
    struct PlayerState *p = &gUnk_03002170[sub_08063b38()];

    if (*(s8 *)&p->filler07[4] == 1 || p->unk0D == 1)
    {
        sub_08006338(*(s16 *)&gUnk_03002490->unk30);
        sub_0806395c(0);
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08080870, gCurTaskIdx);
}

void sub_08080b00(void)
{
    gUnk_03002490->unk15 = 2;
    sub_08006214();
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08080b2c();
}

void sub_08080b28(void)
{
}

void sub_08080b2c(void)
{
    struct Task *t;

    while (1)
    {
        sub_08006338(*(s16 *)&gUnk_03002490->unk30);
        if (gUnk_03002490->unk30 == 6)
            TaskYieldTrampoline(7);
        else
            TaskYieldTrampoline(8);
        t = gUnk_03002490;
        t->unk30 += t->unk34;
        if (t->unk30 == 4 || t->unk30 == 8)
            t->unk34 = -t->unk34;
    }
}
