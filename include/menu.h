#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "gba/types.h"

/* menu.h: the RAM cells and ROM tables of the main menu, its sprite tasks and
   the BG scroll animator (M03).  One declaration per symbol, with the type
   its consumers prove (issue #36 phase 2, docs/header-conventions.md). */

/* EWRAM */
extern s8 gModeListExtraRows;
extern s8 gPlayerCountCursor;
extern u32 gMenuBufferedKeys;
extern u8 gBgScrollActive;
extern s8 gFileMenuCursor;
extern s8 gSoundTestRepeatCount;
extern s32 gBgScrollSpeeds[2][4];
extern s8 gMenuScreen;
extern s8 gMenuTransitionTimer;
extern u8 gEraseConfirmCount;
extern s32 gBgScrollTargets[2][4];
extern s8 gMenuChoiceCursor;
extern s8 gPrevMenuScreen;
extern s8 gUnk_02007FC8;
extern s16 gSoundTestSelection[];
extern s8 gMenuCursor;

/* IWRAM */
extern u16 gUnk_030012F0[][16];
extern u16 gUnk_03001310[][16];
extern u16 gUnk_03001372[];
extern u16 gUnk_0300153C[];
extern u16 gUnk_03001550[];
extern u16 gUnk_030015D0[];
extern u16 gUnk_030015F0[];
extern u16 gUnk_030015F4[];
extern u16 gUnk_03001612[];
extern u16 gUnk_03001668[];

/* ROM */
extern u8 gUnk_08550B9C[];
extern u8 gUnk_08551110[];
extern u8 gUnk_08553210[];
extern u8 gUnk_08553510[];
extern u8 gUnk_08553810[];
extern u16 gUnk_08554B60[][4];
extern u16 gUnk_08554B78[];
extern u16 gUnk_08554D7A[];
extern u16 gUnk_08554D80[];
extern u16 gUnk_085563C8[];
extern u16 gUnk_08559B68[][10];
extern u16 gUnk_08559B90[];
extern u16 gUnk_08559BA4[][16];
extern u16 gUnk_08559C24[][16];
extern u16 gUnk_08559CE6[];
extern u16 gUnk_08559CEC[];
extern const u8 gUnk_0855A5F8[];
extern u16 gUnk_0855D2F8[][10];
extern u16 gUnk_0855D320[];
extern u16 gUnk_0855D334[][16];
extern u16 gUnk_08560DBC[][16];
extern u16 gUnk_08560F9C[];
extern u16 gUnk_08561224[][10];
extern u16 gUnk_08562FE4[][8];
extern u16 gUnk_08563024[][13];
extern u16 gUnk_0856342C[][10];
extern u16 gUnk_085634D8[][16];
extern u16 gUnk_08564F34[];
extern u16 gUnk_08564F38[][5];
extern const u8 gUnk_085653C4[];
extern u16 *gUnk_08731CF8[];
extern u16 *gUnk_08731D28[];
extern u16 gUnk_08731D58[];
extern u32 gUnk_08731D70[];
extern vs32 *const gBgScrollYPtrs[4];
extern vs32 *const gBgScrollXPtrs[4];
extern const s16 gUnk_08731DC0[];
extern u16 gUnk_08731E18[];
extern u16 gUnk_08731E1E[2][3];
extern u8 *gUnk_08731E2C[2];
extern void *const gUnk_08731E34[];
extern u16 gUnk_08731E4C[];
extern u16 gUnk_08731E52[];
extern s32 gUnk_08731E58[][4];
extern s32 gUnk_08731E98[];
extern u16 gUnk_08731EA8[];
extern s16 gUnk_08731EB0[];
extern s16 gUnk_08731EB8[];
extern s16 gUnk_08731EC0[];
extern s32 gUnk_08731EC8[][4];
extern s32 gUnk_08731F08[][4];
extern s16 gUnk_08731F48[][4];
extern u32 gUnk_08755620[];
extern u32 gUnk_08755650[];
extern u32 gUnk_08755688[];
extern u32 gUnk_087556D4[];


/* Functions (defined in the files named above each group). */

/* src/menu_0b920.c */
void MainMenuMain(void);
void MenuDrawSaveSlots(void);
void MenuLoadSaveSlotLabel(s32 slot);
void MenuLoadSaveSlotPicture(s32 slot, u32 pal);
s32 MenuLoadSaveSlotPalette(s32 slot, u32 pal);
void MenuLoadSaveSlotPercent(s32 slot, s32 value, s32 mode);

/* src/menu_0c09c.c */
void MenuFileSelect(void);
void MenuSetupFileMenu(void);
void MenuFileMenuSelect(void);
void MenuNormalExtraSelect(void);
void MenuPlayerCountSelect(void);
void MenuEraseSelect(void);

/* src/menu_0ca10.c */
void MenuEnterModeList(void);
void MenuDrawModeList(void);
void MenuModeListSelect(void);
void MenuEnterLinkPlay(void);
void MenuModePlayerCountSelect(void);
void MenuEnterSoundTest(void);
void SoundTestHighlightCursor(void);
void SoundTestDrawNumber(s32 a);
void SoundTestPlaySfx(void);

/* src/menu_0d450.c */
void MenuSoundTest(void);
void MenuLinkPlay(void);
void MenuSetLinkSessionMode(void);
s32 sub_0800da74(void);
void CreateFileSelectSprites(s32 mode);

/* src/menutask_0daf8.c */
void Task_FileSelectSlotLabel(void);
void Task_FileSelectSlot(void);
void FileSelectSlotUpdate(void);
void FileSelectSlotSlideIn(void);
void Task_FileSelectCursor(void);
void FileSelectCursorUpdate(void);
void Task_FileMenuSlot(void);
void FileMenuSlotUpdate(void);
void Task_FileMenuHighlight(void);
void FileMenuHighlightUpdate(void);
void MenuUpdateFileMenuPalette(void);
s32 MenuLoadPicture(s32 id, s32 part);

/* src/menutask_0e314.c */
void Task_NormalExtraPanel(void);
void NormalExtraPanelUpdate(void);
void Task_PlayerCountPanel(void);
void PlayerCountPanelUpdate(void);
void Task_ModeListCursor(void);
void ModeListCursorUpdate(void);
void ModeListHighlightRow(void);
void Task_ModePlayerCountPanel(void);
void ModePlayerCountPanelUpdate(void);
void Task_EraseConfirmDialog(void);

/* src/menutask_0ea0c.c */
void EraseConfirmDialogUpdate(void);
void Task_EraseFileWipe(void);
void sub_0800ec08(void);
void Task_SoundTestCursors(void);
void sub_0800ecb8(void);
void Task_SoundTestPulse(void);
void sub_0800ef30(void);
void sub_0800f084(void);

/* src/menutask_0f180.c */
void Task_LinkPlayPlayerList(void);
void LinkPlayPlayerListUpdate(void);
void Task_LinkPlayConsole(void);
void LinkPlayConsoleUpdate(void);
void Task_LinkPlayCable(void);
void LinkPlayCableUpdate(void);
void Task_MenuScreenTitle(void);
void MenuScreenTitleUpdate(void);
void Task_MenuBgPaletteCycle(void);
void Task_MenuBackground(void);

/* src/bgscroll_0fcbc.c */
void BgScrollInit(void);
void BgScrollStart(s32 xspeed, s32 yspeed, s32 xdist, s32 ydist, s32 bg);
void BgScrollStartX(s32 speed, s32 dist, s32 bg);
void BgScrollStartY(s32 speed, s32 dist, s32 bg);
s32 BgScrollStartBg3Slide(s32 speed);
void BgScrollFinish(void);
void Task_BgScroll(void);
s32 sub_0800ffd8(void);
u8 TaskIsOnScreenNoCamera(void);
void SetBlend(s32 a, s32 b, s32 c, s32 d);
void SetWindow(s32 in, s32 out, s32 h, s32 v, s32 win);

#endif /* GUARD_MENU_H */
