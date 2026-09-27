#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "cutscene.h"
#include "room.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void BlendColors(s32 a, s32 b, s32 c, s32 d, void *e);
s32 PlaySfx(s32 id);

void sub_08019000(void)
{

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_087553DC;
    gCurTask->updateCallback = (u32)sub_08019590;
    gCurTask->unk70 = 0;
    gCurTask->tileWord = 0x9210;
    LZ77UnCompWram((const void *)gNightmarePowerOrbGfx[3], (void *)0x02026000);
    RequestCopy(4, 0x02026000, 0x06014000, 128 << 6);
    RequestCopy(2, gNightmarePowerOrbGfx[2], 0x03001590, 32);
    gCurTask->posX = 240 << 15;
    gCurTask->posY = 144 << 15;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(88);
    CreateCutsceneActor(56, 32);
    TaskYieldTrampoline(66);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 7;
        TaskYieldTrampoline(2);
        gCurTask->frame |= -1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        gCurTask->frame |= -1;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFA0000;
    gCurTask->accelY = 160 << 7;
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 128 << 9;
        gCurTask->accelY = 0xFFFFF000;
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        gCurTask->accelY = 128 << 5;
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
    TaskSetMotion(0xFFF80000, 160 << 7, 0x5A5A5A5A, 0, 192 << 2, 0x5A5A5A5A);
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->accelY = 0xFFFFFD00;
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->accelX = 0xFFFFF000;
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    PlaySfx(144 << 2);
    gCurTask->accelX = 128 << 9;
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->accelY = 0xFFFFB000;
    gCurTask->frame = 3;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 15);
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_08019590(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 > 23)
        t->unk70 = 0;
    BlendColors((s32)gUnk_082FE0E4, (s32)gUnk_082FE104,
                 gUnk_0874AD44[(s16)gCurTask->unk70], 16, (void *)0x03001590);
    gCurTask->unk70++;
}

void sub_080195ec(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0801a310;
    t->layer = 6;
    gCurTask->frameTable = gUnk_087553FC;
    gCurTask->tileWord = 0xA310;
    gCurTask->posX = 240 << 15;
    gCurTask->posY = 192 << 14;
    TaskStop();
    gCurTask->frame = 0;
    TaskYieldTrampoline(156);
    PlaySfx(138 << 1);
    TaskSetMotion(0xFFFF0000, 160 << 3, 0x5A5A5A5A, 0xFFFB8000, 158 << 7, 0x5A5A5A5A);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(1);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(1);
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    PlaySfx(0x00000115);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFA0000, 158 << 7, 0x5A5A5A5A);
    gCurTask->frame = 12;
    TaskYieldTrampoline(1);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 12;
    TaskYieldTrampoline(1);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 12;
    TaskYieldTrampoline(1);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    PlaySfx(0x00000115);
    gCurTask->velY = 0xFFFCB000;
    gCurTask->accelY = 160 << 7;
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 12;
    TaskYieldTrampoline(1);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(1);
    gCurTask->frame = 11;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 0;
    TaskYieldTrampoline(255);
    gCurTask->frame = 0;
    TaskYieldTrampoline(190);
    gCurTask->velX = 160 << 11;
    gCurTask->velY = 0xFFFB0000;
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 0x0000FFFF;
    TaskSleepForever();
}

void sub_08019b30(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0801a3e4;
    t->layer = 15;
    gCurTask->frameTable = gUnk_08755440;
    gCurTask->tileWord = 0xD350;
    RequestCopy(2, 0x085E0070, (u32)gUnk_03001610, 32);
    gCurTask->posX = 160 << 17;
    gCurTask->posY = 160 << 15;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(5);
        gCurTask->frame = 1;
        TaskYieldTrampoline(5);
        gCurTask->frame = 2;
        TaskYieldTrampoline(5);
        gCurTask->frame = 3;
        TaskYieldTrampoline(5);
        gCurTask->frame = 4;
        TaskYieldTrampoline(5);
        gCurTask->frame = 5;
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 40);
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame = 7;
    TaskYieldTrampoline(2);
    gCurTask->frame = 8;
    TaskYieldTrampoline(2);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_08019c44(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0801a3e4;
    t->layer = 15;
    gCurTask->frameTable = gUnk_0875546C;
    gCurTask->tileWord = 0xD350;
    RequestCopy(2, 0x085E0070, (u32)gUnk_03001610, 32);
    gCurTask->posX = 160 << 17;
    gCurTask->posY = 160 << 15;
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(27);
    gCurTask->frame = 4;
    TaskYieldTrampoline(5);
    gCurTask->frame = 3;
    TaskYieldTrampoline(5);
    gCurTask->frame = 2;
    TaskYieldTrampoline(5);
    gCurTask->frame = 1;
    TaskYieldTrampoline(5);
    gCurTask->frame = 0;
    TaskYieldTrampoline(5);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_08019d30(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0801a3e4;
    t->layer = 14;
    gCurTask->frameTable = gUnk_08755484;
    gCurTask->tileWord = 0xD350;
    RequestCopy(2, 0x085E0070, (u32)gUnk_03001610, 32);
    gCurTask->posX = 160 << 17;
    gCurTask->posY = 160 << 15;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(5);
        gCurTask->frame = 1;
        TaskYieldTrampoline(5);
        gCurTask->frame = 2;
        TaskYieldTrampoline(5);
        gCurTask->frame = 3;
        TaskYieldTrampoline(5);
        gCurTask->frame = 4;
        TaskYieldTrampoline(5);
        gCurTask->frame = 5;
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 39);
    gCurTask->frame = 0;
    TaskYieldTrampoline(5);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(5);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(1);
    gCurTask->frame = 1;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskSleepForever();
}

void sub_08019e48(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 v;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_08019ecc;
    t->posX = ((s16)gSpriteCameraX + 120) << 16;
    t->posY = ((s16)gSpriteCameraY + 90) << 16;
    TaskStop();
    TaskYieldTrampoline(76);
    TaskYieldTrampoline(40);
    u = gCurTask;
    v = 0x10000;
    u->velX = v;
    TaskYieldTrampoline(255);
    TaskYieldTrampoline(20);
    TaskStop();
    TaskYieldTrampoline(190);
    gCurTask->velX = v;
    TaskYieldTrampoline(237);
    TaskStop();
    TaskSleepForever();
}

void sub_08019ecc(void)
{
    struct Task *t = gCurTask;

    SetCameraFocus(t->pixelX, t->pixelY);
}
