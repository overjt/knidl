/* game_code_and_rodata 0x08091F9C-0x08093F64 (issue #67, module M25 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08091F9C 0x08093F64 src/enemy_91f9c.c --newpb
 *
 * M25's third and fourth boss scripts.  The third (entry Task_Bugzzy in
 * src/enemy_91f08.c, table 0x08743ADC) starts here with BugzzyInit, which
 * installs the per-frame body BugzzyUpdate and the animation script
 * gUnk_08743AC8.  BugzzyUpdate is the busiest body in the module: besides the
 * usual Task.updateState dispatch it calls ClampTaskToRoom (the camera/room hook) on
 * entry, and when the row gUnk_08743A58[Task.unk34] is non-null it runs the
 * "hit the wall" transition - ActorCheckPlayerHitsWithBox, then Task.unk1C = Task.hitterSlot,
 * a re-seat of the actor at gUnk_030023B4 - Task.facing * 16, and a hand-off to
 * HoldPlayer / TaskSetEntry.
 *
 * States 0-12 follow as <body, guard> pairs.  BugzzyWalk is the attack
 * chooser: it walks Task.unk6C over gUnk_08743A70[Task.unk74] rounds, and per
 * round stores |TaskGetNearestPlayerDx()| in gUnk_03001F2C, classifies it into
 * gUnk_02007D00[6] (0/1/2) against the RNG, and plays one of three yield
 * sequences; BugzzyCharge is the three-phase charge, BugzzyHop the
 * multi-hit dive, BugzzyBackdrop the four-way finisher whose case 3 spawns the
 * actor 154 at gUnk_030023B4/gUnk_030023D4, and BugzzyDefeat the defeat
 * sequence.  BugzzyFlapStep is the shake helper the first states yield to and
 * BugzzyChooseBackdrop is the collision probe: ten GetCollisionTileAtOffset samples along
 * gUnk_08743AB8, mapped through the terrain-class table gCollisionTileCollides into a
 * two-bit result that picks the next Task.unk28 direction from gUnk_08743AC2.
 * BugzzySlamHeldPlayer / BugzzyJumpBack / BugzzyShakeVertically are the shared step sequences,
 * BugzzyHitWall the hit hook, BugzzyHitCeiling the four-instruction "stop moving"
 * leaf the census had missed, and BugzzyAfterimageUpdate the companion body.
 *
 * The fourth boss starts at Task_BonkersNut (table 0x087441A4, graphics
 * gBonkersNutFrames): BonkersNutInit installs BonkersNutUpdate as its body,
 * BonkersNutFlight is its one state, Task_PoppyBrosSrBomb / PoppyBrosSrBombInit are the second
 * entry pair (graphics gPoppyBrosSrBombFrames, Actor.sfxOverride = 0x20E), PoppyBrosSrBombHeld and
 * PoppyBrosSrBombFlight are the endless spawners that call CreateChildTaskAtOffsetFacing(181, -8, -8, 1)
 * every six frames, and PoppyBrosSrBombHeldUpdate / Task_PoppyBrosSrBombSpark / PoppyBrosSrBombLand are the
 * companions that copy the boss's 16.16 position (±8 rows) and expire with it.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "actor.h"
#include "enemy.h"

/* ROM tables */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern s8 gCollisionTileCollides[];
extern vu8 gTerrainResult;

/* Externals */
extern void ClampTaskToRoom(struct Task *t);
extern u32 ActorCheckPlayerHitsWithBox(s32 a);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void ActorCheckHits(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void ReleaseHeldPlayer(s32 i, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAux(u32 v);
extern void ActorSetExtraAttackBox(u32 v);
extern s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8);
extern void ActorCheckHitsWithExtraBox(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern u8 ActorHasExtraFrame(void);

/* Defined below */
void BugzzyInit(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)BugzzyUpdate;
    ActorIntroPoseUntilMidBossFight(gUnk_08743AC8);
    u = gCurTask;
    u->bugzzySavedPalette = u->u8C.actor->palette;
    ActorSetState(BUGZZY_STATE_INTRO);
    CallTableEntry(gCurTask->state, 13, gBugzzyStates);
}

void BugzzyEnterState(void)
{
    CallTableEntry(gCurTask->state, 13, gBugzzyStates);
}

void BugzzyUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 m;

    ClampTaskToRoom(gCurTask);
    t = gCurTask;
    if (t->bugzzyIgnoreTerrainTimer != 0)
    {
        t->bugzzyIgnoreTerrainTimer--;
        CallTableEntry(t->updateState, 13, gBugzzyStateUpdates);
    }
    else if (ActorHasExtraFrame() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 13, gBugzzyStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 13, gBugzzyStateUpdates);
    }
    u = gCurTask;
    if (u->bugzzyFlashing == 1)
    {
        if (u->hitTimer != 0)
            ActorFlashPalette(&gUnk_082959A8, 16);
        else
        {
            u->bugzzyFlashing = 0;
            ActorClearPaletteOverride();
        }
    }
    ActorSetAttackBox(gUnk_08743A10[gCurTask->bugzzyBoxSet]);
    ActorSetAux(gUnk_08743A28[gCurTask->bugzzyBoxSet]);
    ActorSetExtraAttackBox(gUnk_08743A40[gCurTask->bugzzyBoxSet]);
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
    if (gUnk_08743A58[gCurTask->bugzzyBoxSet] != 0)
    {
        ActorCheckPlayerHitsWithBox(gUnk_08743A58[gCurTask->bugzzyBoxSet]);
        v = gCurTask;
        if (v->hitKind == HIT_KIND_CATCH)
        {
            v->bugzzyHeldPlayerSlot = v->hitterSlot;
            TaskFaceToward(v->bugzzyHeldPlayerSlot);
            w = gCurTask;
            w->bugzzyBoxSet = 5;
            w->bugzzyRushing = 0;
            TaskGetPosSlot(w->bugzzyHeldPlayerSlot);
            x = gCurTask;
            x->pixelX = gUnk_030023B4 - x->facing * 16;
            m = x->pixelX;
            x->posX = m << 16;
            ClampTaskToRoom(x);
            y = gCurTask;
            y->u8C.actor->palette = y->bugzzySavedPalette;
            TaskSetFrame(36);
            if (gCurTask->bugzzyHeldPlayerSlot == gLocalPlayer)
                PlaySfx(0x23D);
            HoldPlayer(gCurTask->bugzzyHeldPlayerSlot, gCurTaskIdx, 3);
            ActorSetState(BUGZZY_STATE_BACKDROP);
            TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
        }
    }
}

void BugzzyIntro(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_INTRO;
    TaskSetFrame(58);
    u = gCurTask;
    u->accelY = 9472;
    u->onGround = 0;
    TaskYieldTrampoline(1);
    if (gCurTask->onGround == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gCurTask->onGround == 0);
        RequestScreenShake(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    sub_08066580();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    ActorSetState(BUGZZY_STATE_WALK);
    TaskSleepForever();
}

void BugzzyIntroUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_INTRO)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void BugzzyWalk(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 r;
    s32 zero;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_WALK;
    u = gCurTask;
    u->bugzzyLoopCount = 0;
    while ((s16)gCurTask->bugzzyLoopCount < gUnk_08743A70[gCurTask->actorSpawnArg])
    {
        gUnk_02007D00[4] = TaskFindNearestPlayer();
        TaskFaceToward(gUnk_02007D00[4]);
        gUnk_02007D00[6] = RandomRange(8);
        gUnk_03001F2C = abs(TaskGetNearestPlayerDx());
        if (gUnk_03001F2C <= 43)
        {
            if (gUnk_02007D00[6] <= 5)
                gUnk_02007D00[6] = 1;
            else
                gUnk_02007D00[6] = 0;
        }
        else if (gUnk_03001F2C <= 87)
        {
            if (gUnk_02007D00[6] <= 4)
                gUnk_02007D00[6] = 1;
            else
                gUnk_02007D00[6] = 0;
        }
        else if (gUnk_02007D00[6] <= 3)
        {
            gUnk_02007D00[6] = 0;
        }
        else if (gUnk_02007D00[6] <= 4)
        {
            gUnk_02007D00[6] = 1;
        }
        else
        {
            gUnk_02007D00[6] = 2;
        }
        switch (gUnk_02007D00[6])
        {
        case 0:
            gCurTask->velX = 0;
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            break;
        case 1:
            TaskSetMotionXFacing(-gUnk_08743A74[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(3);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(3);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            break;
        case 2:
            TaskSetMotionXFacing(gUnk_08743A74[gCurTask->actorSpawnArg], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            TaskSetFrame(8);
            break;
        }
        gCurTask->bugzzyLoopCount++;
    }
    v = gCurTask;
    zero = 0;
    v->velX = zero;
    if (++v->bugzzyWalkCount > 2)
    {
        v->bugzzyWalkCount = zero;
        ActorSetState(BUGZZY_STATE_SUMMON);
    }
    else
    {
        gUnk_02007D00[6] = gPlayerStates[gUnk_02007D00[4]].mode;
        if (gUnk_02007D00[6] == 14)
            gUnk_02007D00[5] = 2;
        else if (gUnk_02007D00[6] == 4)
            gUnk_02007D00[5] = 1;
        else
            gUnk_02007D00[5] = zero;
        if (gCurTask->actorSpawnArg != 0)
            gUnk_02007D00[5] += 3;
        r = RandomRange(8);
        gUnk_02007D00[6] = r;
        if (r < gUnk_08743A7C[gUnk_02007D00[5]])
            ActorSetState(BUGZZY_STATE_CHARGE);
        else if (r < gUnk_08743A82[gUnk_02007D00[5]])
            ActorSetState(BUGZZY_STATE_FLY_UP);
        else if (r < gUnk_08743A88[gUnk_02007D00[5]])
            ActorSetState(BUGZZY_STATE_JUMP);
        else
            ActorSetState(BUGZZY_STATE_HOP);
    }
    TaskSleepForever();
}
void BugzzyWalkUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_WALK)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzySummon(void)
{
    struct Task *t;
    struct ActorSpawn spawn;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_SUMMON;
    TaskFaceNearestPlayer();
    TaskSetFrame(29);
    TaskYieldTrampoline(12);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    spawn.subtype = 30;
    spawn.taskType = TASK_BUGZZY_LADYBUG;
    spawn.variant = BUGZZY_LADYBUG_VARIANT_INIT;
    spawn.spawnArg = gCurTask->actorSpawnArg;
    spawn.x = 0xFFFE;
    spawn.y = 0;
    spawn.tileWord = ActorGetGfxTileWordPalOffset(1);
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    spawn.subtype = 30;
    spawn.taskType = TASK_BUGZZY_LADYBUG;
    spawn.variant = 1;
    spawn.spawnArg = gCurTask->actorSpawnArg;
    spawn.x = 0xFFFE;
    spawn.y = 0;
    spawn.tileWord = ActorGetGfxTileWordPalOffset(1);
    spawn.checkTerrain = 0;
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    TaskYieldTrampoline(4);
    gCurTask->bugzzyLoopCount = 0;
    do
    {
        gCurTask->frame--;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 1);
    ActorSetState(BUGZZY_STATE_WALK);
    TaskSleepForever();
}

void BugzzySummonUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_SUMMON)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void BugzzyCharge(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *v;
    struct Task *v2;
    struct Task *v3;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 zero;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_CHARGE;
    TaskFaceNearestPlayer();
    PlaySfx(0x23F);
    gCurTask->bugzzyLoopCount = 0;
    while ((s16)gCurTask->bugzzyLoopCount < gUnk_08743A8E[gCurTask->actorSpawnArg])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u = gCurTask;
        u->bugzzySavedPalette = u->u8C.actor->palette;
        u->u8C.actor->palette = 0;
        u->frame++;
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->u8C.actor->palette = v->bugzzySavedPalette;
        gCurTask->bugzzyLoopCount++;
    }
    gCurTask->bugzzyLoopCount = 0;
    do
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u2 = gCurTask;
        u2->bugzzySavedPalette = u2->u8C.actor->palette;
        u2->u8C.actor->palette = 0;
        u2->frame++;
        TaskYieldTrampoline(2);
        v2 = gCurTask;
        v2->u8C.actor->palette = v2->bugzzySavedPalette;
    } while ((s16)++gCurTask->bugzzyLoopCount <= 0);
    gCurTask->bugzzyLoopCount = 0;
    while ((s16)gCurTask->bugzzyLoopCount < gUnk_08743A8E[gCurTask->actorSpawnArg])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u3 = gCurTask;
        u3->bugzzySavedPalette = u3->u8C.actor->palette;
        u3->u8C.actor->palette = 0;
        u3->frame++;
        TaskYieldTrampoline(2);
        v3 = gCurTask;
        v3->u8C.actor->palette = v3->bugzzySavedPalette;
        gCurTask->bugzzyLoopCount++;
    }
    w = gCurTask;
    zero = 0;
    w->velX = zero;
    w->frame++;
    TaskYieldTrampoline(12);
    PlaySfx(500);
    x = gCurTask;
    x->bugzzyBoxSet = 3;
    gTasks[CreateChildTask(TASK_BUGZZY_AFTERIMAGE, x->pixelX, x->pixelY, ActorGetGfxTileWord())].unk74 =
        gCurTask->actorSpawnArg;
    gCurTask->bugzzyRushing = 1;
    TaskSetMotionXFacing(gUnk_08743A94[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    gCurTask->bugzzyLoopCount = zero;
    do
    {
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
        TaskSetFrame(26);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 9);
    TaskSetMotionXFacing(0x5A5A5A5A, 0xFFFFC000);
    while (1)
    {
        TaskYieldTrampoline(1);
        y = gCurTask;
        if (y->facing == 1)
        {
            if (y->velX <= 0)
                break;
        }
        else if (y->velX >= 0)
        {
            break;
        }
    }
    TaskStopX();
    gCurTask->bugzzyBoxSet = 1;
    ActorSetState(BUGZZY_STATE_FALL);
    TaskSleepForever();
}
void BugzzyChargeUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_CHARGE)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyFlyUp(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_FLY_UP;
    TaskSetFrame(60);
    TaskYieldTrampoline(6);
    v = gCurTask;
    v->frame--;
    TaskYieldTrampoline(10);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->velY = gUnk_08743A9C[u->actorSpawnArg];
    u->accelY = 9472;
    if (u->velY < 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while (gCurTask->velY < 0);
    }
    TaskStopY();
    w = gCurTask;
    if (w->actorSpawnArg == 0)
    {
        gUnk_02007D00[4] = 2;
        w->bugzzyLoopCount = 0;
        do
        {
            BugzzyFlapStep();
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->bugzzyLoopCount <= 59);
    }
    if (abs(TaskGetNearestPlayerDx()) <= 31)
        ActorSetState(BUGZZY_STATE_FALL);
    else if (abs(TaskGetNearestPlayerDy()) <= 15)
        ActorSetState(BUGZZY_STATE_CHARGE);
    else
        ActorSetState(BUGZZY_STATE_FLY_FORWARD);
    TaskSleepForever();
}

void BugzzyFlyUpUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_FLY_UP)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyFlyForward(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_FLY_FORWARD;
    TaskSetMotionXFacing(gUnk_08743AA4[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    if (TaskGetYDirBitToNearestPlayer() == 1)
        gCurTask->velY = 49152;
    gUnk_02007D00[4] = 2;
    gCurTask->bugzzyLoopCount = 0;
    do
    {
        BugzzyFlapStep();
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 119);
    ActorSetState(BUGZZY_STATE_FALL);
    TaskSleepForever();
}

void BugzzyFlyForwardUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_FLY_FORWARD)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyJump(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_JUMP;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame--;
    TaskYieldTrampoline(10);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08743AAC[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFC0000;
    u->accelY = 9472;
    u->bugzzyLoopCount = 0;
    do
    {
        if ((gFrameCount & 2) != 0)
            TaskSetFrame(52);
        else
            TaskSetFrame(61);
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 13);
    ActorSetState(BUGZZY_STATE_FALL);
    TaskSleepForever();
}

void BugzzyJumpUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_JUMP)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void BugzzyHop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_HOP;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gUnk_02007D00[4] =
        gUnk_08743AB4[RandomRange(2) + gCurTask->actorSpawnArg * 2];
    gCurTask->bugzzyLoopCount = 0;
    while ((s16)gCurTask->bugzzyLoopCount < gUnk_02007D00[4])
    {
        gCurTask->onGround = 0;
        v = gCurTask;
        v->velY = 0xFFFB0000;
        v->accelY = 20480;
        v->bugzzyLoopCount = 0;
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->bugzzyLoopCount <= 13);
        while (gCurTask->onGround == 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        }
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(0x1F7);
        TaskSetFrame(59);
        TaskYieldTrampoline(5);
        gCurTask->bugzzyLoopCount++;
    }
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    p = &gTasks[TaskFindNearestPlayer()];
    if (p->onGround == 0)
        ActorSetState(BUGZZY_STATE_FLY_UP);
    else
        ActorSetState(BUGZZY_STATE_CHARGE);
    TaskSleepForever();
}
void BugzzyHopUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_HOP)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyFall(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_FALL;
    u = gCurTask;
    u->accelY = 9472;
    if (u->onGround == 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        } while (gCurTask->onGround == 0);
        RequestScreenShake(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    ActorSetState(BUGZZY_STATE_WALK);
    TaskSleepForever();
}

void BugzzyFallUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_FALL)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyBounceOffWall(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_BOUNCE_OFF_WALL;
    TaskStop();
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFD0000;
    u->accelY = 9472;
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    v = gCurTask;
    v->frame--;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    gCurTask->velX = 0;
    BugzzyShakeVertically();
    ActorSetState(BUGZZY_STATE_LAND);
    TaskSleepForever();
}

void BugzzyBounceOffWallUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_BOUNCE_OFF_WALL)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void BugzzyLand(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_LAND;
    TaskSetMotionXFacing(0xFFFFC000, 0x5A5A5A5A);
    TaskSetFrame(48);
    TaskYieldTrampoline(4);
    gCurTask->velX = 0;
    TaskSetFrame(48);
    TaskYieldTrampoline(10);
    TaskSetFrame(50);
    TaskYieldTrampoline(8);
    TaskSetFrame(43);
    TaskYieldTrampoline(16);
    u = gCurTask;
    u->bugzzyBoxSet = 1;
    ActorSetState(BUGZZY_STATE_WALK);
    TaskSleepForever();
}

void BugzzyLandUpdate(void)
{
    if (gCurTask->state != BUGZZY_STATE_LAND)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void BugzzyBackdrop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *s;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_BACKDROP;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->velX = 131072;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE0000;
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->frame++;
    u->velX = 131072;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE0000;
    TaskYieldTrampoline(2);
    TaskStop();
    v = gCurTask;
    v->accelY = 9472;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    BugzzyChooseBackdrop();
    s = gCurTask;
    switch (s->bugzzyBackdropMove)
    {
    case 0:
        TaskSetMotionXFacing(81920, 0x5A5A5A5A);
    case 1:
        gCurTask->onGround = 0;
        w = gCurTask;
        w->velY = 0xFFFC0000;
        w->accelY = 9472;
        TaskSetFrame(51);
        TaskYieldTrampoline(13);
        while (gCurTask->velY < 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        }
        x = gCurTask;
        x->velY = 262144;
        x->accelY = 0;
        TaskSetFrame(53);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        BugzzySlamHeldPlayer();
        BugzzyJumpBack();
        break;
    case 2:
        s->onGround = 0;
        TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
        y = gCurTask;
        y->velY = 0xFFFC0000;
        y->accelY = 16384;
        TaskSetFrame(45);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskSetFrame(49);
        TaskYieldTrampoline(4);
        TaskSetFrame(47);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        BugzzySlamHeldPlayer();
        ActorSetState(BUGZZY_STATE_LAND);
        break;
    case 3:
        s->bugzzyLoopCount = 0;
        do
        {
            TaskSetFrame(38);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            RequestScreenShake(2);
            BugzzyPlaySfxForHeldPlayer(504);
            TaskGetPosSlot(gCurTask->bugzzyHeldPlayerSlot);
            CreateChildTaskAt(TASK_IMPACT_STAR, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
            TaskYieldTrampoline(8);
        } while ((s16)++gCurTask->bugzzyLoopCount <= 5);
        SetHeldPlayerState(gCurTask->bugzzyHeldPlayerSlot, 4);
        BugzzyJumpBack();
        break;
    }
    TaskSleepForever();
}

void BugzzyBackdropUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != BUGZZY_STATE_BACKDROP)
    {
        t->bugzzyHeldPlayerSlot = -1;
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
    }
}

void BugzzyDefeat(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = BUGZZY_STATE_DEFEAT;
    gUnk_02007D00[4] = 0;
    if (--gUnk_02007D00[7] == 0)
        EndMidBossFightWithReward();
    MidBossStartDefeat(1, 63);
    CreateStarFlash(1, 0, 0);
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 9472, 196608);
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(15);
    gCurTask->bugzzyBoxSet = 4;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    CreateStarFlash(0, 0, 10);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    TaskStopX();
    TaskYieldTrampoline(170);
    CreateStarFlash(1, 0, 0);
    ActorShakeVertically();
    gUnk_02007D00[4] = 1;
    TaskSleepForever();
}

void BugzzyDefeatUpdate(void)
{
    ActorFlashPalette(&gUnk_082959A8, 16);
    if (gUnk_02007D00[4] == 1)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void BugzzyFlapStep(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    if (--gUnk_02007D00[4] == 0)
    {
        t = gCurTask;
        if (++t->frame > 35)
            t->frame = 34;
        TaskUpdateFlip();
        gUnk_02007D00[4] = 2;
    }
}
void BugzzyChooseBackdrop(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s16 flags;
    s32 i;
    s32 f;
    s16 m;
    s32 n;

    flags = 0;
    for (i = 9; i >= 0; i--)
    {
        t = gCurTask;
        m = GetCollisionTileAtOffset(t->pixelX, t->pixelY, gUnk_08743AB8[i] * t->facing, 0);
        if (m == -1)
            m = 1;
        if (gCollisionTileCollides[m] != 0)
        {
            if (i > 4)
                flags |= 1;
            else
                flags |= 2;
        }
    }
    n = (u16)RandomRange(8);
    f = flags;
    if (f == 0)
    {
        if ((s16)n <= 3)
        {
            u = gCurTask;
            if (u->bugzzyBackdropMove != 0)
                u->bugzzyBackdropMove = f;
            else
                u->bugzzyBackdropMove = 2;
        }
        else if ((s16)n <= 6)
        {
            u = gCurTask;
            if (u->bugzzyBackdropMove != 2)
                u->bugzzyBackdropMove = 2;
            else
                u->bugzzyBackdropMove = 3;
        }
        else
        {
            u = gCurTask;
            if (u->bugzzyBackdropMove == 3)
                u->bugzzyBackdropMove = 1;
            else
                u->bugzzyBackdropMove = 3;
        }
    }
    else if (f == 1)
    {
        u = gCurTask;
        u->bugzzyBackdropMove = 2;
    }
    else if (f == 3)
    {
        u = gCurTask;
        u->bugzzyBackdropMove = 1;
    }
    else if ((s16)n <= 5)
    {
        w = gCurTask;
        if (w->bugzzyBackdropMove != gUnk_08743AC2[f - 1])
            w->bugzzyBackdropMove = gUnk_08743AC2[f - 1];
        else
            w->bugzzyBackdropMove = 3;
    }
    else
    {
        w = gCurTask;
        if (w->bugzzyBackdropMove != 3)
            w->bugzzyBackdropMove = 3;
        else
            w->bugzzyBackdropMove = gUnk_08743AC2[f - 1];
    }
}
void BugzzySlamHeldPlayer(void)
{
    SetHeldPlayerState(gCurTask->bugzzyHeldPlayerSlot, 4);
    RequestScreenShake(4);
    TaskGetPosSlot(gCurTask->bugzzyHeldPlayerSlot);
    CreateChildTaskAt(TASK_IMPACT_STAR, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
    BugzzyPlaySfxForHeldPlayer(504);
    BugzzyShakeVertically();
    TaskStop();
}

void BugzzyJumpBack(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
    t = gCurTask;
    t->velY = 0xFFFE0000;
    t->accelY = 12032;
    t->bugzzyLoopCount = 0;
    do
    {
        TaskSetFrame(56);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 2);
    gCurTask->bugzzyLoopCount = 0;
    do
    {
        TaskSetFrame(58);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 1);
    TaskSetFrame(59);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->bugzzyBoxSet = 1;
    ActorSetState(BUGZZY_STATE_WALK);
}

void BugzzyShakeVertically(void)
{
    gCurTask->bugzzyLoopCount = 0;
    do
    {
        gCurTask->onGround = 0;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 65536;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->bugzzyLoopCount <= 6);
}

s32 BugzzyHitWall(void)
{
    struct Task *t;
    struct Task *u;
    s32 r;
    s8 k;

    r = 0;
    t = gCurTask;
    switch (t->state)
    {
    case BUGZZY_STATE_CHARGE:
        k = t->facing;
        if ((k == 1 && (k & gTerrainResult) != 0)
         || (k == -1 && (gTerrainResult & 2) != 0))
        {
            u = gCurTask;
            u->bugzzyRushing = 0;
            u->bugzzyBoxSet = 2;
            ActorSetState(BUGZZY_STATE_BOUNCE_OFF_WALL);
            TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
            r = 1;
        }
        break;
    case BUGZZY_STATE_JUMP:
    case BUGZZY_STATE_FALL:
        TaskTurnAroundAndReverseX();
        break;
    }
    return r;
}

void BugzzyHitCeiling(void)
{
    gCurTask->velY = 0;
}

s32 BugzzyReactToDamage(void)
{
    gCurTask->bugzzyFlashing = 1;
    CreateStarFlash(1, 0, 0);
    RequestScreenShake(2);
    return 0;
}

s32 BugzzyReactToDefeat(void)
{
    struct Task *t;
    s32 n;

    t = gCurTask;
    t->bugzzyBoxSet = 0;
    t->bugzzyRushing = 0;
    n = t->bugzzyHeldPlayerSlot;
    if (n != -1)
    {
        ReleaseHeldPlayer(n, -t->facing);
        gCurTask->bugzzyHeldPlayerSlot = -1;
    }
    ActorSetHitReactions(gBugzzyDefeatedHitReactions);
    ActorSetState(BUGZZY_STATE_DEFEAT);
    TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
    return 1;
}

void Task_BugzzyAfterimage(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_087536FC;
    u->updateCallback = (u32)BugzzyAfterimageUpdate;
    TaskFaceLikeParent();
    v = gCurTask;
    v->bugzzyAfterimageMoveTimer = 4;
    v->bugzzyAfterimageLifeTimer = gUnk_08743B48[v->bugzzyAfterimageSpawnArg];
    while (--gCurTask->bugzzyAfterimageLifeTimer >= 0)
    {
        w = gCurTask;
        if ((w->bugzzyAfterimageLifeTimer & 1) != 0)
        {
            w->frame = 0xFFFF;
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(1);
        }
    }
    TaskExitTrampoline();
}

void BugzzyAfterimageUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->parent] != -1
     && (u = &gTasks[i])->u76.subtype == 5 && u->bugzzyRushing != 0)
    {
        if (--t->bugzzyAfterimageMoveTimer == 0)
        {
            t->posX = u->posX;
            t->posY = u->posY;
            t->pixelX = u->pixelX;
            t->pixelY = u->pixelY;
            t->bugzzyAfterimageMoveTimer = 4;
        }
    }
    else
    {
        t->bugzzyAfterimageLifeTimer = 0;
    }
}

void BugzzyPlaySfxForHeldPlayer(s32 a)
{
    if (gCurTask->bugzzyHeldPlayerSlot == gLocalPlayer)
        PlaySfx(a);
}

void Task_BonkersNut(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gBonkersNutFrames;
    CallTableEntry(u->variant, 1, gBonkersNutVariants);
}

void BonkersNutInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)BonkersNutUpdate;
    TaskFaceLikeParent();
    ActorSetState(BONKERS_NUT_STATE_FLIGHT);
    CallTableEntry(gCurTask->state, 1, gBonkersNutStates);
}

void BonkersNutUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gBonkersNutStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void BonkersNutFlight(void)
{
    struct Task *t;

    gCurTask->updateState = BONKERS_NUT_STATE_FLIGHT;
    gCurTask->bonkersNutBounced = 0;
    TaskSetFrame(4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(98304, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE2000, 5376, 196608);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(49152, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFEE000, 5376, 196608);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    t = gCurTask;
    t->bonkersNutBounced++;
    TaskSleepForever();
}

void BonkersNutFlightUpdate(void)
{
    if (gCurTask->bonkersNutBounced != 0)
    {
        ActorSetHitReactions(gBonkersNutDieHitReactions);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

s32 BonkersNutHitWall(void)
{
    ActorSetHitReactions(gBonkersNutDieHitReactions);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void Task_PoppyBrosSrBomb(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 13;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrBombFrames;
    TaskFaceLikeParent();
    v = gCurTask;
    v->u8C.actor->sfxOverride = 0x20E;
    v->onGround = 0;
    CallTableEntry(gCurTask->variant, 2, gPoppyBrosSrBombVariants);
}

void PoppyBrosSrBombInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)PoppyBrosSrBombUpdate;
    ActorSetState(POPPY_BROS_SR_BOMB_STATE_HELD);
    CallTableEntry(gCurTask->state, 2, gPoppyBrosSrBombStates);
}

void PoppyBrosSrBombEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gPoppyBrosSrBombStates);
}

void PoppyBrosSrBombUpdate(void)
{
    switch (gCurTask->updateState)
    {
    case 0:
        CallTableEntry(0, 2, gPoppyBrosSrBombStateUpdates);
        break;
    case 1:
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 2, gPoppyBrosSrBombStateUpdates);
        ActorCheckHits();
        ActorReactToHit();
        break;
    }
}

void PoppyBrosSrBombHeld(void)
{
    gCurTask->updateState = POPPY_BROS_SR_BOMB_STATE_HELD;
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(6);
        CreateChildTaskAtOffsetFacing(TASK_POPPY_BROS_SR_BOMB_SPARK, -8, -8, 1);
    }
}

void PoppyBrosSrBombHeldUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *p;
    s16 *q;
    s32 i;
    s32 x;
    s32 y;

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->parent] != -1)
    {
        u = &gTasks[i];
        t->facing = u->facing;
        TaskUpdateFlip();
        if (u->poppyBrosSrHandReleased != 0)
        {
            v = gCurTask;
            p = &u->pixelX;
            x = *p;
            x <<= 16;
            v->posX = x;
            p += 1;
            v->posY = (*p + 8) << 16;
            y = x >> 16;
            v->pixelX = y;
            v->pixelY = v->posY >> 16;
            ActorSetState(POPPY_BROS_SR_BOMB_STATE_FLIGHT);
            TaskSetEntry(PoppyBrosSrBombEnterState, gCurTaskIdx);
        }
        else
        {
            v = gCurTask;
            q = &u->pixelX;
            x = *q;
            x <<= 16;
            v->posX = x;
            q += 1;
            v->posY = (*q - 8) << 16;
            y = x >> 16;
            v->pixelX = y;
            v->pixelY = v->posY >> 16;
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void PoppyBrosSrBombFlight(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = POPPY_BROS_SR_BOMB_STATE_FLIGHT;
    u = gCurTask;
    u->layer = 9;
    v = gCurTask;
    v->poppyBrosSrBombBouncesLeft = 3;
    TaskSetMotionXFacing(gUnk_0874417C[v->variant][v->actorSpawnArg], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_0874418C[gCurTask->variant][gCurTask->actorSpawnArg],
                 8192, 458752);
    gCurTask->onGround = 0;
    while (1)
    {
        TaskYieldTrampoline(6);
        CreateChildTaskAtOffsetFacing(TASK_POPPY_BROS_SR_BOMB_SPARK, -8, -8, 1);
    }
}

void PoppyBrosSrBombFlightUpdate(void)
{
}

void Task_PoppyBrosSrBombSpark(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gPoppyBrosSrBombFrames;
    t->layer = gTasks[t->parent].layer - 1;
    TaskFaceLikeParent();
    u = gCurTask;
    u->velY = 0xFFFE0000;
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

s32 PoppyBrosSrBombHitWall(void)
{
    ActorSetHitReactions(gPoppyBrosSrBombDieHitReactions);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 PoppyBrosSrBombLand(void)
{
    struct Task *t;
    s32 r;

    r = 0;
    t = gCurTask;
    if (--t->poppyBrosSrBombBouncesLeft == 0)
    {
        ActorSetHitReactions(gPoppyBrosSrBombDieHitReactions);
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->onGround = 0;
        TaskSetMotionY(gUnk_0874419C[gCurTask->actorSpawnArg], 8192, 458752);
    }
    return r;
}
