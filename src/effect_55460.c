#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "effect.h"

/* effect_55460.c (0x08055460-0x08055B23, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 16-21 (entry 16, PlayerEffectDanceStarBurst, sits after entries 17 and 18 in
 * the ROM).  Variants 16, 17 and 19 are spawned by M16 (16 also by M17's
 * actor core), 18 and 20 by M10, 21 by M11. 16 (PlayerEffectDanceStarBurst), 17
 * (sub_08055460) and 18 (sub_08055520) are animations that stay put
 * (TaskUpdatePixelPos), from gUnk_0874C804, gUnk_0874C960 and gUnk_0874C980; 19
 * (sub_0805574c) has no draw hook and draws itself through its Task.updateCallback
 * callback sub_080557d4 (QueueSprite); 20 (sub_0805587c) is a
 * three-sub-state puff whose sub-states 0 and 1 (one differing store, then a
 * shared body) spawn its sub-state 2; 21 (PlayerEffectLocalPlayerArrow) rides on its
 * spawner, and PlayerEffectLocalPlayerArrowUpdate kills it (or, while gUnk_0300244C is set, hides
 * it) when the player is in mode 13, 16, 18 or 20. */

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */

void sub_08055460(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C960;
    t->posX = (t->u8C.parentTask)->pixelX << 16;
    t->posY = ((t->u8C.parentTask)->pixelY - 8) << 16;
    t->frame = 0;
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
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08055520(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gUnk_0874C980;
    t->posX = (t->u8C.parentTask)->pixelX << 16;
    t->posY = (t->u8C.parentTask)->pixelY << 16;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 12;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->playerEffectLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->playerEffectLoopCount++;
    } while ((s16)gCurTask->playerEffectLoopCount <= 7);
    TaskExitTrampoline();
}

void PlayerEffectDanceStarBurst(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gUnk_0874C804;
    t->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0805574c(void)
{
    struct Task *t;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)sub_080557d4;
    gCurTask->layer = 5;
    t = gCurTask;
    t->pixelX = (t->u8C.parentTask)->pixelX;
    t->pixelY = (t->u8C.parentTask)->pixelY;
    t->frame = 0;
    TaskYieldTrampoline(4);
    gCurTask->playerEffectLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->playerEffectLoopCount++;
    } while ((s16)gCurTask->playerEffectLoopCount <= 6);
    gCurTask->player->unk40 |= 0x80;
    TaskExitTrampoline();
}

void sub_080557d4(void)
{
    QueueSprite(gCurTask->layer, gUnk_0874C784[gCurTask->frame], 0, 0,
                 gCurTask->pixelX - 48 - gSpriteCameraX,
                 gCurTask->pixelY - gSpriteCameraY);
    QueueSprite(gCurTask->layer, gUnk_0874C784[gCurTask->frame], 0, 0,
                 gCurTask->pixelX + 48 - gSpriteCameraX,
                 gCurTask->pixelY - gSpriteCameraY);
}

void sub_0805587c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *p;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    switch (t->playerEffectSpawnWord & 15)
    {
    case 0:
        t->facing = 1;
        goto common;
    case 1:
        t->facing = -1;
    common:
        TaskStop();
        u = gCurTask;
        if (u->facing == 1)
            u->posX = ((p = u->u8C.parentTask)->pixelX - 6) << 16;
        else
            u->posX = ((p = u->u8C.parentTask)->pixelX + 6) << 16;
        u->posY = ((u->u8C.parentTask)->pixelY + 8) << 16;
        gCurTask->accelY = -0x2000;
        TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 20, 2);
        TaskSetMotionXFacing(0x5A5A5A5A, 0);
        TaskYieldTrampoline(1);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        break;
    case 2:
        gCurTask->posX = (gCurTask->pixelX + RandomSpreadFacing(-8, 1, 8)) << 16;
        gCurTask->posY = (gCurTask->pixelY + RandomSpread(-8, 1, 8)) << 16;
        TaskSetMotionXFacing(0x5A5A5A5A, 0x4000);
        gCurTask->accelY = -0x4000;
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectLocalPlayerArrow(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)PlayerEffectLocalPlayerArrowUpdate;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C780;
    t->posX = 0;
    t->posY = -0xC0000;
    t->playerEffectLoopCount = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(8);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->playerEffectLoopCount++;
    } while ((s16)gCurTask->playerEffectLoopCount <= 7);
    TaskExitTrampoline();
}

void PlayerEffectLocalPlayerArrowUpdate(void)
{
    if (gUnk_0300244C == 0)
    {
        u8 a = gCurTask->player->mode;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            TaskFree(gCurTaskIdx);
    }
    else
    {
        u8 a = gCurTask->player->mode;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            gCurTask->frame = 0xFFFF;
    }
}
