#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_4ab70.c (0x0804AB70-0x0804B5B3, issue #88).
 *
 * Player action bodies, part 19: actions 50-51 and handlers 47-48.
 * PlayerActionTornado (action 50, mode 13) is a rolling move: sound 150, then
 * the velocity presets 63-65 of M11's PlayerSetMotionXPreset over three states
 * that fall into each other (animations 0xDC5/0xDDD/0xDDB-0xDDC by the
 * facing, effects 28 and 45 x3).  Its handler PlayerActionTornadoUpdate jumps on B
 * (preset 50) or falls (49) in state 1, turns the player round at a
 * wall (PlayerFaceHeldDirection or the collision block gTerrainResult: Task.velX
 * and unk5C negated, and the facing on a block hit, with an 8-frame
 * lock in Task.unk28), stops a rise on a ceiling hit, registers the
 * collider gUnk_0873C2A0 and requests action 23 through PlayerHasCrossedWaterSurface.
 * PlayerActionCrash (action 51, mode 13) is a screen-wide blast with the
 * stage frozen (gPauseDisabled = 1): it switches the DISPCNT shadow
 * gDispCnt to windowed BG1-BG3, remembers the height Task.posY in
 * Task.unk2C, shakes the screen (RequestScreenShake(5), SetRoomUpdateFlags(2)),
 * flashes the player's palette gPlayerPalettes[player] towards
 * gUnk_082030B8 twice (BlendColors, presets 51/52), waits for the
 * blast task through PlayerState.unk16 and TaskSetSkipMask, restores the
 * default script gPlayerDefaultTerrainBox and resets the HUD ability panel
 * (SetPlayerAbility(0, -1, player)).  Its handler PlayerActionCrashUpdate fades the
 * palette in and back out (gUnk_08203098, Task.unk28 in steps of 10 and
 * 16) and keeps the player under the height Task.unk2C. */

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);       /* landed (hud_099fc.c); M12 calls it as SetPlayerAbility(0, -1, p->unk00) */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionTornado(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_TORNADO;
    gCurTask->unk28 = 0;
    gCurTask->unk2C = 0;
    gCurTask->variant = 0;
    gCurTask->u80.attackAbility = ABILITY_TORNADO;
    switch (gCurTask->variant) {
    case 0:
        PlaySfxIfLocalPlayer(150, gCurTask->player->playerIndex);
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        PlayerSetMotionXPreset(11, 63);
        gCurTask->spriteFlags &= 0x7FFF;
        TaskSetFrame(0xDC5);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 10);
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        CreatePlayerEffect(gCurTask->player->playerIndex, 28, 0);
        gCurTask->player->unk16 = 0;
        PlayerSetMotionXPreset(11, 64);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame = 0xDDD;
            TaskYieldTrampoline(2);
            gCurTask->unk6E = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++gCurTask->unk6E <= 6);
        } while ((s16)++gCurTask->playerLoopCount <= 5);
        gCurTask->variant = 2;
        /* fallthrough */
    case 2:
        gCurTask->player->unk16 = 255;
        CreatePlayerEffect(gCurTask->player->playerIndex, 45, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 45, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 45, 2);
        PlayerSetMotionXPreset(11, 65);
        {
            struct Task *t = gCurTask;
            if (t->facing == 1) {
                t->frame = 0xDDC;
                TaskYieldTrampoline(2);
            } else {
                t->frame = 0xDDB;
                TaskYieldTrampoline(2);
            }
        }
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame -= 2;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 10);
        TaskSetFrame(0xDE5);
        TaskYieldTrampoline(1);
        gCurTask->variant = 3;
    }
    TaskSleepForever();
}

void PlayerActionTornadoUpdate(void)
{
    switch (gCurTask->variant) {
    case 1:
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 2) {
            gCurTask->onGround = 0;
            PlayerSetMotionYPreset(50);
        } else if ((gCurTask->onGround & 1) == 0) {
            PlayerSetMotionYPreset(49);
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C-- == 0) {
                PlaySfxIfLocalPlayer(149, u->player->playerIndex);
                gCurTask->unk2C = 3;
            }
        }
        goto common;
    case 0:
    case 2:
        if ((gCurTask->onGround & 1) == 0)
            PlayerSetMotionYPreset(49);
    common:
        {
            struct Task *u = gCurTask;
            if (u->unk28 == 0) {
                if (PlayerFaceHeldDirection() != 0) {
                    struct Task *v = gCurTask;
                    v->unk28 = 8;
                    v->velX = -v->velX;
                    v->accelX = -v->accelX;
                }
            } else {
                u->unk28--;
            }
        }
        if (gTerrainResult.unk0 != 0) {
            gCurTask->unk28 = 8;
            gCurTask->facing = -gCurTask->facing;
            {
                struct Task *v = gCurTask;
                v->velX = -v->velX;
                v->accelX = -v->accelX;
            }
        }
        {
            struct Task *w = gCurTask;
            if (w->velY < 0 && gTerrainResult.ceilingHits != 0)
                w->velY = 0;
        }
        if (gCurTask->variant != 2)
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                         gUnk_0873C2A0);
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    if (gCurTask->onGround & 1)
        PlayerCheckLanding();
    if (gCurTask->waterFlags != 0 && PlayerHasCrossedWaterSurface(0) != 0) {
        PlayerSetWaterMotionY();
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        gCurTask->player->unk16 = 255;
    }
}

void PlayerActionCrash(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_CRASH;
    gPauseDisabled = 1;
    gCurTask->u80.attackAbility = ABILITY_CRASH;
    PlayerStopAxes(3);
    FreezeOtherTasks(15);
    if ((gDispCnt & 0x400) == 0) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
    gCurTask->player->unk42 |= 0x700;
    gCurTask->playerCrashSavedPosY = gCurTask->posY;
    CreatePlayerEffect(gCurTask->player->playerIndex, 46, 0);
    gCurTask->player->terrainBox = 0;
    gCurTask->variant = 0;
    RequestScreenShake(5);
    SetRoomUpdateFlags(2);
    PlaySfx(248);
    gCurTask->playerLoopCount = 0;
    do {
        TaskSetFrame(0xDE7);
        TaskYieldTrampoline(4);
        TaskSetFrame(0xDFA);
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->playerLoopCount <= 1);
    gCurTask->playerLoopCount = 0;
    do {
        TaskSetFrame(0xDE7);
        TaskYieldTrampoline(2);
        TaskSetFrame(0xDFA);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->playerLoopCount <= 3);
    TaskSetFrame(0xDE8);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    PlayerSetMotionYPreset(51);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->playerCrashBlendRatio = 0;
    gCurTask->player->unk42 |= 16;
    gCurTask->playerLoopCount = 0;
    do {
        BlendColors(gPlayerPalettes[gCurTask->player->playerIndex], gUnk_082030B8,
                     (u16)gCurTask->playerCrashBlendRatio, 16,
                     (u16 *)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)));
        gCurTask->playerCrashBlendRatio += 85;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->playerLoopCount <= 2);
    TaskYieldTrampoline(3);
    gCurTask->player->unk42 &= 0xFFEF;
    TaskYieldTrampoline(1);
    PlayerStopAxes(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlayerSetMotionYPreset(52);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->playerCrashBlendRatio = 0;
    gCurTask->player->unk42 |= 16;
    gCurTask->playerLoopCount = 0;
    do {
        BlendColors(gPlayerPalettes[gCurTask->player->playerIndex], gUnk_082030B8,
                     (u16)gCurTask->playerCrashBlendRatio, 16,
                     (u16 *)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)));
        gCurTask->playerCrashBlendRatio += 85;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->playerLoopCount <= 2);
    gCurTask->player->unk42 &= 0xFFEF;
    TaskYieldTrampoline(9);
    PlayerStopAxes(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    CreatePlayerEffect(gCurTask->player->playerIndex, 46, 1);
    PlayerSetMotionYPreset(53);
    TaskSetFrame(0xDE7);
    TaskYieldTrampoline(3);
    gCurTask->player->unk42 |= 16;
    gCurTask->variant = 1;
    gCurTask->playerCrashBlendRatio = 0;
    gCurTask->player->unk16 = 1;
    {
        /* a second pseudo for the task-pointer address (lesson 3.291) */
        struct Task **c = &gCurTask;

        gCurTask->playerLoopCount = 0;
        do {
            TaskSetFrame(0xDE7);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xDEF);
            TaskYieldTrampoline(2);
            gCurTask->unk6E = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while (++gCurTask->unk6E <= 9);
        } while ((s16)++gCurTask->playerLoopCount <= 1);
        if ((s8)(*c)->player->unk16 == 0) {
            TaskSetFrame(0xDE7);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xDEF);
            TaskYieldTrampoline(2);
            (*c)->unk6E = 0;
            do {
                (*c)->frame++;
                TaskYieldTrampoline(2);
            } while (++(*c)->unk6E <= 9);
        } else {
            TaskSetSkipMask(2, gCurTaskIdx);
            do {
                if ((s8)(*c)->player->unk16 == 0) {
                    TaskSetSkipMask(0, gCurTaskIdx);
                    (*c)->player->unk16 = 2;
                }
                TaskSetFrame(0xDE7);
                TaskYieldTrampoline(2);
                TaskSetFrame(0xDEF);
                TaskYieldTrampoline(2);
                (*c)->unk6E = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                } while (++gCurTask->unk6E <= 9);
            } while ((s8)(*c)->player->unk16 != 2);
        }
    }
    TaskSetFrame(0xDE7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0xDFB);
    TaskYieldTrampoline(2);
    PlayerStopAxes(2);
    gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
    SetPlayerAbility(ABILITY_NORMAL, -1, gCurTask->player->playerIndex);
    gCurTask->variant = 2;
    gCurTask->frame++;
    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 31; gCurTask->playerLoopCount++) {
        if (gPauseDisabled == 0)
            gCurTask->player->unk42 &= 0xFBFF;
        TaskYieldTrampoline(1);
    }
    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    gCurTask->player->unk42 &= 0xFCFF;
    gCurTask->variant = 3;
    TaskSleepForever();
}

void PlayerActionCrashUpdate(void)
{
    struct Task *t = gCurTask;

    switch (t->variant) {
    case 1:
        BlendColors(gPlayerPalettes[t->player->playerIndex], gUnk_08203098, (u16)t->playerCrashBlendRatio, 16,
                     (u16 *)(gObjPalette + ((t->tileWord >> 12) << 5)));
        {
            struct Task *u = gCurTask;
            if (u->playerCrashBlendRatio == 256) {
                u->variant = 0;
            } else {
                u->playerCrashBlendRatio += 10;
                if (u->playerCrashBlendRatio > 255)
                    u->playerCrashBlendRatio = 256;
            }
        }
        break;
    case 2:
        BlendColors(gPlayerPalettes[t->player->playerIndex], gUnk_08203098, (u16)t->playerCrashBlendRatio, 16,
                     (u16 *)(gObjPalette + ((t->tileWord >> 12) << 5)));
        {
            struct Task *u = gCurTask;
            if (u->playerCrashBlendRatio == 0) {
                u->variant = 0;
                RequestScreenShake(0);
                gCurTask->player->unk42 &= 0xFFEF;
            } else {
                u->playerCrashBlendRatio -= 16;
                if (u->playerCrashBlendRatio <= 0)
                    u->playerCrashBlendRatio = 0;
            }
        }
        break;
    case 3:
        if (t->onGround & 1)
            t->player->requestedAction = PLAYER_ACTION_STAND;
        else
            t->player->requestedAction = PLAYER_ACTION_FALL;
        break;
    }
    {
        struct Task *v = gCurTask;
        if (v->velY > 0 && v->posY > v->playerCrashSavedPosY) {
            PlayerStopAxes(2);
            {
                struct Task *w = gCurTask;
                w->posY = w->playerCrashSavedPosY;
                w->pixelY = w->playerCrashSavedPosY >> 16;
            }
        }
    }
}
