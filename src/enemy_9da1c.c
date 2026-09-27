
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gViewRect[];
extern struct Task gTasks[];
extern u8 gTerrainResult[];
extern u32 gUnk_08745CEC[];
extern u32 gMaceKnightVariants[];
extern u32 gUnk_08747BD8[];
extern u32 gUnk_08747BE0[];
extern u32 gUnk_08747BE8[];
extern u32 gTridentKnightVariants[];
extern u32 gUnk_08747C04[];
extern u32 gUnk_08747C14[];
extern u8 gUnk_08747C28[];
extern u32 gUnk_08747DB4[];
extern u32 gUnk_08747E0C[];
extern u32 gUnk_08753354[];
extern u32 gMaceKnightFrames[];
extern u32 gUnk_08753404[];
extern u32 gTridentKnightFrames[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskIntegrateMotion(void);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern s32 TaskGetNearestPlayerDx(void);
extern void ActorDestroy(void);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern void TaskFaceLikeParent(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio);
extern void ActorDrawWorldInViewOrDestroy(void);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern void sub_0809d1c0(void);
extern void sub_0809d71c(void);
extern void sub_0809f2f4(void);
extern void TaskFaceScreenCenter(void);
extern void sub_0809f90c(void);
extern void sub_0809f930(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

/* Defined below */
void sub_0809da9c(void);
void sub_0809dd08(void);
void sub_0809ddbc(void);
void sub_0809de54(void);
void sub_0809dfc8(void);
void sub_0809e04c(void);
s32 sub_0809e214(void);
void sub_0809e2c4(void);
void sub_0809e630(void);
void sub_0809e780(void);
void sub_0809e864(void);
void sub_0809e8cc(void);
void sub_0809e914(void);
void sub_0809ebc0(void);
void sub_0809ec2c(void);
void sub_0809ec84(void);
void sub_0809ed08(void);
void sub_0809ed74(void);
void sub_0809ef98(void);
void sub_0809f0f0(void);
void sub_0809f26c(void);
s32 sub_0809f29c(s32 a);

void sub_0809da1c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gUnk_08753354;
    u->updateCallback = (u32)sub_0809da9c;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        gCurTask->velY = 0xFFFC8000;
    else
        gCurTask->velY = 0xFFFF0000;
    gCurTask->accelY = 184 << 5;
    TaskSleepForever();
}

void sub_0809da9c(void)
{
    struct Task *t;
    s32 vx;
    s32 vy;

    t = gCurTask;
    if (t->pixelY > gViewRect[2] + 196)
    {
        ActorDestroy();
        return;
    }
    vy = t->velY;
    if (vy < 0)
    {
        vx = t->velX;
        if (abs(vx) >> 2 < -vy >> 2)
            TaskSetFrame(4);
        else if (abs(vx) >> 1 < -vy)
            TaskSetFrame(5);
        else
            TaskSetFrame(6);
    }
    else
    {
        vx = t->velX;
        if (abs(vx) >> 2 < vy >> 2)
            TaskSetFrame(8);
        else if (abs(vx) >> 1 < vy)
            TaskSetFrame(7);
        else
            TaskSetFrame(6);
    }
    ActorCheckHits();
    ActorReactToHit();
}

u8 sub_0809db48(void)
{
    struct Task *t;
    struct Task *u;
    u8 s;
    s32 n;

    switch (gCurTask->variant)
    {
    case 0:
        TaskStopY();
        t = gCurTask;
        s = t->state;
        if (s == 1)
        {
            if (--t->unk28 > 0)
            {
                n = 1;
            }
            else
            {
                sub_0809d71c();
                return 0;
            }
        }
        else if (s == 3)
        {
            n = 0;
        }
        else if (s == 4)
        {
            n = 5;
        }
        else
        {
            break;
        }
        ActorSetState(n);
        TaskSetEntry(sub_0809d1c0, gCurTaskIdx);
        return 1;
    case 1:
        TaskStopY();
        u = gCurTask;
        if (u->unk2C == 1)
            u->unk30 = 0;
        return 0;
    }
}

u8 sub_0809dbc4(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        if (t->state == 1)
            t->velX = -t->velX;
        else
            TaskStopX();
        return 0;
    case 1:
        if ((t->onGround & 1) == 0 || (gTerrainResult[3] & 1) == 0)
        {
            t->velX = -t->velX;
            if (--t->unk28 > 0)
                break;
            t->unk28 = 4;
            t->unk2C = (t->unk2C + 1) & 1;
        }
        gCurTask->unk30 = 0;
        return 0;
    }
}

u8 sub_0809dc3c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        if (t->state == 3)
            t->velY = 0;
        return 0;
    case 1:
        TaskStopY();
        u = gCurTask;
        if (u->unk2C == 0)
            u->unk30 = 0;
        return 0;
    }
}

void sub_0809dc7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gMaceKnightFrames;
    ActorLoadDef((u32)gUnk_08747DB4);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->variant, 3, gMaceKnightVariants);
}

void sub_0809dcbc(void)
{
    gCurTask->updateCallback = (u32)sub_0809dd08;
    TaskFaceScreenCenter();
    while (1)
    {
        TaskSetFrame(9);
        TaskYieldTrampoline(8);
        TaskSetFrame(10);
        TaskYieldTrampoline(12);
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(10);
        TaskYieldTrampoline(12);
        sub_0809e04c();
    }
}

void sub_0809dd08(void)
{
    struct Task *t;
    s32 i;
    struct Task *w;
    s8 v;
    s16 *p;

    ActorCollideTerrain();
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
    v = gCurTask->hitKind;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gCurTask->unk46;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809dd7c(void)
{
    gCurTask->updateCallback = (u32)sub_0809ddbc;
    TaskFaceScreenCenter();
    ActorSetState(0);
    sub_0809de54();
}

void sub_0809dda0(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08747BD8);
}

void sub_0809ddbc(void)
{
    struct Task *t;
    s32 i;
    struct Task *w;
    s8 v;
    s16 *p;

    if (ActorCollideTerrain() == 0)
    {
        if (sub_0809f994() != 0)
            sub_0809f930();
        CallTableEntry(gCurTask->updateState, 2, gUnk_08747BE0);
    }
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
    v = gCurTask->hitKind;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gCurTask->unk46;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809de54(void)
{
    gCurTask->updateState = 0;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(5);
    TaskSetFrame(4);
    TaskYieldTrampoline(6);
    TaskSetFrame(26);
    TaskYieldTrampoline(5);
    TaskSetFrame(6);
    TaskYieldTrampoline(3);
    TaskSetFrame(7);
    TaskYieldTrampoline(5);
    TaskSetFrame(8);
    TaskYieldTrampoline(6);
    TaskSetFrame(27);
    TaskYieldTrampoline(5);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0809dee0(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_0809dda0, gCurTaskIdx);
}

void sub_0809df08(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    sub_0809e04c();
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0809df2c(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_0809dda0, gCurTaskIdx);
}

void sub_0809df54(void)
{
    gCurTask->updateCallback = (u32)sub_0809dfc8;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(176 << 9, 0x5A5A5A5A);
    sub_0809e214();
    while (1)
    {
        TaskSetFrame(15);
        TaskYieldTrampoline(3);
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        TaskSetFrame(12);
        TaskYieldTrampoline(4);
        TaskSetFrame(14);
        TaskYieldTrampoline(3);
        TaskSetFrame(16);
        TaskYieldTrampoline(4);
        TaskSetFrame(12);
        TaskYieldTrampoline(4);
    }
}

void sub_0809dfc8(void)
{
    struct Task *t;
    struct Task *w;
    s16 *p;
    s8 v;

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
    v = gCurTask->hitKind;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gCurTask->unk46;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void sub_0809e04c(void)
{
    s32 k;

    TaskSetFrame(28);
    TaskYieldTrampoline(1);
    TaskSetFrame(29);
    TaskYieldTrampoline(2);
    TaskSetFrame(30);
    TaskYieldTrampoline(2);
    TaskSetFrame(31);
    TaskYieldTrampoline(2);
    TaskSetFrame(32);
    TaskYieldTrampoline(2);
    TaskSetFrame(33);
    TaskYieldTrampoline(3);
    TaskSetFrame(34);
    TaskYieldTrampoline(16);
    sub_0809e214();
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(13);
    TaskYieldTrampoline(4);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(14);
    TaskYieldTrampoline(3);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(13);
    TaskYieldTrampoline(4);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(14);
    TaskYieldTrampoline(3);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(13);
    TaskYieldTrampoline(4);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(14);
    TaskYieldTrampoline(3);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    sub_0809f90c();
    TaskSetFrame(17);
    TaskYieldTrampoline(3);
    TaskSetFrame(18);
    TaskYieldTrampoline(3);
    TaskSetFrame(24);
    TaskYieldTrampoline(60);
    TaskSetFrame(25);
    TaskYieldTrampoline(4);
    TaskSetFrame(21);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFFC0000, k = 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFFE0000, k);
    TaskSetFrame(20);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 10, k);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 11, k);
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(10);
}

s32 sub_0809e214(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s16 *p;
    s32 z;
    s32 r;

    sp.subtype = 27;
    sp.taskType = 130;
    sp.variant = (t = gCurTask)->variant;
    z = 0;
    sp.spawnArg = t->unk74;
    sp.x = z;
    sp.y = 0xFFF0;
    sp.tileWord = gUnk_08745CEC[2];
    sp.checkTerrain = 0;
    r = CreateActorFromDesc(&sp, 1);
    p = &gCurTask->unk46;
    *p = r;
    ((struct Task *)(*p * 144 + (s32)gTasks))->unk2C = z;
}

void sub_0809e284(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)sub_0809e2c4;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_08753404;
    TaskFaceLikeParent();
    u = gCurTask;
    CallTableEntry(u->variant, 3, gUnk_08747BE8);
}

void sub_0809e2c4(void)
{
    struct Task *t;
    s16 *q;

    TaskIntegrateMotion();
    t = gCurTask;
    q = &t->parent;
    t->pixelX = ((struct Task *)(*q * 144 + (s32)gTasks))->pixelX + (t->posX >> 16);
    t->pixelY = ((struct Task *)(*q * 144 + (s32)gTasks))->pixelY + (t->posY >> 16);
}

void sub_0809e320(void)
{
    struct Task *t;
    struct Task *u;
    s32 k;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809e630;
    t->layer = 10;
    gCurTask->unk6C = 0;
    do
    {
        PlaySfx(213);
        TaskSetFrame(1);
        TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->layer = 12;
        TaskSetFrame(3);
        TaskSetMotionXFacing(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        PlaySfx(213);
        TaskSetFrame(2);
        TaskSetMotionXFacing(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->layer = 10;
        TaskSetFrame(0);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 1);
    PlaySfx(213);
    TaskSetFrame(1);
    TaskSetMotionXFacing(128 << 12, k = 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 11, k);
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(3);
    gCurTask->layer = 12;
    TaskSetFrame(3);
    TaskSetMotionXFacing(0xFFFC0000, k);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFF80000, k);
    TaskYieldTrampoline(2);
    PlaySfx(213);
    TaskSetFrame(2);
    TaskSetMotionXFacing(0xFFF80000, k);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFFC0000, k);
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(3);
    gCurTask->layer = 10;
    TaskSetFrame(0);
    TaskFaceLikeParent();
    u = gCurTask;
    if (u->facing == 1)
        u->posX = 0xFFF00000;
    else
        u->posX = 128 << 13;
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    gCurTask->velY = 128 << 12;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(160 << 11, 0x5A5A5A5A);
    gCurTask->velY = 160 << 11;
    TaskYieldTrampoline(1);
    TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
    gCurTask->velY = 192 << 10;
    TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(213);
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(12);
    TaskSetMotionXFacing(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
    TaskYieldTrampoline(7);
    TaskStop();
    TaskYieldTrampoline(2);
    gTasks[gCurTask->parent].unk46 = 0xFFFF;
    ActorDestroy();
}

void sub_0809e630(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 0)
    {
        gTasks[t->parent].unk46 = 0xFFFF;
        ActorDestroy();
    }
    else
    {
        ActorCheckHits();
    }
}

void sub_0809e670(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809e780;
    t->layer = 12;
    TaskSetFrame(0);
    gCurTask->posY = 0xFFF00000;
    while (1)
    {
        gCurTask->posX = 0;
        PlaySfx(213);
        TaskSetFrame(1);
        TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->layer = 12;
        TaskSetFrame(3);
        TaskSetMotionXFacing(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        PlaySfx(213);
        TaskSetFrame(2);
        TaskSetMotionXFacing(0xFFF80000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0xFFFC0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->layer = 10;
        TaskSetFrame(0);
        TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    }
}

void sub_0809e780(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk2C != 0)
    {
        gTasks[t->parent].unk46 = 0xFFFF;
        ActorDestroy();
    }
    else
    {
        t->posY = 0xFFF00000;
        ActorCheckHits();
    }
}

u8 sub_0809e7c8(void)
{
    TaskStopY();
    return 0;
}

u8 sub_0809e7d4(void)
{
    gCurTask->accelY = 148 << 6;
    return 0;
}

u8 sub_0809e7e8(void)
{
    struct Task *t;

    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        break;
    case 1:
        if (t->state == 0)
            sub_0809f930();
        break;
    case 2:
        sub_0809f930();
        break;
    }
    return 0;
}

u8 sub_0809e820(void)
{
    return 0;
}

void sub_0809e824(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)sub_0809e864;
    t->frameTable = gTridentKnightFrames;
    ActorLoadDef((u32)gUnk_08747E0C);
    u = gCurTask;
    u->unk24 = 0;
    CallTableEntry(u->variant, 4, gTridentKnightVariants);
}

void sub_0809e864(void)
{
    ActorDrawWorldInViewOrDestroy();
    sub_0809f2f4();
}

void sub_0809e874(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateCallback = (u32)sub_0809e8cc;
    TaskFaceScreenCenter();
    t = gCurTask;
    t->unk2C = 60;
    t->unk28 = 0;
    t->onGround = 0;
    u = gCurTask;
    u->accelY = 148 << 6;
    u->updateState = 4;
    sub_0809e914();
}

void sub_0809e8b0(void)
{
    CallTableEntry(gCurTask->state, 4, gUnk_08747C04);
}

void sub_0809e8cc(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gUnk_08747C14);
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

void sub_0809e914(void)
{
    if (gCurTask->unk28 == 0)
    {
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        while (1)
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(9);
            TaskYieldTrampoline(3);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(10);
            TaskYieldTrampoline(3);
            TaskSetFrame(8);
            TaskYieldTrampoline(4);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
        }
    }
    else
    {
        TaskSetMotionXFacing(0xFFFF4000, 0x5A5A5A5A);
        while (1)
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(7);
            TaskYieldTrampoline(2);
            TaskSetFrame(8);
            TaskYieldTrampoline(4);
            TaskSetFrame(10);
            TaskYieldTrampoline(3);
            TaskSetFrame(6);
            TaskYieldTrampoline(3);
            TaskSetFrame(5);
            TaskYieldTrampoline(2);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(9);
            TaskYieldTrampoline(3);
        }
    }
}

void sub_0809ea08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gCurTask;
    if (t->unk2C == 61)
    {
        t->unk2C = 60;
        if (t->unk28 != 1)
        {
            t->unk28 = 1;
            ActorSetState(0);
            TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
        }
        return;
    }
    if (sub_0809f994() != 0)
    {
        sub_0809ec84();
        return;
    }
    u = gCurTask;
    n = u->unk2C - 1;
    u->unk2C = n;
    if (n > 59)
        return;
    if (n <= 0)
    {
        sub_0809ed08();
        return;
    }
    if ((n & 7) != 7)
        return;
    if (u->unk28 != 0)
        return;
    if (abs(TaskGetNearestPlayerDx()) <= 63)
    {
        v = gCurTask;
        v->unk28 = 1;
        ActorSetState(0);
        TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809eab8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 1;
    sub_0809f90c();
    u = gCurTask;
    u->unk2C = 120;
    u->onGround = z;
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = 0xFFFD0000;
    v->accelY = 148 << 6;
    TaskSetFrame(29);
    TaskYieldTrampoline(18);
    TaskSetFrame(21);
    TaskSleepForever();
}

void sub_0809eb10(void)
{
}

void sub_0809eb14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 2;
    u = gCurTask;
    u->unk2C = 120;
    TaskStop();
    sub_0809ebc0();
    TaskYieldTrampoline(10);
    ActorSetState(0);
    sub_0809f90c();
    v = gCurTask;
    v->unk28 = 1;
    TaskSleepForever();
}

void sub_0809eb50(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 2)
    {
        t->updateState = 0;
        TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809eb7c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    u = gCurTask;
    u->unk2C = 120;
    TaskStop();
    gCurTask->onGround = z;
    v = gCurTask;
    v->velY = 0xFFFD0000;
    v->accelY = 148 << 6;
    sub_0809ec2c();
    TaskSleepForever();
}

void sub_0809ebbc(void)
{
}

void sub_0809ebc0(void)
{
    sub_0809f90c();
    TaskSetFrame(11);
    TaskYieldTrampoline(8);
    TaskSetFrame(12);
    TaskYieldTrampoline(3);
    TaskSetFrame(13);
    TaskYieldTrampoline(4);
    TaskSetFrame(14);
    TaskYieldTrampoline(14);
    sub_0809f29c(0);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(27);
    TaskYieldTrampoline(1);
    TaskSetFrame(28);
    TaskYieldTrampoline(1);
    TaskSetFrame(16);
}

void sub_0809ec2c(void)
{
    sub_0809f90c();
    TaskSetFrame(11);
    TaskYieldTrampoline(6);
    TaskSetFrame(12);
    TaskYieldTrampoline(3);
    TaskSetFrame(13);
    TaskYieldTrampoline(3);
    TaskSetFrame(14);
    TaskYieldTrampoline(19);
    sub_0809f29c(0);
    TaskSetFrame(15);
    TaskYieldTrampoline(5);
    TaskSetFrame(16);
}

void sub_0809ec80(void)
{
}

void sub_0809ec84(void)
{
    if (gCurTask->unk2C > 59
        || abs(TaskGetNearestPlayerDx()) > 63)
    {
        sub_0809ed74();
    }
    else if (abs(TaskGetNearestPlayerDx()) <= 31)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
    }
    else
    {
        ActorSetState(2);
        TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
    }
}

void sub_0809ed08(void)
{
    gCurTask->unk2C = 60;
    if (RandomRange(2) == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) > 63)
        {
            ActorSetState(3);
            TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
        }
        else
        {
            ActorSetState(2);
            TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
        }
    }
}

void sub_0809ed74(void)
{
    struct Task *t;
    s32 vx;
    s8 f;

    sub_0809f90c();
    t = gCurTask;
    vx = -t->velX;
    t->velX = vx;
    f = t->facing;
    if ((f == 1 && vx > 0) || (f == -1 && vx < 0))
        gCurTask->unk28 = 0;
    else
        gCurTask->unk28 = 1;
    ActorSetState(0);
    gCurTask->updateState = 0;
    TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
}

void sub_0809eddc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 k;
    s32 z;
    s32 n;
    s8 *p;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809ef98;
    p = &t->onGround;
    z = 0;
    *p = 1;
    TaskFaceScreenCenter();
    u = gCurTask;
    u->unk28 = u->facing;
    TaskSetMotionXFacing(128 << 10, k = 0x5A5A5A5A);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(9);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 9, k);
    TaskSetFrame(18);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, k);
    TaskSetFrame(21);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 7, k);
    TaskYieldTrampoline(8);
    TaskStop();
    v = gCurTask;
    v->facing = v->unk28;
    TaskUpdateFlip();
    gCurTask->unk2C = z;
    while (1)
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(10);
        TaskSetFrame(24);
        TaskYieldTrampoline(10);
        TaskSetFrame(25);
        TaskYieldTrampoline(10);
        TaskSetFrame(23);
        TaskYieldTrampoline(10);
        TaskSetFrame(24);
        TaskYieldTrampoline(10);
        TaskSetFrame(25);
        TaskYieldTrampoline(10);
        sub_0809f90c();
        TaskSetFrame(11);
        TaskYieldTrampoline(8);
        TaskSetFrame(12);
        TaskYieldTrampoline(3);
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        TaskSetFrame(14);
        TaskYieldTrampoline(14);
        sub_0809f29c(gUnk_08747C28[gCurTask->unk2C]);
        w = gCurTask;
        n = w->unk2C + 1;
        w->unk2C = n;
        if (n > 6)
            w->unk2C = 0;
        TaskSetFrame(15);
        TaskYieldTrampoline(3);
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        TaskSetFrame(28);
        TaskYieldTrampoline(1);
        TaskSetFrame(16);
        TaskYieldTrampoline(10);
    }
}

void sub_0809ef98(void)
{
    struct Task *t;

    ActorCollideTerrain();
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

void sub_0809efc8(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s8 *p;
    s32 k;
    s32 z;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809f0f0;
    p = &t->onGround;
    z = 0;
    *p = 1;
    TaskFaceScreenCenter();
    gCurTask->unk28 = z;
    TaskSetMotionXFacing(128 << 10, k = 0x5A5A5A5A);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(9);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 9, k);
    TaskSetFrame(18);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, k);
    TaskSetFrame(21);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 7, k);
    TaskYieldTrampoline(8);
    gCurTask->variant = z;
    ActorSetState(0);
    v = gCurTask;
    v->updateCallback = (u32)sub_0809e8cc;
    v->updateState = z;
    w = gCurTask;
    w->unk2C = 60;
    sub_0809e914();
}

void sub_0809f0f0(void)
{
    struct Task *t;

    ActorCollideTerrain();
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

void sub_0809f120(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s8 *p;
    s32 k;
    s32 z;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809f26c;
    p = &t->onGround;
    z = 0;
    *p = 1;
    TaskFaceScreenCenter();
    gCurTask->unk28 = z;
    TaskSetMotionXFacing(128 << 10, k = 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(9);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(9);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(128 << 9, k);
    TaskSetFrame(18);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, k);
    TaskSetFrame(21);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 7, k);
    TaskYieldTrampoline(8);
    gCurTask->variant = z;
    ActorSetState(0);
    v = gCurTask;
    v->updateCallback = (u32)sub_0809e8cc;
    v->updateState = z;
    w = gCurTask;
    w->unk2C = 60;
    sub_0809e914();
}

void sub_0809f26c(void)
{
    struct Task *t;

    ActorCollideTerrain();
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

s32 sub_0809f29c(s32 a)
{
    struct ActorSpawn sp;
    s32 r;

    PlaySfx(214);
    sp.subtype = 28;
    sp.taskType = 131;
    sp.variant = a;
    sp.spawnArg = gCurTask->unk74;
    sp.x = 12;
    sp.y = 0xFFEC;
    sp.tileWord = 0xF310;
    sp.checkTerrain = 0;
    r = CreateActorFromDescAtOffsetFacing(&sp, 1);
    gCurTask->unk46 = r;
}
