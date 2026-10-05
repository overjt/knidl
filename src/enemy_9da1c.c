
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells / ROM tables */
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorLoadDef(u32 def);
extern void ActorSetState(u16 v);
extern u32 ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);

void Task_JavelinKnightJavelin(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = gCurTask;
    u->frameTable = gJavelinKnightJavelinFrames;
    u->updateCallback = (u32)JavelinKnightJavelinUpdate;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        gCurTask->velY = 0xFFFC8000;
    else
        gCurTask->velY = 0xFFFF0000;
    gCurTask->accelY = 184 << 5;
    TaskSleepForever();
}

void JavelinKnightJavelinUpdate(void)
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

u8 JavelinKnightLand(void)
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
            if (--t->metaKnightsKnightJavelinHopsLeft > 0)
            {
                n = 1;
            }
            else
            {
                JavelinKnightChooseNextState();
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
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
        return 1;
    case 1:
        TaskStopY();
        u = gCurTask;
        if (u->metaKnightsKnightJavelinFallDir == 1)
            u->metaKnightsKnightJavelinPushOn = 0;
        return 0;
    }
}

u8 JavelinKnightHitWall(void)
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
            if (--t->metaKnightsKnightJavelinBouncesLeft > 0)
                break;
            t->metaKnightsKnightJavelinBouncesLeft = 4;
            t->metaKnightsKnightJavelinFallDir = (t->metaKnightsKnightJavelinFallDir + 1) & 1;
        }
        gCurTask->metaKnightsKnightJavelinPushOn = 0;
        return 0;
    }
}

u8 JavelinKnightHitCeiling(void)
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
        if (u->metaKnightsKnightJavelinFallDir == 0)
            u->metaKnightsKnightJavelinPushOn = 0;
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
    ActorLoadDef((u32)gMaceKnightDef);
    u = gCurTask;
    u->metaKnightsKnightFlashTimer = 0;
    CallTableEntry(u->variant, 3, gMaceKnightVariants);
}

void MaceKnightStand(void)
{
    gCurTask->updateCallback = (u32)MaceKnightStandUpdate;
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
        MaceKnightThrowMace();
    }
}

void MaceKnightStandUpdate(void)
{
    struct Task *t;
    s32 i;
    struct Task *w;
    s8 v;
    s16 *p;

    ActorCollideTerrain();
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
    v = gCurTask->hitKind;
    if (v == HIT_KIND_DEFEAT || v == HIT_KIND_INHALE || v == HIT_KIND_GRAB)
    {
        p = &gCurTask->metaKnightsKnightWeaponSlot;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void MaceKnightVariant1(void)
{
    gCurTask->updateCallback = (u32)MaceKnightUpdate;
    TaskFaceScreenCenter();
    ActorSetState(0);
    MaceKnightWalk();
}

void MaceKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMaceKnightStates);
}

void MaceKnightUpdate(void)
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
        CallTableEntry(gCurTask->updateState, 2, gMaceKnightStateUpdates);
    }
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
    v = gCurTask->hitKind;
    if (v == HIT_KIND_DEFEAT || v == HIT_KIND_INHALE || v == HIT_KIND_GRAB)
    {
        p = &gCurTask->metaKnightsKnightWeaponSlot;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void MaceKnightWalk(void)
{
    gCurTask->updateState = MACE_KNIGHT_STATE_WALK;
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
    ActorSetState(MACE_KNIGHT_STATE_THROW);
    TaskSleepForever();
}

void MaceKnightWalkUpdate(void)
{
    if (gCurTask->state != MACE_KNIGHT_STATE_WALK)
        TaskSetEntry(MaceKnightEnterState, gCurTaskIdx);
}

void MaceKnightThrow(void)
{
    gCurTask->updateState = MACE_KNIGHT_STATE_THROW;
    TaskStop();
    MaceKnightThrowMace();
    ActorSetState(MACE_KNIGHT_STATE_WALK);
    TaskSleepForever();
}

void MaceKnightThrowUpdate(void)
{
    if (gCurTask->state != MACE_KNIGHT_STATE_THROW)
        TaskSetEntry(MaceKnightEnterState, gCurTaskIdx);
}

void MaceKnightVariant2(void)
{
    gCurTask->updateCallback = (u32)sub_0809dfc8;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(176 << 9, 0x5A5A5A5A);
    CreateMaceKnightMace();
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
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
    v = gCurTask->hitKind;
    if (v == 1 || v == 3 || v == 4)
    {
        p = &gCurTask->metaKnightsKnightWeaponSlot;
        if (*p != -1)
        {
            w = &gTasks[*p];
            w->unk2C = 1;
        }
    }
}

void MaceKnightThrowMace(void)
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
    CreateMaceKnightMace();
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

s32 CreateMaceKnightMace(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s16 *p;
    s32 z;
    s32 r;

    sp.subtype = 27;
    sp.taskType = TASK_MACE_KNIGHT_MACE;
    sp.variant = (t = gCurTask)->variant;
    z = 0;
    sp.spawnArg = t->unk74;
    sp.x = z;
    sp.y = 0xFFF0;
    sp.tileWord = gUnk_08745CEC[2];
    sp.checkTerrain = 0;
    r = CreateActorFromDesc(&sp, 1);
    p = &gCurTask->metaKnightsKnightWeaponSlot;
    *p = r;
    ((struct Task *)(*p * 144 + (s32)gTasks))->maceKnightMaceDismissed = z;
}

void Task_MaceKnightMace(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)MaceKnightMaceMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gMaceKnightMaceFrames;
    TaskFaceLikeParent();
    u = gCurTask;
    CallTableEntry(u->variant, 3, gMaceKnightMaceVariants);
}

void MaceKnightMaceMove(void)
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
    gCurTask->maceKnightMaceLoopCount = 0;
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
    } while ((s16)++gCurTask->maceKnightMaceLoopCount <= 1);
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
    gTasks[gCurTask->parent].metaKnightsKnightWeaponSlot = 0xFFFF;
    ActorDestroy();
}

void sub_0809e630(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->maceKnightMaceDismissed != 0)
    {
        gTasks[t->parent].metaKnightsKnightWeaponSlot = 0xFFFF;
        ActorDestroy();
    }
    else
    {
        ActorCheckHits();
    }
}

void MaceKnightMaceVariant2(void)
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
    if (t->maceKnightMaceDismissed != 0)
    {
        gTasks[t->parent].metaKnightsKnightWeaponSlot = 0xFFFF;
        ActorDestroy();
    }
    else
    {
        t->posY = 0xFFF00000;
        ActorCheckHits();
    }
}

u8 MaceKnightLand(void)
{
    TaskStopY();
    return 0;
}

u8 MaceKnightStartFall(void)
{
    gCurTask->accelY = 148 << 6;
    return 0;
}

u8 MaceKnightHitWall(void)
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

u8 MaceKnightHitCeiling(void)
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
    ActorLoadDef((u32)gTridentKnightDef);
    u = gCurTask;
    u->metaKnightsKnightFlashTimer = 0;
    CallTableEntry(u->variant, 4, gTridentKnightVariants);
}

void sub_0809e864(void)
{
    ActorDrawWorldInViewOrDestroy();
    sub_0809f2f4();
}

void TridentKnightVariant0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateCallback = (u32)TridentKnightUpdate;
    TaskFaceScreenCenter();
    t = gCurTask;
    t->metaKnightsKnightTridentTimer = 60;
    t->metaKnightsKnightTridentWalkBack = 0;
    t->onGround = 0;
    u = gCurTask;
    u->accelY = 148 << 6;
    u->updateState = 4;
    TridentKnightWalk();
}

void TridentKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gTridentKnightStates);
}

void TridentKnightUpdate(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 5, gTridentKnightStateUpdates);
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void TridentKnightWalk(void)
{
    if (gCurTask->metaKnightsKnightTridentWalkBack == 0)
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
    if (t->metaKnightsKnightTridentTimer == 61)
    {
        t->metaKnightsKnightTridentTimer = 60;
        if (t->metaKnightsKnightTridentWalkBack != 1)
        {
            t->metaKnightsKnightTridentWalkBack = 1;
            ActorSetState(TRIDENT_KNIGHT_STATE_WALK);
            TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
        }
        return;
    }
    if (sub_0809f994() != 0)
    {
        sub_0809ec84();
        return;
    }
    u = gCurTask;
    n = u->metaKnightsKnightTridentTimer - 1;
    u->metaKnightsKnightTridentTimer = n;
    if (n > 59)
        return;
    if (n <= 0)
    {
        sub_0809ed08();
        return;
    }
    if ((n & 7) != 7)
        return;
    if (u->metaKnightsKnightTridentWalkBack != 0)
        return;
    if (abs(TaskGetNearestPlayerDx()) <= 63)
    {
        v = gCurTask;
        v->metaKnightsKnightTridentWalkBack = 1;
        ActorSetState(TRIDENT_KNIGHT_STATE_WALK);
        TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
    }
}

void TridentKnightJump(void)
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
    u->metaKnightsKnightTridentTimer = 120;
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

void TridentKnightJumpUpdate(void)
{
}

void TridentKnightThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 2;
    u = gCurTask;
    u->metaKnightsKnightTridentTimer = 120;
    TaskStop();
    sub_0809ebc0();
    TaskYieldTrampoline(10);
    ActorSetState(TRIDENT_KNIGHT_STATE_WALK);
    sub_0809f90c();
    v = gCurTask;
    v->metaKnightsKnightTridentWalkBack = 1;
    TaskSleepForever();
}

void TridentKnightThrowUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != TRIDENT_KNIGHT_STATE_THROW)
    {
        t->updateState = 0;
        TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
    }
}

void TridentKnightJumpThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    u = gCurTask;
    u->metaKnightsKnightTridentTimer = 120;
    TaskStop();
    gCurTask->onGround = z;
    v = gCurTask;
    v->velY = 0xFFFD0000;
    v->accelY = 148 << 6;
    sub_0809ec2c();
    TaskSleepForever();
}

void TridentKnightJumpThrowUpdate(void)
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
    CreateTridentKnightTrident(0);
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
    CreateTridentKnightTrident(0);
    TaskSetFrame(15);
    TaskYieldTrampoline(5);
    TaskSetFrame(16);
}

void sub_0809ec80(void)
{
}

void sub_0809ec84(void)
{
    if (gCurTask->metaKnightsKnightTridentTimer > 59
        || abs(TaskGetNearestPlayerDx()) > 63)
    {
        sub_0809ed74();
    }
    else if (abs(TaskGetNearestPlayerDx()) <= 31)
    {
        ActorSetState(TRIDENT_KNIGHT_STATE_JUMP);
        TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
    }
    else
    {
        ActorSetState(TRIDENT_KNIGHT_STATE_THROW);
        TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
    }
}

void sub_0809ed08(void)
{
    gCurTask->metaKnightsKnightTridentTimer = 60;
    if (RandomRange(2) == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) > 63)
        {
            ActorSetState(TRIDENT_KNIGHT_STATE_JUMP_THROW);
            TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
        }
        else
        {
            ActorSetState(TRIDENT_KNIGHT_STATE_THROW);
            TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
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
        gCurTask->metaKnightsKnightTridentWalkBack = 0;
    else
        gCurTask->metaKnightsKnightTridentWalkBack = 1;
    ActorSetState(TRIDENT_KNIGHT_STATE_WALK);
    gCurTask->updateState = 0;
    TaskSetEntry(TridentKnightEnterState, gCurTaskIdx);
}

void TridentKnightVariant1(void)
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
    u->metaKnightsKnightTridentSavedFacing = u->facing;
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
    v->facing = v->metaKnightsKnightTridentSavedFacing;
    TaskUpdateFlip();
    gCurTask->metaKnightsKnightTridentThrowIndex = z;
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
        CreateTridentKnightTrident(gUnk_08747C28[gCurTask->metaKnightsKnightTridentThrowIndex]);
        w = gCurTask;
        n = w->metaKnightsKnightTridentThrowIndex + 1;
        w->metaKnightsKnightTridentThrowIndex = n;
        if (n > 6)
            w->metaKnightsKnightTridentThrowIndex = 0;
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
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void TridentKnightVariant2(void)
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
    gCurTask->metaKnightsKnightTridentWalkBack = z;
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
    v->updateCallback = (u32)TridentKnightUpdate;
    v->updateState = z;
    w = gCurTask;
    w->metaKnightsKnightTridentTimer = 60;
    TridentKnightWalk();
}

void sub_0809f0f0(void)
{
    struct Task *t;

    ActorCollideTerrain();
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void TridentKnightVariant3(void)
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
    gCurTask->metaKnightsKnightTridentWalkBack = z;
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
    v->updateCallback = (u32)TridentKnightUpdate;
    v->updateState = z;
    w = gCurTask;
    w->metaKnightsKnightTridentTimer = 60;
    TridentKnightWalk();
}

void sub_0809f26c(void)
{
    struct Task *t;

    ActorCollideTerrain();
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

s32 CreateTridentKnightTrident(s32 a)
{
    struct ActorSpawn sp;
    s32 r;

    PlaySfx(214);
    sp.subtype = 28;
    sp.taskType = TASK_TRIDENT_KNIGHT_TRIDENT;
    sp.variant = a;
    sp.spawnArg = gCurTask->actorSpawnArg;
    sp.x = 12;
    sp.y = 0xFFEC;
    sp.tileWord = 0xF310;
    sp.checkTerrain = 0;
    r = CreateActorFromDescAtOffsetFacing(&sp, 1);
    gCurTask->metaKnightsKnightWeaponSlot = r;
}
