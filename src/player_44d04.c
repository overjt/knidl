#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_44d04.c (0x08044D04-0x080455C7, issue #87).
 *
 * Player action bodies, part 12: action 35 and per-frame handler 32.
 * PlayerActionSword (action 35, mode 13) is an attack with a ground form
 * (Task.variant = 0) and an air form (1), each in two variants picked by
 * Task.waterFlags bit 0 (Task.unk30).  It installs the player's collider
 * record gPlayerBodyBoxes[] (registered with M05's RegisterCollider) and
 * block hit-box set gPlayerHitBoxSets[] (tested by M09's TaskBreakBlocks) from
 * gUnk_0873BF28/gUnk_0873CCAC or gUnk_0873BF84/gUnk_0873CCFC, points
 * PlayerState.hitBoxSet at the set while the swing is live and steps it
 * through the 8-byte rows of gUnk_0873CCB4 (ground) or gUnk_0873CD04 /
 * gUnk_0873CD44 (air, two passes, counters Task.unk6C/unk6E) with
 * LoadPlayerHitBoxSet, with effects 31 and 28 and sounds 147/148; the air form
 * restores the facing Task.facing it saved in Task.unk34.  Its handler
 * PlayerActionSwordUpdate copies the collider row Task.unk2C of gUnk_0873BF3C
 * (ground) or gUnk_0873BF98/gUnk_0873BFD8 (air) with LoadPlayerBodyBoxRect and
 * registers it every frame, requests action 23 through PlayerHasCrossedWaterSurface and
 * picks the PlayerSetMotionXPreset preset from the held left/right keys. */

void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionSword(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 32;
    {
        struct Task *t = gCurTask;
        t->playerActionDone28 = 0;
        t->unk2C = -1;
        if (t->waterFlags & 1)
            t->unk30 = 1;
        else
            t->unk30 = 0;
    }
    gCurTask->unk34 = gCurTask->facing;
    gCurTask->u80.attackAbility = ABILITY_SWORD;
    if (gCurTask->onGround & 1)
        gCurTask->variant = 0;
    else
        gCurTask->variant = 1;
    switch (gCurTask->variant) {
    case 0:
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873BF28;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CCAC;
        if (gCurTask->unk30 == 0) {
            TaskSetFrame(0x4BA);
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 31, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            PlaySfxIfLocalPlayer(147, gCurTask->player->playerIndex);
            {
                struct Task *t = gCurTask;
                t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                t->unk2C++;
                LoadPlayerHitBoxSet(t->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } else {
            TaskSetFrame(0x4CA);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            CreatePlayerEffect(gCurTask->player->playerIndex, 31, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            PlaySfxIfLocalPlayer(147, gCurTask->player->playerIndex);
            {
                struct Task *t = gCurTask;
                t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                t->unk2C++;
                LoadPlayerHitBoxSet(t->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->unk2C = -1;
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        break;
    case 1:
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873BF84;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CCFC;
        gCurTask->player->hitBoxSet = 0;
        if (gCurTask->unk30 == 0) {
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++) {
                PlaySfxIfLocalPlayer(148, gCurTask->player->playerIndex);
                {
                    struct Task *t = gCurTask;
                    t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                    t->unk2C = 0;
                    LoadPlayerHitBoxSet(t->player->playerIndex, (s32)gUnk_0873CD04);
                }
                TaskSetFrame(0x4DA);
                TaskYieldTrampoline(1);
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 6; gCurTask->unk6E++) {
                    gCurTask->unk2C++;
                    LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CD04 + gCurTask->unk2C * 8));
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x4E2);
            TaskYieldTrampoline(1);
        } else {
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++) {
                PlaySfxIfLocalPlayer(148, gCurTask->player->playerIndex);
                {
                    struct Task *t = gCurTask;
                    t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                    t->unk2C = 0;
                    LoadPlayerHitBoxSet(t->player->playerIndex, (s32)gUnk_0873CD44);
                }
                TaskSetFrame(0x4E3);
                TaskYieldTrampoline(1);
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 10; gCurTask->unk6E++) {
                    gCurTask->unk2C++;
                    LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CD44 + gCurTask->unk2C * 8));
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x4EF);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSwordUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 0:
        if (t->playerActionDone28 != 0) {
            PlayerRequestLocomotion();
        } else {
            if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
                gCurTask->player->requestedAction = 23;
            {
                struct Task *u = gCurTask;
                if (u->unk2C != -1) {
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BF3C + u->unk2C * 8);
                    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                                 (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
                }
            }
        }
        PlayerSetMotionXPreset(0, 72);
        break;
    case 1:
        if ((t->onGround & 1) || t->playerActionDone28 != 0)
            PlayerRequestLocomotion();
        else if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = 23;
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1) {
                if (u->unk30 == 0)
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BF98 + u->unk2C * 8);
                else
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BFD8 + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 56);
                else
                    PlayerSetMotionXPreset(10, 58);
            } else {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 57);
                else
                    PlayerSetMotionXPreset(10, 59);
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
