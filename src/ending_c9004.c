#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "sound.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "enemy.h"
#include "ending.h"

/* ending_c9004.c (0x080C9004-0x080CAA3B, issue #100).
 *
 * The second ending scene: task type #101 (class 3), which M37's
 * EndingStarRodReturnScene spawns in AgbMain state 11 and waits on (gEndingSceneActive).
 *   Task_EndingStarRodReturn   the body: variant 0 loads the graphics (EndingStarRodReturnLoadGraphics) and
 *       spawns variants 1, 3, 6, 7, 8 and 11 from the list gEndingStarRodReturnObjectVariants
 *       (CreateEndingStarRodReturnObjects); variants 1-11 run gEndingStarRodReturnVariants[Task.variant].
 *   EndingStarRodReturnStarRod / EndingStarRodReturnStarRodDraw   variant 1, a sprite that falls in, then
 *       rides the BG3 layer and flashes its palette.
 *   EndingStarRodReturnConvergingStars   variant 2, a 22-frame animation played six times.
 *   EndingStarRodReturnWarpStar / EndingStarRodReturnWarpStarUpdate / EndingStarRodReturnWarpStarDraw   variant 3, which spawns its
 *       variant-5 companion, crosses the screen twice leaving variant-4
 *       tasks behind, fades the music out and ends the scene (clearing
 *       gEndingSceneActive).
 *   EndingStarRodReturnTrailStar, EndingStarRodReturnKirby / EndingStarRodReturnKirbyDraw   variants 4 and 5.
 *   EndingStarRodReturnFountainJet, sub_080ca830, sub_080ca8f0   variants 6-8, scripted
 *       sprites drawn by M05's FountainSpriteDraw.
 *   EndingStarRodReturnBurstStar   variant 9, eleven sprites bursting out of one point.
 *   EndingStarRodReturnFallingStar / EndingStarRodReturnFallingStarDriftFast / EndingStarRodReturnFallingStarDriftSlow   variant 10, sixteen falling
 *       sprites with two sway scripts.
 *   sub_080c9a28 / sub_080c9cf0   variant 11, the finale: palette flashes,
 *       variants 2, 9 and 10, a fade to an OBJ-only display. */

/* Not from main.h: this file's view of gObjPalette differs (lesson 3.517). */
extern vu16 gDispCnt;          /* DISPCNT shadow */
extern vs32 gBg3ScrollY;
extern vs32 gBg3ScrollX;          /* ... BG3 */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u8 gObjVram[];         /* OBJ VRAM */
extern u16 gObjPalette[];
extern u16 gBgPalette[];
extern vu16 gFadeSteps;
extern vs16 gBrightness;
extern vs16 gFadeStep;
extern vu16 gFadeTimer;
extern vu16 gFadeInterval;
extern vu16 gFadeBlankAtWhite;
extern u16 *gFadeKeepMask;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                                 /* random 0 .. range-1 */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
u32 TaskIsOnScreen(void);
u32 TaskIsInView(void);
void TaskDrawWorld(void);
void TaskSleepForever(void);                                     /* end the running task */
void TaskStop(void);

/* Task type #101 (class 3): variant 0 loads the graphics and spawns the
   other variants; variants 1-11 run the anchor table gEndingStarRodReturnVariants[]. */
void Task_EndingStarRodReturn(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    if (gCurTask->variant == 0) {
        EndingStarRodReturnLoadGraphics();
        CreateEndingStarRodReturnObjects();
    } else {
        CallTableEntry(gCurTask->variant, 12, gEndingStarRodReturnVariants);
    }
    TaskExitTrampoline();
}

/* Task type #101's graphics. */
void EndingStarRodReturnLoadGraphics(void)
{
    struct GfxHeader *h = &gUnk_0859A09C;

    LZ77UnCompWram(h->tiles, gUnk_02020000);
    RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->tileCount << 5);
    RequestCopy(2, (u32)h->palette, (u32)gObjPaletteBank8, h->paletteBankCount << 5);
    if (gEndingPlayerCount > 1)
        RequestCopy(2, (u32)gPlayerPalettes[gEndingLocalPlayer], (u32)gObjPaletteBank8, 22);
    RequestCopy(2, (u32)gUnk_085E0070, (u32)&gObjPaletteBank8[80], 32);
    LZ77UnCompWram(gUnk_085E0090, gUnk_02020000);
}

/* Spawn one task type #101 per variant listed in gEndingStarRodReturnObjectVariants[] (1, 3, 6,
   7, 8 and 11, ended by 12, the anchor table's size). */
void CreateEndingStarRodReturnObjects(void)
{
    s32 i;
    s32 starRodReturnSlot;
    s32 v;
    struct Task *t;

    for (i = 0; v = gEndingStarRodReturnObjectVariants[i], (s16)gEndingStarRodReturnObjectVariants[i] <= 11; i++) {
        starRodReturnSlot = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
        if (starRodReturnSlot == -1)
            for (;;)
                ;
        t = &gTasks[starRodReturnSlot];
        t->variant = v;
    }
}

/* Task type #101 variant 1: a sprite at the camera's (120, -24) that falls
   in, plays a 16-frame animation twice, then pins itself to the BG3 layer
   (EndingStarRodReturnStarRodDraw) and flashes its palette ever faster. */
void EndingStarRodReturnStarRod(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 11;
    gCurTask->frameTable = gEndingStarRodReturnFrames;
    gCurTask->tileWord = 0x8810;
    gCurTask->posX = (gSpriteCameraX + 120) << 16;
    gCurTask->posY = (gSpriteCameraY - 24) << 16;
    TaskYieldTrampoline(32);
    TaskStop();
    gCurTask->velY = 0x10000;
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
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
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
        gCurTask->frame = 16;
        TaskYieldTrampoline(2);
        gCurTask->frame = 17;
        TaskYieldTrampoline(2);
        gCurTask->frame = 18;
        TaskYieldTrampoline(2);
        gCurTask->frame = 19;
        TaskYieldTrampoline(2);
        gCurTask->frame = 20;
        TaskYieldTrampoline(2);
        gCurTask->frame = 21;
        TaskYieldTrampoline(2);
        gCurTask->frame = 22;
        TaskYieldTrampoline(2);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 1);
    gCurTask->frame = 23;
    TaskYieldTrampoline(3);
    TaskStop();
    PlaySfx(238);
    gCurTask->drawCallback = (u32)EndingStarRodReturnStarRodDraw;
    gCurTask->posX = (gCurTask->pixelX - gSpriteCameraX + (gBg3ScrollX >> 16)) << 16;
    gCurTask->posY = (gCurTask->pixelY - gSpriteCameraY + (gBg3ScrollY >> 16)) << 16;
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 11);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 11);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->tileWord >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 10);
    TaskYieldTrampoline(96);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void EndingStarRodReturnStarRodDraw(void)
{
    struct Task *t;
    u32 *g;

    if (gCurTask->frameTable == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    if (TaskIsInView() == 0)
        return;
    t = gCurTask;
    if (t->pixelX - (gBg3ScrollX >> 16) > -64 && t->pixelX - (gBg3ScrollX >> 16) <= 303
        && t->pixelY - (gBg3ScrollY >> 16) > -64 && t->pixelY - (gBg3ScrollY >> 16) <= 223) {
        g = t->frameTable;
        QueueSprite(t->layer, g[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - (gBg3ScrollX >> 16), t->pixelY - (gBg3ScrollY >> 16));
    }
}

/* Task type #101 variant 3: spawns its variant-5 companion (which follows
   it), flies across the screen twice leaving a trail of variant-4 tasks
   (EndingStarRodReturnWarpStarUpdate while Task.unk28 is set), then fades the music out
   (EndingStarRodReturnWarpStarDraw) and clears gEndingSceneActive. */
void EndingStarRodReturnWarpStar(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)EndingStarRodReturnWarpStarDraw;
    gCurTask->layer = 8;
    gCurTask->frameTable = gEndingStarRodReturnFrames;
    gCurTask->tileWord = 0x8810;
    gCurTask->updateCallback = (u32)EndingStarRodReturnWarpStarUpdate;
    gCurTask->endingStarRodReturnBgmVolume = 255;
    gCurTask->endingStarRodReturnBgmFadeStep = 0;
    gCurTask->endingStarRodReturnChildSlot = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
    gTasks[gCurTask->endingStarRodReturnChildSlot].variant = 5;
    gTasks[gCurTask->endingStarRodReturnChildSlot].parent = gCurTaskIdx;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(99);
    TaskYieldTrampoline(96);
    gCurTask->endingStarRodReturnTrailOn = 1;
    gCurTask->posX = (gSpriteCameraX + 255) << 16;
    gCurTask->posY = (gSpriteCameraY + 80) << 16;
    PlaySfx(290);
    gCurTask->frame = 5;
    gCurTask->velX = -0x80000;
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0xC000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x14000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(13);
    TaskStop();
    gCurTask->endingStarRodReturnTrailOn = 0;
    TaskYieldTrampoline(70);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(97);
    TaskYieldTrampoline(210);
    PlaySfx(291);
    gCurTask->posX = (gSpriteCameraX - 16) << 16;
    gCurTask->posY = (gSpriteCameraY + 176) << 16;
    gCurTask->velX = 0x80000;
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x14000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(24);
    TaskYieldTrampoline(20);
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = (gCurTask->pixelY << 16) - 1;
    gCurTask->velX = 0xE000;
    gCurTask->velY = -0xE000;
    TaskYieldTrampoline(20);
    gCurTask->velX = 0xC000;
    gCurTask->velY = -0xC000;
    TaskYieldTrampoline(80);
    TaskYieldTrampoline(82);
    gCurTask->endingStarRodReturnBgmFadeStep = 2;
    TaskYieldTrampoline(128);
    gEndingSceneActive = 0;
    TaskSleepForever();
}

/* Task type #101 variant 3's per-frame callback: while Task.unk28 is set,
   spawn a variant-4 task at this task's position every third frame. */
void EndingStarRodReturnWarpStarUpdate(void)
{
    s32 id;
    struct Task *t;

    if (gCurTask->endingStarRodReturnTrailOn != 0) {
        if (--gCurTask->endingStarRodReturnTrailTimer <= 0) {
            id = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
            t = &gTasks[id];
            t->variant = ENDING_STAR_ROD_RETURN_VARIANT_TRAIL_STAR;
            t->pixelX = gCurTask->pixelX;
            t->pixelY = gCurTask->pixelY;
            t->posX = t->pixelX << 16;
            t->posY = t->pixelY << 16;
        }
        if (gCurTask->endingStarRodReturnTrailTimer <= 0)
            gCurTask->endingStarRodReturnTrailTimer = 3;
    }
}

/* Task type #101 variant 3's draw callback: cycle the frame offset through
   gUnk_08757432[6], draw, and fade the music out by Task.unk24 per frame
   (volume in Task.unk34). */
void EndingStarRodReturnWarpStarDraw(void)
{
    s16 off;

    if ((s16)gCurTask->endingStarRodReturnFramePhase < 0)
        gCurTask->endingStarRodReturnFramePhase = 0;
    if ((s16)gCurTask->endingStarRodReturnFramePhase > 5)
        gCurTask->endingStarRodReturnFramePhase = 0;
    off = gUnk_08757432[(s16)gCurTask->endingStarRodReturnFramePhase];
    gCurTask->endingStarRodReturnFramePhase++;
    if (gCurTask->frameTable != NULL && gCurTask->frame != -1 && TaskIsOnScreen() != 0) {
        struct Task *u = gCurTask;
        u32 *g = u->frameTable;

        QueueSprite(u->layer, g[u->frame + off], u->spriteFlags, u->tileWord,
                     u->pixelX - gSpriteCameraX, u->pixelY - gSpriteCameraY);
    }
    if (gCurTask->endingStarRodReturnBgmFadeStep != 0) {
        gCurTask->endingStarRodReturnBgmVolume -= gCurTask->endingStarRodReturnBgmFadeStep;
        if (gCurTask->endingStarRodReturnBgmVolume < 0)
            gCurTask->endingStarRodReturnBgmVolume = 0;
        SetBgmVolume(gCurTask->endingStarRodReturnBgmVolume);
    }
}

/* Task type #101 variant 4. */
void EndingStarRodReturnTrailStar(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0874C500;
    gCurTask->tileWord = 0;
    gCurTask->frame = RandomRange(4) + 4;
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(24);
    TaskExitTrampoline();
}

/* Task type #101 variant 5: a sprite drawn at another task's position
   (EndingStarRodReturnKirbyDraw), flipped after 863 frames, then looping a 4-frame
   animation. */
void EndingStarRodReturnKirby(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)EndingStarRodReturnKirbyDraw;
    gCurTask->layer = 7;
    gCurTask->frameTable = gEndingStarRodReturnFrames;
    gCurTask->tileWord = 0x8810;
    gCurTask->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
    gCurTask->frame = 0;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(207);
    gCurTask->spriteFlags |= SPRITE_FLAG_FLIP_X;
    TaskYieldTrampoline(56);
    for (;;) {
        gCurTask->frame = 1;
        TaskYieldTrampoline(5);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame = 4;
        TaskYieldTrampoline(5);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
    }
}

void EndingStarRodReturnKirbyDraw(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    u32 *g;

    t->pixelX = gTasks[t->parent].pixelX;
    t->pixelY = gTasks[t->parent].pixelY;
    if (t->frameTable != NULL && t->frame != -1 && TaskIsOnScreen() != 0) {
        u = gCurTask;
        g = u->frameTable;
        QueueSprite(u->layer, g[u->frame], u->spriteFlags, u->tileWord,
                     u->pixelX - gSpriteCameraX, u->pixelY - gSpriteCameraY);
    }
}

/* Task type #101 variant 11: flash the OBJ palettes, spawn variant 2 and
   eleven variant-9 tasks, fade the screen out, switch the display to OBJ
   only and spawn sixteen variant-10 tasks. */
void sub_080c9a28(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)sub_080c9cf0;
    gCurTask->posX = (gSpriteCameraX + 120) << 16;
    gCurTask->posY = (gSpriteCameraY + 80) << 16;
    TaskStop();
    TaskYieldTrampoline(99);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_085E2920, gUnk_085E2B20, (u16)((s16)gCurTask->endingStarRodReturnLoopCount * 32), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 7);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_085E2B20, gUnk_085E2920, (u16)((s16)gCurTask->endingStarRodReturnLoopCount * 32), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 7);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        BlendColors(gUnk_085E2920, gUnk_085E2A20, (u16)((s16)gCurTask->endingStarRodReturnLoopCount * 16), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 15);
    TaskYieldTrampoline(68);
    TaskYieldTrampoline(80);
    gCurTask->endingStarRodReturnChildSlot = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
    gTasks[gCurTask->endingStarRodReturnChildSlot].variant = 2;
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(30);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        gCurTask->endingStarRodReturnChildSlot = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
        gTasks[gCurTask->endingStarRodReturnChildSlot].variant = 9;
        gTasks[gCurTask->endingStarRodReturnChildSlot].endingStarRodReturnIndex = gCurTask->endingStarRodReturnLoopCount;
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 10);
    PlaySfx(288);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x20000;
    gFadeSteps = 32;
    gBrightness = 0;
    gFadeStep = 1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 0;
    gFadeKeepMask = 0;
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    RequestCopy(2, (u32)gUnk_0875743E, (u32)gBgPalette, 2);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1000;
    gBrightness = 0;
    TaskStop();
    TaskYieldTrampoline(120);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        gCurTask->endingStarRodReturnChildSlot = TaskCreateFrom(TASK_ENDING_STAR_ROD_RETURN, 32);
        gTasks[gCurTask->endingStarRodReturnChildSlot].variant = 10;
        gTasks[gCurTask->endingStarRodReturnChildSlot].endingStarRodReturnIndex = gCurTask->endingStarRodReturnLoopCount;
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 15);
    TaskSleepForever();
}

/* Task type #101 variant 11's per-frame hook: move the camera target to
   the task's position (M07's SetCameraFocusOrAnchor). */
void sub_080c9cf0(void)
{
    struct Task *t = gCurTask;

    SetCameraFocusOrAnchor(t->pixelX, t->pixelY);
}

/* Task type #101 variant 2: a sprite at the camera's (120, 40) that plays
   a 22-frame animation six times. */
void EndingStarRodReturnConvergingStars(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0874CF94;
    gCurTask->tileWord = 0;
    gCurTask->posX = (gSpriteCameraX + 120) << 16;
    gCurTask->posY = (gSpriteCameraY + 40) << 16;
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(1);
        gCurTask->frame = 18;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(1);
        gCurTask->frame = 19;
        TaskYieldTrampoline(1);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 20;
        TaskYieldTrampoline(1);
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(1);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(1);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 5);
    TaskExitTrampoline();
}

/* Task type #101 variant 9: one of eleven sprites that burst out of the
   camera's (120, 48) point; Task.unk74 picks its sprite, frame, direction
   and speed, each slowing down over four steps. */
void EndingStarRodReturnBurstStar(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->posX = 120 << 16;
    gCurTask->posY = 48 << 16;
    gCurTask->tileWord = 0;
    switch (gCurTask->endingStarRodReturnIndex) {
    case 0:
        gCurTask->frameTable = gUnk_0874C500;
        gCurTask->frame = 4;
        gCurTask->velX = 0x30000;
        gCurTask->velY = -0x30000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0x14000;
        gCurTask->velY = -0x14000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0xC000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->velX = 0x4000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 1:
        gCurTask->frameTable = gUnk_0874C500;
        gCurTask->frame = 4;
        gCurTask->velX = -0x30000;
        gCurTask->velY = -0x30000;
        TaskYieldTrampoline(6);
        gCurTask->velX = -0x14000;
        gCurTask->velY = -0x14000;
        TaskYieldTrampoline(12);
        gCurTask->velX = -0xC000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->velX = -0x4000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 2:
        gCurTask->frameTable = gUnk_0874C500;
        gCurTask->frame = 4;
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(6);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(18);
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 3:
        gCurTask->frameTable = gUnk_0874CF28;
        gCurTask->frame = 24;
        gCurTask->velX = 0x14000;
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0xC000;
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->velX = 0x2000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 4:
        gCurTask->frameTable = gUnk_0874C44C;
        gCurTask->frame = 4;
        gCurTask->velX = -0x14000;
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->velX = -0xC000;
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(12);
        gCurTask->velX = -0x8000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->velX = -0x2000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 5:
        gCurTask->frameTable = gUnk_0874C44C;
        gCurTask->frame = 4;
        gCurTask->velX = 0x40000;
        gCurTask->velY = -0x14000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0x20000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0xC000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->velX = 0x4000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 6:
        gCurTask->frameTable = gUnk_0874CF28;
        gCurTask->frame = 26;
        gCurTask->velX = -0x40000;
        gCurTask->velY = -0x14000;
        TaskYieldTrampoline(6);
        gCurTask->velX = -0x20000;
        gCurTask->velY = -0xC000;
        TaskYieldTrampoline(12);
        gCurTask->velX = -0xC000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->velX = -0x4000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 7:
        TaskYieldTrampoline(2);
        gCurTask->frameTable = gUnk_0874CF28;
        gCurTask->frame = 26;
        gCurTask->velX = 0xA000;
        gCurTask->velY = -0x1E000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0x4000;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(18);
        gCurTask->velX = 0x2000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 8:
        TaskYieldTrampoline(2);
        gCurTask->frameTable = gUnk_0874C500;
        gCurTask->frame = 4;
        gCurTask->velX = -0xA000;
        gCurTask->velY = -0x1E000;
        TaskYieldTrampoline(6);
        gCurTask->velX = -0x8000;
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(12);
        gCurTask->velX = -0x4000;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(18);
        gCurTask->velX = -0x2000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 9:
        TaskYieldTrampoline(4);
        gCurTask->frameTable = gUnk_0874C500;
        gCurTask->frame = 4;
        gCurTask->velX = 0x1E000;
        gCurTask->velY = -0xA000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0x10000;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0x8000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->velX = 0x4000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        break;
    case 10:
        TaskYieldTrampoline(4);
        gCurTask->frameTable = gUnk_0874CF28;
        gCurTask->frame = 24;
        gCurTask->velX = -0x1E000;
        gCurTask->velY = -0xA000;
        TaskYieldTrampoline(6);
        gCurTask->velX = -0x10000;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(12);
        gCurTask->velX = -0x8000;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->velX = -0x4000;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        break;
    }
    TaskExitTrampoline();
}

/* Task type #101 variant 10: one of sixteen falling sprites; Task.unk74
   picks its start delay, its column and (0-7 / 8-15) which of the two
   sway scripts it runs, over and over. */
void EndingStarRodReturnFallingStar(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->tileWord = 0;
    for (;;) {
        gCurTask->frame = 0xFFFF;
        TaskStop();
        switch (gCurTask->endingStarRodReturnIndex) {
        case 0:
            TaskYieldTrampoline(30);
            gCurTask->posX = 64 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 1:
            TaskYieldTrampoline(34);
            gCurTask->posX = 96 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 2:
            TaskYieldTrampoline(100);
            gCurTask->posX = 0;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 3:
            TaskYieldTrampoline(160);
            TaskYieldTrampoline(210);
            gCurTask->posX = 128 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 4:
            TaskYieldTrampoline(14);
            TaskYieldTrampoline(210);
            gCurTask->posX = 160 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 5:
            TaskYieldTrampoline(210);
            gCurTask->posX = 32 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 6:
            TaskYieldTrampoline(200);
            TaskYieldTrampoline(210);
            gCurTask->posX = 80 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 7:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(34);
            gCurTask->posX = 192 << 16;
            EndingStarRodReturnFallingStarDriftFast();
            break;
        case 8:
            TaskYieldTrampoline(30);
            gCurTask->posX = 64 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 9:
            TaskYieldTrampoline(90);
            gCurTask->posX = 144 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 10:
            TaskYieldTrampoline(130);
            gCurTask->posX = 96 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 11:
            TaskYieldTrampoline(180);
            gCurTask->posX = 32 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 12:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gCurTask->posX = 128 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 13:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(160);
            gCurTask->posX = 240 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 14:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(130);
            gCurTask->posX = 160 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        case 15:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gCurTask->posX = 192 << 16;
            EndingStarRodReturnFallingStarDriftSlow();
            break;
        }
    }
}

/* Task type #101 variant 10, one of its two falling kinds: sway left and
   right five times while drifting down. */
void EndingStarRodReturnFallingStarDriftFast(void)
{
    gCurTask->frameTable = gUnk_0874C44C;
    gCurTask->frame = RandomRange(2) * 2 + 4;
    gCurTask->posY = -0x80000;
    gCurTask->velY = 0x8000;
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        gCurTask->velX = 0x2000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(20);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x3000;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x800;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x8000;
        TaskYieldTrampoline(20);
        gCurTask->velX = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x800;
        TaskYieldTrampoline(4);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 4);
}

/* Task type #101 variant 10, the other falling kind: sway right and left
   ten times while drifting down. */
void EndingStarRodReturnFallingStarDriftSlow(void)
{
    gCurTask->frameTable = gUnk_0874C500;
    gCurTask->frame = RandomRange(4) + 4;
    gCurTask->posY = -0x80000;
    gCurTask->velY = 0x4000;
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
        gCurTask->velX = -0x2000;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x10000;
        TaskYieldTrampoline(16);
        gCurTask->velX = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = -0x2000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x800;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->velX = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        TaskYieldTrampoline(4);
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 9);
}

/* Task type #101 variant 6. */
void EndingStarRodReturnFountainJet(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)FountainSpriteDraw;
    gCurTask->layer = 11;
    gCurTask->frameTable = gFountainJetFrames;
    gCurTask->tileWord = 0xD350;
    gCurTask->posX = 128 << 16;
    gCurTask->posY = 192 << 16;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(131);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(2);
    gCurTask->frame = 7;
    TaskYieldTrampoline(2);
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
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
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 10);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

/* Task type #101 variant 7. */
void sub_080ca830(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)FountainSpriteDraw;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_0875546C;
    gCurTask->tileWord = 0xD350;
    gCurTask->posX = 128 << 16;
    gCurTask->posY = 192 << 16;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(115);
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
    TaskYieldTrampoline(65);
    gCurTask->frame = 5;
    TaskYieldTrampoline(200);
    gCurTask->frame = 5;
    TaskYieldTrampoline(72);
    gCurTask->frame = -1;
    TaskSleepForever();
}

/* Task type #101 variant 8. */
void sub_080ca8f0(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)FountainSpriteDraw;
    gCurTask->layer = 10;
    gCurTask->frameTable = gUnk_08755484;
    gCurTask->tileWord = 0xD350;
    gCurTask->posX = 128 << 16;
    gCurTask->posY = 192 << 16;
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(140);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->endingStarRodReturnLoopCount = 0;
    do {
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
        gCurTask->endingStarRodReturnLoopCount++;
    } while ((s16)gCurTask->endingStarRodReturnLoopCount <= 9);
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
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}
