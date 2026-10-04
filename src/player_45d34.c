#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_45d34.c (0x08045D34-0x0804632F, issue #87).
 *
 * Player action bodies, part 14: action 38 and per-frame handler 35.
 * PlayerActionMike (action 38, mode 13) spends one charge of the ability
 * counter PlayerState.abilityUses (the HUD update SetPlayerAbilityNoHud) and plays one
 * of three sequences picked by the charges left (Task.variant = unk0E - 1),
 * each with its own song (StopSfx) and sound; meanwhile it freezes
 * the stage (gPauseDisabled = 1), switches the DISPCNT shadow
 * gDispCnt to BG0, BG2, BG3 and OBJ unless BG2 is already on, raises
 * PlayerState.unk42 bits 8-10 and repeats the last loop until the effect
 * counter PlayerState.unk16 runs out.  With the last charge spent it
 * drops the ability (HudShowAbility) unless the ability is 7.  Its handler
 * PlayerActionMikeUpdate requests action 1 or 7 (on the ground or in the air) once
 * it has finished. */

s32 PlaySfx(s32 id);
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionMike(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 35;
    gCurTask->variant = gCurTask->player->abilityUses - 1;
    gCurTask->player->unk42 &= 0xFFEF;
    if (--gCurTask->player->abilityUses == 0) {
        SetPlayerAbilityNoHud(0, -1, gCurTask->player->playerIndex);
    } else {
        struct PlayerState *p = gCurTask->player;
        SetPlayerAbilityNoHud(p->ability, p->abilityUses, p->playerIndex);
    }
    gPauseDisabled = 1;
    PlayerStopAxes(3);
    FreezeOtherTasks(15);
    if (!(gDispCnt & 0x400)) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    gCurTask->player->unk16 = 0;
    CreatePlayerEffect(gCurTask->player->playerIndex, 37, 0);
    gCurTask->unk28 = 0;
    SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
    gCurTask->player->unk42 |= 0x700;
    switch (gCurTask->variant) {
    case 2:
        PlayerSetMotionXPreset(11, 42);
        TaskSetFrame(0x66B);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        PlayerSetMotionXPreset(11, 43);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStopAxes(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x670);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlaySfx(160);
        gCurTask->player->unk16++;
        RequestScreenShake(4);
        SetRoomUpdateFlags(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 37, 1);
        gCurTask->playerLoopCount = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x674);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 7);
        while ((s8)gCurTask->player->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x674);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x670);
        TaskYieldTrampoline(4);
        break;
    case 1:
        TaskSetFrame(0x676);
        TaskYieldTrampoline(4);
        gCurTask->playerLoopCount = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(4);
        } while ((s16)++gCurTask->playerLoopCount <= 5);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        StopSfx(160);
        TaskYieldTrampoline(1);
        PlaySfx(161);
        gCurTask->player->unk16++;
        RequestScreenShake(4);
        SetRoomUpdateFlags(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 37, 1);
        gCurTask->playerLoopCount = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x67D);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 7);
        while ((s8)gCurTask->player->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x67D);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x679);
        TaskYieldTrampoline(4);
        break;
    case 0:
        TaskSetFrame(0x67F);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(5);
        gCurTask->player->terrainBox = 0;
        PlayerSetMotionYPreset(35);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        TaskSetFrame(0x684);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        StopSfx(161);
        TaskYieldTrampoline(1);
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        PlayerStopAxes(2);
        PlaySfx(162);
        gCurTask->player->unk16++;
        RequestScreenShake(4);
        SetRoomUpdateFlags(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 37, 1);
        gCurTask->playerLoopCount = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x689);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->playerLoopCount <= 7);
        while ((s8)gCurTask->player->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x689);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x683);
        TaskYieldTrampoline(4);
        break;
    }
    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    FreezeOtherTasks(0);
    {
        struct PlayerState *p = gCurTask->player;
        if (p->ability != 7)
            HudShowAbility(p->ability, p->playerIndex);
        else
            p->unk22 = 2;
    }
    gCurTask->unk28++;
    gPauseDisabled = 0;
    gCurTask->player->unk42 &= 0xF8FF;
    TaskSleepForever();
}

void PlayerActionMikeUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        if (t->onGround & 1)
            t->player->requestedAction = 1;
        else
            t->player->requestedAction = 7;
    }
    PlayerStopAtCeilingAndWall();
}
