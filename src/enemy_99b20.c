/* game_code_and_rodata 0x08099B20-0x0809BA44 (issue #68, module M27 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08099B20 0x0809BA44 src/enemy_99b20.c --newpb
 *
 * The tail of the first mid-boss script, M27's second one, and the three small
 * companion tasks that close the module.
 *
 * sub_08099b20 (22 cases) and sub_08099c4c (18) are the first script's
 * remaining jump-table dispatchers; sub_08099dec picks the next animation from
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
 * States 0-23 follow as <body, guard> pairs; sub_0809b438 is the timer leaf
 * the table word at 0x0874580C points at.
 *
 * Task_MrFrostyIceCube, Task_MrTickTockRing and Task_MrTickTockNote are the three companion tasks
 * (graphics gUnk_0874CB7C, gMrTickTockRingFrames, gMrTickTockNoteFrames).  They use
 * ActorDrawWorldInViewOrDestroy as the per-frame hook and Task.layer = 9; sub_0809b6ac and
 * sub_0809b964 read the parent's state out of gTasks[Task.parent], and
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
extern void sub_08063a00(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern u32 ActorCheckHits(void);
extern void sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);

/* Defined below */
void sub_0809a080(u8 a);

u8 sub_08099b20(void)
{
    switch (gCurTask->state)
    {
    case 9:
        RequestScreenShake(1);
        PlaySfx(0x1F7);
        ActorSetState(10);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        ActorSetState(11);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 4:
        TaskSetFrame(4);
        sub_0809a080(1);
        CreateLandingDust(0, 16);
        TaskStop();
    stop:
        gCurTask->unk28 = 1;
        break;
    case 2:
        TaskSetFrame(4);
        sub_0809a080(1);
        CreateLandingDust(0, 16);
        goto stop;
    case 22:
        sub_0809a080(1);
        ActorSetState(0);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 7:
        sub_0809a080(1);
        ActorSetState(8);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 15:
        sub_0809a080(1);
        ActorSetState(16);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 19:
        sub_0809a080(0);
        ActorSetState(20);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 23:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(0);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08099c4c(void)
{
    switch (gCurTask->state)
    {
    case 17:
        ActorSetState(18);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 4:
        gCurTask->velX = 0;
        return 0;
    case 5:
        ActorSetState(6);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 6:
        ActorSetState(15);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 3:
        gCurTask->velX = -gCurTask->velX;
        break;
    case 12:
        ActorSetState(15);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    case 16:
        TaskStop();
        gCurTask->unk28 = 1;
        break;
    case 19:
        ActorSetTerrainBox(gUnk_08745A24);
    case 7:
    case 13:
    case 15:
        TaskStopX();
        break;
    case 20:
        ActorSetState(21);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 MrTickTockReactToDamage(void)
{
    gCurTask->unk2C = 32;
    CreateChildTaskHere(142, 0);
    RequestScreenShake(4);
    return 0;
}

u8 MrTickTockReactToDefeat(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->state == 11)
        StopSfxOnPlayer(t->unk1C, 0x219);
    ActorSetHitReactions(gUnk_08745A98);
    u = gCurTask;
    u->unk18 = 0;
    ActorSetState(19);
    TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    return 1;
}

u8 sub_08099db0(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34 = (t->unk34 + 1) & 19;
    if (t->unk34 == 3)
    {
        ActorSetState(9);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08099dec(void)
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
    if (gCurTask->state == 2)
        gCurTask->unk30 = gUnk_087456D0[RandomRange(2)];
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
    sp.taskType = 118;
    sp.variant = 0;
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
    gCurTask->unk70 = RandomRange(4);
    gCurTask->unk6C = (s8)gUnk_087456CC[(s16)gCurTask->unk70];
    sp.subtype = 17;
    sp.taskType = 119;
    sp.variant = zero;
    sp.spawnArg = a;
    sp.x = gCurTask->unk6C;
    sp.y = 0xFFF0;
    sp.tileWord = act->savedTileWord;
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
}

u8 sub_08099fb4(void)
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
    switch (t->unk28)
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
    switch (u->unk28)
    {
    case 0:
        u->velY = -32768;
        break;
    case 1:
        u->velY = -65536;
        break;
    }
}

void sub_0809a03c(void)
{
    switch (gCurTask->state)
    {
    case 19:
    case 20:
    case 21:
        ActorSetAttackBox(gUnk_087459D4);
        sub_08063a00(gUnk_087459F0);
        break;
    default:
        ActorSetAttackBox(gUnk_08745964);
        sub_08063a00(gUnk_08745980);
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

    sub_08066088(0);
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    zero = 0;
    u = gCurTask;
    u->frameTable = gMrTickTockFrames;
    gUnk_02007D00[0]++;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_080666cc(gUnk_08745744);
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gMrTickTockVariants);
}

void MrTickTockInit(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateCallback = (u32)MrTickTockUpdate;
    if (sub_08067060() != 0)
    {
        u = gCurTask;
        u->unk20 = 0;
        u->onGround = 0;
        ActorSetState(23);
    }
    else
    {
        v = gCurTask;
        v->unk20 = 1;
        sub_08066580();
        ActorSetState(23);
        ActorSetState(0);
    }
    CallTableEntry(gCurTask->state, 24, gMrTickTockStates);
}

void MrTickTockUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk18 != 0)
    {
        if (t->unk2C > 0)
        {
            t->unk2C--;
            ActorFlashPalette(&gUnk_082797C8, 16);
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
            CallTableEntry(gCurTask->updateState, 24, gMrTickTockStateUpdates);
    }
    else
    {
        CallTableEntry(u->updateState, 24, gMrTickTockStateUpdates);
    }
    sub_0809a03c();
    sub_08068f68();
    ActorReactToHit();
}

void MrTickTockEnterState(void)
{
    sub_08098afc();
    CallTableEntry(gCurTask->state, 24, gMrTickTockStates);
}

void MrTickTockWait(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    u = gCurTask;
    u->unk28 = zero;
    u->unk30 = 120;
    TaskStop();
    v = gCurTask;
    v->unk6C = zero;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(15);
        TaskSetFrame(16);
        TaskYieldTrampoline(9);
    }
    while ((s16)++gCurTask->unk6C <= 4);
    TaskSetFrame(4);
    TaskSleepForever();
}

void MrTickTockWaitUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(1);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockState1(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 1;
    u = gCurTask;
    u->unk28 = zero;
    TaskStop();
    TaskSleepForever();
}

void sub_0809a2c0(void)
{
    struct Task *t;

    if (gCurTask->unk28 != 0)
    {
        sub_08099dec();
    }
    else
    {
        sub_08099db0();
        t = gCurTask;
        t->unk28 = 1;
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
    u->unk28 = zero;
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
    if (t->unk28 != 0)
    {
        if (--t->unk30 <= 0)
            ActorSetState(1);
        else
            ActorSetState(2);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockState3(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 3;
    u = gCurTask;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 64;
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
        v->unk30 = 64;
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

void sub_0809a434(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 <= 0)
    {
        ActorSetState(1);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockState4(void)
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
    u->unk30 = 16;
    u->onGround = zero;
    v = gCurTask;
    v->unk28 = zero;
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    w = gCurTask;
    w->velY = -327680;
    w->accelY = 0x3700;
    w->speedLimitY = 0x30000;
    switch (w->unk74)
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

void sub_0809a4f0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 != 0)
    {
        if (--t->unk30 < 0)
        {
            ActorSetState(1);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockState5(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 5;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    u = gCurTask;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 20;
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
        u->unk30 = 22;
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

void sub_0809a624(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(6);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
}

void MrTickTockDash(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 6;
    u = gCurTask;
    u->unk28 = zero;
    gCurTask->unk46 = CreateDustTrail(1, 10, -12, 16);
    TaskStop();
    switch (gCurTask->unk74)
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
        gCurTask->unk28 = 1;
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
        gCurTask->unk28 = 1;
        TaskYieldTrampoline(2);
        break;
    }
    TaskSetFrame(11);
    TaskSleepForever();
}

void MrTickTockDashUpdate(void)
{
    s32 v;

    if (gCurTask->unk28 != 0)
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v > 32)
        {
            ActorSetState(12);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(7);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockState7(void)
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

void sub_0809a7d8(void)
{
}

void MrTickTockState8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 8;
    u = gCurTask;
    u->unk30 = 120;
    gCurTask->unk6C = CreateMrTickTockRing();
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

void sub_0809a82c(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        if (sub_08099db0() == 0)
        {
            ActorSetState(14);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockState9(void)
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

void sub_0809a8b4(void)
{
}

void MrTickTockState10(void)
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

void sub_0809a918(void)
{
}

void MrTickTockState11(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 11;
    u = gCurTask;
    u->unk30 = 116;
    TaskStop();
    gCurTask->unk1C = PlaySfx(0x219);
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

void sub_0809a974(void)
{
    struct Task *t;

    t = gCurTask;
    switch (--t->unk30)
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
        StopSfxOnPlayer(t->unk1C, 0x219);
        if (sub_08099db0() == 0)
        {
            ActorSetState(14);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
        break;
    }
}

void MrTickTockState12(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 12;
    gCurTask->unk30 = (s8)gUnk_087456D2[RandomRange(2)];
    PlaySfx(502);
    u = gCurTask;
    switch (u->unk74)
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

void sub_0809aaf0(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    if (--t->unk30 == 0)
    {
        ActorSetState(9);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
    if (gCurTask->unk30 < 0)
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            ActorSetState(7);
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
            ActorSetState(13);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
}

void MrTickTockState13(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 13;
    u = gCurTask;
    u->unk30 = 120;
    u->unk70 = u->velX;
    u->unk28 = zero;
    switch (u->unk74)
    {
    case 0:
        TaskSetMotionXFacing(0x5A5A5A5A, -2048);
        break;
    case 1:
        TaskSetMotionXFacing(0x5A5A5A5A, -1536);
        break;
    }
    gCurTask->unk6C = 0;
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
    while ((s16)++gCurTask->unk6C <= 5);
    gCurTask->unk28 = 1;
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

void sub_0809ac54(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 a;
    s32 b;

    t = gCurTask;
    a = (s16)t->unk70;
    if (a < 0)
        a = -a;
    b = t->velX;
    if (b < 0)
        b = -b;
    if (a > b)
        TaskStop();
    u = gCurTask;
    if (u->unk28 != 0)
    {
        if (--u->unk30 <= 0)
        {
            sub_08099db0();
            ActorSetState(14);
            TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
        }
    }
    v = gCurTask;
    v->unk70 = v->velX;
}

void MrTickTockState14(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 14;
    TaskFaceNearestPlayer();
    u = gCurTask;
    switch (u->unk74)
    {
    case 0:
        u->unk30 = 64;
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
        u->unk30 = 36;
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

void sub_0809ad6c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        if (sub_08099fb4() != 0)
        {
            u = gCurTask;
            if (GetShapeAtPixelIgnoringOneWay(u->pixelX - ((s8)u->facing << 4), u->pixelY) != 0)
            {
                ActorSetState(18);
                TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
            }
            else
            {
                ActorSetState(17);
                TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
            }
        }
        else
        {
            ActorSetState(1);
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
    u->unk30 = zero;
    u->unk28 = zero;
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

void sub_0809aefc(void)
{
    struct Task *t;

    t = gCurTask;
    switch (++t->unk30)
    {
    case 32:
        t->unk28 = 1;
        break;
    case 62:
        t->velY = 0x10000;
        ActorSetTerrainBox(gUnk_08745A1C);
        ActorSetState(14);
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
    u->unk30 = 200;
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
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 1);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        sub_08099fd0();
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        gCurTask->frame--;
        TaskYieldTrampoline(8);
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 1);
    }
}

void sub_0809b09c(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(gUnk_08745714[RandomRange(16)]);
        TaskSetEntry(MrTickTockEnterState, gCurTaskIdx);
    }
    if ((gCurTask->unk30 & 1) != 0)
    {
        if (sub_08099fb4() == 0)
        {
            ActorSetState(1);
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
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 1);
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        TaskYieldTrampoline(8);
        sub_08099fd0();
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        TaskYieldTrampoline(8);
        gCurTask->unk6C = 0;
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
        while ((s16)++gCurTask->unk6C <= 1);
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
    ActorSetHitReactions(gUnk_08745A98);
    ActorSetTerrainBox(gUnk_08745A24);
    u = gCurTask;
    u->onGround = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
    sub_080667c0(1, 7);
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
    u->unk30 = 32;
    CreateStarFlash(1, 0, 0);
    gCurTask->unk46 = CreateDustTrail(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = -196608;
    v->accelY = 0x1A00;
    TaskSetFrame(14);
    TaskSleepForever();
}

void sub_0809b310(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_082797C8, 16);
    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(21);
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
    u->unk30 = zero;
    CreateStarFlash(1, 0, 0);
    TaskStop();
    TaskSetFrame(14);
    TaskYieldTrampoline(170);
    v = gCurTask;
    v->unk20 = zero;
    sub_0806ad18();
    w = gCurTask;
    w->unk30 = 1;
    TaskSleepForever();
}

void sub_0809b394(void)
{
    struct Task *t;

    ActorFlashPalette(&gUnk_082797C8, 16);
    if (gCurTask->unk30 != 0)
    {
        TaskStop();
        t = gCurTask;
        t->unk20 = 1;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

void MrTickTockState22(void)
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

void sub_0809b404(void)
{
}

void MrTickTockState23(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 23;
    u = gCurTask;
    u->unk30 = 24;
    u->accelY = 0x5000;
    u->speedLimitY = 0x30000;
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0809b438(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk30 <= 0)
        t->unk20 = 1;
    u = gCurTask;
    u->unk30--;
}

u8 sub_0809b454(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if (t->state == 0)
    {
        ActorSetState(2);
        TaskSetEntry(MrFrostyIceCubeEnterState, gCurTaskIdx);
        return 1;
    }
    t->onGround = 0;
    u = gCurTask;
    switch (u->unk28)
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

void sub_0809b4d8(void)
{
}

u8 sub_0809b4dc(void)
{
    ActorSetState(2);
    TaskSetEntry(MrFrostyIceCubeEnterState, gCurTaskIdx);
    return 1;
}

void sub_0809b4fc(void)
{
    s32 v;

    v = TaskGetNearestPlayerDy();
    if (v < 0)
        v = -v;
    if (v > 29)
        gCurTask->unk28 = 0;
    else
        gCurTask->unk28 = 1;
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
    u->unk70 = u->tileWord;
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
    t->facing = t->unk74;
    ActorSetState(0);
    u = gCurTask;
    CallTableEntry(u->state, 3, gMrFrostyIceCubeStates);
}

void MrFrostyIceCubeUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gMrFrostyIceCubeStateUpdates);
    if (gCurTask->state != 2)
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
    v->unk28 = zero;
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
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_0809b6ac(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 != 0)
    {
        t->unk2C = (gTasks + t->parent)->state;
        if (t->unk2C == 12)
        {
            ActorSetState(1);
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
    t->updateState = 1;
    u = gCurTask;
    u->speedLimitY = 0x30000;
    u->onGround = zero;
    PlaySfx(0x1FB);
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    sub_0809b4fc();
    switch (gCurTask->unk28)
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

void MrFrostyIceCubeState2(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->u8C.actor;
    zero = 0;
    t->updateState = 2;
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

void sub_0809b7ec(void)
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
    t->facing = t->unk74;
    ActorSetState(0);
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
    gCurTask->unk2C = PlaySfx(0x219);
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

void sub_0809b964(void)
{
    struct Task *t;

    t = gCurTask;
    t->pixelX = (gTasks + t->parent)->pixelX;
    t->unk28 = (gTasks + t->parent)->state;
    if (t->unk28 != 8 && t->unk28 != 13)
    {
        StopSfxOnPlayer(t->unk2C, 0x219);
        ActorDestroy();
    }
}

u8 sub_0809b9c0(void)
{
    ActorSetState(1);
    TaskSetEntry(MrTickTockNoteEnterState, gCurTaskIdx);
    return 1;
}

u8 sub_0809b9e0(void)
{
    ActorSetState(1);
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
