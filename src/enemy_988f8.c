/* game_code_and_rodata 0x080988F8-0x08099B20 (issue #68, module M27 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080988F8 0x08099B20 src/enemy_988f8.c --newpb
 *
 * M27's first mid-boss script, built exactly like M25's bosses (rom-map section 9).
 * Task_MrFrosty is the task entry: it installs ActorMove as the draw hook
 * (Task.moveCallback) and sub_08065438 as the per-frame hook (Task.drawCallback), points
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
 * animation-row selector sub_08098de4 plus sub_08068f68 / ActorReactToHit.
 * MrFrostyEnterState is the re-arm hook every guard installs through
 * TaskSetEntry(fn, gCurTaskIdx).
 *
 * The rest are the states.  sub_080988f8 / sub_08098a04 are the two jump-table
 * dispatchers that turn Task.state into the next animation, sub_08098afc frees
 * the helper task recorded in Task.unk46 once gTaskSlotTypes[] says its type is
 * 143 and gTasks[] says this task is its parent, MrFrostyChooseNextState walks the
 * gUnk_08745618 / gUnk_0874561F rows with the decimal-digit buffer
 * gDigits[1] as the index, sub_08098c54 fires the timed
 * RandomRange-gated transitions at Task.unk30 == 120 / 60 / 45, CreateMrFrostyIceCube
 * spawns the actor 13 through CreateActorFromDescAtOffsetFacing and sub_08098da4 is the "close
 * enough" probe (|TaskGetDxTo(Task.unk1C)| <= 10).  MrFrostyBounceOffWallUpdate and
 * sub_08099a0c are empty state handlers, and sub_08099ad0 is the timer leaf
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
extern void sub_08063a00(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern void RequestScreenShake(s32 a);
extern void sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern void PlaySfx(s32 id);

/* Defined below */
void sub_0809a080(s32 a);

u8 sub_080988f8(void)
{
    switch (gCurTask->state)
    {
    case 0:
    case 1:
        sub_0809a080(1);
        gCurTask->unk28 = 1;
        break;
    case 4:
        sub_0809a080(1);
        gCurTask->onGround = 0;
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(5);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 5:
        gCurTask->velY = -65536;
        break;
    case 13:
        sub_0809a080(0);
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(14);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 16:
        sub_0809a080(1);
        ActorSetState(0);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        TaskSetFrame(19);
        break;
    case 18:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(0);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08098a04(void)
{
    switch (gCurTask->state)
    {
    case 2:
        ActorSetState(7);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 3:
        ActorSetState(4);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 5:
        gCurTask->onGround = 1;
        TaskStopY();
        ActorSetState(6);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    case 4:
    case 13:
        gCurTask->velX = 0;
        return 0;
    case 14:
        ActorSetState(15);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrFrostyReactToDefeat(void)
{
    ActorSetHitReactions(gUnk_08745A80);
    gCurTask->unk18 = 0;
    ActorSetState(13);
    TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    return 1;
}

u8 MrFrostyReactToDamage(void)
{
    gCurTask->unk2C = 32;
    CreateChildTaskHere(142, 0);
    RequestScreenShake(4);
    return 0;
}

void sub_08098afc(void)
{
    if ((s16)gTaskSlotTypes[gCurTask->unk46] != -1
        && gTaskSlotTypes[gCurTask->unk46] == 143
        && gTasks[gCurTask->unk46].parent == gCurTaskIdx)
    {
        TaskFree(gCurTask->unk46);
        gCurTask->unk46 = 0;
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
        gCurTask->unk34 = 2;
        ActorSetState(2);
        break;
    case 1:
        IntToDigits((s16)RandomRange(20));
        t = gCurTask;
        t->unk30 = gUnk_0874561F[(s8)gDigits[1]];
        t->unk34 = 2;
        ActorSetState(1);
        break;
    case 2:
        u = gCurTask;
        if (--u->unk34 != 0)
        {
            ActorSetState(8);
            break;
        }
        switch (RandomRange(2))
        {
        case 0:
            ActorSetState(2);
            break;
        case 1:
            IntToDigits((s16)RandomRange(20));
            gCurTask->unk30 = gUnk_0874561F[(s8)gDigits[1]];
            ActorSetState(1);
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
    switch (--t->unk30)
    {
    case 45:
        if (t->unk74 == 1 && RandomRange(4) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(0);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        break;
    case 60:
        if (t->unk74 == 0 && RandomRange(2) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(0);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        break;
    case 120:
        if (t->unk74 == 1 && RandomRange(4) == 0)
        {
            gCurTask->velX = 0;
            ActorSetState(0);
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
        ActorSetState(9);
        break;
    case 1:
        ActorSetState(10);
        break;
    case 2:
        ActorSetState(11);
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
    sp.taskType = 115;
    sp.variant = 0;
    sp.spawnArg = t->facing;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 1;
    gCurTask->unk1C = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

u8 sub_08098da4(void)
{
    s32 v;

    v = TaskGetDxTo(gCurTask->unk1C);
    if (v < 0)
        v = -v;
    if (v <= 10)
    {
        ActorSetState(17);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08098de4(void)
{
    switch (gCurTask->state)
    {
    case 13:
    case 14:
    case 15:
        ActorSetAttackBox(gUnk_087458F4);
        sub_08063a00(gUnk_08745910);
        break;
    case 8:
        ActorSetAttackBox(gUnk_087458A0);
        sub_08063a00(gUnk_087458BC);
        break;
    case 1:
        ActorSetAttackBox(gUnk_0874592C);
        sub_08063a00(gUnk_08745948);
    default:
        ActorSetAttackBox(gUnk_08745868);
        sub_08063a00(gUnk_08745884);
        break;
    }
}

void Task_MrFrosty(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    sub_08066088(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    zero = 0;
    gCurTask->frameTable = gMrFrostyFrames;
    gUnk_02007D00[0]++;
    sub_080666cc(gUnk_08745624);
    u = gCurTask;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gMrFrostyVariants);
}

void MrFrostyInit(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateCallback = (u32)MrFrostyUpdate;
    t->unk30 = 90;
    t->unk34 = 2;
    if (sub_08067060() != 0)
    {
        u = gCurTask;
        u->unk20 = 0;
        u->onGround = 0;
        ActorSetState(18);
    }
    else
    {
        v = gCurTask;
        v->unk20 = 1;
        sub_08066580();
        ActorSetState(0);
    }
    CallTableEntry(gCurTask->state, 19, gMrFrostyStates);
}

void MrFrostyUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk18 != 0)
    {
        if (t->unk2C > 0)
        {
            t->unk2C--;
            ActorFlashPalette(&gUnk_08274840, 16);
        }
        else
        {
            ActorClearPaletteOverride();
        }
    }
    u = gCurTask;
    if (u->unk20 != 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 19, gMrFrostyStateUpdates);
    }
    else
    {
        CallTableEntry(u->updateState, 19, gMrFrostyStateUpdates);
    }
    sub_08098de4();
    sub_08068f68();
    ActorReactToHit();
}

void MrFrostyEnterState(void)
{
    sub_08098afc();
    CallTableEntry(gCurTask->state, 19, gMrFrostyStates);
}

void MrFrostyWait(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->unk28 = 1;
    t->updateState = 0;
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
    if (--t->unk30 < 0)
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
    t->updateState = 1;
    u = gCurTask;
    u->onGround = zero;
    v = gCurTask;
    v->unk28 = zero;
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
    if (t->unk28 != 0)
    {
        if (--t->unk30 == 0)
        {
            t->unk30 = 30;
            ActorSetState(0);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(1);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState2(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->unk30 = 160;
    zero = 0;
    t->updateState = 2;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk28 = zero;
    switch (u->unk74)
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
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 3);
        gCurTask->unk28 = 1;
    }
}

void sub_08099180(void)
{
    if (gCurTask->unk28 != 0)
    {
        ActorSetState(3);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyDash(void)
{
    struct Task *t;
    struct Task *v;

    t = gCurTask;
    t->updateState = 3;
    gCurTask->unk46 = CreateDustTrail(1, 10, -8, 24);
    PlaySfx(502);
    v = gCurTask;
    switch (v->unk74)
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
    t->updateState = 4;
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
    t->unk30 = 32;
    t->updateState = 5;
    TaskStop();
    u = gCurTask;
    u->accelY = 0x8000;
    u->speedLimitY = 0x30000;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -65536;
    RequestScreenShake(2);
    gCurTask->unk46 = CreateDustTrail(0, 4, 8, 24);
    CreateStarFlash(0, 0, 24);
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_0809931c(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        TaskStopY();
        ActorSetState(6);
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
    t->unk30 = 20;
    t->updateState = 6;
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

void sub_08099394(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        t->onGround = 1;
        u = gCurTask;
        u->unk30 = 30;
        ActorSetTerrainBox(gUnk_08745A0C);
        ActorSetState(0);
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
    t->updateState = 7;
    u = gCurTask;
    u->unk28 = zero;
    while (1)
    {
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 2);
        gCurTask->unk28 = 1;
    }
}

void sub_08099448(void)
{
    if (gCurTask->unk28 != 0)
    {
        ActorSetState(3);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
    }
}

void MrFrostyState8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 8;
    u = gCurTask;
    u->unk30 = 44;
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

void sub_08099508(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
        sub_08098cf4();
}

void MrFrostyState9(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 9;
    u = gCurTask;
    u->unk30 = 48;
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

void sub_080995b8(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        if (sub_08098da4() == 0)
        {
            ActorSetState(12);
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
    t->updateState = 10;
    u = gCurTask;
    u->unk30 = 48;
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

void sub_08099690(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        TaskTurnAround();
        if (sub_08098da4() == 0)
        {
            ActorSetState(12);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState11(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 11;
    u = gCurTask;
    u->unk30 = 48;
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

void sub_08099734(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        if (sub_08098da4() == 0)
        {
            ActorSetState(12);
            TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        }
    }
}

void MrFrostyState12(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 12;
    u = gCurTask;
    u->unk30 = 18;
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

void sub_080997e4(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        t->unk30 = 30;
        ActorSetState(0);
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
    t->updateState = 13;
    ActorSetHitReactions(gUnk_08745A80);
    u = gCurTask;
    u->onGround = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
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
    t->updateState = 14;
    u = gCurTask;
    u->unk30 = 32;
    CreateStarFlash(1, 0, 0);
    gCurTask->unk46 = CreateDustTrail(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_08099908(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_08274840, 16);
    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(15);
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
    t->updateState = 15;
    u = gCurTask;
    u->unk30 = zero;
    CreateStarFlash(1, 0, 0);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(170);
    v = gCurTask;
    v->unk20 = zero;
    sub_0806ad18();
    w = gCurTask;
    w->unk30 = 1;
    TaskSleepForever();
}

void sub_0809998c(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_08274840, 16);
    t = gCurTask;
    if (t->unk30 != 0)
    {
        t->unk20 = 1;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

void MrFrostyState16(void)
{
    struct Task *t;

    t = gCurTask;
    t->accelY = 0x5000;
    t->updateState = 16;
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

void sub_08099a0c(void)
{
}

void MrFrostyState17(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 17;
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(1);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(36);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_08099a54(void)
{
    if (gCurTask->state != 17)
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
}

void MrFrostyState18(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 18;
    u = gCurTask;
    u->unk30 = 24;
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

void sub_08099ad0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk30 <= 0)
        t->unk20 = 1;
    u = gCurTask;
    u->unk30--;
}

u8 sub_08099aec(void)
{
    if (gCurTask->state == 0)
    {
        ActorSetState(22);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}
