#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

/* subgame_c3f44.c (0x080C3F44-0x080C462F, issue #98).
 *
 * Sub-game 2: task type #96 variant 2, the racers' effect sprites.
 * 
 *   sub_080c3f44   variant 2's body.  sub_080c2078(a, racer, kind) spawns it
 *       with Task.unk18 = the racer's task, Task.unk20 = the kind and
 *       Task.unk1C = a, and each kind sets its sprite table, animation and one
 *       of the callbacks below.  The kinds as AirGrindRacerRaceStep/sub_080c3698 use
 *       them (player 0 only unless noted): 0 and 1 every 4th / 8th frame while
 *       A is held on the course, 2 on a press (M37Game.unk450 blocks a second
 *       copy), 5 on the release (M37Game.unk451), 6 / 7 the two boost ratings,
 *       3 / 4 thrown to either side at the start of a penalty, 8 the penalty
 *       itself (for every racer).  Kinds 0 and 3/4 are scaled by
 *       sub_080c623c and scattered with LCG stream 4.
 *   sub_080c42dc   the shared step: copy the racer's layer, priority and
 *       palette bits, and its position unless Task.updateState is set.
 *   sub_080c4364 ... sub_080c45fc   the per-kind callbacks (Task.updateCallback):
 *       movement, the Task.unk6C/unk6E animation counters, and
 *       TaskFree(gCurTaskIdx) when the effect ends. */

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void TaskFree(s32 id);                                   /* kill task */
void TaskSleepForever(void);                                     /* end the running task */
u32 sub_080c4f60(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */

void sub_080c3f44(void)
{
    struct Task *u = &gTasks[gCurTask->unk18];
    s32 scale;
    s32 x;
    s32 y;
    s32 vx;
    s32 vy;

    switch (gCurTask->unk20) {
    case 1:
        gCurTask->unk6E = 3;
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->frameTable = gUnk_08755FC4;
        gCurTask->frame = 5;
        gCurTask->updateCallback = (u32)sub_080c44bc;
        break;
    case 2:
        gCurTask->unk6C = 0;
        gCurTask->unk6E = 0;
        gCurTask->updateState = 0;
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->frameTable = gUnk_08755FC4;
        gCurTask->updateCallback = (u32)sub_080c44f0;
        gAirGrindPtr->unk450 = 1;
        break;
    case 5:
        gCurTask->unk6C = 2;
        gCurTask->unk6E = 2;
        gCurTask->updateState = 1;
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->tileWord = 0x8210;
        gCurTask->frameTable = gUnk_087572E0;
        gCurTask->frame = 0;
        gCurTask->updateCallback = (u32)sub_080c4568;
        gAirGrindPtr->unk451 = 1;
        break;
    case 0:
        scale = sub_080c623c(gAirGrindCoursePtr->players[gCurTask->unk18].unk08);
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->layer = u->layer;
        gCurTask->spriteFlags = u->spriteFlags & 0x6000;
        gCurTask->moveCallback = (u32)TaskMove;
        gCurTask->velX = -u->velX;
        gCurTask->unk28 = u->unk28;
        x = u->pixelX - scale / 32;
        y = u->pixelY + (s32)((AirGrindRandom(4) & 7) - 3) * scale / 256;
        gCurTask->posX = x << 16;
        gCurTask->posY = y << 16;
        gCurTask->unk6C = 2;
        gCurTask->unk6E = 18;
        gCurTask->frameTable = gUnk_08755FC4;
        gCurTask->frame = 3;
        gCurTask->updateCallback = (u32)sub_080c4364;
        break;
    case 3:
    case 4:
        scale = sub_080c623c(gAirGrindCoursePtr->players[gCurTask->unk18].unk08);
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->moveCallback = (u32)TaskMove;
        gCurTask->layer = u->layer;
        gCurTask->spriteFlags = u->spriteFlags & 0x6000;
        vx = (s32)(AirGrindRandom(4) & 0xFFF) * (scale << 6) / 256;
        if (gCurTask->unk20 == 3)
            vx = -vx;
        vy = -((s32)((AirGrindRandom(4) & 0xFFF) * 48 + 0x10000) * scale) / 256;
        gCurTask->velX = vx;
        gCurTask->velY = vy;
        gCurTask->accelY = scale << 6;
        gCurTask->unk28 = u->unk28;
        x = u->pixelX + (s32)((AirGrindRandom(4) & 31) - 15) * scale / 256;
        y = u->pixelY - scale / 32;
        gCurTask->posX = x << 16;
        gCurTask->posY = y << 16;
        gCurTask->unk6C = 2;
        gCurTask->unk6E = 50;
        gCurTask->frameTable = gUnk_08755FC4;
        gCurTask->frame = 3;
        gCurTask->updateCallback = (u32)sub_080c43e8;
        break;
    case 6:
    case 7:
        gCurTask->drawCallback = (u32)TaskDrawScreen;
        gCurTask->moveCallback = (u32)TaskMove;
        gCurTask->layer = 3;
        gCurTask->spriteFlags = 0;
        gCurTask->posX = u->pixelX << 16;
        gCurTask->posY = u->pixelY << 16;
        gCurTask->velY = u->pixelY < 80 ? 0x10000 : -0x10000;
        gCurTask->velX = 0x10000;
        gCurTask->unk6E = 40;
        gCurTask->frameTable = gUnk_08755FEC;
        gCurTask->frame = gCurTask->unk20 == 6 ? 15 : 14;
        gCurTask->updateCallback = (u32)sub_080c45d4;
        break;
    case 8:
        gCurTask->unk6C = 12;
        gCurTask->unk6E = 9;
        gCurTask->updateState = 0;
        gCurTask->drawCallback = (u32)sub_080c4e10;
        gCurTask->frameTable = gUnk_08755FC4;
        gCurTask->frame = 9;
        gCurTask->updateCallback = (u32)sub_080c45fc;
        break;
    }
    TaskSetSkipMask(1, gCurTaskIdx);
    TaskSleepForever();
}

void sub_080c42dc(s32 layer)
{
    struct Task *u = &gTasks[gCurTask->unk18];

    gCurTask->unk28 = u->unk28;
    gCurTask->layer = u->layer + layer;
    gCurTask->spriteFlags = u->spriteFlags & 0x6000;
    if (gCurTask->updateState == 0) {
        gCurTask->pixelX = u->pixelX;
        gCurTask->pixelY = u->pixelY;
    } else {
        gCurTask->pixelX = gAirGrindCoursePtr->players[gCurTask->unk18].unk0C;
        gCurTask->pixelY = gAirGrindCoursePtr->players[gCurTask->unk18].unk10;
    }
}

void sub_080c4364(void)
{
    struct Task *u = &gTasks[gCurTask->unk18];

    gCurTask->layer = u->layer;
    gCurTask->spriteFlags = u->spriteFlags & 0x6000;
    gCurTask->unk28 += 14;
    if (--gCurTask->unk6E < 0) {
        TaskFree(gCurTaskIdx);
    } else {
        if ((s16)gCurTask->unk6C <= 0) {
            gCurTask->unk6C = 2;
            if (--gCurTask->frame < 0)
                gCurTask->frame = 3;
        }
        gCurTask->unk6C--;
    }
}

void sub_080c43e8(void)
{
    if (--gCurTask->unk6E < 0) {
        TaskFree(gCurTaskIdx);
        return;
    }
    if ((s16)gCurTask->unk6C <= 0) {
        gCurTask->unk6C = 2;
        if (--gCurTask->frame < 0)
            gCurTask->frame = 3;
    }
    gCurTask->unk6C--;
    if ((u16)(gCurTask->pixelX + 63) <= 366
        && gCurTask->pixelY > -64 && gCurTask->pixelY < 224) {
        u32 *tbl = gCurTask->frameTable;

        QueueSprite(gCurTask->layer,
                     sub_080c4f60((u16 *)tbl[gCurTask->frame], gCurTask->unk28),
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->pixelX, gCurTask->pixelY);
    } else
        TaskFree(gCurTaskIdx);
}

void sub_080c44bc(void)
{
    if (--gCurTask->unk6E < 0)
        TaskFree(gCurTaskIdx);
    else
        sub_080c42dc(0);
}

void sub_080c44f0(void)
{
    struct Task *t = gCurTask;

    /* Task.unk6C is u16 in task.h; this callback counts it as s16 */
    if ((*(s16 *)&t->unk6C)-- <= 0) {
        t->unk6C = 2;
        if (gUnk_080CFF52[++t->unk6E] != -1) {
            t->frame = gUnk_080CFF52[t->unk6E];
        } else {
            gAirGrindPtr->unk450 = 0;
            TaskFree(gCurTaskIdx);
            return;
        }
    }
    sub_080c42dc(0);
}

void sub_080c4568(void)
{
    if ((s16)gCurTask->unk6C <= 0) {
        gCurTask->unk6C = 2;
        if (++gCurTask->frame > gCurTask->unk6E) {
            gAirGrindPtr->unk451 = 0;
            TaskFree(gCurTaskIdx);
            return;
        }
    }
    gCurTask->unk6C--;
    sub_080c42dc(0);
}

void sub_080c45d4(void)
{
    if (--gCurTask->unk6E < 0)
        TaskFree(gCurTaskIdx);
}

void sub_080c45fc(void)
{
    /* Task.unk6C is u16 in task.h; this callback counts it as s16 */
    if ((*(s16 *)&gCurTask->unk6C)-- <= 0)
        TaskFree(gCurTaskIdx);
    else
        sub_080c42dc(-1);
}
