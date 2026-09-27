#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"

/* effect_55b24.c (0x08055B24-0x08056447, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 22-25, spawned by M10's action 16 (PlayerActionHurt) and by M11.
 * Each is a loop of short animations around its spawner (22-24 at random
 * offsets from RandomSpread/RandomSpreadFacing; tables gUnk_0874C718,
 * gUnk_0874C7A4, gUnk_0874C7B4, gUnk_0874C7CC) that ends once its companion
 * sets Task.unk28: sub_08055d24, sub_080560fc, sub_08056300 and sub_08056428
 * do so when the player leaves mode 17 (in sub-state 0 of 22 and 23 also
 * when the spawner's Task.onGround is set).  Variant 24 (sub_0805614c) sets its
 * velocities with TaskSetMotion and alternates two directions; variant 25
 * (sub_08056320) stays on the spawner's position, with Task.facing = 1 when
 * gFrameCount bit 0 is set and the inherited facing flipped otherwise;
 * variant 23 (sub_08055d74, 904 bytes) is the longest. */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void TaskMove(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
void sub_08055d24(void);
void sub_080560fc(void);
void sub_08056300(void);
void sub_08056428(void);

void sub_08055b24(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)sub_08055d24;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C718;
    TaskStop();
    gCurTask->unk28 = 0;
    if ((gCurTask->unk18 & 15) == 0)
    {
        do
        {
            TaskSetMotionXFacing(0, -0x2000);
            gCurTask->velY = 0;
            gCurTask->accelY = -0x2000;
            gCurTask->posX = (RandomSpreadFacing(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(3);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
    }
    else
    {
        do
        {
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->velY = -0x8000;
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
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(4);
        } while (gCurTask->unk28 == 0);
    }
    TaskExitTrampoline();
}

void sub_08055d24(void)
{
    struct Task *t = gCurTask;

    if ((t->unk18 & 15) == 0)
    {
        if (t->unk28 == 0 && (t->player->mode != 17 || ((struct Task *)t->unk8C)->onGround != 0))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && t->player->mode != 17)
            t->unk28 = 1;
    }
}

void sub_08055d74(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)sub_080560fc;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C7A4;
    t->unk28 = 0;
    TaskStop();
    if ((gCurTask->unk18 & 15) == 0)
    {
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpreadFacing(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 0;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 3;
            TaskYieldTrampoline(2);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 2;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(4);
            gCurTask->posX = (RandomSpreadFacing(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 2;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 1;
            TaskYieldTrampoline(2);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 0;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(4);
        }
    }
    else
    {
        while (gCurTask->unk28 == 0)
        {
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(4);
            gCurTask->posX = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            gCurTask->frame = 3;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->frame = 0xFFFF;
            TaskYieldTrampoline(2);
        }
    }
    TaskExitTrampoline();
}

void sub_080560fc(void)
{
    struct Task *t = gCurTask;

    if ((t->unk18 & 15) == 0)
    {
        if (t->unk28 == 0 && (t->player->mode != 17 || ((struct Task *)t->unk8C)->onGround != 0))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && t->player->mode != 17)
            t->unk28 = 1;
    }
}

void sub_0805614c(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)sub_08056300;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C7B4;
    t->unk28 = 0;
    TaskStop();
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1 && gCurTask->unk28 == 0; gCurTask->unk6C++)
    {
        gCurTask->posX = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
        gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
        TaskSetMotion(0x4000, -0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 4);
        if (gCurTask->unk28 != 0)
            break;
        gCurTask->posX = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
        gCurTask->posY = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
        TaskSetMotion(-0x4000, 0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 4);
    }
    TaskExitTrampoline();
}

void sub_08056300(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && t->player->mode != 17)
        t->unk28 = 1;
}

void sub_08056320(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)sub_08056428;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C7CC;
    t->unk28 = 0;
    TaskStop();
    if (gFrameCount & 1)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -gCurTask->facing;
    gCurTask->velY = -0x8000;
    while (gCurTask->unk28 == 0)
    {
        struct Task *u = gCurTask;

        u->posX = ((struct Task *)u->unk8C)->pixelX << 16;
        u->posY = ((struct Task *)u->unk8C)->pixelY << 16;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(3);
        gCurTask->frame += 2;
        TaskYieldTrampoline(3);
        gCurTask->frame += 2;
        TaskYieldTrampoline(3);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_08056428(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && t->player->mode != 17)
        t->unk28 = 1;
}
