/* game_code_and_rodata 0x08091F9C-0x08093F64 (issue #67, module M25 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08091F9C 0x08093F64 src/enemy_91f9c.c --newpb
 *
 * M25's third and fourth boss scripts.  The third (entry sub_08091f08 in
 * src/enemy_91f08.c, table 0x08743ADC) starts here with sub_08091f9c, which
 * installs the per-frame body sub_08091ffc and the animation script
 * gUnk_08743AC8.  sub_08091ffc is the busiest body in the module: besides the
 * usual Task.unk15 dispatch it calls sub_080227a4 (the camera/room hook) on
 * entry, and when the row gUnk_08743A58[Task.unk34] is non-null it runs the
 * "hit the wall" transition - sub_0806914c, then Task.unk1C = Task.unk7E,
 * a re-seat of the actor at gUnk_030023B4 - Task.unk43 * 16, and a hand-off to
 * sub_080685ec / TaskSetEntry.
 *
 * States 0-12 follow as <body, guard> pairs.  sub_08092250 is the attack
 * chooser: it walks Task.unk6C over gUnk_08743A70[Task.unk74] rounds, and per
 * round stores |TaskGetNearestPlayerDx()| in gUnk_03001F2C, classifies it into
 * gUnk_02007D00[6] (0/1/2) against the RNG, and plays one of three yield
 * sequences; sub_080926fc is the three-phase charge, sub_08092cdc the
 * multi-hit dive, sub_080930ac the four-way finisher whose case 3 spawns the
 * actor 154 at gUnk_030023B4/gUnk_030023D4, and sub_08093380 the defeat
 * sequence.  sub_080934b8 is the shake helper the first states yield to and
 * sub_080934f8 is the collision probe: ten sub_08021bb4 samples along
 * gUnk_08743AB8, mapped through the terrain-class table gUnk_087339F0 into a
 * two-bit result that picks the next Task.unk28 direction from gUnk_08743AC2.
 * sub_0809364c / sub_080936a0 / sub_08093780 are the shared step sequences,
 * sub_080937d0 the hit hook, sub_08093858 the four-instruction "stop moving"
 * leaf the census had missed, and sub_0809397c the companion body.
 *
 * The fourth boss starts at sub_08093a24 (table 0x087441A4, graphics
 * gUnk_08752F60): sub_08093a64 installs sub_08093a98 as its body,
 * sub_08093ac8 is its one state, sub_08093bd4 / sub_08093c30 are the second
 * entry pair (graphics gUnk_08753160, Actor.unk38 = 0x20E), sub_08093ccc and
 * sub_08093dcc are the endless spawners that call CreateChildTaskAtOffsetFacing(181, -8, -8, 1)
 * every six frames, and sub_08093cf8 / sub_08093e58 / sub_08093f00 are the
 * companions that copy the boss's 16.16 position (±8 rows) and expire with it.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern s32 gUnk_03001F2C;
extern u16 gFrameCount;
extern struct PlayerState gUnk_03002170[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern u16 gLocalPlayer;
extern vs16 gTaskSlotTypes[];

/* ROM tables */
extern u8 gUnk_08743A70[];
extern u8 gUnk_08743A8E[];
extern s32 gUnk_08743A9C[];
extern u32 gUnk_08743AA4[];
extern u32 gUnk_08743AAC[];
extern u8 gUnk_08743AB4[];
extern s8 gUnk_08743AB8[];
extern s8 gUnk_08743AC2[];
extern s8 gUnk_087339F0[];
extern u32 gUnk_0874410C[];
extern u32 gUnk_08744170[];
extern u32 gUnk_08744174[];
extern u32 gUnk_08744178[];
extern u32 gUnk_08752F60[];
extern u32 gUnk_0874430C[];
extern u32 gUnk_08753160[];
extern u32 gUnk_087441A4[];
extern u32 gUnk_087441AC[];
extern u32 gUnk_0874417C[][2];
extern u32 gUnk_0874418C[][2];
extern u32 gUnk_08744324[];
extern u32 gUnk_0874419C[];
extern u32 gUnk_087441B4[];
extern u8 gUnk_08743B48[];
extern u32 gUnk_087536FC[];
extern vu8 gUnk_03005550;
extern u32 gUnk_08743A94[];
extern u32 gUnk_08743A74[];
extern u8 gUnk_08743A7C[];
extern u8 gUnk_08743A82[];
extern u8 gUnk_08743A88[];
extern u32 gUnk_08743A10[];
extern u32 gUnk_08743A28[];
extern u32 gUnk_08743A40[];
extern u32 gUnk_08743A58[];
extern u32 gUnk_08743B14[];
extern void *gUnk_082959A8;
extern struct AnimCmd gUnk_08743AC8[];
extern u32 gUnk_08743AE0[];
extern struct AnimCmd gUnk_0874397C[];
extern u32 gUnk_08743988[];
extern u32 gUnk_087439A4[];
extern u32 gUnk_087440F4[];
extern struct AnimCmd *gUnk_08743A00[];
extern u32 gUnk_08743ADC[];
extern u32 gUnk_087535FC[];
extern u32 gUnk_08753128[];
extern u32 gUnk_08753148[];
extern u8 gUnk_087438DC[];
extern u32 gUnk_087438E4[];
extern u32 gUnk_087438EC[];
extern u32 gUnk_0874391C[];
extern u32 gUnk_0874394C[];
extern struct GfxHeader gUnk_0827565C;
extern u32 gUnk_08275670;

/* Externals */
extern void sub_080227a4(struct Task *t);
extern u32 sub_0806914c(s32 a);
extern void TaskFaceToward(u32 i);
extern u16 sub_0806660c(u16 a);
extern u16 sub_080665fc(void);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void sub_08068920(s32 i, u8 c);
extern s32 sub_08021bb4(s16 x, s16 y, s32 c, s32 d);
extern void TaskGetPosSlot(u32 i);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern s32 TaskFindNearestPlayer(void);
extern void TaskYieldTrampoline(u32 a);
extern void TaskExitTrampoline(void);
extern void TaskFaceLikeParent(void);
extern u16 sub_08066630(u16 a);
extern void ActorDrawWorldInView(void);
extern void TaskMove(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorCheckHits(void);
extern void TaskTurnAroundAndReverseX(void);
extern void sub_080689c8(s32 i, s32 d);
extern void sub_080653ec(void);
extern void ActorMove(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskFree(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionX(s32 a, s32 b, s32 c);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopX(void);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern void sub_080261d4(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskFaceNearestPlayer(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorStepAnim(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void ActorFlashPalette(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066480(struct GfxHeader *h, u32 src, u32 size);
extern void sub_080664cc(struct GfxHeader *h);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern void ActorDie(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern s32 Div(s32 numerator, s32 denominator);

/* Defined below */
void sub_08091fe0(void);
void sub_08091ffc(void);
void sub_080934b8(void);
void sub_080934f8(void);
void sub_0809364c(void);
void sub_080936a0(void);
void sub_08093780(void);
void sub_08093a00(s32 a);
void sub_0809397c(void);
void sub_08093a98(void);
void sub_08093c7c(void);
void sub_08093c60(void);
void sub_08091f9c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = (u32)sub_08091ffc;
    sub_080666cc(gUnk_08743AC8);
    u = gCurTask;
    u->unk24 = u->unk8C->unk28;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 13, gUnk_08743AE0);
}

void sub_08091fe0(void)
{
    CallTableEntry(gCurTask->unk14, 13, gUnk_08743AE0);
}

void sub_08091ffc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 m;

    sub_080227a4(gCurTask);
    t = gCurTask;
    if (t->unk20 != 0)
    {
        t->unk20--;
        CallTableEntry(t->unk15, 13, gUnk_08743B14);
    }
    else if (sub_0806acf8() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 13, gUnk_08743B14);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 13, gUnk_08743B14);
    }
    u = gCurTask;
    if (u->unk30 == 1)
    {
        if (u->unk75 != 0)
            ActorFlashPalette(&gUnk_082959A8, 16);
        else
        {
            u->unk30 = 0;
            sub_08066468();
        }
    }
    ActorSetAttackBox(gUnk_08743A10[gCurTask->unk34]);
    sub_080639f0(gUnk_08743A28[gCurTask->unk34]);
    sub_08063a00(gUnk_08743A40[gCurTask->unk34]);
    sub_08068f68();
    ActorReactToHit();
    if (gUnk_08743A58[gCurTask->unk34] != 0)
    {
        sub_0806914c(gUnk_08743A58[gCurTask->unk34]);
        v = gCurTask;
        if (v->unk7C == 8)
        {
            v->unk1C = v->unk7E;
            TaskFaceToward(v->unk1C);
            w = gCurTask;
            w->unk34 = 5;
            w->unk18 = 0;
            TaskGetPosSlot(w->unk1C);
            x = gCurTask;
            x->unk48 = gUnk_030023B4 - x->unk43 * 16;
            m = x->unk48;
            x->unk4C = m << 16;
            sub_080227a4(x);
            y = gCurTask;
            y->unk8C->unk28 = y->unk24;
            TaskSetFrame(36);
            if (gCurTask->unk1C == gLocalPlayer)
                PlaySfx(0x23D);
            sub_080685ec(gCurTask->unk1C, gCurTaskIdx, 3);
            ActorSetState(10);
            TaskSetEntry(sub_08091fe0, gCurTaskIdx);
        }
    }
}

void sub_08092198(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 0;
    TaskSetFrame(58);
    u = gCurTask;
    u->unk60 = 9472;
    u->unk7A = 0;
    TaskYieldTrampoline(1);
    if (gCurTask->unk7A == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gCurTask->unk7A == 0);
        sub_080261d4(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    sub_08066580();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08092228(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}
void sub_08092250(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 r;
    s32 zero;

    t = gCurTask;
    t->unk15 = 1;
    u = gCurTask;
    u->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_08743A70[gCurTask->unk74])
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
            gCurTask->unk54 = 0;
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            gCurTask->unk3C++;
            TaskYieldTrampoline(7);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(7);
            break;
        case 1:
            TaskSetMotionXFacing(-gUnk_08743A74[gCurTask->unk74], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(3);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(3);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            break;
        case 2:
            TaskSetMotionXFacing(gUnk_08743A74[gCurTask->unk74], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            TaskSetFrame(8);
            break;
        }
        gCurTask->unk6C++;
    }
    v = gCurTask;
    zero = 0;
    v->unk54 = zero;
    if (++v->unk2C > 2)
    {
        v->unk2C = zero;
        ActorSetState(2);
    }
    else
    {
        gUnk_02007D00[6] = gUnk_03002170[gUnk_02007D00[4]].unk04;
        if (gUnk_02007D00[6] == 14)
            gUnk_02007D00[5] = 2;
        else if (gUnk_02007D00[6] == 4)
            gUnk_02007D00[5] = 1;
        else
            gUnk_02007D00[5] = zero;
        if (gCurTask->unk74 != 0)
            gUnk_02007D00[5] += 3;
        r = RandomRange(8);
        gUnk_02007D00[6] = r;
        if (r < gUnk_08743A7C[gUnk_02007D00[5]])
            ActorSetState(3);
        else if (r < gUnk_08743A82[gUnk_02007D00[5]])
            ActorSetState(4);
        else if (r < gUnk_08743A88[gUnk_02007D00[5]])
            ActorSetState(6);
        else
            ActorSetState(7);
    }
    TaskSleepForever();
}
void sub_08092590(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_080925b8(void)
{
    struct Task *t;
    struct ActorSpawn spawn;

    t = gCurTask;
    t->unk15 = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(29);
    TaskYieldTrampoline(12);
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    spawn.unk00 = 30;
    spawn.unk04 = 133;
    spawn.unk08 = 0;
    spawn.unk09 = gCurTask->unk74;
    spawn.unk0C = 0xFFFE;
    spawn.unk0E = 0;
    spawn.unk10 = sub_0806660c(1);
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(24);
    spawn.unk00 = 30;
    spawn.unk04 = 133;
    spawn.unk08 = 1;
    spawn.unk09 = gCurTask->unk74;
    spawn.unk0C = 0xFFFE;
    spawn.unk0E = 0;
    spawn.unk10 = sub_0806660c(1);
    spawn.unk0A = 0;
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    TaskYieldTrampoline(4);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 1);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080926d4(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}
void sub_080926fc(void)
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
    t->unk15 = 3;
    TaskFaceNearestPlayer();
    PlaySfx(0x23F);
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_08743A8E[gCurTask->unk74])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        if (gCurTask->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u = gCurTask;
        u->unk24 = u->unk8C->unk28;
        u->unk8C->unk28 = 0;
        u->unk3C++;
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->unk8C->unk28 = v->unk24;
        gCurTask->unk6C++;
    }
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        if (gCurTask->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u2 = gCurTask;
        u2->unk24 = u2->unk8C->unk28;
        u2->unk8C->unk28 = 0;
        u2->unk3C++;
        TaskYieldTrampoline(2);
        v2 = gCurTask;
        v2->unk8C->unk28 = v2->unk24;
    } while ((s16)++gCurTask->unk6C <= 0);
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_08743A8E[gCurTask->unk74])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        if (gCurTask->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u3 = gCurTask;
        u3->unk24 = u3->unk8C->unk28;
        u3->unk8C->unk28 = 0;
        u3->unk3C++;
        TaskYieldTrampoline(2);
        v3 = gCurTask;
        v3->unk8C->unk28 = v3->unk24;
        gCurTask->unk6C++;
    }
    w = gCurTask;
    zero = 0;
    w->unk54 = zero;
    w->unk3C++;
    TaskYieldTrampoline(12);
    PlaySfx(500);
    x = gCurTask;
    x->unk34 = 3;
    gTasks[CreateChildTask(200, x->unk48, x->unk4A, sub_080665fc())].unk74 =
        gCurTask->unk74;
    gCurTask->unk18 = 1;
    TaskSetMotionXFacing(gUnk_08743A94[gCurTask->unk74], 0x5A5A5A5A);
    gCurTask->unk6C = zero;
    do
    {
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
        TaskSetFrame(26);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 9);
    TaskSetMotionXFacing(0x5A5A5A5A, 0xFFFFC000);
    while (1)
    {
        TaskYieldTrampoline(1);
        y = gCurTask;
        if (y->unk43 == 1)
        {
            if (y->unk54 <= 0)
                break;
        }
        else if (y->unk54 >= 0)
        {
            break;
        }
    }
    TaskStopX();
    gCurTask->unk34 = 1;
    ActorSetState(8);
    TaskSleepForever();
}
void sub_080929ec(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_08092a14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->unk15 = 4;
    TaskSetFrame(60);
    TaskYieldTrampoline(6);
    v = gCurTask;
    v->unk3C--;
    TaskYieldTrampoline(10);
    gCurTask->unk7A = 0;
    u = gCurTask;
    u->unk58 = gUnk_08743A9C[u->unk74];
    u->unk60 = 9472;
    if (u->unk58 < 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while (gCurTask->unk58 < 0);
    }
    TaskStopY();
    w = gCurTask;
    if (w->unk74 == 0)
    {
        gUnk_02007D00[4] = 2;
        w->unk6C = 0;
        do
        {
            sub_080934b8();
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 59);
    }
    if (abs(TaskGetNearestPlayerDx()) <= 31)
        ActorSetState(8);
    else if (abs(TaskGetNearestPlayerDy()) <= 15)
        ActorSetState(3);
    else
        ActorSetState(5);
    TaskSleepForever();
}

void sub_08092b30(void)
{
    if (gCurTask->unk14 != 4)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_08092b58(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk15 = 5;
    TaskSetMotionXFacing(gUnk_08743AA4[gCurTask->unk74], 0x5A5A5A5A);
    if (TaskGetYDirBitToNearestPlayer() == 1)
        gCurTask->unk58 = 49152;
    gUnk_02007D00[4] = 2;
    gCurTask->unk6C = 0;
    do
    {
        sub_080934b8();
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 119);
    ActorSetState(8);
    TaskSleepForever();
}

void sub_08092bd8(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_08092c00(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 6;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->unk3C--;
    TaskYieldTrampoline(10);
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(gUnk_08743AAC[gCurTask->unk74], 0x5A5A5A5A);
    u = gCurTask;
    u->unk58 = 0xFFFC0000;
    u->unk60 = 9472;
    u->unk6C = 0;
    do
    {
        if ((gFrameCount & 2) != 0)
            TaskSetFrame(52);
        else
            TaskSetFrame(61);
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 13);
    ActorSetState(8);
    TaskSleepForever();
}

void sub_08092cb4(void)
{
    if (gCurTask->unk14 != 6)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}
void sub_08092cdc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    t = gCurTask;
    t->unk15 = 7;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_02007D00[4] =
        gUnk_08743AB4[RandomRange(2) + gCurTask->unk74 * 2];
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_02007D00[4])
    {
        gCurTask->unk7A = 0;
        v = gCurTask;
        v->unk58 = 0xFFFB0000;
        v->unk60 = 20480;
        v->unk6C = 0;
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 13);
        while (gCurTask->unk7A == 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        }
        TaskStopY();
        sub_080261d4(2);
        PlaySfx(0x1F7);
        TaskSetFrame(59);
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    }
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    p = &gTasks[TaskFindNearestPlayer()];
    if (p->unk7A == 0)
        ActorSetState(4);
    else
        ActorSetState(3);
    TaskSleepForever();
}
void sub_08092e40(void)
{
    if (gCurTask->unk14 != 7)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_08092e68(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 8;
    u = gCurTask;
    u->unk60 = 9472;
    if (u->unk7A == 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        } while (gCurTask->unk7A == 0);
        sub_080261d4(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08092f04(void)
{
    if (gCurTask->unk14 != 8)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_08092f2c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 9;
    TaskStop();
    sub_080261d4(4);
    PlaySfx(0x1F7);
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    u = gCurTask;
    u->unk58 = 0xFFFD0000;
    u->unk60 = 9472;
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    v = gCurTask;
    v->unk3C--;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080261d4(4);
    PlaySfx(0x1F7);
    sub_0806cc90(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    gCurTask->unk54 = 0;
    sub_08093780();
    ActorSetState(11);
    TaskSleepForever();
}

void sub_08092ff4(void)
{
    if (gCurTask->unk14 != 9)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}

void sub_0809301c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 11;
    TaskSetMotionXFacing(0xFFFFC000, 0x5A5A5A5A);
    TaskSetFrame(48);
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0;
    TaskSetFrame(48);
    TaskYieldTrampoline(10);
    TaskSetFrame(50);
    TaskYieldTrampoline(8);
    TaskSetFrame(43);
    TaskYieldTrampoline(16);
    u = gCurTask;
    u->unk34 = 1;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08093084(void)
{
    if (gCurTask->unk14 != 11)
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
}
void sub_080930ac(void)
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
    t->unk15 = 10;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->unk54 = 131072;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->unk3C++;
    u->unk54 = 131072;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    TaskStop();
    v = gCurTask;
    v->unk60 = 9472;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_080934f8();
    s = gCurTask;
    switch (s->unk28)
    {
    case 0:
        TaskSetMotionXFacing(81920, 0x5A5A5A5A);
    case 1:
        gCurTask->unk7A = 0;
        w = gCurTask;
        w->unk58 = 0xFFFC0000;
        w->unk60 = 9472;
        TaskSetFrame(51);
        TaskYieldTrampoline(13);
        while (gCurTask->unk58 < 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        }
        x = gCurTask;
        x->unk58 = 262144;
        x->unk60 = 0;
        TaskSetFrame(53);
        while (gCurTask->unk7A == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        sub_0809364c();
        sub_080936a0();
        break;
    case 2:
        s->unk7A = 0;
        TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
        y = gCurTask;
        y->unk58 = 0xFFFC0000;
        y->unk60 = 16384;
        TaskSetFrame(45);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        TaskSetFrame(49);
        TaskYieldTrampoline(4);
        TaskSetFrame(47);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        while (gCurTask->unk7A == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        sub_0809364c();
        ActorSetState(11);
        break;
    case 3:
        s->unk6C = 0;
        do
        {
            TaskSetFrame(38);
            TaskYieldTrampoline(6);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            sub_080261d4(2);
            sub_08093a00(504);
            TaskGetPosSlot(gCurTask->unk1C);
            CreateChildTaskAt(154, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
            TaskYieldTrampoline(8);
        } while ((s16)++gCurTask->unk6C <= 5);
        sub_08068920(gCurTask->unk1C, 4);
        sub_080936a0();
        break;
    }
    TaskSleepForever();
}

void sub_08093354(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk14 != 10)
    {
        t->unk1C = -1;
        TaskSetEntry(sub_08091fe0, gCurTaskIdx);
    }
}

void sub_08093380(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 12;
    gUnk_02007D00[4] = 0;
    if (--gUnk_02007D00[7] == 0)
        sub_0806684c();
    sub_080667c0(1, 63);
    sub_0806caa0(1, 0, 0);
    TaskStop();
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 9472, 196608);
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->unk3C--;
    TaskYieldTrampoline(15);
    gCurTask->unk34 = 4;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_0806caa0(0, 0, 10);
    sub_080261d4(4);
    PlaySfx(0x1F7);
    sub_0806cc90(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    TaskStopX();
    TaskYieldTrampoline(170);
    sub_0806caa0(1, 0, 0);
    sub_0806ad18();
    gUnk_02007D00[4] = 1;
    TaskSleepForever();
}

void sub_08093488(void)
{
    ActorFlashPalette(&gUnk_082959A8, 16);
    if (gUnk_02007D00[4] == 1)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_080934b8(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    if (--gUnk_02007D00[4] == 0)
    {
        t = gCurTask;
        if (++t->unk3C > 35)
            t->unk3C = 34;
        TaskUpdateFlip();
        gUnk_02007D00[4] = 2;
    }
}
void sub_080934f8(void)
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
        m = sub_08021bb4(t->unk48, t->unk4A, gUnk_08743AB8[i] * t->unk43, 0);
        if (m == -1)
            m = 1;
        if (gUnk_087339F0[m] != 0)
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
            if (u->unk28 != 0)
                u->unk28 = f;
            else
                u->unk28 = 2;
        }
        else if ((s16)n <= 6)
        {
            u = gCurTask;
            if (u->unk28 != 2)
                u->unk28 = 2;
            else
                u->unk28 = 3;
        }
        else
        {
            u = gCurTask;
            if (u->unk28 == 3)
                u->unk28 = 1;
            else
                u->unk28 = 3;
        }
    }
    else if (f == 1)
    {
        u = gCurTask;
        u->unk28 = 2;
    }
    else if (f == 3)
    {
        u = gCurTask;
        u->unk28 = 1;
    }
    else if ((s16)n <= 5)
    {
        w = gCurTask;
        if (w->unk28 != gUnk_08743AC2[f - 1])
            w->unk28 = gUnk_08743AC2[f - 1];
        else
            w->unk28 = 3;
    }
    else
    {
        w = gCurTask;
        if (w->unk28 != 3)
            w->unk28 = 3;
        else
            w->unk28 = gUnk_08743AC2[f - 1];
    }
}
void sub_0809364c(void)
{
    sub_08068920(gCurTask->unk1C, 4);
    sub_080261d4(4);
    TaskGetPosSlot(gCurTask->unk1C);
    CreateChildTaskAt(154, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
    sub_08093a00(504);
    sub_08093780();
    TaskStop();
}

void sub_080936a0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
    t = gCurTask;
    t->unk58 = 0xFFFE0000;
    t->unk60 = 12032;
    t->unk6C = 0;
    do
    {
        TaskSetFrame(56);
        TaskYieldTrampoline(2);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(58);
        TaskYieldTrampoline(2);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 1);
    TaskSetFrame(59);
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->unk34 = 1;
    ActorSetState(1);
}

void sub_08093780(void)
{
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk7A = 0;
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = 65536;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 6);
}

s32 sub_080937d0(void)
{
    struct Task *t;
    struct Task *u;
    s32 r;
    s8 k;

    r = 0;
    t = gCurTask;
    switch (t->unk14)
    {
    case 3:
        k = t->unk43;
        if ((k == 1 && (k & gUnk_03005550) != 0)
         || (k == -1 && (gUnk_03005550 & 2) != 0))
        {
            u = gCurTask;
            u->unk18 = 0;
            u->unk34 = 2;
            ActorSetState(9);
            TaskSetEntry(sub_08091fe0, gCurTaskIdx);
            r = 1;
        }
        break;
    case 6:
    case 8:
        TaskTurnAroundAndReverseX();
        break;
    }
    return r;
}

void sub_08093858(void)
{
    gCurTask->unk58 = 0;
}

s32 sub_08093868(void)
{
    gCurTask->unk30 = 1;
    sub_0806caa0(1, 0, 0);
    sub_080261d4(2);
    return 0;
}

s32 sub_0809388c(void)
{
    struct Task *t;
    s32 n;

    t = gCurTask;
    t->unk34 = 0;
    t->unk18 = 0;
    n = t->unk1C;
    if (n != -1)
    {
        sub_080689c8(n, -t->unk43);
        gCurTask->unk1C = -1;
    }
    ActorSetHitReactions(gUnk_0874410C);
    ActorSetState(12);
    TaskSetEntry(sub_08091fe0, gCurTaskIdx);
    return 1;
}

void sub_080938e4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk42 = 12;
    u = gCurTask;
    u->unk38 = gUnk_087536FC;
    u->unk04 = (u32)sub_0809397c;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk28 = 4;
    v->unk2C = gUnk_08743B48[v->unk74];
    while (--gCurTask->unk2C >= 0)
    {
        w = gCurTask;
        if ((w->unk2C & 1) != 0)
        {
            w->unk3C = 0xFFFF;
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

void sub_0809397c(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->unk44] != -1
     && (u = &gTasks[i])->unk76 == 5 && u->unk18 != 0)
    {
        if (--t->unk28 == 0)
        {
            t->unk4C = u->unk4C;
            t->unk50 = u->unk50;
            t->unk48 = u->unk48;
            t->unk4A = u->unk4A;
            t->unk28 = 4;
        }
    }
    else
    {
        t->unk2C = 0;
    }
}

void sub_08093a00(s32 a)
{
    if (gCurTask->unk1C == gLocalPlayer)
        PlaySfx(a);
}

void sub_08093a24(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 9;
    u = gCurTask;
    u->unk38 = gUnk_08752F60;
    CallTableEntry(u->unk73, 1, gUnk_08744170);
}

void sub_08093a64(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_08093a98;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08744174);
}

void sub_08093a98(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_08744178);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08093ac8(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk28 = 0;
    TaskSetFrame(4);
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(98304, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE2000, 5376, 196608);
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(49152, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFEE000, 5376, 196608);
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    t = gCurTask;
    t->unk28++;
    TaskSleepForever();
}

void sub_08093b80(void)
{
    if (gCurTask->unk28 != 0)
    {
        ActorSetHitReactions(gUnk_0874430C);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
}

s32 sub_08093bb0(void)
{
    ActorSetHitReactions(gUnk_0874430C);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_08093bd4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 13;
    u = gCurTask;
    u->unk38 = gUnk_08753160;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk8C->unk38 = 0x20E;
    v->unk7A = 0;
    CallTableEntry(gCurTask->unk73, 2, gUnk_087441A4);
}

void sub_08093c30(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_08093c7c;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_087441AC);
}

void sub_08093c60(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_087441AC);
}

void sub_08093c7c(void)
{
    switch (gCurTask->unk15)
    {
    case 0:
        CallTableEntry(0, 2, gUnk_087441B4);
        break;
    case 1:
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->unk15, 2, gUnk_087441B4);
        ActorCheckHits();
        ActorReactToHit();
        break;
    }
}

void sub_08093ccc(void)
{
    gCurTask->unk15 = 0;
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(6);
        CreateChildTaskAtOffsetFacing(181, -8, -8, 1);
    }
}

void sub_08093cf8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *p;
    s16 *q;
    s32 i;
    s32 x;
    s32 y;

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->unk44] != -1)
    {
        u = &gTasks[i];
        t->unk43 = u->unk43;
        TaskUpdateFlip();
        if (u->unk1C != 0)
        {
            v = gCurTask;
            p = &u->unk48;
            x = *p;
            x <<= 16;
            v->unk4C = x;
            p += 1;
            v->unk50 = (*p + 8) << 16;
            y = x >> 16;
            v->unk48 = y;
            v->unk4A = v->unk50 >> 16;
            ActorSetState(1);
            TaskSetEntry(sub_08093c60, gCurTaskIdx);
        }
        else
        {
            v = gCurTask;
            q = &u->unk48;
            x = *q;
            x <<= 16;
            v->unk4C = x;
            q += 1;
            v->unk50 = (*q - 8) << 16;
            y = x >> 16;
            v->unk48 = y;
            v->unk4A = v->unk50 >> 16;
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void sub_08093dcc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 1;
    u = gCurTask;
    u->unk42 = 9;
    v = gCurTask;
    v->unk28 = 3;
    TaskSetMotionXFacing(gUnk_0874417C[v->unk73][v->unk74], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_0874418C[gCurTask->unk73][gCurTask->unk74],
                 8192, 458752);
    gCurTask->unk7A = 0;
    while (1)
    {
        TaskYieldTrampoline(6);
        CreateChildTaskAtOffsetFacing(181, -8, -8, 1);
    }
}

void sub_08093e54(void)
{
}

void sub_08093e58(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk38 = gUnk_08753160;
    t->unk42 = gTasks[t->unk44].unk42 - 1;
    TaskFaceLikeParent();
    u = gCurTask;
    u->unk58 = 0xFFFE0000;
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

s32 sub_08093edc(void)
{
    ActorSetHitReactions(gUnk_08744324);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 sub_08093f00(void)
{
    struct Task *t;
    s32 r;

    r = 0;
    t = gCurTask;
    if (--t->unk28 == 0)
    {
        ActorSetHitReactions(gUnk_08744324);
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->unk7A = 0;
        TaskSetMotionY(gUnk_0874419C[gCurTask->unk74], 8192, 458752);
    }
    return r;
}
