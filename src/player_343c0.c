#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_343c0.c (0x080343C0-0x08034F8B, issue #92).
 *
 * Player mode bodies, part 2.  The player task starts the "enter"
 * coroutine of the requested action PlayerState.action from
 * gPlayerActions[62] and every frame the "per-frame" handler Task.updateState
 * from gPlayerActionHandlers[57] (CallTableEntry(index, count, table); entry 0 of
 * both tables is NULL).  Here: actions 3-6 and 22.
 * PlayerActionRun enters mode 2 (handler 3, PlayerActionRunUpdate), PlayerActionSkid
 * mode 3 (handler 4, PlayerActionSkidUpdate), PlayerActionJump and PlayerActionReleaseJump mode 4
 * (handlers 5 and 6, PlayerActionJumpUpdate and PlayerActionReleaseJumpUpdate); sub_08034f70
 * (action 22) clears PlayerState.terrainBox and runs PlayerActionFall, the
 * mode-5 coroutine of the next file.  The enter coroutines switch on the
 * ability PlayerState.ability for the animation (TaskSetFrame) and loop
 * on TaskYieldTrampoline; the handlers run M11's transition predicates
 * and write the next request into PlayerState.requestedAction. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionRun(void)
{
    struct Task *t;
    struct PlayerState *p;
    u16 *q;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 2;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_RUN;
    t = gCurTask;
    t->unk28 = 0;
    t->unk2C = -1;
    PlayerSetMotionXPreset(3, 72);
    if (gCurTask->player->prevMode != 2)
    {
        gCurTask->player->savedWallSide = 0;
        q = gLatchedHeldKeys;
        p = gCurTask->player;
        if (q[p->playerIndex] & 48)
        {
            if (p->bumpKind == 2)
                p->bumpKind = 0;
        }
        PlayerPlayBump();
        PlayerStartSfx(117, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 7, 0);
        if (PlayerGetFacingSlope(gCurTask->player->playerIndex) == 4)
            gCurTask->variant = 1;
        else
            gCurTask->variant = 0;
    }
    if (gCurTask->variant == 0)
    {
        if (gCurTask->player->mouthState == 1)
        {
            while (1)
            {
                TaskSetFrame(0x153);
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount = 0;
                do
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->playerLoopCount++;
                } while ((s16)gCurTask->playerLoopCount <= 3);
                TaskSetFrame(0x148);
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount = 0;
                do
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->playerLoopCount++;
                } while ((s16)gCurTask->playerLoopCount <= 9);
            }
        }
        else
        {
            gCurTask->playerBaseFrame = gUnk_0873D31C[gCurTask->player->ability];
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount = 0;
                do
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(3);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->playerLoopCount++;
                } while ((s16)gCurTask->playerLoopCount <= 2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
    }
    CreatePlayerEffect(gCurTask->player->playerIndex, 6, 0x200);
    if (gCurTask->player->mouthState == 1)
        gCurTask->playerBaseFrame = 0x15D;
    else
        gCurTask->playerBaseFrame = gUnk_0873D350[gCurTask->player->ability];
    switch (gCurTask->player->ability)
    {
    case ABILITY_FIRE:
    case ABILITY_SPARK:
    case ABILITY_BURNING:
    case ABILITY_TORNADO:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    }
}

void PlayerActionRunUpdate(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 m5;
    s32 y;
    s32 m3;

    t = gCurTask;
    if (t->unk28 == 0)
    {
        m2 = gTerrainResult.unkD;
        if (m2 != 0)
        {
            t->player->unk14 = 5;
            t->unk28 = 1;
        }
        else
        {
            t->player->unk14 = m2;
        }
    }
    else
    {
        p = t->player;
        if ((s16)p->unk14 == 0)
        {
            if (IsFullBlockAtPixel(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gCurTask->onGround = 1;
        }
        else
        {
            p->unk14--;
        }
    }
    t = gCurTask;
    if ((t->onGround & 1) != 0 || (t->player->boundsClamp & 3) != 0)
    {
        t->unk28 = 0;
        t->player->unk14 = 0;
    }
    while (PlayerCheckSkid() == 0 && PlayerCheckJump() == 0)
    {
        if (gCurTask->unk28 == 0 && PlayerCheckFallOrWater() != 0)
            break;
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (PlayerCheckLadder() != 0)
            break;
        if (PlayerCheckDuckOrSwallow() != 0)
            goto end;
        if (PlayerCheckFloat() != 0)
            goto end;
        if (PlayerCheckBButton() != 0)
            goto end;
        if (PlayerCheckDropAbility() != 0)
            goto end;
        q = gLatchedHeldKeys;
        t3 = gCurTask;
        p4 = t3->player;
        m = q[p4->playerIndex] & 48;
        if (m == 0)
        {
            x = abs(t3->velX);
            if ((u32)x <= 0x14BFF)
            {
                p4->running = m;
                gCurTask->player->requestedAction = PLAYER_ACTION_WALK;
                goto end;
            }
        }
        m5 = gTerrainResult.unk0;
        if (m5 != 0)
        {
            PlayerCheckBump();
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
            goto end;
        }
        if (gCurTask->variant == 0)
        {
            if (PlayerGetFacingSlope(gCurTask->player->playerIndex) == 4)
            {
                gCurTask->variant = 1;
                TaskSetEntry(PlayerActionRun, gCurTaskIdx);
            }
        }
        else if (PlayerGetFacingSlope(gCurTask->player->playerIndex) != 4)
        {
            gCurTask->variant = m5;
            TaskSetEntry(PlayerActionRun, gCurTaskIdx);
        }
        goto end;
    }
end:
    PlayerSetMotionXPreset(3, 72);
}

void PlayerActionSkid(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 3;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SKID;
    PlayerSetMotionXPreset(4, 72);
    if (gCurTask->player->prevMode != 3)
    {
        PlaySfxIfLocalPlayer(119, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 6, 0);
    }
    if (gCurTask->player->mouthState == 1)
        gCurTask->playerBaseFrame = 0x15D;
    else
        gCurTask->playerBaseFrame = gUnk_0873D350[gCurTask->player->ability];
    switch (gCurTask->player->ability)
    {
    case ABILITY_FIRE:
    case ABILITY_SPARK:
    case ABILITY_BURNING:
    case ABILITY_TORNADO:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    }
}

void PlayerActionSkidUpdate(void)
{
    while (PlayerCheckJump() == 0 && PlayerCheckFallOrWater() == 0 && PlayerCheckBButton() == 0 && PlayerCheckDropAbility() == 0)
    {
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
                {
                    gCurTask->facing = 1;
                }
                gCurTask->accelX = 0;
                if (gCurTask->player->running == 0)
                    gCurTask->player->requestedAction = PLAYER_ACTION_WALK;
                else
                    gCurTask->player->requestedAction = PLAYER_ACTION_RUN;
            }
            else
            {
                gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
            }
            if (gCurTask->player->requestedAction != PLAYER_ACTION_NONE)
                break;
        }
        if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
        }
        break;
    }
}

void PlayerActionJump(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 4;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_JUMP;
    if (gCurTask->player->prevMode != 4)
    {
        if (gCurTask->player->prevMode == 9)
            gCurTask->player->unk14 = 4;
        else
            gCurTask->player->unk14 = 23;
        PlayerSetMotionYPreset(0);
        PlaySfxIfLocalPlayer(SE_JUMP, gCurTask->player->playerIndex);
        gCurTask->variant = 0;
    }
    PlayerPlayBump();
    if (gCurTask->player->mouthState == 1)
    {
        TaskSetFrame(0x158);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskSleepForever();
    }
    gCurTask->playerBaseFrame = gUnk_0873D384[gCurTask->player->ability];
    switch (gCurTask->player->ability)
    {
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    case ABILITY_FIRE:
    case ABILITY_SPARK:
    case ABILITY_SWORD:
    case ABILITY_BURNING:
    case ABILITY_HAMMER:
    case ABILITY_PARASOL:
    case ABILITY_HI_JUMP:
    case ABILITY_BEAM:
    case ABILITY_STONE:
    case ABILITY_TORNADO:
    case ABILITY_BACKDROP:
    case ABILITY_THROW:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
}

void PlayerActionJumpUpdate(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckLadder() == 0 && PlayerCheckBButton() == 0 && PlayerCheckDropAbility() == 0 && PlayerCheckEnterDoor() == 0)
    {
        if (gCurTask->onGround & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            goto end;
        }
        if (gTerrainResult.ceilingHits != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
            break;
        }
        switch (gCurTask->variant)
        {
        case 0:
            if (--gCurTask->player->unk14 == 0
                || (gLatchedHeldKeys[gCurTask->player->playerIndex] & 1) == 0)
            {
                gCurTask->variant = 1;
                PlayerSetMotionYPreset(1);
                gCurTask->player->unk14 = 6;
            }
            break;
        case 1:
            if (PlayerCheckAirFloat() == 0 && --gCurTask->player->unk14 == 0)
            {
                PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
                gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
            }
            break;
        }
        if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            if (gCurTask->player->bumpKind & 7)
                TaskSetEntry(PlayerActionJump, gCurTaskIdx);
        }
        break;
    }
end:
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void PlayerActionReleaseJump(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 4;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_RELEASE_JUMP;
    if (gCurTask->player->prevMode != 4)
    {
        gCurTask->variant = 0;
        PlayerSetMotionYPreset(0);
        PlaySfxIfLocalPlayer(SE_JUMP, gCurTask->player->playerIndex);
    }
    gCurTask->playerBaseFrame = gUnk_0873D384[gCurTask->player->ability];
    switch (gCurTask->player->ability)
    {
    case ABILITY_FIRE:
    case ABILITY_SPARK:
    case ABILITY_SWORD:
    case ABILITY_BURNING:
    case ABILITY_HAMMER:
    case ABILITY_PARASOL:
    case ABILITY_HI_JUMP:
    case ABILITY_BEAM:
    case ABILITY_STONE:
    case ABILITY_TORNADO:
    case ABILITY_BACKDROP:
    case ABILITY_THROW:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    }
}

void PlayerActionReleaseJumpUpdate(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckBButton() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->onGround & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult.ceilingHits != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
            break;
        }
        switch (gCurTask->variant)
        {
        case 0:
            if (--gCurTask->player->unk14 == 0)
            {
                gCurTask->variant = 1;
                PlayerSetMotionYPreset(1);
                gCurTask->player->unk14 = 5;
            }
            break;
        case 1:
            if (--gCurTask->player->unk14 == 0)
            {
                PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
                gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
            }
            break;
        }
        if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            if (gCurTask->player->bumpKind & 7)
                TaskSetEntry(PlayerActionJump, gCurTaskIdx);
        }
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void sub_08034f70(void)
{
    gCurTask->player->terrainBox = 0;
    PlayerActionFall();
}
