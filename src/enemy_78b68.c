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
 *     states (rows 0/1: `sub_08078c80`, `sub_08078d88`) index by Task.unk74;
 *   * Task_Pengy's (#12) four-state row `PengyInit`+`PengyUpdate` with its
 *     state table gPengyStates (`0x08740758`) and the `sub_080795d8` check
 *     (the nearest player within 64 px horizontally, plus a cooldown timer);
 *   * `PengyShoot`, Pengy's breath state, which spawns actor type 102 ten
 *     times (cycling Task.unk74 0-2) from a stack `struct ActorSpawn` and
 *     clears Task.unk74 on the companion it gets back from
 *     CreateChildTaskAtOffsetFacing;
 *   * the `sub_08079eec` / `sub_08079f18` / `sub_08079f54` sound-cue chain
 *     (all `u16`-parameterised) that every later script funnels its
 *     "player hit me" reaction through;
 *   * the two 0x1C0-byte Task_Sparky states `sub_0807a1c0` (row 0) and
 *     `sub_0807a634` (row 2) (identical: a seventeen-step frame script followed by an
 *     eight-iteration palette flip between `0x08740DE4` and `0x0873F774`);
 *   * Task_Scarfy's `sub_0807a8fc`, the bank's only `mov pc` jump table (five
 *     cases over Task.variant), and `sub_0807a968`, which places Scarfy at an
 *     offset from the nearest player, clamping the point from
 *     `0x08740824` into the camera box `gViewRect[0..3]`.
 *
 * `sub_0807927c`, `sub_080794d0`, `sub_080799a0`, `sub_08079db8` and
 * `sub_0807a4e4` are dead exports: each is a copy of its host's tail dispatch
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

void sub_08078c80(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void sub_08078cb8(void)
{
}

void sub_08078cbc(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void sub_08078ce4(void)
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

void sub_08078d88(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    TaskStop();
    t = gCurTask;
    t->unk28 = gUnk_08740668[t->unk74];
    TaskSetMotionXFacing(gUnk_08740648[t->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void sub_08078dd4(void)
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

void sub_08078e10(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void sub_08078e38(void)
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

void sub_08078eec(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08740648[gCurTask->unk74], 0x5A5A5A5A);
    TaskSleepForever();
}

void sub_08078f24(void)
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

void sub_08078f8c(void)
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

void sub_0807906c(void)
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

void sub_080790c0(void)
{
    gCurTask->updateState = 2;
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_08078b68();
    TaskSleepForever();
}

void sub_080790e8(void)
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

void sub_08079194(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    sub_08078b68();
}

void sub_080791bc(void)
{
}

void sub_080791c0(void)
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
        sub_08066ba8();
        while (1)
        {
            sub_08066bdc();
            TaskYieldTrampoline(8);
        }
    }
}

void sub_0807921c(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_08079238(void)
{
    gCurTask->updateCallback = (u32)sub_08079298;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740700);
}

void sub_0807927c(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740700);
}

void sub_08079298(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740704);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_080792dc(void)
{
    gCurTask->updateState = 0;
    sub_08078b68();
}

void sub_080792f4(void)
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

void sub_080793a8(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    sub_08078b68();
}

void sub_080793c4(void)
{
    TaskFaceNearestPlayer();
}

void sub_080793d0(void)
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
        sub_08066ba8();
    }
    TaskSleepForever();
}

void sub_08079424(void)
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
    sub_0806a0f0(-2);
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
    sub_08079578();
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

void sub_08079578(void)
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

void sub_080795d8(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk28;

    if (n <= 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (gCurTask->unk2C <= 0 && RandomRange(2) == 0)
                sub_0807964c();
            else
                sub_080796d8();
        }
        else
        {
            gCurTask->unk2C = 0;
            sub_0807968c();
        }
    }
    else
    {
        t->unk2C = 0;
        t->unk28 = n - 1;
        sub_0807968c();
    }
}

void sub_0807964c(void)
{
    ActorSetState(2);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void sub_0807966c(void)
{
    ActorSetState(1);
    TaskSetEntry(PengyEnterState, gCurTaskIdx);
}

void sub_0807968c(void)
{
    s32 n = ++gCurTask->unk30;

    if (n == 120)
    {
        sub_0807966c();
    }
    else if (n == 75 || n == 90 || n == 105)
    {
        if (RandomRange(gUnk_08740720[gCurTask->unk74]) == 0)
            sub_0807966c();
    }
}

void sub_080796d8(void)
{
    struct Task *t = gCurTask;

    if (++t->unk2C == 33)
    {
        t->unk2C = 3;
        if (RandomRange(3) == 0)
            sub_0807968c();
        else
            sub_0807964c();
    }
    else
    {
        sub_0807968c();
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

void sub_080797b4(void)
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

void sub_080798b8(void)
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

void sub_08079914(void)
{
}

void sub_08079918(void)
{
    gCurTask->updateCallback = (u32)sub_0807995c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740778);
}

void sub_0807995c(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0874077C);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_080799a0(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740778);
}

void sub_080799bc(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_080799dc(void)
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
    sub_0806a0f0(-2);
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

void sub_08079b98(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0)
        t->unk28 = 1;
}

void sub_08079bac(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskTurnAround();
    TaskSetFrame(10);
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSleepForever();
}

void sub_08079be0(void)
{
}

void sub_08079be4(void)
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

void sub_08079c84(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
}

void sub_08079cac(void)
{
    u8 v;

    gCurTask->updateState = 3;
    TaskStop();
    v = gUnk_02006178;
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

void sub_08079d2c(void)
{
}

void sub_08079d30(void)
{
    gCurTask->updateCallback = (u32)sub_08079d74;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087407A8);
}

void sub_08079d74(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087407AC);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08079db8(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_087407A8);
}

void sub_08079dd4(void)
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

void sub_08079e20(void)
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
    if (--gUnk_02007FB8[0] < 0)
        sub_0806ee2c();
}

void sub_08079e8c(void)
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

void sub_08079f54(u16 a)
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

void sub_08079fa8(u16 a)
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
    sub_0806a0f0(-2);
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
    sub_08079fa8(2);
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

void sub_0807a128(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    TaskSetFrame(7);
    TaskYieldTrampoline(gUnk_087407C0[gCurTask->unk74]);
    sub_08079e8c();
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

void sub_0807a1bc(void)
{
}

void sub_0807a1c0(void)
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

void sub_0807a380(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk2C;

    if (n == 1)
    {
        sub_08079f54(2);
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

void sub_0807a3bc(void)
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

void sub_0807a408(void)
{
    if (gCurTask->unk2C != 0)
        sub_08079f54(2);
}

void sub_0807a424(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    TaskTurnAround();
    TaskSetFrame(8);
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSleepForever();
}

void sub_0807a458(void)
{
}

void sub_0807a45c(void)
{
    gCurTask->updateCallback = (u32)sub_0807a4a0;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740804);
}

void sub_0807a4a0(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740808);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807a4e4(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740804);
}

void sub_0807a500(void)
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

void sub_0807a574(void)
{
}

void SparkyStandInit(void)
{
    gCurTask->updateCallback = (u32)SparkyStandUpdate;
    sub_08079fa8(1);
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

void sub_0807a634(void)
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

void sub_0807a7f4(void)
{
    struct Task *t = gCurTask;
    s32 n = t->unk2C;

    if (n == 1)
    {
        sub_08079f54(1);
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

void sub_0807a830(void)
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

void sub_0807a87c(void)
{
    if (gCurTask->unk2C != 0)
        sub_08079f54(1);
}

void Task_Scarfy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gScarfyFrames;
    CallTableEntry(0, 1, gUnk_08740820);
}

void sub_0807a8d4(void)
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

void sub_0807aa0c(void)
{
    struct Task *t = gCurTask;

    if (t->hitKind == 6 && (u16)(t->hitEffect - 2) <= 1)
    {
        ActorSetAttackBox(gUnk_08740E1C);
        ActorSetState(3);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}
