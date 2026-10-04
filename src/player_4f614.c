#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_4f614.c (0x0804F614-0x0804F947, issue #90).
 *
 * Helpers of action 49's sub-actions.  PlayerBallPlayBump plays the reaction
 * PlayerState.bumpKind asks for (effect 4 through CreatePlayerEffect, then one of
 * hit poses 0xCEA/0xCEB, mirrored through Task.spriteFlags bit 15), PlayerBallGetRollDelay
 * picks a speed class 2-4 from |Task.velX|, PlayerBallStepRoll steps the
 * 16-step direction Task.unk46 with the sign of Task.velX, PlayerBallCheckVariant is the
 * four key probes the sub-handlers share (mode 0-3 -> next sub-action 4, 6
 * or 8) and PlayerBallCheckLanding the landing check (M11's PlayerCheckLanding, effect 4
 * on a fast landing). */

s32 PlayerBallPlayBump(void)
{
    u8 k = gCurTask->player->bumpKind;

    if (k != 0)
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
        gCurTask->player->bumpKind = 0;
        gCurTask->unk24 = 1;
        switch (k)
        {
        case 1:
            gCurTask->frame = 0xCEB;
            TaskYieldTrampoline(2);
            {
                struct Task *t = gCurTask;
                t->velY = -t->unk20;
                t->speedLimitY = 0x50000;
            }
            break;
        case 2:
            break;
        case 3:
        case 4:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            if (gCurTask->tileWord & 0x8000)
            {
                TaskSetFrameNoFlip(0xCEA);
                TaskYieldTrampoline(2);
                gCurTask->spriteFlags |= 0x8000;
            }
            else
            {
                gCurTask->frame = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        case 5:
        case 6:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            if (!(gCurTask->tileWord & 0x8000))
            {
                TaskSetFrameFlip(0xCEA);
                TaskYieldTrampoline(2);
                gCurTask->spriteFlags &= 0x7FFF;
            }
            else
            {
                gCurTask->frame = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        }
        gCurTask->unk24 = 0;
    }
}

s32 PlayerBallGetRollDelay(void)
{
    s32 v = abs(gCurTask->velX);
    s32 r;

    if (v <= 0xFFFF)
        r = 4;
    else if (v <= 0x20000)
        r = 3;
    else
        r = 2;
    return r;
}

void PlayerBallStepRoll(void)
{
    struct Task *t = gCurTask;

    if (t->unk6E == 1)
    {
        if (t->velX < 0)
        {
            if (--t->playerBallRollFrame < 0)
                t->playerBallRollFrame = 15;
            return;
        }
    }
    else if (t->velX > 0)
    {
        if (--t->playerBallRollFrame < 0)
            t->playerBallRollFrame = 15;
        return;
    }
    if (++t->playerBallRollFrame > 15)
        t->playerBallRollFrame = 0;
}

s32 PlayerBallCheckVariant(s32 a)
{
    s32 r = 0;

    switch (a)
    {
    case 0:
    {
        struct Task *t = gCurTask;
        if ((t->onGround & 1) && !(gLatchedHeldKeys[t->player->playerIndex] & 0x80)
            && (gLatchedPressedKeys[t->player->playerIndex] & 1))
        {
            t->variant = 4;
            r = 4;
        }
        break;
    }
    case 1:
    {
        struct Task *t = gCurTask;
        if (!(t->onGround & 1))
        {
            t->variant = 6;
            r = 6;
        }
        break;
    }
    case 2:
        if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            gCurTask->variant = 8;
            r = 8;
        }
        break;
    case 3:
        if (PlayerCheckEnterDoor() != 0)
        {
            gCurTask->player->requestedAction = PLAYER_ACTION_NONE;
            gCurTask->unk74 = 1;
            gCurTask->variant = 8;
            r = 8;
        }
        break;
    }
    return r;
}

s32 PlayerBallCheckLanding(s32 a0)
{
    if (PlayerCheckLanding())
    {
        if ((gCurTask->waterFlags & 1) == 0
            && (gCurTask->velY & 0xFFFF0000) != 0 && a0 != 0)
            CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}
