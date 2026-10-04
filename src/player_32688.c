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
 * M11's PlayerMove, unk04 = PlayerUpdate, unk08 = sub_0803332c, unk0C =
 * M11's sub_0803ddc0), sets up the ability (PlayerState.ability) and the
 * stage entry mode (gUnk_02000020, gRoomEntryMode), and starts the first
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
 * sub_0803332c (Task.lateUpdateCallback) runs the 10-frame timer PlayerState.unk2B;
 * sub_08033414 (called by M11's sub_0803ddc0) turns the frame's hit
 * event Task.hitKind and the status bits PlayerState.unk40 into an action
 * request, re-binds the task to PlayerStartRequestedAction when one is pending and
 * adds the 8.8 offsets PlayerState.pixelOffsetX/unk26 to the 16.16 position. */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
u32 RandomRange(u32 range);
void TaskSetEntry(void *a, u32 i);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);   /* M09's callers pass ldrsh values unextended (LESSONS 8) */
void sub_08021c74(s8 *box, s32 id);
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void sub_0803c9b4(s32 a);                     /* M10: mov r8, r0 on entry, void epilogue */
void sub_0803cbd8(void);                      /* M10: no argument read, void epilogue */

void Task_Player(void)
{
    struct Task *t;

    gCurTask->parent = gCurTaskIdx;
    gCurTask->player = &gPlayerStates[gCurTaskIdx];
    if (gPlayerLives[gCurTask->player->playerIndex] == 0 && gPlayerHealth[gCurTask->player->playerIndex] == 0)
    {
        gCurTask->updateCallback = 0;
        gCurTask->taskClass = 4;
        if (gUnk_030023B0 == 0)
        {
            if (gGameState != 20)
                sub_080b9610();
            else
                sub_080b9118();
            TaskSleepForever();
        }
        TaskSleepForever();
    }
    else
    {
        sub_080b8ebc();
    }
    gCurTask->facing = 1;
    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->drawCallback = (u32)sub_0803ddc0;
    t->updateCallback = (u32)PlayerUpdate;
    t->lateUpdateCallback = (u32)sub_0803332c;
    t->frameTable = gUnk_0874CFEC;
    if (gPlayerCount > 1 && gLocalPlayer == t->player->playerIndex)
        t->layer = 6;
    else
        t->layer = 7;
    gCurTask->tileWord = (gCurTask->player->playerIndex << 13) | (gCurTask->player->playerIndex << 7);
    gCurTask->unk76 = 0;
    gCurTask->player->requestedAction = 0;
    if (gMetaKnightmareMode == 0)
        gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
    else
        gCurTask->player->bodyBox = (u32)gUnk_0873CA54;
    gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
    gCurTask->player->hitBoxSet = 0;
    gCurTask->player->prevPixelX = gCurTask->posX >> 16;
    gCurTask->player->prevPixelY = gCurTask->posY >> 16;
    gCurTask->health = gPlayerHealth[gCurTask->player->playerIndex];
    if (gCurTask->player->ability != 0)
    {
        LoadAbilityTiles();
        switch (gCurTask->player->ability)
        {
        case 1:
        case 2:
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            sub_08049a58();
            break;
        case 10:
            {
                struct M11R20 *d = gPlayerBodyBoxes;

                d[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873C358;
            }
            gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CF94;
            break;
        case 7:
        case 20:
        case 21:
            gCurTask->player->unk22 = 2;
            break;
        case 24:
            if (gGameState != 5)
                break;
        case 11:
            SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
            break;
        case 25:
            if (gUnk_02000020 != 2 && gUnk_02000020 != 3)
                SetPlayerAbilityNoHud(0, -1, gCurTask->player->playerIndex);
            break;
        }
    }
    gCurTask->player->unk36 = 0;
    switch (gUnk_02000020)
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
        SetPlayerAbility(25, -1, gCurTask->player->playerIndex);
        gCurTask->player->mouthState = 3;
        PlayerActionStarRodFlight();
        TaskSleepForever();
    case 3:
        gCurTask->player->unk37 = 3;
        SetPlayerAbilityNoHud(25, -1, gCurTask->player->playerIndex);
    }
    switch (gRoomEntryMode)
    {
    case 2:
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
        if (gCurTask->player->playerIndex == 0 || gUnk_020061E0 == 0)
            sub_08071cc0(gCurTask->posX, gCurTask->posY, sub_080260b0());
        sub_0806ee30();
        TaskSleepForever();
    case 3:
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
        gPauseDisabled = 1;
        gCurTask->state = 4;
        PlayerCannonInit();
        TaskSleepForever();
    case 1:
        gPauseDisabled = 1;
        gCurTask->player->action = 21;
        break;
    case 0:
    default:
        TaskInitWaterFlags();
        sub_08021c74((s8 *)gPlayerDefaultTerrainBox, gCurTaskIdx);
        gCurTask->player->prevWaterFlags = gCurTask->waterFlags;
        if (!(gCurTask->waterFlags & 1))
        {
            if (gCurTask->onGround & 1)
                gCurTask->player->action = 1;
            else
                gCurTask->player->action = 7;
        }
        else
        {
            if (gCurTask->onGround & 1)
                gCurTask->player->action = 24;
            else
                gCurTask->player->action = 23;
        }
        if (gCurTask->player->ability == 24)
            gCurTask->player->action = 55;
        gCurTask->player->mode = 21;
        sub_08040808(gCurTask->player->playerIndex);
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
    gCurTask->player->requestedAction = 0;
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
            gCurTask->player->bodyBox = (u32)gUnk_0873CA54;
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        gCurTask->player->hitBoxSet = 0;
    }
    gCurTask->player->pixelOffsetX = gCurTask->player->pixelOffsetY = 0;
    if (gCurTask->player->invulnerability == 3 && (s16)gCurTask->player->invulnerabilityTimer == -0x8000)
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->prevAction == 28 && gCurTask->player->ability != 0)
            LoadAbilityTiles();
        if (gCurTask->player->ability != 0)
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

    if ((gCurTask->player->unk40 & 1) && (gCurTask->skipMask & 1))
        goto post;
    if (gCurTask->player->ability == 10 && gCurTask->player->mode == 5)
    {
        gCurTask->unk2C += gCurTask->unk28;
        gCurTask->posX += gCurTask->unk2C;
        gCurTask->pixelX = gCurTask->posX >> 16;
    }
    if (gCurTask->player->hitBoxSet != 0)
    {
        gCurTask->player->blocksBroken = TaskBreakBlocks(gCurTask->player->hitBoxSet, gCurTask->player->playerIndex);
        if (gCurTask->player->blocksBroken != 0)
            gCurTask->unk76 |= 1;
    }
    else
    {
        gCurTask->player->blocksBroken = 0;
    }
    gCurTask->player->prevWaterFlags = gCurTask->waterFlags;
    gCurTask->player->unk4E = 0xFFFF;
    if (gCurTask->player->terrainBox != 0)
    {
        PlayerProbeTerrain(gCurTask->player->terrainBox);
        gCurTask->player->boundsClamp = gTerrainBoundsClamp;
        if (gUnk_02005574[0] == 0 && (gTerrainBoundsClamp & 4) && gTerrainResult.unk0 != 0)
            gCurTask->player->unk4E = gUnk_03005544;
        gCurTask->player->prevTerrainBox = (u32 *)gCurTask->player->terrainBox;
        if (gTerrainResult.damage != 0 && !(gCurTask->player->unk42 & 0x200)
         && gCurTask->player->invulnerability != 1 && gCurTask->player->invincible == 0)
        {
            r = AddPlayerHealth(-8, gCurTask->player->playerIndex);
            if (r != 0)
            {
                gCurTask->hitEffect = gTerrainResult.damage | 0x80;
                gCurTask->hitKind = 2;
            }
            else
            {
                gCurTask->hitKind = 1;
                gCurTask->hitEffect = 0;
                goto post;
            }
        }
    }
    else
    {
        gTerrainResult.unk0 = gTerrainResult.ceilingHits = gTerrainResult.unk2 = 0;
        gTerrainResult.unk3 = gTerrainResult.slope = gTerrainResult.unk5 = 0;
        gTerrainResult.unk8 = gTerrainResult.onSlipperyFloor = gTerrainResult.damage = 0;
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
    sub_0803fb54();
    if (!(gCurTask->player->unk42 & 32))
        sub_0803e080();
    if ((gMetaKnightmareMode == 1 || gUnk_0300244C != 0)
     && (gCurTask->player->unk40 & 1) && (gCurTask->skipMask & 1))
        goto check;
    if (gCurTask->velY >= 0)
    {
        if (gCurTask->waterFlags & 0x80)
            CreatePlayerEffect(gCurTask->player->playerIndex, 9, gTerrainResult.unk8);
    }
    else if (gCurTask->player->mouthState == 2)
    {
        if (gCurTask->waterFlags & 0x80)
            CreatePlayerEffect(gCurTask->player->playerIndex, 10, gTerrainResult.unk8);
    }
    else if ((gCurTask->player->prevWaterFlags & 1) && !(gCurTask->waterFlags & 1))
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, 10, gTerrainResult.unk8);
    }
    if ((gCurTask->waterFlags & 65) == 1)
    {
        if (--gPlayerBubbleTimers[gCurTask->player->playerIndex] == 0)
        {
            gPlayerBubbleTimers[gCurTask->player->playerIndex] = RandomRange(90) + 120;
            CreatePlayerEffectHighSlot(gCurTask->player->playerIndex, 11, 0);
        }
    }
    else
    {
        gPlayerBubbleTimers[gCurTask->player->playerIndex] = 60;
    }
check:
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->ability == 10)
        {
            x = gCurTask->frame - 0x808;
            if (x >= 0 && LoadPlayerBodyBoxRect(gCurTask->player->playerIndex, (u8 *)gUnk_0873C36C + x * 8) != 0)
            {
                if (gCurTask->frame <= 0x8D1)
                {
                    struct M11R20 *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->player->playerIndex])[12] = 2;
                }
                else
                {
                    struct M11R20 *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->player->playerIndex])[12] = 5;
                }
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
            x = gCurTask->frame - 0x8D2;
            if (x >= 0 && LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CF9C + x * 8)) != 0)
                TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex], gCurTask->player->playerIndex);
        }
        if (gUnk_02005E00.unk04[gCurTaskIdx] & 1)
        {
            if ((gPlayerHeldKeys[gCurTask->player->playerIndex] & 0x300) == 0x300)
                gUnk_02005E00.unk04[gCurTaskIdx] = (gUnk_02005E00.unk04[gCurTaskIdx] & 0xF0) | 2;
        }
    }
    else if (gCurTask->player->unk10 != 0)
    {
        gCurTask->player->unk10--;
    }
tail:
    if (gCurTask->player->unk37 != 2 && gLocalPlayer == gCurTask->player->playerIndex)
        SetCameraFocus(gCurTask->posX >> 16, gCurTask->posY >> 16);
    if (gCurTask->player->terrainBox != 0 && !(gCurTask->skipMask & 2))
    {
        gCurTask->player->prevPixelX = gCurTask->pixelX;
        gCurTask->player->prevPixelY = gCurTask->pixelY;
    }
}

void sub_0803332c(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    if (t->player->requestedAction == 0)
    {
        if (t->unk76 & 1)
        {
            t->unk76 &= 0xFFFE;
            if ((s8)t->player->unk2B == 0)
            {
                PlayerStartOffsetScript(0);
                gCurTask->player->unk2B = 10;
            }
        }
        p = gCurTask->player;
        if (p->unk40 & 1)
        {
            sub_0803cbd8();
            p = gCurTask->player;
            if (!(p->unk40 & 1) && gMetaKnightmareMode == 0 && p->mode == 7 && p->blocksBroken == 0)
            {
                TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
                gCurTask->player->requestedAction = 18;
            }
        }
        else if ((s8)p->unk2B != 0)
        {
            p->unk2B--;
        }
    }
    if (gCurTask->player->requestedAction == 0)
        PlayerCheckShareItem();
}

void sub_08033414(void)
{
    struct Task *t;
    struct PlayerState *p;

    if (gCurTask->player->requestedAction > 31 && gUnk_02007CF0 == 1)
        gCurTask->player->requestedAction = 0;
    switch (gCurTask->hitKind)
    {
    default:
        if (gMetaKnightmareMode == 0 && (gCurTask->player->unk40 & 32))
        {
            if ((s16)gPlayerAbilities[gCurTask->player->playerIndex] != 0)
            {
                gSavedPlayerAbilities[gCurTask->player->playerIndex] = gPlayerAbilities[gCurTask->player->playerIndex];
                gSavedPlayerAbilityUses[gCurTask->player->playerIndex] = gPlayerAbilityUses[gCurTask->player->playerIndex];
            }
            else
            {
                gSavedPlayerAbilities[gCurTask->player->playerIndex] = 4;
                gSavedPlayerAbilityUses[gCurTask->player->playerIndex] = 0xFFFF;
            }
            gCurTask->player->pendingAbility = 4;
            gCurTask->player->pendingAbilityUses = 255;
            gCurTask->player->unk37 = 1;
            gCurTask->player->requestedAction = 29;
        }
        else if (gCurTask->player->unk40 & 64)
        {
            SetPlayerInvulnerability(5, 0, gCurTask->player->playerIndex);
            gCurTask->player->unk40 &= 0xFFBF;
            PlayBgm(19);
            PlayerStartItemShare(gCurTask->player->playerIndex, 3);
        }
        break;
    case 1:
        gCurTask->player->requestedAction = 17;
        gCurTask->player->unk22 = 0;
        gCurTask->player->unk1E = gCurTask->player->unk20 = 0;
        break;
    case 2:
        if (gCurTask->player->unk37 != 2)
        {
            gCurTask->player->requestedAction = 16;
            gCurTask->player->unk16 = 255;
        }
        else
        {
            gCurTask->player->requestedAction = 58;
            gCurTask->variant = 3;
        }
        gCurTask->player->unk22 = 0;
        gCurTask->player->unk1E = gCurTask->player->unk20 = 0;
        break;
    }
    gCurTask->hitKind = 0;
    if (gCurTask->player->requestedAction != 0)
    {
        if (gCurTask->player->unk40 & 1)
        {
            gCurTask->player->offsetScriptStep = 0;
            gCurTask->player->offsetScriptDelay = 1;
            gCurTask->player->unk2B = 0;
            gCurTask->player->pixelOffsetX = gCurTask->player->pixelOffsetY = 0;
            TaskSetSkipMask(0, gCurTaskIdx);
        }
        if (gCurTask->player->sfxPlayer != -1)
            PlayerStopSfx();
        gCurTask->unk76 = 0;
        gCurTask->player->unk40 = 0;
        gCurTask->player->unk50 = 0;
        gCurTask->u80.attackAbility = 0;
    }
    else if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->unk76 & 2)
        {
            gCurTask->player->unk40 |= 2;
            gCurTask->variant = 1;
            gCurTask->player->requestedAction = 8;
            gCurTask->unk76 &= 0xFFFD;
        }
        if (!(gCurTask->player->unk40 & 128))
            sub_0803ce98();
    }
    if (gCurTask->player->requestedAction != 0)
        TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->unk40 & 4)
        {
            if (gCurTask->player->ability == 0)
                sub_0803c9b4(0);
            else
                sub_0803c9b4(1);
        }
        if ((s8)gCurTask->player->unk36 != 0)
        {
            switch (gCurTask->player->prevAction)
            {
            case 32:
            case 33:
                CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
                sub_08049a58();
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
