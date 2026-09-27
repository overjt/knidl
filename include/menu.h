#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "gba/types.h"

/* menu.h: the RAM cells and ROM tables of the main menu, its sprite tasks and
   the BG scroll animator (M03).  One declaration per symbol, with the type
   its consumers prove (issue #36 phase 2, docs/header-conventions.md). */

/* EWRAM */
extern s8 gModeListExtraRows;
extern s8 gUnk_02004B44;
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

#endif /* GUARD_MENU_H */
