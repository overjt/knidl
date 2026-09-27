/* game_code_and_rodata 0x08063698-0x080653EC (issue #65, module M17 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08063698 0x080653EC src/actor_63698.c --newpb
 *
 * Actor lifecycle and geometry: binding a task to its ROM descriptor
 * (sub_08063704/ActorLoadDefSlot), resetting the actor record (sub_080637E4),
 * 16.16 position/velocity accessors, ArcTan2 aiming, rectangle and distance
 * queries, animation-script walking, and the spawn helpers
 * (sub_08064A78/CreateChildTask/CreateActor) every later module calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02006178;
extern u8 gTaskSkipMaskStack[][64];
extern u32 gUnk_02007F50;
extern u16 gNextActorSerial;
extern u8 gActivePlayerMask;
extern vs16 gTaskSlotTypes[];
extern u16 gPlayerCount;
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

extern struct ActorDef *gUnk_0873ECEC[];
extern struct ActorDef *gUnk_0873ED90[];
extern struct ActorDef *gUnk_0873EDB8[];
extern struct ActorDef *gUnk_0873EDDC[];
extern struct ActorDef *gUnk_0873EE70[];
extern struct ActorDef *gUnk_0873EE88[];

extern s32 TaskCreateInRange(u32 type, s32 start, s32 end);
extern void sub_08065ce0(u32 i);
extern u16 ActorComputeHealth(u32 a);
extern u16 ActorComputeHealthSlot(u32 i);
extern void sub_08069ac4(u32 i);
extern void sub_080b54a4(u32 i);
extern void TaskFree(s32 id);
extern void TaskSetFrame(s32 a);
extern void TaskSetMotionX(s32 a, s32 b, s32 c);
extern void TaskExitTrampoline(void);
extern s32 gTaskRunPhase;
extern s16 gCosTable[];
extern u8 gActivePlayerCount;
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s16 gViewRect[];
extern u16 gGameState;
extern u16 gFrameCount;
extern s32 gUnk_0873DF14[];
extern struct Actor gActors[];
extern struct PlayerState gPlayerStates[];
extern u32 gUnk_0873F198[];
extern u32 gUnk_0873F23C[];
extern u32 gUnk_0873F264[];
extern u32 gUnk_0873F288[];
extern u32 gUnk_0873F2A0[];
extern void AddPlayerScore(s32 a, u32 b);
extern s32 sub_08021a40(s16 x, s16 y);
extern u32 TaskIsOnScreen(void);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern u8 sub_08066a6c(void);
extern u8 sub_08066a80(void);
extern void HudRemoveHpBar(void);

void ActorLoadDefSlot(u32 i, struct ActorDef *d);
void ActorSetAttackBoxSlot(u32 i, u32 v);
void sub_08063a14(u32 i, u32 v);
void sub_08063704(u32 i);
void ActorResetHealthSlot(u32 i);
void sub_080637e4(u32 i);
s32 sub_08063a9c(u32 i);
s32 TaskFindNearestPlayer(void);
s32 GetDistSq(struct PointPair *p);
s32 GetTaskDistSq(u32 i, u32 j);
s32 TaskGetDistSqTo(u32 i);
s32 GetTaskDx(u32 i, u32 j);
s32 TaskGetDxTo(u32 i);
s32 GetTaskDy(u32 i, u32 j);
s32 TaskGetDyTo(u32 i);
void TaskGetPosSlot(u32 i);
s32 TaskGetFacingToward(u32 i);
s32 TaskGetFacingTowardNearestPlayer(void);
s32 TaskIsInRectSlot(struct Rect *r, u32 i);
void ActorDestroySlot(s32 i);
void TaskFaceNearestPlayer(void);
s32 ActorStepAnim(void);
u16 GetPointAngle(s16 x0, s16 y0, s16 x1, s16 y1, s32 prec);
u16 GetTaskAngle(u32 i, u32 j, s32 prec);
u16 TaskGetAngleTo(u32 i, s32 prec);
u8 TaskGetYDirBitTo(u32 i);
u8 TaskGetXDirBitTo(u32 i);
u8 TaskGetCompassDirTo(u32 i);
u16 TaskGetAngleToNearestPlayer(s32 prec);
void TaskAccelerateAxisPlus(s32 step, s32 limit, u8 axis);
void TaskAccelerateAxisMinus(s32 step, s32 limit, u8 axis);
void TaskDecelerateAxis(s32 step, u8 axis);
void TaskGetScreenPosSlot(u32 i);
s8 TaskGetParentFacing(void);
void ActorInitSlot(u32 i);
s32 sub_08064a78(struct ActorSpawn *p);
s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg);
s32 CreateChildTaskHere(u32 type, u8 keepPrio);
s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg, int prioArg,
                 int altArg);
s32 sub_08064e5c(u32 sub, u32 type, u8 p2);
s32 CreateActor(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio);
u8 ActorIsInView(void);
void ActorDestroy(void);

s32 sub_08063698(u32 type, s32 start)
{
    s32 i;
    struct Task *t;

    i = TaskCreateInRange(type, start, 62);
    if (i == -1)
        return i;
    t = &gTasks[i];
    if (gUnk_02006178 != 1)
        return i;
    gTaskSkipMaskStack[0][i] = gTaskSkipMaskStack[1][i] = 0;
    t->skipMask = 15;
    return i;
}

void ActorInitSlot(u32 i)
{
    sub_08063704(i);
    sub_080637e4(i);
    sub_08063a9c(i);
    sub_08069ac4(i);
}

void sub_08063704(u32 i)
{
    struct Task *t;
    struct Actor *a;

    t = &gTasks[i];
    a = t->unk8C;
    a->unk44 = NULL;
    switch (t->unk72)
    {
    case 0:
        a->unk44 = gUnk_0873ECEC[t->unk76];
        break;
    case 1:
    case 3:
        a->unk44 = gUnk_0873ED90[t->unk76];
        break;
    case 2:
        a->unk44 = gUnk_0873EDB8[t->unk76];
        gUnk_02007F50 = t->unk76;
        break;
    case 4:
        a->unk44 = gUnk_0873EDDC[t->unk76];
        break;
    case 5:
        a->unk44 = gUnk_0873EE70[t->unk76];
        break;
    default:
        a->unk44 = gUnk_0873EE88[t->unk76];
        break;
    }
}

void ActorResetHealthSlot(u32 i)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk78 = ActorComputeHealthSlot(i);
}

void sub_080637cc(u32 a)
{
    gCurTask->unk78 = ActorComputeHealth(a);
}

void sub_080637e4(u32 i)
{
    struct Task *t;
    struct Actor *a;
    struct ActorDef *d;

    t = &gTasks[i];
    a = t->unk8C;
    a->unk1C = 0xFFFF;
    a->unk04 = 0;
    a->unk05 = 0;
    a->unk06 = 0;
    a->unk07 = 0;
    a->unk2C = 0;
    a->unk01 = 0;
    a->unk08 = 0;
    a->unk09 = 0;
    a->unk0A = 0;
    a->unk0B = 0;
    a->unk0E = -1;
    a->unk10 = -1;
    a->unk12 = 0xFFFE;
    a->unk22 = t->unk40 & 0xF000;
    a->unk24 = 16;
    a->unk28 = 0;
    a->unk0C = t->unk73 >> 4;
    t->unk73 = t->unk73 & 15;
    if (a->unk0C != 0)
        sub_08065ce0(i);
    t->unk16 = gNextActorSerial;
    if (gNextActorSerial > 0xFFFE)
        gNextActorSerial = 0;
    else
        gNextActorSerial = gNextActorSerial + 1;
    a->unk3C = 0;
    a->unk34 = -1;
    a->unk38 = -1;
    a->unk0D = 0;
    a->unk18 = 0;
    a->unk16 = 0;
    a->unk03 = 0;
    a->unk1E = 0;
    a->unk1A = 0xFFFF;
    a->unk02 = 0;
    d = a->unk44;
    if (d != NULL)
    {
        a->unk48 = d->unk14;
        a->unk4C = 0;
        a->unk50 = d->unk18;
        a->unk58 = a->unk54 = d->unk1C;
        a->unk5C = d->unk20;
        a->unk00 = d->unk0C;
        a->unk30 = d->unk08;
        ActorResetHealthSlot(i);
        if (a->unk44->unk24 != NULL)
            a->unk44->unk24(i);
        a->unk40 = a->unk44->unk28;
        a->unk60 = a->unk44->unk10;
    }
    else
    {
        a->unk48 = 0;
        a->unk4C = 0;
        a->unk50 = 0;
        a->unk54 = 0;
        a->unk58 = 0;
        a->unk5C = 0;
        a->unk00 = 0;
        a->unk30 = 0;
        t->unk78 = 0;
        a->unk60 = 0;
        a->unk40 = 0;
    }
}

void ActorLoadDef(struct ActorDef *d)
{
    ActorLoadDefSlot(gCurTaskIdx, d);
}

void ActorLoadDefSlot(u32 i, struct ActorDef *d)
{
    struct Actor *a;

    a = (&gTasks[i])->unk8C;
    a->unk44 = d;
    a->unk48 = d->unk14;
    a->unk50 = d->unk18;
    a->unk58 = a->unk54;
    a->unk54 = d->unk1C;
    a->unk5C = d->unk20;
    a->unk00 = d->unk0C;
    a->unk30 = d->unk08;
    a->unk40 = d->unk28;
    ActorResetHealthSlot(i);
}

void ActorSetState(u8 v)
{
    struct Task *t;

    t = gCurTask;
    t->unk8C->unk1C = t->unk14;
    t->unk14 = v;
}

void ActorSetStateSlot(u32 i, u8 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk8C->unk1C = t->unk14;
    t->unk14 = v;
}

void ActorSetTerrainHandlers(u32 v)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    a->unk58 = a->unk54;
    a->unk54 = v;
}

void ActorSetHitReactions(u32 v)
{
    gCurTask->unk8C->unk5C = v;
}

void ActorSetAttackBox(u32 v)
{
    ActorSetAttackBoxSlot(gCurTaskIdx, v);
}

void ActorSetAttackBoxSlot(u32 i, u32 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk8C->unk48 = v;
}

void ActorSetTerrainBox(u32 v)
{
    gCurTask->unk8C->unk50 = v;
}

void sub_080639f0(struct ActorAux *v)
{
    gCurTask->unk8C->unk60 = v;
}

void sub_08063a00(u32 v)
{
    sub_08063a14(gCurTaskIdx, v);
}

void sub_08063a14(u32 i, u32 v)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk8C->unk4C = v;
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
            d = gCurTask->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - gCurTask->unk48;
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
s32 sub_08063a9c(u32 i)
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
            d = t->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - t->unk48;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    t->unk80 = best;
    t->unk88 = bestPtr;
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
            d = gCurTask->unk48 - o->unk48;
            if (d < 0)
                d = o->unk48 - gCurTask->unk48;
            if (best == -1 || bestDist > d)
            {
                bestDist = d;
                best = j;
                bestPtr = o;
            }
        }
    }
    gCurTask->unk80 = best;
    gCurTask->unk88 = bestPtr;
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
    p.x1 = a->unk48;
    p.y1 = a->unk4A;
    p.x0 = b->unk48;
    p.y0 = b->unk4A;
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
    return a->unk48 - b->unk48;
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
    return a->unk4A - b->unk4A;
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
    gUnk_030023B4 = t->unk48;
    gUnk_030023D4 = t->unk4A;
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
    d = t->unk48 - gCurTask->unk48;
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
    if (t->unk48 > r->left && t->unk48 < r->right
        && t->unk4A > r->top && t->unk4A < r->bottom)
        return 1;
    return 0;
}

u8 sub_08063e74(struct Rect *r, s16 x, u16 y)
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
    if (t->unk48 > r->left && t->unk48 < r->right
        && t->unk4A > r->top && t->unk4A < r->bottom)
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
    a = t->unk8C;
    if (a != NULL && (u8)(t->unk72 - 7) > 3)
    {
        if (a->unk40 != NULL)
            a->unk40();
        if (a->unk10 != -1)
        {
            TaskFree(a->unk10);
            a->unk10 = 0xFFFF;
        }
        switch (t->unk72)
        {
        case 0:
            sub_080b54a4(i);
            break;
        case 6:
            if (gCurTask->unk76 != 0)
                sub_080b54a4(i);
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
    TaskSetMotionX(-gCurTask->unk54, -gCurTask->unk5C,
                 gCurTask->unk64);
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
    TaskSetMotionX(-gCurTask->unk54, -gCurTask->unk5C,
                 gCurTask->unk64);
}

s32 ActorStartAnimNoFlip(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    a->unk2C = p;
    a->unk09 = 0;
    a->unk08 = 1;
    return ActorStepAnim();
}

void ActorStopAnim(void)
{
    gCurTask->unk8C->unk2C = NULL;
}

s32 ActorStartAnim(struct AnimCmd *p)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    a->unk2C = p;
    a->unk09 = 0;
    a->unk08 = 0;
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

    a = gCurTask->unk8C;
    p = a->unk2C;
    p += a->unk09;
    cmd = p->unk00;
    if (cmd == -3)
    {
        a->unk09 = 0;
        p = a->unk2C;
    }
    else if (cmd == -2)
    {
        delay = cmd;
        a->unk2C = NULL;
        a->unk09++;
        return delay;
    }
    if (a->unk08 != 0)
        gCurTask->frame = p->unk00;
    else
        TaskSetFrame(p->unk00);
    delay = p->unk02;
    a->unk09++;
    return delay;
}

s32 ActorTickAnimFacingNearestPlayer(s32 n)
{
    if (gCurTask->unk8C->unk2C != NULL)
    {
        if (n <= 0)
        {
            TaskFaceNearestPlayer();
            n = ActorStepAnim();
        }
        n--;
    }
    return n;
}

s32 ActorTickAnim(s32 n)
{
    if (gCurTask->unk8C->unk2C != NULL)
    {
        if (n <= 0)
            n = ActorStepAnim();
        n--;
    }
    return n;
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
    return GetPointAngle(b->unk48, b->unk4A, a->unk48, a->unk4A, prec);
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
        if (t->unk54 < 0)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else if (t->unk54 < limit)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->unk54 = limit;
        }
    }
    else
    {
        t = gCurTask;
        if (t->unk58 < 0 || t->unk58 < limit)
        {
            t->unk58 += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->unk58 = limit;
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
        if (t->unk54 > 0)
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
        else if (abs(t->unk54) < limit)
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
        else
        {
            t->unk54 = -limit;
        }
    }
    else
    {
        t = gCurTask;
        v = t->unk58;
        if (v > 0 || abs(v) < limit)
        {
            t->unk58 = v - step;
            gUnk_030023D4 = -1;
        }
        else
        {
            t->unk58 = -limit;
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
        if (t->unk54 == 0)
            return;
        if (t->unk54 < 0)
        {
            t->unk54 += step;
            gUnk_030023B4 = 1;
        }
        else
        {
            t->unk54 -= step;
            gUnk_030023B4 = -1;
        }
    }
    else
    {
        t = gCurTask;
        if (t->unk58 == 0)
            return;
        if (t->unk58 < 0)
        {
            t->unk58 += step;
            gUnk_030023D4 = 1;
        }
        else
        {
            t->unk58 -= step;
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
        gUnk_030023B4 = t->unk48 - gSpriteCameraX;
        gUnk_030023D4 = t->unk4A - gSpriteCameraY;
    }
    else
    {
        gUnk_030023B4 = t->unk48 - gViewRect[0];
        gUnk_030023D4 = t->unk4A - gViewRect[2];
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

s32 sub_08064984(s32 range)
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
    a = t->unk8C;
    if (gGameState == 18)
        return;
    v = a->unk30;
    if (t->unk72 == 0 && t->unk76 == 37)
    {
        k = gFrameCount & 3;
        v = gUnk_0873DF14[k];
    }
    v *= mul;
    if (gCurTask->unk72 == 0 && gCurTask->unk76 == 40
        && (u8)(a->unk04 - 2) <= 1)
        v = 200;
    AddPlayerScore(v, arg);
}

s8 TaskGetParentFacing(void)
{
    struct Task *t;
    s32 i;

    i = gCurTask->unk44;
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

    if (p->unk0A == 1)
    {
        if (sub_08021a40(p->unk0C, p->unk0E) != 0)
            return -1;
    }
    i = sub_08063698(p->unk04, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk72 = 4;
        t->unk76 = p->unk00;
        t->unk73 = p->unk08;
        t->unk74 = p->unk09;
        t->unk48 = p->unk0C;
        t->unk4A = p->unk0E;
        t->posX = p->unk0C << 16;
        t->posY = p->unk0E << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = p->unk10;
        t->unk8C = &gActors[i];
        ActorInitSlot(i);
    }
    return i;
}

s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gCurTask;
    p->unk0C = t->unk48;
    p->unk0E = t->unk4A;
    if (keepPrio == 0)
        p->unk10 = t->unk40;
    return sub_08064a78(p);
}

s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio)
{
    struct Task *t;

    t = gCurTask;
    p->unk0C = t->unk48 + p->unk0C * t->facing;
    p->unk0E += t->unk4A;
    if (keepPrio == 0)
        p->unk10 = t->unk40;
    return sub_08064a78(p);
}

s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio)
{
    if (keepPrio == 0)
        p->unk10 = gCurTask->unk40;
    return sub_08064a78(p);
}

/* Cycle the running task's frame between 4 and 7 every other tick. */
/* Cycle the running task's frame between 4 and 7 every other tick. */
void sub_08064bcc(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk24 <= 0)
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
        gCurTask->unk24 = 2;
    }
    else
    {
        t->unk24--;
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

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        switch (gCurTask->unk72)
        {
        case 2:
        case 7:
            t->unk72 = 7;
            break;
        case 1:
        case 8:
            t->unk72 = 8;
            break;
        default:
            t->unk72 = 9;
            break;
        }
        t->unk48 = x;
        t->unk4A = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = prio;
        t->unk78 = 2;
        gTasks[i].unk16 = gTasks[gCurTaskIdx].unk16;
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
    x = (s16)(t->unk48 + t->facing * dx);
    y = (s16)(dy + t->unk4A);
    if (keepPrio != 0)
        prio = t->unk40;
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
    x = t->unk48;
    y = t->unk4A;
    if (keepPrio != 0)
        prio = t->unk40;
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
        prio = gCurTask->unk40;
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

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        if (alt != 0)
            t->unk72 = 6;
        else
            t->unk72 = 5;
        t->unk76 = sub;
        t->unk73 = 0;
        t->unk74 = p2;
        t->unk48 = x;
        t->unk4A = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->unk44 = gCurTaskIdx;
        t->unk40 = prio;
        t->unk8C = &gActors[i];
        ActorInitSlot(i);
    }
    return i;
}

s32 sub_08064e5c(u32 sub, u32 type, u8 p2)
{
    struct Task *t;

    t = gCurTask;
    return sub_08064d9c(sub, type, p2, t->unk48, t->unk4A, 0, 1);
}

s32 sub_08064e90(u32 sub, u32 type, u8 p2, s16 xArg, s16 yArg)
{
    s32 x = xArg;
    s32 y = yArg;

    return sub_08064d9c(sub, type, p2, x, y, 0, 1);
}

s32 sub_08064eb8(u8 p2)
{
    struct Task *t;
    struct Actor *a;
    struct PlayerState *p;
    s32 i;

    i = sub_08064e5c(0, 68, p2);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk88 = p = &gPlayerStates[gCurTaskIdx];
        a = t->unk8C;
        t->unk18 = p->unk0D;
        t->unk1C = p->unk0E;
        t->unk20 = gCurTaskIdx;
        a->unk00 = p->unk0D;
    }
    CreateChildTaskHere(166, 0);
    return i;
}

/* Generic task spawn: every field of the new task comes from an argument. */
s32 CreateActor(u8 cls, u32 sub, u32 type, u8 p3, u8 p4, int x, int y,
                 u16 prio)
{
    struct Task *t;
    s32 i;

    i = sub_08063698(type, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk72 = cls;
        t->unk76 = sub;
        t->unk73 = p3;
        t->unk74 = p4;
        t->unk48 = x;
        t->unk4A = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->unk40 = prio;
        t->unk8C = &gActors[i];
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
        type = gUnk_0873F198[sub];
        break;
    case 1:
    case 3:
        type = gUnk_0873F23C[sub];
        break;
    case 2:
        type = gUnk_0873F264[sub];
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
    a = t->unk8C;
    i = CreateActor(t->unk72, t->unk76, gTaskSlotTypes[gCurTaskIdx], p3, p4,
                     x, y, prio);
    if (i != -1)
    {
        u = &gTasks[i];
        b = u->unk8C;
        u->unk44 = gCurTaskIdx;
        b->unk64 = a->unk64;
    }
    return i;
}

s32 sub_08065100(s16 x, s16 y, u32 p2, u8 p3, u8 p4)
{
    struct Task *t;
    s32 i;

    i = CreateActor(0, 40, 48, 0, 0, x, y, 0);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk7C = p3;
        t->unk82 = p4;
        t->unk7E = p2;
    }
    return i;
}

/* Is the running task inside the 64px-padded camera window? */
u8 ActorIsInView(void)
{
    if (gViewRect[0] - 64 < gCurTask->unk48
        && gCurTask->unk48 < gViewRect[1] + 64
        && gViewRect[2] - 64 < gCurTask->unk4A
        && gCurTask->unk4A < gViewRect[3] + 64)
        return 1;
    return 0;
}

void ActorDrawWorldInView(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->unk38 == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->unk38;
    QueueSprite(t->layer, tbl[t->frame], t->unk3E, t->unk40,
                 t->unk48 - gSpriteCameraX,
                 (s16)(t->unk4A - gSpriteCameraY));
}

void ActorDrawWorldInViewOrDestroy(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->unk38 == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        t = gCurTask;
        tbl = t->unk38;
        QueueSprite(t->layer, tbl[t->frame], t->unk3E, t->unk40,
                     t->unk48 - gSpriteCameraX,
                     (s16)(t->unk4A - gSpriteCameraY));
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
    if (p->unk38 == NULL)
        return;
    if (p->frame == -1)
        return;
    if (sub_08066a6c() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->unk38;
    QueueSprite(t->layer, tbl[t->frame], t->unk3E, t->unk40,
                 t->unk48 - gSpriteCameraX,
                 (s16)(t->unk4A - gSpriteCameraY));
}

void sub_08065350(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;

    p = gCurTask;
    if (p->unk38 == NULL)
        return;
    if (p->frame == -1)
        return;
    if (sub_08066a6c() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        t = gCurTask;
        tbl = t->unk38;
        QueueSprite(t->layer, tbl[t->frame], t->unk3E, t->unk40,
                     t->unk48 - gSpriteCameraX,
                     (s16)(t->unk4A - gSpriteCameraY));
    }
    else if (sub_08066a80() != 0)
    {
        HudRemoveHpBar();
        ActorDestroy();
    }
}
