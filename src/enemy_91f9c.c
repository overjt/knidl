/* game_code_and_rodata 0x08091F9C-0x08093F64 (issue #67, module M25 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08091F9C 0x08093F64 src/enemy_91f9c.c --newpb
 *
 * M25's third and fourth boss scripts.  The third (entry Task_Bugzzy in
 * src/enemy_91f08.c, table 0x08743ADC) starts here with BugzzyInit, which
 * installs the per-frame body BugzzyUpdate and the animation script
 * gUnk_08743AC8.  BugzzyUpdate is the busiest body in the module: besides the
 * usual Task.updateState dispatch it calls ClampTaskToRoom (the camera/room hook) on
 * entry, and when the row gUnk_08743A58[Task.unk34] is non-null it runs the
 * "hit the wall" transition - sub_0806914c, then Task.unk1C = Task.hitterSlot,
 * a re-seat of the actor at gUnk_030023B4 - Task.facing * 16, and a hand-off to
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
 * sub_080934f8 is the collision probe: ten GetCollisionTileAtOffset samples along
 * gUnk_08743AB8, mapped through the terrain-class table gUnk_087339F0 into a
 * two-bit result that picks the next Task.unk28 direction from gUnk_08743AC2.
 * sub_0809364c / sub_080936a0 / sub_08093780 are the shared step sequences,
 * sub_080937d0 the hit hook, sub_08093858 the four-instruction "stop moving"
 * leaf the census had missed, and sub_0809397c the companion body.
 *
 * The fourth boss starts at Task_BonkersNut (table 0x087441A4, graphics
 * gBonkersNutFrames): sub_08093a64 installs sub_08093a98 as its body,
 * sub_08093ac8 is its one state, Task_PoppyBrosSrBomb / sub_08093c30 are the second
 * entry pair (graphics gPoppyBrosSrBombFrames, Actor.sfxOverride = 0x20E), sub_08093ccc and
 * sub_08093dcc are the endless spawners that call CreateChildTaskAtOffsetFacing(181, -8, -8, 1)
 * every six frames, and sub_08093cf8 / sub_08093e58 / sub_08093f00 are the
 * companions that copy the boss's 16.16 position (±8 rows) and expire with it.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "actor.h"
#include "enemy.h"

/* ROM tables */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern s8 gUnk_087339F0[];
extern vu8 gTerrainResult;

/* Externals */
extern void ClampTaskToRoom(struct Task *t);
extern u32 sub_0806914c(s32 a);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void ActorCheckHits(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void sub_080689c8(s32 i, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern u8 sub_0806acf8(void);

/* Defined below */
void BugzzyInit(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)BugzzyUpdate;
    sub_080666cc(gUnk_08743AC8);
    u = gCurTask;
    u->unk24 = u->unk8C->palette;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 13, gBugzzyStates);
}

void BugzzyEnterState(void)
{
    CallTableEntry(gCurTask->state, 13, gBugzzyStates);
}

void BugzzyUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 m;

    ClampTaskToRoom(gCurTask);
    t = gCurTask;
    if (t->unk20 != 0)
    {
        t->unk20--;
        CallTableEntry(t->updateState, 13, gBugzzyStateUpdates);
    }
    else if (sub_0806acf8() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 13, gBugzzyStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 13, gBugzzyStateUpdates);
    }
    u = gCurTask;
    if (u->unk30 == 1)
    {
        if (u->hitTimer != 0)
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
        if (v->hitKind == 8)
        {
            v->unk1C = v->hitterSlot;
            TaskFaceToward(v->unk1C);
            w = gCurTask;
            w->unk34 = 5;
            w->unk18 = 0;
            TaskGetPosSlot(w->unk1C);
            x = gCurTask;
            x->pixelX = gUnk_030023B4 - x->facing * 16;
            m = x->pixelX;
            x->posX = m << 16;
            ClampTaskToRoom(x);
            y = gCurTask;
            y->unk8C->palette = y->unk24;
            TaskSetFrame(36);
            if (gCurTask->unk1C == gLocalPlayer)
                PlaySfx(0x23D);
            sub_080685ec(gCurTask->unk1C, gCurTaskIdx, 3);
            ActorSetState(10);
            TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
        }
    }
}

void sub_08092198(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 0;
    TaskSetFrame(58);
    u = gCurTask;
    u->accelY = 9472;
    u->onGround = 0;
    TaskYieldTrampoline(1);
    if (gCurTask->onGround == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gCurTask->onGround == 0);
        RequestScreenShake(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    sub_08066580();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08092228(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void sub_08092250(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 r;
    s32 zero;

    t = gCurTask;
    t->updateState = 1;
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
            gCurTask->velX = 0;
            TaskSetFrame(4);
            TaskYieldTrampoline(8);
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            break;
        case 1:
            TaskSetMotionXFacing(-gUnk_08743A74[gCurTask->unk74], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(3);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(3);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            break;
        case 2:
            TaskSetMotionXFacing(gUnk_08743A74[gCurTask->unk74], 0x5A5A5A5A);
            TaskSetFrame(8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            TaskSetFrame(8);
            break;
        }
        gCurTask->unk6C++;
    }
    v = gCurTask;
    zero = 0;
    v->velX = zero;
    if (++v->unk2C > 2)
    {
        v->unk2C = zero;
        ActorSetState(2);
    }
    else
    {
        gUnk_02007D00[6] = gPlayerStates[gUnk_02007D00[4]].mode;
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
    if (gCurTask->state != 1)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_080925b8(void)
{
    struct Task *t;
    struct ActorSpawn spawn;

    t = gCurTask;
    t->updateState = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(29);
    TaskYieldTrampoline(12);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    spawn.subtype = 30;
    spawn.taskType = 133;
    spawn.variant = 0;
    spawn.spawnArg = gCurTask->unk74;
    spawn.x = 0xFFFE;
    spawn.y = 0;
    spawn.tileWord = sub_0806660c(1);
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    spawn.subtype = 30;
    spawn.taskType = 133;
    spawn.variant = 1;
    spawn.spawnArg = gCurTask->unk74;
    spawn.x = 0xFFFE;
    spawn.y = 0;
    spawn.tileWord = sub_0806660c(1);
    spawn.checkTerrain = 0;
    CreateActorFromDescAtOffsetFacing(&spawn, 1);
    TaskYieldTrampoline(4);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame--;
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 1);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080926d4(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
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
    t->updateState = 3;
    TaskFaceNearestPlayer();
    PlaySfx(0x23F);
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_08743A8E[gCurTask->unk74])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u = gCurTask;
        u->unk24 = u->unk8C->palette;
        u->unk8C->palette = 0;
        u->frame++;
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->unk8C->palette = v->unk24;
        gCurTask->unk6C++;
    }
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u2 = gCurTask;
        u2->unk24 = u2->unk8C->palette;
        u2->unk8C->palette = 0;
        u2->frame++;
        TaskYieldTrampoline(2);
        v2 = gCurTask;
        v2->unk8C->palette = v2->unk24;
    } while ((s16)++gCurTask->unk6C <= 0);
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_08743A8E[gCurTask->unk74])
    {
        TaskSetMotionXFacing(65536, 0x5A5A5A5A);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        if (gCurTask->onGround != 0)
            CreateDustTrail(1, 1, -24, 24);
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        u3 = gCurTask;
        u3->unk24 = u3->unk8C->palette;
        u3->unk8C->palette = 0;
        u3->frame++;
        TaskYieldTrampoline(2);
        v3 = gCurTask;
        v3->unk8C->palette = v3->unk24;
        gCurTask->unk6C++;
    }
    w = gCurTask;
    zero = 0;
    w->velX = zero;
    w->frame++;
    TaskYieldTrampoline(12);
    PlaySfx(500);
    x = gCurTask;
    x->unk34 = 3;
    gTasks[CreateChildTask(200, x->pixelX, x->pixelY, sub_080665fc())].unk74 =
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
        if (y->facing == 1)
        {
            if (y->velX <= 0)
                break;
        }
        else if (y->velX >= 0)
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
    if (gCurTask->state != 3)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_08092a14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->updateState = 4;
    TaskSetFrame(60);
    TaskYieldTrampoline(6);
    v = gCurTask;
    v->frame--;
    TaskYieldTrampoline(10);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->velY = gUnk_08743A9C[u->unk74];
    u->accelY = 9472;
    if (u->velY < 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while (gCurTask->velY < 0);
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
    if (gCurTask->state != 4)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_08092b58(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 5;
    TaskSetMotionXFacing(gUnk_08743AA4[gCurTask->unk74], 0x5A5A5A5A);
    if (TaskGetYDirBitToNearestPlayer() == 1)
        gCurTask->velY = 49152;
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
    if (gCurTask->state != 5)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_08092c00(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 6;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame--;
    TaskYieldTrampoline(10);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08743AAC[gCurTask->unk74], 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFC0000;
    u->accelY = 9472;
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
    if (gCurTask->state != 6)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}
void sub_08092cdc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    t = gCurTask;
    t->updateState = 7;
    TaskFaceNearestPlayer();
    TaskSetFrame(60);
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gUnk_02007D00[4] =
        gUnk_08743AB4[RandomRange(2) + gCurTask->unk74 * 2];
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_02007D00[4])
    {
        gCurTask->onGround = 0;
        v = gCurTask;
        v->velY = 0xFFFB0000;
        v->accelY = 20480;
        v->unk6C = 0;
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 13);
        while (gCurTask->onGround == 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        }
        TaskStopY();
        RequestScreenShake(2);
        PlaySfx(0x1F7);
        TaskSetFrame(59);
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    }
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    p = &gTasks[TaskFindNearestPlayer()];
    if (p->onGround == 0)
        ActorSetState(4);
    else
        ActorSetState(3);
    TaskSleepForever();
}
void sub_08092e40(void)
{
    if (gCurTask->state != 7)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_08092e68(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 8;
    u = gCurTask;
    u->accelY = 9472;
    if (u->onGround == 0)
    {
        do
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(58);
            else
                TaskSetFrame(57);
            TaskYieldTrampoline(1);
        } while (gCurTask->onGround == 0);
        RequestScreenShake(2);
        PlaySfx(0x1F7);
    }
    TaskStop();
    TaskSetFrame(59);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08092f04(void)
{
    if (gCurTask->state != 8)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_08092f2c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 9;
    TaskStop();
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFD0000;
    u->accelY = 9472;
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    v = gCurTask;
    v->frame--;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    gCurTask->velX = 0;
    sub_08093780();
    ActorSetState(11);
    TaskSleepForever();
}

void sub_08092ff4(void)
{
    if (gCurTask->state != 9)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
}

void sub_0809301c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 11;
    TaskSetMotionXFacing(0xFFFFC000, 0x5A5A5A5A);
    TaskSetFrame(48);
    TaskYieldTrampoline(4);
    gCurTask->velX = 0;
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
    if (gCurTask->state != 11)
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
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
    t->updateState = 10;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->velX = 131072;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE0000;
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->frame++;
    u->velX = 131072;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE0000;
    TaskYieldTrampoline(2);
    TaskStop();
    v = gCurTask;
    v->accelY = 9472;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_080934f8();
    s = gCurTask;
    switch (s->unk28)
    {
    case 0:
        TaskSetMotionXFacing(81920, 0x5A5A5A5A);
    case 1:
        gCurTask->onGround = 0;
        w = gCurTask;
        w->velY = 0xFFFC0000;
        w->accelY = 9472;
        TaskSetFrame(51);
        TaskYieldTrampoline(13);
        while (gCurTask->velY < 0)
        {
            if ((gFrameCount & 2) != 0)
                TaskSetFrame(52);
            else
                TaskSetFrame(61);
            TaskYieldTrampoline(1);
        }
        x = gCurTask;
        x->velY = 262144;
        x->accelY = 0;
        TaskSetFrame(53);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        sub_0809364c();
        sub_080936a0();
        break;
    case 2:
        s->onGround = 0;
        TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
        y = gCurTask;
        y->velY = 0xFFFC0000;
        y->accelY = 16384;
        TaskSetFrame(45);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskSetFrame(49);
        TaskYieldTrampoline(4);
        TaskSetFrame(47);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        while (gCurTask->onGround == 0)
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
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            RequestScreenShake(2);
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
    if (t->state != 10)
    {
        t->unk1C = -1;
        TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
    }
}

void sub_08093380(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 12;
    gUnk_02007D00[4] = 0;
    if (--gUnk_02007D00[7] == 0)
        sub_0806684c();
    sub_080667c0(1, 63);
    CreateStarFlash(1, 0, 0);
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 9472, 196608);
    TaskSetFrame(63);
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(15);
    gCurTask->unk34 = 4;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    CreateStarFlash(0, 0, 10);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 24, 24);
    TaskStopY();
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    TaskStopX();
    TaskYieldTrampoline(170);
    CreateStarFlash(1, 0, 0);
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
        if (++t->frame > 35)
            t->frame = 34;
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
        m = GetCollisionTileAtOffset(t->pixelX, t->pixelY, gUnk_08743AB8[i] * t->facing, 0);
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
    RequestScreenShake(4);
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

    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFE8000, 0x5A5A5A5A);
    t = gCurTask;
    t->velY = 0xFFFE0000;
    t->accelY = 12032;
    t->unk6C = 0;
    do
    {
        TaskSetFrame(56);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(58);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 1);
    TaskSetFrame(59);
    while (gCurTask->onGround == 0)
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
        gCurTask->onGround = 0;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 65536;
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
    switch (t->state)
    {
    case 3:
        k = t->facing;
        if ((k == 1 && (k & gTerrainResult) != 0)
         || (k == -1 && (gTerrainResult & 2) != 0))
        {
            u = gCurTask;
            u->unk18 = 0;
            u->unk34 = 2;
            ActorSetState(9);
            TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
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
    gCurTask->velY = 0;
}

s32 sub_08093868(void)
{
    gCurTask->unk30 = 1;
    CreateStarFlash(1, 0, 0);
    RequestScreenShake(2);
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
        sub_080689c8(n, -t->facing);
        gCurTask->unk1C = -1;
    }
    ActorSetHitReactions(gUnk_0874410C);
    ActorSetState(12);
    TaskSetEntry(BugzzyEnterState, gCurTaskIdx);
    return 1;
}

void sub_080938e4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 12;
    u = gCurTask;
    u->frameTable = gUnk_087536FC;
    u->updateCallback = (u32)sub_0809397c;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk28 = 4;
    v->unk2C = gUnk_08743B48[v->unk74];
    while (--gCurTask->unk2C >= 0)
    {
        w = gCurTask;
        if ((w->unk2C & 1) != 0)
        {
            w->frame = 0xFFFF;
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

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->parent] != -1
     && (u = &gTasks[i])->unk76 == 5 && u->unk18 != 0)
    {
        if (--t->unk28 == 0)
        {
            t->posX = u->posX;
            t->posY = u->posY;
            t->pixelX = u->pixelX;
            t->pixelY = u->pixelY;
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

void Task_BonkersNut(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gBonkersNutFrames;
    CallTableEntry(u->variant, 1, gUnk_08744170);
}

void sub_08093a64(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_08093a98;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08744174);
}

void sub_08093a98(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_08744178);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08093ac8(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->unk28 = 0;
    TaskSetFrame(4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(98304, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE2000, 5376, 196608);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(49152, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFEE000, 5376, 196608);
    while (gCurTask->onGround == 0)
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

void Task_PoppyBrosSrBomb(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 13;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrBombFrames;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk8C->sfxOverride = 0x20E;
    v->onGround = 0;
    CallTableEntry(gCurTask->variant, 2, gUnk_087441A4);
}

void sub_08093c30(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_08093c7c;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_087441AC);
}

void sub_08093c60(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_087441AC);
}

void sub_08093c7c(void)
{
    switch (gCurTask->updateState)
    {
    case 0:
        CallTableEntry(0, 2, gUnk_087441B4);
        break;
    case 1:
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 2, gUnk_087441B4);
        ActorCheckHits();
        ActorReactToHit();
        break;
    }
}

void sub_08093ccc(void)
{
    gCurTask->updateState = 0;
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

    if ((s16)gTaskSlotTypes[i = (t = gCurTask)->parent] != -1)
    {
        u = &gTasks[i];
        t->facing = u->facing;
        TaskUpdateFlip();
        if (u->unk1C != 0)
        {
            v = gCurTask;
            p = &u->pixelX;
            x = *p;
            x <<= 16;
            v->posX = x;
            p += 1;
            v->posY = (*p + 8) << 16;
            y = x >> 16;
            v->pixelX = y;
            v->pixelY = v->posY >> 16;
            ActorSetState(1);
            TaskSetEntry(sub_08093c60, gCurTaskIdx);
        }
        else
        {
            v = gCurTask;
            q = &u->pixelX;
            x = *q;
            x <<= 16;
            v->posX = x;
            q += 1;
            v->posY = (*q - 8) << 16;
            y = x >> 16;
            v->pixelX = y;
            v->pixelY = v->posY >> 16;
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
    t->updateState = 1;
    u = gCurTask;
    u->layer = 9;
    v = gCurTask;
    v->unk28 = 3;
    TaskSetMotionXFacing(gUnk_0874417C[v->variant][v->unk74], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_0874418C[gCurTask->variant][gCurTask->unk74],
                 8192, 458752);
    gCurTask->onGround = 0;
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
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gPoppyBrosSrBombFrames;
    t->layer = gTasks[t->parent].layer - 1;
    TaskFaceLikeParent();
    u = gCurTask;
    u->velY = 0xFFFE0000;
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
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
        t->onGround = 0;
        TaskSetMotionY(gUnk_0874419C[gCurTask->unk74], 8192, 458752);
    }
    return r;
}
