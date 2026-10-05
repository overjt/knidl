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
    t->kabuJumpIndex = -1;
    ActorSetState(KABU_JUMP_STATE_SPIN);
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

    gCurTask->updateState = KABU_JUMP_STATE_SPIN;
    a = gCurTask;
    if (a->kabuJumpIndex != -1)
    {
        a->frame = 12;
        TaskYieldTrampoline(15);
    }
    b = gCurTask;
    b->kabuFrameDelay = gKabuFrameDelays[b->actorSpawnArg];
    n = RandomRange(3);
    k = gCurTask;
    m = n + 1;
    k->kabuSpinCount = n;
    if (m != 0)
    {
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(gCurTask->kabuFrameDelay);
            *(s16 *)&gCurTask->kabuLoopCount = 0;
            do
            {
                c = gCurTask;
                c->frame++;
                TaskYieldTrampoline(c->kabuFrameDelay);
            } while (++*(s16 *)&gCurTask->kabuLoopCount <= 6);
        } while (gCurTask->kabuSpinCount-- != 0);
    }
    d = gCurTask;
    d->frame = 12;
    TaskYieldTrampoline(gUnk_0874131A[d->actorSpawnArg]);
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
        *(s16 *)&e->kabuLoopCount = z;
    inner:
        f = *g;
        p = (s16 *)&f->kabuLoopCount;
        q = *p;
        q += f->actorSpawnArg * 5;
        f->unk18 += *(u8 *)(q + (s32)tbl);
        if (f->unk18 <= f->unk1C)
        {
            if (++*p <= 4)
                goto inner;
        }
        r = *g;
    } while (r->kabuJumpIndex == *(s16 *)&r->kabuLoopCount);
    j = *h;
    j->kabuJumpIndex = *(s16 *)&j->kabuLoopCount;
    ActorSetState(KABU_JUMP_STATE_JUMP);
    TaskSleepForever();
}

void KabuJumpSpinUpdate(void)
{
    if (gCurTask->state != KABU_JUMP_STATE_SPIN)
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
}

void KabuJump(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = KABU_JUMP_STATE_JUMP;
    PlaySfx(188);
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->kabuFrameDelay = gUnk_08741350[u->kabuJumpIndex];
    TaskSetMotionXFacing(gUnk_08741328[u->kabuJumpIndex], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_0874133C[gCurTask->kabuJumpIndex], 0x2000, 0x60000);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gCurTask->kabuFrameDelay);
        *(s16 *)&gCurTask->kabuLoopCount = 0;
        do
        {
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(t->kabuFrameDelay);
        } while (++*(s16 *)&gCurTask->kabuLoopCount <= 6);
    }
}

void KabuJumpState1Update(void)
{
    if ((s8)gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(KABU_JUMP_STATE_SPIN);
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
    }
}

void KabuJumpFall(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->updateState = KABU_JUMP_STATE_FALL;
    a = gCurTask;
    if (a->kabuJumpIndex != -1)
        a->kabuJumpIndex = 0;
    b = gCurTask;
    b->accelY = 0x2000;
    b->speedLimitY = 0x60000;
    b->kabuFrameDelay = gKabuFrameDelays[b->actorSpawnArg];
    while (1)
    {
        c = gCurTask;
        if (++c->frame > 11)
            c->frame = 4;
        TaskYieldTrampoline(gCurTask->kabuFrameDelay);
    }
}

void KabuJumpFallUpdate(void)
{
    if ((s8)gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(KABU_JUMP_STATE_SPIN);
        TaskSetEntry(KabuJumpEnterState, gCurTaskIdx);
    }
}

void KabuTeleportInit(void)
{
    gCurTask->updateCallback = (u32)KabuTeleportUpdate;
    ActorSetHitReactions(gKabuTeleportHitReactions);
    ActorSetState(KABU_TELEPORT_STATE_SPIN);
    CallTableEntry(gCurTask->state, 3, gKabuTeleportStates);
}

void KabuTeleportEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gKabuTeleportStates);
}

void KabuTeleportUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == KABU_TELEPORT_STATE_2)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 3, gKabuTeleportStateUpdates);
    }
    else
    {
        CallTableEntry(t->updateState, 3, gKabuTeleportStateUpdates);
    }
    if (gCurTask->state != KABU_TELEPORT_STATE_TELEPORT)
        ActorCheckHits();
    ActorReactToHit();
}

void KabuTeleportSpin(void)
{
    struct Task *a;
    struct Task *b;

    gCurTask->updateState = KABU_TELEPORT_STATE_SPIN;
    a = gCurTask;
    a->kabuTeleportTimer = gUnk_08741355[a->actorSpawnArg];
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        *(s16 *)&gCurTask->kabuLoopCount = 0;
        do
        {
            b = gCurTask;
            b->frame++;
            TaskYieldTrampoline(8);
        } while (++*(s16 *)&gCurTask->kabuLoopCount <= 6);
    }
}

void KabuTeleportSpinUpdate(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        gCurTask->kabuTeleportTimer = 0;
    t = gCurTask;
    if (--t->kabuTeleportTimer <= 0)
    {
        ActorSetState(KABU_TELEPORT_STATE_TELEPORT);
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

    gCurTask->updateState = KABU_TELEPORT_STATE_TELEPORT;
    a = gCurTask;
    a->kabuBlinkTimer = 48;
    a->kabuBlinkRow = 7;
    a->kabuBlinkFrame = a->frame;
    a->kabuBlinkStepTimer = 4;
    do
    {
        b = gCurTask;
        if (b->kabuBlinkStepTimer-- == 0)
        {
            if (++b->kabuBlinkFrame > 11)
                b->kabuBlinkFrame = 4;
            gCurTask->kabuBlinkStepTimer = 4;
        }
        TaskYieldTrampoline(1);
    } while (gCurTask->kabuBlinkTimer != 0);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(60);
    o = &gTasks[TaskFindNearestPlayer()];
    *(s16 *)&gCurTask->kabuLoopCount = 0;
    while (1)
    {
        TaskYieldTrampoline(1);
        n = RandomRange(6);
        c = gCurTask;
        c->kabuTeleportX = o->pixelX + (s8)gUnk_08741367[n];
        m = RandomRange(9);
        d = gCurTask;
        d->kabuTeleportY = o->pixelY + (s8)gUnk_0874136D[m];
        if (sub_0807f6a8(d->kabuTeleportX, d->kabuTeleportY, (s8 *)d->u8C.actor->terrainBox) != 0)
            break;
        if (++*(s16 *)&gCurTask->kabuLoopCount > 59)
            break;
    }
    if (*(s16 *)&gCurTask->kabuLoopCount == 60)
        ActorDestroy();
    e = gCurTask;
    e->posX = e->kabuTeleportX << 16;
    e->posY = e->kabuTeleportY << 16;
    e->onGround = 0;
    f = gCurTask;
    f->kabuBlinkTimer = 48;
    f->kabuBlinkRow = 0;
    f->kabuBlinkStepTimer = 4;
    do
    {
        b = gCurTask;
        if (b->kabuBlinkStepTimer-- == 0)
        {
            if (++b->kabuBlinkFrame > 11)
                b->kabuBlinkFrame = 4;
            gCurTask->kabuBlinkStepTimer = 4;
        }
        TaskYieldTrampoline(1);
    } while (gCurTask->kabuBlinkTimer != 0);
    ActorSetState(KABU_TELEPORT_STATE_2);
    TaskSleepForever();
}

void KabuTeleportState1Update(void)
{
    struct Task *t = gCurTask;
    s32 n;
    s32 m;

    n = t->kabuBlinkTimer;
    if (n != 0)
    {
        m = gUnk_08741357[(n >> 3) + t->kabuBlinkRow];
        t->unk18 = m;
        t->unk1C = 1 << (n & 7);
        if ((m & t->unk1C) != 0)
            TaskSetFrame(*(s16 *)&t->kabuBlinkFrame);
        else
            t->frame = 0xFFFF;
        gCurTask->kabuBlinkTimer--;
    }
    if (gCurTask->state != KABU_TELEPORT_STATE_TELEPORT)
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

    gCurTask->updateState = KABU_TELEPORT_STATE_2;
    a = gCurTask;
    a->kabuFrameDelay = 4;
    while (1)
    {
        if ((s8)gCurTask->onGround == 0)
        {
            TaskSetMotionY(0, 0x2000, 0x60000);
            b = gCurTask;
            b->kabuFrameTimer = b->kabuFrameDelay;
            if ((s8)b->onGround == 0)
            {
                do
                {
                    c = gCurTask;
                    if (c->kabuFrameTimer-- == 0)
                    {
                        if (++c->frame > 11)
                            c->frame = 4;
                        gCurTask->kabuFrameTimer = gCurTask->kabuFrameDelay;
                    }
                    TaskYieldTrampoline(1);
                } while ((s8)gCurTask->onGround == 0);
            }
        }
        TaskStopY();
        d = gCurTask;
        d->frame = 12;
        TaskYieldTrampoline(gUnk_08741365[d->actorSpawnArg]);
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
        gCurTask->kabuFrameDelay = 1;
    }
}

void KabuTeleportState2Update(void)
{
}

void KabuSlideInit(void)
{
    gCurTask->updateCallback = (u32)KabuSlideUpdate;
    ActorSetState(KABU_SLIDE_STATE_SLIDE);
    CallTableEntry(gCurTask->state, 2, gKabuSlideStates);
}

void KabuSlideEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gKabuSlideStates);
}

void KabuSlideUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == KABU_SLIDE_STATE_1)
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

    gCurTask->updateState = KABU_SLIDE_STATE_SLIDE;
    t = gCurTask;
    t->kabuPassCount = 3;
    t->kabuTurnTimer = 0;
    n = TaskFindNearestPlayer();
    u = gCurTask;
    u->kabuPlayerSlot = n;
    o = &gTasks[n];
    if (o->posX > u->posX)
        u->kabuSlideSide = 0;
    else
        u->kabuSlideSide = 1;
    TaskStopX();
    while (1)
    {
        do
        {
            TaskFaceToward(gCurTask->kabuPlayerSlot);
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
    n = t->kabuTurnTimer;
    if (n != 0)
    {
        t->unk18 = 1;
        t->kabuTurnTimer = n - 1;
        switch (t->kabuTurnTimer)
        {
        case 0:
        case 60:
        case 120:
            u = gCurTask;
            u->kabuPassCount--;
            u->kabuTurnTimer = 0;
            u->kabuSlideSide ^= 1;
            break;
        }
    }
    v = gCurTask;
    o = &gTasks[v->kabuPlayerSlot];
    v->unk1C = o->pixelX + gUnk_08741378[v->kabuSlideSide];
    d = v->unk1C - v->pixelX;
    v->unk20 = d;
    if (v->unk18 == 0)
    {
        if (abs(d) <= 1)
            v->kabuTurnTimer = 180;
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
    if (gCurTask->kabuPassCount == 0)
    {
        ActorSetState(KABU_SLIDE_STATE_1);
        TaskSetEntry(KabuSlideEnterState, gCurTaskIdx);
    }
}

void KabuSlideState1(void)
{
    struct Task *t;

    gCurTask->updateState = KABU_SLIDE_STATE_1;
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
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(KABU_IDLE_STATE_IDLE);
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

    gCurTask->updateState = KABU_IDLE_STATE_IDLE;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        *(s16 *)&gCurTask->kabuLoopCount = 0;
        do
        {
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(5);
        } while (++*(s16 *)&gCurTask->kabuLoopCount <= 6);
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
        ActorSetState(KABU_JUMP_STATE_FALL);
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
    u->twisterAscentCount = 0;
    u->twisterPhaseTimer = 1;
    u->twisterSoundTimer = 0;
    ActorStartAnim(gUnk_087413EC);
    if (RandomRange(4) != 0)
        gCurTask->twisterPhase = 1;
    else
        gCurTask->twisterPhase = 3;
    CallTableEntry(gCurTask->variant, 2, gTwisterVariants);
}

void TwisterInit(void)
{
    gCurTask->updateCallback = (u32)TwisterUpdate;
    ActorSetState(TWISTER_STATE_0);
    CallTableEntry(gCurTask->state, 3, gTwisterStates);
}

void TwisterEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gTwisterStates);
}

void TwisterUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state == TWISTER_STATE_0)
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

    gCurTask->updateState = TWISTER_STATE_0;
    ActorSetAttackBox(gUnk_0873F720);
    t = gCurTask;
    if ((s8)t->onGround == 0)
    {
        t->accelY = 0x1500;
        t->speedLimitY = 0x30000;
    }
    while (gCurTask->twisterPhase != 4)
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

    switch (gCurTask->twisterPhase)
    {
    case 0:
        a = gCurTask;
        if (--a->actorAnimDelay34 == 0)
        {
            gCurTask->actorAnimDelay34 = ActorStepAnim();
            TwisterTickSound(gCurTask->actorAnimDelay34, 10);
        }
        b = gCurTask;
        if ((--b->twisterPhaseTimer & 28) == 0)
        {
            b->twisterPhase++;
            b->twisterPhaseTimer = 23;
            b->actorAnimDelay34 = 1;
            ActorStartAnim(gUnk_087413EC);
        }
        break;
    case 1:
        c = gCurTask;
        n = c->twisterPhaseTimer - 1;
        c->twisterPhaseTimer = n;
        if (n != 0)
        {
            if (--c->actorAnimDelay34 > 0)
                break;
            c->actorAnimDelay34 = ~(n >> 3) & 3;
            ActorStepAnim();
            TwisterTickSound(1, 10);
        }
        else
        {
            c->twisterPhase++;
            c->twisterPhaseTimer = gUnk_087413D8[c->actorSpawnArg];
            c->actorAnimDelay34 = 1;
            TaskFaceNearestPlayer();
        }
        break;
    case 2:
        d = gCurTask;
        if (--d->twisterPhaseTimer != 0)
        {
            if (--d->actorAnimDelay34 == 0)
                gCurTask->actorAnimDelay34 = ActorStepAnim();
        }
        else
        {
            d->twisterPhase++;
            d->twisterPhaseTimer = 31;
            d->actorAnimDelay34 = 1;
        }
        break;
    case 3:
        e = gCurTask;
        m = e->twisterPhaseTimer - 1;
        e->twisterPhaseTimer = m;
        if ((28 & m) != 0)
        {
            if (--e->actorAnimDelay34 > 0)
                break;
            e->actorAnimDelay34 = m >> 3;
            ActorStepAnim();
            TwisterTickSound(1, 10);
        }
        else
        {
            e->twisterPhase++;
            e->twisterPhaseTimer = 31;
            e->actorAnimDelay34 = 1;
            ActorStartAnim(gUnk_08741420);
        }
        break;
    case 4:
        f = gCurTask;
        if (--f->actorAnimDelay34 == 0)
        {
            gCurTask->actorAnimDelay34 = ActorStepAnim();
            TwisterTickSound(gCurTask->actorAnimDelay34, 10);
        }
        g = gCurTask;
        if ((--g->twisterPhaseTimer & 28) == 0)
        {
            ActorSetState(TWISTER_STATE_1);
            TaskSetEntry(TwisterEnterState, gCurTaskIdx);
        }
        break;
    }
}

void TwisterState1(void)
{
    struct Task *t;
    s32 n;

    gCurTask->updateState = TWISTER_STATE_1;
    ActorSetAttackBox(gUnk_08741ADC);
    gCurTask->twisterPhase = -1;
    gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08741454);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->velY = 0xFFFF8000;
    *(s16 *)&t->twisterLoopCount = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (++*(s16 *)&gCurTask->twisterLoopCount <= 39);
    gCurTask->twisterPhase++;
    n = TaskFindNearestPlayer();
    gCurTask->twisterPlayerSlot = n;
    if (TaskGetDxTo(n) > 0)
        gCurTask->twisterTargetSide = 1;
    else
        gCurTask->twisterTargetSide = 0;
    TaskStopY();
    gCurTask->twisterPhaseTimer = 120;
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
    if (--t->actorAnimDelay34 == 0)
    {
        gCurTask->actorAnimDelay34 = ActorStepAnim();
        TwisterTickSound(gCurTask->actorAnimDelay34, 8);
    }
    t = gCurTask;
    m = t->twisterPhase;
    if (m == -1)
        return;
    if (--t->twisterPhaseTimer == 0)
    {
        if (m == 2)
        {
            ActorSetState(TWISTER_STATE_2);
            TaskSetEntry(TwisterEnterState, gCurTaskIdx);
            return;
        }
        if (abs(TaskGetDyTo(gCurTask->twisterPlayerSlot)) <= 23)
        {
            u = gCurTask;
            u->twisterTargetSide ^= 1;
            u->twisterPhase = 2;
            u->twisterPhaseTimer = 120;
        }
        else
        {
            v = gCurTask;
            v->twisterPhase++;
            v->twisterPhaseTimer = 40;
        }
    }
    if ((gFrameCount & 7) != 0)
        return;
    w = gCurTask;
    o = &gTasks[w->twisterPlayerSlot];
    p = (u16 *)&o->pixelX;
    gCurTask->unk1C =
        (u16)ArcTan2((s8)gUnk_087413DA[w->twisterTargetSide] + *p - *(u16 *)&w->pixelX,
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
        x->velX = x->unk1C - gUnk_087413DC[x->actorSpawnArg];
    }
    else
    {
        x->unk1C = x->velX;
        x->velX = x->unk1C + gUnk_087413DC[x->actorSpawnArg];
    }
    x = gCurTask;
    n = x->velX;
    if (abs(n) >= gUnk_087413E4[x->actorSpawnArg])
        x->velX = x->unk1C;
}

void TwisterState2(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;

    gCurTask->updateState = TWISTER_STATE_2;
    a = gCurTask;
    a->velX = 0;
    a->accelY = 0xFFFF1900;
    while (gCurTask->velY > -0x20000)
        TaskYieldTrampoline(1);
    b = gCurTask;
    b->velY = -0x20000;
    b->accelY = 0;
    if (b->twisterAscentCount++ == 0)
    {
        while (1)
        {
            c = gCurTask;
            if ((u8)sub_0807f6a8(c->pixelX, c->pixelY, (s8 *)c->u8C.actor->terrainBox) != 0)
                break;
            TaskYieldTrampoline(1);
        }
        d = gCurTask;
        d->twisterPhaseTimer = 31;
        d->twisterPhase = 0;
        d->actorAnimDelay34 = 1;
        ActorStartAnim(gUnk_08741420);
        ActorSetState(TWISTER_STATE_0);
    }
    TaskSleepForever();
}

void TwisterState2Update(void)
{
    struct Task *t = gCurTask;

    if (--t->actorAnimDelay34 == 0)
    {
        gCurTask->actorAnimDelay34 = ActorStepAnim();
        TwisterTickSound(gCurTask->actorAnimDelay34, 8);
    }
    if (gCurTask->state != TWISTER_STATE_2)
        TaskSetEntry(TwisterEnterState, gCurTaskIdx);
}

void TwisterIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)TwisterIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(TWISTER_IDLE_STATE_IDLE);
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
    gCurTask->updateState = TWISTER_IDLE_STATE_IDLE;
    TaskSleepForever();
}

void TwisterIdleState0Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void TwisterTickSound(s32 a, s32 b)
{
    struct Task *t = gCurTask;

    t->twisterSoundTimer -= a;
    if (t->twisterSoundTimer <= 0)
    {
        t->twisterSoundTimer = b;
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
    CreateChildTaskHere(TASK_HOT_HEAD_FLAME, 0);
    CallTableEntry(gCurTask->variant, 3, gHotHeadVariants);
}

void HotHeadWalkInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)HotHeadWalkUpdate;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->hotHeadLoopFrame = 6;
    t->hotHeadFrameStep = -1;
    if (RandomRange(4) == 0)
        ActorSetState(HOT_HEAD_WALK_STATE_SHOOT);
    else
        ActorSetState(HOT_HEAD_WALK_STATE_WALK);
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

    gCurTask->updateState = HOT_HEAD_WALK_STATE_WALK;
    t = gCurTask;
    t->hotHeadShotTimer = 90;
    t->hotHeadOddsIndex = 0;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_08080b2c();
}

void HotHeadWalkState0Update(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->hotHeadShotTimer == 0)
    {
        if (RandomRange(gHotHeadShootOdds[t->hotHeadOddsIndex]) != 0)
        {
            ActorSetState(HOT_HEAD_WALK_STATE_SHOOT);
            TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->hotHeadShotTimer = 60;
            u->hotHeadOddsIndex = 1;
        }
    }
}

void HotHeadWalkShoot(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = HOT_HEAD_WALK_STATE_SHOOT;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.subtype = 6;
        sp.taskType = TASK_HOT_HEAD_FIRE;
        sp.variant = HOT_HEAD_FIRE_VARIANT_BREATH;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 0;
        *(s16 *)&gCurTask->hotHeadLoopCount = 0;
        do
        {
            *(s16 *)&gCurTask->hotHeadFlameIndex = 0;
            do
            {
                gCurTask->hotHeadFireSlot = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->hotHeadFlameIndex <= 4);
        } while (++*(s16 *)&gCurTask->hotHeadLoopCount <= 5);
    }
    else
    {
        sp.subtype = 6;
        sp.taskType = TASK_HOT_HEAD_FIRE;
        sp.variant = HOT_HEAD_FIRE_VARIANT_BALL;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        gCurTask->hotHeadFireSlot = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->hotHeadLoopCount = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->hotHeadLoopCount <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(HOT_HEAD_WALK_STATE_WALK);
    TaskSleepForever();
}

void HotHeadWalkShootUpdate(void)
{
    struct PlayerState *p = &gPlayerStates[TaskFindNearestPlayer()];

    if (*(s8 *)&p->pendingAbility == ABILITY_FIRE || p->ability == ABILITY_FIRE)
    {
        TaskSetFrame(*(s16 *)&gCurTask->hotHeadLoopFrame);
        ActorSetState(HOT_HEAD_WALK_STATE_WALK);
    }
    if (gCurTask->state != HOT_HEAD_WALK_STATE_SHOOT)
        TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
}

void HotHeadWalkFall(void)
{
    gCurTask->updateState = HOT_HEAD_WALK_STATE_FALL;
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
    t->hotHeadLoopFrame = 6;
    t->hotHeadFrameStep = -1;
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(HOT_HEAD_IDLE_STATE_IDLE);
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
    gCurTask->updateState = HOT_HEAD_IDLE_STATE_IDLE;
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
    t->hotHeadLoopFrame = 6;
    t->hotHeadFrameStep = -1;
    if (RandomRange(4) == 0)
        ActorSetState(HOT_HEAD_STAND_STATE_SHOOT);
    else
        ActorSetState(HOT_HEAD_STAND_STATE_WAIT);
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

    gCurTask->updateState = HOT_HEAD_STAND_STATE_WAIT;
    t = gCurTask;
    t->hotHeadShotTimer = 90;
    t->hotHeadOddsIndex = 0;
    sub_08080b2c();
}

void HotHeadStandWaitUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (--t->hotHeadShotTimer == 0)
    {
        if (RandomRange(gHotHeadShootOdds[t->hotHeadOddsIndex]) != 0)
        {
            ActorSetState(HOT_HEAD_STAND_STATE_SHOOT);
            TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
        }
        else
        {
            u = gCurTask;
            u->hotHeadShotTimer = 60;
            u->hotHeadOddsIndex = 1;
        }
    }
}

void HotHeadStandShoot(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = HOT_HEAD_STAND_STATE_SHOOT;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    if (abs(TaskGetNearestPlayerDistSq()) <= 0x143F)
    {
        sp.subtype = 6;
        sp.taskType = TASK_HOT_HEAD_FIRE;
        sp.variant = HOT_HEAD_FIRE_VARIANT_BREATH;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 0;
        *(s16 *)&gCurTask->hotHeadLoopCount = 0;
        do
        {
            *(s16 *)&gCurTask->hotHeadFlameIndex = 0;
            do
            {
                gCurTask->hotHeadFireSlot = CreateActorFromDescHere(&sp, 1);
                TaskSetFrame(10);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++*(s16 *)&gCurTask->hotHeadFlameIndex <= 4);
        } while (++*(s16 *)&gCurTask->hotHeadLoopCount <= 5);
    }
    else
    {
        sp.subtype = 6;
        sp.taskType = TASK_HOT_HEAD_FIRE;
        sp.variant = HOT_HEAD_FIRE_VARIANT_BALL;
        sp.spawnArg = 0;
        sp.tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
        sp.checkTerrain = 1;
        gCurTask->hotHeadFireSlot = CreateActorFromDescHere(&sp, 1);
        *(s16 *)&gCurTask->hotHeadLoopCount = 0;
        do
        {
            TaskSetFrame(10);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->hotHeadLoopCount <= 5);
    }
    TaskStop();
    TaskYieldTrampoline(15);
    ActorSetState(HOT_HEAD_STAND_STATE_WAIT);
    TaskSleepForever();
}

void HotHeadStandShootUpdate(void)
{
    struct PlayerState *p = &gPlayerStates[TaskFindNearestPlayer()];

    if (*(s8 *)&p->pendingAbility == ABILITY_FIRE || p->ability == ABILITY_FIRE)
    {
        TaskSetFrame(*(s16 *)&gCurTask->hotHeadLoopFrame);
        ActorSetState(HOT_HEAD_STAND_STATE_WAIT);
    }
    if (gCurTask->state != HOT_HEAD_STAND_STATE_SHOOT)
        TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
}

void HotHeadStandFall(void)
{
    gCurTask->updateState = HOT_HEAD_STAND_STATE_FALL;
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
        TaskSetFrame(*(s16 *)&gCurTask->hotHeadLoopFrame);
        if (gCurTask->hotHeadLoopFrame == 6)
            TaskYieldTrampoline(7);
        else
            TaskYieldTrampoline(8);
        t = gCurTask;
        t->hotHeadLoopFrame += t->hotHeadFrameStep;
        if (t->hotHeadLoopFrame == 4 || t->hotHeadLoopFrame == 8)
            t->hotHeadFrameStep = -t->hotHeadFrameStep;
    }
}
