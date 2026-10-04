#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
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
 * (sub_0802eba4, BossDoorSignUpdate, sub_0802ee88) that DMAs that frame's tiles
 * into OBJ VRAM with RequestCopy and resets unk28 to -1.  The last three
 * spawners are the three variants of type #229 (obj_2f62c.c). */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(u32 a);

s32 CreateArenaDoorSign(s32 x, s32 y, s32 a)
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
        t->u76.doorIndex = a;
    }
    return id;
}

void Task_ArenaDoorSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gArenaDoorSignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->arenaDoorSignTileFrame = -1;
    u->frame = 0;
    u->updateCallback = (u32)sub_0802eba4;
    for (;;)
    {
        gCurTask->arenaDoorSignTileFrame = 0;
        TaskYieldTrampoline(60);
        gCurTask->arenaDoorSignTileFrame = 1;
        TaskYieldTrampoline(4);
        gCurTask->arenaDoorSignTileFrame = 2;
        TaskYieldTrampoline(4);
        gCurTask->arenaDoorSignTileFrame = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802eba4(void)
{
    u8 *src;

    if (gCurTask->arenaDoorSignTileFrame != -1)
    {
        RequestCopy(4, (u32)&gUnk_085A0638[gCurTask->arenaDoorSignTileFrame * 384], OBJ_VRAM0 + 0x2180, 128);
        src = gUnk_085A0638;
        RequestCopy(4, (u32)(src + (gCurTask->arenaDoorSignTileFrame * 384 + 128)), OBJ_VRAM0 + 0x2580, 128);
        RequestCopy(4, (u32)(src + (gCurTask->arenaDoorSignTileFrame * 384 + 256)), OBJ_VRAM0 + 0x2980, 128);
        gCurTask->arenaDoorSignTileFrame = -1;
    }
}

s32 CreateBossDoorSign(s32 x, s32 y, s32 a)
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
        t->u76.doorIndex = a;
    }
    return id;
}

void Task_BossDoorSign(void)
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
    u->bossDoorSignTileFrame = -1;
    u->bossDoorSignDoorFrames = (s32)gUnk_087558DC;
    u->bossDoorSignDoorFrame = 0;
    u->frame = 0;
    u->updateCallback = (u32)BossDoorSignUpdate;
    for (;;)
    {
        gCurTask->bossDoorSignTileFrame = 0;
        TaskYieldTrampoline(30);
        gCurTask->bossDoorSignTileFrame = 1;
        TaskYieldTrampoline(4);
        gCurTask->bossDoorSignTileFrame = 2;
        TaskYieldTrampoline(4);
        gCurTask->bossDoorSignTileFrame = 3;
        TaskYieldTrampoline(4);
        gCurTask->bossDoorSignTileFrame = 4;
        TaskYieldTrampoline(4);
        gCurTask->bossDoorSignTileFrame = 0;
        TaskYieldTrampoline(30);
        gCurTask->bossDoorSignTileFrame = 5;
        TaskYieldTrampoline(4);
    }
}

void BossDoorSignUpdate(void)
{
    u8 *src;

    if (gCurTask->bossDoorSignTileFrame != -1)
    {
        RequestCopy(4, (u32)gUnk_085A0C38[gCurTask->bossDoorSignTileFrame * 9], OBJ_VRAM0 + 0x2100, 96);
        src = (u8 *)gUnk_085A0C38;
        RequestCopy(4, (u32)(src + (gCurTask->bossDoorSignTileFrame * 9 + 3) * 32), OBJ_VRAM0 + 0x2500, 96);
        RequestCopy(4, (u32)(src + (gCurTask->bossDoorSignTileFrame * 9 + 6) * 32), OBJ_VRAM0 + 0x2900, 96);
        gCurTask->bossDoorSignTileFrame = -1;
    }
}

s32 CreateDoorOpening(s32 x, s32 y, s32 a)
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
        t->doorOpeningKind = a;
    }
    return id;
}

void Task_DoorOpening(void)
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
    v->doorOpeningTileFrame = -1;
    v->updateCallback = (u32)sub_0802ee88;
    PlaySfx(222);
    gCurTask->doorOpeningTileFrame = 0;
    TaskYieldTrampoline(3);
    gCurTask->doorOpeningTileFrame = 1;
    TaskYieldTrampoline(3);
    w = gCurTask;
    w->doorOpeningTileFrame = 2;
    w->unk1C = 0;
    do
        TaskYieldTrampoline(1);
    while (gCurTask->unk1C == 0);
    PlaySfx(223);
    gCurTask->doorOpeningTileFrame = 2;
    TaskYieldTrampoline(3);
    gCurTask->doorOpeningTileFrame = 1;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0802ee88(void)
{
    struct Task *t;
    s32 idx;
    u8 *src;

    t = gCurTask;
    idx = t->doorOpeningTileFrame;
    if (idx != -1)
    {
        switch (t->doorOpeningKind)
        {
        case 0:
            RequestCopy(4, (u32)&gUnk_085A12F8[idx * 768], OBJ_VRAM0 + 0x1100, 256);
            src = gUnk_085A12F8;
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 256)), OBJ_VRAM0 + 0x1500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 512)), OBJ_VRAM0 + 0x1900, 256);
            break;
        case 1:
            RequestCopy(4, (u32)&gUnk_085A1BF8[idx * 768], OBJ_VRAM0 + 0x1100, 256);
            src = gUnk_085A1BF8;
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 256)), OBJ_VRAM0 + 0x1500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 512)), OBJ_VRAM0 + 0x1900, 256);
            break;
        case 2:
            RequestCopy(4, (u32)&gUnk_085A24F8[idx * 768], OBJ_VRAM0 + 0x1100, 256);
            src = gUnk_085A24F8;
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 256)), OBJ_VRAM0 + 0x1500, 256);
            RequestCopy(4, (u32)(src + (gCurTask->doorOpeningTileFrame * 768 + 512)), OBJ_VRAM0 + 0x1900, 256);
            break;
        }
        gCurTask->doorOpeningTileFrame = -1;
    }
}

s32 CreateStageClearFlag(s32 x, s32 y, s32 a, s32 b)
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

void Task_StageClearFlag(void)
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

s32 CreateQuickDrawDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->u76.doorIndex = b;
    }
    return id;
}

void Task_QuickDrawDoorSign(void)
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

s32 CreateBombRallyDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->u76.doorIndex = b;
    }
    return id;
}

void Task_BombRallyDoorSign(void)
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

s32 CreateAirGrindDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->u76.doorIndex = b;
    }
    return id;
}

void Task_AirGrindDoorSign(void)
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

s32 CreateMuseumDoorSign(s32 x, s32 y, s32 a)
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
        t->u76.doorIndex = a;
    }
    return id;
}

void Task_MuseumDoorSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gMuseumDoorSignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->frame = 0;
    TaskYieldTrampoline(20);
    TaskSleepForever();
}

s32 CreateStageDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->stageDoorSignIndex = a;
        t->stageDoorSignAnimated = 1;
        t->stageDoorSignDoorAnimated = 2;
        t->u76.doorIndex = b;
    }
    return id;
}

s32 CreateClearedStageDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->stageDoorSignIndex = a;
        t->stageDoorSignAnimated = 0;
        t->stageDoorSignDoorAnimated = 2;
        t->u76.doorIndex = b;
    }
    return id;
}

s32 CreateCompletedStageDoorSign(s32 x, s32 y, s32 a, s32 b)
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
        t->stageDoorSignIndex = a;
        t->stageDoorSignAnimated = 0;
        t->stageDoorSignDoorAnimated = 0;
        t->u76.doorIndex = b;
    }
    return id;
}
