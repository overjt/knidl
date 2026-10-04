/* game_code_and_rodata 0x08078B68-0x0807AA5C (issue #77, module M20 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08078B68 0x0807AA5C src/enemy_78b68.c --newpb
 *
 * M20 is enemy behaviour bank 1 (#155 run 2 identified its enemies from local
 * sprite renders and their ActorDef.ability): twenty-one ROM task types whose
 * bodies use the same three-table pattern as M21/M22/M24/M25/M26
 * (rom-map section 9):
 *
 *   entry       -> installs Task.updateCallback (the per-frame hook) and hands
 *                  Task.state / Task.updateState to CallTableEntry, which indexes the
 *                  script's tables;
 *   unk14 table -> the coroutine BODIES (each runs a chain of
 *                  TaskYieldTrampoline waits);
 *   unk15 table -> the per-frame HANDLERS;
 *   unk73 table -> the class-3 dispatch a task type's body selects with.
 *
 * This batch holds the bank's first twelve scripts, among them:
 *   * the shared eight-frame "bob" coroutine `sub_08078b68`, which nine of
 *     the bank's idle bodies tail-call;
 *   * the `0x08740648` / `0x08740668` cue+delay pair Task_WaddleDee's walk
 *     states (rows 0/1: `WaddleDeeWalk`, `WaddleDeePaceWalk`) index by Task.unk74;
 *   * Task_Pengy's (#12) four-state row `PengyInit`+`PengyUpdate` with its
 *     state table gPengyStates (`0x08740758`) and the `PengyWaitUpdate` check
 *     (the nearest player within 64 px horizontally, plus a cooldown timer);
 *   * `PengyShoot`, Pengy's breath state, which spawns actor type 102 ten
 *     times (cycling Task.unk74 0-2) from a stack `struct ActorSpawn` and
 *     clears Task.unk74 on the companion it gets back from
 *     CreateChildTaskAtOffsetFacing;
 *   * the `sub_08079eec` / `sub_08079f18` / `SparkyPickNextState` sound-cue chain
 *     (all `u16`-parameterised) that every later script funnels its
 *     "player hit me" reaction through;
 *   * the two 0x1C0-byte Task_Sparky states `SparkyJumpDischarge` (row 0) and
 *     `SparkyStandDischarge` (row 2) (identical: a seventeen-step frame script followed by an
 *     eight-iteration palette flip between `0x08740DE4` and `0x0873F774`);
 *   * Task_Scarfy's `sub_0807a8fc`, the bank's only `mov pc` jump table (five
 *     cases over Task.variant), and `sub_0807a968`, which places Scarfy at an
 *     offset from the nearest player, clamping the point from
 *     `0x08740824` into the camera box `gViewRect[0..3]`.
 *
 * `WaddleDeeIdleEnterState`, `sub_080794d0`, `PengyIdleEnterState`, `BomberIdleEnterState` and
 * `SparkyIdleEnterState` are dead exports: each is a copy of its host's tail dispatch
 * that nothing in the ROM references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "cutscene.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 ActorReactToHit(void);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void sub_08066b34(u32 *p);

void sub_08078b68(void)
{
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(11);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(11);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    }
}

void WaddleDeeWalkInit(void)
{
    gCurTask->updateCallback = (u32)WaddleDeeWalkUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gWaddleDeeWalkStates);
}

void WaddleDeeWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gWaddleDeeWalkStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void WaddleDeeWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gWaddleDeeWalkStates);
}

void WaddleDeeWalk(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void WaddleDeeWalkState0Update(void)
{
}

void WaddleDeeWalkFall(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void WaddleDeeWalkFallUpdate(void)
{
}

void WaddleDeePaceInit(void)
{
    gCurTask->updateCallback = (u32)WaddleDeePaceUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gWaddleDeePaceStates);
}

void WaddleDeePaceUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gWaddleDeePaceStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void WaddleDeePaceEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gWaddleDeePaceStates);
}

void WaddleDeePaceWalk(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    TaskStop();
    t = gCurTask;
    t->unk28 = gUnk_08740668[t->unk74];
    TaskSetMotionXFacing(gUnk_08740648[t->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void WaddleDeePaceWalkUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 <= 0)
    {
        struct Task *u;

        TaskTurnAroundAndReverseX();
        u = gCurTask;
        u->unk28 = gUnk_08740668[u->unk74];
    }
    else
    {
        t->unk28--;
    }
}

void WaddleDeePaceFall(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void WaddleDeePaceFallUpdate(void)
{
}

void WaddleDeeJumpInit(void)
{
    gCurTask->updateCallback = (u32)WaddleDeeJumpUpdate;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 80;
    ActorSetState(0);
    gCurTask->unk34 = ActorStartAnim(gUnk_087406A0);
    CallTableEntry(gCurTask->state, 3, gWaddleDeeJumpStates);
}

void WaddleDeeJumpEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gWaddleDeeJumpStates);
}

void WaddleDeeJumpUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gWaddleDeeJumpStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void WaddleDeeJumpWalk(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->unk74], 0x5A5A5A5A);
    TaskSleepForever();
}

void WaddleDeeJumpWalkUpdate(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    if (--gCurTask->unk28 < 0)
    {
        if (RandomRange(4) == 0)
        {
            ActorSetState(1);
            TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        }
        else
        {
            gCurTask->unk28 = 30;
            ActorSetState(0);
            TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        }
    }
}

void WaddleDeeJump(void)
{
    s32 saved;

    gCurTask->updateState = 1;
    {
        struct Task *t = gCurTask;

        t->unk2C = 0;
        saved = t->velX;
        t->velX = 0;
        t->unk6C = 0;
    }
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 1);
    {
        struct Task *u = gCurTask;

        u->velX = saved;
        u->unk2C = 1;
        u->onGround = 0;
    }
    TaskSetMotionY(-gUnk_08740680[gCurTask->unk74],
                 gUnk_08740690[gCurTask->unk74], 0x30000);
    TaskSleepForever();
}

void WaddleDeeJumpState1Update(void)
{
    struct Task *t = gCurTask;

    if (t->unk2C != 0 && t->onGround != 0)
    {
        TaskStopY();
        gCurTask->unk28 = 30;
        gCurTask->unk34 = ActorStartAnim(gUnk_087406A0);
        ActorSetState(0);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
    }
}

void WaddleDeeJumpFall(void)
{
    gCurTask->updateState = 2;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void WaddleDeeJumpFallUpdate(void)
{
}

void ParasolWaddleDeeWalkInit(void)
{
    gCurTask->updateCallback = (u32)ParasolWaddleDeeWalkUpdate;
    sub_08066b34(gParasolWaddleDeeDef);
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gParasolWaddleDeeWalkStates);
}

void ParasolWaddleDeeWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gParasolWaddleDeeWalkStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void ParasolWaddleDeeWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gParasolWaddleDeeWalkStates);
}

void ParasolWaddleDeeWalk(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    sub_08078b68();
}

void ParasolWaddleDeeWalkState0Update(void)
{
}

void ParasolWaddleDeeWalkState1(void)
{
    gCurTask->updateState = 1;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskSetMotionY(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gCurTask->unk34 = ActorStartAnim(gUnk_087406EC);
        TaskStartParasolDrift();
        while (1)
        {
            TaskStepParasolDrift();
            TaskYieldTrampoline(8);
        }
    }
}

void ParasolWaddleDeeWalkState1Update(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void WaddleDeeIdleInit(void)
{
    gCurTask->updateCallback = (u32)WaddleDeeIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gWaddleDeeIdleStates);
}

void WaddleDeeIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gWaddleDeeIdleStates);
}

void WaddleDeeIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gWaddleDeeIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void WaddleDeeIdle(void)
{
    gCurTask->updateState = 0;
    sub_08078b68();
}

void WaddleDeeIdleState0Update(void)
{
}

void ParasolWaddleDeeStandInit(void)
{
    gCurTask->updateCallback = (u32)ParasolWaddleDeeStandUpdate;
    sub_08066b34(gParasolWaddleDeeDef);
    gCurTask->unk74 = 2;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gParasolWaddleDeeStandStates);
}

void ParasolWaddleDeeStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gParasolWaddleDeeStandStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void ParasolWaddleDeeStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gParasolWaddleDeeStandStates);
}

void ParasolWaddleDeeStandState0(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    sub_08078b68();
}

void ParasolWaddleDeeStandState0Update(void)
{
    TaskFaceNearestPlayer();
}

void ParasolWaddleDeeStandState1(void)
{
    gCurTask->updateState = 1;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskSetMotionY(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gCurTask->unk34 = ActorStartAnim(gUnk_087406EC);
        TaskStartParasolDrift();
    }
    TaskSleepForever();
}

void ParasolWaddleDeeStandState1Update(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
}

void Task_Pengy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gPengyFrames;
    CallTableEntry(gCurTask->variant, 2, gPengyVariants);
}

s32 sub_08079480(void)
{
    ActorSetState(3);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_080794a0(void)
{
    ActorSetState(1);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_080794c0(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_080794d0(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void PengyInit(void)
{
    gCurTask->updateCallback = (u32)PengyUpdate;
    TaskFaceNearestPlayer();
    PengyPickStartState();
    CallTableEntry(gCurTask->state, 4, gPengyStates);
}

void PengyUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gPengyStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void PengyEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gPengyStates);
}

void PengyPickStartState(void)
{
    struct Task *t = gCurTask;
    s32 r;

    t->unk2C = 0;
    t->unk30 = 0;
    r = RandomRange(4);
    if (r == 0)
    {
        gCurTask->unk28 = r;
        ActorSetState(0);
    }
    else
    {
        gCurTask->unk28 = 240;
        ActorSetState(2);
    }
}

void PengyWait(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    while (1)
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
    }
}

void PengyWaitUpdate(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk28;

    if (n <= 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (gCurTask->unk2C <= 0 && RandomRange(2) == 0)
                PengyStartShoot();
            else
                PengyCheckShoot();
        }
        else
        {
            gCurTask->unk2C = 0;
            PengyCheckWalk();
        }
    }
    else
    {
        t->unk2C = 0;
        t->unk28 = n - 1;
        PengyCheckWalk();
    }
}

void PengyStartShoot(void)
{
    ActorSetState(2);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void PengyStartWalk(void)
{
    ActorSetState(1);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void PengyCheckWalk(void)
{
    s32 n = ++gCurTask->unk30;

    if (n == 120)
    {
        PengyStartWalk();
    }
    else if (n == 75 || n == 90 || n == 105)
    {
        if (RandomRange(gUnk_08740720[gCurTask->unk74]) == 0)
            PengyStartWalk();
    }
}

void PengyCheckShoot(void)
{
    struct Task *t = gCurTask;

    if (++t->unk2C == 33)
    {
        t->unk2C = 3;
        if (RandomRange(3) == 0)
            PengyCheckWalk();
        else
            PengyStartShoot();
    }
    else
    {
        PengyCheckWalk();
    }
}

void PengyWalk(void)
{
    struct Task *t = gCurTask;
    s16 *delay;
    u32 *cue;
    s32 i;
    s32 idx;
    u32 *cbase;
    s16 *dbase;

    t->u8C.actor->animScript = 0;
    t->updateState = 1;
    TaskStop();
    gCurTask->unk34 = ActorStartAnim(gUnk_0874074C);
    idx = gCurTask->unk74;
    cbase = gUnk_08740728;
    dbase = gUnk_08740740;
    i = 2;
    delay = &dbase[idx];
    cue = &cbase[idx];
    do
    {
        TaskSetMotionXFacing(*cue, 0x5A5A5A5A);
        TaskYieldTrampoline(*delay);
        delay += 2;
        cue += 2;
    } while (--i >= 0);
    gCurTask->velX = 0;
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    ActorSetState(0);
    TaskSleepForever();
}

void PengyWalkUpdate(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    if (gCurTask->state != 1)
    {
        gCurTask->unk30 = 0;
        TaskSetEntry(PengyEnterState, gCurTaskIdx);
    }
}

void PengyShoot(void)
{
    struct ActorSpawn spawn;
    s32 i;

    gCurTask->updateState = 2;
    TaskSetFrame(7);
    TaskYieldTrampoline(15);
    i = 0;
    spawn.subtype = 0;
    spawn.taskType = 102;
    spawn.variant = 0;
    spawn.checkTerrain = 0;
    gCurTask->unk6C = 0;
    do
    {
        spawn.spawnArg = i;
        spawn.x = 10;
        spawn.y = 0;
        PlaySfx(164);
        gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 0);
        CreateChildTaskAtOffsetFacing(216, 10, 0, 1);
        {
            s32 id = CreateChildTaskAtOffsetFacing(217, 10, 0, 1);

            if (id != -1)
            {
                struct Task *p = &gTasks[id];

                p->unk74 = i;
            }
        }
        TaskSetFrame(8);
        TaskYieldTrampoline(5);
        gCurTask->frame++;
        TaskYieldTrampoline(5);
        if (++i > 2)
            i = 0;
    } while ((s16)++gCurTask->unk6C <= 9);
    ActorSetState(0);
    TaskSleepForever();
}

void PengyShootUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 2)
    {
        t->unk28 = 240;
        t->unk2C = 0;
        t->unk30 = 0;
        TaskSetEntry(PengyEnterState, gCurTaskIdx);
    }
}

void PengyFall(void)
{
    gCurTask->updateState = 3;
    TaskSetFrame(5);
    TaskSetMotionY(0, 0x2500, 0x30000);
    TaskSleepForever();
}

void PengyFallUpdate(void)
{
}

void PengyIdleInit(void)
{
    gCurTask->updateCallback = (u32)PengyIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gPengyIdleStates);
}

void PengyIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gPengyIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void PengyIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gPengyIdleStates);
}

void PengyIdle(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrame(4);
    TaskSleepForever();
}

void PengyIdleState0Update(void)
{
}

void Task_Bomber(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gBomberFrames;
    CallTableEntry(gCurTask->variant, 2, gBomberVariants);
}

s32 sub_08079a20(void)
{
    ActorSetState(1);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_08079a40(void)
{
    ActorSetState(3);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_08079a60(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_08079a70(void)
{
    ActorSetState(2);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_08079a90(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void BomberInit(void)
{
    gCurTask->updateCallback = (u32)BomberUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    gCurTask->unk28 = 0;
    CallTableEntry(gCurTask->state, 4, gBomberStates);
}

void BomberUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gBomberStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void BomberEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gBomberStates);
}

void BomberWalk(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(0x4D00, 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
}

void BomberWalkUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0)
        t->unk28 = 1;
}

void BomberState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskTurnAround();
    TaskSetFrame(10);
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSleepForever();
}

void BomberState1Update(void)
{
}

void BomberState2(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    TaskStop();
    t = gCurTask;
    t->posX = (t->pixelX + t->facing * 4) << 16;
    if (t->unk28 == 1)
    {
        t->unk6C = 0;
        do
        {
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 4);
    }
    {
        struct Task *u = gCurTask;

        u->posX = (u->pixelX + u->facing * 4) << 16;
    }
    ActorSetState(1);
    TaskSleepForever();
}

void BomberState2Update(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
}

void BomberExplode(void)
{
    u8 v;

    gCurTask->updateState = 3;
    TaskStop();
    v = gScreenAttackActive;
    if (v == 0)
    {
        ActorSetAttackBox(gUnk_0873F7AC);
        FreezeStage(15);
        gCurTask->unk6C = v;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(1);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 3);
        ThawStage();
        ActorSetHitReactions(gUnk_08740F2C);
        ActorDie();
    }
    else
    {
        TaskSleepForever();
    }
}

void BomberExplodeUpdate(void)
{
}

void BomberIdleInit(void)
{
    gCurTask->updateCallback = (u32)BomberIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gBomberIdleStates);
}

void BomberIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gBomberIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void BomberIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gBomberIdleStates);
}

void BomberIdle(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
}

void BomberIdleState0Update(void)
{
}

void Task_Sparky(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSparkyFrames;
    AcquirePaletteAnim(2, 0);
    CallTableEntry(gCurTask->variant, 3, gSparkyVariants);
}

void sub_08079e70(void)
{
    if (--gPaletteAnimRefCounts[0] < 0)
        sub_0806ee2c();
}

void SparkySetJumpMotion(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0)
        TaskSetMotionXFacing(gUnk_087407DC[t->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-gUnk_087407C4[gCurTask->unk28],
                 gUnk_087407D0[gCurTask->unk28], 0x30000);
    gCurTask->onGround = 0;
}

void sub_08079eec(u16 a)
{
    s32 r = RandomRange(4);

    gCurTask->unk28 = r;
    if (r > 2)
        gCurTask->unk28 = 2;
    ActorSetState(a);
}

void sub_08079f18(u16 a)
{
    if (TaskGetNearestPlayerDistSq() > 0x1000)
    {
        sub_08079eec(a);
    }
    else if (RandomRange(4) == 0)
    {
        ActorSetState(0);
    }
    else
    {
        sub_08079eec(a);
    }
}

void SparkyPickNextState(u16 a)
{
    TaskFaceNearestPlayer();
    sub_08079f18(a);
    switch (gCurTask->variant)
    {
    case 0:
        TaskSetEntry(SparkyJumpEnterState, gCurTaskIdx);
        break;
    case 2:
        TaskSetEntry(SparkyStandEnterState, gCurTaskIdx);
        break;
    }
}

void SparkyPickStartState(u16 a)
{
    TaskFaceNearestPlayer();
    if (RandomRange(4) == 0)
        ActorSetState(0);
    else
        sub_08079f18(a);
}

s32 sub_08079fd0(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(3);
    TaskSetEntry(SparkyJumpEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807a008(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(1);
    TaskSetEntry(SparkyJumpEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807a040(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_0807a050(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void sub_0807a05c(void)
{
    gCurTask->velY = 0;
}

void SparkyJumpInit(void)
{
    gCurTask->updateCallback = (u32)SparkyJumpUpdate;
    SparkyPickStartState(2);
    CallTableEntry(gCurTask->state, 4, gSparkyJumpStates);
}

void SparkyJumpUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gSparkyJumpStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        struct Task *t;

        ActorCheckHits();
        ActorReactToHit();
        t = gCurTask;
        if (t->hitKind != 0 && t->unk46 != -1)
            ActorDestroySlot(t->unk46);
    }
}

void SparkyJumpEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gSparkyJumpStates);
}

void SparkyJump(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_087407C0[gCurTask->unk74]);
    SparkySetJumpMotion();
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSleepForever();
}

void SparkyJumpState2Update(void)
{
}

void SparkyJumpDischarge(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 3;
    gCurTask->frame = 20;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 19;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 21;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    gCurTask->frame = 18;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 20;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->unk2C = 2;
    gCurTask->unk6C = 0;
    do
    {
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame = 10;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 7);
    gCurTask->unk2C = 1;
    TaskSleepForever();
}

void SparkyJumpDischargeUpdate(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk2C;

    if (n == 1)
    {
        SparkyPickNextState(2);
    }
    else if (n == 2)
    {
        if (t->unk30 <= 0)
        {
            PlaySfx(191);
            gCurTask->unk30 = 3;
        }
        gCurTask->unk30--;
    }
}

void SparkyJumpLand(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk2C = 0;
    TaskStopY();
    gCurTask->velX >>= 1;
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_087407BC[gCurTask->unk74]);
    gCurTask->unk2C = 1;
    TaskSleepForever();
}

void SparkyJumpLandUpdate(void)
{
    if (gCurTask->unk2C != 0)
        SparkyPickNextState(2);
}

void SparkyJumpState3(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    TaskTurnAround();
    TaskSetFrame(8);
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSleepForever();
}

void SparkyJumpState3Update(void)
{
}

void SparkyIdleInit(void)
{
    gCurTask->updateCallback = (u32)SparkyIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gSparkyIdleStates);
}

void SparkyIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gSparkyIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SparkyIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gSparkyIdleStates);
}

void SparkyIdle(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
}

void SparkyIdleState0Update(void)
{
}

void SparkyStandInit(void)
{
    gCurTask->updateCallback = (u32)SparkyStandUpdate;
    SparkyPickStartState(1);
    CallTableEntry(gCurTask->state, 2, gSparkyStandStates);
}

void SparkyStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gSparkyStandStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        struct Task *t;

        ActorCheckHits();
        ActorReactToHit();
        t = gCurTask;
        if (t->hitKind != 0 && t->unk46 != -1)
            ActorDestroySlot(t->unk46);
    }
}

void SparkyStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gSparkyStandStates);
}

void SparkyStandDischarge(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 3;
    gCurTask->frame = 20;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 19;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 22;
    TaskYieldTrampoline(4);
    gCurTask->frame = 21;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    gCurTask->frame = 18;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->frame = 20;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 23;
    TaskYieldTrampoline(4);
    gCurTask->unk2C = 2;
    gCurTask->unk6C = 0;
    do
    {
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame = 10;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08740DE4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_0873F774);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 7);
    gCurTask->unk2C = 1;
    TaskSleepForever();
}

void SparkyStandDischargeUpdate(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk2C;

    if (n == 1)
    {
        SparkyPickNextState(1);
    }
    else if (n == 2)
    {
        if (t->unk30 <= 0)
        {
            PlaySfx(191);
            gCurTask->unk30 = 3;
        }
        gCurTask->unk30--;
    }
}

void SparkyStandWait(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk2C = 0;
    TaskStopY();
    gCurTask->velX >>= 1;
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_0874080C[gCurTask->unk74]);
    gCurTask->unk2C = 1;
    TaskSleepForever();
}

void SparkyStandWaitUpdate(void)
{
    if (gCurTask->unk2C != 0)
        SparkyPickNextState(1);
}

void Task_Scarfy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gScarfyFrames;
    CallTableEntry(0, 1, gScarfyVariants);
}

void ScarfyPickStartState(void)
{
    if (gCurTask->variant == 0)
        ActorSetState(2);
    else
        ActorSetState(0);
}

void sub_0807a8fc(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        gCurTask->unk28 = -1;
        gCurTask->unk34 = ActorStartAnim(gUnk_08740854);
        break;
    case 1:
        gCurTask->unk28 = 1;
        break;
    case 2:
        gCurTask->unk28 = 1;
        break;
    case 3:
    case 4:
        gCurTask->unk28 = -1;
        break;
    }
}

void sub_0807a968(void)
{
    struct Task *t = gCurTask;
    s32 i = (t->variant - 1) * 3;
    s32 x;
    s32 y;

    t->velY = gUnk_08740824[i];
    TaskGetNearestPlayerPos();
    x = gUnk_08740824[i + 1] + gUnk_030023B4;
    y = gUnk_08740824[i + 2] + gUnk_030023D4;
    if (x <= gViewRect[0] - 64)
        x = gViewRect[0] - 48;
    if (x >= gViewRect[1] + 64)
        x = gViewRect[1] + 48;
    if (y <= gViewRect[2] - 64)
        y = gViewRect[2] - 48;
    if (y >= gViewRect[3] + 64)
        y = gViewRect[3] + 48;
    gCurTask->posX = x << 16;
    gCurTask->posY = y << 16;
}

void ScarfyCheckTransform(void)
{
    struct Task *t = gCurTask;

    if (t->hitKind == 6 && (u16)(t->hitEffect - 2) <= 1)
    {
        ActorSetAttackBox(gUnk_08740E1C);
        ActorSetState(3);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}
