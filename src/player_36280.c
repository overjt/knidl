#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_36280.c (0x08036280-0x08036C93, issue #91).
 *
 * Player action bodies, part 4: per-frame handler 9 and actions 10 and
 * 11.  PlayerActionFloatUpdate (handler 9) is the other half of M09's mode-14
 * coroutine PlayerActionFloat: a seven-state switch over Task.variant that
 * re-binds the coroutine with the next state (1 on a held A or up, 4 on
 * a newly-pressed B, 2/3/5 from the ground flags Task.onGround/unk7B).
 * PlayerActionDuck (action 10, mode 6) installs the scripts
 * gUnk_0873BD28/gUnk_0873CB24 in PlayerState.bodyBox/unk68 and plays
 * gUnk_0873D4BC[ability][column]; its handler PlayerActionDuckUpdate requests
 * action 11 on a newly-pressed A or B and 7 on the collision flag
 * gTerrainResult.unk5.  PlayerActionSlide (action 11, mode 7) installs the
 * attack hit-box set gUnk_0873CC84 in PlayerState.hitBoxSet; its handler
 * PlayerActionSlideUpdate registers the box gUnk_0873BE9C with M09's collision
 * registry RegisterCollider while the player moves faster than 0xE000 and
 * drops to state 1 below 0x8000. */

void TaskSetEntry(void *a, u32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionFloatUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    if (gCurTask->variant != 6 && PlayerCheckDropAbility() != 0)
    {
        if (gCurTask->variant == 5)
            gCurTask->variant = 2;
        return;
    }
    PlayerTurnToHeldDirection();
    switch (gCurTask->variant)
    {
    case 0:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
        {
            PlayerSetMotionYPreset(8);
        }
        else if (gCurTask->onGround & 1)
        {
            PlayerStopAxes(2);
        }
        else if (gCurTask->waterFlags & 1)
        {
            gCurTask->variant = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        else
        {
            PlayerSetMotionYPreset(7);
        }
        gCurTask->onGround = 0;
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        break;
    case 1:
        if (PlayerCheckEnterDoor() != 0)
            break;
        gCurTask->onGround = 0;
        PlayerSetMotionYPreset(8);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        if (gCurTask->onGround & 1)
            PlayerStopAxes(2);
        if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            gCurTask->variant = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 2:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->onGround & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        t = gCurTask;
        if (t->onGround & 1)
        {
            t->variant = 3;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            PlayerStopAxes(2);
            break;
        }
        if (gLatchedHeldKeys[t->player->playerIndex] & 65)
        {
            t->variant = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[t->player->playerIndex] & 2)
        {
            t->variant = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (!(t->waterFlags & 1))
            break;
        PlayerStopAxes(2);
        gCurTask->unk28 = -1;
        gCurTask->variant = 5;
        TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        break;
    case 3:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->onGround & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        u = gCurTask;
        if (!(u->onGround & 1))
        {
            u->variant = 2;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedHeldKeys[u->player->playerIndex] & 65)
        {
            u->variant = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[u->player->playerIndex] & 2)
        {
            u->variant = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 4:
        v = gCurTask;
        if (!(v->onGround & 1))
        {
            if (!(v->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        }
        PlayerSetMotionXPreset(7, 72);
        w = gCurTask;
        if (w->velY < 0)
        {
            if (gTerrainResult.ceilingHits != 0)
                w->velY = 0;
        }
        else if (w->onGround & 1)
        {
            if ((u32)w->velY > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        PlayerStopAtWall();
        break;
    case 5:
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtWall();
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
        {
            PlayerStopAxes(2);
            gCurTask->variant = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        else if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            PlayerStopAxes(2);
            gCurTask->variant = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        break;
    case 6:
        PlayerRequestLocomotion();
        x = gCurTask;
        if (x->onGround & 1)
        {
            if ((u32)x->velY > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        break;
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
    {
        gCurTask->player->mouthState = 0;
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    }
}

void PlayerActionDuck(void)
{
    struct Task *t;
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 6;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_DUCK;
    t = gCurTask;
    if (t->player->prevMode != 6)
    {
        t->player->bodyBox = (u32)gUnk_0873BD28;
        t->player->terrainBox = (u32)gUnk_0873CB24;
        t->playerPoseSlope = t->player->slope;
        PlayerSetMotionXPreset(0, 72);
    }
    gCurTask->playerDuckDropTimer = 8;
    gCurTask->player->facingSlope = PlayerGetFacingSlope(gCurTask->player->playerIndex);
    p = gCurTask->player;
    p->blinkTimer = 0;
    p->blinkScriptPos = 0;
    gCurTask->playerBaseFrame = gUnk_0873D4BC[gCurTask->player->ability][PlayerGetFacingSlope(gCurTask->player->playerIndex)];
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
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case ABILITY_HI_JUMP:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(6);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(6);
            }
        }
    case ABILITY_NORMAL:
    default:
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskSleepForever();
    }
}

void PlayerActionDuckUpdate(void)
{
    s32 dir = gCurTask->facing;

    PlayerFaceHeldDirection();
    while (PlayerCheckFallOrWater() == 0 && PlayerCheckDropAbility() == 0)
    {
        u16 *q = gLatchedPressedKeys;
        struct Task *t = gCurTask;

        if (q[t->player->playerIndex] & 3)
        {
            t->player->requestedAction = PLAYER_ACTION_SLIDE;
            break;
        }
        if (!(gLatchedHeldKeys[t->player->playerIndex] & 128))
        {
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult.unk5 != 0)
        {
            if (t->playerDuckDropTimer == 0)
            {
                t->onGround = 0;
                gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
                gCurTask->unk84 = 0;
                gCurTask->posY += 0x10000;
                break;
            }
            t->playerDuckDropTimer--;
        }
        {
            struct Task *w = gCurTask;

            if (w->player->slope != w->playerPoseSlope || dir != w->facing)
            {
                if (gMetaKnightmareMode == 0)
                    TaskSetEntry(PlayerActionDuck, gCurTaskIdx);
                else
                    TaskSetEntry(MetaKnightActionDuck, gCurTaskIdx);
            }
        }
        break;
    }
    gCurTask->playerPoseSlope = gCurTask->player->slope;
    if (gTerrainResult.unk0 != 0)
        PlayerStopAxes(1);
}

void PlayerActionSlide(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 7;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SLIDE;
    t = gCurTask;
    if (t->player->prevMode != 7)
    {
        t->playerActionDone28 = 0;
        t->variant = 0;
        gCurTask->player->unk14 = 10;
        PlayerStartSfx(118, gCurTask->player->playerIndex);
        gCurTask->player->hitBoxSet = gUnk_0873CC84;
        PlayerSetMotionXPreset(11, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SLIDE_DUST, 0);
    }
    switch (gCurTask->variant)
    {
    case 0:
        gCurTask->playerBaseFrame = gUnk_0873D5CA[gCurTask->player->ability][0];
        switch (gCurTask->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskSleepForever();
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            }
        case ABILITY_SWORD:
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
    case 1:
        TaskSetFrame(gUnk_0873D5CA[gCurTask->player->ability][1]);
        break;
    }
    gCurTask->player->hitBoxSet = 0;
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSlideUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;

    while (1)
    {
        if (PlayerCheckFallOrWater() != 0)
        {
            if (gCurTask->accelX == 0)
                PlayerSetMotionXPreset(5, 72);
            break;
        }
        if (PlayerCheckDropAbility() != 0)
            break;
        t = gCurTask;
        if (t->playerActionDone28 != 0)
        {
            t->player->requestedAction = PLAYER_ACTION_STAND;
        }
        else if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(1);
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
        }
        else
        {
            if (abs(t->velX) <= 0x7FFF)
            {
                t->variant = 1;
                TaskSetEntry(PlayerActionSlide, gCurTaskIdx);
            }
            if (abs(gCurTask->velX) > 0xE000)
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE9C);
        }
        break;
    }
    p = gCurTask->player;
    if ((s16)p->unk14 == 0)
    {
        PlayerSetMotionXPreset(5, 72);
        gCurTask->player->unk14--;
    }
    else if ((s16)p->unk14 > 0)
    {
        p->unk14--;
    }
}
