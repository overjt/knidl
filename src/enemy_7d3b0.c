/* game_code_and_rodata 0x0807D3B0-0x0807F044 (issue #77, module M20 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807D3B0 0x0807F044 src/enemy_7d3b0.c --newpb
 *
 * The last third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Mostly "moving scenery": scripts
 * that drive Task.unk54/unk58 (the 16.16 x/y velocity pair) and Task.unk60
 * from ROM tables, then wait on the collision flag Task.unk7A:
 *   * the two class-8 platform intros `sub_0807d490` and `sub_0807d510`
 *     (the latter a seventeen-step cue script over `0x087410C0`);
 *   * task #216's rider `sub_0807d6c4`+`sub_0807d718`, whose per-frame
 *     handlers `sub_0807d82c` / `sub_0807d918` re-centre on the nearest
 *     player when `|TaskGetNearestPlayerDx()| <= 49` and `|TaskGetDyTo()| <= 15`;
 *   * the class-3 three-way branch pair `sub_0807dd70` / `sub_0807dddc`
 *     (`switch (Task.unk73)` with an empty `case 1`);
 *   * the `0x08741220` function-pointer table the three `sub_0807e244` /
 *     `sub_0807e3b0` hooks dispatch through;
 *   * the two 0x120/0xA8-byte cutscene coroutines `sub_0807e290` and
 *     `sub_0807e768`, which spawn a companion with `CreateActorFromDescAtOffsetFacing` and then
 *     bounce between velocity presets until Task.unk7A fires;
 *   * the swing/orbit loops `sub_0807ea84`, `sub_0807eb60`, `sub_0807ec4c`,
 *     `sub_0807ed20` and `sub_0807ef7c`, each an infinite eight-step ramp.
 *
 * `sub_0807daf0`, `sub_0807e428`, `sub_0807e5a0`, `sub_0807e904`,
 * `sub_0807ee44` and `sub_0807ef08` are dead exports: each is a copy of its
 * host's tail dispatch that nothing in the ROM references (curated in
 * tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02007FB8[];
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern vs16 gTaskSlotTypes[];

/* ROM tables */
extern struct AnimCmd gUnk_087412A8[];
extern struct AnimCmd gUnk_087412EC[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_087410C0[];
extern u32 gUnk_087410D8[];
extern u32 gUnk_08741158[];
extern u32 gUnk_08741174[];
extern u32 gUnk_087411C0[];
extern u32 gUnk_087411CC[];
extern u32 gUnk_087411E0[];
extern u32 gUnk_087411F4[];
extern u32 gUnk_087411F8[];
extern u32 gUnk_087411FC[];
extern u32 gUnk_08741208[];
extern u32 gUnk_08741218[];
extern u32 gUnk_08741220[];
extern u32 gUnk_08741228[];
extern u32 gUnk_08741234[];
extern u32 gUnk_08741240[];
extern u32 gUnk_0874124C[];
extern u32 gUnk_08741258[];
extern u32 gUnk_08741264[];
extern u32 gUnk_08741268[];
extern u32 gUnk_0874126C[];
extern u32 gUnk_08741278[];
extern u32 gUnk_0874127C[];
extern u32 gUnk_08741280[];
extern u32 gUnk_08741288[];
extern u32 gUnk_08741290[];
extern u32 gUnk_08741294[];
extern u32 gUnk_08741298[];
extern u32 gUnk_087412A0[];
extern u32 gUnk_087412BC[];
extern u32 gUnk_087412CC[];
extern u32 gUnk_087412D0[];
extern u32 gUnk_087412D4[];
extern u32 gUnk_087412D8[];
extern u32 gUnk_087412DC[];
extern u32 gUnk_087412E0[];
extern u32 gUnk_087412E4[];
extern u32 gUnk_087412E8[];
extern u32 gUnk_08741300[];
extern u32 gUnk_08741308[];
extern u32 gUnk_0874130C[];
extern u32 gUnk_08741310[];
extern u32 gUnk_08741314[];
extern u32 gUnk_08741380[];
extern u32 gUnk_0874183C[];
extern u32 gUnk_08752108[];
extern u32 gUnk_08752194[];
extern u32 gUnk_08752234[];
extern u32 gUnk_0875230C[];
extern u32 gUnk_0875235C[];
extern u32 gUnk_0875237C[];
extern u32 gUnk_087523EC[];
extern u32 gUnk_08752438[];
extern u8 gUnk_08741214[];
extern u8 gUnk_08741216[];

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
void sub_0807d718(void);
void sub_0807db0c(void);
void sub_0807db9c(void);
void sub_0807dd10(void);
void sub_0807df30(void);
void sub_0807e060(void);
void sub_0807e444(void);
void sub_0807e484(void);
void sub_0807e5bc(void);
void sub_0807e730(void);
void sub_0807e920(void);
void sub_0807ea60(void);
void sub_0807eb20(void);
void sub_0807ec10(void);
void sub_0807ecfc(void);
void sub_0807ee60(void);
void sub_0807ef24(void);
void sub_0807ef7c(void);

void sub_0807d3b0(void)
{
    s32 v;

    if (gTaskSlotTypes[gCurTask->unk44] == -1)
        goto kill1;
    v = (u8)TaskHasSameSerial(gCurTask->unk44);
    if (v != 1)
        goto kill1;
    {
        struct Task *t = gCurTask;
        struct Task *o = &gTasks[t->unk44];
        s16 *s;

        if ((u16)(o->unk76 - 28) > 1)
            goto kill2;
        s = &o->unk48;
        t->unk48 = o->unk2C * 20 + *s;
        t->unk4A = o->unk4A;
        if (t->unk18 != 0)
        {
            ActorCheckHitsWithBox(gUnk_08741174);
            return;
        }
    }
    if (ActorCheckHitsWithBox(gUnk_08741158) != 0)
    {
        if (gCurTask->unk7C == 6)
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

void sub_0807d490(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    t = gCurTask;
    t->unk38 = gUnk_08752234;
    t->unk40 = (0xFFF & t->unk40) | 0xF000;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0x30000, -0x5000);
    {
        struct Task *u = gCurTask;

        u->unk60 = -0x4000;
        u->frame = 2;
    }
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}

void sub_0807d510(void)
{
    u32 a;
    u32 b;
    s32 i;
    s32 j;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    {
        struct Task *t = gCurTask;

        t->unk38 = gUnk_08752234;
        t->unk40 = (0xFFF & t->unk40) | 0xF000;
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

        t->unk58 = b;
        t->frame = -1;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->unk58 = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->unk58 = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(a, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->unk58 = b;
        t->frame = 4;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    {
        struct Task *t = gCurTask;

        t->unk58 = gUnk_087410D8[t->unk74];
        t->frame--;
    }
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0807d684(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->unk38 = gUnk_08752194;
    CallTableEntry(gCurTask->unk73, 3, gUnk_087411C0);
}

void sub_0807d6c4(void)
{
    gCurTask->unk04 = (u32)sub_0807d718;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 5, gUnk_087411CC);
}

void sub_0807d6fc(void)
{
    CallTableEntry(gCurTask->unk14, 5, gUnk_087411CC);
}

void sub_0807d718(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 5, gUnk_087411E0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807d748(void)
{
    gCurTask->unk15 = 0;
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

            t->unk54 = 0;
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

            t->unk54 = 0;
            t->frame++;
        }
        TaskYieldTrampoline(8);
    } while ((s16)++gCurTask->unk6C <= 1);
    gCurTask->unk54 = 0;
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
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807d8f8(void)
{
    gCurTask->unk15 = 1;
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
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807d9ac(void)
{
    gCurTask->unk7A = 0;
    gCurTask->unk15 = 2;
    TaskSetMotionXFacing(0x18000, 0);
    {
        struct Task *t = gCurTask;

        t->unk58 = -0x2E800;
        t->unk60 = 0x2000;
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
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807da30(void)
{
    gCurTask->unk15 = 3;
    TaskSetFrame(12);
    TaskStop();
    TaskYieldTrampoline(16);
    ActorSetState(4);
    TaskSleepForever();
}

void sub_0807da5c(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807da84(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk58 = 0x80000;
    TaskSetFrame(12);
    TaskSleepForever();
}

void sub_0807daa8(void)
{
}

void sub_0807daac(void)
{
    gCurTask->unk04 = (u32)sub_0807db0c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087411F4);
}

void sub_0807daf0(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087411F4);
}

void sub_0807db0c(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087411F8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807db30(void)
{
    gCurTask->unk15 = 0;
    while (1)
        sub_0807dd10();
}

void sub_0807db44(void)
{
}

void sub_0807db48(void)
{
    gCurTask->unk04 = (u32)sub_0807db9c;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087411FC);
}

void sub_0807db80(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087411FC);
}

void sub_0807db9c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08741208);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807dbcc(void)
{
    gCurTask->unk15 = 0;
    {
        struct Task *t = gCurTask;

        t->unk54 = 0;
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
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_0807db80, gCurTaskIdx);
}

void sub_0807dca0(void)
{
    gCurTask->unk15 = 1;
    sub_0807dd10();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807dcc0(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_0807db80, gCurTaskIdx);
}

void sub_0807dce8(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk58 = 0x80000;
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

        t->unk54 = 0;
        t->unk58 = 0;
    }
    RequestScreenShake(1);
    PlaySfx(163);
    r = 0;
    switch (gCurTask->unk73)
    {
    case 0:
        ActorSetState(1);
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(1);
        TaskSetEntry(sub_0807db80, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807dddc(void)
{
    s32 r = 0;

    switch (gCurTask->unk73)
    {
    case 0:
        ActorSetState(4);
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(sub_0807db80, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807de30(void)
{
    s32 r = 0;

    if (gCurTask->unk73 == 0)
    {
        ActorSetState(3);
        TaskSetEntry(sub_0807d6fc, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_0807de64(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 == 2)
        t->unk54 = 0;
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_0807de88(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807de98(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->unk38 = gUnk_0875230C;
    TaskFaceNearestPlayer();
    {
        struct Task *t = gCurTask;

        t->unk2C = 0;
        CallTableEntry(t->unk73, 3, gUnk_08741228);
    }
}

void sub_0807dee4(void)
{
    gCurTask->unk04 = (u32)sub_0807df30;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741234);
}

void sub_0807df14(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08741234);
}

void sub_0807df30(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08741240);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807df60(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
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
        gCurTask->unk54 = 0;
        TaskSetEntry(sub_0807df14, gCurTaskIdx);
    }
}

void sub_0807e014(void)
{
    gCurTask->unk04 = (u32)sub_0807e060;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874124C);
}

void sub_0807e044(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874124C);
}

void sub_0807e060(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08741258);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e090(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
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
        gCurTask->unk54 = 0;
        TaskSetEntry(sub_0807e044, gCurTaskIdx);
    }
}

void sub_0807e188(void)
{
    struct ActorSpawn spawn;

    gCurTask->unk15 = 1;
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
    spawn.unk08 = 0;
    spawn.unk09 = 0;
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

        if (t->unk7A != 0 && t->unk28 > 0)
            t->unk28--;
    }
    {
        struct Task *t = gCurTask;

        if (t->unk14 != 1)
            TaskSetEntry((void *)gUnk_08741220[t->unk73], gCurTaskIdx);
    }
}

void sub_0807e290(void)
{
    struct ActorSpawn spawn;

    gCurTask->unk15 = 2;
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
    spawn.unk08 = 0;
    spawn.unk09 = 1;
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

        if (t->unk7A != 0)
        {
            struct Task *u;

            t->unk7A = 0;
            u = gCurTask;
            u->unk58 = -0x40000;
            u->unk60 = 0x4000;
            while (gCurTask->unk7A == 0)
                TaskYieldTrampoline(1);
        }
    }
    {
        struct Task *t = gCurTask;

        t->unk58 = 0;
        t->unk60 = 0;
        t->facing = t->unk30;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0807e3b0(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 2)
        TaskSetEntry((void *)gUnk_08741220[t->unk73], gCurTaskIdx);
}

void sub_0807e3e4(void)
{
    gCurTask->unk04 = (u32)sub_0807e444;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741264);
}

void sub_0807e428(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741264);
}

void sub_0807e444(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08741268);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e468(void)
{
    gCurTask->unk15 = 0;
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
    TaskSetMotionX(-t->unk54, -t->unk5C, t->unk64);
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

void sub_0807e530(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    CallTableEntry(gCurTask->unk73, 3, gUnk_0874126C);
}

void sub_0807e568(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0807e5bc;
    t->unk38 = gUnk_0875235C;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741278);
}

void sub_0807e5a0(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741278);
}

void sub_0807e5bc(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_0874127C);
    ActorCheckHits();
    {
        struct Task *t = gCurTask;

        if (t->unk7C == 3)
        {
            gCurTask->unk46 = CreateActorByKind(0, 8, 1, 0, t->unk48, t->unk4A,
                                                t->unk40);
            sub_080b5540(gCurTaskIdx, gCurTask->unk46);
        }
    }
    ActorReactToHit();
}

void sub_0807e640(void)
{
    gCurTask->unk15 = 0;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->unk28 = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        while (--gCurTask->unk28 >= 0)
        {
            gCurTask->unk7A = 0;
            {
                struct Task *u = gCurTask;

                u->unk58 = -0x10000;
                u->unk60 = 0x1000;
            }
            while (gCurTask->unk7A == 0)
                TaskYieldTrampoline(1);
            gCurTask->unk3E ^= 0x8000;
        }
    }
}

void sub_0807e6d0(void)
{
}

void sub_0807e6d4(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0807e730;
    t->unk38 = gUnk_0875237C;
    ActorLoadDef(gUnk_0874183C);
    ActorSetState(1);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08741280);
}

void sub_0807e714(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08741280);
}

void sub_0807e730(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 2, gUnk_08741288);
    if (gCurTask->unk15 != 1)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e768(void)
{
    gCurTask->unk15 = 0;
    TaskSetFrame(4);
    while (1)
    {
        gCurTask->unk28 = 40;
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        TaskSetFrame((s16)gCurTask->frame);
        while (--gCurTask->unk28 >= 0)
        {
            gCurTask->unk7A = 0;
            {
                struct Task *u = gCurTask;

                u->unk58 = -0x10000;
                u->unk60 = 0x1000;
            }
            while (gCurTask->unk7A == 0)
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
    gCurTask->unk15 = 1;
    TaskFaceNearestPlayer();
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(-0x8000, 0);
    {
        struct Task *t = gCurTask;

        t->unk58 = -0x20000;
        t->unk60 = 0x2000;
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
    if (gCurTask->unk7A != 0)
    {
        ActorSetState(0);
        TaskSetEntry(sub_0807e714, gCurTaskIdx);
    }
}

void sub_0807e8b8(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0807e920;
    t->unk38 = gUnk_0875235C;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741290);
}

void sub_0807e904(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741290);
}

void sub_0807e920(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_08741294);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807e950(void)
{
    gCurTask->unk15 = 0;
    gCurTask->frame = 4;
    while (1)
    {
        gCurTask->unk7A = 0;
        {
            struct Task *t = gCurTask;

            t->unk58 = -0x10000;
            t->unk60 = 0x1000;
        }
        while (gCurTask->unk7A == 0)
            TaskYieldTrampoline(1);
        gCurTask->unk3E ^= 0x8000;
    }
}

void sub_0807e9b0(void)
{
}

s32 sub_0807e9b4(void)
{
    struct Task *t = gCurTask;

    t->unk54 = -t->unk54;
    return 0;
}

s32 sub_0807e9c8(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807e9d8(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    {
        struct Task *t = gCurTask;

        t->unk38 = gUnk_08752108;
        t->unk7A = 0;
    }
    TaskFaceNearestPlayer();
    gCurTask->unk28 = ActorStartAnim(gUnk_087412A8);
    CallTableEntry(gCurTask->unk73, 4, gUnk_087412BC);
}

void sub_0807ea30(void)
{
    gCurTask->unk04 = (u32)sub_0807ea60;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087412CC);
}

void sub_0807ea60(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087412D0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ea84(void)
{
    gCurTask->unk15 = 0;
    while (1)
    {
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807ead4(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807eaf0(void)
{
    gCurTask->unk04 = (u32)sub_0807eb20;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087412D4);
}

void sub_0807eb20(void)
{
    if ((u8)sub_0806956c() == 1)
    {
        struct Task *t = gCurTask;

        t->unk58 = -t->unk58;
        t->unk7A = 0;
    }
    CallTableEntry(gCurTask->unk15, 1, gUnk_087412D8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807eb60(void)
{
    gCurTask->unk15 = 0;
    {
        struct Task *t = gCurTask;

        t->unk58 = gUnk_08741298[t->unk74];
    }
    while (1)
    {
        gCurTask->unk54 = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ebc4(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807ebe0(void)
{
    gCurTask->unk04 = (u32)sub_0807ec10;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087412DC);
}

void sub_0807ec10(void)
{
    if ((u8)sub_080695bc() == 1)
    {
        struct Task *t = gCurTask;

        t->unk54 = -t->unk54;
    }
    CallTableEntry(gCurTask->unk15, 1, gUnk_087412E0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ec4c(void)
{
    gCurTask->unk15 = 0;
    {
        struct Task *t = gCurTask;

        t->unk54 = gUnk_087412A0[t->unk74];
    }
    while (1)
    {
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ecb0(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807eccc(void)
{
    gCurTask->unk04 = (u32)sub_0807ecfc;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087412E4);
}

void sub_0807ecfc(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087412E8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ed20(void)
{
    gCurTask->unk15 = 0;
    while (1)
    {
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(96);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = 0xC000;
        TaskYieldTrampoline(96);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(16);
    }
}

void sub_0807ed98(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807edb4(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->unk38 = gUnk_087523EC;
    AcquirePaletteAnim(0, 0);
    gCurTask->facing = 255;
    gCurTask->unk28 = ActorStartAnim(gUnk_087412EC);
    CallTableEntry(gCurTask->unk73, 2, gUnk_08741300);
}

void sub_0807ee14(void)
{
    gCurTask->unk04 = (u32)sub_0807ee60;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741308);
}

void sub_0807ee44(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741308);
}

void sub_0807ee60(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_0874130C);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ee84(void)
{
    gCurTask->unk15 = 0;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_0807ef7c();
}

void sub_0807eea8(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

void sub_0807eec4(void)
{
    gCurTask->unk04 = (u32)sub_0807ef24;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741310);
}

void sub_0807ef08(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741310);
}

void sub_0807ef24(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08741314);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0807ef48(void)
{
    gCurTask->unk15 = 0;
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
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807efec(void)
{
    gUnk_02007FB8[0]--;
}

void sub_0807effc(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->unk38 = gUnk_08752438;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->unk73, 4, gUnk_08741380);
}
