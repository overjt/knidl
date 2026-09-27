#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "effect.h"

/* effect_56448.c (0x08056448-0x08056DD3, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 26-28.  Variant 26 (sub_08056448, M10) is a two-step animation
 * from gUnk_0874C930.  Variant 27 (sub_080564ac, M13) has three sub-states
 * (animation table gUnk_0874C828) and spawns its own sub-states.  Variant 28
 * (sub_08056770) is the most common ability effect (twenty call sites in
 * M11-M14): six sub-states over a jump table, some riding on the spawner and
 * some in world space, respawning variant 28 in other sub-states; its
 * per-frame callback sub_08056da8 sets Task.unk28 in sub-state 4 once the
 * player leaves mode 13. */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08056da8(void);

void sub_08056448(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C930;
    t->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    TaskExitTrampoline();
}

void sub_080564ac(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C828;
    switch (t->unk18 & 15)
    {
    case 0:
        t->posY = (t->pixelY + 4) << 16;
        t->frame = 16;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 27, 2);
        {
            struct Task *u = gCurTask;

            u->waterFlags = ((struct Task *)u->unk8C)->waterFlags;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 6);
        break;
    case 1:
        if (!(((struct Task *)t->unk8C)->waterFlags & 1))
        {
            t->velX = 0x60000;
            t->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->velX = 0x20000;
            gCurTask->accelX = -0x1000;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
        }
        else
        {
            t->velX = 0x30000;
            t->frame = 1;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->velX = 0x10000;
            gCurTask->accelX = -0x800;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
        break;
    case 2:
        if (!(((struct Task *)t->unk8C)->waterFlags & 1))
        {
            t->velX = -0x60000;
            t->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0x20000;
            gCurTask->accelX = 0x1000;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
        }
        else
        {
            t->velX = -0x30000;
            t->frame = 0;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->velX = -0x10000;
            gCurTask->accelX = 0x800;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        }
        break;
    }
    TaskExitTrampoline();
}

void sub_08056770(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        CreatePlayerEffect(gCurTask->player->playerIndex, 28, 1);
        while ((s8)gCurTask->player->unk16 == 0 && gCurTask->player->mode == 13)
        {
            {
                struct Task *u = gCurTask;
                struct Task *p = (struct Task *)u->unk8C;

                if (p->onGround != 0)
                {
                    u->facing = -p->facing;
                    {
                        struct Task *v = gCurTask;

                        v->posY = (((struct Task *)v->unk8C)->pixelY + 6) << 16;
                        if (v->facing == 1)
                            v->posX = (((struct Task *)v->unk8C)->pixelX + 8) << 16;
                        else
                            v->posX = (((struct Task *)v->unk8C)->pixelX - 8) << 16;
                    }
                    TaskStop();
                    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
                    {
                        struct Task *w = gCurTask;

                        w->accelY = -0x2000;
                        if (w->facing == 1)
                        {
                            w->frame = 1;
                            TaskYieldTrampoline(2);
                        }
                        else
                        {
                            w->frame = 0;
                            TaskYieldTrampoline(2);
                        }
                    }
                    gCurTask->frame += 2;
                    TaskYieldTrampoline(2);
                    gCurTask->frame += 2;
                    TaskYieldTrampoline(1);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 28, 2);
                    TaskYieldTrampoline(1);
                    gCurTask->frame -= 2;
                    TaskYieldTrampoline(2);
                    gCurTask->frame -= 2;
                    TaskYieldTrampoline(1);
                }
            }
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        {
            struct Task *u = gCurTask;

            u->unk8C = ((struct Task *)u->unk8C)->unk8C;
            u->parent = ((struct Task *)u->unk8C)->parent;
            u->moveCallback = (u32)TaskMoveRelativeToParent;
        }
        while ((s8)gCurTask->player->unk16 == 0 && gCurTask->player->mode == 13)
        {
            struct Task *u = gCurTask;
            struct Task *p = (struct Task *)u->unk8C;

            if (p->onGround == 0)
            {
                u->frame = 0xFFFF;
                TaskYieldTrampoline(1);
            }
            else
            {
                u->facing = -p->facing;
                gCurTask->posX = RandomSpreadFacing(-8, 1, 16) << 16;
                gCurTask->posY = (RandomSpread(0, 1, 8) << 16) + 0x80000;
                {
                    struct Task *w = gCurTask;

                    w->accelX = 0;
                    w->accelY = 0;
                    if (w->facing == 1)
                    {
                        w->velX = -0x20000;
                        w->velY = -0x18000;
                        w->frame = 10;
                        TaskYieldTrampoline(3);
                        gCurTask->velX = -0x20000;
                        gCurTask->accelX = 0x10000;
                        gCurTask->frame = 0;
                        TaskYieldTrampoline(3);
                        gCurTask->velX = 0x20000;
                        gCurTask->accelX = 0x800;
                        gCurTask->frame++;
                        TaskYieldTrampoline(2);
                    }
                    else
                    {
                        w->velX = 0x20000;
                        w->velY = -0x18000;
                        w->frame = 11;
                        TaskYieldTrampoline(3);
                        gCurTask->velX = 0x20000;
                        gCurTask->accelX = -0x10000;
                        gCurTask->frame = 1;
                        TaskYieldTrampoline(3);
                        gCurTask->velX = -0x20000;
                        gCurTask->accelX = -0x800;
                        gCurTask->frame--;
                        TaskYieldTrampoline(2);
                    }
                }
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
                gCurTask->velY = -0x10000;
                gCurTask->frame += 4;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
            }
        }
        break;
    case 2:
        gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
        gCurTask->posX = RandomSpreadFacing(-8, 1, 8) << 16;
        gCurTask->posY = RandomSpread(-8, 1, 8) << 16;
        TaskSetMotionXFacing(0x5A5A5A5A, -0x4000);
        {
            struct Task *u = gCurTask;

            u->accelY = -0x4000;
            if (u->facing == 1)
            {
                u->frame = 5;
                TaskYieldTrampoline(2);
            }
            else
            {
                u->frame = 4;
                TaskYieldTrampoline(2);
            }
        }
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        break;
    case 3:
        {
            struct Task *u = gCurTask;

            u->posY = (u->pixelY + 6) << 16;
            if (u->facing == 1)
                u->posX = (u->pixelX - 8) << 16;
            else
                u->posX = (u->pixelX + 8) << 16;
        }
        if (!(((struct Task *)gCurTask->unk8C)->waterFlags & 1))
        {
            TaskSetMotionXFacing(-0x24000, 0x1800);
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
        }
        else
        {
            TaskSetMotionXFacing(-0x12000, 0xC00);
            gCurTask->velY = -0x2000;
            gCurTask->accelY = -0x1000;
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            TaskSetFrameByFacing(6);
            TaskYieldTrampoline(4);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
        }
        break;
    case 4:
        {
            struct Task *u = gCurTask;

            u->unk28 = 0;
            u->updateCallback = (u32)sub_08056da8;
        }
        do
        {
            {
                struct Task *u = gCurTask;

                u->posY = (((struct Task *)u->unk8C)->pixelY + 10) << 16;
                if (u->facing == 1)
                    u->posX = (((struct Task *)u->unk8C)->pixelX - 8) << 16;
                else
                    u->posX = (((struct Task *)u->unk8C)->pixelX + 8) << 16;
            }
            TaskStop();
            TaskSetMotionXFacing(-0x30000, 0x6000);
            gCurTask->velY = -0x20000;
            gCurTask->accelY = 0x4000;
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(4);
            TaskSetFrameByFacing(10);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
        } while (gCurTask->unk28 == 0);
        break;
    case 5:
        {
            struct Task *u = gCurTask;

            u->posY = (((struct Task *)u->unk8C)->pixelY + 10) << 16;
            if (u->facing == 1)
                u->posX = (((struct Task *)u->unk8C)->pixelX - 8) << 16;
            else
                u->posX = (((struct Task *)u->unk8C)->pixelX + 8) << 16;
        }
        TaskSetMotionXFacing(-0x20000, -0x800);
        gCurTask->velY = -0x4000;
        gCurTask->accelY = -0x800;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        break;
    }
    TaskExitTrampoline();
}

void sub_08056da8(void)
{
    struct Task *t = gCurTask;

    if ((t->unk18 & 15) == 4 && t->unk28 == 0 && t->player->mode != 13)
        t->unk28 = 1;
}
