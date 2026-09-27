#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_57ce0.c (0x08057CE0-0x0805880F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 35-39, all spawned by M12's actions (39 also by M11 and M13).
 * Variants 35 (sub_08057ce0, two sub-states), 36 (sub_08057e90) and 39
 * (sub_08058720) are animations in world space (gUnk_08751D88,
 * gUnk_08751DB0, gUnk_08751E00).  Variant 37 (sub_08057f90) stops other
 * tasks: it fills the 20-slot table gUnk_0200B000 with 0xFFFF, collects the
 * indices of the tasks of kinds 1, 2, 7 and 8 (Task.unk72) whose
 * gTaskSlotTypes entry is not -1 (while gUnk_03002444 is clear), stops them
 * through the task skip mask (TaskRestoreSkipMask, with gUnk_02006178 = 1), and
 * after the yield releases them (TaskSaveSkipMask, TaskSetSkipMask(15, i)); then it
 * walks the tasks 32-62 of kinds 0, 3, 4, 6 and 9 one at a time the same
 * way.  Its callback sub_08058410 kills it once the player leaves mode 13
 * and otherwise, while PlayerState.unk16 is set, registers the collider row
 * gUnk_0873C04C (RegisterCollider).  Variant 38 (sub_08058460) rides on its
 * spawner through four sub-states and draws through M11's sub_0803dfc8;
 * sub_080586fc kills it once the player leaves mode 13. */

/* M08's per-player camera positions (src/camera_28b8c.c) */
struct CamPos { u16 x, y; };

extern u32 gUnk_08751D88[];
extern u32 gUnk_08751DB0[];
extern u16 gUnk_0873BAEE[];
extern struct CamPos gPlayerCameraPos[4];
extern u8 gUnk_02006178;
extern s8 gUnk_03002444;
extern vs16 gTaskSlotTypes[];
extern vu16 gDispCnt;              /* DISPCNT shadow */
extern u16 gUnk_0200B000[];
extern u16 gBgPalette[];
extern u8 gUnk_0873BAFA[];
extern u8 gUnk_0873C04C[];
extern u32 gUnk_08751DD0[];
extern u32 gUnk_08751DBC[];
extern u16 gUnk_0873BAFC[];
extern u32 gUnk_08751E00[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetFrame(s32 a);
void TaskRestoreSkipMask(u32 idx);
void TaskSaveSkipMask(u32 idx);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803dfc8(void);
void LoadBackdropColor(u32 src);
void sub_08058410(void);
void sub_080586fc(void);

void sub_08057ce0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751D88;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    switch (t->unk18 & 15)
    {
    case 0:
        if (t->facing == 1)
            t->posX = (t->unk48 + 32) << 16;
        else
            t->posX = (t->unk48 - 20) << 16;
        gCurTask->facing = 1;
        break;
    case 1:
        if (t->facing == 1)
            t->posX = (t->unk48 + 22) << 16;
        else
            t->posX = (t->unk48 - 32) << 16;
        gCurTask->facing = -1;
        break;
    }
    u = gCurTask;
    u->posY = u->unk4A << 16;
    if (!(((struct Task *)u->unk8C)->unk7B & 1))
    {
        TaskSetMotionXFacing(0x30000, -0x2800);
        gCurTask->unk58 = 0;
        gCurTask->unk60 = -0x2000;
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
        gCurTask->unk58 = 0;
        gCurTask->unk60 = -0x1000;
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

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751DB0;
    t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xF00C;
    if (t->facing == 1)
    {
        t->posX = (((struct Task *)t->unk8C)->unk48 + 40) << 16;
        t->posY = (((struct Task *)t->unk8C)->unk4A + 4) << 16;
    }
    else
    {
        t->posX = (((struct Task *)t->unk8C)->unk48 - 40) << 16;
        t->posY = (((struct Task *)t->unk8C)->unk4A + 4) << 16;
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
    u->unk58 = d;
    u->unk60 = 0;
    u->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskExitTrampoline();
}

void sub_08057f90(void)
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
        t->unk00 = 0;
        t->unk0C = 0;
        t->unk04 = (u32)sub_08058410;
        t->unk80 = 7;
        while ((s8)gCurTask->unk88->unk16 == 0)
            TaskYieldTrampoline(1);
        u = gCurTask;
        u->unk28 = gPlayerCameraPos[u->unk44].x;
        u->unk2C = gPlayerCameraPos[u->unk44].y;
        gUnk_02006178 = 0;
        for (i = 0; i < 20; i++)
            gUnk_0200B000[i] |= 0xFFFF;
        k = 0;
        for (i = 0; i <= 62; i++)
        {
            if (gUnk_03002444 == 0 && gTaskSlotTypes[i] != -1)
            {
                switch (gTasks[i].unk72)
                {
                case 1:
                case 2:
                case 7:
                case 8:
                    gUnk_0200B000[k++] = i;
                    break;
                }
            }
        }
        m = 0;
        n = 0;
        while ((s16)gUnk_0200B000[n] != -1 && n != 20)
        {
            gUnk_02006178 = 1;
            TaskRestoreSkipMask((s16)gUnk_0200B000[n++]);
            m++;
        }
        TaskYieldTrampoline(1);
        while (n != 0)
        {
            n--;
            switch (gTasks[(s16)gUnk_0200B000[n]].unk72)
            {
            case 1:
            case 2:
            case 7:
            case 8:
                TaskSaveSkipMask((s16)gUnk_0200B000[n]);
                break;
            default:
                gTasks[(s16)gUnk_0200B000[n]].skipMask = 0;
                TaskSaveSkipMask((s16)gUnk_0200B000[n]);
                break;
            }
            TaskSetSkipMask(15, (s16)gUnk_0200B000[n]);
        }
        if (m != 0)
            TaskYieldTrampoline(3);
        gUnk_02006178 = 0;
        x0 = gPlayerCameraPos[gCurTask->unk44].x - 120;
        y0 = gPlayerCameraPos[gCurTask->unk44].y - 80;
        for (i = 32; i <= 62; i++)
        {
            if (gUnk_03002444 != 0)
                continue;
            if (gTaskSlotTypes[i] == -1)
                continue;
            q = &gTasks[i];
            if (q->skipMask == 0)
                continue;
            if (q->unk0C == 0)
                continue;
            if (q->unk48 >= (s16)x0 && q->unk48 < (s16)x0 + 240
                && q->unk4A >= (s16)y0 && q->unk4A < (s16)y0 + 160)
                gUnk_02006178 = 1;
            switch (gTasks[i].unk72)
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
                gUnk_02006178 = 0;
            }
        }
        if (m == 0)
            TaskYieldTrampoline(1);
        gCurTask->unk88->unk16--;
        gUnk_02006178 = 0;
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

void sub_08058410(void)
{
    struct Task *t = gCurTask;

    if (t->unk88->unk04 != 13)
        TaskFree(gCurTaskIdx);
    else if ((s8)t->unk88->unk16 != 0)
        RegisterCollider((u8)gCurTaskIdx, t->unk28, t->unk2C, gUnk_0873C04C);
}

void sub_08058460(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 *p;

    t = gCurTask;
    t->unk04 = (u32)sub_080586fc;
    switch (t->unk18 & 15)
    {
    case 0:
    case 1:
    case 2:
        t->unk00 = (u32)TaskMoveRelativeToParent;
        t->unk0C = (u32)TaskDrawWorld;
        t->layer = 5;
        u = gCurTask;
        u->unk38 = gUnk_08751DD0;
        u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 8;
        if (u->facing == 1)
            u->posX = 0x60000;
        else
            u->posX = -0x60000;
        u->posY = -0x20000;
        p = &gUnk_0873BAFC[(gCurTask->unk18 & 15) * 3];
        TaskSetMotionXFacing((p[0] & 0x8000) ? (p[0] << 8) | 0xFF000000 : p[0] << 8,
                     (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
        v = gCurTask;
        v->unk58 = -0x8000;
        v->unk60 = 0x200;
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
        t->unk00 = (u32)TaskMoveRelativeToParent;
        t->unk0C = (u32)sub_0803dfc8;
        t->layer = 8;
        u = gCurTask;
        u->unk38 = gUnk_08751DBC;
        u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
        u->posX = 0;
        u->posY = -0x80000;
        u->unk58 = -0x20000;
        u->unk60 = 0x2000;
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

void sub_080586fc(void)
{
    if (gCurTask->unk88->unk04 != 13)
        TaskFree(gCurTaskIdx);
}

void sub_08058720(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751E00;
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
    gCurTask->posY = (gCurTask->unk4A + 4) << 16;
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
