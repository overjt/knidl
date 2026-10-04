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

/* menu_0d450.c (0x0800D450-0x0800DAF7, issue #99).
 *
 * The sound-test input loop MenuSoundTest (two columns in
 * gSoundTestSelection[]: music 0-43, mapped to song ids through gUnk_08731DC0,
 * and sound effects 0-273; A plays or stops, B returns to the file menu)
 * and the link-play connection screen MenuLinkPlay, which starts the SIO
 * multi-play session for the mode MenuSetLinkSessionMode picks, waits for the
 * partners (sub_0800da74) and leaves for game state 5 or 13.
 * CreateFileSelectSprites spawns the file-select sprite tasks #238-#240. */

s32 PlaySfx(s32 id);

void MenuSoundTest(void)
{
    while (1) {
        if (gMenuTransitionTimer != 0) {
            if (--gMenuTransitionTimer != 0 && (gPressedKeys & 2)) {
                gMenuTransitionTimer = 0;
                StopHBlankScroll();
                BgScrollFinish();
            }
            if (gMenuTransitionTimer == 0) {
                gMenuCursor = 0;
                SoundTestDrawNumber(0);
                SoundTestDrawNumber(1);
                SoundTestHighlightCursor();
                TaskCreateFrom(TASK_SOUND_TEST_CURSORS, 32);
                TaskCreateFrom(TASK_SOUND_TEST_PULSE, 32);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        gUnk_03001F2C = gUnk_08731DC0[gSoundTestSelection[gMenuCursor]];
        if (gPressedKeys & 1) {
            if (gMenuCursor == 0) {
                if (gUnk_03001F2C == gCurrentBgm
                    && (s32)gMPlayTable[gSongTable[gUnk_03001F2C].ms].info->status >= 0) {
                    m4aSongNumStop(gUnk_03001F2C);
                    LoadGfxSet(49);
                } else {
                    PlayBgm(gUnk_03001F2C | 0x800);
                    LoadGfxSet(48);
                }
            } else {
                StopAllSfx();
                SoundTestPlaySfx();
                LoadGfxSet(48);
            }
            RunFrames(3);
            LoadGfxSet(47);
        } else if (gPressedKeys & 2) {
            gKeyRepeatDelay = 10;
            gKeyRepeatInterval = 6;
            StopAllSfx();
            PlaySfx(215);
            LoadGfxSet(50);
            RunFrames(3);
            LoadGfxSet(47);
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1A00;
            BgScrollStartX(0x100000, 256, 3);
            gBg2ScrollX = 0;
            gPrevMenuScreen = gMenuScreen;
            gMenuScreen = 1;
            if (gUnk_03001F2C != 40
                && (s32)gMPlayTable[gSongTable[gUnk_03001F2C].ms].info->status >= 0)
                FadeOutBgm(32);
            RunFrames(8);
            PlayBgm(40);
            MenuSetupFileMenu();
            return;
        }
        if (gMenuTransitionTimer == 0) {
            if ((gPressedKeys & 16) && gMenuCursor == 0) {
                gMenuCursor = 1;
                SoundTestHighlightCursor();
                gSoundTestRepeatCount = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            } else if ((gPressedKeys & 32) && gMenuCursor == 1) {
                gMenuCursor = 0;
                SoundTestHighlightCursor();
                gSoundTestRepeatCount = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            } else if (gRepeatedKeys & 128) {
                gSoundTestSelection[gMenuCursor]--;
                if (++gSoundTestRepeatCount == 5) {
                    gKeyRepeatDelay = 10;
                    gKeyRepeatInterval = 3;
                }
                if (gSoundTestSelection[gMenuCursor] < 0) {
                    if (gMenuCursor == 0)
                        gSoundTestSelection[gMenuCursor] = 43;
                    else
                        gSoundTestSelection[gMenuCursor] = 0x111;
                }
                SoundTestDrawNumber(gMenuCursor);
            } else if (gRepeatedKeys & 64) {
                gSoundTestSelection[gMenuCursor]++;
                if (++gSoundTestRepeatCount == 5) {
                    gKeyRepeatDelay = 10;
                    gKeyRepeatInterval = 3;
                }
                if (gMenuCursor == 0 && gSoundTestSelection[gMenuCursor] > 43)
                    gSoundTestSelection[gMenuCursor] = 0;
                else if (gMenuCursor == 1 && gSoundTestSelection[gMenuCursor] > 0x111)
                    gSoundTestSelection[gMenuCursor] = 0;
                SoundTestDrawNumber(gMenuCursor);
            }
            if (!(gHeldKeys & 0xC0)) {
                gSoundTestRepeatCount = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            }
        }
        RunFrame();
    }
}

void MenuLinkPlay(void)
{
    CopySaveSlotToLinkSlot();
    MenuSetLinkSessionMode();
    LinkSetupInit();
    MultiBootInitWithParams(gUnk_0876B1FC, gUnk_0876F690);
    gUnk_02007FC8 = 0;
    RunFrames(4);
    while (1) {
        if (gPressedKeys & 2) {
            PlaySfx(215);
            LinkSetupStop();
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1A00;
            if (gUnk_02007FC8 == 1)
                LoadGfxSet(37);
            if (gPrevMenuScreen == 3) {
                LoadGfxSet(39);
                gPrevMenuScreen = gMenuScreen;
                gMenuScreen = 3;
                gBg2ScrollY = gBg3ScrollY = 0;
                BgScrollStartX(0x100000, 256, 2);
                BgScrollStartX(0x100000, 256, 3);
                StartHBlankScroll(6);
                RunFrames(8);
                MenuSetupFileMenu();
                return;
            }
            LoadGfxSet(41);
            MenuEnterModeList();
            return;
        }
        if (gUnk_02007FC8 == 0) {
            LinkSetupMain(gLinkSessionMode);
            if (gMultiBootStruct[2] == 3) {
                gMenuScreen = 9;
                gMetaKnightmareMode = 0;
                if (gPrevMenuScreen == 3)
                    gGameState = GAME_STATE_HUB;
                else
                    gGameState = GAME_STATE_EXTRA_MODE_TITLE;
                if (ConnectLink())
                    LinkErrorScreen();
                return;
            }
            if (gMultiBootStruct[2] == 2 && gMultiBootStruct[0] == 0 && (gPressedKeys & 9)) {
                if (gLinkSetupMode != 1) {
                    gMenuScreen = 9;
                    return;
                }
                LinkSetupRequestStart();
            }
            if ((gMultiBootStruct[3] & 1) || sub_0800da74() == 1) {
                LinkSetupStop();
                gUnk_02007FC8 = 1;
                LoadGfxSet(38);
            }
        }
        RunFrame();
    }
}

void MenuSetLinkSessionMode(void)
{
    if (gPrevMenuScreen == 3) {
        if (gExtraMode == 0)
            gLinkSessionMode = 1;
        else
            gLinkSessionMode = 2;
    } else if (gUnk_02007FCC <= 2) {
        gLinkSessionMode = gUnk_02007FCC + 4;
    } else if (gUnk_02007FCC == 6) {
        gLinkSessionMode = 3;
    }
}

s32 sub_0800da74(void)
{
    s32 r = 0;

    switch (gLinkSessionMode) {
    case 1:
    case 2:
    case 3:
        if (gLinkSetupMode == 2)
            r = 1;
        break;
    case 4:
        break;
    }
    return r;
}

void CreateFileSelectSprites(s32 mode)
{
    s32 i;
    struct Task *t;
    s32 id;

    id = TaskCreateFrom(TASK_FILE_SELECT_CURSOR, 32);
    t = &gTasks[id];
    t->unk18 = mode;
    for (i = 0; i <= 2; i++) {
        id = TaskCreateFrom(TASK_FILE_SELECT_SLOT_LABEL, 32);
        t = &gTasks[id];
        t->unk18 = mode;
        t->unk1C = i;
        id = TaskCreateFrom(TASK_FILE_SELECT_SLOT, 32);
        t = &gTasks[id];
        t->unk18 = mode;
        t->unk1C = i;
    }
}
