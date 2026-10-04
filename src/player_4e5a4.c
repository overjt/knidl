#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"

/* player_4e5a4.c (0x0804E5A4-0x0804E78B, issue #90).
 *
 * Player action bodies, part 25: action 49, its re-entry callback and
 * per-frame handler 46.  Action 49 is a move set one level down: its enter
 * body PlayerActionBall (mode 13, Task.u80.attackAbility = 18) clears Task.variant/unk74/
 * unk24 and PlayerState.bumpKind and dispatches Task.variant through its own
 * table of nine sub-actions gPlayerBallVariants; PlayerActionBallEnterVariant, the callback the
 * sub-handlers re-bind, does the same after clearing PlayerState.running.
 * Its handler PlayerActionBallUpdate plays the animation gUnk_0873DB0A[Task.unk46]
 * mirrored by Task.unk6E, registers the body collider (gUnk_0873C28C, and
 * the block hit-box set gUnk_0873CF7C while moving) when the vertical
 * speed exceeds 2 pixels a frame, requests action 23 through M11's
 * PlayerHasCrossedWaterSurface and otherwise runs the sub-handler Task.variant of
 * gPlayerBallVariantUpdates. */

/* task / sprite services (landed prototypes) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */

void PlayerActionBall(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 46;
    gCurTask->unk74 = 0;
    gCurTask->player->bumpKind = 0;
    gCurTask->unk24 = 0;
    gCurTask->variant = 0;
    gCurTask->u80.attackAbility = ABILITY_BALL;
    CallTableEntry(gCurTask->variant, 9, gPlayerBallVariants);
}

void PlayerActionBallEnterVariant(void)
{
    gCurTask->player->running = 0;
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    CallTableEntry(gCurTask->variant, 9, gPlayerBallVariants);
}

void PlayerActionBallUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->playerBallRollFrame != -1)
    {
        if (t->unk24 == 0)
        {
            if (t->unk6E == 1)
                TaskSetFrameNoFlip((s16)gUnk_0873DB0A[t->playerBallRollFrame]);
            else
                TaskSetFrameFlip(gUnk_0873DB0A[t->playerBallRollFrame]);
        }
        if (gCurTask->playerBallRollFrame == 9)
            gCurTask->unk6E = gCurTask->facing;
    }
    if (abs(gCurTask->velY) > 0x20000)
    {
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        if ((u32)abs(gCurTask->velX) > 0x8000)
            gCurTask->player->hitBoxSet = gUnk_0873CF7C;
        else
            gCurTask->player->hitBoxSet = 0;
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873C28C);
    }
    else
    {
        gCurTask->player->hitBoxSet = 0;
        SetPlayerInvulnerability(0xFF, 0, gCurTask->player->playerIndex);
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
        gCurTask->player->requestedAction = 23;
    else
        CallTableEntry(gCurTask->variant, 9, gPlayerBallVariantUpdates);
}
