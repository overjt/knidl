#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* effect_57ce0.c (0x08057CE0-0x0805880F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 35-39, all spawned by M12's actions (39 also by M11 and M13).
 * Variants 35 (sub_08057ce0, two sub-states), 36 (sub_08057e90) and 39
 * (sub_08058720) are animations in world space (gUnk_08751D88,
 * gUnk_08751DB0, gUnk_08751E00).  Variant 37 (PlayerEffectMikeAttack) stops other
 * tasks: it fills the 20-slot table gScreenAttackTasks with 0xFFFF, collects the
 * indices of the tasks of kinds 1, 2, 7 and 8 (Task.actorKind) whose
 * gTaskSlotTypes entry is not -1 (while gInHub is clear), stops them
 * through the task skip mask (TaskRestoreSkipMask, with gScreenAttackActive = 1), and
 * after the yield releases them (TaskSaveSkipMask, TaskSetSkipMask(15, i)); then it
 * walks the tasks 32-62 of kinds 0, 3, 4, 6 and 9 one at a time the same
 * way.  Its callback PlayerEffectMikeAttackUpdate kills it once the player leaves mode 13
 * and otherwise, while PlayerState.unk16 is set, registers the collider row
 * gUnk_0873C04C (RegisterCollider).  Variant 38 (PlayerEffectSleepBubble) rides on its
 * spawner through four sub-states and draws through M11's sub_0803dfc8;
 * PlayerEffectSleepBubbleUpdate kills it once the player leaves mode 13. */

void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void LoadBackdropColor(u32 src);

void sub_08057ce0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751D88;
    t->tileWord = ((t->u8C.parentTask)->tileWord + 0x1800) | 12;
    switch (t->unk18 & 15)
    {
    case 0:
        if (t->facing == 1)
            t->posX = (t->pixelX + 32) << 16;
        else
            t->posX = (t->pixelX - 20) << 16;
        gCurTask->facing = 1;
        break;
    case 1:
        if (t->facing == 1)
            t->posX = (t->pixelX + 22) << 16;
        else
            t->posX = (t->pixelX - 32) << 16;
        gCurTask->facing = -1;
        break;
    }
    u = gCurTask;
    u->posY = u->pixelY << 16;
    if (!((u->u8C.parentTask)->waterFlags & 1))
    {
        TaskSetMotionXFacing(0x30000, -0x2800);
        gCurTask->velY = 0;
        gCurTask->accelY = -0x2000;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(3);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
    }
    else
    {
        TaskSetMotionXFacing(0x18000, -0x1400);
        gCurTask->velY = 0;
        gCurTask->accelY = -0x1000;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(6);
        gCurTask->frame += 2;
        TaskYieldTrampoline(4);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
    }
    TaskExitTrampoline();
}

void sub_08057e90(void)
{
    struct Task *t;
    struct Task *u;
    u16 *p;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751DB0;
    t->tileWord = (t->u8C.parentTask)->tileWord | 0xF00C;
    if (t->facing == 1)
    {
        t->posX = ((t->u8C.parentTask)->pixelX + 40) << 16;
        t->posY = ((t->u8C.parentTask)->pixelY + 4) << 16;
    }
    else
    {
        t->posX = ((t->u8C.parentTask)->pixelX - 40) << 16;
        t->posY = ((t->u8C.parentTask)->pixelY + 4) << 16;
    }
    p = &gUnk_0873BAEE[(gCurTask->unk18 & 3) * 2];
    a = p[0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    TaskSetMotionXFacing(b, -0x1000);
    u = gCurTask;
    c = p[1];
    d = c << 8;
    if (c & 0x8000)
        d |= 0xFF000000;
    u->velY = d;
    u->accelY = 0;
    u->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskExitTrampoline();
}

void PlayerEffectMikeAttack(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *q;
    s16 *p;
    s32 i;
    s32 n;
    s32 m;
    s32 k;
    s32 r;
    u16 x0;
    u16 y0;
    u16 pal;

    t = gCurTask;
    if ((t->unk18 & 15) != 1)
    {
        t->moveCallback = 0;
        t->drawCallback = 0;
        t->updateCallback = (u32)PlayerEffectMikeAttackUpdate;
        t->u80.attackAbility = 7;
        while ((s8)gCurTask->player->unk16 == 0)
            TaskYieldTrampoline(1);
        u = gCurTask;
        u->unk28 = gPlayerCameraPos[u->parent].x;
        u->unk2C = gPlayerCameraPos[u->parent].y;
        gScreenAttackActive = 0;
        for (i = 0; i < 20; i++)
            gScreenAttackTasks[i] |= 0xFFFF;
        k = 0;
        for (i = 0; i <= 62; i++)
        {
            if (gInHub == 0 && gTaskSlotTypes[i] != -1)
            {
                switch (gTasks[i].actorKind)
                {
                case 1:
                case 2:
                case 7:
                case 8:
                    gScreenAttackTasks[k++] = i;
                    break;
                }
            }
        }
        m = 0;
        n = 0;
        while ((s16)gScreenAttackTasks[n] != -1 && n != 20)
        {
            gScreenAttackActive = 1;
            TaskRestoreSkipMask((s16)gScreenAttackTasks[n++]);
            m++;
        }
        TaskYieldTrampoline(1);
        while (n != 0)
        {
            n--;
            switch (gTasks[(s16)gScreenAttackTasks[n]].actorKind)
            {
            case 1:
            case 2:
            case 7:
            case 8:
                TaskSaveSkipMask((s16)gScreenAttackTasks[n]);
                break;
            default:
                gTasks[(s16)gScreenAttackTasks[n]].skipMask = 0;
                TaskSaveSkipMask((s16)gScreenAttackTasks[n]);
                break;
            }
            TaskSetSkipMask(15, (s16)gScreenAttackTasks[n]);
        }
        if (m != 0)
            TaskYieldTrampoline(3);
        gScreenAttackActive = 0;
        x0 = gPlayerCameraPos[gCurTask->parent].x - 120;
        y0 = gPlayerCameraPos[gCurTask->parent].y - 80;
        for (i = 32; i <= 62; i++)
        {
            if (gInHub != 0)
                continue;
            if (gTaskSlotTypes[i] == -1)
                continue;
            q = &gTasks[i];
            if (q->skipMask == 0)
                continue;
            if (q->drawCallback == 0)
                continue;
            if (q->pixelX >= (s16)x0 && q->pixelX < (s16)x0 + 240
                && q->pixelY >= (s16)y0 && q->pixelY < (s16)y0 + 160)
                gScreenAttackActive = 1;
            switch (gTasks[i].actorKind)
            {
            case 6:
                r = 0;
                if (gTasks[i].unk76 != 5)
                {
                    TaskRestoreSkipMask(i);
                    m++;
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 0:
            case 3:
            case 4:
            case 9:
                TaskRestoreSkipMask(i);
                m++;
                TaskYieldTrampoline(2);
                r = 2;
                break;
            default:
                continue;
            }
            if (r != 0)
            {
                TaskSaveSkipMask(i);
                TaskSetSkipMask(15, i);
                TaskYieldTrampoline(2);
                gScreenAttackActive = 0;
            }
        }
        if (m == 0)
            TaskYieldTrampoline(1);
        gCurTask->player->unk16--;
        gScreenAttackActive = 0;
        TaskExitTrampoline();
    }
    pal = gBgPalette[0];
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(1);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(2);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(1);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    LoadBackdropColor((u32)&pal);
    TaskExitTrampoline();
}

void PlayerEffectMikeAttackUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->player->mode != 13)
        TaskFree(gCurTaskIdx);
    else if ((s8)t->player->unk16 != 0)
        RegisterCollider((u8)gCurTaskIdx, t->unk28, t->unk2C, gUnk_0873C04C);
}

void PlayerEffectSleepBubble(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 *p;

    t = gCurTask;
    t->updateCallback = (u32)PlayerEffectSleepBubbleUpdate;
    switch (t->unk18 & 15)
    {
    case 0:
    case 1:
    case 2:
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)TaskDrawWorld;
        t->layer = 5;
        u = gCurTask;
        u->frameTable = gUnk_08751DD0;
        u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 8;
        if (u->facing == 1)
            u->posX = 0x60000;
        else
            u->posX = -0x60000;
        u->posY = -0x20000;
        p = &gUnk_0873BAFC[(gCurTask->unk18 & 15) * 3];
        TaskSetMotionXFacing((p[0] & 0x8000) ? (p[0] << 8) | 0xFF000000 : p[0] << 8,
                     (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
        v = gCurTask;
        v->velY = -0x8000;
        v->accelY = 0x200;
        v->unk6C = 0;
        do
        {
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(6);
            TaskSetMotionXFacing(0x5A5A5A5A, (p[2] & 0x8000) ? (p[2] << 8) | 0xFF000000 : p[2] << 8);
            gCurTask->frame += 2;
            TaskYieldTrampoline(4);
            TaskSetMotionXFacing(0x5A5A5A5A, (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
            gCurTask->frame += 2;
            TaskYieldTrampoline(6);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(0x5A5A5A5A, (p[2] & 0x8000) ? (p[2] << 8) | 0xFF000000 : p[2] << 8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(4);
        TaskSetMotionXFacing(0x5A5A5A5A, (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
        gCurTask->frame += 2;
        TaskYieldTrampoline(6);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(4);
        break;
    case 3:
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)sub_0803dfc8;
        t->layer = 8;
        u = gCurTask;
        u->frameTable = gUnk_08751DBC;
        u->tileWord = ((u->u8C.parentTask)->tileWord + 0x1800) | 12;
        u->posX = 0;
        u->posY = -0x80000;
        u->velY = -0x20000;
        u->accelY = 0x2000;
        TaskSetFrame(0);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(3);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectSleepBubbleUpdate(void)
{
    if (gCurTask->player->mode != 13)
        TaskFree(gCurTaskIdx);
}

void sub_08058720(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751E00;
    if ((t->unk18 & 15) == 0)
    {
        t->facing = 1;
        gCurTask->frame = 1;
    }
    else
    {
        t->facing = -1;
        gCurTask->frame = 0;
    }
    gCurTask->posY = (gCurTask->pixelY + 4) << 16;
    TaskSetMotionXFacing(0x80000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0x20000, -0x1000);
    gCurTask->frame += 2;
    TaskYieldTrampoline(3);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(4);
    gCurTask->frame += 2;
    TaskYieldTrampoline(3);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
