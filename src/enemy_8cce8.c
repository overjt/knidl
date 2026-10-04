/* game_code_and_rodata 0x0808CCE8-0x0808E404 (issue #70, module M24 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808CCE8 0x0808E404 src/enemy_8cce8.c --newpb
 *
 * M24 is a bank of six enemy/object behaviour scripts built to the same
 * three-table pattern as M22/M25/M26 (rom-map section 9):
 *
 *   entry       -> installs the draw hook in Task.moveCallback (TaskMove or
 *                  ActorMove) and the per-frame hook in Task.drawCallback, points
 *                  Task.frameTable at a TaskGfx block, and hands Task.variant to
 *                  CallTableEntry with the script's entry table;
 *   unk14 table -> the coroutine BODIES: each installs its own resume function
 *                  in Task.updateCallback and then runs a chain of TaskYieldTrampoline
 *                  waits;
 *   unk15 table -> the per-frame GUARDS that re-arm the body through
 *                  TaskSetEntry(fn, gCurTaskIdx) when the state changes.
 *
 * This batch holds:
 *   * the two stand-alone class-2 bodies `Task_ChillyFreezeSparkle` (a two-variant intro
 *     that walks Task.posX/unk50 with RandomSpread and waits on the room byte
 *     gTaskSlotTypes[Task.parent] through `sub_0808cfec`) and `Task_WaddleDooBeam`,
 *     plus the smaller `Task_GlunkShot` and `Task_GipStar`;
 *   * script 1: entry `Task_BroomHatter` (Task.variant -> `0x08743188`, 3 rows) with
 *     the row bodies `BroomHatterVariant0` / `BroomHatterVariant1` / `BroomHatterIdleInit`, the
 *     body tables `0x08743194` / `0x087431AC` / `0x087431C4` and the guard
 *     tables `0x087431A0` / `0x087431B8` / `0x087431C8`;
 *   * its movement library: `sub_0808d364` / `sub_0808d388` snap Task.unk2C to
 *     the 16-pixel grid, `BroomHatterPickNextState` rolls a new mode out of the 8-entry
 *     table `gUnk_0874313C`, `sub_0808d460` flips the sprite through
 *     Task.spriteFlags and `sub_0808d494` / `sub_0808d4a8` / `sub_0808d4bc` /
 *     `sub_0808d4d0` set the animation id in Actor.extraFrame;
 *   * script 2's entry `Task_LaserBall` (Task.variant -> `0x087431E4`) and its
 *     aiming half: `LaserBallSetTargetX` / `sub_0808e0d0` / `LaserBallSetMoveDir` turn the
 *     vector to the target into a heading with ArcTan2, `CreateLaserBallLaser` spawns
 *     actor 103, `LaserBallCheckShoot` is the GetDistSq proximity test and
 *     `LaserBallReaim` / `LaserBallAccelerateInMoveDir` are the per-frame step.  The script's
 *     rows continue in src/enemy_8e404.c.
 *
 * `sub_0808d388` is a pointer-referenced leaf the prologue scan could not
 * propose (no `push`, lesson 3.75); it and the module's three other census
 * fixes are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void TaskSetEntry(void *a, u32 i);
extern u16 RandomSpread(s32 base, u8 scale, u8 amount);
extern s32 GetShapeAtPixelIgnoringOneWay(s32 x, s32 y);
extern void ActorSetState(u16 v);
extern void ActorSetAttackBox(void *p);
extern void AngleToVector(s32 a, s32 b);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_080699a8(void);
extern u32 ActorReactToHit(void);

void Task_ChillyFreezeSparkle(void)
{

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 10;
    gCurTask->frameTable = gChillyFreezeFrames;
    switch (gCurTask->state)
    {
    case 0:
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-20, 1, 32) + gTasks[gCurTask->parent].pixelX) << 16;
        gCurTask->posY = (RandomSpread(20, 1, 8) + gTasks[gCurTask->parent].pixelY) << 16;
        TaskSetMotionXFacing(0xFFFF4000, 128 << 5);
        gCurTask->velY = 0xFFFEC000;
        gCurTask->accelY = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-20, 1, 32) + gTasks[gCurTask->parent].pixelX) << 16;
        gCurTask->posY = (RandomSpread(4, 1, 8) + gTasks[gCurTask->parent].pixelY) << 16;
        TaskSetMotionXFacing(0xFFFF4000, 128 << 5);
        gCurTask->velY = 0xFFFEC000;
        gCurTask->accelY = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        break;
    case 1:
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-12, 1, 32) + gTasks[gCurTask->parent].pixelX) << 16;
        gCurTask->posY = (RandomSpread(16, 1, 8) + gTasks[gCurTask->parent].pixelY) << 16;
        TaskSetMotionXFacing(192 << 8, 0xFFFFF000);
        gCurTask->velY = 0xFFFEC000;
        gCurTask->accelY = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gCurTask->posY = (RandomSpread(0, 1, 8) + gTasks[gCurTask->parent].pixelY) << 16;
        TaskSetMotionXFacing(192 << 8, 0xFFFFF000);
        gCurTask->velY = 0xFFFEC000;
        gCurTask->accelY = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        break;
    }
    TaskExitTrampoline();
}

void sub_0808cfec(void)
{
    if (gTaskSlotTypes[gCurTask->parent] != 104)
        TaskExitTrampoline();
}

void Task_WaddleDooBeam(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;
    s32 v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gWaddleDooBeamFrames;
    u->updateCallback = (u32)sub_0808d100;
    u->onGround = 0;
    w = gCurTask;
    w->unk28 = 0;
    v = gTasks[w->parent].waddleDooBeamStep;
    w->waddleDooBeamAngleIndex = v;
    if (w->actorSpawnArg <= 1)
        w->waddleDooBeamAngleIndex = v >> 1;
    gCurTask->facing = TaskGetParentFacing();
    AngleToVector(gUnk_08742FAC[gCurTask->waddleDooBeamAngleIndex], 128 << 4);
    TaskSetMotionXFacing(gUnk_030023B4, 0x5A5A5A5A);
    gCurTask->velY = gUnk_030023D4;
    TaskSetFrame(0);
    gCurTask->waddleDooBeamLoopCount = 0;
    do
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        z = gCurTask;
        z->waddleDooBeamLoopCount++;
    } while ((s16)z->waddleDooBeamLoopCount <= 2);
    ActorDestroy();
}

void sub_0808d100(void)
{
    if (sub_08069660() != 0) {
        TaskStop();
        TaskSetEntry(ActorDie, gCurTaskIdx);
    } else {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0808d130(void)
{
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_GlunkShot(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_0808d1d0;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gGlunkShotFrames;
    gCurTask->facing = TaskGetParentFacing();
    gCurTask->onGround = 0;
    gCurTask->velY = 0xFFFA0000;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 3);
    ActorDestroy();
}

void sub_0808d1d0(void)
{
    if (sub_08069604() != 0)
    {
        TaskStop();
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0808d200(void)
{
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_GipStar(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    t = gCurTask;
    t->frameTable = gGipStarFrames;
    t->updateCallback = (u32)GipStarUpdate;
    gCurTask->facing = TaskGetParentFacing();
    TaskSetMotionXFacing(192 << 9, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 168 << 5, 192 << 10);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
    }
}

void GipStarUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

s32 BroomHatterStartFall(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(2);
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        ActorSetState(2);
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 BroomHatterLand(void)
{
    struct Task *t;

    t = gCurTask;
    t->broomHatterLanded = 1;
    t->broomHatterMove = 1;
    switch (t->variant)
    {
    case 0:
        ActorSetState(0);
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        ActorSetState(0);
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 BroomHatterEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_0808d364(void)
{
    if (gCurTask->velX != 0 && sub_080699a8() != 0)
        TaskStop();
    return 0;
}

s32 sub_0808d388(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->velX >= 0)
        t->unk2C = ((t->pixelX - 16) & 0xFFF0) + 15;
    else
        t->unk2C = (t->pixelX + 16) & 0xFFF0;
    u = gCurTask;
    u->unk18 = u->unk2C - u->pixelX;
    u->pixelX = u->unk2C + u->unk18;
    u->posX = u->pixelX << 16;
    return 0;
}

void BroomHatterPickNextState(void)
{
    struct Task *t;
    s8 i;
    u8 v;

    do
    {
        IntToDigits((s16)RandomRange(8));
        t = gCurTask;
        i = gDigits[0];
    } while (t->broomHatterMove == gUnk_0874313C[i]);
    v = gUnk_0874313C[i];
    switch (v)
    {
    case 0:
        t->broomHatterMove = v;
        ActorSetState(1);
        break;
    case 1:
        t->broomHatterMove = v;
        ActorSetState(0);
        break;
    case 2:
        t->broomHatterMove = v;
        t->facing = -1 * t->facing;
        ActorSetState(0);
        break;
    default:
        sub_0806ee2c();
        break;
    }
}

void sub_0808d460(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->facing == 1)
        t->spriteFlags |= 0x8000;
    else
        t->spriteFlags &= 0x7FFF;
}

void sub_0808d494(void)
{
    gCurTask->u8C.actor->extraFrame = 9;
}

void sub_0808d4a8(void)
{
    gCurTask->u8C.actor->extraFrame = 8;
}

void sub_0808d4bc(void)
{
    gCurTask->u8C.actor->extraFrame = 10;
}

void sub_0808d4d0(void)
{
    gCurTask->u8C.actor->extraFrame = -1;
}

void Task_BroomHatter(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->u8C.actor->unk16 = 0;
    t->u8C.actor->extraOffsetY = 0;
    t->u8C.actor->extraTileWord = (t->tileWord & 0xFFF) | (240 << 8);
    t->broomHatterMove = 1;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_08065640;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gBroomHatterFrames;
    CallTableEntry(u->variant, 3, gBroomHatterVariants);
}

void BroomHatterVariant0(void)
{
    gCurTask->updateCallback = (u32)sub_0808d58c;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_08743194);
}

void sub_0808d58c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = ActorCollideTerrain();
    t = gCurTask;
    if ((t->onGround & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            t->unk24 = (u16)t->unk24 | 0x10000;
        if ((gCurTask->onGround & 1) != 0)
            goto skip;
    }
    u = gCurTask;
    u->unk24 = (u16)u->unk24;
skip:
    if (r == 0)
    {
        sub_0808d364();
        CallTableEntry(gCurTask->updateState, 3, gUnk_087431A0);
    }
    v = gCurTask;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->pixelY;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808d624(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_08743194);
}

void sub_0808d640(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    if (gCurTask->broomHatterMove == 2)
        TaskTurnAround();
    t = gCurTask;
    t->broomHatterSweepDone = 0;
    t->broomHatterLoopCount = 0;
    do
    {
        sub_0808d460();
        TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(5);
        TaskUpdateFlip();
        TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->broomHatterLoopCount++;
    } while ((s16)u->broomHatterLoopCount <= 1);
    TaskStop();
    v = gCurTask;
    v->broomHatterSweepDone = 2;
    TaskSleepForever();
}

void sub_0808d764(void)
{
    if (gCurTask->broomHatterSweepDone == 2)
    {
        BroomHatterPickNextState();
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d790(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->broomHatterSweepDone = 0;
    TaskFaceNearestPlayer();
    sub_0808d460();
    TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gCurTask->frame = 6;
    TaskYieldTrampoline(8);
    gCurTask->broomHatterLoopCount = 0;
    do
    {
        TaskUpdateFlip();
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(4);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->broomHatterLoopCount++;
    } while ((s16)t->broomHatterLoopCount <= 2);
    TaskStop();
    gCurTask->broomHatterSweepDone = 1;
    TaskSleepForever();
}

void sub_0808d938(void)
{
    if (gCurTask->broomHatterSweepDone == 1)
    {
        BroomHatterPickNextState();
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d964(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808d9fc(void)
{
}

void BroomHatterVariant1(void)
{
    gCurTask->updateCallback = (u32)sub_0808da34;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_087431AC);
}

void sub_0808da34(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = ActorCollideTerrain();
    t = gCurTask;
    if ((t->onGround & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            t->unk24 = (u16)t->unk24 | 0x10000;
        if ((gCurTask->onGround & 1) != 0)
            goto skip;
    }
    u = gCurTask;
    u->unk24 = (u16)u->unk24;
skip:
    if (r == 0)
    {
        sub_0808d364();
        CallTableEntry(gCurTask->updateState, 3, gUnk_087431B8);
    }
    v = gCurTask;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->pixelY;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808dacc(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_087431AC);
}

void sub_0808dae8(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    if (gCurTask->broomHatterMove == 2)
        TaskTurnAround();
    t = gCurTask;
    t->broomHatterSweepDone = 0;
    t->broomHatterLoopCount = 0;
    do
    {
        sub_0808d460();
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(7);
        TaskUpdateFlip();
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(3);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->broomHatterLoopCount++;
    } while ((s16)u->broomHatterLoopCount <= 1);
    TaskStop();
    gCurTask->broomHatterSweepDone = 2;
    TaskSleepForever();
}

void sub_0808dc68(void)
{
    if (gCurTask->broomHatterSweepDone == 2)
    {
        BroomHatterPickNextState();
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808dc94(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->broomHatterSweepDone = 0;
    TaskFaceNearestPlayer();
    sub_0808d460();
    if (gCurTask->broomHatterLanded != 0)
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gCurTask->frame = 6;
    TaskYieldTrampoline(8);
    gCurTask->broomHatterLoopCount = 0;
    do
    {
        TaskUpdateFlip();
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(4);
        TaskYieldTrampoline(7);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(7);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(7);
        TaskYieldTrampoline(7);
        if (gCurTask->broomHatterLanded != 0)
            TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->broomHatterLoopCount++;
    } while ((s16)t->broomHatterLoopCount <= 2);
    TaskStop();
    gCurTask->broomHatterSweepDone = 1;
    TaskSleepForever();
}

void sub_0808de90(void)
{
    if (gCurTask->broomHatterSweepDone == 1)
    {
        BroomHatterPickNextState();
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808debc(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808df54(void)
{
}

void BroomHatterIdleInit(void)
{
    gCurTask->updateCallback = (u32)BroomHatterIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gBroomHatterIdleStates);
}

void BroomHatterIdleUpdate(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 1, gBroomHatterIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BroomHatterIdle(void)
{
    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    gCurTask->onGround = 1;
    TaskStop();
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void BroomHatterIdleState0Update(void)
{
}

void sub_0808e054(void)
{
    if (--gPaletteAnimRefCounts[1] < 0)
        sub_0806ee2c();
}

void LaserBallSetTargetX(void)
{
    switch (TaskGetXDirBitToNearestPlayer())
    {
    case 4:
        gCurTask->laserBallTargetX = (&gTasks[TaskFindNearestPlayer()])->pixelX - 64;
        break;
    case 8:
        gCurTask->laserBallTargetX = (&gTasks[TaskFindNearestPlayer()])->pixelX + 64;
        break;
    }
}

void sub_0808e0d0(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;

    switch (TaskGetXDirBitToNearestPlayer())
    {
    case 4:
        gCurTask->laserBallTargetX = (&gTasks[TaskFindNearestPlayer()])->pixelX - 64;
        break;
    case 8:
        gCurTask->laserBallTargetX = (&gTasks[TaskFindNearestPlayer()])->pixelX + 64;
        break;
    }
    t = gCurTask;
    dx = (s16)(t->laserBallTargetX - (u16)t->pixelX);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->pixelY - (u16)gCurTask->pixelY);
    gCurTask->laserBallTargetDir = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
}

void LaserBallSetMoveDir(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;
    s32 i;

    t = gCurTask;
    dx = (s16)(t->laserBallTargetX - (u16)t->pixelX);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->pixelY - (u16)gCurTask->pixelY);
    i = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
    switch (i)
    {
    case 0:
        gCurTask->laserBallMoveDir = 0;
        break;
    case 1:
        gCurTask->laserBallMoveDir = 1;
        break;
    case 2:
        gCurTask->laserBallMoveDir = 2;
        break;
    case 3:
        gCurTask->laserBallMoveDir = 3;
        break;
    case 4:
        gCurTask->laserBallMoveDir = 4;
        break;
    case 5:
        gCurTask->laserBallMoveDir = 5;
        break;
    case 6:
        gCurTask->laserBallMoveDir = 6;
        break;
    case 7:
        gCurTask->laserBallMoveDir = 7;
        break;
    }
}

void CreateLaserBallLaser(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    t = gCurTask;
    if (GetShapeAtPixelIgnoringOneWay(t->pixelX + (t->facing << 4), t->pixelY) == 0)
    {
        sp.subtype = 1;
        sp.taskType = 103;
        sp.variant = 0;
        sp.spawnArg = gCurTask->laserBallLaserDir;
        sp.x = 16;
        sp.y = 0;
        sp.checkTerrain = 1;
        CreateActorFromDescAtOffsetFacing(&sp, 0);
    }
}

void LaserBallCheckShoot(void)
{
    struct PointPair p;
    struct Task *t;

    p.x0 = gCurTask->laserBallTargetX;
    p.y0 = (&gTasks[TaskFindNearestPlayer()])->pixelY;
    t = gCurTask;
    p.x1 = t->pixelX;
    p.y1 = t->pixelY;
    if (GetDistSq(&p) <= 16)
    {
        ActorSetState(1);
        TaskSetEntry(LaserBallEnterState, gCurTaskIdx);
    }
}

void LaserBallReaim(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->laserBallSteerTimer == 0)
    {
        t->laserBallSteerTimer = 16;
        t->laserBallPrevTargetDir = t->laserBallTargetDir;
        sub_0808e0d0();
        u = gCurTask;
        if (u->laserBallPrevTargetDir != u->laserBallTargetDir)
            LaserBallSetMoveDir();
    }
}

void LaserBallAccelerateInMoveDir(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->laserBallSteerTimer == 16 || t->laserBallSteerTimer == 8)
        TaskAccelerateInDir(gUnk_087431CC[t->actorSpawnArg], gUnk_087431D8[t->actorSpawnArg], (u16)t->laserBallMoveDir);
}

void Task_LaserBall(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gLaserBallFrames;
    AcquirePaletteAnim(3, 1);
    SetPaletteAnimSource(1, 0, gCurTask->u8C.actor->paletteVariant);
    CallTableEntry(gCurTask->variant, 2, gLaserBallVariants);
}
