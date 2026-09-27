#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "camera.h"

/* obj_2eac8.c (0x0802EAC8-0x0802F62B, issue #86).
 *
 * Task types #221-#228 (class 3), the stage objects M07 places: for each
 * type a spawner (s32 f(x, y, ...) -> TaskCreateInRange(type, 32, 63), which
 * stores the position and the object's parameters and returns the task
 * id or -1), the type's body (sets the update/draw callbacks, the
 * graphics table Task.frameTable and the palette, then runs its animation as
 * a TaskYieldTrampoline coroutine) and its callbacks.  The animated ones
 * keep the frame to upload in Task.unk28 and a Task.updateCallback callback
 * (sub_0802eba4, sub_0802ed20, sub_0802ee88) that DMAs that frame's tiles
 * into OBJ VRAM with RequestCopy and resets unk28 to -1.  The last three
 * spawners are the three variants of type #229 (obj_2f62c.c). */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(u32 a);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskUpdatePixelPos(void);
void TaskDrawWorld(void);
void TaskSleepForever(void);
void sub_0802f718(void);
void DoorObjectDraw(void);
void sub_0802ff70(void);
void sub_0802eba4(void);
void sub_0802ed20(void);
void sub_0802ee88(void);
void sub_0802f110(void);
void sub_0802f1dc(void);
void sub_0802f2b0(void);
void sub_0802f2fc(void);
void sub_0802f3d0(void);
void sub_0802f400(void);

s32 sub_0802eac8(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(221, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk20 = 1;
        t->unk76 = a;
    }
    return id;
}

void sub_0802eb28(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gUnk_087558BC;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->unk28 = -1;
    u->frame = 0;
    u->updateCallback = (u32)sub_0802eba4;
    for (;;)
    {
        gCurTask->unk28 = 0;
        TaskYieldTrampoline(60);
        gCurTask->unk28 = 1;
        TaskYieldTrampoline(4);
        gCurTask->unk28 = 2;
        TaskYieldTrampoline(4);
        gCurTask->unk28 = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802eba4(void)
{
    u8 *src;

    if (gCurTask->unk28 != -1)
    {
        RequestCopy(4, (u32)&gUnk_085A0638[gCurTask->unk28 * 384], 0x06012180, 128);
        src = gUnk_085A0638;
        RequestCopy(4, (u32)(src + (gCurTask->unk28 * 384 + 128)), 0x06012580, 128);
        RequestCopy(4, (u32)(src + (gCurTask->unk28 * 384 + 256)), 0x06012980, 128);
        gCurTask->unk28 = -1;
    }
}

s32 sub_0802ec1c(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(222, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk20 = 2;
        t->unk76 = a;
    }
    return id;
}

void sub_0802ec7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)sub_0802f718;
    t->frameTable = gUnk_087558C0;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->unk2C = -1;
    u->unk34 = (s32)gUnk_087558DC;
    u->unk28 = 0;
    u->frame = 0;
    u->updateCallback = (u32)sub_0802ed20;
    for (;;)
    {
        gCurTask->unk2C = 0;
        TaskYieldTrampoline(30);
        gCurTask->unk2C = 1;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = 2;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = 3;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = 4;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = 0;
        TaskYieldTrampoline(30);
        gCurTask->unk2C = 5;
        TaskYieldTrampoline(4);
    }
}

void sub_0802ed20(void)
{
    u8 *src;

    if (gCurTask->unk2C != -1)
    {
        RequestCopy(4, (u32)gUnk_085A0C38[gCurTask->unk2C * 9], 0x06012100, 96);
        src = (u8 *)gUnk_085A0C38;
        RequestCopy(4, (u32)(src + (gCurTask->unk2C * 9 + 3) * 32), 0x06012500, 96);
        RequestCopy(4, (u32)(src + (gCurTask->unk2C * 9 + 6) * 32), 0x06012900, 96);
        gCurTask->unk2C = -1;
    }
}

s32 sub_0802ed94(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(223, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->pixelY = y;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_0802ede4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_087558E8;
    t->frame = 0;
    t->layer = 14;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->taskClass = 4;
    v = gCurTask;
    v->unk28 = -1;
    v->updateCallback = (u32)sub_0802ee88;
    PlaySfx(222);
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(3);
    gCurTask->unk28 = 1;
    TaskYieldTrampoline(3);
    w = gCurTask;
    w->unk28 = 2;
    w->unk1C = 0;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->unk1C == 0);
    PlaySfx(223);
    gCurTask->unk28 = 2;
    TaskYieldTrampoline(3);
    gCurTask->unk28 = 1;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0802ee88(void)
{
    struct Task *t;
    s32 idx;
    u8 *src;

    t = gCurTask;
    idx = t->unk28;
    if (idx != -1)
    {
        switch (t->unk18)
        {
        case 0:
            RequestCopy(4, (u32)&gUnk_085A12F8[idx * 768], 0x06011100, 256);
            src = gUnk_085A12F8;
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 256)), 0x06011500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        case 1:
            RequestCopy(4, (u32)&gUnk_085A1BF8[idx * 768], 0x06011100, 256);
            src = gUnk_085A1BF8;
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 256)), 0x06011500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        case 2:
            RequestCopy(4, (u32)&gUnk_085A24F8[idx * 768], 0x06011100, 256);
            src = gUnk_085A24F8;
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 256)), 0x06011500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        }
        gCurTask->unk28 = -1;
    }
}

s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(224, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x + 26;
        t->pixelY = y;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->layer = a;
        t->spriteFlags = b;
    }
    return id;
}

void sub_0802eff8(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_087558EC;
    t->tileWord = 0x8800;
    for (;;)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

s32 sub_0802f05c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(225, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 25;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f0cc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->frameTable = gUnk_087558FC;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    if (u->unk1C == 0)
        sub_0802f1dc();
    else
        sub_0802f110();
    TaskExitTrampoline();
}

void sub_0802f110(void)
{
    gCurTask->drawCallback = (u32)DoorObjectDraw;
    for (;;)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(64);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
}

void sub_0802f1dc(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)sub_0802ff70;
    t->frame = 0;
    TaskSleepForever();
}

s32 sub_0802f1fc(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(226, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 25;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f26c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->frameTable = gUnk_08755930;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    if (u->unk1C == 0)
        sub_0802f2fc();
    else
        sub_0802f2b0();
    TaskExitTrampoline();
}

void sub_0802f2b0(void)
{
    gCurTask->drawCallback = (u32)DoorObjectDraw;
    for (;;)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(62);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(6);
        gCurTask->frame = 2;
        TaskYieldTrampoline(30);
    }
}

void sub_0802f2fc(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)sub_0802ff70;
    t->frame = 0;
    TaskSleepForever();
}

s32 sub_0802f31c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(227, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 25;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f38c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->frameTable = gUnk_0875593C;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    if (u->unk1C == 0)
        sub_0802f400();
    else
        sub_0802f3d0();
    TaskExitTrampoline();
}

void sub_0802f3d0(void)
{
    gCurTask->drawCallback = (u32)DoorObjectDraw;
    for (;;)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(20);
        gCurTask->frame++;
        TaskYieldTrampoline(20);
    }
}

void sub_0802f400(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)sub_0802ff70;
    t->frame = 0;
    TaskSleepForever();
}

s32 sub_0802f420(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(228, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 28;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk20 = 1;
        t->unk76 = a;
    }
    return id;
}

void sub_0802f480(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gUnk_08755944;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->frame = 0;
    TaskYieldTrampoline(20);
    TaskSleepForever();
}

s32 sub_0802f4c8(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(229, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 24;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk18 = a;
        t->unk1C = 1;
        t->unk20 = 2;
        t->unk76 = b;
    }
    return id;
}

s32 sub_0802f53c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(229, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 24;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk18 = a;
        t->unk1C = 0;
        t->unk20 = 2;
        t->unk76 = b;
    }
    return id;
}

s32 sub_0802f5b4(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = TaskCreateInRange(229, 32, 63);
    if (id != -1)
    {
        t = &gTasks[id];
        t->pixelX = x;
        t->posX = t->pixelX << 16;
        t->pixelY = y - 24;
        t->posY = t->pixelY << 16;
        t->velX = x;
        t->velY = y;
        t->unk18 = a;
        t->unk1C = 0;
        t->unk20 = 0;
        t->unk76 = b;
    }
    return id;
}
