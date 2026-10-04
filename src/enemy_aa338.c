#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "collision.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"
#include "constants/game_states.h"

/* RAM cells / ROM tables */
/* Not from room.h: this file's view of gCurSaveSlot differs (lesson 3.517). */
extern s16 gMaxHealth;
extern s16 gPlayerHealth[];
extern s8 gUnk_02005590[];
extern struct Unk020055D8 gRoomObjectList;
extern u8 gUnk_02005E10[];
extern u8 gRoomEntryMode;
extern u32 gUsedRoomObjects[8][8];
extern s16 gPlayerLives[];
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern s16 gUnk_0200AF0C;
extern u8 gWarpStarStationLevels;
extern u8 gUnk_0200B078;
extern u8 gMidBossFightState;
extern s16 gCameraAnchorY;
extern s32 gUnk_03001F2C;
extern u8 gMetaKnightmareMode;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern u8 gActivePlayerCount;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern u32 gBigSwitchFlags[];
extern u16 gGameState;
extern u32 gCurSaveSlot[];
extern s8 gStageIndex;
extern s32 gUnk_03002448;
extern u8 gExtraMode;
extern s8 gRoomIndex;
extern s16 gRoomBounds[];
extern struct Unk03005680 gScrollLock;

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
extern void ExitClearedStage();
extern void ExitKingDededeStage();
extern void ExitToNextRoom();
extern void ExitToEnding();
extern void RequestScreenShake(u32 a);
extern void sub_080275cc();
extern void StartScrollLock();
extern s32 CreateMapEvent();
extern void CreateWarpStarStationNumber();
extern void TaskBreakBlocksNoPlayer();
extern void TaskBreakTopBlockRow();
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
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

void NightmareWizardInit(void)
{
    gCurTask->updateCallback = (u32)NightmareWizardUpdate;
    gCurTask->nightmareWizardSpotIndex = -1;
    gCurTask->nightmareWizardWaitIndex = 0;
    gCurTask->nightmareWizardOpenCloakCount = 0;
    gCurTask->nightmareWizardVanishCount = 0;
    gCurTask->nightmareWizardMaxHealth = (s16)ActorComputeHealth();
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 12, gNightmareWizardStates);
}

void NightmareWizardEnterState(void)
{
    CallTableEntry(gCurTask->state, 12, gNightmareWizardStates);
}

void NightmareWizardUpdate(void)
{
    s32 n;

    CallTableEntry(gCurTask->updateState, 12, gNightmareWizardStateUpdates);
    if ((u16)gCurTask->frame <= 35)
    {
        gUnk_03001F2C = gUnk_08749380[gCurTask->frame];
        ActorSetAttackBox(gUnk_08749358[gUnk_03001F2C]);
        sub_08063a00(gUnk_0874936C[gUnk_03001F2C]);
        ActorCheckHitsWithExtraBox();
    }
    else
    {
        n = gUnk_02007D00[0] & 1;
        if (n != 0)
        {
            if (gUnk_02007D00[1] == 0)
            {
                ActorSetAttackBox((u32)gUnk_08749870);
                sub_080639f0((struct ActorAux *)gUnk_08749AF8);
            }
            else
            {
                ActorSetAttackBox((u32)gUnk_0874988C);
                sub_080639f0((struct ActorAux *)gUnk_08749B00);
            }
            ActorCheckHits();
        }
        else
        {
            gCurTask->hitKind = n;
            gCurTask->hitTimer = n;
            gCurTask->hitterSlot = 255;
            gCurTask->hitterPlayer = -1;
        }
    }
    ActorReactToHit();
}

void NightmareWizardState0(void)
{
    gCurTask->updateState = 0;
    ActorStopAnim();
    NightmareWizardMoveToNextSpot();
    TaskStop();
    NightmareWizardAppear(1);
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    sub_08066544();
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(15);
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 7;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 7;
        TaskYieldTrampoline(15);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void NightmareWizardState0Update(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 0)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardState1(void)
{
    s32 v;
    struct Task *t;
    s32 r1v;
    s32 r2v;

    gCurTask->updateState = 1;
    r1v = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    t = gCurTask;
    t->actorAnimDelay18 = r1v;
    t->nightmareWizardSpotAttack = gUnk_08749224[t->nightmareWizardSpotIndex];
    if (t->nightmareWizardSpotAttack == 0)
        goto is0;
    if (t->nightmareWizardSpotAttack == 3)
    {
        v = 18;
        goto setv;
    }
    goto els;
is0:
    v = 60;
    goto setv;
els:
    r2v = RandomRange(3);
    t = gCurTask;
    t->nightmareWizardWaitIndex = (t->nightmareWizardWaitIndex + r2v + 1) & 3;
    v = gUnk_08749220[t->nightmareWizardWaitIndex];
setv:
    t->nightmareWizardAttackTimer = v;
    if (gCurTask->nightmareWizardSpotAttack != 4)
    {
        for (;;)
        {
            gCurTask->velY = -0x8000;
            TaskYieldTrampoline(10);
            gCurTask->velY = -0x10000;
            TaskYieldTrampoline(10);
            gCurTask->velY = -0x8000;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 9;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(10);
        }
    }
    TaskSleepForever();
}

void NightmareWizardState1Update(void)
{
    s32 w;

    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    w = gCurTask->nightmareWizardAttackTimer - 1;
    gCurTask->nightmareWizardAttackTimer = w;
    if (w == 0)
    {
        TaskStop();
        ActorSetState(gUnk_08749230[gCurTask->nightmareWizardSpotAttack][0]);
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
    }
}

void NightmareWizardState2(void)
{
    gCurTask->updateState = 2;
    NightmareWizardMoveToNextSpot();
    TaskStop();
    NightmareWizardAppear(1);
    ActorSetState(1);
    TaskSleepForever();
}

void NightmareWizardState2Update(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardState3(void)
{
    s32 n;

    gCurTask->updateState = 3;
    TaskStop();
    NightmareWizardVanish(1);
    n = gCurTask->nightmareWizardVanishCount + 1;
    gCurTask->nightmareWizardVanishCount = n;
    if ((n & 3) != 0)
    {
        TaskYieldTrampoline(RandomRange(31) + 30);
        ActorSetState(2);
    }
    else
        ActorSetState(10);
    TaskSleepForever();
}

void NightmareWizardState3Update(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardState4(void)
{
    struct Task **c;
    struct Task *u;
    s32 *pb;
    s32 w;

    gCurTask->updateState = 4;
    TaskGetNearestPlayerScreenPos();
    if (gUnk_030023B4 > 128)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    TaskSetFrame(36);
    gCurTask->nightmareWizardFrameTimer = 2;
    c = &gCurTask;
    pb = &gUnk_030023B4;
top:
    u = *c;
    w = u->nightmareWizardFrameTimer - 1;
    u->nightmareWizardFrameTimer = w;
    if (w == 0)
    {
        u->frame++;
        if ((s16)u->frame > 39)
            u->frame = 36;
        (*c)->nightmareWizardFrameTimer = 2;
    }
    TaskGetScreenPos();
    if (((*c)->facing == 1 && *pb <= 31)
        || ((*c)->facing == -1 && *pb > 208))
        goto out;
    TaskYieldTrampoline(1);
    goto top;
out:
    if (abs(TaskGetNearestPlayerDx()) <= 111)
    {
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        gCurTask->nightmareWizardLoopCount = 0;
        do
        {
            sub_080ab7cc();
            gCurTask->nightmareWizardLoopCount++;
        } while ((s16)gCurTask->nightmareWizardLoopCount <= 1);
        gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
        TaskSetMotionXFacing(176 << 11, 0x5A5A5A5A);
        gCurTask->nightmareWizardLoopCount = 0;
        do
        {
            gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
            TaskYieldTrampoline(1);
            gCurTask->nightmareWizardLoopCount++;
        } while ((s16)gCurTask->nightmareWizardLoopCount <= 23);
        TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
        TaskSetFrame(56);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->facing *= -1;
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        sub_080ab7cc();
        TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    }
    else
    {
        TaskStop();
        gCurTask->facing *= -1;
        TaskSetFrame(56);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->facing *= -1;
        sub_080ab7cc();
    }
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        sub_080ab810();
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 5);
    TaskStop();
    if (gUnk_08749224[gCurTask->nightmareWizardSpotIndex] == 2)
        ActorSetState(7);
    else
        ActorSetState(6);
    TaskSleepForever();
}

void NightmareWizardState4Update(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardPoint(void)
{
    struct ActorSpawn sp;
    s32 d;

    gCurTask->updateState = 7;
    gUnk_02007D00[7] = 0;
    ActorStopAnim();
    TaskSetFrame(52);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    CreateChildTask(TASK_NIGHTMARE_WIZARD_POINTING_HAND, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->u8C.actor->savedTileWord | (128 << 4));
    CreateChildTask(TASK_NIGHTMARE_WIZARD_POINT_TORNADO, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    PlaySfx(0x235);
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_087492C0);
    if (gUnk_02007D00[7] == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gUnk_02007D00[7] == 0);
    }
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_087492D4);
    PlaySfx(140 << 2);
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        sp.subtype = 33;
        sp.taskType = TASK_NIGHTMARE_WIZARD_STAR;
        sp.spawnArg = 0;
        sp.x = 48;
        sp.y = 16;
        sp.tileWord = gCurTask->u8C.actor->savedTileWord;
        sp.checkTerrain = 1;
        d = (s16)gCurTask->health;
        if (d <= Div(gCurTask->nightmareWizardMaxHealth, 3) && (gCurTask->nightmareWizardLoopCount & 1))
        {
            sp.variant = 1;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
            sp.variant = 2;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
        }
        else
        {
            sp.variant = 0;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
        }
        TaskYieldTrampoline(8);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 5);
    TaskYieldTrampoline(31);
    gUnk_02007D00[7] = 2;
    ActorStopAnim();
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        sub_080ab810();
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardPointUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 7)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardOpenPalm(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 6;
    ActorStopAnim();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(36);
    gCurTask->nightmareWizardFrameTimer = 2;
    while (1)
    {
        if (--gCurTask->nightmareWizardFrameTimer == 0)
        {
            if ((s16)++gCurTask->frame > 39)
                gCurTask->frame = 36;
            gCurTask->nightmareWizardFrameTimer = 2;
        }
        TaskGetScreenPos();
        if (gUnk_030023D4 > 64)
            break;
        if (TaskGetYDirBitToNearestPlayer() != 1)
            break;
        if (abs(TaskGetNearestPlayerDy()) <= 7)
            break;
        TaskYieldTrampoline(1);
    }
    TaskStop();
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_087492AC);
    gUnk_03001F2C = (s16)gCurTask->health > Div(gCurTask->nightmareWizardMaxHealth, 3);
    gUnk_02007D00[6] = gUnk_0874921C[gUnk_03001F2C];
    gUnk_02007D00[7] = gUnk_0874921E[gUnk_03001F2C];
    gUnk_02007D00[5] = 0;
    CreateChildTask(TASK_NIGHTMARE_WIZARD_PALM, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->u8C.actor->savedTileWord | 0x800);
    CreateChildTask(TASK_NIGHTMARE_WIZARD_PALM_TORNADO, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    gCurTask->nightmareWizardPalmStarDir = 0;
    for (gCurTask->nightmareWizardLoopCount = 0; (s16)gCurTask->nightmareWizardLoopCount < gUnk_02007D00[7]; gCurTask->nightmareWizardLoopCount++)
    {
        TaskYieldTrampoline(gUnk_02007D00[6]);
        gCurTask->nightmareWizardPalmStarDir += RandomRange(4) + 1;
        if (gCurTask->nightmareWizardPalmStarDir > 4)
            gCurTask->nightmareWizardPalmStarDir -= 5;
        sp.subtype = 33;
        sp.taskType = TASK_NIGHTMARE_WIZARD_STAR;
        sp.variant = 3;
        sp.x = 0;
        sp.y = 8;
        sp.tileWord = gCurTask->u8C.actor->savedTileWord;
        sp.checkTerrain = 1;
        sp.spawnArg = gCurTask->nightmareWizardPalmStarDir;
        CreateActorFromDescAtOffsetFacing(&sp, 1);
    }
    TaskYieldTrampoline(4);
    ActorStopAnim();
    gUnk_02007D00[5] = 1;
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        sub_080ab810();
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardOpenPalmUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 6)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardOpenCloak(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 5;
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    gCurTask->velY = 0;
    gCurTask->accelY = -0x2000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 128 << 6;
    TaskYieldTrampoline(16);
    TaskStopY();
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749298);
    CreateChildTask(TASK_NIGHTMARE_WIZARD_PENDANT, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->u8C.actor->savedTileWord | (128 << 4));
    CreateChildTask(TASK_NIGHTMARE_WIZARD_CLOAK_HANDS, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->u8C.actor->savedTileWord | (128 << 4));
    CreateChildTask(TASK_NIGHTMARE_WIZARD_CLOAK_TORNADO, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    sp.subtype = 33;
    sp.taskType = TASK_NIGHTMARE_WIZARD_STAR;
    sp.variant = 4;
    sp.tileWord = gCurTask->u8C.actor->savedTileWord;
    sp.checkTerrain = 1;
    if (gCurTask->nightmareWizardOpenCloakCount & 1)
    {
        TaskYieldTrampoline(20);
        sp.spawnArg = 0;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 1;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 2;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 3;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 4;
        CreateActorFromDescHere(&sp, 1);
    }
    else
    {
        TaskYieldTrampoline(20);
        sp.spawnArg = 4;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 2;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 3;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 0;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 1;
        CreateActorFromDescHere(&sp, 1);
    }
    gCurTask->nightmareWizardOpenCloakCount++;
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardOpenCloakUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 5)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardTwist(void)
{
    s32 d;

    gCurTask->updateState = 8;
    gCurTask->nightmareWizardSfxTimer = 1;
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_087492E8);
    d = (s16)gCurTask->health;
    if (d > Div(gCurTask->nightmareWizardMaxHealth, 3))
    {
        TaskYieldTrampoline(60);
        gCurTask->velY = 160 << 8;
        gCurTask->nightmareWizardLoopCount = 0;
        do
        {
            NightmareWizardSteerTowardNearestPlayer();
            TaskYieldTrampoline(8);
            gCurTask->nightmareWizardLoopCount++;
        } while ((s16)gCurTask->nightmareWizardLoopCount <= 9);
        gCurTask->velY = 0;
        gCurTask->nightmareWizardLoopCount = 0;
        do
        {
            NightmareWizardSteerTowardNearestPlayer();
            TaskYieldTrampoline(8);
            gCurTask->nightmareWizardLoopCount++;
        } while ((s16)gCurTask->nightmareWizardLoopCount <= 3);
        gCurTask->velY = -0xA000;
        gCurTask->nightmareWizardLoopCount = 0;
        do
        {
            NightmareWizardSteerTowardNearestPlayer();
            TaskYieldTrampoline(8);
            gCurTask->nightmareWizardLoopCount++;
        } while ((s16)gCurTask->nightmareWizardLoopCount <= 9);
    }
    else
    {
        gCurTask->accelY = -0x1900;
        if (gCurTask->pixelY - gViewRect[2] >= -40)
        {
            do
                TaskYieldTrampoline(1);
            while (gCurTask->pixelY - gViewRect[2] >= -40);
        }
        TaskStopY();
        ActorStopAnim();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(30);
        gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_0874930C);
        gUnk_02007D00[1] = 1;
        gCurTask->velY = 128 << 12;
        TaskYieldTrampoline(12);
        gCurTask->velY = 128 << 11;
        TaskYieldTrampoline(10);
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(5);
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(5);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(5);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(10);
        gCurTask->velY = -0x80000;
        TaskYieldTrampoline(6);
        gUnk_02007D00[1] = 0;
    }
    TaskStop();
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardTwistUpdate(void)
{
    s32 w;

    w = gCurTask->nightmareWizardSfxTimer - 1;
    gCurTask->nightmareWizardSfxTimer = w;
    if (w == 0)
    {
        PlaySfx(0x22E);
        gCurTask->nightmareWizardSfxTimer = 6;
    }
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 8)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardSwoop(void)
{
    gCurTask->updateState = 9;
    TaskYieldTrampoline(24);
    PlaySfx(0x231);
    TaskFaceNearestPlayer();
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = 144 << 9;
    TaskYieldTrampoline(24);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    ActorStopAnim();
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskSetFrame(56);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(24);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(8);
    ActorStopAnim();
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    TaskSetFrame(56);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    sub_080ab7cc();
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        sub_080ab810();
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 3);
    TaskStop();
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardSwoopUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 9)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardState10(void)
{
    gCurTask->updateState = 10;
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        NightmareWizardMoveToSpot(5);
        NightmareWizardAppear(1);
        NightmareWizardVanish(1);
        TaskYieldTrampoline(2);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 2);
    ActorSetState(2);
    TaskSleepForever();
}

void NightmareWizardState10Update(void)
{
    if (gCurTask->state != 10)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardHurt(void)
{
    gCurTask->updateState = 11;
    sub_080ab5c0();
    gUnk_02007D00[0] = 0;
    ActorSetState(3);
    TaskSleepForever();
}

void NightmareWizardHurtUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
    if (gCurTask->state != 11)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void NightmareWizardMoveToNextSpot(void)
{
    s32 n;

    n = gCurTask->nightmareWizardSpotIndex + 1;
    gCurTask->nightmareWizardSpotIndex = n;
    if (n == 10)
        gCurTask->nightmareWizardSpotIndex = 0;
    NightmareWizardMoveToSpot(gUnk_08749224[gCurTask->nightmareWizardSpotIndex]);
}

void NightmareWizardMoveToSpot(s32 a)
{
    gCurTask->pixelX = gViewRect[0] + 128;
    if (gUnk_08749244[a] != 0)
        gCurTask->pixelX += RandomRange(gUnk_08749244[a] + 1) - (gUnk_08749244[a] >> 1);
    gCurTask->pixelY = gUnk_08749252[a] + gViewRect[2];
    if (gUnk_08749260[a] != 0)
        gCurTask->pixelY += RandomRange(gUnk_08749260[a] + 1) - (gUnk_08749260[a] >> 1);
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
}

void NightmareWizardSteerTowardNearestPlayer(void)
{
    s32 v;

    if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
    {
        v = gCurTask->velX + (128 << 7);
        gCurTask->velX = v;
        if (v > 192 << 9)
            gCurTask->velX = 192 << 9;
    }
    else
    {
        v = gCurTask->velX - 0x4000;
        gCurTask->velX = v;
        if (v < -0x18000)
            gCurTask->velX = -0x18000;
    }
}

void sub_080ab5c0(void)
{
    TaskStop();
    if (gUnk_02007D00[1] == 0)
        gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749330);
    else
    {
        gCurTask->actorAnimDelay18 = ActorStartAnim((struct AnimCmd *)gUnk_08749344);
        gUnk_02007D00[1] = 0;
    }
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x4000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    gCurTask->velX = 0;
}

void NightmareWizardAppear(s32 a)
{
    gCurTask->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    if (a != 0)
        PlaySfx(141 << 2);
    TaskSetFrame(67);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->drawCallback = (u32)NightmareWizardDrawStreamedFrameNearView;
    TaskSetFrame(50);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
}

void NightmareWizardVanish(s32 a)
{
    TaskStop();
    gCurTask->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    if (a != 0)
        PlaySfx(0x233);
    TaskSetFrame(60);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
}

void sub_080ab7cc(void)
{
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
}

void sub_080ab810(void)
{
    TaskSetFrame(36);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
}

s32 NightmareWizardReactToDamage(void)
{
    gUnk_02007D00[0] |= 1;
    gCurTask->facing = TaskGetFacingToward(gCurTask->hitterPlayer);
    CreateStarFlash(1, 0, 0);
    ActorSetState(11);
    TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
    return 1;
}

s32 NightmareWizardReactToDefeat(void)
{
    TaskStop();
    gPaletteAnimRefCounts[0] = 0;
    gCurTask->facing = TaskGetFacingToward(gCurTask->hitterPlayer);
    ActorSetHitReactions((u32)gUnk_08749B60);
    gUnk_02007D00[0] |= 2;
    if (gGameState == GAME_STATE_BOSS_ENDURANCE)
    {
        HudStopClock();
        SaveBossEnduranceBestTime(gCurSaveSlot[0]);
    }
    if (gUnk_02007D00[1] != 0)
    {
        gCurTask->frameTable = gUnk_08754568;
        TaskFaceNearestPlayer();
        TaskSetFrame(0);
    }
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void NightmareWizardDefeat(void)
{
    gPaletteAnimRefCounts[0] = 0;
    gCurTask->drawCallback = (u32)NightmareWizardDrawStreamedFrameNearView;
    gCurTask->updateCallback = (u32)NightmareWizardDefeatUpdate;
    gCurTask->frameTable = gNightmareWizardFrames;
    gCurTask->layer = 11;
    sub_080ab5c0();
    ActorStopAnim();
    NightmareWizardVanish(0);
    NightmareWizardMoveToSpot(6);
    TaskStop();
    NightmareWizardAppear(0);
    gCurTask->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    PlaySfx(143 << 2);
    RequestScreenShake(7);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->frame = 76;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 9);
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->frame = 78;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 79;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 9);
    CreateChildTask(TASK_NIGHTMARE_WIZARD_DEFEAT_FLASH, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->u8C.actor->savedTileWord);
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->frame = 80;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 81;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 9);
    RequestScreenShake(6);
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->frame = 82;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 85;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 14);
    gCurTask->nightmareWizardLoopCount = 0;
    do
    {
        gCurTask->frame = 83;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 84;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->nightmareWizardLoopCount++;
    } while ((s16)gCurTask->nightmareWizardLoopCount <= 7);
    RequestScreenShake(5);
    gCurTask->frame = 86;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(2);
    gCurTask->frame = 87;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 88;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    RequestScreenShake(2);
    TaskYieldTrampoline(180);
    if (gGameState == GAME_STATE_BOSS_ENDURANCE)
        goto far;
    {
        RequestScreenShake(7);
        NightmareWizardScatterStarsAt(gViewRect[0] + 80, gViewRect[2] + 136, 30);
        NightmareWizardScatterStarsAt(gViewRect[0] + 224, gViewRect[2] + 136, 15);
        NightmareWizardScatterStarsAt(gViewRect[0] + 208, gViewRect[2] + 136, 0);
        NightmareWizardScatterStarsAt(gViewRect[0] + 48, gViewRect[2] + 136, 30);
        NightmareWizardScatterStarsAt(gViewRect[0] + 192, gViewRect[2] + 136, 10);
        NightmareWizardScatterStarsAt(gViewRect[0] + 112, gViewRect[2] + 136, 10);
        NightmareWizardScatterStarsAt(gViewRect[0] + 32, gViewRect[2] + 136, 10);
        NightmareWizardScatterStarsAt(gViewRect[0] + 216, gViewRect[2] + 136, 45);
        NightmareWizardScatterStarsAt(gViewRect[0] + 80, gViewRect[2] + 136, 0);
        NightmareWizardScatterStarsAt(gViewRect[0] + 200, gViewRect[2] + 136, 30);
        NightmareWizardScatterStarsAt(gViewRect[0] + 224, gViewRect[2] + 136, 0);
        RequestScreenShake(0);
    }
    goto fin;
far:
    TaskYieldTrampoline(180);
fin:
    ExitToEnding();
    TaskExitTrampoline();
}

void NightmareWizardDefeatUpdate(void)
{
    gCurTask->actorAnimDelay18 = ActorTickAnim(gCurTask->actorAnimDelay18);
}

void NightmareWizardScatterStarsAt(s32 x, s32 y, s32 d)
{
    PlaySfx(189);
    CreateChildTaskAt(TASK_STAR_SCATTER, (s16)x, (s16)y, 0);
    if (d != 0)
        TaskYieldTrampoline(d);
}

void Task_NightmareWizardDefeatFlash(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 10;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 16;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_NightmareWizardPalm(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)NightmareWizardPalmFollowBody;
    TaskFaceLikeParent();
    TaskSetFrame(9);
    TaskSleepForever();
}

void NightmareWizardPalmFollowBody(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && gUnk_02007D00[0] == 0 && gUnk_02007D00[5] == 0)
        {
            t->posX = o->pixelX << 16;
            t->posY = o->pixelY << 16;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardPalmTornado(void)
{
    s32 w;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)NightmareWizardTornadoDrawStreamedFrameNearView;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_08754708;
    gCurTask->updateCallback = (u32)NightmareWizardPalmTornadoUpdate;
    TaskFaceLikeParent();
    gCurTask->nightmareWizardPalmTornadoFrameTimer = 2;
    TaskSetFrame(0);
    while (gUnk_02007D00[5] == 0)
    {
        w = gCurTask->nightmareWizardPalmTornadoFrameTimer - 1;
        gCurTask->nightmareWizardPalmTornadoFrameTimer = w;
        if (w == 0)
        {
            gCurTask->nightmareWizardPalmTornadoFrameTimer = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 3)
                gCurTask->frame = w;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void NightmareWizardPalmTornadoUpdate(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardPointingHand(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)NightmareWizardPointingHandFollowBody;
    TaskFaceLikeParent();
    TaskSetFrame(10);
    TaskYieldTrampoline(85);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gUnk_02007D00[7] = 1;
    TaskSetFrame(12);
    TaskSleepForever();
}

void NightmareWizardPointingHandFollowBody(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && gUnk_02007D00[0] == 0)
        {
            if (gUnk_02007D00[7] == 1)
            {
                t->posX = o->pixelX << 16;
                t->posY = o->pixelY << 16;
            }
            else if (gUnk_02007D00[7] == 2)
                TaskFree(gCurTaskIdx);
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardPointTornado(void)
{
    s32 w;
    s32 w2;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)NightmareWizardTornadoDrawStreamedFrameNearView;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_08754718;
    gCurTask->updateCallback = (u32)NightmareWizardPointTornadoUpdate;
    TaskFaceLikeParent();
    gCurTask->nightmareWizardPointTornadoFrameTimer = 2;
    TaskSetFrame(0);
    while (gUnk_02007D00[7] == 0)
    {
        w = gCurTask->nightmareWizardPointTornadoFrameTimer - 1;
        gCurTask->nightmareWizardPointTornadoFrameTimer = w;
        if (w == 0)
        {
            gCurTask->nightmareWizardPointTornadoFrameTimer = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 3)
                gCurTask->frame = w;
        }
        TaskYieldTrampoline(1);
    }
    gCurTask->nightmareWizardPointTornadoFrameTimer = 2;
    TaskSetFrame(4);
    while (gUnk_02007D00[7] == 1)
    {
        w2 = gCurTask->nightmareWizardPointTornadoFrameTimer - 1;
        gCurTask->nightmareWizardPointTornadoFrameTimer = w2;
        if (w2 == 0)
        {
            gCurTask->nightmareWizardPointTornadoFrameTimer = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 7)
                gCurTask->frame = 4;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void NightmareWizardPointTornadoUpdate(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardCloakHands(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)NightmareWizardCloakPartFollowBody;
    TaskFaceLikeParent();
    TaskSetFrame(4);
    gCurTask->nightmareWizardCloakHandsBobY = 0;
    for (;;)
    {
        gCurTask->nightmareWizardCloakHandsLoopCount = 0;
        do
        {
            gCurTask->nightmareWizardCloakHandsBobY--;
            TaskYieldTrampoline(1);
            gCurTask->nightmareWizardCloakHandsLoopCount++;
        } while ((s16)gCurTask->nightmareWizardCloakHandsLoopCount <= 2);
        gCurTask->nightmareWizardCloakHandsLoopCount = 0;
        do
        {
            gCurTask->nightmareWizardCloakHandsBobY++;
            TaskYieldTrampoline(1);
            gCurTask->nightmareWizardCloakHandsLoopCount++;
        } while ((s16)gCurTask->nightmareWizardCloakHandsLoopCount <= 2);
    }
}

void NightmareWizardCloakPartFollowBody(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && o->state == 5 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY + t->unk28;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardPendant(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)NightmareWizardCloakPartFollowBody;
    gCurTask->unk28 = 0;
    TaskFaceLikeParent();
    for (;;)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
}

void Task_NightmareWizardCloakTornado(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)NightmareWizardTornadoDrawStreamedFrameNearView;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_087546F8;
    gCurTask->updateCallback = (u32)NightmareWizardCloakTornadoUpdate;
    TaskFaceLikeParent();
    for (;;)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
}

void NightmareWizardCloakTornadoUpdate(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 8 && o->state == 5 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void Task_NightmareWizardHitBox(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)NightmareWizardHitBoxUpdate;
    TaskSleepForever();
}

void NightmareWizardHitBoxUpdate(void)
{
    vs16 *arr;
    struct Task *t;
    struct Task *o;
    s16 i;
    struct Task *u;
    s32 v;
    s32 w;
    s32 wv0;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        o = &gTasks[i];

        if (o->u76.subtype == 8 && gUnk_02007D00[0] != 2)
        {
            if (o->frame == -1)
                return;
            t->pixelX = o->pixelX;
            wv0 = *(u16 *)((u8 *)o + 74);
    t->pixelY = wv0;
            t->facing = o->facing;
            gCurTask->nightmareWizardHitBoxIndex = gUnk_087494C8[o->frame];
            if (gCurTask->nightmareWizardHitBoxIndex == -1)
                return;
            ActorCheckHitsWithBox((s32)gUnk_08749458[gCurTask->nightmareWizardHitBoxIndex]);
            u = gCurTask;
            v = u->hitKind;
            if (v == 6 && u->hitEffect == 9)
            {
                o = &gTasks[u->hitterSlot];
                o->hitKind = v;
            }
            else
            {
                w = gUnk_08749490[gCurTask->nightmareWizardHitBoxIndex];
                if (w == 0)
                    return;
                ActorCheckHitsWithBox(w);
                u = gCurTask;
                v = u->hitKind;
                if (v == 6 && u->hitEffect == 9)
                {
                    o = &gTasks[u->hitterSlot];
                    o->hitKind = v;
                }
            }
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

s32 sub_080ac678(void)
{
    TaskSetFrame(0);
}

void Task_MetaKnightSword(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_08753DA0;
    TaskFaceLikeParent();
    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        t->layer = 12;
        gCurTask->updateCallback = (u32)sub_080ac72c;
        ActorSetState(0);
        break;
    case 1:
        t->layer = 3;
        gCurTask->updateCallback = (u32)sub_080ac82c;
        ActorSetState(1);
        break;
    case 2:
        t->layer = 12;
        gCurTask->updateCallback = (u32)sub_080ac84c;
        ActorSetState(2);
        break;
    }
    CallTableEntry(gCurTask->state, 3, gUnk_08749B8C);
}

void sub_080ac72c(void)
{
    s32 d;
    s32 k;

    ActorCollideTerrainCeilingAndFloor();
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
    if (gCurTask->metaKnightSwordLanded > 0)
        ActorCheckHits();
    if (gCurTask->hitKind == 7)
    {
        for (gCurTask->metaKnightSwordPickerIndex = 0; (s16)gCurTask->metaKnightSwordPickerIndex < gPlayerCount; gCurTask->metaKnightSwordPickerIndex++)
        {
            d = gCurTask->hitterPlayer;
            k = (s16)gCurTask->metaKnightSwordPickerIndex;
            if ((d >> k) & 1)
            {
                if (gPlayerStates[k].ability == ABILITY_UFO)
                {
                    if (gTasks[k].variant > 6)
                    {
                        sub_08040858(k);
                        gUnk_02007D00[2]++;
                        break;
                    }
                }
                else if (!(gPlayerStates[k].unk42 & 2) && gPlayerStates[k].mode != 13
                         && gPlayerStates[k].mode != 10)
                {
                    sub_08040858(k);
                    gUnk_02007D00[2]++;
                    break;
                }
            }
        }
        if ((s16)gCurTask->metaKnightSwordPickerIndex != gPlayerCount)
            ActorDestroy();
    }
}

void sub_080ac82c(void)
{
    ActorCollideTerrainCeilingAndFloor();
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
}

void sub_080ac84c(void)
{
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
}

void sub_080ac868(void)
{
    struct ActorSpawn sp;
    s32 w;
    u8 v74;

    gCurTask->updateState = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    gCurTask->metaKnightSwordLanded = 0;
    gCurTask->onGround = 0;
    gCurTask->metaKnightSwordFallFrameTimer = 2;
    TaskSetFrame(10);
    while (gCurTask->onGround == 0)
    {
        TaskYieldTrampoline(1);
        w = gCurTask->metaKnightSwordFallFrameTimer - 1;
        gCurTask->metaKnightSwordFallFrameTimer = w;
        if (w == 0)
        {
            gCurTask->metaKnightSwordFallFrameTimer = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 11)
                gCurTask->frame = 4;
        }
    }
    TaskStop();
    TaskSetFrame(12);
    gCurTask->metaKnightSwordLanded = 1;
    v74 = gCurTask->actorSpawnArg;
    if (v74 == 0)
    {
        sp.subtype = 18;
        sp.taskType = TASK_META_KNIGHT_SWORD;
        sp.variant = 2;
        sp.spawnArg = v74;
        sp.x = gViewRect[0] + 120;
        sp.y = gViewRect[2] + 72;
        sp.tileWord = (128 << 5) + gCurTask->tileWord;
        sp.checkTerrain = 0;
        CreateActorFromDesc(&sp, 1);
    }
    TaskSleepForever();
}

void sub_080ac94c(void)
{
}

void sub_080ac950(void)
{
    s32 w;

    gCurTask->updateState = 1;
    if (abs(TaskGetNearestPlayerDx()) <= 15)
    {
        TaskGetNearestPlayerScreenPos();
        if (gUnk_030023B4 <= 119)
            gCurTask->velX = 128 << 8;
        else
            gCurTask->velX = -0x8000;
    }
    TaskSetMotionY(-0x40000, 192 << 6, 192 << 10);
    gCurTask->onGround = 0;
    gCurTask->metaKnightSwordHopFrameTimer = 2;
    TaskSetFrame(22);
    while (gCurTask->onGround == 0)
    {
        TaskYieldTrampoline(1);
        w = gCurTask->metaKnightSwordHopFrameTimer - 1;
        gCurTask->metaKnightSwordHopFrameTimer = w;
        if (w == 0)
        {
            gCurTask->metaKnightSwordHopFrameTimer = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 23)
                gCurTask->frame = 16;
        }
        if (gCurTask->velY > 0)
            gCurTask->layer = 12;
    }
    TaskStop();
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_080aca38(void)
{
}

void sub_080aca3c(void)
{
    gCurTask->updateState = 2;
    gCurTask->frame = 35;
    TaskYieldTrampoline(216);
    TaskExitTrampoline();
}

void sub_080aca60(void)
{
    if ((s16)gTaskSlotTypes[gCurTask->parent] == -1)
        ActorDestroy();
}

void Task_KrackoStarman(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 12;
    gCurTask->frameTable = gKrackoStarmanFrames;
    TaskFaceNearestPlayer();
    gUnk_02007D00[2]++;
    gCurTask->updateCallback = (u32)KrackoStarmanUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gKrackoStarmanStates);
}

void KrackoStarmanUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gKrackoStarmanStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void KrackoStarmanState0(void)
{
    s32 w;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(0, 148 << 10, 192 << 10);
    TaskSetMotionXFacing(192 << 8, 0x5A5A5A5A);
    gCurTask->krackoStarmanFrameTimer = 6;
    TaskSetFrame(11);
    while (gCurTask->onGround == 0)
    {
        w = gCurTask->krackoStarmanFrameTimer - 1;
        gCurTask->krackoStarmanFrameTimer = w;
        if (w == 0)
        {
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 13)
                gCurTask->frame = 11;
            gCurTask->krackoStarmanFrameTimer = 6;
        }
        TaskYieldTrampoline(1);
    }
    TaskStopY();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(8);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(10);
        TaskSetFrame(9);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void KrackoStarmanCheckParent(void)
{
    vs16 *arr;
    s16 i;

    arr = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->u76.subtype == 6 && o->krackoDefeatStage == 0)
            return;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_080acc8c(void)
{
    gUnk_02007D00[2]--;
}

void Task_NightmareWizardStar(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)NightmareWizardStarUpdate;
    gCurTask->nightmareWizardStarDone = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gNightmareWizardStarStates);
}

void NightmareWizardStarUpdate(void)
{
    if ((u8)ActorCollideTerrainAlongVelocity() == 1)
        TaskSetEntry(ActorDie, gCurTaskIdx);
    else
    {
        CallTableEntry(gCurTask->updateState, 1, gNightmareWizardStarStateUpdates);
        ActorCheckHits();
        ActorReactToHit();
    }
}

void NightmareWizardStarState0(void)
{
    struct Task *t;
    s32 w;
    s32 a2;
    struct Task *u;
    struct Task *u2;
    struct Task **c;
    struct Task **c2;
    struct Task **c3;

    gCurTask->updateState = 0;
    TaskFaceLikeParent();
    gCurTask->actorAnimDelay30 = ActorStartAnim((struct AnimCmd *)gUnk_08749BD0);
    gCurTask->onGround = 0;
    t = gCurTask;
    if (t->variant == 3)
    {
        a2 = gUnk_08749BB1[t->actorSpawnArg] + (160 << 2);
        t->nightmareWizardStarAngle = (a2 - (t->facing << 7)) & 0x1FF;
        AngleToVector(t->nightmareWizardStarAngle, 128 << 3);
        u = gCurTask;
        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
        PlaySfx(0x22F);
    }
    else if (t->variant == 4)
    {
        AngleToVector(gUnk_08749BAC[t->actorSpawnArg], 128 << 3);
        u = gCurTask;
        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
        PlaySfx(0x22F);
    }
    else
    {
        TaskSetMotionXFacing(gUnk_08749BB8[t->variant], 0x5A5A5A5A);
        u2 = gCurTask;
        u2->velY = gUnk_08749BC4[u2->variant];
    }
    TaskYieldTrampoline(255);
    c = &gCurTask;
    u = *c;
    c2 = c;
    u->nightmareWizardStarHomingTimer = 255;
    c3 = c2;
    do
    {
        if (((*c3)->nightmareWizardStarHomingTimer & 3) == 0)
            TaskAccelerateTowardNearestPlayer(154 << 7, 0x18100);
        TaskYieldTrampoline(1);
        u = *c2;
        w = u->nightmareWizardStarHomingTimer - 1;
        u->nightmareWizardStarHomingTimer = w;
    } while (w != 0);
    gCurTask->nightmareWizardStarDone++;
    TaskSleepForever();
}

void NightmareWizardStarState0Update(void)
{
    gCurTask->actorAnimDelay30 = ActorTickAnim(gCurTask->actorAnimDelay30);
    if (gUnk_02007D00[0] == 2)
        gCurTask->nightmareWizardStarDone++;
    if (gCurTask->nightmareWizardStarDone != 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_PaintRoller(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    gCurTask->frameTable = gPaintRollerFrames;
    gCurTask->layer = 11;
    ActorInitBossGfx(0);
    sub_08063a00((u32)gUnk_0874B3A8);
    gCurTask->unk34 = 0;
    gUnk_02007D00[0] = 1;
    gUnk_02007D00[2] = 1;
    gUnk_02007D00[3] = 0x10001;
    gUnk_02007D00[4] = 0;
    gUnk_02007D00[5] = -1;
    gUnk_02007D00[9] = ActorComputeHealth();
    PaintRollerMoveToSpot();
    ActorSetState(0);
    gCurTask->paintRollerEnteredState = 0;
    gCurTask->updateCallback = (u32)sub_080acf3c;
    ActorIntroPoseUntilHpBarFull((struct AnimCmd *)gUnk_08749CEC);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080acf3c(void)
{
    PaintRollerUpdate();
}

void PaintRollerEnterState(void)
{
    gCurTask->paintRollerEnteredState = gCurTask->state;
    CallTableEntry(gCurTask->state, 3, gPaintRollerStates);
}

void PaintRollerState1(void)
{
    gCurTask->updateCallback = (u32)PaintRollerUpdate;
    gUnk_02007D00[5] = -1;
    PaintRollerMoveToSpot();
    gUnk_02007D00[1] = 0;
    gCurTask->paintRollerStepsLeft = 16;
    CreateDustTrail(1, 3, 8, 10);
    while (gCurTask->paintRollerStepsLeft > 0)
        PaintRollerRunMoveStep();
    PaintRollerPickNextSpot();
    while (gCurTask->paintRollerStepsLeft > 0)
        PaintRollerRunMoveStep();
    TaskStop();
    PaintRollerMoveToSpot();
    if (PaintRollerHasHalfHealth() != 0)
    {
        gUnk_02007D00[5] = 2;
        sub_080ad08c();
    }
    ActorSetState(2);
    TaskSleepForever();
}

void PaintRollerSummon(void)
{
    s32 w;

    gUnk_02007D00[5] = -1;
    gUnk_02007D00[1] = 7;
    gCurTask->paintRollerStepsLeft = 10;
    while (gCurTask->paintRollerStepsLeft > 0)
        PaintRollerRunMoveStep();
    if (PaintRollerHasHalfHealth() != 0)
        TaskYieldTrampoline(32);
    CreatePaintRollerPainting();
    gUnk_02007D00[1] = 8;
    gCurTask->paintRollerStepsLeft = 27;
    while (gCurTask->paintRollerStepsLeft > 0)
        PaintRollerRunMoveStep();
    sub_080ad3a0();
    gUnk_02007D00[5] = 1;
    if (gCurTask->paintRollerStepsLeft > 0)
    {
        do
        {
            sub_080ad08c();
            w = gCurTask->paintRollerStepsLeft - 1;
            gCurTask->paintRollerStepsLeft = w;
        } while (w > 0);
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080ad08c(void)
{
    TaskStop();
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(24);
    TaskYieldTrampoline(4);
    TaskSetFrame(26);
    TaskYieldTrampoline(11);
    TaskSetFrame(24);
    TaskYieldTrampoline(4);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    TaskSetFrame(18);
    TaskYieldTrampoline(11);
}

void PaintRollerUpdate(void)
{
    struct Task *t;

    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
    t = gCurTask;
    if (t->paintRollerEnteredState != t->state)
        TaskSetEntry(PaintRollerEnterState, gCurTaskIdx);
}

void PaintRollerReactToDamage(void)
{
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    sub_080ad458();
}

s32 PaintRollerReactToDefeat(void)
{
    ActorSetHitReactions((u32)gUnk_0874B4EC);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void PaintRollerDropStarRodPiece(void)
{
    CreateStarRodPiece(0, 128, 104);
}

void PaintRollerPickNextSpot(void)
{
    s32 v3;
    u8 v5;
    s32 w;
    s32 w2;
    s32 w3;

    v3 = gUnk_02007D00[0];
    v5 = gUnk_02007D00[0];
    if (gUnk_02007D00[4] > 1)
    {
        gUnk_02007D00[4] = 0;
        gUnk_030023D4 = RandomRange(2);
        v5 = v5 + 1;
        v5 &= 1;
        v5 = v5 + gUnk_030023D4 * 2;
        gUnk_030023D4 = gUnk_08749D1C[gUnk_02007D00[0] * 4 + v5];
    }
    else if (gUnk_02007D00[4] <= -2)
    {
        gUnk_02007D00[4] = 0;
        v5 += 2;
        v5 &= 3;
        gUnk_030023D4 = gUnk_08749D1C[v3 * 4 + v5];
    }
    else
    {
        w2 = RandomRange(3);
        w3 = v5 + 1;
        v5 = w3 + w2;
        v5 &= 3;
        gUnk_030023D4 = gUnk_08749D1C[gUnk_02007D00[0] * 4 + v5];
        w = gUnk_08749D38[gUnk_030023D4];
        if (gUnk_02007D00[4] != w)
            gUnk_02007D00[4] = w;
        else
            gUnk_02007D00[4] <<= 1;
    }
    gUnk_02007D00[0] = v5;
    gUnk_02007D00[1] = gUnk_08749D2C[gUnk_030023D4];
    gCurTask->paintRollerStepsLeft = gUnk_08749D44[gUnk_02007D00[1]];
}

void PaintRollerRunMoveStep(void)
{
    s32 w;

    w = gCurTask->paintRollerStepsLeft - 1;
    gCurTask->paintRollerStepsLeft = w;
    gUnk_030023D4 = gUnk_08749D4C[gUnk_02007D00[1]];
    TaskSetFrame(*(s16 *)(gUnk_030023D4 + w * 2));
    gUnk_030023D4 = gUnk_08749D70[gUnk_02007D00[1]];
    TaskSetMotionXFacing(*(s32 *)(gUnk_030023D4 + gCurTask->paintRollerStepsLeft * 4), 0x5A5A5A5A);
    gUnk_030023D4 = gUnk_08749D94[gUnk_02007D00[1]];
    gCurTask->velY = *(s32 *)(gUnk_030023D4 + gCurTask->paintRollerStepsLeft * 4);
    gUnk_030023D4 = gUnk_08749DB8[gUnk_02007D00[1]];
    PlaySfx(*(s32 *)(gUnk_030023D4 + gCurTask->paintRollerStepsLeft * 4));
    gUnk_030023D4 = gUnk_08749DDC[gUnk_02007D00[1]];
    TaskYieldTrampoline(*(u8 *)(gUnk_030023D4 + gCurTask->paintRollerStepsLeft));
}

void PaintRollerMoveToSpot(void)
{
    struct Task *t;

    gCurTask->facing = gUnk_0874AAD0[gUnk_02007D00[0]];
    t = gCurTask;
    t->posX = gUnk_0874AAD4[gUnk_02007D00[0]] << 16;
    t->posY = gUnk_0874AADC[gUnk_02007D00[0]] << 16;
}

s32 PaintRollerHasHalfHealth(void)
{
    s32 r = 0;

    if ((s16)gCurTask->health >= gUnk_02007D00[9] >> 1)
        r = 1;
    return r;
}

void sub_080ad3a0(void)
{
    s32 r;

    r = PaintRollerHasHalfHealth();
    if (r != 0)
        gUnk_030023D4 = 2;
    else
        gUnk_030023D4 = r;
    gUnk_02007D00[2] = (gUnk_02007D00[2] + 1) & 1;
    gCurTask->paintRollerStepsLeft = gUnk_02007D00[2] + gUnk_030023D4;
}

void CreatePaintRollerPainting(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    sp.subtype = 14;
    sp.taskType = TASK_PAINT_ROLLER_PAINTING;
    sp.variant = t->variant;
    sp.spawnArg = t->actorSpawnArg;
    sp.x = gUnk_0874AAE4[gUnk_02007D00[0]];
    sp.y = gUnk_0874AAEC[gUnk_02007D00[0]];
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 0;
    gCurTask->paintRollerPaintingSlot = CreateActorFromDesc(&sp, 1);
}

void sub_080ad458(void)
{
    BossStartHitStun(13, (u32)sub_080ad47c, (u32)gUnk_082DFFA8, 32, 1);
}

void sub_080ad47c(void)
{
    if (gUnk_02006190[3] == 0)
    {
        BossEndHitStun();
        if (gUnk_02007D00[5] >= 0)
        {
            ActorSetState((u16)gUnk_02007D00[5]);
            TaskSetEntry(PaintRollerEnterState, gCurTaskIdx);
        }
    }
}

void Task_HeavyMole(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    gCurTask->moveCallback = (u32)HeavyMoleMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->frameTable = gHeavyMoleFrames;
    gCurTask->layer = 10;
    ActorInitBossGfx(0);
    gCurTask->tileWord |= 128 << 4;
    sub_08066144();
    gCurTask->facing = 1;
    sub_08063a00((u32)gUnk_0874B3FC);
    t = gCurTask;
    t->heavyMoleCameraX = gCameraAnchorX << 16;
    t->posX = 176 << 16;
    t->pixelX = gCameraAnchorX + 56;
    t->posY = gCameraAnchorY << 16;
    t->pixelY = t->posY >> 16;
    t->heavyMoleMoveTimer = 120;
    t->heavyMoleAnimSpeedTimer = 120;
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = 0;
    gUnk_02007D00[2] = 128 << 10;
    gUnk_02007D00[3] = 0;
    gUnk_02007D00[4] = 0;
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 240;
    gUnk_02007D00[7] = 3;
    gUnk_02007D00[9] = ActorComputeHealth();
    sp.subtype = 19;
    sp.taskType = TASK_HEAVY_MOLE_UPPER_ARM;
    sp.variant = gCurTask->variant;
    sp.spawnArg = gCurTask->actorSpawnArg;
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 0);
    sp.subtype = 19;
    sp.taskType = TASK_HEAVY_MOLE_LOWER_ARM;
    sp.variant = gCurTask->variant;
    sp.spawnArg = gCurTask->actorSpawnArg;
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 0);
    CreateChildTaskHere(TASK_HEAVY_MOLE_TURBINES, 1);
    CreateChildTaskHere(TASK_HEAVY_MOLE_MISSILE_HATCH, 1);
    gUnk_02007D00[8] = CreateChildTaskHere(TASK_HEAVY_MOLE_EYE, 1);
    CreateChildTaskHere(TASK_HEAVY_MOLE_SMOKE, 1);
    gCurTask->u8C.actor->defeatSweepCallback = (u32)HeavyMoleDefeatSweepFilter;
    sub_08066544();
    gCurTask->updateCallback = (u32)HeavyMoleUpdate;
    for (;;)
    {
        gCurTask->frame = 4;
        sub_080ad630();
        gCurTask->frame = 5;
        sub_080ad630();
        gCurTask->frame = 6;
        sub_080ad630();
        gCurTask->frame = 7;
        sub_080ad630();
    }
}

void sub_080ad630(void)
{
    u8 *tb = gUnk_0874AAF4;
    s16 *p = (s16 *)gUnk_02007D00;

    TaskYieldTrampoline(tb[p[5]]);
}

void HeavyMoleMove(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    s16 *sp5;
    s32 v;
    s32 w;
    s32 v2;
    s32 w2;

    if (gUnk_02006190[3] <= 0)
        TaskIntegrateMotion();
    else
        gUnk_030023D4 = (s16)gCurTask->health;
    ta = gCurTask;
    v = ta->posY >> 16;
    sp5 = gRoomBounds;
    w = sp5[2] + 90;
    if (v < w)
        ta->posY = (w << 16) + (128 << 8);
    tb = gCurTask;
    v2 = tb->posY >> 16;
    w2 = sp5[3] - 90;
    if (v2 >= w2)
        tb->posY = (w2 << 16) + -0x8000;
    t = gCurTask;
    t->heavyMoleCameraX = (u16)t->heavyMoleCameraX + (gCameraAnchorX << 16) + t->heavyMoleScrollSpeed;
    gCameraAnchorX = t->heavyMoleCameraX >> 16;
    gCameraAnchorY = t->posY >> 16;
    t->pixelX = gCameraAnchorX + (t->posX >> 16) - 120;
    t->pixelY = t->posY >> 16;
    gUnk_02006190[0] = t->pixelX;
    gUnk_02006190[1] = t->pixelY;
}

void HeavyMoleUpdate(void)
{
    s32 w;
    s32 w2;

    if (gHudHpBarFilled != 0)
        ActorResetAttackBox();
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
    TaskBreakBlocksNoPlayer((u32)gUnk_0874B538);
    w = gCurTask->heavyMoleMoveTimer - 1;
    gCurTask->heavyMoleMoveTimer = w;
    if (w <= 0)
        HeavyMoleStartNextMove();
    w2 = gCurTask->heavyMoleAnimSpeedTimer - 1;
    gCurTask->heavyMoleAnimSpeedTimer = w2;
    if (w2 == 0)
    {
        sub_080ad788();
        gCurTask->heavyMoleAnimSpeedTimer = 120;
    }
    if (gUnk_02007D00[6] > 0)
    {
        gUnk_02007D00[6]--;
        if (gUnk_02007D00[6] <= 0)
        {
            gUnk_02007D00[5]--;
            if (gUnk_02007D00[5] < 0)
                gUnk_02007D00[5] = 2;
        }
    }
}

void sub_080ad788(void)
{
    gUnk_030023D4 = gUnk_02007D00[2] >> 16;
    if (gCurTask->velX > 0)
    {
        gUnk_030023D4++;
        if (gUnk_030023D4 > 2)
            gUnk_030023D4 = 2;
    }
    else if (gCurTask->velX < 0)
    {
        gUnk_030023D4--;
        if (gUnk_030023D4 < 0)
            gUnk_030023D4 = 0;
    }
    else if (gUnk_030023D4 != 1)
        gUnk_030023D4 = 1;
    else if ((gUnk_02007D00[2] & 255) == 0)
        gUnk_030023D4 = 2;
    else
        gUnk_030023D4 = 0;
    gUnk_02007D00[2] = ((s16 *)gUnk_02007D00)[5] + (gUnk_030023D4 << 16);
}

void HeavyMoleStartNextMove(void)
{
    struct Task *t;
    struct Task **c8;
    s32 d;
    s32 va;
    s32 r;
    struct Task *u2;

    gUnk_030023D4 = gUnk_0874AAF7[gUnk_02007D00[1]];
    gCurTask->heavyMoleMoveTimer = gUnk_0874AB26[gUnk_030023D4];
    gUnk_030023B4 = gUnk_0874AB50[gUnk_030023D4] + gUnk_02007D00[3];
    gCurTask->velX = gUnk_0874ABA0[gUnk_030023B4];
    gCurTask->velY = gUnk_0874AC24[gUnk_030023B4];
    switch (gUnk_030023D4)
    {
    case 0:
        HeavyMoleSetPhaseFromHealth();
        c8 = &gCurTask;
        t = *c8;
        t->heavyMoleScrollSpeed = gUnk_0874ACBC[gUnk_02007D00[3]];
        gUnk_030023D4 = va = gUnk_0874ACA8[gUnk_02007D00[3]];
        gUnk_030023B4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        d = (gUnk_030023B4 << 16) - t->posX;
        if (d < 0)
            d = t->posX - (gUnk_030023B4 << 16);
        r = Div(d, va);
        t = *c8;
        t->heavyMoleMoveTimer = r;
        if (t->posX >> 16 > gUnk_030023B4)
            gUnk_030023D4 = -gUnk_030023D4;
        t->velX = gUnk_030023D4;
        gUnk_02007D00[1]++;
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 1:
        TaskStop();
        gUnk_030023D4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        gCurTask->posX = (gUnk_030023D4 << 16) + (128 << 8);
        sub_080ada20();
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 4:
    case 5:
        TaskStop();
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        gUnk_02007D00[1]++;
        break;
    case 2:
    case 3:
        if (gUnk_030023D4 == 3)
            gUnk_030023B4 = 1;
        else
            gUnk_030023B4 = 0;
        gUnk_030023D4 = sub_080adaf8(gUnk_030023B4);
        switch (gUnk_030023D4)
        {
        case 1:
            break;
        case 0:
        case 3:
            if (RandomRange(2) == 0)
                break;
        case 2:
            gCurTask->velY = -gCurTask->velY;
            break;
        }
        gUnk_02007D00[1]++;
        break;
    default:
        gUnk_02007D00[1]++;
        break;
    }
}

void HeavyMoleSetPhaseFromHealth(void)
{
    if (gCurTask->health < gUnk_02007D00[9] >> 1)
        gUnk_02007D00[3] = 2;
    else if (gCurTask->health < Div(gUnk_02007D00[9] * 5, 6) + 1)
        gUnk_02007D00[3] = 1;
    else
        gUnk_02007D00[3] = 0;
}

void sub_080ada20(void)
{
    s16 v;

    gUnk_030023D4 = 255 - gUnk_0874ACD4[gUnk_02007D00[3] * 4 + (gUnk_02007D00[0] & 255)];
    gUnk_030023B4 = RandomRange(gUnk_030023D4);
    gUnk_030023D4 = 0;
    gCurTask->heavyMolePatternIndex = 0;
    while (1)
    {
        if ((s16)gCurTask->heavyMolePatternIndex != gUnk_02007D00[0])
        {
            gUnk_030023D4 += gUnk_0874ACD4[gUnk_02007D00[3] * 4 + (s16)gCurTask->heavyMolePatternIndex];
            if (gUnk_030023D4 >= gUnk_030023B4)
                break;
            if ((s16)gCurTask->heavyMolePatternIndex == 3)
                break;
        }
        gCurTask->heavyMolePatternIndex++;
    }
    v = gCurTask->heavyMolePatternIndex;
    gUnk_030023D4 = v;
    gUnk_02007D00[0] = v;
    gUnk_02007D00[1] = gUnk_0874ACE4[(gUnk_030023B4 & gUnk_0874ACE0[v]) + v * 2];
}

s32 sub_080adaf8(s32 arg)
{
    s32 *pd;
    u16 *a;
    s16 *p5;
    u16 *tt;
    s32 v4;

    pd = &gUnk_030023D4;
    *pd = 0;
    v4 = gCurTask->posY >> 16;
    p5 = gRoomBounds;
    tt = gUnk_0874ACEE;
    a = &tt[gUnk_02007D00[3] * 2 + arg];
    if (v4 < p5[2] + a[0])
        *pd = 1;
    if (v4 > p5[3] - a[0])
        *pd |= 2;
    return *pd;
}

void HeavyMoleReactToDamage(void)
{
    CreateChildTaskHere(TASK_STAR_FLASH_ON_PARENT, 0);
    BossStartHitStun(23, (u32)sub_080adb90, (u32)gUnk_082F65D4, 32, 0);
    TaskSetSkipMask(4, gCurTaskIdx);
}

void sub_080adb90(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 *p;

    TaskBreakBlocksNoPlayer(gUnk_0874B538);
    c = &gCurTask;
    t = *c;
    t->heavyMoleAnimSpeedTimer--;
    if (t->heavyMoleAnimSpeedTimer == 0)
    {
        sub_080ad788();
        u = *c;
        u->heavyMoleAnimSpeedTimer = 120;
    }
    p = gUnk_02007D00;
    if (p[6] > 0)
    {
        p[6]--;
        if (p[6] <= 0)
        {
            p[5]--;
            if (p[5] < 0)
                p[5] = 2;
        }
    }
    if (gUnk_02006190[3] == 0)
        BossEndHitStun();
}

s32 HeavyMoleReactToDefeat(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *tb;

    t = gCurTask;
    t->posX = t->pixelX << 16;
    ActorSetHitReactions((u32)gUnk_0874B504);
    tb = gTasks;
    u = &tb[gUnk_02007D00[8]];
    u->frame = 13;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_080adc44(void)
{
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(150);
    if (gGameState == GAME_STATE_STAGE && gMetaKnightmareMode == 0)
    {
        if (GetLivingActivePlayerHealth() != 0)
        {
            FreezeStage(15);
            ExitToNextRoom();
        }
    }
    else
    {
        if (GetLivingActivePlayerHealth() != 0)
        {
            FreezeStage(15);
            ExitClearedStage();
        }
    }
    TaskSleepForever();
}

void Task_HeavyMoleMissileHatch(void)
{
    struct Task **c;
    s32 *p;
    s32 *p2;
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 9;
    u = gCurTask;
    u->lateUpdateCallback = (u32)HeavyMoleMissileHatchFollowBody;
    c = &gCurTask;
    p = gUnk_02007D00;
top:
    (*c)->frame = 14;
    if (p[7] > 0)
    {
        p2 = gUnk_02007D00;
        do
        {
            TaskYieldTrampoline(1);
        } while (p2[7] > 0);
    }
    PlaySfx(131 << 2);
    (*c)->frame++;
    TaskYieldTrampoline(4);
    (*c)->frame++;
    TaskYieldTrampoline(48);
    CreateHeavyMoleMissile();
    TaskYieldTrampoline(64);
    (*c)->frame--;
    TaskYieldTrampoline(4);
    (*c)->frame--;
    TaskYieldTrampoline(2);
    p[7] = 3;
    goto top;
}

void HeavyMoleMissileHatchFollowBody(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
        TaskSetEntry(sub_080ade98, gCurTaskIdx);
    else if (gUnk_02006190[3] != 0)
        TaskSetSkipMask(1, gCurTaskIdx);
    else
        TaskSetSkipMask(0, gCurTaskIdx);
}

void CreateHeavyMoleMissile(void)
{
    struct ActorSpawn sp;
    s32 n;

    PlaySfx(0x20B);
    if (gUnk_02007D00[4] == 0)
        n = RandomRange(4);
    else if (gUnk_02007D00[4] == 1)
    {
        n = 1;
        gUnk_02007D00[4] = 2;
    }
    else
        n = RandomRange(3);
    if (n == 0)
    {
        gUnk_02007D00[4] = 1;
        sp.subtype = 21;
        sp.taskType = TASK_HEAVY_MOLE_RED_MISSILE;
        sp.variant = gCurTask->variant;
        sp.spawnArg = gCurTask->unk74;
        sp.checkTerrain = 1;
        CreateActorFromDescHere(&sp, 0);
    }
    else
    {
        sp.subtype = 20;
        sp.taskType = TASK_HEAVY_MOLE_YELLOW_MISSILE;
        sp.variant = gCurTask->variant;
        sp.spawnArg = gCurTask->unk74;
        CreateActorFromDescHere(&sp, 0);
    }
}

void sub_080ade98(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080adebc;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080adebc(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->parent].frame == -1)
        t->drawCallback = 0;
    else
        t->drawCallback = (u32)TaskDrawWorld;
}

void Task_HeavyMoleTurbines(void)
{
    struct Task **c;
    s32 k;
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 7;
    u = gCurTask;
    u->lateUpdateCallback = (u32)HeavyMoleTurbinesFollowBody;
    c = &gCurTask;
    k = 8;
top:
    (*c)->frame = k;
    sub_080ad630();
    (*c)->frame = 9;
    sub_080ad630();
    (*c)->frame = 10;
    sub_080ad630();
    goto top;
}

void HeavyMoleTurbinesFollowBody(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
        TaskSetEntry(sub_080adfd4, gCurTaskIdx);
}

void sub_080adfd4(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080adff8;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080adff8(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->parent].frame == -1)
        t->drawCallback = 0;
    else
        t->drawCallback = (u32)TaskDrawWorld;
}

void Task_HeavyMoleEye(void)
{
    struct Task **c;
    s32 k;
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    s32 w;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 9;
    u = gCurTask;
    u->lateUpdateCallback = (u32)HeavyMoleEyeFollowBody;
    u->heavyMoleEyeBlinkTimer = 96;
    c = &gCurTask;
    k = 11;
top:
    t2 = *c;
    w = t2->heavyMoleEyeBlinkTimer;
    if (w >= 0)
    {
        t2->heavyMoleEyeBlinkTimer = w - 1;
        t2->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        goto top;
    }
    t2->frame = k;
    TaskYieldTrampoline(4);
    u2 = *c;
    u2->frame = 12;
    TaskYieldTrampoline(4);
    u3 = *c;
    u3->frame = k;
    TaskYieldTrampoline(4);
    u4 = *c;
    u4->heavyMoleEyeBlinkTimer = 96;
    goto top;
}

void HeavyMoleEyeFollowBody(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    s32 k1;
    s32 k2;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
    {
        k1 = 13;
        t->frame = k1;
        TaskSetEntry(sub_080ae174, gCurTaskIdx);
    }
    else if (gUnk_02006190[3] != 0)
    {
        k2 = 13;
        t->frame = k2;
        TaskSetSkipMask(1, gCurTaskIdx);
    }
    else
        TaskSetSkipMask(0, gCurTaskIdx);
}

void sub_080ae174(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080ae198;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080ae198(void)
{
    vs16 *arr;
    struct Task *t;
    s32 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((u16)arr[i] != 62)
        ActorDestroy();
    else
    {
        if (gTasks[i].frame == -1)
            t->drawCallback = 0;
        else
            t->drawCallback = (u32)TaskDrawWorld;
    }
}

void Task_HeavyMoleSmoke(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->frameTable = gHeavyMoleFrames;
    gCurTask->layer = 7;
    gCurTask->lateUpdateCallback = (u32)sub_080ae37c;
    while (1)
    {
        TaskStop();
        gCurTask->frame = 0xFFFF;
        while (gUnk_02007D00[7] <= 0)
            TaskYieldTrampoline(1);
        while (gUnk_02007D00[6] > 0)
            TaskYieldTrampoline(1);
        gCurTask->heavyMoleSmokePuffCount = 20;
        while (gUnk_02007D00[7] > 0)
        {
            if (gUnk_02007D00[5] == 0)
                gUnk_030023D4 = (gCurTask->heavyMoleSmokePuffCount & 1) + 1;
            else
                gUnk_030023D4 = gUnk_02007D00[5];
            gCurTask->heavyMoleSmokeRow = gUnk_030023D4 >> 1;
            gCurTask->posX = (gTasks[gCurTask->parent].pixelX - 24) << 16;
            gCurTask->posY = (gTasks[gCurTask->parent].pixelY
                                    + gUnk_0874ACFA[gCurTask->heavyMoleSmokeRow]) << 16;
            gCurTask->heavyMoleSmokeStep = 3;
            do
            {
                gCurTask->heavyMoleSmokeStep--;
                gCurTask->frame = gUnk_0874ACFE[gCurTask->heavyMoleSmokeStep * 2 + gCurTask->heavyMoleSmokeRow];
                gCurTask->velX = gUnk_0874AD0C[gCurTask->heavyMoleSmokeStep];
                gCurTask->velY = gUnk_0874AD18[gCurTask->heavyMoleSmokeStep * 2 + gCurTask->heavyMoleSmokeRow];
                TaskYieldTrampoline(gUnk_0874AD30[gCurTask->heavyMoleSmokeStep]);
            } while (gCurTask->heavyMoleSmokeStep > 0);
            if (--gCurTask->heavyMoleSmokePuffCount <= 0)
                break;
        }
        gUnk_02007D00[6] = 240;
    }
}

void sub_080ae37c(void)
{
}

s32 HeavyMoleDefeatSweepFilter(s32 i)
{
    s32 v;

    v = (s16)(u16)gTaskSlotTypes[i];
    switch (v)
    {
    case 121 ... 122:
        return 1;
    case 62:
        return 1;
    case 123 ... 124:
        return 1;
    case 189 ... 191:
        return 1;
    case 192:
        return 1;
    default:
        return 0;
    }
}
