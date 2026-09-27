/* game_code_and_rodata 0x0807D3B0-0x0807F044 (issue #77, module M20 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807D3B0 0x0807F044 src/enemy_7d3b0.c --newpb
 *
 * The last third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Enemies (Task_Rocky,
 * Task_SirKibble, Task_Cappy, Task_Gordo, Task_CoolSpook and the head of
 * Task_Kabu) and their helper tasks (#155 run 2; an older reading called them
 * moving scenery): scripts that drive Task.velX/velY and Task.accelY from ROM
 * tables and wait on Task.onGround:
 *   * task types #216 and #217 (`Task_PengyIceBreathPuff`, `Task_PengyIceBreathSparkle`, the latter a
 *     seventeen-step cue script over `0x087410C0`), the effect tasks Pengy's
 *     breath state PengyShoot spawns with CreateChildTaskAtOffsetFacing;
 *   * Task_Rocky's (#9) row 0 `RockyWalkInit`+`RockyWalkUpdate`, whose per-frame
 *     handlers `sub_0807d82c` / `sub_0807d918` re-centre on the nearest
 *     player when `|TaskGetNearestPlayerDx()| <= 49` and `|TaskGetDyTo()| <= 15`;
 *   * the class-3 three-way branch pair `sub_0807dd70` / `sub_0807dddc`
 *     (`switch (Task.variant)` with an empty `case 1`);
 *   * Task_SirKibble's `0x08741220` function-pointer table the three
 *     `sub_0807e244` / `sub_0807e3b0` hooks dispatch through;
 *   * Sir Kibble's jump-and-throw state `SirKibbleJump` (rows 0/1, state 2)
 *     and the capless Cappy's hop `sub_0807e768` (row 1, state 0), which
 *     spawn a companion with `CreateActorFromDescAtOffsetFacing` and then
 *     bounce between velocity presets until Task.onGround fires;
 *   * Gordo's four movement states (`sub_0807ea84`, `sub_0807eb60`,
 *     `sub_0807ec4c`, `sub_0807ed20`) and Cool Spook's float `sub_0807ef7c`,
 *     each an infinite eight-step velocity ramp.
 *
 * `sub_0807daf0`, `sub_0807e428`, `sub_0807e5a0`, `sub_0807e904`,
 * `sub_0807ee44` and `sub_0807ef08` are dead exports: each is a copy of its
 * host's tail dispatch that nothing in the ROM references (curated in
 * tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetDxTo(s32 i);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetDyTo(u32 i);
extern s32 TaskGetFacingTowardNearestPlayer(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnim(s32 n);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_0806956c();
extern s32 sub_080695bc();
extern s32 ActorReactToHit(void);
extern u32 ActorCheckHitsWithBox(void *p);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u8 TaskHasSameSerial(s32 i);
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskFree(s32 a);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionX(s32 a, s32 b, s32 c);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void RequestScreenShake(s32 a);
extern void ActorLoadDef(u32 *def);
extern void ActorSetState(u32 v);
extern void ActorSetAttackBox(u32 *p);
extern void TaskFaceToward(u32 i);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskFaceLikeParent(void);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void AcquirePaletteAnim(u32 p0, s32 idx);
extern void sub_0806a0f0(s32 a);
extern void sub_080b5540(s32 a, s32 b);

/* Forward declarations */
void RockyWalkUpdate(void);
void sub_0807db0c(void);
void RockyStandUpdate(void);
void sub_0807dd10(void);
void SirKibbleStandUpdate(void);
void SirKibbleWalkUpdate(void);
void sub_0807e444(void);
void sub_0807e484(void);
void CappyCappedUpdate(void);
void CappyCaplessUpdate(void);
void sub_0807e920(void);
void GordoBobUpdate(void);
void GordoBounceVerticalUpdate(void);
void GordoBounceHorizontalUpdate(void);
void GordoSweepUpdate(void);
void sub_0807ee60(void);
void sub_0807ef24(void);
void sub_0807ef7c(void);

void sub_0807d3b0(void)
{
    s32 v;

    if (gTaskSlotTypes[gCurTask->parent] == -1)
        goto kill1;
    v = (u8)TaskHasSameSerial(gCurTask->parent);
    if (v != 1)
        goto kill1;
    {
        struct Task *t = gCurTask;
        struct Task *o = &gTasks[t->parent];
        s16 *s;

        if ((u16)(o->unk76 - 28) > 1)
            goto kill2;
        s = &o->pixelX;
        t->pixelX = o->unk2C * 20 + *s;
        t->pixelY = o->pixelY;
        if (t->unk18 != 0)
        {
            ActorCheckHitsWithBox(gUnk_08741174);
            return;
        }
    }
    if (ActorCheckHitsWithBox(gUnk_08741158) != 0)
    {
        if (gCurTask->hitKind == 6)
        {
            PlaySfx(243);
            gCurTask->unk18 = v;
        }
    }
    return;

kill2:
    TaskFree(gCurTaskIdx);
    return;

kill1:
    TaskFree(gCurTaskIdx);
}

void Task_PengyIceBreathPuff(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gPengyIceBreathFrames;
    t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0x30000, -0x5000);
    {
        struct Task *u = gCurTask;

        u->accelY = -0x4000;
        u->frame = 2;
    }
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}

void Task_PengyIceBreathSparkle(void)
{
    u32 a;
    u32 b;
    s32 i;
    s32 j;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    {
        struct Task *t = gCurTask;

        t->frameTable = gPengyIceBreathFrames;
        t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    }
    TaskFaceLikeParent();
    i = gCurTask->unk74;
    j = i * 2;
    a = gUnk_087410C0[j];
    b = gUnk_087410C0[j + 1];
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = -1;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->velY = gUnk_087410D8[t->unk74];
        t->frame--;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void Task_Rocky(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gRockyFrames;
    CallTableEntry(gCurTask->variant, 3, gRockyVariants);
}

void RockyWalkInit(void)
{
    gCurTask->updateCallback = (u32)RockyWalkUpdate;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 5, gRockyWalkStates);
}

void RockyWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 5, gRockyWalkStates);
}

void RockyWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gRockyWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807d748(void)
{
    gCurTask->updateState = 0;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x5800, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->frame++;
        }
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x4800, 0x5A5A5A5A);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->frame++;
        }
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->unk6C <= 1);
    gCurTask->velX = 0;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0807d82c(void)
{
    gCurTask->unk2C = TaskFindNearestPlayer();
    if (abs(TaskGetDxTo(gCurTask->unk2C)) <= 49)
    {
        if (abs(TaskGetDyTo(gCurTask->unk2C)) <= 15)
        {
            struct Task *t = gCurTask;
            s32 n = --t->unk28;

            if (n <= 0)
            {
                t->unk30 = n < 0 ? 4 : 3;
                if (RandomRange(gCurTask->unk30) == 0)
                {
                    TaskFaceToward(gCurTask->unk2C);
                    ActorSetState(2);
                }
                gCurTask->unk28 = 60;
            }
        }
    }
    if (gCurTask->state != 0)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void sub_0807d8f8(void)
{
    gCurTask->updateState = 1;
    sub_0807dd10();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807d918(void)
{
    gCurTask->unk2C = TaskFindNearestPlayer();
    if (abs(TaskGetDxTo(gCurTask->unk2C)) <= 49)
    {
        if (abs(TaskGetDyTo(gCurTask->unk2C)) <= 15)
            TaskFaceToward(gCurTask->unk2C);
    }
    if (gCurTask->state != 1)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void sub_0807d9ac(void)
{
    gCurTask->onGround = 0;
    gCurTask->updateState = 2;
    TaskSetMotionXFacing(0x18000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x2E800;
        t->accelY = 0x2000;
    }
    TaskSetFrame(14);
    TaskYieldTrampoline(24);
    TaskSetFrame(12);
    TaskStop();
    TaskYieldTrampoline(16);
    ActorSetState(4);
    TaskSleepForever();
}

void sub_0807da08(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void sub_0807da30(void)
{
    gCurTask->updateState = 3;
    TaskSetFrame(12);
    TaskStop();
    TaskYieldTrampoline(16);
    ActorSetState(4);
    TaskSleepForever();
}

void sub_0807da5c(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
}

void sub_0807da84(void)
{
    gCurTask->updateState = 4;
    gCurTask->velY = 0x80000;
    TaskSetFrame(12);
    TaskSleepForever();
}

void sub_0807daa8(void)
{
}

void sub_0807daac(void)
{
    gCurTask->updateCallback = (u32)sub_0807db0c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087411F4);
}

void sub_0807daf0(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_087411F4);
}

void sub_0807db0c(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087411F8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807db30(void)
{
    gCurTask->updateState = 0;
    while (1)
        sub_0807dd10();
}

void sub_0807db44(void)
{
}

void RockyStandInit(void)
{
    gCurTask->updateCallback = (u32)RockyStandUpdate;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gRockyStandStates);
}

void RockyStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gRockyStandStates);
}

void RockyStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gRockyStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807dbcc(void)
{
    gCurTask->updateState = 0;
    {
        struct Task *t = gCurTask;

        t->velX = 0;
        t->unk6C = 0;
    }
    do
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->unk6C <= 1);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0807dc78(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
}

void sub_0807dca0(void)
{
    gCurTask->updateState = 1;
    sub_0807dd10();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807dcc0(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
}

void sub_0807dce8(void)
{
    gCurTask->updateState = 2;
    gCurTask->velY = 0x80000;
    TaskSetFrame(12);
    TaskSleepForever();
}

void sub_0807dd0c(void)
{
}

void sub_0807dd10(void)
{
    TaskSetFrame(12);
    TaskYieldTrampoline(100);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame--;
    TaskYieldTrampoline(60);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame--;
    TaskYieldTrampoline(18);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
}

s32 sub_0807dd70(void)
{
    s32 r;

    {
        struct Task *t = gCurTask;

        t->velX = 0;
        t->velY = 0;
    }
    RequestScreenShake(1);
    PlaySfx(163);
    r = 0;
    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(1);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(1);
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807dddc(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(4);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(RockyStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807de30(void)
{
    s32 r = 0;

    if (gCurTask->variant == 0)
    {
        ActorSetState(3);
        TaskSetEntry(RockyWalkEnterState, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_0807de64(void)
{
    struct Task *t = gCurTask;

    if (t->state == 2)
        t->velX = 0;
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_0807de88(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void Task_SirKibble(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSirKibbleFrames;
    TaskFaceNearestPlayer();
    {
        struct Task *t = gCurTask;

        t->unk2C = 0;
        CallTableEntry(t->variant, 3, gSirKibbleVariants);
    }
}

void SirKibbleStandInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleStandUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gSirKibbleStandStates);
}

void SirKibbleStandEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gSirKibbleStandStates);
}

void SirKibbleStandUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gSirKibbleStandStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SirKibbleWait(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk28 = gUnk_08741216[t->unk74];
    sub_0807e484();
}

void sub_0807df8c(void)
{
    struct Task *t = gCurTask;

    if (--t->unk28 == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (RandomRange(gUnk_08741214[gCurTask->unk74]) == 0)
                ActorSetState(2);
            else
                ActorSetState(1);
        }
        else
        {
            ActorSetState(1);
        }
        gCurTask->velX = 0;
        TaskSetEntry(SirKibbleStandEnterState, gCurTaskIdx);
    }
}

void SirKibbleWalkInit(void)
{
    gCurTask->updateCallback = (u32)SirKibbleWalkUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gSirKibbleWalkStates);
}

void SirKibbleWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gSirKibbleWalkStates);
}

void SirKibbleWalkUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gSirKibbleWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void SirKibbleWalk(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk28 = gUnk_08741216[t->unk74];
    TaskSetMotionXFacing(gUnk_08741218[t->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(12);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        TaskSetFrame(8);
        TaskYieldTrampoline(12);
        TaskSetFrame(6);
        TaskYieldTrampoline(12);
    }
}

void sub_0807e100(void)
{
    struct Task *t = gCurTask;

    if (--t->unk28 == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 63)
        {
            if (RandomRange(gUnk_08741214[gCurTask->unk74]) == 0)
                ActorSetState(2);
            else
                ActorSetState(1);
        }
        else
        {
            ActorSetState(1);
        }
        gCurTask->velX = 0;
        TaskSetEntry(SirKibbleWalkEnterState, gCurTaskIdx);
    }
}

void SirKibbleShoot(void)
{
    struct ActorSpawn spawn;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(9);
    TaskYieldTrampoline(40);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    spawn.subtype = 3;
    spawn.taskType = 105;
    spawn.variant = 0;
    spawn.spawnArg = 0;
    spawn.x = 16;
    spawn.y = 0;
    spawn.checkTerrain = 0;
    {
        struct Task *t;
        s32 id = CreateActorFromDescAtOffsetFacing(&spawn, 0);

        t = gCurTask;
        t->unk46 = id;
        t->unk28 = 88;
    }
    do
        TaskYieldTrampoline(1);
    while (gCurTask->unk28 != 0);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(12);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807e244(void)
{
    {
        struct Task *t = gCurTask;

        if (t->onGround != 0 && t->unk28 > 0)
            t->unk28--;
    }
    {
        struct Task *t = gCurTask;

        if (t->state != 1)
            TaskSetEntry((void *)gUnk_08741220[t->variant], gCurTaskIdx);
    }
}

void SirKibbleJump(void)
{
    struct ActorSpawn spawn;

    gCurTask->updateState = 2;
    {
        struct Task *t;
        s32 r = TaskGetFacingTowardNearestPlayer();

        t = gCurTask;
        t->unk30 = r;
        if (r == 1)
            t->facing = 255;
        else
            t->facing = 1;
    }
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(9);
    TaskYieldTrampoline(40);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    spawn.subtype = 3;
    spawn.taskType = 105;
    spawn.variant = 0;
    spawn.spawnArg = 1;
    spawn.x = 16;
    spawn.y = 0;
    spawn.checkTerrain = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 0);
    TaskYieldTrampoline(30);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;

        if (t->onGround != 0)
        {
            struct Task *u;

            t->onGround = 0;
            u = gCurTask;
            u->velY = -0x40000;
            u->accelY = 0x4000;
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
        }
    }
    {
        struct Task *t = gCurTask;

        t->velY = 0;
        t->accelY = 0;
        t->facing = t->unk30;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807e3b0(void)
{
    struct Task *t = gCurTask;

    if (t->state != 2)
        TaskSetEntry((void *)gUnk_08741220[t->variant], gCurTaskIdx);
}

void sub_0807e3e4(void)
{
    gCurTask->updateCallback = (u32)sub_0807e444;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741264);
}

void sub_0807e428(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741264);
}

void sub_0807e444(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08741268);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e468(void)
{
    gCurTask->updateState = 0;
    sub_0807e484();
}

void sub_0807e480(void)
{
}

void sub_0807e484(void)
{
    TaskSetFrame(5);
    while (1)
    {
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(16);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
    }
}

s32 sub_0807e4cc(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    TaskSetFrame((s16)gCurTask->frame);
    t = gCurTask;
    TaskSetMotionX(-t->velX, -t->accelX, t->speedLimitX);
    return 0;
}

s32 sub_0807e4fc(void)
{
    TaskSetMotionY(0, 0x2500, 0x30000);
    return 0;
}

s32 sub_0807e514(void)
{
    TaskStopY();
    return 0;
}

s32 sub_0807e520(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void Task_Cappy(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    CallTableEntry(gCurTask->variant, 3, gCappyVariants);
}

void CappyCappedInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CappyCappedUpdate;
    t->frameTable = gCappyFrames;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gCappyCappedStates);
}

void sub_0807e5a0(void)
{
    CallTableEntry(gCurTask->state, 1, gCappyCappedStates);
}

void CappyCappedUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gCappyCappedStateUpdates);
    ActorCheckHits();
    {
        struct Task *t = gCurTask;

        if (t->hitKind == 3)
        {
            gCurTask->unk46 = CreateActorByKind(0, 8, 1, 0, t->pixelX, t->pixelY,
                                                t->tileWord);
            sub_080b5540(gCurTaskIdx, gCurTask->unk46);
        }
    }
    ActorReactToHit();
}

void sub_0807e640(void)
{
    gCurTask->updateState = 0;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->unk28 = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        while (--gCurTask->unk28 >= 0)
        {
            gCurTask->onGround = 0;
            {
                struct Task *u = gCurTask;

                u->velY = -0x10000;
                u->accelY = 0x1000;
            }
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
            gCurTask->spriteFlags ^= 0x8000;
        }
    }
}

void sub_0807e6d0(void)
{
}

void CappyCaplessInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CappyCaplessUpdate;
    t->frameTable = gCappyCaplessFrames;
    ActorLoadDef(gUnk_0874183C);
    ActorSetState(1);
    CallTableEntry(gCurTask->state, 2, gCappyCaplessStates);
}

void CappyCaplessEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gCappyCaplessStates);
}

void CappyCaplessUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gCappyCaplessStateUpdates);
    if (gCurTask->updateState != 1)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e768(void)
{
    gCurTask->updateState = 0;
    TaskSetFrame(4);
    while (1)
    {
        gCurTask->unk28 = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        TaskSetFrame((s16)gCurTask->frame);
        while (--gCurTask->unk28 >= 0)
        {
            gCurTask->onGround = 0;
            {
                struct Task *u = gCurTask;

                u->velY = -0x10000;
                u->accelY = 0x1000;
            }
            while (gCurTask->onGround == 0)
                TaskYieldTrampoline(1);
            if ((s16)gCurTask->frame == 6)
                TaskSetFrame(4);
            else
                TaskSetFrame(6);
        }
    }
}

void sub_0807e810(void)
{
}

void sub_0807e814(void)
{
    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(-0x8000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x20000;
        t->accelY = 0x2000;
    }
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
}

void sub_0807e884(void)
{
    if (gCurTask->onGround != 0)
    {
        ActorSetState(0);
        TaskSetEntry(CappyCaplessEnterState, gCurTaskIdx);
    }
}

void sub_0807e8b8(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_0807e920;
    t->frameTable = gCappyFrames;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741290);
}

void sub_0807e904(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741290);
}

void sub_0807e920(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_08741294);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e950(void)
{
    gCurTask->updateState = 0;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->onGround = 0;
        {
            struct Task *t = gCurTask;

            t->velY = -0x10000;
            t->accelY = 0x1000;
        }
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        gCurTask->spriteFlags ^= 0x8000;
    }
}

void sub_0807e9b0(void)
{
}

s32 sub_0807e9b4(void)
{
    struct Task *t = gCurTask;

    t->velX = -t->velX;
    return 0;
}

s32 sub_0807e9c8(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void Task_Gordo(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    {
        struct Task *t = gCurTask;

        t->frameTable = gGordoFrames;
        t->onGround = 0;
    }
    TaskFaceNearestPlayer();
    gCurTask->unk28 = ActorStartAnim(gUnk_087412A8);
    CallTableEntry(gCurTask->variant, 4, gGordoVariants);
}

void GordoBobInit(void)
{
    gCurTask->updateCallback = (u32)GordoBobUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gGordoBobStates);
}

void GordoBobUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gGordoBobStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ea84(void)
{
    gCurTask->updateState = 0;
    while (1)
    {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807ead4(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void GordoBounceVerticalInit(void)
{
    gCurTask->updateCallback = (u32)GordoBounceVerticalUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gGordoBounceVerticalStates);
}

void GordoBounceVerticalUpdate(void)
{
    if ((u8)sub_0806956c() == 1)
    {
        struct Task *t = gCurTask;

        t->velY = -t->velY;
        t->onGround = 0;
    }
    CallTableEntry(gCurTask->updateState, 1, gGordoBounceVerticalStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807eb60(void)
{
    gCurTask->updateState = 0;
    {
        struct Task *t = gCurTask;

        t->velY = gUnk_08741298[t->unk74];
    }
    while (1)
    {
        gCurTask->velX = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ebc4(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void GordoBounceHorizontalInit(void)
{
    gCurTask->updateCallback = (u32)GordoBounceHorizontalUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gGordoBounceHorizontalStates);
}

void GordoBounceHorizontalUpdate(void)
{
    if ((u8)sub_080695bc() == 1)
    {
        struct Task *t = gCurTask;

        t->velX = -t->velX;
    }
    CallTableEntry(gCurTask->updateState, 1, gGordoBounceHorizontalStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ec4c(void)
{
    gCurTask->updateState = 0;
    {
        struct Task *t = gCurTask;

        t->velX = gUnk_087412A0[t->unk74];
    }
    while (1)
    {
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ecb0(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void GordoSweepInit(void)
{
    gCurTask->updateCallback = (u32)GordoSweepUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gGordoSweepStates);
}

void GordoSweepUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gGordoSweepStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ed20(void)
{
    gCurTask->updateState = 0;
    while (1)
    {
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(96);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0xC000;
        TaskYieldTrampoline(96);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
    }
}

void sub_0807ed98(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void Task_CoolSpook(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gCoolSpookFrames;
    AcquirePaletteAnim(0, 0);
    gCurTask->facing = 255;
    gCurTask->unk28 = ActorStartAnim(gUnk_087412EC);
    CallTableEntry(gCurTask->variant, 2, gCoolSpookVariants);
}

void sub_0807ee14(void)
{
    gCurTask->updateCallback = (u32)sub_0807ee60;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741308);
}

void sub_0807ee44(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741308);
}

void sub_0807ee60(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0874130C);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ee84(void)
{
    gCurTask->updateState = 0;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_0807ef7c();
}

void sub_0807eea8(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807eec4(void)
{
    gCurTask->updateCallback = (u32)sub_0807ef24;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741310);
}

void sub_0807ef08(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741310);
}

void sub_0807ef24(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08741314);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ef48(void)
{
    gCurTask->updateState = 0;
    sub_0807ef7c();
}

void sub_0807ef60(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807ef7c(void)
{
    while (1)
    {
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807efec(void)
{
    gUnk_02007FB8[0]--;
}

void Task_Kabu(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gKabuFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 4, gKabuVariants);
}
