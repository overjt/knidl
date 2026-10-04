#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_4a54c.c (0x0804A54C-0x0804AB6F, issue #88).
 *
 * Player action bodies, part 18: actions 47-48 and handlers 44-45.
 * PlayerActionBeam (action 47, mode 13) is a linear yield script (animation
 * 0xBBD, M14's CreatePlayerObject(player, 8, 0..2), three two-step loops over
 * 0xBBF/0xBC1/0xBC3); its handler PlayerActionBeamUpdate lets M11's PlayerRequestLocomotion
 * end it once Task.unk28 is set.  PlayerActionStone (action 48, mode 13) is
 * a re-entrant four-state move: state 0 winds up (sound 152, velocity
 * preset 39 and animation 0xC53 on the ground, then 0xC40), state 1
 * holds animation 0xC48 until B is newly pressed after Task.unk28
 * frames, state 2 releases it (preset 42, 13 or 2 by ground and
 * variant, animation 0xC4D, effect 43 x4) and state 3 ends it.  Its
 * handler PlayerActionStoneUpdate registers the collider gUnk_0873C264 in the air
 * or tests the block set gUnk_0873CF6C on the ground in state 0; in
 * state 1 it plays the landing (effect 27, sound 143, RequestScreenShake(2))
 * and the terrain animation gUnk_0873B654[gTerrainResult.unk4] on the
 * ground, or the rising presets 40/41 and the collider gUnk_0873C278 in
 * the air, and it stops a rise on a ceiling hit (gTerrainResult.unk1). */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionBeam(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_BEAM;
    gCurTask->playerActionDone28 = 0;
    gCurTask->u80.attackAbility = ABILITY_NORMAL;
    TaskSetFrame(0xBBD);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    CreatePlayerObject(gCurTask->player->playerIndex, 8, 0);
    CreatePlayerObject(gCurTask->player->playerIndex, 8, 1);
    CreatePlayerObject(gCurTask->player->playerIndex, 8, 2);
    gCurTask->playerLoopCount = 0;
    do {
        TaskSetFrame(0xBBF);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    } while ((s16)++gCurTask->playerLoopCount <= 1);
    gCurTask->playerLoopCount = 0;
    do {
        TaskSetFrame(0xBC1);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    } while ((s16)++gCurTask->playerLoopCount <= 1);
    gCurTask->playerLoopCount = 0;
    do {
        TaskSetFrame(0xBC3);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    } while ((s16)++gCurTask->playerLoopCount <= 1);
    TaskSetFrame(0xBBF);
    TaskYieldTrampoline(4);
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionBeamUpdate(void)
{
    if (gCurTask->playerActionDone28 != 0)
        PlayerRequestLocomotion();
    sub_0803e55c();
}

void PlayerActionStone(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_STONE;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            struct Task *u;
            t->variant = 0;
            u = gCurTask;
            u->unk28 = 15;
            u->u80.attackAbility = ABILITY_STONE;
        }
    }
    switch (gCurTask->variant) {
    case 0:
        PlaySfxIfLocalPlayer(SE_STONE_ATTACK, gCurTask->player->playerIndex);
        if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) == 0)
            PlayerStopAxes(1);
        if (gCurTask->onGround & 1) {
            PlayerSetMotionYPreset(39);
            TaskSetFrame(0xC53);
            TaskYieldTrampoline(3);
        } else {
            PlayerStopAxes(2);
        }
        TaskSetFrame(0xC40);
        TaskYieldTrampoline(1);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 3);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->layer = 13;
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        PlayerStopAxes(1);
        TaskSetFrame(0xC48);
        while (1) {
            if (gCurTask->unk28 == 0) {
                if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
                    break;
            } else {
                gCurTask->unk28--;
            }
            TaskYieldTrampoline(1);
        }
        gCurTask->variant = 2;
        /* fallthrough */
    case 2:
        gCurTask->layer = 7;
        {
            struct Task *t = gCurTask;
            if (t->onGround & 1) {
                PlayerSetMotionYPreset(42);
            } else if (t->waterFlags & 1) {
                PlayerSetMotionYPreset(13);
            } else {
                PlayerSetMotionYPreset(2);
                gCurTask->velY = gCurTask->speedLimitY;
            }
        }
        TaskSetFrame(0xC4D);
        TaskYieldTrampoline(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 43, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 43, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 43, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 43, 3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 3);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->variant = 3;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    }
    TaskSleepForever();
}

void PlayerActionStoneUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 0:
        if ((t->onGround & 1) == 0) {
            PlayerTurnToHeldDirection();
            PlayerSetMotionXPreset(7, 72);
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                         gUnk_0873C264);
        } else {
            PlayerLand(1);
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CF6C, gCurTask->player->playerIndex);
        }
        break;
    case 1:
        if (t->onGround & 1) {
            if (t->velY != 0) {
                CreatePlayerEffect(t->player->playerIndex, 27, 0);
                PlaySfxIfLocalPlayer(SE_STONE_LAND, gCurTask->player->playerIndex);
                RequestScreenShake(2);
                PlayerStopAxes(2);
            }
            if (gTerrainResult.slope == 0)
                TaskSetFrame(0xC48);
            else
                TaskSetFrameNoFlip(gUnk_0873B654[gTerrainResult.slope]);
            {
                struct PlayerState *p = gCurTask->player;
                if (p->invulnerability != 2)
                    SetPlayerInvulnerability(2, 0, p->playerIndex);
            }
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CF6C, gCurTask->player->playerIndex);
        } else {
            TaskSetFrame(0xC48);
            if (gCurTask->waterFlags & 1)
                PlayerSetWaterMotionY();
            if ((gCurTask->waterFlags & 1) == 0)
                PlayerSetMotionYPreset(40);
            else
                PlayerSetMotionYPreset(41);
            if (gCurTask->velY >= 0) {
                struct PlayerState *p = gCurTask->player;
                if (p->invulnerability != 3)
                    SetPlayerInvulnerability(3, 0, p->playerIndex);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             gUnk_0873C278);
            }
        }
        break;
    case 2:
        if (t->onGround & 1)
            PlayerLand(1);
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    if (gCurTask->velY < 0 && gTerrainResult.ceilingHits != 0)
        gCurTask->velY = 0;
}
