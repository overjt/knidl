#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "menu.h"

/* menutask_0ea0c.c (0x0800EA0C-0x0800F17F, issue #99).
 *
 * Menu sprite and palette tasks, second part.  EraseConfirmDialogUpdate is the
 * per-frame body of task type #243 (two sprites plus a palette
 * cross-fade); task type #244 (Task_EraseFileWipe, body EraseFileWipeDraw) slides a
 * sprite pair in, bobs it and slides it out; task types #254
 * (Task_SoundTestCursors, body SoundTestCursorsUpdate) and #255 (Task_SoundTestPulse) animate the
 * sound-test screen (menu screen 7: the cursor sprites and the palette
 * pulse of the selected column, which stays lit while its song plays);
 * task types #249 (Task_LinkPlayPalettePulse) and #250 (Task_LinkPlayColorCycle) cycle the
 * palettes of the link-play screen (menu screen 8) through BlendColors
 * blends. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);

void EraseConfirmDialogUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    s32 n;
    s32 k;

    if (TaskIsOnScreenNoCamera()) {
        struct Task *s = gCurTask;
        u32 *tbl = s->frameTable;

        QueueSprite(3, tbl[8], 0, 0, s->pixelX, s->pixelY);
        QueueSprite(2, tbl[9], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    }
    t = gCurTask;
    if (t->eraseConfirmDialogBlendRatio == 256) {
        if (++t->eraseConfirmDialogBlendFrom > 1)
            t->eraseConfirmDialogBlendFrom = 0;
        u = gCurTask;
        if (++u->eraseConfirmDialogBlendTo > 1)
            u->eraseConfirmDialogBlendTo = 0;
        gCurTask->eraseConfirmDialogBlendRatio = 0;
    }
    v = gCurTask;
    n = v->eraseConfirmDialogBlendRatio + 32;
    v->eraseConfirmDialogBlendRatio = n;
    if (n > 256)
        v->eraseConfirmDialogBlendRatio = 256;
    k = gMenuChoiceCursor * 2;
    x = gCurTask;
    BlendColors(gUnk_08559BA4[k + x->eraseConfirmDialogBlendFrom], gUnk_08559BA4[k + x->eraseConfirmDialogBlendTo], (u16)x->eraseConfirmDialogBlendRatio, 16, gObjPaletteBank11);
}

void Task_EraseFileWipe(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    t->updateCallback = (u32)EraseFileWipeDraw;
    t->frameTable = gUnk_08755650;
    t->eraseFileWipeOffsetX = 0;
    t->eraseFileWipeShakeY = 0;
    for (t->eraseFileWipeLoopCount = 0; (s16)gCurTask->eraseFileWipeLoopCount <= 9; gCurTask->eraseFileWipeLoopCount++) {
        struct Task *u = gCurTask;

        u->eraseFileWipeOffsetX += ((s16)u->eraseFileWipeLoopCount + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    for (gCurTask->eraseFileWipeLoopCount = 0; (s16)gCurTask->eraseFileWipeLoopCount <= 1; gCurTask->eraseFileWipeLoopCount++) {
        for (gCurTask->eraseFileWipeShakeFrameCount = 0; gCurTask->eraseFileWipeShakeFrameCount <= 1; gCurTask->eraseFileWipeShakeFrameCount++) {
            gCurTask->eraseFileWipeShakeY += 0x60000;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->eraseFileWipeShakeFrameCount = 0; gCurTask->eraseFileWipeShakeFrameCount <= 1; gCurTask->eraseFileWipeShakeFrameCount++) {
            gCurTask->eraseFileWipeShakeY -= 0x60000;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(16);
    for (gCurTask->eraseFileWipeLoopCount = 0; (s16)gCurTask->eraseFileWipeLoopCount <= 9; gCurTask->eraseFileWipeLoopCount++) {
        struct Task *u = gCurTask;

        u->eraseFileWipeOffsetX -= ((s16)u->eraseFileWipeLoopCount + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void EraseFileWipeDraw(void)
{
    if (TaskIsOnScreenNoCamera()) {
        struct Task *t = gCurTask;
        u32 *tbl = t->frameTable;

        QueueSprite(1, tbl[10], 0, 0, (t->eraseFileWipeOffsetX >> 16) - 127, (t->eraseFileWipeShakeY >> 16) + 144);
        QueueSprite(1, tbl[11], 0, 0, 0x16F - (gCurTask->eraseFileWipeOffsetX >> 16), (gCurTask->eraseFileWipeShakeY >> 16) + 144);
    }
}

void Task_SoundTestCursors(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)SoundTestCursorsUpdate;
    t->soundTestCursorsBlendFrom = 0;
    t->soundTestCursorsBlendTo = 1;
    t->soundTestCursorsBlendRatio = 0;
    while (gMenuScreen == 7)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void SoundTestCursorsUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 v;

    if (TaskIsOnScreenNoCamera()) {
        u32 *tbl = gUnk_087556D4;

        QueueSprite(9, tbl[0], 0, 0, gMenuCursor * 52 + 94, 136);
        QueueSprite(8, tbl[1], 0, 0, 94, 136);
        QueueSprite(8, tbl[2], 0, 0, 146, 136);
    }
    t = gCurTask;
    if (t->soundTestCursorsBlendRatio == 256) {
        t->soundTestCursorsBlendFrom ^= 1;
        t->soundTestCursorsBlendTo ^= 1;
        t->soundTestCursorsBlendRatio = 0;
    }
    u = gCurTask;
    v = u->soundTestCursorsBlendRatio + 32;
    u->soundTestCursorsBlendRatio = v;
    if (v > 256)
        u->soundTestCursorsBlendRatio = 256;
    w = gCurTask;
    BlendColors(gUnk_08564F38[w->soundTestCursorsBlendFrom], gUnk_08564F38[w->soundTestCursorsBlendTo], (u16)w->soundTestCursorsBlendRatio, 5, gObjPaletteBank12Color2);
}

void Task_SoundTestPulse(void)
{
    struct Task *t = gCurTask;

    t->soundTestPulseHoldTimer = 0;
    t->soundTestPulseBlendFrom = 0;
    t->soundTestPulseBlendTo = 1;
    t->soundTestPulseBlendRatio = 0;
    t->soundTestPulseActive = 0;
    while (gMenuScreen == 7) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gCurTask;
        if (u->soundTestPulseBlendRatio == 256) {
            if (++u->soundTestPulseBlendFrom > 5) {
                u->soundTestPulseBlendFrom = 0;
                u->soundTestPulseHoldTimer = 4;
                u->soundTestPulseActive = 0;
            }
            v = gCurTask;
            if (++v->soundTestPulseBlendTo > 5)
                v->soundTestPulseBlendTo = 0;
            gCurTask->soundTestPulseBlendRatio = 0;
        }
        v = gCurTask;
        if (v->soundTestPulseHoldTimer != 0) {
            v->soundTestPulseHoldTimer--;
        } else {
            n = v->soundTestPulseBlendRatio + 128;
            v->soundTestPulseBlendRatio = n;
            if (n > 256)
                v->soundTestPulseBlendRatio = 256;
        }
        if (gMenuCursor == 0) {
            if ((s32)gMPlayTable[gSongTable[gSoundTestSelection[gMenuCursor]].ms].info->status >= 0) {
                w = gCurTask;
                BlendColors(gUnk_085634D8[w->soundTestPulseBlendFrom], gUnk_085634D8[w->soundTestPulseBlendTo], (u16)w->soundTestPulseBlendRatio, 16, gBgPaletteBank5[gMenuCursor]);
            } else {
                BlendColors(gUnk_085634D8[5], gUnk_085634D8[5], (u16)gCurTask->soundTestPulseBlendRatio, 16, gBgPaletteBank5[gMenuCursor]);
            }
        } else {
            if (gPressedKeys & 1) {
                struct Task *x = gCurTask;

                x->soundTestPulseActive = 1;
                x->soundTestPulseHoldTimer = 0;
                x->soundTestPulseBlendFrom = 0;
                x->soundTestPulseBlendTo = 1;
                x->soundTestPulseBlendRatio = 0;
            }
            w = gCurTask;
            if (w->soundTestPulseActive != 0)
                BlendColors(gUnk_085634D8[w->soundTestPulseBlendFrom], gUnk_085634D8[w->soundTestPulseBlendTo], (u16)w->soundTestPulseBlendRatio, 16, gBgPaletteBank5[gMenuCursor]);
            else
                BlendColors(gUnk_085634D8[5], gUnk_085634D8[5], (u16)w->soundTestPulseBlendRatio, 16, gBgPaletteBank5[gMenuCursor]);
        }
        RequestCopy(2, (u32)&gUnk_085634D8[0][(gMenuCursor + 6) * 16], (u32)gBgPaletteBank5[(s8)(gMenuCursor ^ 1)], 32);
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void Task_LinkPlayPalettePulse(void)
{
    struct Task *t = gCurTask;

    t->linkPlayPalettePulseActiveIndex = 0;
    t->linkPlayPalettePulseBlendFrom = 0;
    t->linkPlayPalettePulseBlendTo = 1;
    t->linkPlayPalettePulseBlendRatio = 0;
    t->linkPlayPalettePulseFadeStep = 0;
    SetBlend(68, 8, 0, 16);
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u;
        struct Task *v;
        s32 n;

        {
            struct Task *s = gCurTask;

            if (s->linkPlayPalettePulseFadeStep <= 19) {
                s->linkPlayPalettePulseFadeStep++;
                gBldAlphaEva = s->linkPlayPalettePulseFadeStep >> 2;
                gBldAlphaEvb = 16 - gBldAlphaEva;
            }
        }
        u = gCurTask;
        if (u->linkPlayPalettePulseBlendRatio == 256) {
            if (++u->linkPlayPalettePulseBlendFrom > 15) {
                u->linkPlayPalettePulseBlendFrom = 0;
                if (++u->linkPlayPalettePulseActiveIndex > 3)
                    u->linkPlayPalettePulseActiveIndex = 0;
            }
            v = gCurTask;
            if (++v->linkPlayPalettePulseBlendTo > 15)
                v->linkPlayPalettePulseBlendTo = 0;
            gCurTask->linkPlayPalettePulseBlendRatio = 0;
        }
        v = gCurTask;
        n = v->linkPlayPalettePulseBlendRatio + 64;
        v->linkPlayPalettePulseBlendRatio = n;
        if (n > 256)
            v->linkPlayPalettePulseBlendRatio = 256;
        for (gCurTask->linkPlayPalettePulseLoopCount = 0; (s16)gCurTask->linkPlayPalettePulseLoopCount <= 3; gCurTask->linkPlayPalettePulseLoopCount++) {
            struct Task *w = gCurTask;
            s32 i = (s16)w->linkPlayPalettePulseLoopCount;

            if (i == w->linkPlayPalettePulseActiveIndex)
                BlendColors(gUnk_08560DBC[w->linkPlayPalettePulseBlendFrom], gUnk_08560DBC[w->linkPlayPalettePulseBlendTo], (u16)w->linkPlayPalettePulseBlendRatio, 16, gBgPaletteBank4[i]);
            else
                RequestCopy(2, (u32)gUnk_08560F9C, (u32)gBgPaletteBank4[i], 32);
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void Task_LinkPlayColorCycle(void)
{
    struct Task *t = gCurTask;

    t->linkPlayColorCyclePrevScreen = gPrevMenuScreen;
    t->linkPlayColorCycleBlendFrom = 0;
    t->linkPlayColorCycleBlendTo = 1;
    t->linkPlayColorCycleBlendRatio = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gCurTask;
        if (u->linkPlayColorCycleBlendRatio == 256) {
            if (++u->linkPlayColorCycleBlendFrom > 6)
                u->linkPlayColorCycleBlendFrom = 0;
            v = gCurTask;
            if (++v->linkPlayColorCycleBlendTo > 6)
                v->linkPlayColorCycleBlendTo = 0;
            gCurTask->linkPlayColorCycleBlendRatio = 0;
        }
        v = gCurTask;
        n = v->linkPlayColorCycleBlendRatio + 16;
        v->linkPlayColorCycleBlendRatio = n;
        if (n > 256)
            v->linkPlayColorCycleBlendRatio = 256;
        w = gCurTask;
        if (w->linkPlayColorCyclePrevScreen == 3)
            BlendColors(gUnk_08561224[w->linkPlayColorCycleBlendFrom], gUnk_08561224[w->linkPlayColorCycleBlendTo], (u16)w->linkPlayColorCycleBlendRatio, 10, gBgPaletteBank8Color1);
        else
            BlendColors(gUnk_0856342C[w->linkPlayColorCycleBlendFrom], gUnk_0856342C[w->linkPlayColorCycleBlendTo], (u16)w->linkPlayColorCycleBlendRatio, 10, gBgPaletteBank8Color1);
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}
