/* game_code_and_rodata 0x080860F8-0x08088000 (issue #80, module M23 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080860F8 0x08088000 src/enemy_860f8.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern u8 gTerrainResult[];

/* ROM tables */
extern s16 gUnk_08742150[];
extern s32 gUnk_08742088[];
extern s32 gUnk_08742090[];
extern s32 gUnk_08742098[];
extern s32 gUnk_087420A8[];
extern s32 gUnk_087420AC[];
extern s32 gUnk_087420F4[];
extern s32 gUnk_0874210C[];
extern s32 gUnk_087425B8[];
extern s32 gUnk_087425C0[];
extern s32 gUnk_087425C8[];
extern s32 gUnk_087425D8[];
extern s32 gUnk_087425DC[];
extern s32 gUnk_08742600[];
extern s32 gUnk_08742614[];
extern struct AnimCmd gUnk_087420C0[];
extern struct AnimCmd gUnk_087420D4[];
extern struct AnimCmd gUnk_08742144[];
extern struct AnimCmd gUnk_08742598[];
extern struct AnimCmd gUnk_087425A4[];
extern struct AnimCmd gUnk_08742634[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_08742080[];
extern u32 gUnk_08742084[];
extern u32 gUnk_087420A0[];
extern u32 gUnk_087420A4[];
extern u32 gUnk_087420BC[];
extern u32 gUnk_087420E8[];
extern u32 gUnk_087420F0[];
extern u32 gUnk_08742100[];
extern u32 gUnk_08742104[];
extern u32 gUnk_08742108[];
extern u32 gUnk_0874212C[];
extern u32 gUnk_08742138[];
extern u32 gUnk_08742570[];
extern u32 gUnk_087425B0[];
extern u32 gUnk_087425B4[];
extern u32 gUnk_087425D0[];
extern u32 gUnk_087425D4[];
extern u32 gUnk_087425EC[];
extern u32 gUnk_087425F0[];
extern u32 gUnk_087425F8[];
extern u32 gUnk_087425FC[];
extern u32 gUnk_0874260C[];
extern u32 gUnk_08742610[];
extern u32 gUnk_0874263C[];
extern u32 gUnk_08742648[];
extern u32 gUnk_08742654[];
extern u32 gUnk_08742660[];
extern u32 gUnk_0874266C[];
extern u32 gUnk_08752560[];

/* Externals */
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorStepAnim(void);
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 TaskGetCompassDirToNearestPlayer(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAroundAndReverseX(void);
extern void AngleToVector(s16 t, s16 mag);
extern void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void sub_080860d8(void);
void sub_08088024(void);

/* Defined below */
void sub_0808614c(void);
void sub_08086320(void);
void sub_0808659c(void);
void sub_080868dc(void);
void sub_08086a44(void);
void sub_08086c14(void);
void sub_08086f40(void);
s32 sub_08086f54(void);
void sub_08087118(void);
void sub_080872bc(void);
void sub_08087508(void);
void sub_08087848(void);
void sub_080879b0(void);
void sub_08087b44(void);
void sub_08087e84(void);

void sub_080860f8(void)
{
    gCurTask->unk04 = (u32)sub_0808614c;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08742080);
}

void sub_08086128(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_0808614c;
    CallTableEntry(t->unk14, 1, gUnk_08742080);
}

void sub_0808614c(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08742084);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086170(void)
{
    struct Task *t;
    s16 *p;

    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08742088[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->unk4A;
    if (*p < gTasks[TaskFindNearestPlayer()].unk4A)
    {
        t = gCurTask;
        t->unk58 = gUnk_08742090[0];
        t->unk60 = -gUnk_08742098[0];
    }
    else
    {
        t = gCurTask;
        t->unk58 = -gUnk_08742090[0];
        t->unk60 = gUnk_08742098[0];
    }
    t = gCurTask;
    t->unk30 = 4;
    t->unk34 = 40;
    while (1)
    {
        if (gCurTask->unk60 >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08086274(void)
{
    struct Task *t = gCurTask;
    s32 n;

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->unk60 < 0)
    {
        if (n < 0)
            return;
        t->unk58 = -gUnk_08742090[0];
        t->unk60 = gUnk_08742098[0];
    }
    else
    {
        t->unk58 = gUnk_08742090[0];
        t->unk60 = -gUnk_08742098[0];
    }
    gCurTask->unk34 = 40;
}

void sub_080862cc(void)
{
    gCurTask->unk04 = (u32)sub_08086320;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087420A0);
}

void sub_080862fc(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_08086320;
    CallTableEntry(t->unk14, 1, gUnk_087420A0);
}

void sub_08086320(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087420A4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086344(void)
{
    struct Task *t;
    s16 *p;

    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_08742088[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->unk4A;
    if (*p < gTasks[TaskFindNearestPlayer()].unk4A)
    {
        t = gCurTask;
        t->unk58 = gUnk_08742090[1];
        t->unk60 = -gUnk_08742098[1];
    }
    else
    {
        t = gCurTask;
        t->unk58 = -gUnk_08742090[1];
        t->unk60 = gUnk_08742098[1];
    }
    gCurTask->unk34 = 40;
    while (1)
    {
        if (gCurTask->unk60 >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08086444(void)
{
    s32 d, a;

    if (--gCurTask->unk34 == 20)
    {
        d = (s16)TaskGetNearestPlayerDy();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gCurTask->unk60 < 0)
                    gCurTask->unk60 = 0xFFFFE000;
                else
                    gCurTask->unk60 = 0xD00;
            }
            else
            {
                if (gCurTask->unk60 < 0)
                    gCurTask->unk60 = 0xFFFFF300;
                else
                    gCurTask->unk60 = 0x2000;
            }
        }
    }
    if (gCurTask->unk34 != 0)
        return;
    if (gCurTask->unk60 < 0)
    {
        gCurTask->unk58 = -gUnk_08742090[1];
        gCurTask->unk60 = gUnk_08742098[1];
    }
    else
    {
        gCurTask->unk58 = gUnk_08742090[1];
        gCurTask->unk60 = -gUnk_08742098[1];
    }
    gCurTask->unk34 = 40;
}

void sub_080864ec(void)
{
    struct Task *u;
    u32 r;

    gCurTask->unk04 = (u32)sub_0808659c;
    if (TaskGetNearestPlayerDy() <= 31)
    {
        u = &gTasks[TaskFindNearestPlayer()];
        gCurTask->unk48 = (u16)u->unk48;
        gCurTask->unk4C = u->unk4C;
    }
    else
    {
        r = RandomRange(4);
        gCurTask->unk48 =
            gUnk_087420AC[r] + (u16)gTasks[TaskFindNearestPlayer()].unk48;
        gCurTask->unk4C = (s16)gCurTask->unk48 << 16;
    }
    gCurTask->unk4A = 0;
    gCurTask->unk50 = 0;
    gCurTask->unk34 = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, (u32 *)gUnk_087420A8);
}

void sub_0808659c(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087420BC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080865c0(void)
{
    u8 k;

    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk60 = 0x2500;
    gCurTask->unk68 = 0x30000;
    TaskSetFrame(4);
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x30000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x8000;
    k = gCurTask->unk74;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gCurTask->unk73 = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gCurTask->unk73 = 1;
    }
    TaskSleepForever();
}

void sub_080867b8(void)
{
    if (gCurTask->unk73 != 2)
    {
        TaskSetEntry(sub_080860d8, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
    if (gCurTask->unk34 != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->unk34 = 1;
}

void sub_08086824(void)
{
    s8 k;

    gCurTask->unk04 = (u32)sub_080868dc;
    k = TaskGetCompassDirToNearestPlayer();
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gCurTask->unk2C = (k << 7) + 64;
    AngleToVector(gCurTask->unk2C,
                 gUnk_087420F4[gCurTask->unk74] << 8 >> 16);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    gCurTask->unk34 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087420F0);
}

void sub_080868b8(void)
{
    gCurTask->unk04 = (u32)sub_080868dc;
    CallTableEntry(gCurTask->unk14, 1, gUnk_087420F0);
}

void sub_080868dc(void)
{
    s32 v;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_08742100);
    if (sub_08086f54() != 0)
    {
        AngleToVector((s16)gCurTask->unk2C,
                     gUnk_087420F4[gCurTask->unk74] << 8 >> 16);
        v = gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        if (v > 0)
            gCurTask->unk43 = 1;
        else if (v < 0)
            gCurTask->unk43 = -1;
        gCurTask->unk34 = 20;
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    }
    gCurTask->unk7A = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086984(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    while (1)
    {
        if (gCurTask->unk34 == 0)
            gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
        TaskYieldTrampoline(1);
    }
}

void sub_080869b8(void)
{
    gCurTask->unk34--;
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_080869f0(void)
{
    gCurTask->unk04 = (u32)sub_08086a44;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08742104);
}

void sub_08086a20(void)
{
    gCurTask->unk04 = (u32)sub_08086a44;
    CallTableEntry(gCurTask->unk14, 1, gUnk_08742104);
}

void sub_08086a44(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08742108);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086a68(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 384;
    do
    {
        switch (gCurTask->unk34)
        {
        case 0:
            TaskSetFrame(4);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(6);
            TaskSetFrame(7);
            TaskYieldTrampoline(1);
            break;
        case 1:
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            break;
        }
    } while (gCurTask->unk30 != 0);
    TaskFaceNearestPlayer();
    TaskTurnAroundAndReverseX();
    TaskSetMotionXFacing(0x6600, 0x5A5A5A5A);
    gCurTask->unk60 = 0xFFFFE700;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
    }
}

void sub_08086b68(void)
{
    if (gCurTask->unk30 != 0)
    {
        gCurTask->unk30--;
        TaskFaceNearestPlayer();
        gCurTask->unk28++;
        if (gCurTask->unk28 == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_0874210C[gCurTask->unk74],
                         gUnk_0874210C[gCurTask->unk74 + 4]);
            gCurTask->unk34 = gUnk_030023D4;
            gCurTask->unk28 = 0;
        }
    }
}

void sub_08086bc0(void)
{
    gCurTask->unk04 = (u32)sub_08086c14;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874212C);
}

void sub_08086bf0(void)
{
    gCurTask->unk04 = (u32)sub_08086c14;
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874212C);
}

void sub_08086c14(void)
{
    if (gCurTask->unk34 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 3, gUnk_08742138);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 3, gUnk_08742138);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08086c5c(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk34 = 1;
    gCurTask->unk7A = 1;
    gCurTask->unk28 = ActorStartAnim(gUnk_08742144);
    while (gCurTask->unk7A != 0)
    {
        if (TaskGetNearestPlayerDx() < 0)
        {
            if (-TaskGetNearestPlayerDx() <= 63)
                break;
        }
        else if (TaskGetNearestPlayerDx() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08086ccc(void)
{
    if (gCurTask->unk14 != 0)
    {
        TaskSetEntry(sub_08086bf0, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08086d18(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk34 = 0;
    PlaySfx(187);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    ActorSetState(2);
    TaskSleepForever();
}

void sub_08086da4(void)
{
    if (gCurTask->unk14 != 1)
    {
        TaskSetEntry(sub_08086bf0, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08086df0(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk28 = ActorStartAnim(gUnk_087420D4);
    switch (gCurTask->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 1:
        TaskYieldTrampoline(32);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_087420C0);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    }
}

void sub_08086ec8(void)
{
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08086efc(void)
{
    gCurTask->unk04 = (u32)sub_08086f40;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(18);
        TaskSetFrame(9);
        TaskYieldTrampoline(10);
    }
}

void sub_08086f40(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_08086f54(void)
{
    u8 n = 0;
    s16 v;

    if ((gCurTask->unk7A & 1) == 0 && (gTerrainResult[1] & 1) == 0
     && (gTerrainResult[0] & 3) == 0)
        return 0;
    switch (gTerrainResult[4])
    {
    case 0:
        if (gCurTask->unk7A & 1)
            n = n + 1;
        if ((gTerrainResult[1] & 1) == 0)
            break;
        n = n + 2;
        break;
    case 3:
        n = n + 3;
        break;
    case 4:
        n = n + 4;
        break;
    case 1:
        n = n + 5;
        break;
    case 2:
        n = n + 6;
        break;
    case 8:
        n = n + 7;
        break;
    case 7:
        n = n + 8;
        break;
    case 6:
        n = n + 9;
        break;
    case 5:
        n = n + 10;
        break;
    }
    if (gTerrainResult[0] & 1)
        n = n + 11;
    if (gTerrainResult[0] & 2)
        n = n + 22;
    v = gUnk_08742150[(n << 4) + (gCurTask->unk2C >> 5)];
    if (v < 0)
        return 0;
    gCurTask->unk2C = v;
    return 1;
}

void sub_0808705c(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_08752560;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->unk73, 10, gUnk_08742570);
}

void sub_080870a4(void)
{
    CallTableEntry(gCurTask->unk73, 10, gUnk_08742570);
}

void sub_080870c4(void)
{
    gCurTask->unk04 = (u32)sub_08087118;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087425B0);
}

void sub_080870f4(void)
{
    gCurTask->unk04 = (u32)sub_08087118;
    CallTableEntry(gCurTask->unk14, 1, gUnk_087425B0);
}

void sub_08087118(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087425B4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808713c(void)
{
    s16 *p;

    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_087425B8[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->unk4A;
    if (*p < gTasks[TaskFindNearestPlayer()].unk4A)
    {
        gCurTask->unk58 = gUnk_087425C0[0];
        gCurTask->unk60 = -gUnk_087425C8[0];
    }
    else
    {
        gCurTask->unk58 = -gUnk_087425C0[0];
        gCurTask->unk60 = gUnk_087425C8[0];
    }
    gCurTask->unk30 = 4;
    gCurTask->unk34 = 40;
    while (1)
    {
        if (gCurTask->unk60 >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
        }
    }
}

void sub_08087210(void)
{
    struct Task *t = gCurTask;
    s32 n;

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->unk60 < 0)
    {
        if (n < 0)
            return;
        t->unk58 = -gUnk_087425C0[0];
        t->unk60 = gUnk_087425C8[0];
    }
    else
    {
        t->unk58 = gUnk_087425C0[0];
        t->unk60 = -gUnk_087425C8[0];
    }
    gCurTask->unk34 = 40;
}

void sub_08087268(void)
{
    gCurTask->unk04 = (u32)sub_080872bc;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087425D0);
}

void sub_08087298(void)
{
    gCurTask->unk04 = (u32)sub_080872bc;
    CallTableEntry(gCurTask->unk14, 1, gUnk_087425D0);
}

void sub_080872bc(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087425D4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080872e0(void)
{
    s16 *p;

    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetMotionXFacing(gUnk_087425B8[gCurTask->unk74], 0x5A5A5A5A);
    p = &gCurTask->unk4A;
    if (*p < gTasks[TaskFindNearestPlayer()].unk4A)
    {
        gCurTask->unk58 = gUnk_087425C0[1];
        gCurTask->unk60 = -gUnk_087425C8[1];
    }
    else
    {
        gCurTask->unk58 = -gUnk_087425C0[1];
        gCurTask->unk60 = gUnk_087425C8[1];
    }
    gCurTask->unk34 = 40;
    while (1)
    {
        if (gCurTask->unk60 >= 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
        }
    }
}

void sub_080873b0(void)
{
    s32 d, a;

    if (--gCurTask->unk34 == 20)
    {
        d = (s16)TaskGetNearestPlayerDy();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gCurTask->unk60 < 0)
                    gCurTask->unk60 = 0xFFFFE000;
                else
                    gCurTask->unk60 = 0xD00;
            }
            else
            {
                if (gCurTask->unk60 < 0)
                    gCurTask->unk60 = 0xFFFFF300;
                else
                    gCurTask->unk60 = 0x2000;
            }
        }
    }
    if (gCurTask->unk34 != 0)
        return;
    if (gCurTask->unk60 < 0)
    {
        gCurTask->unk58 = -gUnk_087425C0[1];
        gCurTask->unk60 = gUnk_087425C8[1];
    }
    else
    {
        gCurTask->unk58 = gUnk_087425C0[1];
        gCurTask->unk60 = -gUnk_087425C8[1];
    }
    gCurTask->unk34 = 40;
}

void sub_08087458(void)
{
    struct Task *u;
    u32 r;

    gCurTask->unk04 = (u32)sub_08087508;
    if (TaskGetNearestPlayerDy() <= 31)
    {
        u = &gTasks[TaskFindNearestPlayer()];
        gCurTask->unk48 = (u16)u->unk48;
        gCurTask->unk4C = u->unk4C;
    }
    else
    {
        r = RandomRange(4);
        gCurTask->unk48 =
            gUnk_087425DC[r] + (u16)gTasks[TaskFindNearestPlayer()].unk48;
        gCurTask->unk4C = (s16)gCurTask->unk48 << 16;
    }
    gCurTask->unk4A = 0;
    gCurTask->unk50 = 0;
    gCurTask->unk34 = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, (u32 *)gUnk_087425D8);
}

void sub_08087508(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087425EC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808752c(void)
{
    u8 k;

    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk60 = 0x2500;
    gCurTask->unk68 = 0x30000;
    TaskSetFrame(4);
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x30000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gCurTask->unk74]);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x8000;
    k = gCurTask->unk74;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gCurTask->unk73 = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gCurTask->unk73 = 1;
    }
    TaskSleepForever();
}

void sub_08087724(void)
{
    if (gCurTask->unk73 != 2)
    {
        TaskSetEntry(sub_080870a4, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
    if (gCurTask->unk34 != 0)
        return;
    if (TaskGetNearestPlayerDy() > 15)
        return;
    gCurTask->unk34 = 1;
}

void sub_08087790(void)
{
    s8 k;

    gCurTask->unk04 = (u32)sub_08087848;
    k = TaskGetCompassDirToNearestPlayer() - 1;
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gCurTask->unk2C = (k << 7) + 64;
    AngleToVector(gCurTask->unk2C,
                 gUnk_08742600[gCurTask->unk74] << 8 >> 16);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    gCurTask->unk34 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087425F8);
}

void sub_08087824(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_08087848;
    CallTableEntry(t->unk14, 1, gUnk_087425F8);
}

void sub_08087848(void)
{
    s32 v;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_087425FC);
    if (sub_08086f54() != 0)
    {
        AngleToVector((s16)gCurTask->unk2C,
                     gUnk_08742600[gCurTask->unk74] << 8 >> 16);
        v = gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        if (v > 0)
            gCurTask->unk43 = 1;
        else if (v < 0)
            gCurTask->unk43 = -1;
        gCurTask->unk34 = 20;
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    }
    gCurTask->unk7A = 0;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080878f0(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    while (1)
    {
        if (gCurTask->unk34 == 0)
            gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
        TaskYieldTrampoline(1);
    }
}

void sub_08087924(void)
{
    gCurTask->unk34--;
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_0808795c(void)
{
    gCurTask->unk04 = (u32)sub_080879b0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_0874260C);
}

void sub_0808798c(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080879b0;
    CallTableEntry(t->unk14, 1, gUnk_0874260C);
}

void sub_080879b0(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08742610);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080879d4(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 384;
    do
    {
        switch (gCurTask->unk34)
        {
        case 0:
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            TaskSetFrame(5);
            TaskYieldTrampoline(8);
            break;
        case 1:
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            break;
        }
    } while (gCurTask->unk30 != 0);
    TaskFaceNearestPlayer();
    TaskTurnAroundAndReverseX();
    TaskSetMotionXFacing(0x6600, 0x5A5A5A5A);
    gCurTask->unk60 = 0xFFFFE700;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
    }
}

void sub_08087a98(void)
{
    if (gCurTask->unk30 != 0)
    {
        gCurTask->unk30--;
        TaskFaceNearestPlayer();
        gCurTask->unk28++;
        if (gCurTask->unk28 == 8)
        {
            TaskAccelerateTowardNearestPlayer(gUnk_08742614[gCurTask->unk74],
                         gUnk_08742614[gCurTask->unk74 + 4]);
            gCurTask->unk34 = gUnk_030023D4;
            gCurTask->unk28 = 0;
        }
    }
}

void sub_08087af0(void)
{
    gCurTask->unk04 = (u32)sub_08087b44;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874263C);
}

void sub_08087b20(void)
{
    gCurTask->unk04 = (u32)sub_08087b44;
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874263C);
}

void sub_08087b44(void)
{
    if (gCurTask->unk34 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 3, gUnk_08742648);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 3, gUnk_08742648);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08087b8c(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk34 = 1;
    gCurTask->unk7A = 1;
    gCurTask->unk28 = ActorStartAnim(gUnk_08742634);
    while (gCurTask->unk7A != 0)
    {
        if (TaskGetNearestPlayerDx() < 0)
        {
            if (-TaskGetNearestPlayerDx() <= 63)
                break;
        }
        else if (TaskGetNearestPlayerDx() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08087bfc(void)
{
    if (gCurTask->unk14 != 0)
    {
        TaskSetEntry(sub_08087b20, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08087c48(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk34 = 0;
    PlaySfx(187);
    gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    ActorSetState(2);
    TaskSleepForever();
}

void sub_08087cd4(void)
{
    if (gCurTask->unk14 != 1)
    {
        TaskSetEntry(sub_08087b20, gCurTaskIdx);
        return;
    }
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08087d20(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk28 = ActorStartAnim(gUnk_087425A4);
    switch (gCurTask->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
        TaskYieldTrampoline(10);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 1:
        TaskYieldTrampoline(32);
        TaskStop();
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->unk28 = ActorStartAnim(gUnk_08742598);
        TaskYieldTrampoline(8);
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSleepForever();
        break;
    }
}

void sub_08087df8(void)
{
    if (gCurTask->unk8C->unk2C != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_08087e2c(void)
{
    gCurTask->unk04 = (u32)sub_08087e84;
    gCurTask->unk7A = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08742654);
}

void sub_08087e60(void)
{
    gCurTask->unk04 = (u32)sub_08087e84;
    CallTableEntry(gCurTask->unk14, 3, gUnk_08742654);
}

void sub_08087e84(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08742660);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08087eb4(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskYieldTrampoline(24);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08087ef0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_08087e60, gCurTaskIdx);
}

void sub_08087f18(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk7A = 0;
    PlaySfx(187);
    TaskSetMotionY(0xFFFE0000, 0x800, 0x30000);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 3);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08087f88(void)
{
}

void sub_08087f8c(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk60 = 0x800;
    gCurTask->unk68 = 0x30000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
    }
}

void sub_08087fc8(void)
{
}

void sub_08087fcc(void)
{
    gCurTask->unk04 = (u32)sub_08088024;
    gCurTask->unk7A = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 6, gUnk_0874266C);
}
