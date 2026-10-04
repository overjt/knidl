#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_49b48.c (0x08049B48-0x08049F97, issue #88).
 *
 * Player action bodies, part 16: actions 44-45 and handlers 41-42, the
 * twins of M11's actions 32-33.  PlayerActionIce (action 44, mode 13) is
 * M11's PlayerActionFire with other constants (Task.u80.attackAbility = 13, animations
 * 0xA0A-0xA0E, sound 140, M14's CreatePlayerObject(player, 7, 0..1), effects
 * 40 x3 and 28): state 0 winds up, state 1 loops the animation until
 * its handler PlayerActionIceUpdate (M11's PlayerActionFireUpdate) re-binds state 2 once
 * Task.unk28 has run out and B is released, and state 2 winds down.
 * PlayerActionFreeze (action 45) is M11's PlayerActionSpark likewise (animations
 * 0xA86/0xA8E, effect 41 x4, sound 141); its handler PlayerActionFreezeUpdate also
 * registers the collider gUnk_0873C214 and tests the block hit-box set
 * gUnk_0873CF4C every frame in state 1. */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionIce(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_ICE;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            struct Task *u;
            t->variant = 0;
            u = gCurTask;
            u->unk28 = 15;
            u->u80.attackAbility = ABILITY_ICE;
        }
    }
    switch (gCurTask->variant) {
    case 0:
        TaskSetFrame(0xA0A);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        TaskSetFrame(0xA0D);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        {
            struct PlayerState *p = gCurTask->player;
            if ((p->unk42 & 128) == 0)
                PlayerStartSfx(SE_ICE_ATTACK, p->playerIndex);
        }
        CreatePlayerObject(gCurTask->player->playerIndex, 7, 0);
        CreatePlayerObject(gCurTask->player->playerIndex, 7, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 40, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 40, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 40, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 28, 4);
        while (1) {
            TaskSetFrame(0x9FA);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 14);
        }
    case 2:
        TaskSetFrame(0xA0E);
        TaskYieldTrampoline(3);
        PlayerStopSfx();
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->variant = 3;
        break;
    }
    TaskSleepForever();
}

void PlayerActionIceUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            u16 *p = (u16 *)gLatchedHeldKeys;
            if ((p[t->player->playerIndex] & 2) == 0) {
                t->variant = 2;
                TaskSetEntry(PlayerActionIce, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        break;
    case 2:
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    sub_0803e55c();
}

void PlayerActionFreeze(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_FREEZE;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            struct Task *u;
            t->variant = 0;
            u = gCurTask;
            u->unk28 = 15;
            u->u80.attackAbility = ABILITY_FREEZE;
        }
    }
    switch (gCurTask->variant) {
    case 0:
        TaskSetFrame(0xA8E);
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        CreatePlayerEffect(gCurTask->player->playerIndex, 41, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 41, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 41, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 41, 3);
        PlayerStartSfx(SE_FREEZE_ATTACK, gCurTask->player->playerIndex);
        while (1) {
            TaskSetFrame(0xA86);
            TaskYieldTrampoline(1);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 6);
        }
    case 2:
        PlayerStopSfx();
        TaskSetFrame(0xA8E);
        TaskYieldTrampoline(1);
        gCurTask->variant = 3;
        break;
    }
    TaskSleepForever();
}

void PlayerActionFreezeUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            if ((gLatchedHeldKeys[t->player->playerIndex] & 2) == 0) {
                t->variant = 2;
                TaskSetEntry(PlayerActionFreeze, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        RegisterCollider((u8)gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                     gUnk_0873C214);
        TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CF4C, gCurTask->player->playerIndex);
        break;
    case 2:
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    sub_0803e55c();
}
