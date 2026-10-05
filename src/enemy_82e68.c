/* game_code_and_rodata 0x08082E68-0x080844C4 (issue #69, module M22 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08082E68 0x080844C4 src/enemy_82e68.c --newpb
 *
 * M22 is a bank of five enemy/object behaviour scripts, all built to the same
 * three-table pattern (rom-map section 9):
 *
 *   entry     -> installs Task.updateCallback (the per-frame hook) and hands
 *                Task.variant / Task.state to CallTableEntry, which indexes the
 *                script's table;
 *   unk14 table -> the coroutine BODIES: each sets Task.updateState to its own state
 *                number and then runs a chain of TaskYieldTrampoline waits;
 *   unk15 table -> the per-frame HANDLERS: each is the six-instruction guard
 *                `if (Task.state != N) TaskSetEntry(entry, gCurTaskIdx);` that
 *                re-arms the entry whenever the requested state changes.
 *
 * The three tables of one script sit consecutively in ROM, so the entry's
 * `count` argument is what separates them (`0x08741778` + 7*4 = `0x08741794`).
 *
 * This batch holds:
 *   * Task_Flamer's rows 0/1 `FlamerInit` (7 states, tables `0x08741778` /
 *     `0x08741794`, per-frame hook `FlamerUpdate`, re-arm `FlamerEnterState`);
 *   * its terrain library: `FlamerGetSurfaceSlopeInDir` / `FlamerGetSurfaceSlopeOnSide` / `sub_08083bbc` /
 *     `FlamerGetSurfaceSlopeAt` probe the room with GetCollisionTileAtPixel/GetCollisionTileAtOffset and turn the
 *     `gCollisionTileCollides` / `gCollisionTileSlope` / `gUnk_087416A4` index chain into a
 *     tile class, `FlamerSetCrawlVelocity` turns a direction code into an aim angle plus
 *     a 16.16 velocity through AngleToVector, and `sub_08083dfc` is the
 *     five-times-four-frame animation wait;
 *   * the one-state script `FlamerIdleInit` (`0x087417B0` / `0x087417B4`);
 *   * the class-2 task #105 script `Task_SirKibbleCutter` (`0x08741E64` /
 *     `0x08741E68` / `0x08741E6C`);
 *   * the class-2 task #108 script `Task_HotHeadFire`, whose unk73 table
 *     `0x08741E7C` has two rows (`0x08741E84`/`0x08741E88` and
 *     `0x08741E8C`/`0x08741E90`);
 *   * the class-2 task #176 one-shot `Task_FlamerFlame` and the class-3 task #10
 *     entry `Task_Noddy`, whose script continues in src/enemy_844c4.c.
 *
 * `FlamerIdleEnterState`, `SirKibbleCutterEnterState`, `HotHeadFireBreathEnterState` and `HotHeadFireBallEnterState` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (lesson 4.30 / 4.34, curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 a);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(s32 a);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern u16 TaskGetAngleToNearestPlayer(s32 a);
extern void AngleToVector(s32 a, s32 b);
extern u8 ActorCollideTerrainAlongVelocity(void);
extern s32 TaskIsNearestPlayerInRect(struct PointPair *p);
extern s32 TaskIsInRect(struct PointPair *r);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern void ActorReactToHit(void);

void FlamerInit(void)
{
    gCurTask->updateCallback = (u32)FlamerUpdate;
    ActorSetState(FLAMER_STATE_0);
    CallTableEntry(gCurTask->state, 7, gFlamerStates);
}

void FlamerEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gFlamerStates);
}

void FlamerUpdate(void)
{
    switch (gCurTask->updateState)
    {
    case 0:
    case 1:
    case 2:
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 7, gFlamerStateUpdates);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        CallTableEntry(gCurTask->updateState, 7, gFlamerStateUpdates);
        break;
    }
    ActorCheckHits();
    ActorReactToHit();
}

void FlamerState0(void)
{
    struct Task *t;
    u16 v;

    gCurTask->updateState = FLAMER_STATE_0;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    t = gCurTask;
    t->flamerSurfaceSlope = 0;
    t->flamerSurfaceSide = 4;
    do
    {
        t = gCurTask;
        t->flamerSurfaceSide -= 2;
        if (FlamerGetSurfaceSlopeInDir(t->flamerSurfaceSide) != 0)
        {
            gCurTask->flamerSurfaceSlope = 1;
            v = TaskGetAngleToNearestPlayer(2);
            t = gCurTask;
            if ((t->flamerSurfaceSide & 1) == 0)
            {
                if ((u16)(v - 64) > 128)
                    t->flamerCrawlDir = 3;
                else
                    t->flamerCrawlDir = 1;
            }
            else
            {
                if (v <= 127)
                    t->flamerCrawlDir = 0;
                else
                    t->flamerCrawlDir = 2;
            }
            gCurTask->flamerCrawlPhase = 0;
            goto done;
        }
    } while (gCurTask->flamerSurfaceSide != 0);
done:
    if (gCurTask->flamerSurfaceSlope == 0)
        ActorSetState(FLAMER_STATE_FALL);
    else
        ActorSetState(FLAMER_STATE_CRAWL);
    TaskSleepForever();
}

void FlamerState0Update(void)
{
    if (gCurTask->state != FLAMER_STATE_0)
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
}

void FlamerCrawl(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_STATE_CRAWL;
    TaskStop();
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(gUnk_087416AD[gCurTask->flamerSpeedLevel]);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 7)
            t->frame = 4;
    }
}

void FlamerCrawlUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct PointPair p;
    s32 n;
    s32 n2;
    s32 m;
    u8 d;
    u8 e;
    u8 f;

    t = gCurTask;
    switch (t->flamerCrawlPhase)
    {
    case 0:
        d = FlamerGetSurfaceSlopeOnSide(t->flamerSurfaceSide);
        if (d == 0)
        {
            u = gCurTask;
            u->flamerCrawlPhase = 1;
            u->velX = 0;
            u->velY = 0;
            break;
        }
        w = gCurTask;
        w->flamerSurfaceSlope = d;
        if ((w->flamerSurfaceSide & 2) != 0 && d == 1)
        {
            e = sub_08083bbc(w->flamerSurfaceSide, w->flamerCrawlDir);
            f = sub_08083bbc(gCurTask->flamerSurfaceSide, 0);
            if (e > 5 && f > 5)
                gCurTask->flamerSurfaceSlope = e;
        }
        d = FlamerGetSurfaceSlopeInDir(gCurTask->flamerCrawlDir);
        if (d == 1 || ((gCurTask->flamerCrawlDir & 1) == 0 && d > 1))
        {
            gUnk_03001F2C = n = gCurTask->flamerCrawlDir;
            gCurTask->flamerCrawlDir = (gCurTask->flamerSurfaceSide + 2) & 3;
            gCurTask->flamerSurfaceSide = n;
            FlamerSetCrawlVelocity(d);
        }
        else
        {
            FlamerSetCrawlVelocity(gCurTask->flamerSurfaceSlope);
        }
        break;
    case 1:
        m = t->flamerSurfaceSlope;
        if (m > 1)
        {
            t->posX &= 0xFFFF0000;
            t->posY &= 0xFFFF0000;
            t->posX += gUnk_087416F8[t->flamerSurfaceSide] - gUnk_08741718[t->flamerCrawlDir];
            t->posY += gUnk_08741708[t->flamerSurfaceSide] - gUnk_08741728[t->flamerCrawlDir];
            t->flamerSurfaceSlope = 1;
            t->flamerCrawlPhase = 0;
        }
        else
        {
            t->posX = (t->posX & 0xFFF00000) | 0x80000;
            t->posY = (t->posY & 0xFFF00000) | 0x80000;
            t->posX += gUnk_087416F8[t->flamerSurfaceSide] + gUnk_08741718[t->flamerCrawlDir];
            t->posY += gUnk_08741708[t->flamerSurfaceSide] + gUnk_08741728[t->flamerCrawlDir];
            t->flamerCrawlPhase = 2;
            gUnk_03001F2C = n2 = t->flamerSurfaceSide;
            t->flamerSurfaceSide = (t->flamerCrawlDir + 2) & 3;
            t->flamerCrawlDir = n2;
            FlamerSetCrawlVelocity(m);
        }
        x = gCurTask;
        x->pixelX = x->posX >> 16;
        x->pixelY = x->posY >> 16;
        break;
    case 2:
        if (FlamerGetSurfaceSlopeOnSide(t->flamerSurfaceSide) == 0)
            ActorSetState(FLAMER_STATE_0);
        y = gCurTask;
        y->flamerCrawlPhase = 0;
        if ((y->flamerCrawlDir & 1) != 0)
            y->velY = 0;
        break;
    }
    v = gCurTask;
    if (v->flamerSurfaceSide != 0)
        v->onGround = 0;
    else
        v->onGround = 1;
    z = gCurTask;
    if ((z->flamerCheckTimer & 0x80) == 0 && --z->flamerCheckTimer == 0)
    {
        p.x0 = z->pixelX - 64;
        p.y0 = z->pixelY - 64;
        p.x1 = z->pixelX + 64;
        p.y1 = gCurTask->pixelY + 64;
        if (TaskIsNearestPlayerInRect(&p) != 0)
            ActorSetState(FLAMER_STATE_3);
        gCurTask->flamerCheckTimer = 20;
    }
    if (gCurTask->state != FLAMER_STATE_CRAWL)
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
}

void FlamerFall(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_STATE_FALL;
    t = gCurTask;
    t->flamerSurfaceSide = 0;
    t->flamerSurfaceSlope = 0;
    t->onGround = 0;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void FlamerFallUpdate(void)
{
    u16 m;

    if (gCurTask->onGround != 0)
    {
        m = TaskGetAngleToNearestPlayer(2);
        m -= 64;
        if (m > 128)
            gCurTask->flamerCrawlDir = 3;
        else
            gCurTask->flamerCrawlDir = 1;
        gCurTask->flamerCrawlPhase = 0;
        ActorSetState(FLAMER_STATE_CRAWL);
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
    }
}

void FlamerState3(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_STATE_3;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetFrame(4);
    gCurTask->flamerLoopCount = 0;
    do
    {
        t = gCurTask;
        t->velY = gUnk_087416B0[(s16)t->flamerLoopCount];
        TaskYieldTrampoline(gUnk_087416CC[(s16)t->flamerLoopCount]);
        gCurTask->frame++;
        gCurTask->flamerLoopCount++;
    } while ((s16)gCurTask->flamerLoopCount <= 6);
    sub_08083dfc();
    ActorSetState(FLAMER_STATE_4);
    TaskSleepForever();
}

void FlamerState3Update(void)
{
    if (gCurTask->state != FLAMER_STATE_3)
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
}

void FlamerState4(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_STATE_4;
    ActorSetAttackBox(gUnk_0873F758);
    gCurTask->flamerFlightAngle = TaskGetAngleToNearestPlayer(3);
    gCurTask->flamerSteerTimer = 1;
    while (1)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->flamerLoopCount = 0;
        do
        {
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(2);
            gCurTask->flamerLoopCount++;
        } while ((s16)gCurTask->flamerLoopCount <= 2);
    }
}

void FlamerState4Update(void)
{
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PointPair p;
    s16 a;

    if (--gCurTask->flamerSteerTimer == 0)
    {
        TaskGetNearestPlayerPos();
        if (TaskGetFacingTowardNearestPlayer() == 1)
            gUnk_030023B4 = gUnk_030023B4 - 64;
        else
            gUnk_030023B4 = gUnk_030023B4 + 64;
        a = (u16)ArcTan2(gUnk_030023B4 - gCurTask->pixelX, gUnk_030023D4 - gCurTask->pixelY) >> 7;
        if (a > 384 && gCurTask->flamerFlightAngle <= 127)
            a = a - 512;
        else if (a <= 127 && gCurTask->flamerFlightAngle > 384)
            a = a + 512;
        if (a > (u = gCurTask)->flamerFlightAngle)
            u->flamerFlightAngle = u->flamerFlightAngle + 32;
        else
            u->flamerFlightAngle = u->flamerFlightAngle - 32;
        v = gCurTask;
        v->flamerFlightAngle &= 0x1FF;
        p.x0 = v->pixelX;
        p.y0 = v->pixelY;
        p.x1 = gUnk_030023B4;
        p.y1 = gUnk_030023D4;
        if (GetDistSq(&p) <= 99)
        {
            ActorSetState(FLAMER_STATE_5);
            TaskSetEntry(FlamerEnterState, gCurTaskIdx);
        }
        else
        {
            w = gCurTask;
            w->flamerSteerTimer = gUnk_08741738[w->flamerSpeedLevel];
            AngleToVector((s16)w->flamerFlightAngle, gUnk_0874173C[w->flamerSpeedLevel]);
            x = gCurTask;
            x->velX = gUnk_030023B4;
            x->velY = gUnk_030023D4;
        }
    }
}

void FlamerState5(void)
{
    s32 n;

    gCurTask->updateState = FLAMER_STATE_5;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->actorAnimDelay20 = ActorStartAnim(gUnk_08741744);
    TaskSetMotionXFacing(-0x18000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0xC000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    gCurTask->actorAnimDelay20 = ActorStartAnim(gUnk_08741758);
    gCurTask->flamerFlameTimer = -1;
    while (1)
    {
        gCurTask->flamerFlameTimer++;
        if ((gCurTask->flamerFlameTimer & 3) == 0)
        {
            gCurTask->flamerFlameSlot = CreateChildTaskAtOffsetFacing(TASK_FLAMER_FLAME, 0, 0, 1);
            (gTasks + (s16)gCurTask->flamerFlameSlot)->flamerFlameArcIndex = (gCurTask->flamerFlameTimer >> 2) & 3;
        }
        n = TaskGetNearestPlayerDx();
        gCurTask->unk1C = n;
        if (gCurTask->facing == 1 && n < 0)
            break;
        if (gCurTask->facing == -1 && n > 0)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(12);
    gCurTask->actorAnimDelay20 = ActorStartAnim(gUnk_08741744);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(6);
    if (gCurTask->flamerDashCount == 0)
    {
        TaskSetMotionXFacing(0, 0x5A5A5A5A);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(6);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(6);
        TaskStop();
        gCurTask->flamerDashCount++;
        ActorSetState(FLAMER_STATE_6);
    }
    TaskSleepForever();
}

void FlamerState5Update(void)
{
    gCurTask->actorAnimDelay20 = ActorTickAnim(gCurTask->actorAnimDelay20);
    if (gCurTask->state != FLAMER_STATE_5)
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
}

void FlamerState6(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_STATE_6;
    ActorSetAttackBox(gUnk_0873F720);
    gCurTask->flamerFlightAngle = TaskGetAngleToNearestPlayer(3);
    gCurTask->flamerSteerTimer = 1;
    TaskSetFrame(8);
    while (gCurTask->flamerSteerTimer != 0)
    {
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 11)
            t->frame = 8;
    }
    TaskStop();
    sub_08083dfc();
    TaskSetFrame(8);
    TaskYieldTrampoline(6);
    gCurTask->flamerLoopCount = 0;
    do
    {
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(6);
        gCurTask->flamerLoopCount++;
    } while ((s16)gCurTask->flamerLoopCount <= 2);
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    gCurTask->flamerLoopCount = 0;
    do
    {
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(8);
        gCurTask->flamerLoopCount++;
    } while ((s16)gCurTask->flamerLoopCount <= 2);
    gCurTask->flamerCheckTimer = 20;
    ActorSetState(FLAMER_STATE_FALL);
    TaskSleepForever();
}

void FlamerState6Update(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    u16 n;

    t = gCurTask;
    if (t->flamerSteerTimer != 0)
    {
        if (--t->flamerSteerTimer == 0)
        {
            n = TaskGetAngleToNearestPlayer(3);
            u = gCurTask;
            if (n > u->flamerFlightAngle)
                u->flamerFlightAngle = u->flamerFlightAngle + 32;
            else
                u->flamerFlightAngle = u->flamerFlightAngle - 32;
            gUnk_03001F2C = 4;
            while (gUnk_03001F2C != 0 && FlamerGetSurfaceSlopeOnSide(gUnk_03001F2C - 1) == 0)
                gUnk_03001F2C--;
            if (gUnk_03001F2C != 0)
            {
                v = gCurTask;
                v->flamerSteerTimer = gUnk_08741738[v->flamerSpeedLevel];
                AngleToVector((s16)v->flamerFlightAngle, gUnk_0874173C[v->flamerSpeedLevel]);
                w = gCurTask;
                w->velX = gUnk_030023B4;
                w->velY = gUnk_030023D4;
            }
        }
    }
    if (gCurTask->state != FLAMER_STATE_6)
        TaskSetEntry(FlamerEnterState, gCurTaskIdx);
}

void FlamerIdleInit(void)
{
    gCurTask->updateCallback = (u32)FlamerIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(FLAMER_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gFlamerIdleStates);
}

void FlamerIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gFlamerIdleStates);
}

void FlamerIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gFlamerIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void FlamerIdle(void)
{
    struct Task *t;

    gCurTask->updateState = FLAMER_IDLE_STATE_IDLE;
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(10);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 7)
            t->frame = 4;
    }
}

void FlamerIdleState0Update(void)
{
}

u8 FlamerGetSurfaceSlopeInDir(s32 dir)
{
    struct Task *t;
    s16 a;
    s16 b;
    s16 i;
    u8 r;

    r = 0;
    t = gCurTask;
    a = t->pixelX + gUnk_08741684[dir];
    b = t->pixelY + gUnk_08741688[dir];
    i = GetCollisionTileAtPixel(a, b);
    if (gCollisionTileCollides[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

u8 FlamerGetSurfaceSlopeOnSide(s32 dir)
{
    struct Task *t;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    u8 r;

    t = gCurTask;
    if ((t->flamerCrawlDir & 2) != 0)
    {
        a = t->pixelX + gUnk_0874168C[dir];
        b = t->pixelY + gUnk_08741690[dir];
        c = t->pixelX + gUnk_08741694[dir];
        d = t->pixelY + gUnk_08741698[dir];
    }
    else
    {
        a = t->pixelX + gUnk_08741694[dir];
        b = t->pixelY + gUnk_08741698[dir];
        c = t->pixelX + gUnk_0874168C[dir];
        d = t->pixelY + gUnk_08741690[dir];
    }
    r = FlamerGetSurfaceSlopeAt(a, b);
    if (r == 0)
        r = FlamerGetSurfaceSlopeAt(c, d);
    return r;
}

u8 sub_08083bbc(s32 dir, s32 k)
{
    struct Task *t;
    u16 a;
    u16 b;
    s16 i;
    u8 r;
    s8 *p;
    s8 *q;
    r = 0;
    t = gCurTask;
    if ((t->flamerCrawlDir & 2) != 0)
    {
        a = t->pixelX + gUnk_0874168C[dir];
        b = t->pixelY + gUnk_08741690[dir];
    }
    else
    {
        a = t->pixelX + gUnk_08741694[dir];
        b = t->pixelY + gUnk_08741698[dir];
    }
    p = &gUnk_0874169C[k];
    q = &gUnk_087416A0[k];
    i = GetCollisionTileAtOffset(a, b, *p, *q);
    if (i == -1)
        return 0;
    if (gCollisionTileCollides[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

u8 FlamerGetSurfaceSlopeAt(s16 x, s16 y)
{
    s32 i;
    u8 r;

    r = 0;
    i = (s16)GetCollisionTileAtPixel(x, y);
    if (i == -1)
        return 0;
    if (gCollisionTileCollides[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

s32 FlamerSetCrawlVelocity(u8 a)
{
    struct Task *t;
    s16 v;

    if (a > 5)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->flamerCrawlDir == 1)
            v = (v + 272) & 0x1FF;
        else
            v = (v - 16) & 0x1FF;
    }
    else if (a > 1)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->flamerCrawlDir == 1)
            v = (v + 256) & 0x1FF;
    }
    else
    {
        v = gUnk_087416E4[gCurTask->flamerCrawlDir];
    }
    AngleToVector(v, gUnk_087416EC[gCurTask->flamerSpeedLevel][0]);
    t = gCurTask;
    t->velX = gUnk_030023B4;
    t->velY = gUnk_030023D4;
}

void sub_08083dfc(void)
{
    struct Task *t;

    gCurTask->flamerLoopCount = 0;
    do
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        gCurTask->flamerLoopCount++;
    } while ((s16)gCurTask->flamerLoopCount <= 4);
}

u8 FlamerEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_SirKibbleCutter(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    gCurTask->frameTable = gSirKibbleCutterFrames;
    PlaySfx(186);
    CallTableEntry(gCurTask->variant, 1, gSirKibbleCutterVariants);
}

void SirKibbleCutterInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleCutterUpdate;
    TaskFaceLikeParent();
    ActorSetState(SIR_KIBBLE_CUTTER_STATE_0);
    CallTableEntry(gCurTask->state, 1, gSirKibbleCutterStates);
}

void SirKibbleCutterEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gSirKibbleCutterStates);
}

void SirKibbleCutterUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gSirKibbleCutterStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SirKibbleCutterState0(void)
{
    struct Task *t;

    gCurTask->updateState = SIR_KIBBLE_CUTTER_STATE_0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08741E54[gCurTask->actorSpawnArg], gUnk_08741E5C[gCurTask->actorSpawnArg]);
    gCurTask->speedLimitX = 0x2A800;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
    }
}

void SirKibbleCutterState0Update(void)
{
    struct Task *t;
    struct PointPair p;
    vu16 *q;
    s16 i;

    q = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)q[i] != -1)
    {
        t = gTasks + i;
        p.x0 = t->pixelX - 6;
        p.y0 = t->pixelY - 6;
        p.x1 = t->pixelX + 6;
        p.y1 = t->pixelY + 6;
        if (TaskIsInRect(&p) != 0)
            ActorDestroy();
    }
}

void Task_HotHeadFire(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 3;
    u = gCurTask;
    u->frameTable = gHotHeadFireFrames;
    u->onGround = 0;
    TaskFaceLikeParent();
    PlaySfx(194);
    CallTableEntry(gCurTask->variant, 2, gHotHeadFireVariants);
}

void HotHeadFireBreathInit(void)
{
    gCurTask->updateCallback = (u32)HotHeadFireBreathUpdate;
    ActorSetState(HOT_HEAD_FIRE_BREATH_STATE_BREATH);
    CallTableEntry(gCurTask->state, 1, gHotHeadFireBreathStates);
}

void HotHeadFireBreathEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gHotHeadFireBreathStates);
}

void HotHeadFireBreathUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gHotHeadFireBreathStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void HotHeadFireBreath(void)
{
    struct Task *t;
    struct Task *o;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 n;
    s32 m;

    t = gCurTask;
    o = gTasks + (s16)t->parent;
    t->updateState = HOT_HEAD_FIRE_BREATH_STATE_BREATH;
    u = gCurTask;
    if (u->facing == 1)
        u->hotHeadFireAngle = 0;
    else
        u->hotHeadFireAngle = 256;
    v = gCurTask;
    n = (s16)o->hotHeadFlameIndex;
    v->hotHeadFireFanIndex = n;
    m = (v->hotHeadFireAngle + gUnk_08741E70[n]) & 0x1FF;
    v->hotHeadFireAngle = m;
    if ((u32)(m - 128) > 255)
        v->posX = v->posX + 0xE0000;
    else
        v->posX = v->posX - 0xE0000;
    AngleToVector((s16)gCurTask->hotHeadFireAngle, 768);
    w = gCurTask;
    w->velX = gUnk_030023B4;
    w->velY = gUnk_030023D4;
    if ((w->hotHeadFireFanIndex & 2) != 0)
        w->spriteFlags = w->spriteFlags & ~SPRITE_FLAG_FLIP_X;
    else
        w->spriteFlags = w->spriteFlags | SPRITE_FLAG_FLIP_X;
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(1);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(2);
    ActorDestroy();
}

void sub_08084248(void)
{
}

void HotHeadFireBallInit(void)
{
    gCurTask->updateCallback = (u32)HotHeadFireBallUpdate;
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorSetState(HOT_HEAD_FIRE_BALL_STATE_BALL);
        CallTableEntry(gCurTask->state, 1, gHotHeadFireBallStates);
    }
}

void HotHeadFireBallEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gHotHeadFireBallStates);
}

void HotHeadFireBallUpdate(void)
{
    if (ActorCollideTerrainAlongVelocity() == 1)
    {
        ActorSetHitReactions(gHotHeadFireBallUpdateHitReactions);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 1, gHotHeadFireBallStateUpdates);
        ActorCheckHits();
        ActorReactToHit();
    }
}

void HotHeadFireBall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    gCurTask->updateState = HOT_HEAD_FIRE_BALL_STATE_BALL;
    gCurTask->unk28 = 0;
    gCurTask->hotHeadFireAimAngle = n = TaskGetAngleToNearestPlayer(3);
    if (n >= 25 && n <= 127)
        gCurTask->hotHeadFireAimAngle = 24;
    else if (n >= 128 && n <= 231)
        gCurTask->hotHeadFireAimAngle = 232;
    else if (n >= 281 && n <= 383)
        gCurTask->hotHeadFireAimAngle = 280;
    else if (n >= 384 && n <= 487)
        gCurTask->hotHeadFireAimAngle = 488;
    AngleToVector((s16)gCurTask->hotHeadFireAimAngle, 768);
    u = gCurTask;
    u->velX = gUnk_030023B4;
    u->velY = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        v = gCurTask;
        v->frame++;
        TaskYieldTrampoline(1);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->frame--;
        TaskYieldTrampoline(1);
        TaskSetFrame(15);
        TaskYieldTrampoline(2);
    }
}

void sub_080843f8(void)
{
}

void Task_FlamerFlame(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 3;
    gCurTask->frameTable = gFlamerFrames;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0x10000, 0);
    u = gCurTask;
    u->velY = gUnk_08741E94[u->flamerFlameArcIndex];
    u->accelY = gUnk_08741EA4[u->flamerFlameArcIndex];
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame++;
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_Noddy(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gNoddyFrames;
    CallTableEntry(u->variant, 2, gNoddyVariants);
}
