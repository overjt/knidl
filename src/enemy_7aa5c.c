/* game_code_and_rodata 0x0807AA5C-0x0807D3B0 (issue #77, module M20 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807AA5C 0x0807D3B0 src/enemy_7aa5c.c --newpb
 *
 * The middle third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Twelve more scripts, among them:
 *   * the class-6 "swinging platform" pair `sub_0807aab8`+`sub_0807aae4` with
 *     its five-state `sub_0807a8fc` dispatch and `sub_0807aa5c`'s two-table
 *     velocity ramp;
 *   * task #12's eight-state script (`sub_0807b300`+`sub_0807b32c`) whose hook
 *     packs Task.pixelY into the low half of Task.unk24 and ORs 0x10000 in
 *     when the four-player flag `gTerrainResult[4]` is out of range;
 *   * the eight-state class-8 rider `sub_0807b3f8`..`sub_0807b8ec`, which
 *     probes for a partner with `TaskIsInRectSlot` over a stack `struct PointPair`
 *     and reacts through the shared `sub_0807b294` state entry;
 *   * the class-4 "conveyor" script `sub_0807c684`+`sub_0807c6b0` with the
 *     `sub_0807c5ac` box test (`struct Rect` + GetDistSq) and the
 *     `sub_0807c530` aim helper that clamps into `0x08740B3C`/`0x08740B60`;
 *   * `sub_0807c828`, which walks a sixteen-entry cue ring through
 *     `sub_0807c508` (`15 & (rand + Task.unk24)`);
 *   * the class-1 lift `sub_0807cc68`+`sub_0807cc9c` and the `sub_0807cbf4`
 *     spin-up (a 512-step angle from `TaskGetAngleTo`).
 *
 * `sub_0807b888`, `sub_0807bd60`, `sub_0807c3d8`, `sub_0807cadc`,
 * `sub_0807ccec`, `sub_0807cdec`, `sub_0807cf64` and `sub_0807d0d8` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gUnk_0300244C;
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s8 gUnk_02007FB8[];
extern struct Task *gCurTask;
extern u8 gTerrainResult[];
extern vs16 gTaskSlotTypes[];

/* ROM tables */
extern s16 gUnk_0874094C[];
extern s16 gUnk_08740B3C[];
extern s16 gUnk_08740B60[];
extern struct AnimCmd gUnk_08740854[];
extern struct AnimCmd gUnk_0874093C[];
extern struct AnimCmd gUnk_0874099C[];
extern struct AnimCmd gUnk_087409C0[];
extern struct AnimCmd gUnk_08740A98[];
extern struct AnimCmd gUnk_08740AAC[];
extern u32 gUnk_0825B350[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F720[];
extern u32 gUnk_08740864[];
extern u32 gUnk_087408AC[];
extern u32 gUnk_08740934[];
extern u32 gUnk_08740950[];
extern u32 gUnk_08740960[];
extern u32 gUnk_08740978[];
extern u32 gUnk_08740990[];
extern u32 gUnk_087409E4[];
extern u32 gUnk_087409FC[];
extern u32 gUnk_08740A1C[];
extern u32 gUnk_08740A3C[];
extern u32 gUnk_08740A40[];
extern u32 gUnk_08740A44[];
extern u32 gUnk_08740A5C[];
extern u32 gUnk_08740A74[];
extern u32 gUnk_08740A78[];
extern u32 gUnk_08740A80[];
extern u32 gUnk_08740A88[];
extern u32 gUnk_08740A90[];
extern u32 gUnk_08740AC8[];
extern u32 gUnk_08740AE0[];
extern u32 gUnk_08740AF8[];
extern u32 gUnk_08740AFC[];
extern u32 gUnk_08740B00[];
extern u32 gUnk_08740B08[];
extern u32 gUnk_08740B84[];
extern u32 gUnk_08740B94[];
extern u32 gUnk_08740BA4[];
extern u32 gUnk_08740BA8[];
extern u32 gUnk_08740BAC[];
extern u32 gUnk_08740BBC[];
extern u32 gUnk_08740BC0[];
extern u32 gUnk_08740BC4[];
extern u32 gUnk_08740BC8[];
extern u32 gUnk_08740BCC[];
extern u32 gUnk_08740BD0[];
extern u32 gUnk_08740E38[];
extern u32 gUnk_08740F50[];
extern u32 gUnk_08740F5C[];
extern u32 gUnk_08740FA4[];
extern u32 gUnk_08741088[];
extern u32 gUnk_0874108C[];
extern u32 gUnk_08741090[];
extern u32 gUnk_08741094[];
extern u32 gUnk_087410A0[];
extern u32 gUnk_087410AC[];
extern u32 gUnk_087410B0[];
extern u32 gUnk_087410B8[];
extern u32 gUnk_0874CB5C[];
extern u32 gUnk_08752234[];
extern u32 gUnk_08752680[];
extern u32 gUnk_087529D8[];
extern u32 gUnk_08752A24[];
extern u32 gUnk_08752B08[];
extern u32 gUnk_08752B4C[];
extern u32 gUnk_08752B8C[];
extern u32 gUnk_08752BA8[];

/* Externals */
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 RandomRange(s32 a);
extern s32 PlaySfx(s32 id);
extern s32 sub_08021a40(s32 x, s32 y);
extern s32 sub_08063a2c(void);
extern s32 GetDistSq(struct PointPair *p);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
extern s32 ActorStartAnimNoFlip(u32 *p);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern s32 ActorTickAnim(s32 n);
extern s32 GetPointAngle(s16 x0, s16 y0, s16 x1, s16 y1, s32 mode);
extern s32 TaskGetAngleToNearestPlayer(s32 prec);
extern s32 sub_08064984();
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 a, u32 b);
extern s32 sub_08066338();
extern s32 ActorReactToHit(void);
extern u16 TaskGetAngleTo(s32 a, s32 b);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorDie(void);
extern u32 ActorAttachToHitter(void);
extern u8 sub_08069604(void);
extern u8 sub_080699a8(void);
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetFrameByFacing(s16 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(u32 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void TaskGetNearestPlayerPos(void);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void ActorStopAnim(void);
extern void AngleToVector(s16 t, s16 mag);
extern void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern void TaskFaceLikeParent(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void SetPaletteAnimSource(u32 i, u32 p1, u8 p2);
extern void AcquirePaletteAnim(u32 p0, s32 idx);
extern void sub_0806a0f0(s32 a);
extern void sub_0806ee2c(void);

/* Forward declarations */
extern s32 sub_0807a8d4();
extern s32 sub_0807a8fc();
extern s32 sub_0807a968();
extern s32 sub_0807aa0c();
void sub_0807aae4(void);
void sub_0807b32c(void);
void sub_0807b3dc(void);
void sub_0807b844(void);
void sub_0807b918(void);
void sub_0807bd14(void);
void sub_0807c0ac(void);
void sub_0807c0fc(void);
void sub_0807c394(void);
s32 sub_0807c5ac(struct Rect *r);
void sub_0807c6b0(void);
void sub_0807ca98(void);
void sub_0807cc9c(void);
void sub_0807cd9c(void);
void sub_0807cf20(void);
void sub_0807cff0(void);
void sub_0807d094(void);
void sub_0807d230(void);
void sub_0807d29c(void);
extern s32 sub_0807d3b0();

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

void sub_0807aab8(void)
{
    gCurTask->updateCallback = (u32)sub_0807aae4;
    sub_0807a8d4();
    CallTableEntry(gCurTask->state, 6, gUnk_08740960);
}

void sub_0807aae4(void)
{
    CallTableEntry(gCurTask->updateState, 6, gUnk_08740978);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->state != 0)
    {
        ActorCheckHits();
        ActorReactToHit();
        sub_0807aa0c();
    }
}

void sub_0807ab38(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_08740960);
}

void sub_0807ab54(void)
{
    gCurTask->updateState = 2;
    sub_0807a8fc();
    sub_0807aa5c();
}

void sub_0807ab70(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
}

void sub_0807ab8c(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_0807abb4(void)
{
    if (sub_08064984(10) != 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0807ab38, gCurTaskIdx);
    }
}

void sub_0807abdc(void)
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
        TaskSetEntry(sub_0807ab38, gCurTaskIdx);
    }
}

void sub_0807ac58(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 3;
    TaskStop();
    gCurTask->frameTable = gUnk_08752680;
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

void sub_0807adcc(void)
{
    struct Task *t = gCurTask;

    if (t->state != 3)
    {
        TaskSetEntry(sub_0807ab38, gCurTaskIdx);
    }
    else if (--t->unk28 <= 0)
    {
        t->unk2C ^= 1;
        t->velX = gUnk_08740934[t->unk2C] * t->unk30;
        t->unk28 = 2;
    }
}

void sub_0807ae1c(void)
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

void sub_0807ae7c(void)
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
        TaskSetEntry(sub_0807ab38, gCurTaskIdx);
    }
    else if ((n & 7) == 0)
    {
        TaskAccelerateTowardNearestPlayer(t->unk2C, t->unk30);
    }
}

void sub_0807aecc(void)
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

void sub_0807af3c(void)
{
    gCurTask->unk34 = ActorTickAnimFacingNearestPlayer(gCurTask->unk34);
}

void sub_0807af58(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_087529D8;
    CallTableEntry(gCurTask->unk73, 3, gUnk_08740990);
}

void sub_0807af98(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752A24;
    CallTableEntry(gCurTask->unk73, 3, gUnk_08740990);
}

s32 sub_0807afd8(void)
{
    if (gCurTask->unk73 != 0)
        return 0;
    ActorSetState(7);
    TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
    return 1;
}

s32 sub_0807b010(void)
{
    if (gCurTask->unk73 != 0)
        return 0;
    ActorSetState(0);
    TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
    return 1;
}

s32 sub_0807b048(void)
{
    sub_0806a0f0(-2);
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

    t->unk8C->animScript = 0;
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
    if (gCurTask->unk8C->animScript == 0)
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

void sub_0807b1e4(void)
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
            TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
        }
    }
}

void sub_0807b294(void *fn)
{
    struct Actor *a = gCurTask->unk8C;
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

void sub_0807b300(void)
{
    gCurTask->updateCallback = (u32)sub_0807b32c;
    sub_0807b11c();
    CallTableEntry(gCurTask->state, 8, gUnk_087409FC);
}

void sub_0807b32c(void)
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
        CallTableEntry(gCurTask->updateState, 8, gUnk_08740A1C);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) | gCurTask->pixelY;
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807b3dc(void)
{
    CallTableEntry(gCurTask->state, 8, gUnk_087409FC);
}

void sub_0807b3f8(void)
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
        sub_0807b294(sub_0807b3dc);
    else
        sub_0807b200();
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk18 == 0)
    {
        if ((t->waterFlags & 1) != 0)
            sub_0806a0f0(-2);
        gCurTask->unk18 = 1;
    }
}

void sub_0807b49c(void)
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b4f0(void)
{
    gCurTask->updateState = 3;
    sub_0807b0f0();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void sub_0807b584(void)
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b5d8(void)
{
    gCurTask->updateState = 4;
    sub_0807b0f0();
    gCurTask->frame = 10;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void sub_0807b66c(void)
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b6e8(void)
{
    gCurTask->updateState = 6;
    TaskStop();
    sub_0807b0f0();
    sub_0807b1e4();
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
        TaskSetEntry(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b7d0(void)
{
    gCurTask->updateState = 7;
    TaskStop();
    TaskSetMotionY(0, 0x1500, 0x30000);
    sub_0807b144();
    TaskSleepForever();
}

void sub_0807b7fc(void)
{
}

void sub_0807b800(void)
{
    gCurTask->updateCallback = (u32)sub_0807b844;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740A3C);
}

void sub_0807b844(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740A40);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807b888(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740A3C);
}

void sub_0807b8a4(void)
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

void sub_0807b8ec(void)
{
    gCurTask->updateCallback = (u32)sub_0807b918;
    sub_0807b11c();
    CallTableEntry(gCurTask->state, 6, gUnk_08740A44);
}

void sub_0807b918(void)
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
        CallTableEntry(gCurTask->updateState, 6, gUnk_08740A5C);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) | gCurTask->pixelY;
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807b9c8(void)
{
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 6, gUnk_08740A44);
}

void sub_0807b9ec(void)
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
        sub_0807b294(sub_0807b9c8);
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk18 == 0)
    {
        if ((t->waterFlags & 1) != 0)
            sub_0806a0f0(-2);
        gCurTask->unk18 = 1;
    }
}

void sub_0807ba7c(void)
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
        TaskSetEntry(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bad0(void)
{
    gCurTask->updateState = 3;
    sub_0807b0f0();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
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
        TaskSetEntry(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bb60(void)
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
        TaskSetEntry(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bbb4(void)
{
    gCurTask->updateState = 4;
    sub_0807b0f0();
    gCurTask->frame = 10;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
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
        TaskSetEntry(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bc44(void)
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
        TaskSetEntry(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bcac(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_0874CB5C;
    CallTableEntry(gCurTask->unk73, 1, gUnk_08740A74);
}

void sub_0807bcec(void)
{
    gCurTask->updateCallback = (u32)sub_0807bd14;
    TaskFaceNearestPlayer();
    gCurTask->frame = 4;
    TaskSleepForever();
}

void sub_0807bd14(void)
{
    ActorAttachToHitter();
}

void sub_0807bd20(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752B08;
    CallTableEntry(gCurTask->unk73, 2, gUnk_08740A78);
}

void sub_0807bd60(void)
{
    TaskFaceNearestPlayer();
    TaskSetEntry(sub_0807c0fc, gCurTaskIdx);
}

void sub_0807bd7c(void)
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
    TaskSetEntry(sub_0807c0fc, gCurTaskIdx);
    return 1;
}

s32 sub_0807be08(void)
{
    struct Task *t = gCurTask;
    struct Actor *a = t->unk8C;
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
        TaskSetEntry(sub_0807c0fc, gCurTaskIdx);
    return 1;
}

s32 sub_0807be9c(void)
{
    sub_0806a0f0(-2);
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

void sub_0807c080(void)
{
    gCurTask->updateCallback = (u32)sub_0807c0ac;
    sub_0807bd7c();
    CallTableEntry(gCurTask->state, 6, gUnk_08740AC8);
}

void sub_0807c0ac(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 6, gUnk_08740AE0);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807c0fc(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_08740AC8);
}

void sub_0807c118(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    sub_0807bf0c();
    TaskSleepForever();
}

void sub_0807c138(void)
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
        TaskSetEntry(sub_0807c0fc, gCurTaskIdx);
        return;
    }
    gCurTask->unk1C = gUnk_08740A88[gCurTask->unk74];
    return;

dec:
    t->unk18--;
}

void sub_0807c1d0(void)
{
    gCurTask->updateState = 1;
    TaskSetMotionY(0, 0x2500, 0x30000);
    TaskSleepForever();
}

void sub_0807c1f4(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_0807c210(void)
{
    gCurTask->updateState = 2;
    sub_0807bf74();
    TaskSleepForever();
}

void sub_0807c22c(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_0807c248(void)
{
    gCurTask->updateState = 3;
    sub_0807bfd0();
    TaskSleepForever();
}

void sub_0807c264(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_0807c280(void)
{
    gCurTask->updateState = 4;
    sub_0807c000();
    TaskSleepForever();
}

void sub_0807c29c(void)
{
    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
}

void sub_0807c2b8(void)
{
    gCurTask->updateState = 5;
    sub_0807c030();
    while (1)
    {
        sub_0806cc90(1, 1, 4, 4);
        TaskYieldTrampoline(8);
    }
}

void sub_0807c2e0(void)
{
    struct Task *t;

    gCurTask->unk34 = ActorTickAnim(gCurTask->unk34);
    t = gCurTask;
    if (t->unk1C <= 0)
    {
        if (--t->unk20 <= 0)
        {
            ActorSetState(0);
            TaskSetEntry(sub_0807c0fc, gCurTaskIdx);
        }
    }
    else if (--t->unk1C <= 0)
    {
        TaskTurnAroundAndReverseX();
        TaskSetMotionXFacing(gUnk_08740A90[gCurTask->unk74], 0x5A5A5A5A);
    }
}

void sub_0807c350(void)
{
    gCurTask->updateCallback = (u32)sub_0807c394;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740AF8);
}

void sub_0807c394(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740AFC);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807c3d8(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740AF8);
}

void sub_0807c3f4(void)
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

void sub_0807c444(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752B4C;
    CallTableEntry(gCurTask->unk73, 2, gUnk_08740B00);
}

void sub_0807c484(void)
{
    if (--gUnk_02007FB8[0] < 0)
        sub_0806ee2c();
}

void sub_0807c4a0(void)
{
    struct Actor *a = gCurTask->unk8C;
    u32 v;

    AcquirePaletteAnim(1, 0);
    v = a->palette;
    if (v == 0)
        v = gUnk_0825B350[2];
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
    sub_0807c4a0();
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
    sub_0807c5ac(&box);
    gCurTask->unk34 = (u16)GetPointAngle(box.left, box.top,
                                             (s16)gCurTask->unk2C,
                                             (s16)gCurTask->unk28, 3);
}

s32 sub_0807c5ac(struct Rect *r)
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

void sub_0807c618(void)
{
    struct ActorSpawn spawn;
    struct Task *t = gCurTask;

    if (sub_08021a40(t->pixelX + t->facing * 16, t->pixelY) == 0)
    {
        spawn.subtype = 31;
        spawn.taskType = 134;
        spawn.unk08 = 0;
        spawn.unk09 = 0;
        spawn.x = 8;
        spawn.y = 0;
        spawn.checkTerrain = 1;
        gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 0);
        PlaySfx(165);
    }
}

void sub_0807c684(void)
{
    gCurTask->updateCallback = (u32)sub_0807c6b0;
    sub_0807c4d4();
    CallTableEntry(gCurTask->state, 4, gUnk_08740B84);
}

void sub_0807c6b0(void)
{
    CallTableEntry(gCurTask->updateState, 4, gUnk_08740B94);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807c6f4(void)
{
    CallTableEntry(gCurTask->state, 4, gUnk_08740B84);
}

void sub_0807c710(void)
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
        TaskSetEntry(sub_0807c6f4, gCurTaskIdx);
}

void sub_0807c80c(void)
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
    TaskSetEntry(sub_0807c6f4, gCurTaskIdx);
}

void sub_0807c8b0(void)
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
    if ((u8)sub_0807c5ac(&box) != 0)
    {
        struct Task *t = gCurTask;

        if (t->unk20 == 4)
        {
            t->unk20 = 0;
            ActorSetState(3);
            TaskSetEntry(sub_0807c6f4, gCurTaskIdx);
        }
        else
        {
            struct Actor *a = t->unk8C;

            if (sub_08021a40(t->pixelX, t->pixelY + ((s8 *)a->terrainBox)[2]) != 0)
                gCurTask->unk1C = 0;
            {
                struct Task *u = gCurTask;

                if (sub_08021a40(u->pixelX + ((s8 *)a->terrainBox)[5], u->pixelY) != 0)
                    gCurTask->unk1C = 0;
            }
            {
                struct Task *u = gCurTask;

                if (sub_08021a40(u->pixelX + ((s8 *)a->terrainBox)[4], u->pixelY) != 0)
                    gCurTask->unk1C = 0;
            }
            ActorSetState(0);
            TaskSetEntry(sub_0807c6f4, gCurTaskIdx);
        }
    }
}

void sub_0807c9b4(void)
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
    sub_0807c618();
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    TaskYieldTrampoline(20);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0807ca18(void)
{
    if (gCurTask->state != 3)
    {
        gCurTask->unk18 = ActorStartAnimNoFlip(gUnk_08740B08);
        TaskSetEntry(sub_0807c6f4, gCurTaskIdx);
    }
}

void sub_0807ca50(void)
{
    gCurTask->updateCallback = (u32)sub_0807ca98;
    TaskFaceNearestPlayer();
    sub_0807c4a0();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740BA4);
}

void sub_0807ca98(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740BA8);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807cadc(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740BA4);
}

void sub_0807caf8(void)
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

void sub_0807cbb4(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752BA8;
    CallTableEntry(gCurTask->unk73, 4, gUnk_08740BAC);
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

void sub_0807cc68(void)
{
    gCurTask->updateCallback = (u32)sub_0807cc9c;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740BBC);
}

void sub_0807cc9c(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740BC0);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->unk18 != 0)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807ccec(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740BBC);
}

void sub_0807cd08(void)
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

void sub_0807cd64(void)
{
    gCurTask->updateCallback = (u32)sub_0807cd9c;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    gCurTask->unk24 = 255;
    CallTableEntry(gCurTask->state, 1, gUnk_08740BC4);
}

void sub_0807cd9c(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740BC8);
    if (gTaskSlotTypes[gCurTaskIdx] != -1 && gCurTask->unk18 != 0)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807cdec(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740BC4);
}

void sub_0807ce08(void)
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

void sub_0807cedc(void)
{
    gCurTask->updateCallback = (u32)sub_0807cf20;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08740BCC);
}

void sub_0807cf20(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08740BD0);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807cf64(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08740BCC);
}

void sub_0807cf80(void)
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

void sub_0807cfd0(void)
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

void sub_0807d008(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    t = gCurTask;
    t->frameTable = gUnk_08752234;
    t->tileWord = (0xFFF & t->tileWord) | 0xF000;
    CallTableEntry(t->unk73, 1, gUnk_08741088);
}

void sub_0807d060(void)
{
    gCurTask->updateCallback = (u32)sub_0807d094;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_0874108C);
}

void sub_0807d094(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08741090);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807d0d8(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874108C);
}

void sub_0807d0f4(void)
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

void sub_0807d17c(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    t = gCurTask;
    t->frameTable = gUnk_08752B8C;
    CallTableEntry(t->unk73, 1, gUnk_087410AC);
}

s32 sub_0807d1bc(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_0807d29c, gCurTaskIdx);
    return 1;
}

s32 sub_0807d1dc(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_0807d29c, gCurTaskIdx);
    return 1;
}

void sub_0807d1fc(void)
{
    gCurTask->updateCallback = (u32)sub_0807d230;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_087410B0);
}

void sub_0807d230(void)
{
    if ((u8)sub_08069604() != 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0807d29c, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 2, gUnk_087410B8);
    }
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0807d29c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_087410B0);
}

void sub_0807d2b8(void)
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
        TaskSetEntry(sub_0807d29c, gCurTaskIdx);
    }
}

void sub_0807d320(void)
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

void sub_0807d388(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_0807d3b0;
    t->unk18 = 0;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}
