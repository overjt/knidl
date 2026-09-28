#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "cutscene.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* Not from main.h: this file's view of gPlayerPressedKeys differs (lesson
   3.517). */
extern u8 gObjPalette[];
extern u16 gPlayerPressedKeys;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void CallTableEntry(u32 a, u32 b, u32 *c);
void TaskSleepForever(void);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskSetFrame(s32 a);

s32 CreateCutsceneActor(s32 a, s32 b)
{
    s32 i;
    struct Task *t;
    struct Task *dst;

    i = TaskCreateInRange(92, b, 62);
    if (i != -1)
    {
        dst = &gTasks[i];
        t = gCurTask;
        dst->pixelX = t->pixelX;
        dst->pixelY = t->pixelY;
        dst->posX = t->posX;
        dst->posY = t->posY;
        dst->facing = t->facing;
        dst->parent = gCurTaskIdx;
        dst->unk18 = a;
        if (gCutsceneSheets[*(s8 *)&gCurLevel] != 0)
            dst->tileWord = 0x8810;
    }
    return i;
}

void CutsceneCheckSkip(void);   /* hdr.c lacks this in-module prototype */

void Task_CutsceneDirector(void)
{
    u16 *p;
    u16 *q;

    gCurTask->unk28 = 0;
    CallTableEntry(*(s8 *)&gCurLevel, 9, gCutsceneStarts);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)CutsceneCheckSkip;
    gCurTask->unk6C = 0;
    p = (u16 *)gCutsceneDurations;
    if ((s16)gCurTask->unk6C < p[*(s8 *)&gCurLevel] - 60)
    {
        q = (u16 *)gCutsceneDurations;
        do
        {
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C < q[*(s8 *)&gCurLevel] - 60);
    }
    gGameState = 5;
    TaskSleepForever();
}

void CutsceneCheckSkip(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        u16 *p = &gPlayerPressedKeys;

        if ((p[i] & 9) != 0)
        {
            if (*(s8 *)&gCurLevel == 7 && gUnk_02007D00[0] != -1)
            {
                StopSfxOnPlayer(gUnk_02007D00[0], 0x21B);
                gUnk_02007D00[0] = -1;
            }
            gGameState = 5;
            TaskFree(gCurTaskIdx);
        }
    }
}

void Task_CutsceneActor(void)
{
    CallTableEntry(gCurTask->unk18, 63, gCutsceneActors);
}

void CutsceneDuelStart(void)
{
    PlayBgm(0);
    CreateCutsceneActor(0, 0);
    CreateCutsceneActor(4, 32);
}

void CutsceneDuelKirby(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754A14;
    gCurTask->posX = 150 << 16;
    gCurTask->posY = 186 << 15;
    TaskStop();
    gCurTask->frame = 40;
    TaskYieldTrampoline(20);
    gCurTask->frame = 38;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->frame = 37;
    TaskYieldTrampoline(8);
    gCurTask->accelX = 0xFFFFE000;
    gCurTask->frame = 38;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 39;
    TaskYieldTrampoline(6);
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 38;
    TaskYieldTrampoline(4);
    TaskStop();
    gCurTask->frame = 40;
    TaskYieldTrampoline(20);
    gCurTask->frame = 38;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->frame = 37;
    TaskYieldTrampoline(8);
    gCurTask->accelX = 0xFFFFE000;
    gCurTask->frame = 38;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 39;
    TaskYieldTrampoline(10);
    gCurTask->accelX = 128 << 2;
    gCurTask->frame = 41;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        TaskStop();
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 15);
    TaskSetMotion(0xFFFE0000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->frame = 17;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->frame = 18;
    TaskYieldTrampoline(2);
    CreateCutsceneActor(1, 32);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 19;
        TaskYieldTrampoline(3);
        gCurTask->frame = 20;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->frame = 21;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFC0000;
    gCurTask->frame = 23;
    TaskYieldTrampoline(2);
    gCurTask->velX = 192 << 11;
    gCurTask->frame = 24;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFD0000;
    gCurTask->frame = 25;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 10;
    gCurTask->frame = 26;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 27;
    TaskYieldTrampoline(2);
    CreateCutsceneActor(2, 32);
    TaskStop();
    gCurTask->frame = 28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 30;
    TaskYieldTrampoline(4);
    gCurTask->frame = 31;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(2);
    gCurTask->frame = 30;
    TaskYieldTrampoline(2);
    gCurTask->frame = 22;
    TaskYieldTrampoline(8);
    gCurTask->frame = 32;
    TaskYieldTrampoline(2);
    gCurTask->frame = 33;
    TaskYieldTrampoline(2);
    gCurTask->frame = 34;
    TaskYieldTrampoline(2);
    gCurTask->frame = 35;
    TaskYieldTrampoline(2);
    gCurTask->frame = 36;
    TaskSleepForever();
}

/* OBJ VRAM tile base; not in hdr.c. */
extern u8 gObjVram[];

void sub_08010834(void)
{
    s32 t;
    s32 u;
    u8 *dst = gObjVram;

    gUnk_03001F2C = 0;
    do
    {
        gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk00 = 0;
        gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk04 = 0;
        gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk08 = 0;
        gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0C = 1;
        gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0D = 0;
        gUnk_03001F2C++;
    } while (gUnk_03001F2C <= 2);
    RequestCopy(1, (u32)gUnk_081AC378, (u32)(dst + 384), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 128, (u32)(dst + 1408), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 256, (u32)(dst + 2432), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 384, (u32)(dst + 3456), 128);
    RequestCopy(2, (u32)gUnk_081AC358, (u32)gObjPalette, 32);
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)sub_08010b38;
    gCurTask->updateCallback = (u32)sub_080109c8;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_08751C44;
    gCurTask->tileWord = 0x1004;
    gCurTask->unk28 = 0;
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask->unk28;
        u = *(s16 *)&gCurTask->unk28;
        gCurTask->unk28 = t + 1;
        TaskSetFrame(u);
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    TaskExitTrampoline();
}
