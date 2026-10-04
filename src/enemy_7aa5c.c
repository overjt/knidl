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
 *   * the sword knights' remaining states and rows `SwordAndBladeKnightWalkState0`..
 *     `SwordAndBladeKnightStandInit`, which probe for a partner with `TaskIsInRectSlot` over a
 *     stack `struct PointPair`
 *     and reacts through the shared `sub_0807b294` state entry;
 *   * Task_UFO's row 0 `UFOInit`+`UFOUpdate` with the
 *     `UFOIsAtTarget` box test (`struct Rect` + GetDistSq) and the
 *     `sub_0807c530` aim helper that clamps into `0x08740B3C`/`0x08740B60`;
 *   * UFO's state check `sub_0807c828` (gUFOStateUpdates[1]), which walks a
 *     sixteen-entry cue ring through
 *     `sub_0807c508` (`15 & (rand + Task.unk24)`);
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
extern s32 sub_08066338();
extern s32 ActorReactToHit(void);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_080699a8(void);
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
        gCurTask->velY = gUnk_08740864[i] * gCurTask->unk28;
        TaskYieldTrampoline(gUnk_08740864[i + 1]);
    }
    gCurTask->unk28 = -gCurTask->unk28;
    while (1)
    {
        for (i = 0; i <= 32; i += 2)
        {
            gCurTask->velY = gUnk_087408AC[i] * gCurTask->unk28;
            TaskYieldTrampoline(gUnk_087408AC[i + 1]);
        }
        gCurTask->unk28 = -gCurTask->unk28;
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
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->state != 0)
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
    gCurTask->updateState = 2;
    sub_0807a8fc();
    sub_0807aa5c();
}

void ScarfyHoverUpdate(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
}

void ScarfyHide(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void ScarfyHideUpdate(void)
{
    if (TaskIsNearestPlayerWithinX(10) != 0)
    {
        ActorSetState(1);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}

void ScarfyState1(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk34 = ActorStartAnim(gUnk_08740854);
    sub_0807a968();
    TaskSleepForever();
}

void sub_0807ac08(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
    if (abs(TaskGetNearestPlayerDy()) <= 4)
    {
        ActorSetState(2);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
}

void ScarfyTransform(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 3;
    TaskStop();
    gCurTask->frameTable = gScarfyAngryFrames;
    ActorSetHitReactions(gUnk_08740F50);
    t = gCurTask;
    t->unk28 = 2;
    t->unk2C = 0;
    v = -t->facing;
    t->unk30 = v;
    t->velX = v * gUnk_08740934[0];
    t->unk6C = 0;
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
    } while ((s16)++gCurTask->unk6C <= 1);
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
    ActorSetState(4);
    TaskSleepForever();
}

void ScarfyTransformUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 3)
    {
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
    else if (--t->unk28 <= 0)
    {
        t->unk2C ^= 1;
        t->velX = gUnk_08740934[t->unk2C] * t->unk30;
        t->unk28 = 2;
    }
}

void ScarfyChase(void)
{
    struct Task *t;
    s32 i;
    s32 j;

    gCurTask->updateState = 4;
    TaskStop();
    t = gCurTask;
    t->unk28 = gUnk_0874094C[t->unk74];
    i = t->unk74;
    j = i * 2;
    t->unk2C = gUnk_08740950[j];
    t->unk30 = gUnk_08740950[j + 1];
    gCurTask->unk34 = ActorStartAnim(gUnk_0874093C);
    TaskSleepForever();
}

void ScarfyChaseUpdate(void)
{
    struct Task *t;
    s32 n;

    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
    t = gCurTask;
    n = t->unk28 - 1;
    t->unk28 = n;
    if (n <= 0)
    {
        ActorSetState(5);
        TaskSetEntry(ScarfyEnterState, gCurTaskIdx);
    }
    else if ((n & 7) == 0)
    {
        TaskAccelerateTowardNearestPlayer(t->unk2C, t->unk30);
    }
}

void ScarfyExplode(void)
{
    gCurTask->updateState = 5;
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 5);
    TaskStopX();
    ActorSetHitReactions(gUnk_08740F5C);
    ActorDie();
}

void ScarfyExplodeUpdate(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
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

s32 sub_0807afd8(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(7);
    TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807b010(void)
{
    if (gCurTask->variant != 0)
        return 0;
    ActorSetState(0);
    TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807b048(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_0807b058(void)
{
    TaskStop();
    gCurTask->unk1C = 1;
    return 0;
}

s32 sub_0807b070(void)
{
    TaskStop();
    gCurTask->unk1C = 1;
    return 0;
}

void sub_0807b088(void)
{
    if (gCurTask->velX != 0 && (u8)sub_080699a8() != 0)
    {
        TaskStop();
        gCurTask->unk1C = 1;
    }
}

void sub_0807b0b4(void)
{
    struct Task *t = gCurTask;

    if (t->unk1C == 0 && --t->unk30 <= 0)
    {
        t->unk30 = 3;
        if (t->unk34 <= 4)
        {
            t->unk34++;
            t->velX = gUnk_087409E4[t->unk34] * t->unk2C;
        }
    }
}

void sub_0807b0f0(void)
{
    struct Task *t = gCurTask;

    t->unk2C = t->facing;
    t->unk30 = 1;
    t->unk34 = 0;
    TaskStop();
    PlaySfx(196);
}

void sub_0807b11c(void)
{
    struct Task *t = gCurTask;

    t->u8C.actor->animScript = 0;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk18 = 0;
    t->unk1C = 0;
    ActorSetState(0);
}

void sub_0807b144(void)
{
    if (gCurTask->u8C.actor->animScript == 0)
        gCurTask->unk34 = ActorStartAnim(gUnk_0874099C);
}

void sub_0807b16c(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 <= 0)
    {
        if (RandomRange(4) == 0)
            goto other;
        t = gCurTask;
    }
    t->unk30 = 90;
    ActorSetState(5);
    return;

other:
    gCurTask->unk28++;
    ActorSetState(4);
}

void sub_0807b1a8(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 <= 0)
    {
        if (RandomRange(4) == 0)
            goto other;
        t = gCurTask;
    }
    t->unk30 = 90;
    ActorSetState(5);
    return;

other:
    gCurTask->unk28++;
    ActorSetState(3);
}

void CreateSwordAndBladeKnightSlash(void)
{
    gCurTask->unk46 = CreateChildTaskHere(173, 1);
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
            ActorSetState(6);
            TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
        }
    }
}

void sub_0807b294(void *fn)
{
    struct Actor *a = gCurTask->u8C.actor;
    struct Task *t;

    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk28 = 0;
    t->unk2C = t->facing;
    t->unk30 = 1;
    t->unk34 = 0;
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
            gCurTask->unk24 = (u16)gCurTask->unk24 | 0x10000;
    }
    if ((gCurTask->onGround & 1) == 0)
        gCurTask->unk24 = (u16)gCurTask->unk24;
    if (v == 0)
    {
        sub_0807b088();
        CallTableEntry(gCurTask->updateState, 8, gSwordAndBladeKnightWalkStateUpdates);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) | gCurTask->pixelY;
        ActorCheckHits();
        ActorReactToHit();
    }
}

void SwordAndBladeKnightWalkEnterState(void)
{
    CallTableEntry(gCurTask->state, 8, gSwordAndBladeKnightWalkStates);
}

void SwordAndBladeKnightWalkState0(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk1C = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_0807b144();
    TaskSleepForever();
}

void sub_0807b430(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        sub_0807b294(SwordAndBladeKnightWalkEnterState);
    else
        sub_0807b200();
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk18 == 0)
    {
        if ((t->waterFlags & 1) != 0)
            ActorStartDrown(-2);
        gCurTask->unk18 = 1;
    }
}

void SwordAndBladeKnightWalkState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskSetFrame(15);
    TaskYieldTrampoline(16);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_0807b4c8(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkState3(void)
{
    gCurTask->updateState = 3;
    sub_0807b0f0();
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
    sub_0807b16c();
    TaskSleepForever();
}

void sub_0807b558(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void SwordAndBladeKnightWalkState2(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    TaskSetFrame(16);
    TaskYieldTrampoline(16);
    ActorSetState(4);
    TaskSleepForever();
}

void sub_0807b5b0(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkState4(void)
{
    gCurTask->updateState = 4;
    sub_0807b0f0();
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
    sub_0807b1a8();
    TaskSleepForever();
}

void sub_0807b640(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void SwordAndBladeKnightWalkState5(void)
{
    gCurTask->updateState = 5;
    TaskStop();
    gCurTask->unk1C = 0;
    TaskFaceNearestPlayer();
    sub_0807b144();
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(gCurTask->unk30);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807b6b4(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    if (gCurTask->state != 5)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkState6(void)
{
    gCurTask->updateState = 6;
    TaskStop();
    sub_0807b0f0();
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
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807b7a8(void)
{
    if (gCurTask->state != 6)
        TaskSetEntry(SwordAndBladeKnightWalkEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightWalkFall(void)
{
    gCurTask->updateState = 7;
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
    ActorSetState(0);
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
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk34 = ActorStartAnim(gUnk_087409C0);
    TaskSleepForever();
}

void sub_0807b8d0(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
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
            gCurTask->unk24 = (u16)gCurTask->unk24 | 0x10000;
    }
    if ((gCurTask->onGround & 1) == 0)
        gCurTask->unk24 = (u16)gCurTask->unk24;
    if (v == 0)
    {
        sub_0807b088();
        CallTableEntry(gCurTask->updateState, 6, gSwordAndBladeKnightStandStateUpdates);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) | gCurTask->pixelY;
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
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->facing = 255;
    gCurTask->unk1C = 0;
    sub_0807b144();
    TaskSleepForever();
}

void sub_0807ba18(void)
{
    struct Task *t;

    if (abs(TaskGetNearestPlayerDx()) <= 63)
        sub_0807b294(SwordAndBladeKnightStandEnterState);
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk18 == 0)
    {
        if ((t->waterFlags & 1) != 0)
            ActorStartDrown(-2);
        gCurTask->unk18 = 1;
    }
}

void SwordAndBladeKnightStandState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskSetFrame(15);
    TaskYieldTrampoline(16);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_0807baa8(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState3(void)
{
    gCurTask->updateState = 3;
    sub_0807b0f0();
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
    sub_0807b16c();
    TaskSleepForever();
}

void sub_0807bb38(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState2(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    TaskSetFrame(16);
    TaskYieldTrampoline(16);
    ActorSetState(4);
    TaskSleepForever();
}

void sub_0807bb8c(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState4(void)
{
    gCurTask->updateState = 4;
    sub_0807b0f0();
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
    sub_0807b1a8();
    TaskSleepForever();
}

void sub_0807bc1c(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(SwordAndBladeKnightStandEnterState, gCurTaskIdx);
}

void SwordAndBladeKnightStandState5(void)
{
    gCurTask->updateState = 5;
    TaskStop();
    gCurTask->unk1C = 0;
    sub_0807b144();
    TaskYieldTrampoline(gCurTask->unk30);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807bc78(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    if (gCurTask->state != 5)
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
    t->unk24 = -1;
    t->unk18 = 0;
    t->unk1C = 0;
    t->unk20 = 0;
    if (r == 0)
        ActorSetState(0);
    else
        ActorSetState(2);
}

s32 sub_0807bdb8(void)
{
    struct Task *t = gCurTask;

    if (t->state == 5)
    {
        t->unk1C = 0;
        TaskSetMotionXFacing(gUnk_08740A90[t->unk74], 0x5A5A5A5A);
    }
    ActorSetState(1);
    TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807be08(void)
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
        ActorSetState(3);
        break;
    case 3:
        if (gUnk_0300244C != 0 && a->animScript == 0)
            gCurTask->unk34 = ActorStartAnim(gUnk_08740AAC);
        ActorSetState(4);
        break;
    case 4:
        ActorSetState(5);
        break;
    }
    if (gCurTask->state != st)
        TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807be9c(void)
{
    ActorStartDrown(-2);
    return 1;
}

s32 sub_0807beac(void)
{
    if (gCurTask->state == 5)
    {
        gCurTask->unk1C = 16;
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
        gCurTask->unk1C = 16;
        TaskStop();
    }
    else
    {
        TaskTurnAroundAndReverseX();
    }
    return 0;
}

void sub_0807befc(void)
{
    gCurTask->velY = 0;
}

void sub_0807bf0c(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->unk24 != 0)
    {
        t->unk20 = gUnk_08740A80[t->unk74];
        t->unk1C = gUnk_08740A88[t->unk74];
        gCurTask->unk34 = ActorStartAnim(gUnk_08740A98);
        gCurTask->unk24 = 0;
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
    gCurTask->unk34 = ActorStartAnim(gUnk_08740AAC);
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

    if (t->unk24 != 5)
    {
        t->unk20 = 60;
        t->unk1C = 0;
        t->unk18 = 150;
        t->unk24 = 5;
        TaskSetMotionXFacing(gUnk_08740A90[t->unk74], 0x5A5A5A5A);
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
    gCurTask->updateState = 0;
    TaskStop();
    sub_0807bf0c();
    TaskSleepForever();
}

void NeedlousWalkUpdate(void)
{
    struct Task *t;

    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk18 > 0)
        goto dec;
    if (--t->unk20 <= 0)
        goto reset;
    if (abs(TaskGetNearestPlayerDy()) <= 9)
    {
        if (--gCurTask->unk1C > 0)
            return;

    reset:
        ActorSetState(2);
        TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
        return;
    }
    gCurTask->unk1C = gUnk_08740A88[gCurTask->unk74];
    return;

dec:
    t->unk18--;
}

void NeedlousFall(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x2500, 0x30000);
    TaskSleepForever();
}

void NeedlousFallUpdate(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void NeedlousState2(void)
{
    gCurTask->updateState = 2;
    sub_0807bf74();
    TaskSleepForever();
}

void sub_0807c22c(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void NeedlousState3(void)
{
    gCurTask->updateState = 3;
    sub_0807bfd0();
    TaskSleepForever();
}

void sub_0807c264(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void NeedlousState4(void)
{
    gCurTask->updateState = 4;
    sub_0807c000();
    TaskSleepForever();
}

void sub_0807c29c(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void NeedlousDash(void)
{
    gCurTask->updateState = 5;
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

    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk1C <= 0)
    {
        if (--t->unk20 <= 0)
        {
            ActorSetState(0);
            TaskSetEntry(NeedlousEnterState, gCurTaskIdx);
        }
    }
    else if (--t->unk1C <= 0)
    {
        TaskTurnAroundAndReverseX();
        TaskSetMotionXFacing(gUnk_08740A90[gCurTask->unk74], 0x5A5A5A5A);
    }
}

void NeedlousIdleInit(void)
{
    gCurTask->updateCallback = (u32)NeedlousIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
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
    gCurTask->updateState = 0;
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

void sub_0807c440(void)
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

void sub_0807c484(void)
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
    ActorSetState(1);
    t = gCurTask;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk24 = 0;
    gCurTask->unk18 = ActorStartAnimNoFlip(gUnk_08740B08);
    UFOStartPaletteAnim();
}

s32 sub_0807c508(void)
{
    s32 r = RandomRange(10);

    if (r <= 4)
        r += 2;
    else
        r += 5;
    r += gCurTask->unk24;
    return 15 & r;
}

void sub_0807c530(s32 a)
{
    struct Rect box;
    struct Task *t;

    gCurTask->unk24 = a;
    TaskGetNearestPlayerPos();
    t = gCurTask;
    t->unk2C = gUnk_08740B3C[t->unk24] + gUnk_030023B4;
    t->unk28 = gUnk_08740B60[t->unk24] + gUnk_030023D4;
    UFOIsAtTarget(&box);
    gCurTask->unk34 = (u16)GetPointAngle(box.left, box.top,
                                             (s16)gCurTask->unk2C,
                                             (s16)gCurTask->unk28, 3);
}

s32 UFOIsAtTarget(struct Rect *r)
{
    struct Task *t = gCurTask;

    r->left = t->pixelX;
    r->top = t->pixelY;
    r->right = t->unk2C;
    r->bottom = t->unk28;
    if (GetDistSq((struct PointPair *)r) <= 64)
        return 1;
    else
        return 0;
}

void sub_0807c5e4(void)
{
    struct Task *t;

    AngleToVector((s16)gCurTask->unk34, 1024);
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
        spawn.taskType = 134;
        spawn.variant = 0;
        spawn.spawnArg = 0;
        spawn.x = 8;
        spawn.y = 0;
        spawn.checkTerrain = 1;
        gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 0);
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

void UFOState0(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gCurTask->unk1C)
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
        gCurTask->unk6C++;
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0807c7d8(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 0)
        TaskSetEntry(UFOEnterState, gCurTaskIdx);
}

void UFOState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskSleepForever();
}

void sub_0807c828(void)
{
    struct Task *t;
    s32 v;

    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    t = gCurTask;
    if (++t->unk1C == 3)
        t->unk1C = 0;
    t = gCurTask;
    if (++t->unk20 == 4)
    {
        TaskFaceNearestPlayer();
        v = gCurTask->facing == 1 ? 16 : 17;
    }
    else
    {
        v = sub_0807c508();
        if (v > 3)
        {
            if (v <= 11)
            {
                if (sub_08066338() != 0)
                    v = sub_0807c508();
            }
        }
    }
    sub_0807c530(v);
    ActorSetState(2);
    TaskSetEntry(UFOEnterState, gCurTaskIdx);
}

void UFOState2(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    sub_0807c5e4();
    TaskSleepForever();
}

void sub_0807c8d0(void)
{
    struct Rect box;

    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if ((u8)UFOIsAtTarget(&box) != 0)
    {
        struct Task *t = gCurTask;

        if (t->unk20 == 4)
        {
            t->unk20 = 0;
            ActorSetState(3);
            TaskSetEntry(UFOEnterState, gCurTaskIdx);
        }
        else
        {
            struct Actor *a = t->u8C.actor;

            if (GetShapeAtPixelIgnoringOneWay(t->pixelX, t->pixelY + ((s8 *)a->terrainBox)[2]) != 0)
                gCurTask->unk1C = 0;
            {
                struct Task *u = gCurTask;

                if (GetShapeAtPixelIgnoringOneWay(u->pixelX + ((s8 *)a->terrainBox)[5], u->pixelY) != 0)
                    gCurTask->unk1C = 0;
            }
            {
                struct Task *u = gCurTask;

                if (GetShapeAtPixelIgnoringOneWay(u->pixelX + ((s8 *)a->terrainBox)[4], u->pixelY) != 0)
                    gCurTask->unk1C = 0;
            }
            ActorSetState(0);
            TaskSetEntry(UFOEnterState, gCurTaskIdx);
        }
    }
}

void UFOShoot(void)
{
    gCurTask->updateState = 3;
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
    ActorSetState(1);
    TaskSleepForever();
}

void UFOShootUpdate(void)
{
    if (gCurTask->state != 3)
    {
        gCurTask->unk18 = ActorStartAnimNoFlip(gUnk_08740B08);
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
    ActorSetState(0);
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
    gCurTask->updateState = 0;
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

void sub_0807cbb0(void)
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

    if (t->unk28 == 1)
        t->unk18 = 1;
    else
        t->unk18 = 0;
    v = TaskGetAngleTo(gCurTask->unk1C, 3);
    AngleToVector((v + 256) & 511, 102);
    gCurTask->velX = gUnk_030023B4;
    TaskSetFrameByFacing(9);
    TaskYieldTrampoline(12);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    gCurTask->unk18 = 1;
}

void ParasolRiseInit(void)
{
    gCurTask->updateCallback = (u32)ParasolRiseUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gParasolRiseStates);
}

void ParasolRiseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gParasolRiseStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->unk18 != 0)
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
    gCurTask->updateState = 0;
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

void sub_0807cd60(void)
{
}

void ParasolChaseInit(void)
{
    gCurTask->updateCallback = (u32)ParasolChaseUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    gCurTask->unk24 = 255;
    CallTableEntry(gCurTask->state, 1, gParasolChaseStates);
}

void ParasolChaseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gParasolChaseStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->unk18 != 0)
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
    gCurTask->updateState = 0;
    sub_0807cbf4();
    gCurTask->unk20 = 15;
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

void sub_0807ce68(void)
{
    struct Task *t;

    if (gCurTask->unk20 <= 0)
    {
        u16 v = TaskGetAngleToNearestPlayer(3);

        v = ((v + 32) & 511) >> 6;
        TaskAccelerateInDir(0x4D00, 0x18100, v);
        gCurTask->unk20 = 15;
    }
    t = gCurTask;
    t->unk20--;
    if (t->unk24 <= 0)
    {
        ActorSetHitReactions(gUnk_08740FA4);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    gCurTask->unk24--;
}

void ParasolIdleInit(void)
{
    gCurTask->updateCallback = (u32)ParasolIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
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
    gCurTask->updateState = 0;
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

void sub_0807cfcc(void)
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
    ActorSetState(0);
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

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08741094[gCurTask->unk74], 0x5A5A5A5A);
    t = gCurTask;
    t->velY = gUnk_087410A0[t->unk74];
    t->unk6C = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk1C = 1;
    } while ((s16)++gCurTask->unk6C <= 4);
    ActorDestroy();
}

void sub_0807d178(void)
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

s32 sub_0807d1bc(void)
{
    ActorSetState(1);
    TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0807d1dc(void)
{
    ActorSetState(1);
    TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    return 1;
}

void UFOLaserInit(void)
{
    gCurTask->updateCallback = (u32)UFOLaserUpdate;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUFOLaserStates);
}

void UFOLaserUpdate(void)
{
    if ((u8)sub_08069604() != 0)
    {
        ActorSetState(1);
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
    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x80000, 0x5A5A5A5A);
    gCurTask->frame = 4;
    TaskSleepForever();
}

void sub_0807d2ec(void)
{
    if ((u8)(gTerrainResult[4] - 1) <= 3)
    {
        TaskStop();
        ActorSetState(1);
        TaskSetEntry(UFOLaserEnterState, gCurTaskIdx);
    }
}

void UFOLaserState1(void)
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
    t->unk18 = 0;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}
