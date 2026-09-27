#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4c64c.c (0x0804C64C-0x0804CC7B, issue #88).
 *
 * Player action bodies, part 22: action 54 and per-frame handler 51.
 * sub_0804c64c (action 54, mode 13) is a charged three-way move, a
 * `while (1) switch (Task.unk73)` state machine.  A fresh entry starts
 * in state 3 with the 120-frame timer PlayerState.unk14 and
 * Task.unk80 = 23: state 3 winds up (animation 0xF73, the frame index
 * PlayerState.unk16 stepping 0-1-2-1-2, effect 28 on the ground) and
 * picks the direction Task.unk28 from the held keys (up 0, down 2,
 * otherwise 1); state 4 (effect 47) holds until the timer runs out or
 * B is pressed, switching to state 5 once PlayerState.unk08 is 0, and
 * then jumps to state unk28.  States 0-2 are the three releases
 * (animations 0xF7C/0xF76/0xF84, sound 236, then 0xF80), which end in
 * state 5: clear PlayerState.unk42 bit 9, SetPlayerInvulnerability(255, 0, player)
 * and end the action (TaskSleepForever, which falls into state 0,
 * lesson 3.403).  Its handler sub_0804ca84 re-binds state 5 from states
 * 0-2 once PlayerState.unk16 >= 0 and unk08 == 0, re-reads the direction
 * every frame in state 4 and cycles the charge animation through
 * Task.unk46 (0xF7A/0xF82/0xF74 and 0xF7B/0xF83/0xF75), and requests
 * action 23 through PlayerHasCrossedWaterSurface. */

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_0804c64c(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 51;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            struct Task *u;
            t->unk73 = 3;
            u = gCurTask;
            u->player->unk14 = 120;
            u->unk80 = 23;
        }
    }
    while (1) {
        switch (gCurTask->unk73) {
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
                gCurTask->unk28 = 0;
            else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
                gCurTask->unk28 = 2;
            else
                gCurTask->unk28 = 1;
            gCurTask->unk73 = 4;
            /* fallthrough */
        case 4:
            CreatePlayerEffect(gCurTask->player->playerIndex, 47, 0);
            gCurTask->unk46 = 0;
            for (;;) {
                {
                    struct Task *t = gCurTask;
                    if ((s8)t->player->unk08 == 0)
                        t->unk73 = 5;
                }
                if ((u16)--gCurTask->player->unk14 == 0)
                    goto done;
                if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
                    goto done;
                TaskYieldTrampoline(1);
            }
        done:
            gCurTask->unk73 = gCurTask->unk28;
            break;
        case 5:
            gCurTask->player->unk42 &= 0xFDFF;
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
            gCurTask->unk73 = 5;
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
            gCurTask->unk73 = 5;
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
            gCurTask->unk73 = 5;
            break;
        }
    }
}

void sub_0804ca84(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 4:
        PlayerTurnToHeldDirection();
        if (PlayerHasCrossedWaterSurface(0) != 0) {
            PlayerSetWaterMotionY();
            gCurTask->player->requestedAction = 23;
            break;
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
            gCurTask->unk28 = 0;
        else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
            gCurTask->unk28 = 2;
        else
            gCurTask->unk28 = 1;
        {
            struct Task *v = gCurTask;
            switch (v->unk46) {
            case 0:
            case 1:
                if (v->unk28 == 0) {
                    v->player->unk16 = 6;
                    TaskSetFrame(0xF7A);
                } else if (v->unk28 == 2) {
                    v->player->unk16 = 11;
                    TaskSetFrame(0xF82);
                } else {
                    v->player->unk16 = 1;
                    TaskSetFrame(0xF74);
                }
                break;
            case 2:
            case 3:
                if (v->unk28 == 0) {
                    v->player->unk16 = 7;
                    TaskSetFrame(0xF7B);
                } else if (v->unk28 == 2) {
                    v->player->unk16 = 12;
                    TaskSetFrame(0xF83);
                } else {
                    v->player->unk16 = 2;
                    TaskSetFrame(0xF75);
                }
                break;
            }
        }
        gCurTask->unk46 = (gCurTask->unk46 + 1) & 3;
        break;
    case 0:
    case 1:
    case 2:
        {
            struct PlayerState *p = t->player;
            if ((s8)p->unk16 >= 0 && (s8)p->unk08 == 0)
                goto rebind;
        }
        break;
    rebind:
        do {
            do {
                t->unk73 = 5;
            } while (0);
        } while (0);
        TaskSetEntry(sub_0804c64c, gCurTaskIdx);
        break;
    case 5:
        PlayerRequestLocomotion();
        return;
    }
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) != 0) {
        gCurTask->player->requestedAction = 23;
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
