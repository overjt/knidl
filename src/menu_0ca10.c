#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "menu.h"
#include "room.h"
#include "save.h"

/* menu_0ca10.c (0x0800CA10-0x0800D44F, issue #99).
 *
 * Main-menu screens, second part (gMenuScreen is the menu screen,
 * gMenuCursor the cursor, gPressedKeys/gRepeatedKeys the newly pressed
 * and auto-repeat keys).  MenuEnterModeList opens the mode list (screen 4)
 * and draws its 3-5 rows with MenuDrawModeList according to the save slot
 * unlock bits; MenuModeListSelect is its input loop (A/START picks a mode and
 * sets gUnk_02007FCC, row 4 leaves for game state 13, B goes back to the
 * file menu).  MenuModePlayerCountSelect runs screen 5 (one player, or link play
 * through MenuEnterLinkPlay, which opens the link-play screen 8),
 * MenuEnterSoundTest opens the sound test (screen 7), and
 * SoundTestHighlightCursor/SoundTestDrawNumber/SoundTestPlaySfx draw its cursor and three-digit
 * numbers and play the chosen sound. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlaySfx(s32 id);

void MenuEnterModeList(void)
{
    s32 i;

    gPrevMenuScreen = gMenuScreen;
    if (gPrevMenuScreen == 11) {
        LoadGfxSet(18);
        LoadGfxSet(21);
        gMenuScreen = 4;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1F00;
        SetBlend(66, 12, 13, 3);
    } else if (gPrevMenuScreen == 1) {
        gMenuCursor = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1E00;
        gMenuScreen = 4;
        BgScrollStartBg3Slide(0x80000);
        LoadGfxSet(25);
        StartHBlankScroll(2);
        RunFrames(8);
        gMenuBufferedKeys = 0;
        for (i = 0; i < 8; i++) {
            if (gPressedKeys & 11) {
                StopHBlankScroll();
                BgScrollFinish();
                gMenuBufferedKeys = gPressedKeys;
                SetWindow(61, 63, 255, 136, 0x2000);
                RunFrame();
                gDispCnt &= 0xDFFF;
                break;
            }
            RunFrame();
        }
        if (gPrevMenuScreen != 11)
            gMenuTransitionTimer = 16;
        else
            gMenuTransitionTimer = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1F00;
    } else {
        gMenuScreen = 5;
        StartHBlankScroll(7);
        RunFrames(8);
        gBg2ScrollY = gBg3ScrollY = 0;
        BgScrollStartX(0x100000, 256, 2);
        BgScrollStartX(0x100000, 256, 3);
        LoadGfxSet(41);
        gMenuTransitionTimer = 16;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1A00;
    }
    LoadGfxSet(31);
    if (gSaveSlots[gCurSaveSlot].milestoneFlags & 2)
        gModeListExtraRows = 2;
    else if (gSaveSlots[gCurSaveSlot].milestoneFlags & 1)
        gModeListExtraRows = 1;
    else
        gModeListExtraRows = 0;
    MenuDrawModeList();
    if (gMenuScreen != 5)
        TaskCreateFrom(247, 32);
}

void MenuDrawModeList(void)
{
    RequestCopy(6, 0, BG_VRAM + 0x3800, 0x800);
    switch (gModeListExtraRows) {
    case 0:
        RequestCopy(1, (u32)&gUnk_0855A5F8[0], BG_VRAM + 0x39C0, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[128], BG_VRAM + 0x3A80, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[256], BG_VRAM + 0x3B40, 128);
        break;
    case 1:
        RequestCopy(1, (u32)&gUnk_0855A5F8[0], BG_VRAM + 0x39C0, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[128], BG_VRAM + 0x3A40, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[256], BG_VRAM + 0x3AC0, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[384], BG_VRAM + 0x3B40, 128);
        break;
    case 2:
        RequestCopy(1, (u32)&gUnk_0855A5F8[0], BG_VRAM + 0x3980, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[128], BG_VRAM + 0x3A00, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[256], BG_VRAM + 0x3A80, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[384], BG_VRAM + 0x3B00, 128);
        RequestCopy(1, (u32)&gUnk_0855A5F8[512], BG_VRAM + 0x3B80, 128);
        break;
    }
}

void MenuModeListSelect(void)
{
    s32 i;

    while (1) {
        if (gMenuTransitionTimer != 0) {
            SetWindow(61, 63, ((16 - gMenuTransitionTimer) << 12) | 0xFF, 135, 0x2000);
            if ((gPressedKeys & 11) || (gMenuBufferedKeys & 11)) {
                gMenuTransitionTimer = 1;
                gMenuBufferedKeys |= gPressedKeys;
            }
            if (--gMenuTransitionTimer == 0)
                gDispCnt &= 0xDFFF;
        }
        if ((gPressedKeys & 9) || (gMenuBufferedKeys & 9)) {
            gMenuBufferedKeys = 0;
            PlaySfx(102);
            if (gMenuCursor == 4) {
                gMenuScreen = 9;
                gExtraMode = 0;
                gMetaKnightmareMode = 1;
                gGameState = 13;
                gUnk_02006090 = 7;
                gUnk_02007FCC = 7;
                return;
            }
            if (gMenuCursor <= 2) {
                gUnk_02006090 = gMenuCursor;
                gUnk_02007FCC = gMenuCursor;
            } else if (gMenuCursor == 3) {
                gMetaKnightmareMode = 0;
                gExtraMode = 0;
                gUnk_02006090 = 6;
                gUnk_02007FCC = 6;
            }
            gSubGameLevel = 0;
            gMenuScreen = 5;
            gMenuChoiceCursor = 0;
            TaskCreateFrom(248, 32);
            RunFrames(6);
            return;
        }
        if ((gPressedKeys & 2) || (gMenuBufferedKeys & 2)) {
            gMenuBufferedKeys = 0;
            PlaySfx(215);
            gPrevMenuScreen = gMenuScreen;
            gMenuScreen = 1;
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1E00;
            BgScrollStartBg3Slide(0xFFF80000);
            LoadGfxSet(33);
            for (i = 1; i < 16; i++) {
                SetWindow(60, 63, (0x10000 - (i << 12)) | 0xFF, 135, 0x2000);
                RunFrame();
                if (i > 8 && (gPressedKeys & 11)) {
                    BgScrollFinish();
                    break;
                }
            }
            gDispCnt &= 0xDFFF;
            MenuSetupFileMenu();
            return;
        }
        if (gRepeatedKeys & 64) {
            PlaySfx(101);
            if (--gMenuCursor < 0)
                gMenuCursor = gModeListExtraRows + 2;
        } else if (gRepeatedKeys & 128) {
            PlaySfx(101);
            if (++gMenuCursor >= gModeListExtraRows + 3)
                gMenuCursor = 0;
        }
        RunFrame();
    }
}

void MenuEnterLinkPlay(void)
{
    s32 i;

    gPrevMenuScreen = gMenuScreen;
    gMenuScreen = 8;
    LoadGfxSet(36);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1A00;
    gBg2ScrollY = gBg3ScrollY = 0;
    BgScrollStartX(0xFFF00000, 256, 2);
    BgScrollStartX(0xFFF00000, 256, 3);
    TaskCreateFrom(250, 32);
    LoadGfxSet(33);
    StartHBlankScroll(5);
    for (i = 0; i < 16; i++) {
        if (i != 0)
            SetWindow(60, 63, (0x10000 - (i << 12)) | 0xFF, 135, 0x2000);
        RunFrame();
    }
    gDispCnt &= 0xDFFF;
    LoadGfxSet(41);
    LoadGfxSet(37);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1E00;
    TaskCreateFrom(249, 32);
    RunFrames(8);
    LoadGfxSet(42);
    TaskCreateFrom(251, 32);
}

void MenuModePlayerCountSelect(void)
{
    gMenuBufferedKeys = 0;
    while (1) {
        if (gMenuTransitionTimer != 0) {
            if (--gMenuTransitionTimer != 0 && (gPressedKeys & 11)) {
                gMenuTransitionTimer = 0;
                StopHBlankScroll();
                BgScrollFinish();
                gMenuBufferedKeys = gPressedKeys;
            }
            if (gMenuTransitionTimer == 0) {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(247, 32);
                gMenuChoiceCursor = 1;
                TaskCreateFrom(248, 32);
                if (gMenuBufferedKeys != 0)
                    RunFrames(1);
            }
        }
        if ((gPressedKeys & 9) || (gMenuBufferedKeys & 9)) {
            gMenuBufferedKeys = 0;
            PlaySfx(102);
            if (gMenuChoiceCursor == 0) {
                gMenuScreen = 9;
                gMetaKnightmareMode = 0;
                gGameState = 13;
                return;
            }
            MenuEnterLinkPlay();
            return;
        }
        if ((gPressedKeys & 2) || (gMenuBufferedKeys & 2)) {
            gMenuBufferedKeys = 0;
            PlaySfx(215);
            gMenuScreen = 4;
            RunFrames(8);
            return;
        }
        if (gMenuTransitionTimer == 0) {
            if ((gHeldKeys & 128) && gMenuChoiceCursor == 0) {
                PlaySfx(101);
                gMenuChoiceCursor = 1;
            } else if ((gHeldKeys & 64) && gMenuChoiceCursor == 1) {
                PlaySfx(101);
                gMenuChoiceCursor = 0;
            }
        }
        RunFrame();
    }
}

void MenuEnterSoundTest(void)
{
    RunFrames(2);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1A00;
    RunFrame();
    FadeOutBgm(32);
    gMenuScreen = 7;
    LoadGfxSet(45);
    StartHBlankScroll(1);
    LoadGfxSet(25);
    gBg3ScrollY = gBg3ScrollX = 0;
    BgScrollStartX(0xFFF00000, 256, 3);
    gBg2ScrollX = 0x1000000;
    RunFrames(8);
    gMenuTransitionTimer = 8;
}

void SoundTestHighlightCursor(void)
{
    u16 *src = gUnk_08564F34;

    RequestCopy(2, (u32)src, (u32)gUnk_030015F0 + ((gMenuCursor * 16 + 1) * 2), 2);
    src++;
    RequestCopy(2, (u32)src, (u32)gUnk_030015F0 + (((s8)(gMenuCursor ^ 1) * 16 + 1) * 2), 2);
}

void SoundTestDrawNumber(s32 a)
{
    s32 i;

    IntToDigits(gSoundTestSelection[a]);
    if (gSoundTestSelection[a] < 100)
        gDigits[2] = 10;
    if (gSoundTestSelection[a] < 10)
        gDigits[1] = 10;
    for (i = 0; i < 3; i++) {
        RequestCopy(3, (u32)&gUnk_085653C4[gDigits[2 - i] * 128], (u32)gObjVram + (a * 6 + i * 2 + 4) * 32, 64);
        RequestCopy(3, (u32)&gUnk_085653C4[(gDigits[2 - i] * 4 + 2) * 32], (u32)gObjVram + (a * 6 + i * 2 + 36) * 32, 64);
    }
}

void SoundTestPlaySfx(void)
{
    s32 n = 0;
    s32 i = 0;
    s16 target = gSoundTestSelection[1];

    if (i != target) {
        i = 1;
        do {
            if (gSfxTable[i].header != NULL)
                n++;
            if (n == target)
                break;
            i++;
        } while (i != 0x1DF);
    }
    PlaySfx(i + 100);
}
