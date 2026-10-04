#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "room.h"
#include "player.h"
#include "enemy.h"
#include "ending.h"

/* ending_c7e4c.c (0x080C7E4C-0x080C9003, issue #100).
 *
 * The first ending scene, part 2: task type #100 variants 4-10.
 *   EndingEpilogueKirby / EndingEpilogueKirbyDraw   variant 4, the long scripted sprite, and
 *       its draw callback (follows the task Task.parent while Task.unk28 is
 *       set, cycles a palette blend, draws scaled like it); near the end it
 *       spawns variant 5 (CreateEndingEpilogueStarRod).
 *   EndingEpilogueStarRod   variant 5, an effect drifting away from its spawner.
 *   EndingEpilogueKingDedede / EndingEpilogueKingDededeDraw   variant 6: hidden for 2278 frames, then a
 *       sprite that drifts in, shrinks while it fades in and falls away.
 *   sub_080c88f0   variant 7: after 240 frames spawns the nine variant-8
 *       sprites (sub_080c8924) and calls M07's sub_080269e8.
 *   sub_080c8958   variant 8, nine sprites: the centre one flickers, the
 *       other eight fly outwards along gUnk_08757374[]..gUnk_087573D4[].
 *   sub_080c8cd4 / sub_080c8e88   variant 9, an invisible task the camera
 *       follows (M07's SetCameraFocusOrAnchor) that shakes vertically.
 *   sub_080c8ea8   variant 10: after 900 frames, eleven times, copy the next
 *       4 KiB of BG tiles from the decompression buffer, blend them in, hold
 *       and blend them out; then turn BG0 off. */

/* Not from main.h: this file's view of gObjPalette differs (lesson 3.517). */
extern vu16 gDispCnt;          /* DISPCNT shadow */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u16 gObjPalette[];
extern vu16 gFrameCount;
extern vs16 gBrightness;
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);

/* Task type #100 variant 4: the long scripted sprite of the first ending
   scene; its draw callback EndingEpilogueKirbyDraw cycles the palette blend.  Near
   the end it spawns variant 5 (CreateEndingEpilogueStarRod) with sound 0x121. */
void EndingEpilogueKirby(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)EndingEpilogueKirbyDraw;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08755708;
    gCurTask->tileWord = 0x8810;
    gCurTask->endingEpilogueFollowParent = 1;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(64);
    gCurTask->frame = 0;
    TaskYieldTrampoline(252);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(60);
    gCurTask->frame = 0;
    TaskYieldTrampoline(96);
    gCurTask->frame = -1;
    TaskYieldTrampoline(46);
    gCurTask->frame = 2;
    TaskYieldTrampoline(208);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(170);
    TaskYieldTrampoline(16);
    gCurTask->frame = 4;
    TaskYieldTrampoline(68);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->frame = 6;
        TaskYieldTrampoline(10);
        gCurTask->frame = 8;
        TaskYieldTrampoline(6);
        gCurTask->frame = 10;
        TaskYieldTrampoline(6);
        gCurTask->frame = 12;
        TaskYieldTrampoline(10);
        gCurTask->frame = 10;
        TaskYieldTrampoline(6);
        gCurTask->frame = 8;
        TaskYieldTrampoline(6);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 1);
    gCurTask->frame = 22;
    TaskYieldTrampoline(6);
    gCurTask->frame = 14;
    TaskYieldTrampoline(4);
    gCurTask->frame = 16;
    TaskYieldTrampoline(6);
    gCurTask->frame = 18;
    TaskYieldTrampoline(6);
    gCurTask->frame = 20;
    TaskYieldTrampoline(10);
    gCurTask->frame = 18;
    TaskYieldTrampoline(6);
    gCurTask->frame = 16;
    TaskYieldTrampoline(6);
    gCurTask->frame = 14;
    TaskYieldTrampoline(8);
    gCurTask->frame = 4;
    TaskYieldTrampoline(6);
    gCurTask->frame = 2;
    TaskYieldTrampoline(6);
    gCurTask->frame = 60;
    TaskYieldTrampoline(134);
    gCurTask->frame = 2;
    TaskYieldTrampoline(12);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(152);
    TaskYieldTrampoline(2);
    gCurTask->endingEpilogueFollowParent = 0;
    gCurTask->frame = 24;
    gCurTask->velX = 0x90000;
    gCurTask->velY = -0xFB000;
    gCurTask->accelY = 0x2000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = -0x3E000;
    gCurTask->accelY = 0x2000;
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(28);
    gCurTask->frame = 26;
    TaskYieldTrampoline(2);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->frame = 28;
        TaskYieldTrampoline(2);
        gCurTask->frame = 30;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 6);
    gCurTask->frame = 28;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 32;
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 34;
    TaskYieldTrampoline(8);
    gCurTask->accelX = 0x2000;
    TaskYieldTrampoline(6);
    TaskStop();
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->frame = 36;
        TaskYieldTrampoline(2);
        gCurTask->frame = 38;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 2);
    gCurTask->accelX = -0x2000;
    gCurTask->velY = -0x1C000;
    gCurTask->accelY = 0x8000;
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 42;
    TaskYieldTrampoline(2);
    gCurTask->frame = 44;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->velY = 0x28000;
    gCurTask->accelY = -0x10000;
    CreateEndingEpilogueStarRod();
    PlaySfx(0x121);
    gCurTask->frame = 46;
    TaskYieldTrampoline(4);
    TaskStop();
    gCurTask->frame = 46;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x2B000;
    gCurTask->accelY = 0x2000;
    gCurTask->frame = 47;
    TaskYieldTrampoline(2);
    gCurTask->frame = 48;
    TaskYieldTrampoline(2);
    gCurTask->frame = 49;
    TaskYieldTrampoline(2);
    gCurTask->frame = 50;
    TaskYieldTrampoline(2);
    gCurTask->frame = 51;
    TaskYieldTrampoline(2);
    gCurTask->frame = 52;
    TaskYieldTrampoline(2);
    gCurTask->frame = 53;
    TaskYieldTrampoline(2);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->frame = 48;
        TaskYieldTrampoline(2);
        gCurTask->frame = 54;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 6);
    gCurTask->frame = 55;
    gCurTask->endingEpilogueFollowParent = 1;
    TaskSleepForever();
}

/* The draw callback task type #100 variant 4 installs: follows the task
   Task.parent while Task.unk28 is set, cycles a palette blend, and draws the
   sprite scaled like the followed task. */
void EndingEpilogueKirbyDraw(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;
    s32 k;
    s32 n;
    s16 s;

    if (gCurTask->endingEpilogueFollowParent != 0) {
        gCurTask->pixelX = gTasks[gCurTask->parent].pixelX;
        gCurTask->pixelY = gTasks[gCurTask->parent].pixelY;
        gCurTask->posX = gCurTask->pixelX << 16;
        gCurTask->posY = gCurTask->pixelY << 16;
    }
    p = &gTasks[gCurTask->parent];
    if (gCurTask->frameTable == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    k = 0;
    if ((u16)gCurTask->frame <= 45 || gCurTask->frame == 60) {
        if ((gFrameCount & 3) == 0)
            k = 1;
        n = gCurTask->endingEpilogueFlashPhase + 1;
        gCurTask->endingEpilogueFlashPhase = n;
        if (n < 0)
            gCurTask->endingEpilogueFlashPhase = 0;
        if (gCurTask->endingEpilogueFlashPhase > 10)
            gCurTask->endingEpilogueFlashPhase = 0;
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, gUnk_08757368[gCurTask->endingEpilogueFlashPhase], 16,
                     &gObjPalette[((gCurTask->tileWord >> 12) + 2) * 16]);
    }
    t = gCurTask;
    if ((u16)(t->pixelX + 63) <= 366 && t->pixelY > -64 && t->pixelY <= 223) {
        tbl = t->frameTable;
        s = gUnk_0873FF98[p->endingEpilogueScale >> 16];
        if ((u16)s != 0x100)
            QueueSprite(gCurTask->layer, DrawAffineSprite(tbl[t->frame + k], s, s, 0),
                         gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX, gCurTask->pixelY);
        else
            QueueSprite(t->layer, tbl[t->frame + k], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
}

/* Spawn task type #100 variant 5 next to the calling task (Task.parent). */
void CreateEndingEpilogueStarRod(void)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(TASK_ENDING_EPILOGUE, 32);
    t = &gTasks[id];
    t->variant = ENDING_EPILOGUE_VARIANT_STAR_ROD;
    t->parent = gCurTaskIdx;
}

/* Task type #100 variant 5: an effect that starts next to the task that
   spawned it (Task.parent), drifts up and left, and loops a 16-frame
   animation. */
void EndingEpilogueStarRod(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreenOrFree;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_0875581C;
    gCurTask->tileWord = 0xA000;
    gCurTask->posX = (gTasks[gCurTask->parent].pixelX - 8) << 16;
    gCurTask->posY = (gTasks[gCurTask->parent].pixelY - 4) << 16;
    gCurTask->velX = -0x40000;
    gCurTask->velY = -0x30000;
    for (;;) {
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 4;
        TaskYieldTrampoline(2);
        gCurTask->frame = 5;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        gCurTask->frame = 7;
        TaskYieldTrampoline(2);
        gCurTask->frame = 8;
        TaskYieldTrampoline(2);
        gCurTask->frame = 9;
        TaskYieldTrampoline(2);
        gCurTask->frame = 10;
        TaskYieldTrampoline(2);
        gCurTask->frame = 11;
        TaskYieldTrampoline(2);
        gCurTask->frame = 12;
        TaskYieldTrampoline(2);
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
    }
}

/* Task type #100 variant 6: hidden for 2278 frames, then a sprite that
   drifts in from (224, 208), shrinks from 63.0 while it fades in, and
   falls away. */
void EndingEpilogueKingDedede(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)EndingEpilogueKingDededeDraw;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08755708;
    gCurTask->tileWord = 0x8810;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(78);
    gCurTask->posX = 224 << 16;
    gCurTask->posY = 208 << 16;
    gCurTask->endingEpilogueScale = 63 << 16;
    gCurTask->endingEpilogueScaleSpeed = 0;
    gCurTask->frame = 64;
    gCurTask->velX = -0x1200;
    gCurTask->velY = -0x4400;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x800;
    gCurTask->velY = 0;
    TaskYieldTrampoline(64);
    gCurTask->velX = 0x400;
    gCurTask->velY = -0x5000;
    TaskYieldTrampoline(72);
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x200, 0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(24);
    gCurTask->endingEpilogueFadeStep = 5;
    gCurTask->endingEpilogueScaleSpeed = -0x71C7;
    TaskSetMotion(-0x4000, 0, 0x5A5A5A5A, 0x18000, -0x600, 0x5A5A5A5A);
    TaskYieldTrampoline(72);
    gCurTask->endingEpilogueScale = 63 << 16;
    gCurTask->endingEpilogueScaleSpeed = 0;
    gCurTask->endingEpilogueFadeStep = 0;
    gCurTask->layer = 13;
    gCurTask->accelX = 0x800;
    gCurTask->frame = 66;
    TaskYieldTrampoline(12);
    gCurTask->frame = 67;
    TaskYieldTrampoline(12);
    gCurTask->accelY = -0x400;
    gCurTask->frame = 68;
    TaskYieldTrampoline(60);
    TaskStop();
    TaskExitTrampoline();
}

/* The draw callback task type #100 variant 6 installs: grows the scale
   Task.unk18 by Task.unk28 (up to 127.0), fades the palette in by Task.unk2C
   per frame, and draws the sprite scaled, frame 64 alternating with 65. */
void EndingEpilogueKingDededeDraw(void)
{
    struct Task *t;
    u32 *tbl;
    s32 k;
    s16 s;

    gCurTask->endingEpilogueScale += gCurTask->endingEpilogueScaleSpeed;
    if (gCurTask->endingEpilogueScale < 0)
        gCurTask->endingEpilogueScale = 0;
    if (gCurTask->endingEpilogueScale >> 16 > 127)
        gCurTask->endingEpilogueScale = 127 << 16;
    if (gCurTask->endingEpilogueFadeStep > 0) {
        gCurTask->endingEpilogueFadeLevel += gCurTask->endingEpilogueFadeStep;
        if (gCurTask->endingEpilogueFadeLevel > 255)
            gCurTask->endingEpilogueFadeLevel = 255;
        BlendColors(gUnk_0859A0F0, gUnk_0859A110, (u16)gCurTask->endingEpilogueFadeLevel, 16,
                     &gObjPalette[(gCurTask->tileWord >> 12) * 16]);
    }
    if (gCurTask->frameTable == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    k = 0;
    if (gCurTask->frame == 64) {
        if ((s16)gCurTask->endingEpilogueFramePhase > 3)
            k = 1;
        gCurTask->endingEpilogueFramePhase++;
        if ((s16)gCurTask->endingEpilogueFramePhase > 7)
            gCurTask->endingEpilogueFramePhase = 0;
    }
    t = gCurTask;
    if ((u16)(t->pixelX + 63) <= 366 && t->pixelY > -64 && t->pixelY <= 223) {
        tbl = t->frameTable;
        s = gUnk_0873FF98[t->endingEpilogueScale >> 16];
        if ((u16)s != 0x100)
            QueueSprite(gCurTask->layer, DrawAffineSprite(tbl[t->frame + k], s, s, 0),
                         gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX, gCurTask->pixelY);
        else
            QueueSprite(t->layer, tbl[t->frame + k], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
}

/* Task type #100 variant 7. */
void sub_080c88f0(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    TaskYieldTrampoline(240);
    sub_080c8924();
    PlaySfx(282);
    TaskYieldTrampoline(12);
    sub_080269e8();
    TaskExitTrampoline();
}

/* Spawn the nine task type #100 variant-8 sprites (Task.unk74 = 0..8). */
void sub_080c8924(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i <= 8; i++) {
        id = TaskCreateFrom(TASK_ENDING_EPILOGUE, 32);
        t = &gTasks[id];
        t->variant = 8;
        t->endingEpilogueIndex = i;
    }
}

/* Task type #100 variant 8 (nine of them, Task.unk74 = 0 .. 8, spawned by
   sub_080c8924): at (103, 52) the centre one (0) flickers through its
   frames, the other eight fly outwards along gUnk_08757374[] ..
   gUnk_087573D4[] while their frames blink. */
void sub_080c8958(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0874CEE8;
    gCurTask->tileWord = 0;
    gCurTask->posX = 103 << 16;
    gCurTask->posY = 52 << 16;
    switch (gCurTask->endingEpilogueIndex) {
    case 0:
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame |= -1;
            TaskYieldTrampoline(3);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(3);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 9);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame |= -1;
            TaskYieldTrampoline(4);
            gCurTask->frame = 14;
            TaskYieldTrampoline(1);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(4);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 11);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(5);
            gCurTask->frame = 14;
            TaskYieldTrampoline(1);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 3);
        break;
    case 1 ... 8:
        gCurTask->velX = gUnk_08757374[gCurTask->endingEpilogueIndex - 1];
        gCurTask->velY = gUnk_08757394[gCurTask->endingEpilogueIndex - 1];
        gCurTask->frame = 4;
        TaskYieldTrampoline(3);
        gCurTask->velX = gUnk_087573B4[gCurTask->endingEpilogueIndex - 1];
        gCurTask->velY = gUnk_087573D4[gCurTask->endingEpilogueIndex - 1];
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 6;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(2);
            gCurTask->frame = 8;
            TaskYieldTrampoline(2);
            gCurTask->frame = -1;
            TaskYieldTrampoline(2);
            gCurTask->frame = 7;
            TaskYieldTrampoline(2);
            gCurTask->frame = -1;
            TaskYieldTrampoline(2);
            gCurTask->frame = 9;
            TaskYieldTrampoline(2);
            gCurTask->frame = -1;
            TaskYieldTrampoline(2);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 11);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 8;
            TaskYieldTrampoline(2);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(2);
            gCurTask->frame = 9;
            TaskYieldTrampoline(2);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(2);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 3);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 10;
            TaskYieldTrampoline(1);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(2);
            gCurTask->frame = 11;
            TaskYieldTrampoline(1);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(2);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 1);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(3);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 2);
        gCurTask->endingEpilogueLoopCount = 0;
        do {
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(3);
            gCurTask->endingEpilogueLoopCount++;
        } while ((s16)gCurTask->endingEpilogueLoopCount <= 2);
        break;
    }
    TaskExitTrampoline();
}

/* Task type #100 variant 9: an invisible task at the camera's (120, 80)
   that shakes vertically with a growing and then shrinking amplitude
   (Task.updateCallback = sub_080c8e88 follows it), with a 4-frame flash between. */
void sub_080c8cd4(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)sub_080c8e88;
    gCurTask->posX = (gSpriteCameraX + 120) << 16;
    gCurTask->posY = (gSpriteCameraY + 80) << 16;
    TaskStop();
    TaskYieldTrampoline(60);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 14);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 14);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->velY = 0x30000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x30000;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 13);
    TaskStop();
    gBrightness = 31;
    TaskYieldTrampoline(4);
    gBrightness = 0;
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 4);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 4);
    TaskStop();
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(64);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(54);
    gCurTask->velY = 0x3000;
    TaskSleepForever();
}

/* Task type #100 variant 9's per-frame hook: move the camera target to
   the task's position (M07's SetCameraFocusOrAnchor). */
void sub_080c8e88(void)
{
    struct Task *t = gCurTask;

    SetCameraFocusOrAnchor(t->pixelX, t->pixelY);
}

/* Task type #100 variant 10: after 900 frames, eleven times: copy the next
   4 KiB of BG tiles from the decompression buffer, blend them in over 32
   frames, hold 224 frames and blend them out again; then clear the
   blend registers and turn BG0 off. */
void sub_080c8ea8(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    gBldCntTarget1 = 65;
    gBldCntTarget2 = 28;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        TaskYieldTrampoline(60);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 14);
    gCurTask->endingEpilogueLoopCount = 0;
    do {
        RequestCopy(1, (u32)gUnk_02020000 + ((s16)gCurTask->endingEpilogueLoopCount << 12), BG_VRAM, 0x1000);
        gCurTask->endingEpilogueBlendStep = 0;
        do {
            gBldAlphaEva = (gCurTask->endingEpilogueBlendStep + 1) >> 1;
            gBldAlphaEvb = 16 - gBldAlphaEva;
            TaskYieldTrampoline(1);
            gCurTask->endingEpilogueBlendStep++;
        } while (gCurTask->endingEpilogueBlendStep <= 31);
        TaskYieldTrampoline(224);
        gCurTask->endingEpilogueBlendStep = 0;
        do {
            gBldAlphaEvb = (gCurTask->endingEpilogueBlendStep + 1) >> 1;
            gBldAlphaEva = 16 - gBldAlphaEvb;
            TaskYieldTrampoline(1);
            gCurTask->endingEpilogueBlendStep++;
        } while (gCurTask->endingEpilogueBlendStep <= 31);
        TaskYieldTrampoline(12);
        gCurTask->endingEpilogueLoopCount++;
    } while ((s16)gCurTask->endingEpilogueLoopCount <= 10);
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
    gDispCnt &= 0xFEFF;
    TaskExitTrampoline();
}
