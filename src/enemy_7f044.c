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
 *                  Task.unk14 / Task.unk15 to CallTableEntry, which indexes the
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
extern u16 gFrameCount;
extern struct PlayerState gUnk_03002170[];
extern struct Task *gCurTask;
extern struct Task gTasks[];

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
extern s32 RandomRange(s32 a);
extern u16 sub_08021c14(s16 x, s16 y);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDistSq(void);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetDyTo(u32 i);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorStepAnim(void);
extern s32 ActorTickAnim(s32 n);
extern s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void sub_080261d4(s32 a);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void TaskFaceToward(u32 i);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroy(void);
extern void sub_0806a0f0(s32 a);
extern void ActorDie(void);
extern void sub_0806d4e4(u32 a, s32 b);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);

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
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0807f094;
    t->unk28 = -1;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741390);
}

void sub_0807f078(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741390);
}

void sub_0807f094(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_0874139C);
    ActorCheckHits();
    ActorReactToHit();
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

    gCurTask->unk15 = 0;
    a = gCurTask;
    if (a->unk28 != -1)
    {
        a->unk3C = 12;
        TaskYieldTrampoline(15);
    }
    b = gCurTask;
    b->unk2C = gUnk_08741318[b->unk74];
    n = RandomRange(3);
    k = gCurTask;
    m = n + 1;
    k->unk30 = n;
    if (m != 0)
    {
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(gCurTask->unk2C);
            *(s16 *)&gCurTask->unk6C = 0;
            do
            {
                c = gCurTask;
                c->unk3C++;
                TaskYieldTrampoline(c->unk2C);
            } while (++*(s16 *)&gCurTask->unk6C <= 6);
        } while (gCurTask->unk30-- != 0);
    }
    d = gCurTask;
    d->unk3C = 12;
    TaskYieldTrampoline(gUnk_0874131A[d->unk74]);
    g = (struct Task *volatile *)&gCurTask;
    tbl = gUnk_0874131C;
    h = &gCurTask;
    z = 0;
    do
    {
        n2 = RandomRange(8);
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
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0807f1f0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_0807f078, gCurTaskIdx);
}

void sub_0807f218(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 1;
    PlaySfx(188);
    gCurTask->unk7A = 0;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk2C = gUnk_08741350[u->unk28];
    TaskSetMotionXFacing(gUnk_08741328[u->unk28], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_0874133C[gCurTask->unk28], 0x2000, 0x60000);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gCurTask->unk2C);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(t->unk2C);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void sub_0807f2b4(void)
{
    if ((s8)gCurTask->unk7A != 0)
    {
        TaskStop();
        ActorSetState(0);
        TaskSetEntry(sub_0807f078, gCurTaskIdx);
    }
}

void sub_0807f2ec(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->unk15 = 2;
    a = gCurTask;
    if (a->unk28 != -1)
        a->unk28 = 0;
    b = gCurTask;
    b->unk60 = 0x2000;
    b->unk68 = 0x60000;
    b->unk2C = gUnk_08741318[b->unk74];
    while (1)
    {
        c = gCurTask;
        if (++c->unk3C > 11)
            c->unk3C = 4;
        TaskYieldTrampoline(gCurTask->unk2C);
    }
}

void sub_0807f348(void)
{
    if ((s8)gCurTask->unk7A != 0)
    {
        TaskStop();
        ActorSetState(0);
        TaskSetEntry(sub_0807f078, gCurTaskIdx);
    }
}

void sub_0807f380(void)
{
    gCurTask->unk04 = (u32)sub_0807f3d4;
    ActorSetHitReactions(gUnk_08741CE0);
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087413A8);
}

void sub_0807f3b8(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087413A8);
}

void sub_0807f3d4(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 == 2)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 3, gUnk_087413B4);
    }
    else
    {
        CallTableEntry(t->unk15, 3, gUnk_087413B4);
    }
    if (gCurTask->unk14 != 1)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_0807f42c(void)
{
    struct Task *a;
    struct Task *b;

    gCurTask->unk15 = 0;
    a = gCurTask;
    a->unk28 = gUnk_08741355[a->unk74];
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            b = gCurTask;
            b->unk3C++;
            TaskYieldTrampoline(8);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void sub_0807f488(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        gCurTask->unk28 = 0;
    t = gCurTask;
    if (--t->unk28 <= 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0807f3b8, gCurTaskIdx);
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

    gCurTask->unk15 = 1;
    a = gCurTask;
    a->unk28 = 48;
    a->unk30 = 7;
    a->unk2C = a->unk3C;
    a->unk34 = 4;
    do
    {
        b = gCurTask;
        if (b->unk34-- == 0)
        {
            if (++b->unk2C > 11)
                b->unk2C = 4;
            gCurTask->unk34 = 4;
        }
        TaskYieldTrampoline(1);
    } while (gCurTask->unk28 != 0);
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(60);
    o = &gTasks[TaskFindNearestPlayer()];
    *(s16 *)&gCurTask->unk6C = 0;
    while (1)
    {
        TaskYieldTrampoline(1);
        n = RandomRange(6);
        c = gCurTask;
        c->unk30 = o->unk48 + (s8)gUnk_08741367[n];
        m = RandomRange(9);
        d = gCurTask;
        d->unk34 = o->unk4A + (s8)gUnk_0874136D[m];
        if (sub_0807f6a8(d->unk30, d->unk34, (s8 *)d->unk8C->unk50) != 0)
            break;
        if (++*(s16 *)&gCurTask->unk6C > 59)
            break;
    }
    if (*(s16 *)&gCurTask->unk6C == 60)
        ActorDestroy();
    e = gCurTask;
    e->unk4C = e->unk30 << 16;
    e->unk50 = e->unk34 << 16;
    e->unk7A = 0;
    f = gCurTask;
    f->unk28 = 48;
    f->unk30 = 0;
    f->unk34 = 4;
    do
    {
        b = gCurTask;
        if (b->unk34-- == 0)
        {
            if (++b->unk2C > 11)
                b->unk2C = 4;
            gCurTask->unk34 = 4;
        }
        TaskYieldTrampoline(1);
    } while (gCurTask->unk28 != 0);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_0807f634(void)
{
    struct Task *t = gCurTask;
    s32 n;
    s32 m;

    n = t->unk28;
    if (n != 0)
    {
        m = gUnk_08741357[(n >> 3) + t->unk30];
        t->unk18 = m;
        t->unk1C = 1 << (n & 7);
        if ((m & t->unk1C) != 0)
            TaskSetFrame(*(s16 *)&t->unk2C);
        else
            t->unk3C = 0xFFFF;
        gCurTask->unk28--;
    }
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_0807f3b8, gCurTaskIdx);
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
    if (gCurTask->unk43 == 1)
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

    gCurTask->unk15 = 2;
    a = gCurTask;
    a->unk2C = 4;
    while (1)
    {
        if ((s8)gCurTask->unk7A == 0)
        {
            TaskSetMotionY(0, 0x2000, 0x60000);
            b = gCurTask;
            b->unk28 = b->unk2C;
            if ((s8)b->unk7A == 0)
            {
                do
                {
                    c = gCurTask;
                    if (c->unk28-- == 0)
                    {
                        if (++c->unk3C > 11)
                            c->unk3C = 4;
                        gCurTask->unk28 = gCurTask->unk2C;
                    }
                    TaskYieldTrampoline(1);
                } while ((s8)gCurTask->unk7A == 0);
            }
        }
        TaskStopY();
        d = gCurTask;
        d->unk3C = 12;
        TaskYieldTrampoline(gUnk_08741365[d->unk74]);
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        if ((s8)gCurTask->unk7A != 0)
        {
            do
            {
                e = gCurTask;
                if (++e->unk3C > 11)
                    e->unk3C = 4;
                TaskYieldTrampoline(1);
            } while ((s8)gCurTask->unk7A != 0);
        }
        gCurTask->unk2C = 1;
    }
}

void sub_0807f888(void)
{
}

void sub_0807f88c(void)
{
    gCurTask->unk04 = (u32)sub_0807f8d8;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_087413C0);
}

void sub_0807f8bc(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_087413C0);
}

void sub_0807f8d8(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 == 1)
    {
        CallTableEntry(t->unk15, 2, gUnk_087413C8);
    }
    else if ((u8)ActorCollideTerrain() == 0)
    {
        CallTableEntry(gCurTask->unk15, 2, gUnk_087413C8);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807f920(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    struct Task *o;

    gCurTask->unk15 = 0;
    t = gCurTask;
    t->unk30 = 3;
    t->unk34 = 0;
    n = TaskFindNearestPlayer();
    u = gCurTask;
    u->unk28 = n;
    o = &gTasks[n];
    if (o->unk4C > u->unk4C)
        u->unk2C = 0;
    else
        u->unk2C = 1;
    TaskStopX();
    while (1)
    {
        do
        {
            TaskFaceToward(gCurTask->unk28);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
        } while (gCurTask->unk3C > 10);
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
        } while (gCurTask->unk3C <= 10);
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

    t = gCurTask;
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
            u = gCurTask;
            u->unk30--;
            u->unk34 = 0;
            u->unk2C ^= 1;
            break;
        }
    }
    v = gCurTask;
    o = &gTasks[v->unk28];
    v->unk1C = o->unk48 + gUnk_08741378[v->unk2C];
    d = v->unk1C - v->unk48;
    v->unk20 = d;
    if (v->unk18 == 0)
    {
        if (abs(d) <= 1)
            v->unk34 = 180;
    }
    w = gCurTask;
    if (w->unk20 >= 0)
        w->unk54 = w->unk54 + 0x2000;
    else
        w->unk54 = w->unk54 + 0xFFFFE000;
    x = gCurTask;
    if (abs(x->unk54) > 0x2FFFF)
    {
        if (x->unk20 >= 0)
            x->unk54 = 0x10000;
        else
            x->unk54 = 0xFFFF0000;
    }
    if (gCurTask->unk30 == 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0807f8bc, gCurTaskIdx);
    }
}

void sub_0807fa98(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    TaskStop();
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(15);
    sub_0806d4e4(1, 0);
    gCurTask->unk58 = 0xFFFC0000;
loop:
l1:
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    if (gCurTask->unk3C > 10)
        goto l1;
    do
    {
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(1);
    } while (gCurTask->unk3C <= 10);
    goto loop;
}

void sub_0807fafc(void)
{
}

void sub_0807fb00(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0807fb60;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087413D0);
}

void sub_0807fb44(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087413D0);
}

void sub_0807fb60(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087413D4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807fb84(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(5);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void sub_0807fbcc(void)
{
}

s32 sub_0807fbd0(void)
{
    struct Task *t;
    s32 r = 0;

    switch (gCurTask->unk73)
    {
    case 0:
    case 2:
    case 3:
        t = gCurTask;
        t->unk54 = -t->unk54;
        break;
    case 1:
        sub_080261d4(1);
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807fc20(void)
{
    s32 r = 0;

    switch (gCurTask->unk73)
    {
    case 0:
        ActorSetState(2);
        TaskSetEntry(sub_0807f078, gCurTaskIdx);
        r = 1;
        break;
    case 1:
    case 2:
        TaskSetMotionY(0, 0x1500, 0x30000);
        break;
    case 3:
        break;
    }
    return r;
}

s32 sub_0807fc70(void)
{
    switch (gCurTask->unk73)
    {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
        TaskStopY();
        break;
    }
    return 0;
}

s32 sub_0807fc94(void)
{
    struct Task *t = gCurTask;

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

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 11;
    gCurTask->unk38 = gUnk_087525E4;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk28 = 0;
    u->unk2C = 1;
    u->unk18 = 0;
    ActorStartAnim(gUnk_087413EC);
    if (RandomRange(4) != 0)
        gCurTask->unk30 = 1;
    else
        gCurTask->unk30 = 3;
    CallTableEntry(gCurTask->unk73, 2, gUnk_08741488);
}

void sub_0807fd34(void)
{
    gCurTask->unk04 = (u32)sub_0807fd80;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741490);
}

void sub_0807fd64(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741490);
}

void sub_0807fd80(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 == 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 3, gUnk_0874149C);
    }
    else
    {
        CallTableEntry(t->unk15, 3, gUnk_0874149C);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807fdc8(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    ActorSetAttackBox(gUnk_0873F720);
    t = gCurTask;
    if ((s8)t->unk7A == 0)
    {
        t->unk60 = 0x1500;
        t->unk68 = 0x30000;
    }
    while (gCurTask->unk30 != 4)
        TaskYieldTrampoline(1);
    TaskSleepForever();
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

    switch (gCurTask->unk30)
    {
    case 0:
        a = gCurTask;
        if (--a->unk34 == 0)
        {
            gCurTask->unk34 = ActorStepAnim();
            sub_08080374(gCurTask->unk34, 10);
        }
        b = gCurTask;
        if ((--b->unk2C & 28) == 0)
        {
            b->unk30++;
            b->unk2C = 23;
            b->unk34 = 1;
            ActorStartAnim(gUnk_087413EC);
        }
        break;
    case 1:
        c = gCurTask;
        n = c->unk2C - 1;
        c->unk2C = n;
        if (n != 0)
        {
            if (--c->unk34 > 0)
                break;
            c->unk34 = ~(n >> 3) & 3;
            ActorStepAnim();
            sub_08080374(1, 10);
        }
        else
        {
            c->unk30++;
            c->unk2C = gUnk_087413D8[c->unk74];
            c->unk34 = 1;
            TaskFaceNearestPlayer();
        }
        break;
    case 2:
        d = gCurTask;
        if (--d->unk2C != 0)
        {
            if (--d->unk34 == 0)
                gCurTask->unk34 = ActorStepAnim();
        }
        else
        {
            d->unk30++;
            d->unk2C = 31;
            d->unk34 = 1;
        }
        break;
    case 3:
        e = gCurTask;
        m = e->unk2C - 1;
        e->unk2C = m;
        if ((28 & m) != 0)
        {
            if (--e->unk34 > 0)
                break;
            e->unk34 = m >> 3;
            ActorStepAnim();
            sub_08080374(1, 10);
        }
        else
        {
            e->unk30++;
            e->unk2C = 31;
            e->unk34 = 1;
            ActorStartAnim(gUnk_08741420);
        }
        break;
    case 4:
        f = gCurTask;
        if (--f->unk34 == 0)
        {
            gCurTask->unk34 = ActorStepAnim();
            sub_08080374(gCurTask->unk34, 10);
        }
        g = gCurTask;
        if ((--g->unk2C & 28) == 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_0807fd64, gCurTaskIdx);
        }
        break;
    }
}

void sub_0807ffa0(void)
{
    struct Task *t;
    s32 n;

    gCurTask->unk15 = 1;
    ActorSetAttackBox(gUnk_08741ADC);
    gCurTask->unk30 = -1;
    gCurTask->unk34 = ActorStartAnim(gUnk_08741454);
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk58 = 0xFFFF8000;
    *(s16 *)&t->unk6C = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (++*(s16 *)&gCurTask->unk6C <= 39);
    gCurTask->unk30++;
    n = TaskFindNearestPlayer();
    gCurTask->unk20 = n;
    if (TaskGetDxTo(n) > 0)
        gCurTask->unk24 = 1;
    else
        gCurTask->unk24 = 0;
    TaskStopY();
    gCurTask->unk2C = 120;
    TaskSleepForever();
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

    t = gCurTask;
    if (--t->unk34 == 0)
    {
        gCurTask->unk34 = ActorStepAnim();
        sub_08080374(gCurTask->unk34, 8);
    }
    t = gCurTask;
    m = t->unk30;
    if (m == -1)
        return;
    if (--t->unk2C == 0)
    {
        if (m == 2)
        {
            ActorSetState(2);
            TaskSetEntry(sub_0807fd64, gCurTaskIdx);
            return;
        }
        if (abs(TaskGetDyTo(gCurTask->unk20)) <= 23)
        {
            u = gCurTask;
            u->unk24 ^= 1;
            u->unk30 = 2;
            u->unk2C = 120;
        }
        else
        {
            v = gCurTask;
            v->unk30++;
            v->unk2C = 40;
        }
    }
    if ((gFrameCount & 7) != 0)
        return;
    w = gCurTask;
    o = &gTasks[w->unk20];
    p = (u16 *)&o->unk48;
    gCurTask->unk1C =
        (u16)ArcTan2((s8)gUnk_087413DA[w->unk24] + *p - *(u16 *)&w->unk48,
                     *(u16 *)&o->unk4A - *(u16 *)&w->unk4A)
        >> 7;
    if (gCurTask->unk1C > 255)
        gCurTask->unk58 = 0xFFFFC000;
    else
        gCurTask->unk58 = 0x4000;
    x = gCurTask;
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
    x = gCurTask;
    n = x->unk54;
    if (abs(n) >= gUnk_087413E4[x->unk74])
        x->unk54 = x->unk1C;
}

void sub_080801cc(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;

    gCurTask->unk15 = 2;
    a = gCurTask;
    a->unk54 = 0;
    a->unk60 = 0xFFFF1900;
    while (gCurTask->unk58 > -0x20000)
        TaskYieldTrampoline(1);
    b = gCurTask;
    b->unk58 = -0x20000;
    b->unk60 = 0;
    if (b->unk28++ == 0)
    {
        while (1)
        {
            c = gCurTask;
            if ((u8)sub_0807f6a8(c->unk48, c->unk4A, (s8 *)c->unk8C->unk50) != 0)
                break;
            TaskYieldTrampoline(1);
        }
        d = gCurTask;
        d->unk2C = 31;
        d->unk30 = 0;
        d->unk34 = 1;
        ActorStartAnim(gUnk_08741420);
        ActorSetState(0);
    }
    TaskSleepForever();
}

void sub_08080278(void)
{
    struct Task *t = gCurTask;

    if (--t->unk34 == 0)
    {
        gCurTask->unk34 = ActorStepAnim();
        sub_08080374(gCurTask->unk34, 8);
    }
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_0807fd64, gCurTaskIdx);
}

void sub_080802bc(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0808031c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087414A8);
}

void sub_08080300(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087414A8);
}

void sub_0808031c(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087414AC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08080340(void)
{
    gCurTask->unk15 = 0;
    TaskSleepForever();
}

void sub_08080358(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_08080374(s32 a, s32 b)
{
    struct Task *t = gCurTask;

    t->unk18 -= a;
    if (t->unk18 <= 0)
    {
        t->unk18 = b;
        PlaySfx(192);
    }
}

s32 sub_08080398(void)
{
    TaskStopY();
    return 0;
}

s32 sub_080803a4(void)
{
    TaskSetMotionY(0, 0x1500, 0x30000);
    return 0;
}

void sub_080803bc(void)
{
    gCurTask->unk58 = 0;
}

s32 sub_080803cc(void)
{
    switch (gCurTask->unk14)
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
    struct Task *t = gCurTask;

    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 11;
    gCurTask->unk38 = gUnk_0875275C;
    CreateChildTaskHere(175, 0);
    CallTableEntry(gCurTask->unk73, 3, gUnk_087414B4);
}

void sub_0808044c(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080804c0;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    if (RandomRange(4) == 0)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087414C0);
}

void sub_080804a4(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087414C0);
}

void sub_080804c0(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_087414CC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080804f0(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    t = gCurTask;
    t->unk28 = 90;
    t->unk2C = 0;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_08080b2c();
}

void sub_0808051c(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (RandomRange(gUnk_087414B0[t->unk2C]) != 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_080804a4, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void sub_08080570(void)
{
    struct ActorSpawn sp;

    gCurTask->unk15 = 1;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 0;
        sp.unk09 = 0;
        sp.unk10 = (gCurTask->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 0;
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            *(s16 *)&gCurTask->unk6E = 0;
            do
            {
                gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->unk6E <= 4);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    else
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 1;
        sp.unk09 = 0;
        sp.unk10 = (gCurTask->unk40 & 0xFFF) | 0xF000;
        gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080806e8(void)
{
    struct PlayerState *p = &gUnk_03002170[TaskFindNearestPlayer()];

    if (*(s8 *)&p->unk0B == 1 || p->unk0D == 1)
    {
        TaskSetFrame(*(s16 *)&gCurTask->unk30);
        ActorSetState(0);
    }
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080804a4, gCurTaskIdx);
}

void sub_08080740(void)
{
    gCurTask->unk15 = 2;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08080b2c();
}

void sub_08080768(void)
{
}

void sub_0808076c(void)
{
    struct Task *t;

    gCurTask->unk04 = (u32)sub_080807d8;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087414D8);
}

void sub_080807bc(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087414D8);
}

void sub_080807d8(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087414DC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080807fc(void)
{
    gCurTask->unk15 = 0;
    sub_08080b2c();
}

void sub_08080814(void)
{
}

void sub_08080818(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0808088c;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    if (RandomRange(4) == 0)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087414E0);
}

void sub_08080870(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087414E0);
}

void sub_0808088c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_087414EC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080808bc(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    t = gCurTask;
    t->unk28 = 90;
    t->unk2C = 0;
    sub_08080b2c();
}

void sub_080808dc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (RandomRange(gUnk_087414B0[t->unk2C]) != 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_08080870, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void sub_08080930(void)
{
    struct ActorSpawn sp;

    gCurTask->unk15 = 1;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 0;
        sp.unk09 = 0;
        sp.unk10 = (gCurTask->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 0;
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            *(s16 *)&gCurTask->unk6E = 0;
            do
            {
                gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->unk6E <= 4);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    else
    {
        sp.unk00 = 6;
        sp.unk04 = 108;
        sp.unk08 = 1;
        sp.unk09 = 0;
        sp.unk10 = (gCurTask->unk40 & 0xFFF) | 0xF000;
        sp.unk0A = 1;
        gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08080aa8(void)
{
    struct PlayerState *p = &gUnk_03002170[TaskFindNearestPlayer()];

    if (*(s8 *)&p->unk0B == 1 || p->unk0D == 1)
    {
        TaskSetFrame(*(s16 *)&gCurTask->unk30);
        ActorSetState(0);
    }
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_08080870, gCurTaskIdx);
}

void sub_08080b00(void)
{
    gCurTask->unk15 = 2;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
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
        TaskSetFrame(*(s16 *)&gCurTask->unk30);
        if (gCurTask->unk30 == 6)
            TaskYieldTrampoline(7);
        else
            TaskYieldTrampoline(8);
        t = gCurTask;
        t->unk30 += t->unk34;
        if (t->unk30 == 4 || t->unk30 == 8)
            t->unk34 = -t->unk34;
    }
}
