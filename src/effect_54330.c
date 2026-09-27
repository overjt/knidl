#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "collision.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* effect_54330.c (0x08054330-0x08054A7F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 7-11.  Variants 7 and 8 (sub_08054330, sub_08054538; spawned by
 * M09-M11's movement code) are the two-sub-state puff of variant 6:
 * sub-state 0 places puffs 6 pixels behind the spawner and 8 below it (7 up
 * to three rounds, until its companion sub_08054504 sets Task.unk28 when the
 * player leaves mode 2 or the spawner's Task.onGround clears; 8 three puffs at
 * decreasing speeds, killed by sub_08054838 once the player leaves mode 7),
 * each spawning its own sub-state 1, a small rising puff.  Variants 9-11 are
 * M09's player task effects: 9 (PlayerEffectSplash) and 10 (sub_080548f0) play
 * short animations from gUnk_0874C520, 9 first calling M11's
 * PlaySfxIfLocalPlayer(134, ...) when the spawner is moving down (Task.velY > 0)
 * and 10 placed at the height the spawner passes in the low half of
 * Task.unk18; 11 (PlayerEffectBubble) asks M07's IsFullBlockAtPixel about its position
 * and, when that returns 0, rises while swaying left and right, until its
 * companion sub_08054a44 (the collision box gUnk_0873CB74 through
 * sub_0801c3a4) kills it once its Task.waterFlags is clear or gTerrainResult.unk1
 * is set. */

void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
u16 sub_0801c3a4(const s8 *p);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */

void sub_08054330(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *p;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        t->updateCallback = (u32)sub_08054504;
        t->unk28 = 0;
        t->unk6C = 0;
        while (t->unk28 == 0)
        {
            u = gCurTask;
            if (u->facing == 1)
                u->posX = ((p = (struct Task *)u->unk8C)->pixelX - 6) << 16;
            else
                u->posX = ((p = (struct Task *)u->unk8C)->pixelX + 6) << 16;
            u->posY = (((struct Task *)u->unk8C)->pixelY + 8) << 16;
            TaskStop();
            gCurTask->accelY = -0x2000;
            TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 7, 1);
            TaskSetMotionXFacing(0x5A5A5A5A, 0);
            TaskYieldTrampoline(1);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(2);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            t = gCurTask;
            t->unk6C++;
            if ((s16)t->unk6C > 2)
                break;
        }
        break;
    case 1:
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

void sub_08054504(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && (t->player->mode != 2 || ((struct Task *)t->unk8C)->onGround == 0))
        t->unk28 = 1;
}

void sub_08054538(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        t->updateCallback = (u32)sub_08054838;
        if (t->facing == 1)
            t->posX = ((p = (struct Task *)t->unk8C)->pixelX - 6) << 16;
        else
            t->posX = ((p = (struct Task *)t->unk8C)->pixelX + 6) << 16;
        t->posY = (((struct Task *)t->unk8C)->pixelY + 8) << 16;
        TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
        gCurTask->accelY = -0x2000;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 8, 1);
        TaskYieldTrampoline(1);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        u = gCurTask;
        if (u->facing == 1)
            u->posX = ((p = (struct Task *)u->unk8C)->pixelX - 6) << 16;
        else
            u->posX = ((p = (struct Task *)u->unk8C)->pixelX + 6) << 16;
        u->posY = (((struct Task *)u->unk8C)->pixelY + 8) << 16;
        TaskSetMotionXFacing(-0x24000, 0x1000);
        gCurTask->velY = -0x4000;
        gCurTask->accelY = -0x2000;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        v = gCurTask;
        if (v->facing == 1)
            v->posX = ((p = (struct Task *)v->unk8C)->pixelX - 6) << 16;
        else
            v->posX = ((p = (struct Task *)v->unk8C)->pixelX + 6) << 16;
        v->posY = (((struct Task *)v->unk8C)->pixelY + 8) << 16;
        TaskSetMotionXFacing(-0x12000, 0x1800);
        gCurTask->velY = -0x4000;
        gCurTask->accelY = -0x2000;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        break;
    case 1:
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

void sub_08054838(void)
{
    if (gCurTask->player->mode != 7)
        TaskFree(gCurTaskIdx);
}

void PlayerEffectSplash(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C520;
    if (((struct Task *)t->unk8C)->velY > 0)
        PlaySfxIfLocalPlayer(134, t->parent);
    u = gCurTask;
    u->posX = ((struct Task *)u->unk8C)->pixelX << 16;
    u->posY = (u16)u->unk18 << 16;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    TaskExitTrampoline();
}

void sub_080548f0(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C520;
    t->posX = ((struct Task *)t->unk8C)->pixelX << 16;
    t->posY = (u16)t->unk18 << 16;
    t->frame = 11;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void PlayerEffectBubble(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorldInViewOrFree;
    gCurTask->updateCallback = (u32)sub_08054a44;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C648;
    if (t->facing == 1)
        t->posX = (t->pixelX + 4) << 16;
    if (IsFullBlockAtPixel(gCurTask->posX >> 16, gCurTask->posY >> 10) == 0)
    {
        gCurTask->velY = -0x10000;
        for (;;)
        {
            gCurTask->velX = -0x10000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(6);
            gCurTask->velX = 0x10000;
            gCurTask->frame++;
            TaskYieldTrampoline(6);
        }
    }
    TaskExitTrampoline();
}

void sub_08054a44(void)
{
    sub_0801c3a4(gUnk_0873CB74);
    if (gCurTask->waterFlags == 0 || gTerrainResult.ceilingHits != 0)
        TaskFree(gCurTaskIdx);
}
