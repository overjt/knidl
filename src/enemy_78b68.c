/* game_code_and_rodata 0x08078B68-0x0807AA5C (issue #77, module M20 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08078B68 0x0807AA5C src/enemy_78b68.c --newpb
 *
 * M20 is enemy/object behaviour bank 1: twenty-one ROM task types whose
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
 *   * the `0x08740648` / `0x08740668` cue+delay pair the door/lift scripts
 *     index by Task.unk74 (`sub_08078c80`, `sub_08078d88`);
 *   * task #9's four-state script `PengyInit`+`PengyUpdate` with its
 *     `0x08740758` class-4 dispatch and the `sub_080795d8` state machine
 *     (a screen-shake amplitude test plus a 120-frame timer);
 *   * `PengyShoot`, which spawns three class-4 actors from a stack
 *     `struct ActorSpawn` and clears Task.unk74 on the companion it gets
 *     back from CreateChildTaskAtOffsetFacing;
 *   * the `sub_08079eec` / `sub_08079f18` / `sub_08079f54` sound-cue chain
 *     (all `u16`-parameterised) that every later script funnels its
 *     "player hit me" reaction through;
 *   * the two 0x1C0-byte cutscene coroutines `sub_0807a1c0` and
 *     `sub_0807a634` (identical: a seventeen-step frame script followed by an
 *     eight-iteration palette flip between `0x08740DE4` and `0x0873F774`);
 *   * `sub_0807a8fc`, the bank's only `mov pc` jump table (five states over
 *     Task.variant), and `sub_0807a968`, which clamps a spawn point from
 *     `0x08740824` into the camera box `gViewRect[0..3]`.
 *
 * `sub_0807927c`, `sub_080794d0`, `sub_080799a0`, `sub_08079db8` and
 * `sub_0807a4e4` are dead exports: each is a copy of its host's tail dispatch
 * that nothing in the ROM references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gViewRect[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s8 gUnk_02007FB8[];
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern u8 gUnk_02006178;
extern vs16 gTaskSlotTypes[];

/* ROM tables */
extern s16 gUnk_08740668[];
extern s16 gUnk_08740740[];
extern s16 gUnk_087407BC[];
extern s16 gUnk_087407C0[];
extern s16 gUnk_0874080C[];
extern struct AnimCmd gUnk_087406A0[];
extern struct AnimCmd gUnk_087406EC[];
extern struct AnimCmd gUnk_0874074C[];
extern struct AnimCmd gUnk_08740854[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F774[];
extern u32 gUnk_0873F7AC[];
extern u32 gUnk_08740648[];
extern u32 gUnk_08740658[];
extern u32 gUnk_08740660[];
extern u32 gUnk_08740670[];
extern u32 gUnk_08740678[];
extern u32 gUnk_08740680[];
extern u32 gUnk_08740690[];
extern u32 gUnk_087406C4[];
extern u32 gUnk_087406D0[];
extern u32 gUnk_087406DC[];
extern u32 gUnk_087406E4[];
extern u32 gUnk_08740700[];
extern u32 gUnk_08740704[];
extern u32 gUnk_08740708[];
extern u32 gUnk_08740710[];
extern u32 gPengyVariants[];
extern u32 gUnk_08740720[];
extern u32 gUnk_08740728[];
extern u32 gPengyStates[];
extern u32 gPengyStateUpdates[];
extern u32 gUnk_08740778[];
extern u32 gUnk_0874077C[];
extern u32 gBomberVariants[];
extern u32 gBomberStates[];
extern u32 gBomberStateUpdates[];
extern u32 gUnk_087407A8[];
extern u32 gUnk_087407AC[];
extern u32 gSparkyVariants[];
extern u32 gUnk_087407C4[];
extern u32 gUnk_087407D0[];
extern u32 gUnk_087407DC[];
extern u32 gUnk_087407E4[];
extern u32 gUnk_087407F4[];
extern u32 gUnk_08740804[];
extern u32 gUnk_08740808[];
extern u32 gUnk_08740810[];
extern u32 gUnk_08740818[];
extern u32 gUnk_08740820[];
extern u32 gUnk_08740824[];
extern u32 gUnk_08740C00[];
extern u32 gUnk_08740DE4[];
extern u32 gUnk_08740E1C[];
extern u32 gUnk_08740F2C[];
extern u32 gPengyFrames[];
extern u32 gBomberFrames[];
extern u32 gSparkyFrames[];
extern u32 gScarfyFrames[];

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 TaskGetNearestPlayerDistSq(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern s32 ActorTickAnim(s32 n);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 ActorReactToHit(void);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorDie(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void TaskGetNearestPlayerPos(void);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroySlot(s32 i);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskTurnAround(void);
extern void ActorStopAnim(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void AcquirePaletteAnim(u32 p0, s32 idx);
extern void sub_08066b34(u32 *p);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void sub_080670ac(u32);
extern void sub_080670d4(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806ee2c(void);

/* Forward declarations */
void sub_08078c14(void);
void sub_08078d1c(void);
void sub_08078e9c(void);
void sub_08079128(void);
void sub_08079298(void);
void sub_0807933c(void);
void PengyUpdate(void);
void PengyEnterState(void);
void sub_08079578(void);
void sub_0807964c(void);
void sub_0807968c(void);
void sub_080796d8(void);
void sub_0807995c(void);
void BomberUpdate(void);
void BomberEnterState(void);
void sub_08079d74(void);
void sub_0807a09c(void);
void sub_0807a10c(void);
void sub_0807a4a0(void);
void sub_0807a5a8(void);
void sub_0807a618(void);
extern s32 ScarfyEnterState();

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

void sub_08078be0(void)
{
    gCurTask->updateCallback = (u32)sub_08078c14;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08740658);
}

void sub_08078c14(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08740660);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08078c64(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08740658);
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

void sub_08078ce8(void)
{
    gCurTask->updateCallback = (u32)sub_08078d1c;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08740670);
}

void sub_08078d1c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08740678);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08078d6c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08740670);
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

void sub_08078e3c(void)
{
    gCurTask->updateCallback = (u32)sub_08078e9c;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 80;
    ActorSetState(0);
    gCurTask->unk34 = ActorStartAnim(gUnk_087406A0);
    CallTableEntry(gCurTask->state, 3, gUnk_087406C4);
}

void sub_08078e80(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_087406C4);
}

void sub_08078e9c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_087406D0);
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
            TaskSetEntry(sub_08078e80, gCurTaskIdx);
        }
        else
        {
            gCurTask->unk28 = 30;
            ActorSetState(0);
            TaskSetEntry(sub_08078e80, gCurTaskIdx);
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
        TaskSetEntry(sub_08078e80, gCurTaskIdx);
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

void sub_080790ec(void)
{
    gCurTask->updateCallback = (u32)sub_08079128;
    sub_08066b34(gUnk_08740C00);
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_087406DC);
}

void sub_08079128(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_087406E4);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08079178(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_087406DC);
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
    if (gCurTask->unk8C->extraFrame == -1)
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

void sub_080792f8(void)
{
    gCurTask->updateCallback = (u32)sub_0807933c;
    sub_08066b34(gUnk_08740C00);
    gCurTask->unk74 = 2;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08740708);
}

void sub_0807933c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08740710);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807938c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08740708);
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
    if (gCurTask->unk8C->extraFrame == -1)
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

    t->unk8C->animScript = 0;
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
        TaskSetEntry(sub_08078c64, gCurTaskIdx);
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
        sub_080670ac(15);
        gCurTask->unk6C = v;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(1);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 3);
        sub_080670d4();
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
        TaskSetEntry(sub_0807a10c, gCurTaskIdx);
        break;
    case 2:
        TaskSetEntry(sub_0807a618, gCurTaskIdx);
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
    TaskSetEntry(sub_0807a10c, gCurTaskIdx);
    return 1;
}

s32 sub_0807a008(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(1);
    TaskSetEntry(sub_0807a10c, gCurTaskIdx);
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

void sub_0807a06c(void)
{
    gCurTask->updateCallback = (u32)sub_0807a09c;
    sub_08079fa8(2);
    CallTableEntry(gCurTask->state, 4, gUnk_087407E4);
}

void sub_0807a09c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gUnk_087407F4);
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

void sub_0807a10c(void)
{
    CallTableEntry(gCurTask->state, 4, gUnk_087407E4);
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

void sub_0807a578(void)
{
    gCurTask->updateCallback = (u32)sub_0807a5a8;
    sub_08079fa8(1);
    CallTableEntry(gCurTask->state, 2, gUnk_08740810);
}

void sub_0807a5a8(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08740818);
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

void sub_0807a618(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08740810);
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

    if (t->hitKind == 6 && (u16)(t->unk82 - 2) <= 1)
    {
        ActorSetAttackBox(gUnk_08740E1C);
        ActorSetState(3);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}
