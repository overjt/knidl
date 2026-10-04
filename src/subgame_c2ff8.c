#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "subgame.h"

/* subgame_c2ff8.c (0x080C2FF8-0x080C3647, issue #98).
 *
 * Sub-game 2: task type #96 (class 3) and its variant 0, the racers.
 * 
 *   Task_AirGrindObject   the body: CallTableEntry(Task.variant, 5, gAirGrindObjectVariants), the
 *       three variants AirGrindRacer / AirGrindScenery / AirGrindEffect (entries
 *       2-4 of gAirGrindPhases; the two words after them are data).
 *   AirGrindRacer   variant 0, one per player (Task.unk1C): resets the
 *       player's AirGrindRacerState record, picks the computer players' speed and
 *       jitter (AirGrindRacerState.cpuTargetLead/unk24) from the level AirGrindState.level when at
 *       most one player is linked (AirGrindState.playerCount), runs until the player
 *       passes the finish line gAirGrindCoursePtr->unk010 (+240), counting frames
 *       in AirGrindState.raceTimes[player].
 *   AirGrindCpuRollTarget / AirGrindCpuHoldsA   the computer players' input: a target
 *       AirGrindRacerState.cpuTarget re-rolled from the LCG around the course record's
 *       unk28, and the resulting "hold A" decision.
 *   AirGrindRacerUpdate   variant 0's per-frame callback: reads the player's keys
 *       (gPlayerHeldKeys/gPlayerPressedKeys for a linked player, AirGrindCpuHoldsA for a
 *       computer one) into AirGrindRacerState.heldKeys/unk04, counts A presses in unk0E
 *       and publishes the position and the pressed flag in the course
 *       record gAirGrindCoursePtr->unk018[player]. */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void TaskSleepForever(void);                                     /* end the running task */

void Task_AirGrindObject(void)
{
    CallTableEntry(gCurTask->variant, 5, gAirGrindObjectVariants);
}

void AirGrindRacer(void)
{
    s32 player = gCurTask->unk1C;
    s32 src;

    gCurTask->drawCallback = (u32)AirGrindRacerDraw;
    gCurTask->moveCallback = (u32)AirGrindRacerMove;
    gCurTask->frameTable = (u32 *)gAirGrindRacerFrames;
    gCurTask->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
    gCurTask->tileWord = gAirGrindLocalPlayerSlots[gAirGrindPtr->localPlayer][player] << 12;
    gCurTask->updateCallback = (u32)AirGrindRacerUpdate;
    gCurTask->lateUpdateCallback = (u32)AirGrindRacerIdleUpdate;
    gCurTask->state = 0;
    gCurTask->unk28 = 256;
    gAirGrindPtr->players[player].liftY = 0x80000;
    gAirGrindPtr->players[player].offsetX = 0;
    gAirGrindPtr->players[player].animStep = 0;
    gAirGrindPtr->players[player].penaltyTimer = 0;
    gAirGrindPtr->players[player].dashTimer = 0;
    gAirGrindPtr->players[player].boostCooldown = 0;
    gAirGrindPtr->players[player].grinding = 0;
    gAirGrindPtr->players[player].segmentEndDistance = -9999;
    gAirGrindPtr->players[player].segmentEnd = 0;
    gAirGrindPtr->players[player].pressedKeys = 0;
    gAirGrindPtr->players[player].heldKeys = 0;
    gAirGrindPtr->players[player].aToggleCount = 0;
    gAirGrindPtr->players[player].fullBoostCount = 0;
    gCurTask->posX = gAirGrindCoursePtr->scrollPos << 16;
    gCurTask->velX = 0x28000;
    gCurTask->accelX = 0;
    gAirGrindPtr->raceTimes[player] = 0;
    gAirGrindPtr->players[player].cpuMode = 0;
    if (gAirGrindPtr->playerCount <= 1) {
        gAirGrindPtr->players[player].cpuMode = 2;
        if (gAirGrindPtr->level == 0) {
            if (player == 1) {
                gAirGrindPtr->players[1].cpuTargetLead = 2;
                gAirGrindPtr->players[1].cpuTargetSpread = 20;
            } else if (player == 2) {
                gAirGrindPtr->players[2].cpuTargetLead = 1;
                gAirGrindPtr->players[2].cpuTargetSpread = 15;
            } else if (player == 3) {
                gAirGrindPtr->players[3].cpuTargetLead = 0;
                gAirGrindPtr->players[3].cpuTargetSpread = 10;
            }
        } else if (gAirGrindPtr->level == 1) {
            if (player == 1) {
                gAirGrindPtr->players[1].cpuTargetLead = 2;
                gAirGrindPtr->players[1].cpuTargetSpread = 5;
            } else if (player == 2) {
                gAirGrindPtr->players[2].cpuTargetLead = 5;
                gAirGrindPtr->players[2].cpuTargetSpread = 15;
            } else if (player == 3) {
                gAirGrindPtr->players[3].cpuTargetLead = 10;
                gAirGrindPtr->players[3].cpuTargetSpread = 20;
            }
        } else {
            if (player == 1) {
                gAirGrindPtr->players[1].cpuTargetLead = 1;
                gAirGrindPtr->players[1].cpuTargetSpread = 2;
            } else if (player == 2) {
                gAirGrindPtr->players[2].cpuTargetLead = 2;
                gAirGrindPtr->players[2].cpuTargetSpread = 4;
            } else if (player == 3) {
                gAirGrindPtr->players[3].cpuTargetLead = 3;
                gAirGrindPtr->players[3].cpuTargetSpread = 6;
            }
        }
    } else {
        src = gAirGrindLocalPlayerSlots[gAirGrindPtr->localPlayer][player];
        if (src >= gAirGrindPtr->playerCount) {
            gAirGrindPtr->players[player].cpuMode = src;
            if (src == 2) {
                gAirGrindPtr->players[player].cpuTargetLead = 5;
                gAirGrindPtr->players[player].cpuTargetSpread = 15;
            } else if (src == 3) {
                gAirGrindPtr->players[player].cpuTargetLead = 10;
                gAirGrindPtr->players[player].cpuTargetSpread = 5;
            } else {
                while (1)
                    ;
            }
        }
    }
    if (gAirGrindPtr->players[player].cpuMode != 0)
        AirGrindCpuRollTarget(player);
    gAirGrindPtr->players[player].unk21 = 1;
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->startLine)
        TaskYieldTrampoline(1);
    gCurTask->state = 1;
    gCurTask->lateUpdateCallback = (u32)AirGrindRacerRaceUpdate;
    while (gAirGrindCoursePtr->players[player].coursePos < gAirGrindCoursePtr->finishLine) {
        gAirGrindPtr->raceTimes[player]++;
        TaskYieldTrampoline(1);
    }
    gCurTask->state = 2;
    gCurTask->lateUpdateCallback = (u32)AirGrindRacerIdleUpdate;
    gCurTask->accelX = 0;
    while (gAirGrindCoursePtr->players[player].coursePos < gAirGrindCoursePtr->finishLine + 240)
        TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    TaskSleepForever();
}

void AirGrindCpuRollTarget(s32 player)
{
    s32 speed = gAirGrindPtr->players[player].cpuTargetLead;
    s32 range = gAirGrindPtr->players[player].cpuTargetSpread;

    if (gAirGrindCoursePtr->players[player].onEvenSegment != 0)
        speed = -speed;
    gAirGrindPtr->players[player].cpuTarget = gAirGrindPtr->players[player].cpuRollSegmentEnd = gAirGrindCoursePtr->players[player].segmentEnd;
    gAirGrindPtr->players[player].cpuTarget += speed + AirGrindRandomRange(player, range) - range / 2;
}

s32 AirGrindCpuHoldsA(s32 player, s32 pos)
{
    s32 flag = 0;
    s32 d;

    if (gAirGrindCoursePtr->players[player].segmentEnd > gAirGrindPtr->players[player].cpuRollSegmentEnd)
        AirGrindCpuRollTarget(player);
    d = gAirGrindPtr->players[player].cpuTarget - pos;
    if (gAirGrindPtr->players[player].cpuMode > 1) {
        if (gAirGrindPtr->players[player].unk21) {
            if (d > 0)
                flag = 1;
        } else if (d < 0) {
            flag = 1;
        }
        if (gAirGrindPtr->players[player].heldKeys != flag)
            gAirGrindPtr->players[player].unk21 = !gAirGrindPtr->players[player].unk21;
        if (d > 100 && gAirGrindCoursePtr->players[player].onEvenSegment) {
            flag = 1;
            gAirGrindPtr->players[player].unk21 = flag;
        }
    }
    if (gAirGrindPtr->players[player].cpuMode == 1) {
        if (gAirGrindCoursePtr->players[player].onEvenSegment) {
            if (d > 0)
                flag = 1;
        } else if (d < 0) {
            flag = 1;
        }
    }
    return flag;
}

void AirGrindRacerUpdate(void)
{
    s32 player = gCurTask->unk1C;
    s32 pos = gCurTask->posX >> 16;
    u16 prev = gAirGrindPtr->players[player].heldKeys;
    s32 src;

    if (gAirGrindPtr->players[player].penaltyTimer != 0)
        gAirGrindPtr->players[player].penaltyTimer--;
    if (gAirGrindPtr->players[player].boostCooldown != 0)
        gAirGrindPtr->players[player].boostCooldown--;
    if (gAirGrindPtr->players[player].dashTimer != 0)
        gAirGrindPtr->players[player].dashTimer--;
    if (pos > gAirGrindCoursePtr->finishLine + 240)
        pos = gAirGrindCoursePtr->finishLine + 240;
    if (gCurTask->state == 1) {
        if (gAirGrindPtr->playerCount <= 1) {
            if (player == 0) {
                gAirGrindPtr->players[0].heldKeys = gPlayerHeldKeys[0];
                gAirGrindPtr->players[0].pressedKeys = gPlayerPressedKeys[0];
            } else {
                u16 keys = AirGrindCpuHoldsA(player, pos);

                gAirGrindPtr->players[player].pressedKeys = keys & ~gAirGrindPtr->players[player].heldKeys;
                gAirGrindPtr->players[player].heldKeys = keys;
            }
        } else {
            src = gAirGrindLocalPlayerSlots[gAirGrindPtr->localPlayer][player];
            if (src < gAirGrindPtr->playerCount) {
                gAirGrindPtr->players[player].heldKeys = gPlayerHeldKeys[src];
                gAirGrindPtr->players[player].pressedKeys = gPlayerPressedKeys[src];
            } else {
                u16 keys = AirGrindCpuHoldsA(player, pos);

                gAirGrindPtr->players[player].pressedKeys = keys & ~gAirGrindPtr->players[player].heldKeys;
                gAirGrindPtr->players[player].heldKeys = keys;
            }
        }
        if ((prev ^ gAirGrindPtr->players[player].heldKeys) & 1)
            gAirGrindPtr->players[player].aToggleCount++;
    }
    gAirGrindCoursePtr->players[player].coursePos = pos;
    if ((gAirGrindPtr->players[player].heldKeys & 1) && gAirGrindPtr->players[player].penaltyTimer == 0)
        gAirGrindCoursePtr->players[player].holdingA = 1;
    else
        gAirGrindCoursePtr->players[player].holdingA = 0;
}
