#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_4c64c.c (0x0804C64C-0x0804CC7B, issue #88).
 *
 * Player action bodies, part 22: action 54 and per-frame handler 51.
 * PlayerActionThrowHold (action 54, mode 13) is a charged three-way move, a
 * `while (1) switch (Task.variant)` state machine.  A fresh entry starts
 * in state 3 with the 120-frame timer PlayerState.unk14 and
 * Task.u80.attackAbility = 23: state 3 winds up (animation 0xF73, the frame index
 * PlayerState.unk16 stepping 0-1-2-1-2, effect 28 on the ground) and
 * picks the direction Task.unk28 from the held keys (up 0, down 2,
 * otherwise 1); state 4 (effect 47) holds until the timer runs out or
 * B is pressed, switching to state 5 once PlayerState.heldCount is 0, and
 * then jumps to state unk28.  States 0-2 are the three releases
 * (animations 0xF7C/0xF76/0xF84, sound 236, then 0xF80), which end in
 * state 5: clear PlayerState.statusFlags bit 9, SetPlayerInvulnerability(255, 0, player)
 * and end the action (TaskSleepForever, which falls into state 0,
 * lesson 3.403).  Its handler PlayerActionThrowHoldUpdate re-binds state 5 from states
 * 0-2 once PlayerState.unk16 >= 0 and unk08 == 0, re-reads the direction
 * every frame in state 4 and cycles the charge animation through
 * Task.unk46 (0xF7A/0xF82/0xF74 and 0xF7B/0xF83/0xF75), and requests
 * action 23 through PlayerHasCrossedWaterSurface. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionThrowHold(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_THROW_HOLD;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            struct Task *u;
            t->variant = 3;
            u = gCurTask;
            u->player->unk14 = 120;
            u->u80.attackAbility = ABILITY_THROW;
        }
    }
    while (1) {
        switch (gCurTask->variant) {
        case 3:
            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            PlayerStartOffsetScript(13);
            {
                struct Task *t = gCurTask;
                if (t->onGround & 1)
                    CreatePlayerEffect(t->player->playerIndex, 28, 3);
                else
                    PlayerStopAxes(2);
            }
            gCurTask->player->unk16 = 0;
            TaskSetFrame(0xF73);
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 1;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->player->unk16 = 2;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->player->unk16 = 1;
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->player->unk16 = 2;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
                gCurTask->playerThrowDir = 0;
            else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
                gCurTask->playerThrowDir = 2;
            else
                gCurTask->playerThrowDir = 1;
            gCurTask->variant = 4;
            /* fallthrough */
        case 4:
            CreatePlayerEffect(gCurTask->player->playerIndex, 47, 0);
            gCurTask->playerThrowHoldFramePhase = 0;
            for (;;) {
                {
                    struct Task *t = gCurTask;
                    if ((s8)t->player->heldCount == 0)
                        t->variant = 5;
                }
                if ((u16)--gCurTask->player->unk14 == 0)
                    goto done;
                if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
                    goto done;
                TaskYieldTrampoline(1);
            }
        done:
            gCurTask->variant = gCurTask->playerThrowDir;
            break;
        case 5:
            gCurTask->player->statusFlags &= 0xFDFF;
            do {
                SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
            } while (0);
            TaskSleepForever();
            /* fallthrough */
        case 0:
            PlayerStartOffsetScript(9);
            CreatePlayerEffect(gCurTask->player->playerIndex, 47, 1);
            gCurTask->player->unk16 = 8;
            TaskSetFrame(0xF7C);
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 9;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 10;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            PlaySfxIfLocalPlayer(236, gCurTask->player->playerIndex);
            gCurTask->player->unk16 = 253;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->variant = 5;
            break;
        case 1:
            PlayerStartOffsetScript(10);
            CreatePlayerEffect(gCurTask->player->playerIndex, 47, 1);
            gCurTask->player->unk16 = 3;
            TaskSetFrame(0xF76);
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 4;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 5;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            PlaySfxIfLocalPlayer(236, gCurTask->player->playerIndex);
            gCurTask->player->unk16 = 252;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->variant = 5;
            break;
        case 2:
            PlayerStartOffsetScript(11);
            CreatePlayerEffect(gCurTask->player->playerIndex, 47, 1);
            gCurTask->player->unk16 = 13;
            TaskSetFrame(0xF84);
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 14;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->player->unk16 = 15;
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            PlaySfxIfLocalPlayer(236, gCurTask->player->playerIndex);
            gCurTask->player->unk16 = 251;
            gCurTask->frame++;
            TaskYieldTrampoline(7);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->variant = 5;
            break;
        }
    }
}

void PlayerActionThrowHoldUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 4:
        PlayerTurnToHeldDirection();
        if (PlayerHasCrossedWaterSurface(0) != 0) {
            PlayerSetWaterMotionY();
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
            break;
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
            gCurTask->playerThrowDir = 0;
        else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
            gCurTask->playerThrowDir = 2;
        else
            gCurTask->playerThrowDir = 1;
        {
            struct Task *v = gCurTask;
            switch (v->playerThrowHoldFramePhase) {
            case 0:
            case 1:
                if (v->playerThrowDir == 0) {
                    v->player->unk16 = 6;
                    TaskSetFrame(0xF7A);
                } else if (v->playerThrowDir == 2) {
                    v->player->unk16 = 11;
                    TaskSetFrame(0xF82);
                } else {
                    v->player->unk16 = 1;
                    TaskSetFrame(0xF74);
                }
                break;
            case 2:
            case 3:
                if (v->playerThrowDir == 0) {
                    v->player->unk16 = 7;
                    TaskSetFrame(0xF7B);
                } else if (v->playerThrowDir == 2) {
                    v->player->unk16 = 12;
                    TaskSetFrame(0xF83);
                } else {
                    v->player->unk16 = 2;
                    TaskSetFrame(0xF75);
                }
                break;
            }
        }
        gCurTask->playerThrowHoldFramePhase = (gCurTask->playerThrowHoldFramePhase + 1) & 3;
        break;
    case 0:
    case 1:
    case 2:
        {
            struct PlayerState *p = t->player;
            if ((s8)p->unk16 >= 0 && (s8)p->heldCount == 0)
                goto rebind;
        }
        break;
    rebind:
        do {
            do {
                t->variant = 5;
            } while (0);
        } while (0);
        TaskSetEntry(PlayerActionThrowHold, gCurTaskIdx);
        break;
    case 5:
        PlayerRequestLocomotion();
        return;
    }
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) != 0) {
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        gCurTask->player->unk16 = 255;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    } else if (!(gCurTask->onGround & 1)) {
        PlayerSetMotionYPreset(2);
        PlayerSetMotionXPreset(11, 2);
    } else {
        PlayerSetMotionXPreset(0, 72);
        PlayerLand(0);
    }
}
