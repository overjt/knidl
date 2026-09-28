#ifndef GUARD_SUBGAME_H
#define GUARD_SUBGAME_H

#include "gba/types.h"

/* subgame.h: the RAM cells and ROM tables of the sub-game framework and the
   three sub-games (M35-M37).  One declaration per symbol, with the type its
   consumers prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxDesc
{
    u16 unk00;
    u16 unk02;
    u32 unk04;
    u32 unk08;
    const void *unk0C;
};

/* per-player records of gAirGrindCourse, M37Course.players[4] (0x3C bytes) */
struct M37CoursePlayer
{
    /*0x00*/ s32 coursePos;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 prevCoursePos;
    /*0x38*/ s32 unk38;
};

/* gAirGrindCourse, reached through gAirGrindCoursePtr (and directly by the
   0x080C5284-0x080C623C builder) */
struct M37Course
{
    /*0x000*/ s32 scrollPos;
    /*0x004*/ s32 unk004;
    /*0x008*/ s32 unk008;
    /*0x00C*/ s32 unk00C;
    /*0x010*/ s32 finishLine;
    /*0x014*/ s32 unk014;
    /*0x018*/ struct M37CoursePlayer players[4];
    /*0x108*/ s32 unk108;
    /*0x10C*/ s32 unk10C;
    /*0x110*/ s32 unk110;
};

/* per-player records, M37Game.players[4] (0x34 bytes) */
struct M37Player
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ s16 unk06;
    /*0x08*/ s16 unk08;
    /*0x0A*/ s16 unk0A;
    /*0x0C*/ s16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ u8 unk20;
    /*0x21*/ u8 unk21;
    /*0x22*/ u8 pad22[2];
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
};

/* 16-byte object records, M37ObjSet.unk04[7] (sub_080c4664, sub_080c4790) */
struct M37Obj
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;          /* 16.16; the ROM also reads its high half */
    /*0x0C*/ s32 unkC;          /* 16.16; the ROM also reads its high half */
};

/* M37Game.unk0EC (0xB8 bytes with the compiler's 2-byte tail pad): task
   type #96 variant 1's seven scrolling objects.  sub_080c4664 addresses an
   object as &set->unk04[i] off the set's own base (`lsls #4; adds #4`), so
   the records are a sub-struct, not flat fields; the two 16-colour rows
   AirGrindSetupRace fills end it exactly at M37Game.randomStates. */
struct M37ObjSet
{
    /*0x00*/ s32 unk00;         /* the scroll position last frame */
    /*0x04*/ struct M37Obj unk04[7];
    /*0x74*/ s16 unk74;         /* the last object's sprite id */
    /*0x76*/ u16 unk76[16];     /* M37Game + 0x162 */
    /*0x96*/ u16 unk96[16];     /* M37Game + 0x182 */
};

/* gAirGrind, the game's state; always used through gAirGrindPtr */
struct M37Game
{
    /*0x000*/ s32 level;       /* the level (M36's AirGrindInit copies gSubGameLevel) */
    /*0x004*/ s32 raceTimes[4];
    /*0x014*/ s32 unk014;
    /*0x018*/ s32 unk018;
    /*0x01C*/ struct M37Player players[4];
    /*0x0EC*/ struct M37ObjSet unk0EC;
    /*0x1A4*/ s32 randomStates[5];    /* five LCG streams (AirGrindRandom, AirGrindRandomRange) */
    /*0x1B8*/ s32 unk1B8;
    /*0x1BC*/ u16 skyLineColors[160]; /* per-scanline colour, HBlank DMA source */
    /*0x2FC*/ u16 backdropColor;
    /*0x2FE*/ u8 pad2FE[2];
    /*0x300*/ u32 frameCount;       /* frame counter */
    /*0x304*/ s16 unk304;       /* AirGrindScaleSprite's OAM list: entry count */
    /*0x306*/ s16 unk306[160];  /* ... and entries */
    /*0x446*/ u16 localPlayer;       /* gLocalPlayer */
    /*0x448*/ u16 playerCount;       /* gLinkPlayerCount */
    /*0x44A*/ u8 pad44A[2];
    /*0x44C*/ s32 unk44C;       /* a task index into gTasks */
    /*0x450*/ u8 unk450;
    /*0x451*/ u8 unk451;
    /*0x452*/ u8 pad452[2];
};

/* the results screen's state (AirGrindResults, sub_080c25c4, sub_080c2740) */
struct M37Results
{
    /*0x00*/ s8 unk00;          /* state */
    /*0x01*/ s8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ u8 unk04[4];       /* the players, sorted by score */
    /*0x08*/ u8 unk08[4];       /* each player's index into unk04 */
    /*0x0C*/ u8 unk0C[4];       /* the places, ties shared */
    /*0x10*/ s32 unk10;         /* the winner's pulsing scale */
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;         /* frame timer */
    /*0x1C*/ s32 unk1C[4];
};

/* gAirGrindScript: a cursor into one of the u16-pair scripts gUnk_087572EC[]
   (AirGrindClearScript clears it, AirGrindStepScript steps it, 0x8000 = end, 0x9999 =
   loop) */
struct M37Script
{
    /*0x00*/ u8 scriptId;           /* script id, 0 = none */
    /*0x01*/ u8 step;           /* step */
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
};

/* four 40-byte records at gAirGrindPaletteFades (AirGrindStartPaletteFade fills one) */
struct M37Timer
{
    /*0x00*/ s32 unk00;         /* in use */
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
};

/* EWRAM */
extern u8 gUnk_02004B5C;
extern s16 gUnk_020055EC;
extern u8 gQuickDrawBestTime;
extern u8 gBombRallySafeBeatsLeft;
extern s8 gBombRallySeats[];
extern u8 gSubGamePhase;
extern u8 gBombRallyOutMask;
extern u8 gBombRallyOutCount;
extern u8 gQuickDrawWins[];
extern u8 gBombRallyFinishOrder[];
extern u8 gUnk_0200B048;
extern u8 gQuickDrawRanking[];
extern struct M37Game gAirGrind;
extern struct M37Game *gAirGrindPtr;
extern struct M37Timer gAirGrindPaletteFades[4];
extern struct M37Results gAirGrindResults;
extern struct M37Course *gAirGrindCoursePtr;
extern u16 gAirGrindFrame;
extern s16 gUnk_02017180[4][256];
extern s16 gUnk_02017980[4][500];
extern u32 gUnk_02018920[];
extern s16 gUnk_02019140[4][500];
extern u32 gUnk_0201A0E0[4][256];
extern struct M37Course gAirGrindCourse;
extern s16 gUnk_0201B1F4;
extern s32 gUnk_0201B200[4][73];
extern s32 gUnk_0201B690[];
extern s16 gUnk_0201B7C0[4][256];
extern s32 gUnk_0201BFC0;

/* IWRAM */
extern u16 gUnk_03001510[];
extern struct M37Script gAirGrindScript;

/* ROM */
extern u8 gUnk_080CFE2C[][4];
extern s32 gUnk_080CFE3C[][3];
extern u8 gUnk_080CFE60[11][3];
extern u8 gUnk_080CFE81[11][3];
extern u8 gUnk_080CFEA2[11][3];
extern u8 gUnk_080CFEC3[11][3];
extern u8 gUnk_080CFEE4[];
extern u8 gUnk_080CFEE9[];
extern s8 gUnk_080CFF01[];
extern u16 gUnk_080CFF1C[];
extern s16 gUnk_080CFF52[];
extern s16 gUnk_080CFF60[];
extern s16 gUnk_080CFF70[];
extern u8 gUnk_080CFF76[4][4][2]; /* OBJ shape/size -> {width, height} */
extern u16 gUnk_080D0198[];
extern s16 gUnk_080D0398[];
extern s16 gUnk_080D059A[];
extern s16 gUnk_080D075A[];
extern s16 gUnk_080D0760[];
extern s16 gUnk_080D0766[];
extern u16 gUnk_08609D42[][16];
extern u16 gUnk_08609E40[];
extern u16 gUnk_08609F40[][16];
extern u16 gUnk_0860A042[];
extern u32 gUnk_087559E4[];
extern u32 gUnk_087559F4[];
extern u32 gUnk_08755A04[];
extern u32 gUnk_08755A14[];
extern u32 gUnk_08755A24[];
extern u32 gUnk_08755A34[];
extern u32 gUnk_08755A5C[];
extern u32 gUnk_08755A68[];
extern u32 gUnk_08755A78[];
extern u32 gUnk_08755A7C[];
extern u32 gUnk_08755A88[];
extern u32 gUnk_08755AA4[];
extern u32 gUnk_08755AB8[];
extern u32 gUnk_08755AC8[];
extern u32 gUnk_08755AD8[];
extern u32 gQuickDrawBurstFrames[];
extern u32 gUnk_08755AF0[];
extern u32 gUnk_08755B18[];
extern u32 gUnk_08755B40[];
extern u32 gUnk_08755B68[];
extern u32 gUnk_08755B90[];
extern u32 gUnk_08755BAC[];
extern u32 gUnk_08755DC0;
extern u32 gBombRallyBombFrames[];
extern u32 gUnk_08755E0C[];
extern u32 gUnk_08755E44[];
extern u32 gUnk_08755E7C[];
extern u32 gUnk_08755EB4;
extern u32 gBombRallyBombSmokeFrames[];
extern u32 gBombRallyStarFrames[];
extern u32 gUnk_08755EFC[];
extern u32 gUnk_08755F1C[];
extern u32 gUnk_08755F2C[];
extern u32 gUnk_08755F3C[];
extern u16 *gUnk_08755F54[];
extern u32 gUnk_08755FA8[];
extern u32 gUnk_08755FBC[];
extern u32 gUnk_08755FC4[];
extern u32 gUnk_08755FEC[];
extern u32 gUnk_0875602C[];
extern u32 gUnk_0875603C[];
extern u32 gUnk_087562A8[][2];
extern u16 gUnk_087562C0[];
extern s32 (*const gSubGameInitHooks[])(void);
extern void *gSubGameBodies[];
extern u16 gUnk_087562E4[];
extern u32 gQuickDrawPhases[];
extern s16 gUnk_087562F0[];
extern s16 gUnk_087562F6[];
extern u32 gQuickDrawStates[];
extern u32 gQuickDrawStateUpdates[];
extern u32 gUnk_08756334[];
extern u32 gUnk_08756350[];
extern s16 gUnk_0875636C[];
extern u32 gUnk_08756378[];
extern u32 gUnk_08756394[];
extern u32 gQuickDrawObjectKinds[];
extern u16 gUnk_087563D8[];
extern u16 gUnk_08756410[];
extern s16 gUnk_08756448[];
extern s16 gUnk_08756450[];
extern s16 gUnk_08756458[];
extern s16 gUnk_08756460[];
extern u32 gUnk_08756468[];
extern u32 gUnk_08756480[];
extern u16 gUnk_08756498[];
extern u32 gUnk_087564A0[];
extern s16 gUnk_087564B0[];
extern struct GfxDesc *const gUnk_087564D0[];
extern u32 gUnk_087564E4[];
extern u32 gUnk_087564FC[];
extern u16 gUnk_08756514[];
extern u16 gUnk_0875651C[];
extern u32 gUnk_08756528[];
extern u16 gUnk_08756538[];
extern s32 gUnk_08756540[];
extern s32 gUnk_08756550[];
extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gBombRallyPhases[];
extern u8 gBombRallyBeatFrames[];
extern u8 gUnk_087565E0[];
extern u8 *gUnk_087565F4[];
extern u8 *gBombRallyBlastOdds[];
extern u8 gBombRallyStartSpeeds[];
extern u8 gBombRallySpeedUpBeats[];
extern u8 gBombRallySafeBeats[];
extern u32 gBombRallyStates[];
extern u32 gBombRallyStateUpdates[];
extern u32 gBombRallyResultsStates[];
extern u32 gBombRallyResultsStateUpdates[];
extern u32 gBombRallyObjectVariants[];
extern u32 gBombRallyPlayerStates[];
extern u32 gBombRallyPlayerStateUpdates[];
extern u32 *gBombRallyPlayerFrames[];
extern u32 *gBombRallyBubblesFrames[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_0875673C[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gUnk_0875674C[];
extern u32 gUnk_0875675C[];
extern u8 gUnk_0875676C[];
extern s16 gUnk_08756770[];
extern u16 gUnk_08756778[];
extern u32 gBombRallyBombStates[];
extern u32 gBombRallyBombStateUpdates[];
extern s16 gUnk_08756798[];
extern s16 gUnk_087567A0[];
extern s32 gUnk_087567A8[][3];
extern s32 *gUnk_08756D3C[][2];
extern s32 gUnk_08756D74[];
extern s32 gUnk_08756DC8[];
extern s32 gUnk_08756E1C[];
extern s32 gUnk_08756E38[];
extern s32 gUnk_08756E54[][4];
extern s32 gUnk_08756EC4[][4];
extern s32 gUnk_08756F34[][4];
extern s32 gUnk_08756FA4[][4];
extern s16 gUnk_08757014[];
extern s32 *gUnk_0875716C[];
extern s32 gUnk_08757178[][4];
extern s32 gUnk_087571B8[][4];
extern u32 gBombRallyStarBurstStates[];
extern u32 gUnk_08757238[];
extern u32 gUnk_08757244[];
extern s32 gUnk_08757250[];
extern s32 gUnk_08757260[];
extern u32 gUnk_08757270[];
extern u32 gUnk_08757278[];
extern u8 gUnk_08757280[];
extern u32 gAirGrindPhases[];
extern void (*gAirGrindObjectVariants[])(void);
extern u32 gUnk_087572E0[];
extern u16 *gUnk_087572EC[];
extern s32 *gUnk_08757300[];
extern s32 *gUnk_08757310[];
extern u16 *gUnk_08757320[];


/* Functions (defined in the files named above each group). */

/* src/subgame_b9d0c.c */
void SubGameReplay(s32 a0);
void SubGameQuit(void);
s32 sub_080b9d48(void);
u8 sub_080b9d68(void);
u8 sub_080b9da8(void);
void sub_080b9de8(void);
void SubGameCheckEnd(void);
void SubGameLoadScreen(s32 a0);
void sub_080b9ea0(s32 a0);
void SubGameRunScreen(s32 a0);
void SubGameRunLinkFrame(void);
void SubGameRunFrame(void);
void SubGameSyncLink(void);
s32 FreeOtherTasks(void);
void SubGameMain(void);
void Task_SubGame(void);
void SubGameStartBody(void);
void QuickDrawInit(void);
void QuickDrawMain(void);
void QuickDrawFreeze(void);
void CreateQuickDrawTimer(void);
void CreateQuickDrawPlayers(s32 a0);
void sub_080ba61c(void);
void CreateQuickDrawSignal(void);
void QuickDrawStartTimer(void);
void QuickDrawWaitForSignal(void);
s32 QuickDrawCountPresses(void);

/* src/subgame_ba774.c */
u8 QuickDrawIsTimeUp(void);
void sub_080ba78c(void);
void sub_080ba7b0(void);
void sub_080ba7fc(u8 a0);
void sub_080ba860(void);
void sub_080ba8cc(void);
void sub_080ba900(void);
void CreateQuickDrawSlash(void);
void CreateQuickDrawBurst(void);
void QuickDrawUpdateRanking(s32 a0);
void QuickDrawAwardRound(void);
void sub_080baabc(void);
void QuickDrawRound(void);
void QuickDrawRoundUpdate(void);
void QuickDrawFalseStart(s32 a0);
u8 QuickDrawFindMatchWinner(void);
void QuickDrawDecideRound(s32 a0);
void QuickDrawEnterState(void);
void QuickDrawRoundWait(void);
void QuickDrawRoundWaitUpdate(void);
void QuickDrawRoundSignal(void);
void QuickDrawRoundSignalUpdate(void);
void QuickDrawRoundTimeUp(void);
void QuickDrawRoundTimeUpUpdate(void);
void QuickDrawRoundAllFalseStart(void);
void QuickDrawRoundAllFalseStartUpdate(void);
void QuickDrawRoundWin(void);
void QuickDrawRoundWinUpdate(void);
void QuickDrawRoundTie(void);
void QuickDrawRoundTieUpdate(void);
void QuickDrawRoundNext(void);
void QuickDrawRoundNextUpdate(void);
void sub_080bafc8(s32 a0);
void CreateQuickDrawOpponent(void);
void sub_080bb0d8(s32 a0);
void sub_080bb120(void);
void sub_080bb174(void);
void sub_080bb19c(void);
void sub_080bb1c4(void);
u8 QuickDrawFindMatchWinnerVsCpu(void);
void QuickDrawEnterStateVsCpu(void);
void sub_080bb258(void);
void sub_080bb29c(void);
void sub_080bb2d8(void);
void sub_080bb2f4(void);
void sub_080bb328(void);
void sub_080bb358(void);
void sub_080bb380(void);
void sub_080bb3a8(void);
void sub_080bb3e8(void);
void sub_080bb410(void);
void sub_080bb42c(void);
void sub_080bb454(void);
void sub_080bb47c(void);
void sub_080bb4fc(void);

/* src/subgame_bb528.c */
void sub_080bb528(void);
void sub_080bb554(void);
void sub_080bb59c(s32 a0);
void sub_080bb5b8(void);
void sub_080bb63c(void);
void sub_080bb66c(void);
void sub_080bb718(u16 a0, u16 a1, u16 a2);
void sub_080bb760(void);
void sub_080bb7a0(void);
void sub_080bb7cc(u8 a0, s16 a1, s16 a2, s8 a3);
void sub_080bb820(u8 a0, s16 a1, s16 a2, s8 a3);
void sub_080bb874(u8 a0, s8 a1);
void sub_080bb8f8(void);
void sub_080bb930(u8 a0, s8 a1);
void sub_080bb9b4(void);
void sub_080bb9cc(void);
void sub_080bb9f0(void);
void sub_080bba1c(void);
void sub_080bbad4(void);
void sub_080bbb70(void);
void sub_080bbbb8(void);
void sub_080bbc04(void);
void sub_080bbc70(void);
void sub_080bbcdc(void);
void sub_080bbd4c(void);
void QuickDrawResults(void);
void QuickDrawResultsUpdate(void);
void QuickDrawResultsEnterState(void);
void sub_080bbe20(void);
void sub_080bbe58(void);
void sub_080bbe80(void);
void sub_080bbedc(void);
void sub_080bbf04(void);
void sub_080bbf44(void);
void sub_080bbf6c(void);
void sub_080bbf9c(void);
void sub_080bbfc4(void);
void sub_080bc008(void);
void sub_080bc03c(void);
void sub_080bc06c(void);
void sub_080bc0a0(void);
void sub_080bc0b8(void);

/* src/subgame_bc0cc.c */
void Task_QuickDrawObject(void);
void sub_080bc0ec(void);
void sub_080bc168(void);
void sub_080bc1c4(void);
void sub_080bc30c(void);
void sub_080bc3c8(void);
void sub_080bc460(void);
void sub_080bc4b0(void);
void sub_080bc54c(void);
void sub_080bc5cc(void);
void sub_080bc680(void);
void sub_080bc70c(void);
void QuickDrawSetPlayerState(s32 a0, u16 a1);
void sub_080bc79c(u16 a0);
u8 sub_080bc7c8(void);
void sub_080bc800(void);
void QuickDrawPlayer(void);
void QuickDrawPlayerUpdate(void);
void QuickDrawPlayerEnterState(void);
void sub_080bc8e0(void);
void sub_080bc948(void);
void sub_080bc988(void);
void sub_080bc9c0(void);
void sub_080bc9c4(void);
void sub_080bca68(void);
void sub_080bca6c(void);
void sub_080bcaac(void);
void sub_080bcadc(void);
void sub_080bcbbc(void);
void sub_080bcbc0(void);
void sub_080bcbf8(void);
void sub_080bcbfc(void);
void sub_080bccbc(void);
void sub_080bcdac(void);
void sub_080bcde0(void);
void QuickDrawTimerCount(void);
void sub_080bce74(void);
void QuickDrawTimer(void);
void sub_080bcf8c(void);
void QuickDrawSlash(void);
void QuickDrawBurst(void);
void sub_080bd110(void);
void QuickDrawLoadOpponentGraphics(s32 a0);
void QuickDrawSetOpponentState(s32 a0, u16 a1);
void sub_080bd210(void);
void sub_080bd25c(void);
void sub_080bd290(void);
void sub_080bd370(void);
void sub_080bd3dc(u8 a0);
void sub_080bd494(void);
void QuickDrawOpponent(void);
void QuickDrawOpponentUpdate(void);
void QuickDrawOpponentEnterState(void);
void sub_080bd544(void);
void sub_080bd594(void);
void sub_080bd5d4(void);
void sub_080bd60c(void);
void sub_080bd610(void);
void sub_080bd62c(void);
void sub_080bd630(void);
void sub_080bd664(void);
void sub_080bd694(void);
void sub_080bd6b0(void);
void sub_080bd6b4(void);
void sub_080bd7ec(void);
void sub_080bd7f0(void);
void sub_080bd828(void);
void sub_080bd8ac(void);
void sub_080bd938(void);
void sub_080bd9b0(void);

/* src/subgame_bd9e8.c */
void BombRallyInit(void);
void BombRallyMain(void);

/* src/subgame_bda2c.c */
void BombRallyRound(void);
void BombRallyRoundUpdate(void);
void BombRallyEnterState(void);
void BombRallyRoundPass(void);
void BombRallyRoundPassUpdate(void);
void BombRallyRoundNext(void);
void BombRallyRoundNextUpdate(void);
u32 BombRallyKnockOutTurnPlayer(void);
u32 BombRallyIsMatchOver(void);
void BombRallyInitSpeed(void);
void BombRallyRestartSpeed(void);
void CreateBombRallyBomb(u32 a);
void CreateBombRallyStarBurst(s32 a, s32 b, u32 c, u32 d);
void CreateBombRallyBurstStar(u32 a);
void CreateBombRallyStartSign(void);
void BombRallySeatPlayers(void);
void BombRallyResults(void);
void BombRallyResultsUpdate(void);
void BombRallyResultsEnterState(void);
void BombRallyResultsShow(void);
void BombRallyResultsShowUpdate(void);
void BombRallyResultsMenu(void);
void BombRallyResultsMenuUpdate(void);
void sub_080be4a4(void);
void sub_080be550(void);
void sub_080be5fc(void);
void CreateBombRallyContinueItems(u32 a);
void CreateBombRallyLevelItems(u32 a);
void BombRallyAwardLives(void);
void sub_080be7c0(u32 a);
void Task_BombRallyObject(void);
void BombRallyPlayer(void);
void BombRallyPlayerUpdate(void);
void BombRallyPlayerEnterState(void);
void BombRallyPlayerServe(void);
void BombRallyPlayerServeUpdate(void);
void BombRallyPlayerReady(void);
void BombRallyPlayerReadyUpdate(void);
void BombRallyPlayerTurn(void);
void BombRallyPlayerTurnUpdate(void);
void BombRallyPlayerThrow(void);
void BombRallyPlayerThrowUpdate(void);
void BombRallyPlayerFollowThrough(void);
void BombRallyPlayerFollowThroughUpdate(void);
u32 BombRallyPlayerJudgePress(void);
void BombRallyPlayerUpdatePose(void);
void BombRallyPlayerCpuReady(void);
void BombRallyPlayerCpuReadyUpdate(void);
void BombRallyPlayerCpuTurn(void);
void BombRallyPlayerCpuTurnUpdate(void);
void BombRallyPlayerCpuThrow(void);
void BombRallyPlayerCpuThrowUpdate(void);
void BombRallyPlayerCpuFollowThrough(void);
void BombRallyPlayerCpuFollowThroughUpdate(void);
void BombRallyPlayerBlownUp(void);
void BombRallyPlayerBlownUpUpdate(void);

/* src/subgame_bf994.c */
void BombRallyPlayerBubblesServe(void);
void BombRallyPlayerBubblesServeUpdate(void);
void BombRallyPlayerBubblesWait(void);
void BombRallyPlayerBubblesWaitUpdate(void);
void BombRallyPlayerBubblesThrow(void);
void BombRallyPlayerBubblesThrowUpdate(void);
void BombRallyBomb(void);
void BombRallyBombUpdate(void);
void BombRallyBombEnterState(void);
void BombRallyBombStart(void);
void BombRallyBombStartUpdate(void);
void BombRallyBombPass(void);
void BombRallyBombPassUpdate(void);
void BombRallyBombExplode(void);
void BombRallyBombExplodeUpdate(void);
void sub_080c05f0(u32 a);
void sub_080c061c(s32 a, s32 b, s32 c, s32 d);
void sub_080c0704(u32 a);
void sub_080c072c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080c0a10(s32 a, s32 b, s32 c, s32 d, s16 e);
void sub_080c0b18(u32 a);
void BombRallyBombSmoke(void);
void BombRallyBombSmokeUpdate(void);
void BombRallyStarBurst(void);

/* src/subgame_c0de8.c */
void sub_080c0de8(void);
void sub_080c0e88(void);
void sub_080c0f54(void);
void sub_080c0fe4(void);
void sub_080c1068(void);
void sub_080c1114(void);
void sub_080c11cc(void);
void sub_080c1260(void);
void sub_080c1300(void);
void sub_080c1390(void);
void sub_080c1424(void);
void sub_080c14b8(void);
void sub_080c1558(void);
void sub_080c1608(void);
void sub_080c168c(void);
void BombRallyStartSign(void);
void sub_080c17ac(void);
void BombRallyResultsPlayer(void);
void sub_080c1804(void);
void sub_080c1820(void);
void sub_080c183c(void);
void sub_080c18c4(void);
void sub_080c18c8(void);
void sub_080c1950(void);
void sub_080c1ab8(void);
void sub_080c1b2c(void);
void BombRallyMenuItem(void);
void sub_080c1b78(void);
void sub_080c1b94(void);
void sub_080c1be8(void);
void sub_080c1cec(void);
void sub_080c1d84(void);
s32 sub_080c1ebc(s32 a, s32 b);
void sub_080c1f88(void);
void AirGrindInit(void);
void AirGrindMain(void);

/* src/subgame_c1ffc.c */
void CreateAirGrindRacers(void);
void CreateAirGrindScenery(s32 unused);
void CreateAirGrindEffect(s32 a, s32 b, s32 c);
void AirGrindSetupRace(void);
void AirGrindRace(void);
void AirGrindRaceUpdate(void);

/* src/subgame_c243c.c */
void AirGrindResults(void);
void sub_080c25c4(void);
void sub_080c2740(void);
void sub_080c2b8c(void);
void sub_080c2ba8(void);
void sub_080c2ccc(s32 mode);
void AirGrindBuildSky(void);
void AirGrindSkyVBlankCallback(void);

/* src/subgame_c2ff8.c */
void Task_AirGrindObject(void);
void AirGrindRacer(void);
void AirGrindCpuRollTarget(s32 player);
s32 AirGrindCpuHoldsA(s32 player, s32 pos);
void AirGrindRacerUpdate(void);

/* src/subgame_c3648.c */
void sub_080c3648(void);
void sub_080c3670(s32 pos);
s32 sub_080c3698(s32 player);
void AirGrindUpdateEngineSound(s32 a, s32 on);
void sub_080c383c(s32 player);
void AirGrindRacerRaceStep(s32 player);
void AirGrindRacerIdleStep(s32 player);
void sub_080c3e18(s32 player);
void AirGrindRacerRaceUpdate(void);
void AirGrindRacerIdleUpdate(void);

/* src/subgame_c3f44.c */
void AirGrindEffect(void);
void AirGrindEffectFollowRacer(s32 layer);
void sub_080c4364(void);
void sub_080c43e8(void);
void sub_080c44bc(void);
void sub_080c44f0(void);
void sub_080c4568(void);
void sub_080c45d4(void);
void sub_080c45fc(void);

/* src/subgame_c4630.c */
void sub_080c4630(s32 idx, s32 x, s32 y, u16 attr);
void sub_080c4664(s32 i);
void AirGrindScenery(void);
void sub_080c4790(void);
void AirGrindCourseSignUpdate(void);
void AirGrindShowCourseSign(s32 y);
void AirGrindStepPaletteFades(void);
void AirGrindStopAllPaletteFades(void);
s32 AirGrindStartPaletteFade(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat);
void AirGrindStopPaletteFade(s32 i);
void AirGrindSetDigitPalette(s32 pal);
void AirGrindDrawDigit(s32 digit, s32 x, s32 y);
void sub_080c4a94(s32 idx, s32 x, s32 y);
void AirGrindDrawTime(s32 t, s32 x, s32 y);
void AirGrindDrawNumber(s32 n, s32 x, s32 y);
void sub_080c4bec(s32 a, s32 b, s32 x, s32 y);
void AirGrindDrawRacerSprite(s32 idx, s32 pal, s32 scale, s32 x, s32 y, u32 layer);
void AirGrindSeedRandom(void);
u32 AirGrindRandom(s32 i);
u32 AirGrindRandomRange(s32 i, u32 range);

/* src/subgame_c4d08.c */
void AirGrindRacerDraw(void);
void AirGrindEffectDrawOrFree(void);
void AirGrindRacerMove(void);
u32 AirGrindScaleSprite(u16 *src, s16 scale);
void AirGrindClearScript(void);
void AirGrindStepScript(void);
void AirGrindStartScript(u16 id);

/* src/subgame_c5284.c */
s32 sub_080c5284(s32 angle);
void sub_080c52c4(s32 lane, s32 x, s32 *px, s32 *py, s32 *pz);
void sub_080c54a4(s32 lane, s32 *idx, s32 x, s32 *out);
s32 sub_080c553c(s32 i, s32 x);
void sub_080c5580(s32 start, s32 end, s32 *set, s32 *clear);
s32 sub_080c55d8(s32 lane, s32 x);
s32 sub_080c5628(s32 lane, s32 x);
void AirGrindLayOutCourse(s32 a);
void AirGrindBuildCourse(s32 a, s32 b);
void AirGrindDrawCourse(void);

/* src/subgame_c623c.c */
s32 sub_080c623c(s32 x);

/* src/sub_080c6258.c */
s32 sub_080c6258(s32 value);

#endif /* GUARD_SUBGAME_H */
