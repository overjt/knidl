#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "ending.h"

/* boot_091ac.c (0x080091AC-0x080099FB, issue #96).
 *
 * The boot and title sequence.  AgbMain state 1 (BootLogoMain) runs the
 * skippable logo sequence PlayBootLogo, whose frame waits BootLogoWait(n)
 * return 1 on A/B/START (sub_080093cc is a dead twin) and whose task type
 * #0 (Task_BootLogo) runs every frame.  State 3 (TitleMain) alternates
 * the title screen TitleScreen - task type #1 (Task_TitlePalette) animates the
 * title palette, task type #2 (Task_TitleSprites) spawns ten sprite children
 * (sub_08009640) - with the nine-scene intro story IntroStory. */

u32 BeginFade(u16 steps, u16 delta, u16 *mask);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);

void BootLogoMain(void)
{
    s32 i, vol;

    if (PlayBootLogo() == 1) {
        BeginFade(16, 2, gUnk_08731C88);
        vol = 256;
        for (i = 0; i < 16; i++) {
            SetBgmVolume(vol);
            RunFrame();
            vol -= 16;
        }
        StopAllSfx();
        SetBgmVolume(256);
        ResetTasksAndOam();
    } else {
        gPrevGameState = GAME_STATE_BOOT_LOGO;
    }
}

s32 PlayBootLogo(void)
{
    s32 i;

    ResetTasksAndOam();
    BootLogoInitObjects();
    gLinkPlayerCount = 0x9999;
    LoadBgLayout(0);
    LoadGfxSet(6);
    LoadGfxSet(7);
    gBg0ScrollY = gBg0ScrollX = gBg1ScrollY = gBg1ScrollX = gBg2ScrollY = gBg2ScrollX = gBg3ScrollY = gBg3ScrollX = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1100;
    BeginFastFadeInFromWhite();
    RunFramesUntilFadeDone();
    RunFrames(60);
    TaskCreateFrom(TASK_BOOT_LOGO, 0);
    RunFrames(60);
    gLinkPlayerCount = 1;
    if (BootLogoWait(70) != 0)
        return 1;
    PlaySfx(0x10D);
    if (BootLogoWait(35) != 0)
        return 1;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    if (BootLogoWait(21) != 0)
        return 1;
    gWinIn0 = 49;
    gWinOut = 62;
    gWin0H = 255;
    gWin0V = 160;
    gDispCnt |= 0x2000;
    for (i = 85; i >= 0; i--) {
        gWin0H = i * 3;
        RunFrame();
    }
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1E00;
    gDispCnt &= 0xDFFF;
    return 0;
}

s32 BootLogoWait(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}

s32 sub_080093cc(void)
{
    while (gFadeSteps != 0) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}

void Task_BootLogo(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    for (;;) {
        BootLogoUpdateObjects();
        TaskYieldTrampoline(1);
    }
}

void Task_TitlePalette(void)
{
    struct Task *t = gCurTask;
    s32 *p;
    u16 *q;

    t->moveCallback = 0;
    t->drawCallback = 0;
    for (t->titlePaletteLoopCount = 0; (s16)gCurTask->titlePaletteLoopCount <= 10; gCurTask->titlePaletteLoopCount++) {
        RequestCopy(2, (u32)gUnk_08541D98[(s16)gCurTask->titlePaletteLoopCount], (u32)gBgPaletteBank14, 32);
        TaskYieldTrampoline(1);
    }
    for (gCurTask->titlePaletteLoopCount = 0; (s16)gCurTask->titlePaletteLoopCount <= 11; gCurTask->titlePaletteLoopCount++) {
        BlendColors(gUnk_08541D98[13], gUnk_08541D98[11], (u16)(((s16)gCurTask->titlePaletteLoopCount + 1) * 21), 16, gBgPaletteBank14);
        BlendColors(gUnk_08541F58, gUnk_08541F58 + 8, (u16)(((s16)gCurTask->titlePaletteLoopCount + 1) * 21), 8, gBgPaletteBank14 + 17);
        TaskYieldTrampoline(1);
    }
    for (gCurTask->titlePaletteLoopCount = 0; (s16)gCurTask->titlePaletteLoopCount <= 7; gCurTask->titlePaletteLoopCount++) {
        BlendColors(gUnk_08541D98[11], gUnk_08541D98[12], (u16)(((s16)gCurTask->titlePaletteLoopCount + 1) * 32), 16, gBgPaletteBank14);
        BlendColors(gUnk_08541F58 + 8, gUnk_08541F58, (u16)(((s16)gCurTask->titlePaletteLoopCount + 1) * 32), 8, gBgPaletteBank14 + 17);
        TaskYieldTrampoline(1);
    }
    /* Loop 4: the ROM hoists the store's &gUnk_03001F2C (after the task
       address) but reloads it from the pool for the u16 re-read.  The store
       goes through p, set inside the outer loop so loop.c hoists it in that
       order; the read goes through q, which is also assigned here, before
       the loop.  That dead assignment makes q live outside the loop, so
       loop.c does not merge q's in-loop load with p (combine_movables skips
       loop-global regs).  q's load then stays in the loop, as in the ROM.
       With a plain `(u16)gUnk_03001F2C` read, GCSE (store address in the
       same iteration) or combine_movables (p in the loop) gives the read a
       hoisted register instead. */
    q = (u16 *)&gUnk_03001F2C;
    for (;;) {
        gCurTask->titlePaletteLoopCount = 0;
        p = &gUnk_03001F2C;
        for (; (s16)gCurTask->titlePaletteLoopCount <= 15; gCurTask->titlePaletteLoopCount++) {
            *p = ((s16)gCurTask->titlePaletteLoopCount > 7 ? 16 - (s16)gCurTask->titlePaletteLoopCount : (s16)gCurTask->titlePaletteLoopCount) << 5;
            q = (u16 *)&gUnk_03001F2C;
            BlendColors(gUnk_08541D98[12], gUnk_08541D98[13], *q, 16, gBgPaletteBank14);
            TaskYieldTrampoline(1);
        }
    }
}

void Task_TitleSprites(void)
{
    if (gCurTask->titleSpritesIndex == -1)
        sub_080095e4();
    else
        sub_08009640();
}

void sub_080095e4(void)
{
    struct Task *t;
    s32 idx;

    TaskYieldTrampoline(gCurTask->titleSpritesStartDelay);
    for (gCurTask->titleSpritesLoopCount = 0; (s16)gCurTask->titleSpritesLoopCount < 10; gCurTask->titleSpritesLoopCount++) {
        idx = TaskCreateFrom(TASK_TITLE_SPRITES, 0);
        t = gCurTask;
        t->titleSpritesChildSlot = idx;
        gTasks[idx].titleSpritesIndex = (s16)t->titleSpritesLoopCount;
        TaskYieldTrampoline(3);
    }
    TaskExitTrampoline();
}

void sub_08009640(void)
{
    struct Task *t, *u, *v;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->frameTable = gUnk_087555B4;
    t->layer = 8;
    u = gCurTask;
    if (u->titleSpritesIndex <= 4) {
        u->posX = u->titleSpritesIndex * 0x140000 + 0x180000;
        u->posY = 0x800000;
    } else {
        u->posX = (u->titleSpritesIndex - 5) * 0x140000 + 0x880000;
        u->posY = 0x800000;
    }
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    v = gCurTask;
    v->frame = gUnk_08731CC8[v->titleSpritesIndex];
    TaskSleepForever();
}

void TitleMain(void)
{
    while (TitleScreen() == 0)
        IntroStory();
    gFadeSteps = 10;
    gBrightness = 2;
    gFadeStep = 3;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
    RunFramesUntilFadeDone();
}

/* The key-wait loop is `while (1)` with both exits as gotos (a `break` would
   make expand_end_loop rotate it), and the result goes through one `ret`
   local: with a plain `return 0` the done block ends in a jump and jump.c's
   "if (foo) bar; else break;" swap moves it in front of the loop body. */
s32 TitleScreen(void)
{
    s32 titleSpritesSlot, i, ret;

    if (gPrevGameState != GAME_STATE_BOOT_LOGO) {
        ResetTasksAndOam();
        LoadBgLayout(0);
        LoadGfxSet(7);
        gBg0ScrollY = gBg0ScrollX = gBg1ScrollY = gBg1ScrollX = gBg2ScrollY = gBg2ScrollX = gBg3ScrollY = gBg3ScrollX = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1E00;
    }
    if (gPrevGameState != GAME_STATE_BOOT_LOGO) {
        BeginFastFadeInFromWhite();
        RunFramesUntilFadeDone();
        titleSpritesSlot = TaskCreateFrom(TASK_TITLE_SPRITES, 0);
        gTasks[titleSpritesSlot].titleSpritesIndex = -1;
        gTasks[titleSpritesSlot].titleSpritesStartDelay = 0;
    } else {
        titleSpritesSlot = TaskCreateFrom(TASK_TITLE_SPRITES, 0);
        gTasks[titleSpritesSlot].titleSpritesIndex = -1;
        gTasks[titleSpritesSlot].titleSpritesStartDelay = 90;
    }
    TaskCreateFrom(TASK_TITLE_PALETTE, 0);
    PlayBgm(BGM_TITLE);
    if (gPrevGameState != GAME_STATE_BOOT_LOGO)
        RunFrames(60);
    else
        RunFrames(210);
    i = 600;
    while (1) {
        if (--i == 0)
            goto timeout;
        RunFrame();
        if (gPressedKeys & 9)
            goto pressed;
    }
pressed:
    ret = 1;
    goto out;
timeout:
    gPrevGameState = GAME_STATE_TITLE;
    BeginFastFadeOutToWhite();
    RunFramesUntilFadeDone();
    ret = 0;
out:
    return ret;
}

void IntroStory(void)
{
    s32 i;
    s32 j;
    s32 t;

    ResetTasksAndOam();
    LoadBgLayout(1);
    LoadGfxSet(8);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1C00;
    gBldCntTarget1 = 84;
    gBldCntTarget2 = 8;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    BeginFadeInFromWhite();
    RunFramesUntilFadeDone();
    for (i = 0; i < 9; i++) {
        LoadGfxSet(gUnk_08731CDC[i]);
        t = TaskCreateFrom(TASK_INTRO_STORY_PICTURE, 0);
        gTasks[t].introStoryPictureIndex = i;
        for (j = 0; j <= 16; j++) {
            gBldAlphaEva = j;
            gBldAlphaEvb = 16 - j;
            if (IntroStoryWait(2))
                goto end;
        }
        if (IntroStoryWait(gUnk_08731CE6[i]))
            break;
        if (i == 8)
            break;
        for (j = 0; j <= 16; j++) {
            gBldAlphaEva = 16 - j;
            gBldAlphaEvb = j;
            if (IntroStoryWait(2))
                goto end;
        }
    }
end:
    BeginFadeOutToWhite();
    RunFramesUntilFadeDone();
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
}

s32 IntroStoryWait(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}
