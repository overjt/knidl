#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_spark_cutter.c (0x080449C8-0x08044D03, issue #87).
 *
 * Player action bodies, part 11: per-frame handler 30 and action 34
 * with its handler 31.  PlayerActionSparkUpdate (handler 30) is the
 * per-frame half of M11's action 33 (PlayerActionSpark): in state 1 it counts
 * Task.unk28 down and re-binds the coroutine to state 2 once B is no
 * longer held, and over the animation frames 0x36B-0x372 it blends the
 * player's palettes gUnk_081BE6BC[player] into the OBJ palette buffer
 * (BlendColors, raising PlayerState.statusFlags bit 4 while it does).
 * PlayerActionCutter (action 34, mode 13) is a linear yield script (animation
 * 0x3E9, effect 28 on the ground, M14's CreatePlayerObjectLowSlot, sound 144); its
 * handler PlayerActionCutterUpdate re-enters it on a newly-pressed B and requests
 * action 2 when left or right is held on the ground. */

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSetEntry(void *a, u32 i);
void CreatePlayerObjectLowSlot(s32 a0, s32 a1, s32 a2);   /* M14, src/player_object_ufo_shot.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionSparkUpdate(void)
{
    struct Task *t = gCurTask;
    u8 *st = &t->variant;

    switch (*st) {
    case 0:
        break;
    case 1:
        if (t->playerAttackHoldTimer == 0) {
            if ((gLatchedHeldKeys[t->player->playerIndex] & 2) == 0) {
                *st = 2;
                TaskSetEntry(PlayerActionSpark, gCurTaskIdx);
            }
        } else {
            t->playerAttackHoldTimer--;
        }
        switch (gCurTask->frame) {
        case 0x36B:
        case 0x36C:
        case 0x36F:
        case 0x370:
            {
                struct Task *u = gCurTask;
                u->player->statusFlags &= ~PLAYER_STATUS_PALETTE_LOCKED;
                u->playerNextBankBlendRatio = 0;
                u->playerBankBlendRatio = 0;
            }
            break;
        case 0x36E:
        case 0x372:
            {
                struct Task *u = gCurTask;
                u->playerBankBlendRatio += 128;
                if (u->playerBankBlendRatio > 256)
                    u->playerBankBlendRatio = 256;
            }
            {
                struct Task *u = gCurTask;
                BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128],
                             (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 32],
                             (u16)u->playerBankBlendRatio, 16,
                             (u16 *)(gObjPalette + ((u->tileWord >> 12) << 5)));
            }
            /* fallthrough */
        case 0x36D:
        case 0x371:
            {
                struct Task *u = gCurTask;
                u->playerNextBankBlendRatio += 64;
                if ((s16)u->playerNextBankBlendRatio > 256)
                    u->playerNextBankBlendRatio = 256;
            }
            {
                struct Task *u = gCurTask;
                BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 64],
                             (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 96],
                             u->playerNextBankBlendRatio, 16,
                             (u16 *)(gObjPalette + (((u->tileWord >> 12) + 1) << 5)));
            }
            gCurTask->player->statusFlags |= PLAYER_STATUS_PALETTE_LOCKED;
            break;
        }
        break;
    case 2:
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    sub_0803e55c();
    {
        struct PlayerState *p = gCurTask->player;
        if (p->requestedAction != PLAYER_ACTION_NONE)
            p->statusFlags &= ~PLAYER_STATUS_PALETTE_LOCKED;
    }
}

void PlayerActionCutter(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_CUTTER;
    gCurTask->playerAttackCutIn = 0;
    gCurTask->u80.attackAbility = ABILITY_NORMAL;
    TaskSetFrame(0x3E9);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    if (gCurTask->onGround & 1)
        CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
    CreatePlayerObjectLowSlot(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_CUTTER_BLADE, 0);
    gCurTask->playerAttackCutIn++;
    PlaySfxIfLocalPlayer(SE_CUTTER_ATTACK, gCurTask->player->playerIndex);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->playerAttackCutIn++;
    TaskSleepForever();
}

void PlayerActionCutterUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->playerAttackCutIn != 0) {
        if (t->playerAttackCutIn == 1) {
            if (gLatchedPressedKeys[t->player->playerIndex] & 2) {
                TaskSetEntry(PlayerActionCutter, gCurTaskIdx);
            } else if (t->onGround & 1) {
                if (gLatchedHeldKeys[t->player->playerIndex] & 48) {
                    PlayerTurnToHeldDirection();
                    gCurTask->player->requestedAction = PLAYER_ACTION_WALK;
                }
            }
        } else {
            PlayerRequestLocomotion();
        }
    }
    sub_0803e55c();
}
