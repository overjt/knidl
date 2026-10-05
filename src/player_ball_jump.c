#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_ball_jump.c (0x0804EE08-0x0804F613, issue #90).
 *
 * Action 49's sub-actions 4-8 (gPlayerBallVariants) and sub-handlers 13-17
 * (gPlayerBallVariantUpdates), each sub-action followed by its sub-handler: more
 * attacks of the same move set (sounds 168-170, 183, 184, 202 and 246,
 * M11's PlayerSetMotionYPreset steering, effect 44), the last one (PlayerBallRevert)
 * installing the hit boxes gPlayerDefaultBodyBox/gPlayerDefaultTerrainBox in PlayerState.
 * Sub-handler 17 (PlayerBallRevertUpdate, a push-less leaf) requests action 7, 20
 * or 23 from the ground flag and the key state. */

void TaskSetEntry(void *a, u32 i);
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerBallJump(void)
{
    s32 i;

    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        t->player->playerJumpPhaseTimer = 0xFFFF;
        t->player->running = 1;
    }
    {
        struct Task *t = gCurTask;
        t->playerBallJumpSavedRollFrame = t->playerBallRollFrame;
        t->playerBallRollFrame = -1;
        t->frame = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;
        t->playerBallRollFrame = t->playerBallJumpSavedRollFrame;
        PlaySfxIfLocalPlayer(170, (u16)t->player->playerIndex);
    }
    PlayerSetMotionYPreset(43);
    gCurTask->player->playerJumpPhaseTimer = 23;
    while (1)
    {
        if (--gCurTask->player->playerJumpPhaseTimer == 0 || !(gLatchedHeldKeys[gCurTask->player->playerIndex] & 1))
            break;
        PlayerBallStepRoll();
        TaskYieldTrampoline(1);
    }
    PlayerSetMotionYPreset(44);
    for (i = 4; i >= 0; i--)
    {
        PlayerBallStepRoll();
        TaskYieldTrampoline(1);
    }
    gCurTask->playerActionDone++;
    PlayerStopAxes(2);
    TaskSleepForever();
}

void PlayerBallJumpUpdate(void)
{
    if ((s16)gCurTask->player->playerJumpPhaseTimer == -1)
        return;
    PlayerTurnToHeldDirection();
    while (!PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (gTerrainResult.ceilingHits != 0 || (gCurTask->player->boundsClamp & 4))
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            if (t->player->bumpKind == 1)
            {
                t->playerBallBumpVelY = t->velY;
                t->velY = 0;
            }
            else
            {
                t->velY = -t->velY;
                t->speedLimitY = 0x50000;
            }
            gCurTask->variant = 6;
        }
        else
        {
            if (gTerrainResult.unk0 != 0)
            {
                struct Task *t = gCurTask;
                t->velX = -t->velX;
                t->accelX = -t->accelX;
                t->facing = -t->facing;
            }
            if ((s16)gCurTask->playerActionDone != 0)
                gCurTask->variant = 6;
        }
        break;
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_JUMP)
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    PlayerSetMotionXPreset(12, 0);
}

void PlayerBallBounce(void)
{
    {
        struct Task *t = gCurTask;
        t->playerBallBounceVelY = t->velY;
        t->player->running = 1;
    }
    PlayerStopAxes(2);
    {
        struct Task *t = gCurTask;
        t->playerBallBounceSavedRollFrame = t->playerBallRollFrame;
        t->playerBallRollFrame = 0xFFFF;
        t->frame = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;
        t->playerBallRollFrame = t->playerBallBounceSavedRollFrame;
        t->onGround = 0;
    }
    gCurTask->velY = gCurTask->playerBallBounceVelY;
    PlayerSetMotionYPreset(46);
    {
        struct Task *t = gCurTask;
        if (abs(t->playerBallBounceVelY) <= 0x1FFFF)
            PlaySfxIfLocalPlayer(184, (u16)t->player->playerIndex);
        else if (abs(t->playerBallBounceVelY) <= 0x2FFFF)
            PlaySfxIfLocalPlayer(183, (u16)t->player->playerIndex);
        else if (abs(t->playerBallBounceVelY) <= 0x3FFFF)
            PlaySfxIfLocalPlayer(170, (u16)t->player->playerIndex);
        else if (abs(t->playerBallBounceVelY) <= 0x57FFF)
            PlaySfxIfLocalPlayer(169, (u16)t->player->playerIndex);
        else
            PlaySfxIfLocalPlayer(168, (u16)t->player->playerIndex);
    }
    PlayerBallPlayBump();
    while (1)
    {
        PlayerBallStepRoll();
        TaskYieldTrampoline(1);
    }
}

void PlayerBallBounceUpdate(void)
{
    if (gCurTask->velY == 0)
        return;
    PlayerTurnToHeldDirection();
    while (!PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (gTerrainResult.ceilingHits != 0 || (gCurTask->player->boundsClamp & 4))
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            if (t->player->bumpKind == 1)
            {
                if (-t->velY > 0x57FFF)
                    PlaySfxIfLocalPlayer(246, (u16)t->player->playerIndex);
                gCurTask->playerBallBumpVelY = gCurTask->velY;
                gCurTask->velY = 0;
            }
            else
            {
                t->velY = -t->velY;
                t->speedLimitY = 0x50000;
            }
            gCurTask->variant = 6;
        }
        else
        {
            if (gTerrainResult.unk0 != 0)
            {
                struct Task *t;
                PlayerCheckBump();
                t = gCurTask;
                t->velX = -t->velX;
                t->accelX = -t->accelX;
                t->facing = -t->facing;
            }
            if (gCurTask->velY >= 0)
                gCurTask->variant = 6;
        }
        break;
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_BOUNCE)
    {
        struct Task *t = gCurTask;
        t->playerBallPosePlaying = 0;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 0);
}

void PlayerBallFall(void)
{
    gCurTask->player->running = 1;
    PlayerSetMotionYPreset(45);
    PlayerBallPlayBump();
    while (1)
    {
        PlayerBallStepRoll();
        TaskYieldTrampoline(1);
    }
}

void PlayerBallFallUpdate(void)
{
    PlayerTurnToHeldDirection();
    if (!PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        else if (gCurTask->onGround & 1)
            gCurTask->variant = 7;
        else if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
        }
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_FALL)
    {
        struct Task *t = gCurTask;
        t->playerBallPosePlaying = 0;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 0);
}

void PlayerBallLand(void)
{
    s32 flag;
    s32 m;

    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        t->playerBallReboundVelY = t->velY;
    }
    PlayerBallCheckLanding(1);
    flag = 0;
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 1)
    {
        flag = 1;
        m = 358;
    }
    else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
    {
        flag = 1;
        m = 76;
    }
    else
        m = 179;
    {
        struct Task *t = gCurTask;
        t->playerBallReboundVelY = m = m * (t->playerBallReboundVelY >> 8);
        if (m <= 0x3FFF)
            flag = 0;
        if (m > 0x57FFF)
            PlaySfxIfLocalPlayer(246, (u16)t->player->playerIndex);
    }
    if (flag)
    {
        struct Task *t = gCurTask;
        t->playerBallPosePlaying = 1;
        t->frame = 0xCE8;
        TaskYieldTrampoline(2);
    }
    else
    {
        struct Task *t = gCurTask;
        if (t->playerBallReboundVelY <= 0x17FFF)
            t->playerBallReboundVelY = 0;
    }
    gCurTask->playerActionDone++;
    TaskSleepForever();
}

void PlayerBallLandUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        t->playerBallPosePlaying = 0;
        if (t->playerBallReboundVelY != 0)
        {
            t->variant = 5;
            if ((u32)gCurTask->playerBallReboundVelY > 0x80000)
                gCurTask->playerBallReboundVelY = 0x80000;
            gCurTask->velY = -gCurTask->playerBallReboundVelY;
            gCurTask->speedLimitY = 0x80000;
        }
        else if (t->velX != 0)
            t->variant = 2;
        else
            t->variant = 1;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
}

void PlayerBallRevert(void)
{
    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        t->playerBallRollFrame = 0xFFFF;
        if (t->playerBallEnterDoor != 0)
        {
            PlayerStopAxes(3);
            RequestScreenShake(0);
            FreezeOtherTasks((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE | TASK_SKIP_LATE_UPDATE));
        }
    }
    if (gCurTask->onGround & 1)
    {
        gCurTask->frame = 0xCE9;
        TaskYieldTrampoline(2);
        if (gCurTask->playerBallEnterDoor == 0)
            PlayerSetMotionYPreset(48);
        gCurTask->frame = 0xCE7;
        TaskYieldTrampoline(3);
    }
    gCurTask->frame = 0xCD2;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x500);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x501);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x502);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x503);
    PlaySfxIfLocalPlayer(202, (u16)gCurTask->player->playerIndex);
    {
        struct Task *t = gCurTask;
        t->facing = t->playerBallRollFacing;
    }
    {
        struct Task *t = gCurTask;
        t->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        t->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        t->playerActionDone++;
    }
    TaskSleepForever();
}

void PlayerBallRevertUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        if (t->playerBallEnterDoor != 0)
            t->player->requestedAction = PLAYER_ACTION_ENTER_DOOR;
        else
            t->player->requestedAction = PLAYER_ACTION_FALL;
    }
    if (gCurTask->player->requestedAction == PLAYER_ACTION_NONE && (gCurTask->waterFlags & 1))
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
}
