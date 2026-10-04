#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "player.h"
#include "effect.h"

/* player_455c8.c (0x080455C8-0x08045D33, issue #87).
 *
 * Player action bodies, part 13: actions 36-37 and per-frame handlers
 * 33-34.  PlayerActionBurning (action 36, mode 13) is a four-state machine over
 * Task.variant: state 0 starts the move (velocity preset 34, effect 32,
 * animation 0x5CB) and installs the attack box gUnk_0873CCA4 in
 * PlayerState.hitBoxSet, state 1 swaps in the scripts gUnk_0873CB2C /
 * gUnk_0873BD3C (PlayerState.terrainBox/unk64) and cycles the hit-box row
 * Task.unk2C through 0-2, state 2 restores the default scripts
 * gPlayerDefaultTerrainBox / gPlayerDefaultBodyBox and lands (preset 2), and state 3 is the
 * bounce-off (sound 153, RequestScreenShake(4), preset 23).  Its handler
 * PlayerActionBurningUpdate drives it from the collision block gTerrainResult -
 * re-binding state 3 on a hit, fading the palette of gUnk_0873B510[]
 * row Task.unk2C and registering the box gUnk_0873BF00.
 * PlayerActionLaser (action 37) is a linear script (animations
 * 0x658/0x65A, M14's CreatePlayerObject, sound 172); its handler PlayerActionLaserUpdate
 * waits for it to finish. */

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSetEntry(void *a, u32 i);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionBurning(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_BURNING;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            t->variant = 0;
            gCurTask->u80.attackAbility = ABILITY_BURNING;
            gCurTask->playerBurningHitWall = 0;
        }
    }
    switch (gCurTask->variant) {
    case 0:
        gCurTask->onGround = 0;
        PlayerSetMotionXPreset(11, 38);
        PlayerSetMotionYPreset(34);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 0);
        TaskSetFrame(0x5CB);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(2);
        TaskYieldTrampoline(3);
        gCurTask->player->hitBoxSet = gUnk_0873CCA4;
        PlayerSetMotionXPreset(11, 39);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionXPreset(11, 40);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        PlayerStartSfx(SE_BURNING_ATTACK, gCurTask->player->playerIndex);
        gCurTask->onGround = 0;
        gCurTask->player->terrainBox = (u32)gUnk_0873CB2C;
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        gCurTask->player->bodyBox = (u32)gUnk_0873BD3C;
        gCurTask->player->statusFlags |= PLAYER_STATUS_PALETTE_LOCKED;
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 0;
        TaskSetFrame(0x5CF);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 3);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 4);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 1;
        TaskSetFrame(0x5D3);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 1;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        gCurTask->playerBurningFadeStep = 1;
        TaskSetFrame(0x5DF);
        TaskYieldTrampoline(2);
        gCurTask->playerBurningFadeStep = -1;
        gCurTask->player->statusFlags &= 0xFFEF;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStopSfx();
        gCurTask->variant = 2;
        /* fallthrough */
    case 2:
        gCurTask->player->hitBoxSet = 0;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        PlayerSetMotionXPreset(11, 41);
        PlayerSetMotionYPreset(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 5);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 6);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 7);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 8);
        TaskSetFrame(0x53C);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x544);
        gCurTask->variant = 4;
        break;
    case 3:
        gCurTask->player->hitBoxSet = 0;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
        gCurTask->player->statusFlags &= 0xFFEF;
        PlayerStopSfx();
        gCurTask->onGround = 0;
        PlaySfxIfLocalPlayer(153, gCurTask->player->playerIndex);
        RequestScreenShake(4);
        PlayerSetMotionXPreset(11, 17);
        PlayerSetMotionYPreset(23);
        TaskSetFrame(0x544);
        break;
    }
    TaskSleepForever();
}

void PlayerActionBurningUpdate(void)
{
    switch (gCurTask->variant) {
    case 0:
        gCurTask->onGround = 0;
        {
            struct Task *t = gCurTask;
            if (t->velY < 0 && gTerrainResult.ceilingHits != 0)
                t->velY = 0;
        }
        if (gTerrainResult.unk0 == 0)
            break;
        PlayerStopAxes(1);
        {
            s8 d = gCurTask->facing;
            if ((d == 1 && gTerrainResult.unk0 == 1)
                || (d == -1 && gTerrainResult.unk0 == 2))
                gCurTask->playerBurningHitWall = 1;
        }
        break;
    case 1:
        {
            struct Task *t = gCurTask;
            if (t->onGround & 1) {
                if (gTerrainResult.slope != 0) {
                    t->player->requestedAction = PLAYER_ACTION_WALK;
                    PlayerSetMotionXPreset(11, 41);
                    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                }
            } else if (*(u16 *)&gTerrainResult != 0 || t->playerBurningHitWall != 0
                       || (t->player->boundsClamp & 11) != 0) {
                gCurTask->variant = 3;
                TaskSetEntry(PlayerActionBurning, gCurTaskIdx);
            }
        }
        {
            struct Task *t = gCurTask;
            if (t->playerBurningFadeStep != -1) {
                struct BurningPaletteFade *f = &gUnk_0873B510[t->playerBurningFadeStep];
                t->playerBurningFadeRatio += f->rate;
                if (t->playerBurningFadeRatio > 255)
                    t->playerBurningFadeRatio = 256;
                BlendColors(f->src, f->dst, (u16)gCurTask->playerBurningFadeRatio, 16,
                             (u16 *)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)));
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                     gUnk_0873BF00);
        break;
    case 2:
        sub_0803e55c();
        break;
    case 3:
    case 4:
        PlayerRequestLocomotion();
        break;
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    {
        struct PlayerState *p = gCurTask->player;
        if (p->requestedAction != PLAYER_ACTION_NONE)
            p->statusFlags &= 0xFFEF;
    }
}

void PlayerActionLaser(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_LASER;
    gCurTask->playerActionDone28 = 0;
    gCurTask->u80.attackAbility = ABILITY_NORMAL;
    TaskSetFrame(0x658);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x65A);
    TaskYieldTrampoline(2);
    CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_LASER_BEAM, 0);
    PlaySfxIfLocalPlayer(SE_LASER_ATTACK, gCurTask->player->playerIndex);
    gCurTask->playerLoopCount = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->playerLoopCount <= 15);
    TaskSetFrame(0x658);
    TaskYieldTrampoline(2);
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionLaserUpdate(void)
{
    if (gCurTask->playerActionDone28 != 0)
        PlayerRequestLocomotion();
    sub_0803e55c();
}
