#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "collision.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_4dc08.c (0x0804DC08-0x0804E39F, issue #90).
 *
 * Player action bodies, part 24: actions 30 and 31 and per-frame handler
 * 27 (handler 28, the pair of action 31, is PR #133's src/sub_0804e3a0.c).
 * PlayerActionBackdrop (action 30, mode 10) is a three-state `switch (Task.variant)`
 * whose states fall into each other: state 0 winds up (animation 0xE79,
 * camera preset PlayerSetMotionXPreset(11, 5)), state 1 plays sound 200 and effect 6
 * and loops animations 0xE7C/0xE85 four times, and state 2 either swings
 * (animation 0xE7D, M11's PlayerSetMotionYPreset steering, effect 28 when it lands)
 * or, once PlayerState.attachedCount is set, plays sound 177, sets
 * PlayerState.statusFlags bit 9 and stops; every pass counts Task.unk28.  Its
 * handler PlayerActionBackdropUpdate re-binds state 2 when PlayerState.attachedCount is set, runs
 * the hit test TaskBreakFirstBlock(gUnk_0873CC64) in state 1 (which spawns
 * CreateBlockStar's object and marks PlayerState.catchKind) and, in state 2,
 * requests action 53, 8 or 1 once the swing is over.  PlayerActionThrow
 * (action 31, mode 10; the twin of M10's PlayerActionInhale) clears the three
 * records gUnk_02007E90[player][] (and gUnk_02007CF4[player] in link
 * play), plays sound 103 and holds animation 0xF71 with PlayerState.actionFlags
 * bit 2 set until PlayerState.attachedCount is non-zero and equal to unk08, then
 * recovers or releases (sound 201, PlayerState.statusFlags bit 9). */

/* Not from room.h: this file's view of gBrokenBlockX differs (lesson 3.517). */
extern s16 gBrokenBlockX[];
extern s32 gUnk_03001F2C;               /* boot_091ac.c spelling */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
u16 TaskBreakFirstBlock(struct HitBoxSet *p, s32 e);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
s32 CreateBlockStar(s32 x, s32 y, u32 p2, u8 p3, u8 p4);   /* this caller passes x and y unnarrowed (ldrsh; adds #8) */

void PlayerActionBackdrop(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_BACKDROP;
    if (gCurTask->player->prevMode != 10)
    {
        gCurTask->playerActionDone28 = 0;
        gCurTask->variant = 0;
        gCurTask->player->unk16 = 0;
        {
            struct PlayerState *p = gCurTask->player;

            p->catchKind = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
    }
    switch (gCurTask->variant)
    {
    case 0:
        PlayerStopAxes(2);
        PlayerSetMotionXPreset(11, 5);
        TaskSetFrame(0xE79);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        PlayerStopAxes(1);
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        PlaySfxIfLocalPlayer(200, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 260);
        PlayerSetMotionXPreset(11, 6);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
        {
            TaskSetFrame(0xE7C);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xE85);
            TaskYieldTrampoline(2);
        }
        gCurTask->variant = 2;
    case 2:
        if ((s8)gCurTask->player->attachedCount == 0)
        {
            if (gCurTask->onGround & 1)
                CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_IMPACT_STAR, 0);
            else if (!(gCurTask->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
            PlayerSetMotionXPreset(11, 7);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            TaskSetFrame(0xE7D);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            TaskSetFrame(0xE7D);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->onGround & 1)
            {
                PlayerStopAxes(2);
                CreatePlayerEffect(gCurTask->player->playerIndex, 28, 5);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                PlayerSetMotionXPreset(11, 8);
                gCurTask->frame++;
                TaskYieldTrampoline(8);
            }
        }
        else
        {
            PlaySfxIfLocalPlayer(177, gCurTask->player->playerIndex);
            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            gCurTask->player->statusFlags |= PLAYER_STATUS_NO_TERRAIN_DAMAGE;
            PlayerStopAxes(1);
            TaskSetFrame(0xE83);
            TaskYieldTrampoline(1);
        }
        gCurTask->playerActionDone28++;
    }
    TaskSleepForever();
}

void PlayerActionBackdropUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;

    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        break;
    case 1:
        if ((s8)t->player->attachedCount != 0)
        {
            t->variant = 2;
            TaskSetEntry(PlayerActionBackdrop, gCurTaskIdx);
        }
        else if (t->player->catchKind == 0)
        {
            if (TaskBreakFirstBlock((struct HitBoxSet *)gUnk_0873CC64, t->player->playerIndex) != 0)
            {
                CreateBlockStar(gBrokenBlockX[0] + 8, gBrokenBlockY[0] + 8, gCurTaskIdx, HIT_KIND_GRAB, HIT_EFFECT_BACKDROP);
                gCurTask->player->catchKind = 2;
            }
            u = gCurTask;
            if (u->player->catchKind == 0)
            {
                if (gTerrainResult.unk0 != 0)
                {
                    RequestScreenShake(1);
                    gCurTask->player->requestedAction = PLAYER_ACTION_RECOIL;
                }
                else
                {
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BED8);
                }
            }
        }
        break;
    case 2:
        if (t->playerActionDone28 != 0)
        {
            p = t->player;
            if ((s8)p->heldCount != 0)
                p->requestedAction = PLAYER_ACTION_BACKDROP_HOLD;
            else if (!(t->onGround & 1))
                p->requestedAction = PLAYER_ACTION_HIGH_FALL;
            else
                p->requestedAction = PLAYER_ACTION_STAND;
        }
        if (gTerrainResult.unk0 != 0)
            PlayerStopAxes(1);
        u = gCurTask;
        if (u->velY != 0)
        {
            if (PlayerHasCrossedWaterSurface(0) != 0)
            {
                PlayerSetWaterMotionY();
                if ((s8)gCurTask->player->attachedCount == 0)
                    gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
            }
            else if (PlayerCheckLanding() != 0)
            {
                struct Task *v = gCurTask;

                if (!(v->waterFlags & 1) && (v->velY & 0xFFFF0000))
                    CreatePlayerEffect(v->player->playerIndex, PLAYER_EFFECT_VARIANT_IMPACT_STAR, 0);
                PlayerStopAxes(2);
            }
        }
        else if (!(u->onGround & 1))
        {
            if (!(u->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        }
        break;
    }
}

void PlayerActionThrow(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_THROW;
    t = gCurTask;
    if (t->player->prevMode != 10)
    {
        t->playerThrowCaught = 0;
        t->variant = 0;
    }
    u = gCurTask;
    switch (u->variant)
    {
    case 0:
        u->playerCatchBlockDelay = 1;
        u->playerThrowGrabTimer = 30;
        u->player->unk16 = 0;
        {
            struct PlayerState *p = gCurTask->player;

            p->catchKind = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
        if (gUnk_0300244C != 0)
            gUnk_02007CF4[gCurTask->player->playerIndex] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].offsetX = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].offsetY = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].velX = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].frameTimer = 1;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].frame = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        gCurTask->variant = 1;
        TaskSetFrame(0xF6E);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStartSfx(103, gCurTask->player->playerIndex);
        gCurTask->player->actionFlags |= PLAYER_ACTION_FLAG_CATCHING;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        while (1)
        {
            TaskSetFrame(0xF71);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame++;
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gCurTask->variant = 2;
    case 2:
        gCurTask->player->actionFlags &= 0xFFFB;
        PlayerStopSfx();
        if ((s8)gCurTask->player->heldCount == 0)
        {
            TaskSetFrame(0xF6E);
            TaskYieldTrampoline(2);
        }
        else
        {
            PlaySfxIfLocalPlayer(SE_THROW_GRAB, gCurTask->player->playerIndex);
            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            gCurTask->player->statusFlags |= PLAYER_STATUS_NO_TERRAIN_DAMAGE;
            TaskSetFrame(0xF73);
            TaskYieldTrampoline(1);
        }
        gCurTask->variant = 3;
    }
    TaskSleepForever();
}
