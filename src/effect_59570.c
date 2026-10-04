#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"

/* effect_59570.c (0x08059570-0x0805A357, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 42-44, spawned by M13's actions (44 also by M14's action 49
 * sub-actions).  Variant 42 (sub_08059570) is a six-way jump table over its
 * sub-state, mixing forms that stay put and forms that move (gUnk_0874C804,
 * gUnk_0874C828, gUnk_08751F0C); its callbacks are sub_08059aac (the
 * collider rows gUnk_0873C23C and gUnk_0873C250 through RegisterCollider in
 * sub-states 3 and 4) and the draw hook sub_08059b18 (QueueSprite when on
 * screen).  Variant 43 (PlayerEffectStonePuff) has four sub-states with the draw
 * hooks TaskDrawWorldTilesLoaded and M11's sub_0803dfc8.  Variant 44 (sub_08059d7c)
 * selects on the second byte of Task.unk18 (0x100-0x500), queues a VRAM
 * transfer (RequestCopy) and installs sub_0805a320, which kills it when the
 * spawner's Task.variant is 8 or the player is in neither mode 13 nor mode 3. */

void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u32 IsWorldPosOnScreen(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */

void sub_08059570(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->frameTable = gUnk_08751F0C;
    switch (gCurTask->playerEffectSpawnWord & 15)
    {
    case 0:
        gCurTask->layer = 8;
        while (1)
        {
            u = gCurTask;
            u->velX = -(u->u8C.parentTask)->velX;
            u->speedLimitX = 0x40000;
            u->velY = 0x20000;
            u->speedLimitY = 0x20000;
            u->posX = (u->u8C.parentTask)->pixelX << 16;
            u->posY = ((u->u8C.parentTask)->pixelY + 16) << 16;
            u->frame = 16;
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
            if (gCurTask->player->invulnerability != 3 || gCurTask->player->mode != 13)
                break;
        }
        break;
    case 1:
        gCurTask->layer = 5;
        while (1)
        {
            v = gCurTask;
            v->velX = -(v->u8C.parentTask)->velX;
            v->speedLimitX = 0x40000;
            v->velY = 0x20000;
            v->speedLimitY = 0x20000;
            gCurTask->posX = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelY + 16) << 16;
            gCurTask->frame = 26;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            if (gCurTask->player->invulnerability != 3 || gCurTask->player->mode != 13)
                break;
        }
        break;
    case 2:
        gCurTask->layer = 5;
        while (1)
        {
            w = gCurTask;
            w->velX = -(w->u8C.parentTask)->velX;
            w->speedLimitX = 0x40000;
            w->velY = 0x20000;
            w->speedLimitY = 0x20000;
            gCurTask->posX = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelY + 16) << 16;
            gCurTask->frame = 22;
            TaskYieldTrampoline(1);
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
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            if (gCurTask->player->invulnerability != 3 || gCurTask->player->mode != 13)
                break;
        }
        break;
    case 3:
        gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
        gCurTask->updateCallback = (u32)sub_08059aac;
        gCurTask->layer = 8;
        gCurTask->frameTable = gUnk_0874C804;
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->updateCallback = 0;
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
    case 4:
        gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
        gCurTask->drawCallback = (u32)TaskDrawWorld;
        gCurTask->updateCallback = (u32)sub_08059aac;
        gCurTask->layer = 5;
        t = gCurTask;
        t->frameTable = gUnk_0874C828;
        t->posY = (t->pixelY + 1) << 16;
        t->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->updateCallback = 0;
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 24;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
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
        TaskYieldTrampoline(1);
        TaskExitTrampoline();
    case 5:
        gCurTask->moveCallback = (u32)TaskMove;
        gCurTask->drawCallback = (u32)sub_08059b18;
        gCurTask->layer = 5;
        t = gCurTask;
        t->unk28 = t->pixelX;
        t->unk2C = t->pixelY;
        t->posX = 0;
        t->posY = 0x30000;
        t->velX = -0x40000;
        t->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        gCurTask->accelX = 0x800;
        gCurTask->frame += 2;
        TaskYieldTrampoline(5);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(3);
        gCurTask->playerEffectLoopCount = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 1);
        gCurTask->playerEffectLoopCount = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 1);
        break;
    }
    TaskExitTrampoline();
}

void sub_08059aac(void)
{
    struct Task *t = gCurTask;

    switch (t->playerEffectSpawnWord & 15)
    {
    case 3:
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873C23C);
        break;
    case 4:
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873C250);
        break;
    }
}

void sub_08059b18(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->frame != -1)
    {
        if (IsWorldPosOnScreen((s16)(t->unk28 + t->pixelX), (s16)(t->unk2C + t->pixelY)) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_0874C828[u->frame], 0, 0,
                         u->unk28 + u->pixelX - gSpriteCameraX,
                         (s16)(u->unk2C + u->pixelY - gSpriteCameraY));
        }
        t = gCurTask;
        if (IsWorldPosOnScreen((s16)(t->unk28 - t->pixelX), (s16)(t->unk2C + t->pixelY)) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_0874C828[(s16)(u->frame | 1)], 0, 0,
                         u->unk28 - u->pixelX - gSpriteCameraX,
                         (s16)(u->unk2C + u->pixelY - gSpriteCameraY));
        }
    }
}

void PlayerEffectStonePuff(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751ECC;
    t->tileWord = ((t->u8C.parentTask)->tileWord + 0x800) | 12;
    t->spriteFlags = 0;
    gCurTask->posX = (RandomSpreadFacing(-4, 1, 8) + (gCurTask->u8C.parentTask)->pixelX) << 16;
    gCurTask->posY = (RandomSpread(-4, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
    u = gCurTask;
    switch (u->playerEffectSpawnWord & 15)
    {
    case 0:
        u->drawCallback = (u32)sub_0803dfc8;
        u->velX = -0x18000;
        u->velY = -0x18000;
        u->frame = 0;
        break;
    case 1:
        u->drawCallback = (u32)TaskDrawWorldTilesLoaded;
        u->velX = 0x18000;
        u->velY = -0x18000;
        u->frame = 4;
        break;
    case 2:
        u->drawCallback = (u32)TaskDrawWorldTilesLoaded;
        u->velX = -0x18000;
        u->velY = 0x18000;
        u->frame = 8;
        break;
    case 3:
        u->drawCallback = (u32)TaskDrawWorldTilesLoaded;
        u->velX = 0x18000;
        u->velY = 0x18000;
        u->frame = 11;
        break;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0;
    gCurTask->velY = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08059d7c(void)
{
    struct Task *t = gCurTask;

    switch (t->playerEffectSpawnWord & 0xFF00)
    {
    case 0x100:
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)sub_0803dfc8;
        t->updateCallback = (u32)sub_0805a320;
        t->layer = 5;
        {
            struct Task *u = gCurTask;

            u->frameTable = gUnk_08751F84;
            u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 12;
            u->posX = 0;
            u->posY = 0;
        }
        for (;;)
        {
            while (gCurTask->player->invulnerability == 3)
            {
                gCurTask->frame = 0;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame = 0xFFFF;
                TaskYieldTrampoline(4);
            }
            gCurTask->frame = 0xFFFF;
            while (gCurTask->player->invulnerability != 3)
                TaskYieldTrampoline(1);
        }
    case 0x200:
        gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
        gCurTask->drawCallback = (u32)TaskDrawWorld;
        gCurTask->updateCallback = (u32)sub_0805a320;
        gCurTask->layer = 8;
        gCurTask->frameTable = gUnk_08751F0C;
        for (;;)
        {
            while (abs((gCurTask->u8C.parentTask)->velY) > 0x2FFFF)
            {
                gCurTask->posX = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
                gCurTask->posY = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelY) << 16;
                gCurTask->frame = 26;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
            gCurTask->frame = 0xFFFF;
            while (abs((gCurTask->u8C.parentTask)->velY) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x300:
        gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
        gCurTask->drawCallback = (u32)TaskDrawWorld;
        gCurTask->updateCallback = (u32)sub_0805a320;
        gCurTask->layer = 5;
        gCurTask->frameTable = gUnk_08751F0C;
        for (;;)
        {
            while (abs((gCurTask->u8C.parentTask)->velY) > 0x2FFFF)
            {
                gCurTask->posX = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelX) << 16;
                gCurTask->posY = (RandomSpread(-8, 1, 16) + (gCurTask->u8C.parentTask)->pixelY) << 16;
                gCurTask->frame = 22;
                TaskYieldTrampoline(1);
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
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame = 0xFFFF;
            while (abs((gCurTask->u8C.parentTask)->velY) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x400:
        gCurTask->moveCallback = (u32)TaskUpdatePixelPos;
        gCurTask->drawCallback = (u32)TaskDrawWorld;
        gCurTask->updateCallback = (u32)sub_0805a320;
        gCurTask->layer = 8;
        gCurTask->frameTable = gUnk_08751F0C;
        for (;;)
        {
            while (abs((gCurTask->u8C.parentTask)->velY) > 0x37FFF)
            {
                gCurTask->posX = (gCurTask->u8C.parentTask)->pixelX << 16;
                gCurTask->posY = (gCurTask->u8C.parentTask)->pixelY << 16;
                gCurTask->frame = 16;
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
            }
            gCurTask->frame = 0xFFFF;
            while (abs((gCurTask->u8C.parentTask)->velY) <= 0x37FFF)
                TaskYieldTrampoline(1);
        }
    case 0x500:
        gCurTask->moveCallback = (u32)TaskMove;
        gCurTask->drawCallback = (u32)TaskDrawWorld;
        gCurTask->layer = 5;
        {
            struct Task *u = gCurTask;
            u32 off;
            u8 *src;

            u->frameTable = gUnk_08751F0C;
            u->tileWord = (u->u8C.parentTask)->tileWord | 0xF008;
            off = ((u->u8C.parentTask)->tileWord & 0x7FF) << 5;
            src = gUnk_081FD870;
            RequestCopy(1, src, (void *)(off + (OBJ_VRAM0 + 0x100)), 128);
            RequestCopy(1, src + 128, (void *)(off + (OBJ_VRAM0 + 0x500)), 128);
            RequestCopy(1, src + 256, (void *)(off + (OBJ_VRAM0 + 0x900)), 128);
            RequestCopy(1, src + 384, (void *)(off + (OBJ_VRAM0 + 0xD00)), 128);
        }
        gCurTask->posX = (gCurTask->pixelX + RandomSpreadFacing(0, 1, 8)) << 16;
        gCurTask->posY = (gCurTask->pixelY + RandomSpread(0, 1, 8)) << 16;
        {
            u16 *row = gUnk_0873BB0E[gCurTask->playerEffectSpawnWord & 15];

            {
                s32 a = row[0];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                TaskSetMotionXFacing(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gCurTask;
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->velY = b;
                w->frame = row[2];
            }
        }
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        TaskExitTrampoline();
        break;
    }
    TaskExitTrampoline();
}

void sub_0805a320(void)
{
    struct Task *t = gCurTask;

    if ((t->u8C.parentTask)->variant == 8 || (t->player->mode != 13 && t->player->mode != 3))
        TaskFree(gCurTaskIdx);
}
