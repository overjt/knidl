#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* effect_54a80.c (0x08054A80-0x0805545F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 12-15.  Variant 12 (PlayerEffectDeathStarRing, M10/M11) has no motion or draw
 * hook: it picks one of eight directions by the low three bits of Task.unk18
 * and flies two particles from the spawner's position, rows gPlayerEffectDeathStarRingMotions[i]
 * and [i + 4] holding each one's 8.8 velocity and acceleration, which its
 * Task.lateUpdateCallback callback PlayerEffectDeathStarRingLateUpdate integrates every frame, drawing both with
 * QueueSprite when on screen (IsOnScreen; camera-relative unless
 * PlayerState.unk37 == 2).  Variant 13 (PlayerEffectMetaKnightDeathBlast, M11) plays the 23
 * frames of gExplosionAnimFrames.  Variant 14 (sub_08054de8, M13's ability get)
 * rides on its spawner through four sub-states of frame loops.  Variant 15
 * (sub_08054fe4; M09, M11, M13) runs one of two endless particle loops
 * chosen by the ability PlayerState.ability (1 or 2), re-seeding a random
 * position around the spawner every round.  Its callbacks: sub_080552fc
 * (Task.updateCallback) kills it when gRoomExitKind or gRoomPlayerMode is 1, the
 * ability is no longer the one saved in Task.unk28 or the player is in mode
 * 13 or 22, and otherwise hides it (TaskSetSkipMask(8, ...)) while the
 * spawner's Task.waterFlags bit 0 is set, M11's sub_0803eaf8 has no offset
 * (0x5A5A5A5A) or the player is in mode 20 with the spawner's Task.unk18
 * set, adding that offset to its position; sub_080553d4 (Task.lateUpdateCallback) draws
 * it with QueueSprite when on screen. */

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
s32 IsOnScreen(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell s32) */
u32 IsWorldPosOnScreen(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */

void PlayerEffectDeathStarRing(void)
{
    struct Task *t;
    s32 a;
    s32 b;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    gCurTask->lateUpdateCallback = (u32)PlayerEffectDeathStarRingLateUpdate;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gPlayerEffectDeathStarRingFrames;
    t->pixelX = (t->u8C.parentTask)->pixelX;
    t->pixelY = (t->u8C.parentTask)->pixelY;
    a = gPlayerEffectDeathStarRingMotions[t->playerEffectSpawnWord & 7][0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk28 = b;
    a = gPlayerEffectDeathStarRingMotions[t->playerEffectSpawnWord & 7][1];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk2C = b;
    t->posX = t->pixelX;
    t->posY = t->pixelY;
    a = gPlayerEffectDeathStarRingMotions[(t->playerEffectSpawnWord & 7) + 4][0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk30 = b;
    a = gPlayerEffectDeathStarRingMotions[(t->playerEffectSpawnWord & 7) + 4][1];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk34 = b;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->playerEffectLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->playerEffectLoopCount++;
    } while ((s16)gCurTask->playerEffectLoopCount <= 7);
    TaskExitTrampoline();
}

void PlayerEffectDeathStarRingLateUpdate(void)
{
    struct Task *t;
    u32 *tbl;
    s32 x;
    s32 y;
    s32 i;
    s32 j;

    t = gCurTask;
    tbl = t->frameTable;
    if (t->player->unk37 == 2)
    {
        x = t->pixelX;
        y = t->pixelY;
    }
    else
    {
        x = t->pixelX - gSpriteCameraX;
        y = t->pixelY - gSpriteCameraY;
    }
    if (IsOnScreen(x, y) != 0)
        QueueSprite(gCurTask->layer, tbl[gCurTask->frame], 0, 0, x, y);
    t = gCurTask;
    if (t->player->unk37 == 2)
    {
        x = t->posX;
        y = t->posY;
    }
    else
    {
        x = t->posX - gSpriteCameraX;
        y = t->posY - gSpriteCameraY;
    }
    if (IsOnScreen(x, y) != 0)
        QueueSprite(gCurTask->layer, tbl[gCurTask->frame], 0, 0, x, y);
    i = gCurTask->playerEffectSpawnWord & 7;
    gCurTask->unk28 += (gPlayerEffectDeathStarRingMotions[i][2] & 0x8000) ? (gPlayerEffectDeathStarRingMotions[i][2] << 8) | 0xFF000000 : gPlayerEffectDeathStarRingMotions[i][2] << 8;
    gCurTask->unk2C += (gPlayerEffectDeathStarRingMotions[i][3] & 0x8000) ? (gPlayerEffectDeathStarRingMotions[i][3] << 8) | 0xFF000000 : gPlayerEffectDeathStarRingMotions[i][3] << 8;
    gCurTask->pixelX += gCurTask->unk28 >> 16;
    gCurTask->pixelY += gCurTask->unk2C >> 16;
    j = (gCurTask->playerEffectSpawnWord & 7) + 4;
    gCurTask->unk30 += (gPlayerEffectDeathStarRingMotions[j][2] & 0x8000) ? (gPlayerEffectDeathStarRingMotions[j][2] << 8) | 0xFF000000 : gPlayerEffectDeathStarRingMotions[j][2] << 8;
    gCurTask->unk34 += (gPlayerEffectDeathStarRingMotions[j][3] & 0x8000) ? (gPlayerEffectDeathStarRingMotions[j][3] << 8) | 0xFF000000 : gPlayerEffectDeathStarRingMotions[j][3] << 8;
    gCurTask->posX += ((s16 *)&gCurTask->unk30)[1];
    gCurTask->posY += ((s16 *)&gCurTask->unk34)[1];
}

void PlayerEffectMetaKnightDeathBlast(void)
{
    struct Task *t;
    s32 i;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gExplosionFrames;
    for (i = 0; i < 23; i++)
    {
        gCurTask->frame = gExplosionAnimFrames[i];
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_08054de8(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C67C;
    switch (t->playerEffectSpawnWord & 15)
    {
    case 0:
        t->posX = -0xA0000;
        t->posY = -0x40000;
        t->frame = 12;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->posX = 0xA0000;
        gCurTask->posY = -0xA0000;
        gCurTask->frame = 12;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->posX = 0xA0000;
        gCurTask->posY = -0x40000;
        gCurTask->frame = 12;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->posX = -0xA0000;
        gCurTask->posY = -0xA0000;
        gCurTask->frame = 12;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        break;
    case 1:
        t->posX = 0;
        t->posY = 0x80000;
        t->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->playerEffectLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 10);
        break;
    case 2:
        t->posX = 0;
        t->posY = 0x80000;
        t->frame = 0xFFFF;
        TaskYieldTrampoline(14);
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->playerEffectLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 5);
        break;
    case 3:
        t->layer = 8;
        gCurTask->posX = 0;
        gCurTask->posY = -0x80000;
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(20);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->playerEffectLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->playerEffectLoopCount++;
        } while ((s16)gCurTask->playerEffectLoopCount <= 7);
        break;
    }
    TaskExitTrampoline();
}

void sub_08054fe4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_080552fc;
    t->lateUpdateCallback = (u32)sub_080553d4;
    t->layer = (t->u8C.parentTask)->layer;
    u = gCurTask;
    u->unk28 = u->player->ability;
    switch (u->player->ability)
    {
    case ABILITY_FIRE:
        u->facing = -1;
        gCurTask->frameTable = gUnk_0874C718;
        for (;;)
        {
            gCurTask->playerEffectLoopCount = 0;
            do
            {
                gCurTask->facing = -gCurTask->facing;
                gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + (gCurTask->u8C.parentTask)->pixelX) << 16;
                gCurTask->posY = (RandomSpread(-12, 1, 12) + (gCurTask->u8C.parentTask)->pixelY - 4) << 16;
                TaskSetMotionXFacing(0xC000, -0x1000);
                gCurTask->velY = -0x8000;
                gCurTask->accelY = 0;
                TaskSetFrameByFacing(18);
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                TaskSetFrameByFacing(14);
                TaskYieldTrampoline(2);
                gCurTask->velY = -0x10000;
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->velY = -0x20000;
                TaskSetFrameByFacing(24);
                TaskYieldTrampoline(2);
                gCurTask->playerEffectLoopCount++;
            } while ((s16)gCurTask->playerEffectLoopCount <= 1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(4);
        }
    case ABILITY_SPARK:
        v = gCurTask;
        v->frameTable = gUnk_08751C74;
        v->tileWord = ((v->u8C.parentTask)->tileWord + 0x1800) | 8;
        for (;;)
        {
            gCurTask->playerEffectLoopCount = 0;
            do
            {
                gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + (gCurTask->u8C.parentTask)->pixelX) << 16;
                gCurTask->posY = (RandomSpread(-16, 1, 12) + (gCurTask->u8C.parentTask)->pixelY) << 16;
                gCurTask->frame = RandomRange(12);
                TaskYieldTrampoline(2);
                gCurTask->playerEffectLoopCount++;
            } while ((s16)gCurTask->playerEffectLoopCount <= 2);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(8);
            gCurTask->playerEffectLoopCount = 0;
            do
            {
                gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + (gCurTask->u8C.parentTask)->pixelX) << 16;
                gCurTask->posY = (RandomSpread(-16, 1, 12) + (gCurTask->u8C.parentTask)->pixelY - 10) << 16;
                gCurTask->frame = RandomRange(12);
                TaskYieldTrampoline(2);
                gCurTask->playerEffectLoopCount++;
            } while ((s16)gCurTask->playerEffectLoopCount <= 2);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(16);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-16, 1, 12) + (gCurTask->u8C.parentTask)->pixelY - 10) << 16;
            gCurTask->frame = RandomRange(12);
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(8);
        }
    }
    TaskExitTrampoline();
}

void sub_080552fc(void)
{
    struct Task *t;
    struct PlayerState *ps;
    s32 ok;
    s32 r;

    if (gRoomExitKind == 1
     || (t = gCurTask, ps = t->player, t->unk28 != ps->ability)
     || ps->mode == 13 || ps->mode == 22
     || gRoomPlayerMode == 1)
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    ok = 1;
    if ((t->u8C.parentTask)->waterFlags & 1)
    {
        ok = 0;
    }
    else
    {
        r = sub_0803eaf8(ps->playerIndex);
        if (r == 0x5A5A5A5A)
            ok = 0;
        gCurTask->pixelX = (gCurTask->posX >> 16) + ((u32)r >> 16);
        gCurTask->pixelY = (gCurTask->posY >> 16) + r;
    }
    if (gCurTask->player->mode == 20 && (gCurTask->u8C.parentTask)->unk18 != 0)
        ok = 0;
    if (ok == 0)
        TaskSetSkipMask(TASK_SKIP_LATE_UPDATE, gCurTaskIdx);
    else
        TaskSetSkipMask(0, gCurTaskIdx);
}

void sub_080553d4(void)
{
    struct Task *t;
    u32 *tbl;
    s32 x, y;

    if (gCurTask->frameTable == 0)
        return;
    if (gCurTask->frame == -1)
        return;
    if (IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY) == 0)
        return;
    t = gCurTask;
    tbl = t->frameTable;
    x = t->pixelX - gSpriteCameraX;
    y = t->pixelY - gSpriteCameraY;
    QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord, x, (s16)y);
}
