#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"

/* effect_58810.c (0x08058810-0x0805956F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 40 and 41, spawned by M13's ability get and actions.  Variant 40
 * (PlayerEffectIceBreathCloud, 1648 bytes) rides on its spawner through three sub-states,
 * each a long yield script whose every step stops once Task.unk28 is set;
 * its callback PlayerEffectIceBreathCloudUpdate sets it when the player leaves mode 13 or its
 * facing no longer matches the spawner's, and kills the task when the
 * ability is no longer 13 or PlayerState.unk40 bit 8 is clear while the
 * spawner's Task.waterFlags bit 0 is set.  Variant 41 (PlayerEffectFreezeAura, 1488 bytes)
 * is four sub-states in world space (gUnk_08751E7C); its callback
 * PlayerEffectFreezeAuraUpdate sets Task.unk28 when the player leaves mode 13 (or, while
 * PlayerState.unk40 bit 8 is clear, when the spawner's Task.variant is not 1)
 * and kills it on the same unk40/unk7B test. */

u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */

void PlayerEffectIceBreathCloud(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)TaskDrawWorld;
    t->updateCallback = (u32)PlayerEffectIceBreathCloudUpdate;
    t->frameTable = gUnk_08751E5C;
    t->tileWord = ((t->u8C.parentTask)->tileWord + 0x1800) | 8;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->layer = 5;
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(16);
        while (1)
        {
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x800;
            gCurTask->frame = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = -0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->accelY = -0x800;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x800;
            gCurTask->frame = 5;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = -0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->accelY = -0x800;
            gCurTask->frame = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x800;
            gCurTask->frame += 2;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = -0x400;
            gCurTask->frame = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->accelY = -0x800;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x800;
            gCurTask->frame += 2;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = 0x400;
            gCurTask->frame = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->accelY = -0x400;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->posY = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->accelY = -0x800;
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
        }
        break;
    case 1:
        gCurTask->layer = 8;
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gCurTask->posX = RandomSpreadFacing(32, 1, 8) << 16;
            gCurTask->posY = RandomSpreadFacing(4, 1, 8) << 16;
            TaskSetMotionXFacing(0x10000, 0x4000);
            gCurTask->velY = 0;
            gCurTask->accelY = (RandomRange(32) - 16) << 8;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    case 2:
        gCurTask->layer = 8;
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gCurTask->posX = RandomSpreadFacing(20, 1, 12) << 16;
            gCurTask->posY = RandomSpreadFacing(0, 1, 8) << 16;
            TaskSetMotionXFacing(0x8000, 0x2000);
            gCurTask->velY = 0;
            gCurTask->accelY = (RandomRange(32) - 16) << 8;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectIceBreathCloudUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && (t->player->mode != 13 || t->facing != (t->u8C.parentTask)->facing))
        t->unk28 = 1;
    if (!(gCurTask->player->unk40 & 0x100) && ((gCurTask->u8C.parentTask)->waterFlags & 1))
        TaskFree(gCurTaskIdx);
    if (gCurTask->player->ability != 13)
        TaskFree(gCurTaskIdx);
}

void PlayerEffectFreezeAura(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)PlayerEffectFreezeAuraUpdate;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751E7C;
    t->tileWord = ((t->u8C.parentTask)->tileWord + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        while (1)
        {
            gCurTask->posX = (RandomSpread(-24, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(4, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(-0x18000, 0x2000);
            gCurTask->velY = -0xC000;
            gCurTask->accelY = -0x1800;
            gCurTask->frame = 8;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = (RandomSpread(-24, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(-0x18000, 0x2000);
            gCurTask->velY = -0xC000;
            gCurTask->accelY = -0x1800;
            gCurTask->frame = 8;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            if (gCurTask->unk28 != 0)
                break;
        }
        break;
    case 1:
        t->frame = 0xFFFF;
        TaskYieldTrampoline(5);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpread(-20, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(16, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(-0xC000, 0x1000);
            gCurTask->velY = -0x14000;
            gCurTask->accelY = -0x2000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = (RandomSpread(-20, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(-0xC000, 0x1000);
            gCurTask->velY = -0x14000;
            gCurTask->accelY = -0x2000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        break;
    case 2:
        t->frame = 0xFFFF;
        TaskYieldTrampoline(10);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpread(-12, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(20, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0xC000, -0x1000);
            gCurTask->velY = -0x14000;
            gCurTask->accelY = -0x2000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posY = (RandomSpread(4, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0xC000, -0x1000);
            gCurTask->velY = -0x14000;
            gCurTask->accelY = -0x2000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        break;
    case 3:
        t->frame = 0xFFFF;
        TaskYieldTrampoline(15);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpread(-8, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(4, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0x18000, -0x2000);
            gCurTask->velY = -0xC000;
            gCurTask->accelY = -0x1800;
            gCurTask->frame = 14;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = (RandomSpread(-8, 1, 32) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0x18000, -0x2000);
            gCurTask->velY = -0xC000;
            gCurTask->accelY = -0x1800;
            gCurTask->frame = 14;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectFreezeAuraUpdate(void)
{
    struct Task *t = gCurTask;
    struct PlayerState *p = t->player;

    if (!(p->unk40 & 0x100))
    {
        if (t->unk28 == 0 && (p->mode != 13 || (t->u8C.parentTask)->variant != 1))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && p->mode != 13)
            t->unk28 = 1;
    }
    if (!(gCurTask->player->unk40 & 0x100) && ((gCurTask->u8C.parentTask)->waterFlags & 1))
        TaskFree(gCurTaskIdx);
}
