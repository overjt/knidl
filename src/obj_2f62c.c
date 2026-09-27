#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_2f62c.c (0x0802F62C-0x08030237, issue #86).
 *
 * Task types #229-#235 (class 3) and the spawner of #236, in the same
 * spawner / body / callback layout as obj_2eac8.c.  The draw callbacks
 * (sub_0802f718, sub_0802f93c, sub_0802fd98, DoorObjectDraw, sub_0802ff70;
 * several types share the last two) draw the frame Task.frame of the
 * Task.frameTable table through QueueWorldSprite, and a second sprite from
 * Task.unk34 at Task.velX/unk58 where the type has one.  #231
 * (sub_0802faa8) flies a fixed path, eight velocity changes per lap.
 * sub_0802ffe8 is also called from M33 (src/hud_b5024.c), and
 * sub_08030100 spawns up to two #235 objects from the table
 * gUnk_087328C0. */

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[6];
};

extern u32 gUnk_08755948[];
extern u32 gUnk_087558DC[];
extern u32 gUnk_087558C4[];
extern struct Unk02004B90 gDoorStates[];
extern u32 gUnk_08755978[];
extern u8 gUnk_085A2DF8[][32];
extern s16 gUnk_020055D4;
extern u32 gUnk_087558D0[];
extern u8 gUnk_02007FC4;
extern u32 gUnk_0875597C[];
extern u32 gUnk_0875599C[];
extern u32 gUnk_087559A4[];
extern u32 gUnk_087559C0[];
extern s16 gUnk_087328C0[][2];
extern u32 gUnk_087559DC[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskMove(void);
void TaskUpdatePixelPos(void);
void TaskDrawWorld(void);
void TaskSleepForever(void);
void sub_08008f10(s32 a0);
s32 QueueWorldSprite(u8 a, s32 b, u16 c, u16 d, s16 x, s16 y);
void sub_0802f684(void);
void sub_0802f6c0(void);
void sub_0802f6f4(void);
void sub_0802f718(void);
void sub_0802f8c8(void);
void sub_0802f93c(void);
void sub_0802fd98(void);
void DoorObjectDraw(void);
s32 sub_08030140(u8 a, s32 b);

void sub_0802f62c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)sub_0802f718;
    t->frameTable = gUnk_08755948;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    if (u->unk20 == 0)
        sub_0802f6f4();
    else if (u->unk1C == 0)
        sub_0802f6c0();
    else
        sub_0802f684();
    TaskExitTrampoline();
}

void sub_0802f684(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk34 = (s32)gUnk_087558DC;
    for (;;)
    {
        u = gCurTask;
        u->unk28 = 0;
        u->frame = u->unk18 << 1;
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->unk28 = 1;
        v->frame++;
        TaskYieldTrampoline(2);
    }
}

void sub_0802f6c0(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34 = (s32)gUnk_087558DC;
    t->frame = t->unk18 << 1;
    for (;;)
    {
        gCurTask->unk28 = 0;
        TaskYieldTrampoline(2);
        gCurTask->unk28 = 1;
        TaskYieldTrampoline(2);
    }
}

void sub_0802f6f4(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34 = (s32)gUnk_087558C4;
    t->unk28 = 0;
    t->frame = t->unk18 << 1;
    TaskSleepForever();
}

void sub_0802f718(void)
{
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    t = gCurTask;
    if (t->frame != -1)
    {
        tbl = t->frameTable;
        QueueWorldSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
    u = gCurTask;
    tbl = (u32 *)u->unk34;
    if (gDoorStates[u->unk76].unk1 != 0)
        QueueWorldSprite(u->layer, tbl[u->unk28], u->spriteFlags, u->tileWord, u->velX, u->velY);
    else
        QueueWorldSprite(u->layer, tbl[2], u->spriteFlags, u->tileWord, u->velX, u->velY);
}

s32 sub_0802f7dc(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(230, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk18 = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f84c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)sub_0802f93c;
    t->frameTable = gUnk_08755978;
    t->layer = 14;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->taskClass = 2;
    v = gCurTask;
    v->updateCallback = (u32)sub_0802f8c8;
    v->frame = 0;
    v->unk34 = 0;
    for (;;)
    {
        gCurTask->unk34 = 0;
        TaskYieldTrampoline(4);
        gCurTask->unk34 = 1;
        TaskYieldTrampoline(4);
        gCurTask->unk34 = 2;
        TaskYieldTrampoline(4);
        gCurTask->unk34 = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802f8c8(void)
{
    u8 *src;

    if (gCurTask->unk34 != -1)
    {
        RequestCopy(4, (u32)gUnk_085A2DF8[gCurTask->unk34 * 9], 0x06013980, 96);
        src = (u8 *)gUnk_085A2DF8;
        RequestCopy(4, (u32)(src + (gCurTask->unk34 * 9 + 3) * 32), 0x06013D80, 96);
        RequestCopy(4, (u32)(src + (gCurTask->unk34 * 9 + 6) * 32), 0x06014180, 96);
        gCurTask->unk34 = -1;
    }
}

void sub_0802f93c(void)
{
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    t = gCurTask;
    if (t->unk18 != 0)
        QueueWorldSprite(t->layer + 1, gUnk_087558D0[1], t->spriteFlags, t->tileWord, t->velX, t->velY);
    else if (gDoorStates[t->unk76].unk1 != 0)
        QueueWorldSprite(t->layer + 1, gUnk_087558D0[0], t->spriteFlags, t->tileWord, t->velX, t->velY);
    else
        QueueWorldSprite(t->layer + 1, gUnk_087558D0[2], t->spriteFlags, t->tileWord, t->velX, t->velY);
    u = gCurTask;
    if (u->frame != -1)
    {
        tbl = u->frameTable;
        QueueWorldSprite(u->layer, tbl[u->frame], gUnk_020055D4, u->tileWord, u->pixelX, u->pixelY);
    }
}

s32 sub_0802fa3c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(231, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->unk24 = a;
        t->unk20 = b;
        gUnk_02007FC4 = 0;
    }
    return id;
}

void sub_0802faa8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0802fd98;
    t->frameTable = gUnk_0875597C;
    t->layer = 14;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->taskClass = 1;
    v = gCurTask;
    v->frame = -1;
    if (v->unk20 != 0)
        while (gUnk_02007FC4 == 0)
            TaskYieldTrampoline(1);
    if (gCurTask->unk24 != 0)
    {
        gCurTask->taskClass = 3;
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0x10000;
        gCurTask->frame = -1;
        TaskYieldTrampoline(4);
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x8000;
        gCurTask->frame = 5;
        TaskYieldTrampoline(4);
        gCurTask->frame = 6;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        gCurTask->frame = 7;
        TaskYieldTrampoline(4);
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->taskClass = 1;
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x8000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(4);
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x10000;
        gCurTask->velY = -0x10000;
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
    }
    for (;;)
    {
        gCurTask->velX = 0x10000;
        gCurTask->velY = -0x10000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(4);
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x8000;
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        gCurTask->frame = 0;
        TaskYieldTrampoline(4);
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->taskClass = 3;
        gCurTask->frame = 7;
        TaskYieldTrampoline(4);
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x8000;
        gCurTask->frame = 5;
        TaskYieldTrampoline(4);
        gCurTask->frame = 6;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0x10000;
        gCurTask->frame = 7;
        TaskYieldTrampoline(4);
        gCurTask->frame = -1;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0x10000;
        gCurTask->frame = -1;
        TaskYieldTrampoline(4);
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x8000;
        gCurTask->frame = 5;
        TaskYieldTrampoline(4);
        gCurTask->frame = 6;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        gCurTask->frame = 7;
        TaskYieldTrampoline(4);
        gCurTask->frame = 4;
        TaskYieldTrampoline(4);
        gCurTask->taskClass = 1;
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x8000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(4);
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x10000;
        gCurTask->velY = -0x10000;
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802fd98(void)
{
    struct Task *t;
    u32 *tbl;

    t = gCurTask;
    if (t->frame != -1)
    {
        tbl = t->frameTable;
        QueueWorldSprite(t->layer, tbl[t->frame], gUnk_020055D4, t->tileWord, t->pixelX, t->pixelY);
    }
}

s32 sub_0802fdf4(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(232, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk18 = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802fe64(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gUnk_0875599C;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->frame = u->unk18;
    TaskSleepForever();
}

void DoorObjectDraw(void)
{
    struct Task *u;
    u32 *tbl;

    if (gDoorStates[gCurTask->unk76].unk1 != 0)
        QueueWorldSprite(gCurTask->layer, gUnk_087558D0[0], gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->velX, gCurTask->velY);
    else
        QueueWorldSprite(gCurTask->layer, gUnk_087558D0[2], gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->velX, gCurTask->velY);
    u = gCurTask;
    if (u->frame != -1)
    {
        tbl = u->frameTable;
        QueueWorldSprite(u->layer, tbl[u->frame], u->spriteFlags, u->tileWord, u->pixelX, u->pixelY);
    }
}

void sub_0802ff70(void)
{
    struct Task *t;
    u32 *tbl;

    t = gCurTask;
    if (t->frame != -1)
    {
        tbl = t->frameTable;
        QueueWorldSprite(15, tbl[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
    QueueWorldSprite(15, gUnk_087558D0[1], gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->velX, gCurTask->velY);
}

s32 sub_0802ffe8(s32 a, s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(233, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->pixelY = y - 28;
        t->posX = x << 16;
        t->posY = (y - 28) << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_08030034(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_087559A4;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD800;
    u->frame = u->unk18;
    TaskSleepForever();
}

s32 sub_08030074(s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(234, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = 128;
        t->pixelY = 420;
        t->posX = 128 << 16;
        t->posY = 420 << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_080300c0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_087559C0;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD800;
    u->frame = u->unk18;
    TaskSleepForever();
}

s32 sub_08030100(u8 a)
{
    s32 i;

    if (a <= 5)
    {
        sub_08008f10(a);
        for (i = 0; i <= 1; i++)
        {
            if (gUnk_087328C0[a][i] != -1)
                sub_08030140(a, i);
        }
    }
}

s32 sub_08030140(u8 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(235, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = gUnk_087328C0[a][b];
        t->pixelY = 96;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->unk18 = b;
    }
    return id;
}

void sub_080301a4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_087559DC;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD3C0;
    u->frame = u->unk18;
    TaskSleepForever();
}

s32 CreateStageEffect(s32 a, s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(236, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->pixelY = y;
        t->posX = x << 16;
        t->posY = y << 16;
        t->state = a;
        t->actorKind = 9;
    }
    return id;
}
