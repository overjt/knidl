#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_4b858.c (0x0804B858-0x0804C64B, issue #88).
 *
 * Player action bodies, part 21: action 53 and per-frame handler 50.
 * PlayerActionBackdropHold (action 53, mode 13) is `loop: switch (Task.variant)` over
 * eight states, a stance with six moves.  A fresh entry starts in
 * state 6 with the 120-frame timer PlayerState.unk14 and Task.u80.attackAbility =
 * 22: the stance (animation 0xE84, effect 28 on the ground) picks the
 * next move from the input - up 0, back 1 or forward 2 (PlayerGetHeldDirection,
 * kept in gUnk_03001F2C), down 3, A 4, leaving the ground 5 - or a
 * random one (gUnk_0873B65E[RandomRange(4)]) when the timer runs out,
 * and goes to state 7 once PlayerState.heldCount is 0.  States 0-5 are the
 * moves (animations 0xE97-0xEE4 with the frame index PlayerState.unk16,
 * velocity presets 2 and 56-68, sounds 178/179, a landing with effect
 * 27 and RequestScreenShake(2)), each back to state 7, which clears
 * PlayerState.statusFlags bit 9, calls SetPlayerInvulnerability(255, 0, player) and ends
 * the action (TaskSleepForever, falling into state 0, lesson 3.403).  The
 * long `bl`s at 0x0804C49E and 0x0804C488 are cross-jumped `goto loop`
 * tails.  Its handler PlayerActionBackdropHoldUpdate lets PlayerRequestLocomotion end state 7,
 * steers state 4 in the air (TaskSetMotionXFacing and the 8.8 speed Task.speedLimitX
 * for the direction in gUnk_03001F2C), re-binds state 7 from the other
 * moves once PlayerState.unk16 >= 0 and unk08 == 0, and requests action
 * 23 through PlayerHasCrossedWaterSurface. */

u32 RandomRange(u32 range);
void TaskSetEntry(void *a, u32 i);
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionBackdropHold(void)
{
    struct Task *h;
    s32 k;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_BACKDROP_HOLD;
    h = gCurTask;
    if (h->player->prevMode != 13)
    {
        struct Task *u;

        h->variant = 6;
        u = gCurTask;
        u->player->playerGrabHoldTimer = 120;
        u->playerActionDone28 = 0;
        u->u80.attackAbility = ABILITY_BACKDROP;
    }
loop:
    switch (gCurTask->variant)
    {
    case 6:
        {
            struct Task *a;

            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            gCurTask->player->playerHoldPose = 0;
            TaskSetFrame(0xE84);
            PlayerStartOffsetScript(12);
            a = gCurTask;
            if (a->onGround & 1)
            {
                CreatePlayerEffect(a->player->playerIndex, 28, 3);
                TaskYieldTrampoline(10);
            }
            else
            {
                PlayerStopAxes(3);
                TaskYieldTrampoline(10);
            }
        }
        for (;;)
        {
            struct Task *b;
            struct Task *c;
            struct PlayerState *p;

            b = gCurTask;
            if (--b->player->playerGrabHoldTimer == 0)
            {
                gCurTask->variant = gUnk_0873B65E[RandomRange(4)];
                goto loop;
            }
            if (!(b->onGround & 1))
            {
                b->variant = 5;
                goto loop;
            }
            if (gLatchedHeldKeys[b->player->playerIndex] & 1)
            {
                b->variant = 4;
                goto loop;
            }
            k = PlayerGetHeldDirection();
            gUnk_03001F2C = k;
            if (k != 0)
            {
                if (k == 1)
                {
                    gCurTask->variant = 2;
                    goto loop;
                }
                gCurTask->variant = 1;
                goto loop;
            }
            c = gCurTask;
            p = c->player;
            if (gLatchedHeldKeys[p->playerIndex] & 0xC0)
            {
                if (gLatchedHeldKeys[p->playerIndex] & 0x40)
                {
                    c->variant = 0;
                    goto loop;
                }
                c->variant = 3;
                goto loop;
            }
            if ((s8)p->heldCount == 0)
                c->variant = 7;
            TaskYieldTrampoline(1);
        }
    case 7:
        {
            struct Task *d = gCurTask;

            d->player->statusFlags &= ~PLAYER_STATUS_NO_TERRAIN_DAMAGE;
            SetPlayerInvulnerability(255, 0, d->player->playerIndex);
        }
        TaskSleepForever();
    case 0:
        gCurTask->player->playerHoldPose = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 3;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 4;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 5;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 6;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(58);
        PlaySfxIfLocalPlayer(178, gCurTask->player->playerIndex);
        gCurTask->player->playerHoldPose = 7;
        gCurTask->frame++;
        TaskYieldTrampoline(17);
        PlayerSetMotionYPreset(59);
        gCurTask->player->playerHoldPose = 24;
        TaskSetFrame(0xEBA);
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->player->playerIndex);
        PlayerStopAxes(2);
        gCurTask->player->playerHoldPose = 25;
        TaskSetFrame(0xEBB);
        TaskYieldTrampoline(20);
        gCurTask->player->playerHoldPose = 255;
        gCurTask->variant = 7;
        goto loop;
    case 1:
        gCurTask->player->playerHoldPose = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 3;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 4;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 5;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 6;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerSetMotionXPreset(11, 66);
        PlayerSetMotionYPreset(60);
        PlaySfxIfLocalPlayer(178, gCurTask->player->playerIndex);
        gCurTask->player->playerHoldPose = 7;
        gCurTask->frame++;
        TaskYieldTrampoline(13);
        gCurTask->player->playerHoldPose = 8;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 9;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(61);
        gCurTask->player->playerHoldPose = 10;
        gCurTask->frame++;
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->player->playerIndex);
        PlayerStopAxes(3);
        gCurTask->player->playerHoldPose = 11;
        TaskSetFrame(0xEA1);
        TaskYieldTrampoline(20);
        gCurTask->player->playerHoldPose = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->variant = 7;
        goto loop;
    case 3:
        gCurTask->player->playerHoldPose = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 3;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 4;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 5;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 6;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(62);
        PlaySfxIfLocalPlayer(178, gCurTask->player->playerIndex);
        gCurTask->player->playerHoldPose = 7;
        gCurTask->frame++;
        TaskYieldTrampoline(11);
        gCurTask->player->playerHoldPose = 12;
        TaskSetFrame(0xEA8);
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 13;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 14;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(63);
        do
        {
            gCurTask->player->playerHoldPose = 15;
            TaskSetFrame(0xEAB);
            TaskYieldTrampoline(1);
            if (gCurTask->onGround & 1)
                goto done3;
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
            {
                gCurTask->player->playerHoldPose++;
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                if (gCurTask->onGround & 1)
                    goto done3;
            }
        } while (!(gCurTask->onGround & 1));
    done3:
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->player->playerIndex);
        PlayerStopAxes(2);
        gCurTask->player->playerHoldPose = 23;
        TaskSetFrame(0xEB3);
        TaskYieldTrampoline(20);
        gCurTask->player->playerHoldPose = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->variant = 7;
        goto loop;
    case 4:
        gCurTask->playerActionDone28 = 0;
        PlayerSetMotionXPreset(11, 67);
        gCurTask->player->playerHoldPose = 37;
        TaskSetFrame(0xECE);
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 38;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 39;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 40;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 41;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->player->playerHoldPose = 42;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(64);
        PlaySfxIfLocalPlayer(178, gCurTask->player->playerIndex);
        gCurTask->player->playerHoldPose = 43;
        gCurTask->frame++;
        TaskYieldTrampoline(11);
        gCurTask->player->playerHoldPose = 44;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 45;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 46;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(65);
        gCurTask->player->playerHoldPose = 47;
        TaskSetFrame(0xED8);
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->player->playerIndex);
        PlayerStopAxes(3);
        gCurTask->playerActionDone28++;
        gCurTask->player->playerHoldPose = 48;
        TaskSetFrame(0xED9);
        TaskYieldTrampoline(20);
        gCurTask->player->playerHoldPose = 255;
        PlayerSetMotionYPreset(57);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 7);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->variant = 7;
        goto loop;
    case 2:
        PlayerSetMotionXPreset(11, 68);
        gCurTask->player->playerHoldPose = 26;
        TaskSetFrame(0xEBD);
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 27;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->player->playerHoldPose = 28;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerSetMotionXPreset(11, 69);
        PlayerSetMotionYPreset(66);
        PlaySfxIfLocalPlayer(178, gCurTask->player->playerIndex);
        gCurTask->player->playerHoldPose = 29;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 30;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 31;
        gCurTask->frame++;
        TaskYieldTrampoline(9);
        gCurTask->player->playerHoldPose = 32;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 33;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(67);
        gCurTask->player->playerHoldPose = 34;
        TaskSetFrame(0xEC5);
        while (!(gCurTask->onGround & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->player->playerIndex);
        PlayerStopAxes(3);
        gCurTask->player->playerHoldPose = 35;
        TaskSetFrame(0xEC6);
        TaskYieldTrampoline(20);
        gCurTask->player->playerHoldPose = 36;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->variant = 7;
        goto loop;
    case 5:
        PlayerStopAxes(3);
        PlayerSetMotionYPreset(68);
        gCurTask->player->playerHoldPose = 49;
        TaskSetFrame(0xEE4);
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(2);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose++;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->player->playerHoldPose = 254;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->player->playerHoldPose = 255;
        gCurTask->variant = 7;
        goto loop;
    }
    goto loop;
}

void PlayerActionBackdropHoldUpdate(void)
{
    switch (gCurTask->variant) {
    case 7:
        if (gCurTask->onGround & 1)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
        return;
    case 4:
        {
            struct Task *t = gCurTask;
            if (t->playerActionDone28 == 0 && !(t->onGround & 1) && (s8)t->player->playerHoldPose != -1) {
                s32 k, x, v;
                k = PlayerGetHeldDirection();
                gUnk_03001F2C = k;
                if (k != 0) {
                    if (k == 1)
                        x = 0x300;
                    else
                        x = 0x100;
                } else {
                    x = 0x200;
                }
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(v, 0);
                {
                    struct Task *u = gCurTask;
                    s32 w = x << 8;
                    if (x & 0x8000)
                        w |= 0xFF000000;
                    u->speedLimitX = w;
                }
            }
        }
        /* fallthrough */
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        {
            struct Task *u = gCurTask;
            struct PlayerState *p = u->player;
            if ((s8)p->playerHoldPose >= 0 && (s8)p->heldCount == 0) {
                if (!(u->onGround & 1)) {
                    PlayerStopAxes(2);
                    PlayerSetMotionYPreset(2);
                }
                PlayerStopAxes(1);
                gCurTask->variant = 7;
                TaskSetEntry(PlayerActionBackdropHold, gCurTaskIdx);
            }
        }
        break;
    }
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) != 0) {
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        gCurTask->player->playerHoldPose = 255;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    } else if (gCurTask->onGround & 1) {
        PlayerLand(0);
    } else if ((s8)gCurTask->player->playerHoldPose == -1) {
        PlayerTurnToHeldDirection();
        PlayerSetMotionXPreset(7, 72);
    }
}
