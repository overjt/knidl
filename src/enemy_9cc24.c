
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

void AxeKnightWalkInShortInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)AxeKnightWalkInShortUpdate;
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
    t->metaKnightsKnightAxeWalkTimer = 90;
    t->metaKnightsKnightAxeWaitTimer = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    AxeKnightAnimateWalk();
}

void AxeKnightWalkInShortUpdate(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && MetaKnightsKnightIsAtEdge() != 0)
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
}

void AxeKnightWalkInLongInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)AxeKnightWalkInLongUpdate;
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
    t->metaKnightsKnightAxeWalkTimer = 90;
    t->metaKnightsKnightAxeWaitTimer = 0;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->frame--;
    TaskYieldTrampoline(5);
    AxeKnightAnimateWalk();
}

void AxeKnightWalkInLongUpdate(void)
{
    struct Task *t;

    if (ActorCollideTerrain() == 0 && MetaKnightsKnightIsAtEdge() != 0)
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
}

void CreateAxeKnightAxe(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    PlaySfx(186);
    sp.subtype = 26;
    sp.taskType = TASK_AXE_KNIGHT_AXE;
    sp.variant = (t = gCurTask)->variant;
    sp.spawnArg = t->actorSpawnArg;
    sp.x = 20;
    sp.y = 0;
    sp.tileWord = gMetaKnightsKnightTileWords[0];
    sp.checkTerrain = 0;
    gCurTask->metaKnightsKnightWeaponSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
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
    u->updateCallback = (u32)AxeKnightAxeUpdate;
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

void AxeKnightAxeUpdate(void)
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
    if (gTaskSlotTypes[i = t->parent] == TASK_META_KNIGHTS_KNIGHT && gTasks[i].actorSpawnArg == 0
        && TaskIsInRectSlot(&box, i) != 0)
    {
        w = &gTasks[gCurTask->parent];
        w->metaKnightsKnightAxeCaught = 1;
        ActorDestroy();
    }
    else
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

u8 AxeKnightLand(void)
{
    u8 r;

    if (gCurTask->variant != 0)
        r = 0;
    else
    {
        TaskStopY();
        ActorSetState(AXE_KNIGHT_STATE_WALK);
        TaskSetEntry(AxeKnightEnterState, gCurTaskIdx);
        r = 1;
    }
    return r;
}

u8 AxeKnightHitWall(void)
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

u8 AxeKnightHitCeiling(void)
{
    return 0;
}

void sub_0809d13c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)JavelinKnightDraw;
    t->frameTable = gJavelinKnightFrames;
    ActorLoadDef((u32)gJavelinKnightDef);
    u = gCurTask;
    u->metaKnightsKnightFlashTimer = 0;
    CallTableEntry(u->variant, 2, gJavelinKnightVariants);
}

void JavelinKnightDraw(void)
{
    ActorDrawWorldInViewOrDestroy();
    sub_0809d994();
}

void JavelinKnightInit(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = (u32)JavelinKnightUpdate;
    t->metaKnightsKnightJavelinHopsLeft = 3;
    sub_0809f90c();
    JavelinKnightPickHop();
    ActorSetState(1);
    u = gCurTask;
    u->updateState = 0;
    JavelinKnightAnimateHop();
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
        if (MetaKnightsKnightIsAtEdge() != 0)
            JavelinKnightHitWall();
        t = gCurTask;
        p = &t->pixelY;
        a = t->u8C.actor;
        q = (s8 *)a->terrainBox;
        lim = q[2];
        if (*p < lim)
        {
            *p = lim;
            t->posY = *p << 16;
            JavelinKnightHitCeiling();
        }
        CallTableEntry(gCurTask->updateState, 5, gJavelinKnightStateUpdates);
    }
    u = gCurTask;
    if (u->metaKnightsKnightFlashTimer > 0)
    {
        u->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
}

void JavelinKnightStartHop(void)
{
    gCurTask->updateState = 0;
    gCurTask->metaKnightsKnightJavelinHopsLeft = 3;
    ActorSetState(JAVELIN_KNIGHT_STATE_HOP);
    JavelinKnightHop();
}

void JavelinKnightHop(void)
{
    gCurTask->updateState = 0;
    TaskYieldTrampoline(2);
    sub_0809f90c();
    JavelinKnightPickHop();
    JavelinKnightAnimateHop();
}

void JavelinKnightAnimateHop(void)
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

void JavelinKnightState0Update(void)
{
}

void JavelinKnightThrust(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 1;
    TaskStop();
    sub_0809f90c();
    JavelinKnightWindUp();
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
    ActorSetState(JAVELIN_KNIGHT_STATE_START_HOP);
    TaskSleepForever();
}

void JavelinKnightThrustUpdate(void)
{
    struct Task *t;
    u16 v;

    if (gCurTask->state != JAVELIN_KNIGHT_STATE_THRUST)
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
    t = gCurTask;
    v = t->frame;
    if ((u16)(v - 12) <= 3)
        ActorCheckHitsWithBox(gUnk_08747B10[t->frame - 12]);
    else if ((u16)(v - 28) <= 4)
        ActorCheckHitsWithBox(gUnk_08747B24[t->frame - 28]);
}

void JavelinKnightJumpThrow(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 2;
    TaskStop();
    sub_0809f90c();
    JavelinKnightWindUp();
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

void JavelinKnightJumpThrowUpdate(void)
{
}

void JavelinKnightJump(void)
{
    struct Task *t;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    TaskStop();
    sub_0809f90c();
    JavelinKnightWindUp();
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

void JavelinKnightJumpUpdate(void)
{
    struct Task *t;
    u16 v;

    t = gCurTask;
    v = t->frame;
    if ((u16)(v - 17) <= 7)
        ActorCheckHitsWithBox(gUnk_08747B38[t->frame - 17]);
}

void JavelinKnightState5(void)
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
    ActorSetState(JAVELIN_KNIGHT_STATE_START_HOP);
    TaskSleepForever();
}

void sub_0809d6b4(void)
{
    if (gCurTask->state != JAVELIN_KNIGHT_STATE_5)
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
}

void JavelinKnightWindUp(void)
{
    gCurTask->metaKnightsKnightLoopCount = 0;
    do
    {
        TaskSetFrame(25);
        TaskYieldTrampoline(2);
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->metaKnightsKnightLoopCount <= 3);
}

void JavelinKnightChooseNextState(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->metaKnightsKnightJavelinJumpedLast != 1 && (gFrameCount & 1) != 0)
    {
        t->metaKnightsKnightJavelinJumpedLast = 1;
        ActorSetState(JAVELIN_KNIGHT_STATE_JUMP);
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
        return;
    }
    gCurTask->metaKnightsKnightJavelinJumpedLast = 0;
    if (abs(TaskGetNearestPlayerDx()) > 39)
    {
        ActorSetState(JAVELIN_KNIGHT_STATE_JUMP_THROW);
        TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
        return;
    }
    ActorSetState(JAVELIN_KNIGHT_STATE_THRUST);
    TaskSetEntry(JavelinKnightEnterState, gCurTaskIdx);
}

void JavelinKnightBounce(void)
{
    struct Task *u;

    gCurTask->updateCallback = (u32)JavelinKnightBounceUpdate;
    TaskFaceScreenCenter();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    u = gCurTask;
    u->metaKnightsKnightJavelinBouncesLeft = 4;
    u->metaKnightsKnightJavelinFallDir = 1;
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

void JavelinKnightBounceUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->metaKnightsKnightJavelinPushOn = 1;
    ActorCollideTerrain();
    if (MetaKnightsKnightIsAtEdge() != 0)
    {
        JavelinKnightHitWall();
        gCurTask->metaKnightsKnightJavelinPushOn = 0;
    }
    if (gCurTask->pixelY <= 15)
    {
        TaskStopY();
        u = gCurTask;
        u->posY = 128 << 13;
        u->pixelY = 16;
        if (u->metaKnightsKnightJavelinFallDir == 0)
            u->metaKnightsKnightJavelinPushOn = 0;
    }
    v = gCurTask;
    if (v->metaKnightsKnightJavelinPushOn != 0)
    {
        if (v->metaKnightsKnightJavelinFallDir == 0)
            v->velY += 0xFFFFFB00;
        else
            v->velY += 160 << 3;
    }
    gCurTask->onGround = 0;
    w = gCurTask;
    if (w->metaKnightsKnightFlashTimer > 0)
    {
        w->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
    ActorCheckHitsWithBox(gUnk_08747B38[gCurTask->frame - 17]);
}

void JavelinKnightPickHop(void)
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
    sp.taskType = TASK_JAVELIN_KNIGHT_JAVELIN;
    sp.variant = (t = gCurTask)->variant;
    sp.spawnArg = t->actorSpawnArg;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = 0xF110;
    sp.checkTerrain = 0;
    gCurTask->metaKnightsKnightWeaponSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
}
