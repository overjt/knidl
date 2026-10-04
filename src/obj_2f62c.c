#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "mode.h"
#include "room.h"
#include "camera.h"

/* obj_2f62c.c (0x0802F62C-0x08030237, issue #86).
 *
 * Task types #229-#235 (class 3) and the spawner of #236, in the same
 * spawner / body / callback layout as obj_2eac8.c.  The draw callbacks
 * (sub_0802f718, sub_0802f93c, sub_0802fd98, DoorObjectDraw, SubGameDoorSignDrawUsed;
 * several types share the last two) draw the frame Task.frame of the
 * Task.frameTable table through QueueWorldSprite, and a second sprite from
 * Task.unk34 at Task.velX/unk58 where the type has one.  #231
 * (Task_WarpStarStationDoorSparkle) flies a fixed path, eight velocity changes per lap.
 * CreateWarpStarStationNumber is also called from M33 (src/hud_b5024.c), and
 * CreateMuseumAbilitySigns spawns up to two #235 objects from the table
 * gUnk_087328C0. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void Task_StageDoorSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)sub_0802f718;
    t->frameTable = gStageDoorSignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    if (u->doorObjectKind == 0)
        StageDoorSignShowStill();
    else if (u->stageDoorSignAnimated == 0)
        StageDoorSignBlinkDoor();
    else
        StageDoorSignBlinkSignAndDoor();
    TaskExitTrampoline();
}

void StageDoorSignBlinkSignAndDoor(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->stageDoorSignDoorFrames = (s32)gUnk_087558DC;
    for (;;)
    {
        u = gCurTask;
        u->stageDoorSignDoorFrame = 0;
        u->frame = u->stageDoorSignIndex << 1;
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->stageDoorSignDoorFrame = 1;
        v->frame++;
        TaskYieldTrampoline(2);
    }
}

void StageDoorSignBlinkDoor(void)
{
    struct Task *t;

    t = gCurTask;
    t->stageDoorSignDoorFrames = (s32)gUnk_087558DC;
    t->frame = t->stageDoorSignIndex << 1;
    for (;;)
    {
        gCurTask->stageDoorSignDoorFrame = 0;
        TaskYieldTrampoline(2);
        gCurTask->stageDoorSignDoorFrame = 1;
        TaskYieldTrampoline(2);
    }
}

void StageDoorSignShowStill(void)
{
    struct Task *t;

    t = gCurTask;
    t->stageDoorSignDoorFrames = (s32)gUnk_087558C4;
    t->stageDoorSignDoorFrame = 0;
    t->frame = t->stageDoorSignIndex << 1;
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
    if (gDoorStates[u->u76.doorIndex].isOpen != 0)
        QueueWorldSprite(u->layer, tbl[u->unk28], u->spriteFlags, u->tileWord, u->velX, u->velY);
    else
        QueueWorldSprite(u->layer, tbl[2], u->spriteFlags, u->tileWord, u->velX, u->velY);
}

s32 CreateWarpStarStationDoorSign(s32 x, s32 y, s32 a, s32 b)
{
    s32 doorSignSlot;
    struct Task *doorSign;

    doorSignSlot = TaskCreateInRange(TASK_WARP_STAR_STATION_DOOR_SIGN, 32, 63);
    if (doorSignSlot != -1)
    {
        doorSign = &gTasks[doorSignSlot];
        doorSign->pixelX = x;
        doorSign->posX = doorSign->pixelX << 16;
        doorSign->pixelY = y - 28;
        doorSign->posY = doorSign->pixelY << 16;
        doorSign->velX = x;
        doorSign->velY = y;
        doorSign->unk18 = a;
        doorSign->doorObjectKind = 1;
        doorSign->u76.doorIndex = b;
    }
    return doorSignSlot;
}

void Task_WarpStarStationDoorSign(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)sub_0802f93c;
    t->frameTable = gWarpStarStationDoorSignFrames;
    t->layer = 14;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->taskClass = 2;
    v = gCurTask;
    v->updateCallback = (u32)WarpStarStationDoorSignUpdate;
    v->frame = 0;
    v->warpStarStationDoorSignTileFrame = 0;
    for (;;)
    {
        gCurTask->warpStarStationDoorSignTileFrame = 0;
        TaskYieldTrampoline(4);
        gCurTask->warpStarStationDoorSignTileFrame = 1;
        TaskYieldTrampoline(4);
        gCurTask->warpStarStationDoorSignTileFrame = 2;
        TaskYieldTrampoline(4);
        gCurTask->warpStarStationDoorSignTileFrame = 3;
        TaskYieldTrampoline(4);
    }
}

void WarpStarStationDoorSignUpdate(void)
{
    u8 *src;

    if (gCurTask->warpStarStationDoorSignTileFrame != -1)
    {
        RequestCopy(4, (u32)gUnk_085A2DF8[gCurTask->warpStarStationDoorSignTileFrame * 9], OBJ_VRAM0 + 0x3980, 96);
        src = (u8 *)gUnk_085A2DF8;
        RequestCopy(4, (u32)(src + (gCurTask->warpStarStationDoorSignTileFrame * 9 + 3) * 32), OBJ_VRAM0 + 0x3D80, 96);
        RequestCopy(4, (u32)(src + (gCurTask->warpStarStationDoorSignTileFrame * 9 + 6) * 32), OBJ_VRAM0 + 0x4180, 96);
        gCurTask->warpStarStationDoorSignTileFrame = -1;
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
    else if (gDoorStates[t->u76.doorIndex].isOpen != 0)
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

s32 CreateWarpStarStationDoorSparkle(s32 x, s32 y, s32 a, s32 b)
{
    s32 doorSparkleSlot;
    struct Task *doorSparkle;

    doorSparkleSlot = TaskCreateInRange(TASK_WARP_STAR_STATION_DOOR_SPARKLE, 32, 63);
    if (doorSparkleSlot != -1)
    {
        doorSparkle = &gTasks[doorSparkleSlot];
        doorSparkle->pixelX = x;
        doorSparkle->posX = doorSparkle->pixelX << 16;
        doorSparkle->pixelY = y - 28;
        doorSparkle->posY = doorSparkle->pixelY << 16;
        doorSparkle->unk24 = a;
        doorSparkle->warpStarStationDoorSparkleWaitReveal = b;
        gWarpStarStationDoorRevealed = 0;
    }
    return doorSparkleSlot;
}

void Task_WarpStarStationDoorSparkle(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0802fd98;
    t->frameTable = gWarpStarStationDoorSparkleFrames;
    t->layer = 14;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->taskClass = 1;
    v = gCurTask;
    v->frame = -1;
    if (v->warpStarStationDoorSparkleWaitReveal != 0)
        while (gWarpStarStationDoorRevealed == 0)
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

s32 CreateLevelDoorSign(s32 x, s32 y, s32 a, s32 b)
{
    s32 levelDoorSignSlot;
    struct Task *levelDoorSign;

    levelDoorSignSlot = TaskCreateInRange(TASK_LEVEL_DOOR_SIGN, 32, 63);
    if (levelDoorSignSlot != -1)
    {
        levelDoorSign = &gTasks[levelDoorSignSlot];
        levelDoorSign->pixelX = x;
        levelDoorSign->posX = levelDoorSign->pixelX << 16;
        levelDoorSign->pixelY = y - 28;
        levelDoorSign->posY = levelDoorSign->pixelY << 16;
        levelDoorSign->velX = x;
        levelDoorSign->velY = y;
        levelDoorSign->levelDoorSignFrame = a;
        levelDoorSign->doorObjectKind = 1;
        levelDoorSign->u76.doorIndex = b;
    }
    return levelDoorSignSlot;
}

void Task_LevelDoorSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)DoorObjectDraw;
    t->frameTable = gLevelDoorSignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0x8800;
    u->frame = u->levelDoorSignFrame;
    TaskSleepForever();
}

void DoorObjectDraw(void)
{
    struct Task *u;
    u32 *tbl;

    if (gDoorStates[gCurTask->u76.doorIndex].isOpen != 0)
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

void SubGameDoorSignDrawUsed(void)
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

s32 CreateWarpStarStationNumber(s32 a, s32 x, s32 y)
{
    s32 stationNumberSlot;
    struct Task *stationNumber;

    stationNumberSlot = TaskCreateInRange(TASK_WARP_STAR_STATION_NUMBER, 32, 63);
    if (stationNumberSlot != -1)
    {
        stationNumber = &gTasks[stationNumberSlot];
        stationNumber->pixelX = x;
        stationNumber->pixelY = y - 28;
        stationNumber->posX = x << 16;
        stationNumber->posY = (y - 28) << 16;
        stationNumber->warpStarStationNumberFrame = a;
    }
    return stationNumberSlot;
}

void Task_WarpStarStationNumber(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gWarpStarStationNumberFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD800;
    u->frame = u->warpStarStationNumberFrame;
    TaskSleepForever();
}

s32 CreateWarpStarStationLevelSign(s32 a)
{
    s32 levelSignSlot;
    struct Task *levelSign;

    levelSignSlot = TaskCreateInRange(TASK_WARP_STAR_STATION_LEVEL_SIGN, 32, 63);
    if (levelSignSlot != -1)
    {
        levelSign = &gTasks[levelSignSlot];
        levelSign->pixelX = 128;
        levelSign->pixelY = 420;
        levelSign->posX = 128 << 16;
        levelSign->posY = 420 << 16;
        levelSign->warpStarStationLevelSignFrame = a;
    }
    return levelSignSlot;
}

void Task_WarpStarStationLevelSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gWarpStarStationLevelSignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD800;
    u->frame = u->warpStarStationLevelSignFrame;
    TaskSleepForever();
}

s32 CreateMuseumAbilitySigns(u8 a)
{
    s32 i;

    if (a <= 5)
    {
        LoadMuseumAbilitySignGfx(a);
        for (i = 0; i <= 1; i++)
        {
            if (gUnk_087328C0[a][i] != -1)
                CreateMuseumAbilitySign(a, i);
        }
    }
}

s32 CreateMuseumAbilitySign(u8 a, s32 b)
{
    s32 museumAbilitySignSlot;
    struct Task *museumAbilitySign;

    museumAbilitySignSlot = TaskCreateInRange(TASK_MUSEUM_ABILITY_SIGN, 32, 63);
    if (museumAbilitySignSlot != -1)
    {
        museumAbilitySign = &gTasks[museumAbilitySignSlot];
        museumAbilitySign->pixelX = gUnk_087328C0[a][b];
        museumAbilitySign->pixelY = 96;
        museumAbilitySign->posX = museumAbilitySign->pixelX << 16;
        museumAbilitySign->posY = museumAbilitySign->pixelY << 16;
        museumAbilitySign->museumAbilitySignFrame = b;
    }
    return museumAbilitySignSlot;
}

void Task_MuseumAbilitySign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gMuseumAbilitySignFrames;
    t->layer = 15;
    u = gCurTask;
    u->tileWord = 0xD3C0;
    u->frame = u->museumAbilitySignFrame;
    TaskSleepForever();
}

s32 CreateStageEffect(s32 a, s32 x, s32 y)
{
    s32 stageEffectSlot;
    struct Task *stageEffect;

    stageEffectSlot = TaskCreateInRange(TASK_STAGE_EFFECT, 32, 63);
    if (stageEffectSlot != -1)
    {
        stageEffect = &gTasks[stageEffectSlot];
        stageEffect->pixelX = x;
        stageEffect->pixelY = y;
        stageEffect->posX = x << 16;
        stageEffect->posY = y << 16;
        stageEffect->state = a;
        stageEffect->actorKind = 9;
    }
    return stageEffectSlot;
}
