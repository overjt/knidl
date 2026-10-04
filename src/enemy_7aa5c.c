/* game_code_and_rodata 0x0807AA5C-0x0807D3B0 (issue #77, module M20 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807AA5C 0x0807D3B0 src/enemy_7aa5c.c --newpb
 *
 * The middle third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Twelve more scripts, among them:
 *   * Task_Scarfy's row `ScarfyInit`+`ScarfyUpdate` with its five-case
 *     `sub_0807a8fc` setup and `sub_0807aa5c`'s two-table float wave (an
 *     older reading called it a swinging platform);
 *   * row 0 of the two sword knights' shared table `gSwordAndBladeKnightVariants` (task
 *     types #36/#37: `SwordAndBladeKnightWalkInit`+`SwordAndBladeKnightWalkUpdate`), whose hook
 *     packs Task.pixelY into the low half of Task.unk24 and ORs 0x10000 in
 *     when the four-player flag `gTerrainResult[4]` is out of range;
 *   * the sword knights' remaining states and rows `SwordAndBladeKnightWalk`..
 *     `SwordAndBladeKnightStandInit`, which probe for a partner with `TaskIsInRectSlot` over a
 *     stack `struct PointPair`
 *     and reacts through the shared `SwordAndBladeKnightPickSlash` state entry;
 *   * Task_UFO's row 0 `UFOInit`+`UFOUpdate` with the
 *     `UFOIsAtTarget` box test (`struct Rect` + GetDistSq) and the
 *     `UFOSetTarget` aim helper that clamps into `0x08740B3C`/`0x08740B60`;
 *   * UFO's state check `UFOPickMoveUpdate` (gUFOStateUpdates[1]), which walks a
 *     sixteen-entry cue ring through
 *     `UFOPickNextPoint` (`15 & (rand + Task.unk24)`);
 *   * Task_Parasol's row 0 `ParasolRiseInit`+`ParasolRiseUpdate` and its aim
 *     `sub_0807cbf4` (a 512-step angle from `TaskGetAngleTo`).
 *
 * `SwordAndBladeKnightIdleEnterState`, `sub_0807bd60`, `NeedlousIdleEnterState`, `UFOIdleEnterState`,
 * `ParasolRiseEnterState`, `ParasolChaseEnterState`, `ParasolIdleEnterState` and `PengyIceBreathEnterState` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 GetShapeAtPixelIgnoringOneWay(s32 x, s32 y);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
extern s32 ActorStartAnimNoFlip(u32 *p);
extern s32 TaskGetAngleToNearestPlayer(s32 prec);
extern s32 AreAllPlayersOnGround();
extern s32 ActorReactToHit(void);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u8 ActorCollideTerrainPointPushOut(void);
extern u8 ActorStepBackFromSlope(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void AngleToVector(s16 t, s16 mag);

void sub_0807aa5c(void)
{
    s32 i;

    for (i = 0; i <= 16; i += 2)
    {
        gCurTask->velY = gUnk_08740864[i] * gCurTask->scarfyWaveSign;
        TaskYieldTrampoline(gUnk_08740864[i + 1]);
    }
    gCurTask->scarfyWaveSign = -gCurTask->scarfyWaveSign;
    while (1)
    {
        for (i = 0; i <= 32; i += 2)
        {
            gCurTask->velY = gUnk_087408AC[i] * gCurTask->scarfyWaveSign;
            TaskYieldTrampoline(gUnk_087408AC[i + 1]);
        }
        gCurTask->scarfyWaveSign = -gCurTask->scarfyWaveSign;
    }
}

void ScarfyInit(void)
{
    gCurTask->updateCallback = (u32)ScarfyUpdate;
    ScarfyPickStartState();
    CallTableEntry(gCurTask->state, 6, gScarfyStates);
}

void ScarfyUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gScarfyStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->state != SCARFY_STATE_HIDE)
    {
        ActorCheckHits();
        ActorReactToHit();
        ScarfyCheckTransform();
    }
}

void ScarfyEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gScarfyStates);
}

void ScarfyHover(void)
{
    gCurTask->updateState = SCARFY_STATE_HOVER;
    sub_0807a8fc();
    sub_0807aa5c();
}

void ScarfyHoverUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay34);
}

void ScarfyHide(void)
{
    gCurTask->updateState = SCARFY_STATE_HIDE;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void ScarfyHideUpdate(void)
{
    if (TaskIsNearestPlayerWithinX(10) != 0)
    {
        ActorSetState(SCARFY_STATE_1);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}

void ScarfyState1(void)
{
    gCurTask->updateState = SCARFY_STATE_1;
    gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08740854);
    sub_0807a968();
    TaskSleepForever();
}

void ScarfyState1Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay34);
    if (abs(TaskGetNearestPlayerDy()) <= 4)
    {
        ActorSetState(SCARFY_STATE_HOVER);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}

void ScarfyTransform(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = SCARFY_STATE_TRANSFORM;
    TaskStop();
    gCurTask->frameTable = gScarfyAngryFrames;
    ActorSetHitReactions(gScarfyTransformHitReactions);
    t = gCurTask;
    t->scarfyShakeTimer = 2;
    t->scarfyShakePhase = 0;
    v = -t->facing;
    t->scarfyShakeDir = v;
    t->velX = v * gUnk_08740934[0];
    t->scarfyLoopCount = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->scarfyLoopCount <= 1);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(12);
    TaskStop();
    ActorSetState(SCARFY_STATE_CHASE);
    TaskSleepForever();
}

void ScarfyTransformUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != SCARFY_STATE_TRANSFORM)
    {
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
    else if (--t->scarfyShakeTimer <= 0)
    {
        t->scarfyShakePhase ^= 1;
        t->velX = gUnk_08740934[t->scarfyShakePhase] * t->scarfyShakeDir;
        t->scarfyShakeTimer = 2;
    }
}

void ScarfyChase(void)
{
    struct Task *t;
    s32 i;
    s32 j;

    gCurTask->updateState = SCARFY_STATE_CHASE;
    TaskStop();
    t = gCurTask;
    t->scarfyChaseTimer = gUnk_0874094C[t->actorSpawnArg];
    i = t->actorSpawnArg;
    j = i * 2;
    t->scarfyChaseAccel = gUnk_08740950[j];
    t->scarfyChaseSpeedLimit = gUnk_08740950[j + 1];
    gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_0874093C);
    TaskSleepForever();
}

void ScarfyChaseUpdate(void)
{
    struct Task *t;
    s32 n;

    gCurTask->actorAnimDelay34 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay34);
    t = gCurTask;
    n = t->scarfyChaseTimer - 1;
    t->scarfyChaseTimer = n;
    if (n <= 0)
    {
        ActorSetState(SCARFY_STATE_EXPLODE);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
    else if ((n & 7) == 0)
    {
        TaskAccelerateTowardNearestPlayer(t->scarfyChaseAccel, t->scarfyChaseSpeedLimit);
    }
}

void ScarfyExplode(void)
{
    gCurTask->updateState = SCARFY_STATE_EXPLODE;
    TaskStop();
    gCurTask->scarfyLoopCount = 0;
    do
    {
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->scarfyLoopCount <= 5);
    TaskStopX();
    ActorSetHitReactions(gScarfyExplodeHitReactions);
    ActorDie();
}

void ScarfyExplodeUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay34);
}

void Task_SwordKnight(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gSwordKnightFrames;
    CallTableEntry(gCurTask->variant, 3, gSwordAndBladeKnightVariants);
}

void Task_BladeKnight(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gBladeKnightFrames;
    CallTableEntry(gCurTask->variant, 3, gSwordAndBladeKnightVariants);
}

s32 SwordAndBladeKnightStartFall(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_FALL);
    TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    return 1;
}

s32 SwordAndBladeKnightLand(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_0);
    TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    return 1;
}

s32 SwordAndBladeKnightEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_0807b058(void)
{
    TaskStop();
    gCurTask->swordAndBladeKnightStopped = 1;
    return 0;
}

s32 SwordAndBladeKnightHitWall(void)
{
    TaskStop();
    gCurTask->swordAndBladeKnightStopped = 1;
    return 0;
}

void SwordAndBladeKnightStopAtSlope(void)
{
    if (gCurTask->velX != 0 && (u8)ActorStepBackFromSlope() != 0)
    {
        TaskStop();
        gCurTask->swordAndBladeKnightStopped = 1;
    }
}

void SwordAndBladeKnightStepLunge(void)
{
    struct Task *t = gCurTask;

    if (t->swordAndBladeKnightStopped == 0 && --t->swordAndBladeKnightLungeTimer <= 0)
    {
        t->swordAndBladeKnightLungeTimer = 3;
        if (t->swordAndBladeKnightLungeStep <= 4)
        {
            t->swordAndBladeKnightLungeStep++;
            t->velX = gUnk_087409E4[t->swordAndBladeKnightLungeStep] * t->swordAndBladeKnightLungeDir;
        }
    }
}

void SwordAndBladeKnightStartLunge(void)
{
    struct Task *t = gCurTask;

    t->swordAndBladeKnightLungeDir = t->facing;
    t->swordAndBladeKnightLungeTimer = 1;
    t->swordAndBladeKnightLungeStep = 0;
    TaskStop();
    PlaySfx(196);
}

void sub_0807b11c(void)
{
    struct Task *t = gCurTask;

    t->u8C.actor->animScript = 0;
    t->swordAndBladeKnightLungeStep = 0;
    t->swordAndBladeKnightLungeTimer = 0;
    t->swordAndBladeKnightLungeDir = 0;
    t->swordAndBladeKnightComboCount = 0;
    t->swordAndBladeKnightWaterChecked = 0;
    t->swordAndBladeKnightStopped = 0;
    ActorSetState(0);
}

void sub_0807b144(void)
{
    if (gCurTask->u8C.actor->animScript == 0)
        gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_0874099C);
}

void SwordAndBladeKnightEndSlashUp(void)
{
    struct Task *t = gCurTask;

    if (t->swordAndBladeKnightComboCount <= 0)
    {
        if (RandomRange(4) == 0)
            goto other;
        t = gCurTask;
    }
    t->swordAndBladeKnightBackOffTime = 90;
    ActorSetState(5);
    return;

other:
    gCurTask->swordAndBladeKnightComboCount++;
    ActorSetState(4);
}

void SwordAndBladeKnightEndSlashDown(void)
{
    struct Task *t = gCurTask;

    if (t->swordAndBladeKnightComboCount <= 0)
    {
        if (RandomRange(4) == 0)
            goto other;
        t = gCurTask;
    }
    t->swordAndBladeKnightBackOffTime = 90;
    ActorSetState(5);
    return;

other:
    gCurTask->swordAndBladeKnightComboCount++;
    ActorSetState(3);
}

void CreateSwordAndBladeKnightSlash(void)
{
    gCurTask->swordAndBladeKnightSlashSlot = CreateChildTaskHere(TASK_SWORD_AND_BLADE_KNIGHT_SLASH, 1);
}

void sub_0807b200(void)
{
    struct PointPair box;
    s32 id = sub_08063a2c();

    if (id != -1)
    {
        struct Task *t = gCurTask;

        box.x0 = t->pixelX - 30;
        box.y0 = t->pixelY - 48;
        box.x1 = t->pixelX + 30;
        box.y1 = t->pixelY + 48;
        if (TaskIsInRectSlot(&box, id) != 0)
        {
            ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_6);
            TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
        }
    }
}

void SwordAndBladeKnightPickSlash(void *fn)
{
    struct Actor *a = gCurTask->u8C.actor;
    struct Task *t;

    TaskFaceNearestPlayer();
    t = gCurTask;
    t->swordAndBladeKnightComboCount = 0;
    t->swordAndBladeKnightLungeDir = t->facing;
    t->swordAndBladeKnightLungeTimer = 1;
    t->swordAndBladeKnightLungeStep = 0;
    a->animScript = 0;
    if (RandomRange(4) == 0)
    {
        ActorSetState(1);
        TaskSetEntry(fn, gCurTaskIdx);
    }
    else
    {
        ActorSetState(2);
        TaskSetEntry(fn, gCurTaskIdx);
    }
}

void SwordAndBladeKnightWalkInit(void)
{
    gCurTask->updateCallback = (u32)SwordAndBladeKnightWalkUpdate;
    sub_0807b11c();
    CallTableEntry(gCurTask->state, 8, gSwordAndBladeKnightWalkStates);
}

void SwordAndBladeKnightWalkUpdate(void)
{
    u8 v = (u8)ActorCollideTerrain();

    if ((gCurTask->onGround & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            gCurTask->actorFlatGroundY = (u16)gCurTask->actorFlatGroundY | 0x10000;
    }
    if ((gCurTask->onGround & 1) == 0)
        gCurTask->actorFlatGroundY = (u16)gCurTask->actorFlatGroundY;
    if (v == 0)
    {
        SwordAndBladeKnightStopAtSlope();
        CallTableEntry(gCurTask->updateState, 8, gSwordAndBladeKnightWalkStateUpdates);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->actorFlatGroundY = (gCurTask->actorFlatGroundY & 0xFFFF0000) | gCurTask->pixelY;
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SwordAndBladeKnightWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 8, gSwordAndBladeKnightWalkStates);
}

void SwordAndBladeKnightWalk(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_0;
    TaskStop();
    gCurTask->swordAndBladeKnightStopped = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_0807b144();
    TaskSleepForever();
}

void SwordAndBladeKnightWalkState0Update(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        SwordAndBladeKnightPickSlash(SwordAndBladeKnightWalkEnterState);
    else
        sub_0807b200();
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    t = gCurTask;
    if (t->swordAndBladeKnightWaterChecked == 0)
    {
        if ((t->waterFlags & 1) != 0)
            ActorStartDrown(-2);
        gCurTask->swordAndBladeKnightWaterChecked = 1;
    }
}

void SwordAndBladeKnightWalkState1(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_1;
    TaskStop();
    TaskSetFrame(15);
    TaskYieldTrampoline(16);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_UP);
    TaskSleepForever();
}

void SwordAndBladeKnightWalkState1Update(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_1)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkSlashUp(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_UP;
    SwordAndBladeKnightStartLunge();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    CreateSwordAndBladeKnightSlash();
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(8);
    SwordAndBladeKnightEndSlashUp();
    TaskSleepForever();
}

void SwordAndBladeKnightWalkSlashUpUpdate(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_UP)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    else
        SwordAndBladeKnightStepLunge();
}

void SwordAndBladeKnightWalkState2(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_2;
    TaskStop();
    TaskSetFrame(16);
    TaskYieldTrampoline(16);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_DOWN);
    TaskSleepForever();
}

void SwordAndBladeKnightWalkState2Update(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_2)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkSlashDown(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_DOWN;
    SwordAndBladeKnightStartLunge();
    gCurTask->frame = 10;
    TaskYieldTrampoline(2);
    CreateSwordAndBladeKnightSlash();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    SwordAndBladeKnightEndSlashDown();
    TaskSleepForever();
}

void SwordAndBladeKnightWalkSlashDownUpdate(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_SLASH_DOWN)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    else
        SwordAndBladeKnightStepLunge();
}

void SwordAndBladeKnightWalkWalkBack(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_WALK_BACK;
    TaskStop();
    gCurTask->swordAndBladeKnightStopped = 0;
    TaskFaceNearestPlayer();
    sub_0807b144();
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(gCurTask->swordAndBladeKnightBackOffTime);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_0);
    TaskSleepForever();
}

void SwordAndBladeKnightWalkWalkBackUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_WALK_BACK)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkState6(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_6;
    TaskStop();
    SwordAndBladeKnightStartLunge();
    CreateSwordAndBladeKnightSlash();
    if (RandomRange(4) != 0)
    {
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
    }
    else
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame = 16;
        TaskYieldTrampoline(8);
    }
    ActorSetState(SWORD_AND_BLADE_KNIGHT_WALK_STATE_0);
    TaskSleepForever();
}

void SwordAndBladeKnightWalkState6Update(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_WALK_STATE_6)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkFall(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_WALK_STATE_FALL;
    TaskStop();
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_0807b144();
    TaskSleepForever();
}

void SwordAndBladeKnightWalkFallUpdate(void)
{
}

void SwordAndBladeKnightIdleInit(void)
{
    gCurTask->updateCallback = (u32)SwordAndBladeKnightIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(SWORD_AND_BLADE_KNIGHT_IDLE_STATE_0);
    CallTableEntry(gCurTask->state, 1, gSwordAndBladeKnightIdleStates);
}

void SwordAndBladeKnightIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gSwordAndBladeKnightIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SwordAndBladeKnightIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gSwordAndBladeKnightIdleStates);
}

void SwordAndBladeKnightIdle(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_IDLE_STATE_0;
    TaskStop();
    gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_087409C0);
    TaskSleepForever();
}

void SwordAndBladeKnightIdleState0Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void SwordAndBladeKnightStandInit(void)
{
    gCurTask->updateCallback = (u32)SwordAndBladeKnightStandUpdate;
    sub_0807b11c();
    CallTableEntry(gCurTask->state, 6, gSwordAndBladeKnightStandStates);
}

void SwordAndBladeKnightStandUpdate(void)
{
    u8 v = (u8)ActorCollideTerrain();

    if ((gCurTask->onGround & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            gCurTask->actorFlatGroundY = (u16)gCurTask->actorFlatGroundY | 0x10000;
    }
    if ((gCurTask->onGround & 1) == 0)
        gCurTask->actorFlatGroundY = (u16)gCurTask->actorFlatGroundY;
    if (v == 0)
    {
        SwordAndBladeKnightStopAtSlope();
        CallTableEntry(gCurTask->updateState, 6, gSwordAndBladeKnightStandStateUpdates);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->actorFlatGroundY = (gCurTask->actorFlatGroundY & 0xFFFF0000) | gCurTask->pixelY;
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SwordAndBladeKnightStandEnterState(void)
{
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 6, gSwordAndBladeKnightStandStates);
}

void SwordAndBladeKnightStandState0(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_0;
    TaskStop();
    gCurTask->facing = 255;
    gCurTask->swordAndBladeKnightStopped = 0;
    sub_0807b144();
    TaskSleepForever();
}

void SwordAndBladeKnightStandState0Update(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        SwordAndBladeKnightPickSlash(SwordAndBladeKnightStandEnterState);
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    t = gCurTask;
    if (t->swordAndBladeKnightWaterChecked == 0)
    {
        if ((t->waterFlags & 1) != 0)
            ActorStartDrown(-2);
        gCurTask->swordAndBladeKnightWaterChecked = 1;
    }
}

void SwordAndBladeKnightStandState1(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_1;
    TaskStop();
    TaskSetFrame(15);
    TaskYieldTrampoline(16);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_UP);
    TaskSleepForever();
}

void SwordAndBladeKnightStandState1Update(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_STAND_STATE_1)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandSlashUp(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_UP;
    SwordAndBladeKnightStartLunge();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    CreateSwordAndBladeKnightSlash();
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(8);
    SwordAndBladeKnightEndSlashUp();
    TaskSleepForever();
}

void SwordAndBladeKnightStandSlashUpUpdate(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_UP)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState2(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_2;
    TaskStop();
    TaskSetFrame(16);
    TaskYieldTrampoline(16);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_DOWN);
    TaskSleepForever();
}

void SwordAndBladeKnightStandState2Update(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_STAND_STATE_2)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandSlashDown(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_DOWN;
    SwordAndBladeKnightStartLunge();
    gCurTask->frame = 10;
    TaskYieldTrampoline(2);
    CreateSwordAndBladeKnightSlash();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    SwordAndBladeKnightEndSlashDown();
    TaskSleepForever();
}

void SwordAndBladeKnightStandSlashDownUpdate(void)
{
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_STAND_STATE_SLASH_DOWN)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState5(void)
{
    gCurTask->updateState = SWORD_AND_BLADE_KNIGHT_STAND_STATE_5;
    TaskStop();
    gCurTask->swordAndBladeKnightStopped = 0;
    sub_0807b144();
    TaskYieldTrampoline(gCurTask->swordAndBladeKnightBackOffTime);
    ActorSetState(SWORD_AND_BLADE_KNIGHT_STAND_STATE_0);
    TaskSleepForever();
}

void SwordAndBladeKnightStandState5Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    if (gCurTask->state != SWORD_AND_BLADE_KNIGHT_STAND_STATE_5)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void Task_BlockStar(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gBlockStarFrames;
    CallTableEntry(gCurTask->variant, 1, gBlockStarVariants);
}

void BlockStarInit(void)
{
    gCurTask->updateCallback = (u32)BlockStarUpdate;
    TaskFaceNearestPlayer();
    gCurTask->frame = 4;
    TaskSleepForever();
}

void BlockStarUpdate(void)
{
    ActorAttachToHitter();
}

void Task_Needlous(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gNeedlousFrames;
    CallTableEntry(gCurTask->variant, 2, gNeedlousVariants);
}

void sub_0807bd60(void)
{
    TaskFaceNearestPlayer();
    TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
}

void NeedlousPickStartState(void)
{
    s32 r;
    struct Task *t;

    TaskFaceNearestPlayer();
    r = RandomRange(4);
    t = gCurTask;
    t->needlousSetupState = -1;
    t->needlousCooldownTimer = 0;
    t->unk1C = 0;
    t->needlousMoveTimer = 0;
    if (r == 0)
        ActorSetState(NEEDLOUS_STATE_WALK);
    else
        ActorSetState(NEEDLOUS_STATE_2);
}

s32 NeedlousStartFall(void)
{
    struct Task *t = gCurTask;

    if (t->state == NEEDLOUS_STATE_DASH)
    {
        t->needlousWallTimer = 0;
        TaskSetMotionXFacing(gUnk_08740A90[t->actorSpawnArg], 0x5A5A5A5A);
    }
    ActorSetState(NEEDLOUS_STATE_FALL);
    TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
    return 1;
}

s32 NeedlousLand(void)
{
    struct Task *t = gCurTask;
    struct Actor *a = t->u8C.actor;
    u8 st = t->state;

    switch (st)
    {
    case 1:
        ActorSetState(a->prevState);
        break;
    case 2:
        ActorSetState(NEEDLOUS_STATE_3);
        break;
    case 3:
        if (gUnk_0300244C != 0 && a->animScript == 0)
            gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08740AAC);
        ActorSetState(NEEDLOUS_STATE_4);
        break;
    case 4:
        ActorSetState(NEEDLOUS_STATE_DASH);
        break;
    }
    if (gCurTask->state != st)
        TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
    return 1;
}

s32 NeedlousEnterWater(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 NeedlousHitWall(void)
{
    if (gCurTask->state == 5)
    {
        gCurTask->needlousWallTimer = 16;
        TaskStop();
    }
    else
    {
        TaskTurnAroundAndReverseX();
    }
    return 0;
}

s32 sub_0807bed4(void)
{
    if (gCurTask->state == 5)
    {
        gCurTask->needlousWallTimer = 16;
        TaskStop();
    }
    else
    {
        TaskTurnAroundAndReverseX();
    }
    return 0;
}

void NeedlousHitCeiling(void)
{
    gCurTask->velY = 0;
}

void sub_0807bf0c(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->needlousSetupState != 0)
    {
        t->needlousMoveTimer = gUnk_08740A80[t->actorSpawnArg];
        t->needlousLevelTimer = gUnk_08740A88[t->actorSpawnArg];
        gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08740A98);
        gCurTask->needlousSetupState = 0;
        ActorSetAttackBox(gUnk_0873F720);
    }
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
}

void sub_0807bf74(void)
{
    TaskFaceNearestPlayer();
    TaskStop();
    ActorStopAnim();
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->actorAnimDelay34 = ActorStartAnim(gUnk_08740AAC);
}

void sub_0807bfd0(void)
{
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x20000, 0x2500, 0x30000);
}

void sub_0807c000(void)
{
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x10000, 0x2500, 0x30000);
}

void sub_0807c030(void)
{
    struct Task *t = gCurTask;

    if (t->needlousSetupState != 5)
    {
        t->needlousMoveTimer = 60;
        t->needlousWallTimer = 0;
        t->needlousCooldownTimer = 150;
        t->needlousSetupState = 5;
        TaskSetMotionXFacing(gUnk_08740A90[t->actorSpawnArg], 0x5A5A5A5A);
        ActorSetAttackBox(gUnk_08740E38);
        PlaySfx(199);
    }
}

void NeedlousInit(void)
{
    gCurTask->updateCallback = (u32)NeedlousUpdate;
    NeedlousPickStartState();
    CallTableEntry(gCurTask->state, 6, gNeedlousStates);
}

void NeedlousUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 6, gNeedlousStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void NeedlousEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gNeedlousStates);
}

void NeedlousWalk(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_WALK;
    TaskStop();
    sub_0807bf0c();
    TaskSleepForever();
}

void NeedlousWalkUpdate(void)
{
    struct Task *t;

    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    t = gCurTask;
    if (t->needlousCooldownTimer > 0)
        goto dec;
    if (--t->needlousMoveTimer <= 0)
        goto reset;
    if (abs(TaskGetNearestPlayerDy()) <= 9)
    {
        if (--gCurTask->needlousLevelTimer > 0)
            return;

    reset:
        ActorSetState(NEEDLOUS_STATE_2);
        TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
        return;
    }
    gCurTask->needlousLevelTimer = gUnk_08740A88[gCurTask->actorSpawnArg];
    return;

dec:
    t->needlousCooldownTimer--;
}

void NeedlousFall(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_FALL;
    TaskSetMotionY(0, 0x2500, 0x30000);
    TaskSleepForever();
}

void NeedlousFallUpdate(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void NeedlousState2(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_2;
    sub_0807bf74();
    TaskSleepForever();
}

void NeedlousState2Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void NeedlousState3(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_3;
    sub_0807bfd0();
    TaskSleepForever();
}

void NeedlousState3Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void NeedlousState4(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_4;
    sub_0807c000();
    TaskSleepForever();
}

void NeedlousState4Update(void)
{
    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
}

void NeedlousDash(void)
{
    gCurTask->updateState = NEEDLOUS_STATE_DASH;
    sub_0807c030();
    while (1)
    {
        CreateDustTrail(1, 1, 4, 4);
        TaskYieldTrampoline(8);
    }
}

void NeedlousDashUpdate(void)
{
    struct Task *t;

    gCurTask->actorAnimDelay34 = ActorTickAnim(gCurTask->actorAnimDelay34);
    t = gCurTask;
    if (t->needlousWallTimer <= 0)
    {
        if (--t->needlousMoveTimer <= 0)
        {
            ActorSetState(NEEDLOUS_STATE_WALK);
            TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
        }
    }
    else if (--t->needlousWallTimer <= 0)
    {
        TaskTurnAroundAndReverseX();
        TaskSetMotionXFacing(gUnk_08740A90[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    }
}

void NeedlousIdleInit(void)
{
    gCurTask->updateCallback = (u32)NeedlousIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(NEEDLOUS_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gNeedlousIdleStates);
}

void NeedlousIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gNeedlousIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void NeedlousIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gNeedlousIdleStates);
}

void NeedlousIdle(void)
{
    gCurTask->updateState = NEEDLOUS_IDLE_STATE_IDLE;
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
}

void NeedlousIdleState0Update(void)
{
}

void Task_UFO(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUFOFrames;
    CallTableEntry(gCurTask->variant, 2, gUFOVariants);
}

void UFOTeardown(void)
{
    if (--gPaletteAnimRefCounts[0] < 0)
        sub_0806ee2c();
}

void UFOStartPaletteAnim(void)
{
    struct Actor *a = gCurTask->u8C.actor;
    u32 v;

    AcquirePaletteAnim(1, 0);
    v = a->palette;
    if (v == 0)
        v = gUFOGfx[2];
    SetPaletteAnimSource(0, v, a->paletteVariant);
}

void sub_0807c4d4(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    ActorSetState(UFO_STATE_PICK_MOVE);
    t = gCurTask;
    t->ufoZigzagCount = 0;
    t->ufoPointCount = 0;
    t->ufoPointIndex = 0;
    gCurTask->actorAnimDelay18 = ActorStartAnimNoFlip(gUnk_08740B08);
    UFOStartPaletteAnim();
}

s32 UFOPickNextPoint(void)
{
    s32 r = RandomRange(10);

    if (r <= 4)
        r += 2;
    else
        r += 5;
    r += gCurTask->ufoPointIndex;
    return 15 & r;
}

void UFOSetTarget(s32 a)
{
    struct Rect box;
    struct Task *t;

    gCurTask->ufoPointIndex = a;
    TaskGetNearestPlayerPos();
    t = gCurTask;
    t->ufoTargetX = gUnk_08740B3C[t->ufoPointIndex] + gUnk_030023B4;
    t->ufoTargetY = gUnk_08740B60[t->ufoPointIndex] + gUnk_030023D4;
    UFOIsAtTarget(&box);
    gCurTask->ufoFlightAngle = (u16)GetPointAngle(box.left, box.top,
                                             (s16)gCurTask->ufoTargetX,
                                             (s16)gCurTask->ufoTargetY, 3);
}

s32 UFOIsAtTarget(struct Rect *r)
{
    struct Task *t = gCurTask;

    r->left = t->pixelX;
    r->top = t->pixelY;
    r->right = t->ufoTargetX;
    r->bottom = t->ufoTargetY;
    if (GetDistSq((struct PointPair *)r) <= 64)
        return 1;
    else
        return 0;
}

void UFOSetFlightVelocity(void)
{
    struct Task *t;

    AngleToVector((s16)gCurTask->ufoFlightAngle, 1024);
    t = gCurTask;
    t->velX = gUnk_030023B4;
    t->velY = gUnk_030023D4;
}

void CreateUFOLaser(void)
{
    struct ActorSpawn spawn;
    struct Task *t = gCurTask;

    if (GetShapeAtPixelIgnoringOneWay(t->pixelX + t->facing * 16, t->pixelY) == 0)
    {
        spawn.subtype = 31;
        spawn.taskType = TASK_UFO_LASER;
        spawn.variant = UFO_LASER_VARIANT_INIT;
        spawn.spawnArg = 0;
        spawn.x = 8;
        spawn.y = 0;
        spawn.checkTerrain = 1;
        gCurTask->ufoLaserSlot = CreateActorFromDescAtOffsetFacing(&spawn, 0);
        PlaySfx(165);
    }
}

void UFOInit(void)
{
    gCurTask->updateCallback = (u32)UFOUpdate;
    sub_0807c4d4();
    CallTableEntry(gCurTask->state, 4, gUFOStates);
}

void UFOUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gUFOStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void UFOEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gUFOStates);
}

void UFOZigzag(void)
{
    gCurTask->updateState = UFO_STATE_ZIGZAG;
    TaskStop();
    gCurTask->ufoLoopCount = 0;
    while ((s16)gCurTask->ufoLoopCount < gCurTask->ufoZigzagCount)
    {
        gCurTask->velX = 0x8000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(15);
        gCurTask->velX = 0x10000;
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(15);
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x8000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->ufoLoopCount++;
    }
    ActorSetState(UFO_STATE_PICK_MOVE);
    TaskSleepForever();
}

void UFOZigzagUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != UFO_STATE_ZIGZAG)
        TaskSetEntry(UFOEnterState, gCurTaskIdx);
}

void UFOPickMove(void)
{
    gCurTask->updateState = UFO_STATE_PICK_MOVE;
    TaskStop();
    TaskSleepForever();
}

void UFOPickMoveUpdate(void)
{
    struct Task *t;
    s32 v;

    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    t = gCurTask;
    if (++t->ufoZigzagCount == 3)
        t->ufoZigzagCount = 0;
    t = gCurTask;
    if (++t->ufoPointCount == 4)
    {
        TaskFaceNearestPlayer();
        v = gCurTask->facing == 1 ? 16 : 17;
    }
    else
    {
        v = UFOPickNextPoint();
        if (v > 3)
        {
            if (v <= 11)
            {
                if (AreAllPlayersOnGround() != 0)
                    v = UFOPickNextPoint();
            }
        }
    }
    UFOSetTarget(v);
    ActorSetState(UFO_STATE_FLY_TO_TARGET);
    TaskSetEntry(UFOEnterState, gCurTaskIdx);
}

void UFOFlyToTarget(void)
{
    gCurTask->updateState = UFO_STATE_FLY_TO_TARGET;
    TaskStop();
    UFOSetFlightVelocity();
    TaskSleepForever();
}

void UFOFlyToTargetUpdate(void)
{
    struct Rect box;

    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if ((u8)UFOIsAtTarget(&box) != 0)
    {
        struct Task *t = gCurTask;

        if (t->ufoPointCount == 4)
        {
            t->ufoPointCount = 0;
            ActorSetState(UFO_STATE_SHOOT);
            TaskSetEntry(UFOEnterState, gCurTaskIdx);
        }
        else
        {
            struct Actor *a = t->u8C.actor;

            if (GetShapeAtPixelIgnoringOneWay(t->pixelX, t->pixelY + ((s8 *)a->terrainBox)[2]) != 0)
                gCurTask->ufoZigzagCount = 0;
            {
                struct Task *u = gCurTask;

                if (GetShapeAtPixelIgnoringOneWay(u->pixelX + ((s8 *)a->terrainBox)[5], u->pixelY) != 0)
                    gCurTask->ufoZigzagCount = 0;
            }
            {
                struct Task *u = gCurTask;

                if (GetShapeAtPixelIgnoringOneWay(u->pixelX + ((s8 *)a->terrainBox)[4], u->pixelY) != 0)
                    gCurTask->ufoZigzagCount = 0;
            }
            ActorSetState(UFO_STATE_ZIGZAG);
            TaskSetEntry(UFOEnterState, gCurTaskIdx);
        }
    }
}

void UFOShoot(void)
{
    gCurTask->updateState = UFO_STATE_SHOOT;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->frame = 4;
    TaskYieldTrampoline(10);
    TaskSetFrame(12);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    CreateUFOLaser();
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    TaskYieldTrampoline(20);
    ActorSetState(UFO_STATE_PICK_MOVE);
    TaskSleepForever();
}

void UFOShootUpdate(void)
{
    if (gCurTask->state != UFO_STATE_SHOOT)
    {
        gCurTask->actorAnimDelay18 = ActorStartAnimNoFlip(gUnk_08740B08);
        TaskSetEntry(UFOEnterState, gCurTaskIdx);
    }
}

void UFOIdleInit(void)
{
    gCurTask->updateCallback = (u32)UFOIdleUpdate;
    TaskFaceNearestPlayer();
    UFOStartPaletteAnim();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(UFO_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gUFOIdleStates);
}

void UFOIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUFOIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void UFOIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gUFOIdleStates);
}

void UFOIdle(void)
{
    gCurTask->updateState = UFO_IDLE_STATE_IDLE;
    TaskStop();
    while (1)
    {
        gCurTask->frame = 4;
        TaskYieldTrampoline(8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(8);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(8);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(8);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(8);
        gCurTask->frame = 7;
        TaskYieldTrampoline(8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(8);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(8);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(8);
    }
}

void UFOIdleState0Update(void)
{
}

void Task_Parasol(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gParasolFrames;
    CallTableEntry(gCurTask->variant, 4, gParasolVariants);
}

void sub_0807cbf4(void)
{
    struct Task *t = gCurTask;
    u16 v;

    if (t->parasolHittableAtOnce == 1)
        t->parasolHittable = 1;
    else
        t->parasolHittable = 0;
    v = TaskGetAngleTo(gCurTask->parasolPlayerSlot, 3);
    AngleToVector((v + 256) & 511, 102);
    gCurTask->velX = gUnk_030023B4;
    TaskSetFrameByFacing(9);
    TaskYieldTrampoline(12);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    gCurTask->parasolHittable = 1;
}

void ParasolRiseInit(void)
{
    gCurTask->updateCallback = (u32)ParasolRiseUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(PARASOL_RISE_STATE_RISE);
    CallTableEntry(gCurTask->state, 1, gParasolRiseStates);
}

void ParasolRiseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gParasolRiseStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->parasolHittable != 0)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void ParasolRiseEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gParasolRiseStates);
}

void ParasolRise(void)
{
    gCurTask->updateState = PARASOL_RISE_STATE_RISE;
    TaskStop();
    sub_0807cbf4();
    gCurTask->velY = -0x10000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void ParasolRiseState0Update(void)
{
}

void ParasolChaseInit(void)
{
    gCurTask->updateCallback = (u32)ParasolChaseUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(PARASOL_CHASE_STATE_CHASE);
    gCurTask->parasolLifeTimer = 255;
    CallTableEntry(gCurTask->state, 1, gParasolChaseStates);
}

void ParasolChaseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gParasolChaseStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->parasolHittable != 0)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void ParasolChaseEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gParasolChaseStates);
}

void ParasolChase(void)
{
    gCurTask->updateState = PARASOL_CHASE_STATE_CHASE;
    sub_0807cbf4();
    gCurTask->parasolSteerTimer = 15;
    TaskStop();
    gCurTask->velY = -0x18100;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void ParasolChaseState0Update(void)
{
    struct Task *t;

    if (gCurTask->parasolSteerTimer <= 0)
    {
        u16 v = TaskGetAngleToNearestPlayer(3);

        v = ((v + 32) & 511) >> 6;
        TaskAccelerateInDir(0x4D00, 0x18100, v);
        gCurTask->parasolSteerTimer = 15;
    }
    t = gCurTask;
    t->parasolSteerTimer--;
    if (t->parasolLifeTimer <= 0)
    {
        ActorSetHitReactions(gParasolChaseHitReactions);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    gCurTask->parasolLifeTimer--;
}

void ParasolIdleInit(void)
{
    gCurTask->updateCallback = (u32)ParasolIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(PARASOL_IDLE_STATE_IDLE);
    CallTableEntry(gCurTask->state, 1, gParasolIdleStates);
}

void ParasolIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gParasolIdleStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void ParasolIdleEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gParasolIdleStates);
}

void ParasolIdle(void)
{
    gCurTask->updateState = PARASOL_IDLE_STATE_IDLE;
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void ParasolIdleState0Update(void)
{
}

void ParasolVariant3(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_0807cff0;
    t->frame = 4;
    TaskSleepForever();
}

void sub_0807cff0(void)
{
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_PengyIceBreath(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    t = gCurTask;
    t->frameTable = gPengyIceBreathFrames;
    t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    CallTableEntry(t->variant, 1, gPengyIceBreathVariants);
}

void PengyIceBreathInit(void)
{
    gCurTask->updateCallback = (u32)PengyIceBreathUpdate;
    TaskFaceLikeParent();
    ActorSetState(PENGY_ICE_BREATH_STATE_0);
    CallTableEntry(gCurTask->state, 1, gPengyIceBreathStates);
}

void PengyIceBreathUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gPengyIceBreathStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void PengyIceBreathEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gPengyIceBreathStates);
}

void PengyIceBreathState0(void)
{
    struct Task *t;

    gCurTask->updateState = PENGY_ICE_BREATH_STATE_0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08741094[gCurTask->actorSpawnArg], 0x5A5A5A5A);
    t = gCurTask;
    t->velY = gUnk_087410A0[t->actorSpawnArg];
    t->pengyIceBreathLoopCount = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk1C = 1;
    } while ((s16)++gCurTask->pengyIceBreathLoopCount <= 4);
    ActorDestroy();
}

void PengyIceBreathState0Update(void)
{
}

void Task_UFOLaser(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    t = gCurTask;
    t->frameTable = gUFOLaserFrames;
    CallTableEntry(t->variant, 1, gUFOLaserVariants);
}

s32 UFOLaserReactToDamage(void)
{
    ActorSetState(UFO_LASER_STATE_VANISH);
    TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    return 1;
}

s32 UFOLaserReactToDefeat(void)
{
    ActorSetState(UFO_LASER_STATE_VANISH);
    TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    return 1;
}

void UFOLaserInit(void)
{
    gCurTask->updateCallback = (u32)UFOLaserUpdate;
    TaskFaceLikeParent();
    ActorSetState(UFO_LASER_STATE_0);
    CallTableEntry(gCurTask->state, 2, gUFOLaserStates);
}

void UFOLaserUpdate(void)
{
    if ((u8)ActorCollideTerrainPointPushOut() != 0)
    {
        ActorSetState(UFO_LASER_STATE_VANISH);
        TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 2, gUFOLaserStateUpdates);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void UFOLaserEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gUFOLaserStates);
}

void UFOLaserState0(void)
{
    gCurTask->updateState = UFO_LASER_STATE_0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x80000, 0x5A5A5A5A);
    gCurTask->frame = 4;
    TaskSleepForever();
}

void UFOLaserState0Update(void)
{
    if ((u8)(gTerrainResult[4] - 1) <= 3)
    {
        TaskStop();
        ActorSetState(UFO_LASER_STATE_VANISH);
        TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    }
}

void UFOLaserVanish(void)
{
    struct Task *t;

    gCurTask->updateCallback = 0;
    TaskStop();
    t = gCurTask;
    t->pixelX += t->facing * 16;
    t->posX = t->pixelX << 16;
    t->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    ActorDestroy();
}

void sub_0807d384(void)
{
}

void Task_SwordAndBladeKnightSlash(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)SwordAndBladeKnightSlashUpdate;
    t->swordAndBladeKnightSlashStruck = 0;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}
