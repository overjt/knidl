/* game_code_and_rodata 0x080988F8-0x08099B20 (issue #68, module M27 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080988F8 0x08099B20 src/enemy_988f8.c --newpb
 *
 * M27's first mid-boss script, built exactly like M25's bosses (rom-map section 9).
 * Task_MrFrosty is the task entry: it installs ActorMove as the draw hook
 * (Task.moveCallback) and ActorDrawStreamedFrameNearView as the per-frame hook (Task.drawCallback), points
 * Task.frameTable at the graphics block gMrFrostyFrames, counts the enemy into
 * gUnk_02007D00[0], loads the animation script gUnk_08745624 and hands
 * Task.variant to CallTableEntry with the one-word table gMrFrostyVariants, whose only
 * entry is MrFrostyInit.
 *
 * MrFrostyInit installs MrFrostyUpdate as the per-frame body and dispatches
 * Task.state through the 19-word guard table gMrFrostyStates; MrFrostyUpdate
 * re-uploads (ActorFlashPalette) or drops (ActorClearPaletteOverride) the 16-byte graphics
 * record gUnk_08274840 while Task.unk18 is set, dispatches Task.updateState through
 * the 19-word body table gMrFrostyStateUpdates that follows it, and finishes with the
 * animation-row selector sub_08098de4 plus ActorCheckHitsWithExtraBox / ActorReactToHit.
 * MrFrostyEnterState is the re-arm hook every guard installs through
 * TaskSetEntry(fn, gCurTaskIdx).
 *
 * The rest are the states.  MrFrostyLand / MrFrostyHitWall are the two jump-table
 * dispatchers that turn Task.state into the next animation, TaskFreeDustTrail frees
 * the helper task recorded in Task.unk46 once gTaskSlotTypes[] says its type is
 * 143 and gTasks[] says this task is its parent, MrFrostyChooseNextState walks the
 * gUnk_08745618 / gUnk_0874561F rows with the decimal-digit buffer
 * gDigits[1] as the index, sub_08098c54 fires the timed
 * RandomRange-gated transitions at Task.unk30 == 120 / 60 / 45, CreateMrFrostyIceCube
 * spawns the actor 13 through CreateActorFromDescAtOffsetFacing and MrFrostyCheckNearIceCube is the "close
 * enough" probe (|TaskGetDxTo(Task.unk1C)| <= 10).  MrFrostyBounceOffWallUpdate and
 * MrFrostyState16Update are empty state handlers, and MrFrostyDropInUpdate is the timer leaf
 * the guard table word at 0x087456C8 points at.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void ActorSetTerrainBox(u32 *p);
extern void ActorSetExtraAttackBox(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorCheckHitsWithExtraBox(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern void PlaySfx(s32 id);

/* Defined below */
void sub_0809a080(s32 a);

u8 MrFrostyLand(void)
{
    switch (gCurTask->state)
    {
    case MR_FROSTY_STATE_WAIT:
    case MR_FROSTY_STATE_HOP:
        sub_0809a080(1);
        gCurTask->mrFrostyStatePhase = 1;
        break;
    case MR_FROSTY_STATE_BOUNCE_OFF_WALL:
        sub_0809a080(1);
        gCurTask->onGround = 0;
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(MR_FROSTY_STATE_5);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_5:
        gCurTask->velY = -65536;
        break;
    case MR_FROSTY_STATE_DEFEAT:
        sub_0809a080(0);
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(MR_FROSTY_STATE_14);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_16:
        sub_0809a080(1);
        ActorSetState(MR_FROSTY_STATE_WAIT);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_10:
        sub_0809a080(1);
        TaskSetFrame(19);
        break;
    case MR_FROSTY_STATE_DROP_IN:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(MR_FROSTY_STATE_WAIT);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrFrostyHitWall(void)
{
    switch (gCurTask->state)
    {
    case MR_FROSTY_STATE_WALK_BACK:
        ActorSetState(MR_FROSTY_STATE_7);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_DASH:
        ActorSetState(MR_FROSTY_STATE_BOUNCE_OFF_WALL);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_5:
        gCurTask->onGround = 1;
        TaskStopY();
        ActorSetState(MR_FROSTY_STATE_6);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case MR_FROSTY_STATE_BOUNCE_OFF_WALL:
    case MR_FROSTY_STATE_DEFEAT:
        gCurTask->velX = 0;
        return 0;
    case MR_FROSTY_STATE_14:
        ActorSetState(MR_FROSTY_STATE_15);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrFrostyReactToDefeat(void)
{
    ActorSetHitReactions(gMrFrostyDefeatedHitReactions);
    gCurTask->mrFrostyFlashEnabled = 0;
    ActorSetState(MR_FROSTY_STATE_DEFEAT);
    TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    return 1;
}

u8 MrFrostyReactToDamage(void)
{
    gCurTask->mrFrostyFlashTimer = 32;
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    RequestScreenShake(4);
    return 0;
}

void TaskFreeDustTrail(void)
{
    if ((s16)gTaskSlotTypes[gCurTask->actorDustTrailSlot] != -1
        && gTaskSlotTypes[gCurTask->actorDustTrailSlot] == TASK_DUST_TRAIL
        && gTasks[gCurTask->actorDustTrailSlot].parent == gCurTaskIdx)
    {
        TaskFree(gCurTask->actorDustTrailSlot);
        gCurTask->actorDustTrailSlot = 0;
    }
}

void MrFrostyChooseNextState(void)
{
    struct Task *t;
    struct Task *u;

    IntToDigits((s16)RandomRange(70));
    switch (gUnk_08745618[(s8)gDigits[1]])
    {
    case 0:
        gCurTask->mrFrostyIceCubeTurnsLeft = 2;
        ActorSetState(MR_FROSTY_STATE_WALK_BACK);
        break;
    case 1:
        IntToDigits((s16)RandomRange(20));
        t = gCurTask;
        t->mrFrostyHopsLeft = gUnk_0874561F[(s8)gDigits[1]];
        t->mrFrostyIceCubeTurnsLeft = 2;
        ActorSetState(MR_FROSTY_STATE_HOP);
        break;
    case 2:
        u = gCurTask;
        if (--u->mrFrostyIceCubeTurnsLeft != 0)
        {
            ActorSetState(MR_FROSTY_STATE_SPIN);
            break;
        }
        switch (RandomRange(2))
        {
        case 0:
            ActorSetState(MR_FROSTY_STATE_WALK_BACK);
            break;
        case 1:
            IntToDigits((s16)RandomRange(20));
            gCurTask->mrFrostyHopsLeft = gUnk_0874561F[(s8)gDigits[1]];
            ActorSetState(MR_FROSTY_STATE_HOP);
            break;
        default:
            sub_0806ee2c();
            break;
        }
        break;
    default:
        sub_0806ee2c();
        break;
    }
    TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
}

void sub_08098c54(void)
{
    struct Task *t;

    t = gCurTask;
    switch (--t->mrFrostyTimer)
    {
    case 45:
        if (t->actorSpawnArg == 1 && RandomRange(4) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(MR_FROSTY_STATE_WAIT);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        break;
    case 60:
        if (t->actorSpawnArg == 0 && RandomRange(2) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(MR_FROSTY_STATE_WAIT);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        break;
    case 120:
        if (t->actorSpawnArg == 1 && RandomRange(4) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(MR_FROSTY_STATE_WAIT);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        break;
    }
}

void sub_08098cf4(void)
{
    IntToDigits((s16)RandomRange(30));
    switch ((s8)gDigits[1])
    {
    case 0:
        ActorSetState(MR_FROSTY_STATE_9);
        break;
    case 1:
        ActorSetState(MR_FROSTY_STATE_10);
        break;
    case 2:
        ActorSetState(MR_FROSTY_STATE_11);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
}

void CreateMrFrostyIceCube(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    sp.subtype = 13;
    sp.taskType = TASK_MR_FROSTY_ICE_CUBE;
    sp.variant = MR_FROSTY_ICE_CUBE_VARIANT_INIT;
    sp.spawnArg = t->facing;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 1;
    gCurTask->mrFrostyIceCubeSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

u8 MrFrostyCheckNearIceCube(void)
{
    s32 v;

    v = TaskGetDxTo(gCurTask->mrFrostyIceCubeSlot);
    if (v < 0)
        v = -v;
    if (v <= 10)
    {
        ActorSetState(MR_FROSTY_STATE_17);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08098de4(void)
{
    switch (gCurTask->state)
    {
    case MR_FROSTY_STATE_DEFEAT:
    case MR_FROSTY_STATE_14:
    case MR_FROSTY_STATE_15:
        ActorSetAttackBox(gUnk_087458F4);
        ActorSetExtraAttackBox(gUnk_08745910);
        break;
    case MR_FROSTY_STATE_SPIN:
        ActorSetAttackBox(gUnk_087458A0);
        ActorSetExtraAttackBox(gUnk_087458BC);
        break;
    case MR_FROSTY_STATE_HOP:
        ActorSetAttackBox(gUnk_0874592C);
        ActorSetExtraAttackBox(gUnk_08745948);
    default:
        ActorSetAttackBox(gUnk_08745868);
        ActorSetExtraAttackBox(gUnk_08745884);
        break;
    }
}

void Task_MrFrosty(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    ActorInitBossGfx(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->layer = 11;
    zero = 0;
    gCurTask->frameTable = gMrFrostyFrames;
    gUnk_02007D00[0]++;
    ActorIntroPoseUntilMidBossFight(gUnk_08745624);
    u = gCurTask;
    u->mrFrostyFlashEnabled = 1;
    u->actorDustTrailSlot = zero;
    MidBossResetHealth();
    CallTableEntry(gCurTask->variant, 1, gMrFrostyVariants);
}

void MrFrostyInit(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateCallback = (u32)MrFrostyUpdate;
    t->mrFrostyTimer = 90;
    t->mrFrostyIceCubeTurnsLeft = 2;
    if (IsMidBossDroppingIn() != 0)
    {
        u = gCurTask;
        u->mrFrostyCollideTerrain = 0;
        u->onGround = 0;
        ActorSetState(MR_FROSTY_STATE_DROP_IN);
    }
    else
    {
        v = gCurTask;
        v->mrFrostyCollideTerrain = 1;
        sub_08066580();
        ActorSetState(MR_FROSTY_STATE_WAIT);
    }
    CallTableEntry(gCurTask->state, 19, gMrFrostyStates);
}

void MrFrostyUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->mrFrostyFlashEnabled != 0)
    {
        if (t->mrFrostyFlashTimer > 0)
        {
            t->mrFrostyFlashTimer--;
            ActorFlashPalette(&gUnk_08274840, 16);
        }
        else
        {
            ActorClearPaletteOverride();
        }
    }
    u = gCurTask;
    if (u->mrFrostyCollideTerrain != 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 19, gMrFrostyStateUpdates);
    }
    else
    {
        CallTableEntry(u->updateState, 19, gMrFrostyStateUpdates);
    }
    sub_08098de4();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void MrFrostyEnterState(void)
{
    TaskFreeDustTrail();
    CallTableEntry(gCurTask->state, 19, gMrFrostyStates);
}

void MrFrostyWait(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->mrFrostyStatePhase = 1;
    t->updateState = MR_FROSTY_STATE_WAIT;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(25);
        TaskYieldTrampoline(4);
        TaskSetFrame(4);
        TaskYieldTrampoline(16);
    }
}

void MrFrostyWaitUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
        MrFrostyChooseNextState();
}

void MrFrostyHop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_HOP;
    u = gCurTask;
    u->onGround = zero;
    v = gCurTask;
    v->mrFrostyStatePhase = zero;
    v->velY = -327680;
    v->accelY = 0x5000;
    v->speedLimitY = 0x70000;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
    }
}

void MrFrostyHopUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->mrFrostyStatePhase != 0)
    {
        if (--t->unk30 == 0)
        {
            t->unk30 = 30;
            ActorSetState(MR_FROSTY_STATE_WAIT);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(MR_FROSTY_STATE_HOP);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyWalkBack(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->mrFrostyTimer = 160;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_WALK_BACK;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->mrFrostyStatePhase = zero;
    switch (u->actorSpawnArg)
    {
    case 0:
        TaskSetMotionXFacing(-24576, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
        break;
    }
    while (1)
    {
        gCurTask->mrFrostyLoopCount = 0;
        do
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
        }
        while ((s16)++gCurTask->mrFrostyLoopCount <= 3);
        gCurTask->mrFrostyStatePhase = 1;
    }
}

void MrFrostyWalkBackUpdate(void)
{
    if (gCurTask->mrFrostyStatePhase != 0)
    {
        ActorSetState(MR_FROSTY_STATE_DASH);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyDash(void)
{
    struct Task *t;
    struct Task *v;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_DASH;
    gCurTask->actorDustTrailSlot = CreateDustTrail(1, 10, -8, 24);
    PlaySfx(502);
    v = gCurTask;
    switch (v->actorSpawnArg)
    {
    case 0:
        TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        break;
    }
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
    }
}

void MrFrostyDashUpdate(void)
{
    sub_08098c54();
}

void MrFrostyBounceOffWall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_BOUNCE_OFF_WALL;
    u = gCurTask;
    u->onGround = zero;
    TaskStop();
    v = gCurTask;
    v->accelY = 0x2500;
    v->speedLimitY = 0x30000;
    TaskSetMotionXFacing(-49152, 0x5A5A5A5A);
    w = gCurTask;
    w->velY = -196608;
    RequestScreenShake(2);
    PlaySfx(0x1F7);
    TaskSetFrame(24);
    TaskSleepForever();
}

void MrFrostyBounceOffWallUpdate(void)
{
}

void MrFrostyState5(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->mrFrostyTimer = 32;
    t->updateState = MR_FROSTY_STATE_5;
    TaskStop();
    u = gCurTask;
    u->accelY = 0x8000;
    u->speedLimitY = 0x30000;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -65536;
    RequestScreenShake(2);
    gCurTask->actorDustTrailSlot = CreateDustTrail(0, 4, 8, 24);
    CreateStarFlash(0, 0, 24);
    TaskSetFrame(24);
    TaskSleepForever();
}

void MrFrostyState5Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        TaskStopY();
        ActorSetState(MR_FROSTY_STATE_6);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyState6(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->mrFrostyTimer = 20;
    t->updateState = MR_FROSTY_STATE_6;
    TaskStop();
    TaskSetFrame(24);
    while (1)
    {
        u = gCurTask;
        u->onGround = 0;
        v = gCurTask;
        v->velY = -65536;
        TaskYieldTrampoline(2);
        w = gCurTask;
        w->velY = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void MrFrostyState6Update(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        t->onGround = 1;
        u = gCurTask;
        u->mrFrostyTimer = 30;
        ActorSetTerrainBox(gUnk_08745A0C);
        ActorSetState(MR_FROSTY_STATE_WAIT);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyState7(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_7;
    u = gCurTask;
    u->mrFrostyStatePhase = zero;
    while (1)
    {
        gCurTask->mrFrostyLoopCount = 0;
        do
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
        }
        while ((s16)++gCurTask->mrFrostyLoopCount <= 2);
        gCurTask->mrFrostyStatePhase = 1;
    }
}

void MrFrostyState7Update(void)
{
    if (gCurTask->mrFrostyStatePhase != 0)
    {
        ActorSetState(MR_FROSTY_STATE_DASH);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostySpin(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_SPIN;
    u = gCurTask;
    u->mrFrostyTimer = 44;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(1);
        TaskTurnAround();
        TaskSetFrame(21);
        TaskYieldTrampoline(1);
        TaskTurnAround();
        TaskSetFrame(22);
        TaskYieldTrampoline(4);
        TaskTurnAround();
        TaskSetFrame(21);
        TaskYieldTrampoline(1);
        TaskTurnAround();
        TaskSetFrame(23);
        TaskYieldTrampoline(1);
        TaskSetFrame(21);
        TaskYieldTrampoline(1);
        TaskTurnAround();
        TaskSetFrame(22);
        TaskYieldTrampoline(4);
        TaskTurnAround();
        TaskSetFrame(21);
        TaskYieldTrampoline(1);
    }
}

void MrFrostySpinUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
        sub_08098cf4();
}

void MrFrostyState9(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_9;
    u = gCurTask;
    u->mrFrostyTimer = 48;
    u->onGround = 0;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -131072;
    CreateMrFrostyIceCube();
    while (1)
    {
        TaskSetFrame(10);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        TaskSetFrame(11);
        gCurTask->accelY = 0x1000;
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0x1000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0;
        TaskYieldTrampoline(8);
    }
}

void MrFrostyState9Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        if (MrFrostyCheckNearIceCube() == 0)
        {
            ActorSetState(MR_FROSTY_STATE_12);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState10(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_10;
    u = gCurTask;
    u->mrFrostyTimer = 48;
    u->onGround = 0;
    PlaySfx(506);
    CreateMrFrostyIceCube();
    TaskTurnAround();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -131072;
    while (1)
    {
        TaskSetFrame(16);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        TaskSetFrame(17);
        gCurTask->accelY = 0x1000;
        TaskYieldTrampoline(8);
        TaskSetFrame(18);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0x1000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->accelY = 0;
        TaskYieldTrampoline(8);
    }
}

void MrFrostyState10Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        TaskTurnAround();
        if (MrFrostyCheckNearIceCube() == 0)
        {
            ActorSetState(MR_FROSTY_STATE_12);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState11(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_11;
    u = gCurTask;
    u->mrFrostyTimer = 48;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    PlaySfx(506);
    CreateMrFrostyIceCube();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
    }
}

void MrFrostyState11Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        if (MrFrostyCheckNearIceCube() == 0)
        {
            ActorSetState(MR_FROSTY_STATE_12);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState12(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_12;
    u = gCurTask;
    u->mrFrostyTimer = 18;
    TaskStop();
    while (1)
    {
        TaskSetMotionXFacing(0x60000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSetFrame(13);
        TaskYieldTrampoline(3);
        TaskSetFrame(14);
        TaskYieldTrampoline(1);
        TaskStop();
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(15);
        TaskYieldTrampoline(25);
        TaskStop();
    }
}

void MrFrostyState12Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        t->mrFrostyTimer = 30;
        ActorSetState(MR_FROSTY_STATE_WAIT);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyDefeat(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_DEFEAT;
    ActorSetHitReactions(gMrFrostyDefeatedHitReactions);
    u = gCurTask;
    u->onGround = zero;
    if (--gUnk_02007D00[0] <= 0)
        EndMidBossFightWithReward();
    sub_080667c0(1, 24);
    TaskSetMotionXFacing(-65536, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    CreateStarFlash(0, -10, 24);
    TaskSetFrame(24);
    TaskSleepForever();
}

void MrFrostyDefeatUpdate(void)
{
    ActorFlashPalette(&gUnk_08274840, 16);
}

void MrFrostyState14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_14;
    u = gCurTask;
    u->mrFrostyTimer = 32;
    CreateStarFlash(1, 0, 0);
    gCurTask->actorDustTrailSlot = CreateDustTrail(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    TaskSetFrame(24);
    TaskSleepForever();
}

void MrFrostyState14Update(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_08274840, 16);
    t = gCurTask;
    if (--t->mrFrostyTimer < 0)
    {
        ActorSetState(MR_FROSTY_STATE_15);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyState15(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = MR_FROSTY_STATE_15;
    u = gCurTask;
    u->mrFrostyDefeatDone = zero;
    CreateStarFlash(1, 0, 0);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(170);
    v = gCurTask;
    v->mrFrostyCollideTerrain = zero;
    ActorShakeVertically();
    w = gCurTask;
    w->mrFrostyDefeatDone = 1;
    TaskSleepForever();
}

void MrFrostyState15Update(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_08274840, 16);
    t = gCurTask;
    if (t->mrFrostyDefeatDone != 0)
    {
        t->mrFrostyCollideTerrain = 1;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

void MrFrostyState16(void)
{
    struct Task *t;

    t = gCurTask;
    t->accelY = 0x5000;
    t->updateState = MR_FROSTY_STATE_16;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
    }
}

void MrFrostyState16Update(void)
{
}

void MrFrostyState17(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_17;
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(1);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(36);
    ActorSetState(MR_FROSTY_STATE_DASH);
    TaskSleepForever();
}

void MrFrostyState17Update(void)
{
    if (gCurTask->state != MR_FROSTY_STATE_17)
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
}

void MrFrostyDropIn(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = MR_FROSTY_STATE_DROP_IN;
    u = gCurTask;
    u->mrFrostyTimer = 24;
    u->accelY = 0x5000;
    u->speedLimitY = 0x70000;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
    }
}

void MrFrostyDropInUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->mrFrostyTimer <= 0)
        t->mrFrostyCollideTerrain = 1;
    u = gCurTask;
    u->mrFrostyTimer--;
}

u8 MrTickTockStartFall(void)
{
    if (gCurTask->state == MR_TICK_TOCK_STATE_WAIT)
    {
        ActorSetState(MR_TICK_TOCK_STATE_22);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}
