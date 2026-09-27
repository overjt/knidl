/* game_code_and_rodata 0x08088000-0x0808AA68 (issue #80, module M23 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08088000 0x0808AA68 src/enemy_88000.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gCurTask;
extern struct Task gTasks[];

/* ROM tables */
extern s16 gUnk_08742824[];
extern s16 gUnk_08742834[];
extern s16 gUnk_08742844[];
extern s32 gUnk_0874269C[];
extern s32 gUnk_087426A4[];
extern s32 gUnk_087426EC[];
extern s32 gUnk_08742734[];
extern s32 gUnk_0874276C[];
extern s32 gUnk_08742818[];
extern u16 gUnk_08742862[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0874266C[];
extern u32 gUnk_08742684[];
extern u32 gUnk_087426AC[];
extern u32 gUnk_087426B0[];
extern u32 gUnk_087426B4[];
extern u32 gUnk_087426C4[];
extern u32 gUnk_087426D8[];
extern u32 gUnk_087426F4[];
extern u32 gUnk_08742704[];
extern u32 gUnk_08742710[];
extern u32 gUnk_0874271C[];
extern u32 gUnk_08742728[];
extern u32 gUnk_0874273C[];
extern u32 gUnk_08742744[];
extern u32 gUnk_08742758[];
extern u32 gUnk_08742798[];
extern u32 gUnk_087427A0[];
extern u32 gUnk_087427A8[];
extern u32 gUnk_087427B4[];
extern u32 gUnk_087427BC[];
extern u32 gUnk_087427E8[];
extern u32 gUnk_08742C14[];
extern u32 gUnk_08742C30[];
extern u32 gUnk_08742C4C[];
extern u32 gUnk_08742C68[];
extern u32 gUnk_08742C84[];
extern u32 gUnk_08742CA0[];
extern u32 gUnk_08742CBC[];
extern u32 gUnk_08742E50[];
extern u32 gUnk_08742E5C[];
extern u32 gUnk_0875262C[];
extern u32 gUnk_087526A8[];
extern u32 gUnk_087527DC[];
extern u32 gUnk_08752808[];
extern u32 gUnk_08752828[];
extern u8 gUnk_08742778[];
extern u8 gUnk_087427B0[];
extern u8 gUnk_087427B2[];
extern u8 gUnk_08742814[];
extern u8 gUnk_08742820[];
extern u8 gUnk_08742822[];
extern u8 gUnk_08742830[];
extern u8 gUnk_08742854[];
extern u8 gUnk_08742856[];
extern u8 gUnk_0874285C[];

/* Externals */
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s8 TaskGetParentFacing(void);
extern u16 sub_08021c14(s16 x, s16 y);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);
extern u8 IsWaterAtPixel(s16 x, s16 y);
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskMove(void);
extern void TaskDrawWorld(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskInitWaterFlags(void);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAroundAndReverseX(void);
extern void AngleToVector(s16 t, s16 mag);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void sub_0806a0f0(s32 a);
void sub_0808705c(void);
void sub_08087e60(void);

/* Defined below */
void sub_08088024(void);
void sub_080883b8(void);
void sub_080884d0(void);
void sub_080886d8(void);
void sub_08088b10(void);
void sub_08088d7c(void);
void sub_08088fac(void);
void sub_0808921c(void);
void sub_080896dc(void);
void sub_08089808(u8 a);
void sub_080898dc(void);
void sub_08089bdc(void);
void sub_08089d44(void);
void sub_0808a7e0(void);
void sub_0808a7f4(u8 a);
void sub_0808a84c(u8 *p, s32 b);
void sub_0808a880(s32 a);

void sub_08088000(void)
{
    gCurTask->unk04 = (u32)sub_08088024;
    CallTableEntry(gCurTask->unk14, 6, gUnk_0874266C);
}

void sub_08088024(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 6, gUnk_08742684);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088054(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskYieldTrampoline(15);
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    switch (RandomRange(8))
    {
    case 0:
    case 1:
        gCurTask->unk54 = 0;
        ActorSetState(1);
        break;
    case 2:
    case 3:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        ActorSetState(1);
        break;
    case 4:
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        ActorSetState(1);
        break;
    case 5:
        ActorSetState(2);
        break;
    case 6:
    case 7:
        ActorSetState(3);
        break;
    }
    TaskSleepForever();
}

void sub_080880fc(void)
{
    s32 v;

    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
    v = gUnk_0874269C[gCurTask->unk74];
    if (v > abs(TaskGetNearestPlayerDx()))
    {
        v = gUnk_087426A4[gCurTask->unk74];
        if (v > abs(TaskGetNearestPlayerDy()))
        {
            ActorSetState(4);
            TaskSetEntry(sub_08088000, gCurTaskIdx);
        }
    }
}

void sub_080881a0(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk7A = 0;
    if (RandomRange(2) != 0)
        TaskTurnAroundAndReverseX();
    TaskSetMotionY(0xFFFE8000, 0x4000, 0x30000);
    TaskSleepForever();
}

void sub_080881e0(void)
{
}

void sub_080881e4(void)
{
    gCurTask->unk15 = 2;
    TaskYieldTrampoline(32);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088208(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
}

void sub_08088230(void)
{
    gCurTask->unk15 = 3;
    TaskYieldTrampoline(64);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088254(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_08088000, gCurTaskIdx);
}

void sub_0808827c(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk7A = 0;
    PlaySfx(187);
    TaskStop();
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskFaceNearestPlayer();
    while (gCurTask->unk58 < 0)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
    }
    gCurTask->unk73 = 4;
    TaskSleepForever();
}

void sub_080882f0(void)
{
    if (gCurTask->unk73 != 7)
    {
        gCurTask->unk04 = 0;
        TaskSetEntry(sub_0808705c, gCurTaskIdx);
    }
}

void sub_08088320(void)
{
    gCurTask->unk15 = 5;
    gCurTask->unk60 = 0x1500;
    gCurTask->unk68 = 0x30000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
    }
}

void sub_0808835c(void)
{
}

void sub_08088360(void)
{
    gCurTask->unk04 = (u32)sub_080883b8;
    gCurTask->unk7A = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087426AC);
}

void sub_08088394(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080883b8;
    CallTableEntry(t->unk14, 1, gUnk_087426AC);
}

void sub_080883b8(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087426B0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080883dc(void)
{
    gCurTask->unk15 = 0;
    TaskFaceNearestPlayer();
    gCurTask->unk34 = 8;
    while (1)
    {
        gCurTask->unk58 = 0xFFFF0000;
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0xFFFF8000;
        gCurTask->unk3C--;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x10000;
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        gCurTask->unk3C--;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0;
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0xFFFF8000;
        gCurTask->unk3C--;
        TaskYieldTrampoline(8);
    }
}

void sub_08088478(void)
{
    if (--gCurTask->unk34 == 0)
    {
        gCurTask->unk34 = 8;
        TaskFaceNearestPlayer();
    }
}

void sub_08088498(void)
{
    gCurTask->unk04 = (u32)sub_080884d0;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_080884d0(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_080884e4(void)
{
    if (gCurTask->unk73 != 9)
    {
        switch (gCurTask->unk73)
        {
        case 6:
            ActorSetState(0);
            TaskSetEntry(sub_08087e60, gCurTaskIdx);
            return 1;
        case 7:
            ActorSetState(0);
            TaskSetEntry(sub_08088000, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08088540(void)
{
    u8 s;

    if (gCurTask->unk73 != 9)
    {
        switch (gCurTask->unk73)
        {
        case 7:
            s = gCurTask->unk14;
            if (s == 0 || s == 2 || s == 3)
            {
                ActorSetState(5);
                TaskSetEntry(sub_08088000, gCurTaskIdx);
                return 1;
            }
            return 0;
        }
        return 0;
    }
}

s32 sub_08088590(void)
{
    if (gCurTask->unk73 != 9)
    {
        switch (gCurTask->unk73)
        {
        case 7:
            if (gCurTask->unk14 == 1)
                TaskTurnAroundAndReverseX();
            return 0;
        }
        return 0;
    }
}

s32 sub_080885c0(void)
{
    if (gCurTask->unk73 != 9)
    {
        switch (gCurTask->unk73)
        {
        case 6:
            gCurTask->unk58 = 0;
            return 0;
        case 7:
            if (gCurTask->unk14 == 4)
            {
                gCurTask->unk73 = 4;
                TaskSetEntry(sub_0808705c, gCurTaskIdx);
                return 1;
            }
            break;
        default:
            return 0;
        }
        return 0;
    }
}

void sub_08088610(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_0875262C;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->unk73, 4, gUnk_087426B4);
}

void sub_08088658(void)
{
    gCurTask->unk04 = (u32)sub_080886d8;
    TaskInitWaterFlags();
    if (gCurTask->unk7B == 3)
    {
        ActorSetState(3);
        gCurTask->unk7A = 0;
        CallTableEntry(gCurTask->unk14, 5, gUnk_087426C4);
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 5, gUnk_087426C4);
}

void sub_080886b4(void)
{
    gCurTask->unk04 = (u32)sub_080886d8;
    CallTableEntry(gCurTask->unk14, 5, gUnk_087426C4);
}

void sub_080886d8(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 5, gUnk_087426D8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088708(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk34 = 100;
    TaskSetMotionXFacing(gUnk_087426EC[gCurTask->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2]);
        gCurTask->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2 + 1]);
        gCurTask->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2]);
        gCurTask->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gCurTask->unk74 * 2 + 1]);
    }
}

void sub_080887a0(void)
{
    s32 n = gCurTask->unk34 - 1;

    gCurTask->unk34 = n;
    switch (n)
    {
    case 20:
    case 40:
    case 60:
    case 80:
        if (RandomRange(4) == 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_080886b4, gCurTaskIdx);
        }
        break;
    case 0:
        ActorSetState(1);
        TaskSetEntry(sub_080886b4, gCurTaskIdx);
        break;
    }
}

void sub_0808880c(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk28 = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(24);
    PlaySfx(188);
    gCurTask->unk7A = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080888a0(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080886b4, gCurTaskIdx);
}

void sub_080888c8(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk28 = 0;
    gCurTask->unk60 = 0x1500;
    gCurTask->unk68 = 0x30000;
    TaskSetFrame(6);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088920(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080886b4, gCurTaskIdx);
}

void sub_08088948(void)
{
    gCurTask->unk15 = 3;
    TaskStop();
    gCurTask->unk58 = 0x4000;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(96);
        gCurTask->unk58 = 0x8000;
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 0xFFFF0000;
        gCurTask->unk7A = 0;
        gCurTask->unk3C--;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 0xFFFFC000;
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
    }
}

void sub_080889c8(void)
{
}

void sub_080889cc(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x1500, 0x30000);
    TaskSetFrame(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088a3c(void)
{
    if (gCurTask->unk14 != 4)
        TaskSetEntry(sub_080886b4, gCurTaskIdx);
}

void sub_08088a64(void)
{
    gCurTask->unk04 = (u32)sub_08088b10;
    TaskFaceNearestPlayer();
    if (gCurTask->unk43 == 1)
        gCurTask->unk4C = (gTasks[TaskFindNearestPlayer()].unk48 - 80) << 16;
    else
        gCurTask->unk4C = (gTasks[TaskFindNearestPlayer()].unk48 + 80) << 16;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08742704);
}

void sub_08088aec(void)
{
    gCurTask->unk04 = (u32)sub_08088b10;
    CallTableEntry(gCurTask->unk14, 3, gUnk_08742704);
}

void sub_08088b10(void)
{
    if (gCurTask->unk28 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 3, gUnk_08742710);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 3, gUnk_08742710);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088b58(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_08088b98(void)
{
    if (sub_08021c14(gCurTask->unk48, gCurTask->unk4A) == 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_08088aec, gCurTaskIdx);
    }
}

void sub_08088bd8(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    TaskSetMotionY(0x40000, 0x1500, 0x30000);
    gCurTask->unk7A = 0;
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_08088ca0(void)
{
}

void sub_08088ca4(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk28 = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFC0000;
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08088ce4(void)
{
}

void sub_08088ce8(void)
{
    gCurTask->unk04 = (u32)sub_08088d7c;
    TaskInitWaterFlags();
    if (gCurTask->unk7B == 3)
    {
        gCurTask->unk73 = 0;
        gCurTask->unk04 = (u32)sub_080886d8;
        gCurTask->unk7A = 0;
        ActorSetState(3);
        CallTableEntry(gCurTask->unk14, 5, gUnk_087426C4);
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874271C);
}

void sub_08088d58(void)
{
    gCurTask->unk04 = (u32)sub_08088d7c;
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874271C);
}

void sub_08088d7c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08742728);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08088dac(void)
{
    struct Task *t;
    struct Task *u;
    s32 a, d;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    u = &gTasks[TaskFindNearestPlayer()];
    t = gCurTask;
    a = t->unk50;
    d = (a >> 16) - (u->unk50 >> 16);
    if (d > 0)
        t->unk34 = 37;
    else if (d < 0)
        t->unk34 = 43;
    else if ((a & 0xFF) - (u->unk50 & 0xFF) >= 0
                 ? (a & 0xFF) - (u->unk50 & 0xFF) <= 15
                 : (u->unk50 & 0xFF) - (a & 0xFF) <= 15)
        gCurTask->unk34 = 37;
    else
        gCurTask->unk34 = 40;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(gUnk_08742734[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2000, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_08088e78(void)
{
    if (--gCurTask->unk34 == 0)
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
}

void sub_08088ea4(void)
{
    gCurTask->unk15 = 2;
    TaskStop();
    TaskSetFrame(5);
    TaskYieldTrampoline(10);
    gCurTask->unk73 = 0;
    TaskSleepForever();
}

void sub_08088ed4(void)
{
    if (gCurTask->unk73 != 2)
        TaskSetEntry(sub_08088610, gCurTaskIdx);
}

void sub_08088efc(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(24);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08088f24(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
}

void sub_08088f4c(void)
{
    gCurTask->unk04 = (u32)sub_08088fac;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(12);
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk3C++;
        TaskYieldTrampoline(12);
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
    }
}

void sub_08088fac(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_08088fc0(void)
{
    if (gCurTask->unk73 != 3)
    {
        switch (gCurTask->unk73)
        {
        case 0:
            gCurTask->unk28 = 1;
            return 0;
        case 1:
            ActorSetState(2);
            TaskSetEntry(sub_08088aec, gCurTaskIdx);
            return 1;
        case 2:
            ActorSetState(2);
            TaskSetEntry(sub_08088d58, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08089024(void)
{
    if (gCurTask->unk73 != 3)
    {
        switch (gCurTask->unk73)
        {
        default:
            return 0;
        case 0:
            ActorSetState(2);
            TaskSetEntry(sub_080886b4, gCurTaskIdx);
            return 1;
        }
    }
}

s32 sub_08089064(void)
{
    if (gCurTask->unk73 != 3)
    {
        gCurTask->unk8C->unk5C = (u32)gUnk_08742E5C;
        switch (gCurTask->unk73)
        {
        case 0:
            ActorSetState(3);
            TaskSetEntry(sub_080886b4, gCurTaskIdx);
            return 1;
        case 2:
            gCurTask->unk73 = 0;
            ActorSetState(3);
            TaskSetEntry(sub_080886b4, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080890d4(void)
{
    if (gCurTask->unk73 != 3)
    {
        gCurTask->unk8C->unk5C = (u32)gUnk_08742E50;
        if (gCurTask->unk73 != 0)
            return 0;
        ActorSetState(4);
        TaskSetEntry(sub_080886b4, gCurTaskIdx);
        return 1;
    }
}

s32 sub_08089120(void)
{
    if (gCurTask->unk73 != 3)
    {
        TaskTurnAroundAndReverseX();
        return 0;
    }
}

s32 sub_0808913c(void)
{
    if (gCurTask->unk73 != 3)
    {
        gCurTask->unk58 = 0;
        if (gCurTask->unk73 != 2)
            return 0;
        ActorSetState(1);
        TaskSetEntry(sub_08088d58, gCurTaskIdx);
        return 1;
    }
}

void sub_08089180(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_087526A8;
    CallTableEntry(gCurTask->unk73, 2, gUnk_0874273C);
}

void sub_080891c0(void)
{
    gCurTask->unk04 = (u32)sub_0808921c;
    gCurTask->unk28 = 12;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 5, gUnk_08742744);
}

void sub_080891f8(void)
{
    gCurTask->unk04 = (u32)sub_0808921c;
    CallTableEntry(gCurTask->unk14, 5, gUnk_08742744);
}

void sub_0808921c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 5, gUnk_08742758);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808924c(void)
{
    s32 n;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    if (gCurTask->unk74 == 0 || RandomRange(2) != 0)
    {
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    }
    else
    {
        TaskFaceNearestPlayer();
        TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    }
    n = RandomRange(3);
    gCurTask->unk34 = n;
    TaskSetMotionY(gUnk_0874276C[n], 0x2000, 0x30000);
    PlaySfx(193);
    while (1)
    {
        sub_08089808(gCurTask->unk28);
        switch (gCurTask->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->unk43 == 1)
        {
            gCurTask->unk28++;
            if (gCurTask->unk28 > 15)
                gCurTask->unk28 = 0;
        }
        else
        {
            gCurTask->unk28--;
            if (gCurTask->unk28 < 0)
                gCurTask->unk28 = 15;
        }
    }
}

void sub_08089330(void)
{
}

void sub_08089334(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk00 = 0;
    gCurTask->unk7A = 0;
    gCurTask->unk43 = -gCurTask->unk43;
    if (gCurTask->unk43 == 1)
        gCurTask->unk3E = gCurTask->unk3E & 0x7FFF;
    else
        gCurTask->unk3E = gCurTask->unk3E | 0x8000;
    ActorSetAttackBox((u32)gUnk_08742CBC);
    gCurTask->unk3C = 21;
    TaskYieldTrampoline(12);
    ActorSetAttackBox((u32)gUnk_08742C68);
    if (gCurTask->unk28 <= 7)
    {
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(3);
    }
    gCurTask->unk00 = (u32)ActorMove;
    TaskSetMotionXFacing(abs(gCurTask->unk54), 0x5A5A5A5A);
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->unk28);
        switch (gCurTask->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->unk43 == 1)
        {
            gCurTask->unk28++;
            if (gCurTask->unk28 > 15)
                gCurTask->unk28 = 0;
        }
        else
        {
            gCurTask->unk28--;
            if (gCurTask->unk28 < 0)
                gCurTask->unk28 = 15;
        }
    }
}

void sub_0808945c(void)
{
}

void sub_08089460(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk00 = 0;
    ActorSetAttackBox((u32)gUnk_08742C84);
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(12);
    ActorSetAttackBox((u32)gUnk_08742C30);
    if (gCurTask->unk28 <= 7)
    {
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(3);
    }
    gCurTask->unk00 = (u32)ActorMove;
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->unk28);
        switch (gCurTask->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->unk54 > 0)
        {
            gCurTask->unk28++;
            if (gCurTask->unk28 > 15)
                gCurTask->unk28 = 0;
        }
        else
        {
            gCurTask->unk28--;
            if (gCurTask->unk28 < 0)
                gCurTask->unk28 = 15;
        }
    }
}

void sub_08089530(void)
{
    gCurTask->unk58 += gCurTask->unk60;
}

void sub_08089544(void)
{
    gCurTask->unk15 = 3;
    gCurTask->unk00 = 0;
    gCurTask->unk58 = 0;
    ActorSetAttackBox((u32)gUnk_08742CA0);
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(13);
    ActorSetAttackBox((u32)gUnk_08742C4C);
    if (gCurTask->unk28 <= 7)
    {
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(3);
    }
    else
    {
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(3);
    }
    ActorSetState(0);
    gCurTask->unk00 = (u32)ActorMove;
    ActorSetAttackBox((u32)gUnk_08742C14);
    TaskSleepForever();
}

void sub_080895c4(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_080891f8, gCurTaskIdx);
}

void sub_080895ec(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk60 = 0x1500;
    gCurTask->unk68 = 0x30000;
    ActorSetAttackBox((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gCurTask->unk28);
        switch (gCurTask->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            TaskSleepForever();
            break;
        }
        if (gCurTask->unk54 > 0)
        {
            gCurTask->unk28++;
            if (gCurTask->unk28 > 15)
                gCurTask->unk28 = 0;
        }
        else
        {
            gCurTask->unk28--;
            if (gCurTask->unk28 < 0)
                gCurTask->unk28 = 15;
        }
    }
}

void sub_0808967c(void)
{
}

void sub_08089680(void)
{
    gCurTask->unk04 = (u32)sub_080896dc;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(20);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(4);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
    }
}

void sub_080896dc(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_080896ec(void)
{
    if (gCurTask->unk73 != 1)
    {
        if (gCurTask->unk14 == 3)
            return 0;
        ActorSetState(3);
        TaskSetEntry(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808972c(void)
{
    if (gCurTask->unk73 != 1)
    {
        if (gCurTask->unk14 != 3)
            return 0;
        ActorSetState(4);
        TaskSetEntry(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808976c(void)
{
    if (gCurTask->unk73 != 1)
    {
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_0808978c(void)
{
    if (gCurTask->unk73 != 1)
    {
        if (gCurTask->unk54 != 0 && gCurTask->unk58 != 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_080891f8, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080897d0(void)
{
    if (gCurTask->unk73 != 1)
    {
        gCurTask->unk58 = -gCurTask->unk58;
        ActorSetState(2);
        TaskSetEntry(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

void sub_08089808(u8 a)
{
    gCurTask->unk3C = gUnk_08742778[a * 2];
    if (gUnk_08742778[a * 2 + 1] != 0)
        gCurTask->unk3E = gCurTask->unk3E | 0x8000;
    else
        gCurTask->unk3E = gCurTask->unk3E & 0x7FFF;
}

void sub_08089848(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_087527DC;
    CallTableEntry(gCurTask->unk73, 2, gUnk_08742798);
}

void sub_08089888(void)
{
    gCurTask->unk04 = (u32)sub_080898dc;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_087427A0);
}

void sub_080898b8(void)
{
    gCurTask->unk04 = (u32)sub_080898dc;
    CallTableEntry(gCurTask->unk14, 2, gUnk_087427A0);
}

void sub_080898dc(void)
{
    if ((u8)sub_08069888() == 0)
        CallTableEntry(gCurTask->unk15, 2, gUnk_087427A8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808990c(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_087427B0[gCurTask->unk74])
    {
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(28);
        gCurTask->unk3C--;
        TaskYieldTrampoline(3);
        gCurTask->unk3C--;
        TaskYieldTrampoline(3);
        gCurTask->unk3C--;
        TaskYieldTrampoline(3);
        gCurTask->unk3C--;
        TaskYieldTrampoline(18);
        gCurTask->unk3C++;
        TaskYieldTrampoline(7);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080899d4(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080898b8, gCurTaskIdx);
}

void sub_080899fc(void)
{
    struct ActorSpawn sp;
    u8 zero;

    gCurTask->unk15 = 1;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_087427B2[gCurTask->unk74])
    {
        if (gCurTask->unk74 == 1)
        {
            sp.unk00 = 5;
            sp.unk04 = 107;
            sp.unk08 = zero = 0;
            sp.unk09 = gCurTask->unk74;
            sp.unk0C = zero;
            sp.unk0E = -8;
            sp.unk0A = 1;
            PlaySfx(195);
            gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
            CreateChildTaskHere(219, 1);
        }
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(12);
        gCurTask->unk6C++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08089aac(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080898b8, gCurTaskIdx);
}

void sub_08089ad4(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 12;
    gCurTask->unk38 = gUnk_08752808;
    gCurTask->unk43 = TaskGetParentFacing();
    gCurTask->unk50 = (gCurTask->unk4A - 8) << 16;
    gCurTask->unk58 = 0xFFFE0000;
    TaskSetFrame(6);
    TaskYieldTrampoline(6);
    TaskSetFrame(7);
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void sub_08089b44(void)
{
    gCurTask->unk04 = (u32)sub_08089bdc;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(34);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(18);
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
    }
}

void sub_08089bdc(void)
{
    sub_08069888();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_08089bf0(void)
{
    if (gCurTask->unk73 != 1)
    {
        TaskStop();
        return 0;
    }
}

s32 sub_08089c0c(void)
{
    if (gCurTask->unk73 != 1)
    {
        gCurTask->unk60 = 0x1500;
        gCurTask->unk68 = 0x30000;
        return 0;
    }
}

s32 sub_08089c30(void)
{
    if (gCurTask->unk73 != 1)
    {
        TaskStop();
        gCurTask->unk58 = 0x4000;
        return 0;
    }
}

void sub_08089c58(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_08752828;
    CallTableEntry(gCurTask->unk73, 2, gUnk_087427B4);
}

void sub_08089c98(void)
{
    gCurTask->unk04 = (u32)sub_08089d44;
    TaskInitWaterFlags();
    if (gCurTask->unk7B == 3)
    {
        gCurTask->unk28 = 0;
        TaskFaceNearestPlayer();
        if (gCurTask->unk43 == 1)
            gCurTask->unk2C = 0;
        else
            gCurTask->unk2C = 256;
        TaskSetFrame(7);
        ActorSetState(6);
    }
    else
    {
        ActorSetState(0);
        gCurTask->unk7A = 0;
        gCurTask->unk28 = 1;
    }
    CallTableEntry(gCurTask->unk14, 11, gUnk_087427BC);
}

void sub_08089d20(void)
{
    gCurTask->unk04 = (u32)sub_08089d44;
    CallTableEntry(gCurTask->unk14, 11, gUnk_087427BC);
}

void sub_08089d44(void)
{
    if (gCurTask->unk28 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 11, gUnk_087427E8);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 11, gUnk_087427E8);
    }
    switch (gCurTask->unk14)
    {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        gCurTask->unk7A = 0;
        break;
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08089da8(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk28 = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(gUnk_08742814[gCurTask->unk74]);
    sub_0808a7f4(15);
    switch (gUnk_030023D4)
    {
    case 0:
        ActorSetState(2);
        TaskSleepForever();
        break;
    case 1:
        ActorSetState(3);
        TaskSleepForever();
        break;
    case 2:
        ActorSetState(2);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 3:
        ActorSetState(3);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
        TaskSleepForever();
        break;
    case 4:
        ActorSetState(2);
        TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
        TaskTurnAroundAndReverseX();
        TaskSleepForever();
        break;
    case 5:
        ActorSetState(1);
        break;
    }
    TaskSleepForever();
}

void sub_08089ea0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
}

void sub_08089ec8(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk7A = 1;
    while (1)
    {
        gUnk_030023D4 = RandomRange(3);
        gCurTask->unk6C = 0;
        while ((s16)gCurTask->unk6C < gUnk_08742820[gCurTask->unk74])
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            if (gUnk_030023D4 != 0)
            {
                if (gCurTask->unk3E & 0x8000)
                    gCurTask->unk3E = gCurTask->unk3E & 0x7FFF;
                else
                    gCurTask->unk3E = gCurTask->unk3E | 0x8000;
            }
            else
            {
                TaskSetFrame(5);
            }
            TaskYieldTrampoline(16);
            gCurTask->unk6C++;
        }
        sub_0808a7f4(0);
        switch (gUnk_030023D4)
        {
        case 0:
            ActorSetState(2);
            TaskSleepForever();
            break;
        case 1:
            ActorSetState(3);
            TaskSleepForever();
            break;
        case 2:
            ActorSetState(2);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 3:
            ActorSetState(3);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
            TaskSleepForever();
            break;
        case 4:
            ActorSetState(2);
            TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
            TaskTurnAroundAndReverseX();
            TaskSleepForever();
            break;
        case 5:
            break;
        }
    }
}

void sub_0808a020(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
}

void sub_0808a048(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    PlaySfx(188);
    TaskSetFrame(8);
    gCurTask->unk58 = 0xFFFB0000;
    gCurTask->unk60 = 0x8000;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->unk58 < 0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_0808a0a8(void)
{
}

void sub_0808a0ac(void)
{
    gCurTask->unk15 = 3;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    PlaySfx(188);
    TaskSetFrame(8);
    gCurTask->unk58 = 0xFFFA0000;
    gCurTask->unk60 = 0x4000;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->unk58 < 0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_0808a10c(void)
{
}

void sub_0808a110(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetFrame(5);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    TaskFaceNearestPlayer();
    gCurTask->unk58 = 0xFFFB0000;
    gCurTask->unk60 = 0x4000;
    TaskSetMotionXFacing(gUnk_08742818[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetFrame(8);
    while (gCurTask->unk58 < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_0808a1d0(void)
{
    u8 r;

    r = IsWaterAtPixel(gCurTask->unk48, gCurTask->unk4A);
    if (r == 0)
    {
        gCurTask->unk28 = 1;
        gCurTask->unk7A = r;
    }
}

void sub_0808a204(void)
{
    gCurTask->unk15 = 5;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    TaskStop();
    gCurTask->unk58 = 0x8000;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    if (gCurTask->unk43 > 0)
        gCurTask->unk2C = 64;
    else
        gCurTask->unk2C = 192;
    TaskSetFrame(5);
    TaskYieldTrampoline(40);
    TaskSetFrame(7);
    ActorSetState(6);
    TaskSleepForever();
}

void sub_0808a270(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
}

void sub_0808a298(void)
{
    gCurTask->unk15 = 6;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    AngleToVector((s16)gCurTask->unk2C, 128);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(gUnk_08742822[gCurTask->unk74]);
    if (RandomRange(4) != 0)
        TaskYieldTrampoline(30);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    switch (RandomRange(3))
    {
    case 0:
        ActorSetState(7);
        break;
    case 1:
        ActorSetState(8);
        break;
    case 2:
        ActorSetState(9);
        break;
    }
    TaskSleepForever();
}

void sub_0808a36c(void)
{
    if (gCurTask->unk14 != 6)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gCurTask->unk48,
                     (u16)gCurTask->unk4A - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a3c4(void)
{
    gCurTask->unk15 = 7;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    TaskStop();
    sub_0808a880(0);
    TaskSetFrame(7);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742824[0]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742824[2]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742824[4]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    ActorSetState(6);
    TaskSleepForever();
}

void sub_0808a478(void)
{
    if (gCurTask->unk14 != 7)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gCurTask->unk48,
                     (u16)gCurTask->unk4A - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a4d0(void)
{
    gCurTask->unk15 = 8;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    while (1)
    {
        TaskStop();
        TaskSetFrame(9);
        TaskYieldTrampoline(gUnk_08742830[gCurTask->unk74]);
        sub_0808a880(8);
        TaskSetFrame(7);
        AngleToVector((s16)gCurTask->unk2C, gUnk_08742834[0]);
        gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->unk2C, gUnk_08742834[2]);
        gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->unk2C, gUnk_08742834[4]);
        gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        AngleToVector((s16)gCurTask->unk2C, gUnk_08742834[6]);
        gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        if (RandomRange(3) != 0)
        {
            ActorSetState(6);
            TaskSleepForever();
        }
    }
}

void sub_0808a5b8(void)
{
    if (gCurTask->unk14 != 8)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gCurTask->unk48,
                     (u16)gCurTask->unk4A - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a610(void)
{
    gCurTask->unk15 = 9;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    TaskStop();
    TaskSetFrame(9);
    TaskYieldTrampoline(gUnk_08742830[gCurTask->unk74]);
    sub_0808a880(16);
    TaskSetFrame(7);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742844[0]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742844[2]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742844[4]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    AngleToVector((s16)gCurTask->unk2C, gUnk_08742844[6]);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    if (RandomRange(3) != 0)
        ActorSetState(6);
    else
        ActorSetState(8);
    TaskSleepForever();
}

void sub_0808a710(void)
{
    if (gCurTask->unk14 != 9)
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gCurTask->unk48,
                     (u16)gCurTask->unk4A - 8) == 0)
    {
        ActorSetState(4);
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a768(void)
{
    gCurTask->unk15 = 10;
    gCurTask->unk28 = 1;
    gCurTask->unk7A = 0;
    TaskStop();
    TaskSetFrame(4);
    gCurTask->unk60 = 0x1500;
    gCurTask->unk68 = 0x30000;
    TaskSleepForever();
}

void sub_0808a7a4(void)
{
}

void sub_0808a7a8(void)
{
    gCurTask->unk04 = (u32)sub_0808a7e0;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->unk78 = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_0808a7e0(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808a7f4(u8 a)
{
    s32 n;
    u8 v;

    n = gCurTask->unk34 + 1;
    gCurTask->unk34 = n;
    if (a == 0)
        v = gUnk_08742854[n & 1];
    else
        v = a;
    if ((gCurTask->unk34 & 1) == 0)
        sub_0808a84c(gUnk_08742856, v);
    else
        sub_0808a84c(gUnk_0874285C, v);
}

void sub_0808a84c(u8 *p, s32 b)
{
    s32 r;
    s32 i;

    r = RandomRange(b + 1);
    i = 0;
    while (p[i] < r)
        i++;
    gUnk_030023B4 = r;
    gUnk_030023D4 = i;
}

void sub_0808a880(s32 a)
{
    u32 v;

    gCurTask->unk2C = v = gUnk_08742862[(u16)TaskGetAngleToNearestPlayer(0) + a];
    if (v < 128 || v > 384)
        gCurTask->unk43 = 1;
    else if (v > 128 && v < 384)
        gCurTask->unk43 = -1;
}

s32 sub_0808a8d4(void)
{
    if (gCurTask->unk73 != 1)
    {
        switch (gCurTask->unk14)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gCurTask->unk2C = 512 - gCurTask->unk2C;
            gCurTask->unk58 = -gCurTask->unk58;
            return 0;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 10:
            ActorSetState(0);
            TaskSetEntry(sub_08089d20, gCurTaskIdx);
            return 1;
        default:
            return 0;
        }
    }
}

s32 sub_0808a964(void)
{
    if (gCurTask->unk73 != 1)
    {
        switch (gCurTask->unk14)
        {
        case 0:
        case 1:
            ActorSetState(10);
            TaskSetEntry(sub_08089d20, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_0808a9a8(void)
{
    if (gCurTask->unk73 != 1)
    {
        ActorSetState(5);
        TaskSetEntry(sub_08089d20, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808a9d8(void)
{
    s32 n;

    if (gCurTask->unk73 != 1)
    {
        switch (gCurTask->unk14)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            n = 768 - gCurTask->unk2C;
            gCurTask->unk2C = n;
            if (n > 0x1FF)
                gCurTask->unk2C = n - 0x200;
            /* fallthrough */
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            TaskTurnAroundAndReverseX();
            return 0;
        default:
            return 0;
        }
    }
}

s32 sub_0808aa28(void)
{
    if (gCurTask->unk73 != 1)
    {
        switch (gCurTask->unk14)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            gCurTask->unk58 = 0;
            return 0;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gCurTask->unk2C = 512 - gCurTask->unk2C;
            gCurTask->unk58 = -gCurTask->unk58;
            return 0;
        default:
            return 0;
        }
    }
}
