#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* External functions */
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlaySfx(s32 id);
extern u32 TaskIsOnScreen(void);
extern void TaskSetEntry(void *a, u32 i);
extern void HudStartHpBar();
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void RequestScreenShake(u32 a);
extern void TaskBreakBlocksNoPlayer();
extern void TaskBreakTopBlockRow();
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAux(struct ActorAux *v);
extern void ActorSetExtraAttackBox(u32 v);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern s16 ActorComputeHealth(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorCollideTerrainAlongVelocity(void);
extern u32 ActorCollideTerrainFloor(void);
extern u32 ActorReactToHit(void);

/* Module functions */
void sub_080a2b2c();
void ReleaseRoomObject();
s32 LoadRoomEnemyGfx();
s32 LoadRoomMidBossGfx();
s32 LoadRoomBossGfx();
void LoadRoomMetaKnightsGfx();
s32 sub_080b5a94();
s32 SpawnRoomEnemy();
s32 sub_080b5d84();

void MetaKnightInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)MetaKnightUpdate;
    t->metaKnightGuardFlags = 0;
    t->metaKnightTargetPlayerSlot = -1;
    t->metaKnightAttackBoxIndex = -1;
    t->metaKnightFollowFlags = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = (s16)ActorComputeHealth();
    ActorSetState(META_KNIGHT_STATE_INTRO);
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightUpdate(void)
{
    struct Task *t;
    u32 rr;
    u32 *q;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 24, gMetaKnightStateUpdates);
    if (gCurTask->metaKnightAttackBoxIndex != -1)
    {
        ActorSetAttackBox(gUnk_08748D38[gCurTask->metaKnightAttackBoxIndex]);
        gUnk_02007D00[1] = gCurTask->health;
        ActorCheckHitsWithExtraBox();
        switch (gCurTask->hitKind)
        {
        case HIT_KIND_DEFEAT:
        case HIT_KIND_DAMAGE:
            if ((u8)MetaKnightTryGuard() == 1)
            {
                gCurTask->hitKind = HIT_KIND_NONE;
                t = gCurTask;
                t->health = gUnk_02007D00[1];
                t->metaKnightAttackBoxIndex = 2;
                ActorSetState(META_KNIGHT_STATE_22);
                sub_080a7168();
            }
            break;
        case HIT_KIND_NO_DAMAGE:
            gUnk_02007D00[0] = gCurTask->hitterPlayer;
            switch (gCurTask->hitEffect)
            {
            case 4:
                gUnk_03001F2C = MetaKnightGetHealthQuarter();
                q = &gUnk_03002448;
                rr = RandomRange(16);
                *q = rr;
                if ((s32)rr < gUnk_08748D6C[gUnk_03001F2C])
                {
                    gCurTask->metaKnightAttackBoxIndex = 2;
                    ActorSetState(META_KNIGHT_STATE_22);
                    sub_080a7168();
                }
                else if ((s32)rr < gUnk_08748D70[gUnk_03001F2C])
                {
                    gCurTask->metaKnightAttackBoxIndex = 2;
                    ActorSetState(META_KNIGHT_STATE_16);
                    sub_080a7168();
                }
                break;
            case 2:
            case 3:
                gCurTask->metaKnightAttackBoxIndex = 2;
                ActorSetState(META_KNIGHT_STATE_23);
                sub_080a7168();
                break;
            }
            break;
        }
        ActorReactToHit();
    }
}

void MetaKnightIntro(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    gCurTask->updateState = META_KNIGHT_STATE_INTRO;
    gCurTask->posX = (gViewRect[0] + 216) << 16;
    gCurTask->posY = (gViewRect[2] + 44) << 16;
    if (gMetaKnightmareMode == 0)
    {
        for (i = 0; i < gActivePlayerCount; i++)
        {
            sp.subtype = 18;
            sp.taskType = TASK_META_KNIGHT_SWORD;
            sp.variant = 0;
            sp.spawnArg = i;
            sp.x = gUnk_08748D28[i + (gActivePlayerCount - 1) * 4] + gViewRect[0];
            sp.y = gViewRect[2];
            sp.tileWord = gCurTask->u8C.actor->savedTileWord;
            sp.checkTerrain = 0;
            CreateActorFromDesc(&sp, 1);
        }
    }
    else
        gUnk_02007D00[2] = gPlayerCount;
    gCurTask->facing = 1;
    TaskSetFrame(4);
    while (gUnk_02007D00[2] != gActivePlayerCount)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(1);
    gCurTask->metaKnightAttackBoxIndex = 2;
    sub_08066544();
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(TASK_META_KNIGHT_CAPE, (s16)(t->pixelX + 8), t->pixelY, t->u8C.actor->savedTileWord);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(18);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(TASK_META_KNIGHT_SPARKLE, (s16)(t->pixelX - 32), t->pixelY, t->u8C.actor->savedTileWord | (240 << 8));
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(20);
    gCurTask->facing = 255;
    TaskSetFrame(60);
    TaskYieldTrampoline(10);
    CreateChildTaskHere(TASK_META_KNIGHT_SWORD_HIT_BOX, 0);
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(META_KNIGHT_STATE_9);
    TaskSleepForever();
}

void MetaKnightIntroUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_INTRO)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightFollow(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = META_KNIGHT_STATE_FOLLOW;
    t = gCurTask;
    t->metaKnightGuardFlags &= ~128;
    t->metaKnightAttackBoxIndex = 0;
    v = TaskFindNearestPlayer();
    gUnk_02007D00[5] = v;
    if (gUnk_0300244C != 0)
    {
        if (gCurTask->metaKnightTargetPlayerSlot != v)
        {
            gCurTask->metaKnightWalkAnimFlags = 0;
            gCurTask->metaKnightFollowPhase = 0;
            gCurTask->metaKnightTargetPlayerSlot = gUnk_02007D00[5];
        }
    }
    MetaKnightAnimateWalk();
}

void MetaKnightFollowUpdate(void)
{
    struct Task *pt;
    s16 *px;
    vu16 *pv;
    vu16 *f9;
    s32 d;

    gCurTask->metaKnightFollowTimer--;
    if (gCurTask->metaKnightFollowTimer == 0)
    {
        MetaKnightPickNextState();
        return;
    }
    pt = &gTasks[gUnk_02007D00[5]];
    px = &pt->pixelX;
    d = (*px + gCurTask->metaKnightFollowOffsetX) - gCurTask->pixelX;
    switch (gCurTask->metaKnightFollowPhase)
    {
    case 0:
        if (gCurTask->metaKnightFollowFlags & 2)
        {
            if (d < 0)
                gCurTask->facing = 255;
            else
                gCurTask->facing = 1;
        }
        else
            TaskFaceNearestPlayer();
        if (abs(d) <= 1)
        {
            gCurTask->metaKnightFollowPhase = 1;
            gCurTask->velX = 0;
            gUnk_02007D00[7] = 0;
        }
        else if (abs(d) <= 27)
        {
            gCurTask->metaKnightWalkAnimFlags &= ~2;
            if (d > 0)
                gCurTask->velX = 128 << 9;
            else
                gCurTask->velX = -0x10000;
        }
        else
        {
            gCurTask->metaKnightWalkAnimFlags |= 2;
            if (d > 0)
                gCurTask->velX = 128 << 10;
            else
                gCurTask->velX = -0x20000;
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 1:
        gCurTask->metaKnightFollowFlags &= ~6;
        if ((u8)sub_080a6f38(gCurTask->pixelX, gCurTask->pixelY) == 1)
        {
            gCurTask->metaKnightFollowPhase = 2;
            gCurTask->velX = 0;
        }
        else
        {
            gCurTask->pixelX = *px + gCurTask->metaKnightFollowOffsetX;
            gCurTask->posX = gCurTask->pixelX << 16;
            TaskFaceNearestPlayer();
            f9 = gPlayerHeldKeys;
            pv = &f9[gUnk_02007D00[5]];
            if ((*pv & 48) && *px != gUnk_02007D00[6])
            {
                if ((u16)(*pv & 16) != 0)
                {
                    if (gUnk_02007D00[7] < 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]++;
                }
                else
                {
                    if (gUnk_02007D00[7] > 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]--;
                }
                if (abs(gUnk_02007D00[7]) == 15)
                    gCurTask->metaKnightFollowPhase = 0;
            }
            else if (pt->velX == 0)
            {
                gCurTask->metaKnightFollowPhase = 2;
                gCurTask->velX = 0;
            }
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 2:
        TaskFaceNearestPlayer();
        if (*px != gUnk_02007D00[6])
        {
            if ((u8)sub_080a6f38((s16)(*px + gCurTask->metaKnightFollowOffsetX), gCurTask->pixelY) == 0)
            {
                gCurTask->metaKnightFollowPhase = 0;
                gCurTask->metaKnightWalkAnimFlags &= ~1;
                break;
            }
        }
        if (gCurTask->frame == 65)
            gCurTask->metaKnightWalkAnimFlags |= 1;
        break;
    }
    if ((u8)MetaKnightClampToRoom() == 1)
    {
        gCurTask->velX = 0;
        gCurTask->metaKnightFollowPhase = 2;
        if (!(gCurTask->metaKnightFollowFlags & 1))
        {
            gCurTask->metaKnightFollowFlags |= 1;
            gCurTask->metaKnightFollowTimer = RandomRange(30) + 30;
        }
    }
    TaskUpdateFlip();
    if (!(gCurTask->metaKnightFollowFlags & 6))
    {
        if (abs(TaskGetNearestPlayerDx()) <= 31)
        {
            ActorSetState(gUnk_08748DA8[pt->onGround][RandomRange(8)]);
            sub_080a7168();
        }
    }
}

void MetaKnightState2(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_2;
    if (!(gCurTask->metaKnightFollowFlags & 2))
    {
        if (gCurTask->facing == 1)
            gCurTask->metaKnightFollowOffsetX = 64;
        else
            gCurTask->metaKnightFollowOffsetX = -64;
    }
    MetaKnightSetFollowTimer();
    gCurTask->metaKnightWalkAnimFlags = 0;
    gCurTask->metaKnightFollowPhase = 0;
    gCurTask->metaKnightFollowFlags &= 2;
    TaskSetFrame(61);
    ActorSetState(META_KNIGHT_STATE_FOLLOW);
    TaskSleepForever();
}

void MetaKnightState2Update(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != META_KNIGHT_STATE_2)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightState3(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_3;
    gCurTask->metaKnightFollowOffsetX = -gCurTask->metaKnightFollowOffsetX;
    MetaKnightSetFollowTimer();
    gCurTask->metaKnightWalkAnimFlags = 0;
    gCurTask->metaKnightFollowPhase = 0;
    gCurTask->metaKnightFollowFlags = 2;
    TaskSetFrame(61);
    ActorSetState(META_KNIGHT_STATE_FOLLOW);
    TaskSleepForever();
}

void MetaKnightState3Update(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != META_KNIGHT_STATE_3)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightState4(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_4;
    gCurTask->metaKnightFollowTimer = 48;
    ActorSetState(META_KNIGHT_STATE_FOLLOW);
    TaskSleepForever();
}

void MetaKnightState4Update(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != META_KNIGHT_STATE_4)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightState5(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_5;
    gCurTask->metaKnightFollowFlags = 4;
    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->facing == 1)
        t->metaKnightFollowOffsetX = -64;
    else
        t->metaKnightFollowOffsetX = 64;
    MetaKnightSetFollowTimer();
    gCurTask->metaKnightWalkAnimFlags = 0;
    gCurTask->metaKnightFollowPhase = 0;
    TaskSetFrame(61);
    ActorSetState(META_KNIGHT_STATE_FOLLOW);
    TaskSleepForever();
}

void MetaKnightState5Update(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != META_KNIGHT_STATE_5)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightApproach(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_APPROACH;
    if (gCurTask->metaKnightFollowOffsetX < 0)
        gUnk_02007D00[6] = 1;
    else
        gUnk_02007D00[6] = 0;
    {
        struct Task *t2 = &gTasks[TaskFindNearestPlayer()];

        gUnk_02007D00[7] = t2->pixelX;
    }
    gCurTask->metaKnightWalkAnimFlags = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskSetFrame(61);
    MetaKnightAnimateWalk();
}

void MetaKnightApproachUpdate(void)
{
    s32 d;
    s32 x;

    ClampTaskToRoom(gCurTask);
    x = gCurTask->pixelX;
    d = x - gUnk_02007D00[7];
    if (d >= 0 ? d <= 5 : gUnk_02007D00[7] - x <= 5)
    {
        if (abs(TaskGetNearestPlayerDy()) <= 47)
            ActorSetState(gUnk_08748E48[RandomRange(16)]);
        else
            ActorSetState(gUnk_08748E68[RandomRange(16)]);
        if (gCurTask->state == META_KNIGHT_STATE_3)
            gUnk_02007D00[6]++;
        gCurTask->metaKnightFollowOffsetX = gUnk_08748D60[gUnk_02007D00[6]];
        gCurTask->metaKnightFollowFlags = 2;
        sub_080a7168();
    }
}

void MetaKnightRun(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_RUN;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(224 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(82);
        TaskYieldTrampoline(2);
        gCurTask->metaKnightLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->metaKnightLoopCount++;
        } while ((s16)gCurTask->metaKnightLoopCount <= 4);
    }
}

void MetaKnightRunUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    if (abs(TaskGetNearestPlayerDx()) <= 43)
    {
        gCurTask->metaKnightFollowFlags = 0;
        if (gCurTask->health >= gUnk_02007D00[3] >> 1)
            ActorSetState(gMetaKnightNearStates[RandomRange(8)]);
        else
            ActorSetState(gMetaKnightNearStatesLowHealth[RandomRange(8)]);
        sub_080a7168();
    }
}

void MetaKnightJumpLow(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_9;
    gCurTask->metaKnightAttackBoxIndex = 0;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x26000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->metaKnightLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->metaKnightLoopCount++;
    } while ((s16)gCurTask->metaKnightLoopCount <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(META_KNIGHT_STATE_LAND);
    TaskSleepForever();
}

void MetaKnightJumpLowUpdate(void)
{
    MetaKnightPickAirAttack();
    if (gCurTask->state != META_KNIGHT_STATE_9)
        sub_080a7168();
}

void MetaKnightJumpHigh(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_8;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x33000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->metaKnightLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->metaKnightLoopCount++;
    } while ((s16)gCurTask->metaKnightLoopCount <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(META_KNIGHT_STATE_LAND);
    TaskSleepForever();
}

void MetaKnightJumpHighUpdate(void)
{
    MetaKnightPickAirAttack();
    if (gCurTask->state != META_KNIGHT_STATE_8)
        sub_080a7168();
}

void MetaKnightLand(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_LAND;
    TaskStop();
    gCurTask->metaKnightAttackBoxIndex = 0;
    TaskSetFrame(81);
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(META_KNIGHT_STATE_2);
    TaskSleepForever();
}

void MetaKnightLandUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_LAND)
        sub_080a7168();
}

void MetaKnightStartJumpForward(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_11;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(META_KNIGHT_STATE_8);
    TaskSleepForever();
}

void MetaKnightStartJumpForwardUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_11)
        sub_080a7168();
}

void MetaKnightStartJumpUp(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_12;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 0;
    ActorSetState(META_KNIGHT_STATE_8);
    TaskSleepForever();
}

void MetaKnightStartJumpUpUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_12)
        sub_080a7168();
}

void MetaKnightSwordSpin(void)
{
    struct Task *t;
    s32 i;

    gCurTask->updateState = META_KNIGHT_STATE_SWORD_SPIN;
    t = gCurTask;
    t->accelY = 144 << 7;
    t->speedLimitY = 128 << 11;
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023D4 <= 43)
        gUnk_02007D00[7] = 4;
    else
        gUnk_02007D00[7] = 2;
    gCurTask->metaKnightLoopCount = 0;
    while ((s16)gCurTask->metaKnightLoopCount < gUnk_02007D00[7])
    {
        PlaySfx(0x225);
        TaskSetFrame(114);
        TaskYieldTrampoline(1);
        gCurTask->metaKnightSpinFrameCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->metaKnightSpinFrameCount++;
        } while ((s16)gCurTask->metaKnightSpinFrameCount <= 6);
        gCurTask->metaKnightLoopCount++;
    }
    TaskSetFrame(122);
    TaskYieldTrampoline(2);
    TaskSetFrame(80);
    TaskSleepForever();
}

void MetaKnightSwordSpinUpdate(void)
{
    if ((u8)MetaKnightClampToRoom() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(META_KNIGHT_STATE_LAND);
        sub_080a7168();
    }
}

void MetaKnightStartJumpDownThrust(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_16;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 15;
    ActorSetState(META_KNIGHT_STATE_8);
    TaskSleepForever();
}

void MetaKnightStartJumpDownThrustUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_16)
        sub_080a7168();
}

void MetaKnightDownThrust(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_DOWN_THRUST;
    TaskStop();
    TaskSetFrame(111);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskSetMotionY(128 << 10, 144 << 7, 128 << 11);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(2);
    PlaySfx(504);
    CreateLandingImpact(0, 0, 3);
    gCurTask->metaKnightAttackBoxIndex = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(META_KNIGHT_STATE_2);
    TaskSleepForever();
}

void MetaKnightDownThrustUpdate(void)
{
    if ((u8)MetaKnightClampToRoom() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->state != META_KNIGHT_STATE_DOWN_THRUST)
        sub_080a7168();
}

void MetaKnightStartJumpUpwardSlash(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_14;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 17;
    ActorSetState(META_KNIGHT_STATE_8);
    TaskSleepForever();
}

void MetaKnightStartJumpUpwardSlashUpdate(void)
{
    if (gCurTask->state != META_KNIGHT_STATE_14)
        sub_080a7168();
}

void MetaKnightUpwardSlashInAir(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_17;
    MetaKnightUpwardSlash();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(80);
    TaskSleepForever();
}

void MetaKnightUpwardSlashInAirUpdate(void)
{
    if ((u8)MetaKnightClampToRoom() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        StopSfxOnPlayer(gUnk_02007D00[4], 137 << 2);
        TaskStop();
        ActorSetState(META_KNIGHT_STATE_LAND);
        sub_080a7168();
    }
}

void MetaKnightUpwardSlashOnGround(void)
{
    gCurTask->updateState = META_KNIGHT_STATE_18;
    TaskStop();
    MetaKnightUpwardSlash();
    ActorSetState(META_KNIGHT_STATE_2);
    TaskSleepForever();
}

void MetaKnightUpwardSlashOnGroundUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != META_KNIGHT_STATE_18)
        sub_080a7168();
}

void MetaKnightSlashShort(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_19;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(12);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(TASK_BACKWARD_DUST_PUFF, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->metaKnightLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->metaKnightLoopCount++;
    } while ((s16)gCurTask->metaKnightLoopCount <= 7);
    gCurTask->metaKnightAttackBoxIndex = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    TaskSetFrame(60);
    TaskYieldTrampoline(28);
    ActorSetState(META_KNIGHT_STATE_5);
    TaskSleepForever();
}

void MetaKnightSlashShortUpdate(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != META_KNIGHT_STATE_19)
        sub_080a7168();
}

void MetaKnightDoubleSlash(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_DOUBLE_SLASH;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(TASK_BACKWARD_DUST_PUFF, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->metaKnightLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->metaKnightLoopCount++;
    } while ((s16)gCurTask->metaKnightLoopCount <= 7);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskSetFrame(97);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(TASK_BACKWARD_DUST_PUFF, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->metaKnightLoopCount = 0;
    do
    {
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->metaKnightLoopCount++;
    } while ((s16)gCurTask->metaKnightLoopCount <= 7);
    TaskSetFrame(88);
    TaskYieldTrampoline(10);
    ActorSetState(META_KNIGHT_STATE_21);
    TaskSleepForever();
}

void MetaKnightDoubleSlashUpdate(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != META_KNIGHT_STATE_DOUBLE_SLASH)
        sub_080a7168();
}

void MetaKnightSlashLong(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_21;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(105);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(TASK_BACKWARD_DUST_PUFF, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(20);
    gCurTask->metaKnightAttackBoxIndex = 0;
    TaskSetFrame(60);
    TaskYieldTrampoline(48);
    ActorSetState(META_KNIGHT_STATE_5);
    TaskSleepForever();
}

void MetaKnightSlashLongUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 25)
    {
        gUnk_02007D00[7]++;
        for (gCurTask->metaKnightLoopCount = 0; (s16)gCurTask->metaKnightLoopCount <= 2
             && gUnk_02007D00[7] >= gUnk_08748D4C[(s16)gCurTask->metaKnightLoopCount]; gCurTask->metaKnightLoopCount++)
            ;
        TaskSetMotionXFacing(gUnk_08748D50[(s16)gCurTask->metaKnightLoopCount], 0x5A5A5A5A);
    }
    if (gCurTask->state != META_KNIGHT_STATE_21)
        sub_080a7168();
}

void MetaKnightState22(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_22;
    gUnk_02007D00[6] = gCurTask->onGround;
    gUnk_02007D00[7] = 20;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    t = gCurTask;
    if (t->onGround == 0)
    {
        CreateChildTask(TASK_META_KNIGHT_SPARKLE, (s16)(t->pixelX + t->facing * 4), (s16)(t->pixelY - 4),
                     t->u8C.actor->savedTileWord | (240 << 8));
        gCurTask->accelY = 168 << 5;
        gCurTask->speedLimitY = 192 << 10;
        gCurTask->metaKnightLoopCount = 0;
        do
        {
            TaskSetFrame(70);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->metaKnightLoopCount++;
        } while ((s16)gCurTask->metaKnightLoopCount <= 3);
    }
    else
    {
        CreateChildTask(TASK_META_KNIGHT_SPARKLE, (s16)(t->pixelX + t->facing * 16), (s16)(t->pixelY + 4),
                     t->u8C.actor->savedTileWord | (240 << 8));
        TaskStop();
        TaskSetFrame(69);
    }
    TaskSleepForever();
}

void MetaKnightState22Update(void)
{
    s32 *pD;
    u8 *b3v;
    s32 *pE;
    s32 w;
    s32 v;

    ClampTaskToRoom(gCurTask);
    pD = gUnk_02007D00;
    v = pD[7];
    if (v == 0)
    {
        gCurTask->metaKnightAttackBoxIndex = v;
        if (gCurTask->onGround != 0)
        {
            if (pD[6] == 0)
            {
                TaskStop();
                ActorSetState(META_KNIGHT_STATE_LAND);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
            else
            {
                TaskStop();
                gCurTask->metaKnightFollowPhase = v;
                MetaKnightPickNextState();
            }
        }
    }
    else
    {
        w = v - 1;
        pD[7] = w;
        pD[5] = 0;
        b3v = (u8 *)gUnk_08748D78;
        pE = pD;
        while (w >= *(u8 *)(pD[5] + (u32)b3v))
            pD[5]++;
        TaskSetMotionXFacing(gUnk_08748D80[pE[5]], 0x5A5A5A5A);
        if (gCurTask->onGround != 0)
        {
            TaskStopY();
            pE[6] = gCurTask->onGround;
            TaskSetFrame(69);
        }
    }
}

void MetaKnightState23(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_STATE_23;
    TaskStop();
    gUnk_02007D00[6] = gCurTask->onGround;
    t = gCurTask;
    if (t->onGround == 0)
    {
        t->accelY = 168 << 5;
        t->speedLimitY = 192 << 10;
    }
    gUnk_02007D00[7] = 60;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    TaskSetFrame(69);
    TaskSleepForever();
}

void MetaKnightState23Update(void)
{
    u16 v;

    ClampTaskToRoom(gCurTask);
    if (gPlayerStates[gUnk_02007D00[0]].unk40 & 4) {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                gUnk_02007D00[6] = gCurTask->onGround;
            }
            if (--gUnk_02007D00[7] == 0) {
                gCurTask->metaKnightAttackBoxIndex = 1;
                v = gUnk_08748EA8[RandomRange(2)];
                ActorSetState(v);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
        }
    } else {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                v = 10;
            } else {
                v = 2;
            }
            ActorSetState(v);
            TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
        } else {
            TaskSetFrame(70);
        }
    }
}

s32 MetaKnightGetHealthQuarter(void)
{
    s32 x = gCurTask->health;
    s32 q = gUnk_02007D00[3] >> 2;

    if (x < q)
        return 0;
    if (x < gUnk_02007D00[3] >> 1)
        return 1;
    if (x < q * 3)
        return 2;
    return 3;
}

void MetaKnightSetFollowTimer(void)
{
    if (gCurTask->health >= gUnk_02007D00[3] >> 1)
        gCurTask->metaKnightFollowTimer = RandomRange(54) + 48;
    else
        gCurTask->metaKnightFollowTimer = RandomRange(84) + 96;
}

void MetaKnightAnimateWalk(void)
{
    struct Task **c = &gCurTask;
    struct Task **d;
    u8 *a = gUnk_08748D98;
    u8 *b = a + 8;
    struct Task *t;
    struct Task *u;
    s32 ix;
    s32 n;

    for (;;)
    {
        t = *c;
        if (t->metaKnightWalkAnimFlags & 2)
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)b));
        }
        else
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)a));
        }
        t = *c;
        n = t->metaKnightWalkAnimFlags & 1;
        d = &gCurTask;
        if (n == 0)
        {
            if ((t->facing == 1 && t->velX >= 0) || (t->facing == -1 && t->velX <= 0))
            {
                u = *c;
                u->frame++;
                if ((s16)u->frame > 68)
                    u->frame = 61;
            }
            else
            {
                u = *d;
                u->frame--;
                if ((s16)u->frame <= 60)
                    u->frame = 68;
            }
        }
    }
}

s32 sub_080a6f38(s16 x, s16 y)
{
    if ((u16)GetCollisionTileAtOffset(x, y, 1, 0) == 0 && (u16)GetCollisionTileAtOffset(x, y, -1, 0) == 0)
        return 0;
    return 1;
}

s32 MetaKnightClampToRoom(void)
{
    u8 v = ClampTaskToRoom(gCurTask);

    if (((v & 1) && gCurTask->velX < 0) || ((v & 2) && gCurTask->velX > 0))
        return 1;
    return 0;
}

void MetaKnightPickAirAttack(void)
{
    s32 k;
    s32 g;

    if ((u8)MetaKnightClampToRoom() == 1)
        TaskTurnAroundAndReverseX();
    if (gUnk_02007D00[7] == 0 && gCurTask->onGround == 0 && gCurTask->velY > 0)
    {
        if (gUnk_02007D00[6] == 0)
        {
            gCurTask->metaKnightFollowFlags = 0;
            gUnk_02007D00[7]++;
            gUnk_02007D00[4] = TaskGetNearestPlayerDx();
            if (TaskGetNearestPlayerDy() < 0)
                k = 0;
            else
            {
                g = gUnk_02007D00[4];
                if (abs(g) <= 15)
                    k = 1;
                else if ((g > 0 && gCurTask->facing == 1) ||
                         (g < 0 && gCurTask->facing == -1))
                    k = 2;
                else
                    k = 3;
            }
            gUnk_02007D00[6] = gUnk_08748E28[k][RandomRange(4)];
            if (gUnk_02007D00[6] == 0)
                return;
        }
        ActorSetState((u16)gUnk_02007D00[6]);
    }
}

void MetaKnightUpwardSlash(void)
{
    gUnk_02007D00[4] = -1;
    TaskSetFrame(98);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gUnk_02007D00[4] = PlaySfx(137 << 2);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x40000;
    gCurTask->accelY = 128 << 8;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskStopY();
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->velY = 128 << 11;
    gCurTask->accelY = -0x8000;
    TaskSetFrame(101);
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
}

void sub_080a7168(void)
{
    gCurTask->metaKnightGuardFlags |= 128;
    TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void MetaKnightPickNextState(void)
{
    struct Task *pt = &gTasks[TaskFindNearestPlayer()];

    if (gCurTask->metaKnightFollowFlags & 1)
        ActorSetState(gUnk_08748DC8[RandomRange(16)]);
    else
        ActorSetState(gUnk_08748DE8[pt->onGround][RandomRange(16)]);
    gCurTask->metaKnightFollowFlags = 0;
    sub_080a7168();
}

s32 MetaKnightTryGuard(void)
{
    u8 k;

    if (gCurTask->metaKnightGuardFlags & 128)
        gCurTask->metaKnightGuardFlags++;
    if (gCurTask->metaKnightGuardFlags & 3)
    {
        gCurTask->metaKnightGuardFlags--;
        k = MetaKnightGetHealthQuarter();
        if (RandomRange(16) < gUnk_08748D74[k])
            return 1;
    }
    return 0;
}

s32 MetaKnightReactToDamage(void)
{
    gBg2Cnt |= 64;
    gBg3Cnt |= 64;
    gCurTask->metaKnightGuardFlags |= 2;
    BossStartHitStun(13, (u32)MetaKnightHitStunUpdate, (u32)gUnk_082F427C, 16, 1);
    return 0;
}

void MetaKnightHitStunUpdate(void)
{
    gBgMosaic = gUnk_08748EAC[gUnk_02006190[3] >> 1];
    if (gUnk_02006190[3] == 0)
    {
        gBg2Cnt &= 0xFFBF;
        gBg3Cnt &= 0xFFBF;
        BossEndHitStun();
    }
}

s32 MetaKnightReactToDefeat(void)
{
    TaskStop();
    ActorSetHitReactions((u32)gMetaKnightReactToDefeatHitReactions);
    gCurTask->metaKnightAttackBoxIndex = -1;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 MetaKnightHitWall(void)
{
    struct Task *t;

    switch (gCurTask->state)
    {
    case 1:
        t = gCurTask;
        t->metaKnightFollowPhase = 1;
        t->velX = 0;
        break;
    case 8:
    case 13:
    case 15:
    case 17:
        TaskTurnAroundAndReverseX();
        break;
    }
    return 0;
}

void MetaKnightDefeatedInit(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)ActorDrawStreamedFrameNearViewOrDestroy;
    t->updateCallback = (u32)MetaKnightDefeatedUpdate;
    t->frameTable = gMetaKnightFrames;
    t->layer = 4;
    TaskFaceNearestPlayer();
    ActorSetState(META_KNIGHT_DEFEATED_STATE_0);
    CallTableEntry(gCurTask->state, 2, gMetaKnightDefeatedStates);
}

void MetaKnightDefeatedUpdate(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 2, gMetaKnightDefeatedStateUpdates);
}

void MetaKnightDefeatedEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMetaKnightDefeatedStates);
}

void sub_080a7438(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = META_KNIGHT_DEFEATED_STATE_0;
    TaskStop();
    gCurTask->onGround = 0;
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    sp.subtype = 18;
    sp.taskType = TASK_META_KNIGHT_SWORD;
    sp.variant = 1;
    sp.spawnArg = 0;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = gCurTask->u8C.actor->savedTileWord + (128 << 5);
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 1);
    TaskGetScreenPosSlot(gCurTaskIdx);
    gCurTask->unk30 = gUnk_030023B4;
    TaskGetNearestPlayerScreenPos();
    if ((u32)(gUnk_030023B4 - 88) > 64)
        gCurTask->unk28 = 128;
    else if (gCurTask->unk30 <= 127)
        gCurTask->unk28 = 64;
    else
        gCurTask->unk28 = 176;
    if (gCurTask->unk30 < gCurTask->unk28)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetFrame(21);
    gCurTask->unk2C = abs(gCurTask->unk30 - gCurTask->unk28) >> 1;
    TaskSetMotionY(-((gCurTask->unk2C >> 1) << 13), 128 << 6, 192 << 10);
    if (gCurTask->velY == 0)
        gCurTask->velY = -0x10000;
    gCurTask->onGround = 0;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    ActorSetState(META_KNIGHT_DEFEATED_STATE_1);
    TaskSleepForever();
}

void sub_080a75a0(void)
{
    if (gCurTask->state != META_KNIGHT_DEFEATED_STATE_0)
        TaskSetEntry(MetaKnightDefeatedEnterState, gCurTaskIdx);
}

void sub_080a75c8(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_DEFEATED_STATE_1;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(14);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    t = gCurTask;
    CreateChildTask(TASK_META_KNIGHT_MASK, (s16)(t->pixelX - t->facing * 2), (s16)(t->pixelY - 1), t->u8C.actor->savedTileWord);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x30000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 128 << 9;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x44000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    TaskYieldTrampoline(9);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetFrame(45);
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(57);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    gCurTask->onGround = 0;
    gCurTask->velY = -0x70000;
    while (gCurTask->pixelY > 10)
        TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(30);
    FadeOutBgm(8);
    TaskYieldTrampoline(32);
    CreateStarRodPiece(0, 128, 104);
    ActorDestroy();
}

void sub_080a787c(void)
{
}

void Task_MetaKnightSwordHitBox(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)MetaKnightSwordHitBoxUpdate;
    TaskSleepForever();
}
