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
extern u16 gBgPaletteBank4[][16];
extern u16 gBgPaletteBank5[][16];
extern u16 gBgPaletteBank8Color1[];
extern u16 gObjPaletteBank6Color6[];
extern u16 gObjPaletteBank7[];
extern u16 gObjPaletteBank11[];
extern u16 gObjPaletteBank12[];
extern u16 gObjPaletteBank12Color2[];
extern u16 gObjPaletteBank13Color1[];
extern u16 gObjPaletteBank15Color12[];

/* ROM */
extern u8 gFileSelectCursorUpdateTiles[];
extern u8 gUnk_08551110[];
extern u8 gUnk_08553210[];
extern u8 gUnk_08553510[];
extern u8 gMenuLoadSaveSlotPictureTiles[];
extern u16 gFileSelectCursorUpdatePalette[][4];
extern u16 gMenuLoadSaveSlotPalettePalette[];
extern u16 gUnk_08554D7A[];
extern u16 gUnk_08554D80[];
extern u16 gMenuBackgroundPalette[];
extern u16 gUnk_08559B68[][10];
extern u16 gUnk_08559B90[];
extern u16 gEraseConfirmDialogUpdatePalette[][16];
extern u16 gUnk_08559C24[][16];
extern u16 gUnk_08559CE6[];
extern u16 gUnk_08559CEC[];
extern const u8 gUnk_0855A5F8[];
extern u16 gUnk_0855D2F8[][10];
extern u16 gUnk_0855D320[];
extern u16 gModePlayerCountPanelUpdatePalette[][16];
extern u16 gUnk_08560DBC[][16];
extern u16 gUnk_08560F9C[];
extern u16 gUnk_08561224[][10];
extern u16 gUnk_08562FE4[][8];
extern u16 gUnk_08563024[][13];
extern u16 gUnk_0856342C[][10];
extern u16 gSoundTestPulsePalette[][16];
extern u16 gSoundTestHighlightCursorPalette[];
extern u16 gSoundTestCursorsUpdatePalette[][5];
extern const u8 gSoundTestDrawNumberTiles[];
extern u16 *gUnk_08731CF8[];
extern u16 *gUnk_08731D28[];
extern u16 gUnk_08731D58[];
extern u32 gMenuScreenTitleGfx[];
extern vs32 *const gBgScrollYPtrs[4];
extern vs32 *const gBgScrollXPtrs[4];
extern const s16 gUnk_08731DC0[];
extern u16 gUnk_08731E18[];
extern u16 gUnk_08731E1E[2][3];
extern u8 *gUnk_08731E2C[2];
extern void *const gUnk_08731E34[];
extern u16 gModeListCursorBaseY[];
extern u16 gModeListCursorRowStep[];
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
extern u32 gLinkPlayFrames[];
extern u32 gUnk_087556D4[];


/* Functions (defined in the files named above each group). */

/* src/menu_main_save_slots.c */
void MainMenuMain(void);
void MenuDrawSaveSlots(void);
void MenuLoadSaveSlotLabel(s32 slot);
void MenuLoadSaveSlotPicture(s32 slot, u32 pal);
s32 MenuLoadSaveSlotPalette(s32 slot, u32 pal);
void MenuLoadSaveSlotPercent(s32 slot, s32 value, s32 mode);

/* src/menu_file_select.c */
void MenuFileSelect(void);
void MenuSetupFileMenu(void);
void MenuFileMenuSelect(void);
void MenuNormalExtraSelect(void);
void MenuPlayerCountSelect(void);
void MenuEraseSelect(void);

/* src/menu_mode_list.c */
void MenuEnterModeList(void);
void MenuDrawModeList(void);
void MenuModeListSelect(void);
void MenuEnterLinkPlay(void);
void MenuModePlayerCountSelect(void);
void MenuEnterSoundTest(void);
void SoundTestHighlightCursor(void);
void SoundTestDrawNumber(s32 a);
void SoundTestPlaySfx(void);

/* src/menu_sound_test_link_play.c */
void MenuSoundTest(void);
void MenuLinkPlay(void);
void MenuSetLinkSessionMode(void);
s32 sub_0800da74(void);
void CreateFileSelectSprites(s32 mode);

/* src/menu_file_select_tasks.c */
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

/* src/menu_panel_tasks.c */
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

/* src/menu_sound_test_tasks.c */
void EraseConfirmDialogUpdate(void);
void Task_EraseFileWipe(void);
void EraseFileWipeDraw(void);
void Task_SoundTestCursors(void);
void SoundTestCursorsUpdate(void);
void Task_SoundTestPulse(void);
void Task_LinkPlayPalettePulse(void);
void Task_LinkPlayColorCycle(void);

/* src/menu_link_play_tasks.c */
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

/* src/menu_bg_scroll.c */
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
