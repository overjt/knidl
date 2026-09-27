#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"

/* effect_56dd4.c (0x08056DD4-0x08057493, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 29-31.  Variant 29 (sub_08056dd4, M11/M13) has three sub-states
 * with random frames (RandomRange); its callback sub_0805707c sets
 * Task.unk28 once the player leaves mode 13 or its facing no longer matches
 * the spawner's, and kills it when the ability is no longer 1 or when
 * PlayerState.unk40 bit 8 is clear while the spawner's Task.waterFlags bit 0 is
 * set.  Variant 30 (sub_0805710c, M11/M13) rides on its spawner with the
 * draw hook TaskDrawWorldLoadTiles (sub-state 0) or TaskDrawWorldTilesLoaded and re-rolls its
 * position every two frames from the {base, scale, amount} rows
 * gUnk_0873BA8C[][2][3]; its callback sub_080573a4 kills it when the player
 * leaves mode 13 or the spawner's Task.variant is not 1, and otherwise, while
 * Task.unk28 is clear, registers the collider row gUnk_0873C038 (M05's
 * RegisterCollider) and tests the block hit-box set gUnk_0873CC94 (M09's
 * TaskBreakBlocksAt) at the spawner's position.  Variant 31 (sub_08057430, M12)
 * is a single animation on its spawner (gUnk_08751CEC). */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y, s32 e);

void sub_08056dd4(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)sub_0805707c;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751CA4;
    t->tileWord = (((struct Task *)t->unk8C)->tileWord + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->frame = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gCurTask->posX = (RandomSpreadFacing(16, 1, 32) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 8) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
            TaskSetMotionXFacing(0x18000, -0x800);
            gCurTask->velY = 0;
            gCurTask->accelY = -0x2000;
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
            TaskYieldTrampoline(4);
        } while (gCurTask->unk28 == 0);
        break;
    case 1:
        t->frame = 0xFFFF;
        TaskYieldTrampoline(8);
        do
        {
            gCurTask->posX = (RandomSpreadFacing(32, 1, 8) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
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
        t->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gCurTask->posX = (RandomSpreadFacing(20, 1, 12) + ((struct Task *)gCurTask->unk8C)->pixelX) << 16;
            gCurTask->posY = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->pixelY) << 16;
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

void sub_0805707c(void)
{
    {
        struct Task *t = gCurTask;

        if (t->unk28 == 0 && (t->player->mode != 13 || t->facing != ((struct Task *)t->unk8C)->facing))
            t->unk28 = 1;
    }
    {
        struct Task *t = gCurTask;

        if (!(t->player->unk40 & 0x100) && (((struct Task *)t->unk8C)->waterFlags & 1))
            TaskFree(gCurTaskIdx);
    }
    if (gCurTask->player->ability != 1)
        TaskFree(gCurTaskIdx);
}

void sub_0805710c(void)
{
    struct Task *t;
    s16 *x;
    s16 *y;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->updateCallback = (u32)sub_080573a4;
    gCurTask->layer = 8;
    t = gCurTask;
    t->frameTable = gUnk_08751CBC;
    t->tileWord = (((struct Task *)t->unk8C)->tileWord + 0x800) | 12;
    if ((t->unk28 = t->unk18 & 15) == 0)
        t->drawCallback = (u32)TaskDrawWorldLoadTiles;
    else
        t->drawCallback = (u32)TaskDrawWorldTilesLoaded;
    x = gUnk_0873BA8C[gCurTask->unk28][0];
    y = gUnk_0873BA8C[gCurTask->unk28][1];
    for (;;)
    {
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(1);
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->posX = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->posY = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
    }
}

void sub_080573a4(void)
{
    struct Task *t = gCurTask;
    struct Task *p;

    if (t->player->mode != 13 || (p = (struct Task *)t->unk8C)->variant != 1)
    {
        TaskFree(gCurTaskIdx);
    }
    else if (t->unk28 == 0)
    {
        RegisterCollider(gCurTaskIdx, p->pixelX, p->pixelY, gUnk_0873C038);
        TaskBreakBlocksAt((struct HitBoxSet *)gUnk_0873CC94, ((struct Task *)gCurTask->unk8C)->pixelX,
                     ((struct Task *)gCurTask->unk8C)->pixelY, gCurTask->parent);
    }
}

void sub_08057430(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_08751CEC;
    gCurTask->posX = RandomSpreadFacing(-32, 1, 16) << 16;
    gCurTask->posY = RandomSpread(-4, 1, 16) << 16;
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}
