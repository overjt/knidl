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
extern void ActorStartCarryingParasol(u32 *p);

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
    ActorSetState(WADDLE_DEE_WALK_STATE_WALK);
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
    gCurTask->updateState = WADDLE_DEE_WALK_STATE_WALK;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    sub_08078b68();
}

void WaddleDeeWalkState0Update(void)
{
}

void WaddleDeeWalkFall(void)
{
    gCurTask->updateState = WADDLE_DEE_WALK_STATE_FALL;
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
    ActorSetState(WADDLE_DEE_PACE_STATE_WALK);
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

    gCurTask->updateState = WADDLE_DEE_PACE_STATE_WALK;
    TaskStop();
    t = gCurTask;
    t->waddleDeeTurnTimer = gUnk_08740668[t->actorSpawnArg];
    TaskSetMotionXFacing(gUnk_08740648[t->actorSpawnArg], 0x5A5A5A5A);
    sub_08078b68();
}

void WaddleDeePaceWalkUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->waddleDeeTurnTimer <= 0)
    {
        struct Task *u;

        TaskTurnAroundAndReverseX();
        u = gCurTask;
        u->waddleDeeTurnTimer = gUnk_08740668[u->actorSpawnArg];
    }
    else
    {
        t->waddleDeeTurnTimer--;
    }
}

void WaddleDeePaceFall(void)
{
    gCurTask->updateState = WADDLE_DEE_PACE_STATE_FALL;
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
    gCurTask->waddleDeeJumpTimer = 80;
    ActorSetState(WADDLE_DEE_JUMP_STATE_WALK);
    gCurTask->actorAnimDelay34 = ActorStartAnim(gWaddleDeeJumpAnim);
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
    gCurTask->updateState = WADDLE_DEE_JUMP_STATE_WALK;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    TaskSleepForever();
}

void WaddleDeeJumpWalkUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    if (--gCurTask->waddleDeeJumpTimer < 0)
    {
        if (RandomRange(4) == 0)
        {
            ActorSetState(WADDLE_DEE_JUMP_STATE_JUMP);
            TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        }
        else
        {
            gCurTask->waddleDeeJumpTimer = 30;
            ActorSetState(WADDLE_DEE_JUMP_STATE_WALK);
            TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        }
    }
}

void WaddleDeeJump(void)
{
    s32 saved;

    gCurTask->updateState = WADDLE_DEE_JUMP_STATE_JUMP;
    {
        struct Task *t = gCurTask;

        t->waddleDeeJumpLaunched = 0;
        saved = t->velX;
        t->velX = 0;
        t->waddleDeeLoopCount = 0;
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
    } while ((s16)++gCurTask->waddleDeeLoopCount <= 1);
    {
        struct Task *u = gCurTask;

        u->velX = saved;
        u->waddleDeeJumpLaunched = 1;
        u->onGround = 0;
    }
    TaskSetMotionY(-gUnk_08740680[gCurTask->actorSpawnArg],
                 gUnk_08740690[gCurTask->actorSpawnArg], 0x30000);
    TaskSleepForever();
}

void WaddleDeeJumpState1Update(void)
{
    struct Task *t = gCurTask;

    if (t->waddleDeeJumpLaunched != 0 && t->onGround != 0)
    {
        TaskStopY();
        gCurTask->waddleDeeJumpTimer = 30;
        gCurTask->actorAnimDelay34 = ActorStartAnim(gWaddleDeeJumpAnim);
        ActorSetState(WADDLE_DEE_JUMP_STATE_WALK);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
    }
}

void WaddleDeeJumpFall(void)
{
    gCurTask->updateState = WADDLE_DEE_JUMP_STATE_FALL;
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
    ActorStartCarryingParasol(gParasolWaddleDeeDef);
    TaskFaceNearestPlayer();
    ActorSetState(PARASOL_WADDLE_DEE_WALK_STATE_0);
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
    gCurTask->updateState = PARASOL_WADDLE_DEE_WALK_STATE_0;
    TaskStop();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    sub_08078b68();
}

void ParasolWaddleDeeWalkState0Update(void)
{
}

void ParasolWaddleDeeWalkState1(void)
{
    gCurTask->updateState = PARASOL_WADDLE_DEE_WALK_STATE_1;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskSetMotionY(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_087406EC);
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
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void WaddleDeeIdleInit(void)
{
    gCurTask->updateCallback = (u32)WaddleDeeIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(WADDLE_DEE_IDLE_STATE_IDLE);
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
    gCurTask->updateState = WADDLE_DEE_IDLE_STATE_IDLE;
    sub_08078b68();
}

void WaddleDeeIdleState0Update(void)
{
}

void ParasolWaddleDeeStandInit(void)
{
    gCurTask->updateCallback = (u32)ParasolWaddleDeeStandUpdate;
    ActorStartCarryingParasol(gParasolWaddleDeeDef);
    gCurTask->unk74 = 2;
    TaskFaceNearestPlayer();
    ActorSetState(PARASOL_WADDLE_DEE_STAND_STATE_0);
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
    gCurTask->updateState = PARASOL_WADDLE_DEE_STAND_STATE_0;
    TaskStop();
    sub_08078b68();
}

void ParasolWaddleDeeStandState0Update(void)
{
    TaskFaceNearestPlayer();
}

void ParasolWaddleDeeStandState1(void)
{
    gCurTask->updateState = PARASOL_WADDLE_DEE_STAND_STATE_1;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskSetMotionY(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_087406EC);
        TaskStartParasolDrift();
    }
    TaskSleepForever();
}

void ParasolWaddleDeeStandState1Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay34);
}

void Task_Pengy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gPengyFrames;
    CallTableEntry(gCurTask->variant, 2, gPengyVariants);
}

s32 PengyStartFall(void)
{
    ActorSetState(PENGY_STATE_FALL);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
    return 1;
}

s32 PengyLand(void)
{
    ActorSetState(PENGY_STATE_WALK);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
    return 1;
}

s32 PengyEnterWater(void)
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

    t->pengyNearTimer = 0;
    t->pengyWaitTimer = 0;
    r = RandomRange(4);
    if (r == 0)
    {
        gCurTask->pengyShotTimer = r;
        ActorSetState(PENGY_STATE_WAIT);
    }
    else
    {
        gCurTask->pengyShotTimer = 240;
        ActorSetState(PENGY_STATE_SHOOT);
    }
}

void PengyWait(void)
{
    gCurTask->updateState = PENGY_STATE_WAIT;
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
    s32 n = t->pengyShotTimer;

    if (n <= 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (gCurTask->pengyNearTimer <= 0 && RandomRange(2) == 0)
                PengyStartShoot();
            else
                PengyCheckShoot();
        }
        else
        {
            gCurTask->pengyNearTimer = 0;
            PengyCheckWalk();
        }
    }
    else
    {
        t->pengyNearTimer = 0;
        t->pengyShotTimer = n - 1;
        PengyCheckWalk();
    }
}

void PengyStartShoot(void)
{
    ActorSetState(PENGY_STATE_SHOOT);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void PengyStartWalk(void)
{
    ActorSetState(PENGY_STATE_WALK);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void PengyCheckWalk(void)
{
    s32 n = ++gCurTask->pengyWaitTimer;

    if (n == 120)
    {
        PengyStartWalk();
    }
    else if (n == 75 || n == 90 || n == 105)
    {
        if (RandomRange(gUnk_08740720[gCurTask->actorSpawnArg]) == 0)
            PengyStartWalk();
    }
}

void PengyCheckShoot(void)
{
    struct Task *t = gCurTask;

    if (++t->pengyNearTimer == 33)
    {
        t->pengyNearTimer = 3;
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
    t->updateState = PENGY_STATE_WALK;
    TaskStop();
    gCurTask->actorAnimDelay34 = ActorStartAnim(gPengyWalkAnim);
    idx = gCurTask->actorSpawnArg;
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
    ActorSetState(PENGY_STATE_WAIT);
    TaskSleepForever();
}

void PengyWalkUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    if (gCurTask->state != PENGY_STATE_WALK)
    {
        gCurTask->pengyWaitTimer = 0;
        TaskSetEntry(PengyEnterState, gCurTaskIdx);
    }
}

void PengyShoot(void)
{
    struct ActorSpawn spawn;
    s32 i;

    gCurTask->updateState = PENGY_STATE_SHOOT;
    TaskSetFrame(7);
    TaskYieldTrampoline(15);
    i = 0;
    spawn.subtype = 0;
    spawn.taskType = TASK_PENGY_ICE_BREATH;
    spawn.variant = PENGY_ICE_BREATH_VARIANT_INIT;
    spawn.checkTerrain = 0;
    gCurTask->pengyLoopCount = 0;
    do
    {
        spawn.spawnArg = i;
        spawn.x = 10;
        spawn.y = 0;
        PlaySfx(164);
        gCurTask->pengyIceBreathSlot = CreateActorFromDescAtOffsetFacing(&spawn, 0);
        CreateChildTaskAtOffsetFacing(TASK_PENGY_ICE_BREATH_PUFF, 10, 0, 1);
        {
            s32 id = CreateChildTaskAtOffsetFacing(TASK_PENGY_ICE_BREATH_SPARKLE, 10, 0, 1);

            if (id != -1)
            {
                struct Task *p = &gTasks[id];

                p->pengyIceBreathSparkleIndex = i;
            }
        }
        TaskSetFrame(8);
        TaskYieldTrampoline(5);
        gCurTask->frame++;
        TaskYieldTrampoline(5);
        if (++i > 2)
            i = 0;
    } while ((s16)++gCurTask->pengyLoopCount <= 9);
    ActorSetState(PENGY_STATE_WAIT);
    TaskSleepForever();
}

void PengyShootUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != PENGY_STATE_SHOOT)
    {
        t->pengyShotTimer = 240;
        t->pengyNearTimer = 0;
        t->pengyWaitTimer = 0;
        TaskSetEntry(PengyEnterState, gCurTaskIdx);
    }
}

void PengyFall(void)
{
    gCurTask->updateState = PENGY_STATE_FALL;
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
    ActorSetState(PENGY_IDLE_STATE_IDLE);
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
    gCurTask->updateState = PENGY_IDLE_STATE_IDLE;
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

s32 BomberStartFall(void)
{
    ActorSetState(BOMBER_STATE_1);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 BomberLand(void)
{
    ActorSetState(BOMBER_STATE_EXPLODE);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 BomberEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_08079a70(void)
{
    ActorSetState(BOMBER_STATE_2);
    TaskSetEntry(BomberEnterState, gCurTaskIdx);
    return 1;
}

s32 BomberBounceOffWall(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void BomberInit(void)
{
    gCurTask->updateCallback = (u32)BomberUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(BOMBER_STATE_WALK);
    gCurTask->bomberHasWalked = 0;
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
    gCurTask->updateState = BOMBER_STATE_WALK;
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

    if (t->bomberHasWalked == 0)
        t->bomberHasWalked = 1;
}

void BomberState1(void)
{
    gCurTask->updateState = BOMBER_STATE_1;
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

    gCurTask->updateState = BOMBER_STATE_2;
    TaskStop();
    t = gCurTask;
    t->posX = (t->pixelX + t->facing * 4) << 16;
    if (t->bomberHasWalked == 1)
    {
        t->bomberLoopCount = 0;
        do
        {
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->bomberLoopCount <= 4);
    }
    {
        struct Task *u = gCurTask;

        u->posX = (u->pixelX + u->facing * 4) << 16;
    }
    ActorSetState(BOMBER_STATE_1);
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

    gCurTask->updateState = BOMBER_STATE_EXPLODE;
    TaskStop();
    v = gScreenAttackActive;
    if (v == 0)
    {
        ActorSetAttackBox(gBomberExplodeAttackBox);
        FreezeStage(15);
        gCurTask->bomberLoopCount = v;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(1);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->bomberLoopCount <= 3);
        ThawStage();
        ActorSetHitReactions(gBomberExplodeHitReactions);
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
    ActorSetState(BOMBER_IDLE_STATE_IDLE);
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
    gCurTask->updateState = BOMBER_IDLE_STATE_IDLE;
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

void SparkyTeardown(void)
{
    if (--gPaletteAnimRefCounts[0] < 0)
        sub_0806ee2c();
}

void SparkySetJumpMotion(void)
{
    struct Task *t = gCurTask;

    if (t->sparkyJumpIndex != 0)
        TaskSetMotionXFacing(gUnk_087407DC[t->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(-gUnk_087407C4[gCurTask->sparkyJumpIndex],
                 gUnk_087407D0[gCurTask->sparkyJumpIndex], 0x30000);
    gCurTask->onGround = 0;
}

void sub_08079eec(u16 a)
{
    s32 r = RandomRange(4);

    gCurTask->sparkyJumpIndex = r;
    if (r > 2)
        gCurTask->sparkyJumpIndex = 2;
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

s32 SparkyStartFall(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(SPARKY_JUMP_STATE_3);
    TaskSetEntry(SparkyJumpEnterState, gCurTaskIdx);
    return 1;
}

s32 SparkyLand(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(SPARKY_JUMP_STATE_LAND);
    TaskSetEntry(SparkyJumpEnterState, gCurTaskIdx);
    return 1;
}

s32 SparkyEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 SparkyBounceOffWall(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void SparkyHitCeiling(void)
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
        if (t->hitKind != HIT_KIND_NONE && t->unk46 != -1)
            ActorDestroySlot(t->unk46);
    }
}

void SparkyJumpEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gSparkyJumpStates);
}

void SparkyJump(void)
{
    gCurTask->updateState = SPARKY_JUMP_STATE_JUMP;
    TaskStop();
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_087407C0[gCurTask->actorSpawnArg]);
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
    gCurTask->updateState = SPARKY_JUMP_STATE_DISCHARGE;
    TaskStop();
    gCurTask->sparkyPhase = 0;
    gCurTask->sparkySoundTimer = 3;
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
    gCurTask->sparkyPhase = 2;
    gCurTask->sparkyLoopCount = 0;
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
    } while ((s16)++gCurTask->sparkyLoopCount <= 7);
    gCurTask->sparkyPhase = 1;
    TaskSleepForever();
}

void SparkyJumpDischargeUpdate(void)
{
    struct Task *t = gCurTask;
    s32 n = t->sparkyPhase;

    if (n == 1)
    {
        SparkyPickNextState(2);
    }
    else if (n == 2)
    {
        if (t->sparkySoundTimer <= 0)
        {
            PlaySfx(191);
            gCurTask->sparkySoundTimer = 3;
        }
        gCurTask->sparkySoundTimer--;
    }
}

void SparkyJumpLand(void)
{
    gCurTask->updateState = SPARKY_JUMP_STATE_LAND;
    gCurTask->sparkyPhase = 0;
    TaskStopY();
    gCurTask->velX >>= 1;
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_087407BC[gCurTask->actorSpawnArg]);
    gCurTask->sparkyPhase = 1;
    TaskSleepForever();
}

void SparkyJumpLandUpdate(void)
{
    if (gCurTask->sparkyPhase != 0)
        SparkyPickNextState(2);
}

void SparkyJumpState3(void)
{
    gCurTask->updateState = SPARKY_JUMP_STATE_3;
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
    ActorSetState(SPARKY_IDLE_STATE_IDLE);
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
    gCurTask->updateState = SPARKY_IDLE_STATE_IDLE;
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
        if (t->hitKind != HIT_KIND_NONE && t->unk46 != -1)
            ActorDestroySlot(t->unk46);
    }
}

void SparkyStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gSparkyStandStates);
}

void SparkyStandDischarge(void)
{
    gCurTask->updateState = SPARKY_STAND_STATE_DISCHARGE;
    TaskStop();
    gCurTask->sparkyPhase = 0;
    gCurTask->sparkySoundTimer = 3;
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
    gCurTask->sparkyPhase = 2;
    gCurTask->sparkyLoopCount = 0;
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
    } while ((s16)++gCurTask->sparkyLoopCount <= 7);
    gCurTask->sparkyPhase = 1;
    TaskSleepForever();
}

void SparkyStandDischargeUpdate(void)
{
    struct Task *t = gCurTask;
    s32 n = t->sparkyPhase;

    if (n == 1)
    {
        SparkyPickNextState(1);
    }
    else if (n == 2)
    {
        if (t->sparkySoundTimer <= 0)
        {
            PlaySfx(191);
            gCurTask->sparkySoundTimer = 3;
        }
        gCurTask->sparkySoundTimer--;
    }
}

void SparkyStandWait(void)
{
    gCurTask->updateState = SPARKY_STAND_STATE_WAIT;
    gCurTask->sparkyPhase = 0;
    TaskStopY();
    gCurTask->velX >>= 1;
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_0874080C[gCurTask->actorSpawnArg]);
    gCurTask->sparkyPhase = 1;
    TaskSleepForever();
}

void SparkyStandWaitUpdate(void)
{
    if (gCurTask->sparkyPhase != 0)
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
        ActorSetState(SCARFY_STATE_HOVER);
    else
        ActorSetState(SCARFY_STATE_HIDE);
}

void sub_0807a8fc(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        gCurTask->scarfyWaveSign = -1;
        gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08740854);
        break;
    case 1:
        gCurTask->scarfyWaveSign = 1;
        break;
    case 2:
        gCurTask->scarfyWaveSign = 1;
        break;
    case 3:
    case 4:
        gCurTask->scarfyWaveSign = -1;
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

    if (t->hitKind == HIT_KIND_NO_DAMAGE && (u16)(t->hitEffect - 2) <= 1)
    {
        ActorSetAttackBox(gScarfyCheckTransformAttackBox);
        ActorSetState(SCARFY_STATE_TRANSFORM);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}
