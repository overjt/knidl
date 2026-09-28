/* game_code_and_rodata 0x08084D14-0x080860F8 (issue #69, module M22 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08084D14 0x080860F8 src/enemy_84d14.c --newpb
 *
 * Three more scripts in the same three-table shape as src/enemy_82e68.c:
 *   * class-3 task #14 (entry `Task_Chilly` in the previous file): unk73 rows
 *     `0x08741FB8`, bodies `0x08741FC0`, guards `0x08741FD4`, per-frame hook
 *     `ChillyUpdate`;
 *   * class-3 task #17 (`Task_WaddleDoo`): `0x08741FE8` / `0x08741FF8` /
 *     `0x08742004`, per-frame hook `WaddleDooWalkUpdate`;
 *   * the `ParasolWaddleDooInit` script: `0x08742030` / `0x08742040`, per-frame hook
 *     `ParasolWaddleDooUpdate`;
 *   * class-3 task #20 (`Task_BrontoBurt` / `sub_080860d8`), whose seven unk73
 *     rows at `0x08742064` all point INTO module M23 - the first cross-module
 *     dispatch found in the behaviour banks.
 *
 * `sub_08085390` / `sub_080853c8` / `sub_08085404` / `sub_0808542c` and
 * `sub_08085e74` / `sub_08085ef0` / `sub_08085fa0` / `sub_08086024` /
 * `sub_08085fec` are the class-3 hook rows at `0x08742D0C` / `0x08742D1C` and
 * `0x08742D28` / `0x08742D38` / `0x08742D40`; the second group switches on
 * Task.variant (0 = plain, 1 = riding a carrier, 2-3 = ignore) instead of only
 * bailing out on 1.
 *
 * `sub_08085fec` is the second leaf the prologue scan missed (lesson 4.30):
 * the table word at `0x08742D40` points at it and it clamps Task.velY (the
 * 16.16 vertical velocity) at zero.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 a);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(s32 a);
extern void ActorSetAttackBox(u32 *p);
extern void sub_08066b34(u32 *p);
extern void sub_08066c3c(u32 *p);
extern void sub_08066c08(u32 *p, s32 b);
extern s32 sub_08021a40(s32 x, s32 y);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern void ActorReactToHit(void);

void ChillyInit(void)
{
    u32 v;

    gCurTask->updateCallback = (u32)ChillyUpdate;
    v = RandomRange(4);
    switch (v)
    {
    case 0:
    case 1:
        ActorSetState(0);
        break;
    case 2:
        ActorSetState(2);
        break;
    case 3:
        ActorSetState(3);
        break;
    }
    CallTableEntry(gCurTask->state, 5, gChillyStates);
}

void ChillyEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ChillyUpdate;
    CallTableEntry(t->state, 5, gChillyStates);
}

void ChillyUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gChillyStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08084dc0(void)
{
    struct Task *t;
    u32 zero;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk6C = zero;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(2);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(2);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(8);
        u4 = gCurTask;
        u4->frame--;
        TaskYieldTrampoline(2);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(2);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(1);
        gCurTask->facing = -gCurTask->facing;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->facing = -gCurTask->facing;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08084e74(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void sub_08084e9c(void)
{
    s32 a;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    gCurTask->updateState = 1;
    while (1)
    {
        a = gCurTask->facing;
        if (a == TaskGetFacingTowardNearestPlayer()
            && abs(TaskGetNearestPlayerDy()) <= 32)
        {
            ActorSetState(2);
            TaskSleepForever();
        }
        gCurTask->facing = -gCurTask->facing;
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(2);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(2);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(8);
        u4 = gCurTask;
        u4->frame--;
        TaskYieldTrampoline(2);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(2);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(1);
    }
}

void sub_08084f50(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void sub_08084f78(void)
{
    struct Task *t;
    u16 zero;

    gCurTask->updateState = 2;
    t = gCurTask;
    zero = 0;
    if (t->unk74 == 0)
    {
        t->unk6C = zero;
        do
        {
            TaskFaceNearestPlayer();
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
            TaskSetFrame(7);
            TaskYieldTrampoline(18);
            TaskStopX();
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(6);
            TaskYieldTrampoline(6);
            TaskSetFrame(5);
            TaskYieldTrampoline(5);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
    else
    {
        t->unk6C = zero;
        do
        {
            TaskFaceNearestPlayer();
            TaskSetFrame(4);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
            TaskSetFrame(5);
            TaskYieldTrampoline(1);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetFrame(7);
            TaskYieldTrampoline(19);
            TaskStopX();
            TaskSetFrame(7);
            TaskYieldTrampoline(3);
            TaskSetFrame(6);
            TaskYieldTrampoline(5);
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            TaskSetFrame(4);
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
    }
    TaskFaceNearestPlayer();
    ActorSetState(3);
    TaskSleepForever();
}

void sub_08085158(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void sub_08085180(void)
{
    struct Task *t;
    struct Task *u;
    struct ActorSpawn sp;
    u16 zero1;
    u8 zero2;

    t = gCurTask;
    zero1 = 0;
    t->updateState = 3;
    TaskStop();
    TaskSetFrame(10);
    gCurTask->unk6C = zero1;
    do
    {
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(1);
        u = gCurTask;
        u->unk6C++;
    } while ((s16)u->unk6C <= 14);
    sp.subtype = 2;
    sp.taskType = 104;
    sp.variant = zero2 = 0;
    sp.spawnArg = u->unk74;
    sp.checkTerrain = zero2;
    gCurTask->unk46 = CreateActorFromDescHere(&sp, 0);
    gCurTask->unk6C = zero2;
    do
    {
        gCurTask->velX = -0x10000;
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 63);
    TaskStop();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08085274(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(ChillyEnterState, gCurTaskIdx);
}

void ChillyFall(void)
{
    struct Task *t;

    gCurTask->updateState = 4;
    TaskStop();
    t = gCurTask;
    t->accelY = 0x2500;
    t->speedLimitY = 0x30000;
    TaskSleepForever();
}

void sub_080852c8(void)
{
}

void sub_080852cc(void)
{
    gCurTask->updateCallback = (u32)sub_0808537c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(18);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
    }
}

void sub_0808537c(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

u8 sub_08085390(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant == 1)
        return 0;
    ActorSetState((u16)t->unk28);
    TaskSetEntry(ChillyEnterState, gCurTaskIdx);
    return 1;
}

u8 sub_080853c8(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant == 1)
        return 0;
    t->unk28 = t->state;
    ActorSetState(4);
    TaskSetEntry(ChillyEnterState, gCurTaskIdx);
    return 1;
}

u8 sub_08085404(void)
{
    if (gCurTask->variant == 1)
        return 0;
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808542c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->variant != 1 && t->state == 2)
        TaskTurnAroundAndReverseX();
    return 0;
}

void Task_WaddleDoo(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gWaddleDooFrames;
    u->u8C.actor->extraFrame = 4;
    CallTableEntry(u->variant, 4, gWaddleDooVariants);
}

void WaddleDooWalkInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)WaddleDooWalkUpdate;
    t->unk28 = 15;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gWaddleDooWalkStates);
}

void WaddleDooWalkEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)WaddleDooWalkUpdate;
    if (t->state == 0)
        t->unk28 = 80;
    CallTableEntry(gCurTask->state, 3, gWaddleDooWalkStates);
}

void WaddleDooWalkUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gWaddleDooWalkStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08085530(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_08742010[gCurTask->unk74], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_08742020[gCurTask->unk74]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08742024[u1->unk74]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08742024[u2->unk74]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08742024[u3->unk74]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08742020[u4->unk74]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08742024[u5->unk74]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08742024[u6->unk74]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08742024[u7->unk74]);
    }
}

void sub_08085608(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gCurTask;
    if (t->unk2C == 0 && --t->unk28 == 0)
    {
        v = RandomRange(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gCurTask->unk28 = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        ActorSetState(n);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
    }
}

void sub_08085660(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x28000, 0x1500, 0x30000);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskSleepForever();
}

void sub_080856dc(void)
{
}

void sub_080856e0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    TaskStop();
    u = gCurTask;
    u->unk6C = zero;
    while ((s16)gCurTask->unk6C < gUnk_08742028[gCurTask->unk74])
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    }
    v = gCurTask;
    zero2 = 0;
    v->unk34 = zero2;
    PlaySfx(110);
    TaskSetFrame(12);
    gCurTask->unk6C = zero2;
    while ((s16)gCurTask->unk6C < gUnk_0874202C[gCurTask->unk74])
    {
        sp.subtype = 4;
        sp.taskType = 106;
        sp.variant = 0;
        w = gCurTask;
        sp.spawnArg = w->unk74;
        sp.x = 8;
        sp.y = 3;
        sp.checkTerrain = 1;
        if (sub_08021a40(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
            gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->unk34++;
        if ((s16)x->frame == 12)
            TaskSetFrame(13);
        else
            TaskSetFrame(12);
        gCurTask->unk6C++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0808582c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C == 0 && t->state != 2)
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
}

void ParasolWaddleDooInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)ParasolWaddleDooUpdate;
    sub_08066b34(gParasolWaddleDooDef);
    gCurTask->unk28 = 15;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 4, gParasolWaddleDooStates);
}

void ParasolWaddleDooEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ParasolWaddleDooUpdate;
    if (t->state == 0)
        t->unk28 = 80;
    CallTableEntry(gCurTask->state, 4, gParasolWaddleDooStates);
}

void ParasolWaddleDooUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 4, gParasolWaddleDooStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080858fc(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateState = 0;
    TaskSetMotionXFacing(gUnk_08742010[1], 0x5A5A5A5A);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_08742020[1]);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(gUnk_08742020[1]);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(gUnk_08742024[1]);
    }
}

void sub_08085998(void)
{
    struct Task *t;
    u32 v;
    s32 n;

    t = gCurTask;
    if (t->unk2C == 0 && t->u8C.actor->extraFrame == -1 && --t->unk28 == 0)
    {
        v = RandomRange(4);
        switch (v)
        {
        case 2:
        case 3:
            n = 2;
            break;
        case 0:
            gCurTask->unk28 = 30;
            return;
        case 1:
            n = 1;
            break;
        default:
            return;
        }
        ActorSetState(n);
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
    }
}

void sub_08085a04(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x28000, 0x1500, 0x30000);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskSleepForever();
}

void sub_08085a80(void)
{
}

void sub_08085a84(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    u16 zero;
    s32 zero2;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    TaskStop();
    u = gCurTask;
    u->unk6C = zero;
    while ((s16)gCurTask->unk6C < gUnk_08742028[1])
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    }
    v = gCurTask;
    zero2 = 0;
    v->unk34 = zero2;
    PlaySfx(110);
    TaskSetFrame(12);
    gCurTask->unk6C = zero2;
    while ((s16)gCurTask->unk6C < gUnk_0874202C[gCurTask->unk74])
    {
        sp.subtype = 4;
        sp.taskType = 106;
        sp.variant = 0;
        sp.spawnArg = 1;
        sp.x = 8;
        sp.y = 3;
        sp.checkTerrain = 1;
        w = gCurTask;
        if (sub_08021a40(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
            gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->unk34++;
        if ((s16)x->frame == 12)
            TaskSetFrame(13);
        else
            TaskSetFrame(12);
        gCurTask->unk6C++;
    }
    ActorSetState(0);
    TaskSleepForever();
}

void sub_08085bb8(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C == 0 && t->state != 2)
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
}

void sub_08085be4(void)
{
    gCurTask->updateState = 3;
    gCurTask->unk30 = ActorStartAnim(gUnk_08742050);
    sub_08066ba8();
    while (1)
    {
        sub_08066bdc();
        TaskYieldTrampoline(8);
    }
}

void sub_08085c10(void)
{
    gCurTask->unk30 = ActorTickAnim(gCurTask->unk30);
}

void sub_08085c2c(void)
{
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateCallback = (u32)sub_08085cc4;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
        u1 = gCurTask;
        u1->frame++;
        TaskYieldTrampoline(7);
        u2 = gCurTask;
        u2->frame++;
        TaskYieldTrampoline(7);
        u3 = gCurTask;
        u3->frame++;
        TaskYieldTrampoline(7);
        u4 = gCurTask;
        u4->frame++;
        TaskYieldTrampoline(10);
        u5 = gCurTask;
        u5->frame--;
        TaskYieldTrampoline(7);
        u6 = gCurTask;
        u6->frame--;
        TaskYieldTrampoline(7);
        u7 = gCurTask;
        u7->frame--;
        TaskYieldTrampoline(7);
    }
}

void sub_08085cc4(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08085cd8(void)
{
    struct Task *w;
    struct Task *x;
    struct ActorSpawn sp;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gCurTask->updateCallback = (u32)sub_08085e60;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskStop();
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(10);
            u1 = gCurTask;
            u1->frame++;
            TaskYieldTrampoline(7);
            u2 = gCurTask;
            u2->frame++;
            TaskYieldTrampoline(7);
            u3 = gCurTask;
            u3->frame++;
            TaskYieldTrampoline(7);
            u4 = gCurTask;
            u4->frame++;
            TaskYieldTrampoline(10);
            u5 = gCurTask;
            u5->frame--;
            TaskYieldTrampoline(7);
            u6 = gCurTask;
            u6->frame--;
            TaskYieldTrampoline(7);
            u7 = gCurTask;
            u7->frame--;
            TaskYieldTrampoline(7);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        TaskSetFrame(6);
        TaskYieldTrampoline(32);
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(2);
            TaskSetFrame(11);
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 15);
        gCurTask->unk34 = 0;
        PlaySfx(110);
        TaskSetFrame(12);
        gCurTask->unk6C = 0;
        do
        {
            sp.subtype = 4;
            sp.taskType = 106;
            sp.variant = 0;
            sp.spawnArg = 0;
            sp.x = 8;
            sp.y = 3;
            sp.checkTerrain = 1;
            w = gCurTask;
            if (sub_08021a40(w->pixelX + (w->facing << 3), w->pixelY + 3) == 0)
                gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
            TaskYieldTrampoline(2);
            x = gCurTask;
            x->unk34++;
            if ((s16)x->frame == 12)
                TaskSetFrame(13);
            else
                TaskSetFrame(12);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 15);
    }
}

void sub_08085e60(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

u8 sub_08085e74(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->unk2C = 0;
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
        return 1;
    case 1:
        ActorStopAnim();
        sub_08066c3c(gWaddleDooDef);
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
        return 1;
    }
}

u8 sub_08085ef0(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->unk2C = 1;
        t->accelY = 0x1500;
        t->speedLimitY = 0x30000;
        ActorSetState(0);
        TaskSetEntry(WaddleDooWalkEnterState, gCurTaskIdx);
        return 1;
    case 1:
        if (t->u8C.actor->extraFrame == -1)
        {
            t->unk2C = 1;
            t->accelY = 0x1500;
            t->speedLimitY = 0x30000;
            ActorSetState(0);
            TaskSetEntry(ParasolWaddleDooEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(3);
            TaskSetEntry(sub_08085be4, gCurTaskIdx);
        }
        return 1;
    }
}

u8 sub_08085fa0(void)
{
    switch (gCurTask->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        sub_0806a0f0(-2);
        return 1;
    case 1:
        sub_08066c08(gWaddleDooDef, 0);
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_08085fec(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        t->velY = 0;
        return 0;
    case 1:
        if (t->velY < 0)
            t->velY = 0;
        return 0;
    }
}

s32 sub_08086024(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        return 0;
    case 0:
        TaskTurnAroundAndReverseX();
        return 0;
    case 1:
        if (t->state == 3)
            sub_08066b70();
        else
            TaskTurnAroundAndReverseX();
        return 0;
    }
}

void sub_0808606c(void)
{
    sub_08066c08(gWaddleDooDef, 0);
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_BrontoBurt(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gBrontoBurtFrames;
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->variant, 7, gBrontoBurtVariants);
}

void sub_080860d8(void)
{
    CallTableEntry(gCurTask->variant, 7, gBrontoBurtVariants);
}
