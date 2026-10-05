/* game_code_and_rodata 0x080820B8-0x08082E68 (issue #71, module M21 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080820B8 0x08082E68 src/enemy_820b8.c --newpb
 *
 * Last third of enemy/object behaviour bank 2 (pattern: src/enemy_7f044.c).
 * It holds:
 *   * task #42's dispatcher `Task_Wheelie` (`0x08741640`) and task #43's
 *     `Task_Flamer` (`0x0874176C`), plus the unk73 quartets at
 *     `0x08741C44` (`PoppyBrosJrRiderStartFall` / `PoppyBrosJrRiderLand` / `PoppyBrosJrRiderBounceOffWall` /
 *     `PoppyBrosJrRiderEnterWater`) and `0x08741C64` (`WheelieStartFall` / `WheelieHitWall` /
 *     `sub_08082db0` / `WheelieEnterWater`);
 *   * five scripts in the entry/hook shape: `PoppyBrosJrRideInit`+`PoppyBrosJrRideUpdate`
 *     (`0x08741610`/`0x08741614`; its hook is the one that spawns the
 *     class-0 sub-actors 30/31/32 through CreateActorByKind and hands them to
 *     TransferRoomObject), `PoppyBrosJrDroppedObjectInit`+`PoppyBrosJrDroppedObjectUpdate` (`0x08741618`),
 *     `PoppyBrosJrRideIdleInit`+`PoppyBrosJrRideIdleUpdate` (`0x08741620`), `WheelieInit`+
 *     `WheelieUpdate` (`0x0874164C`/`0x08741664`, six states) and
 *     `WheelieIdleInit`+`WheelieIdleUpdate` (`0x0874167C`);
 *   * `PoppyBrosJrRiderHop` / `PoppyBrosJrRiderHopUpdate`, the hop cycle that drives the actor
 *     record's unk16/unk18/unk1A/unk1E straight from the task, and
 *     `sub_08082cc4`, the four-step Task.unk2C-scaled animation loop;
 *   * `WheelieCheckSkid`, the "turn around once every 30 frames if the player is
 *     behind and within 31 units" probe shared by the bank's walkers.
 *
 * `PoppyBrosJrRideEnterState`, `PoppyBrosJrDroppedObjectEnterState`, `PoppyBrosJrRideIdleEnterState` and `WheelieIdleEnterState` are dead
 * exports; `PoppyBrosJrDroppedObjectState0Update` is a pointer-referenced leaf the census originally
 * missed (both curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 TaskGetAngleToNearestPlayer(s32 prec);
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
extern void ActorSetAttackBox(u32 *p);
extern void AngleToVector(s16 t, s16 mag);

void PoppyBrosJrRideInit(void)
{
    gCurTask->updateCallback = (u32)PoppyBrosJrRideUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(POPPY_BROS_JR_RIDE_STATE_RIDE);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideStates);
}

void PoppyBrosJrRideEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideStates);
}

void PoppyBrosJrRideUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gPoppyBrosJrRideStateUpdates);
    ActorCheckHits();
    t = gCurTask;
    switch (t->hitKind)
    {
    case HIT_KIND_INHALE:
    case HIT_KIND_GRAB:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        if (t->poppyBrosJrMountKind == 0)
            gCurTask->poppyBrosJrSpawnSlot =
                CreateActorByKind(ACTOR_KIND_ENEMY, 31, POPPY_BROS_JR_RIDE_VARIANT_DROPPED_OBJECT, t->actorSpawnArg, t->pixelX, t->pixelY, t->tileWord);
        else
            gCurTask->poppyBrosJrSpawnSlot =
                CreateActorByKind(ACTOR_KIND_ENEMY, 32, POPPY_BROS_JR_RIDE_VARIANT_DROPPED_OBJECT, t->actorSpawnArg, t->pixelX, t->pixelY, t->tileWord);
        TransferRoomObject(gCurTaskIdx, gCurTask->poppyBrosJrSpawnSlot);
        u = gCurTask;
        u->tileWord = u->poppyBrosJrSavedTileWord;
        if (gUnk_0300244C != 0)
            u->frameTable = gPoppyBrosJrFrames;
        TaskSetFrame(1);
        break;
    case HIT_KIND_DEFEAT:
        if (gScreenAttackActive == 1)
        {
            t->u8C.actor->keepExtraOnDefeat = 1;
            gCurTask->u8C.actor->extraFrame = 8;
        }
        else
        {
            gCurTask->poppyBrosJrSpawnSlot =
                CreateActorByKind(ACTOR_KIND_ENEMY, 30, 1, t->actorSpawnArg, t->pixelX, t->pixelY - 14, t->poppyBrosJrSavedTileWord);
        }
        TransferRoomObject(gCurTaskIdx, gCurTask->poppyBrosJrSpawnSlot);
        break;
    }
    ActorReactToHit();
}

void PoppyBrosJrRide(void)
{
    gCurTask->updateState = POPPY_BROS_JR_RIDE_STATE_RIDE;
    TaskSetMotionXFacing(gUnk_087415E4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    PoppyBrosJrRiderHop();
}

void PoppyBrosJrRideState0Update(void)
{
    PoppyBrosJrRiderHopUpdate();
}

void PoppyBrosJrDroppedObjectInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)PoppyBrosJrDroppedObjectUpdate;
    t->poppyBrosJrDropFrameTimer = 0;
    t->unk20 = 0;
    t->poppyBrosJrBounceCount = 0;
    ActorSetState(POPPY_BROS_JR_DROPPED_OBJECT_STATE_0);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrDroppedObjectStates);
}

void PoppyBrosJrDroppedObjectEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrDroppedObjectStates);
}

void PoppyBrosJrDroppedObjectUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gPoppyBrosJrDroppedObjectStateUpdates);
    if (gCurTask->poppyBrosJrBounceCount == 2)
        ActorCheckHits();
    ActorReactToHit();
}

void PoppyBrosJrDroppedObjectState0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 n;
    s32 *p;
    struct Task **g;

    gCurTask->updateState = POPPY_BROS_JR_DROPPED_OBJECT_STATE_0;
    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->facing == 1)
        t->facing = -1;
    else
        t->facing = 1;
    n = TaskGetAngleToNearestPlayer(3);
    u = gCurTask;
    u->unk18 = ((u16)n + 256) & 511;
    AngleToVector(u->unk18, 128);
    gCurTask->velX = gUnk_030023B4;
    TaskSetFrame(4);
    if (gCurTask->poppyBrosJrBounceCount != 2)
    {
        g = &gCurTask;
        p = gUnk_087415F4;
        do
        {
            (*g)->onGround = 0;
            v = *g;
            TaskSetMotionY(p[v->poppyBrosJrBounceCount], gUnk_087415FC[v->poppyBrosJrBounceCount], 0x30000);
            if ((*g)->onGround == 0)
            {
                do
                {
                    TaskYieldTrampoline(1);
                } while (gCurTask->onGround == 0);
            }
            w = *g;
        } while (++w->poppyBrosJrBounceCount != 2);
    }
    TaskStop();
    x = gCurTask;
    if (x->poppyBrosJrMountKind == 1)
    {
        gCurTask->poppyBrosJrSpawnSlot = CreateActorByKind(ACTOR_KIND_ITEM, 2, 0, 0, x->pixelX, x->pixelY, 0);
        TransferRoomObject(gCurTaskIdx, gCurTask->poppyBrosJrSpawnSlot);
        ActorDestroy();
    }
    TaskSleepForever();
}

void PoppyBrosJrDroppedObjectState0Update(void)
{
    struct Task *t = gCurTask;

    if (t->poppyBrosJrBounceCount != 2)
    {
        if ((++t->poppyBrosJrDropFrameTimer & 3) == 0)
        {
            t->poppyBrosJrDropFrameTimer = 0;
            if (++t->frame > 11)
                t->frame = 4;
        }
    }
}

void PoppyBrosJrRideIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)PoppyBrosJrRideIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(POPPY_BROS_JR_RIDE_IDLE_STATE_RIDE_IDLE);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideIdleStates);
}

void PoppyBrosJrRideIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideIdleStates);
}

void PoppyBrosJrRideIdleUpdate(void)
{
    struct Task *t;

    CallTableEntry(gCurTask->updateState, 1, gPoppyBrosJrRideIdleStateUpdates);
    ActorCheckHits();
    t = gCurTask;
    switch (t->hitKind)
    {
    case HIT_KIND_INHALE:
    case HIT_KIND_GRAB:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        t->tileWord = t->poppyBrosJrSavedTileWord;
        TaskSetFrame(1);
        break;
    }
    ActorReactToHit();
}

void PoppyBrosJrRideIdle(void)
{
    PoppyBrosJrRiderHop();
}

void sub_08082548(void)
{
    PoppyBrosJrRiderHopUpdate();
}

void PoppyBrosJrRiderHop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    TaskSetFrame(9);
    gCurTask->poppyBrosJrRollTimer = gPoppyBrosJrRiderRollTimes[gCurTask->actorSpawnArg];
    while (1)
    {
        t = gCurTask;
        t->poppyBrosJrRiderOffsetY = 0;
        t->poppyBrosJrRiderVelY = -1;
        t->poppyBrosJrRiderFrame = 7;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->poppyBrosJrRiderVelY = 0xFFFE8000;
        u->poppyBrosJrRiderFrame--;
        TaskYieldTrampoline(2);
        gCurTask->poppyBrosJrRiderFrame--;
        TaskYieldTrampoline(2);
        gCurTask->poppyBrosJrRiderFrame--;
        TaskYieldTrampoline(16);
        gCurTask->poppyBrosJrRiderFrame++;
        TaskYieldTrampoline(6);
        v = gCurTask;
        v->poppyBrosJrRiderFrame++;
        if (v->poppyBrosJrRiderOffsetY != 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gCurTask->poppyBrosJrRiderOffsetY != 0);
        }
    }
}

void PoppyBrosJrRiderHopUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s16 h;

    t = gCurTask;
    if (--t->poppyBrosJrRollTimer == 0)
    {
        t->poppyBrosJrRollTimer = gPoppyBrosJrRiderRollTimes[t->actorSpawnArg];
        if (++t->frame > 16)
            t->frame = 9;
    }
    u = gCurTask;
    if (u->poppyBrosJrRiderVelY != -1)
    {
        u->poppyBrosJrRiderVelY += 0x1800;
        u->poppyBrosJrRiderOffsetY += u->poppyBrosJrRiderVelY;
        if (u->poppyBrosJrRiderOffsetY > 0)
            u->poppyBrosJrRiderOffsetY = 0;
    }
    v = gCurTask;
    a = v->u8C.actor;
    a->unk16 = 0;
    h = ((s16 *)&v->poppyBrosJrRiderOffsetY)[1];
    v->u8C.actor->extraOffsetY = h - 16;
    v->u8C.actor->extraTileWord = v->poppyBrosJrSavedTileWord;
    v->u8C.actor->extraFrame = v->poppyBrosJrRiderFrame;
    v->u8C.actor->extraLayerOffset = -1;
}

s32 PoppyBrosJrRiderStartFall(void)
{
    if (gCurTask->variant == 0)
        TaskSetMotionY(0, 0x1500, 0x30000);
    return 0;
}

s32 PoppyBrosJrRiderLand(void)
{
    if (gCurTask->variant == 0)
        TaskStopY();
    return 0;
}

s32 PoppyBrosJrRiderBounceOffWall(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

s32 PoppyBrosJrRiderEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Wheelie(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gWheelieFrames;
    CallTableEntry(gCurTask->variant, 3, gWheelieVariants);
}

void WheelieInit(void)
{
    gCurTask->updateCallback = (u32)WheelieUpdate;
    TaskFaceNearestPlayer();
    gCurTask->wheelieCheckTimer = 30;
    ActorSetState(WHEELIE_STATE_ROLL_START);
    CallTableEntry(gCurTask->state, 6, gWheelieStates);
}

void WheelieEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gWheelieStates);
}

void WheelieUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 6, gWheelieStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void WheelieRollStart(void)
{
    struct Task *t;

    gCurTask->updateState = WHEELIE_STATE_ROLL_START;
    TaskSetMotionXFacing(gUnk_08741628[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    *(s16 *)&gCurTask->wheelieLoopCount = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    } while (++*(s16 *)&gCurTask->wheelieLoopCount <= 3);
    ActorSetState(WHEELIE_STATE_ROLL_LOOP);
    TaskSleepForever();
}

void WheelieRollStartUpdate(void)
{
    WheelieCheckSkid();
    if (gCurTask->state != WHEELIE_STATE_ROLL_START)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void WheelieRollLoop(void)
{
    gCurTask->updateState = WHEELIE_STATE_ROLL_LOOP;
    TaskSetMotionXFacing(gUnk_08741630[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    gCurTask->wheelieFrameScale = 1;
    sub_08082cc4();
}

void WheelieRollLoopUpdate(void)
{
    WheelieCheckSkid();
    if (gCurTask->state != WHEELIE_STATE_ROLL_LOOP)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void WheelieSkid(void)
{
    struct Task *t;

    gCurTask->updateState = WHEELIE_STATE_SKID;
    t = gCurTask;
    t->wheelieStateTimer = 42;
    t->wheelieSkidStopped = 0;
    while (1)
    {
        CreateDustTrail(1, 1, -4, 6);
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
}

void WheelieSkidUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if (t->wheelieSkidStopped == 0)
    {
        if (t->velX > 0)
            t->velX = t->velX + 0xFFFFF400;
        else if (t->velX < 0)
            t->velX = t->velX + 0xC00;
        u = gCurTask;
        if (u->onGround == 0)
        {
            TaskStopX();
            gCurTask->wheelieSkidStopped = 1;
        }
    }
    v = gCurTask;
    if (--v->wheelieStateTimer == 0)
    {
        ActorSetState(WHEELIE_STATE_WAIT);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void WheelieWait(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gCurTask->updateState = WHEELIE_STATE_WAIT;
    TaskStopX();
    t = gCurTask;
    if (t->facing == 1)
        t->facing = -1;
    else
        t->facing = 1;
    n = RandomRange(3);
    u = gCurTask;
    u->wheelieStateTimer = gUnk_08741638[n];
    u->wheelieFrameScale = 2;
    sub_08082cc4();
}

void WheelieWaitUpdate(void)
{
    struct Task *t = gCurTask;

    if (--t->wheelieStateTimer == 0)
    {
        t->wheelieCheckTimer = 30;
        ActorSetState(WHEELIE_STATE_ROLL_LOOP);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void WheelieBounceOffWall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = WHEELIE_STATE_BOUNCE_OFF_WALL;
    TaskStop();
    RequestScreenShake(1);
    PlaySfx(197);
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFD0000, 0x2500, 0x30000);
    t = gCurTask;
    t->wheelieFrameIndex = 0;
    t->wheelieFrameTimer = 3;
    TaskSetFrame(gUnk_0874163B[0]);
    if (gCurTask->onGround == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
            u = gCurTask;
            if (--u->wheelieFrameTimer == 0)
            {
                if (++u->wheelieFrameIndex > 3)
                    u->wheelieFrameIndex = 0;
                TaskSetFrame(gUnk_0874163B[gCurTask->wheelieFrameIndex]);
                gCurTask->wheelieFrameTimer = 3;
            }
        } while (gCurTask->onGround == 0);
    }
    TaskStop();
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    v = gCurTask;
    if (v->facing == 1)
        v->facing = -1;
    else
        v->facing = 1;
    ActorSetState(WHEELIE_STATE_ROLL_LOOP);
    TaskSleepForever();
}

void WheelieBounceOffWallUpdate(void)
{
    if (gCurTask->state != WHEELIE_STATE_BOUNCE_OFF_WALL)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void WheelieFall(void)
{
    gCurTask->updateState = WHEELIE_STATE_FALL;
    gCurTask->wheelieRestoreTimer = 0;
    TaskSetMotionY(0, 0x2500, 0x30000);
    gCurTask->wheelieFrameScale = 1;
    sub_08082cc4();
}

void WheelieFallUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->wheelieRestoreTimer != 0)
    {
        if (--t->wheelieRestoreTimer == 0)
        {
            t->posX = t->wheelieSavedPosX;
            t->posY = t->wheelieSavedPosY;
        }
    }
    if (gCurTask->onGround != 0)
    {
        TaskStopY();
        if (TaskGetFacingTowardNearestPlayer() != gCurTask->facing)
            ActorSetState(WHEELIE_STATE_SKID);
        else
            ActorSetState(WHEELIE_STATE_ROLL_LOOP);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void WheelieIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)WheelieIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gIdleAttackBox);
    gCurTask->health = 2;
    ActorSetState(WHEELIE_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gWheelieIdleStates);
}

void WheelieIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gWheelieIdleStates);
}

void WheelieIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gWheelieIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void WheelieIdle(void)
{
    gCurTask->updateState = WHEELIE_IDLE_STATE_IDLE;
    gCurTask->wheelieFrameScale = 1;
    sub_08082cc4();
}

void WheelieIdleState0Update(void)
{
}

void WheelieCheckSkid(void)
{
    struct Task *t = gCurTask;

    if (t->variant != 0)
        return;
    if (--t->wheelieCheckTimer != 0)
        return;
    t->wheelieCheckTimer = 30;
    if (RandomRange(2) == 0)
        return;
    if (TaskGetFacingTowardNearestPlayer() == gCurTask->facing)
        return;
    if (abs(TaskGetNearestPlayerDy()) <= 31)
        ActorSetState(WHEELIE_STATE_SKID);
}

void sub_08082cc4(void)
{
    struct Task *t;

    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(gCurTask->wheelieFrameScale * 2);
        TaskSetFrame(4);
        t = gCurTask;
        TaskYieldTrampoline(t->wheelieFrameScale * 3);
        TaskSetFrame(6);
        TaskYieldTrampoline(gCurTask->wheelieFrameScale * 2);
        TaskSetFrame(5);
        t = gCurTask;
        TaskYieldTrampoline(t->wheelieFrameScale * 3);
    }
}

s32 WheelieStartFall(void)
{
    s32 r = 0;

    switch (gCurTask->state)
    {
    case WHEELIE_STATE_ROLL_START:
    case WHEELIE_STATE_ROLL_LOOP:
    case WHEELIE_STATE_SKID:
        ActorSetState(WHEELIE_STATE_FALL);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 WheelieHitWall(void)
{
    s32 r = 0;
    struct Task *t;

    switch (gCurTask->state)
    {
    case WHEELIE_STATE_ROLL_START:
    case WHEELIE_STATE_ROLL_LOOP:
        ActorSetState(WHEELIE_STATE_BOUNCE_OFF_WALL);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
        r = 1;
        break;
    case WHEELIE_STATE_FALL:
        TaskTurnAroundAndReverseX();
        t = gCurTask;
        t->wheelieRestoreTimer = 2;
        t->wheelieSavedPosX = t->posX;
        t->wheelieSavedPosY = t->posY;
        break;
    case WHEELIE_STATE_SKID:
        TaskStopX();
        gCurTask->wheelieSkidStopped = 1;
        break;
    }
    return r;
}

s32 sub_08082db0(void)
{
    if (gCurTask->state == 2)
    {
        TaskStopX();
        gCurTask->wheelieSkidStopped = 1;
    }
    return 0;
}

s32 WheelieEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

void Task_Flamer(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gFlamerFrames;
    switch (u->variant)
    {
    case 0:
        u->flamerCheckTimer = 20;
        break;
    case 1:
        u->variant = 0;
        v = gCurTask;
        if (++v->flamerSpeedLevel > 2)
            gCurTask->flamerSpeedLevel = 2;
        gCurTask->flamerCheckTimer = 128;
        break;
    case 2:
        break;
    }
    w = gCurTask;
    w->flamerDashCount = 0;
    w->flamerSurfaceSide = 0;
    w->flamerCrawlDir = 1;
    CallTableEntry(w->variant, 3, gFlamerVariants);
}
