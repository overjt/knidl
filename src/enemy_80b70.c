/* game_code_and_rodata 0x08080B70-0x080820B8 (issue #71, module M21 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08080B70 0x080820B8 src/enemy_80b70.c --newpb
 *
 * Second third of enemy/object behaviour bank 2 (see src/enemy_7f044.c for
 * the three-table pattern all of M21 is built from).  It holds:
 *   * task #175, the only class-4 type in the bank: `Task_HotHeadFlame` is the
 *     coroutine itself (it re-seats the actor next to gTasks[unk44]
 *     every cycle) with per-frame hook `sub_08080d58`, and `sub_08080b70` /
 *     `sub_08080bcc` are its unk73 handlers (`0x08741BF4`);
 *   * task #31's dispatcher `Task_Starman` (`0x08741544`, four rows) and
 *     task #38's `Task_PoppyBrosJr` (`0x087415B8`, three rows);
 *   * six scripts in the entry/hook shape: `sub_08080e10`+`sub_08080e5c`
 *     (`0x08741554`/`0x0874156C`, six states), `sub_08081408`+`sub_0808145c`
 *     (`0x08741584`), `sub_08081614`+`sub_08081668` (`0x0874159C`),
 *     `sub_08081774`+`sub_080817d4` (`0x087415A4`), `sub_080819f4`+
 *     `sub_08081a74` (`0x087415C4`) and `sub_08081d24`+`sub_08081d84`
 *     (`0x087415DC`);
 *   * the shared helpers `sub_08080374` (drain Task.unk18 by N and fire cue
 *     192 when it runs out), `sub_08081814` (the eight-step walk animation)
 *     and `sub_08081e64` (flip Task.facing from the gTerrainResult[4] input,
 *     returning 1 when the input already matches the facing).
 *
 * `sub_0808164c`, `sub_080817b8` and `sub_08081d68` are dead exports of the
 * same kind as batch 1's; `sub_08081960`, `sub_08081e40`, `sub_08081f08` and
 * `sub_08081f18` are pointer-referenced leaves the census originally missed
 * (both classes curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gViewRect[];
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern vu16 gTaskSlotTypes[];
extern u8 gTerrainResult[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern s32 gUnk_087414F8[];
extern u8 gUnk_08741500[];
extern u8 gUnk_08741502[];
extern s32 gUnk_08741504[];
extern u8 gUnk_0874150C[];
extern s32 gUnk_0874151C[];
extern s32 gUnk_08741524[];
extern u32 gStarmanVariants[];
extern u32 gUnk_08741554[];
extern u32 gUnk_0874156C[];
extern u32 gUnk_08741584[];
extern u32 gUnk_08741590[];
extern u32 gUnk_0874159C[];
extern u32 gUnk_087415A0[];
extern u32 gUnk_087415A4[];
extern u32 gUnk_087415A8[];
extern s32 gUnk_087415AC[];
extern u32 gPoppyBrosJrVariants[];
extern u32 gUnk_087415C4[];
extern u32 gUnk_087415D0[];
extern u32 gUnk_087415DC[];
extern u32 gUnk_087415E0[];
extern u32 gUnk_08741604[];
extern u32 gUnk_087419F4[];
extern u32 gUnk_08741A20[];
extern u32 gUnk_08741B3C[];
extern u32 gUnk_08741B44[];
extern u32 gUnk_0874CBD0[];
extern u32 gStarmanFrames[];
extern u32 gPoppyBrosJrFrames[];
extern u32 gUnk_087528F0[];
extern u32 gUnk_08752934[];
extern u32 gUnk_08752978[];
extern u32 gUnk_087529A8[];

/* Externals */
extern s32 RandomRange(s32 a);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern s32 TaskGetFacingTowardNearestPlayer(void);
extern s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d);
extern s32 TaskGetAngleToNearestPlayer(s32 prec);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 TaskHasSameSerial(u32 i);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskFree(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void RequestScreenShake(s32 a);
extern void ActorLoadDef(u32 *def);
extern void ActorSetState(u32 v);
extern void ActorSetAttackBox(u32 *p);
extern void ActorSetTerrainBox(u32 *p);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskFaceLikeParent(void);
extern void sub_0806a0f0(s32 a);
extern void sub_08065640(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void HotHeadWalkEnterState(void);
extern void HotHeadStandEnterState(void);

/* Defined below */
void sub_08080d58(void);
void sub_08080e5c(void);
void sub_08081814(void);
void sub_0808145c(void);
void sub_08081668(void);
void sub_080817d4(void);
void sub_08081a74(void);
s32 sub_08081e64(void);
void sub_08081d84(void);

s32 sub_08080b70(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(2);
        TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
        break;
    case 1:
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
        break;
    }
    return 1;
}

s32 sub_08080bcc(void)
{
    TaskStopY();
    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(0);
        TaskSetEntry(HotHeadWalkEnterState, gCurTaskIdx);
        break;
    case 1:
        break;
    case 2:
        ActorSetState(0);
        TaskSetEntry(HotHeadStandEnterState, gCurTaskIdx);
        break;
    }
    return 1;
}

s32 sub_08080c2c(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_08080c38(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void Task_HotHeadFlame(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *o;
    s32 n;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->updateCallback = (u32)sub_08080d58;
    t->layer = 10;
    gCurTask->frameTable = gUnk_0874CBD0;
    while (1)
    {
        o = &gTasks[gCurTask->parent];
        TaskFaceLikeParent();
        u = gCurTask;
        u->pixelX = *(u16 *)&o->pixelX - (u->facing << 2);
        u->posX = u->pixelX << 16;
        n = RandomRange(16);
        v = gCurTask;
        v->pixelY = *(u16 *)&o->pixelY + 4 - n;
        v->posY = v->pixelY << 16;
        TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFF8000;
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        TaskSetFrame(20);
        TaskYieldTrampoline(2);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskSetFrame(16);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(10);
    }
}

void sub_08080d58(void)
{
    struct Task *p;
    struct Actor **q;
    struct Actor *a;
    s32 n;
    s32 k;

    if (TaskHasSameSerial(gCurTask->parent) == 1)
    {
        p = gTasks;
        n = gCurTask->parent;
        k = n * 144;
        q = (struct Actor **)((u8 *)p + 140);
        a = *(struct Actor **)((u8 *)q + k);
        if ((s16)gTaskSlotTypes[n] == -1 || a->hitState == 2)
            TaskFree(gCurTaskIdx);
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_Starman(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gStarmanFrames;
    CallTableEntry(u->variant, 4, gStarmanVariants);
}

void sub_08080e10(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_08080e5c;
    t->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 6, gUnk_08741554);
}

void sub_08080e40(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_08741554);
}

void sub_08080e5c(void)
{
    struct Task *t = gCurTask;

    if (t->state == 5)
    {
        CallTableEntry(t->updateState, 6, gUnk_0874156C);
    }
    else if ((u8)ActorCollideTerrain() == 0)
    {
        CallTableEntry(gCurTask->updateState, 6, gUnk_0874156C);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08080ea4(void)
{
    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(gUnk_087414F8[gCurTask->unk74], 0x5A5A5A5A);
    sub_08081814();
}

void sub_08080edc(void)
{
    gCurTask->unk18 = (u16)TaskGetAngleToNearestPlayer(3);
    if (gCurTask->unk18 == 0 || gCurTask->unk18 > 255)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 47)
        {
            ActorSetState(1);
            TaskSetEntry(sub_08080e40, gCurTaskIdx);
        }
    }
}

void sub_08080f34(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->updateState = 1;
    a = gCurTask;
    a->unk2C = 0;
    a->velX = 0;
    TaskSetFrame(11);
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    b = gCurTask;
    b->velY = 0xFFFE0000;
    b->unk30 = 16;
    while (1)
    {
        TaskYieldTrampoline(6);
        c = gCurTask;
        if (++c->unk30 > 18)
            c->unk30 = 16;
    }
}

void sub_08080fa4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;

    i = 2;
    t = gCurTask;
    if ((s8)t->onGround != 0)
        return;
    if (gViewRect[i] - 9 > t->pixelY)
    {
        TaskStop();
        gCurTask->frame = 0xFFFF;
        ActorSetState(2);
        TaskSetEntry(sub_08080e40, gCurTaskIdx);
        return;
    }
    if (TaskGetNearestPlayerDx() < 0)
    {
        u = gCurTask;
        u->velX = u->velX + 0xFFFFE700;
    }
    else
    {
        u = gCurTask;
        u->velX = u->velX + 0x1900;
    }
    v = gCurTask;
    if (abs(v->velX) > 0x20000)
    {
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    }
    else
    {
        if (v->velX > 0)
            v->facing = 1;
        else
            v->facing = -1;
    }
    w = gCurTask;
    if (abs(w->velX) <= 0x5FFF)
        TaskSetFrame(13);
    else
        TaskSetFrame(*(s16 *)&w->unk30);
}

void sub_08081084(void)
{
    struct Task *a;
    struct Task *b;

    gCurTask->updateState = 2;
    a = gCurTask;
    a->unk2C++;
    TaskStop();
    b = gCurTask;
    b->frame = 0xFFFF;
    b->unk30 = gUnk_08741500[b->unk74];
    TaskSleepForever();
}

void sub_080810c4(void)
{
    struct Task *t;
    struct Task *o;
    s32 n;
    s32 i;

    i = 2;
    t = gCurTask;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (n == 0)
    {
        ActorSetState(3);
        TaskSetEntry(sub_08080e40, gCurTaskIdx);
    }
    else
    {
        if (n > gUnk_08741502[t->unk74])
        {
            o = &gTasks[TaskFindNearestPlayer()];
            gCurTask->posX = o->pixelX << 16;
        }
        gCurTask->posY = (gViewRect[i] - 9) << 16;
    }
}

void sub_08081140(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->updateState = 3;
    TaskSetMotionY(0x20000, 0x1500, 0x30000);
    TaskSetFrame(20);
    while ((s8)gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    if (gCurTask->unk2C != 0)
        RequestScreenShake(1);
    gCurTask->onGround = 0;
    TaskSetFrame(13);
    TaskSetMotionY(0xFFFE0000, 0x1500, 0x30000);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    a = gCurTask;
    a->unk30 = 0;
    if ((s8)a->onGround == 0)
    {
        do
        {
            b = gCurTask;
            if (--b->unk30 <= 0)
            {
                if (++b->frame > 15)
                    b->frame = 14;
                gCurTask->unk30 = 4;
            }
            TaskYieldTrampoline(1);
        } while ((s8)gCurTask->onGround == 0);
    }
    TaskStop();
    c = gCurTask;
    if (c->unk28++ == 0)
    {
        TaskSetFrame(13);
        TaskYieldTrampoline(60);
        ActorSetState(0);
    }
    else
    {
        ActorSetState(5);
    }
    TaskSleepForever();
}

void sub_0808124c(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(sub_08080e40, gCurTaskIdx);
}

void sub_08081274(void)
{
    struct Task *t;

    gCurTask->updateState = 5;
    if (TaskGetFacingTowardNearestPlayer() == 1)
        gCurTask->facing = -1;
    else
        gCurTask->facing = 1;
    TaskSetFrame(11);
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x6600, 0);
    t = gCurTask;
    t->velY = 0;
    t->accelY = 0xFFFFE700;
    TaskSetFrame(13);
    TaskSleepForever();
}

void sub_080812ec(void)
{
}

void sub_080812f0(void)
{
    struct Task *a;
    struct Task *b;
    struct Task *c;

    gCurTask->updateState = 4;
    ActorSetTerrainBox(gUnk_08741B44);
    a = gCurTask;
    a->accelX = 0;
    a->velY = 0x18000;
    TaskSetFrame(16);
    b = gCurTask;
    b->unk30 = 4;
    if ((s8)b->onGround == 0)
    {
        do
        {
            c = gCurTask;
            if (--c->unk30 == 0)
            {
                if (++c->frame > 18)
                    TaskSetFrame(16);
                gCurTask->unk30 = 4;
            }
            TaskYieldTrampoline(1);
        } while ((s8)gCurTask->onGround == 0);
    }
    TaskStopY();
    TaskSetMotionXFacing(0x5A5A5A5A, gUnk_08741504[gCurTask->unk74]);
    TaskSetFrame(16);
    while (1)
    {
        CreateDustTrail(1, 1, -2, 5);
        TaskYieldTrampoline(8);
    }
}

void sub_080813a4(void)
{
    struct Task *t = gCurTask;
    s32 d;

    if ((s8)t->onGround != 0)
    {
        d = t->facing;
        if ((d == 1 && t->velX <= 0) || (d == -1 && t->velX >= 0))
        {
            TaskStopX();
            ActorSetTerrainBox(gUnk_08741B3C);
            ActorSetState(0);
            TaskSetEntry(sub_08080e40, gCurTaskIdx);
        }
    }
}

void sub_08081408(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_0808145c;
    t->unk28 = 216;
    t->unk2C = 0;
    t->unk30 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_08741584);
}

void sub_08081440(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_08741584);
}

void sub_0808145c(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_08741590);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808148c(void)
{
    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    sub_08081814();
}

void sub_080814b4(void)
{
    struct Task *t = gCurTask;
    s32 n;

    n = t->unk28 + 1;
    t->unk28 = n;
    if (n == t->unk2C)
    {
        ActorSetState(1);
        TaskSetEntry(sub_08081440, gCurTaskIdx);
    }
    else if (n == 217)
    {
        t->unk28 = 0;
        gCurTask->unk2C = gUnk_0874150C[RandomRange(16)];
    }
}

void sub_08081508(void)
{
    gCurTask->updateState = 1;
    gCurTask->velX = 0;
    TaskSetFrame(11);
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFE0000, 0x1000, 0x30000);
    TaskSetFrame(13);
    TaskSleepForever();
}

void sub_08081560(void)
{
    struct Task *t = gCurTask;

    if ((s8)t->onGround == 0 && t->velY >= 0)
    {
        ActorSetState(2);
        TaskSetEntry(sub_08081440, gCurTaskIdx);
    }
}

void sub_0808159c(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->velX = 0;
    t->accelY = 0x1000;
    t->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
    }
}

void sub_080815dc(void)
{
    if ((s8)gCurTask->onGround != 0)
    {
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(sub_08081440, gCurTaskIdx);
    }
}

void sub_08081614(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_08081668;
    t->onGround = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_0874159C);
}

void sub_0808164c(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874159C);
}

void sub_08081668(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087415A0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808168c(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->unk28 = 6;
    t->unk2C = 0;
    t->unk30 = 6;
    TaskSetMotionXFacing(gUnk_0874151C[t->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(16);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void sub_080816e8(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *x;
    s32 n;
    s32 v;
    s32 w;

    n = t->unk30;
    if (n == 0)
        return;
    if (--t->unk28 != 0)
        return;
    if (++t->unk2C == 8)
    {
        t->unk2C = 0;
        t->unk30 = n - 1;
        if (t->unk30 == 0)
        {
            t->accelY = 0xFFFFE700;
            return;
        }
    }
    v = TaskGetNearestPlayerDy();
    u = gCurTask;
    u->unk18 = v;
    w = gUnk_08741524[u->unk2C];
    u->unk1C = w;
    if (abs(v) > 10)
    {
        if (v < 0)
        {
            if (w < 0)
                u->unk28 = 8;
            else
                u->unk28 = 6;
        }
        else
        {
            if (w > 0)
                u->unk28 = 8;
            else
                u->unk28 = 6;
        }
    }
    else
    {
        u->unk28 = 6;
    }
    x = gCurTask;
    x->velY = x->unk1C;
}

void sub_08081774(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_080817d4;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087415A4);
}

void sub_080817b8(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_087415A4);
}

void sub_080817d4(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087415A8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080817f8(void)
{
    gCurTask->updateState = 0;
    sub_08081814();
}

void sub_08081810(void)
{
}

void sub_08081814(void)
{
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(8);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(10);
        TaskSetFrame(9);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

s32 sub_08081884(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 1 && t->unk30 != 0)
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_080818a8(void)
{
    s32 r = 0;
    struct Task *t = gCurTask;

    switch (t->variant)
    {
    case 0:
        if (t->state == 1)
        {
            ActorSetState(3);
            TaskSetEntry(sub_08080e40, gCurTaskIdx);
            r = 1;
        }
        break;
    case 1:
        t->velY = 0;
        ActorSetState(2);
        TaskSetEntry(sub_08081440, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        break;
    }
    return r;
}

s32 sub_08081900(void)
{
    s32 r = 0;
    struct Task *t = gCurTask;

    switch (t->variant)
    {
    case 0:
        if (t->state == 0 || t->state == 4)
        {
            ActorSetState(4);
            TaskSetEntry(sub_08080e40, gCurTaskIdx);
            r = 1;
        }
        break;
    case 1:
        t->unk30++;
        ActorSetState(2);
        TaskSetEntry(sub_08081440, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        break;
    }
    return r;
}

s32 sub_08081960(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 0 && t->state == 1)
        t->onGround = 0;
    return 0;
}

s32 sub_08081984(void)
{
    s32 n = gCurTask->variant;

    if (n >= 0)
    {
        if (n <= 1)
            TaskTurnAroundAndReverseX();
    }
    return 0;
}

s32 sub_080819a4(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void Task_PoppyBrosJr(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gPoppyBrosJrFrames;
    CallTableEntry(gCurTask->variant, 3, gPoppyBrosJrVariants);
}

void sub_080819f4(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateCallback = (u32)sub_08081a74;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk28 = 0;
    t->unk2C = 0;
    t->onGround = 0;
    u = gCurTask;
    if (u->variant == 0)
    {
        u->unk30 = 0;
        ActorSetState(0);
    }
    else
    {
        u->variant = 0;
        gCurTask->unk30 = 30;
        ActorSetState(2);
    }
    CallTableEntry(gCurTask->state, 3, gUnk_087415C4);
}

void sub_08081a58(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_087415C4);
}

void sub_08081a74(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_087415D0);
    if (gCurTask->unk30 == 0)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_08081aac(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 0;
    while (1)
    {
        gCurTask->unk34 = -1;
        TaskStop();
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        gCurTask->onGround = 0;
        TaskSetMotionXFacing(gUnk_087415AC[gCurTask->unk74], 0x5A5A5A5A);
        t = gCurTask;
        t->velY = 0xFFFF3300;
        t->unk34 = 0;
        t->unk24 = 6;
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->frame++;
        if (u->onGround == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gCurTask->onGround == 0);
        }
    }
}

void sub_08081b5c(void)
{
    struct Task *t = gCurTask;

    if (t->unk34 == -1)
        return;
    if (t->onGround != 0)
    {
        sub_08081e64();
    }
    else if (t->unk24 != 0)
    {
        if (--t->unk24 == 0)
        {
            t->accelY = 0x1500;
            t->speedLimitY = 0x30000;
        }
    }
    if (gCurTask->state != 0)
        TaskSetEntry(sub_08081a58, gCurTaskIdx);
}

void sub_08081bc4(void)
{
    gCurTask->updateState = 1;
    TaskStopY();
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(gUnk_087415AC[gCurTask->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
    }
}

void sub_08081c3c(void)
{
    if (gCurTask->onGround != 0 && (u8)sub_08081e64() == 0)
    {
        ActorSetState(0);
        TaskSetEntry(sub_08081a58, gCurTaskIdx);
    }
}

void sub_08081c78(void)
{
    gCurTask->updateState = 2;
    TaskSetMotionY(0xFFFE0000, 0x1500, 0x30000);
    TaskSetFrame(6);
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(12);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskSleepForever();
}

void sub_08081ce0(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk30 != 0)
        t->unk30--;
    u = gCurTask;
    if (u->onGround != 0)
    {
        u->unk30 = 0;
        ActorSetState(0);
        TaskSetEntry(sub_08081a58, gCurTaskIdx);
    }
}

void sub_08081d24(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_08081d84;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087415DC);
}

void sub_08081d68(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_087415DC);
}

void sub_08081d84(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_087415E0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08081db4(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 0;
    gCurTask->unk24 = 0;
    while (1)
    {
        TaskStop();
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        gCurTask->onGround = 0;
        t = gCurTask;
        t->velY = 0xFFFF3300;
        t->unk24 = 6;
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        u = gCurTask;
        u->frame++;
        if (u->onGround == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gCurTask->onGround == 0);
        }
    }
}

void sub_08081e40(void)
{
    struct Task *t = gCurTask;

    if (t->unk24 != 0)
    {
        if (--t->unk24 == 0)
        {
            t->accelY = 0x1500;
            t->speedLimitY = 0x30000;
        }
    }
}

s32 sub_08081e64(void)
{
    s32 r = 0;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gCurTask;
    n = gTerrainResult[4];
    t->unk1C = n;
    if (t->facing == 1)
    {
        if (n == 1 || n == 3)
            r = 1;
    }
    else
    {
        if (n == 2 || n == 4)
            r = 1;
    }
    if (r == 0)
    {
        u = gCurTask;
        u->unk28++;
        if (u->unk2C == 0)
        {
            if ((u->unk28 & 1) == 0 && RandomRange(3) == 0)
            {
                v = gCurTask;
                v->unk2C = 1;
                if (v->facing == 1)
                    v->facing = -1;
                else
                    v->facing = 1;
            }
        }
        else
        {
            u->unk2C--;
            if (u->facing == 1)
                u->facing = -1;
            else
                u->facing = 1;
        }
    }
    else
    {
        ActorSetState(1);
    }
    return r;
}

void sub_08081f08(void)
{
    gCurTask->velY = 0;
}

s32 sub_08081f18(void)
{
    struct Task *t = gCurTask;

    if (t->state == 1)
    {
        t->accelY = 0x1500;
        t->speedLimitY = 0x30000;
    }
    return 0;
}

s32 sub_08081f38(void)
{
    struct Task *t;

    TaskTurnAroundAndReverseX();
    t = gCurTask;
    t->unk2C = 0;
    t->unk28 = 0;
}

s32 sub_08081f50(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08081f60(void)
{
    struct Task *s;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    s = gCurTask;
    s->moveCallback = (u32)ActorMove;
    s->layer = 11;
    t = gCurTask;
    switch (t->variant)
    {
    case 0:
    case 2:
        u = gCurTask;
        u->drawCallback = (u32)sub_08065640;
        u->frameTable = gUnk_087528F0;
        break;
    case 1:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        ActorLoadDef(gUnk_087419F4);
        gCurTask->frameTable = gUnk_08752978;
        break;
    }
    v = gCurTask;
    v->unk28 = 0;
    v->unk2C = v->tileWord;
    v->tileWord = (v->tileWord & 0xFFF) | 0xF000;
    CallTableEntry(v->variant, 3, gUnk_08741604);
}

void sub_08082008(void)
{
    struct Task *s;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    s = gCurTask;
    s->moveCallback = (u32)ActorMove;
    s->layer = 11;
    t = gCurTask;
    switch (t->variant)
    {
    case 0:
    case 2:
        u = gCurTask;
        u->drawCallback = (u32)sub_08065640;
        u->frameTable = gUnk_08752934;
        u->unk74 = 0;
        break;
    case 1:
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        ActorLoadDef(gUnk_08741A20);
        gCurTask->frameTable = gUnk_087529A8;
        break;
    }
    v = gCurTask;
    v->unk2C = v->tileWord;
    v->unk28 = 1;
    v->tileWord = (v->tileWord & 0xFFF) | 0xF000;
    CallTableEntry(v->variant, 3, gUnk_08741604);
}
