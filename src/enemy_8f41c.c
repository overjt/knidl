/* game_code_and_rodata 0x0808F41C-0x0809000C (issue #70, module M24 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808F41C 0x0809000C src/enemy_8f41c.c --newpb
 *
 * See src/enemy_8cce8.c for the three-table pattern all of M24 is built on.
 *
 * This batch holds:
 *   * script 4's bodies and guards (tables `0x0874329C`/`0x087432A8`,
 *     `0x087432B4`/`0x087432C0`, `0x087432CC`/`0x087432D8` and the
 *     single-row `0x087432E4`/`0x087432E8`); the bodies walk the `s16[][4]`
 *     aim table `gUnk_0874325A` one row per Task.unk34 / Task.variant through
 *     `sub_0808eec4`;
 *   * the class-3 hook row `0x08743518` — `sub_0808f9b8`, `sub_0808f978`,
 *     `sub_0808f9d8` and `sub_0808f9f8`;
 *   * script 5: entry `Task_Coner` (`0x087432F4`, 2 rows), rows
 *     `ConerInit` / `ConerIdleInit`, bodies `0x087432FC` (3) and
 *     `0x08743308` (1);
 *   * script 6: entry `Task_LaserBallLaser` (`0x08743600`, 1 row), row
 *     `sub_0808fc90`, bodies `0x08743604` (2), guards `0x0874360C` (2);
 *   * script 7: entry `Task_ShotzoCannonball` (`0x0874362C`, 4 identical rows), row
 *     `sub_0808fdf8`, body `0x0874363C` (`sub_0808fe88`, the class-2 wanderer
 *     that picks its heading from gTasks[Task.parent].unk34) and guard
 *     `0x08743640` (`sub_0808ffe0`).
 *
 * `sub_0808fa04` and `sub_0808fe6c` are dead exports (twins of
 * `sub_0808f9f8` and of `sub_0808fdf8`'s cue call) that no ROM word points at;
 * both are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *a, u32 i);
extern void ActorSetState(u16 v);
extern void ActorSetAttackBox(void *p);
extern void AngleToVector(s32 a, s32 b);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_08069888(void);
extern u32 ActorReactToHit(void);

void sub_0808f41c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 2;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 0;
    PlaySfx(190);
    sub_0808efdc();
    u = gCurTask;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gCurTask;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gCurTask;
        w->unk6E++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gCurTask;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    TaskSleepForever();
}

void sub_0808f4b4(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        ActorSetState(0);
        TaskSetEntry(ShotzoAimEnterState, gCurTaskIdx);
    }
}

void sub_0808f4f8(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    TaskSleepForever();
}

void sub_0808f51c(void)
{
    sub_08069888();
}

void sub_0808f528(void)
{
    gCurTask->updateState = 0;
    TaskStopY();
    switch (gCurTask->variant)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    TaskSleepForever();
}

void sub_0808f578(void)
{
    struct Task *t;

    if (sub_08069888() == 0)
    {
        t = gCurTask;
        switch (t->variant)
        {
        case 1:
            t->unk34 = 2;
            break;
        case 2:
            t->unk34 = 1;
            break;
        case 3:
            t->unk34 = 3;
            break;
        }
        ActorSetState(2);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
    }
}

void sub_0808f5cc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 1;
    while (1)
    {
        TaskYieldTrampoline(100);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk70 = 0;
            PlaySfx(190);
            sub_0808f058();
            u = gCurTask;
            u->unk2C = 0;
            u->unk6E = 0;
            do
            {
                v = gCurTask;
                sub_0808eec4(gUnk_0874325A[v->variant][v->unk2C]);
                w = gCurTask;
                w->unk2C++;
                TaskYieldTrampoline(2);
                TaskStop();
                x = gCurTask;
                x->unk6E++;
            } while ((s16)x->unk6E <= 3);
            gCurTask->unk70 = 1;
            TaskYieldTrampoline(7);
            y = gCurTask;
            y->unk6C++;
        } while ((s16)y->unk6C <= 2);
        gCurTask->unk28 = 1;
    }
}

void sub_0808f678(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        TaskStop();
        ActorSetState(2);
        TaskSetEntry(ShotzoFixedEnterState, gCurTaskIdx);
    }
}

void sub_0808f6c0(void)
{
    struct Task *t;

    switch (gCurTask->variant)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    gCurTask->updateState = 1;
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    TaskSleepForever();
}

void sub_0808f71c(void)
{
    sub_08069888();
}

void sub_0808f728(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->unk74 = 1;
    t = gCurTask;
    t->unk28 = gUnk_08743248[t->unk74];
    TaskStop();
    while (1)
        sub_0808f0d0();
}

void sub_0808f75c(void)
{
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        if (sub_08069888() == 0)
            sub_0808f1b4(1, ParasolShotzoEnterState);
    }
    else if (ActorCollideTerrain() == 0)
        sub_0808f1b4(1, ParasolShotzoEnterState);
}

void sub_0808f7ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->updateState = 1;
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 0;
    PlaySfx(190);
    sub_0808efdc();
    u = gCurTask;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gCurTask;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gCurTask;
        w->unk6E++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gCurTask;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    TaskSleepForever();
}

void sub_0808f844(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        ActorSetState(0);
        TaskSetEntry(ParasolShotzoEnterState, gCurTaskIdx);
    }
}

void sub_0808f888(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    if (gCurTask->u8C.actor->extraFrame == -1)
    {
        ActorStopAnim();
        TaskStop();
        t = gCurTask;
        t->accelY = 168 << 5;
        t->speedLimitY = 192 << 10;
        TaskSleepForever();
    }
    else
    {
        sub_08066ba8();
        while (1)
        {
            sub_08066bdc();
            TaskYieldTrampoline(8);
        }
    }
}

void sub_0808f8dc(void)
{
    ActorCollideTerrain();
}

void ShotzoIdleInit(void)
{
    gCurTask->updateCallback = (u32)ShotzoIdleUpdate;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 1, gShotzoIdleStates);
}

void ShotzoIdleUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gShotzoIdleStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void ShotzoIdle(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrameNoFlip(6);
    TaskSleepForever();
}

void sub_0808f974(void)
{
}

s32 sub_0808f978(void)
{
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
    {
        ActorSetState(2);
        TaskSetEntry(ConerEnterState, gCurTaskIdx);
        return 1;
    }
    else
    {
        ActorSetState(1);
        TaskSetEntry(ConerEnterState, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808f9b8(void)
{
    ActorSetState(0);
    TaskSetEntry(ConerEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9d8(void)
{
    ActorSetState(2);
    TaskSetEntry(ConerEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9f8(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_0808fa04(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void Task_Coner(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gConerFrames;
    CallTableEntry(u->variant, 2, gConerVariants);
}

void ConerInit(void)
{
    gCurTask->updateCallback = (u32)ConerUpdate;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gConerStates);
}

void ConerUpdate(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void ConerEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gConerStates);
}

void ConerWalk(void)
{
    TaskStop();
    while (1)
    {
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][0]);
        TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][1]);
        TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][2]);
        gCurTask->velX = 0;
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][3]);
    }
}

void sub_0808fb50(void)
{
    TaskInitWaterFlags();
    gCurTask->accelY = 128 << 5;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0808fb80(void)
{
    TaskStopY();
    gCurTask->velY = 128 << 7;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void ConerIdleInit(void)
{
    gCurTask->updateCallback = (u32)ConerIdleUpdate;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gConerIdleStates);
}

void ConerIdleUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

void ConerIdle(void)
{
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[0][0]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][1]);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[0][2]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][3]);
    }
}

void Task_LaserBallLaser(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gLaserBallLaserFrames;
    TaskFaceLikeParent();
    gCurTask->onGround = 0;
    u = gCurTask;
    u->unk28 = 0;
    CallTableEntry(u->variant, 1, gUnk_08743600);
}

void sub_0808fc90(void)
{
    gCurTask->updateCallback = (u32)sub_0808fcd4;
    PlaySfx(165);
    TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
    ActorSetState(1);
    CallTableEntry(gCurTask->state, 2, gUnk_08743604);
}

void sub_0808fcd4(void)
{
    if (sub_08069604() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_0874360C);
    else
    {
        ActorSetState(0);
        TaskSetEntry(sub_0808fd1c, gCurTaskIdx);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808fd1c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08743604);
}

void sub_0808fd38(void)
{
    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    gCurTask->frame = 0;
    TaskSleepForever();
}

void sub_0808fd5c(void)
{
}

void sub_0808fd60(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    gCurTask->updateCallback = 0;
    TaskStop();
    t = gCurTask;
    t->pixelX += t->facing * 16;
    t->posX = t->pixelX << 16;
    t->frame = 1;
    TaskYieldTrampoline(2);
    ActorDestroy();
    TaskSleepForever();
}

void sub_0808fdb4(void)
{
}

void Task_ShotzoCannonball(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gShotzoCannonballFrames;
    CallTableEntry(u->variant, 4, gUnk_0874362C);
}

void sub_0808fdf8(void)
{
    gCurTask->updateCallback = (u32)sub_0808fe28;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_0874363C);
}

void sub_0808fe28(void)
{
    if (sub_08069660() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_08743640);
    else
        TaskSetEntry(ActorDie, gCurTaskIdx);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808fe6c(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874363C);
}

void sub_0808fe88(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    t = gCurTask;
    t->unk28 = gUnk_08743614[t->unk74];
    switch (t->unk30 = (&gTasks[t->parent])->unk34)
    {
    case 0:
        AngleToVector(0, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 1:
        AngleToVector(224 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 2:
        AngleToVector(192 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 3:
        AngleToVector(160 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 4:
        AngleToVector(128 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    u = gCurTask;
    u->velX = gUnk_030023B4;
    u->velY = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
}

void sub_0808ffe0(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk28 < 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}
