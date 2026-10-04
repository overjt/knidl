#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_337f4.c (0x080337F4-0x080343BF, issue #92).
 *
 * Player mode bodies, part 1.  The six empty handlers sub_080337f4 ...
 * sub_08033808 are entries 59-61 of the action table gPlayerActions and
 * entry 56 of the per-frame table gPlayerActionHandlers (two of them, sub_080337f8
 * and sub_08033800, are dead exports nothing points at).  Then actions 1
 * and 2: PlayerActionStand enters mode 0 (per-frame handler 1, PlayerActionStandUpdate)
 * and PlayerActionWalk mode 1 (handler 2, PlayerActionWalkUpdate).  Their animations
 * come from gPlayerStandFrames[ability][5] (column picked by M11's
 * PlayerGetFacingSlope, row 26 when PlayerState.mouthState == 1) and gUnk_0873D2E8. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void sub_080337f4(void)
{
}

void sub_080337f8(void)
{
}

void sub_080337fc(void)
{
}

void sub_08033800(void)
{
}

void sub_08033804(void)
{
}

void sub_08033808(void)
{
}

void PlayerActionStand(void)
{
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 0;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_STAND;

    if (gCurTask->player->prevMode != 0)
    {
        struct Task *t;
        struct Task *t2;

        PlayerStopAxes(3);
        t = gCurTask;
        t->playerStandSavedClampedTopY = (u16)t->player->clampedTopY;
        t->playerPoseSlope = t->player->slope;
        if (t->player->wallSide != 0)
            t->player->savedWallSide = t->player->wallSide;
        gCurTask->player->running = 0;
        t2 = gCurTask;
        t2->player->actionFlags &= 0xFFEF;
        t2->player->runTapTimer = 0;
        PlayerPlayBump();
    }
    gCurTask->player->facingSlope = PlayerGetFacingSlope(gCurTask->player->playerIndex);
    p = gCurTask->player;
    p->blinkTimer = 0;
    p->blinkScriptPos = 0;
    if (gCurTask->player->mouthState == 1)
        gCurTask->playerBaseFrame = gPlayerStandFrames[26][PlayerGetFacingSlope(gCurTask->player->playerIndex)];
    else
        gCurTask->playerBaseFrame = gPlayerStandFrames[gCurTask->player->ability][PlayerGetFacingSlope(gCurTask->player->playerIndex)];
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
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case ABILITY_HI_JUMP:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
        }
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    }
}

void PlayerActionStandUpdate(void)
{
    s32 dir = gCurTask->facing;
    s32 turn = 0;

    if (PlayerFaceHeldDirection() != 0)
        PlayerUpdateFlip();
    while (PlayerCheckJump() == 0 && PlayerCheckFallOrWater() == 0 && PlayerCheckEnterDoor() == 0
           && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0 && PlayerCheckFloat() == 0
           && PlayerCheckBButton() == 0)
    {
        if (PlayerCheckDropAbility() != 0)
            goto end;
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if (gCurTask->player->savedWallSide != 0)
            {
                if (dir == gCurTask->facing)
                {
                    if (gCurTask->facing == 1)
                    {
                        if (IsFullBlockAtPixel(gCurTask->pixelX + 7, gCurTask->pixelY) == 0)
                            turn = 1;
                    }
                    else
                    {
                        if (IsFullBlockAtPixel(gCurTask->pixelX - 7, gCurTask->pixelY) == 0)
                            turn = 1;
                    }
                    if (turn == 0)
                    {
                        struct PlayerState *q = gCurTask->player;
                        u8 v = q->savedWallSide;

                        if (v == 1 && (gLatchedHeldKeys[q->playerIndex] & 32))
                            turn = 1;
                        else if (v == 2 && (gLatchedHeldKeys[q->playerIndex] & 16))
                            turn = 1;
                    }
                }
                else
                {
                    turn = 1;
                }
            }
            else
            {
                turn = 1;
            }
        }
        if (turn != 0)
        {
            struct Task *u = gCurTask;
            s32 x = u->playerStandSavedClampedTopY;

            if (x != -1 && dir == u->facing && IsAtPlayerBoundsTop(x, u->player->playerIndex) != 0)
                turn = 0;
        }
        if (turn != 0)
        {
            gCurTask->player->requestedAction = PLAYER_ACTION_WALK;
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                gCurTask->posX = (gCurTask->posX & 0xFFFF0000) | 0xF000;
            else
                gCurTask->posX = (gCurTask->posX & 0xFFFF0000) | 0x1000;
            if (gMetaKnightmareMode == 0)
            {
                if (gCurTask->player->mouthState == 1)
                    gCurTask->playerBaseFrame = gPlayerStandFrames[26][PlayerGetFacingSlope(gCurTask->player->playerIndex)];
                else
                    gCurTask->playerBaseFrame = gPlayerStandFrames[gCurTask->player->ability][PlayerGetFacingSlope(gCurTask->player->playerIndex)];
            }
            else
            {
                gCurTask->playerBaseFrame = gMetaKnightStandFrames[PlayerGetFacingSlope(gCurTask->player->playerIndex)];
            }
            TaskSetFrame(gCurTask->playerBaseFrame);
            gCurTask->player->savedWallSide = 0;
            break;
        }
        {
            struct Task *w = gCurTask;

            if (w->player->slope != w->playerPoseSlope || dir != w->facing)
            {
                if (gMetaKnightmareMode == 0)
                    TaskSetEntry(PlayerActionStand, gCurTaskIdx);
                else
                    TaskSetEntry(MetaKnightActionStand, gCurTaskIdx);
            }
        }
        break;
    }
end:
    gCurTask->playerPoseSlope = gCurTask->player->slope;
}

void PlayerActionWalk(void)
{
    struct PlayerState *p;
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 1;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_WALK;
    gCurTask->unk2C = -1;
    PlayerSetMotionXPreset(1, 72);
    p = gCurTask->player;
    if (p->prevMode != 1)
    {
        p->running = 0;
        gCurTask->player->runTapTimer = 0;
        gCurTask->player->savedWallSide = 0;
        gCurTask->playerWalkStepDelay = 0;
        PlayerPlayBump();
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
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                TaskSetFrame(0x148);
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 5);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 5);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
            }
        }
        gCurTask->playerBaseFrame = gUnk_0873D2E8[gCurTask->player->ability];
        switch (gCurTask->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 8);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                TaskSetFrame((s16)(gCurTask->playerBaseFrame - 4));
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 8);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
            }
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 2);
                gCurTask->playerLoopCount = 0;
                do
                {
                    t = gCurTask;
                    t->frame++;
                    TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                    gCurTask->playerLoopCount++;
                } while ((s16)gCurTask->playerLoopCount <= 10);
                TaskSetFrame((s16)(gCurTask->playerBaseFrame - 8));
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 2);
                gCurTask->playerLoopCount = 0;
                do
                {
                    t = gCurTask;
                    t->frame++;
                    TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                    gCurTask->playerLoopCount++;
                } while ((s16)gCurTask->playerLoopCount <= 6);
            }
        case ABILITY_SWORD:
        case ABILITY_HI_JUMP:
        case ABILITY_BEAM:
        case ABILITY_STONE:
        case ABILITY_BACKDROP:
        case ABILITY_THROW:
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                TaskSetFrame((s16)(gCurTask->playerBaseFrame - 5));
                TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
            }
        }
    }
    CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 0x200);
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

void PlayerActionWalkUpdate(void)
{
    while (PlayerCheckSkid() == 0 && PlayerCheckJump() == 0 && PlayerCheckFallOrWater() == 0
           && PlayerCheckEnterDoor() == 0 && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0
           && PlayerCheckFloat() == 0)
    {
        struct Task *t;

        if (PlayerCheckBButton() != 0)
            goto end;
        if (PlayerCheckDropAbility() != 0)
            goto end;
        t = gCurTask;
        if (t->velX == 0 && t->speedLimitX == 0)
        {
            t->player->requestedAction = PLAYER_ACTION_STAND;
        }
        else if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
        }
        else
        {
            struct Task *t2 = gCurTask;
            struct PlayerState *p = t2->player;
            s32 v = p->running;

            if (v != 0)
            {
                p->requestedAction = PLAYER_ACTION_RUN;
            }
            else
            {
                if ((gLatchedHeldKeys[p->playerIndex] & 48) == 0)
                {
                    s32 d = abs(t2->velX);

                    if ((u32)d <= 0xFFFF)
                        t2->playerWalkStepDelay = 2;
                }
                else
                {
                    t2->playerWalkStepDelay = v;
                }
                if (gCurTask->variant == 0)
                {
                    if (PlayerGetFacingSlope(gCurTask->player->playerIndex) == 4)
                    {
                        gCurTask->variant = 1;
                        TaskSetEntry(PlayerActionWalk, gCurTaskIdx);
                    }
                }
                else if (PlayerGetFacingSlope(gCurTask->player->playerIndex) != 4)
                {
                    gCurTask->variant = 0;
                    TaskSetEntry(PlayerActionWalk, gCurTaskIdx);
                }
            }
        }
        break;
    }
end:
    PlayerSetMotionXPreset(2, 72);
}
