#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_46330.c (0x08046330-0x08046BFF, issue #87).
 *
 * Player action bodies, part 15: action 39 and per-frame handler 36.
 * PlayerActionWheel (action 39, mode 13) is a re-entrant five-state machine
 * over Task.variant: state 0 winds up (animation 0x701, effect 6, sound
 * 154), state 1 installs the script gUnk_0873CB34 and the block hit-box
 * set gUnk_0873CDAC and spins in an endless yield loop, state 2 turns
 * round (it negates the facing Task.facing) and goes back to state 1,
 * state 3 finishes the move and state 4 bounces off (sound 153, the
 * screen shake RequestScreenShake(4), velocity preset 36).  Its handler
 * PlayerActionWheelUpdate is what leaves the spin: every frame of state 1 it
 * re-binds the coroutine to state 3 on a newly-pressed B, to state 2
 * when the held direction opposes the facing, and to state 4 when the
 * collision block gTerrainResult reports a hit; it keeps the player on
 * slopes and ledges with M06's terrain probes IsFullBlockAtPixel and
 * IsWaterAtPixel and registers the collider gUnk_0873BF14. */

void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionWheel(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_WHEEL;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            t->playerWheelOnWater = 0;
            if (t->onGround & 1)
                t->playerWheelWasOnGround = 1;
            else
                t->playerWheelWasOnGround = 0;
            gCurTask->unk30 = 0;
            CreatePlayerEffect(gCurTask->player->playerIndex, 34, 0);
            gCurTask->u80.attackAbility = ABILITY_WHEEL;
            gCurTask->variant = 0;
        }
    }
again:
    {
        struct Task *t = gCurTask;
        t->player->hitBoxSet = 0;
        switch (t->variant) {
        case 0:
            PlayerSetMotionXPreset(11, 44);
            TaskSetFrame(0x701);
            TaskYieldTrampoline(4);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 30);
            PlayerSetMotionXPreset(11, 45);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 7);
            PlayerStartSfx(154, gCurTask->player->playerIndex);
            gCurTask->variant = 1;
            PlayerSetMotionXPreset(11, 46);
            /* fallthrough */
        case 1:
            {
                struct Task *u = gCurTask;
                u->player->terrainBox = (u32)gUnk_0873CB34;
                u->player->hitBoxSet = gUnk_0873CDAC;
                SetPlayerInvulnerability(3, 0, u->player->playerIndex);
            }
            while (1) {
                if (gCurTask->onGround & 1 || gCurTask->playerWheelOnWater != 0)
                    CreatePlayerEffect(gCurTask->player->playerIndex, 34, 1);
                TaskSetFrame(0x70A);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
        case 2:
            SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
            PlaySfxIfLocalPlayer(245, gCurTask->player->playerIndex);
            if (gCurTask->onGround & 1 || gCurTask->playerWheelOnWater != 0) {
                CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 4);
                TaskSetFrame(0x70E);
                TaskYieldTrampoline(4);
                PlayerSetMotionXPreset(11, 47);
                gCurTask->facing = -gCurTask->facing;
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                PlayerSetMotionXPreset(11, 48);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                PlayerSetMotionXPreset(11, 49);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                PlayerStopAxes(1);
            } else {
                PlayerSetMotionXPreset(11, 50);
                TaskSetFrame(0x70E);
                TaskYieldTrampoline(4);
                gCurTask->facing = -gCurTask->facing;
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            {
                struct Task *u = gCurTask;
                if (u->onGround & 1 || u->playerWheelOnWater != 0) {
                    CreatePlayerEffect(u->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 4);
                    PlayerSetMotionXPreset(11, 46);
                }
            }
            gCurTask->variant = 1;
            goto again;
        case 3:
            PlayerStopSfx();
            {
                struct Task *u = gCurTask;
                u->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
                SetPlayerInvulnerability(255, 0, u->player->playerIndex);
            }
            PlayerSetMotionXPreset(11, 51);
            TaskSetFrame(0x702);
            TaskYieldTrampoline(1);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 6);
            TaskSetFrame(0x701);
            gCurTask->variant = 5;
            break;
        case 4:
            {
                struct Task *u = gCurTask;
                u->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
                SetPlayerInvulnerability(255, 0, u->player->playerIndex);
            }
            PlayerStopSfx();
            PlaySfxIfLocalPlayer(153, gCurTask->player->playerIndex);
            RequestScreenShake(4);
            gCurTask->onGround = 0;
            PlayerSetMotionXPreset(11, 17);
            PlayerSetMotionYPreset(36);
            while (1) {
                TaskSetFrame(0x702);
                TaskYieldTrampoline(1);
                gCurTask->playerLoopCount = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->playerLoopCount <= 6);
            }
        }
    }
    TaskSleepForever();
}

void PlayerActionWheelUpdate(void)
{
    {
        struct Task *t = gCurTask;
        if (t->unk30 == 0) {
            if (gTerrainResult.overGap != 0) {
                t->player->playerOverGapTimer = 5;
                t->unk30 = 1;
            } else {
                t->player->playerOverGapTimer = 0;
            }
        } else {
            struct PlayerState *p = t->player;
            if ((s16)p->playerOverGapTimer == 0) {
                if (IsFullBlockAtPixel(t->pixelX, (t->pixelY & ~15) + 16) != 0)
                    gCurTask->onGround = 1;
            } else {
                p->playerOverGapTimer--;
            }
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->onGround & 1) {
            t->unk30 = 0;
            t->player->playerOverGapTimer = 0;
        }
    }
    switch (gCurTask->variant) {
    case 0:
    case 3:
        if (gCurTask->onGround & 1) {
            PlayerLand(1);
            PlayerSetMotionXPreset(0, 72);
        } else {
            PlayerSetMotionYPreset(2);
            PlayerSetMotionXPreset(11, 2);
        }
        if (gTerrainResult.unk0 == 0)
            break;
        {
            struct Task *t = gCurTask;
            if (t->variant == 0)
                t->velX = 0;
            else
                PlayerStopAxes(1);
        }
        break;
    case 1:
        {
            u16 k = gLatchedPressedKeys[gCurTask->player->playerIndex] & 2;
            struct Task *t = gCurTask;
            if (k) {
                t->variant = 3;
                TaskSetEntry(PlayerActionWheel, gCurTaskIdx);
            } else {
                if (t->onGround & 1) {
                    t->playerWheelOnWater = 0;
                    PlayerSetMotionXPreset(11, 46);
                } else if (t->playerWheelOnWater == 0) {
                    if (IsWaterAtPixel(t->pixelX, t->pixelY + 15) != 0) {
                        {
                            struct Task *u = gCurTask;
                            if (u->waterFlags & 1)
                                u->pixelY -= 8;
                        }
                        PlayerStopAxes(2);
                        {
                            struct Task *u = gCurTask;
                            u->posY = ((u->pixelY & 0xFFF0) + 5) << 16;
                            u->pixelY = u->posY >> 16;
                            u->playerWheelOnWater = 1;
                        }
                    } else {
                        if (gCurTask->player->boundsClamp & 3)
                            PlayerStopAxes(1);
                        PlayerSetMotionXPreset(11, 50);
                    }
                } else {
                    PlayerSetMotionXPreset(11, 46);
                }
                if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16 && gCurTask->facing == -1)
                    || (gLatchedHeldKeys[gCurTask->player->playerIndex] & 32 && gCurTask->facing == 1)) {
                    gCurTask->variant = 2;
                    TaskSetEntry(PlayerActionWheel, gCurTaskIdx);
                } else if (*(u16 *)&gTerrainResult != 0
                           || ((gCurTask->player->boundsClamp & 3)
                               && ((gCurTask->onGround & 1) || gCurTask->playerWheelOnWater != 0))) {
                    if (gTerrainResult.ceilingHits != 0)
                        gCurTask->velY = 0;
                    else
                        PlayerStopAxes(1);
                    gCurTask->variant = 4;
                    TaskSetEntry(PlayerActionWheel, gCurTaskIdx);
                }
            }
        }
        {
            struct Task *t = gCurTask;
            if ((t->onGround & 1) || t->playerWheelOnWater != 0) {
                PlayerStopAxes(2);
            } else {
                if (t->playerWheelWasOnGround != 0)
                    PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BF14);
        break;
    case 2:
        {
            u16 k = gLatchedPressedKeys[gCurTask->player->playerIndex] & 2;
            if (k) {
                gCurTask->variant = 3;
                TaskSetEntry(PlayerActionWheel, gCurTaskIdx);
                break;
            }
            if (*(u16 *)&gTerrainResult != 0 || (gCurTask->player->boundsClamp != 0 && (gCurTask->onGround & 1))) {
                if (gTerrainResult.ceilingHits != 0)
                    gCurTask->velY = 0;
                else
                    PlayerStopAxes(1);
                gCurTask->variant = 4;
                TaskSetEntry(PlayerActionWheel, gCurTaskIdx);
            }
        }
        if (gCurTask->onGround & 1) {
            gCurTask->playerWheelWasOnGround = 1;
            if (gCurTask->velY != 0)
                PlayerStopAxes(2);
        } else {
            gCurTask->playerWheelWasOnGround = 0;
            PlayerSetMotionYPreset(2);
        }
        break;
    case 4:
        {
            struct Task *t = gCurTask;
            if (t->velY < 0) {
                if (gTerrainResult.ceilingHits != 0)
                    t->velY = 0;
                break;
            }
        }
        /* fallthrough */
    case 5:
        PlayerRequestLocomotion();
        {
            struct PlayerState *p = gCurTask->player;
            if (p->requestedAction == PLAYER_ACTION_WALK)
                p->requestedAction = PLAYER_ACTION_SKID;
        }
        return;
    }
    if (gCurTask->waterFlags & 1) {
        PlayerStopSfx();
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    }
    {
        struct Task *t = gCurTask;
        if (t->onGround & 1)
            t->playerWheelWasOnGround = 1;
        else
            t->playerWheelWasOnGround = 0;
    }
}
