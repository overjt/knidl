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
 *       player's M37Player record, picks the computer players' speed and
 *       jitter (M37Player.unk28/unk24) from the level M37Game.level when at
 *       most one player is linked (M37Game.playerCount), runs until the player
 *       passes the finish line gAirGrindCoursePtr->unk010 (+240), counting frames
 *       in M37Game.raceTimes[player].
 *   AirGrindCpuRollTarget / AirGrindCpuHoldsA   the computer players' input: a target
 *       M37Player.unk30 re-rolled from the LCG around the course record's
 *       unk28, and the resulting "hold A" decision.
 *   AirGrindRacerUpdate   variant 0's per-frame callback: reads the player's keys
 *       (gPlayerHeldKeys/gPlayerPressedKeys for a linked player, AirGrindCpuHoldsA for a
 *       computer one) into M37Player.unk02/unk04, counts A presses in unk0E
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
    gCurTask->frameTable = (u32 *)gUnk_08755F54;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->tileWord = gUnk_080CFE2C[gAirGrindPtr->localPlayer][player] << 12;
    gCurTask->updateCallback = (u32)AirGrindRacerUpdate;
    gCurTask->lateUpdateCallback = (u32)AirGrindRacerIdleUpdate;
    gCurTask->state = 0;
    gCurTask->unk28 = 256;
    gAirGrindPtr->players[player].unk14 = 0x80000;
    gAirGrindPtr->players[player].unk18 = 0;
    gAirGrindPtr->players[player].unk06 = 0;
    gAirGrindPtr->players[player].unk08 = 0;
    gAirGrindPtr->players[player].unk0C = 0;
    gAirGrindPtr->players[player].unk0A = 0;
    gAirGrindPtr->players[player].unk00 = 0;
    gAirGrindPtr->players[player].unk10 = -9999;
    gAirGrindPtr->players[player].unk1C = 0;
    gAirGrindPtr->players[player].unk04 = 0;
    gAirGrindPtr->players[player].unk02 = 0;
    gAirGrindPtr->players[player].unk0E = 0;
    gAirGrindPtr->players[player].unk01 = 0;
    gCurTask->posX = gAirGrindCoursePtr->scrollPos << 16;
    gCurTask->velX = 0x28000;
    gCurTask->accelX = 0;
    gAirGrindPtr->raceTimes[player] = 0;
    gAirGrindPtr->players[player].unk20 = 0;
    if (gAirGrindPtr->playerCount <= 1) {
        gAirGrindPtr->players[player].unk20 = 2;
        if (gAirGrindPtr->level == 0) {
            if (player == 1) {
                gAirGrindPtr->players[1].unk28 = 2;
                gAirGrindPtr->players[1].unk24 = 20;
            } else if (player == 2) {
                gAirGrindPtr->players[2].unk28 = 1;
                gAirGrindPtr->players[2].unk24 = 15;
            } else if (player == 3) {
                gAirGrindPtr->players[3].unk28 = 0;
                gAirGrindPtr->players[3].unk24 = 10;
            }
        } else if (gAirGrindPtr->level == 1) {
            if (player == 1) {
                gAirGrindPtr->players[1].unk28 = 2;
                gAirGrindPtr->players[1].unk24 = 5;
            } else if (player == 2) {
                gAirGrindPtr->players[2].unk28 = 5;
                gAirGrindPtr->players[2].unk24 = 15;
            } else if (player == 3) {
                gAirGrindPtr->players[3].unk28 = 10;
                gAirGrindPtr->players[3].unk24 = 20;
            }
        } else {
            if (player == 1) {
                gAirGrindPtr->players[1].unk28 = 1;
                gAirGrindPtr->players[1].unk24 = 2;
            } else if (player == 2) {
                gAirGrindPtr->players[2].unk28 = 2;
                gAirGrindPtr->players[2].unk24 = 4;
            } else if (player == 3) {
                gAirGrindPtr->players[3].unk28 = 3;
                gAirGrindPtr->players[3].unk24 = 6;
            }
        }
    } else {
        src = gUnk_080CFE2C[gAirGrindPtr->localPlayer][player];
        if (src >= gAirGrindPtr->playerCount) {
            gAirGrindPtr->players[player].unk20 = src;
            if (src == 2) {
                gAirGrindPtr->players[player].unk28 = 5;
                gAirGrindPtr->players[player].unk24 = 15;
            } else if (src == 3) {
                gAirGrindPtr->players[player].unk28 = 10;
                gAirGrindPtr->players[player].unk24 = 5;
            } else {
                while (1)
                    ;
            }
        }
    }
    if (gAirGrindPtr->players[player].unk20 != 0)
        AirGrindCpuRollTarget(player);
    gAirGrindPtr->players[player].unk21 = 1;
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->unk00C)
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
    s32 speed = gAirGrindPtr->players[player].unk28;
    s32 range = gAirGrindPtr->players[player].unk24;

    if (gAirGrindCoursePtr->players[player].unk14 != 0)
        speed = -speed;
    gAirGrindPtr->players[player].unk30 = gAirGrindPtr->players[player].unk2C = gAirGrindCoursePtr->players[player].unk28;
    gAirGrindPtr->players[player].unk30 += speed + AirGrindRandomRange(player, range) - range / 2;
}

s32 AirGrindCpuHoldsA(s32 player, s32 pos)
{
    s32 flag = 0;
    s32 d;

    if (gAirGrindCoursePtr->players[player].unk28 > gAirGrindPtr->players[player].unk2C)
        AirGrindCpuRollTarget(player);
    d = gAirGrindPtr->players[player].unk30 - pos;
    if (gAirGrindPtr->players[player].unk20 > 1) {
        if (gAirGrindPtr->players[player].unk21) {
            if (d > 0)
                flag = 1;
        } else if (d < 0) {
            flag = 1;
        }
        if (gAirGrindPtr->players[player].unk02 != flag)
            gAirGrindPtr->players[player].unk21 = !gAirGrindPtr->players[player].unk21;
        if (d > 100 && gAirGrindCoursePtr->players[player].unk14) {
            flag = 1;
            gAirGrindPtr->players[player].unk21 = flag;
        }
    }
    if (gAirGrindPtr->players[player].unk20 == 1) {
        if (gAirGrindCoursePtr->players[player].unk14) {
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
    u16 prev = gAirGrindPtr->players[player].unk02;
    s32 src;

    if (gAirGrindPtr->players[player].unk08 != 0)
        gAirGrindPtr->players[player].unk08--;
    if (gAirGrindPtr->players[player].unk0A != 0)
        gAirGrindPtr->players[player].unk0A--;
    if (gAirGrindPtr->players[player].unk0C != 0)
        gAirGrindPtr->players[player].unk0C--;
    if (pos > gAirGrindCoursePtr->finishLine + 240)
        pos = gAirGrindCoursePtr->finishLine + 240;
    if (gCurTask->state == 1) {
        if (gAirGrindPtr->playerCount <= 1) {
            if (player == 0) {
                gAirGrindPtr->players[0].unk02 = gPlayerHeldKeys[0];
                gAirGrindPtr->players[0].unk04 = gPlayerPressedKeys[0];
            } else {
                u16 keys = AirGrindCpuHoldsA(player, pos);

                gAirGrindPtr->players[player].unk04 = keys & ~gAirGrindPtr->players[player].unk02;
                gAirGrindPtr->players[player].unk02 = keys;
            }
        } else {
            src = gUnk_080CFE2C[gAirGrindPtr->localPlayer][player];
            if (src < gAirGrindPtr->playerCount) {
                gAirGrindPtr->players[player].unk02 = gPlayerHeldKeys[src];
                gAirGrindPtr->players[player].unk04 = gPlayerPressedKeys[src];
            } else {
                u16 keys = AirGrindCpuHoldsA(player, pos);

                gAirGrindPtr->players[player].unk04 = keys & ~gAirGrindPtr->players[player].unk02;
                gAirGrindPtr->players[player].unk02 = keys;
            }
        }
        if ((prev ^ gAirGrindPtr->players[player].unk02) & 1)
            gAirGrindPtr->players[player].unk0E++;
    }
    gAirGrindCoursePtr->players[player].coursePos = pos;
    if ((gAirGrindPtr->players[player].unk02 & 1) && gAirGrindPtr->players[player].unk08 == 0)
        gAirGrindCoursePtr->players[player].unk04 = 1;
    else
        gAirGrindCoursePtr->players[player].unk04 = 0;
}
