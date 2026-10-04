#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_49f98.c (0x08049F98-0x0804A54B, issue #88).
 *
 * Player action bodies, part 17: action 46 and per-frame handler 43.
 * PlayerActionHiJump (action 46, mode 13) is a five-state move whose states
 * fall into each other; a fresh entry starts in state 0, or in state 4
 * when it comes from mode 5.  State 0 starts it (effect 42, animation
 * 0xB2D), state 1 points PlayerState.hitBoxSet at the block hit-box set
 * gUnk_0873CF5C with effects 42 x4 and sound 174 and runs velocity
 * presets 38, 0 and 1, and states 3-4 hand the player over to mode 5
 * with handler 7 (animation 0xAD2, then the ability's loop from
 * gUnk_0873D3B8[ability][1]).  Its handler PlayerActionHiJumpUpdate
 * flashes the palette gUnk_081F59F0 (the VRAM transfer queue
 * RequestCopy) in state 1, re-binds state 3 on a newly-pressed B after
 * the PlayerState.unk14 frames, registers the collider gUnk_0873C228,
 * steers with the held left/right keys, picks one of five animation
 * rows 0xB2E-0xB3E by |Task.velX| and cycles Task.unk46 through them,
 * ends the move on landing and re-binds state 4 on a ceiling hit. */

void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionHiJump(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_HI_JUMP;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            if (t->player->prevMode == 5)
                t->variant = 4;
            else
                t->variant = 0;
            gCurTask->u80.attackAbility = ABILITY_HI_JUMP;
        }
    }
    switch (gCurTask->variant) {
    case 0:
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 5);
        PlayerStopAxes(2);
        PlayerStartOffsetScript(5);
        TaskSetFrame(0xB2D);
        TaskYieldTrampoline(4);
        gCurTask->variant = 1;
        gCurTask->player->unk14 = 4;
        /* fallthrough */
    case 1:
        PlayerSetMotionYPreset(38);
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        gCurTask->player->hitBoxSet = gUnk_0873CF5C;
        PlayerStartSfx(SE_HI_JUMP_ATTACK, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 4);
        gCurTask->unk46 = 0;
        gCurTask->unk28 = 2;
        TaskYieldTrampoline(23);
        PlayerSetMotionYPreset(0);
        TaskYieldTrampoline(10);
        gCurTask->variant = 2;
        /* fallthrough */
    case 2:
        {
            struct Task *t = gCurTask;
            t->player->unk42 &= 0xFFEF;
            SetPlayerInvulnerability(255, 0, t->player->playerIndex);
        }
        TaskYieldTrampoline(13);
        PlayerSetMotionYPreset(1);
        TaskYieldTrampoline(5);
        gCurTask->variant = 3;
        /* fallthrough */
    case 3:
        gCurTask->player->unk42 &= 0xFFEF;
        PlayerSetMotionYPreset(2);
        gCurTask->player->prevMode = gCurTask->player->mode;
        gCurTask->player->mode = 5;
        gCurTask->updateState = PLAYER_ACTION_HANDLER_FALL;
        gCurTask->player->hitBoxSet = 0;
        gCurTask->variant = 4;
        gCurTask->player->unk14 = 300;
        TaskSetFrame(0xAD2);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 4);
        /* fallthrough */
    case 4:
        {
            struct Task *t = gCurTask;
            struct PlayerState *p;
            t->player->unk42 &= 0xFFEF;
            p = t->player;
            if (p->mode != 5) {
                p->prevMode = p->mode;
                gCurTask->player->mode = 5;
                gCurTask->updateState = PLAYER_ACTION_HANDLER_FALL;
                gCurTask->player->hitBoxSet = 0;
                PlayerSetMotionYPreset(2);
            }
        }
        {
            struct Task *t = gCurTask;
            t->player->unk14 = 30;
            t->unk46 = gUnk_0873D3B8[t->player->ability][1];
        }
        while (1) {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
}

void PlayerActionHiJumpUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 1:
        t->player->unk42 &= 0xFFEF;
        if ((gFrameCount & 7) <= 3) {
            RequestCopy(2, gUnk_081F59F0, gObjPalette + (t->tileWord >> 12) * 32, 64);
            gCurTask->player->unk42 |= 16;
        }
        /* fallthrough */
    case 2:
        {
            struct Task *u = gCurTask;
            struct PlayerState *p = u->player;
            if ((s16)p->unk14 == 0) {
                if (gLatchedPressedKeys[p->playerIndex] & 2) {
                    u->variant = 3;
                    TaskSetEntry(PlayerActionHiJump, gCurTaskIdx);
                    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                    PlayerStopSfx();
                    CreatePlayerEffect(gCurTask->player->playerIndex, 42, 3);
                    PlayerStopAxes(2);
                }
            } else {
                p->unk14--;
            }
        }
        /* fallthrough */
    case 3:
        {
            struct Task *u = gCurTask;
            if (u->unk28-- == 0) {
                u->playerHiJumpFramePhase = (u->playerHiJumpFramePhase + 1) & 3;
                u->unk28 = 2;
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873C228);
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            PlayerFaceHeldDirection();
            PlayerSetMotionXPreset(11, 60);
        } else {
            PlayerSetMotionXPreset(11, 61);
        }
        if (gTerrainResult.unk0 != 0)
            PlayerStopAxes(1);
        {
            struct Task *u = gCurTask;
            if ((u8)(u->variant - 1) <= 1) {
                s32 a;
                if (abs(u->velX) <= 0x4000)
                    a = 0xB2E;
                else if (abs(u->velX) <= 0x10000)
                    a = 0xB32;
                else if (abs(u->velX) <= 0x14000)
                    a = 0xB36;
                else if (abs(u->velX) <= 0x1C000)
                    a = 0xB3A;
                else
                    a = 0xB3E;
                {
                    struct Task *w = gCurTask;
                    if (w->velX == 0)
                        TaskSetFrame((s16)(w->playerHiJumpFramePhase + a));
                    else if (w->velX < 0)
                        TaskSetFrameFlip(w->playerHiJumpFramePhase + a);
                    else
                        TaskSetFrameNoFlip((s16)(w->playerHiJumpFramePhase + a));
                }
            }
        }
        {
            struct Task *w = gCurTask;
            if (w->onGround & 1) {
                PlayerCheckBump();
                PlayerLand(1);
                PlayerRequestLocomotion();
                break;
            }
            if (w->velY < 0 && (gTerrainResult.ceilingHits != 0 || (w->player->boundsClamp & 4))) {
                w->velY = 0;
                w->variant = 4;
                TaskSetEntry(PlayerActionHiJump, gCurTaskIdx);
                SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                PlayerStopSfx();
                return;
            }
        }
        PlayerCheckLadder();
        break;
    }
    {
        struct PlayerState *p = gCurTask->player;
        if (p->requestedAction != PLAYER_ACTION_NONE)
            p->unk42 &= 0xFFEF;
    }
}
