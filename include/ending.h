#ifndef GUARD_ENDING_H
#define GUARD_ENDING_H

#include "gba/types.h"

/* ending.h: the RAM cells and ROM tables of the ending: AgbMain state 11, the
   final results, the staff credits, the boot logo objects and the game-over
   screen (M37-M38).  One declaration per symbol, with the type its consumers
   prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxHeader;

/* EWRAM */
extern u16 gEndingPlayerCount;
extern s16 gGameOverTimer; /* game-over screen: frames left */
extern s8 gGameOverCursor; /* game-over screen: cursor (continue = 0?) */
extern u16 gEndingLinkPlayerCount;
extern u8 gGameOverDone; /* game-over screen: done flag */
extern s16 gGameOverPlayerTask; /* game-over screen: the #264 variant-0 task's index */
extern u16 gEndingLinkIsMaster;
extern u8 gEndingSceneActive;
extern s16 *gBootLogoSavedCursors[];
extern u8 gCreditsTextPageStaged;
extern s32 gCreditsTextScrollY; /* credits: BG0 vertical scroll, 16.16 */
extern s32 gUnk_0201C1A4; /* credits: the score saved over the demos */
extern u8 gCreditsTextPage;
extern s32 gCreditsTextScrollX; /* credits: BG0 horizontal scroll, 16.16 */
extern u8 gUnk_0201C1B0; /* credits: current demo scene */
extern s32 gCreditsTextPageScroll; /* credits: scroll since the last page copy, 1/16 pixel */

/* IWRAM */
extern u16 gObjPaletteBank4[];

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
extern u32 gGameOverKnockedOutPlayersFrames[];
extern u32 gUnk_087548B8[];
extern u32 gUnk_08754908[];
extern u32 gGameOverPlayerFrames[];
extern u32 gUnk_08754984[];
extern u32 gUnk_087549B0[];
extern u32 gUnk_087549FC[];
extern u32 gBootLogoSprites[];
extern u32 gGameOverScreenFrames[];
extern u32 gEndingEpilogueFrames[];
extern u32 gEndingEpilogueStarRodFrames[];
extern u32 gEndingStarRodReturnFrames[];
extern void (*gEndingEpilogueVariants[])(void);
extern u16 gEndingEpilogueObjectVariants[];
extern s32 gUnk_08757374[];
extern s32 gUnk_08757394[];
extern s32 gUnk_087573B4[];
extern s32 gUnk_087573D4[];
extern void (*gEndingStarRodReturnVariants[])(void);
extern u16 gEndingStarRodReturnObjectVariants[];
extern u16 gUnk_08757432[];
extern u16 gUnk_0875743E[];
extern s16 gBootLogoObjectSeeds[];
extern s16 *gBootLogoScripts[];
extern u16 gUnk_08758274[];
extern u16 gUnk_08758284[];
extern void (*gGameOverObjectVariants[])(void);
extern void (*gGameOverPlayerStates[])(void);
extern void (*gGameOverPlayerStateUpdates[])(void);
extern void (*gGameOverChoiceStates[])(void);
extern void (*gGameOverChoiceStateUpdates[])(void);
extern s32 gUnk_087582F4[];
extern void (*gUnk_08758324[])(void);
extern void (*gUnk_0875832C[])(void);
extern u16 gUnk_08758334[];
extern u16 gUnk_08758374[];
extern u32 *gCreditsTextPages[]; /* credits: the 14 compressed text pages */
extern u32 gCreditsDemoRecordings[][8]; /* credits: per variant, the scenes' recorded demos, 0-terminated */
extern u16 gCreditsDemoLengths[][7]; /* credits: per variant, the scenes' lengths in frames */


/* Functions (defined in the files named above each group). */

/* src/mode_c6260.c */
void EndingMain(void);
void EndingEpilogueScene(void);
void CreateEndingEpilogue(void);
void EndingStarRodReturnScene(void);
void CreateEndingStarRodReturn(void);

/* src/results_c6420.c */
void FinalResultsScreen(void);
void DrawLargeClockScreen(void);
void ShowMilestonePicture(void);
void ShowMilestonePictureForMode(s32 slot);
void DrawScoreToBgMap(s32 v, s32 x, s32 y);
void DrawClockToBgMap(u16 *time, s32 x, s32 y);
void CopyToBgMap(u16 *src, s32 x, s32 y, s32 n);

/* src/ending_c6c64.c */
void Task_EndingEpilogue(void);
void EndingEpilogueLoadGraphics(void);
void CreateEndingEpilogueObjects(void);
void EndingEpilogueWarpStar(void);
void EndingEpilogueWarpStarDraw(void);
void CreateEndingEpilogueWarpStarEffects(void);
void EndingEpilogueWarpStarEffect(void);
void EndingEpilogueTrailStar(void);

/* src/ending_c7e4c.c */
void EndingEpilogueKirby(void);
void EndingEpilogueKirbyDraw(void);
void CreateEndingEpilogueStarRod(void);
void EndingEpilogueStarRod(void);
void EndingEpilogueKingDedede(void);
void EndingEpilogueKingDededeDraw(void);
void EndingEpilogueExplosion(void);
void CreateEndingEpilogueExplosionSprites(void);
void EndingEpilogueExplosionSprite(void);
void EndingEpilogueCamera(void);
void EndingEpilogueCameraUpdate(void);
void EndingEpilogueStoryText(void);

/* src/ending_c9004.c */
void Task_EndingStarRodReturn(void);
void EndingStarRodReturnLoadGraphics(void);
void CreateEndingStarRodReturnObjects(void);
void EndingStarRodReturnStarRod(void);
void EndingStarRodReturnStarRodDraw(void);
void EndingStarRodReturnWarpStar(void);
void EndingStarRodReturnWarpStarUpdate(void);
void EndingStarRodReturnWarpStarDraw(void);
void EndingStarRodReturnTrailStar(void);
void EndingStarRodReturnKirby(void);
void EndingStarRodReturnKirbyDraw(void);
void sub_080c9a28(void);
void sub_080c9cf0(void);
void EndingStarRodReturnConvergingStars(void);
void EndingStarRodReturnBurstStar(void);
void EndingStarRodReturnFallingStar(void);
void EndingStarRodReturnFallingStarDriftFast(void);
void EndingStarRodReturnFallingStarDriftSlow(void);
void EndingStarRodReturnFountainJet(void);
void sub_080ca830(void);
void sub_080ca8f0(void);

/* src/boot_caa3c.c */
void BootLogoInitObjects(void);

/* src/boot_caab8.c */
void BootLogoUpdateObjects(void);

/* src/gameover_cacf0.c */
void GameOverMain(void);
void GameOverScreen(void);
void GameOverMetaKnightmareScreen(void);
void GameOverShowClock(s32 n);
void GameOverBossEnduranceScreen(void);
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
void GameOverPlayerWait(void);
void GameOverPlayerWaitUpdate(void);
void GameOverPlayerContinue(void);
void GameOverPlayerContinueUpdate(void);
void GameOverPlayerGiveUp(void);
void GameOverPlayerGiveUpUpdate(void);

/* src/gameover_cbed4.c */
void GameOverChoice(void);
void GameOverChoiceUpdate(void);
void CreateGameOverObject(u8 variant);
void GameOverPlayerShowChoice(void);
void sub_080cbfac(void);
void GameOverChoiceEnterState(void);
void GameOverChoiceWait(void);
void GameOverChoiceWaitUpdate(void);
void GameOverChoiceMove(void);
void GameOverChoiceMoveUpdate(void);
void GameOverChoiceContinueStart(void);
void GameOverChoiceContinueStartUpdate(void);
void GameOverChoiceContinueEnd(void);
void GameOverChoiceContinueEndUpdate(void);
void GameOverChoiceGiveUpStart(void);
void GameOverChoiceGiveUpStartUpdate(void);
void GameOverChoiceGiveUpEnd(void);
void GameOverChoiceGiveUpEndUpdate(void);

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
void GameOverKnockedOutPlayers(void);

/* src/credits_cd330.c */
void CreditsMain(void);
void CreditsLoadScene(void);
void CreditsInitText(void);
void CreditsStreamText(void);
void CreditsScrollText(void);

#endif /* GUARD_ENDING_H */
