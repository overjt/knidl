/* game_code_and_rodata 0x080988F8-0x08099B20 (issue #68, module M27 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080988F8 0x08099B20 src/enemy_988f8.c --newpb
 *
 * M27's first mid-boss script, built exactly like M25's bosses (rom-map section 9).
 * sub_08098e64 is the task entry: it installs ActorMove as the draw hook
 * (Task.unk00) and sub_08065438 as the per-frame hook (Task.unk0C), points
 * Task.unk38 at the graphics block gUnk_08753090, counts the enemy into
 * gUnk_02007D00[0], loads the animation script gUnk_08745624 and hands
 * Task.unk73 to CallTableEntry with the one-word table gUnk_08745630, whose only
 * entry is sub_08098ed4.
 *
 * sub_08098ed4 installs sub_08098f38 as the per-frame body and dispatches
 * Task.unk14 through the 19-word guard table gUnk_08745634; sub_08098f38
 * re-uploads (ActorFlashPalette) or drops (sub_08066468) the 16-byte graphics
 * record gUnk_08274840 while Task.unk18 is set, dispatches Task.unk15 through
 * the 19-word body table gUnk_08745680 that follows it, and finishes with the
 * animation-row selector sub_08098de4 plus sub_08068f68 / ActorReactToHit.
 * sub_08098fb0 is the re-arm hook every guard installs through
 * TaskSetEntry(fn, gCurTaskIdx).
 *
 * The rest are the states.  sub_080988f8 / sub_08098a04 are the two jump-table
 * dispatchers that turn Task.unk14 into the next animation, sub_08098afc frees
 * the helper task recorded in Task.unk46 once gTaskSlotTypes[] says its type is
 * 143 and gTasks[] says this task is its parent, sub_08098b60 walks the
 * gUnk_08745618 / gUnk_0874561F rows with the decimal-digit buffer
 * gDigits[1] as the index, sub_08098c54 fires the timed
 * RandomRange-gated transitions at Task.unk30 == 120 / 60 / 45, sub_08098d58
 * spawns the actor 13 through CreateActorFromDescAtOffsetFacing and sub_08098da4 is the "close
 * enough" probe (|TaskGetDxTo(Task.unk1C)| <= 10).  sub_080992a8 and
 * sub_08099a0c are empty state handlers, and sub_08099ad0 is the timer leaf
 * the guard table word at 0x087456C8 points at.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern u8 gDigits[6];
extern vu16 gTaskSlotTypes[];

/* ROM tables */
extern u8 gUnk_08745618[];
extern u8 gUnk_0874561F[];
extern struct AnimCmd gUnk_08745624[];
extern u32 gUnk_08745630[];
extern u32 gUnk_08745634[];
extern u32 gUnk_08745680[];
extern u32 gUnk_08745868[];
extern u32 gUnk_08745884[];
extern u32 gUnk_087458A0[];
extern u32 gUnk_087458BC[];
extern u32 gUnk_087458F4[];
extern u32 gUnk_08745910[];
extern u32 gUnk_0874592C[];
extern u32 gUnk_08745948[];
extern u32 gUnk_08745A0C[];
extern u32 gUnk_08745A14[];
extern u32 gUnk_08745A80[];
extern u32 gUnk_08753090[];
extern void *gUnk_08274840;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void IntToDigits(s16 n);
extern void TaskFree(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskStopY(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void ActorSetTerrainBox(u32 *p);
extern void sub_08063a00(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_08066580(void);
extern void sub_08066ae0(void);
extern void sub_08065438(void);
extern void ActorMove(void);
extern void RequestScreenShake(s32 a);
extern void sub_0806ee2c(void);
extern void TaskStop(void);
extern void TaskFaceNearestPlayer(void);
extern void ActorFlashPalette(void *src, u32 size);
extern void sub_08066468(void);
extern u8 sub_08067060(void);
extern void sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSleepForever(void);
extern void PlaySfx(s32 id);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void TaskTurnAround(void);
extern void sub_0806684c(void);
extern void sub_080667c0(u8 a, u16 b);
extern void ActorDie(void);
extern void sub_0806ad18(void);

/* Defined below */
void sub_08098f38(void);
void sub_0809a1f4(void);
void sub_08098fb0(void);
void sub_0809a080(s32 a);

u8 sub_080988f8(void)
{
    switch (gCurTask->unk14)
    {
    case 0:
    case 1:
        sub_0809a080(1);
        gCurTask->unk28 = 1;
        break;
    case 4:
        sub_0809a080(1);
        gCurTask->unk7A = 0;
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(5);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 5:
        gCurTask->unk58 = -65536;
        break;
    case 13:
        sub_0809a080(0);
        ActorSetTerrainBox(gUnk_08745A14);
        ActorSetState(14);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 16:
        sub_0809a080(1);
        ActorSetState(0);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        TaskSetFrame(19);
        break;
    case 18:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(0);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08098a04(void)
{
    switch (gCurTask->unk14)
    {
    case 2:
        ActorSetState(7);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 3:
        ActorSetState(4);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 5:
        gCurTask->unk7A = 1;
        TaskStopY();
        ActorSetState(6);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    case 4:
    case 13:
        gCurTask->unk54 = 0;
        return 0;
    case 14:
        ActorSetState(15);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08098aa0(void)
{
    ActorSetHitReactions(gUnk_08745A80);
    gCurTask->unk18 = 0;
    ActorSetState(13);
    TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    return 1;
}

u8 sub_08098ad8(void)
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
        && gTasks[gCurTask->unk46].unk44 == gCurTaskIdx)
    {
        TaskFree(gCurTask->unk46);
        gCurTask->unk46 = 0;
    }
}

void sub_08098b60(void)
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
    TaskSetEntry(sub_08098fb0, gCurTaskIdx);
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
            gCurTask->unk54 = 0;
            ActorSetState(0);
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
        break;
    case 60:
        if (t->unk74 == 0 && RandomRange(2) == 0)
        {
            gCurTask->unk54 = 0;
            ActorSetState(0);
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
        break;
    case 120:
        if (t->unk74 == 1 && RandomRange(4) == 0)
        {
            gCurTask->unk54 = 0;
            ActorSetState(0);
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
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
    TaskSetEntry(sub_08098fb0, gCurTaskIdx);
}

void sub_08098d58(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    sp.unk00 = 13;
    sp.unk04 = 115;
    sp.unk08 = 0;
    sp.unk09 = t->facing;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = a->unk20;
    sp.unk0A = 1;
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_08098de4(void)
{
    switch (gCurTask->unk14)
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

void sub_08098e64(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    sub_08066088(0);
    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)sub_08065438;
    t->layer = 11;
    zero = 0;
    gCurTask->unk38 = gUnk_08753090;
    gUnk_02007D00[0]++;
    sub_080666cc(gUnk_08745624);
    u = gCurTask;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_08066ae0();
    CallTableEntry(gCurTask->unk73, 1, gUnk_08745630);
}

void sub_08098ed4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk04 = (u32)sub_08098f38;
    t->unk30 = 90;
    t->unk34 = 2;
    if (sub_08067060() != 0)
    {
        u = gCurTask;
        u->unk20 = 0;
        u->unk7A = 0;
        ActorSetState(18);
    }
    else
    {
        v = gCurTask;
        v->unk20 = 1;
        sub_08066580();
        ActorSetState(0);
    }
    CallTableEntry(gCurTask->unk14, 19, gUnk_08745634);
}

void sub_08098f38(void)
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
            sub_08066468();
        }
    }
    u = gCurTask;
    if (u->unk20 != 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 19, gUnk_08745680);
    }
    else
    {
        CallTableEntry(u->unk15, 19, gUnk_08745680);
    }
    sub_08098de4();
    sub_08068f68();
    ActorReactToHit();
}

void sub_08098fb0(void)
{
    sub_08098afc();
    CallTableEntry(gCurTask->unk14, 19, gUnk_08745634);
}

void sub_08098fd0(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->unk28 = 1;
    t->unk15 = 0;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(25);
        TaskYieldTrampoline(4);
        TaskSetFrame(4);
        TaskYieldTrampoline(16);
    }
}

void sub_08099004(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
        sub_08098b60();
}

void sub_08099020(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 1;
    u = gCurTask;
    u->unk7A = zero;
    v = gCurTask;
    v->unk28 = zero;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x70000;
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

void sub_08099080(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 != 0)
    {
        if (--t->unk30 == 0)
        {
            t->unk30 = 30;
            ActorSetState(0);
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
        else
        {
            ActorSetState(1);
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080990d4(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->unk30 = 160;
    zero = 0;
    t->unk15 = 2;
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_080991ac(void)
{
    struct Task *t;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 3;
    gCurTask->unk46 = sub_0806cc90(1, 10, -8, 24);
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

void sub_08099238(void)
{
    sub_08098c54();
}

void sub_08099244(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 4;
    u = gCurTask;
    u->unk7A = zero;
    TaskStop();
    v = gCurTask;
    v->unk60 = 0x2500;
    v->unk68 = 0x30000;
    TaskSetMotionXFacing(-49152, 0x5A5A5A5A);
    w = gCurTask;
    w->unk58 = -196608;
    RequestScreenShake(2);
    PlaySfx(0x1F7);
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_080992a8(void)
{
}

void sub_080992ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk30 = 32;
    t->unk15 = 5;
    TaskStop();
    u = gCurTask;
    u->unk60 = 0x8000;
    u->unk68 = 0x30000;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -65536;
    RequestScreenShake(2);
    gCurTask->unk46 = sub_0806cc90(0, 4, 8, 24);
    sub_0806caa0(0, 0, 24);
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099350(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->unk30 = 20;
    t->unk15 = 6;
    TaskStop();
    TaskSetFrame(24);
    while (1)
    {
        u = gCurTask;
        u->unk7A = 0;
        v = gCurTask;
        v->unk58 = -65536;
        TaskYieldTrampoline(2);
        w = gCurTask;
        w->unk58 = 0x10000;
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
        t->unk7A = 1;
        u = gCurTask;
        u->unk30 = 30;
        ActorSetTerrainBox(gUnk_08745A0C);
        ActorSetState(0);
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_080993dc(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 7;
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099474(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 8;
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

void sub_08099524(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 9;
    u = gCurTask;
    u->unk30 = 48;
    u->unk7A = 0;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -131072;
    sub_08098d58();
    while (1)
    {
        TaskSetFrame(10);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        TaskSetFrame(11);
        gCurTask->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0;
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
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080995f4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 10;
    u = gCurTask;
    u->unk30 = 48;
    u->unk7A = 0;
    PlaySfx(506);
    sub_08098d58();
    TaskTurnAround();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -131072;
    while (1)
    {
        TaskSetFrame(16);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        TaskSetFrame(17);
        gCurTask->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        TaskSetFrame(18);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0x1000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk60 = 0;
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
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_080996d0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 11;
    u = gCurTask;
    u->unk30 = 48;
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    PlaySfx(506);
    sub_08098d58();
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
            TaskSetEntry(sub_08098fb0, gCurTaskIdx);
        }
    }
}

void sub_08099770(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 12;
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099818(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 13;
    ActorSetHitReactions(gUnk_08745A80);
    u = gCurTask;
    u->unk7A = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
    sub_080667c0(1, 24);
    TaskSetMotionXFacing(-65536, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_0806caa0(0, -10, 24);
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_08099890(void)
{
    ActorFlashPalette(&gUnk_08274840, 16);
}

void sub_080998a4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 14;
    u = gCurTask;
    u->unk30 = 32;
    sub_0806caa0(1, 0, 0);
    gCurTask->unk46 = sub_0806cc90(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
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
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
    }
}

void sub_08099944(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 15;
    u = gCurTask;
    u->unk30 = zero;
    sub_0806caa0(1, 0, 0);
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

void sub_080999c4(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk60 = 0x5000;
    t->unk15 = 16;
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

void sub_08099a10(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk15 = 17;
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
    if (gCurTask->unk14 != 17)
        TaskSetEntry(sub_08098fb0, gCurTaskIdx);
}

void sub_08099a7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 18;
    u = gCurTask;
    u->unk30 = 24;
    u->unk60 = 0x5000;
    u->unk68 = 0x70000;
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
    if (gCurTask->unk14 == 0)
    {
        ActorSetState(22);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}
