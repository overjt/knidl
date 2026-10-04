#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "player.h"

/* plobj_514f8.c (0x080514F8-0x0805239B, issue #90).
 *
 * Task type #6, variants 3-6, each variant body followed by the callbacks
 * only it installs; variants 3-5 read their spawner's task through
 * Task.u8C.parentTask (position, facing, OAM flags).  Variant 3 (PlayerObjectWaterShot,
 * animation table gPlayerObjectWaterShotFrames) is a four-way `switch (Task.unk28)` of
 * endless loops that the ROM lays out in the order 3, 1, 0, 2, with M11's
 * callback PlayerDrawWorldLoadTilesAndPalette and the collision callback PlayerObjectWaterShotUpdate (sound
 * 133).  Variant 4 (PlayerObjectFireBreath, gPlayerObjectFireBreathFrames/gUnk_08751CA4) installs
 * PlayerObjectFireBreathUpdate, which registers the collider row gUnk_0873BDD4 and calls
 * the hit test TaskBreakBlocksAt at the spawner's position with only three
 * arguments.  Variant 5 (PlayerObjectCutterBlade, gUnk_08751A98) and its callback
 * PlayerObjectCutterBladeUpdate re-bind the body or the shared exit PlayerObjectVanish.  Variant
 * 6 (PlayerObjectLaserBeam, gPlayerObjectLaserBeamFrames, the animation/velocity pairs
 * gUnk_0873B7B0) and its callback PlayerObjectLaserBeamUpdate (a nine-way `switch` on
 * the collision result gTerrainResult.unk4, sounds 173 and 211) turn the
 * object into variant 10's body PlayerObjectUFOShot or the burst PlayerObjectLaserBeamVanish on
 * contact. */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
u32 RandomRange(u32 range);
void TaskSetEntry(void *a, u32 i);
u16 RandomSpread(u16 base, u8 scale, u8 amount);
s16 RandomSpreadFacing(u16 base, u8 scale, u8 amount);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void TerrainCollideBoxAlongVelocity(const s8 *p);
void sub_0802205c(s8 *box);
u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y);   /* this file's call passes only x and y: the ROM leaves r3 as it was (the definition in src/block_30804.c takes a fourth, `e`) */
s32 TaskBreakBlocks(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/PlayerObjectAirPuffUpdate.c); landed M09/M12/M13 files spell it u16 */

void PlayerObjectWaterShot(void)
{
    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)PlayerDrawWorldLoadTilesAndPalette;
        t->updateCallback = (u32)PlayerObjectWaterShotUpdate;
        t->layer = 7;
    }
    {
        struct Task *t = gCurTask;
        t->frameTable = gPlayerObjectWaterShotFrames;
        t->tileWord = (t->u8C.parentTask)->tileWord | 0xE006;
        t->unk28 = (t->u8C.parentTask)->unk28;
        t->unk2C = 1;
        gPlayerBodyBoxes[t->player->playerIndex] = *(struct PlayerBodyBox *)gUnk_0873BDA0;
    }
    LoadPlayerBodyBoxRect(gCurTask->player->playerIndex, (u8 *)gUnk_0873BDB4 + gCurTask->unk28 * 8);
    {
        struct Task *t;
        gPlayerHitBoxSets[(t = gCurTask)->player->playerIndex] = *(struct PlayerHitBoxSet *)gUnk_0873CBAC;
        LoadPlayerHitBoxSet(t->player->playerIndex, (s32)((u8 *)gUnk_0873CBB4 + t->unk28 * 8));
    }
    switch (gCurTask->unk28)
    {
    case 3:
        while (1)
        {
            {
                struct Task *t = gCurTask;
                t->posX = 0;
                t->posY = -0x100000;
            }
            TaskStopY();
            {
                struct Task *t = gCurTask;
                t->velY = -0x30000;
                t->accelY = 0x8000;
                t->frame = 8;
            }
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
    case 1:
        while (1)
        {
            {
                struct Task *t = gCurTask;
                t->posX = 0;
                t->posY = 0x100000;
            }
            TaskStopY();
            {
                struct Task *t = gCurTask;
                t->velY = 0x30000;
                t->accelY = -0x8000;
                t->frame = 9;
            }
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
    case 0:
        while (1)
        {
            {
                struct Task *t = gCurTask;
                t->posX = 0x100000;
                t->posY = 0;
            }
            TaskStopX();
            {
                struct Task *t = gCurTask;
                t->velX = 0x30000;
                t->accelX = -0x8000;
                t->frame = 0;
            }
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
    case 2:
        while (1)
        {
            {
                struct Task *t = gCurTask;
                t->posX = -0x100000;
                t->posY = 0;
            }
            TaskStopX();
            {
                struct Task *t = gCurTask;
                t->velX = -0x30000;
                t->accelX = 0x8000;
                t->frame = 1;
            }
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
    }
}

void PlayerObjectWaterShotUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->player->mode != 11 || t->unk28 != (t->u8C.parentTask)->unk28)
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    if (--t->unk2C == 0)
    {
        t->unk2C = 4;
        PlaySfxIfLocalPlayer(133, t->parent);
    }
    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                 (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
    TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex], gCurTask->parent);
}

void PlayerObjectFireBreath(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->updateCallback = (u32)PlayerObjectFireBreathUpdate;
    t->unk28 = 0;
    t->u80.attackAbility = ABILITY_FIRE;
    t = gCurTask;
    switch (t->playerObjectSpawnWord & 15)
    {
    case 0:
        t->drawCallback = (u32)PlayerDrawWorldLoadTilesAndPalette;
        t->frameTable = gPlayerObjectFireBreathFrames;
        t->layer = 7;
        {
            struct Task *u = gCurTask;
            u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 8;
            u->frame = 0xFFFF;
        }
        TaskYieldTrampoline(18);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpreadFacing(24, 1, 8) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0x10000, 0x4000);
            gCurTask->velY = 0;
            gCurTask->accelY = (RandomRange(36) - 24) << 8;
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 4; gCurTask->playerObjectLoopCount++)
            {
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
            }
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->posX = (RandomSpreadFacing(24, 1, 8) + (gCurTask->u8C.parentTask)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + (gCurTask->u8C.parentTask)->pixelY) << 16;
            TaskSetMotionXFacing(0x10000, 0x4000);
            gCurTask->velY = 0;
            gCurTask->accelY = (RandomRange(36) - 24) << 8;
            TaskSetFrameByFacing(14);
            TaskYieldTrampoline(2);
            for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 4; gCurTask->playerObjectLoopCount++)
            {
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
            }
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        t->drawCallback = (u32)TaskDrawWorld;
        t->frameTable = gUnk_08751CA4;
        t->layer = 5;
        {
            struct Task *u = gCurTask;
            u->facing = (u->u8C.parentTask)->facing;
        }
        {
            struct Task *u = gCurTask;
            u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 12;
        }
        do
        {
            {
                struct Task *u = gCurTask;
                if (u->facing == 1)
                {
                    u->posX = ((u->u8C.parentTask)->pixelX + 24) << 16;
                    u->posY = (u->u8C.parentTask)->pixelY << 16;
                }
                else
                {
                    u->posX = ((u->u8C.parentTask)->pixelX - 24) << 16;
                    u->posY = (u->u8C.parentTask)->pixelY << 16;
                }
            }
            TaskSetMotionXFacing(0x10000, (RandomRange(32) + 16) << 8);
            gCurTask->velY = 0;
            gCurTask->accelY = -(RandomRange(32) << 8);
            gCurTask->frame = 0;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    }
    TaskExitTrampoline();
}

void PlayerObjectFireBreathUpdate(void)
{
    {
        struct Task *t = gCurTask;
        if (!(t->player->unk40 & 0x100) && ((t->u8C.parentTask)->waterFlags & 1))
        {
            TaskFree(gCurTaskIdx);
            return;
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->player->ability != ABILITY_FIRE)
        {
            TaskFree(gCurTaskIdx);
            return;
        }
        if ((t->playerObjectSpawnWord & 15) == 1)
        {
            s32 dx;
            t->health = 127;
            RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873BDD4);
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CBDC, gCurTask->parent);
            t = gCurTask;
            dx = (t->facing == 1) ? 12 : -12;
            /* the ROM passes no 4th argument: r3 is whatever the last ldrsh left (0) */
            TaskBreakBlocksAt((struct HitBoxSet *)gUnk_0873CBDC, (t->u8C.parentTask)->pixelX + dx,
                         (t->u8C.parentTask)->pixelY + 2);
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->unk28 == 0)
        {
            if (t->player->mode != 13 || t->facing != (t->u8C.parentTask)->facing)
                t->unk28 = 1;
        }
    }
}

void PlayerObjectCutterBlade(void)
{
    {
        struct Task *t = gCurTask;
        if (t->frameTable == NULL)
        {
            t->moveCallback = (u32)TaskMove;
            t->drawCallback = (u32)TaskDrawWorldInView;
            t->updateCallback = (u32)PlayerObjectCutterBladeUpdate;
            t->layer = 5;
            {
                struct Task *u = gCurTask;
                u->frameTable = gUnk_08751A98;
                u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 8;
            }
            sub_0802205c(gUnk_0873CB54);
            gCurTask->u80.attackAbility = ABILITY_CUTTER;
            {
                struct Task *u = gCurTask;
                u->unk28 = 0;
                u->unk2C = 0;
            }
        }
    }
    {
        struct Task *t = gCurTask;
        switch (t->unk28)
        {
        case 0:
            if (t->facing == 1)
                t->posX = (t->pixelX + 8) << 16;
            else
                t->posX = (t->pixelX - 8) << 16;
            TaskSetMotionXFacing(0x68000, -0x5000);
            gCurTask->speedLimitX = 0x68000;
            while (1)
            {
                TaskSetFrameByFacing(0);
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
            }
        case 1:
            gCurTask->unk2C = 20;
            TaskStopX();
            TaskSetMotionXFacing(-0x15500, 0x5A5A5A5A);
            {
                struct Task *u = gCurTask;
                u->velY = -0x20000;
                u->accelY = 0x4000;
            }
            while (1)
            {
                TaskSetFrameByFacing(8);
                TaskYieldTrampoline(2);
                for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 6; gCurTask->playerObjectLoopCount++)
                {
                    gCurTask->frame += 2;
                    TaskYieldTrampoline(2);
                }
            }
        }
    }
    TaskExitTrampoline();
}

void PlayerObjectCutterBladeUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->player->ability != ABILITY_CUTTER)
    {
        TaskSetEntry(PlayerObjectVanish, gCurTaskIdx);
        return;
    }
    switch (t->unk28)
    {
    case 0:
        if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CBEC, t->parent))
            gCurTask->hitKind = 1;
        if (gCurTask->hitKind != 0)
        {
            TaskSetEntry(PlayerObjectVanish, gCurTaskIdx);
            return;
        }
        TerrainCollideBoxAlongVelocity(gUnk_0873CB54);
        if ((*(u32 *)&gTerrainResult & 0xFFFFFF) != 0)
        {
            {
                struct Task *u = gCurTask;
                if (u->velX > 0)
                    u->facing = 1;
                else
                    u->facing = -1;
            }
            gCurTask->unk28 = 1;
            TaskSetEntry(PlayerObjectCutterBlade, gCurTaskIdx);
            break;
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C == 0)
            {
                s8 f = u->facing;
                if ((f == 1 && u->velX < 0) || (f == -1 && u->velX > 0))
                {
                    struct Task *v = gCurTask;
                    v->drawCallback = (u32)TaskDrawWorldInViewOrFree;
                    v->unk2C++;
                }
                if (gCurTask->unk2C == 0)
                    break;
            }
        }
        {
            struct Task *u = gCurTask;
            struct Task *p = u->u8C.parentTask;
            if (p->pixelX + 8 > u->pixelX && u->pixelX > p->pixelX - 8
                && p->pixelY + 8 > u->pixelY && u->pixelY > p->pixelY - 8)
            {
                TaskFree(gCurTaskIdx);
                return;
            }
        }
        break;
    case 1:
        if (t->hitKind != 0)
        {
            TaskSetEntry(PlayerObjectVanish, gCurTaskIdx);
            return;
        }
        if (--t->unk2C == 0)
        {
            TaskFree(gCurTaskIdx);
            return;
        }
        if (t->unk2C <= 7)
        {
            if (gFrameCount & 1)
                t->frameTable = gUnk_08751A98;
            else
                t->frameTable = NULL;
        }
        break;
    }
    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BDE8);
}

void PlayerObjectLaserBeam(void)
{
    u16 *e;

    if (gCurTask->frameTable == NULL)
    {
        {
            struct Task *t = gCurTask;
            t->moveCallback = (u32)TaskMove;
            t->drawCallback = (u32)TaskDrawWorldInViewOrFree;
            t->layer = 7;
        }
        {
            struct Task *t = gCurTask;
            t->frameTable = gPlayerObjectLaserBeamFrames;
            t->tileWord = ((t->u8C.parentTask)->tileWord + 0x800) | 12;
            if (t->facing == 1)
            {
                t->posX = ((t->u8C.parentTask)->pixelX + 6) << 16;
                t->unk28 = 1;
            }
            else
            {
                t->posX = ((t->u8C.parentTask)->pixelX - 6) << 16;
                t->unk28 = 3;
            }
        }
        {
            struct Task *t = gCurTask;
            t->posY = ((t->u8C.parentTask)->pixelY + 2) << 16;
            t->unk2C = 3;
        }
        TaskInitWaterFlags();
        sub_0802233c(gUnk_0873CB5C);
        gCurTask->u80.attackAbility = ABILITY_LASER;
    }
    else
    {
        gCurTask->updateCallback = 0;
        TaskStop();
        gCurTask->frame = 16;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->onGround = 0;
    }
    {
        struct Task *t = gCurTask;
        s32 v;
        e = gUnk_0873B7B0[t->unk28];
        if (t->unk28 == 1 || t->unk28 == 3)
        {
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
        }
        else
        {
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->velY = v;
        }
    }
    gCurTask->updateCallback = (u32)PlayerObjectLaserBeamUpdate;
    while (1)
    {
        gCurTask->frame = e[0];
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
    }
}

void PlayerObjectLaserBeamUpdate(void)
{
    s32 hit;

    if (gCurTask->player->ability == ABILITY_NORMAL)
        goto rebind;
    hit = 0;
    gTerrainResult.unk2 = 0;
    switch (gCurTask->unk28)
    {
    case 1:
    case 3:
        if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CBFC, gCurTask->parent))
        {
            gCurTask->hitKind = 1;
            hit = 1;
        }
        else
            TerrainCollideBoxTileEdge(gUnk_0873CB5C);
        break;
    case 0:
    case 2:
        if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CC0C, gCurTask->parent))
        {
            gCurTask->hitKind = 1;
            hit = 1;
        }
        else
            TerrainCollideBoxTileEdge(gUnk_0873CB64);
        break;
    }
    if (gTerrainResult.slope != 0)
    {
        struct Task *t = gCurTask;
        if (t->unk2C-- == 0)
            t->hitKind = 1;
        else if (t->player->ability == ABILITY_LASER)
            TaskSetEntry(PlayerObjectLaserBeam, gCurTaskIdx);
        else
            TaskSetEntry(PlayerObjectUFOShot, gCurTaskIdx);
        hit = 1;
    }
    else if (*(u32 *)&gTerrainResult & 0xFFFFFF)
    {
        gCurTask->hitKind = 1;
        hit = 1;
    }
    if (hit != 0)
    {
        {
            struct Task *t = gCurTask;
            if (t->player->ability == ABILITY_LASER)
            {
                if (!(t->player->unk42 & 128))
                    PlaySfxIfLocalPlayer(173, t->parent);
            }
            else
            {
                if (!(t->player->unk42 & 128))
                    PlaySfxIfLocalPlayer(211, t->parent);
            }
        }
        switch (gCurTask->unk28)
        {
        case 1:
        case 3:
            gCurTask->unk24 = (s32)gUnk_0873BDFC;
            break;
        case 0:
        case 2:
            gCurTask->unk24 = (s32)gUnk_0873BE10;
            break;
        }
    }
    if (gCurTask->hitKind != 0)
    {
    rebind:
        TaskSetEntry(PlayerObjectLaserBeamVanish, gCurTaskIdx);
        return;
    }
    switch (gTerrainResult.slope)
    {
    case 0:
        switch (gCurTask->unk28)
        {
        case 1:
        case 3:
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BDFC);
            break;
        case 0:
        case 2:
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE10);
            break;
        }
        break;
    case 1:
    case 3:
        if (gCurTask->unk28 == 1)
            gCurTask->unk28 = 0;
        else if (gCurTask->unk28 == 2)
            gCurTask->unk28 = 3;
        break;
    case 2:
    case 4:
        if (gCurTask->unk28 == 3)
            gCurTask->unk28 = 0;
        else if (gCurTask->unk28 == 2)
            gCurTask->unk28 = 1;
        break;
    case 6:
    case 8:
        if (gCurTask->unk28 == 3)
            gCurTask->unk28 = 2;
        else if (gCurTask->unk28 == 0)
            gCurTask->unk28 = 1;
        break;
    case 5:
    case 7:
        if (gCurTask->unk28 == 1)
            gCurTask->unk28 = 2;
        else if (gCurTask->unk28 == 0)
            gCurTask->unk28 = 3;
        break;
    }
}
