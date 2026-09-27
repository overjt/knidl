#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0b920.c (0x0800B920-0x0800C09B, issue #99).
 *
 * AgbMain state 4 (MainMenuMain), the main menu: it resets the menu cells,
 * spawns the background tasks #256-#259, picks the first screen from the
 * return state gPrevGameState (3 = file select, 14-16/20/21 = back from an
 * extra mode, straight to the mode list) and dispatches on the menu
 * screen gMenuScreen until it reaches 9 (start a game: state 5 or 13)
 * or 10 (back to the title).  The rest draws the file-select screen's
 * three save slots: MenuDrawSaveSlots all three, sub_0800bda4 a slot's label
 * (empty, finished, or its number through the digit buffer gDigits),
 * sub_0800be8c/sub_0800bf10 its picture and palette, sub_0800bf6c its
 * second number. */

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
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

extern s8 gLinkSessionMode;
extern u32 gMenuBufferedKeys;
extern u8 gUnk_02006090;
extern s8 gMenuScreen;
extern s8 gMenuTransitionTimer;
extern u8 gCutscenePending;
extern s16 gSoundTestSelection[];
extern s8 gMenuCursor;
extern struct SaveSlot gSaveSlots[];
extern u32 gLinkSetupMode;
extern vu16 gFadeBlankAtWhite;
extern vs16 gFadeStep;
extern vs32 gBg3ScrollY;
extern vs16 gBrightness;
extern vu16 gKeyRepeatDelay;
extern vu16 gFadeTimer;
extern u16 gUnk_03001490[];
extern vu16 gFadeSteps;
extern vu16 gFadeInterval;
extern vu16 gKeyRepeatInterval;
extern u16 *gFadeKeepMask;
extern vu16 gDispCnt;
extern s8 gDigits[];
extern u8 gUnk_03001F30;
extern u16 gPrevGameState;
extern u16 gPlayerCount;
extern u16 gGameState;
extern s32 gCurSaveSlot;
extern s16 gUnk_0300244C;
extern u32 gLinkStatus;
extern u32 gObjVram[];
extern u8 gUnk_08551110[];
extern u8 gUnk_08553210[];
extern u8 gUnk_08553510[];
extern u8 gUnk_08553810[];
extern u16 gUnk_08554B78[];
extern u16 gUnk_08731E18[];
extern u16 gUnk_08731E1E[2][3];
extern u8 *gUnk_08731E2C[2];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetFadeAndBlend(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void RunFrame(void);
void LinkStopKeyExchange(void);
void RunFrames(s32 count);
void RunFramesUntilFadeDone(void);
void RunLinkFramesUntilFadeDone(void);
void IntToDigits(s16 n);
s32 PlayBgm(s32 songId);
void FadeOutBgm(s32 speed);
void LinkSetupStop(void);
void LinkSetupMain(u16 a);
s32 TaskCreateFrom(u32 type, s32 idx);
void DisableSerial(void);
void LoadBgLayout(s32 a0);
void LoadGfxSet(u16 a0);
void ResetScoresAndMaxHealth(void);
void sub_0800c09c(void);
void sub_0800c34c(void);
void sub_0800c558(void);
void sub_0800c610(void);
void sub_0800c8a0(void);
void MenuEnterModeList(void);
void sub_0800cd60(void);
void sub_0800d0f4(void);
void sub_0800d450(void);
void sub_0800d85c(void);
void CreateFileSelectSprites(s32 mode);
void BgScrollInit(void);
void BgScrollStartY(s32 speed, s32 dist, s32 bg);
void SetBlend(s32 a, s32 b, s32 c, s32 d);
void ResetHBlankScroll(void);
void SelectLatestSaveSlot(void);
void LoadSaveSlot(s32 a);
void sub_080b81a0(s32 a);
s32 CheckNewMilestones(void);
void sub_080b8888(void);
void sub_080b8918(void);
void MenuDrawSaveSlots(void);
void sub_0800bda4(s32 slot);
void sub_0800be8c(s32 slot, u32 pal);
s32 sub_0800bf10(s32 slot, u32 pal);
void sub_0800bf6c(s32 slot, s32 value, s32 mode);

void MainMenuMain(void)
{
    LinkSetupStop();
    DisableSerial();
    gLinkStatus = 0;
    ResetTasksAndOam();
    ResetFadeAndBlend();
    ResetHBlankScroll();
    if (gCurSaveSlot == -1 || gCurSaveSlot == 3)
        SelectLatestSaveSlot();
    gSoundTestSelection[0] = 0;
    gSoundTestSelection[1] = 0;
    gKeyRepeatDelay = 10;
    gKeyRepeatInterval = 6;
    gMenuTransitionTimer = 0;
    LoadBgLayout(2);
    LoadGfxSet(22);
    BgScrollInit();
    TaskCreateFrom(0x101, 32);
    TaskCreateFrom(0x102, 32);
    TaskCreateFrom(0x103, 32);
    TaskCreateFrom(0x100, 32);
    switch (gPrevGameState)
    {
    case 3:
        gBg3ScrollY = 64;
        BgScrollStartY(0x80000, 80, 3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1C00;
        gMenuScreen = 0;
        MenuDrawSaveSlots();
        CreateFileSelectSprites(0);
        break;
    case 14:
    case 15:
    case 16:
    case 20:
    case 21:
        gBg3ScrollY = 64;
        BgScrollStartY(0x80000, 80, 3);
        if (gPrevGameState <= 16)
            gMenuCursor = gPrevGameState - 14;
        else if (gPrevGameState == 20)
            gMenuCursor = 3;
        else
            gMenuCursor = 4;
        gMenuScreen = 11;
        MenuEnterModeList();
        gMenuBufferedKeys = 0;
        break;
    }
    PlayBgm(40);
    gFadeSteps = 10;
    gBrightness = 30;
    gFadeStep = -3;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
    RunFramesUntilFadeDone();
    RunFrames(6);
    do
    {
        switch (gMenuScreen)
        {
        case 0:
            sub_0800c09c();
            break;
        case 1:
            sub_0800c34c();
            break;
        case 2:
            sub_0800c558();
            break;
        case 3:
            sub_0800c610();
            break;
        case 4:
            sub_0800cd60();
            break;
        case 5:
            sub_0800d0f4();
            break;
        case 6:
            sub_0800c8a0();
            break;
        case 7:
            sub_0800d450();
            break;
        case 8:
            sub_0800d85c();
            break;
        }
    } while (gMenuScreen != 9 && gMenuScreen != 10);
    BeginFastFadeOutToWhite();
    if (gLinkSetupMode == 2)
    {
        FadeOutBgm(16);
        while (gFadeSteps != 0)
        {
            RunFrame();
            LinkSetupMain(gLinkSessionMode);
        }
        gFadeBlankAtWhite = 0;
        gUnk_03001F30 = 0;
        gGameState = 13;
    }
    else
    {
        if (gGameState == 5 || gGameState == 3)
            FadeOutBgm(16);
        if (gPlayerCount > 1)
        {
            if (gGameState == 5)
            {
                sub_080b8888();
                sub_080b8918();
            }
            LinkStopKeyExchange();
            RunLinkFramesUntilFadeDone();
        }
        else
        {
            gUnk_0300244C = 1;
            RunFramesUntilFadeDone();
        }
    }
    SetBlend(0, 0, 0, 0);
    gDispCnt &= 0xDFFF;
    switch (gGameState)
    {
    case 3: /* empty but load-bearing: it adds the `cmp #5; ble` split */
        break;
    case 5:
        LoadSaveSlot(gCurSaveSlot);
        CheckNewMilestones();
        ResetScoresAndMaxHealth();
        gCutscenePending = 1;
        break;
    case 13:
        if (gUnk_02006090 == 6 || gUnk_02006090 == 7)
        {
            LoadSaveSlot(gCurSaveSlot);
            CheckNewMilestones();
            if (gUnk_02006090 == 7)
                sub_080b81a0(gCurSaveSlot);
        }
        gPrevGameState = 4;
        break;
    }
}

void MenuDrawSaveSlots(void)
{
    s32 i;

    gMenuCursor = gCurSaveSlot;
    LoadGfxSet(18);
    for (i = 0; i < 3; i++)
    {
        sub_0800bda4(i);
        sub_0800be8c(i, (s8)gSaveSlots[i].unk16[gSaveSlots[i].unk12[1] ? (gSaveSlots[i].unk04 != 0x99999999) : 0]);
        sub_0800bf6c(i, gSaveSlots[i].unk12[gSaveSlots[i].unk12[1] ? (gSaveSlots[i].unk04 != 0x99999999) : 0], 0);
    }
}

void sub_0800bda4(s32 slot)
{
    s32 n;

    if (gSaveSlots[slot].unk04 == 0x99999999)
    {
        RequestCopy(3, (u32)gUnk_08553510, (u32)gObjVram + ((slot * 64 + 576) << 5), 0x180);
        RequestCopy(3, (u32)&gUnk_08553510[0x180], (u32)gObjVram + ((slot * 64 + 608) << 5), 0x180);
    }
    else if (gSaveSlots[slot].unk10 & 2)
    {
        RequestCopy(3, (u32)gUnk_08553210, (u32)gObjVram + ((slot * 64 + 576) << 5), 0x180);
        RequestCopy(3, (u32)&gUnk_08553210[0x180], (u32)gObjVram + ((slot * 64 + 608) << 5), 0x180);
    }
    else
    {
        IntToDigits(gSaveSlots[slot].unk12[0]);
        n = (gDigits[1] + gDigits[2] * 10) * 2;
        RequestCopy(3, (u32)&gUnk_08551110[n * 0x180], (u32)gObjVram + ((slot * 64 + 576) << 5), 0x180);
        n++;
        RequestCopy(3, (u32)&gUnk_08551110[n * 0x180], (u32)gObjVram + ((slot * 64 + 608) << 5), 0x180);
    }
}

void sub_0800be8c(s32 slot, u32 pal)
{
    s32 i;

    if (pal > 6)
        pal = 7;
    i = pal * 15;
    RequestCopy(3, (u32)&gUnk_08553810[i * 32], (u32)gObjVram + (gUnk_08731E18[slot] << 5), 160);
    RequestCopy(3, (u32)&gUnk_08553810[(i + 5) * 32], (u32)gObjVram + ((gUnk_08731E18[slot] + 32) << 5), 160);
    RequestCopy(3, (u32)&gUnk_08553810[(i + 10) * 32], (u32)gObjVram + ((gUnk_08731E18[slot] + 64) << 5), 160);
    sub_0800bf10(slot, pal);
}

s32 sub_0800bf10(s32 slot, u32 pal)
{
    if (pal > 6)
        pal = 7;
    if (slot == gMenuCursor)
        RequestCopy(2, (u32)&gUnk_08554B78[pal * 16], (u32)&gUnk_03001490[slot * 16], 32);
    else
        RequestCopy(2, (u32)&gUnk_08554B78[(pal + 8) * 16], (u32)&gUnk_03001490[slot * 16], 32);
}

void sub_0800bf6c(s32 slot, s32 value, s32 mode)
{
    IntToDigits(value);
    if (value < 0 || value > 100)
    {
        gDigits[1] = 10;
        gDigits[2] = 10;
        gDigits[0] = 0;
    }
    else if (value < 10)
    {
        gDigits[1] = 10;
        gDigits[2] = 10;
    }
    else if (value < 100)
    {
        gDigits[2] = 10;
    }

    switch (mode)
    {
    case 0:
        if (value == 100)
        {
            gDigits[1] = 11;
            gDigits[0] = 12;
        }
        RequestCopy(3, (u32)(gUnk_08731E2C[0] + gDigits[1] * 32), (u32)gObjVram + ((gUnk_08731E1E[0][slot] + 1) << 5), 32);
        RequestCopy(3, (u32)(gUnk_08731E2C[0] + gDigits[0] * 32), (u32)gObjVram + ((gUnk_08731E1E[0][slot] + 2) << 5), 32);
        break;
    case 1:
        RequestCopy(3, (u32)(gUnk_08731E2C[1] + gDigits[2] * 32), (u32)gObjVram + (gUnk_08731E1E[mode][slot] << 5), 32);
        RequestCopy(3, (u32)(gUnk_08731E2C[1] + gDigits[1] * 32), (u32)gObjVram + ((gUnk_08731E1E[mode][slot] + 1) << 5), 32);
        RequestCopy(3, (u32)(gUnk_08731E2C[1] + gDigits[0] * 32), (u32)gObjVram + ((gUnk_08731E1E[mode][slot] + 2) << 5), 32);
        break;
    }
}
