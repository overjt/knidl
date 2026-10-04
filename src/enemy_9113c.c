/* game_code_and_rodata 0x0809113C-0x08091F9C (issue #67, module M25 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0809113C 0x08091F9C src/enemy_9113c.c --newpb
 *
 * M25's second boss script, dispatched through the 15-entry anchor table at
 * 0x08743984; its entry (Task_PoppyBrosSr) is the last function of
 * src/enemy_9000c.c.  Same skeleton as the first: PoppyBrosSrInit installs the
 * per-frame body PoppyBrosSrUpdate and the animation script gUnk_0874397C,
 * PoppyBrosSrUpdate dispatches Task.updateState and reloads the graphics record
 * gPoppyBrosSrGfx, and states 0-6 are <body, guard> pairs.  Two states shell out
 * to sub_08091954, which runs the boss's attack loop: it repeats Task.unk1C
 * times, flips Task.unk20 between the two step helpers sub_08091a30 and
 * sub_08091a98 (they walk Task.frame up and down while yielding Task.unk24
 * frames per step), and waits on Task.onGround between passes.
 *
 * PoppyBrosSrReactToDamage / PoppyBrosSrReactToDefeat / sub_08091b60 are the hit hooks (they return
 * 1 when they take over the task), and Task_PoppyBrosSrHand is the class-4 companion
 * the boss spawns: it builds an ActorSpawn on the stack, then flies the task
 * along three 16.16 ramps (Task.unk28 / Task.unk30) with a wait in the middle
 * for gTasks[Task.parent].unk20 to clear.  PoppyBrosSrHandUpdate is that
 * companion's per-frame body (it integrates the ramps and copies the boss's
 * position and facing), and Task_PoppyBrosSrHead installs PoppyBrosSrHeadUpdate, the animation
 * walker that mirrors the leader's animation onto the companion and steps the
 * companion's own AnimCmd script from gPoppyBrosSrHeadAnims.
 *
 * Task_Bugzzy (the last function here) is the entry of M25's third boss, whose
 * states live in src/enemy_91f9c.c: it installs ActorMove / sub_080653ec as
 * the draw and per-frame hooks, points Task.frameTable at gBugzzyFrames, counts the
 * boss into gUnk_02007D00[7], seeds the state block (Task.unk28 = -1,
 * Task.unk34 = 1, Task.unk1C = -1, Task.unk24 = Actor.palette) and dispatches
 * Task.variant through the 27-entry anchor table at 0x08743ADC.
 *
 * PoppyBrosSrHeadUpdate's end-of-script store (`w->unk28 = 0`) emits no
 * code: the timer is already 0 on that path, so post-reload cse deletes it,
 * but until reload it keeps the decremented timer live into the frame test,
 * which is the ROM's register allocation (lessons-learned 3.526).
 */#include "gba/gba.h"
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
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u8 ActorCollideTerrain(void);
extern s32 ActorReactToHit(void);
extern u8 sub_0806acf8(void);
extern s32 Div(s32 numerator, s32 denominator);

void PoppyBrosSrInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)PoppyBrosSrUpdate;
    t->unk2C = 0;
    t->unk30 = 0;
    t->unk34 = 0;
    gCurTask->unk46 = CreateChildTaskHere(179, 1);
    sub_080666cc(gUnk_0874397C);
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 7, gPoppyBrosSrStates);
}

void PoppyBrosSrEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gPoppyBrosSrStates);
}

void PoppyBrosSrUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk18 != 0)
    {
        t->unk18--;
        CallTableEntry(t->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    else if (sub_0806acf8() == 0)
    {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 7, gPoppyBrosSrStateUpdates);
    }
    u = gCurTask;
    if (u->unk34 != 0)
    {
        if (u->hitTimer != 1)
            sub_08066480(&gPoppyBrosSrGfx, (u32)&gUnk_08275670, 16);
        else
        {
            u->unk34 = 0;
            ActorLoadHeaderPalette(&gPoppyBrosSrGfx);
        }
    }
    ActorSetAttackBox(gUnk_087438EC[gCurTask->frame]);
    sub_080639f0(gUnk_0874391C[gCurTask->frame]);
    sub_08063a00(gUnk_0874394C[gCurTask->frame]);
    sub_08068f68();
    ActorReactToHit();
}

void PoppyBrosSrState0(void)
{
    struct Task *t;
    u8 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = zero;
    if (sub_08067060() == 1)
    {
        gCurTask->onGround = zero;
        TaskSetFrame(4);
        TaskSetMotionY(0, 5376, 196608);
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        PlaySfx(528);
        TaskStop();
    }
    sub_08066580();
    ActorSetState(1);
    TaskSleepForever();
}

void PoppyBrosSrState0Update(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState1(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 1;
    u = gCurTask;
    u->unk20 = zero;
    gCurTask->unk1C =
        gUnk_087438DC[RandomRange(4) + gCurTask->unk74 * 4];
    sub_08091954();
    ActorSetState(2);
    TaskSleepForever();
}

void PoppyBrosSrState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState2(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *p2;
    s32 zero;
    s32 n;

    t = gCurTask;
    zero = 0;
    t->updateState = 2;
    gCurTask->onGround = zero;
    TaskSetMotionY(0xFFFD0000, 4096, 196608);
    if (abs(TaskGetNearestPlayerDy()) <= 63)
        gCurTask->unk1C = 1;
    else
        gCurTask->unk1C = 0;
    gCurTask->unk20 = -1;
    p = &gTasks[(s16)CreateChildTaskHere(180, 1)];
    p->unk18 = gCurTask->unk1C;
    gCurTask->unk30 = 2;
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 1;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(4);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(4);
    v = gCurTask;
    if (v->unk1C == 0)
    {
        v->unk20 = 4;
    }
    else
    {
        p2 = v;
        p2->unk24 = abs(TaskGetNearestPlayerDx());
        w = gCurTask;
        n = w->unk24;
        if (n <= 47)
            w->unk20 = 64;
        else if (n <= 79)
            w->unk20 = 1;
        else
            w->unk20 = 36;
    }
    while (1)
    {
        if (gCurTask->unk20-- == 0)
            break;
        TaskYieldTrampoline(1);
    }
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(3);
    TaskSleepForever();
}

void PoppyBrosSrState2Update(void)
{
    struct Task *t;

    if (gCurTask->unk20 != 0)
    {
        TaskFaceNearestPlayer();
        TaskUpdateFlip();
    }
    t = gCurTask;
    if (t->velY > 0 && t->unk30 == 2)
        t->unk30 = 3;
    if (gCurTask->state != 2)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState3(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 3;
    u = gCurTask;
    u->unk20 = zero;
    u->unk1C = 2;
    sub_08091954();
    ActorSetState(4);
    TaskSleepForever();
}

void PoppyBrosSrState3Update(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState4(void)
{
    struct Task *t;
    u8 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 4;
    TaskFaceNearestPlayer();
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(81920, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFEC000, 4096, 196608);
    gCurTask->unk24 = 6;
    sub_08091a30();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    PlaySfx(528);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFEC000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE8000, 4096, 196608);
    sub_08091a98();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(5);
    TaskSleepForever();
}

void PoppyBrosSrState4Update(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrState5(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateState = 5;
    gCurTask->onGround = zero;
    TaskSetMotionXFacing(163840, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE4000, 4096, 196608);
    gCurTask->unk24 = 7;
    sub_08091a30();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    PlaySfx(528);
    gCurTask->onGround = 0;
    if (abs(TaskGetNearestPlayerDx()) <= 55)
    {
        TaskSetMotionXFacing(0xFFFD8000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE4000;
    }
    else
    {
        TaskStopX();
        u = gCurTask;
        u->velY = 0xFFFE0000;
        u->unk24 = 10;
    }
    sub_08091a98();
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(528);
    ActorSetState(1);
    TaskSleepForever();
}

void PoppyBrosSrState5Update(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
}

void PoppyBrosSrDefeat(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 6;
    if (--gUnk_02007D00[0] == 0)
        sub_0806684c();
    sub_080667c0(0, 10);
    CreateStarFlash(1, 0, 0);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 6656, 196608);
    TaskSetFrame(10);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(2);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 4, 16, 4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFF0000, 32768, 196608);
    TaskSetFrame(11);
    TaskYieldTrampoline(32);
    TaskStop();
    TaskYieldTrampoline(170);
    TaskStopY();
    sub_0806ad18();
    gCurTask->unk2C = 2;
    TaskSleepForever();
}

void PoppyBrosSrDefeatUpdate(void)
{
    sub_08066480(&gPoppyBrosSrGfx, (u32)&gUnk_08275670, 16);
    if (gCurTask->unk2C == 2)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_08091954(void)
{
    struct Task *u;
    struct Task *v;

    while (gCurTask->unk1C-- > 0)
    {
        TaskFaceNearestPlayer();
        if (abs(TaskGetNearestPlayerDx()) <= 39)
            gCurTask->unk20 = 1;
        gCurTask->onGround = 0;
        TaskSetMotionXFacing(gUnk_087438E4[gCurTask->unk20], 0x5A5A5A5A);
        TaskSetMotionY(0xFFFEE000, 4096, 196608);
        u = gCurTask;
        u->unk24 = 3;
        if (u->unk20 == 0)
            sub_08091a30();
        else
            sub_08091a98();
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
        v = gCurTask;
        v->unk20 ^= 1;
        PlaySfx(528);
    }
    TaskStop();
}

void sub_08091a30(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->unk30 = 0;
    TaskSetFrame(4);
    TaskYieldTrampoline(gCurTask->unk24);
    u = gCurTask;
    u->frame += 2;
    TaskYieldTrampoline(u->unk24);
    v = gCurTask;
    v->frame += 2;
    TaskYieldTrampoline(v->unk24);
    w = gCurTask;
    w->frame += 1;
    TaskYieldTrampoline(w->unk24);
    x = gCurTask;
    x->frame -= 2;
    TaskYieldTrampoline(x->unk24);
    y = gCurTask;
    y->frame -= 2;
    TaskYieldTrampoline(y->unk24);
}

void sub_08091a98(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->unk30 = 1;
    TaskSetFrame(5);
    TaskYieldTrampoline(gCurTask->unk24);
    u = gCurTask;
    u->frame += 2;
    TaskYieldTrampoline(u->unk24);
    v = gCurTask;
    v->frame += 2;
    TaskYieldTrampoline(v->unk24);
    w = gCurTask;
    w->frame -= 1;
    TaskYieldTrampoline(w->unk24);
    x = gCurTask;
    x->frame -= 2;
    TaskYieldTrampoline(x->unk24);
    y = gCurTask;
    y->frame -= 2;
    TaskYieldTrampoline(y->unk24);
}

s32 PoppyBrosSrReactToDamage(void)
{
    gCurTask->unk34 = 1;
    CreateStarFlash(1, 0, -8);
    RequestScreenShake(2);
    return 0;
}

s32 PoppyBrosSrReactToDefeat(void)
{
    TaskSetFrame(10);
    ActorSetHitReactions(gUnk_087440F4);
    gCurTask->unk2C = 1;
    ActorSetState(6);
    TaskSetEntry(PoppyBrosSrEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_08091b60(void)
{
    TaskStopX();
    return 0;
}

void Task_PoppyBrosSrHand(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *p;
    struct Task *q;
    struct ActorSpawn spawn;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->moveCallback = zero;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 14;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrHandFrames;
    u->updateCallback = (u32)PoppyBrosSrHandUpdate;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk2C = 0x100000;
    v->unk30 = zero;
    v->unk34 = 0x40000;
    v->unk1C = zero;
    spawn.subtype = 9;
    spawn.taskType = 111;
    spawn.variant = v->unk18;
    spawn.spawnArg = v->unk74;
    spawn.x = zero;
    spawn.y = zero;
    spawn.checkTerrain = 1;
    spawn.tileWord = sub_08066630(1);
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&spawn, 1);
    gCurTask->unk28 = 0x30000;
    gCurTask->unk30 = 0xFFFA0000;
    TaskSetFrame(5);
    gCurTask->unk6C = zero;
    do
    {
        x = gCurTask;
        x->unk28 += 0xFFFF7000;
        x->unk30 += 0x8000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 15);
    y = gCurTask;
    y->unk28 = 0;
    y->unk30 = 0;
    p = gCurTask;
    while ((q = &gTasks[p->parent])->unk20 != 0)
    {
        TaskYieldTrampoline(1);
        p = gCurTask;
    }
    z = gCurTask;
    z->unk28 = 0x88000;
    z->unk30 = 0xFFFC0000;
    z->unk6C = 0;
    do
    {
        a = gCurTask;
        a->unk28 += 0xFFFF0000;
        a->unk30 += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 3);
    b = gCurTask;
    b->unk1C++;
    PlaySfx(0x20F);
    TaskSetFrame(1);
    gCurTask->unk6C = 0;
    do
    {
        c = gCurTask;
        c->unk28 += 0xFFFF0000;
        c->unk30 += 0x1A000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 3);
    d = gCurTask;
    d->unk28 = 0xFFFF0000;
    d->unk30 = 0xFFFF0000;
    TaskSetFrame(3);
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void PoppyBrosSrHandUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 m;

    if ((s16)gTaskSlotTypes[gCurTask->parent] != -1)
    {
        t = gCurTask;
        u = &gTasks[t->parent];
        if (u->u76.subtype == 1 && u->unk2C == 0)
        {
            t->facing = u->facing;
            TaskUpdateFlip();
            v = gCurTask;
            v->unk2C += v->unk28;
            v->unk34 += v->unk30;
            v->pixelX = u->pixelX + (v->unk2C * u->facing >> 16);
            m = ((s16 *)v)[27];
            v->pixelY = u->pixelY + m;
        }
        else
        {
            TaskFree(gCurTaskIdx);
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_PoppyBrosSrHead(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gPoppyBrosSrHeadFrames;
    u->updateCallback = (u32)PoppyBrosSrHeadUpdate;
    u->unk30 = -1;
    TaskSleepForever();
}

void PoppyBrosSrHeadUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct AnimCmd *p;
    struct AnimCmd *q;
    u32 base;
    s16 frame;

    if ((s16)gTaskSlotTypes[gCurTask->parent] != -1)
    {
        t = gCurTask;
        u = &gTasks[t->parent];
        if (u->u76.subtype == 1 && u->unk2C == 0)
        {
            t->pixelX = u->pixelX;
            t->pixelY = u->pixelY;
            t->facing = u->facing;
            TaskUpdateFlip();
            v = gCurTask;
            if (v->unk30 != u->unk30)
            {
                v->unk30 = u->unk30;
                v->unk2C = 0;
                p = gPoppyBrosSrHeadAnims[v->unk30];
                v->unk28 = p->delay;
                v->frame = p->frame;
            }
            w = gCurTask;
            if (w->unk28 != 0 && --w->unk28 == 0)
            {
                w->unk2C++;
                base = (u32)gPoppyBrosSrHeadAnims[w->unk30];
                q = (struct AnimCmd *)(w->unk2C * 4 + base);
                frame = q->frame;
                if (frame == -1)
                {
                    /* End of the script: stop with the timer at 0.  The
                     * store is redundant on this path (the timer is already
                     * 0): cse turns it into a store of the decremented
                     * value and post-reload cse (reload_cse) deletes it, so
                     * it emits no code, but it keeps the decremented value
                     * live into the frame test, as the ROM's registers show
                     * (`subs r4, r0, #1` and the ldrsh scratches; lessons
                     * 3.156, 3.526).  Do not remove it. */
                    w->unk28 = 0;
                }
                else
                {
                    w->unk28 = q->delay;
                    w->frame = frame;
                }
            }
        }
        else
        {
            TaskFree(gCurTaskIdx);
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void Task_Bugzzy(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_080653ec;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gBugzzyFrames;
    gUnk_02007D00[7]++;
    u->unk28 = -1;
    u->unk2C = 0;
    u->unk30 = 0;
    u->unk34 = 1;
    u->unk1C = -1;
    u->unk24 = u->u8C.actor->palette;
    if (sub_08067060() == 1)
        gCurTask->unk20 = 24;
    else
        gCurTask->unk20 = 0;
    sub_08066ae0();
    CallTableEntry(gCurTask->variant, 1, gBugzzyVariants);
}
