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
 * sub_08099e9c and sub_08099ee4 build struct ActorSpawn records for the actors
 * 16 and 17; sub_0809a080 is the shared hit reaction (rumble RequestScreenShake(2)
 * or (4), then SE 0x1F7).  sub_08099fe0 and sub_08099fe4 are two dead `bx lr`
 * state handlers nothing in the ROM points at.
 *
 * The second script starts at sub_0809a0a8 (graphics gUnk_08753180, animation
 * gUnk_08745744, one-word table gUnk_0874574C): sub_0809a118 installs
 * sub_0809a17c and dispatches Task.unk14 through the 24-word guard table
 * gUnk_08745750, sub_0809a17c dispatches Task.unk15 through the 24-word body
 * table gUnk_087457B0 that follows it, and sub_0809a1f4 is its re-arm hook.
 * States 0-23 follow as <body, guard> pairs; sub_0809b438 is the timer leaf
 * the table word at 0x0874580C points at.
 *
 * sub_0809b528, sub_0809b7f0 and sub_0809ba00 are the three companion tasks
 * (graphics gUnk_0874CB7C, gUnk_087531C4, gUnk_087531DC).  They use
 * ActorDrawWorldInViewOrDestroy as the per-frame hook and Task.layer = 9; sub_0809b6ac and
 * sub_0809b964 read the parent's state out of gTasks[Task.unk44], and
 * the third one's own states live in the next module - gUnk_08745B1C points at
 * sub_0809ba44.  sub_0809b8ac is a dead copy of the gUnk_08745B04 re-arm.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern u8 gDigits[6];
extern vu16 gTaskSlotTypes[];

/* ROM tables */
extern u8 gUnk_087456D0[];
extern u8 gUnk_087456D2[];
extern u8 gUnk_087456D4[];
extern u8 gUnk_087456E4[];
extern u8 gUnk_087456F4[];
extern u8 gUnk_08745704[];
extern u8 gUnk_08745714[];
extern u8 gUnk_087456CC[];
extern u32 gUnk_08745964[];
extern u32 gUnk_08745980[];
extern u32 gUnk_087459D4[];
extern u32 gUnk_087459F0[];
extern struct AnimCmd gUnk_08745744[];
extern u32 gUnk_0874574C[];
extern u32 gUnk_08745750[];
extern u32 gUnk_087457B0[];
extern u32 gUnk_08745A1C[];
extern u32 gUnk_08745A24[];
extern u32 gUnk_08745A98[];
extern u32 gUnk_08745AE4[];
extern u32 gUnk_08745B00[];
extern u32 gUnk_08745B04[];
extern u32 gUnk_08745B1C[];
extern u32 gUnk_08745B08[];
extern u32 gUnk_08745BD0[];
extern u32 gUnk_08745BEC[];
extern u32 gUnk_08745C08[];
extern u32 gUnk_08745C24[];
extern u32 gUnk_08745C40[];
extern u32 gUnk_08745AE8[];
extern u32 gUnk_08745AF4[];
extern u32 gUnk_0874CB7C[];
extern u32 gUnk_08753180[];
extern u32 gUnk_087531C4[];
extern u32 gUnk_087531DC[];
extern void *gUnk_082797C8;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void IntToDigits(s16 n);
extern void TaskFree(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern s32 PlaySfx(s32 id);
extern void RequestScreenShake(s32 a);
extern void StopSfxOnPlayer(s32 player, s32 songId);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern u32 sub_08021a40(s32 x, s32 y);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void ActorSetTerrainBox(u32 *p);
extern void sub_08063a00(u32 *p);
extern s32 TaskGetDxTo(s32 i);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAround(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void ActorFlashPalette(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern void sub_08065438(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern u32 ActorCheckHits(void);
extern void ActorAttachEffect(s32 a, s32 b);
extern void sub_0806d65c(void);
extern void ActorDestroy(void);
extern void ActorMove(void);
extern u8 sub_08067060(void);
extern void sub_08068f68(void);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern void ActorDie(void);
extern void sub_0806ad18(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern void sub_0806ee2c(void);
extern void sub_08098afc(void);
extern void sub_0809baec(void);

/* Defined below */
void sub_0809a080(u8 a);
void sub_0809a17c(void);
void sub_0809a1f4(void);
void sub_0809b5b4(void);
void sub_0809b868(void);
void sub_0809b5ec(void);

u8 sub_08099b20(void)
{
    switch (gCurTask->unk14)
    {
    case 9:
        RequestScreenShake(1);
        PlaySfx(0x1F7);
        ActorSetState(10);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 10:
        sub_0809a080(1);
        ActorSetState(11);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 4:
        TaskSetFrame(4);
        sub_0809a080(1);
        sub_0806cffc(0, 16);
        TaskStop();
    stop:
        gCurTask->unk28 = 1;
        break;
    case 2:
        TaskSetFrame(4);
        sub_0809a080(1);
        sub_0806cffc(0, 16);
        goto stop;
    case 22:
        sub_0809a080(1);
        ActorSetState(0);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 7:
        sub_0809a080(1);
        ActorSetState(8);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 15:
        sub_0809a080(1);
        ActorSetState(16);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 19:
        sub_0809a080(0);
        ActorSetState(20);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 23:
        sub_0809a080(0);
        sub_08066580();
        ActorSetState(0);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08099c4c(void)
{
    switch (gCurTask->unk14)
    {
    case 17:
        ActorSetState(18);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 4:
        gCurTask->unk54 = 0;
        return 0;
    case 5:
        ActorSetState(6);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 6:
        ActorSetState(15);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    case 3:
        gCurTask->unk54 = -gCurTask->unk54;
        break;
    case 12:
        ActorSetState(15);
        ActorSetTerrainBox(gUnk_08745A24);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        return 1;
    }
    return 0;
}

u8 sub_08099d40(void)
{
    gCurTask->unk2C = 32;
    CreateChildTaskHere(142, 0);
    RequestScreenShake(4);
    return 0;
}

u8 sub_08099d64(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk14 == 11)
        StopSfxOnPlayer(t->unk1C, 0x219);
    ActorSetHitReactions(gUnk_08745A98);
    u = gCurTask;
    u->unk18 = 0;
    ActorSetState(19);
    TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
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
    if (gCurTask->unk14 == 2)
        gCurTask->unk30 = gUnk_087456D0[RandomRange(2)];
    TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
}

s32 sub_08099e9c(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    sp.subtype = 16;
    sp.taskType = 118;
    sp.unk08 = 0;
    sp.unk09 = t->facing;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 0;
    return CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void sub_08099ee4(u8 a)
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
        if (sub_08021a40(t->unk48 - ((s8)t->facing << 4), t->unk4A) != 0)
            return;
        break;
    case 1:
    case 3:
        t = gCurTask;
        if (sub_08021a40(t->unk48 + ((s8)t->facing << 4), t->unk4A) != 0)
            return;
        break;
    }
    act = gCurTask->unk8C;
    zero = 0;
    gCurTask->unk70 = RandomRange(4);
    gCurTask->unk6C = (s8)gUnk_087456CC[(s16)gCurTask->unk70];
    sp.subtype = 17;
    sp.taskType = 119;
    sp.unk08 = zero;
    sp.unk09 = a;
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
    t->unk54 = -t->unk54;
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
        t->unk58 = 0x8000;
        break;
    case 1:
        t->unk58 = 0x10000;
        break;
    }
}

void sub_0809a00c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk7A = 0;
    u = gCurTask;
    switch (u->unk28)
    {
    case 0:
        u->unk58 = -32768;
        break;
    case 1:
        u->unk58 = -65536;
        break;
    }
}

void sub_0809a03c(void)
{
    switch (gCurTask->unk14)
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

void sub_0809a0a8(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    sub_08066088(0);
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)sub_08065438;
    t->layer = 11;
    zero = 0;
    u = gCurTask;
    u->unk38 = gUnk_08753180;
    gUnk_02007D00[0]++;
    u->unk18 = 1;
    u->unk46 = zero;
    sub_080666cc(gUnk_08745744);
    sub_08066ae0();
    CallTableEntry(gCurTask->unk73, 1, gUnk_0874574C);
}

void sub_0809a118(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk04 = (u32)sub_0809a17c;
    if (sub_08067060() != 0)
    {
        u = gCurTask;
        u->unk20 = 0;
        u->unk7A = 0;
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
    CallTableEntry(gCurTask->unk14, 24, gUnk_08745750);
}

void sub_0809a17c(void)
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
            sub_08066468();
        }
    }
    u = gCurTask;
    if (u->unk20 != 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 24, gUnk_087457B0);
    }
    else
    {
        CallTableEntry(u->unk15, 24, gUnk_087457B0);
    }
    sub_0809a03c();
    sub_08068f68();
    ActorReactToHit();
}

void sub_0809a1f4(void)
{
    sub_08098afc();
    CallTableEntry(gCurTask->unk14, 24, gUnk_08745750);
}

void sub_0809a214(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = zero;
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

void sub_0809a270(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk30 < 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a2a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 1;
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

void sub_0809a2e8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 2;
    u = gCurTask;
    u->unk28 = zero;
    u->unk7A = zero;
    TaskSetFrame(6);
    TaskStop();
    v = gCurTask;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x70000;
    TaskSleepForever();
}

void sub_0809a32c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 != 0)
    {
        if (--t->unk30 <= 0)
            ActorSetState(1);
        else
            ActorSetState(2);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a36c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 3;
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a464(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 4;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk30 = 16;
    u->unk7A = zero;
    v = gCurTask;
    v->unk28 = zero;
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    w = gCurTask;
    w->unk58 = -327680;
    w->unk60 = 0x3700;
    w->unk68 = 0x30000;
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
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a528(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 5;
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809a654(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 6;
    u = gCurTask;
    u->unk28 = zero;
    gCurTask->unk46 = sub_0806cc90(1, 10, -12, 16);
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

void sub_0809a744(void)
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
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
        else
        {
            ActorSetState(7);
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a798(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 7;
    u = gCurTask;
    u->unk7A = zero;
    TaskStop();
    v = gCurTask;
    v->unk58 = -327680;
    v->unk60 = 0x5000;
    v->unk68 = 0x30000;
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0809a7d8(void)
{
}

void sub_0809a7dc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 8;
    u = gCurTask;
    u->unk30 = 120;
    gCurTask->unk6C = sub_08099e9c();
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
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809a868(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 9;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk7A = zero;
    TaskStop();
    v = gCurTask;
    v->unk58 = -163840;
    v->unk60 = 0x3000;
    v->unk68 = 0x30000;
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSleepForever();
}

void sub_0809a8b4(void)
{
}

void sub_0809a8b8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 10;
    TaskFaceNearestPlayer();
    u = gCurTask;
    u->unk7A = zero;
    TaskStop();
    v = gCurTask;
    v->unk58 = -163840;
    v->unk60 = 0x3000;
    v->unk68 = 0x30000;
    TaskSetMotionXFacing(-81920, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0809a918(void)
{
}

void sub_0809a91c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 11;
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
        sub_08099ee4(0);
        break;
    case 76:
        PlaySfx(0x1FB);
        sub_08099ee4(1);
        break;
    case 52:
        PlaySfx(0x1FB);
        sub_08099ee4(2);
        break;
    case 28:
        PlaySfx(0x1FB);
        sub_08099ee4(3);
        break;
    case 0:
        StopSfxOnPlayer(t->unk1C, 0x219);
        if (sub_08099db0() == 0)
        {
            ActorSetState(14);
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
        break;
    }
}

void sub_0809aa24(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 12;
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
    if (gCurTask->unk30 < 0)
    {
        v = TaskGetNearestPlayerDx();
        if (v < 0)
            v = -v;
        if (v <= 32)
        {
            ActorSetState(7);
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
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
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809ab70(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 13;
    u = gCurTask;
    u->unk30 = 120;
    u->unk70 = u->unk54;
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
    sub_08099e9c();
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
    b = t->unk54;
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
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
    v = gCurTask;
    v->unk70 = v->unk54;
}

void sub_0809acbc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 14;
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
            if (sub_08021a40(u->unk48 - ((s8)u->facing << 4), u->unk4A) != 0)
            {
                ActorSetState(18);
                TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
            }
            else
            {
                ActorSetState(17);
                TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
            }
        }
        else
        {
            ActorSetState(1);
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809adf4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 15;
    u = gCurTask;
    u->unk7A = zero;
    v = gCurTask;
    v->unk54 = -65536;
    v->unk58 = -196608;
    v->unk60 = 0x2500;
    v->unk68 = 0x30000;
    RequestScreenShake(2);
    TaskSetFrame(7);
    TaskSleepForever();
}

void sub_0809ae3c(void)
{
}

void sub_0809ae40(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 16;
    u = gCurTask;
    u->unk30 = zero;
    u->unk28 = zero;
    sub_0806caa0(0, 0, 24);
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
        t->unk58 = 0x10000;
        ActorSetTerrainBox(gUnk_08745A1C);
        ActorSetState(14);
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        break;
    }
}

void sub_0809af4c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 17;
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
    if ((gCurTask->unk30 & 1) != 0)
    {
        if (sub_08099fb4() == 0)
        {
            ActorSetState(1);
            TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
        }
    }
}

void sub_0809b104(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk15 = 17;
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

void sub_0809b214(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 19;
    ActorSetHitReactions(gUnk_08745A98);
    ActorSetTerrainBox(gUnk_08745A24);
    u = gCurTask;
    u->unk7A = zero;
    if (--gUnk_02007D00[0] <= 0)
        sub_0806684c();
    sub_080667c0(1, 7);
    TaskSetMotionXFacing(-65536, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
    sub_0806caa0(0, -10, 24);
    TaskSetFrame(7);
    TaskSleepForever();
}

void sub_0809b298(void)
{
    ActorFlashPalette(&gUnk_082797C8, 16);
}

void sub_0809b2ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 20;
    u = gCurTask;
    u->unk30 = 32;
    sub_0806caa0(1, 0, 0);
    gCurTask->unk46 = sub_0806cc90(0, 4, 8, 24);
    TaskStop();
    TaskSetMotionXFacing(-32768, 0x5A5A5A5A);
    v = gCurTask;
    v->unk58 = -196608;
    v->unk60 = 0x1A00;
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
        TaskSetEntry(sub_0809a1f4, gCurTaskIdx);
    }
}

void sub_0809b34c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = 21;
    u = gCurTask;
    u->unk30 = zero;
    sub_0806caa0(1, 0, 0);
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

void sub_0809b3d4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 22;
    u = gCurTask;
    u->unk60 = 0x5000;
    u->unk68 = 0x30000;
    TaskSetFrame(11);
    TaskYieldTrampoline(8);
    TaskSleepForever();
}

void sub_0809b404(void)
{
}

void sub_0809b408(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 23;
    u = gCurTask;
    u->unk30 = 24;
    u->unk60 = 0x5000;
    u->unk68 = 0x30000;
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
    if (t->unk14 == 0)
    {
        ActorSetState(2);
        TaskSetEntry(sub_0809b5ec, gCurTaskIdx);
        return 1;
    }
    t->unk7A = 0;
    u = gCurTask;
    switch (u->unk28)
    {
    case 0:
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        v = gCurTask;
        v->unk58 = -196608;
        v->unk60 = 0x1E00;
        break;
    case 1:
        TaskSetMotionXFacing(0x2A000, 0x5A5A5A5A);
        v = gCurTask;
        v->unk58 = -98304;
        v->unk60 = 0x1E00;
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
    TaskSetEntry(sub_0809b5ec, gCurTaskIdx);
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

void sub_0809b528(void)
{
    struct Task *t;
    struct Task *u;
    u16 zero;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    u = gCurTask;
    u->unk70 = u->unk40;
    u->unk38 = gUnk_0874CB7C;
    u->unk40 = zero;
    CallTableEntry(u->unk73, 1, gUnk_08745AE4);
}

void sub_0809b57c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = (u32)sub_0809b5b4;
    t->facing = t->unk74;
    ActorSetState(0);
    u = gCurTask;
    CallTableEntry(u->unk14, 3, gUnk_08745AE8);
}

void sub_0809b5b4(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_08745AF4);
    if (gCurTask->unk14 != 2)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_0809b5ec(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08745AE8);
}

void sub_0809b608(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = zero;
    TaskSetFrameNoFlip(4);
    u = gCurTask;
    u->unk7A = zero;
    v = gCurTask;
    v->unk28 = zero;
    v->unk68 = 0x30000;
    PlaySfx(506);
    ActorAttachEffect(3, 0);
    gCurTask->unk58 = -262144;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -131072;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -65536;
    TaskYieldTrampoline(8);
    ActorSetAttackBox(gUnk_08745BD0);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x30000;
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
        t->unk2C = (gTasks + t->unk44)->unk14;
        if (t->unk2C == 12)
        {
            ActorSetState(1);
            TaskSetEntry(sub_0809b5ec, gCurTaskIdx);
        }
    }
}

void sub_0809b6f8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->unk8C;
    zero = 0;
    t->unk15 = 1;
    u = gCurTask;
    u->unk68 = 0x30000;
    u->unk7A = zero;
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
        v->unk58 = -196608;
        v->unk60 = 0x1E00;
        break;
    case 1:
        TaskSetMotionXFacing(0x2A000, 0x5A5A5A5A);
        v = gCurTask;
        v->unk58 = -98304;
        v->unk60 = 0x1E00;
        break;
    }
    TaskSleepForever();
}

void sub_0809b790(void)
{
}

void sub_0809b794(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->unk8C;
    zero = 0;
    t->unk15 = 2;
    u = gCurTask;
    u->unk28 = zero;
    u->unk7A = zero;
    TaskStop();
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    TaskSetFrameNoFlip(4);
    TaskYieldTrampoline(2);
    sub_0806d65c();
    ActorDestroy();
}

void sub_0809b7ec(void)
{
}

void sub_0809b7f0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->unk38 = gUnk_087531C4;
    CallTableEntry(u->unk73, 1, gUnk_08745B00);
}

void sub_0809b830(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = (u32)sub_0809b868;
    t->facing = t->unk74;
    ActorSetState(0);
    u = gCurTask;
    CallTableEntry(u->unk14, 1, gUnk_08745B04);
}

void sub_0809b868(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08745B08);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0809b8ac(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08745B04);
}

void sub_0809b8c8(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->unk15 = zero;
    TaskSetFrame(4);
    u = gCurTask;
    u->unk7A = zero;
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
    t->unk48 = (gTasks + t->unk44)->unk48;
    t->unk28 = (gTasks + t->unk44)->unk14;
    if (t->unk28 != 8 && t->unk28 != 13)
    {
        StopSfxOnPlayer(t->unk2C, 0x219);
        ActorDestroy();
    }
}

u8 sub_0809b9c0(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_0809baec, gCurTaskIdx);
    return 1;
}

u8 sub_0809b9e0(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_0809baec, gCurTaskIdx);
    return 1;
}

void sub_0809ba00(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->unk38 = gUnk_087531DC;
    u->facing = 1;
    CallTableEntry(gCurTask->unk73, 1, gUnk_08745B1C);
}
