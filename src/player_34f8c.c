#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_34f8c.c (0x08034F8C-0x0803627F, issue #92).
 *
 * Player mode bodies, part 3: modes 5, 8 and 14.  Each action has an
 * "enter" coroutine (gPlayerActions[], by PlayerState.action) and a
 * "per-frame" handler (gPlayerActionHandlers[], by Task.updateState).  An enter
 * coroutine records the previous mode
 * (PlayerState.prevMode = unk04), sets the new one and the task's animation
 * set (Task.updateState), then plays the animation of the current ability
 * (PlayerState.ability, 0..25) out of a per-mode table (gUnk_0873D3B8,
 * gUnk_0873D420, gUnk_0873D7E4) with TaskSetFrame and TaskYieldTrampoline;
 * the per-frame handler runs M11's transition predicates in order and
 * writes the next mode request into PlayerState.requestedAction, or re-binds the
 * task to another coroutine with TaskSetEntry.  PlayerActionFall/PlayerActionFallUpdate
 * are mode 5, PlayerActionHighFall/PlayerActionHighFallUpdate mode 8 (Task.variant is its
 * sub-state) and PlayerActionFloat mode 14, a six-state loop over Task.variant
 * whose per-frame handler is M10's PlayerActionFloatUpdate. */

void TaskSetEntry(void *a, u32 i);
/* src/player_1a76c.c defines it with u16 x/y; the ROM passes the task's
   s16 position unextended, so this file's callers see s16 parameters */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
void sub_0803ccd8(s32 a);                     /* M10: lsls r0, #2 on entry, void epilogue */
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void CreatePlayerObject(s32 a, s32 b, s32 c);

void PlayerActionFall(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 5;
    gCurTask->updateState = 7;
    PlayerSetMotionYPreset(2);
    gCurTask->unk30 = 0;
    if (gCurTask->player->prevMode != 5)
        gCurTask->player->unk14 = 300;
    if (gCurTask->player->bumpKind & 1)
        gCurTask->unk30 = 1;
    if (gCurTask->player->ability == 10)
    {
        gCurTask->unk2C = 0;
        gCurTask->unk28 = 0;
    }
    PlayerPlayBump();
    gCurTask->player->unk14 = 30;
    if (gCurTask->player->prevMode == 4)
    {
        gCurTask->player->unk14 = 300;
        if (gCurTask->player->mouthState == 1)
        {
            TaskSetFrame(346);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->player->unk14 = 0;
        }
        else if (gCurTask->unk30 != 0)
        {
            gCurTask->player->unk14 = 30;
        }
        else
        {
            gCurTask->playerBaseFrame = gUnk_0873D3B8[gCurTask->player->ability][0];
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->player->unk14 = 20;
                break;
            case 4:
            case 9:
            case 10:
            case 25:
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(4);
                gCurTask->player->unk14 = 28;
                break;
            }
        }
    }
    gCurTask->unk30 = 0;
    if (gCurTask->player->mouthState == 1)
    {
        while (1)
        {
            TaskSetFrame(0x15D);
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
        }
    }
    gCurTask->playerBaseFrame = gUnk_0873D3B8[gCurTask->player->ability][1];
    switch (gCurTask->player->ability)
    {
    case 0:
    default:
        while (1)
        {
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case 10:
        if (gCurTask->player->prevMode != 8)
            gCurTask->player->unk14 = 0;
        do
        {
            TaskSetFrame(0x841);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)gCurTask->player->unk14 != 0 || gCurTask->velY <= 0x1FFFF);
        while (1)
        {
            if (gCurTask->facing == 1)
                gCurTask->unk28 = -0x400;
            else
                gCurTask->unk28 = 0x400;
            sub_0803ccd8(0);
            TaskSetFrame(0x839);
            TaskYieldTrampoline(8);
            sub_0803ccd8(1);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(2);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(3);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(4);
            gCurTask->frame--;
            TaskYieldTrampoline(8);
            if (gCurTask->facing == 1)
                gCurTask->unk28 = -0x800;
            else
                gCurTask->unk28 = 0x800;
            sub_0803ccd8(5);
            TaskSetFrame(0x840);
            TaskYieldTrampoline(4);
            sub_0803ccd8(6);
            TaskSetFrame(0x836);
            TaskYieldTrampoline(4);
            sub_0803ccd8(7);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            sub_0803ccd8(8);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            sub_0803ccd8(9);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            if (gCurTask->facing == 1)
                gCurTask->unk28 = 0x400;
            else
                gCurTask->unk28 = -0x400;
            sub_0803ccd8(10);
            TaskSetFrame(0x83A);
            TaskYieldTrampoline(8);
            sub_0803ccd8(11);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(12);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(13);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(14);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            if (gCurTask->facing == 1)
                gCurTask->unk28 = 0x800;
            else
                gCurTask->unk28 = -0x800;
            sub_0803ccd8(15);
            TaskSetFrame(0x83F);
            TaskYieldTrampoline(4);
            sub_0803ccd8(16);
            TaskSetFrame(0x83D);
            TaskYieldTrampoline(4);
            sub_0803ccd8(17);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
            sub_0803ccd8(18);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
            sub_0803ccd8(19);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
        }
    }
}

void PlayerActionFallUpdate(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterWater() == 0 && PlayerCheckLadder() == 0 && PlayerCheckEnterDoor() == 0
           && PlayerCheckFloat() == 0 && PlayerCheckAirFloat() == 0 && PlayerCheckBButton() == 0
           && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->onGround & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        if (gMetaKnightmareMode == 0 && gCurTask->player->ability != 10
            && gCurTask->player->mouthState == 0 && gCurTask->velY > 0
            && --gCurTask->player->unk14 == 0)
        {
            gCurTask->player->requestedAction = 8;
            break;
        }
        if (gCurTask->player->ability != 10 && gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            if (gCurTask->player->bumpKind & 7)
                TaskSetEntry(PlayerActionFall, gCurTaskIdx);
        }
        if (gCurTask->player->ability == 10)
        {
            if ((s16)gCurTask->player->unk14 != 0)
                gCurTask->player->unk14--;
            if (gCurTask->unk28 != 0
                && (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128))
            {
                PlayerStopAxes(1);
                gCurTask->player->requestedAction = 8;
            }
        }
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    if (gCurTask->onGround & 1)
        PlayerLand(1);
    PlayerStopAtCeilingAndWall();
}

void PlayerActionHighFall(void)
{
    s16 *anim;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 8;
    gCurTask->updateState = 8;
    if (gCurTask->player->prevMode != 8)
    {
        gCurTask->variant = 0;
        gCurTask->player->unk40 &= 0xFFFD;
    }
    gCurTask->player->hitBoxSet = 0;
    gCurTask->unk28 = 0;
    anim = gUnk_0873D420[gCurTask->player->ability];
    switch (gCurTask->variant)
    {
    case 0:
        gCurTask->player->hitBoxSet = gUnk_0873CC74;
        switch (gCurTask->player->ability)
        {
        case 10:
            PlayerSetMotionYPreset(2);
        case 0:
        default:
            TaskSetFrame(anim[0]);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk28++;
            TaskSetFrame(anim[1]);
            TaskSleepForever();
        case 1:
        case 2:
        case 5:
        case 15:
        case 16:
        case 17:
        case 19:
        case 22:
        case 23:
            TaskSetFrame(anim[0]);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk28++;
            gCurTask->playerBaseFrame = anim[1];
            while (1)
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case 2:
        gCurTask->player->unk42 |= 0x100;
        CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
        gCurTask->player->bumpKind = 0;
        TaskSetFrame(anim[2]);
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        gCurTask->player->bumpKind = 0;
        PlayerSetMotionYPreset(3);
        TaskSetFrame((s16)(anim[2] + 1));
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk28++;
        break;
    }
    TaskSleepForever();
}

void PlayerActionHighFallUpdate(void)
{
    if (PlayerCheckLadder() == 0 && PlayerCheckEnterDoor() == 0 && PlayerCheckEnterWater() == 0
        && PlayerCheckFloat() == 0 && PlayerCheckAirFloat() == 0 && PlayerCheckBButton() == 0)
        PlayerCheckDropAbility();
    if (gCurTask->player->requestedAction != 0)
    {
        gCurTask->player->unk42 &= 0xFEFF;
    }
    else
    {
        PlayerTurnToHeldDirection();
        PlayerSetMotionXPreset(7, 72);
        switch (gCurTask->variant)
        {
        case 0:
            if (PlayerCheckLanding() != 0)
            {
                PlayerCheckBump();
                PlaySfxIfLocalPlayer(116, gCurTask->player->playerIndex);
                gCurTask->variant = 2;
                TaskSetEntry(PlayerActionHighFall, gCurTaskIdx);
            }
            else
            {
                if (gCurTask->player->blocksBroken != 0)
                    gCurTask->player->unk40 |= 2;
                if (gCurTask->player->unk40 & 2)
                {
                    gCurTask->variant = 1;
                    TaskSetEntry(PlayerActionHighFall, gCurTaskIdx);
                }
                else
                {
                    if (gTerrainResult.unk0 != 0)
                    {
                        PlayerCheckBump();
                        if (gCurTask->player->bumpKind & 7)
                            TaskSetEntry(PlayerActionFall, gCurTaskIdx);
                    }
                    if (gCurTask->unk28 != 0)
                        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BEB0);
                }
            }
            break;
        case 1:
            if (gCurTask->onGround & 1)
            {
                PlayerCheckBump();
                PlayerLand(0);
                PlayerRequestLocomotion();
                gCurTask->player->unk42 &= 0xFEFF;
                break;
            }
        case 2:
            if (gCurTask->unk28 != 0)
            {
                gCurTask->player->requestedAction = 7;
                gCurTask->player->unk42 &= 0xFEFF;
            }
            break;
        }
    }
    PlayerStopAtCeilingAndWall();
}

void PlayerActionFloat(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 14;
    gCurTask->updateState = 9;
    if (gCurTask->player->prevMode != 14)
    {
        gCurTask->player->running = 0;
        gCurTask->variant = 0;
    }
    gCurTask->player->bodyBox = (u32)gUnk_0873BD14;
    while (1)
    {
        switch (gCurTask->variant)
        {
        case 0:
            PlaySfxIfLocalPlayer(228, gCurTask->player->playerIndex);
            TaskSetFrame(gUnk_0873D7E4[gCurTask->player->ability][0]);
            TaskYieldTrampoline(2);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
            gCurTask->variant = 2;
            gCurTask->player->mouthState = 2;
            break;
        case 1:
            PlaySfxIfLocalPlayer(115, gCurTask->player->playerIndex);
            gCurTask->playerBaseFrame = gUnk_0873D7E4[gCurTask->player->ability][1];
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(4);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(4);
                    gCurTask->frame -= 2;
                    TaskYieldTrampoline(2);
                    if (!(gLatchedHeldKeys[gCurTask->player->playerIndex] & 65))
                        break;
                    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 1)
                        PlaySfxIfLocalPlayer(115, gCurTask->player->playerIndex);
                }
                break;
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 5));
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 5));
                    TaskYieldTrampoline(2);
                    if (!(gLatchedHeldKeys[gCurTask->player->playerIndex] & 65))
                        break;
                    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 1)
                        PlaySfxIfLocalPlayer(115, gCurTask->player->playerIndex);
                }
                break;
            }
            gCurTask->variant = 2;
            break;
        case 2:
            gCurTask->playerBaseFrame = gUnk_0873D7E4[gCurTask->player->ability][1];
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(6);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 5));
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 1));
                    TaskYieldTrampoline(6);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 5));
                    TaskYieldTrampoline(3);
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(3);
                    gCurTask->frame++;
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 10));
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                    TaskYieldTrampoline(3);
                    gCurTask->frame--;
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 9));
                    TaskYieldTrampoline(3);
                }
            }
        case 3:
            gCurTask->playerBaseFrame = gUnk_0873D7E4[gCurTask->player->ability][1];
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(4);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame++;
                        TaskYieldTrampoline(4);
                    }
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame--;
                        TaskYieldTrampoline(4);
                    }
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(2);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 7; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame++;
                        TaskYieldTrampoline(2);
                    }
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 5; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame--;
                        TaskYieldTrampoline(2);
                    }
                }
            }
        case 4:
            if (gCurTask->player->mouthState == 2)
            {
                gCurTask->player->mouthState = 0;
                gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
                gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
                CreatePlayerObject(gCurTask->player->playerIndex, 0, 0);
                gCurTask->unk28++;
                PlayerSetMotionYPreset(2);
                TaskSetFrame(gUnk_0873D7E4[gCurTask->player->ability][2]);
                TaskYieldTrampoline(6);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(gUnk_0873D7E4[gCurTask->player->ability][2]);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
            if (PlayerCheckEnterDoor() == 0)
            {
                if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 64) && !(gCurTask->waterFlags & 1))
                {
                    gCurTask->player->bodyBox = (u32)gUnk_0873BD14;
                    gCurTask->variant = 0;
                    break;
                }
                gCurTask->variant = 6;
            }
            TaskSleepForever();
        case 5:
            gCurTask->playerBaseFrame = gUnk_0873D7E4[gCurTask->player->ability][1];
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(4);
                    PlayerSetMotionYPreset(9);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame++;
                        TaskYieldTrampoline(4);
                    }
                    PlayerSetMotionYPreset(10);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame--;
                        TaskYieldTrampoline(4);
                    }
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    PlayerSetMotionYPreset(9);
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(2);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 5; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame++;
                        TaskYieldTrampoline(2);
                    }
                    PlayerSetMotionYPreset(10);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame--;
                        TaskYieldTrampoline(2);
                    }
                    gCurTask->velY = 0;
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                }
            }
        }
    }
}
