#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "player.h"

/* player_4f948.c (0x0804F948-0x0804FFDB, issue #90).
 *
 * Player action bodies, part 26: actions 56-58 and per-frame handlers
 * 53-55.  PlayerActionStarRod (action 56, mode 13) installs the collider row
 * gUnk_0873C2C8, plays sound 155 and animations 0x1026/0x102A and spawns
 * task type #6's variant 11 through sub_08053a44; its handler PlayerActionStarRodUpdate
 * re-binds it and requests action 2 once the animation ends.
 * PlayerActionStarRodJump (action 57, mode 13) plays animation 0xFE5 with the collider
 * row gUnk_0873C304; its handler PlayerActionStarRodJumpUpdate is a `switch (Task.variant)`
 * with M11's steering and camera presets.  Action 58 is a second move set
 * one level down: PlayerActionStarRodFlight (mode 13) dispatches Task.variant through its
 * four sub-actions gUnk_0873B6AC and sub_0804fee8 is the re-entry callback;
 * its handler PlayerActionStarRodFlightUpdate runs the sub-handler Task.variant of
 * gUnk_0873B6BC, the steering helper sub_080506dc, clamps the player to
 * 16-224 x 18-132 and, once gSpriteCameraY passes 888, subtracts the
 * player's whole health (AddPlayerHealth) and requests action 17. */

/* task / sprite services (landed prototypes) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
s32 sub_08053a44(s8 player, u8 variant, s32 arg);

void PlayerActionStarRod(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 53;
    gCurTask->u80.attackAbility = 25;
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->unk2C = -1;
        gPlayerBodyBoxes[t->player->playerIndex] = *(struct M11R20 *)gUnk_0873C2C8;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        PlaySfxIfLocalPlayer(155, (u16)t->player->playerIndex);
    }
    TaskSetFrame(0x1026);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskSetFrame(0x102A);
    TaskYieldTrampoline(1);
    sub_08053a44(gCurTask->player->playerIndex, 11, 0);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        struct Task *t = gCurTask;
        t->unk2C++;
        t->frame++;
        TaskYieldTrampoline(1);
    }
    {
        struct Task *t = gCurTask;
        t->unk2C = -1;
        t->frame++;
    }
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->unk28++;
        t->frame++;
    }
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk70++;
    TaskSleepForever();
}

void PlayerActionStarRodUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        PlayerRequestLocomotion();
    }
    else
    {
        if (t->unk28 != 0)
        {
            if (gLatchedPressedKeys[t->player->playerIndex] & 2)
            {
                TaskSetEntry(PlayerActionStarRod, gCurTaskIdx);
            }
            else if ((t->onGround & 1) && (gLatchedHeldKeys[t->player->playerIndex] & 0x30))
            {
                PlayerTurnToHeldDirection();
                gCurTask->player->requestedAction = 2;
            }
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1)
            {
                LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C2DC + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             &gPlayerBodyBoxes[gCurTask->player->playerIndex]);
            }
        }
    }
    sub_0803e55c();
}

void PlayerActionStarRodJump(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 54;
    gCurTask->variant = 0;
    gCurTask->player->unk14 = 23;
    PlayerSetMotionYPreset(0);
    PlaySfxIfLocalPlayer(100, (u16)gCurTask->player->playerIndex);
    {
        struct Task *t = gCurTask;
        t->unk2C = t->facing;
        t->u80.attackAbility = 25;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = -1;
        gPlayerBodyBoxes[t->player->playerIndex] = *(struct M11R20 *)gUnk_0873C304;
    }
    while (1)
    {
        {
            struct Task *t = gCurTask;
            t->unk28++;
            PlaySfxIfLocalPlayer(0x11D, (u16)t->player->playerIndex);
        }
        TaskSetFrame(0xFE5);
        TaskYieldTrampoline(1);
        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 6; gCurTask->unk6E++)
        {
            struct Task *t = gCurTask;
            t->unk28++;
            t->frame++;
            TaskYieldTrampoline(1);
        }
        gCurTask->unk28 = -1;
    }
}

void PlayerActionStarRodJumpUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant)
    {
    case 0:
        if (--t->player->unk14 == 0 || !(gLatchedHeldKeys[t->player->playerIndex] & 1))
        {
            t->variant = 1;
            PlayerSetMotionYPreset(1);
            gCurTask->player->unk14 = 5;
        }
        break;
    case 1:
        if (--t->player->unk14 == 0)
        {
            PlayerStopAxes(2);
            PlayerSetMotionYPreset(2);
            gCurTask->variant = 2;
        }
        break;
    case 2:
        break;
    }
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x30)
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x10)
            gCurTask->unk2C = 1;
        else
            gCurTask->unk2C = -1;
    }
    if (PlayerCheckFloat() == 0)
    {
        if (PlayerCheckBButton() == 0)
        {
            if (gCurTask->onGround & 1)
            {
                PlayerCheckBump();
                PlayerRequestLocomotion();
                gCurTask->facing = gCurTask->unk2C;
            }
        }
        else
        {
            gCurTask->facing = gCurTask->unk2C;
        }
    }
    {
        struct Task *u = gCurTask;
        if (u->unk28 != -1)
        {
            LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C318 + u->unk28 * 8);
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                         &gPlayerBodyBoxes[gCurTask->player->playerIndex]);
        }
    }
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x30)
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x10)
            PlayerSetMotionXPreset(10, 70);
        else
            PlayerSetMotionXPreset(10, 71);
    }
    else
    {
        PlayerSetMotionXPreset(11, 2);
    }
    if (!(gCurTask->onGround & 1))
        PlayerSetMotionYPreset(2);
    else
        PlayerLand(1);
    PlayerStopAtCeilingAndWall();
}

void PlayerActionStarRodFlight(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 55;
    if (gCurTask->player->prevMode != 13)
    {
        gCurTask->player->running = 0;
        gCurTask->player->unk14 = 0;
        gCurTask->player->unk10 = 0;
        PlayerStopAxes(3);
        gCurTask->variant = 0;
        gCurTask->u80.attackAbility = 0;
        gCurTask->lateUpdateCallback = 0;
        gCurTask->player->bodyBox = 0;
        gCurTask->player->terrainBox = 0;
        gCurTask->player->hitBoxSet = 0;
    }
    CallTableEntry(gCurTask->variant, 4, gUnk_0873B6AC);
}

void sub_0804fee8(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    CallTableEntry(gCurTask->variant, 4, gUnk_0873B6AC);
}

void PlayerActionStarRodFlightUpdate(void)
{
    CallTableEntry(gCurTask->variant, 4, gUnk_0873B6BC);
    sub_080506dc();
    {
        struct Task *t = gCurTask;
        if (t->pixelX < 16)
        {
            t->pixelX = 16;
            t->posX = 16 << 16;
        }
        else if (t->pixelX > 224)
        {
            t->pixelX = 224;
            t->posX = 224 << 16;
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->pixelY < 18)
        {
            t->pixelY = 18;
            t->posY = 18 << 16;
        }
        else if (t->pixelY > 132)
        {
            t->pixelY = 132;
            t->posY = 132 << 16;
        }
    }
    if (gSpriteCameraY > 888)
    {
        gCurTask->hitKind = 1;
        AddPlayerHealth(-gPlayerHealth[gCurTask->player->playerIndex], gCurTask->player->playerIndex);
        gCurTask->player->requestedAction = 17;
    }
}
