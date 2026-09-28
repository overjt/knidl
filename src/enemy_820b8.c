/* game_code_and_rodata 0x080820B8-0x08082E68 (issue #71, module M21 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080820B8 0x08082E68 src/enemy_820b8.c --newpb
 *
 * Last third of enemy/object behaviour bank 2 (pattern: src/enemy_7f044.c).
 * It holds:
 *   * task #42's dispatcher `Task_Wheelie` (`0x08741640`) and task #43's
 *     `Task_Flamer` (`0x0874176C`), plus the unk73 quartets at
 *     `0x08741C44` (`sub_08082678` / `sub_080826a0` / `sub_080826bc` /
 *     `sub_080826c8`) and `0x08741C64` (`sub_08082d14` / `sub_08082d4c` /
 *     `sub_08082db0` / `sub_08082dd4`);
 *   * five scripts in the entry/hook shape: `PoppyBrosJrRideInit`+`PoppyBrosJrRideUpdate`
 *     (`0x08741610`/`0x08741614`; its hook is the one that spawns the
 *     class-0 sub-actors 30/31/32 through CreateActorByKind and hands them to
 *     TransferRoomObject), `PoppyBrosJrDroppedObjectInit`+`PoppyBrosJrDroppedObjectUpdate` (`0x08741618`),
 *     `PoppyBrosJrRideIdleInit`+`PoppyBrosJrRideIdleUpdate` (`0x08741620`), `WheelieInit`+
 *     `WheelieUpdate` (`0x0874164C`/`0x08741664`, six states) and
 *     `WheelieIdleInit`+`WheelieIdleUpdate` (`0x0874167C`);
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
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideStates);
}

void sub_080820ec(void)
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
    case 3:
    case 4:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        if (t->unk28 == 0)
            gCurTask->unk46 =
                CreateActorByKind(0, 31, 1, t->unk74, t->pixelX, t->pixelY, t->tileWord);
        else
            gCurTask->unk46 =
                CreateActorByKind(0, 32, 1, t->unk74, t->pixelX, t->pixelY, t->tileWord);
        TransferRoomObject(gCurTaskIdx, gCurTask->unk46);
        u = gCurTask;
        u->tileWord = u->unk2C;
        if (gUnk_0300244C != 0)
            u->frameTable = gPoppyBrosJrFrames;
        TaskSetFrame(1);
        break;
    case 1:
        if (gScreenAttackActive == 1)
        {
            t->u8C.actor->unk0D = 1;
            gCurTask->u8C.actor->extraFrame = 8;
        }
        else
        {
            gCurTask->unk46 =
                CreateActorByKind(0, 30, 1, t->unk74, t->pixelX, t->pixelY - 14, t->unk2C);
        }
        TransferRoomObject(gCurTaskIdx, gCurTask->unk46);
        break;
    }
    ActorReactToHit();
}

void sub_08082270(void)
{
    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_087415E4[gCurTask->unk74], 0x5A5A5A5A);
    sub_08082554();
}

void sub_080822a4(void)
{
    sub_080825ec();
}

void PoppyBrosJrDroppedObjectInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)PoppyBrosJrDroppedObjectUpdate;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk24 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrDroppedObjectStates);
}

void sub_080822e4(void)
{
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrDroppedObjectStates);
}

void PoppyBrosJrDroppedObjectUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gPoppyBrosJrDroppedObjectStateUpdates);
    if (gCurTask->unk24 == 2)
        ActorCheckHits();
    ActorReactToHit();
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

    gCurTask->updateState = 0;
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
    if (gCurTask->unk24 != 2)
    {
        g = &gCurTask;
        p = gUnk_087415F4;
        do
        {
            (*g)->onGround = 0;
            v = *g;
            TaskSetMotionY(p[v->unk24], gUnk_087415FC[v->unk24], 0x30000);
            if ((*g)->onGround == 0)
            {
                do
                {
                    TaskYieldTrampoline(1);
                } while (gCurTask->onGround == 0);
            }
            w = *g;
        } while (++w->unk24 != 2);
    }
    TaskStop();
    x = gCurTask;
    if (x->unk28 == 1)
    {
        gCurTask->unk46 = CreateActorByKind(6, 2, 0, 0, x->pixelX, x->pixelY, 0);
        TransferRoomObject(gCurTaskIdx, gCurTask->unk46);
        ActorDestroy();
    }
    TaskSleepForever();
}

void sub_08082458(void)
{
    struct Task *t = gCurTask;

    if (t->unk24 != 2)
    {
        if ((++t->unk1C & 3) == 0)
        {
            t->unk1C = 0;
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
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gPoppyBrosJrRideIdleStates);
}

void sub_080824d0(void)
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
    case 3:
    case 4:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        t->tileWord = t->unk2C;
        TaskSetFrame(1);
        break;
    }
    ActorReactToHit();
}

void PoppyBrosJrRideIdle(void)
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

    TaskSetFrame(9);
    gCurTask->unk20 = gUnk_087415F0[gCurTask->unk74];
    while (1)
    {
        t = gCurTask;
        t->unk34 = 0;
        t->unk18 = -1;
        t->unk1C = 7;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->unk18 = 0xFFFE8000;
        u->unk1C--;
        TaskYieldTrampoline(2);
        gCurTask->unk1C--;
        TaskYieldTrampoline(2);
        gCurTask->unk1C--;
        TaskYieldTrampoline(16);
        gCurTask->unk1C++;
        TaskYieldTrampoline(6);
        v = gCurTask;
        v->unk1C++;
        if (v->unk34 != 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gCurTask->unk34 != 0);
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

    t = gCurTask;
    if (--t->unk20 == 0)
    {
        t->unk20 = gUnk_087415F0[t->unk74];
        if (++t->frame > 16)
            t->frame = 9;
    }
    u = gCurTask;
    if (u->unk18 != -1)
    {
        u->unk18 += 0x1800;
        u->unk34 += u->unk18;
        if (u->unk34 > 0)
            u->unk34 = 0;
    }
    v = gCurTask;
    a = v->u8C.actor;
    a->unk16 = 0;
    h = ((s16 *)&v->unk34)[1];
    v->u8C.actor->extraOffsetY = h - 16;
    v->u8C.actor->extraTileWord = v->unk2C;
    v->u8C.actor->extraFrame = v->unk1C;
    v->u8C.actor->extraLayerOffset = -1;
}

s32 sub_08082678(void)
{
    if (gCurTask->variant == 0)
        TaskSetMotionY(0, 0x1500, 0x30000);
    return 0;
}

s32 sub_080826a0(void)
{
    if (gCurTask->variant == 0)
        TaskStopY();
    return 0;
}

s32 sub_080826bc(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_080826c8(void)
{
    sub_0806a0f0(-2);
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
    gCurTask->unk28 = 30;
    ActorSetState(0);
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

void sub_0808279c(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_08741628[gCurTask->unk74], 0x5A5A5A5A);
    *(s16 *)&gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    } while (++*(s16 *)&gCurTask->unk6C <= 3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08082818(void)
{
    sub_08082c5c();
    if (gCurTask->state != 0)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void sub_08082844(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionXFacing(gUnk_08741630[gCurTask->unk74], 0x5A5A5A5A);
    gCurTask->unk2C = 1;
    sub_08082cc4();
}

void sub_0808287c(void)
{
    sub_08082c5c();
    if (gCurTask->state != 1)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void sub_080828a8(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->unk30 = 42;
    t->unk34 = 0;
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

void sub_08082908(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if (t->unk34 == 0)
    {
        if (t->velX > 0)
            t->velX = t->velX + 0xFFFFF400;
        else if (t->velX < 0)
            t->velX = t->velX + 0xC00;
        u = gCurTask;
        if (u->onGround == 0)
        {
            TaskStopX();
            gCurTask->unk34 = 1;
        }
    }
    v = gCurTask;
    if (--v->unk30 == 0)
    {
        ActorSetState(3);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void sub_08082980(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gCurTask->updateState = 3;
    TaskStopX();
    t = gCurTask;
    if (t->facing == 1)
        t->facing = -1;
    else
        t->facing = 1;
    n = RandomRange(3);
    u = gCurTask;
    u->unk30 = gUnk_08741638[n];
    u->unk2C = 2;
    sub_08082cc4();
}

void sub_080829d4(void)
{
    struct Task *t = gCurTask;

    if (--t->unk30 == 0)
    {
        t->unk28 = 30;
        ActorSetState(1);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void sub_08082a08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = 4;
    TaskStop();
    RequestScreenShake(1);
    PlaySfx(197);
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFD0000, 0x2500, 0x30000);
    t = gCurTask;
    t->unk30 = 0;
    t->unk34 = 3;
    TaskSetFrame(gUnk_0874163B[0]);
    if (gCurTask->onGround == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
            u = gCurTask;
            if (--u->unk34 == 0)
            {
                if (++u->unk30 > 3)
                    u->unk30 = 0;
                TaskSetFrame(gUnk_0874163B[gCurTask->unk30]);
                gCurTask->unk34 = 3;
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
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08082aec(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
}

void sub_08082b14(void)
{
    gCurTask->updateState = 5;
    gCurTask->unk30 = 0;
    TaskSetMotionY(0, 0x2500, 0x30000);
    gCurTask->unk2C = 1;
    sub_08082cc4();
}

void sub_08082b48(void)
{
    struct Task *t = gCurTask;

    if (t->unk30 != 0)
    {
        if (--t->unk30 == 0)
        {
            t->posX = t->unk34;
            t->posY = t->unk24;
        }
    }
    if (gCurTask->onGround != 0)
    {
        TaskStopY();
        if (TaskGetFacingTowardNearestPlayer() != gCurTask->facing)
            ActorSetState(2);
        else
            ActorSetState(1);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
    }
}

void WheelieIdleInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)WheelieIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gWheelieIdleStates);
}

void sub_08082bfc(void)
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
    gCurTask->updateState = 0;
    gCurTask->unk2C = 1;
    sub_08082cc4();
}

void sub_08082c58(void)
{
}

void sub_08082c5c(void)
{
    struct Task *t = gCurTask;

    if (t->variant != 0)
        return;
    if (--t->unk28 != 0)
        return;
    t->unk28 = 30;
    if (RandomRange(2) == 0)
        return;
    if (TaskGetFacingTowardNearestPlayer() == gCurTask->facing)
        return;
    if (abs(TaskGetNearestPlayerDy()) <= 31)
        ActorSetState(2);
}

void sub_08082cc4(void)
{
    struct Task *t;

    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(gCurTask->unk2C * 2);
        TaskSetFrame(4);
        t = gCurTask;
        TaskYieldTrampoline(t->unk2C * 3);
        TaskSetFrame(6);
        TaskYieldTrampoline(gCurTask->unk2C * 2);
        TaskSetFrame(5);
        t = gCurTask;
        TaskYieldTrampoline(t->unk2C * 3);
    }
}

s32 sub_08082d14(void)
{
    s32 r = 0;

    switch (gCurTask->state)
    {
    case 0:
    case 1:
    case 2:
        ActorSetState(5);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08082d4c(void)
{
    s32 r = 0;
    struct Task *t;

    switch (gCurTask->state)
    {
    case 0:
    case 1:
        ActorSetState(4);
        TaskSetEntry(WheelieEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        TaskTurnAroundAndReverseX();
        t = gCurTask;
        t->unk30 = 2;
        t->unk34 = t->posX;
        t->unk24 = t->posY;
        break;
    case 2:
        TaskStopX();
        gCurTask->unk34 = 1;
        break;
    }
    return r;
}

s32 sub_08082db0(void)
{
    if (gCurTask->state == 2)
    {
        TaskStopX();
        gCurTask->unk34 = 1;
    }
    return 0;
}

s32 sub_08082dd4(void)
{
    sub_0806a0f0(-2);
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
        u->unk28 = 20;
        break;
    case 1:
        u->variant = 0;
        v = gCurTask;
        if (++v->unk74 > 2)
            gCurTask->unk74 = 2;
        gCurTask->unk28 = 128;
        break;
    case 2:
        break;
    }
    w = gCurTask;
    w->unk2C = 0;
    w->unk30 = 0;
    w->unk34 = 1;
    CallTableEntry(w->variant, 3, gFlamerVariants);
}
