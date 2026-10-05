#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_ball_roll.c (0x0804E78C-0x0804EE07, issue #90).
 *
 * Action 49's sub-actions 0-3 (gPlayerBallVariants) and sub-handlers 9-12
 * (gPlayerBallVariantUpdates), each sub-action followed by its sub-handler.  The
 * sub-actions are yield scripts (PlayerBallTransform: sound 171, animations
 * 0xCCA-0xCDA and effect 44 four times; PlayerBallSkid: sound 119,
 * effect 6 and camera preset PlayerSetMotionXPreset(11, 62)); the sub-handlers read
 * the keys and the ground flag Task.onGround, pick the next sub-action
 * (Task.variant) and re-bind it through PlayerActionBallEnterVariant, most of them through
 * the helper PlayerBallCheckVariant's four key probes. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerBallTransform(void)
{
    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        t->playerBallRollFrame = 0xFFFF;
        t->player->bodyBox = (u32)gUnk_0873BD50;
        t->player->terrainBox = (u32)gUnk_0873CB3C;
        PlaySfxIfLocalPlayer(171, (u16)t->player->playerIndex);
    }
    if (gCurTask->onGround & 1)
    {
        PlayerSetMotionYPreset(47);
        TaskSetFrame(0xCCA);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(0xCCC);
    TaskYieldTrampoline(2);
    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    gCurTask->frame = 0xCDA;
    TaskYieldTrampoline(2);
    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 5; gCurTask->playerLoopCount++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    {
        struct Task *t = gCurTask;
        t->playerBallRollFacing = t->facing;
        t->playerActionDone++;
        CreatePlayerEffect(t->player->playerIndex, 44, 0x100);
    }
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x200);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x300);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x400);
    LoadAbilityTiles();
    TaskSleepForever();
}

void PlayerBallTransformUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        if (t->onGround & 1)
        {
            if (gLatchedHeldKeys[t->player->playerIndex] & 1)
                t->variant = 4;
            else
                t->variant = 1;
        }
        else
            t->variant = 6;
        gCurTask->playerBallRollFrame = 0;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
    PlayerBallCheckLanding(0);
}

void PlayerBallStand(void)
{
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->playerBallStandWallSide = t->player->wallSide;
    }
    PlayerBallPlayBump();
    {
        struct Task *t = gCurTask;
        if (t->playerBallRollFrame == 0)
            t->playerBallStandRollDir = 0;
        else if (t->playerBallRollFrame <= 7)
            t->playerBallStandRollDir = -1;
        else
            t->playerBallStandRollDir = 1;
    }
    if (gCurTask->playerBallRollFrame != 0)
    {
        do
        {
            struct Task *t = gCurTask;
            t->playerBallRollFrame += t->playerBallStandRollDir;
            if (t->playerBallRollFrame > 15)
                t->playerBallRollFrame = 0;
            TaskYieldTrampoline(1);
        } while (gCurTask->playerBallRollFrame != 0);
    }
    while (1)
    {
        gCurTask->playerBallRollFrame = 16;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame++;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame++;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame++;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame++;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame--;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame--;
        TaskYieldTrampoline(3);
        gCurTask->playerBallRollFrame--;
        TaskYieldTrampoline(3);
    }
}

void PlayerBallStandUpdate(void)
{
    PlayerFaceHeldDirection();
    while (!PlayerBallCheckVariant(0) && !PlayerBallCheckVariant(1) && !PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) && gCurTask->playerBallStandWallSide == 1)
                break;
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->playerBallStandWallSide == 2)
                break;
            gCurTask->variant = 2;
        }
        break;
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_STAND)
    {
        struct Task *t = gCurTask;
        t->playerBallPosePlaying = 0;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
}

void PlayerBallRoll(void)
{
    PlayerBallPlayBump();
    PlayerSetMotionXPreset(12, 1);
    gCurTask->playerBallRollDelay = PlayerBallGetRollDelay();
    gCurTask->playerBallRollSlope = gCurTask->player->slope;
    while (1)
    {
        PlayerBallStepRoll();
        TaskYieldTrampoline(gCurTask->playerBallRollDelay);
    }
}

void PlayerBallRollUpdate(void)
{
    while (!PlayerBallCheckVariant(0) && !PlayerBallCheckVariant(1) && !PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
            gCurTask->playerBallPosePlaying = 0;
            TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
            break;
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
        {
            if (gCurTask->facing == -1)
            {
                gCurTask->variant = 3;
                break;
            }
        }
        else if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->facing == 1)
        {
            gCurTask->variant = 3;
            break;
        }
        if (gCurTask->velX == 0 && gCurTask->speedLimitX == 0)
        {
            gCurTask->variant = 1;
            break;
        }
        if (gCurTask->playerBallRollSlope != gCurTask->player->slope)
        {
            gCurTask->playerBallPosePlaying = 0;
            TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
        }
        break;
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_ROLL)
    {
        struct Task *t = gCurTask;
        t->playerBallPosePlaying = 0;
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 1);
    gCurTask->playerBallRollDelay = PlayerBallGetRollDelay();
}

void PlayerBallSkid(void)
{
    PlayerSetMotionXPreset(11, 62);
    PlaySfxIfLocalPlayer(119, (u16)gCurTask->player->playerIndex);
    gCurTask->playerBallSkidDustSlot = CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 60);
    TaskSleepForever();
}

void PlayerBallSkidUpdate(void)
{
    while (!PlayerBallCheckVariant(0) && !PlayerBallCheckVariant(1) && !PlayerBallCheckVariant(3) && !PlayerBallCheckVariant(2))
    {
        if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
            gCurTask->variant = 2;
            break;
        }
        if (gCurTask->velX == 0)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
                if (gCurTask->facing == 1)
                {
                    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 32)
                        gCurTask->facing = -1;
                }
                else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                    gCurTask->facing = 1;
                PlayerUpdateFlip();
                gCurTask->accelX = 0;
                gCurTask->variant = 2;
            }
            else
                gCurTask->variant = 1;
        }
        break;
    }
    if (gCurTask->variant != PLAYER_BALL_VARIANT_SKID)
    {
        TaskSetEntry(PlayerActionBallEnterVariant, gCurTaskIdx);
        gTasks[gCurTask->playerBallSkidDustSlot].playerEffectStopRequested = -1;
    }
}
