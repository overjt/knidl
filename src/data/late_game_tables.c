#include "global.h"
#include "main.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "menu.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"
#include "subgame.h"
#include "ending.h"

/* The behaviour tables of late_game_rodata (0x0875607C-0x08758447, the rodata of
 * M33-M38; issue #167): 54 state/handler tables, 265 function pointers, in
 * 23 runs of adjacent tables, each run in address order.  Every table is
 * defined with the element type its declaration gives (the line above it
 * names the declaring header and the consumer): most are u32 [] because their
 * consumers pass them to CallTableEntry(index, count, table) (src/early_2b04.c,
 * `if (index < count) table[index]();`), whose count is the table's length
 * unless the line says otherwise.  Every word is a function by its name
 * (the function-entry rule of docs/data.md section 5) or 0/NULL, exactly
 * the words the data file had, so the link and the shift test see the same
 * relocations.  Tables whose span holds other values, or whose element type
 * no C declares, stay structure-only data between the runs.
 *
 * Each run is a named section .late_tbl_<address>: linker.ld lists the runs
 * between the data pieces of late_game_rodata inside ONE output section
 * (tools/ldgroup.py, docs/data.md 5.2).  The tables are const only where
 * their declaration already is: the qualifier would reach CallTableEntry's
 * table parameter (u32 * / void (**)(void)) under -Werror, and a named
 * section holds objects of one kind, so a const table gets a run of its
 * own.  The section attribute is what places them in ROM.  A function whose
 * prototype differs from the declared element type is cast to it, as its
 * consumers call it. */

#define LATE_TBL(addr) __attribute__((section(".late_tbl_" #addr)))

/* ---- 0x0875607C-0x08756084: 1 table(s), 2 function pointer(s), section .late_tbl_0875607c ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_OneUp, Task_MaximTomato, Task_InvincibleCandy, Task_EnergyDrink */
u32 gPickupVariants[2] LATE_TBL(0875607c) = {
    (u32)PickupInit,
    (u32)PickupInit,
};

/* ---- 0x087560A0-0x087560C0: 3 table(s), 8 function pointer(s), section .late_tbl_087560a0 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in PickupInit, PickupEnterState */
u32 gPickupStates[3] LATE_TBL(087560a0) = {
    (u32)sub_080b4174,
    (u32)PickupFall,
    (u32)PickupFallInWater,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in PickupUpdate */
u32 gPickupStateUpdates[3] LATE_TBL(087560a0) = {
    (u32)sub_080b4190,
    (u32)PickupFallUpdate,
    (u32)PickupFallInWaterUpdate,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in PickupHeal */
u32 gUnk_087560B8[2] LATE_TBL(087560a0) = {
    (u32)MaximTomatoHeal,
    (u32)EnergyDrinkHeal,
};

/* ---- 0x087560D0-0x087560EC: 3 table(s), 7 function pointer(s), section .late_tbl_087560d0 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_AbilityStar, AbilityStarEnterState */
u32 gAbilityStarStates[2] LATE_TBL(087560d0) = {
    (u32)sub_080b4770,
    (u32)AbilityStarSink,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in AbilityStarUpdate */
u32 gAbilityStarStateUpdates[2] LATE_TBL(087560d0) = {
    (u32)sub_080b4788,
    (u32)AbilityStarSinkUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_StarRodPiece */
u32 gStarRodPieceVariants[3] LATE_TBL(087560d0) = {
    (u32)StarRodPieceHoverInit,
    (u32)StarRodPieceSlideOutInit,
    (u32)sub_080b4e04,
};

/* ---- 0x08756150-0x08756178: 4 table(s), 10 function pointer(s), section .late_tbl_08756150 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in StarRodPieceHoverInit, StarRodPieceHoverEnterState */
u32 gStarRodPieceHoverStates[2] LATE_TBL(08756150) = {
    (u32)sub_080b4b18,
    (u32)sub_080b4bb0,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in StarRodPieceHoverUpdate */
u32 gStarRodPieceHoverStateUpdates[2] LATE_TBL(08756150) = {
    (u32)sub_080b4b94,
    (u32)sub_080b4bd8,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in StarRodPieceSlideOutInit, StarRodPieceSlideOutEnterState */
u32 gStarRodPieceSlideOutStates[3] LATE_TBL(08756150) = {
    (u32)sub_080b4ca0,
    (u32)sub_080b4d50,
    (u32)sub_080b4dd0,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in StarRodPieceSlideOutUpdate */
u32 gStarRodPieceSlideOutStateUpdates[3] LATE_TBL(08756150) = {
    (u32)sub_080b4d1c,
    (u32)sub_080b4db4,
    (u32)sub_080b4df8,
};

/* ---- 0x08756198-0x087561C4: 1 table(s), 11 function pointer(s), section .late_tbl_08756198 ---- */
/* include/enemy.h; read by UpdateHBlankScroll, UpdateRoomHBlankScroll */
s16 (*gHBlankScrollEffects[11])(void) LATE_TBL(08756198) = {
    (s16 (*)(void))sub_080b6154,
    (s16 (*)(void))sub_080b6290,
    (s16 (*)(void))sub_080b63a4,
    (s16 (*)(void))sub_080b6474,
    (s16 (*)(void))sub_080b6570,
    (s16 (*)(void))sub_080b6570,
    (s16 (*)(void))sub_080b67dc,
    (s16 (*)(void))sub_080b67dc,
    (s16 (*)(void))sub_080b6b08,
    (s16 (*)(void))sub_080b6c40,
    (s16 (*)(void))sub_080b6d04,
};

/* ---- 0x08756270-0x087562A8: 2 table(s), 14 function pointer(s), section .late_tbl_08756270 ---- */
/* include/save.h; CallTableEntry(i, 7, ...) in sub_080b9610, sub_080b9658 */
u32 gUnk_08756270[7] LATE_TBL(08756270) = {
    (u32)sub_080b9674,
    (u32)sub_080b96a0,
    (u32)sub_080b9710,
    (u32)sub_080b9740,
    (u32)sub_080b9770,
    (u32)sub_080b97a4,
    (u32)sub_080b97dc,
};
/* include/save.h; CallTableEntry(i, 7, ...) in sub_080b963c */
u32 gUnk_0875628C[7] LATE_TBL(08756270) = {
    (u32)sub_080b9690,
    (u32)sub_080b96bc,
    (u32)sub_080b9730,
    (u32)sub_080b9764,
    (u32)sub_080b9798,
    (u32)sub_080b97d0,
    (u32)sub_080b97f8,
};

/* ---- 0x087562CC-0x087562D8: 1 table(s), 3 function pointer(s), section .late_tbl_087562cc ---- */
/* include/subgame.h; read by SubGameInit */
s32 (*const gSubGameInitHooks[3])(void) LATE_TBL(087562cc) = {
    (s32 (*)(void))QuickDrawInit,
    (s32 (*)(void))BombRallyInit,
    (s32 (*)(void))AirGrindInit,
};

/* ---- 0x087562D8-0x087562E4: 1 table(s), 3 function pointer(s), section .late_tbl_087562d8 ---- */
/* include/subgame.h; read by SubGameStartBody */
void *gSubGameBodies[3] LATE_TBL(087562d8) = {
    (void *)QuickDrawMain,
    (void *)BombRallyMain,
    (void *)AirGrindMain,
};

/* ---- 0x087562E8-0x087562F0: 1 table(s), 2 function pointer(s), section .late_tbl_087562e8 ---- */
/* include/subgame.h; CallTableEntry(i, 2, ...) in QuickDrawMain */
u32 gQuickDrawPhases[2] LATE_TBL(087562e8) = {
    (u32)QuickDrawRound,
    (u32)QuickDrawResults,
};

/* ---- 0x087562FC-0x0875636C: 4 table(s), 28 function pointer(s), section .late_tbl_087562fc ---- */
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawRound, QuickDrawEnterState */
u32 gQuickDrawStates[7] LATE_TBL(087562fc) = {
    (u32)QuickDrawRoundWait,
    (u32)QuickDrawRoundSignal,
    (u32)QuickDrawRoundTimeUp,
    (u32)QuickDrawRoundAllFalseStart,
    (u32)QuickDrawRoundWin,
    (u32)QuickDrawRoundTie,
    (u32)QuickDrawRoundNext,
};
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawRoundUpdate */
u32 gQuickDrawStateUpdates[7] LATE_TBL(087562fc) = {
    (u32)QuickDrawRoundWaitUpdate,
    (u32)QuickDrawRoundSignalUpdate,
    (u32)QuickDrawRoundTimeUpUpdate,
    (u32)QuickDrawRoundAllFalseStartUpdate,
    (u32)QuickDrawRoundWinUpdate,
    (u32)QuickDrawRoundTieUpdate,
    (u32)QuickDrawRoundNextUpdate,
};
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawRound, QuickDrawEnterStateVsCpu */
u32 gQuickDrawStatesVsCpu[7] LATE_TBL(087562fc) = {
    (u32)QuickDrawRoundWaitVsCpu,
    (u32)QuickDrawRoundSignalVsCpu,
    (u32)QuickDrawRoundFalseStartVsCpu,
    (u32)QuickDrawRoundWinVsCpu,
    (u32)QuickDrawRoundLoseVsCpu,
    (u32)QuickDrawRoundTieVsCpu,
    (u32)QuickDrawRoundNextVsCpu,
};
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawRoundUpdate */
u32 gQuickDrawStateUpdatesVsCpu[7] LATE_TBL(087562fc) = {
    (u32)QuickDrawRoundWaitVsCpuUpdate,
    (u32)QuickDrawRoundSignalVsCpuUpdate,
    (u32)QuickDrawRoundFalseStartVsCpuUpdate,
    (u32)QuickDrawRoundWinVsCpuUpdate,
    (u32)QuickDrawRoundLoseVsCpuUpdate,
    (u32)QuickDrawRoundTieVsCpuUpdate,
    (u32)QuickDrawRoundNextVsCpuUpdate,
};

/* ---- 0x08756378-0x087563D8: 3 table(s), 24 function pointer(s), section .late_tbl_08756378 ---- */
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawResults, QuickDrawResultsEnterState */
u32 gQuickDrawResultsStates[7] LATE_TBL(08756378) = {
    (u32)QuickDrawResultsPlaySong,
    (u32)QuickDrawResultsBonusSign,
    (u32)QuickDrawResultsAwardBonuses,
    (u32)QuickDrawResultsRanking,
    (u32)QuickDrawResultsContinueMenu,
    (u32)QuickDrawResultsLevelMenu,
    (u32)QuickDrawResultsWaitQuit,
};
/* include/subgame.h; CallTableEntry(i, 7, ...) in QuickDrawResultsUpdate */
u32 gQuickDrawResultsStateUpdates[7] LATE_TBL(08756378) = {
    (u32)QuickDrawResultsPlaySongUpdate,
    (u32)QuickDrawResultsBonusSignUpdate,
    (u32)QuickDrawResultsAwardBonusesUpdate,
    (u32)QuickDrawResultsRankingUpdate,
    (u32)QuickDrawResultsContinueMenuUpdate,
    (u32)QuickDrawResultsLevelMenuUpdate,
    (u32)QuickDrawResultsWaitQuitUpdate,
};
/* include/subgame.h; CallTableEntry(i, 12, ...) in Task_QuickDrawObject: the bound exceeds the 10 entries, so indices 10-11 would read the next label, gQuickDrawSeatX */
u32 gQuickDrawObjectKinds[10] LATE_TBL(08756378) = {
    (u32)QuickDrawPlayer,
    (u32)QuickDrawLabel,
    (u32)QuickDrawTimer,
    (u32)QuickDrawSlash,
    (u32)QuickDrawBurst,
    (u32)QuickDrawOpponent,
    (u32)QuickDrawSweatDrop,
    (u32)QuickDrawBonusSign,
    (u32)QuickDrawBonus,
    (u32)QuickDrawRankLabel,
};

/* ---- 0x08756468-0x08756498: 2 table(s), 12 function pointer(s), section .late_tbl_08756468 ---- */
/* include/subgame.h; CallTableEntry(i, 6, ...) in QuickDrawPlayer, QuickDrawPlayerEnterState */
u32 gQuickDrawPlayerStates[6] LATE_TBL(08756468) = {
    (u32)QuickDrawPlayerArrive,
    (u32)QuickDrawPlayerReady,
    (u32)QuickDrawPlayerStrike,
    (u32)QuickDrawPlayerLose,
    (u32)QuickDrawPlayerFalseStart,
    (u32)QuickDrawPlayerResults,
};
/* include/subgame.h; CallTableEntry(i, 6, ...) in QuickDrawPlayerUpdate */
u32 gQuickDrawPlayerStateUpdates[6] LATE_TBL(08756468) = {
    (u32)QuickDrawPlayerArriveUpdate,
    (u32)QuickDrawPlayerReadyUpdate,
    (u32)QuickDrawPlayerStrikeUpdate,
    (u32)QuickDrawPlayerLoseUpdate,
    (u32)QuickDrawPlayerFalseStartUpdate,
    (u32)QuickDrawPlayerResultsUpdate,
};

/* ---- 0x087564E4-0x08756514: 2 table(s), 12 function pointer(s), section .late_tbl_087564e4 ---- */
/* include/subgame.h; CallTableEntry(i, 6, ...) in QuickDrawOpponent, QuickDrawOpponentEnterState */
u32 gQuickDrawOpponentStates[6] LATE_TBL(087564e4) = {
    (u32)QuickDrawOpponentArrive,
    (u32)QuickDrawOpponentReady,
    (u32)QuickDrawOpponentStrike,
    (u32)QuickDrawOpponentLose,
    (u32)QuickDrawOpponentTie,
    (u32)QuickDrawOpponentTag,
};
/* include/subgame.h; CallTableEntry(i, 6, ...) in QuickDrawOpponentUpdate */
u32 gQuickDrawOpponentStateUpdates[6] LATE_TBL(087564e4) = {
    (u32)QuickDrawOpponentArriveUpdate,
    (u32)QuickDrawOpponentReadyUpdate,
    (u32)QuickDrawOpponentStrikeUpdate,
    (u32)QuickDrawOpponentLoseUpdate,
    (u32)QuickDrawOpponentTieUpdate,
    (u32)QuickDrawOpponentTagUpdate,
};

/* ---- 0x08756568-0x08756570: 1 table(s), 2 function pointer(s), section .late_tbl_08756568 ---- */
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyMain */
u32 gBombRallyPhases[2] LATE_TBL(08756568) = {
    (u32)BombRallyRound,
    (u32)BombRallyResults,
};

/* ---- 0x08756668-0x0875670C: 7 table(s), 41 function pointer(s), section .late_tbl_08756668 ---- */
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyRound, BombRallyEnterState */
u32 gBombRallyStates[2] LATE_TBL(08756668) = {
    (u32)BombRallyRoundPass,
    (u32)BombRallyRoundNext,
};
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyRoundUpdate */
u32 gBombRallyStateUpdates[2] LATE_TBL(08756668) = {
    (u32)BombRallyRoundPassUpdate,
    (u32)BombRallyRoundNextUpdate,
};
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyResults, BombRallyResultsEnterState */
u32 gBombRallyResultsStates[2] LATE_TBL(08756668) = {
    (u32)BombRallyResultsShow,
    (u32)BombRallyResultsMenu,
};
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyResultsUpdate */
u32 gBombRallyResultsStateUpdates[2] LATE_TBL(08756668) = {
    (u32)BombRallyResultsShowUpdate,
    (u32)BombRallyResultsMenuUpdate,
};
/* include/subgame.h; CallTableEntry(i, 7, ...) in Task_BombRallyObject */
u32 gBombRallyObjectVariants[7] LATE_TBL(08756668) = {
    (u32)BombRallyPlayer,
    (u32)BombRallyBomb,
    (u32)BombRallyBombSmoke,
    (u32)BombRallyStarBurst,
    (u32)BombRallyStartSign,
    (u32)BombRallyResultsPlayer,
    (u32)BombRallyMenuItem,
};
/* include/subgame.h; CallTableEntry(i, 13, ...) in BombRallyPlayer, BombRallyPlayerEnterState */
u32 gBombRallyPlayerStates[13] LATE_TBL(08756668) = {
    (u32)BombRallyPlayerServe,
    (u32)BombRallyPlayerReady,
    (u32)BombRallyPlayerTurn,
    (u32)BombRallyPlayerThrow,
    (u32)BombRallyPlayerFollowThrough,
    (u32)BombRallyPlayerCpuReady,
    (u32)BombRallyPlayerCpuTurn,
    (u32)BombRallyPlayerCpuThrow,
    (u32)BombRallyPlayerCpuFollowThrough,
    (u32)BombRallyPlayerBlownUp,
    (u32)BombRallyPlayerBubblesServe,
    (u32)BombRallyPlayerBubblesWait,
    (u32)BombRallyPlayerBubblesThrow,
};
/* include/subgame.h; CallTableEntry(i, 13, ...) in BombRallyPlayerUpdate */
u32 gBombRallyPlayerStateUpdates[13] LATE_TBL(08756668) = {
    (u32)BombRallyPlayerServeUpdate,
    (u32)BombRallyPlayerReadyUpdate,
    (u32)BombRallyPlayerTurnUpdate,
    (u32)BombRallyPlayerThrowUpdate,
    (u32)BombRallyPlayerFollowThroughUpdate,
    (u32)BombRallyPlayerCpuReadyUpdate,
    (u32)BombRallyPlayerCpuTurnUpdate,
    (u32)BombRallyPlayerCpuThrowUpdate,
    (u32)BombRallyPlayerCpuFollowThroughUpdate,
    (u32)BombRallyPlayerBlownUpUpdate,
    (u32)BombRallyPlayerBubblesServeUpdate,
    (u32)BombRallyPlayerBubblesWaitUpdate,
    (u32)BombRallyPlayerBubblesThrowUpdate,
};

/* ---- 0x08756780-0x08756798: 2 table(s), 6 function pointer(s), section .late_tbl_08756780 ---- */
/* include/subgame.h; CallTableEntry(i, 3, ...) in BombRallyBomb, BombRallyBombEnterState */
u32 gBombRallyBombStates[3] LATE_TBL(08756780) = {
    (u32)BombRallyBombStart,
    (u32)BombRallyBombPass,
    (u32)BombRallyBombExplode,
};
/* include/subgame.h; CallTableEntry(i, 3, ...) in BombRallyBombUpdate */
u32 gBombRallyBombStateUpdates[3] LATE_TBL(08756780) = {
    (u32)BombRallyBombStartUpdate,
    (u32)BombRallyBombPassUpdate,
    (u32)BombRallyBombExplodeUpdate,
};

/* ---- 0x087571F8-0x08757250: 3 table(s), 22 function pointer(s), section .late_tbl_087571f8 ---- */
/* include/subgame.h; CallTableEntry(i, 16, ...) in BombRallyStarBurst */
u32 gBombRallyStarBurstStates[16] LATE_TBL(087571f8) = {
    (u32)sub_080c0de8,
    (u32)sub_080c1114,
    (u32)sub_080c1424,
    (u32)sub_080c1114,
    (u32)sub_080c0e88,
    (u32)sub_080c0f54,
    (u32)sub_080c0fe4,
    (u32)sub_080c1068,
    (u32)sub_080c11cc,
    (u32)sub_080c1260,
    (u32)sub_080c1300,
    (u32)sub_080c1390,
    (u32)sub_080c14b8,
    (u32)sub_080c1558,
    (u32)sub_080c1608,
    (u32)sub_080c168c,
};
/* include/subgame.h; CallTableEntry(i, 3, ...) in BombRallyResultsPlayer, BombRallyResultsPlayerEnterState */
u32 gBombRallyResultsPlayerStates[3] LATE_TBL(087571f8) = {
    (u32)BombRallyResultsPlayerPose,
    (u32)BombRallyResultsPlayerPlace,
    (u32)BombRallyResultsPlayerLives,
};
/* include/subgame.h; CallTableEntry(i, 3, ...) in BombRallyResultsPlayerUpdate */
u32 gBombRallyResultsPlayerStateUpdates[3] LATE_TBL(087571f8) = {
    (u32)BombRallyResultsPlayerPoseUpdate,
    (u32)BombRallyResultsPlayerPlaceUpdate,
    (u32)BombRallyResultsPlayerLivesUpdate,
};

/* ---- 0x08757270-0x08757280: 2 table(s), 4 function pointer(s), section .late_tbl_08757270 ---- */
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyMenuItem */
u32 gBombRallyMenuItemStates[2] LATE_TBL(08757270) = {
    (u32)BombRallyMenuItemContinue,
    (u32)BombRallyMenuItemLevel,
};
/* include/subgame.h; CallTableEntry(i, 2, ...) in BombRallyMenuItemUpdate */
u32 gBombRallyMenuItemStateUpdates[2] LATE_TBL(08757270) = {
    (u32)BombRallyMenuItemContinueUpdate,
    (u32)BombRallyMenuItemLevelUpdate,
};

/* ---- 0x087572CC-0x087572E0: 2 table(s), 5 function pointer(s), section .late_tbl_087572cc ---- */
/* include/subgame.h; CallTableEntry(i, 2, ...) in AirGrindMain */
u32 gAirGrindPhases[2] LATE_TBL(087572cc) = {
    (u32)AirGrindRace,
    (u32)AirGrindResults,
};
/* include/subgame.h; CallTableEntry(i, 5, ...) in Task_AirGrindObject: the bound exceeds the 3 entries, so indices 3-4 would read the next label, gUnk_087572E0 */
void (*gAirGrindObjectVariants[3])(void) LATE_TBL(087572cc) = {
    AirGrindRacer,
    AirGrindScenery,
    AirGrindEffect,
};

/* ---- 0x08757330-0x0875735C: 1 table(s), 10 function pointer(s), section .late_tbl_08757330 ---- */
/* include/ending.h; CallTableEntry(i, 11, ...) in Task_EndingEpilogue */
void (*gEndingEpilogueVariants[11])(void) LATE_TBL(08757330) = {
    NULL,
    sub_080c6d84,
    sub_080c7810,
    sub_080c7cc0,
    sub_080c7e4c,
    sub_080c8498,
    sub_080c85d8,
    sub_080c88f0,
    sub_080c8958,
    sub_080c8cd4,
    sub_080c8ea8,
};

/* ---- 0x087573F4-0x08757424: 1 table(s), 11 function pointer(s), section .late_tbl_087573f4 ---- */
/* include/ending.h; CallTableEntry(i, 12, ...) in Task_EndingStarRodReturn */
void (*gEndingStarRodReturnVariants[12])(void) LATE_TBL(087573f4) = {
    NULL,
    sub_080c9114,
    sub_080c9d10,
    sub_080c94cc,
    sub_080c9884,
    sub_080c98d8,
    sub_080ca71c,
    sub_080ca830,
    sub_080ca8f0,
    sub_080c9e8c,
    sub_080ca344,
    sub_080c9a28,
};

/* ---- 0x08758294-0x087582F4: 5 table(s), 24 function pointer(s), section .late_tbl_08758294 ---- */
/* include/ending.h; CallTableEntry(i, 6, ...) in Task_GameOverObject */
void (*gGameOverObjectVariants[6])(void) LATE_TBL(08758294) = {
    GameOverPlayer,
    GameOverChoice,
    sub_080ccd4c,
    sub_080ccec8,
    sub_080cd24c,
    sub_080cd2f8,
};
/* include/ending.h; CallTableEntry(i, 3, ...) in GameOverPlayer, GameOverPlayerEnterState */
void (*gGameOverPlayerStates[3])(void) LATE_TBL(08758294) = {
    GameOverPlayerWait,
    GameOverPlayerContinue,
    GameOverPlayerGiveUp,
};
/* include/ending.h; CallTableEntry(i, 3, ...) in GameOverPlayerUpdate */
void (*gGameOverPlayerStateUpdates[3])(void) LATE_TBL(08758294) = {
    GameOverPlayerWaitUpdate,
    GameOverPlayerContinueUpdate,
    GameOverPlayerGiveUpUpdate,
};
/* include/ending.h; CallTableEntry(i, 6, ...) in GameOverChoice, GameOverChoiceEnterState */
void (*gGameOverChoiceStates[6])(void) LATE_TBL(08758294) = {
    GameOverChoiceWait,
    GameOverChoiceMove,
    sub_080cc180,
    sub_080cc2e0,
    sub_080cc608,
    sub_080cc768,
};
/* include/ending.h; CallTableEntry(i, 6, ...) in GameOverChoiceUpdate */
void (*gGameOverChoiceStateUpdates[6])(void) LATE_TBL(08758294) = {
    GameOverChoiceWaitUpdate,
    GameOverChoiceMoveUpdate,
    sub_080cc2b8,
    sub_080cc5d4,
    sub_080cc740,
    sub_080ccd10,
};

/* ---- 0x08758324-0x08758334: 2 table(s), 4 function pointer(s), section .late_tbl_08758324 ---- */
/* include/ending.h; CallTableEntry(i, 2, ...) in sub_080ccec8 */
void (*gUnk_08758324[2])(void) LATE_TBL(08758324) = {
    sub_080ccf2c,
    sub_080cd0cc,
};
/* include/ending.h; CallTableEntry(i, 2, ...) in sub_080ccf10 */
void (*gUnk_0875832C[2])(void) LATE_TBL(08758324) = {
    sub_080cd0c8,
    sub_080cd248,
};
