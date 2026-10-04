/* game_code_and_rodata 0x0807F044-0x08080B70 (issue #71, module M21 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807F044 0x08080B70 src/enemy_7f044.c --newpb
 *
 * M21 is enemy/object behaviour bank 2: nine ROM task types (eight class-3
 * plus the class-4 task #175) whose bodies are built from the same three-table
 * pattern as M22/M24/M25/M26 (rom-map section 9):
 *
 *   entry       -> installs Task.updateCallback (the per-frame hook) and hands
 *                  Task.state / Task.updateState to CallTableEntry, which indexes the
 *                  script's tables;
 *   unk14 table -> the coroutine BODIES (each runs a chain of
 *                  TaskYieldTrampoline waits);
 *   unk15 table -> the per-frame HANDLERS;
 *   unk73 table -> the class-3 dispatch a task type's body selects with.
 *
 * This batch holds:
 *   * task #27's class-3 dispatcher `Task_Twister` (`0x08741488`, two rows)
 *     and its unk73 quartet `KabuHitWall` / `KabuStartFall` / `KabuLand` /
 *     `KabuEnterWater` (`0x08741BB8`);
 *   * task #32's dispatcher `Task_HotHead` (`0x087414B4`, three rows);
 *   * nine scripts in the entry/hook shape: `KabuJumpInit`+`KabuJumpUpdate`
 *     (`0x08741390`/`0x0874139C`), `KabuTeleportInit`+`KabuTeleportUpdate`
 *     (`0x087413A8`/`0x087413B4`), `KabuSlideInit`+`KabuSlideUpdate`
 *     (`0x087413C0`/`0x087413C8`), `KabuIdleInit`+`KabuIdleUpdate`
 *     (`0x087413D0`/`0x087413D4`), `TwisterInit`+`TwisterUpdate`
 *     (`0x08741490`/`0x0874149C`), `TwisterIdleInit`+`TwisterIdleUpdate`
 *     (`0x087414A8`), `HotHeadWalkInit`+`HotHeadWalkUpdate` (`0x087414C0`),
 *     `HotHeadIdleInit`+`HotHeadIdleUpdate` (`0x087414D8`) and `HotHeadStandInit`+
 *     `HotHeadStandUpdate` (`0x087414E0`);
 *   * the bank's four-corner terrain probe `sub_0807f6a8` (four
 *     sub_08021c14 samples around a box whose six signed offsets come from
 *     Actor.terrainBox) and the jump-table state machine `TwisterState0Update`
 *     (five states over Task.unk30);
 *   * the six-frame flap loop `sub_08080b2c` and the "spawn a puff of six
 *     class-6 actors" routines `HotHeadWalkShoot` / `HotHeadStandShoot`.
 *
 * `KabuIdleEnterState`, `TwisterIdleEnterState` and `HotHeadIdleEnterState` are dead exports: each is
 * a copy of its host's tail dispatch that nothing in the ROM references
 * (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 TaskGetDxTo(u32 i);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, u32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);

void KabuJumpInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)KabuJumpUpdate;
    t->unk28 = -1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gKabuJumpStates);
}

void KabuJumpEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gKabuJumpStates);
}

void KabuJumpUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gKabuJumpStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void KabuJumpSpin(void)
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

    gCurTask->updateState = 0;
    a = gCurTask;
    if (a->unk28 != -1)
    {
        a->frame = 12;
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
                c->frame++;
                TaskYieldTrampoline(c->unk2C);
            } while (++*(s16 *)&gCurTask->unk6C <= 6);
        } while (gCurTask->unk30-- != 0);
    }
    d = gCurTask;
    d->frame = 12;
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

void KabuJumpSpinUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
}

void KabuJump(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 1;
    PlaySfx(188);
    gCurTask->onGround = 0;
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
            t->frame++;
            TaskYieldTrampoline(t->unk2C);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void KabuJumpState1Update(void)
{
    if ((s8)gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(0);
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
    }
}

void KabuJumpFall(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->updateState = 2;
    a = gCurTask;
    if (a->unk28 != -1)
        a->unk28 = 0;
    b = gCurTask;
    b->accelY = 0x2000;
    b->speedLimitY = 0x60000;
    b->unk2C = gUnk_08741318[b->unk74];
    while (1)
    {
        c = gCurTask;
        if (++c->frame > 11)
            c->frame = 4;
        TaskYieldTrampoline(gCurTask->unk2C);
    }
}

void KabuJumpFallUpdate(void)
{
    if ((s8)gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(0);
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
    }
}

void KabuTeleportInit(void)
{
    gCurTask->updateCallback = (u32)KabuTeleportUpdate;
    ActorSetHitReactions(gKabuTeleportHitReactions);
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gKabuTeleportStates);
}

void KabuTeleportEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gKabuTeleportStates);
}

void KabuTeleportUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == 2)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gKabuTeleportStateUpdates);
    }
    else
    {
        CallTableEntry(t->updateState, 3, gKabuTeleportStateUpdates);
    }
    if (gCurTask->state != 1)
        ActorCheckHits();
    ActorReactToHit();
}

void KabuTeleportSpin(void)
{
    struct Task *a;
    struct Task *b;

    gCurTask->updateState = 0;
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
            b->frame++;
            TaskYieldTrampoline(8);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void KabuTeleportSpinUpdate(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        gCurTask->unk28 = 0;
    t = gCurTask;
    if (--t->unk28 <= 0)
    {
        ActorSetState(1);
        TaskSetEntry(KabuTeleportEnterState, gCurTaskIdx);
    }
}

void KabuTeleport(void)
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

    gCurTask->updateState = 1;
    a = gCurTask;
    a->unk28 = 48;
    a->unk30 = 7;
    a->unk2C = a->frame;
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
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(60);
    o = &gTasks[TaskFindNearestPlayer()];
    *(s16 *)&gCurTask->unk6C = 0;
    while (1)
    {
        TaskYieldTrampoline(1);
        n = RandomRange(6);
        c = gCurTask;
        c->unk30 = o->pixelX + (s8)gUnk_08741367[n];
        m = RandomRange(9);
        d = gCurTask;
        d->unk34 = o->pixelY + (s8)gUnk_0874136D[m];
        if (sub_0807f6a8(d->unk30, d->unk34, (s8 *)d->u8C.actor->terrainBox) != 0)
            break;
        if (++*(s16 *)&gCurTask->unk6C > 59)
            break;
    }
    if (*(s16 *)&gCurTask->unk6C == 60)
        ActorDestroy();
    e = gCurTask;
    e->posX = e->unk30 << 16;
    e->posY = e->unk34 << 16;
    e->onGround = 0;
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

void KabuTeleportState1Update(void)
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
            t->frame = 0xFFFF;
        gCurTask->unk28--;
    }
    if (gCurTask->state != 1)
        TaskSetEntry(KabuTeleportEnterState, gCurTaskIdx);
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
    if (gCurTask->facing == 1)
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

void KabuTeleportState2(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;

    gCurTask->updateState = 2;
    a = gCurTask;
    a->unk2C = 4;
    while (1)
    {
        if ((s8)gCurTask->onGround == 0)
        {
            TaskSetMotionY(0, 0x2000, 0x60000);
            b = gCurTask;
            b->unk28 = b->unk2C;
            if ((s8)b->onGround == 0)
            {
                do
                {
                    c = gCurTask;
                    if (c->unk28-- == 0)
                    {
                        if (++c->frame > 11)
                            c->frame = 4;
                        gCurTask->unk28 = gCurTask->unk2C;
                    }
                    TaskYieldTrampoline(1);
                } while ((s8)gCurTask->onGround == 0);
            }
        }
        TaskStopY();
        d = gCurTask;
        d->frame = 12;
        TaskYieldTrampoline(gUnk_08741365[d->unk74]);
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        if ((s8)gCurTask->onGround != 0)
        {
            do
            {
                e = gCurTask;
                if (++e->frame > 11)
                    e->frame = 4;
                TaskYieldTrampoline(1);
            } while ((s8)gCurTask->onGround != 0);
        }
        gCurTask->unk2C = 1;
    }
}

void KabuTeleportState2Update(void)
{
}

void KabuSlideInit(void)
{
    gCurTask->updateCallback = (u32)KabuSlideUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gKabuSlideStates);
}

void KabuSlideEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gKabuSlideStates);
}

void KabuSlideUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == 1)
    {
        CallTableEntry(t->updateState, 2, gKabuSlideStateUpdates);
    }
    else if ((u8)ActorCollideTerrain() == 0)
    {
        CallTableEntry(gCurTask->updateState, 2, gKabuSlideStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void KabuSlide(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    struct Task *o;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk30 = 3;
    t->unk34 = 0;
    n = TaskFindNearestPlayer();
    u = gCurTask;
    u->unk28 = n;
    o = &gTasks[n];
    if (o->posX > u->posX)
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
        } while (gCurTask->frame > 10);
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(4);
        } while (gCurTask->frame <= 10);
    }
}

void KabuSlideState0Update(void)
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
    v->unk1C = o->pixelX + gUnk_08741378[v->unk2C];
    d = v->unk1C - v->pixelX;
    v->unk20 = d;
    if (v->unk18 == 0)
    {
        if (abs(d) <= 1)
            v->unk34 = 180;
    }
    w = gCurTask;
    if (w->unk20 >= 0)
        w->velX = w->velX + 0x2000;
    else
        w->velX = w->velX + 0xFFFFE000;
    x = gCurTask;
    if (abs(x->velX) > 0x2FFFF)
    {
        if (x->unk20 >= 0)
            x->velX = 0x10000;
        else
            x->velX = 0xFFFF0000;
    }
    if (gCurTask->unk30 == 0)
    {
        ActorSetState(1);
        TaskSetEntry(KabuSlideEnterState, gCurTaskIdx);
    }
}

void KabuSlideState1(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    TaskStop();
    gCurTask->frame = 12;
    TaskYieldTrampoline(15);
    CreateBurstEffect(1, 0);
    gCurTask->velY = 0xFFFC0000;
loop:
l1:
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    if (gCurTask->frame > 10)
        goto l1;
    do
    {
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(1);
    } while (gCurTask->frame <= 10);
    goto loop;
}

void KabuSlideState1Update(void)
{
}

void KabuIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)KabuIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gKabuIdleStates);
}

void KabuIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gKabuIdleStates);
}

void KabuIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gKabuIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void KabuIdle(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(5);
        } while (++*(s16 *)&gCurTask->unk6C <= 6);
    }
}

void KabuIdleState0Update(void)
{
}

s32 KabuHitWall(void)
{
    struct Task *t;
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
    case 2:
    case 3:
        t = gCurTask;
        t->velX = -t->velX;
        break;
    case 1:
        RequestScreenShake(1);
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 KabuStartFall(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(2);
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
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

s32 KabuLand(void)
{
    switch (gCurTask->variant)
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

s32 KabuHitCeiling(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 0)
        t->velY = 0;
    return 0;
}

s32 KabuEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Twister(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gTwisterFrames;
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
    CallTableEntry(gCurTask->variant, 2, gTwisterVariants);
}

void TwisterInit(void)
{
    gCurTask->updateCallback = (u32)TwisterUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gTwisterStates);
}

void TwisterEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gTwisterStates);
}

void TwisterUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gTwisterStateUpdates);
    }
    else
    {
        CallTableEntry(t->updateState, 3, gTwisterStateUpdates);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void TwisterState0(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    ActorSetAttackBox(gUnk_0873F720);
    t = gCurTask;
    if ((s8)t->onGround == 0)
    {
        t->accelY = 0x1500;
        t->speedLimitY = 0x30000;
    }
    while (gCurTask->unk30 != 4)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void TwisterState0Update(void)
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
            TwisterTickSound(gCurTask->unk34, 10);
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
            TwisterTickSound(1, 10);
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
            TwisterTickSound(1, 10);
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
            TwisterTickSound(gCurTask->unk34, 10);
        }
        g = gCurTask;
        if ((--g->unk2C & 28) == 0)
        {
            ActorSetState(1);
            TaskSetEntry(TwisterEnterState, gCurTaskIdx);
        }
        break;
    }
}

void TwisterState1(void)
{
    struct Task *t;
    s32 n;

    gCurTask->updateState = 1;
    ActorSetAttackBox(gUnk_08741ADC);
    gCurTask->unk30 = -1;
    gCurTask->unk34 = ActorStartAnim(gUnk_08741454);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->velY = 0xFFFF8000;
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

void TwisterState1Update(void)
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
        TwisterTickSound(gCurTask->unk34, 8);
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
            TaskSetEntry(TwisterEnterState, gCurTaskIdx);
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
    p = (u16 *)&o->pixelX;
    gCurTask->unk1C =
        (u16)ArcTan2((s8)gUnk_087413DA[w->unk24] + *p - *(u16 *)&w->pixelX,
                     *(u16 *)&o->pixelY - *(u16 *)&w->pixelY)
        >> 7;
    if (gCurTask->unk1C > 255)
        gCurTask->velY = 0xFFFFC000;
    else
        gCurTask->velY = 0x4000;
    x = gCurTask;
    if (x->unk1C >= 128 && x->unk1C <= 383)
    {
        x->unk1C = x->velX;
        x->velX = x->unk1C - gUnk_087413DC[x->unk74];
    }
    else
    {
        x->unk1C = x->velX;
        x->velX = x->unk1C + gUnk_087413DC[x->unk74];
    }
    x = gCurTask;
    n = x->velX;
    if (abs(n) >= gUnk_087413E4[x->unk74])
        x->velX = x->unk1C;
}

void TwisterState2(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;

    gCurTask->updateState = 2;
    a = gCurTask;
    a->velX = 0;
    a->accelY = 0xFFFF1900;
    while (gCurTask->velY > -0x20000)
        TaskYieldTrampoline(1);
    b = gCurTask;
    b->velY = -0x20000;
    b->accelY = 0;
    if (b->unk28++ == 0)
    {
        while (1)
        {
            c = gCurTask;
            if ((u8)sub_0807f6a8(c->pixelX, c->pixelY, (s8 *)c->u8C.actor->terrainBox) != 0)
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

void TwisterState2Update(void)
{
    struct Task *t = gCurTask;

    if (--t->unk34 == 0)
    {
        gCurTask->unk34 = ActorStepAnim();
        TwisterTickSound(gCurTask->unk34, 8);
    }
    if (gCurTask->state != 2)
        TaskSetEntry(TwisterEnterState, gCurTaskIdx);
}

void TwisterIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)TwisterIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gTwisterIdleStates);
}

void TwisterIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gTwisterIdleStates);
}

void TwisterIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gTwisterIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void TwisterIdle(void)
{
    gCurTask->updateState = 0;
    TaskSleepForever();
}

void TwisterIdleState0Update(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void TwisterTickSound(s32 a, s32 b)
{
    struct Task *t = gCurTask;

    t->unk18 -= a;
    if (t->unk18 <= 0)
    {
        t->unk18 = b;
        PlaySfx(192);
    }
}

s32 TwisterLand(void)
{
    TaskStopY();
    return 0;
}

s32 TwisterStartFall(void)
{
    TaskSetMotionY(0, 0x1500, 0x30000);
    return 0;
}

void TwisterHitCeiling(void)
{
    gCurTask->velY = 0;
}

s32 TwisterEnterWater(void)
{
    switch (gCurTask->state)
    {
    case 0:
        ActorStartDrown(-2);
        break;
    case 1:
    case 2:
        ActorStartDrown(-2);
        break;
    }
    return 1;
}

void Task_HotHead(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gHotHeadFrames;
    CreateChildTaskHere(175, 0);
    CallTableEntry(gCurTask->variant, 3, gHotHeadVariants);
}

void HotHeadWalkInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)HotHeadWalkUpdate;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    if (RandomRange(4) == 0)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gHotHeadWalkStates);
}

void HotHeadWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gHotHeadWalkStates);
}

void HotHeadWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gHotHeadWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void HotHeadWalk(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk28 = 90;
    t->unk2C = 0;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_08080b2c();
}

void HotHeadWalkState0Update(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (RandomRange(gUnk_087414B0[t->unk2C]) != 0)
        {
            ActorSetState(1);
            TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void HotHeadWalkShoot(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.subtype = 6;
        sp.taskType = 108;
        sp.variant = 0;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 0;
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            *(s16 *)&gCurTask->unk6E = 0;
            do
            {
                gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->unk6E <= 4);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    else
    {
        sp.subtype = 6;
        sp.taskType = 108;
        sp.variant = 1;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(0);
    TaskSleepForever();
}

void HotHeadWalkShootUpdate(void)
{
    struct PlayerState *p = &gPlayerStates[TaskFindNearestPlayer()];

    if (*(s8 *)&p->pendingAbility == 1 || p->ability == 1)
    {
        TaskSetFrame(*(s16 *)&gCurTask->unk30);
        ActorSetState(0);
    }
    if (gCurTask->state != 1)
        TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
}

void HotHeadWalkFall(void)
{
    gCurTask->updateState = 2;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08080b2c();
}

void HotHeadWalkFallUpdate(void)
{
}

void HotHeadIdleInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)HotHeadIdleUpdate;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gHotHeadIdleStates);
}

void HotHeadIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gHotHeadIdleStates);
}

void HotHeadIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gHotHeadIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void HotHeadIdle(void)
{
    gCurTask->updateState = 0;
    sub_08080b2c();
}

void HotHeadIdleState0Update(void)
{
}

void HotHeadStandInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)HotHeadStandUpdate;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk30 = 6;
    t->unk34 = -1;
    if (RandomRange(4) == 0)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gHotHeadStandStates);
}

void HotHeadStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gHotHeadStandStates);
}

void HotHeadStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gHotHeadStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void HotHeadStandWait(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk28 = 90;
    t->unk2C = 0;
    sub_08080b2c();
}

void HotHeadStandWaitUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->unk28 == 0)
    {
        if (RandomRange(gUnk_087414B0[t->unk2C]) != 0)
        {
            ActorSetState(1);
            TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->unk28 = 60;
            u->unk2C = 1;
        }
    }
}

void HotHeadStandShoot(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.subtype = 6;
        sp.taskType = 108;
        sp.variant = 0;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 0;
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            *(s16 *)&gCurTask->unk6E = 0;
            do
            {
                gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->unk6E <= 4);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    else
    {
        sp.subtype = 6;
        sp.taskType = 108;
        sp.variant = 1;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 1;
        gCurTask->unk46 = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(0);
    TaskSleepForever();
}

void HotHeadStandShootUpdate(void)
{
    struct PlayerState *p = &gPlayerStates[TaskFindNearestPlayer()];

    if (*(s8 *)&p->pendingAbility == 1 || p->ability == 1)
    {
        TaskSetFrame(*(s16 *)&gCurTask->unk30);
        ActorSetState(0);
    }
    if (gCurTask->state != 1)
        TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
}

void HotHeadStandFall(void)
{
    gCurTask->updateState = 2;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08080b2c();
}

void HotHeadStandFallUpdate(void)
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
