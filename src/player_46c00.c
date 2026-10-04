#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_46c00.c (0x08046C00-0x080474E7, issue #87).
 *
 * Player action bodies, part 16: action 40 and per-frame handler 37.
 * PlayerActionHammer (action 40, mode 13) is PlayerActionSword's sibling: the same
 * opening, a ground form (Task.variant = 0) and an air form (1) in two
 * variants (Task.unk30 = Task.waterFlags bit 0), the collider record
 * gUnk_0873C060 and the block hit-box sets gUnk_0873CDB4 (ground) /
 * gUnk_0873CDF4 (air) whose rows gUnk_0873CDBC, gUnk_0873CDFC and
 * gUnk_0873CE64 it steps with LoadPlayerHitBoxSet.  On the ground it probes the
 * metatile 20 pixels ahead (sub_0802259c); a solid one (bits 0-1) gives
 * the impact: sound 241, the screen shake RequestScreenShake(2) and effect 35.
 * Its handler PlayerActionHammerUpdate is PlayerActionSwordUpdate's twin with the collider rows
 * gUnk_0873C074, gUnk_0873C0C0 and gUnk_0873C128, and in the air it
 * records the held left/right direction in Task.unk34. */

void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionHammer(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_HAMMER;
    {
        struct Task *t = gCurTask;
        t->playerActionDone28 = 0;
        t->playerAttackStep = -1;
        if (t->waterFlags & 1)
            t->unk30 = 1;
        else
            t->unk30 = 0;
    }
    gCurTask->unk34 = gCurTask->facing;
    gCurTask->u80.attackAbility = ABILITY_HAMMER;
    if (gCurTask->onGround & 1)
        gCurTask->variant = 0;
    else
        gCurTask->variant = 1;
    switch (gCurTask->variant) {
    case 0:
        PlaySfxIfLocalPlayer(130, gCurTask->player->playerIndex);
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct PlayerBodyBox *)gUnk_0873C060;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct PlayerHitBoxSet *)gUnk_0873CDB4;
        gCurTask->player->hitBoxSet = &gPlayerHitBoxSets[gCurTask->player->playerIndex];
        LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)gUnk_0873CDBC);
        if (gCurTask->unk30 == 0) {
            gCurTask->playerAttackStep++;
            TaskSetFrame(0x7D2);
            TaskYieldTrampoline(8);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++) {
                gCurTask->playerAttackStep++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->playerAttackStep * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->playerAttackStep++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->playerAttackStep * 8));
            {
                u16 x;
                s16 r;
                if (gCurTask->facing == 1)
                    x = gCurTask->pixelX + 20;
                else
                    x = gCurTask->pixelX - 20;
                r = sub_0802259c(x, gCurTask->pixelY + 13);
                if (r & 3) {
                    PlaySfxIfLocalPlayer(241, gCurTask->player->playerIndex);
                    RequestScreenShake(2);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame--;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(8);
                } else {
                    TaskSetFrame(0x7D9);
                    TaskYieldTrampoline(11);
                }
            }
            gCurTask->playerAttackStep = -1;
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x7DD);
            TaskYieldTrampoline(2);
            TaskSetFrame(0x7DA);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } else {
            gCurTask->playerAttackStep++;
            TaskSetFrame(0x7DE);
            TaskYieldTrampoline(10);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++) {
                gCurTask->playerAttackStep++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->playerAttackStep * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
            gCurTask->playerAttackStep++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->playerAttackStep * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->playerAttackStep++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->playerAttackStep * 8));
            {
                u16 x;
                s16 r;
                if (gCurTask->facing == 1)
                    x = gCurTask->pixelX + 20;
                else
                    x = gCurTask->pixelX - 20;
                r = sub_0802259c(x, gCurTask->pixelY + 13);
                if (r & 3) {
                    PlaySfxIfLocalPlayer(241, gCurTask->player->playerIndex);
                    RequestScreenShake(2);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame--;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(10);
                } else {
                    TaskSetFrame(0x7E5);
                    TaskYieldTrampoline(13);
                }
            }
            gCurTask->playerAttackStep = -1;
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x7E9);
            TaskYieldTrampoline(4);
            TaskSetFrame(0x7E6);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        PlaySfxIfLocalPlayer(131, gCurTask->player->playerIndex);
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct PlayerBodyBox *)gUnk_0873C060;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct PlayerHitBoxSet *)gUnk_0873CDF4;
        gCurTask->player->hitBoxSet = &gPlayerHitBoxSets[gCurTask->player->playerIndex];
        LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)gUnk_0873CDFC);
        if (gCurTask->unk30 == 0) {
            gCurTask->playerAttackStep++;
            TaskSetFrame(0x7EA);
            TaskYieldTrampoline(1);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 10; gCurTask->playerLoopCount++) {
                gCurTask->playerAttackStep++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDFC + gCurTask->playerAttackStep * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->playerAttackStep++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDFC + gCurTask->playerAttackStep * 8));
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x7F6);
            TaskYieldTrampoline(1);
        } else {
            gCurTask->playerAttackStep++;
            TaskSetFrame(0x7F7);
            TaskYieldTrampoline(1);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 14; gCurTask->playerLoopCount++) {
                gCurTask->playerAttackStep++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CE64 + gCurTask->playerAttackStep * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->playerAttackStep++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CE64 + gCurTask->playerAttackStep * 8));
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x807);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionHammerUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 0:
        if (t->playerActionDone28 != 0)
            PlayerRequestLocomotion();
        else if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        {
            struct Task *u = gCurTask;
            if (u->playerAttackStep != -1) {
                LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C074 + u->playerAttackStep * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        PlayerSetMotionXPreset(0, 72);
        break;
    case 1:
        if (gLatchedHeldKeys[t->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[t->player->playerIndex] & 16)
                t->unk34 = 1;
            else
                t->unk34 = -1;
        }
        {
            struct Task *u = gCurTask;
            if ((u->onGround & 1) || u->playerActionDone28 != 0) {
                u->player->hitBoxSet = 0;
                PlayerRequestLocomotion();
            } else if (u->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0) {
                gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
            }
        }
        {
            struct Task *u = gCurTask;
            if (u->playerAttackStep != -1) {
                if (u->unk30 == 0)
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C0C0 + u->playerAttackStep * 8);
                else
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C128 + u->playerAttackStep * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 52);
                else
                    PlayerSetMotionXPreset(10, 54);
            } else {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 53);
                else
                    PlayerSetMotionXPreset(10, 55);
            }
        } else {
            if (gCurTask->unk30 == 0)
                PlayerSetMotionXPreset(11, 2);
            else
                PlayerSetMotionXPreset(11, 4);
        }
        break;
    }
    {
        struct Task *u = gCurTask;
        if ((u->onGround & 1) == 0) {
            if ((u->waterFlags & 1) == 0)
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        } else {
            PlayerLand(1);
        }
    }
    PlayerStopAtCeilingAndWall();
}
