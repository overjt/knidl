/* game_code_and_rodata 0x08099B20-0x0809BA44 (issue #68, module M27 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08099B20 0x0809BA44 src/enemy_99b20.c --newpb
 *
 * The tail of the first mid-boss script, M27's second one, and the three small
 * companion tasks that close the module.
 *
 * MrTickTockLand (22 cases) and MrTickTockHitWall (18) are the first script's
 * remaining jump-table dispatchers; MrTickTockChooseNextState picks the next animation from
 * one of gUnk_087456D4 / gUnk_087456E4 / gUnk_087456F4 / gUnk_08745704 by
 * classifying |TaskGetNearestPlayerDx()| against 128 and |TaskGetNearestPlayerDy()| against 64;
 * CreateMrTickTockRing and CreateMrTickTockNote build struct ActorSpawn records for the actors
 * 16 and 17; sub_0809a080 is the shared hit reaction (rumble RequestScreenShake(2)
 * or (4), then SE 0x1F7).  sub_08099fe0 and sub_08099fe4 are two dead `bx lr`
 * state handlers nothing in the ROM points at.
 *
 * The second script starts at Task_MrTickTock (graphics gMrTickTockFrames, animation
 * gUnk_08745744, one-word table gMrTickTockVariants): MrTickTockInit installs
 * MrTickTockUpdate and dispatches Task.state through the 24-word guard table
 * gMrTickTockStates, MrTickTockUpdate dispatches Task.updateState through the 24-word body
 * table gMrTickTockStateUpdates that follows it, and MrTickTockEnterState is its re-arm hook.
 * States 0-23 follow as <body, guard> pairs; MrTickTockDropInUpdate is the timer leaf
 * the table word at 0x0874580C points at.
 *
 * Task_MrFrostyIceCube, Task_MrTickTockRing and Task_MrTickTockNote are the three companion tasks
 * (graphics gUnk_0874CB7C, gMrTickTockRingFrames, gMrTickTockNoteFrames).  They use
 * ActorDrawWorldInViewOrDestroy as the per-frame hook and Task.layer = 9; MrFrostyIceCubeState0Update and
 * MrTickTockRingState0Update read the parent's state out of gTasks[Task.parent], and
 * the third one's own states live in the next module - gMrTickTockNoteVariants points at
 * MrTickTockNoteInit.  MrTickTockRingEnterState is a dead copy of the gMrTickTockRingStates re-arm.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void TaskSetEntry(void *fn, s32 i);
extern s32 PlaySfx(s32 id);
extern void RequestScreenShake(s32 a);
extern u32 GetShapeAtPixelIgnoringOneWay(s32 x, s32 y);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void ActorSetTerrainBox(u32 *p);
extern void ActorSetExtraAttackBox(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern u32 ActorCheckHits(void);
extern void ActorCheckHitsWithExtraBox(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);

/* Defined below */
void sub_0809a080(u8 a);

u8 MrTickTockLand(void)
{
    switch (gCurTask->state)
    {
    case MR_TICK_TOCK_STATE_JUMP_LOW:
        RequestScreenShake(1);
        PlaySfx(0x1F7);
        ActorSetState(MR_TICK_TOCK_STATE_JUMP_BACK);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_JUMP_BACK:
        sub_0809a080(1);
        ActorSetState(MR_TICK_TOCK_STATE_SHOOT_NOTES);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_JUMP_FORWARD:
        TaskSetFrame(4);
        sub_0809a080(1);
        CreateLandingDust(0, 16);
        TaskStop();
    stop:
        gCurTask->mrTickTockStatePhase = 1;
        break;
    case MR_TICK_TOCK_STATE_HOP:
        TaskSetFrame(4);
        sub_0809a080(1);
        CreateLandingDust(0, 16);
        goto stop;
    case MR_TICK_TOCK_STATE_FALL:
        sub_0809a080(1);
        ActorSetState(MR_TICK_TOCK_STATE_WAIT_LONG);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_JUMP_HIGH:
        sub_0809a080(1);
        ActorSetState(MR_TICK_TOCK_STATE_RING_FROM_JUMP);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_BOUNCE_OFF_WALL:
        sub_0809a080(1);
        ActorSetState(MR_TICK_TOCK_STATE_16);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_DEFEAT:
        sub_0809a080(0);
        ActorSetState(MR_TICK_TOCK_STATE_20);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_DROP_IN:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(MR_TICK_TOCK_STATE_WAIT_LONG);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrTickTockHitWall(void)
{
    switch (gCurTask->state)
    {
    case MR_TICK_TOCK_STATE_17:
        ActorSetState(MR_TICK_TOCK_STATE_18);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_JUMP_FORWARD:
        gCurTask->velX = 0;
        return 0;
    case MR_TICK_TOCK_STATE_DASH_WIND_UP:
        ActorSetState(MR_TICK_TOCK_STATE_DASH_START);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_DASH_START:
        ActorSetState(MR_TICK_TOCK_STATE_BOUNCE_OFF_WALL);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_WALK_BACK:
        gCurTask->velX = -gCurTask->velX;
        break;
    case MR_TICK_TOCK_STATE_DASH_LOOP:
        ActorSetState(MR_TICK_TOCK_STATE_BOUNCE_OFF_WALL);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case MR_TICK_TOCK_STATE_16:
        TaskStop();
        gCurTask->mrTickTockStatePhase = 1;
        break;
    case MR_TICK_TOCK_STATE_DEFEAT:
        ActorSetTerrainBox(gUnk_08745A24);
    case MR_TICK_TOCK_STATE_JUMP_HIGH:
    case MR_TICK_TOCK_STATE_RING_FROM_DASH:
    case MR_TICK_TOCK_STATE_BOUNCE_OFF_WALL:
        TaskStopX();
        break;
    case MR_TICK_TOCK_STATE_20:
        ActorSetState(MR_TICK_TOCK_STATE_21);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrTickTockReactToDamage(void)
{
    gCurTask->mrTickTockFlashTimer = 32;
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    RequestScreenShake(4);
    return 0;
}

u8 MrTickTockReactToDefeat(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->state == MR_TICK_TOCK_STATE_SHOOT_NOTES)
        StopSfxOnPlayer(t->mrTickTockSfxPlayer, 0x219);
    ActorSetHitReactions(gMrTickTockDefeatedHitReactions);
    u = gCurTask;
    u->mrTickTockFlashEnabled = 0;
    ActorSetState(MR_TICK_TOCK_STATE_DEFEAT);
    TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    return 1;
}

u8 MrTickTockCheckJumpLow(void)
{
    struct Task *t;

    t = gCurTask;
    t->mrTickTockCheckPhase = (t->mrTickTockCheckPhase + 1) & 19;
    if (t->mrTickTockCheckPhase == 3)
    {
        ActorSetState(MR_TICK_TOCK_STATE_JUMP_LOW);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void MrTickTockChooseNextState(void)
{
    s32 v;

    gCurTask->unk1C = TaskGetNearestPlayerDy();
    v = TaskGetNearestPlayerDx();
    if (v < 0)
        v = -v;
    if (v > 128)
    {
        v = TaskGetNearestPlayerDy();
        if (v < 0)
            v = -v;
        if (v > 64)
            ActorSetState(gUnk_087456D4[RandomRange(16)]);
        else
            ActorSetState(gUnk_087456E4[RandomRange(16)]);
    }
    else
    {
        v = TaskGetNearestPlayerDy();
        if (v < 0)
            v = -v;
        if (v > 64)
            ActorSetState(gUnk_087456F4[RandomRange(16)]);
        else
            ActorSetState(gUnk_08745704[RandomRange(16)]);
    }
    if (gCurTask->state == MR_TICK_TOCK_STATE_HOP)
        gCurTask->mrTickTockHopsLeft = gUnk_087456D0[RandomRange(2)];
    TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
}

s32 CreateMrTickTockRing(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    sp.subtype = 16;
    sp.taskType = TASK_MR_TICK_TOCK_RING;
    sp.variant = MR_TICK_TOCK_RING_VARIANT_INIT;
    sp.spawnArg = t->facing;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 0;
    return CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void CreateMrTickTockNote(u8 a)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *act;
    s32 zero;

    switch (a)
    {
    case 0:
    case 2:
        t = gCurTask;
        if (GetShapeAtPixelIgnoringOneWay(t->pixelX - ((s8)t->facing << 4), t->pixelY) != 0)
            return;
        break;
    case 1:
    case 3:
        t = gCurTask;
        if (GetShapeAtPixelIgnoringOneWay(t->pixelX + ((s8)t->facing << 4), t->pixelY) != 0)
            return;
        break;
    }
    act = gCurTask->u8C.actor;
    zero = 0;
    gCurTask->mrTickTockNoteOffsetIndex = RandomRange(4);
    gCurTask->mrTickTockNoteOffsetX = (s8)gUnk_087456CC[(s16)gCurTask->mrTickTockNoteOffsetIndex];
    sp.subtype = 17;
    sp.taskType = TASK_MR_TICK_TOCK_NOTE;
    sp.variant = zero;
    sp.spawnArg = a;
    sp.x = gCurTask->mrTickTockNoteOffsetX;
    sp.y = 0xFFF0;
    sp.tileWord = act->savedTileWord;
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
}

u8 MrTickTockIsFarFromPlayer(void)
{
    s32 v;

    v = TaskGetNearestPlayerDx();
    if (v < 0)
        v = -v;
    if (v > 111)
        return 1;
    return 0;
}

void sub_08099fd0(void)
{
    struct Task *t;

    t = gCurTask;
    t->velX = -t->velX;
}

void sub_08099fe0(void)
{
}

void sub_08099fe4(void)
{
}

void sub_08099fe8(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->mrTickTockStatePhase)
    {
    case 0:
        t->velY = 0x8000;
        break;
    case 1:
        t->velY = 0x10000;
        break;
    }
}

void sub_0809a00c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->onGround = 0;
    u = gCurTask;
    switch (u->mrTickTockStatePhase)
    {
    case 0:
        u->velY = -32768;
        break;
    case 1:
        u->velY = -65536;
        break;
    }
}

void MrTickTockUpdateAttackBoxes(void)
{
    switch (gCurTask->state)
    {
    case MR_TICK_TOCK_STATE_DEFEAT:
    case MR_TICK_TOCK_STATE_20:
    case MR_TICK_TOCK_STATE_21:
        ActorSetAttackBox(gUnk_087459D4);
        ActorSetExtraAttackBox(gUnk_087459F0);
        break;
    default:
        ActorSetAttackBox(gUnk_08745964);
        ActorSetExtraAttackBox(gUnk_08745980);
        break;
    }
}

void sub_0809a080(u8 a)
{
    if (a == 1)
        RequestScreenShake(2);
    else
        RequestScreenShake(4);
    PlaySfx(0x1F7);
}

void Task_MrTickTock(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    ActorInitBossGfx(0);
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->layer = 11;
    zero = 0;
    u = gCurTask;
    u->frameTable = gMrTickTockFrames;
    gUnk_02007D00[0]++;
    u->mrTickTockFlashEnabled = 1;
    u->actorDustTrailSlot = zero;
    ActorIntroPoseUntilMidBossFight(gUnk_08745744);
    MidBossResetHealth();
    CallTableEntry(gCurTask->variant, 1, gMrTickTockVariants);
}

void MrTickTockInit(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateCallback = (u32)MrTickTockUpdate;
    if (IsMidBossDroppingIn() != 0)
    {
        u = gCurTask;
        u->mrTickTockCollideTerrain = 0;
        u->onGround = 0;
        ActorSetState(MR_TICK_TOCK_STATE_DROP_IN);
    }
    else
    {
        v = gCurTask;
        v->mrTickTockCollideTerrain = 1;
        sub_08066580();
        ActorSetState(MR_TICK_TOCK_STATE_DROP_IN);
        ActorSetState(MR_TICK_TOCK_STATE_WAIT_LONG);
    }
    CallTableEntry(gCurTask->state, 24, gMrTickTockStates);
}

void MrTickTockUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->mrTickTockFlashEnabled != 0)
    {
        if (t->mrTickTockFlashTimer > 0)
        {
            t->mrTickTockFlashTimer--;
            ActorFlashPalette(&gUnk_082797C8, 16);
        }
        else
        {
            ActorClearPaletteOverride();
        }
    }
    u = gCurTask;
    if (u->mrTickTockCollideTerrain != 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 24, gMrTickTockStateUpdates);
    }
    else
    {
        CallTableEntry(u->updateState, 24, gMrTickTockStateUpdates);
    }
    MrTickTockUpdateAttackBoxes();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void MrTickTockEnterState(void)
{
    TaskFreeDustTrail();
    CallTableEntry(gCurTask->state, 24, gMrTickTockStates);
}

void MrTickTockWaitLong(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    u = gCurTask;
    u->mrTickTockStatePhase = zero;
    u->mrTickTockTimer = 120;
    TaskStop();
    v = gCurTask;
    v->mrTickTockLoopCount = zero;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(15);
        TaskSetFrame(16);
        TaskYieldTrampoline(9);
    }
    while ((s16)++gCurTask->mrTickTockLoopCount <= 4);
    TaskSetFrame(4);
    TaskSleepForever();
}

void MrTickTockWaitLongUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockPickMove(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 1;
    u = gCurTask;
    u->mrTickTockStatePhase = zero;
    TaskStop();
    TaskSleepForever();
}

void MrTickTockPickMoveUpdate(void)
{
    struct Task *t;

    if (gCurTask->mrTickTockStatePhase != 0)
    {
        MrTickTockChooseNextState();
    }
    else
    {
        MrTickTockCheckJumpLow();
        t = gCurTask;
        t->mrTickTockStatePhase = 1;
    }
}

void MrTickTockHop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    u = gCurTask;
    u->mrTickTockStatePhase = zero;
    u->onGround = zero;
    TaskSetFrame(6);
    TaskStop();
    v = gCurTask;
    v->velY = -327680;
    v->accelY = 0x5000;
    v->speedLimitY = 0x70000;
    TaskSleepForever();
}

void MrTickTockHopUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->mrTickTockStatePhase != 0)
    {
        if (--t->mrTickTockHopsLeft <= 0)
            ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
        else
            ActorSetState(MR_TICK_TOCK_STATE_HOP);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockWalkBack(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 3;
    u = gCurTask;
    switch (u->actorSpawnArg)
    {
    case 0:
        u->mrTickTockTimer = 64;
        TaskSetMotionXFacing(-65536, 0x5A5A5A5A);
        while (1)
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(7);
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            gCurTask->frame--;
            TaskYieldTrampoline(7);
            gCurTask->frame--;
            TaskYieldTrampoline(7);
        }
    case 1:
        v = gCurTask;
        v->mrTickTockTimer = 64;
        TaskSetMotionXFacing(-98304, 0x5A5A5A5A);
        while (1)
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(5);
        }
    }
}

void MrTickTockWalkBackUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrTickTockTimer <= 0)
    {
        ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockJumpForward(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 4;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->mrTickTockTimer = 16;
    u->onGround = zero;
    v = gCurTask;
    v->mrTickTockStatePhase = zero;
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    w = gCurTask;
    w->velY = -327680;
    w->accelY = 0x3700;
    w->speedLimitY = 0x30000;
    switch (w->actorSpawnArg)
    {
    case 0:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
        break;
    }
    TaskSleepForever();
}

void MrTickTockJumpForwardUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->mrTickTockStatePhase != 0)
    {
        if (--t->mrTickTockTimer < 0)
        {
            ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockDashWindUp(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 5;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    u = gCurTask;
    switch (u->actorSpawnArg)
    {
    case 0:
        u->mrTickTockTimer = 20;
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        break;
    case 1:
        u->mrTickTockTimer = 22;
        TaskSetFrame(11);
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        break;
    }
    TaskSleepForever();
}

void MrTickTockDashWindUpUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        ActorSetState(MR_TICK_TOCK_STATE_DASH_START);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockDashStart(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 6;
    u = gCurTask;
    u->mrTickTockStatePhase = zero;
    gCurTask->actorDustTrailSlot = CreateDustTrail(1, 10, -12, 16);
    TaskStop();
    switch (gCurTask->actorSpawnArg)
    {
    case 0:
        TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->mrTickTockStatePhase = 1;
        TaskYieldTrampoline(4);
        break;
    case 1:
        TaskSetMotionXFacing(0x1C000, 0x5A5A5A5A);
        TaskSetFrame(11);
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->mrTickTockStatePhase = 1;
        TaskYieldTrampoline(2);
        break;
    }
    TaskSetFrame(11);
    TaskSleepForever();
}

void MrTickTockDashStartUpdate(void)
{
    s32 v;

    if (gCurTask->mrTickTockStatePhase != 0)
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v > 32)
        {
            ActorSetState(MR_TICK_TOCK_STATE_DASH_LOOP);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(MR_TICK_TOCK_STATE_JUMP_HIGH);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockJumpHigh(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 7;
    u = gCurTask;
    u->onGround = zero;
    TaskStop();
    v = gCurTask;
    v->velY = -327680;
    v->accelY = 0x5000;
    v->speedLimitY = 0x30000;
    TaskSetFrame(6);
    TaskSleepForever();
}

void MrTickTockJumpHighUpdate(void)
{
}

void MrTickTockRingFromJump(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 8;
    u = gCurTask;
    u->mrTickTockTimer = 120;
    gCurTask->mrTickTockRingSlot = CreateMrTickTockRing();
    while (1)
    {
        TaskSetFrameFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrameNoFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
    }
}

void MrTickTockRingFromJumpUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        if (MrTickTockCheckJumpLow() == 0)
        {
            ActorSetState(MR_TICK_TOCK_STATE_WAIT_SHORT);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockJumpLow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 9;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->onGround = zero;
    TaskStop();
    v = gCurTask;
    v->velY = -163840;
    v->accelY = 0x3000;
    v->speedLimitY = 0x30000;
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSleepForever();
}

void MrTickTockJumpLowUpdate(void)
{
}

void MrTickTockJumpBack(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 10;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->onGround = zero;
    TaskStop();
    v = gCurTask;
    v->velY = -163840;
    v->accelY = 0x3000;
    v->speedLimitY = 0x30000;
    TaskSetMotionXFacing(-81920, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskSleepForever();
}

void MrTickTockJumpBackUpdate(void)
{
}

void MrTickTockShootNotes(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 11;
    u = gCurTask;
    u->mrTickTockTimer = 116;
    TaskStop();
    gCurTask->mrTickTockSfxPlayer = PlaySfx(0x219);
    while (1)
    {
        TaskSetFrameFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrameNoFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
    }
}

void MrTickTockShootNotesUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    switch (--t->mrTickTockTimer)
    {
    case 100:
        PlaySfx(0x1FB);
        CreateMrTickTockNote(0);
        break;
    case 76:
        PlaySfx(0x1FB);
        CreateMrTickTockNote(1);
        break;
    case 52:
        PlaySfx(0x1FB);
        CreateMrTickTockNote(2);
        break;
    case 28:
        PlaySfx(0x1FB);
        CreateMrTickTockNote(3);
        break;
    case 0:
        StopSfxOnPlayer(t->mrTickTockSfxPlayer, 0x219);
        if (MrTickTockCheckJumpLow() == 0)
        {
            ActorSetState(MR_TICK_TOCK_STATE_WAIT_SHORT);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
        break;
    }
}

void MrTickTockDashLoop(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 12;
    gCurTask->mrTickTockTimer = (s8)gUnk_087456D2[RandomRange(2)];
    PlaySfx(502);
    u = gCurTask;
    switch (u->actorSpawnArg)
    {
    case 0:
        TaskYieldTrampoline(4);
        while (1)
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame--;
            TaskYieldTrampoline(6);
            gCurTask->frame--;
            TaskYieldTrampoline(6);
        }
    case 1:
        TaskYieldTrampoline(2);
        while (1)
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(6);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
        }
    }
}

void MrTickTockDashLoopUpdate(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    if (--t->mrTickTockTimer == 0)
    {
        ActorSetState(MR_TICK_TOCK_STATE_JUMP_LOW);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
    if (gCurTask->mrTickTockTimer < 0)
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            ActorSetState(MR_TICK_TOCK_STATE_JUMP_HIGH);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
    else
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            ActorSetState(MR_TICK_TOCK_STATE_RING_FROM_DASH);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockRingFromDash(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 13;
    u = gCurTask;
    u->mrTickTockTimer = 120;
    u->mrTickTockPrevVelX = u->velX;
    u->mrTickTockStatePhase = zero;
    switch (u->actorSpawnArg)
    {
    case 0:
        TaskSetMotionXFacing(0x5A5A5A5A, -2048);
        break;
    case 1:
        TaskSetMotionXFacing(0x5A5A5A5A, -1536);
        break;
    }
    gCurTask->mrTickTockLoopCount = 0;
    do
    {
        TaskSetFrameFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrameNoFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
    }
    while ((s16)++gCurTask->mrTickTockLoopCount <= 5);
    gCurTask->mrTickTockStatePhase = 1;
    CreateMrTickTockRing();
    while (1)
    {
        TaskSetFrameFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrameNoFlip(8);
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
    }
}

void MrTickTockRingFromDashUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    t = gCurTask;
    a = (s16)t->mrTickTockPrevVelX;
    if (a < 0)
        a = -a;
    b = t->velX;
    if (b < 0)
        b = -b;
    if (a > b)
        TaskStop();
    u = gCurTask;
    if (u->mrTickTockStatePhase != 0)
    {
        if (--u->mrTickTockTimer <= 0)
        {
            MrTickTockCheckJumpLow();
            ActorSetState(MR_TICK_TOCK_STATE_WAIT_SHORT);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
    v = gCurTask;
    v->mrTickTockPrevVelX = v->velX;
}

void MrTickTockWaitShort(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 14;
    TaskFaceNearestPlayer();
    u = gCurTask;
    switch (u->actorSpawnArg)
    {
    case 0:
        u->mrTickTockTimer = 64;
        TaskSetFrame(4);
        TaskYieldTrampoline(14);
        TaskSetFrame(16);
        TaskYieldTrampoline(8);
        TaskSetFrame(4);
        TaskYieldTrampoline(14);
        TaskSetFrame(16);
        TaskYieldTrampoline(8);
        TaskSetFrame(4);
        TaskYieldTrampoline(14);
        TaskSetFrame(16);
        TaskYieldTrampoline(8);
        break;
    case 1:
        u->mrTickTockTimer = 36;
        TaskSetFrame(4);
        TaskYieldTrampoline(13);
        TaskSetFrame(16);
        TaskYieldTrampoline(8);
        TaskSetFrame(4);
        TaskYieldTrampoline(13);
        TaskSetFrame(16);
        TaskYieldTrampoline(8);
        break;
    }
    TaskSleepForever();
}

void MrTickTockWaitShortUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        if (MrTickTockIsFarFromPlayer() != 0)
        {
            u = gCurTask;
            if (GetShapeAtPixelIgnoringOneWay(u->pixelX - ((s8)u->facing << 4), u->pixelY) != 0)
            {
                ActorSetState(MR_TICK_TOCK_STATE_18);
                TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
            }
            else
            {
                ActorSetState(MR_TICK_TOCK_STATE_17);
                TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
            }
        }
        else
        {
            ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockBounceOffWall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 15;
    u = gCurTask;
    u->onGround = zero;
    v = gCurTask;
    v->velX = -65536;
    v->velY = -196608;
    v->accelY = 0x2500;
    v->speedLimitY = 0x30000;
    RequestScreenShake(2);
    TaskSetFrame(7);
    TaskSleepForever();
}

void MrTickTockBounceOffWallUpdate(void)
{
}

void MrTickTockState16(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 16;
    u = gCurTask;
    u->mrTickTockTimer = zero;
    u->mrTickTockStatePhase = zero;
    CreateStarFlash(0, 0, 24);
    TaskStop();
    while (1)
    {
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        sub_08099fe8();
        TaskYieldTrampoline(2);
        sub_0809a00c();
        TaskYieldTrampoline(2);
        sub_08099fe8();
    }
}

void MrTickTockState16Update(void)
{
    struct Task *t;

    t = gCurTask;
    switch (++t->mrTickTockTimer)
    {
    case 32:
        t->mrTickTockStatePhase = 1;
        break;
    case 62:
        t->velY = 0x10000;
        ActorSetTerrainBox(gUnk_08745A1C);
        ActorSetState(MR_TICK_TOCK_STATE_WAIT_SHORT);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        break;
    }
}

void MrTickTockState17(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 17;
    u = gCurTask;
    u->mrTickTockTimer = 200;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(-49152, 0x5A5A5A5A);
    TaskSetFrame(10);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    while (1)
    {
        sub_08099fd0();
        gCurTask->mrTickTockLoopCount = 0;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
        }
        while ((s16)++gCurTask->mrTickTockLoopCount <= 1);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        sub_08099fd0();
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->mrTickTockLoopCount = 0;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
        }
        while ((s16)++gCurTask->mrTickTockLoopCount <= 1);
    }
}

void MrTickTockState17Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        ActorSetState(gUnk_08745714[RandomRange(16)]);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
    if ((gCurTask->mrTickTockTimer & 1) != 0)
    {
        if (MrTickTockIsFarFromPlayer() == 0)
        {
            ActorSetState(MR_TICK_TOCK_STATE_PICK_MOVE);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockState18(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 17;
    TaskSetFrame(11);
    TaskYieldTrampoline(8);
    TaskSetFrame(12);
    TaskYieldTrampoline(8);
    TaskSetFrame(11);
    TaskYieldTrampoline(8);
    TaskSetFrame(10);
    TaskYieldTrampoline(8);
    while (1)
    {
        sub_08099fd0();
        gCurTask->mrTickTockLoopCount = 0;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            TaskSetFrame(12);
            TaskYieldTrampoline(8);
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            TaskSetFrame(10);
            TaskYieldTrampoline(8);
        }
        while ((s16)++gCurTask->mrTickTockLoopCount <= 1);
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        TaskYieldTrampoline(8);
        sub_08099fd0();
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        TaskYieldTrampoline(8);
        gCurTask->mrTickTockLoopCount = 0;
        do
        {
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            TaskSetFrame(10);
            TaskYieldTrampoline(8);
            TaskSetFrame(11);
            TaskYieldTrampoline(8);
            TaskSetFrame(12);
            TaskYieldTrampoline(8);
        }
        while ((s16)++gCurTask->mrTickTockLoopCount <= 1);
    }
}

void sub_0809b210(void)
{
}

void MrTickTockDefeat(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 19;
    ActorSetHitReactions(gMrTickTockDefeatedHitReactions);
    ActorSetTerrainBox(gUnk_08745A24);
    u = gCurTask;
    u->onGround = zero;
    if (--gUnk_02007D00[0] <= 0)
        EndMidBossFightWithReward();
    MidBossStartDefeat(1, 7);
    TaskSetMotionXFacing(-65536, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    CreateStarFlash(0, -10, 24);
    TaskSetFrame(7);
    TaskSleepForever();
}

void MrTickTockDefeatUpdate(void)
{
    ActorFlashPalette(&gUnk_082797C8, 16);
}

void MrTickTockState20(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 20;
    u = gCurTask;
    u->mrTickTockTimer = 32;
    CreateStarFlash(1, 0, 0);
    gCurTask->actorDustTrailSlot = CreateDustTrail(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    TaskSetFrame(14);
    TaskSleepForever();
}

void MrTickTockState20Update(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_082797C8, 16);
    t = gCurTask;
    if (--t->mrTickTockTimer < 0)
    {
        ActorSetState(MR_TICK_TOCK_STATE_21);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockState21(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 21;
    u = gCurTask;
    u->mrTickTockDefeatDone = zero;
    CreateStarFlash(1, 0, 0);
    TaskStop();
    TaskSetFrame(14);
    TaskYieldTrampoline(170);
    v = gCurTask;
    v->mrTickTockCollideTerrain = zero;
    ActorShakeVertically();
    w = gCurTask;
    w->mrTickTockDefeatDone = 1;
    TaskSleepForever();
}

void MrTickTockState21Update(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_082797C8, 16);
    if (gCurTask->mrTickTockDefeatDone != 0)
    {
        TaskStop();
        t = gCurTask;
        t->mrTickTockCollideTerrain = 1;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

void MrTickTockFall(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 22;
    u = gCurTask;
    u->accelY = 0x5000;
    u->speedLimitY = 0x30000;
    TaskSetFrame(11);
    TaskYieldTrampoline(8);
    TaskSleepForever();
}

void MrTickTockFallUpdate(void)
{
}

void MrTickTockDropIn(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 23;
    u = gCurTask;
    u->mrTickTockTimer = 24;
    u->accelY = 0x5000;
    u->speedLimitY = 0x30000;
    TaskSetFrame(6);
    TaskSleepForever();
}

void MrTickTockDropInUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->mrTickTockTimer <= 0)
        t->mrTickTockCollideTerrain = 1;
    u = gCurTask;
    u->mrTickTockTimer--;
}

u8 MrFrostyIceCubeLand(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if (t->state == MR_FROSTY_ICE_CUBE_STATE_0)
    {
        ActorSetState(MR_FROSTY_ICE_CUBE_STATE_BURST);
        TaskSetEntry(MrFrostyIceCubeEnterState, gCurTaskIdx);
        return 1;
    }
    t->onGround = 0;
    u = gCurTask;
    switch (u->mrFrostyIceCubeArc)
    {
    case 0:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        v = gCurTask;
        v->velY = -196608;
        v->accelY = 0x1E00;
        break;
    case 1:
        TaskSetMotionXFacing(0x2A000, 0x5A5A5A5A);
        v = gCurTask;
        v->velY = -98304;
        v->accelY = 0x1E00;
        break;
    }
    return 0;
}

void MrFrostyIceCubeStartFall(void)
{
}

u8 MrFrostyIceCubeHitWall(void)
{
    ActorSetState(MR_FROSTY_ICE_CUBE_STATE_BURST);
    TaskSetEntry(MrFrostyIceCubeEnterState, gCurTaskIdx);
    return 1;
}

void MrFrostyIceCubePickArc(void)
{
    s32 v;

    v = TaskGetNearestPlayerDy();
    if (v < 0)
        v = -v;
    if (v > 29)
        gCurTask->mrFrostyIceCubeArc = 0;
    else
        gCurTask->mrFrostyIceCubeArc = 1;
}

void Task_MrFrostyIceCube(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    u = gCurTask;
    u->mrFrostyIceCubeSavedTileWord = u->tileWord;
    u->frameTable = gUnk_0874CB7C;
    u->tileWord = zero;
    CallTableEntry(u->variant, 1, gMrFrostyIceCubeVariants);
}

void MrFrostyIceCubeInit(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)MrFrostyIceCubeUpdate;
    t->facing = t->actorSpawnArg;
    ActorSetState(MR_FROSTY_ICE_CUBE_STATE_0);
    u = gCurTask;
    CallTableEntry(u->state, 3, gMrFrostyIceCubeStates);
}

void MrFrostyIceCubeUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gMrFrostyIceCubeStateUpdates);
    if (gCurTask->state != MR_FROSTY_ICE_CUBE_STATE_BURST)
        ActorCheckHits();
    ActorReactToHit();
}

void MrFrostyIceCubeEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gMrFrostyIceCubeStates);
}

void MrFrostyIceCubeState0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    TaskSetFrameNoFlip(4);
    u = gCurTask;
    u->onGround = zero;
    v = gCurTask;
    v->mrFrostyIceCubeReady = zero;
    v->speedLimitY = 0x30000;
    PlaySfx(506);
    ActorAttachEffect(3, 0);
    gCurTask->velY = -262144;
    TaskYieldTrampoline(8);
    gCurTask->velY = -131072;
    TaskYieldTrampoline(8);
    gCurTask->velY = -65536;
    TaskYieldTrampoline(8);
    ActorSetAttackBox(gUnk_08745BD0);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->mrFrostyIceCubeReady = 1;
    TaskSleepForever();
}

void MrFrostyIceCubeState0Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->mrFrostyIceCubeReady != 0)
    {
        t->mrFrostyIceCubeParentState = (gTasks + t->parent)->state;
        if (t->mrFrostyIceCubeParentState == 12)
        {
            ActorSetState(MR_FROSTY_ICE_CUBE_STATE_FLIGHT);
            TaskSetEntry(MrFrostyIceCubeEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyIceCubeFlight(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->u8C.actor;
    zero = 0;
    t->updateState = MR_FROSTY_ICE_CUBE_STATE_FLIGHT;
    u = gCurTask;
    u->speedLimitY = 0x30000;
    u->onGround = zero;
    PlaySfx(0x1FB);
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    MrFrostyIceCubePickArc();
    switch (gCurTask->mrFrostyIceCubeArc)
    {
    case 0:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        v = gCurTask;
        v->velY = -196608;
        v->accelY = 0x1E00;
        break;
    case 1:
        TaskSetMotionXFacing(0x2A000, 0x5A5A5A5A);
        v = gCurTask;
        v->velY = -98304;
        v->accelY = 0x1E00;
        break;
    }
    TaskSleepForever();
}

void MrFrostyIceCubeFlightUpdate(void)
{
}

void MrFrostyIceCubeBurst(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->u8C.actor;
    zero = 0;
    t->updateState = MR_FROSTY_ICE_CUBE_STATE_BURST;
    u = gCurTask;
    u->unk28 = zero;
    u->onGround = zero;
    TaskStop();
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    TaskSetFrameNoFlip(4);
    TaskYieldTrampoline(2);
    PlayRayBurstAnim();
    ActorDestroy();
}

void MrFrostyIceCubeBurstUpdate(void)
{
}

void Task_MrTickTockRing(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gMrTickTockRingFrames;
    CallTableEntry(u->variant, 1, gMrTickTockRingVariants);
}

void MrTickTockRingInit(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)MrTickTockRingUpdate;
    t->facing = t->actorSpawnArg;
    ActorSetState(MR_TICK_TOCK_RING_STATE_0);
    u = gCurTask;
    CallTableEntry(u->state, 1, gMrTickTockRingStates);
}

void MrTickTockRingUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gMrTickTockRingStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void MrTickTockRingEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gMrTickTockRingStates);
}

void MrTickTockRingState0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    TaskSetFrame(4);
    u = gCurTask;
    u->onGround = zero;
    gCurTask->mrTickTockRingSfxPlayer = PlaySfx(0x219);
    while (1)
    {
        ActorSetAttackBox(gUnk_08745BEC);
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        TaskSetFrame(1);
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08745C08);
        TaskSetFrame(2);
        TaskYieldTrampoline(2);
        TaskSetFrame(3);
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08745C24);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        ActorSetAttackBox(gUnk_08745C40);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
    }
}

void MrTickTockRingState0Update(void)
{
    struct Task *t;

    t = gCurTask;
    t->pixelX = (gTasks + t->parent)->pixelX;
    t->mrTickTockRingParentState = (gTasks + t->parent)->state;
    if (t->mrTickTockRingParentState != 8 && t->mrTickTockRingParentState != 13)
    {
        StopSfxOnPlayer(t->mrTickTockRingSfxPlayer, 0x219);
        ActorDestroy();
    }
}

u8 MrTickTockNoteLand(void)
{
    ActorSetState(MR_TICK_TOCK_NOTE_STATE_VANISH);
    TaskSetEntry(MrTickTockNoteEnterState, gCurTaskIdx);
    return 1;
}

u8 MrTickTockNoteHitWall(void)
{
    ActorSetState(MR_TICK_TOCK_NOTE_STATE_VANISH);
    TaskSetEntry(MrTickTockNoteEnterState, gCurTaskIdx);
    return 1;
}

void Task_MrTickTockNote(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gMrTickTockNoteFrames;
    u->facing = 1;
    CallTableEntry(gCurTask->variant, 1, gMrTickTockNoteVariants);
}
