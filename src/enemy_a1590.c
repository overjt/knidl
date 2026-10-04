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
#include "enemy.h"
#include "save.h"

/* RAM cells / ROM tables */
/* Not from actor.h: this file's view of gUnk_02006040 differs (lesson 3.517). */
extern u32 gBossHitStunFlashPalette;
extern u32 gUnk_02006040[];
extern s32 gUnk_02006190[];
extern s32 gUnk_02007D00[];
extern s8 gPaletteAnimRefCounts[];
extern u8 gHudHpBarFilled;
extern u8 gMidBossDropsIn;
extern struct PlayerState gPlayerStates[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* External functions */
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
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
extern s32 TaskCreatePausedInScreenAttack(u32 type, s32 start);
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorLoadDefSlot(u32 i, struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAttackBoxSlot(u32 i, u32 v);
extern void ActorSetAux(struct ActorAux *v);
extern void ActorSetExtraAttackBox(u32 v);
extern void ActorSetExtraAttackBoxSlot(u32 i, u32 v);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetDyTo(u32 i);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskGetPosSlot(u32 i);
extern void TaskGetNearestPlayerPos(void);
extern s32 TaskGetFacingToward(u32 i);
extern void TaskFaceNearestPlayer(void);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern void ActorDestroySlot(s32 i);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskToggleFacingAndReverseX(void);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void ActorStopAnim(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern s32 ActorTickAnim(s32 n);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleTo(u32 i, s32 prec);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u8 TaskGetYDirBitTo(u32 i);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern u8 TaskGetXDirBitTo(u32 i);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern s32 TaskFindNearestPlayerInScreenXBand(u16 lo, u16 hi);
extern s32 TaskFindNearestPlayerInScreenYBand(u16 lo, u16 hi);
extern void TaskGetScreenPosSlot(u32 i);
extern s32 TaskGetNearestPlayerScreenPos(void);
extern void TaskGetScreenPos(void);
extern void TaskFaceLikeParent(void);
extern s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio);
extern void TaskStepSpinFrameFacing(void);
extern s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 CreateChildActorOfSameType(u8 p3, u8 p4, u32 x, u32 y, u16 prio);
extern u8 ActorIsInView(void);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorDrawStreamedFrameNearViewOrDestroy(void);
extern void ActorDrawStreamedFrameNearView(void);
extern void ActorDrawWorldInViewOrDestroyWithExtra(void);
extern void ActorMove(void);
extern void TaskMoveRelativeToView(void);
extern void ResetBgPaletteBlend(void);
extern void EndBgPaletteBlend(u32 v);
extern void StartBgPaletteBlend(u32 a, u32 b);
extern void LoadEnemyPaletteVariant(u32 slot, u32 sub);
extern void sub_08065dd0(u32 slot, u32 i);
extern void sub_08065dfc(u32 slot);
extern u8 TaskHasSameSerial(u32 i);
extern s16 ActorComputeHealth(void);
extern u16 ActorInitBossGfx(u32 mode);
extern void sub_08066144(void);
extern void BossStartHitStun(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4);
extern void BossEndHitStun(void);
extern s32 GetLivingActivePlayerHealth(void);
extern void ActorIntroPoseUntilHpBarFull(struct AnimCmd *p);
extern void sub_08066544(void);
extern void ActorResetAttackBox(void);
extern void sub_08066580(void);
extern u16 ActorGetGfxTileWordPalOffset(u16 a);
extern u16 ActorGetTileWordPalOffset(u16 a);
extern void ActorIntroPoseUntilScrollLocked(struct AnimCmd *p);
extern u32 BossCheckScrollLock(void);
extern void ActorLoadPalette(void *src, u32 size, u8 force);
extern u8 ActorIsInNearView(void);
extern void CreateNextRoomWarpStar(s32 x, s32 y);
extern void CreateStarRodPiece(u8 p3, s16 x, s16 y);
extern void CreateRoomStarRodPiece(void);
extern void FreezeStage(u16 a);
extern void ThawStage(void);
extern void DisablePause(void);
extern void EnablePause(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern void HoldPlayer(s32 i, s32 j, u8 c);
extern void SetHeldPlayerState(s32 i, u8 c);
extern void sub_08068950(s16 x, s16 y, s16 d);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorCollideTerrainAlongVelocity(void);
extern u32 ActorCollideTerrainCeilingAndFloor(void);
extern u32 ActorCollideTerrainWalls(void);
extern u32 ActorCollideTerrainInCameraBounds(void);
extern u32 ActorCollideTerrainFloor(void);
extern u32 ActorReactToHit(void);
extern u32 ActorReactToHitOrTerrainDamage(void);
extern u32 PickupReactToHit(void);
extern u32 ActorReactToDefeat(void);
extern void ActorDie(void);
extern void BossDefeatFlash(void);
extern void BossDefeatExplode(void);
extern s16 CreateStarFlash(u8 kind, s32 dx, s32 dy);
extern s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d);
extern void CreateBurstEffect(u32 a, s32 b);
extern void PlayExplosionAnim(void);
extern s32 CreateDashFlame(s16 x, s16 y);
extern s32 CreateDashFireTrail(s16 x, s16 y);
extern s32 CreateLandingImpact(u8 a, s16 x, s16 y);

/* Module functions */
void MrBrightFlashPalette();
void ReleaseRoomObject();
s32 LoadRoomEnemyGfx();
s32 LoadRoomMidBossGfx();
s32 LoadRoomBossGfx();
void LoadRoomMetaKnightsGfx();
s32 LoadRoomStageObjectGfx();
s32 SpawnRoomEnemy();
s32 SpawnRoomMapEvent();

/* KingDededeDefeatedFall (0x080A1590-0x080A15F0) */
void KingDededeDefeatedFall(void)
{
    struct Task *t;

    gCurTask->updateState = KING_DEDEDE_DEFEATED_STATE_FALL;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(9);
    t = gCurTask;
    t->accelY = 160 << 6;
    t->speedLimitY = 160 << 11;
    if (!(t->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(KING_DEDEDE_DEFEATED_STATE_HOLD_BACK);
    TaskSleepForever();
}

/* KingDededeDefeatedFallUpdate (0x080A15F0-0x080A1618) */
void KingDededeDefeatedFallUpdate(void)
{
    if (gCurTask->state != KING_DEDEDE_DEFEATED_STATE_FALL)
        TaskSetEntry(KingDededeDefeatedEnterState, gCurTaskIdx);
}

/* KingDededeDefeatHook (0x080A1618-0x080A1624) */
s32 KingDededeDefeatHook(void)
{
    TaskSetFrame(0);
}

/* KingDededeDefeatedLockPlayers (0x080A1624-0x080A1668) */
void KingDededeDefeatedLockPlayers(void)
{
    s32 i;
    struct PlayerState *p;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            gPlayerStates[i].statusFlags = 64;
    }
}

/* KingDededeDefeatedHoldBackPlayer (0x080A1668-0x080A1704) */
void KingDededeDefeatedHoldBackPlayer(s32 a)
{
    struct Task *t = &gTasks[a];

    if (gCurTask->facing == -1)
    {
        if (t->pixelX <= gRoomBounds[0] - 95)
            t->facing = 255;
        else
            t->facing = 1;
    }
    else
    {
        if (t->pixelX <= gRoomBounds[1] + 95)
            t->facing = 255;
        else
            t->facing = 1;
    }
    gPlayerStates[a].requestedAction = PLAYER_ACTION_RECOIL;
    gPlayerStates[a].statusFlags = 64;
    gUnk_02007D00[a] = -1;
}

/* sub_080a1704 (0x080A1704-0x080A1740) */
void sub_080a1704(s32 a)
{
    if (TaskGetFacingToward(a) == gCurTask->facing)
        KingDededeDefeatedCheckHoldBackPlayer(a);
    else
        gUnk_02007D00[a] = -1;
}

/* KingDededeDefeatedCheckHoldBackPlayer (0x080A1740-0x080A1790) */
void KingDededeDefeatedCheckHoldBackPlayer(s32 a)
{
    if (abs(TaskGetDxTo(a)) <= 48)
        KingDededeDefeatedHoldBackPlayer(a);
    else if (gCurTask->velX == 0)
        gUnk_02007D00[a] = -1;
}

/* KingDededeDefeatedCheckPlayers (0x080A1790-0x080A180C) */
void KingDededeDefeatedCheckPlayers(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            switch (gUnk_02007D00[i])
            {
            case 1:
                sub_080a1704(i);
                break;
            case 0:
                if (gPlayerStates[i].mode <= 2)
                    KingDededeDefeatedCheckHoldBackPlayer(i);
                break;
            default:
                if (gPlayerStates[i].mode <= 2)
                    gUnk_02007D00[i + 3]--;
                break;
            }
        }
    }
}

/* KingDededeDefeatedCheckWaitEnd (0x080A180C-0x080A1864) */
void KingDededeDefeatedCheckWaitEnd(void)
{
    s32 i;
    s32 sum;

    i = 0;
    sum = 0;
    for (; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            sum += gUnk_02007D00[i + 3];
    }
    if (sum == 0)
    {
        gCurTask->kingDededeDefeatedWaitEnd = 1;
        KingDededeDefeatedLockPlayers();
    }
}

/* KingDededeDefeatedStartHoldBack (0x080A1864-0x080A18D4) */
void KingDededeDefeatedStartHoldBack(void)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            if (gPlayerStates[i].mode <= 2)
                gUnk_02007D00[i] = 1;
            else
                gUnk_02007D00[i] = 0;
            gUnk_02007D00[i + 3] = 8;
        }
    }
    t = gCurTask;
    t->kingDededeDefeatedWaitEnd = 0;
    t->kingDededeDefeatedTimeLimit = 120;
    t->kingDededeDefeatedAnimating = 0;
    t->kingDededeDefeatedCheckingPlayers = 0;
}

/* KingDededeDefeatedHoldBack (0x080A18D4-0x080A1980) */
void KingDededeDefeatedHoldBack(void)
{
    s32 i;
    s32 v;

    gCurTask->updateState = KING_DEDEDE_DEFEATED_STATE_HOLD_BACK;
    TaskStop();
    KingDededeDefeatedStartHoldBack();
    TaskYieldTrampoline(16);
    RequestScreenShake(4);
    gCurTask->kingDededeDefeatedCheckingPlayers = 1;
    TaskSetFrame(10);
    v = 128 << 10;
    for (i = 2; i >= 0; i--)
    {
        TaskSetMotionXFacing(v, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        v >>= 1;
    }
    TaskStop();
    gCurTask->actorAnimDelay24 = ActorStartAnim(gKingDededeDefeatedHoldBackAnim);
    gCurTask->kingDededeDefeatedAnimating = 1;
    while (gCurTask->kingDededeDefeatedWaitEnd == 0)
        TaskYieldTrampoline(1);
    gCurTask->kingDededeDefeatedCheckingPlayers = 0;
    if (gCurTask->kingDededeDefeatedWaitEnd != 2)
        TaskYieldTrampoline(150);
    TaskYieldTrampoline(24);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = 0;
    ExitKingDededeStage();
    TaskSleepForever();
}

/* KingDededeDefeatedHoldBackUpdate (0x080A1980-0x080A19CC) */
void KingDededeDefeatedHoldBackUpdate(void)
{
    if (gCurTask->kingDededeDefeatedAnimating != 0)
        gCurTask->actorAnimDelay24 = ActorTickAnim(gCurTask->actorAnimDelay24);
    if (gCurTask->kingDededeDefeatedWaitEnd == 0)
    {
        if (gCurTask->kingDededeDefeatedTimeLimit <= 0)
        {
            gCurTask->kingDededeDefeatedWaitEnd = 2;
            KingDededeDefeatedLockPlayers();
        }
        else
        {
            gCurTask->kingDededeDefeatedTimeLimit--;
            if (gCurTask->kingDededeDefeatedCheckingPlayers != 0)
            {
                KingDededeDefeatedCheckPlayers();
                KingDededeDefeatedCheckWaitEnd();
            }
        }
    }
}

/* Task_MrShineAndMrBright (0x080A19CC-0x080A19EC) */
void Task_MrShineAndMrBright(void)
{
    CallTableEntry(gCurTask->variant, 4, gMrShineAndMrBrightVariants);
}

/* MrShineAndMrBrightFillHpBars (0x080A19EC-0x080A1AD0) */
void MrShineAndMrBrightFillHpBars(void)
{
    struct Task *tb;
    struct Task *ta;
    struct Actor *ab;
    struct Actor *aa;

    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    ab = tb->u8C.actor;
    aa = ta->u8C.actor;
    ActorSetAttackBoxSlot(gUnk_02007D00[1], ab->unk60->altAttackBox);
    ActorSetAttackBoxSlot(gUnk_02007D00[0], aa->unk60->altAttackBox);
    gHudHpBarFilled = 0;
    HudShowHpBar();
    HudStartHpBar(tb->health * 2, tb->health);
    while (gHudHpBarFilled == 0)
        TaskYieldTrampoline(1);
    gHudHpBarFilled = 0;
    HudShowHpBar();
    HudStartHpBar(ta->health * 2, ta->health);
    while (gHudHpBarFilled == 0)
        TaskYieldTrampoline(1);
    if (gCreditsDemoSet != 0)
        TaskYieldTrampoline(120);
    ActorSetAttackBoxSlot(gUnk_02007D00[1], (u32)gUnk_0874883C);
    ActorSetExtraAttackBoxSlot(gUnk_02007D00[1], (u32)gUnk_08748874);
    ActorSetAttackBoxSlot(gUnk_02007D00[0], (u32)gUnk_087488AC);
    ActorSetExtraAttackBoxSlot(gUnk_02007D00[0], (u32)gUnk_087488E4);
}

/* CreateMrShineAndMrBright (0x080A1AD0-0x080A1B94) */
void CreateMrShineAndMrBright(void)
{
    struct Actor *a;
    u32 v;
    u32 w;
    s32 r;

    a = gCurTask->u8C.actor;
    gUnk_02007D00[1] = CreateChildActorOfSameType(1, 0, gViewRect[0] + 192, gViewRect[2] + 48, a->savedTileWord);
    v = a->savedTileWord & 0xFFF;
    w = gCurTask->tileWord & 0xF000;
    gUnk_02007D00[0] = CreateChildActorOfSameType(2, 0, gViewRect[0] + 32, gViewRect[2] + 64, v | w);
    r = CreateChildActorOfSameType(3, 0, 0, 0, gCurTask->tileWord);
    gUnk_02006040[5] = r;
    if (r != -1)
    {
        struct Task *t = &gTasks[r];

        t->parent = gUnk_02007D00[0];
    }
    ActorLoadDefSlot(gUnk_02007D00[0], gUnk_087487BC);
    gUnk_02007D00[6] = 1;
    gUnk_02007D00[8] = ActorComputeHealth() >> 1;
}

/* MrShineAndMrBrightPickFirstAscender (0x080A1B94-0x080A1BD8) */
void MrShineAndMrBrightPickFirstAscender(void)
{
    s32 a;
    s32 b;

    if (RandomRange(2) != 0)
    {
        b = 1;
        a = 2;
        gUnk_02007D00[2] = 0;
        sub_080275cc(2);
    }
    else
    {
        b = 2;
        a = 1;
        gUnk_02007D00[2] = 1;
        sub_080275cc(0);
    }
    MrShineAndMrBrightSetStates(a, b);
}

/* MrShineAndMrBrightSetStates (0x080A1BD8-0x080A1C1C) */
void MrShineAndMrBrightSetStates(u16 a, u16 b)
{
    ActorSetStateSlot(gUnk_02007D00[1], a);
    TaskSetEntry(MrShineEnterState, gUnk_02007D00[1]);
    ActorSetStateSlot(gUnk_02007D00[0], b);
    TaskSetEntry(MrBrightEnterState, gUnk_02007D00[0]);
}

/* MrShineAndMrBrightMoveToMidpoint (0x080A1C1C-0x080A1C90) */
void MrShineAndMrBrightMoveToMidpoint(void)
{
    struct Task *tb;
    struct Task *ta;
    struct Task *t;

    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    t = gCurTask;
    t->pixelX = (tb->pixelX + ta->pixelX) >> 1;
    t->pixelY = (tb->pixelY + ta->pixelY) >> 1;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
}

/* MrShineAndMrBrightStartDefeat (0x080A1C90-0x080A1D2C) */
void MrShineAndMrBrightStartDefeat(void)
{
    struct Task *tb;
    struct Task *ta;

    gCurTask->health = 0;
    gUnk_02007D00[7] = 0;
    gUnk_02007D00[6] = 0;
    MrShineAndMrBrightMoveToMidpoint();
    ActorSetState(MR_SHINE_AND_MR_BRIGHT_STATE_2);
    TaskSetEntry(MrShineAndMrBrightEnterState, gCurTaskIdx);
    if (gUnk_02006040[6] != -1)
    {
        StopSfxOnPlayer(gUnk_02006040[6], 0x216);
        gUnk_02006040[6] = -1;
    }
    MrShineAndMrBrightSetStates(16, 16);
    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    tb->mrShineAndMrBrightFrozen = 1;
    ta->mrShineAndMrBrightFrozen = 1;
    gCurTask->lateUpdateCallback = 0;
    sub_080275cc(1);
}

/* MrShineAndMrBrightStartConvergeEnd (0x080A1D2C-0x080A1D84) */
void MrShineAndMrBrightStartConvergeEnd(void)
{
    struct Task *tb;
    struct Task *ta;

    gUnk_02007D00[7] = 0;
    ActorSetState(MR_SHINE_AND_MR_BRIGHT_STATE_3);
    TaskSetEntry(MrShineAndMrBrightEnterState, gCurTaskIdx);
    MrShineAndMrBrightSetStates(17, 17);
    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    tb->mrShineAndMrBrightFrozen = 1;
    ta->mrShineAndMrBrightFrozen = 1;
}

/* MrShineAndMrBrightEndConverge (0x080A1D84-0x080A1DBC) */
void MrShineAndMrBrightEndConverge(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    ActorSetHitReactions((u32)gMrShineAndMrBrightDefeatedHitReactions);
    a->defeatSweepCallback = (u32)MrShineAndMrBrightDefeatSweepFilter;
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

/* MrShineAndMrBrightDefeatSweepFilter (0x080A1DBC-0x080A1DD4) */
s32 MrShineAndMrBrightDefeatSweepFilter(s32 a)
{
    if ((u16)gTaskSlotTypes[a] == TASK_MR_SHINE_AND_MR_BRIGHT)
        return 0;
    return 1;
}

/* MrShineAndMrBrightCheckAscend (0x080A1DD4-0x080A1DF8) */
void MrShineAndMrBrightCheckAscend(void)
{
    if (gUnk_02007D00[5] == 0 && gUnk_02007D00[3] != 0)
    {
        gUnk_02007D00[3] = 3;
        ActorSetState(2);
    }
}

/* MrShineAndMrBrightLand (0x080A1DF8-0x080A1E4C) */
s32 MrShineAndMrBrightLand(void)
{
    if (gCurTask->state == 8)
    {
        ActorSetState(1);
        if (gCurTask->variant == 2)
            TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
        else
            TaskSetEntry(MrShineEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

/* MrShineAndMrBrightHitWall (0x080A1E4C-0x080A1EC4) */
s32 MrShineAndMrBrightHitWall(void)
{
    struct Task *t = gCurTask;

    switch (t->state)
    {
    default:
        t->velX = 0;
        return 0;
    case 11:
        TaskSetMotionX(-t->velX, -t->accelX, t->speedLimitX);
        return 0;
    case 12:
        t->velX = 0;
        MrShineAndMrBrightEndFlash();
        ActorSetState(13);
        if (gCurTask->variant == 2)
            TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
        else
            TaskSetEntry(MrShineEnterState, gCurTaskIdx);
        return 1;
    }
}

/* MrShineAndMrBrightReactToDefeat (0x080A1EC4-0x080A1F90) */
s32 MrShineAndMrBrightReactToDefeat(void)
{
    RequestScreenShake(4);
    CreateBurstEffect(1, 0);
    HudSetTaskHpBar();
    PlaySfx(510);
    CreateChildTaskHere(TASK_BOSS_SCREEN_FLASH, 1);
    if (gCurTask->mrShineAndMrBrightDashTrailSlot != -1)
    {
        ActorDestroySlot(gCurTask->mrShineAndMrBrightDashTrailSlot);
        gCurTask->mrShineAndMrBrightDashTrailSlot = 0xFFFF;
    }
    if (gCurTask->variant == 2)
        sub_080a30d0();
    if (gUnk_02007D00[5] != 0)
        ActorSetState(15);
    else
        ActorSetState(2);
    gCurTask->mrShineAndMrBrightKnockedOut++;
    gUnk_02007D00[5]++;
    if (gCurTask->variant == 2)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    else
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    TaskStop();
    return 1;
}

/* MrShineAndMrBrightHitStunUpdate (0x080A1F90-0x080A1FC8) */
void MrShineAndMrBrightHitStunUpdate(void)
{
    if (gCurTask->variant == 2)
        MrBrightFlashPalette(gBossHitStunFlashPalette, gUnk_02006190[4]);
    if (gUnk_02006190[3] <= 0)
        BossEndHitStun();
}

/* MrShineAndMrBrightReactToDamage (0x080A1FC8-0x080A2020) */
s32 MrShineAndMrBrightReactToDamage(void)
{
    if (gCurTask->variant == 2)
        BossStartHitStun(13, (u32)MrShineAndMrBrightHitStunUpdate, (u32)gUnk_082FB230, 16, 2);
    else
        BossStartHitStun(13, (u32)MrShineAndMrBrightHitStunUpdate, (u32)gUnk_082FB210, 16, 0);
    CreateStarFlash(1, 0, 0);
    return 0;
}

/* sub_080a2020 (0x080A2020-0x080A2030) */
void sub_080a2020(void)
{
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

/* MrShineAndMrBrightStartDescend (0x080A2030-0x080A2090) */
void MrShineAndMrBrightStartDescend(void)
{
    HudSetTaskHpBar();
    if (gCurTask->pixelY - gViewRect[2] <= 31)
        TaskSetMotionY(128 << 9, 168 << 5, 192 << 11);
    else
        TaskSetMotionY(-0x30000, 168 << 5, 192 << 11);
    MrShineAndMrBrightEndFlash();
    gCurTask->layer = 10;
}

/* MrShineAndMrBrightStartAscend (0x080A2090-0x080A2164) */
void MrShineAndMrBrightStartAscend(void)
{
    struct Task *t;
    s32 v;

    v = RandomRange(24) + 16;
    gUnk_02006040[4] = v;
    t = gCurTask;
    if (t->pixelX - gViewRect[0] <= 99)
        t->facing = 1;
    else
        t->facing = 255;
    if (gCurTask->variant == 2)
    {
        gUnk_02007D00[2] = 1;
        ActorSetAttackBox((u32)gMrShineAndMrBrightStartAscendAttackBox);
        sub_080275cc(0);
    }
    else
    {
        gUnk_02007D00[2] = 0;
        ActorSetAttackBox((u32)gUnk_08748858);
        sub_080275cc(2);
    }
    gUnk_02007D00[4] = 0;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskSetMotionY(-0x60000, 148 << 6, 192 << 10);
    MrShineAndMrBrightEndFlash();
    gCurTask->layer = 12;
}

/* MrShineAndMrBrightDescendUpdate (0x080A2164-0x080A21A0) */
void MrShineAndMrBrightDescendUpdate(void)
{
    struct Task *t = gCurTask;
    u16 d = t->pixelY - gViewRect[2];

    if ((s16)d <= 15)
    {
        t->pixelY = gViewRect[2] + 17;
        t->posY = t->pixelY << 16;
        t->velY = 0;
    }
}

/* MrShineAndMrBrightAscendUpdate (0x080A21A0-0x080A2224) */
void MrShineAndMrBrightAscendUpdate(void *a)
{
    struct Task *t = gCurTask;
    u16 d = t->pixelY - gViewRect[2];

    if (t->velY < 0)
    {
        if ((s16)d > 16)
            return;
        t->pixelY = gViewRect[2] + 17;
        t->posY = t->pixelY << 16;
        t->velY = 0;
    }
    else if ((s16)d <= (s32)gUnk_02006040[4])
    {
        if ((s16)d > 16)
            return;
        t->pixelY = gViewRect[2] + 17;
        t->posY = t->pixelY << 16;
        t->velY = 0;
    }
    else
    {
        t->pixelY = gViewRect[2] + gUnk_02006040[4];
        t->posY = t->pixelY << 16;
        TaskStop();
        ActorSetState(3);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* MrShineAndMrBrightCheckKnockOuts (0x080A2224-0x080A2274) */
s32 MrShineAndMrBrightCheckKnockOuts(void *a)
{
    s32 v;

    if (gUnk_02007D00[5] != 0)
    {
        if (gUnk_02007D00[5] == 2 && gCurTask->state != 16)
            v = 15;
        else
        {
            if (gCurTask->mrShineAndMrBrightKnockedOut != 0)
                return 0;
            v = 8;
        }
        ActorSetState(v);
        TaskSetEntry(a, gCurTaskIdx);
        return 1;
    }
    return 0;
}

/* MrShineAndMrBrightCheckChase (0x080A2274-0x080A22B0) */
void MrShineAndMrBrightCheckChase(void *a)
{
    if (TaskFindNearestPlayerInScreenYBand(16, 64) != 0)
    {
        gCurTask->mrShineAndMrBrightTargetSlot = gUnk_030023D4;
        ActorSetState(6);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* MrShineAndMrBrightGetWaitTime (0x080A22B0-0x080A22D4) */
s32 MrShineAndMrBrightGetWaitTime(void)
{
    if (gCurTask->health < gUnk_02007D00[8])
        return 60;
    return 120;
}

/* MrShineAndMrBrightPickGroundMove (0x080A22D4-0x080A2390) */
void MrShineAndMrBrightPickGroundMove(void)
{
    struct Task *t;
    s32 ofs;
    s32 r;
    s32 acc;
    s32 i;

    t = gCurTask;
    ofs = 0;
    if (t->health >= gUnk_02007D00[8])
        ofs = 4;
    r = 8 - gUnk_087484EC[t->mrShineAndMrBrightLastMove + ofs];
    r = RandomRange(r);
    acc = 0;
    i = 3;
    if (gCurTask->mrShineAndMrBrightLastMove != 3)
    {
        acc = gUnk_087484EC[ofs + 3];
        if (acc >= r)
            goto sel;
    }
dec:
    /* Zero-code stand-in: the loop note of this do/while (0) weights the
       refs of acc inside it, so global allocation ranks acc (r2) above r
       (r3); acc's constant `= 0` set halves its priority otherwise. */
    do
    {
        i--;
        if (i < 0)
            goto sel;
        if (gCurTask->mrShineAndMrBrightLastMove == i)
            goto dec;
        acc += gUnk_087484EC[i + ofs];
        if (acc < r)
            goto dec;
    } while (0);
sel:
    if (gUnk_02007D00[4] != 0 && !(i & 1))
    {
        if (RandomRange(2) != 0)
            i++;
        else
            i--;
    }
    i &= 3;
    gCurTask->mrShineAndMrBrightLastMove = i;
    ActorSetState(gUnk_087484E4[i]);
}

/* MrShineAndMrBrightClampToRoom (0x080A2390-0x080A23C0) */
s32 MrShineAndMrBrightClampToRoom(void)
{
    s32 ret = 0;
    u8 v = ClampTaskToRoom(gCurTask);

    if ((v & 1) || (v & 2))
        ret = 1;
    return ret;
}

/* MrShineAndMrBrightPickJumpDir (0x080A23C0-0x080A2400) */
void MrShineAndMrBrightPickJumpDir(void)
{
    TaskFaceNearestPlayer();
    if (RandomRange(4) == 0)
        gCurTask->mrShineAndMrBrightJumpDir = -gCurTask->facing;
    else
        gCurTask->mrShineAndMrBrightJumpDir = gCurTask->facing;
}

/* MrShineAndMrBrightEndSkyAttack (0x080A2400-0x080A2448) */
void MrShineAndMrBrightEndSkyAttack(void *a)
{
    gUnk_02007D00[4] = 0;
    if (gCurTask->mrShineAndMrBrightKnockedOut != 0)
        ActorSetState(3);
    else
    {
        gUnk_02007D00[7] = 0;
        gCurTask->mrShineAndMrBrightSkyTimer = 300;
        ActorSetState(7);
    }
    TaskSetEntry(a, gCurTaskIdx);
}

/* MrShineAndMrBrightEndChase (0x080A2448-0x080A2488) */
void MrShineAndMrBrightEndChase(void *a)
{
    if (gUnk_02007D00[7] == 0)
    {
        if (gUnk_02007D00[3] == 0)
            gUnk_02007D00[3] = 1;
        MrShineAndMrBrightCheckDescend(a);
    }
    else
    {
        ActorSetState(4);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* MrShineAndMrBrightChaseStep (0x080A2488-0x080A24F8) */
void MrShineAndMrBrightChaseStep(void *a)
{
    if ((u8)MrShineAndMrBrightFindChaseTarget() != 0)
    {
        AngleToVector((s16)TaskGetAngleTo(gCurTask->mrShineAndMrBrightTargetSlot, 3), 64);
        gCurTask->velX = gUnk_030023B4;
        gCurTask->velY = gUnk_030023D4;
    }
    else
    {
        if (gUnk_02007D00[7] == 0)
            ActorSetState(7);
        else
            ActorSetState(3);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* MrShineAndMrBrightFindChaseTarget (0x080A24F8-0x080A2590) */
s32 MrShineAndMrBrightFindChaseTarget(void)
{
    s32 ret = 0;
    s32 d2;
    s32 d = gCurTask->pixelY - gViewRect[2];

    if (d >= 16 && d <= 63)
    {
        if ((u8)MrShineAndMrBrightClampToRoom() == 0 && TaskFindNearestPlayerInScreenYBand(16, 64) != 0 && TaskFindNearestPlayerInScreenXBand(16, 208) != 0)
        {
            gCurTask->mrShineAndMrBrightTargetSlot = gUnk_030023D4;
            ret = 1;
        }
    }
    else
    {
        if (d <= 15)
            gCurTask->pixelY = gViewRect[2] + 17;
        d2 = gCurTask->pixelY - gViewRect[2];
        if (d2 > 63)
            gCurTask->pixelY = gViewRect[2] + 64;
        gCurTask->posY = gCurTask->pixelY << 16;
    }
    return ret;
}

/* MrShineAndMrBrightCheckDescend (0x080A2590-0x080A25C4) */
void MrShineAndMrBrightCheckDescend(void *a)
{
    if ((gUnk_02007D00[3] >> 1) & 1)
    {
        gUnk_02007D00[3] = 0;
        ActorSetState(8);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* MrShineAndMrBrightWaitToAttackUpdate (0x080A25C4-0x080A2608) */
void MrShineAndMrBrightWaitToAttackUpdate(void *a)
{
    if ((u8)MrShineAndMrBrightCheckKnockOuts(a) == 0)
    {
        gCurTask->mrShineAndMrBrightSkyTimer--;
        if (gCurTask->mrShineAndMrBrightSkyTimer <= 0)
        {
            ActorSetState(4);
            TaskSetEntry(a, gCurTaskIdx);
        }
        else
            MrShineAndMrBrightCheckChase(a);
    }
}

/* MrShineAndMrBrightChaseUpdate (0x080A2608-0x080A263C) */
void MrShineAndMrBrightChaseUpdate(void *a)
{
    if ((u8)MrShineAndMrBrightCheckKnockOuts(a) == 0)
    {
        gCurTask->mrShineAndMrBrightSkyTimer--;
        if (gCurTask->mrShineAndMrBrightSkyTimer <= 0)
            MrShineAndMrBrightEndChase(a);
        else
            MrShineAndMrBrightChaseStep(a);
    }
}

/* MrShineAndMrBrightWaitToDescendUpdate (0x080A263C-0x080A268C) */
void MrShineAndMrBrightWaitToDescendUpdate(void *a)
{
    if ((u8)MrShineAndMrBrightCheckKnockOuts(a) == 0)
    {
        if (gUnk_02007D00[3] == 0)
        {
            gCurTask->mrShineAndMrBrightSkyTimer--;
            if (gCurTask->mrShineAndMrBrightSkyTimer > 0)
                MrShineAndMrBrightCheckChase(a);
            else
            {
                gUnk_02007D00[3]++;
                MrShineAndMrBrightCheckDescend(a);
            }
        }
        else
            MrShineAndMrBrightCheckDescend(a);
    }
}

/* MrShineAndMrBrightStopNearParent (0x080A268C-0x080A2754) */
void MrShineAndMrBrightStopNearParent(s32 a)
{
    if (gCurTask->velX != 0)
    {
        if (abs(TaskGetDxTo(gCurTask->parent)) <= a)
        {
            gCurTask->velX = 0;
            gCurTask->mrShineAndMrBrightArrivedAxes++;
        }
    }
    if (gCurTask->velY != 0)
    {
        if (abs(TaskGetDyTo(gCurTask->parent)) <= a)
        {
            gCurTask->velY = 0;
            gCurTask->mrShineAndMrBrightArrivedAxes++;
        }
    }
    if (gCurTask->mrShineAndMrBrightArrivedAxes == 2)
        gUnk_02007D00[7]++;
}

/* MrShineAndMrBrightAimAtParent (0x080A2754-0x080A27B8) */
void MrShineAndMrBrightAimAtParent(void)
{
    TaskStop();
    gCurTask->mrShineAndMrBrightArrivedAxes = 0;
    gCurTask->mrShineAndMrBrightFrozen = 0;
    AngleToVector((s16)TaskGetAngleTo(gCurTask->parent, 3), 256);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    if (gCurTask->velX == 0)
        gCurTask->mrShineAndMrBrightArrivedAxes++;
    if (gCurTask->velY == 0)
        gCurTask->mrShineAndMrBrightArrivedAxes++;
}

/* sub_080a27b8 (0x080A27B8-0x080A2814) */
void sub_080a27b8(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 11;
    t = gCurTask;
    t->frameTable = gMrShineFrames;
    TaskFaceNearestPlayer();
    ActorSetState(MR_SHINE_STATE_FALL);
    t = gCurTask;
    t->unk34 = 0;
    t->unk30 = 0;
    t->mrShineAndMrBrightPaletteTimer = 0;
    t->mrShineAndMrBrightPaletteIndex = 0;
    t->unk24 = 0;
    t->mrShineAndMrBrightKnockedOut = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->mrShineAndMrBrightFlashing = 0;
    t->mrShineAndMrBrightFrozen = 0;
    t->mrShineAndMrBrightPaletteTimer = 0;
    t->mrShineAndMrBrightPaletteIndex = 0;
}

/* MrShineUpdatePalette (0x080A2814-0x080A28D0) */
void MrShineUpdatePalette(void)
{
    struct Task *t = gCurTask;

    if (!(t->u8C.actor->paletteLocked & 1))
    {
        if ((s16)t->mrShineAndMrBrightFlashing != 0 || t->mrShineAndMrBrightKnockedOut != 0)
        {
            u32 v = 2;

            if (t->mrShineAndMrBrightKnockedOut != 0)
                v = 4;
            v &= gFrameCount;
            if (v == 0)
                v = (u32)gUnk_082FB210;
            else
                v = (u32)gUnk_082FB190;
            RequestCopy(2, v, (u32)(gObjPalette + ((t->tileWord >> 12) << 5)), 32);
        }
        else
        {
            if (t->mrShineAndMrBrightPaletteTimer <= 0)
            {
                RequestCopy(2, gUnk_0874850C[t->mrShineAndMrBrightPaletteIndex],
                             (u32)(gObjPalette + ((t->tileWord >> 12) << 5)), 32);
                t = gCurTask;
                t->mrShineAndMrBrightPaletteTimer = 12;
                t->mrShineAndMrBrightPaletteIndex = (t->mrShineAndMrBrightPaletteIndex + 1) & 3;
            }
            gCurTask->mrShineAndMrBrightPaletteTimer--;
        }
    }
}

/* MrShineUpdateWalkSpeed (0x080A28D0-0x080A291C) */
void MrShineUpdateWalkSpeed(void)
{
    s32 i;
    s32 *p;

    gCurTask->mrShineAndMrBrightStepTimer--;
    i = 0;
    p = gUnk_0874851C;
    if (*p > gCurTask->mrShineAndMrBrightStepTimer)
    {
        do
        {
            p++;
            i++;
            if (i > 3)
                break;
        } while (*p > gCurTask->mrShineAndMrBrightStepTimer);
    }
    TaskSetMotionXFacing(gUnk_0874852C[i], 0x5A5A5A5A);
}

/* CreateMrShineCrescent (0x080A291C-0x080A2954) */
void CreateMrShineCrescent(void)
{
    struct ActorSpawn sp;

    sp.subtype = 24;
    sp.taskType = TASK_MR_SHINE_AND_MR_BRIGHT_ATTACK;
    sp.variant = MR_SHINE_AND_MR_BRIGHT_ATTACK_VARIANT_MR_SHINE_CRESCENT;
    sp.spawnArg = 0;
    sp.x = 24;
    sp.y = 0;
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    PlaySfx(0x226);
}

/* CreateMrShineFallingStar (0x080A2954-0x080A2994) */
void CreateMrShineFallingStar(void)
{
    struct ActorSpawn sp;
    struct Task *t = gCurTask;

    t->mrShineAndMrBrightStarCount--;
    t->mrShineAndMrBrightStarTimer = 45;
    sp.subtype = 24;
    sp.taskType = TASK_MR_SHINE_AND_MR_BRIGHT_ATTACK;
    sp.variant = MR_SHINE_AND_MR_BRIGHT_ATTACK_VARIANT_MR_SHINE_FALLING_STAR;
    sp.spawnArg = t->mrShineAndMrBrightStarCount;
    sp.x = 0;
    sp.y = 0;
    sp.checkTerrain = 0;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
}

/* MrShineSetSkyFrame (0x080A2994-0x080A29CC) */
void MrShineSetSkyFrame(void)
{
    struct Task *t = gCurTask;

    if (t->facing == -1)
        t->frame = 4;
    else
        t->frame = 8;
    if (gCurTask->mrShineAndMrBrightKnockedOut != 0)
        TaskSetFrame(19);
}

/* MrShineStartWaitToAttack (0x080A29CC-0x080A2A00) */
void MrShineStartWaitToAttack(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    TaskStop();
    MrShineSetSkyFrame();
    if (a->prevState != 6)
        gCurTask->mrShineAndMrBrightSkyTimer = 300;
    gUnk_02007D00[7] = 1;
}

/* MrShineStartWindUp (0x080A2A00-0x080A2A24) */
void MrShineStartWindUp(void)
{
    TaskStop();
    gUnk_02007D00[4] = 1;
    gCurTask->mrShineAndMrBrightFlashing = 1;
    MrShineSetSkyFrame();
}

/* MrShineStartDropStars (0x080A2A24-0x080A2A44) */
void MrShineStartDropStars(void)
{
    TaskStop();
    gCurTask->mrShineAndMrBrightStarTimer = 45;
    gCurTask->mrShineAndMrBrightStarCount = 8;
    MrShineSetSkyFrame();
}

/* sub_080a2a44 (0x080A2A44-0x080A2A94) */
void sub_080a2a44(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 11;
    t = gCurTask;
    t->frameTable = gMrBrightFrames;
    TaskFaceNearestPlayer();
    ActorSetState(MR_BRIGHT_STATE_FALL);
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 0;
    gCurTask->unk2C = 0;
    gCurTask->unk28 = 0;
    gCurTask->unk24 = 0;
    gCurTask->mrShineAndMrBrightKnockedOut = 0;
    gCurTask->actorAnimDelay1C = 0;
    gCurTask->mrShineAndMrBrightTargetScreenX = 0;
}

/* MrBrightUpdateBob (0x080A2A94-0x080A2AF4) */
void MrBrightUpdateBob(void)
{
    s32 i;
    s32 *p;

    gCurTask->mrShineAndMrBrightBobFrame--;
    if (gCurTask->mrShineAndMrBrightBobFrame < 0)
        gCurTask->mrShineAndMrBrightBobFrame = 47;
    gCurTask->onGround = 0;
    i = 0;
    p = gUnk_08748604;
    if (*p > gCurTask->mrShineAndMrBrightBobFrame)
    {
        do
        {
            p++;
            i++;
            if (i > 7)
                break;
        } while (*p > gCurTask->mrShineAndMrBrightBobFrame);
    }
    gCurTask->velY = gUnk_08748614[i];
}

/* MrBrightStartJump (0x080A2AF4-0x080A2B14) */
void MrBrightStartJump(void)
{
    TaskStop();
    MrShineAndMrBrightPickJumpDir();
    ActorStopAnim();
    TaskSetFrame(4);
    TaskYieldTrampoline(12);
}

/* sub_080a2b14 (0x080A2B14-0x080A2B2C) */
void sub_080a2b14(void)
{
    TaskStop();
    TaskSetFrame(10);
    TaskYieldTrampoline(12);
}

/* MrBrightFlashPalette (0x080A2B2C-0x080A2BC4) */
void MrBrightFlashPalette(void *a, u32 b)
{
    struct Task *t = gCurTask;
    struct TaskGfx *g;
    u32 *gt;
    s16 fr;

    if (t->frameTable != 0 && t->frame != -1 && !(t->u8C.actor->paletteLocked & 1))
    {
        t->u8C.actor->paletteOverridden |= 1;
        gt = gTasks[gUnk_02006040[5]].frameTable;
        fr = gTasks[gUnk_02006040[5]].frame;
        if (fr != -1)
            g = (struct TaskGfx *)gt[fr];
        else
            g = (struct TaskGfx *)gt[0];
        if (!(gFrameCount & 2))
            ActorLoadPalette((void *)((u32)g->palette + 2), g->palette[0], 0);
        else
            ActorLoadPalette(a, b << 1, 1);
    }
}

/* MrBrightUpdatePalette (0x080A2BC4-0x080A2C90) */
void MrBrightUpdatePalette(void)
{
    struct Task *t = gCurTask;
    struct Actor *act = t->u8C.actor;
    struct TaskGfx *g;
    u32 *gt;
    struct Task *o;
    u32 v;

    if (!(act->paletteLocked & 1))
    {
        if ((s16)t->mrShineAndMrBrightFlashing != 0 || t->mrShineAndMrBrightKnockedOut != 0)
        {
            v = 2;
            if (t->mrShineAndMrBrightKnockedOut != 0)
                v = 4;
            act->paletteOverridden |= 1;
            v &= gFrameCount;
            if (v == 0)
                RequestCopy(2, (u32)gUnk_082FB230,
                             (u32)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)), 32);
            else
            {
                gt = gTasks[gUnk_02006040[5]].frameTable;
                o = &gTasks[gUnk_02006040[5]];
                if (o->frame != -1)
                    g = (struct TaskGfx *)gt[o->frame];
                else
                    g = (struct TaskGfx *)gt[0];
                RequestCopy(2, (u32)g->palette + 2,
                             (u32)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)),
                             g->palette[0]);
            }
        }
    }
}

/* MrShineAndMrBrightEndFlash (0x080A2C90-0x080A2CA8) */
void MrShineAndMrBrightEndFlash(void)
{
    gCurTask->u8C.actor->paletteOverridden = 0;
    gCurTask->mrShineAndMrBrightFlashing = 0;
}

/* CreateMrBrightFireball (0x080A2CA8-0x080A2CE8) */
void CreateMrBrightFireball(void)
{
    struct ActorSpawn sp;

    sp.subtype = 24;
    sp.taskType = TASK_MR_SHINE_AND_MR_BRIGHT_ATTACK;
    sp.variant = MR_SHINE_AND_MR_BRIGHT_ATTACK_VARIANT_MR_BRIGHT_FIREBALL;
    sp.spawnArg = 0;
    sp.x = 24;
    sp.y = 0;
    sp.tileWord = ActorGetGfxTileWordPalOffset(1);
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
    PlaySfx(0x227);
}

/* CreateMrBrightBeam (0x080A2CE8-0x080A2D38) */
void CreateMrBrightBeam(void)
{
    struct ActorSpawn sp;

    sp.subtype = 24;
    sp.taskType = TASK_MR_SHINE_AND_MR_BRIGHT_ATTACK;
    sp.variant = MR_SHINE_AND_MR_BRIGHT_ATTACK_VARIANT_MR_BRIGHT_BEAM;
    sp.spawnArg = 0;
    sp.x = 0;
    sp.y = 64;
    sp.tileWord = ActorGetGfxTileWordPalOffset(2);
    sp.checkTerrain = 0;
    gUnk_02006040[0] = CreateActorFromDesc(&sp, 1);
    gUnk_02006040[6] = PlaySfx(0x216);
}

/* CreateMrBrightBeamEffects (0x080A2D38-0x080A2DE0) */
void CreateMrBrightBeamEffects(void)
{
    s32 beamEffectSlot;
    struct Task *beamEffect;

    beamEffectSlot = CreateChildTask(TASK_MR_BRIGHT_BEAM_EFFECT, 0, 96, (u16)ActorGetGfxTileWordPalOffset(2));
    gUnk_02006040[3] = beamEffectSlot;
    if (beamEffectSlot != -1)
    {
        beamEffect = &gTasks[beamEffectSlot];
        beamEffect->variant = 0;
    }
    beamEffectSlot = CreateChildTask(TASK_MR_BRIGHT_BEAM_EFFECT, 0, 16, (u16)ActorGetGfxTileWordPalOffset(2));
    gUnk_02006040[1] = beamEffectSlot;
    if (beamEffectSlot != -1)
    {
        beamEffect = &gTasks[beamEffectSlot];
        beamEffect->variant = 1;
    }
    beamEffectSlot = CreateChildTask(TASK_MR_BRIGHT_BEAM_EFFECT, 0, 8, (u16)ActorGetGfxTileWordPalOffset(2));
    gUnk_02006040[2] = beamEffectSlot;
    if (beamEffectSlot != -1)
    {
        beamEffect = &gTasks[beamEffectSlot];
        beamEffect->variant = 2;
    }
}

/* MrBrightStopAtTarget (0x080A2DE0-0x080A2E88) */
s32 MrBrightStopAtTarget(void)
{
    s32 ret = 0;
    struct Task *t = gCurTask;

    if (t->velX == 0 && t->velY == 0)
        ret = 1;
    else
    {
        t = gCurTask;
        if (t->facing == 1)
        {
            if (t->pixelX - gViewRect[0] >= t->mrShineAndMrBrightTargetScreenX)
                t->velX = 0;
        }
        else
        {
            if (t->pixelX - gViewRect[0] <= t->mrShineAndMrBrightTargetScreenX)
                t->velX = 0;
        }
        t = gCurTask;
        if (t->velY >= 0)
        {
            if (t->pixelY >= (s32)gUnk_02006040[4])
                t->velY = 0;
        }
        else
        {
            if (t->pixelY <= (s32)gUnk_02006040[4])
                t->velY = 0;
        }
    }
    return ret;
}

/* MrBrightStartSkyAnim (0x080A2E88-0x080A2EDC) */
void MrBrightStartSkyAnim(u8 a)
{
    TaskStop();
    if (a == 1)
        gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_0874859C);
    else if (gCurTask->mrShineAndMrBrightKnockedOut != 0)
    {
        ActorStopAnim();
        TaskSetFrame(20);
    }
    else
        gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_08748588);
}

/* MrBrightStartWaitToAttack (0x080A2EDC-0x080A2F38) */
void MrBrightStartWaitToAttack(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    TaskStop();
    if (gCurTask->mrShineAndMrBrightKnockedOut != 0)
    {
        ActorStopAnim();
        TaskSetFrame(20);
    }
    else
        gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_08748588);
    if (a->prevState != 6)
        gCurTask->mrShineAndMrBrightSkyTimer = 300;
    gUnk_02007D00[7] = 1;
}

/* MrBrightStartWindUp (0x080A2F38-0x080A3000) */
void MrBrightStartWindUp(void)
{
    TaskStop();
    gUnk_02007D00[4] = 1;
    gCurTask->mrShineAndMrBrightFlashing = 1;
    MrBrightStartSkyAnim(1);
    gCurTask->mrShineAndMrBrightTargetSlot = TaskGetNearestPlayerScreenPos();
    if (gUnk_030023B4 <= 15)
        gUnk_030023B4 = 16;
    if (gUnk_030023B4 > 207)
        gUnk_030023B4 = 208;
    gCurTask->mrShineAndMrBrightTargetScreenX = gUnk_030023B4;
    gUnk_02006040[4] = 48;
    switch ((u8)TaskGetXDirBitTo(gCurTask->mrShineAndMrBrightTargetSlot))
    {
    case 4:
        gCurTask->facing = 1;
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        break;
    case 8:
        gCurTask->facing = 255;
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        break;
    }
    if ((s32)gUnk_02006040[4] < gCurTask->pixelY)
        gCurTask->velY = -0x10000;
    else
        gCurTask->velY = 128 << 9;
}

/* MrBrightStartFireBeam (0x080A3000-0x080A301C) */
void MrBrightStartFireBeam(void)
{
    TaskStop();
    gCurTask->mrShineAndMrBrightBeamTimer = 120;
    MrBrightStartSkyAnim(1);
}

/* MrBrightMoveTowardNearestPlayer (0x080A301C-0x080A306C) */
void MrBrightMoveTowardNearestPlayer(void)
{
    switch ((u8)TaskGetXDirBitTo(TaskGetNearestPlayerScreenPos()))
    {
    case 4:
        gCurTask->facing = 1;
        gCurTask->velX = 152 << 7;
        break;
    case 8:
        gCurTask->facing = 255;
        TaskSetMotionXFacing(152 << 7, 0x5A5A5A5A);
        break;
    }
}

/* MrBrightRemoveBeam (0x080A306C-0x080A30D0) */
void MrBrightRemoveBeam(void)
{
    if (gUnk_02006040[0] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[0]);
    if (gUnk_02006040[3] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[3]);
    if (gUnk_02006040[1] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[1]);
    if (gUnk_02006040[2] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[2]);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[3] = -1;
    gUnk_02006040[0] = -1;
    if (gUnk_02006040[6] != -1)
    {
        StopSfxOnPlayer(gUnk_02006040[6], 0x216);
        gUnk_02006040[6] = -1;
    }
}

/* sub_080a30d0 (0x080A30D0-0x080A3114) */
void sub_080a30d0(void)
{
    if (gUnk_02006040[0] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[0]);
    if (gUnk_02006040[1] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[1]);
    if (gUnk_02006040[2] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[2]);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[0] = -1;
}

/* MrShineAndMrBrightInit (0x080A3114-0x080A314C) */
void MrShineAndMrBrightInit(void)
{
    ActorInitBossGfx(0);
    CreateMrShineAndMrBright();
    gCurTask->updateCallback = (u32)MrShineAndMrBrightUpdate;
    ActorSetState(MR_SHINE_AND_MR_BRIGHT_STATE_0);
    CallTableEntry(gCurTask->state, 4, gMrShineAndMrBrightStates);
}

/* MrShineAndMrBrightUpdate (0x080A314C-0x080A3168) */
void MrShineAndMrBrightUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gMrShineAndMrBrightStateUpdates);
}

/* MrShineAndMrBrightEnterState (0x080A3168-0x080A3184) */
void MrShineAndMrBrightEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gMrShineAndMrBrightStates);
}

/* MrShineAndMrBrightIntro (0x080A3184-0x080A31A4) */
void MrShineAndMrBrightIntro(void)
{
    gCurTask->updateState = MR_SHINE_AND_MR_BRIGHT_STATE_0;
    MrShineAndMrBrightFillHpBars();
    ActorSetState(MR_SHINE_AND_MR_BRIGHT_STATE_1);
    TaskSleepForever();
}

/* MrShineAndMrBrightIntroUpdate (0x080A31A4-0x080A31D0) */
void MrShineAndMrBrightIntroUpdate(void)
{
    if (gCurTask->state != MR_SHINE_AND_MR_BRIGHT_STATE_0)
    {
        TaskSetEntry(MrShineAndMrBrightEnterState, gCurTaskIdx);
        MrShineAndMrBrightPickFirstAscender();
    }
}

/* MrShineAndMrBrightWaitForKnockOut (0x080A31D0-0x080A31F0) */
void MrShineAndMrBrightWaitForKnockOut(void)
{
    gCurTask->updateState = MR_SHINE_AND_MR_BRIGHT_STATE_1;
    gCurTask->lateUpdateCallback = (u32)MrShineAndMrBrightWaitForKnockOutLateUpdate;
    TaskSleepForever();
}

/* MrShineAndMrBrightWaitForKnockOutUpdate (0x080A31F0-0x080A31F4) */
void MrShineAndMrBrightWaitForKnockOutUpdate(void)
{
}

/* MrShineAndMrBrightWaitForKnockOutLateUpdate (0x080A31F4-0x080A3238) */
void MrShineAndMrBrightWaitForKnockOutLateUpdate(void)
{
    struct Task *t;

    if (gUnk_02007D00[5] == 2)
        MrShineAndMrBrightStartDefeat();
    else
    {
        t = &gTasks[gUnk_02007D00[2] != 0 ? gUnk_02007D00[1] : gUnk_02007D00[0]];
        gCurTask->health = t->health;
    }
}

/* MrShineAndMrBrightWaitForConvergeStart (0x080A3238-0x080A3250) */
void MrShineAndMrBrightWaitForConvergeStart(void)
{
    gCurTask->updateState = MR_SHINE_AND_MR_BRIGHT_STATE_2;
    TaskSleepForever();
}

/* MrShineAndMrBrightWaitForConvergeStartUpdate (0x080A3250-0x080A3268) */
void MrShineAndMrBrightWaitForConvergeStartUpdate(void)
{
    if (gUnk_02007D00[7] == 2)
        MrShineAndMrBrightStartConvergeEnd();
}

/* MrShineAndMrBrightWaitForConvergeEnd (0x080A3268-0x080A3280) */
void MrShineAndMrBrightWaitForConvergeEnd(void)
{
    gCurTask->updateState = MR_SHINE_AND_MR_BRIGHT_STATE_3;
    TaskSleepForever();
}

/* MrShineAndMrBrightWaitForConvergeEndUpdate (0x080A3280-0x080A3298) */
void MrShineAndMrBrightWaitForConvergeEndUpdate(void)
{
    if (gUnk_02007D00[7] == 2)
        MrShineAndMrBrightEndConverge();
}

/* MrShineInit (0x080A3298-0x080A32C4) */
void MrShineInit(void)
{
    gCurTask->updateCallback = (u32)MrShineUpdate;
    sub_080a27b8();
    CallTableEntry(gCurTask->state, 18, gMrShineStates);
}

/* MrShineUpdate (0x080A32C4-0x080A332C) */
void MrShineUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0 && gCurTask->mrShineAndMrBrightFrozen == 0)
        CallTableEntry(gCurTask->updateState, 18, gMrShineStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        MrShineUpdatePalette();
        if (gUnk_02007D00[6] != 0)
            sub_080a2020();
    }
}

/* MrShineEnterState (0x080A332C-0x080A3348) */
void MrShineEnterState(void)
{
    CallTableEntry(gCurTask->state, 18, gMrShineStates);
}

/* MrShineFall (0x080A3348-0x080A33A4) */
void MrShineFall(void)
{
    gCurTask->updateState = MR_SHINE_STATE_FALL;
    TaskSetFrame(10);
    gCurTask->onGround = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSleepForever();
}

/* MrShineFallUpdate (0x080A33A4-0x080A33DC) */
void MrShineFallUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != MR_SHINE_STATE_FALL)
    {
        t->mrShineAndMrBrightLastMove = 3;
        t->mrShineAndMrBrightKnockedOut = 0;
        t->mrShineAndMrBrightDashTrailSlot = 0xFFFF;
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    }
}

/* MrShineWait (0x080A33DC-0x080A3424) */
void MrShineWait(void)
{
    gCurTask->updateState = MR_SHINE_STATE_WAIT;
    TaskStop();
    MrShineAndMrBrightCheckAscend();
    gCurTask->mrShineAndMrBrightFaceTimer = 8;
    ActorSetAttackBox((u32)gUnk_0874883C);
    TaskSetFrame(10);
    TaskYieldTrampoline((s16)MrShineAndMrBrightGetWaitTime());
    MrShineAndMrBrightPickGroundMove();
    TaskSleepForever();
}

/* MrShineWaitUpdate (0x080A3424-0x080A346C) */
void MrShineWaitUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightFaceTimer <= 0)
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(10);
        gCurTask->mrShineAndMrBrightFaceTimer = 8;
    }
    gCurTask->mrShineAndMrBrightFaceTimer--;
    if (gCurTask->state != MR_SHINE_STATE_WAIT)
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
}

/* MrShineAscend (0x080A346C-0x080A348C) */
void MrShineAscend(void)
{
    gCurTask->updateState = MR_SHINE_STATE_ASCEND;
    TaskSetFrame(13);
    MrShineAndMrBrightStartAscend();
    TaskSleepForever();
}

/* MrShineAscendUpdate (0x080A348C-0x080A349C) */
void MrShineAscendUpdate(void)
{
    MrShineAndMrBrightAscendUpdate(MrShineEnterState);
}

/* MrShineWaitToAttack (0x080A349C-0x080A34B8) */
void MrShineWaitToAttack(void)
{
    gCurTask->updateState = MR_SHINE_STATE_WAIT_TO_ATTACK;
    MrShineStartWaitToAttack();
    TaskSleepForever();
}

/* MrShineWaitToAttackUpdate (0x080A34B8-0x080A34C8) */
void MrShineWaitToAttackUpdate(void)
{
    MrShineAndMrBrightWaitToAttackUpdate(MrShineEnterState);
}

/* MrShineWindUp (0x080A34C8-0x080A34F0) */
void MrShineWindUp(void)
{
    gCurTask->updateState = MR_SHINE_STATE_4;
    MrShineStartWindUp();
    TaskYieldTrampoline(60);
    ActorSetState(MR_SHINE_STATE_DROP_STARS);
    TaskSleepForever();
}

/* MrShineWindUpUpdate (0x080A34F0-0x080A352C) */
void MrShineWindUpUpdate(void)
{
    void *f = MrShineEnterState;

    if ((u8)MrShineAndMrBrightCheckKnockOuts(f) == 0 && gCurTask->state != MR_SHINE_STATE_4)
    {
        MrShineAndMrBrightEndFlash();
        TaskSetEntry(f, gCurTaskIdx);
    }
}

/* MrShineDropStars (0x080A352C-0x080A3548) */
void MrShineDropStars(void)
{
    gCurTask->updateState = MR_SHINE_STATE_DROP_STARS;
    MrShineStartDropStars();
    TaskSleepForever();
}

/* MrShineDropStarsUpdate (0x080A3548-0x080A3588) */
void MrShineDropStarsUpdate(void)
{
    void *f = MrShineEnterState;

    if ((u8)MrShineAndMrBrightCheckKnockOuts(f) == 0)
    {
        gCurTask->mrShineAndMrBrightStarTimer--;
        if (gCurTask->mrShineAndMrBrightStarTimer <= 0)
        {
            if (gCurTask->mrShineAndMrBrightStarCount > 0)
                CreateMrShineFallingStar();
            else
                MrShineAndMrBrightEndSkyAttack(f);
        }
    }
}

/* MrShineChase (0x080A3588-0x080A35A8) */
void MrShineChase(void)
{
    gCurTask->updateState = MR_SHINE_STATE_CHASE;
    TaskStop();
    MrShineSetSkyFrame();
    TaskSleepForever();
}

/* MrShineChaseUpdate (0x080A35A8-0x080A35B8) */
void MrShineChaseUpdate(void)
{
    MrShineAndMrBrightChaseUpdate(MrShineEnterState);
}

/* MrShineWaitToDescend (0x080A35B8-0x080A35D8) */
void MrShineWaitToDescend(void)
{
    gCurTask->updateState = MR_SHINE_STATE_WAIT_TO_DESCEND;
    TaskStop();
    MrShineSetSkyFrame();
    TaskSleepForever();
}

/* MrShineWaitToDescendUpdate (0x080A35D8-0x080A35E8) */
void MrShineWaitToDescendUpdate(void)
{
    MrShineAndMrBrightWaitToDescendUpdate(MrShineEnterState);
}

/* MrShineDescend (0x080A35E8-0x080A361C) */
void MrShineDescend(void)
{
    gCurTask->updateState = MR_SHINE_STATE_DESCEND;
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    MrShineAndMrBrightStartDescend();
    TaskSleepForever();
}

/* MrShineDescendUpdate (0x080A361C-0x080A3628) */
void MrShineDescendUpdate(void)
{
    MrShineAndMrBrightDescendUpdate();
}

/* MrShineWalk (0x080A3628-0x080A368C) */
void MrShineWalk(void)
{
    gCurTask->updateState = MR_SHINE_STATE_WALK;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->mrShineAndMrBrightLoopCount = 0;
    do
    {
        gCurTask->mrShineAndMrBrightStepTimer = 20;
        TaskSetFrame(10);
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->mrShineAndMrBrightLoopCount++;
    } while ((s16)gCurTask->mrShineAndMrBrightLoopCount <= 3);
    TaskStop();
    ActorSetState(MR_SHINE_STATE_WAIT);
    TaskSleepForever();
}

/* MrShineWalkUpdate (0x080A368C-0x080A36C8) */
void MrShineWalkUpdate(void)
{
    if (gCurTask->state != MR_SHINE_STATE_WALK)
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    else
    {
        MrShineUpdateWalkSpeed();
        if ((u8)MrShineAndMrBrightClampToRoom() != 0)
            TaskTurnAroundAndReverseX();
    }
}

/* MrShineJump (0x080A36C8-0x080A3768) */
void MrShineJump(void)
{
    gCurTask->updateState = MR_SHINE_STATE_JUMP;
    TaskStop();
    MrShineAndMrBrightPickJumpDir();
    TaskSetFrame(10);
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    TaskSetFrame(13);
    gCurTask->velX = gCurTask->mrShineAndMrBrightJumpDir << 16;
    TaskSetMotionY(-0x60000, 128 << 7, 192 << 11);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    ActorSetState(MR_SHINE_STATE_WAIT);
    TaskSleepForever();
}

/* MrShineJumpUpdate (0x080A3768-0x080A37AC) */
void MrShineJumpUpdate(void)
{
    if (gCurTask->state != MR_SHINE_STATE_JUMP)
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
    {
        gCurTask->velX = -gCurTask->velX;
        gCurTask->mrShineAndMrBrightJumpDir = -gCurTask->mrShineAndMrBrightJumpDir;
    }
}

/* MrShineJumpBack (0x080A37AC-0x080A3840) */
void MrShineJumpBack(void)
{
    gCurTask->updateState = MR_SHINE_STATE_JUMP_BACK;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->mrShineAndMrBrightFlashing = 1;
    gCurTask->onGround = 0;
    TaskSetFrame(13);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(10);
    ActorSetState(MR_SHINE_STATE_DASH);
    TaskSleepForever();
}

/* MrShineJumpBackUpdate (0x080A3840-0x080A387C) */
void MrShineJumpBackUpdate(void)
{
    if (gCurTask->state != MR_SHINE_STATE_JUMP_BACK)
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
        gCurTask->velX = 0;
}

/* MrShineDash (0x080A387C-0x080A38EC) */
void MrShineDash(void)
{
    gCurTask->updateState = MR_SHINE_STATE_DASH;
    TaskStop();
    PlaySfx(500);
    TaskSetMotionXFacing(144 << 11, 0x5A5A5A5A);
    gCurTask->mrShineAndMrBrightDashTrailSlot = CreateDashFlame(-16, 7);
    gCurTask->spriteFlags &= 0x7FFF;
    if (gCurTask->facing == 1)
        gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_0874853C);
    else
        gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_08748558);
    TaskSleepForever();
}

/* MrShineDashUpdate (0x080A38EC-0x080A3954) */
void MrShineDashUpdate(void)
{
    if ((u8)MrShineAndMrBrightClampToRoom() != 0)
    {
        gCurTask->velX = 0;
        MrShineAndMrBrightEndFlash();
        ActorDestroySlot(gCurTask->mrShineAndMrBrightDashTrailSlot);
        gCurTask->mrShineAndMrBrightDashTrailSlot = 0xFFFF;
        ActorSetState(MR_SHINE_STATE_RECOIL);
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    }
    else
        gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
}

/* MrShineRecoil (0x080A3954-0x080A3A10) */
void MrShineRecoil(void)
{
    gCurTask->updateState = MR_SHINE_STATE_RECOIL;
    TaskStop();
    PlaySfx(0x1F7);
    RequestScreenShake(4);
    ActorSetExtraAttackBox((u32)gMrShineRecoilExtraAttackBox);
    gCurTask->onGround = 0;
    TaskSetFrame(14);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(5);
    TaskSetFrame(10);
    TaskYieldTrampoline(5);
    TaskSetFrame(12);
    TaskYieldTrampoline(5);
    ActorSetState(MR_SHINE_STATE_WAIT);
    TaskSleepForever();
}

/* MrShineRecoilUpdate (0x080A3A10-0x080A3A40) */
void MrShineRecoilUpdate(void)
{
    if (gCurTask->state != MR_SHINE_STATE_RECOIL)
    {
        ActorSetExtraAttackBox((u32)gUnk_08748874);
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    }
}

/* MrShineThrow (0x080A3A40-0x080A3B10) */
void MrShineThrow(void)
{
    gCurTask->updateState = MR_SHINE_STATE_THROW;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(11);
    TaskYieldTrampoline(15);
    TaskSetFrame(15);
    TaskYieldTrampoline(20);
    gCurTask->frame++;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    CreateMrShineCrescent();
    gCurTask->frame++;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(20);
    ActorSetState(MR_SHINE_STATE_WAIT);
    TaskSleepForever();
}

/* MrShineThrowUpdate (0x080A3B10-0x080A3B4C) */
void MrShineThrowUpdate(void)
{
    if (gCurTask->state != MR_SHINE_STATE_THROW)
        TaskSetEntry(MrShineEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
        gCurTask->velX = 0;
}

/* MrShineWaitToConverge (0x080A3B4C-0x080A3B78) */
void MrShineWaitToConverge(void)
{
    gCurTask->updateState = MR_SHINE_STATE_15;
    TaskStop();
    gCurTask->mrShineAndMrBrightFlashing = 1;
    TaskSetFrame(19);
    TaskSleepForever();
}

/* MrShineWaitToConvergeUpdate (0x080A3B78-0x080A3B7C) */
void MrShineWaitToConvergeUpdate(void)
{
}

/* MrShineConvergeStart (0x080A3B7C-0x080A3BA0) */
void MrShineConvergeStart(void)
{
    gCurTask->updateCallback = (u32)MrShineConvergeStartUpdate;
    MrShineAndMrBrightAimAtParent();
    TaskSetFrame(19);
    TaskSleepForever();
}

/* MrShineConvergeStartUpdate (0x080A3BA0-0x080A3BC0) */
void MrShineConvergeStartUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightArrivedAxes != 2)
        MrShineAndMrBrightStopNearParent(8);
    MrShineUpdatePalette();
}

/* MrShineConvergeEnd (0x080A3BC0-0x080A3C08) */
void MrShineConvergeEnd(void)
{
    gCurTask->updateCallback = (u32)MrShineConvergeEndUpdate;
    MrShineAndMrBrightAimAtParent();
    TaskSetFrame(19);
    for (;;)
    {
        TaskYieldTrampoline(4);
        gCurTask->velX = -gCurTask->velX;
        gCurTask->velY = -gCurTask->velY;
        TaskYieldTrampoline(2);
        gCurTask->velX = -gCurTask->velX;
        gCurTask->velY = -gCurTask->velY;
    }
}

/* MrShineConvergeEndUpdate (0x080A3C08-0x080A3C28) */
void MrShineConvergeEndUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightArrivedAxes != 2)
        MrShineAndMrBrightStopNearParent(3);
    MrShineUpdatePalette();
}

/* MrBrightInit (0x080A3C28-0x080A3C54) */
void MrBrightInit(void)
{
    gCurTask->updateCallback = (u32)MrBrightUpdate;
    sub_080a2a44();
    CallTableEntry(gCurTask->state, 18, gMrBrightStates);
}

/* MrBrightUpdate (0x080A3C54-0x080A3CBC) */
void MrBrightUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0 && gCurTask->mrShineAndMrBrightFrozen == 0)
        CallTableEntry(gCurTask->updateState, 18, gMrBrightStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        MrBrightUpdatePalette();
        if (gUnk_02007D00[6] != 0)
            sub_080a2020();
    }
}

/* MrBrightEnterState (0x080A3CBC-0x080A3CD8) */
void MrBrightEnterState(void)
{
    CallTableEntry(gCurTask->state, 18, gMrBrightStates);
}

/* MrBrightFall (0x080A3CD8-0x080A3D3C) */
void MrBrightFall(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_FALL;
    gCurTask->actorAnimDelay1C = ActorStartAnim(gUnk_08748574);
    gCurTask->onGround = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSleepForever();
}

/* MrBrightFallUpdate (0x080A3D3C-0x080A3D84) */
void MrBrightFallUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != MR_BRIGHT_STATE_FALL)
    {
        t->mrShineAndMrBrightLastMove = 3;
        t->mrShineAndMrBrightKnockedOut = 0;
        t->mrShineAndMrBrightDashTrailSlot = 0xFFFF;
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    }
    gCurTask->actorAnimDelay1C = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay1C);
}

/* MrBrightWait (0x080A3D84-0x080A3DD0) */
void MrBrightWait(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_WAIT;
    TaskStop();
    MrShineAndMrBrightCheckAscend();
    ActorSetAttackBox((u32)gUnk_087488AC);
    gCurTask->actorAnimDelay1C = ActorStartAnim(gUnk_08748574);
    gCurTask->mrShineAndMrBrightBobFrame = 48;
    TaskYieldTrampoline((s16)MrShineAndMrBrightGetWaitTime());
    MrShineAndMrBrightPickGroundMove();
    TaskSleepForever();
}

/* MrBrightWaitUpdate (0x080A3DD0-0x080A3E10) */
void MrBrightWaitUpdate(void)
{
    struct Task *t;

    gCurTask->actorAnimDelay1C = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay1C);
    MrBrightUpdateBob();
    t = gCurTask;
    if (t->state != MR_BRIGHT_STATE_WAIT && t->mrShineAndMrBrightBobFrame == 47)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
}

/* MrBrightAscend (0x080A3E10-0x080A3E3C) */
void MrBrightAscend(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_ASCEND;
    MrShineAndMrBrightStartAscend();
    gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightAscendAnim);
    TaskSleepForever();
}

/* MrBrightAscendUpdate (0x080A3E3C-0x080A3E60) */
void MrBrightAscendUpdate(void)
{
    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    MrShineAndMrBrightAscendUpdate(MrBrightEnterState);
}

/* MrBrightWaitToAttack (0x080A3E60-0x080A3E7C) */
void MrBrightWaitToAttack(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_WAIT_TO_ATTACK;
    MrBrightStartWaitToAttack();
    TaskSleepForever();
}

/* MrBrightWaitToAttackUpdate (0x080A3E7C-0x080A3EA0) */
void MrBrightWaitToAttackUpdate(void)
{
    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    MrShineAndMrBrightWaitToAttackUpdate(MrBrightEnterState);
}

/* MrBrightWindUp (0x080A3EA0-0x080A3EDC) */
void MrBrightWindUp(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_4;
    MrBrightStartWindUp();
    while ((u8)MrBrightStopAtTarget() == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(60);
    ActorSetState(MR_BRIGHT_STATE_5);
    TaskSleepForever();
}

/* MrBrightWindUpUpdate (0x080A3EDC-0x080A3F24) */
void MrBrightWindUpUpdate(void)
{
    void *f;

    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    f = MrBrightEnterState;
    if ((u8)MrShineAndMrBrightCheckKnockOuts(f) == 0 && gCurTask->state != MR_BRIGHT_STATE_4)
    {
        MrShineAndMrBrightEndFlash();
        TaskSetEntry(f, gCurTaskIdx);
    }
}

/* MrBrightFireBeam (0x080A3F24-0x080A3F54) */
void MrBrightFireBeam(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_5;
    MrBrightStartFireBeam();
    TaskYieldTrampoline(6);
    CreateMrBrightBeam();
    CreateMrBrightBeamEffects();
    StartBgPaletteBlend(10, 80);
    TaskSleepForever();
}

/* MrBrightFireBeamUpdate (0x080A3F54-0x080A3FB8) */
void MrBrightFireBeamUpdate(void)
{
    void *f;

    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    f = MrBrightEnterState;
    if ((u8)MrShineAndMrBrightCheckKnockOuts(f) == 0)
    {
        if (gCurTask->mrShineAndMrBrightBeamTimer <= 95)
            MrBrightMoveTowardNearestPlayer();
        if (gCurTask->mrShineAndMrBrightBeamTimer <= 0)
        {
            MrShineAndMrBrightEndSkyAttack(f);
            EndBgPaletteBlend(16);
            MrBrightRemoveBeam();
        }
        gCurTask->mrShineAndMrBrightBeamTimer--;
    }
    else
    {
        ResetBgPaletteBlend();
        MrBrightRemoveBeam();
    }
}

/* MrBrightChase (0x080A3FB8-0x080A3FD4) */
void MrBrightChase(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_CHASE;
    MrBrightStartSkyAnim(0);
    TaskSleepForever();
}

/* MrBrightChaseUpdate (0x080A3FD4-0x080A3FF8) */
void MrBrightChaseUpdate(void)
{
    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    MrShineAndMrBrightChaseUpdate(MrBrightEnterState);
}

/* MrBrightWaitToDescend (0x080A3FF8-0x080A4018) */
void MrBrightWaitToDescend(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_WAIT_TO_DESCEND;
    TaskStop();
    MrBrightStartSkyAnim(0);
    TaskSleepForever();
}

/* MrBrightWaitToDescendUpdate (0x080A4018-0x080A403C) */
void MrBrightWaitToDescendUpdate(void)
{
    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    MrShineAndMrBrightWaitToDescendUpdate(MrBrightEnterState);
}

/* MrBrightDescend (0x080A403C-0x080A406C) */
void MrBrightDescend(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_DESCEND;
    TaskStop();
    gCurTask->actorAnimDelay1C = ActorStartAnimNoFlip(gUnk_087485FC);
    MrShineAndMrBrightStartDescend();
    TaskSleepForever();
}

/* MrBrightDescendUpdate (0x080A406C-0x080A408C) */
void MrBrightDescendUpdate(void)
{
    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    MrShineAndMrBrightDescendUpdate();
}

/* MrBrightHop (0x080A408C-0x080A412C) */
void MrBrightHop(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_HOP;
    MrBrightStartJump();
    gCurTask->mrShineAndMrBrightLoopCount = 0;
    do
    {
        gCurTask->onGround = 0;
        TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
        TaskSetMotionY(-0x20000, 128 << 7, 128 << 10);
        gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightHopAnim);
        if (!(gCurTask->onGround & 1))
        {
            while (!(gCurTask->onGround & 1))
                TaskYieldTrampoline(1);
        }
        sub_080a2b14();
        gCurTask->mrShineAndMrBrightLoopCount++;
    } while ((s16)gCurTask->mrShineAndMrBrightLoopCount <= 1);
    TaskStop();
    ActorSetState(MR_BRIGHT_STATE_WAIT);
    TaskSleepForever();
}

/* MrBrightHopUpdate (0x080A412C-0x080A4180) */
void MrBrightHopUpdate(void)
{
    struct Task *t = gCurTask;

    if (!(t->onGround & 1))
        gCurTask->actorAnimDelay1C = ActorTickAnim(t->actorAnimDelay1C);
    if (gCurTask->state != MR_BRIGHT_STATE_HOP)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
        TaskTurnAroundAndReverseX();
}

/* MrBrightJump (0x080A4180-0x080A4200) */
void MrBrightJump(void)
{
    struct Task *t;

    gCurTask->updateState = MR_BRIGHT_STATE_JUMP;
    MrBrightStartJump();
    gCurTask->onGround = 0;
    t = gCurTask;
    t->velX = t->mrShineAndMrBrightJumpDir * 9 << 13;
    TaskSetMotionY(-0x60000, 128 << 7, 192 << 11);
    gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightJumpAnim);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    sub_080a2b14();
    ActorSetState(MR_BRIGHT_STATE_WAIT);
    TaskSleepForever();
}

/* MrBrightJumpUpdate (0x080A4200-0x080A4260) */
void MrBrightJumpUpdate(void)
{
    struct Task *t = gCurTask;

    if (!(t->onGround & 1))
        gCurTask->actorAnimDelay1C = ActorTickAnim(t->actorAnimDelay1C);
    if (gCurTask->state != MR_BRIGHT_STATE_JUMP)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
    {
        gCurTask->velX = -gCurTask->velX;
        gCurTask->mrShineAndMrBrightJumpDir = -gCurTask->mrShineAndMrBrightJumpDir;
    }
}

/* MrBrightJumpBack (0x080A4260-0x080A42F8) */
void MrBrightJumpBack(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_JUMP_BACK;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->mrShineAndMrBrightFlashing = 1;
    ActorStopAnim();
    sub_080a2b14();
    gCurTask->onGround = 0;
    gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightJumpBackAnim);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(MR_BRIGHT_STATE_DASH);
    TaskSleepForever();
}

/* MrBrightJumpBackUpdate (0x080A42F8-0x080A4350) */
void MrBrightJumpBackUpdate(void)
{
    struct Task *t = gCurTask;

    if (!(t->onGround & 1))
        gCurTask->actorAnimDelay1C = ActorTickAnim(t->actorAnimDelay1C);
    if (gCurTask->state != MR_BRIGHT_STATE_JUMP_BACK)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
        gCurTask->velX = 0;
}

/* MrBrightDash (0x080A4350-0x080A43D8) */
void MrBrightDash(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_DASH;
    TaskStop();
    gCurTask->mrShineAndMrBrightDashTrailSlot = CreateDashFireTrail(24, 5);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[0] = -1;
    PlaySfx(500);
    TaskSetMotionXFacing(144 << 11, 0x5A5A5A5A);
    TaskSetFrame(12);
    TaskYieldTrampoline(3);
    gUnk_02006040[0] = CreateDashFireTrail(24, 5);
    TaskYieldTrampoline(3);
    gUnk_02006040[1] = CreateDashFireTrail(24, 5);
    TaskYieldTrampoline(3);
    gUnk_02006040[2] = CreateDashFireTrail(24, 5);
    TaskSleepForever();
}

/* MrBrightDashUpdate (0x080A43D8-0x080A4430) */
void MrBrightDashUpdate(void)
{
    if ((u8)MrShineAndMrBrightClampToRoom() != 0)
    {
        gCurTask->velX = 0;
        MrShineAndMrBrightEndFlash();
        ActorDestroySlot(gCurTask->mrShineAndMrBrightDashTrailSlot);
        gCurTask->mrShineAndMrBrightDashTrailSlot = 0xFFFF;
        sub_080a30d0();
        ActorSetState(MR_BRIGHT_STATE_RECOIL);
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    }
}

/* MrBrightRecoil (0x080A4430-0x080A44C4) */
void MrBrightRecoil(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_RECOIL;
    TaskStop();
    PlaySfx(0x1F7);
    RequestScreenShake(4);
    gCurTask->onGround = 0;
    gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightRecoilAnim);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->onGround & 1))
    {
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(MR_BRIGHT_STATE_WAIT);
    TaskSleepForever();
}

/* MrBrightRecoilUpdate (0x080A44C4-0x080A4508) */
void MrBrightRecoilUpdate(void)
{
    struct Task *t = gCurTask;

    if (!(t->onGround & 1))
        gCurTask->actorAnimDelay1C = ActorTickAnim(t->actorAnimDelay1C);
    if (gCurTask->state != MR_BRIGHT_STATE_RECOIL)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
}

/* MrBrightThrow (0x080A4508-0x080A45BC) */
void MrBrightThrow(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_THROW;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->actorAnimDelay1C = ActorStartAnim(gMrBrightThrowAnim);
    TaskYieldTrampoline(64);
    CreateMrBrightFireball();
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(20);
    ActorSetState(MR_BRIGHT_STATE_WAIT);
    TaskSleepForever();
}

/* MrBrightThrowUpdate (0x080A45BC-0x080A4604) */
void MrBrightThrowUpdate(void)
{
    struct Task *t;

    gCurTask->actorAnimDelay1C = ActorTickAnim(gCurTask->actorAnimDelay1C);
    if (gCurTask->state != MR_BRIGHT_STATE_THROW)
        TaskSetEntry(MrBrightEnterState, gCurTaskIdx);
    else if ((u8)MrShineAndMrBrightClampToRoom() != 0)
        gCurTask->velX = 0;
}

/* MrBrightWaitToConverge (0x080A4604-0x080A4630) */
void MrBrightWaitToConverge(void)
{
    gCurTask->updateState = MR_BRIGHT_STATE_15;
    TaskStop();
    gCurTask->mrShineAndMrBrightFlashing = 1;
    TaskSetFrame(20);
    TaskSleepForever();
}

/* MrBrightWaitToConvergeUpdate (0x080A4630-0x080A4634) */
void MrBrightWaitToConvergeUpdate(void)
{
}

/* MrBrightConvergeStart (0x080A4634-0x080A4658) */
void MrBrightConvergeStart(void)
{
    gCurTask->updateCallback = (u32)MrBrightConvergeStartUpdate;
    MrShineAndMrBrightAimAtParent();
    TaskSetFrame(20);
    TaskSleepForever();
}

/* MrBrightConvergeStartUpdate (0x080A4658-0x080A4678) */
void MrBrightConvergeStartUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightArrivedAxes != 2)
        MrShineAndMrBrightStopNearParent(8);
    MrBrightUpdatePalette();
}

/* MrBrightConvergeEnd (0x080A4678-0x080A46C0) */
void MrBrightConvergeEnd(void)
{
    gCurTask->updateCallback = (u32)MrBrightConvergeEndUpdate;
    MrShineAndMrBrightAimAtParent();
    TaskSetFrame(20);
    for (;;)
    {
        TaskYieldTrampoline(4);
        gCurTask->velX = -gCurTask->velX;
        gCurTask->velY = -gCurTask->velY;
        TaskYieldTrampoline(2);
        gCurTask->velX = -gCurTask->velX;
        gCurTask->velY = -gCurTask->velY;
    }
}

/* MrBrightConvergeEndUpdate (0x080A46C0-0x080A46E0) */
void MrBrightConvergeEndUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightArrivedAxes != 2)
        MrShineAndMrBrightStopNearParent(3);
    MrBrightUpdatePalette();
}

/* MrShineAndMrBrightDefeatHook (0x080A46E0-0x080A4708) */
void MrShineAndMrBrightDefeatHook(void)
{
    ActorDestroySlot(gUnk_02007D00[1]);
    ActorDestroySlot(gUnk_02007D00[0]);
    ActorDestroySlot(gUnk_02006040[5]);
}

/* MrShineAndMrBrightDropStarRodPiece (0x080A4708-0x080A472C) */
void MrShineAndMrBrightDropStarRodPiece(void)
{
    CreateStarRodPiece(0, gCurTask->pixelX, gCurTask->pixelY);
}

/* MrShineAndMrBrightVariant3 (0x080A472C-0x080A4808) */
void MrShineAndMrBrightVariant3(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)sub_080a488c;
    t->updateCallback = (u32)sub_080a4808;
    t->frameTable = gUnk_08754180;
    t->layer = 11;
    for (;;)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
}

/* sub_080a4808 (0x080A4808-0x080A4814) */
void sub_080a4808(void)
{
    sub_080a4814();
}

/* sub_080a4814 (0x080A4814-0x080A488C) */
void sub_080a4814(void)
{
    struct Task *pt = &gTasks[gUnk_02007D00[0]];
    struct Task *t = gCurTask;
    struct Actor *myact = t->u8C.actor;
    struct Actor *pact = gTasks[gUnk_02007D00[0]].u8C.actor;

    switch (pt->frame)
    {
    case 13:
        t->frameTable = gUnk_087541E0;
        break;
    case 12:
        t->frameTable = gUnk_087541B0;
        break;
    default:
        t->frameTable = gUnk_08754180;
        break;
    }
    if (pt->frame > 15)
        gCurTask->frame = 0xFFFF;
    gCurTask->facing = pt->facing;
    myact->paletteOverridden = pact->paletteOverridden;
}

/* sub_080a488c (0x080A488C-0x080A498C) */
void sub_080a488c(void)
{
    struct Task *t;
    struct Actor *act;
    struct TaskGfx *g;
    u16 *src;
    u16 *nx;
    u32 *gt;
    u32 dst;
    u32 x;

    if ((u8)ActorIsInNearView() != 0 && TaskIsOnScreen() != 0)
    {
        t = gCurTask;
        x = t->tileWord;
        x &= 0x7FF;
        dst = (x << 5) + (BG_VRAM + 0xFE00);
        act = t->u8C.actor;
        gt = t->frameTable;
        if (t->frame != -1)
        {
            g = (struct TaskGfx *)gt[t->frame];
            src = g->tiles;
            while (src[0] != 0xFFFF)
            {
                nx = src + 1;
                RequestCopy(4, (u32)nx, dst, src[0]);
                src = (u16 *)((u32)nx + src[0]);
                dst += 128 << 3;
            }
            QueueSprite(gCurTask->layer, (u32)g->oamTemplate, gCurTask->spriteFlags,
                         gCurTask->tileWord | (128 << 4),
                         gCurTask->pixelX - gSpriteCameraX,
                         (s16)(gCurTask->pixelY - gSpriteCameraY));
        }
        else
            g = (struct TaskGfx *)gt[0];
        if (!(act->paletteOverridden & 1))
            ActorLoadPalette((void *)((u32)g->palette + 2), g->palette[0], 0);
    }
}

/* Task_KingDededeStar (0x080A498C-0x080A49CC) */
void Task_KingDededeStar(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    gCurTask->frameTable = gUnk_0874C44C;
    CallTableEntry(gCurTask->variant, 1, gKingDededeStarVariants);
}

/* KingDededeStarReleasePlayer (0x080A49CC-0x080A4A1C) */
void KingDededeStarReleasePlayer(void)
{
    struct Task *t;

    gUnk_02007D00[8] = -1;
    SetHeldPlayerState(gUnk_02007D00[1], 2);
    t = gCurTask;
    sub_08068950(t->pixelX, t->pixelY, -t->facing);
    RequestScreenShake(4);
    gCurTask->kingDededeStarReleased = 1;
}

/* KingDededeStarInit (0x080A4A1C-0x080A4A60) */
void KingDededeStarInit(void)
{
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->updateCallback = (u32)KingDededeStarUpdate;
    TaskFaceLikeParent();
    ActorSetAttackBox((u32)gKingDededeStarInitAttackBox);
    ActorSetState(KING_DEDEDE_STAR_STATE_0);
    CallTableEntry(gCurTask->state, 1, gKingDededeStarStates);
}

/* KingDededeStarUpdate (0x080A4A60-0x080A4AA8) */
void KingDededeStarUpdate(void)
{
    TaskStepSpinFrameFacing();
    CallTableEntry(gCurTask->updateState, 1, gKingDededeStarStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* KingDededeStarEnterState (0x080A4AA8-0x080A4AC4) */
void KingDededeStarEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gKingDededeStarStates);
}

/* KingDededeStarFlight (0x080A4AC4-0x080A4B1C) */
void KingDededeStarFlight(void)
{
    struct Task *t;

    gCurTask->updateState = KING_DEDEDE_STAR_STATE_0;
    gCurTask->onGround = 0;
    t = gCurTask;
    t->actorSpinFrameTimer = 2;
    t->kingDededeStarFirstUpdate = 1;
    t->frame = 4;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    gCurTask->kingDededeStarReleased = 0;
    while (gCurTask->kingDededeStarReleased == 0)
        TaskYieldTrampoline(1);
    ActorSetHitReactions((u32)gUnk_08748D04);
    ActorDie();
}

/* KingDededeStarFlightUpdate (0x080A4B1C-0x080A4B68) */
void KingDededeStarFlightUpdate(void)
{
    u8 v;

    if (gCurTask->kingDededeStarFirstUpdate != 0)
    {
        SetHeldPlayerState(gUnk_02007D00[1], 1);
        gCurTask->kingDededeStarFirstUpdate = 0;
    }
    else
    {
        v = ClampTaskToRoom(gCurTask);
        if ((v & 1) || (v & 2))
            KingDededeStarReleasePlayer();
    }
}

/* Task_KingDededeAirPuff (0x080A4B68-0x080A4BA8) */
void Task_KingDededeAirPuff(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    gCurTask->frameTable = gKingDededeAirPuffFrames;
    CallTableEntry(gCurTask->variant, 1, gKingDededeAirPuffVariants);
}

/* KingDededeAirPuffInit (0x080A4BA8-0x080A4BDC) */
void KingDededeAirPuffInit(void)
{
    gCurTask->updateCallback = (u32)KingDededeAirPuffUpdate;
    TaskFaceLikeParent();
    ActorSetState(KING_DEDEDE_AIR_PUFF_STATE_0);
    CallTableEntry(gCurTask->state, 1, gKingDededeAirPuffStates);
}

/* KingDededeAirPuffUpdate (0x080A4BDC-0x080A4C20) */
void KingDededeAirPuffUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gKingDededeAirPuffStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* KingDededeAirPuffEnterState (0x080A4C20-0x080A4C3C) */
void KingDededeAirPuffEnterState(void)
{
    CallTableEntry(gCurTask->state, 1, gKingDededeAirPuffStates);
}

/* KingDededeAirPuffFlight (0x080A4C3C-0x080A4C80) */
void KingDededeAirPuffFlight(void)
{
    s32 i;
    u32 *p;

    gCurTask->updateState = KING_DEDEDE_AIR_PUFF_STATE_0;
    gCurTask->onGround = 0;
    TaskSetFrame(0);
    p = gUnk_087489C0;
    for (i = 4; i >= 0; i--)
    {
        TaskSetMotionXFacing(*p++, 0x5A5A5A5A);
        TaskYieldTrampoline(6);
    }
    ActorDestroy();
}

/* KingDededeAirPuffFlightUpdate (0x080A4C80-0x080A4C84) */
void KingDededeAirPuffFlightUpdate(void)
{
}

/* Task_MrShineAndMrBrightAttack (0x080A4C84-0x080A4CC4) */
void Task_MrShineAndMrBrightAttack(void)
{
    struct Task *t = gCurTask;
    struct Actor *a = t->u8C.actor;

    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    a->sfxOverride = -2;
    CallTableEntry(gCurTask->variant, 4, gMrShineAndMrBrightAttackVariants);
}

/* MrShineCrescentInit (0x080A4CC4-0x080A4D00) */
void MrShineCrescentInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)MrShineCrescentUpdate;
    t->frameTable = gMrShineCrescentFrames;
    TaskFaceLikeParent();
    ActorSetState(MR_SHINE_CRESCENT_STATE_0);
    CallTableEntry(gCurTask->state, 2, gMrShineCrescentStates);
}

/* MrShineCrescentUpdate (0x080A4D00-0x080A4D6C) */
void MrShineCrescentUpdate(void)
{
    if ((u8)ActorCollideTerrainWalls() != 0)
    {
        ActorSetState(MR_SHINE_CRESCENT_STATE_1);
        TaskSetEntry(MrShineCrescentEnterState, gCurTaskIdx);
    }
    else
        CallTableEntry(gCurTask->updateState, 2, gMrShineCrescentStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* MrShineCrescentEnterState (0x080A4D6C-0x080A4D88) */
void MrShineCrescentEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMrShineCrescentStates);
}

/* MrShineCrescentFlight (0x080A4D88-0x080A4DD8) */
void MrShineCrescentFlight(void)
{
    gCurTask->updateState = MR_SHINE_CRESCENT_STATE_0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    if (gCurTask->facing == 1)
        gCurTask->actorAnimDelay = ActorStartAnimNoFlip(gUnk_08748A14);
    else
        gCurTask->actorAnimDelay = ActorStartAnimNoFlip(gUnk_08748A00);
    TaskSleepForever();
}

/* MrShineCrescentFlightUpdate (0x080A4DD8-0x080A4DF4) */
void MrShineCrescentFlightUpdate(void)
{
    gCurTask->actorAnimDelay = ActorTickAnim(gCurTask->actorAnimDelay);
}

/* MrShineCrescentDie (0x080A4DF4-0x080A4E10) */
void MrShineCrescentDie(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a4e10 (0x080A4E10-0x080A4E14) */
void sub_080a4e10(void)
{
}

/* MrShineFallingStarSetStart (0x080A4E14-0x080A4E9C) */
void MrShineFallingStarSetStart(void)
{
    s32 r;
    s32 m;

    m = RandomRange(32) + 16;
    gCurTask->posY = ((s16)m + gViewRect[2]) << 16;
    gCurTask->posX = (gUnk_08748A38[gCurTask->actorSpawnArg & 3] + gViewRect[0]) << 16;
    r = RandomRange(3);
    gCurTask->mrShineAndMrBrightAttackStartVelY = ((gCurTask->actorSpawnArg & 1) + 3) << 16;
    if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
        r += 2;
    gCurTask->mrShineAndMrBrightAttackStartVelX = gUnk_08748A40[r];
}

/* MrShineFallingStarInit (0x080A4E9C-0x080A4EE0) */
void MrShineFallingStarInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)MrShineFallingStarUpdate;
    t->frameTable = gMrShineFallingStarFrames;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B34);
    ActorSetState(MR_SHINE_FALLING_STAR_STATE_0);
    CallTableEntry(gCurTask->state, 2, gMrShineFallingStarStates);
}

/* MrShineFallingStarUpdate (0x080A4EE0-0x080A4F24) */
void MrShineFallingStarUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gMrShineFallingStarStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* MrShineFallingStarEnterState (0x080A4F24-0x080A4F40) */
void MrShineFallingStarEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMrShineFallingStarStates);
}

/* MrShineFallingStarFlight (0x080A4F40-0x080A5008) */
void MrShineFallingStarFlight(void)
{
    struct Task *t;

    gCurTask->updateState = MR_SHINE_FALLING_STAR_STATE_0;
    TaskStop();
    gCurTask->onGround = 0;
    gCurTask->mrShineAndMrBrightAttackSpinning = 0;
    MrShineFallingStarSetStart();
    gCurTask->mrShineAndMrBrightAttackLoopCount = 0;
    do
    {
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->mrShineAndMrBrightAttackLoopCount++;
    } while ((s16)gCurTask->mrShineAndMrBrightAttackLoopCount <= 1);
    gCurTask->tileWord = 0;
    gCurTask->velX = gCurTask->mrShineAndMrBrightAttackStartVelX;
    gCurTask->velY = gCurTask->mrShineAndMrBrightAttackStartVelY;
    switch ((s32)RandomRange(3))
    {
    case 0:
        PlaySfx(0x213);
        break;
    case 1:
        PlaySfx(133 << 2);
        break;
    case 2:
        PlaySfx(0x215);
        break;
    }
    t = gCurTask;
    t->frameTable = gUnk_0874C44C;
    t->frame = 4;
    t->mrShineAndMrBrightAttackSpinning = 1;
    t->actorSpinFrameTimer = 2;
    TaskSleepForever();
}

/* MrShineFallingStarFlightUpdate (0x080A5008-0x080A5020) */
void MrShineFallingStarFlightUpdate(void)
{
    if (gCurTask->mrShineAndMrBrightAttackSpinning != 0)
        TaskStepSpinFrameFacing();
}

/* MrShineFallingStarDie (0x080A5020-0x080A503C) */
void MrShineFallingStarDie(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a503c (0x080A503C-0x080A5040) */
void sub_080a503c(void)
{
}

/* MrBrightFireballInit (0x080A5040-0x080A5084) */
void MrBrightFireballInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)MrBrightFireballUpdate;
    t->frameTable = gMrBrightFireballFrames;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B60);
    ActorSetState(MR_BRIGHT_FIREBALL_STATE_0);
    CallTableEntry(gCurTask->state, 2, gMrBrightFireballStates);
}

/* MrBrightFireballUpdate (0x080A5084-0x080A50F0) */
void MrBrightFireballUpdate(void)
{
    if ((u8)ActorCollideTerrainWalls() != 0)
    {
        ActorSetState(MR_BRIGHT_FIREBALL_STATE_1);
        TaskSetEntry(MrBrightFireballEnterState, gCurTaskIdx);
    }
    else
        CallTableEntry(gCurTask->updateState, 2, gMrBrightFireballStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* MrBrightFireballEnterState (0x080A50F0-0x080A510C) */
void MrBrightFireballEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMrBrightFireballStates);
}

/* MrBrightFireballFlight (0x080A510C-0x080A5164) */
void MrBrightFireballFlight(void)
{
    gCurTask->updateState = MR_BRIGHT_FIREBALL_STATE_0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(2);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
}

/* MrBrightFireballFlightUpdate (0x080A5164-0x080A5168) */
void MrBrightFireballFlightUpdate(void)
{
}

/* MrBrightFireballDie (0x080A5168-0x080A5184) */
void MrBrightFireballDie(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a5184 (0x080A5184-0x080A5188) */
void sub_080a5184(void)
{
}

/* MrBrightBeamInit (0x080A5188-0x080A51DC) */
void MrBrightBeamInit(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->layer = 12;
    t = gCurTask;
    t->updateCallback = (u32)MrBrightBeamUpdate;
    t->frameTable = gUnk_08754290;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B8C);
    ActorSetState(MR_BRIGHT_BEAM_STATE_0);
    CallTableEntry(gCurTask->state, 2, gMrBrightBeamStates);
}

/* MrBrightBeamUpdate (0x080A51DC-0x080A5220) */
void MrBrightBeamUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gMrBrightBeamStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* MrBrightBeamEnterState (0x080A5220-0x080A523C) */
void MrBrightBeamEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gMrBrightBeamStates);
}

/* MrBrightBeamDropStars (0x080A523C-0x080A528C) */
void MrBrightBeamDropStars(void)
{
    struct Task *t;
    s16 x;
    s16 y;

    t = gCurTask;
    x = t->pixelX + 16;
    y = t->pixelY + 32;
    CreateInhalableStar(x, y, 1, 1);
    t = gCurTask;
    x = t->pixelX - 16;
    y = t->pixelY + 32;
    CreateInhalableStar(x, y, -1, 1);
}

/* MrBrightBeamState0 (0x080A528C-0x080A52C8) */
void MrBrightBeamState0(void)
{
    gCurTask->updateState = MR_BRIGHT_BEAM_STATE_0;
    TaskStop();
    gCurTask->onGround = 0;
    gCurTask->mrShineAndMrBrightAttackFrameTimer = 0;
    gCurTask->mrShineAndMrBrightAttackBeamFrame = 0;
    TaskYieldTrampoline(2);
    MrBrightBeamDropStars();
    TaskYieldTrampoline(84);
    MrBrightBeamDropStars();
    TaskSleepForever();
}

/* MrBrightBeamState0Update (0x080A52C8-0x080A5304) */
void MrBrightBeamState0Update(void)
{
    struct Task *t = gCurTask;

    if (t->mrShineAndMrBrightAttackFrameTimer <= 0)
    {
        if (t->mrShineAndMrBrightAttackBeamFrame != 0)
            t->frame = 3;
        else
            t->frame = 2;
        gCurTask->mrShineAndMrBrightAttackFrameTimer = 2;
        gCurTask->mrShineAndMrBrightAttackBeamFrame ^= 1;
    }
    gCurTask->mrShineAndMrBrightAttackFrameTimer--;
}

/* MrBrightBeamState1 (0x080A5304-0x080A5320) */
void MrBrightBeamState1(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    ActorDestroy();
}

/* sub_080a5320 (0x080A5320-0x080A5324) */
void sub_080a5320(void)
{
}

/* Task_KingDededeLandingStar (0x080A5324-0x080A5388) */
void Task_KingDededeLandingStar(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    t = gCurTask;
    t->frameTable = gUnk_0874C500;
    TaskFaceLikeParent();
    gCurTask->frame = 0;
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

/* Task_KingDededeHammerHitBox (0x080A5388-0x080A53A8) */
void Task_KingDededeHammerHitBox(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)KingDededeHammerHitBoxUpdate;
    TaskSleepForever();
}

/* KingDededeHammerHitBoxUpdate (0x080A53A8-0x080A5484) */
void KingDededeHammerHitBoxUpdate(void)
{
    vs16 *arr;
    s16 i;
    s16 dx;
    s16 dy;
    u32 *tbl;
    s32 ok;

    arr = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)arr[i] != -1) {
        if ((u8)TaskHasSameSerial(i) == 1) {
            struct Task *o = &gTasks[gCurTask->parent];

            switch (o->frame) {
            case 30:
            case 31:
            case 32:
            case 33:
                dx = -20;
                dy = -30;
                ok = 1;
                tbl = gUnk_08748C0C;
                break;
            case 35:
                dx = 40;
                dy = 8;
                ok = 1;
                tbl = gUnk_08748C28;
                break;
            case 34:
                dx = 40;
                dy = 16;
                ok = 1;
                tbl = gUnk_08748C0C;
                break;
            default:
                ok = 0;
                break;
            }
            if (ok != 0) {
                gCurTask->pixelX = o->pixelX + dx * o->facing;
                gCurTask->pixelY = dy + o->pixelY;
                ActorCheckHitsWithBox((s32)tbl);
            }
        }
    }
}

/* Task_KingDededeInhaleHitBox (0x080A5484-0x080A54A4) */
void Task_KingDededeInhaleHitBox(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)KingDededeInhaleHitBoxUpdate;
    TaskSleepForever();
}

/* KingDededeInhaleHitBoxUpdate (0x080A54A4-0x080A54E4) */
void KingDededeInhaleHitBoxUpdate(void)
{
    struct Task **c = &gCurTask;

    if (ActorCheckHitsWithBox((s32)gUnk_08748C44) != 0)
    {
        gUnk_02007D00[8] = (*c)->hitterSlot;
        HoldPlayer((*c)->hitterSlot, (*c)->parent, 0);
    }
}

/* Task_MrBrightBeamEffect (0x080A54E4-0x080A5524) */
void Task_MrBrightBeamEffect(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    gCurTask->frameTable = gUnk_08754290;
    CallTableEntry(gCurTask->variant, 3, gMrBrightBeamEffectVariants);
}

/* MrBrightBeamEffectVariant0 (0x080A5524-0x080A556C) */
void MrBrightBeamEffectVariant0(void)
{
    struct Task **c;
    u16 z;
    u16 x;

    TaskStop();
    c = &gCurTask;
    z = 0;
    for (;;) {
        (*c)->frame = 1;
        TaskYieldTrampoline(1);
        x = (*c)->frame;
        (*c)->frame = x | 0xFFFF;
        TaskYieldTrampoline(2);
        (*c)->frame = z;
        TaskYieldTrampoline(1);
        x = (*c)->frame;
        (*c)->frame = x | 0xFFFF;
        TaskYieldTrampoline(3);
    }
}

/* MrBrightBeamEffectVariant1 (0x080A556C-0x080A55AC) */
void MrBrightBeamEffectVariant1(void)
{
    struct Task **c;
    u32 *base;
    u32 *p;
    s32 v;
    s32 i;

    v = gCurTask->posY;
    gCurTask->frame = 4;
    gCurTask->velY = 128 << 12;
    c = &gCurTask;
    base = gUnk_08748A80;
    for (;;)
    {
        p = base;
        for (i = 4; i >= 0; i--)
        {
            (*c)->posY = v;
            (*c)->posX = *p++;
            TaskYieldTrampoline(10);
        }
    }
}

/* MrBrightBeamEffectVariant2 (0x080A55AC-0x080A55EC) */
void MrBrightBeamEffectVariant2(void)
{
    struct Task **c;
    u32 *base;
    u32 *p;
    s32 v;
    s32 i;

    v = gCurTask->posY;
    gCurTask->frame = 5;
    gCurTask->velY = 128 << 13;
    c = &gCurTask;
    base = gUnk_08748A94;
    for (;;)
    {
        p = base;
        for (i = 6; i >= 0; i--)
        {
            (*c)->posY = v;
            (*c)->posX = *p++;
            TaskYieldTrampoline(5);
        }
    }
}

/* Task_MetaKnight (0x080A55EC-0x080A5644) */
void Task_MetaKnight(void)
{
    struct Task *t;

    ActorInitBossGfx(0);
    sub_08066144();
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearViewOrDestroy;
    t->layer = 11;
    gCurTask->frameTable = gMetaKnightFrames;
    ActorSetExtraAttackBox((u32)gMetaKnightExtraAttackBox);
    CallTableEntry(gCurTask->variant, 1, gMetaKnightVariants);
}
