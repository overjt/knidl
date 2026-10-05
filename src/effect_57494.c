#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"

/* effect_57494.c (0x08057494-0x08057CDF, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 32-34.  Variant 32 (PlayerEffectBurningFlames, M12/M13) is a nine-way jump
 * table over its sub-state (cases 5-8 share one arm) with the per-sub-state
 * rows gUnk_0873BAB0[][3] (8.8 x velocity, 8.8 y acceleration, frame); its
 * sub-states respawn variant 32 and install PlayerEffectBurningFlamesUpdate, which kills the
 * task once the player leaves mode 13 or the spawner's Task.waterFlags bit 0 is
 * set.  Variant 33 (PlayerEffectUFOLaserTrail, spawned by M14's task type #6) is a short
 * animation from gPlayerUFOShotFrames.  Variant 34 (sub_08057ad4, M12) has two
 * sub-states with the draw hooks TaskDrawWorldInViewOrFree and sub_0805af80 (shared with
 * variant 48) and the kill test sub_08057c98 (player mode 13). */

void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */

void PlayerEffectBurningFlames(void)
{
    struct Task *t;
    u16 *row;
    s32 s;

    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gPlayerEffectBurningFlamesFrames;
    s = t->playerEffectSpawnWord & 15;
    row = gUnk_0873BAB0[s];
    switch (s)
    {
    case 0:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMove;
            u->frameTable = gUnk_0874C600;
            u->updateCallback = (u32)PlayerEffectBurningFlamesUpdate;
            u->playerEffectLoopCount = 0;
        }
        do
        {
            {
                struct Task *v = gCurTask;
                struct Task *p;

                if (v->facing == 1)
                    v->posX = ((p = v->u8C.parentTask)->pixelX - 6) << 16;
                else
                    v->posX = ((p = v->u8C.parentTask)->pixelX + 6) << 16;
                v->posY = ((v->u8C.parentTask)->pixelY + 8) << 16;
            }
            {
                s32 a = row[0];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                TaskSetMotionXFacing(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gCurTask;

                {
                    s32 a = row[1];
                    s32 b = a << 8;

                    if (a & 0x8000)
                        b |= 0xFF000000;
                    w->accelY = b;
                }
            }
            TaskSetFrameByFacing(row[2]);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 1);
            TaskYieldTrampoline(1);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(2);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            TaskStop();
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 1);
        break;
    case 1:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMove;
            u->frameTable = gUnk_0874C600;
        }
        gCurTask->posX = (gCurTask->pixelX + RandomSpreadFacing(-8, 1, 8)) << 16;
        gCurTask->posY = (gCurTask->pixelY + RandomSpread(-8, 1, 8)) << 16;
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, b);
        }
        {
            struct Task *w = gCurTask;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->accelY = b;
            }
        }
        TaskSetFrameByFacing(row[2]);
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        break;
    case 2:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMove;
            u->frameTable = gUnk_0874C718;
        }
        gCurTask->posX = (RandomSpreadFacing(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
        gCurTask->posY = (RandomSpread(-4, 1, 16) + (gCurTask->u8C.parentTask)->pixelY - 8) << 16;
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        gCurTask->accelY = -0x6000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(2);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 24;
        TaskYieldTrampoline(2);
        break;
    case 3:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMove;
            u->tileWord = (u->u8C.parentTask)->tileWord | 0x1808;
            u->updateCallback = (u32)PlayerEffectBurningFlamesUpdate;
            u->playerEffectLoopCount = 0;
        }
        do
        {
            TaskSetMotionXFacing(-0x10000, -0x2000);
            gCurTask->posX = (RandomSpreadFacing(-16, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetFrameByFacing(16);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 2);
        break;
    case 4:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMove;
            u->tileWord = (u->u8C.parentTask)->tileWord | 0x1808;
            u->updateCallback = (u32)PlayerEffectBurningFlamesUpdate;
            u->playerEffectLoopCount = 0;
        }
        do
        {
            TaskSetMotionXFacing(-0x20000, -0x2000);
            gCurTask->posX = (RandomSpreadFacing(-16, 1, 24) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-4, 1, 16) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetFrameByFacing(16);
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 2);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        {
            struct Task *u = gCurTask;

            u->moveCallback = (u32)TaskMoveRelativeToParent;
            u->tileWord = (u->u8C.parentTask)->tileWord | 0xF008;
        }
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            TaskSetMotionXFacing(b, 0x5A5A5A5A);
        }
        {
            struct Task *w = gCurTask;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->velY = b;
            }
        }
        gCurTask->posX = RandomSpreadFacing(-4, 1, 8) << 16;
        gCurTask->posY = RandomSpread(-4, 1, 8) << 16;
        gCurTask->frame = row[2];
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectBurningFlamesUpdate(void)
{
    if (gCurTask->player->mode != 13 || ((gCurTask->u8C.parentTask)->waterFlags & 1))
        TaskFree(gCurTaskIdx);
}

void PlayerEffectUFOLaserTrail(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gPlayerUFOShotFrames;
    t->tileWord = gTasks[t->parent].tileWord;
    t->frame = gUnk_0873BAE6[t->playerEffectSpawnWord & 15];
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08057ad4(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    switch (t->playerEffectSpawnWord & 15)
    {
    case 0:
        t->drawCallback = (u32)sub_0805af80;
        t->updateCallback = (u32)sub_08057c98;
        t->layer = 8;
        {
            struct Task *u = gCurTask;

            u->frameTable = gUnk_08751D80;
            u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 4;
            u->frame = 0xFFFF;
        }
        while ((gCurTask->u8C.parentTask)->variant == 0)
            TaskYieldTrampoline(1);
        for (;;)
        {
            {
                struct Task *v = gCurTask;

                v->posX = (v->u8C.parentTask)->pixelX << 16;
                v->posY = (v->u8C.parentTask)->pixelY << 16;
            }
            TaskSetFrame(0);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            {
                struct Task *w = gCurTask;

                if ((w->u8C.parentTask)->variant == 2)
                {
                    w->velX = 0;
                    w->frame = 0xFFFF;
                    while ((gCurTask->u8C.parentTask)->variant == 2)
                        TaskYieldTrampoline(1);
                }
            }
        }
    case 1:
        gCurTask->drawCallback = (u32)TaskDrawWorldInViewOrFree;
        gCurTask->layer = 5;
        {
            struct Task *u = gCurTask;

            u->frameTable = gUnk_08751D50;
            if (u->facing == 1)
                u->posX = ((u->u8C.parentTask)->pixelX - 8) << 16;
            else
                u->posX = ((u->u8C.parentTask)->pixelX + 8) << 16;
        }
        {
            struct Task *v = gCurTask;

            v->posY = (v->u8C.parentTask)->pixelY << 16;
            if ((v->u8C.parentTask)->playerWheelOnWater == 0)
            {
                v->tileWord = ((v->u8C.parentTask)->tileWord + 0x1800) | 8;
                TaskSetFrameByFacing(0);
            }
            else
            {
                v->tileWord = 0;
                TaskSetFrameByFacing(6);
            }
        }
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(4);
        break;
    }
    TaskExitTrampoline();
}

void sub_08057c98(void)
{
    struct Task *t = gCurTask;
    u8 s;

    if (t->player->mode != 13 || (s = (t->u8C.parentTask)->variant) == 3 || s == 4)
        TaskFree(gCurTaskIdx);
    else
        t->facing = (t->u8C.parentTask)->facing;
}
