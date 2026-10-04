#ifndef GUARD_MAIN_H
#define GUARD_MAIN_H

#include "gba/types.h"

/* main.h: the RAM cells and ROM tables of the main loop, interrupts, input,
   display shadows, sprites and fades (engine zone).  One declaration per
   symbol, with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct MusicPlayerInfo;

/* EWRAM */
/* Link session sequencer state / frame counters (EWRAM). */
extern u32 gLinkDriverMode;

/* IWRAM */
extern vs32 gBg0ScrollY; /* BG0 16.16 scroll shadows ... */
extern void (*gFrameEndCallback)(void);
extern vu16 gIntrEnable; /* REG_IE shadow */
extern u32 gUnk_03000020[4];
extern vu16 gPressedKeys; /* keys newly pressed */
extern void (*gFrameCallback)(void);
extern vu8 gBldCntTarget2; /* BLDCNT hi shadow */
extern vu16 gWin0V; /* WIN0V shadow */
extern vu16 gFadeBlankAtWhite;
extern vu16 gOamBuffer[]; /* OAM shadow (128 entries * 4 halfwords) */
extern u16 gPlayTime[4];
extern u32 gBlockAnimHook; /* per-frame stage hook (UpdateBlockAnims / UpdateBlockAnimsWithEdges / UpdateBg1BlockAnims) */
extern vs16 gFadeStep;
extern u32 gIntrTable[]; /* IRQ dispatch table (copied from 0x080CFDE8) */
extern const u32 gIntrTableTemplate[]; /* its ROM template, copied by AgbInit */
extern u16 gSpriteQueue[][6]; /* 12-byte records, indexed by gSpriteQueueTop */
extern u32 gVBlankEndCallback;
extern vu16 gSoundDisabled;
extern vu16 gWin1V; /* WIN1V shadow */
extern vu32 gWarmBoot;
extern u16 *gOamBufferCursor; /* OAM shadow write cursor */
extern vu8 gBldAlphaEva; /* BLDALPHA lo shadow (EVA) */
extern vs16 gVolumeRampMode;
extern vu16 gBg2Cnt; /* BG2CNT shadow */
extern vu16 gBg1Cnt; /* BG1CNT shadow */
extern vu8 gWinIn0; /* WININ lo shadow */
extern vu16 gOamAffineCount; /* affine matrix index */
extern vu16 gUnk_03000B20;
extern vu32 gSpriteLayerCounts[16]; /* per-lane counts (16 words, CpuFastSet-cleared) */
extern vu16 gRepeatedKeys; /* keys pressed w/ auto-repeat */
extern u32 gUnk_03000B74;
extern vs32 gBg3ScrollX; /* BG3HOFS shadow (16.16) */
extern vu16 gUnk_03000B7C;
extern vu8 gWinOut; /* transfer ring buffer end (0x03000B80 + 0x3FC) */
extern u32 gCopyQueueRead; /* copy-request queue read pointer */
extern vs32 gBg2ScrollX; /* BG2HOFS shadow (16.16) */
extern void (*gUnk_03000F90)(void);
extern vu16 gPlayerHeldKeys[]; /* per-player keys held */
extern vu32 gIsrSavedSp;
extern u32 gVBlankCallback;
extern vs32 gBg3ScrollY; /* BG3VOFS shadow (16.16) */
extern vu16 gVBlankCount;
extern vu32 gPaletteSource; /* palette source buffer ptr (0x03001270) */
extern vu32 gRngValue; /* RNG state */
extern vs16 gBrightness;
extern vs16 gVolumeRampLevel;
extern vs32 gBg1ScrollY; /* BG1VOFS shadow (16.16) */
extern u32 gCopyQueueWrite; /* copy-request queue write pointer */
extern vu16 gKeyRepeatDelay; /* auto-repeat first delay (14) */
extern vu16 gVolumeRampSpeed;
extern vu16 gFrameInProgress;
extern vu16 gWin0H; /* WIN0H shadow */
extern vu16 gUnk_03000FD8;
extern vu8 gObjMosaic; /* MOSAIC hi shadow */
extern vu16 gUnk_03001004;
extern vu16 gUnk_03001008;
extern vu16 gUnk_0300100C; /* keys currently held */
extern vu8 gWinObj; /* WINOUT hi shadow */
extern vu16 gUnk_03001014;
extern vu16 gWin1H; /* WIN1H shadow */
extern vu16 gUnk_03001020;
extern vu16 gUnk_03001170;
extern vu16 gFadeTimer;
extern vu16 gUnk_03001178;
extern vs32 gBg0ScrollX; /* BG0HOFS shadow (16.16) */
extern vu16 gBg0Cnt; /* BG0CNT shadow */
extern vu8 gBldCntTarget1; /* BLDCNT lo shadow */
extern vs16 gAffineSpriteBuffer[]; /* affine OBJ staging buffer */
extern u16 gBgPalette[];
extern u8 gObjPalette[]; /* OBJ palette buffer (M11 spelling) */
extern vu16 gKeyRepeatTimer; /* auto-repeat countdown */
extern u8 gSpriteLayerLists[16][64]; /* per-lane byte lists [lane][64] */
extern vu16 gAffineSpriteBufferPos; /* staging buffer write index */
extern u16 gFadedPalette[];
extern vu16 gFadeSteps; /* frames left to wait */
extern vs32 gBg2ScrollY; /* the wavy-scroll drivers of src/save_b6b08.c and src/save_b6d04.c read it as vu32 */
extern vu16 gFadeInterval;
extern vu16 gDispStat; /* REG_DISPSTAT shadow */
extern vu16 gUnk_03001EA0;
extern vu16 gFrameCount;
extern vu8 gBldAlphaEvb; /* BLDALPHA hi shadow (EVB) */
extern vu8 gBgMosaic; /* MOSAIC lo shadow */
extern vu16 gBg3Cnt; /* BG3CNT shadow */
extern vu16 gPlayerPressedKeys[]; /* keys pressed per player */
extern vu16 gWaitingForVBlank; /* VBlank wait flag */
extern vu16 gOamEntryCount; /* number of OAM entries used */
extern vu16 gKeyRepeatInterval; /* auto-repeat interval (4) */
extern vu8 gWinIn1; /* WININ hi shadow */
extern u16 *gFadeKeepMask;
extern vu16 gDispCnt; /* REG_DISPCNT shadow */
extern vs32 gBg1ScrollX; /* BG1HOFS shadow (16.16) */
extern vu16 gSoundDriverOn;
extern vs32 gSpriteQueueTop; /* record counter, -1 = empty */
extern vu16 gBldY; /* BLDY shadow */
extern vu16 gHeldKeys; /* keys held last frame */
extern vu16 gIntrMasterEnable; /* REG_IME shadow */
extern s8 gDigits[]; /* decimal digit buffer, [5] = sign/flag */
extern u16 gLinkIsMaster; /* link-mode flag */
extern s16 gSpriteCameraX; /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY; /* scalar, read with ldrsh (32 landed files) */
extern u16 gLinkPlayerCount; /* number of linked players */
extern u32 gLinkPauseFrames;
extern u16 gLinkCommand; /* link session state, high byte = command */
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;

/* ROM */
extern u8 gObjVram[]; /* OBJ tile VRAM (M13's LoadAbilityTiles) */
extern const u8 gUnk_0872EB14[4][4][2]; /* shape/size -> {w,h} half-dims */
extern const u8 gBootSignature[];
extern s16 gCosTable[]; /* trig table (mid pointer) */


/* Functions (defined in the files named above each group). */

struct TransferNode;

/* src/agb_init.c */
void AgbInit(void);

/* src/early_08e8.c */
void UpdateFade(void);

/* src/early_0de4.c */
void EndFrame(void);

/* src/early_10cc.c */
void VBlankIntr(void);

/* src/early_11ac.c */
void CopyOamAndPalette(void);
void ReadKeys(void);
void FlushDisplayRegs(void);
void ProcessCopyQueue(void);
void SetHBlankIntr(void (*fn)(void));
void ClearHBlankIntr(void);
void SetVCountIntr(void (*fn)(void), u8 vcount);
void ClearVCountIntr(void);

/* src/early_1518.c */
void IntrDummy(void);
void EnableForcedBlank(void);
void DisableForcedBlank(void);
void RequestCopyList(struct TransferNode *node);
void ResetOamShadow(void);
void ResetSpriteQueue(void);
void RunBuildOamInIwram(void);

/* src/early_1b08.c */
void BuildOam(void);

/* src/early_1fd0.c */
void ResetBgScroll(void);
void ResetFadeAndBlend(void);
void BeginFadeInFromWhite(void);
void BeginFadeInFromBlack(void);
void BeginFastFadeInFromWhite(void);
void BeginFadeOutToWhite(void);
void BeginFastFadeOutToWhite(void);
void BeginFadeOutToBlack(void);
u32 CheckWarmBoot(void);
void ResetTasksAndOam(void);
void ResetPlayTime(void);
void RunFrameNoTasks(void);
void RunFrame(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkStartRecordExchange(void);
void LinkRequestSync(void);

/* src/early_5d9c.c */
void TaskDrawWorld(void);
void TaskDrawWorldOrFree(void);
void TaskDrawWorldInView(void);
void TaskDrawWorldInViewOrFree(void);
void TaskDrawWorldLoadTiles(void);
void TaskDrawWorldTilesLoaded(void);
void sub_080060c0(void);
void TaskSleepForever(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionX(s32 a, s32 b, s32 c);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStopX(void);
void TaskSetMotionY(s32 a, s32 b, s32 c);
void TaskStopY(void);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskStopSlot(u32 i);
void TaskUpdateFlip(void);
void TaskSetFrame(s32 a);
void TaskSetFrameNoFlip(s32 a);
void TaskSetFrameFlip(s32 a);
void TaskStepForward(s16 a);

/* src/main.c */
void AgbMain(void);

#endif /* GUARD_MAIN_H */
