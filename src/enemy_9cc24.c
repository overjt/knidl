
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
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);

void sub_0809cc24(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)sub_0809cd4c;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    t = gCurTask;
    t->frame = 4;
    TaskYieldTrampoline(9);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(7);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->variant = 0;
    ActorSetState(0);
    t = gCurTask;
    t->updateCallback = (u32)AxeKnightUpdate;
    t->updateState = 0;
    t = gCurTask;
    t->unk2C = 90;
    t->unk30 = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cd4c(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && sub_0809f994() != 0)
        sub_0809f930();
    t = gCurTask;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0809cd8c(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)sub_0809cec4;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    t = gCurTask;
    t->frame = 4;
    TaskYieldTrampoline(9);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    gCurTask->variant = 0;
    ActorSetState(0);
    t = gCurTask;
    t->updateCallback = (u32)AxeKnightUpdate;
    t->updateState = 0;
    t = gCurTask;
    t->unk2C = 90;
    t->unk30 = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    sub_0809c5a4();
}

void sub_0809cec4(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && sub_0809f994() != 0)
        sub_0809f930();
    t = gCurTask;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void CreateAxeKnightAxe(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    PlaySfx(186);
    sp.subtype = 26;
    sp.taskType = 129;
    sp.variant = (t = gCurTask)->variant;
    sp.spawnArg = t->unk74;
    sp.x = 20;
    sp.y = 0;
    sp.tileWord = gUnk_08745CEC[0];
    sp.checkTerrain = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void Task_AxeKnightAxe(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gAxeKnightAxeFrames;
    u->updateCallback = (u32)sub_0809cfe0;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(224 << 10, 0xFFFFDB00);
    gCurTask->speedLimitX = 128 << 11;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(5);
        TaskYieldTrampoline(2);
    }
}

void sub_0809cfe0(void)
{
    struct PointPair box;
    struct Task *t;
    struct Task *w;
    s32 i;

    t = gCurTask;
    box.x0 = t->pixelX - 10;
    box.y0 = t->pixelY - 4;
    box.x1 = t->pixelX + 10;
    box.y1 = t->pixelY + 4;
    if (gTaskSlotTypes[i = t->parent] == 58 && gTasks[i].unk74 == 0
        && TaskIsInRectSlot(&box, i) != 0)
    {
        w = &gTasks[gCurTask->parent];
        w->unk18 = 1;
        ActorDestroy();
    }
    else
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

u8 sub_0809d0a0(void)
{
    u8 r;

    if (gCurTask->variant != 0)
        r = 0;
    else
    {
        TaskStopY();
        ActorSetState(0);
        TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
        r = 1;
    }
    return r;
}

u8 sub_0809d0dc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->variant == 0 && t->state == 1)
    {
        t->posX -= t->velX;
        t->pixelX = t->posX >> 16;
        TaskStopX();
    }
    else
    {
        u = gCurTask;
        if (u->variant != 1)
        {
            sub_0809f930();
        }
        else
        {
            u->posX -= u->velX;
            u->pixelX = u->posX >> 16;
            sub_0809f970();
        }
    }
    return 0;
}

u8 sub_0809d138(void)
{
    return 0;
}

void sub_0809d13c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)sub_0809d17c;
    t->frameTable = gJavelinKnightFrames;
    ActorLoadDef((u32)gUnk_08747E64);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->variant, 2, gJavelinKnightVariants);
}

void sub_0809d17c(void)
{
    ActorDrawWorldInViewOrDestroy();
    sub_0809d994();
}

void sub_0809d18c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)JavelinKnightUpdate;
    t->unk28 = 3;
    sub_0809f90c();
    sub_0809d8f8();
    ActorSetState(1);
    u = gCurTask;
    u->updateState = 0;
    sub_0809d2a4();
}

void JavelinKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gJavelinKnightStates);
}

void JavelinKnightUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s16 *p;
    s8 *q;
    s32 lim;

    if (ActorCollideTerrain() == 0)
    {
        if (sub_0809f994() != 0)
            sub_0809dbc4();
        t = gCurTask;
        p = &t->pixelY;
        a = t->u8C.actor;
        q = (s8 *)a->terrainBox;
        lim = q[2];
        if (*p < lim)
        {
            *p = lim;
            t->posY = *p << 16;
            sub_0809dc3c();
        }
        CallTableEntry(gCurTask->updateState, 5, gJavelinKnightStateUpdates);
    }
    u = gCurTask;
    if (u->unk24 > 0)
    {
        u->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0809d25c(void)
{
    gCurTask->updateState = 0;
    gCurTask->unk28 = 3;
    ActorSetState(1);
    sub_0809d280();
}

void sub_0809d280(void)
{
    gCurTask->updateState = 0;
    TaskYieldTrampoline(2);
    sub_0809f90c();
    sub_0809d8f8();
    sub_0809d2a4();
}

void sub_0809d2a4(void)
{
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        TaskSetFrame(8);
        TaskYieldTrampoline(4);
        TaskSetFrame(9);
        TaskYieldTrampoline(4);
        TaskSetFrame(10);
        TaskYieldTrampoline(4);
        TaskSetFrame(11);
        TaskYieldTrampoline(4);
    }
}

void sub_0809d308(void)
{
}

void sub_0809d30c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 1;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->onGround = z;
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFF0000;
    u->accelY = 128 << 5;
    TaskSetFrame(12);
    TaskYieldTrampoline(2);
    TaskSetFrame(13);
    TaskYieldTrampoline(2);
    TaskSetFrame(14);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(8);
    TaskSetFrame(28);
    TaskYieldTrampoline(6);
    TaskSetFrame(29);
    TaskYieldTrampoline(4);
    TaskSetFrame(30);
    TaskYieldTrampoline(4);
    TaskSetFrame(31);
    TaskYieldTrampoline(4);
    TaskSetFrame(32);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(10);
    TaskYieldTrampoline(4);
    TaskSetFrame(11);
    TaskYieldTrampoline(3);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809d42c(void)
{
    struct Task *t;
    u16 v;

    if (gCurTask->state != 2)
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
    t = gCurTask;
    v = t->frame;
    if ((u16)(v - 12) <= 3)
        ActorCheckHitsWithBox(gUnk_08747B10[t->frame - 12]);
    else if ((u16)(v - 28) <= 4)
        ActorCheckHitsWithBox(gUnk_08747B24[t->frame - 28]);
}

void sub_0809d4a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 2;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->onGround = z;
    u = gCurTask;
    u->velY = 0xFFFE0000;
    u->accelY = 128 << 5;
    TaskSetFrame(4);
    TaskYieldTrampoline(6);
    TaskSetFrame(5);
    TaskYieldTrampoline(6);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    CreateJavelinKnightJavelin();
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(8);
        TaskYieldTrampoline(6);
        TaskSetFrame(9);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(6);
        TaskSetFrame(11);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
    }
}

void sub_0809d568(void)
{
}

void sub_0809d56c(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    TaskStop();
    sub_0809f90c();
    sub_0809d6dc();
    gCurTask->onGround = z;
    u = gCurTask;
    u->velY = 0xFFFC0000;
    u->accelY = 128 << 6;
    while (1)
    {
        PlaySfx(231);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        TaskSetFrame(20);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        TaskSetFrame(22);
        TaskYieldTrampoline(2);
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d608(void)
{
    struct Task *t;
    u16 v;

    t = gCurTask;
    v = t->frame;
    if ((u16)(v - 17) <= 7)
        ActorCheckHitsWithBox(gUnk_08747B38[t->frame - 17]);
}

void sub_0809d638(void)
{
    gCurTask->updateState = 4;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809d6b4(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
}

void sub_0809d6dc(void)
{
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(25);
        TaskYieldTrampoline(2);
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 3);
}

void sub_0809d71c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 1 && (gFrameCount & 1) != 0)
    {
        t->unk2C = 1;
        ActorSetState(4);
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
        return;
    }
    gCurTask->unk2C = 0;
    if (abs(TaskGetNearestPlayerDx()) > 39)
    {
        ActorSetState(3);
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
        return;
    }
    ActorSetState(2);
    TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
}

void sub_0809d7a4(void)
{
    struct Task *u;

    gCurTask->updateCallback = (u32)sub_0809d83c;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    u = gCurTask;
    u->unk28 = 4;
    u->unk2C = 1;
    while (1)
    {
        PlaySfx(231);
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        TaskSetFrame(20);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(2);
        TaskSetFrame(22);
        TaskYieldTrampoline(2);
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
    }
}

void sub_0809d83c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->unk30 = 1;
    ActorCollideTerrain();
    if (sub_0809f994() != 0)
    {
        sub_0809dbc4();
        gCurTask->unk30 = 0;
    }
    if (gCurTask->pixelY <= 15)
    {
        TaskStopY();
        u = gCurTask;
        u->posY = 128 << 13;
        u->pixelY = 16;
        if (u->unk2C == 0)
            u->unk30 = 0;
    }
    v = gCurTask;
    if (v->unk30 != 0)
    {
        if (v->unk2C == 0)
            v->velY += 0xFFFFFB00;
        else
            v->velY += 160 << 3;
    }
    gCurTask->onGround = 0;
    w = gCurTask;
    if (w->unk24 > 0)
    {
        w->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
    ActorCheckHitsWithBox(gUnk_08747B38[gCurTask->frame - 17]);
}

void sub_0809d8f8(void)
{
    struct Task *t;
    u32 r;

    r = (u8)RandomRange(8);
    TaskSetMotionXFacing(gUnk_08747B58[r >> 1], 0x5A5A5A5A);
    t = gCurTask;
    t->velY = gUnk_08747B68[r];
    t->accelY = 128 << 5;
    t->onGround = 0;
}

void CreateJavelinKnightJavelin(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    sp.subtype = 29;
    sp.taskType = 132;
    sp.variant = (t = gCurTask)->variant;
    sp.spawnArg = t->unk74;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = 0xF110;
    sp.checkTerrain = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
}
