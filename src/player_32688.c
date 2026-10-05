#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "save.h"

/* player_32688.c (0x08032688-0x080337F3, issue #92).
 *
 * Task type #5 (class 1), the player task, and its callbacks.
 * Task_Player is the body: it binds the task to its player record
 * (Task.player = &gPlayerStates[gCurTaskIdx]), kills it when the player
 * has no lives and no health left, installs the callbacks (Task.moveCallback =
 * M11's PlayerMove, unk04 = PlayerUpdate, unk08 = PlayerLateUpdate, unk0C =
 * M11's sub_0803ddc0), sets up the ability (PlayerState.ability) and the
 * stage entry mode (gRoomPlayerMode, gRoomEntryMode), and starts the first
 * action.  The actions are two tables of void (*)(void) dispatched
 * through CallTableEntry(index, count, table), entry 0 NULL: the "enter"
 * coroutine of action PlayerState.action from gPlayerActions[62] (M11's
 * gMetaKnightActions[30] when gMetaKnightmareMode != 0) and the "per-frame" handler
 * Task.updateState from gPlayerActionHandlers[57] (M11's gMetaKnightActionHandlers[27]).  A handler requests
 * the next action in PlayerState.requestedAction; PlayerStartRequestedAction is the coroutine
 * that switches to it (unk03 = previous, unk02 = new, unk01 = 0).
 * PlayerUpdate (Task.updateCallback) runs every frame: the attack hit-boxes
 * (TaskBreakBlocks on PlayerState.hitBoxSet), the collision registry, the
 * per-frame handler and the damage and star-block reactions;
 * PlayerLateUpdate (Task.lateUpdateCallback) runs the 10-frame timer PlayerState.blockBreakCooldown;
 * sub_08033414 (called by M11's sub_0803ddc0) turns the frame's hit
 * event Task.hitKind and the status bits PlayerState.actionFlags into an action
 * request, re-binds the task to PlayerStartRequestedAction when one is pending and
 * adds the 8.8 offsets PlayerState.pixelOffsetX/unk26 to the 16.16 position. */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
u32 RandomRange(u32 range);
void TaskSetEntry(void *a, u32 i);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);   /* M09's callers pass ldrsh values unextended (LESSONS 8) */
void sub_08021c74(s8 *box, s32 id);
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void sub_0803c9b4(s32 a);                     /* M10: mov r8, r0 on entry, void epilogue */
void PlayerStepOffsetScript(void);                      /* M10: no argument read, void epilogue */

void Task_Player(void)
{
    struct Task *t;

    gCurTask->parent = gCurTaskIdx;
    gCurTask->player = &gPlayerStates[gCurTaskIdx];
    if (gPlayerLives[gCurTask->player->playerIndex] == 0 && gPlayerHealth[gCurTask->player->playerIndex] == 0)
    {
        gCurTask->updateCallback = 0;
        gCurTask->taskClass = 4;
        if (gCreditsDemoSet == 0)
        {
            if (gGameState != GAME_STATE_BOSS_ENDURANCE)
                PlayerLifeRequestInit();
            else
                PlayerLifeRequestShowGameOver();
            TaskSleepForever();
        }
        TaskSleepForever();
    }
    else
    {
        PlayerClearOwnLifeRequests();
    }
    gCurTask->facing = 1;
    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->drawCallback = (u32)sub_0803ddc0;
    t->updateCallback = (u32)PlayerUpdate;
    t->lateUpdateCallback = (u32)PlayerLateUpdate;
    t->frameTable = gPlayerFrames;
    if (gPlayerCount > 1 && gLocalPlayer == t->player->playerIndex)
        t->layer = 6;
    else
        t->layer = 7;
    gCurTask->tileWord = (gCurTask->player->playerIndex << 13) | (gCurTask->player->playerIndex << 7);
    gCurTask->u76.unk76 = 0;
    gCurTask->player->requestedAction = PLAYER_ACTION_NONE;
    if (gMetaKnightmareMode == 0)
        gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
    else
        gCurTask->player->bodyBox = (u32)gMetaKnightDefaultBodyBox;
    gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
    gCurTask->player->hitBoxSet = 0;
    gCurTask->player->prevPixelX = gCurTask->posX >> 16;
    gCurTask->player->prevPixelY = gCurTask->posY >> 16;
    gCurTask->health = gPlayerHealth[gCurTask->player->playerIndex];
    if (gCurTask->player->ability != ABILITY_NORMAL)
    {
        LoadAbilityTiles();
        switch (gCurTask->player->ability)
        {
        case ABILITY_FIRE:
        case ABILITY_SPARK:
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            PlayerLoadSparkTiles();
            break;
        case ABILITY_PARASOL:
            {
                struct PlayerBodyBox *d = gPlayerBodyBoxes;

                d[gCurTask->player->playerIndex] = *(struct PlayerBodyBox *)gPlayerParasolBodyBox;
            }
            gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct PlayerHitBoxSet *)gPlayerParasolHitBoxSet;
            break;
        case ABILITY_MIKE:
        case ABILITY_CRASH:
        case ABILITY_LIGHT:
            gCurTask->player->paletteFlashMode = 2;
            break;
        case ABILITY_UFO:
            if (gGameState != GAME_STATE_HUB)
                break;
        case ABILITY_SLEEP:
            SetPlayerAbility(ABILITY_NORMAL, -1, gCurTask->player->playerIndex);
            break;
        case ABILITY_STAR_ROD:
            if (gRoomPlayerMode != 2 && gRoomPlayerMode != 3)
                SetPlayerAbilityNoHud(ABILITY_NORMAL, -1, gCurTask->player->playerIndex);
            break;
        }
    }
    gCurTask->player->unk36 = 0;
    switch (gRoomPlayerMode)
    {
    case 0:
        break;
    case 1:
        gCurTask->updateCallback = 0;
        gCurTask->lateUpdateCallback = 0;
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
        PlayerGoalGameInit();
        TaskSleepForever();
    case 2:
        gCurTask->player->unk37 = 2;
        SetPlayerAbility(ABILITY_STAR_ROD, -1, gCurTask->player->playerIndex);
        gCurTask->player->mouthState = 3;
        PlayerActionStarRodFlight();
        TaskSleepForever();
    case 3:
        gCurTask->player->unk37 = 3;
        SetPlayerAbilityNoHud(ABILITY_STAR_ROD, -1, gCurTask->player->playerIndex);
    }
    switch (gRoomEntryMode)
    {
    case ROOM_ENTRY_WARP_STAR:
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
        if (gCurTask->player->playerIndex == 0 || gUnk_020061E0 == 0)
            CreateFlyingWarpStar(gCurTask->posX, gCurTask->posY, PickWarpStarArrivalFlight());
        PlayerWarpStarRideInit();
        TaskSleepForever();
    case ROOM_ENTRY_CANNON:
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
        gPauseDisabled = 1;
        gCurTask->state = 4;
        PlayerCannonInit();
        TaskSleepForever();
    case ROOM_ENTRY_DOOR:
        gPauseDisabled = 1;
        gCurTask->player->action = PLAYER_ACTION_EXIT_DOOR;
        break;
    case ROOM_ENTRY_NORMAL:
    default:
        TaskInitWaterFlags();
        sub_08021c74((s8 *)gPlayerDefaultTerrainBox, gCurTaskIdx);
        gCurTask->player->prevWaterFlags = gCurTask->waterFlags;
        if (!(gCurTask->waterFlags & 1))
        {
            if (gCurTask->onGround & 1)
                gCurTask->player->action = PLAYER_ACTION_STAND;
            else
                gCurTask->player->action = PLAYER_ACTION_FALL;
        }
        else
        {
            if (gCurTask->onGround & 1)
                gCurTask->player->action = PLAYER_ACTION_STAND_IN_WATER;
            else
                gCurTask->player->action = PLAYER_ACTION_SWIM;
        }
        if (gCurTask->player->ability == ABILITY_UFO)
            gCurTask->player->action = PLAYER_ACTION_UFO;
        gCurTask->player->mode = 21;
        CreateLocalPlayerArrow(gCurTask->player->playerIndex);
        break;
    }
    if (gMetaKnightmareMode == 0)
    {
        struct PlayerState *p = gCurTask->player;
        gCurTask->state = p->action;
        CallTableEntry(p->action, 62, gPlayerActions);
    }
    else
    {
        struct PlayerState *p = gCurTask->player;
        gCurTask->state = p->action;
        CallTableEntry(p->action, 30, gMetaKnightActions);
    }
}

void PlayerStartRequestedAction(void)
{
    if (gPlayerCount > 1)
    {
        if (gLocalPlayer == gCurTask->player->playerIndex)
            gCurTask->layer = 6;
        else
            gCurTask->layer = 7;
    }
    gCurTask->player->prevAction = gCurTask->player->action;
    gCurTask->player->action = gCurTask->player->requestedAction;
    gCurTask->player->requestedAction = PLAYER_ACTION_NONE;
    if (gCurTask->player->mouthState == 1)
    {
        gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        gCurTask->player->hitBoxSet = 0;
    }
    else if (gCurTask->player->unk37 != 2)
    {
        if (gMetaKnightmareMode == 0)
            gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        else
            gCurTask->player->bodyBox = (u32)gMetaKnightDefaultBodyBox;
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        gCurTask->player->hitBoxSet = 0;
    }
    gCurTask->player->pixelOffsetX = gCurTask->player->pixelOffsetY = 0;
    if (gCurTask->player->invulnerability == 3 && (s16)gCurTask->player->invulnerabilityTimer == -0x8000)
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->prevAction == PLAYER_ACTION_WATER_SHOT && gCurTask->player->ability != ABILITY_NORMAL)
            LoadAbilityTiles();
        if (gCurTask->player->ability != ABILITY_NORMAL)
            gCurTask->player->unk36 = 1;
    }
    if (gMetaKnightmareMode == 0)
    {
        struct PlayerState *p = gCurTask->player;
        gCurTask->state = p->action;
        CallTableEntry(p->action, 62, gPlayerActions);
    }
    else
    {
        struct PlayerState *p = gCurTask->player;
        gCurTask->state = p->action;
        CallTableEntry(p->action, 30, gMetaKnightActions);
    }
}

void PlayerUpdate(void)
{
    s32 x;
    s32 y;
    s32 r;
    struct PlayerState *p;

    if ((gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_OFFSET_SCRIPT) && (gCurTask->skipMask & TASK_SKIP_COROUTINE))
        goto post;
    if (gCurTask->player->ability == ABILITY_PARASOL && gCurTask->player->mode == 5)
    {
        gCurTask->playerParasolSwayVelX += gCurTask->playerParasolSwayAccelX;
        gCurTask->posX += gCurTask->playerParasolSwayVelX;
        gCurTask->pixelX = gCurTask->posX >> 16;
    }
    if (gCurTask->player->hitBoxSet != 0)
    {
        gCurTask->player->blocksBroken = TaskBreakBlocks(gCurTask->player->hitBoxSet, gCurTask->player->playerIndex);
        if (gCurTask->player->blocksBroken != 0)
            gCurTask->u76.unk76 |= PLAYER_HIT_LANDED;
    }
    else
    {
        gCurTask->player->blocksBroken = 0;
    }
    gCurTask->player->prevWaterFlags = gCurTask->waterFlags;
    gCurTask->player->clampedTopY = 0xFFFF;
    if (gCurTask->player->terrainBox != 0)
    {
        PlayerProbeTerrain(gCurTask->player->terrainBox);
        gCurTask->player->boundsClamp = gTerrainBoundsClamp;
        if (gUnk_02005574[0] == 0 && (gTerrainBoundsClamp & 4) && gTerrainResult.unk0 != 0)
            gCurTask->player->clampedTopY = gTerrainClampedTopY;
        gCurTask->player->prevTerrainBox = (u32 *)gCurTask->player->terrainBox;
        if (gTerrainResult.damage != 0 && !(gCurTask->player->statusFlags & PLAYER_STATUS_NO_TERRAIN_DAMAGE)
         && gCurTask->player->invulnerability != 1 && gCurTask->player->invincible == 0)
        {
            r = AddPlayerHealth(-8, gCurTask->player->playerIndex);
            if (r != 0)
            {
                gCurTask->hitEffect = gTerrainResult.damage | HIT_EFFECT_TERRAIN_DAMAGE;
                gCurTask->hitKind = HIT_KIND_DAMAGE;
            }
            else
            {
                gCurTask->hitKind = HIT_KIND_DEFEAT;
                gCurTask->hitEffect = 0;
                goto post;
            }
        }
    }
    else
    {
        gTerrainResult.unk0 = gTerrainResult.ceilingHits = gTerrainResult.unk2 = 0;
        gTerrainResult.unk3 = gTerrainResult.slope = gTerrainResult.unk5 = 0;
        gTerrainResult.waterSurfaceY = gTerrainResult.onSlipperyFloor = gTerrainResult.damage = 0;
        gTerrainResult.atDoor = 0;
        gTerrainProbeResult.onSlipperyFloor = 0;
    }
    gCurTask->player->wallSide = gTerrainResult.unk0;
    gCurTask->player->slope = gTerrainResult.slope;
    gCurTask->player->onSlipperyFloor = gTerrainProbeResult.onSlipperyFloor;
    gCurTask->player->atDoor = gTerrainResult.atDoor;
    if (PlayerCheckDie() != 0)
        goto tail;
    p = gCurTask->player;
    if (p->bodyBox != 0)
    {
        x = gCurTask->pixelX;
        y = gCurTask->pixelY;
        if (p->unk37 == 2)
        {
            x += gSpriteCameraX;
            y += gSpriteCameraY;
        }
        RegisterCollider(gCurTaskIdx, x, y, (u8 *)p->bodyBox);
    }
    if (gMetaKnightmareMode == 0)
        CallTableEntry(gCurTask->updateState, 57, gPlayerActionHandlers);
    else
        CallTableEntry(gCurTask->updateState, 27, gMetaKnightActionHandlers);
    PlayerUpdateInvulnerability();
post:
    gCurTask->player->hitsThisFrame = 0;
    PlayerUpdateRunning();
    if (!(gCurTask->player->statusFlags & PLAYER_STATUS_TIMERS_FROZEN))
        PlayerUpdatePaletteFlash();
    if ((gMetaKnightmareMode == 1 || gUnk_0300244C != 0)
     && (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_OFFSET_SCRIPT) && (gCurTask->skipMask & TASK_SKIP_COROUTINE))
        goto check;
    if (gCurTask->velY >= 0)
    {
        if (gCurTask->waterFlags & 0x80)
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPLASH, gTerrainResult.waterSurfaceY);
    }
    else if (gCurTask->player->mouthState == 2)
    {
        if (gCurTask->waterFlags & 0x80)
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_LEAVE_WATER_SPLASH, gTerrainResult.waterSurfaceY);
    }
    else if ((gCurTask->player->prevWaterFlags & 1) && !(gCurTask->waterFlags & 1))
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_LEAVE_WATER_SPLASH, gTerrainResult.waterSurfaceY);
    }
    if ((gCurTask->waterFlags & 65) == 1)
    {
        if (--gPlayerBubbleTimers[gCurTask->player->playerIndex] == 0)
        {
            gPlayerBubbleTimers[gCurTask->player->playerIndex] = RandomRange(90) + 120;
            CreatePlayerEffectHighSlot(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BUBBLE, 0);
        }
    }
    else
    {
        gPlayerBubbleTimers[gCurTask->player->playerIndex] = 60;
    }
check:
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->ability == ABILITY_PARASOL)
        {
            x = gCurTask->frame - 0x808;
            if (x >= 0 && LoadPlayerBodyBoxRect(gCurTask->player->playerIndex, (u8 *)gUnk_0873C36C + x * 8) != 0)
            {
                if (gCurTask->frame <= 0x8D1)
                {
                    struct PlayerBodyBox *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->player->playerIndex])[12] = 2;
                }
                else
                {
                    struct PlayerBodyBox *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->player->playerIndex])[12] = 5;
                }
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
            x = gCurTask->frame - 0x8D2;
            if (x >= 0 && LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CF9C + x * 8)) != 0)
                TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex], gCurTask->player->playerIndex);
        }
        if (gLifeRequests.requests[gCurTaskIdx] & 1)
        {
            if ((gPlayerHeldKeys[gCurTask->player->playerIndex] & 0x300) == 0x300)
                gLifeRequests.requests[gCurTaskIdx] = (gLifeRequests.requests[gCurTaskIdx] & 0xF0) | 2;
        }
    }
    else if (gCurTask->player->flightCoastTimer != 0)
    {
        gCurTask->player->flightCoastTimer--;
    }
tail:
    if (gCurTask->player->unk37 != 2 && gLocalPlayer == gCurTask->player->playerIndex)
        SetCameraFocus(gCurTask->posX >> 16, gCurTask->posY >> 16);
    if (gCurTask->player->terrainBox != 0 && !(gCurTask->skipMask & TASK_SKIP_MOVE))
    {
        gCurTask->player->prevPixelX = gCurTask->pixelX;
        gCurTask->player->prevPixelY = gCurTask->pixelY;
    }
}

void PlayerLateUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    if (t->player->requestedAction == PLAYER_ACTION_NONE)
    {
        if (t->u76.unk76 & PLAYER_HIT_LANDED)
        {
            t->u76.unk76 &= 0xFFFE;
            if ((s8)t->player->blockBreakCooldown == 0)
            {
                PlayerStartOffsetScript(0);
                gCurTask->player->blockBreakCooldown = 10;
            }
        }
        p = gCurTask->player;
        if (p->actionFlags & PLAYER_ACTION_FLAG_OFFSET_SCRIPT)
        {
            PlayerStepOffsetScript();
            p = gCurTask->player;
            if (!(p->actionFlags & PLAYER_ACTION_FLAG_OFFSET_SCRIPT) && gMetaKnightmareMode == 0 && p->mode == 7 && p->blocksBroken == 0)
            {
                TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
                gCurTask->player->requestedAction = PLAYER_ACTION_RECOIL;
            }
        }
        else if ((s8)p->blockBreakCooldown != 0)
        {
            p->blockBreakCooldown--;
        }
    }
    if (gCurTask->player->requestedAction == PLAYER_ACTION_NONE)
        PlayerCheckShareItem();
}

void sub_08033414(void)
{
    struct Task *t;
    struct PlayerState *p;

    if (gCurTask->player->requestedAction > 31 && gRoomExitKind == 1)
        gCurTask->player->requestedAction = PLAYER_ACTION_NONE;
    switch (gCurTask->hitKind)
    {
    default:
        if (gMetaKnightmareMode == 0 && (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_METAKNIGHT_SWORD))
        {
            if ((s16)gPlayerAbilities[gCurTask->player->playerIndex] != ABILITY_NORMAL)
            {
                gSavedPlayerAbilities[gCurTask->player->playerIndex] = gPlayerAbilities[gCurTask->player->playerIndex];
                gSavedPlayerAbilityUses[gCurTask->player->playerIndex] = gPlayerAbilityUses[gCurTask->player->playerIndex];
            }
            else
            {
                gSavedPlayerAbilities[gCurTask->player->playerIndex] = ABILITY_SWORD;
                gSavedPlayerAbilityUses[gCurTask->player->playerIndex] = 0xFFFF;
            }
            gCurTask->player->pendingAbility = ABILITY_SWORD;
            gCurTask->player->pendingAbilityUses = 255;
            gCurTask->player->unk37 = 1;
            gCurTask->player->requestedAction = PLAYER_ACTION_GET_ABILITY;
        }
        else if (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_INVINCIBLE_CANDY)
        {
            SetPlayerInvulnerability(5, 0, gCurTask->player->playerIndex);
            gCurTask->player->actionFlags &= ~PLAYER_ACTION_FLAG_INVINCIBLE_CANDY;
            PlayBgm(BGM_INVINCIBLE);
            PlayerStartItemShare(gCurTask->player->playerIndex, 3);
        }
        break;
    case HIT_KIND_DEFEAT:
        gCurTask->player->requestedAction = PLAYER_ACTION_DIE;
        gCurTask->player->paletteFlashMode = 0;
        gCurTask->player->unk1E = gCurTask->player->unk20 = 0;
        break;
    case HIT_KIND_DAMAGE:
        if (gCurTask->player->unk37 != 2)
        {
            gCurTask->player->requestedAction = PLAYER_ACTION_HURT;
            gCurTask->player->playerHoldPose = 255;
        }
        else
        {
            gCurTask->player->requestedAction = PLAYER_ACTION_STAR_ROD_FLIGHT;
            gCurTask->variant = 3;
        }
        gCurTask->player->paletteFlashMode = 0;
        gCurTask->player->unk1E = gCurTask->player->unk20 = 0;
        break;
    }
    gCurTask->hitKind = HIT_KIND_NONE;
    if (gCurTask->player->requestedAction != PLAYER_ACTION_NONE)
    {
        if (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_OFFSET_SCRIPT)
        {
            gCurTask->player->offsetScriptStep = 0;
            gCurTask->player->offsetScriptDelay = 1;
            gCurTask->player->blockBreakCooldown = 0;
            gCurTask->player->pixelOffsetX = gCurTask->player->pixelOffsetY = 0;
            TaskSetSkipMask(0, gCurTaskIdx);
        }
        if (gCurTask->player->sfxPlayer != -1)
            PlayerStopSfx();
        gCurTask->u76.unk76 = 0;
        gCurTask->player->actionFlags = 0;
        gCurTask->player->unk50 = 0;
        gCurTask->u80.attackAbility = ABILITY_NORMAL;
    }
    else if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->u76.unk76 & PLAYER_HIT_HIGH_FALL_BOUNCE)
        {
            gCurTask->player->actionFlags |= PLAYER_ACTION_FLAG_HIGH_FALL_BOUNCE;
            gCurTask->variant = 1;
            gCurTask->player->requestedAction = PLAYER_ACTION_HIGH_FALL;
            gCurTask->u76.unk76 &= 0xFFFD;
        }
        if (!(gCurTask->player->actionFlags & 128))
            PlayerUpdateBlink();
    }
    if (gCurTask->player->requestedAction != PLAYER_ACTION_NONE)
        TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_CATCHING)
        {
            if (gCurTask->player->ability == ABILITY_NORMAL)
                sub_0803c9b4(0);
            else
                sub_0803c9b4(1);
        }
        if ((s8)gCurTask->player->unk36 != 0)
        {
            switch (gCurTask->player->prevAction)
            {
            case PLAYER_ACTION_FIRE:
            case PLAYER_ACTION_SPARK:
                CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
                PlayerLoadSparkTiles();
                break;
            }
            gCurTask->player->unk36 = 0;
        }
    }
    t = gCurTask;
    if (t->player->unk37 == 2)
    {
        if (t->pixelY + gSpriteCameraY > 900)
            t->posY = (t->pixelY = 900 - gSpriteCameraY) << 16;
    }
    t = gCurTask;
    t->pixelX = (t->posX + ((s16)t->player->pixelOffsetX & 0x8000 ? ((s16)t->player->pixelOffsetX << 8) | 0xFF000000 : (s16)t->player->pixelOffsetX << 8)) >> 16;
    t = gCurTask;
    t->pixelY = (t->posY + ((s16)t->player->pixelOffsetY & 0x8000 ? ((s16)t->player->pixelOffsetY << 8) | 0xFF000000 : (s16)t->player->pixelOffsetY << 8)) >> 16;
}
