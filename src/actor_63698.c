/* game_code_and_rodata 0x08063698-0x080653EC (issue #65, module M17 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08063698 0x080653EC src/actor_63698.c --newpb
 *
 * Actor lifecycle and geometry: binding a task to its ROM descriptor
 * (ActorBindDefSlot/ActorLoadDefSlot), resetting the actor record (ActorInitFromDefSlot),
 * 16.16 position/velocity accessors, ArcTan2 aiming, rectangle and distance
 * queries, animation-script walking, and the spawn helpers
 * (sub_08064A78/CreateChildTask/CreateActor) every later module calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "camera.h"
#include "actor.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern u16 ActorComputeHealth(u32 a);
extern u16 ActorComputeHealthSlot(u32 i);
extern void ActorInitTerrainFlagsSlot(u32 i);
extern void ReleaseRoomObject(u32 i);
extern s32 GetShapeAtPixelIgnoringOneWay(s16 x, s16 y);
extern u32 TaskIsOnScreen(void);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

s32 TaskGetDxTo(u32 i);
s32 TaskIsInRectSlot(struct Rect *r, u32 i);
u16 TaskGetAngleToNearestPlayer(s32 prec);
s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg, int prioArg,
                 int altArg);
s32 CreateActor(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio);

s32 TaskCreatePausedInScreenAttack(u32 type, s32 start)
{
    s32 i;
    struct Task *t;

    i = TaskCreateInRange(type, start, 62);
    if (i == -1)
        return i;
    t = &gTasks[i];
    if (gScreenAttackActive != 1)
        return i;
    gTaskSkipMaskStack[0][i] = gTaskSkipMaskStack[1][i] = 0;
    t->skipMask = 15;
    return i;
}

void ActorInitSlot(u32 i)
{
    ActorBindDefSlot(i);
    ActorInitFromDefSlot(i);
    TaskFindNearestPlayerSlot(i);
    ActorInitTerrainFlagsSlot(i);
}

void ActorBindDefSlot(u32 i)
{
    struct Task *t;
    struct Actor *a;

    t = &gTasks[i];
    a = t->u8C.actor;
    a->def = NULL;
    switch (t->actorKind)
    {
    case 0:
        a->def = gEnemyDefs[t->u76.subtype];
        break;
    case 1:
    case 3:
        a->def = gMidBossDefs[t->u76.subtype];
        break;
    case 2:
        a->def = gBossDefs[t->u76.subtype];
        gBossSubtype = t->u76.subtype;
        break;
    case 4:
        a->def = gChildActorDefs[t->u76.subtype];
        break;
    case 5:
        a->def = gUnk_0873EE70[t->u76.subtype];
        break;
    default:
        a->def = gUnk_0873EE88[t->u76.subtype];
        break;
    }
}

void ActorResetHealthSlot(u32 i)
{
    struct Task *t;

    t = &gTasks[i];
    t->health = ActorComputeHealthSlot(i);
}

void ActorResetHealth(u32 a)
{
    gCurTask->health = ActorComputeHealth(a);
}

void ActorInitFromDefSlot(u32 i)
{
    struct Task *t;
    struct Actor *a;
    struct ActorDef *d;

    t = &gTasks[i];
    a = t->u8C.actor;
    a->prevState = 0xFFFF;
    a->unk04 = 0;
    a->hitState = 0;
    a->hitterClass = 0;
    a->hitterKind = 0;
    a->animScript = 0;
    a->hitStunTimer = 0;
    a->animNoFlip = 0;
    a->animScriptPos = 0;
    a->paletteOverridden = 0;
    a->paletteLocked = 0;
    a->hitterParent = -1;
    a->attachedTask = -1;
    a->attachedTaskLifetime = 0xFFFE;
    a->savedPaletteBits = t->tileWord & 0xF000;
    a->paletteColorCount = 16;
    a->palette = 0;
    a->paletteVariant = t->variant >> 4;
    t->variant = t->variant & 15;
    if (a->paletteVariant != 0)
        ActorSelectPaletteVariant(i);
    t->serial = gNextActorSerial;
    if (gNextActorSerial > 0xFFFE)
        gNextActorSerial = 0;
    else
        gNextActorSerial = gNextActorSerial + 1;
    a->defeatSweepCallback = 0;
    a->unk34 = -1;
    a->sfxOverride = -1;
    a->unk0D = 0;
    a->extraOffsetY = 0;
    a->unk16 = 0;
    a->extraLayerOffset = 0;
    a->extraTileWord = 0;
    a->extraFrame = 0xFFFF;
    a->healthBonus = 0;
    d = a->def;
    if (d != NULL)
    {
        a->attackBox = d->attackBox;
        a->extraAttackBox = 0;
        a->terrainBox = d->terrainBox;
        a->prevTerrainHandlers = a->terrainHandlers = d->terrainHandlers;
        a->hitReactions = d->hitReactions;
        a->ability = d->ability;
        a->score = d->score;
        ActorResetHealthSlot(i);
        if (a->def->initCallback != NULL)
            a->def->initCallback(i);
        a->teardown = a->def->teardown;
        a->unk60 = a->def->unk10;
    }
    else
    {
        a->attackBox = 0;
        a->extraAttackBox = 0;
        a->terrainBox = 0;
        a->terrainHandlers = 0;
        a->prevTerrainHandlers = 0;
        a->hitReactions = 0;
        a->ability = ABILITY_NORMAL;
        a->score = 0;
        t->health = 0;
        a->unk60 = 0;
        a->teardown = 0;
    }
}

void ActorLoadDef(struct ActorDef *def)
{
    ActorLoadDefSlot(gCurTaskIdx, def);
}

void ActorLoadDefSlot(u32 i, struct ActorDef *d)
{
    struct Actor *a;

    a = (&gTasks[i])->u8C.actor;
    a->def = d;
    a->attackBox = d->attackBox;
    a->terrainBox = d->terrainBox;
    a->prevTerrainHandlers = a->terrainHandlers;
    a->terrainHandlers = d->terrainHandlers;
    a->hitReactions = d->hitReactions;
    a->ability = d->ability;
    a->score = d->score;
    a->teardown = d->teardown;
    ActorResetHealthSlot(i);
}

void ActorSetState(u8 state)
{
    struct Task *t;

    t = gCurTask;
    t->u8C.actor->prevState = t->state;
    t->state = state;
}

void ActorSetStateSlot(u32 i, u8 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->u8C.actor->prevState = t->state;
    t->state = v;
}

void ActorSetTerrainHandlers(u32 v)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    a->prevTerrainHandlers = a->terrainHandlers;
    a->terrainHandlers = v;
}

void ActorSetHitReactions(u32 reactions)
{
    gCurTask->u8C.actor->hitReactions = reactions;
}

void ActorSetAttackBox(u32 box)
{
    ActorSetAttackBoxSlot(gCurTaskIdx, box);
}

void ActorSetAttackBoxSlot(u32 i, u32 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->u8C.actor->attackBox = v;
}

void ActorSetTerrainBox(u32 box)
{
    gCurTask->u8C.actor->terrainBox = box;
}

void sub_080639f0(struct ActorAux *v)
{
    gCurTask->u8C.actor->unk60 = v;
}

void sub_08063a00(u32 v)
{
    sub_08063a14(gCurTaskIdx, v);
}

void sub_08063a14(u32 i, u32 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->u8C.actor->extraAttackBox = v;
}

/* Nearest task in slots 4..15 to the running one, along X. */
/* Nearest task in slots 4..15 to the running one, along X. */
s32 sub_08063a2c(void)
{
    struct Task *o;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 i;

    best = -1;
    bestDist = 0;
    for (i = 4; i <= 15; i++)
    {
        if (gTaskSlotTypes[i] != -1)
        {
            o = &gTasks[i];
            d = gCurTask->pixelX - o->pixelX;
            if (d < 0)
                d = o->pixelX - gCurTask->pixelX;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = i;
            }
        }
    }
    return best;
}

/* Same, but over the active-player mask, relative to task `i`. */
s32 TaskFindNearestPlayerSlot(u32 i)
{
    struct Task *t;
    struct Task *o;
    void *bestPtr;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 j;

    best = -1;
    bestDist = 0;
    t = &gTasks[i];
    for (j = 0; j < gPlayerCount; j++)
    {
        if ((gActivePlayerMask >> j) & 1)
        {
            o = &gTasks[j];
            d = t->pixelX - o->pixelX;
            if (d < 0)
                d = o->pixelX - t->pixelX;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    t->u80.nearestPlayer = best;
    t->player = bestPtr;
    return best;
}

s32 TaskFindNearestPlayer(void)
{
    struct Task *o;
    void *bestPtr;
    s32 best;
    s32 bestDist;
    s32 d;
    s32 j;

    best = -1;
    bestDist = 0;
    for (j = 0; j < gPlayerCount; j++)
    {
        if ((gActivePlayerMask >> j) & 1)
        {
            o = &gTasks[j];
            d = gCurTask->pixelX - o->pixelX;
            if (d < 0)
                d = o->pixelX - gCurTask->pixelX;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    gCurTask->u80.nearestPlayer = best;
    gCurTask->player = bestPtr;
    return best;
}

s32 GetDistSq(struct PointPair *p)
{
    s16 dx;
    s16 dy;

    dx = p->x0 - p->x1;
    dy = p->y0 - p->y1;
    return dx * dx + dy * dy;
}

s32 GetTaskDistSq(u32 i, u32 j)
{
    struct PointPair p;
    struct Task *a;
    struct Task *b;

    a = &gTasks[i];
    b = &gTasks[j];
    p.x1 = a->pixelX;
    p.y1 = a->pixelY;
    p.x0 = b->pixelX;
    p.y0 = b->pixelY;
    return GetDistSq(&p);
}

s32 TaskGetDistSqTo(u32 i)
{
    return GetTaskDistSq(i, gCurTaskIdx);
}

s32 TaskGetNearestPlayerDistSq(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetDistSqTo(i);
}

s32 GetTaskDx(u32 i, u32 j)
{
    struct Task *a;
    struct Task *b;

    a = &gTasks[i];
    b = &gTasks[j];
    return a->pixelX - b->pixelX;
}

s32 TaskGetDxTo(u32 i)
{
    return GetTaskDx(i, gCurTaskIdx);
}

s32 TaskGetNearestPlayerDx(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetDxTo(i);
}

s32 GetTaskDy(u32 i, u32 j)
{
    struct Task *a;
    struct Task *b;

    a = &gTasks[i];
    b = &gTasks[j];
    return a->pixelY - b->pixelY;
}

s32 TaskGetDyTo(u32 i)
{
    return GetTaskDy(i, gCurTaskIdx);
}

s32 TaskGetNearestPlayerDy(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetDyTo(i);
}

void TaskGetPosSlot(u32 i)
{
    struct Task *t;

    t = &gTasks[i];
    gUnk_030023B4 = t->pixelX;
    gUnk_030023D4 = t->pixelY;
}

void TaskGetNearestPlayerPos(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
    {
        gUnk_030023B4 = gUnk_030023D4 = 0;
    }
    else
    {
        TaskGetPosSlot(i);
    }
}

s32 TaskGetFacingToward(u32 i)
{
    struct Task *t;
    s32 d;
    s32 v;

    t = &gTasks[i];
    d = t->pixelX - gCurTask->pixelX;
    v = 1;
    if (d < 0)
        v = -1;
    return v;
}

void TaskFaceToward(u32 i)
{
    gCurTask->facing = TaskGetFacingToward(i);
}

s32 TaskGetFacingTowardNearestPlayer(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetFacingToward(i);
}

void TaskFaceNearestPlayer(void)
{
    gCurTask->facing = TaskGetFacingTowardNearestPlayer();
}

u8 TaskIsInRect(struct Rect *r)
{
    struct Task *t;

    t = gCurTask;
    if (t->pixelX > r->left && t->pixelX < r->right
        && t->pixelY > r->top && t->pixelY < r->bottom)
        return 1;
    return 0;
}

u8 IsPointInRect(struct Rect *r, s16 x, u16 y)
{
    if (x > r->left && x < r->right
        && (s16)y > r->top && (s16)y < r->bottom)
        return 1;
    return 0;
}

s32 TaskIsInRectSlot(struct Rect *r, u32 i)
{
    struct Task *t;

    t = &gTasks[i];
    if (t->pixelX > r->left && t->pixelX < r->right
        && t->pixelY > r->top && t->pixelY < r->bottom)
        return 1;
    return 0;
}

s32 TaskIsNearestPlayerInRect(struct Rect *r)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskIsInRectSlot(r, i);
}

/* Tear down task `i`: run the actor's teardown hook, release its child task
   and its slot, then either re-dispatch (if it killed itself) or free it. */
void ActorDestroySlot(s32 i)
{
    struct Task *t;
    struct Actor *a;

    if (i <= 31)
        return;
    t = &gTasks[i];
    a = t->u8C.actor;
    if (a != NULL && (u8)(t->actorKind - 7) > 3)
    {
        if (a->teardown != NULL)
            a->teardown();
        if (a->attachedTask != -1)
        {
            TaskFree(a->attachedTask);
            a->attachedTask = 0xFFFF;
        }
        switch (t->actorKind)
        {
        case 0:
            ReleaseRoomObject(i);
            break;
        case 6:
            if (gCurTask->u76.subtype != 0)
                ReleaseRoomObject(i);
            break;
        }
    }
    gTaskSkipMaskStack[0][i] = gTaskSkipMaskStack[1][i] = 0;
    if (i == gCurTaskIdx && gTaskRunPhase == 1)
        TaskExitTrampoline();
    else
        TaskFree(i);
}

void ActorDestroy(void)
{
    ActorDestroySlot(gCurTaskIdx);
}

void TaskTurnAroundAndReverseX(void)
{
    if (gCurTask->facing == 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    TaskSetFrame(gCurTask->frame);
    TaskSetMotionX(-gCurTask->velX, -gCurTask->accelX,
                 gCurTask->speedLimitX);
}

void TaskTurnAround(void)
{
    if (gCurTask->facing == 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    TaskSetFrame(gCurTask->frame);
}

void TaskToggleFacingAndReverseX(void)
{
    if (gCurTask->facing == 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    TaskSetMotionX(-gCurTask->velX, -gCurTask->accelX,
                 gCurTask->speedLimitX);
}

s32 ActorStartAnimNoFlip(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    a->animScript = p;
    a->animScriptPos = 0;
    a->animNoFlip = 1;
    return ActorStepAnim();
}

void ActorStopAnim(void)
{
    gCurTask->u8C.actor->animScript = NULL;
}

s32 ActorStartAnim(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    a->animScript = p;
    a->animScriptPos = 0;
    a->animNoFlip = 0;
    return ActorStepAnim();
}

/* Step the running actor's animation script; returns the new delay. */
/* Step the running actor's animation script; returns the new delay. */
/* Step the running actor's animation script; returns the new delay. */
s32 ActorStepAnim(void)
{
    struct Actor *a;
    struct AnimCmd *p;
    s32 delay;
    s32 cmd;

    a = gCurTask->u8C.actor;
    p = a->animScript;
    p += a->animScriptPos;
    cmd = p->frame;
    if (cmd == -3)
    {
        a->animScriptPos = 0;
        p = a->animScript;
    }
    else if (cmd == -2)
    {
        delay = cmd;
        a->animScript = NULL;
        a->animScriptPos++;
        return delay;
    }
    if (a->animNoFlip != 0)
        gCurTask->frame = p->frame;
    else
        TaskSetFrame(p->frame);
    delay = p->delay;
    a->animScriptPos++;
    return delay;
}

s32 ActorTickAnimFacingNearestPlayer(s32 delay)
{
    if (gCurTask->u8C.actor->animScript != NULL)
    {
        if (delay <= 0)
        {
            TaskFaceNearestPlayer();
            delay = ActorStepAnim();
        }
        delay--;
    }
    return delay;
}

s32 ActorTickAnim(s32 delay)
{
    if (gCurTask->u8C.actor->animScript != NULL)
    {
        if (delay <= 0)
            delay = ActorStepAnim();
        delay--;
    }
    return delay;
}

/* Dead export: split `mag` into the trig-table components of the angle from
   (x0, y0) to (x1, y1) and leave them in gUnk_030023B4/gUnk_030023D4. */
u32 sub_080641b0(s16 x0, s16 y0, s16 x1, s16 y1, u16 mag)
{
    u32 t;

    t = (u16)ArcTan2(x1 - x0, y1 - y0) >> 7;
    gUnk_030023B4 = *(gCosTable + t) * (s16)mag;
    gUnk_030023D4 = *(gCosTable - 128 + t) * (s16)mag;
    return t;
}

void AngleToVector(s16 t, s16 mag)
{
    gUnk_030023B4 = *(gCosTable + t) * mag;
    gUnk_030023D4 = *(gCosTable - 128 + t) * mag;
}

/* Angle from (x0, y0) to (x1, y1), narrowed to `prec` steps of resolution. */
u16 GetPointAngle(s16 x0, s16 y0, s16 x1, s16 y1, s32 prec)
{
    u16 a;

    a = ArcTan2(x1 - x0, y1 - y0);
    switch (prec)
    {
    case 0:
        a >>= 1;
    case 1:
        a >>= 4;
    case 2:
        a >>= 1;
    case 3:
        a >>= 7;
    }
    return a;
}

u16 GetTaskAngle(u32 i, u32 j, s32 prec)
{
    struct Task *a;
    struct Task *b;

    a = &gTasks[i];
    b = &gTasks[j];
    return GetPointAngle(b->pixelX, b->pixelY, a->pixelX, a->pixelY, prec);
}

u16 TaskGetAngleTo(u32 i, s32 prec)
{
    return GetTaskAngle(i, gCurTaskIdx, prec);
}

u16 TaskGetAngleToNearestPlayer(s32 prec)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetAngleTo(i, prec);
}

u8 TaskGetYDirBitTo(u32 i)
{
    s32 d;

    d = TaskGetDyTo(i);
    if (d == 0)
        return 0;
    if (d < 0)
        return 2;
    return 1;
}

u8 TaskGetYDirBitToNearestPlayer(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetYDirBitTo(i);
}

u8 TaskGetXDirBitTo(u32 i)
{
    s32 d;

    d = TaskGetDxTo(i);
    if (d == 0)
        return 0;
    if (d < 0)
        return 8;
    return 4;
}

u8 TaskGetXDirBitToNearestPlayer(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetXDirBitTo(i);
}

/* Combine the two axis signs into one of the 8 compass directions. */
u8 TaskGetCompassDirTo(u32 i)
{
    u8 dir;

    dir = TaskGetYDirBitTo(i);
    dir |= TaskGetXDirBitTo(i);
    switch (dir)
    {
    case 5:
        dir = 1;
        break;
    case 9:
        dir = 2;
        break;
    case 6:
        dir = 0;
        break;
    case 10:
        dir = 3;
        break;
    case 1:
        dir = 5;
        break;
    case 2:
        dir = 4;
        break;
    case 4:
        dir = 7;
        break;
    case 8:
        dir = 6;
        break;
    default:
        dir = 8;
        break;
    }
    return dir;
}

u8 TaskGetCompassDirToNearestPlayer(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    return TaskGetCompassDirTo(i);
}

/* Accelerate the running task towards +limit on the given axis. */
/* Accelerate the running task towards +limit on the given axis. */
void TaskAccelerateAxisPlus(s32 step, s32 limit, u8 axis)
{
    struct Task *t;

    if (axis == 0)
    {
        t = gCurTask;
        if (t->velX < 0)
        {
            t->velX += step;
            gUnk_030023B4 = 1;
        }
        else if (t->velX < limit)
        {
            t->velX += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->velX = limit;
        }
    }
    else
    {
        t = gCurTask;
        if (t->velY < 0 || t->velY < limit)
        {
            t->velY += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->velY = limit;
        }
    }
}

/* Accelerate the running task towards -limit on the given axis. */
/* Accelerate the running task towards -limit on the given axis. */
void TaskAccelerateAxisMinus(s32 step, s32 limit, u8 axis)
{
    struct Task *t;
    s32 v;

    if (axis == 0)
    {
        t = gCurTask;
        if (t->velX > 0)
        {
            t->velX -= step;
            gUnk_030023B4 = -1;
        }
        else if (abs(t->velX) < limit)
        {
            t->velX -= step;
            gUnk_030023B4 = -1;
        }
        else
        {
            t->velX = -limit;
        }
    }
    else
    {
        t = gCurTask;
        v = t->velY;
        if (v > 0 || abs(v) < limit)
        {
            t->velY = v - step;
            gUnk_030023D4 = -1;
        }
        else
        {
            t->velY = -limit;
        }
    }
}

/* Decay the running task's speed towards zero on the given axis. */
void TaskDecelerateAxis(s32 step, u8 axis)
{
    struct Task *t;

    if (axis == 0)
    {
        t = gCurTask;
        if (t->velX == 0)
            return;
        if (t->velX < 0)
        {
            t->velX += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->velX -= step;
            gUnk_030023B4 = -1;
        }
    }
    else
    {
        t = gCurTask;
        if (t->velY == 0)
            return;
        if (t->velY < 0)
        {
            t->velY += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->velY -= step;
            gUnk_030023D4 = -1;
        }
    }
}

/* Steer the running task towards the nearest player. */
void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit)
{
    gUnk_030023B4 = gUnk_030023D4 = 0;
    switch (TaskGetAngleToNearestPlayer(0))
    {
    case 0:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskDecelerateAxis(step, 1);
        break;
    case 1:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 2:
        TaskDecelerateAxis(step, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 3:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 4:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskDecelerateAxis(step, 1);
        break;
    case 5:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    case 6:
        TaskDecelerateAxis(step, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    case 7:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    }
}

/* TaskAccelerateTowardNearestPlayer with the direction supplied by the caller. */
void TaskAccelerateInDir(s32 step, s32 limit, u16 dir)
{
    gUnk_030023B4 = gUnk_030023D4 = 0;
    switch (dir)
    {
    case 0:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskDecelerateAxis(step, 1);
        break;
    case 1:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 2:
        TaskDecelerateAxis(step, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 3:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskAccelerateAxisPlus(step, limit, 1);
        break;
    case 4:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskDecelerateAxis(step, 1);
        break;
    case 5:
        TaskAccelerateAxisMinus(step, limit, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    case 6:
        TaskDecelerateAxis(step, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    case 7:
        TaskAccelerateAxisPlus(step, limit, 0);
        TaskAccelerateAxisMinus(step, limit, 1);
        break;
    }
}

/* Nearest active player whose X offset falls inside [lo, hi). */
s32 TaskFindNearestPlayerInScreenXBand(u16 lo, u16 hi)
{
    s32 best;
    s32 bestDist;
    s32 d;
    s32 v;
    s32 found;
    s32 i;

    best = -1;
    found = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            TaskGetScreenPosSlot(i);
            v = gUnk_030023B4;
            if (v >= (s16)lo && v < (s16)hi)
            {
                found = 1;
                if (best == -1)
                {
                    best = i;
                    bestDist = TaskGetDistSqTo(best);
                }
                else
                {
                    d = TaskGetDistSqTo(i);
                    if (bestDist > d)
                    {
                        bestDist = d;
                        best = i;
                    }
                }
            }
        }
    }
    gUnk_030023D4 = best;
    return found;
}

/* Same over the Y offset. */
s32 TaskFindNearestPlayerInScreenYBand(u16 lo, u16 hi)
{
    s32 best;
    s32 bestDist;
    s32 d;
    s32 v;
    s32 found;
    s32 i;

    best = -1;
    found = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            TaskGetScreenPosSlot(i);
            v = gUnk_030023D4;
            if (v >= (s16)lo && v < (s16)hi)
            {
                found = 1;
                if (best == -1)
                {
                    best = i;
                    bestDist = TaskGetDistSqTo(best);
                }
                else
                {
                    d = TaskGetDistSqTo(i);
                    if (bestDist > d)
                    {
                        bestDist = d;
                        best = i;
                    }
                }
            }
        }
    }
    gUnk_030023D4 = best;
    return found;
}

/* Screen-space offset of task `i` from the camera, into gUnk_030023B4/D4. */
void TaskGetScreenPosSlot(u32 i)
{
    struct Task *t;

    t = &gTasks[i];
    if (gPlayerCount == 1 || gActivePlayerCount == 1)
    {
        gUnk_030023B4 = t->pixelX - gSpriteCameraX;
        gUnk_030023D4 = t->pixelY - gSpriteCameraY;
    }
    else
    {
        gUnk_030023B4 = t->pixelX - gViewRect[0];
        gUnk_030023D4 = t->pixelY - gViewRect[2];
    }
}

s32 TaskGetNearestPlayerScreenPos(void)
{
    s32 i;

    i = TaskFindNearestPlayer();
    if (i != -1)
        TaskGetScreenPosSlot(i);
    else
        gUnk_030023B4 = gUnk_030023D4 = 0;
    return i;
}

void TaskGetScreenPos(void)
{
    TaskGetScreenPosSlot(gCurTaskIdx);
}

s32 TaskIsNearestPlayerWithinX(s32 range)
{
    s32 i;
    s32 d;

    i = TaskFindNearestPlayer();
    if (i == -1)
        return 0;
    d = TaskGetDxTo(i);
    if (abs(d) <= range)
        return 1;
    return 0;
}

void ActorAwardScore(u32 arg, s32 mul)
{
    struct Task *t;
    struct Actor *a;
    s32 v;
    u32 k;

    t = gCurTask;
    a = t->u8C.actor;
    if (gGameState == 18)
        return;
    v = a->score;
    if (t->actorKind == 0 && t->u76.subtype == 37)
    {
        k = gFrameCount & 3;
        v = gUnk_0873DF14[k];
    }
    v *= mul;
    if (gCurTask->actorKind == 0 && gCurTask->u76.subtype == 40
        && (u8)(a->unk04 - 2) <= 1)
        v = 200;
    AddPlayerScore(v, arg);
}

s8 TaskGetParentFacing(void)
{
    struct Task *t;
    s32 i;

    i = gCurTask->parent;
    t = &gTasks[i];
    return t->facing;
}

void TaskFaceLikeParent(void)
{
    gCurTask->facing = TaskGetParentFacing();
}

/* Spawn a class-4 task from a descriptor; returns its slot or -1. */
s32 sub_08064a78(struct ActorSpawn *p)
{
    struct Task *t;
    s32 i;

    if (p->checkTerrain == 1)
    {
        if (GetShapeAtPixelIgnoringOneWay(p->x, p->y) != 0)
            return -1;
    }
    i = TaskCreatePausedInScreenAttack(p->taskType, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->actorKind = 4;
        t->u76.subtype = p->subtype;
        t->variant = p->variant;
        t->unk74 = p->spawnArg;
        t->pixelX = p->x;
        t->pixelY = p->y;
        t->posX = p->x << 16;
        t->posY = p->y << 16;
        t->parent = gCurTaskIdx;
        t->tileWord = p->tileWord;
        t->u8C.actor = &gActors[i];
        ActorInitSlot(i);
    }
    return i;
}

s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gCurTask;
    p->x = t->pixelX;
    p->y = t->pixelY;
    if (keepPrio == 0)
        p->tileWord = t->tileWord;
    return sub_08064a78(p);
}

s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gCurTask;
    p->x = t->pixelX + p->x * t->facing;
    p->y += t->pixelY;
    if (keepPrio == 0)
        p->tileWord = t->tileWord;
    return sub_08064a78(p);
}

s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio)
{
    if (keepPrio == 0)
        p->tileWord = gCurTask->tileWord;
    return sub_08064a78(p);
}

/* Cycle the running task's frame between 4 and 7 every other tick. */
/* Cycle the running task's frame between 4 and 7 every other tick. */
void TaskStepSpinFrameFacing(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->actorSpinFrameTimer <= 0)
    {
        if (t->facing == 1)
        {
            t->frame++;
            if (t->frame > 7)
                t->frame = 4;
        }
        else
        {
            t->frame--;
            if (t->frame <= 3)
                t->frame = 7;
        }
        gCurTask->actorSpinFrameTimer = 2;
    }
    else
    {
        t->actorSpinFrameTimer--;
    }
}

/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class. */
/* Spawn a helper task at (x, y) inheriting the running task's class.
   The three 16-bit arguments are declared `int` and narrowed into u16
   locals: CreateChildTaskAtOffsetFacing passes them sign-extended, so the ROM's call site
   never converts (issue #65 lessons). */
s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg)
{
    struct Task *t;
    s32 i;
    u16 x = xArg;
    u16 y = yArg;
    u16 prio = prioArg;

    i = TaskCreatePausedInScreenAttack(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        switch (gCurTask->actorKind)
        {
        case 2:
        case 7:
            t->actorKind = 7;
            break;
        case 1:
        case 8:
            t->actorKind = 8;
            break;
        default:
            t->actorKind = 9;
            break;
        }
        t->pixelX = x;
        t->pixelY = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->parent = gCurTaskIdx;
        t->tileWord = prio;
        t->health = 2;
        gTasks[i].serial = gTasks[gCurTaskIdx].serial;
    }
    return i;
}

s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio)
{
    struct Task *t;
    s32 x;
    s32 y;
    u16 prio;

    t = gCurTask;
    x = (s16)(t->pixelX + t->facing * dx);
    y = (s16)(dy + t->pixelY);
    if (keepPrio != 0)
        prio = t->tileWord;
    else
        prio = 0;
    return CreateChildTask(type, x, y, prio);
}

s32 CreateChildTaskHere(u32 type, u8 keepPrio)
{
    struct Task *t;
    s32 x;
    s32 y;
    u16 prio;

    t = gCurTask;
    x = t->pixelX;
    y = t->pixelY;
    if (keepPrio != 0)
        prio = t->tileWord;
    else
        prio = 0;
    return CreateChildTask(type, x, y, prio);
}

s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio)
{
    s32 x = xArg;
    s32 y = yArg;
    u16 prio;

    if (keepPrio != 0)
        prio = gCurTask->tileWord;
    else
        prio = 0;
    return CreateChildTask(type, x, y, prio);
}

/* Spawn a class-5/6 task; the 16-bit arguments are `int` for the same
   reason as CreateChildTask's. */
s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg,
                 int prioArg, int altArg)
{
    struct Task *t;
    s32 i;
    u8 p2 = p2Arg;
    u16 x = xArg;
    u16 y = yArg;
    u16 prio = prioArg;
    u8 alt = altArg;

    i = TaskCreatePausedInScreenAttack(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        if (alt != 0)
            t->actorKind = 6;
        else
            t->actorKind = 5;
        t->u76.subtype = sub;
        t->variant = 0;
        t->unk74 = p2;
        t->pixelX = x;
        t->pixelY = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->parent = gCurTaskIdx;
        t->tileWord = prio;
        t->u8C.actor = &gActors[i];
        ActorInitSlot(i);
    }
    return i;
}

s32 sub_08064e5c(u32 sub, u32 type, u8 p2)
{
    struct Task *t;

    t = gCurTask;
    return sub_08064d9c(sub, type, p2, t->pixelX, t->pixelY, 0, 1);
}

s32 sub_08064e90(u32 sub, u32 type, u8 p2, s16 xArg, s16 yArg)
{
    s32 x = xArg;
    s32 y = yArg;

    return sub_08064d9c(sub, type, p2, x, y, 0, 1);
}

s32 CreateAbilityStar(u8 p2)
{
    struct Task *t;
    struct Actor *a;
    struct PlayerState *p;
    s32 i;

    i = sub_08064e5c(0, TASK_ABILITY_STAR, p2);
    if (i != -1)
    {
        t = &gTasks[i];
        t->player = p = &gPlayerStates[gCurTaskIdx];
        a = t->u8C.actor;
        t->unk18 = p->ability;
        t->unk1C = p->abilityUses;
        t->unk20 = gCurTaskIdx;
        a->ability = p->ability;
    }
    CreateChildTaskHere(TASK_ABILITY_RELEASE_FLASH, 0);
    return i;
}

/* Generic task spawn: every field of the new task comes from an argument. */
s32 CreateActor(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio)
{
    struct Task *t;
    s32 i;

    i = TaskCreatePausedInScreenAttack(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->actorKind = cls;
        t->u76.subtype = sub;
        t->variant = p3;
        t->unk74 = p4;
        t->pixelX = x;
        t->pixelY = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->tileWord = prio;
        t->u8C.actor = &gActors[i];
        ActorInitSlot(i);
    }
    return i;
}

/* Pick the task type for `cls` out of one of five ROM tables. */
/* Pick the task type for `cls` out of one of five ROM tables. */
s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio)
{
    u32 type;

    switch (cls)
    {
    case 0:
        type = gEnemyTaskTypes[sub];
        break;
    case 1:
    case 3:
        type = gMidBossTaskTypes[sub];
        break;
    case 2:
        type = gBossTaskTypes[sub];
        break;
    case 5:
        type = gUnk_0873F288[sub];
        break;
    case 6:
        type = gUnk_0873F2A0[sub];
        break;
    default:
        while (1)
            ;
    }
    return CreateActor(cls, sub, type, p3, p4, x, y, prio);
}

/* Clone the running task's class/sub into a fresh task. */
s32 sub_0806505c(u8 p3, u8 p4, u32 x, u32 y, u16 prio)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    struct Actor *b;
    s32 i;

    t = gCurTask;
    a = t->u8C.actor;
    i = CreateActor(t->actorKind, t->u76.subtype, gTaskSlotTypes[gCurTaskIdx], p3, p4,
                     x, y, prio);
    if (i != -1)
    {
        u = &gTasks[i];
        b = u->u8C.actor;
        u->parent = gCurTaskIdx;
        b->gfx = a->gfx;
    }
    return i;
}

s32 CreateBlockStar(s16 x, s16 y, u32 p2, u8 p3, u8 p4)
{
    struct Task *t;
    s32 i;

    i = CreateActor(0, 40, TASK_BLOCK_STAR, 0, 0, x, y, 0);
    if (i != -1)
    {
        t = &gTasks[i];
        t->hitKind = p3;
        t->hitEffect = p4;
        t->hitterSlot = p2;
    }
    return i;
}

/* Is the running task inside the 64px-padded camera window? */
u8 ActorIsInView(void)
{
    if (gViewRect[0] - 64 < gCurTask->pixelX
        && gCurTask->pixelX < gViewRect[1] + 64
        && gViewRect[2] - 64 < gCurTask->pixelY
        && gCurTask->pixelY < gViewRect[3] + 64)
        return 1;
    return 0;
}

void ActorDrawWorldInView(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->frameTable;
    QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                 t->pixelX - gSpriteCameraX,
                 (s16)(t->pixelY - gSpriteCameraY));
}

void ActorDrawWorldInViewOrDestroy(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        t = gCurTask;
        tbl = t->frameTable;
        QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - gSpriteCameraX,
                     (s16)(t->pixelY - gSpriteCameraY));
    }
    else
    {
        ActorDestroy();
    }
}

void sub_080652c8(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInNearView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->frameTable;
    QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                 t->pixelX - gSpriteCameraX,
                 (s16)(t->pixelY - gSpriteCameraY));
}

void sub_08065350(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInNearView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        t = gCurTask;
        tbl = t->frameTable;
        QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - gSpriteCameraX,
                     (s16)(t->pixelY - gSpriteCameraY));
    }
    else if (ActorIsInFarView() != 0)
    {
        HudRemoveHpBar();
        ActorDestroy();
    }
}
