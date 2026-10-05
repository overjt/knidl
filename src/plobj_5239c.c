#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"

/* plobj_5239c.c (0x0805239C-0x08052F6B, issue #90).
 *
 * Task type #6, variants 7-9, each variant body followed by the callbacks
 * only it installs.  Variant 7 (PlayerObjectIceBreath, animation tables
 * gPlayerObjectIceBreathFrames/gIceBreathCloudFrames) switches on the sub-state Task.unk18 & 15
 * and installs M11's PlayerDrawWorldLoadTilesAndPalette and its own collision callback
 * PlayerObjectIceBreathUpdate, which registers the collider row gUnk_0873BE24 and runs the
 * hit test TaskBreakBlocksAt(gUnk_0873CC1C) at the spawner's position (Task.u8C.parentTask).
 * Variant 8 (PlayerObjectBeamOrb, gPlayerObjectBeamOrbFrames) traces six-step paths from the
 * 8.8 velocity rows gUnk_0873B7C0[k] and the animation rows
 * gUnk_0873B808[k] (sound 129) with the callback PlayerObjectBeamOrbUpdate (collider row
 * gUnk_0873BE38, hit test gUnk_0873CC2C), which variant 10
 * (src/plobj_52f6c.c) installs too; both end in a `pop {r1}` epilogue
 * without setting r0, so they are declared s32 with no return.  Variant 9
 * (PlayerObjectLightOrb, gPlayerObjectLightOrbFrames) is a copy of the spawner's sprite:
 * sub-state 0 queues the tiles gUnk_08204B98 (four 320-byte rows) and the
 * palette gUnk_08204B78 into the spawner's OBJ slots through the VRAM
 * transfer queue RequestCopy, blinks, flies to the top centre of the
 * screen over 30 frames (Div) leaving sub-state-1 sparkles of itself,
 * plays sounds 175 and 176 and calls M07's race-record hook sub_08027588;
 * sub-state 1 is the three-frame sparkle. */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
u16 RandomSpread(u16 base, u8 scale, u8 amount);
s16 RandomSpreadFacing(u16 base, u8 scale, u8 amount);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y, s32 e);
s32 TaskBreakBlocks(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/PlayerObjectAirPuffUpdate.c); landed M09/M12/M13 files spell it u16 */

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg);
s32 PlayerObjectBeamOrbUpdate(void);   /* returns a value: pop {r1} epilogue; plobj_52f6c.c spells it void */

void PlayerObjectIceBreath(void)
{
    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMove;
        t->updateCallback = (u32)PlayerObjectIceBreathUpdate;
        t->playerObjectBreathStopped = 0;
        t->u80.attackAbility = ABILITY_ICE;
    }
    {
        struct Task *t = gCurTask;
        switch (t->playerObjectSpawnWord & 15)
        {
        case 0:
            t->drawCallback = (u32)PlayerDrawWorldLoadTilesAndPalette;
            t->frameTable = gPlayerObjectIceBreathFrames;
            t->layer = 7;
            {
                struct Task *u = gCurTask;
                u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 12;
                u->frame = 0xFFFF;
            }
            TaskYieldTrampoline(8);
            while (gCurTask->playerObjectBreathStopped == 0)
            {
                {
                    s16 d = RandomSpreadFacing(24, 1, 8);
                    struct Task *u = gCurTask;
                    u->posX = (d + (u->u8C.parentTask)->pixelX) << 16;
                }
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
                if (gCurTask->playerObjectBreathStopped != 0)
                    break;
                {
                    s16 d = RandomSpreadFacing(24, 1, 8);
                    struct Task *u = gCurTask;
                    u->posX = (d + (u->u8C.parentTask)->pixelX) << 16;
                }
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
            t->frameTable = gIceBreathCloudFrames;
            t->layer = 7;
            gCurTask->facing = (gCurTask->u8C.parentTask)->facing;
            {
                struct Task *u = gCurTask;
                u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 8;
            }
            do
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
                TaskSetMotionXFacing(0x10000, (RandomRange(32) + 16) << 8);
                gCurTask->velY = 0;
                {
                    s32 r = RandomRange(32);
                    struct Task *v = gCurTask;
                    v->accelY = -(r << 8);
                    v->frame = 0;
                }
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
            } while (gCurTask->playerObjectBreathStopped == 0);
            break;
        }
    }
    TaskExitTrampoline();
}

void PlayerObjectIceBreathUpdate(void)
{
    struct Task *t = gCurTask;

    if (!(t->player->actionFlags & PLAYER_ACTION_FLAG_GET_ABILITY) && ((t->u8C.parentTask)->waterFlags & 1))
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    if (gCurTask->player->ability != ABILITY_ICE)
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    {
        struct Task *u = gCurTask;
        if ((u->playerObjectSpawnWord & 15) == 1)
        {
            u->health = 127;
            RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BE24);
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CC1C, gCurTask->parent);
            {
                struct Task *v = gCurTask;
                s32 off;
                if (v->facing == 1)
                    off = 12;
                else
                    off = -12;
                TaskBreakBlocksAt((struct HitBoxSet *)gUnk_0873CC1C, (v->u8C.parentTask)->pixelX + off,
                             (v->u8C.parentTask)->pixelY + 2, v->parent);
            }
        }
    }
    {
        struct Task *u = gCurTask;
        if (u->playerObjectBreathStopped == 0)
        {
            if (u->player->mode != 13 || u->facing != (u->u8C.parentTask)->facing)
                u->playerObjectBreathStopped = 1;
        }
    }
}

s32 PlayerObjectBeamOrb(void)
{
    u16 *xs;
    u16 *ys;
    u8 *e;

    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)TaskDrawWorld;
        t->updateCallback = (u32)PlayerObjectBeamOrbUpdate;
        t->layer = 5;
    }
    {
        struct Task *t = gCurTask;
        t->frameTable = gPlayerObjectBeamOrbFrames;
        t->u80.attackAbility = ABILITY_BEAM;
    }
    {
        struct Task *t = gCurTask;
        t->tileWord = (t->u8C.parentTask)->tileWord | 0xF008;
        if (t->facing == 1)
            t->unk28 = 14;
        else
            t->unk28 = -14;
    }
    {
        struct Task *t = gCurTask;
        t->unk2C = 2;
        switch (t->playerObjectSpawnWord & 15)
        {
        case 0:
            if (!(t->player->statusFlags & PLAYER_STATUS_NO_ATTACK_SFX))
                PlaySfxIfLocalPlayer(129, t->parent);
            xs = gUnk_0873B7C0[0][0];
            ys = gUnk_0873B7C0[0][1];
            for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 5; gCurTask->playerObjectLoopCount++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->posX = u->unk28 << 16;
                    u->posY = u->unk2C << 16;
                    v = xs[(s16)u->playerObjectLoopCount] << 8;
                    if (xs[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->playerObjectLoopCount] << 8;
                    if (ys[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                    u->velY = v;
                    e = gUnk_0873B808[0][(s16)u->playerObjectLoopCount];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->frame = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->velX = 0;
                    u->velY = 0;
                }
            }
            break;
        case 1:
            t->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            xs = gUnk_0873B7C0[1][0];
            ys = gUnk_0873B7C0[1][1];
            for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 5; gCurTask->playerObjectLoopCount++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->posX = u->unk28 << 16;
                    u->posY = u->unk2C << 16;
                    v = xs[(s16)u->playerObjectLoopCount] << 8;
                    if (xs[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->playerObjectLoopCount] << 8;
                    if (ys[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                    u->velY = v;
                    e = gUnk_0873B808[1][(s16)u->playerObjectLoopCount];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->frame = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->velX = 0;
                    u->velY = 0;
                }
            }
            break;
        case 2:
            t->frame = 0xFFFF;
            TaskYieldTrampoline(2);
            xs = gUnk_0873B7C0[2][0];
            ys = gUnk_0873B7C0[2][1];
            for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 5; gCurTask->playerObjectLoopCount++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->posX = u->unk28 << 16;
                    u->posY = u->unk2C << 16;
                    v = xs[(s16)u->playerObjectLoopCount] << 8;
                    if (xs[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->playerObjectLoopCount] << 8;
                    if (ys[(s16)u->playerObjectLoopCount] & 0x8000)
                        v |= 0xFF000000;
                    u->velY = v;
                    e = gUnk_0873B808[2][(s16)u->playerObjectLoopCount];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->frame = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->velX = 0;
                    u->velY = 0;
                }
            }
            break;
        }
    }
    TaskExitTrampoline();
}

s32 PlayerObjectBeamOrbUpdate(void)
{
    struct Task *t = gCurTask;

    t->health = 127;
    if (t->player->mode != 13)
    {
        TaskFree(gCurTaskIdx);
    }
    else if ((t->playerObjectSpawnWord & 15) == 0)
    {
        if (t->frame != -1)
            RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873BE38);
        TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CC2C, gCurTask->parent);
    }
}

void PlayerObjectLightOrb(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->frameTable = gPlayerObjectLightOrbFrames;
    switch (t->playerObjectSpawnWord & 15)
    {
    case 0:
        t->layer = 5;
        {
            u32 off = ((gCurTask->u8C.parentTask)->tileWord & 0xFFF) << 5;

            RequestCopy(1, gUnk_08204B98, (void *)(off + (OBJ_VRAM0 + 0x80)), 320);
            RequestCopy(1, gUnk_08204B98 + 320, (void *)(off + (OBJ_VRAM0 + 0x480)), 320);
            RequestCopy(1, gUnk_08204B98 + 640, (void *)(off + (OBJ_VRAM0 + 0x880)), 320);
            RequestCopy(1, gUnk_08204B98 + 960, (void *)(off + (OBJ_VRAM0 + 0xC80)), 320);
        }
        RequestCopy(2, gUnk_08204B78,
                     gObjPalette + ((((gCurTask->u8C.parentTask)->tileWord >> 12) + 1) << 5), 32);
        {
            struct Task *u = gCurTask;

            u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 4;
            if (u->facing == 1)
                u->posX = (u->pixelX - gSpriteCameraX + 3) << 16;
            else
                u->posX = (u->pixelX - gSpriteCameraX - 3) << 16;
            u->posY = (u->pixelY - gSpriteCameraY) << 16;
        }
        PlaySfx(175);
        gCurTask->velY = 0x6000;
        for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 1; gCurTask->playerObjectLoopCount++)
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 3; gCurTask->playerObjectLoopCount++)
        {
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        }
        gCurTask->velX = Div((120 - gCurTask->pixelX) << 16, 30);
        gCurTask->velY = Div(-gCurTask->pixelY << 16, 30);
        for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 14; gCurTask->playerObjectLoopCount++)
        {
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_LIGHT_ORB, 1);
        }
        PlaySfx(176);
        sub_08027588();
        {
            struct Task *u = gCurTask;

            u->drawCallback = (u32)TaskDrawWorld;
            u->posX = (gSpriteCameraX + 120) << 16;
            u->posY = gSpriteCameraY << 16;
        }
        TaskStop();
        gCurTask->accelY = 0x400;
        gCurTask->frame = 3;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        for (gCurTask->playerObjectLoopCount = 0; (s16)gCurTask->playerObjectLoopCount <= 3; gCurTask->playerObjectLoopCount++)
        {
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        t->layer = 8;
        {
            struct Task *u = gCurTask;

            u->tileWord = gTasks[u->parent].tileWord;
            u->frame = 14;
        }
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        break;
    }
    TaskExitTrampoline();
}
