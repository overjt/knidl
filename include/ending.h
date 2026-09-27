#ifndef GUARD_ENDING_H
#define GUARD_ENDING_H

#include "gba/types.h"

/* ending.h: the RAM cells and ROM tables of the ending: AgbMain state 11, the
   final results, the staff credits, the boot logo objects and the game-over
   screen (M37-M38).  One declaration per symbol, with the type its consumers
   prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxHeader;

/* EWRAM */
extern u16 gUnk_02004C94;
extern s16 gGameOverTimer; /* game-over screen: frames left */
extern s8 gGameOverCursor; /* game-over screen: cursor (continue = 0?) */
extern u16 gUnk_0200616C;
extern u8 gGameOverDone; /* game-over screen: done flag */
extern s16 gGameOverPlayerTask; /* game-over screen: the #264 variant-0 task's index */
extern u16 gUnk_02007D3C;
extern u8 gEndingSceneActive;
extern s16 *gUnk_0201BFD0[];
extern u8 gUnk_0201C19C;
extern s32 gUnk_0201C1A0; /* credits: BG0 vertical scroll, 16.16 */
extern s32 gUnk_0201C1A4; /* credits: the score saved over the demos */
extern u8 gUnk_0201C1A8;
extern s32 gUnk_0201C1AC; /* credits: BG0 horizontal scroll, 16.16 */
extern u8 gUnk_0201C1B0; /* credits: current demo scene */
extern s32 gUnk_0201C1B4; /* credits: scroll since the last page copy, 1/16 pixel */

/* IWRAM */
extern u16 gUnk_030014F0[];

/* ROM */
extern u32 gUnk_080DBEF8[];
extern u32 gUnk_080DBF00[];
extern u32 gUnk_080DBF08[];
extern u32 gUnk_080DBF18[];
extern u32 gUnk_080DBF28[];
extern u32 gUnk_080DBF30[];
extern u16 gUnk_08584BB0[][4];
extern struct GfxHeader gUnk_085995AC;
extern struct GfxHeader gUnk_0859990C;
extern struct GfxHeader gUnk_0859A09C;
extern u16 gUnk_0859A0B0[];
extern u16 gUnk_0859A0D0[];
extern u16 gUnk_0859A0F0[];
extern u16 gUnk_0859A110[];
extern u16 gUnk_085E0070[];
extern u32 gUnk_085E0090[];
extern u32 gUnk_085E2C20[];
extern u32 gUnk_085E2CE0[];
extern u32 gUnk_085E4064[];
extern u32 gUnk_085E5BC4[];
extern u32 gUnk_0874CEE8[];
extern u32 gUnk_0874CF28[];
extern u32 gUnk_0874CF94[];
extern u32 gUnk_087548A8[];
extern u32 gUnk_087548B8[];
extern u32 gUnk_08754908[];
extern u32 gUnk_08754914[];
extern u32 gUnk_08754984[];
extern u32 gUnk_087549B0[];
extern u32 gUnk_087549FC[];
extern u32 gUnk_087554B8[];
extern u32 gUnk_087556E0[];
extern u32 gUnk_08755708[];
extern u32 gUnk_0875581C[];
extern u32 gUnk_0875585C[];
extern void (*gUnk_08757330[])(void);
extern u16 gUnk_0875735C[];
extern s32 gUnk_08757374[];
extern s32 gUnk_08757394[];
extern s32 gUnk_087573B4[];
extern s32 gUnk_087573D4[];
extern void (*gUnk_087573F4[])(void);
extern u16 gUnk_08757424[];
extern u16 gUnk_08757432[];
extern u16 gUnk_0875743E[];
extern s16 gUnk_08757440[];
extern s16 *gUnk_087577D8[];
extern u16 gUnk_08758274[];
extern u16 gUnk_08758284[];
extern void (*gGameOverObjectVariants[])(void);
extern void (*gUnk_087582AC[])(void);
extern void (*gUnk_087582B8[])(void);
extern void (*gUnk_087582C4[])(void);
extern void (*gUnk_087582DC[])(void);
extern s32 gUnk_087582F4[];
extern void (*gUnk_08758324[])(void);
extern void (*gUnk_0875832C[])(void);
extern u16 gUnk_08758334[];
extern u16 gUnk_08758374[];
extern u32 *gUnk_087583B4[]; /* credits: the 14 compressed text pages */
extern u32 gUnk_087583CC[][8]; /* credits: per variant, the scenes' recorded demos, 0-terminated */
extern u16 gUnk_0875841E[][7]; /* credits: per variant, the scenes' lengths in frames */


/* Functions (defined in the files named above each group). */

/* src/mode_c6260.c */
void EndingMain(void);
void sub_080c62f0(void);
void sub_080c6354(void);
void sub_080c6388(void);
void sub_080c63ec(void);

/* src/results_c6420.c */
void FinalResultsScreen(void);
void DrawLargeClockScreen(void);
void ShowMilestonePicture(void);
void ShowMilestonePictureForMode(s32 slot);
void DrawScoreToBgMap(s32 v, s32 x, s32 y);
void DrawClockToBgMap(u16 *time, s32 x, s32 y);
void CopyToBgMap(u16 *src, s32 x, s32 y, s32 n);

/* src/ending_c6c64.c */
void sub_080c6c64(void);
void sub_080c6ca0(void);
void sub_080c6d38(void);
void sub_080c6d84(void);
void sub_080c769c(void);
void sub_080c77cc(void);
void sub_080c7810(void);
void sub_080c7cc0(void);

/* src/ending_c7e4c.c */
void sub_080c7e4c(void);
void sub_080c8298(void);
void sub_080c8468(void);
void sub_080c8498(void);
void sub_080c85d8(void);
void sub_080c8778(void);
void sub_080c88f0(void);
void sub_080c8924(void);
void sub_080c8958(void);
void sub_080c8cd4(void);
void sub_080c8e88(void);
void sub_080c8ea8(void);

/* src/ending_c9004.c */
void sub_080c9004(void);
void sub_080c9040(void);
void sub_080c90c8(void);
void sub_080c9114(void);
void sub_080c9418(void);
void sub_080c94cc(void);
void sub_080c972c(void);
void sub_080c97a0(void);
void sub_080c9884(void);
void sub_080c98d8(void);
void sub_080c9974(void);
void sub_080c9a28(void);
void sub_080c9cf0(void);
void sub_080c9d10(void);
void sub_080c9e8c(void);
void sub_080ca344(void);
void sub_080ca570(void);
void sub_080ca640(void);
void sub_080ca71c(void);
void sub_080ca830(void);
void sub_080ca8f0(void);

/* src/boot_caa3c.c */
void BootLogoInitObjects(void);

/* src/boot_caab8.c */
void BootLogoUpdateObjects(void);

/* src/gameover_cacf0.c */
void GameOverMain(void);
void GameOverScreen(void);
void sub_080caeec(void);
void GameOverShowClock(s32 n);
void sub_080cb058(void);
void GameOverMoveCursor(void);
u8 GameOverIsUpDownPressed(void);
u8 GameOverCheckConfirm(void);
void GameOverCheckTimeout(void);
void GameOverCheckChoice(void);
void GameOverLoadGraphics(void);
void GameOverResetWait(void);
void CreateGameOverObjects(void);

/* src/gameover_cb354.c */
void Task_GameOverSprite(void);
void Task_GameOverCursor(void);
void Task_GameOverPalette(void);
void Task_HalveScore(void);
void Task_GameOverObject(void);
void GameOverPlayer(void);
void GameOverPlayerUpdate(void);
void GameOverPlayerEnterState(void);

/* src/gameover_cb64c.c */
void sub_080cb64c(void);
void sub_080cb6d8(void);
void sub_080cb70c(void);
void sub_080cbabc(void);
void sub_080cbac0(void);
void sub_080cbea4(void);

/* src/gameover_cbed4.c */
void GameOverChoice(void);
void GameOverChoiceUpdate(void);
void CreateGameOverObject(u8 variant);
void sub_080cbf68(void);
void sub_080cbfac(void);
void GameOverChoiceEnterState(void);
void sub_080cbfe4(void);
void sub_080cc024(void);
void sub_080cc0a4(void);
void sub_080cc14c(void);
void sub_080cc180(void);
void sub_080cc2b8(void);
void sub_080cc2e0(void);
void sub_080cc5d4(void);
void sub_080cc608(void);
void sub_080cc740(void);
void sub_080cc768(void);
void sub_080ccd10(void);

/* src/gameover_ccd4c.c */
void sub_080ccd4c(void);
void sub_080cce98(void);
void sub_080ccec8(void);
void sub_080ccf10(void);
void sub_080ccf2c(void);
void sub_080cd0c8(void);
void sub_080cd0cc(void);
void sub_080cd248(void);
void sub_080cd24c(void);
void sub_080cd2f8(void);

/* src/credits_cd330.c */
void CreditsMain(void);
void CreditsLoadScene(void);
void CreditsInitText(void);
void CreditsStreamText(void);
void CreditsScrollText(void);

#endif /* GUARD_ENDING_H */
