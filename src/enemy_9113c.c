/* game_code_and_rodata 0x0809113C-0x08091F9C (issue #67, module M25 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0809113C 0x08091F9C src/enemy_9113c.c --newpb
 *
 * M25's second boss script, dispatched through the 15-entry anchor table at
 * 0x08743984; its entry (Task_PoppyBrosSr) is the last function of
 * src/enemy_9000c.c.  Same skeleton as the first: PoppyBrosSrInit installs the
 * per-frame body PoppyBrosSrUpdate and the animation script gUnk_0874397C,
 * PoppyBrosSrUpdate dispatches Task.updateState and reloads the graphics record
 * gPoppyBrosSrGfx, and states 0-6 are <body, guard> pairs.  Two states shell out
 * to sub_08091954, which runs the boss's attack loop: it repeats Task.unk1C
 * times, flips Task.unk20 between the two step helpers sub_08091a30 and
 * sub_08091a98 (they walk Task.frame up and down while yielding Task.unk24
 * frames per step), and waits on Task.onGround between passes.
 *
 * PoppyBrosSrReactToDamage / PoppyBrosSrReactToDefeat / PoppyBrosSrHitWall are the hit hooks (they return
 * 1 when they take over the task), and Task_PoppyBrosSrHand is the class-4 companion
 * the boss spawns: it builds an ActorSpawn on the stack, then flies the task
 * along three 16.16 ramps (Task.unk28 / Task.unk30) with a wait in the middle
 * for gTasks[Task.parent].unk20 to clear.  PoppyBrosSrHandUpdate is that
 * companion's per-frame body (it integrates the ramps and copies the boss's
 * position and facing), and Task_PoppyBrosSrHead installs PoppyBrosSrHeadUpdate, the animation
 * walker that mirrors the leader's animation onto the companion and steps the
 * companion's own AnimCmd script from gPoppyBrosSrHeadAnims.
 *
 * Task_Bugzzy (the last function here) is the entry of M25's third boss, whose
 * states live in src/enemy_91f9c.c: it installs ActorMove / ActorDrawStreamedFrameNearViewOrDestroy as
 * the draw and per-frame hooks, points Task.frameTable at gBugzzyFrames, counts the
 * boss into gUnk_02007D00[7], seeds the state block (Task.unk28 = -1,
 * Task.unk34 = 1, Task.unk1C = -1, Task.unk24 = Actor.palette) and dispatches
 * Task.variant through the 27-entry anchor table at 0x08743ADC.
 *
 * PoppyBrosSrHeadUpdate's end-of-script store (`w->unk28 = 0`) emits no
 * code: the timer is already 0 on that path, so post-reload cse deletes it,
 * but until reload it keeps the decremented timer live into the frame test,
 * which is the ROM's register allocation (lessons-learned 3.526).
 */#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(u32 v);
extern void ActorSetExtraAttackBox(u32 v);
extern s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8);
extern void ActorCheckHitsWithExtraBox(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern u8 ActorHasExtraFrame(void);
extern s32 Div(s32 numerator, s32 denominator);

void PoppyBrosSrInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)PoppyBrosSrUpdate;
    t->poppyBrosSrDefeatPhase = 0;
    t->poppyBrosSrHeadAnimIndex = 0;
    t->poppyBrosSrFlashing = 0;
    gCurTask->poppyBrosSrHeadSlot = CreateChildTaskHere(TASK_POPPY_BROS_SR_HEAD, 1);
    ActorIntroPoseUntilMidBossFight(gUnk_0874397C);
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 7, gPoppyBrosSrStates);
}

void PoppyBrosSrEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gPoppyBrosSrStates);
}

void PoppyBrosSrUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->poppyBrosSrIgnoreTerrainTimer != 0)
    {
        t->poppyBrosSrIgnoreTerrainTimer--;
        CallTableEntry(t->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    else if (ActorHasExtraFrame() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    u = gCurTask;
    if (u->poppyBrosSrFlashing != 0)
    {
        if (u->hitTimer != 1)
            ActorFlashHeaderPalette(&gPoppyBrosSrGfx, (u32)&gUnk_08275670, 16);
        else
        {
            u->poppyBrosSrFlashing = 0;
            ActorLoadHeaderPalette(&gPoppyBrosSrGfx);
        }
    }
    ActorSetAttackBox(gUnk_087438EC[gCurTask->frame]);
    sub_080639f0(gUnk_0874391C[gCurTask->frame]);
    ActorSetExtraAttackBox(gUnk_0874394C[gCurTask->frame]);
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void PoppyBrosSrState0(void)
{
    struct Task *t;
    u8 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    if (IsMidBossDroppingIn() == 1)
    {
        gCurTask->onGround = zero;
        TaskSetFrame(4);
        TaskSetMotionY(0, 5376, 196608);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        PlaySfx(528);
        TaskStop();
    }
    sub_08066580();
    ActorSetState(1);
    TaskSleepForever();
}

void PoppyBrosSrState0Update(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState1(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 1;
    u = gCurTask;
    u->poppyBrosSrHopPhase = zero;
    gCurTask->poppyBrosSrHopsLeft =
        gUnk_087438DC[RandomRange(4) + gCurTask->actorSpawnArg * 4];
    sub_08091954();
    ActorSetState(2);
    TaskSleepForever();
}

void PoppyBrosSrState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState2(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *p2;
    s32 zero;
    s32 n;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    gCurTask->onGround = zero;
    TaskSetMotionY(0xFFFD0000, 4096, 196608);
    if (abs(TaskGetNearestPlayerDy()) <= 63)
        gCurTask->poppyBrosSrPlayerNearY = 1;
    else
        gCurTask->poppyBrosSrPlayerNearY = 0;
    gCurTask->poppyBrosSrAimTimer = -1;
    p = &gTasks[(s16)CreateChildTaskHere(TASK_POPPY_BROS_SR_HAND, 1)];
    p->poppyBrosSrHandBombVariant = gCurTask->poppyBrosSrPlayerNearY;
    gCurTask->poppyBrosSrHeadAnimIndex = 2;
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 1;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(4);
    v = gCurTask;
    if (v->poppyBrosSrPlayerNearY == 0)
    {
        v->poppyBrosSrAimTimer = 4;
    }
    else
    {
        p2 = v;
        p2->poppyBrosSrPlayerDistX = abs(TaskGetNearestPlayerDx());
        w = gCurTask;
        n = w->poppyBrosSrPlayerDistX;
        if (n <= 47)
            w->poppyBrosSrAimTimer = 64;
        else if (n <= 79)
            w->poppyBrosSrAimTimer = 1;
        else
            w->poppyBrosSrAimTimer = 36;
    }
    while (1)
    {
        if (gCurTask->poppyBrosSrAimTimer-- == 0)
            break;
        TaskYieldTrampoline(1);
    }
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(3);
    TaskSleepForever();
}

void PoppyBrosSrState2Update(void)
{
    struct Task *t;

    if (gCurTask->poppyBrosSrAimTimer != 0)
    {
        TaskFaceNearestPlayer();
        TaskUpdateFlip();
    }
    t = gCurTask;
    if (t->velY > 0 && t->poppyBrosSrHeadAnimIndex == 2)
        t->poppyBrosSrHeadAnimIndex = 3;
    if (gCurTask->state != 2)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState3(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 3;
    u = gCurTask;
    u->poppyBrosSrHopPhase = zero;
    u->poppyBrosSrHopsLeft = 2;
    sub_08091954();
    ActorSetState(4);
    TaskSleepForever();
}

void PoppyBrosSrState3Update(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState4(void)
{
    struct Task *t;
    u8 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 4;
    TaskFaceNearestPlayer();
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(81920, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFEC000, 4096, 196608);
    gCurTask->poppyBrosSrFrameDelay = 6;
    sub_08091a30();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    PlaySfx(528);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFEC000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE8000, 4096, 196608);
    sub_08091a98();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(5);
    TaskSleepForever();
}

void PoppyBrosSrState4Update(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState5(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 5;
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(163840, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE4000, 4096, 196608);
    gCurTask->poppyBrosSrFrameDelay = 7;
    sub_08091a30();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    PlaySfx(528);
    gCurTask->onGround = 0;
    if (abs(TaskGetNearestPlayerDx()) <= 55)
    {
        TaskSetMotionXFacing(0xFFFD8000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE4000;
    }
    else
    {
        TaskStopX();
        u = gCurTask;
        u->velY = 0xFFFE0000;
        u->poppyBrosSrFrameDelay = 10;
    }
    sub_08091a98();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(1);
    TaskSleepForever();
}

void PoppyBrosSrState5Update(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrDefeat(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 6;
    if (--gUnk_02007D00[0] == 0)
        EndMidBossFightWithReward();
    sub_080667c0(0, 10);
    CreateStarFlash(1, 0, 0);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 6656, 196608);
    TaskSetFrame(10);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(2);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 16, 4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFF0000, 32768, 196608);
    TaskSetFrame(11);
    TaskYieldTrampoline(32);
    TaskStop();
    TaskYieldTrampoline(170);
    TaskStopY();
    ActorShakeVertically();
    gCurTask->poppyBrosSrDefeatPhase = 2;
    TaskSleepForever();
}

void PoppyBrosSrDefeatUpdate(void)
{
    ActorFlashHeaderPalette(&gPoppyBrosSrGfx, (u32)&gUnk_08275670, 16);
    if (gCurTask->poppyBrosSrDefeatPhase == 2)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_08091954(void)
{
    struct Task *u;
    struct Task *v;

    while (gCurTask->poppyBrosSrHopsLeft-- > 0)
    {
        TaskFaceNearestPlayer();
        if (abs(TaskGetNearestPlayerDx()) <= 39)
            gCurTask->poppyBrosSrHopPhase = 1;
        gCurTask->onGround = 0;
        TaskSetMotionXFacing(gUnk_087438E4[gCurTask->poppyBrosSrHopPhase], 0x5A5A5A5A);
        TaskSetMotionY(0xFFFEE000, 4096, 196608);
        u = gCurTask;
        u->poppyBrosSrFrameDelay = 3;
        if (u->poppyBrosSrHopPhase == 0)
            sub_08091a30();
        else
            sub_08091a98();
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        v = gCurTask;
        v->poppyBrosSrHopPhase ^= 1;
        PlaySfx(528);
    }
    TaskStop();
}

void sub_08091a30(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->poppyBrosSrHeadAnimIndex = 0;
    TaskSetFrame(4);
    TaskYieldTrampoline(gCurTask->poppyBrosSrFrameDelay);
    u = gCurTask;
    u->frame += 2;
    TaskYieldTrampoline(u->poppyBrosSrFrameDelay);
    v = gCurTask;
    v->frame += 2;
    TaskYieldTrampoline(v->poppyBrosSrFrameDelay);
    w = gCurTask;
    w->frame += 1;
    TaskYieldTrampoline(w->poppyBrosSrFrameDelay);
    x = gCurTask;
    x->frame -= 2;
    TaskYieldTrampoline(x->poppyBrosSrFrameDelay);
    y = gCurTask;
    y->frame -= 2;
    TaskYieldTrampoline(y->poppyBrosSrFrameDelay);
}

void sub_08091a98(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->poppyBrosSrHeadAnimIndex = 1;
    TaskSetFrame(5);
    TaskYieldTrampoline(gCurTask->poppyBrosSrFrameDelay);
    u = gCurTask;
    u->frame += 2;
    TaskYieldTrampoline(u->poppyBrosSrFrameDelay);
    v = gCurTask;
    v->frame += 2;
    TaskYieldTrampoline(v->poppyBrosSrFrameDelay);
    w = gCurTask;
    w->frame -= 1;
    TaskYieldTrampoline(w->poppyBrosSrFrameDelay);
    x = gCurTask;
    x->frame -= 2;
    TaskYieldTrampoline(x->poppyBrosSrFrameDelay);
    y = gCurTask;
    y->frame -= 2;
    TaskYieldTrampoline(y->poppyBrosSrFrameDelay);
}

s32 PoppyBrosSrReactToDamage(void)
{
    gCurTask->poppyBrosSrFlashing = 1;
    CreateStarFlash(1, 0, -8);
    RequestScreenShake(2);
    return 0;
}

s32 PoppyBrosSrReactToDefeat(void)
{
    TaskSetFrame(10);
    ActorSetHitReactions(gPoppyBrosSrDefeatedHitReactions);
    gCurTask->poppyBrosSrDefeatPhase = 1;
    ActorSetState(6);
    TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
    return 1;
}

s32 PoppyBrosSrHitWall(void)
{
    TaskStopX();
    return 0;
}

void Task_PoppyBrosSrHand(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *p;
    struct Task *q;
    struct ActorSpawn spawn;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->moveCallback = zero;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 14;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrHandFrames;
    u->updateCallback = (u32)PoppyBrosSrHandUpdate;
    TaskFaceLikeParent();
    v = gCurTask;
    v->poppyBrosSrHandOffsetX = 0x100000;
    v->poppyBrosSrHandOffsetVelY = zero;
    v->poppyBrosSrHandOffsetY = 0x40000;
    v->poppyBrosSrHandReleased = zero;
    spawn.subtype = 9;
    spawn.taskType = TASK_POPPY_BROS_SR_BOMB;
    spawn.variant = v->poppyBrosSrHandBombVariant;
    spawn.spawnArg = v->unk74;
    spawn.x = zero;
    spawn.y = zero;
    spawn.checkTerrain = 1;
    spawn.tileWord = ActorGetTileWordPalOffset(1);
    gCurTask->poppyBrosSrHandBombSlot = CreateActorFromDescAtOffsetFacing(&spawn, 1);
    gCurTask->poppyBrosSrHandOffsetVelX = 0x30000;
    gCurTask->poppyBrosSrHandOffsetVelY = 0xFFFA0000;
    TaskSetFrame(5);
    gCurTask->poppyBrosSrHandLoopCount = zero;
    do
    {
        x = gCurTask;
        x->poppyBrosSrHandOffsetVelX += 0xFFFF7000;
        x->poppyBrosSrHandOffsetVelY += 0x8000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->poppyBrosSrHandLoopCount <= 15);
    y = gCurTask;
    y->poppyBrosSrHandOffsetVelX = 0;
    y->poppyBrosSrHandOffsetVelY = 0;
    p = gCurTask;
    while ((q = &gTasks[p->parent])->poppyBrosSrAimTimer != 0)
    {
        TaskYieldTrampoline(1);
        p = gCurTask;
    }
    z = gCurTask;
    z->poppyBrosSrHandOffsetVelX = 0x88000;
    z->poppyBrosSrHandOffsetVelY = 0xFFFC0000;
    z->poppyBrosSrHandLoopCount = 0;
    do
    {
        a = gCurTask;
        a->poppyBrosSrHandOffsetVelX += 0xFFFF0000;
        a->poppyBrosSrHandOffsetVelY += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->poppyBrosSrHandLoopCount <= 3);
    b = gCurTask;
    b->poppyBrosSrHandReleased++;
    PlaySfx(0x20F);
    TaskSetFrame(1);
    gCurTask->poppyBrosSrHandLoopCount = 0;
    do
    {
        c = gCurTask;
        c->poppyBrosSrHandOffsetVelX += 0xFFFF0000;
        c->poppyBrosSrHandOffsetVelY += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->poppyBrosSrHandLoopCount <= 3);
    d = gCurTask;
    d->poppyBrosSrHandOffsetVelX = 0xFFFF0000;
    d->poppyBrosSrHandOffsetVelY = 0xFFFF0000;
    TaskSetFrame(3);
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void PoppyBrosSrHandUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 m;

    if ((s16)gTaskSlotTypes[gCurTask->parent] != -1)
    {
        t = gCurTask;
        u = &gTasks[t->parent];
        if (u->u76.subtype == 1 && u->poppyBrosSrDefeatPhase == 0)
        {
            t->facing = u->facing;
            TaskUpdateFlip();
            v = gCurTask;
            v->poppyBrosSrHandOffsetX += v->poppyBrosSrHandOffsetVelX;
            v->poppyBrosSrHandOffsetY += v->poppyBrosSrHandOffsetVelY;
            v->pixelX = u->pixelX + (v->poppyBrosSrHandOffsetX * u->facing >> 16);
            m = ((s16 *)v)[27];
            v->pixelY = u->pixelY + m;
        }
        else
        {
            TaskFree(gCurTaskIdx);
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_PoppyBrosSrHead(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrHeadFrames;
    u->updateCallback = (u32)PoppyBrosSrHeadUpdate;
    u->poppyBrosSrHeadShownIndex = -1;
    TaskSleepForever();
}

void PoppyBrosSrHeadUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct AnimCmd *p;
    struct AnimCmd *q;
    u32 base;
    s16 frame;

    if ((s16)gTaskSlotTypes[gCurTask->parent] != -1)
    {
        t = gCurTask;
        u = &gTasks[t->parent];
        if (u->u76.subtype == 1 && u->poppyBrosSrDefeatPhase == 0)
        {
            t->pixelX = u->pixelX;
            t->pixelY = u->pixelY;
            t->facing = u->facing;
            TaskUpdateFlip();
            v = gCurTask;
            if (v->poppyBrosSrHeadShownIndex != u->poppyBrosSrHeadAnimIndex)
            {
                v->poppyBrosSrHeadShownIndex = u->poppyBrosSrHeadAnimIndex;
                v->poppyBrosSrHeadScriptPos = 0;
                p = gPoppyBrosSrHeadAnims[v->poppyBrosSrHeadShownIndex];
                v->poppyBrosSrHeadStepTimer = p->delay;
                v->frame = p->frame;
            }
            w = gCurTask;
            if (w->poppyBrosSrHeadStepTimer != 0 && --w->poppyBrosSrHeadStepTimer == 0)
            {
                w->poppyBrosSrHeadScriptPos++;
                base = (u32)gPoppyBrosSrHeadAnims[w->poppyBrosSrHeadShownIndex];
                q = (struct AnimCmd *)(w->poppyBrosSrHeadScriptPos * 4 + base);
                frame = q->frame;
                if (frame == -1)
                {
                    /* End of the script: stop with the timer at 0.  The
                     * store is redundant on this path (the timer is already
                     * 0): cse turns it into a store of the decremented
                     * value and post-reload cse (reload_cse) deletes it, so
                     * it emits no code, but it keeps the decremented value
                     * live into the frame test, as the ROM's registers show
                     * (`subs r4, r0, #1` and the ldrsh scratches; lessons
                     * 3.156, 3.526).  Do not remove it. */
                    w->poppyBrosSrHeadStepTimer = 0;
                }
                else
                {
                    w->poppyBrosSrHeadStepTimer = q->delay;
                    w->frame = frame;
                }
            }
        }
        else
        {
            TaskFree(gCurTaskIdx);
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_Bugzzy(void)
{
    struct Task *t;
    struct Task *u;

    ActorInitBossGfx(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gBugzzyFrames;
    gUnk_02007D00[7]++;
    u->bugzzyBackdropMove = -1;
    u->bugzzyWalkCount = 0;
    u->bugzzyFlashing = 0;
    u->bugzzyBoxSet = 1;
    u->bugzzyHeldPlayerSlot = -1;
    u->bugzzySavedPalette = u->u8C.actor->palette;
    if (IsMidBossDroppingIn() == 1)
        gCurTask->bugzzyIgnoreTerrainTimer = 24;
    else
        gCurTask->bugzzyIgnoreTerrainTimer = 0;
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gBugzzyVariants);
}
