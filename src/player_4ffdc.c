#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* player_4ffdc.c (0x0804FFDC-0x080507BB, issue #90).
 *
 * Action 58's sub-actions 18-21 (gPlayerStarRodFlightVariants) and sub-handlers 22-25
 * (gPlayerStarRodFlightVariantUpdates), interleaved in the ROM as 18, 22, 19, 23, 20, 24, 21,
 * 25, and their two helpers.  PlayerStarRodFlightIntro is a long yield script driven by
 * the 8.8 velocity pairs gUnk_0873B6CC; PlayerStarRodFlightShoot spawns task type #6's
 * variant 12 through CreatePlayerObjectLowSlot; PlayerStarRodFlightHurt switches the player to mode
 * 17 with camera presets PlayerSetMotionXPreset(10, 24-27).  PlayerStarRodFlightCheckShoot re-binds
 * sub-action 2 when a direction is pressed or held for ten frames, and
 * PlayerStarRodFlightSteer steers the player with the held direction (velocity pairs
 * gUnk_0873B724[Task.unk6E], PlayerState.flightCoastTimer counting the glide
 * frames). */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
s32 CreatePlayerObjectLowSlot(s8 player, u8 variant, s32 arg);

void PlayerStarRodFlightIntro(void)
{
    gPauseDisabled = 1;
    {
        struct Task *t = gCurTask;
        t->updateCallback = (u32)PlayerStarRodFlightIntroUpdate;
        t->playerActionDone = 0;
        t->frame = 0xFFFF;
        if (gGameState != GAME_STATE_BOSS_ENDURANCE)
        {
            gPlayerHealth[t->player->playerIndex] = 0;
            AddPlayerHealth(gMaxHealth, t->player->playerIndex);
        }
    }
    {
        struct Task *t = gCurTask;
        t->posX = 0x80000;
        t->posY = 0xB00000;
    }
    TaskYieldTrampoline(65);
    {
        struct Task *t = gCurTask;
        t->frame = 0x1056;
        t->speedLimitX = 0x80000000;
        t->speedLimitY = 0x80000000;
        t->velX = 0x10000;
        t->velY = 0xFFFE0000;
    }
    TaskYieldTrampoline(58);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(20);
    {
        struct Task *t = gCurTask;
        t->velY = 0xFFFF8000;
        t->frame--;
    }
    TaskYieldTrampoline(8);
    gCurTask->frame--;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;
        t->velX = 0x8000;
        t->velY = 0x8000;
        t->frame = 0x1051;
    }
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0xFFFFE000;
    TaskYieldTrampoline(6);
    gCurTask->frame = 0x1040;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1042;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1045;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0;
        t->velY = 0;
        t->frame = 0x104B;
    }
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104E;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->frame = 0x105D;
        t->velX = 0xFFFF0000;
    }
    TaskYieldTrampoline(16);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(16);
    if (gPlayerOrderShuffleCount == 0)
        ShufflePlayerOrder();
    {
        struct Task *t = gCurTask;
        u16 *e = gUnk_0873B6CC[t->playerEntryOrder];
        s32 v;

        t->velX = 0xFFFF8000;
        v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->velY = v;
        TaskYieldTrampoline(16);
        t = gCurTask;
        v = e[1] << 8;
        if (e[1] & 0x8000)
            v |= 0xFF000000;
        t->velY = v;
    }
    TaskYieldTrampoline(16);
    gCurTask->frame = 0x105E;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1041;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1044;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1045;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0xFFFFE000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1049;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104C;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104D;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104F;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0;
        t->frame = 0x103F;
        t->velY = 0x2000;
    }
    TaskYieldTrampoline(6);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(14);
    gCurTask->velY = 0;
    TaskYieldTrampoline(14);
    {
        struct Task *t = gCurTask;
        t->playerActionDone++;
        t->player->bodyBox = (u32)gPlayerDefaultBodyBox;
    }
    TaskSleepForever();
}

void PlayerStarRodFlightIntroUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        t->variant = 1;
        gCurTask->updateCallback = (u32)PlayerUpdate;
        gCurTask->lateUpdateCallback = (u32)PlayerLateUpdate;
        TaskSetEntry(PlayerActionStarRodFlightEnterVariant, gCurTaskIdx);
        gPauseDisabled = 0;
    }
}

void PlayerStarRodFlightFly(void)
{
    struct Task *t = gCurTask;

    t->playerStarRodFrameIndex = 0;
    t->unk28 = 0;
    TaskSleepForever();
}

void PlayerStarRodFlightFlyUpdate(void)
{
    if (PlayerStarRodFlightCheckShoot() == 0)
    {
        u8 k = gUnk_0873B6DC[(gLatchedHeldKeys[gCurTask->player->playerIndex] & 0xF0) >> 4];
        struct Task *t = gCurTask;
        u16 *row = gUnk_0873B6E8[t->unk28];

        if (t->unk28 == 0)
        {
            if (t->playerStarRodFrameIndex == 0)
                t->unk28 = k;
            else
                t->playerStarRodFrameIndex--;
        }
        else if (t->unk28 == k)
        {
            if (row[t->playerStarRodFrameIndex + 1] != 0xFFFF)
                t->playerStarRodFrameIndex++;
        }
        else
        {
            if (t->playerStarRodFrameIndex != 0)
                t->playerStarRodFrameIndex--;
            else
                t->unk28 = k;
        }
        gCurTask->frame = row[gCurTask->playerStarRodFrameIndex];
    }
}

void PlayerStarRodFlightShoot(void)
{
    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        PlaySfxIfLocalPlayer(155, (u16)t->player->playerIndex);
    }
    gCurTask->frame = 0x1040;
    TaskYieldTrampoline(4);
    gCurTask->frame = 0x1041;
    TaskYieldTrampoline(1);
    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
    CreatePlayerObjectLowSlot(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_STAR_ROD_FLIGHT_SHOT, 0);
    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 9; gCurTask->playerLoopCount++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
    gCurTask->playerActionDone++;
    TaskSleepForever();
}

void PlayerStarRodFlightShootUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        t->variant = 1;
        TaskSetEntry(PlayerActionStarRodFlight, gCurTaskIdx);
    }
}

void PlayerStarRodFlightHurt(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 17;
    {
        struct Task *t = gCurTask;
        t->playerActionDone = 0;
        t->player->invulnerability = 1;
    }
    {
        struct Task *t = gCurTask;
        t->player->invulnerabilityTimer = 0x8000;
        if (gFrameCount & 1)
            PlaySfxIfLocalPlayer(111, (u16)t->player->playerIndex);
        else
            PlaySfxIfLocalPlayer(112, (u16)t->player->playerIndex);
    }
    PlayerStopAxes(3);
    if ((s8)gCurTask->hitDirection == 0)
        PlayerSetMotionXPreset(10, 24);
    else
        PlayerSetMotionXPreset(10, 26);
    gCurTask->frame = 0x105D;
    TaskYieldTrampoline(4);
    if ((s8)gCurTask->hitDirection == 0)
        PlayerSetMotionXPreset(10, 25);
    else
        PlayerSetMotionXPreset(10, 27);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlayerStopAxes(1);
    SetPlayerInvulnerability(1, 96, gCurTask->player->playerIndex);
    gCurTask->playerActionDone++;
    TaskSleepForever();
}

void PlayerStarRodFlightHurtUpdate(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->playerActionDone != 0)
    {
        t->variant = 1;
        TaskSetEntry(PlayerActionStarRodFlightEnterVariant, gCurTaskIdx);
    }
}

s32 PlayerStarRodFlightCheckShoot(void)
{
    if (!(gLatchedPressedKeys[gCurTask->player->playerIndex] & 3)
        && (!(gLatchedHeldKeys[gCurTask->player->playerIndex] & 3)
            || (s16)++gCurTask->player->playerStarRodHoldFrames != 10))
        return 0;
    gCurTask->player->playerStarRodHoldFrames = 0;
    gCurTask->variant = 2;
    TaskSetEntry(PlayerActionStarRodFlightEnterVariant, gCurTaskIdx);
    return 1;
}

void PlayerStarRodFlightSteer(void)
{
    struct Task *t = gCurTask;

    if (t->player->mode == 13)
    {
        u16 k = gLatchedHeldKeys[t->player->playerIndex] & 0xF0;

        if (k != 0)
        {
            u16 *e;
            s32 v;

            t->unk6E = k >> 4;
            e = gUnk_0873B724[t->unk6E];
            v = e[0] << 8;
            if (e[0] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
            t->speedLimitX = 0x20000;
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->velY = v;
            t->speedLimitY = 0x20000;
            t->player->flightCoastTimer = 8;
        }
        else if (t->player->flightCoastTimer != 0)
        {
            u16 *e;
            s32 v;

            e = gUnk_0873B724[t->unk6E];
            v = e[2] << 8;
            if (e[2] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
            v = e[3] << 8;
            if (e[3] & 0x8000)
                v |= 0xFF000000;
            t->velY = v;
            if (--t->player->flightCoastTimer == 0)
            {
                gCurTask->velX = 0;
                gCurTask->velY = 0;
            }
        }
    }
}
