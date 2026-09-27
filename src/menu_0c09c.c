#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0c09c.c (0x0800C09C-0x0800CA0F, issue #99).
 *
 * Main-menu screens, first part.  sub_0800c09c is the file-select input
 * loop (screen 0: three save slots, A/START loads or creates the slot,
 * B returns to the title screen); MenuSetupFileMenu opens the file menu
 * (screen 1) and sub_0800c34c is its input loop over four entries
 * (gFileMenuCursor: start, the mode list, the sound test, erase);
 * sub_0800c558 and sub_0800c610 run the two-choice screens 2/3 that
 * lead into a game, and sub_0800c8a0 the two-step erase confirmation
 * that clears the slot with EraseSaveSlot. */

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 generation;
    /*0x0C*/ s32 saveCount;
    /*0x10*/ u16 milestoneFlags;
    /*0x12*/ u16 completionPercent[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern s8 gUnk_02004B44;
extern u32 gMenuBufferedKeys;
extern s8 gFileMenuCursor;
extern s8 gMenuScreen;
extern s8 gMenuTransitionTimer;
extern u8 gEraseConfirmCount;
extern s8 gMenuChoiceCursor;
extern s8 gPrevMenuScreen;
extern s8 gMenuCursor;
extern struct SaveSlot gSaveSlots[];
extern vu16 gPressedKeys;
extern vu16 gRepeatedKeys;
extern vs32 gBg3ScrollY;
extern vs32 gBg2ScrollY;
extern vu16 gDispCnt;
extern vu16 gHeldKeys;
extern u8 gUnk_03001F30;
extern u16 gPrevGameState;
extern u16 gGameState;
extern s32 gCurSaveSlot;
extern u8 gExtraMode;

void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void RunFrame(void);
void RunFrames(s32 count);
void RunFramesUntilFadeDone(void);
s32 PlaySfx(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void LoadBgLayout(s32 a0);
void LoadGfxSet(u16 a0);
void MenuDrawSaveSlots(void);
void sub_0800bda4(s32 slot);
void sub_0800bf6c(s32 slot, s32 value, s32 mode);
void MenuEnterModeList(void);
void sub_0800d280(void);
void CreateFileSelectSprites(s32 mode);
void MenuUpdateFileMenuPalette(void);
void BgScrollStartX(s32 speed, s32 dist, s32 bg);
s32 BgScrollStartBg3Slide(s32 speed);
void BgScrollFinish(void);
void SetBlend(s32 a, s32 b, s32 c, s32 d);
void StopHBlankScroll(void);
void StartHBlankScroll(s32 a);
void InitNewSaveFile(s32 a);
void EraseSaveSlot(s32 a);
void LoadSaveSlot(s32 a);
s32 CheckNewMilestones(void);
void ShowMilestonePictureForMode(s32 a);
void MenuSetupFileMenu(void);

void sub_0800c09c(void)
{
    vu16 *keys = &gPressedKeys;
    s8 *state = &gMenuScreen;
    s32 i;
    s32 done;

    while (1)
    {
        if (*keys & 9)
        {
            PlaySfx(102);
            gCurSaveSlot = gMenuCursor;
            if (gSaveSlots[gCurSaveSlot].unk04 == 0x99999999)
                InitNewSaveFile(gCurSaveSlot);
            done = 0;
            for (i = 0; i < 2; i++)
            {
                gExtraMode = i;
                LoadSaveSlot(gCurSaveSlot);
                if (CheckNewMilestones() & 2)
                {
                    if (done == 0)
                    {
                        BeginFastFadeOutToWhite();
                        RunFramesUntilFadeDone();
                    }
                    ShowMilestonePictureForMode(i);
                    done = 1;
                }
            }
            if (done)
            {
                LoadBgLayout(2);
                MenuDrawSaveSlots();
                LoadGfxSet(19);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1C00;
                BeginFastFadeInFromWhite();
                RunFramesUntilFadeDone();
            }
            BgScrollStartBg3Slide(0x80000);
            gPrevMenuScreen = *state;
            *state = 1;
            MenuSetupFileMenu();
            return;
        }
        if (*keys & 2)
        {
            PlaySfx(215);
            *state = 10;
            gPrevGameState = 4;
            gGameState = 3;
            return;
        }
        if (gRepeatedKeys & 0x40)
        {
            PlaySfx(101);
            if (--gMenuCursor < 0)
                gMenuCursor = 2;
        }
        else if (gRepeatedKeys & 0x80)
        {
            PlaySfx(101);
            if (++gMenuCursor > 2)
                gMenuCursor = 0;
        }
        RunFrame();
    }
}

void MenuSetupFileMenu(void)
{
    gEraseConfirmCount = 0;
    TaskCreateFrom(241, 32);
    MenuUpdateFileMenuPalette();
    LoadGfxSet(21);
    sub_0800bda4(gCurSaveSlot);
    sub_0800bf6c(gCurSaveSlot, gSaveSlots[gCurSaveSlot].completionPercent[0], 1);
    SetBlend(66, 12, 13, 3);
    switch (gPrevMenuScreen)
    {
    case 0:
        gFileMenuCursor = 0;
        LoadGfxSet(25);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1E00;
        StartHBlankScroll(0);
        RunFrames(8);
        gMenuTransitionTimer = 9;
        break;
    case 4:
        gFileMenuCursor = 1;
        LoadGfxSet(25);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1F00;
        TaskCreateFrom(242, 32);
        StartHBlankScroll(3);
        gMenuTransitionTimer = 17;
        break;
    case 8:
        gFileMenuCursor = 0;
        LoadGfxSet(26);
        gMenuTransitionTimer = 17;
        break;
    case 7:
        gFileMenuCursor = 2;
        LoadGfxSet(25);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1A00;
        StartHBlankScroll(0);
        gMenuTransitionTimer = 17;
        break;
    }
}

void sub_0800c34c(void)
{
    gMenuBufferedKeys = 0;
    while (1)
    {
        if (gMenuTransitionTimer != 0)
        {
            if (--gMenuTransitionTimer != 0 && (gPressedKeys & 11))
            {
                gMenuTransitionTimer = 0;
                StopHBlankScroll();
                BgScrollFinish();
                gMenuBufferedKeys = gPressedKeys;
            }
            if (gMenuTransitionTimer == 0)
            {
                if (gPrevMenuScreen != 4)
                    TaskCreateFrom(242, 32);
                if (gMenuBufferedKeys != 0)
                    RunFrames(4);
            }
        }
        if ((gPressedKeys & 9) || (gMenuBufferedKeys & 9))
        {
            PlaySfx(102);
            switch (gFileMenuCursor)
            {
            case 0:
                gUnk_03001F30 = 0;
                gExtraMode = 0;
                if (gSaveSlots[gCurSaveSlot].milestoneFlags & 4)
                {
                    gMenuScreen = 2;
                    gMenuChoiceCursor = 0;
                    TaskCreateFrom(245, 32);
                    RunFrames(10);
                }
                else
                {
                    gMenuScreen = 3;
                    gUnk_02004B44 = 0;
                    TaskCreateFrom(246, 32);
                    RunFrames(10);
                }
                break;
            case 3:
                gMenuScreen = 6;
                break;
            case 1:
                MenuEnterModeList();
                break;
            case 2:
                sub_0800d280();
                break;
            }
            return;
        }
        if ((gPressedKeys & 2) || (gMenuBufferedKeys & 2))
        {
            PlaySfx(215);
            gMenuScreen = 0;
            MenuDrawSaveSlots();
            CreateFileSelectSprites(1);
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1E00;
            StartHBlankScroll(1);
            LoadGfxSet(25);
            BgScrollStartBg3Slide(0xFFF80000);
            RunFrames(18);
            return;
        }
        if (gMenuTransitionTimer == 0)
        {
            if (gRepeatedKeys & 0x40)
            {
                PlaySfx(101);
                if (--gFileMenuCursor < 0)
                    gFileMenuCursor = 3;
            }
            else if (gRepeatedKeys & 0x80)
            {
                PlaySfx(101);
                if (++gFileMenuCursor > 3)
                    gFileMenuCursor = 0;
            }
        }
        RunFrame();
    }
}

void sub_0800c558(void)
{
    vu16 *keys = &gPressedKeys;
    s8 *state = &gMenuScreen;

    while (!(*keys & 9))
    {
        if (*keys & 2)
        {
            PlaySfx(215);
            *state = 1;
            RunFrames(10);
            return;
        }
        if ((gHeldKeys & 0x80) && gMenuChoiceCursor == 0)
        {
            PlaySfx(101);
            gMenuChoiceCursor = 1;
        }
        else if ((gHeldKeys & 0x40) && gMenuChoiceCursor == 1)
        {
            PlaySfx(101);
            gMenuChoiceCursor = 0;
        }
        RunFrame();
    }
    PlaySfx(102);
    gExtraMode = gMenuChoiceCursor;
    *state = 3;
    gUnk_02004B44 = 0;
    TaskCreateFrom(246, 32);
    RunFrames(10);
}

void sub_0800c610(void)
{
    gMenuBufferedKeys = 0;
    while (1)
    {
        if (gMenuTransitionTimer != 0)
        {
            if (--gMenuTransitionTimer != 0 && (gPressedKeys & 11))
            {
                gMenuTransitionTimer = 0;
                StopHBlankScroll();
                BgScrollFinish();
                gMenuBufferedKeys = gPressedKeys;
            }
            if (gMenuTransitionTimer == 0)
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(242, 32);
                if (gSaveSlots[gCurSaveSlot].milestoneFlags & 4)
                {
                    gMenuChoiceCursor = gExtraMode;
                    TaskCreateFrom(245, 32);
                }
                gUnk_02004B44 = 1;
                TaskCreateFrom(246, 32);
                if (gMenuBufferedKeys != 0)
                    RunFrames(1);
            }
        }
        if ((gPressedKeys & 9) || (gMenuBufferedKeys & 9))
        {
            PlaySfx(102);
            if (gUnk_02004B44 == 0)
            {
                gMenuScreen = 9;
                gUnk_03001F30 = 0;
                gGameState = 5;
            }
            else
            {
                gPrevMenuScreen = gMenuScreen;
                gMenuScreen = 8;
                LoadGfxSet(36);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1A00;
                gBg2ScrollY = gBg3ScrollY = 0;
                BgScrollStartX(0xFFF00000, 256, 2);
                BgScrollStartX(0xFFF00000, 256, 3);
                TaskCreateFrom(250, 32);
                LoadGfxSet(26);
                StartHBlankScroll(4);
                RunFrames(16);
                LoadGfxSet(39);
                LoadGfxSet(37);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1E00;
                TaskCreateFrom(249, 32);
                RunFrames(8);
                LoadGfxSet(40);
                TaskCreateFrom(251, 32);
            }
            return;
        }
        if ((gPressedKeys & 2) || (gMenuBufferedKeys & 2))
        {
            PlaySfx(215);
            if (gSaveSlots[gCurSaveSlot].milestoneFlags & 4)
                gMenuScreen = 2;
            else
                gMenuScreen = 1;
            RunFrames(10);
            return;
        }
        if (gMenuTransitionTimer == 0)
        {
            if ((gHeldKeys & 0x80) && gUnk_02004B44 == 0)
            {
                PlaySfx(101);
                gUnk_02004B44 = 1;
            }
            else if ((gHeldKeys & 0x40) && gUnk_02004B44 == 1)
            {
                PlaySfx(101);
                gUnk_02004B44 = 0;
            }
        }
        RunFrame();
    }
}

void sub_0800c8a0(void)
{
    vu16 *keys;

    gEraseConfirmCount = 0;
    gMenuChoiceCursor = 1;
    LoadGfxSet(29);
    TaskCreateFrom(243, 32);
    RunFrames(10);
    keys = &gPressedKeys;
    while (1)
    {
        if ((gPressedKeys & 9) && gMenuChoiceCursor == 0)
        {
            PlaySfx(102);
            if (++gEraseConfirmCount == 1)
            {
                LoadGfxSet(30);
                gMenuChoiceCursor = 1;
                RunFrames(10);
            }
            else
            {
                TaskCreateFrom(244, 32);
                RunFrames(10);
                PlaySfx(268);
                EraseSaveSlot(gCurSaveSlot);
                gMenuScreen = 0;
                MenuDrawSaveSlots();
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1E00;
                StartHBlankScroll(1);
                LoadGfxSet(25);
                BgScrollStartBg3Slide(0xFFF80000);
                RunFrames(28);
                CreateFileSelectSprites(1);
                RunFrames(16);
                return;
            }
        }
        else if (*keys & 11)
        {
            if (*keys & 9)
                PlaySfx(102);
            else
                PlaySfx(215);
            gMenuScreen = 1;
            RunFrames(10);
            return;
        }
        if ((gHeldKeys & 0x20) && gMenuChoiceCursor == 1)
        {
            PlaySfx(101);
            gMenuChoiceCursor = 0;
        }
        else if ((gHeldKeys & 0x10) && gMenuChoiceCursor == 0)
        {
            PlaySfx(101);
            gMenuChoiceCursor = 1;
        }
        RunFrame();
    }
}
