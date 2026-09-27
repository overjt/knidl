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
 *     aim table `gUnk_0874325A` one row per Task.unk34 / Task.unk73 through
 *     `sub_0808eec4`;
 *   * the class-3 hook row `0x08743518` — `sub_0808f9b8`, `sub_0808f978`,
 *     `sub_0808f9d8` and `sub_0808f9f8`;
 *   * script 5: entry `sub_0808fa10` (`0x087432F4`, 2 rows), rows
 *     `sub_0808fa50` / `sub_0808fbac`, bodies `0x087432FC` (3) and
 *     `0x08743308` (1);
 *   * script 6: entry `sub_0808fc40` (`0x08743600`, 1 row), row
 *     `sub_0808fc90`, bodies `0x08743604` (2), guards `0x0874360C` (2);
 *   * script 7: entry `sub_0808fdb8` (`0x0874362C`, 4 identical rows), row
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

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u8 gUnk_08743248[];
extern s16 gUnk_0874325A[][4];
extern u32 gUnk_087432E4[];
extern u32 gUnk_087432E8[];
extern u8 gUnk_087432EC[][4];
extern u32 gUnk_087432F4[];
extern u32 gUnk_087432FC[];
extern u32 gUnk_08743308[];
extern u32 gUnk_08743600[];
extern u32 gUnk_08743604[];
extern u32 gUnk_0874360C[];
extern u8 gUnk_08743614[];
extern s16 gUnk_0874361A[];
extern u32 gUnk_0874362C[];
extern u32 gUnk_0874363C[];
extern u32 gUnk_08743640[];
extern u32 gUnk_0875227C[];
extern u32 gUnk_08752520[];
extern u32 gUnk_08752A70[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern void TaskInitWaterFlags(void);
extern void ActorSetState(u16 v);
extern void ActorSetAttackBox(void *p);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void ActorStopAnim(void);
extern void AngleToVector(s32 a, s32 b);
extern void TaskFaceLikeParent(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_08069888(void);
extern u32 ActorReactToHit(void);
extern void ActorDie(void);
extern void sub_0806ee2c(void);
extern void sub_0808eec4(s32 a);
extern void sub_0808efdc(void);
extern void sub_0808f058(void);
extern void sub_0808f0d0(void);
extern void sub_0808f1b4(u16 a, void *b);
extern void sub_0808f380(void);
extern void sub_0808f39c(void);
extern void sub_0808f3b8(void);

/* Forward declarations */
void sub_0808f41c(void);
void sub_0808f4b4(void);
void sub_0808f4f8(void);
void sub_0808f51c(void);
void sub_0808f528(void);
void sub_0808f578(void);
void sub_0808f5cc(void);
void sub_0808f678(void);
void sub_0808f6c0(void);
void sub_0808f71c(void);
void sub_0808f728(void);
void sub_0808f75c(void);
void sub_0808f7ac(void);
void sub_0808f844(void);
void sub_0808f888(void);
void sub_0808f8dc(void);
void sub_0808f8e8(void);
void sub_0808f930(void);
void sub_0808f954(void);
void sub_0808f974(void);
s32 sub_0808f978(void);
s32 sub_0808f9b8(void);
s32 sub_0808f9d8(void);
s32 sub_0808f9f8(void);
s32 sub_0808fa04(void);
void sub_0808fa10(void);
void sub_0808fa50(void);
void sub_0808fa84(void);
void sub_0808fa98(void);
void sub_0808fab4(void);
void sub_0808fb50(void);
void sub_0808fb80(void);
void sub_0808fbac(void);
void sub_0808fbf0(void);
void sub_0808fc00(void);
void sub_0808fc40(void);
void sub_0808fc90(void);
void sub_0808fcd4(void);
void sub_0808fd1c(void);
void sub_0808fd38(void);
void sub_0808fd5c(void);
void sub_0808fd60(void);
void sub_0808fdb4(void);
void sub_0808fdb8(void);
void sub_0808fdf8(void);
void sub_0808fe28(void);
void sub_0808fe6c(void);
void sub_0808fe88(void);
void sub_0808ffe0(void);

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
        TaskSetEntry(sub_0808f380, gCurTaskIdx);
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
    switch (gCurTask->unk73)
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
        switch (t->unk73)
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
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
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
                sub_0808eec4(gUnk_0874325A[v->unk73][v->unk2C]);
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
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
    }
}

void sub_0808f6c0(void)
{
    struct Task *t;

    switch (gCurTask->unk73)
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
    if (gCurTask->unk8C->extraFrame == -1)
    {
        if (sub_08069888() == 0)
            sub_0808f1b4(1, sub_0808f3b8);
    }
    else if (ActorCollideTerrain() == 0)
        sub_0808f1b4(1, sub_0808f3b8);
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
        TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
    }
}

void sub_0808f888(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    if (gCurTask->unk8C->extraFrame == -1)
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

void sub_0808f8e8(void)
{
    gCurTask->updateCallback = (u32)sub_0808f930;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->state, 1, gUnk_087432E4);
}

void sub_0808f930(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087432E8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808f954(void)
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
        TaskSetEntry(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
    else
    {
        ActorSetState(1);
        TaskSetEntry(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808f9b8(void)
{
    ActorSetState(0);
    TaskSetEntry(sub_0808fa98, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9d8(void)
{
    ActorSetState(2);
    TaskSetEntry(sub_0808fa98, gCurTaskIdx);
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

void sub_0808fa10(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gUnk_08752A70;
    CallTableEntry(u->unk73, 2, gUnk_087432F4);
}

void sub_0808fa50(void)
{
    gCurTask->updateCallback = (u32)sub_0808fa84;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gUnk_087432FC);
}

void sub_0808fa84(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808fa98(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_087432FC);
}

void sub_0808fab4(void)
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

void sub_0808fbac(void)
{
    gCurTask->updateCallback = (u32)sub_0808fbf0;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08743308);
}

void sub_0808fbf0(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808fc00(void)
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

void sub_0808fc40(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gUnk_0875227C;
    TaskFaceLikeParent();
    gCurTask->onGround = 0;
    u = gCurTask;
    u->unk28 = 0;
    CallTableEntry(u->unk73, 1, gUnk_08743600);
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

void sub_0808fdb8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gUnk_08752520;
    CallTableEntry(u->unk73, 4, gUnk_0874362C);
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

