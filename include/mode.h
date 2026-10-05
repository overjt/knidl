#ifndef GUARD_MODE_H
#define GUARD_MODE_H

#include "gba/types.h"
#include "constants/game_states.h"

/* mode.h: the RAM cells and ROM tables of the game-state bodies AgbMain
   dispatches into, the boot/title sequence and the screen loaders (M02).  One
   declaration per symbol, with the type its consumers prove (issue #36 phase
   2, docs/header-conventions.md). */

/* One boot-logo sprite object: a command script (BootLogoUpdateObjects) moving a
   sprite in 24.8 fixed point. */
struct BootLogoObject
{
    /*0x00*/ s16 *scriptPos;    /* script cursor */
    /*0x04*/ s16 scriptId;     /* script id, -1 = off */
    /*0x06*/ s16 spriteId;     /* sprite id (gBootLogoSprites), -1 = none */
    /*0x08*/ s16 layer;     /* layer */
    /*0x0A*/ s16 sleepFrames;     /* frames to wait */
    /*0x0C*/ s32 posX;     /* x << 8 */
    /*0x10*/ s32 posY;     /* y << 8 */
    /*0x14*/ s16 velX;     /* x velocity */
    /*0x16*/ s16 velY;     /* y velocity */
    /*0x18*/ s16 accelX;     /* x acceleration */
    /*0x1A*/ s16 accelY;     /* y acceleration */
    /*0x1C*/ s16 loopCount;     /* loop count */
    /*0x1E*/ u16 unk1E;
};

struct TransferNode
{
    u32 cmd; /* mode in bits 0-3, byte size in bits 8-31 */
    u32 src;
    u32 dst;
};

/* EWRAM */
extern s8 gLinkSessionMode;
extern u32 gUnk_02004000[];
extern u16 gPausingPlayer;
extern u8 gExtraModeTitleSeen;
extern u8 gExtraModeTitleIndex;
extern s8 gBoardedWarpStarSlot;
extern u8 gExtraModeIndex;
extern u32 gUnk_02028000[];
extern struct BootLogoObject gUnk_02030000[];

/* IWRAM */
extern vu16 gUnk_03000B24;
extern u16 gBgPaletteBank9[];
extern u8 gBgPaletteBank10[];
extern u16 gBgPaletteBank14[];
extern u16 gPrevGameState; /* requested/next game state */
extern s32 gExtraModeTitlePhase;

/* ROM */
extern u8 gLoadMetaKnightmareOneUpTilesTiles[];
extern u8 gUnk_080D2AD0[];
extern u16 gUnk_08541D98[][16];
extern u16 gUnk_08541F58[];
extern u8 gUnk_0856F2A8[];
extern u8 gUnk_0856F308[];
extern u8 gUnk_0857014C[];
extern u8 gUnk_085704CC[];
extern u8 gUnk_085707D4[];
extern u8 gUnk_085708A8[];
extern u8 gUnk_085709EC[];
extern u8 gUnk_08570B28[];
extern u8 gUnk_08570F1C[];
extern u8 gUnk_0857111C[];
extern u16 gPauseChoicePalettes[][11];
extern u8 gUnk_08571248[];
extern u8 gUnk_0857172C[];
extern u8 gUnk_08571838[];
extern u8 gUnk_08571BE0[];
extern u8 gUnk_08571D74[];
extern u8 gUnk_08572164[];
extern u8 gUnk_085A3CB8[];
extern u8 gHudLoadLevelNamePalette[];
extern u8 gUnk_085A49E8[];
extern u16 gUnk_085B113C[];
extern u16 gUnk_085B119C[];
extern u16 gUnk_085B274C[];
extern u16 gUnk_085B2D0C[];
extern u16 gUnk_085B2D8C[];
extern u16 gUnk_085B450C[];
extern u16 gUnk_085B4ACC[];
extern u16 gUnk_085B4B4C[];
extern u16 gUnk_085B64C8[];
extern u8 gUnk_085B6A90[];
extern u8 gUnk_085B6AC0[];
extern u8 gUnk_085B6AC8[];
extern u16 gExtraModeTitleLevelBarUpdatePalette[][3][16];
extern u16 gUnk_085B6F98[];
extern u8 gUnk_085CCB58[];
extern void (*gExtraModeTitleSpriteStates[])(void);
extern void (*gExtraModeTitleSpriteStateUpdates[])(void);
extern u8 gUnk_0873079C[];
extern u16 *gUnk_08730884[];
extern struct TransferNode *gUnk_0873185C[];
extern u32 gUnk_08731980[][2][2];
extern u16 gUnk_087319B0[][2][2];
extern u32 gUnk_087319C8[][3];
extern u32 gExtraModeTitlePictures[][3];
extern u8 gExtraModeTitlePaletteSizes[];
extern u32 gAbilityPictures[][2];
extern u32 gUnk_08731B70[];
extern u16 gUnk_08731B88[][2];
extern u32 gUnk_08731BA0[][2];
extern u16 gUnk_08731C88[];
extern u16 gTitlePressStartLetterFrames[];
extern u8 gUnk_08731CDC[];
extern u32 gUnk_087555B4[];
extern u32 gUnk_08756054[];
extern u8 gUnk_0876B1FC[];
extern u8 gUnk_0876F690[];
extern u8 gUnk_087954C0[];
extern u8 gUnk_087C0A4C[];


/* Functions (defined in the files named above each group). */

/* src/mode_hub_stage.c */
void CheckPauseButton(void);
void HubMain(void);
void BigSwitchViewMain(void);
void StageMain(void);

/* src/mode_extra_mode_title.c */
s32 SendLinkBlockAndVerify(u32 *src, u32 *dst, u32 size);
s32 ExtraModeTitleSendModeData(void);
void ExtraModeTitleLinkErrorScreen(void);
void ExtraModeTitleLoadMultiBootImage(void);
void ExtraModeTitleMain(void);

/* src/mode_extra_mode_title_sprites.c */
void CreateExtraModeTitleSprites(void);
void Task_ExtraModeTitleSprite(void);
void ExtraModeTitleSpriteUpdate(void);
void ExtraModeTitleTransferIcon(void);
void ExtraModeTitleTransferIconUpdate(void);
void ExtraModeTitleLevelBar(void);
void ExtraModeTitleLevelBarUpdate(void);

/* src/mode_pause_boss_endurance.c */
void PauseScreen(void);
void PauseScreenLoadChoicePalette(s32 n);
void BossEnduranceMain(void);

/* src/mode_gfx_loaders.c */
void LinkErrorScreen(void);
void LoadBgLayout(s32 layout);
void LoadGfxSet(u16 set);
void LoadMetaKnightmareOneUpTiles(void);
void sub_08008cb8(void);
void SubGameLoadObjTiles(s32 a0, s32 a1);
void CutsceneLoadBgGraphics(s32 a0);
void ExtraModeTitleLoadPicture(s32 a0);
void HudLoadLevelName(s32 a0);
void HudClearAbilityPicture(void);
void HudLoadAbilityPicture(s32 a0);
void LoadMuseumAbilitySignGfx(s32 a0);
void PauseScreenLoadGraphics(s32 a0, s32 a1);

/* src/mode_boot_sequence.c */
void BootLogoMain(void);
s32 PlayBootLogo(void);
s32 BootLogoWait(s32 n);
s32 sub_080093cc(void);
void Task_BootLogo(void);
void Task_TitlePalette(void);
void Task_TitleSprites(void);
void TitleSpritesCreateLetters(void);
void TitleSpritesLetter(void);
void TitleMain(void);
s32 TitleScreen(void);
void IntroStory(void);
s32 IntroStoryWait(s32 n);

/* src/mode_hub_stage_init.c */
void ResetGameSession(void);
void ResetScoresAndMaxHealth(void);
void ResetPlayerRecords(void);
void HubResetPlayers(void);
void ResetTasksAndPlayers(void);
void StageInit(void);
void HubInit(void);
void BigSwitchViewInit(void);

#endif /* GUARD_MODE_H */
