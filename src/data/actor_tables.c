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

/* The behaviour tables of actor_rodata (0x0873EEA0-0x0874C44B, the rodata of
 * M17-M32; issue #167): 408 state/handler tables, 1443 function pointers, in
 * 102 runs of adjacent tables, each run in address order, and Whispy Woods'
 * seven script streams (one run, 9 function pointers; their opcodes and
 * operands are numbers, as in the data file).  Every table is
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
 * Each run is a named section .actor_tbl_<address>: linker.ld lists the runs
 * between the data pieces of actor_rodata inside ONE output section
 * (tools/ldgroup.py, docs/data.md 5.2).  The tables are const only where
 * their declaration already is: the qualifier would reach CallTableEntry's
 * table parameter (u32 * / void (**)(void)) under -Werror, and a named
 * section holds objects of one kind, so a const table gets a run of its
 * own.  The section attribute is what places them in ROM.  A function whose
 * prototype differs from the declared element type is cast to it, as its
 * consumers call it. */

#define ACTOR_TBL(addr) __attribute__((section(".actor_tbl_" #addr)))

/* ---- 0x0873FB04-0x0873FB7C: 4 table(s), 26 function pointer(s), section .actor_tbl_0873fb04 ---- */
/* include/actor.h; CallTableEntry(i, 8, ...) in PlayerWarpStarRideInit, PlayerWarpStarRideEnterState */
u32 gPlayerWarpStarRideStates[8] ACTOR_TBL(0873fb04) = {
    0,
    (u32)PlayerWarpStarRideState1,
    (u32)PlayerWarpStarRideState2,
    (u32)PlayerWarpStarRideState3,
    (u32)PlayerWarpStarRideState4,
    (u32)PlayerWarpStarRideState5,
    (u32)PlayerWarpStarRideState6,
    (u32)PlayerWarpStarRideState7,
};
/* include/actor.h; CallTableEntry(i, 8, ...) in PlayerWarpStarRideUpdate */
u32 gPlayerWarpStarRideStateUpdates[8] ACTOR_TBL(0873fb04) = {
    0,
    (u32)PlayerWarpStarRideState1Update,
    (u32)PlayerWarpStarRideState2Update,
    (u32)PlayerWarpStarRideState3Update,
    (u32)PlayerWarpStarRideState4Update,
    (u32)PlayerWarpStarRideState5Update,
    (u32)PlayerWarpStarRideState6Update,
    (u32)PlayerWarpStarRideState7Update,
};
/* include/actor.h; CallTableEntry(i, 7, ...) in MetaKnightWarpStarRideInit, MetaKnightWarpStarRideEnterState */
u32 gMetaKnightWarpStarRideStates[7] ACTOR_TBL(0873fb04) = {
    0,
    (u32)MetaKnightWarpStarRideState1,
    (u32)MetaKnightWarpStarRideState2,
    (u32)MetaKnightWarpStarRideState3,
    (u32)MetaKnightWarpStarRideState4,
    (u32)MetaKnightWarpStarRideState5,
    (u32)MetaKnightWarpStarRideState6,
};
/* include/actor.h; CallTableEntry(i, 7, ...) in MetaKnightWarpStarRideUpdate */
u32 gMetaKnightWarpStarRideStateUpdates[7] ACTOR_TBL(0873fb04) = {
    0,
    (u32)MetaKnightWarpStarRideState1Update,
    (u32)MetaKnightWarpStarRideState2Update,
    (u32)MetaKnightWarpStarRideState3Update,
    (u32)MetaKnightWarpStarRideState4Update,
    (u32)MetaKnightWarpStarRideState5Update,
    (u32)MetaKnightWarpStarRideState6Update,
};

/* ---- 0x0873FBAC-0x0873FCF8: 5 table(s), 83 function pointer(s), section .actor_tbl_0873fbac ---- */
/* include/cutscene.h; CallTableEntry(i, 3, ...) in Task_WarpStar, WarpStarEnterState */
u32 gWarpStarStates[3] ACTOR_TBL(0873fbac) = {
    (u32)WarpStarState0,
    (u32)WarpStarState1,
    (u32)WarpStarVanish,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in WarpStarUpdate */
u32 gWarpStarStateUpdates[3] ACTOR_TBL(0873fbac) = {
    (u32)WarpStarState0Update,
    (u32)WarpStarState1Update,
    (u32)sub_08071774,
};
/* include/cutscene.h; CallTableEntry(i, 26, ...) in WarpStarStartFlight, WarpStarFlightEnterState */
u32 gWarpStarFlights[26] ACTOR_TBL(0873fbac) = {
    (u32)sub_08071d60,
    (u32)sub_08071e80,
    (u32)sub_08071d60,
    (u32)sub_08071e80,
    (u32)sub_08074420,
    (u32)WarpStarFlight5,
    (u32)WarpStarFlight6,
    (u32)sub_08071e80,
    (u32)WarpStarFlight8,
    (u32)sub_08071e80,
    (u32)WarpStarFlight10,
    (u32)WarpStarFlight11,
    (u32)WarpStarFlight12,
    (u32)WarpStarFlight13,
    (u32)WarpStarFlight14,
    (u32)WarpStarFlight15,
    (u32)sub_08071e80,
    (u32)WarpStarFlight17,
    (u32)WarpStarFlight18,
    (u32)WarpStarFlight19,
    (u32)WarpStarFlight20,
    (u32)WarpStarFlight21,
    (u32)WarpStarFlight22,
    (u32)WarpStarFlight23,
    (u32)WarpStarFlight24,
    (u32)sub_08074420,
};
/* include/cutscene.h; CallTableEntry(i, 26, ...) in WarpStarFlightUpdate */
u32 gWarpStarFlightUpdates[26] ACTOR_TBL(0873fbac) = {
    (u32)sub_08071e74,
    (u32)sub_08071ebc,
    (u32)sub_08071e74,
    (u32)sub_08071ebc,
    (u32)WarpStarFlight4Update,
    (u32)WarpStarFlight5Update,
    (u32)WarpStarFlight6Update,
    (u32)sub_08071ebc,
    (u32)WarpStarFlight8Update,
    (u32)sub_08071ebc,
    (u32)WarpStarFlight10Update,
    (u32)WarpStarFlight11Update,
    (u32)WarpStarFlight12Update,
    (u32)WarpStarFlight13Update,
    (u32)WarpStarFlight14Update,
    (u32)WarpStarFlight15Update,
    (u32)sub_08071ebc,
    (u32)WarpStarFlight17Update,
    (u32)WarpStarFlight18Update,
    (u32)WarpStarFlight19Update,
    (u32)WarpStarFlight20Update,
    (u32)WarpStarFlight21Update,
    (u32)WarpStarFlight22Update,
    (u32)WarpStarFlight23Update,
    (u32)WarpStarFlight24Update,
    (u32)sub_080743f0,
};
/* include/cutscene.h; CallTableEntry(i, 25, ...) in Task_WarpStarCamera */
u32 gWarpStarCameraPaths[25] ACTOR_TBL(0873fbac) = {
    (u32)sub_080745dc,
    (u32)sub_08074628,
    (u32)sub_080745dc,
    (u32)sub_08074628,
    (u32)WarpStarCameraFollowPlayer,
    (u32)WarpStarCameraPath5,
    (u32)WarpStarCameraPath6,
    (u32)sub_080745d0,
    (u32)WarpStarCameraPath8,
    (u32)sub_080745d0,
    (u32)WarpStarCameraPath10,
    (u32)WarpStarCameraPath11,
    (u32)WarpStarCameraPath12,
    (u32)WarpStarCameraPath13,
    (u32)WarpStarCameraPath14,
    (u32)WarpStarCameraPath15,
    (u32)sub_080745d0,
    (u32)WarpStarCameraPath17,
    (u32)WarpStarCameraPath18,
    (u32)sub_080745d0,
    (u32)WarpStarCameraPath20,
    (u32)WarpStarCameraPath21,
    (u32)sub_080745d0,
    (u32)WarpStarCameraPath23,
    (u32)WarpStarCameraPath24,
};

/* ---- 0x087400B0-0x087400E4: 3 table(s), 13 function pointer(s), section .actor_tbl_087400b0 ---- */
/* include/cutscene.h; CallTableEntry(i, 6, ...) in PlayerCannonInit, PlayerCannonEnterState */
u32 gPlayerCannonStates[6] ACTOR_TBL(087400b0) = {
    (u32)PlayerCannonState0,
    (u32)PlayerCannonState1,
    (u32)PlayerCannonState2,
    (u32)PlayerCannonState3,
    (u32)PlayerCannonState4,
    (u32)PlayerCannonState5,
};
/* include/cutscene.h; CallTableEntry(i, 6, ...) in PlayerCannonUpdate */
u32 gPlayerCannonStateUpdates[6] ACTOR_TBL(087400b0) = {
    (u32)PlayerCannonState0Update,
    (u32)PlayerCannonState1Update,
    (u32)PlayerCannonState2Update,
    (u32)PlayerCannonState3Update,
    (u32)PlayerCannonState4Update,
    (u32)PlayerCannonState5Update,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_Cannon */
u32 gCannonVariants[1] ACTOR_TBL(087400b0) = {
    (u32)CannonInit,
};

/* ---- 0x08740100-0x08740124: 3 table(s), 9 function pointer(s), section .actor_tbl_08740100 ---- */
/* include/cutscene.h; CallTableEntry(i, 4, ...) in CannonInit, CannonEnterState */
u32 gCannonStates[4] ACTOR_TBL(08740100) = {
    (u32)CannonWait,
    (u32)CannonState1,
    (u32)CannonState2,
    (u32)CannonState3,
};
/* include/cutscene.h; CallTableEntry(i, 4, ...) in CannonUpdate */
u32 gCannonStateUpdates[4] ACTOR_TBL(08740100) = {
    (u32)CannonWaitUpdate,
    (u32)CannonState1Update,
    (u32)CannonState2Update,
    (u32)CannonState3Update,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_CannonFuse */
u32 gCannonFuseVariants[1] ACTOR_TBL(08740100) = {
    (u32)CannonFuseInit,
};

/* ---- 0x087402BC-0x08740320: 9 table(s), 25 function pointer(s), section .actor_tbl_087402bc ---- */
/* include/cutscene.h; CallTableEntry(i, 3, ...) in CannonFuseInit, CannonFuseEnterState */
u32 gCannonFuseStates[3] ACTOR_TBL(087402bc) = {
    (u32)CannonFuseWait,
    (u32)CannonFuseBurn,
    (u32)CannonFuseState2,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in CannonFuseUpdate */
u32 gCannonFuseStateUpdates[3] ACTOR_TBL(087402bc) = {
    (u32)CannonFuseWaitUpdate,
    (u32)CannonFuseBurnUpdate,
    (u32)CannonFuseState2Update,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_BigSwitch */
u32 gBigSwitchVariants[1] ACTOR_TBL(087402bc) = {
    (u32)BigSwitchInit,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in BigSwitchInit, BigSwitchEnterState */
u32 gBigSwitchStates[3] ACTOR_TBL(087402bc) = {
    (u32)BigSwitchWait,
    (u32)BigSwitchState1,
    (u32)BigSwitchRefill,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in BigSwitchUpdate */
u32 gBigSwitchStateUpdates[3] ACTOR_TBL(087402bc) = {
    (u32)BigSwitchWaitUpdate,
    (u32)BigSwitchState1Update,
    (u32)BigSwitchRefillUpdate,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_Stake */
u32 gStakeVariants[1] ACTOR_TBL(087402bc) = {
    (u32)StakeInit,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in StakeInit */
u32 gStakeStates[1] ACTOR_TBL(087402bc) = {
    (u32)StakeState0,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in StakeUpdate */
u32 gStakeStateUpdates[1] ACTOR_TBL(087402bc) = {
    (u32)StakeState0Update,
};
/* include/cutscene.h; CallTableEntry(i, 5, ...) in Task_RoomParticles: entries 5-8 lie past that bound; the extent is the span up to the next label (docs/data.md 5.1) */
u32 gRoomParticlesVariants[9] ACTOR_TBL(087402bc) = {
    (u32)RoomParticlesVariant0,
    (u32)RoomParticlesVariant1,
    (u32)RoomParticlesVariant2,
    (u32)RoomParticlesVariant3,
    (u32)RoomParticlesVariant4,
    (u32)sub_080786b4,
    (u32)sub_08078734,
    (u32)sub_080787b8,
    (u32)sub_080788e0,
};

/* ---- 0x08740630-0x08740648: 1 table(s), 6 function pointer(s), section .actor_tbl_08740630 ---- */
/* include/cutscene.h; CallTableEntry(i, 6, ...) in Task_WaddleDee */
u32 gWaddleDeeVariants[6] ACTOR_TBL(08740630) = {
    (u32)WaddleDeeWalkInit,
    (u32)WaddleDeePaceInit,
    (u32)WaddleDeeJumpInit,
    (u32)ParasolWaddleDeeWalkInit,
    (u32)WaddleDeeIdleInit,
    (u32)ParasolWaddleDeeStandInit,
};

/* ---- 0x08740658-0x08740668: 2 table(s), 4 function pointer(s), section .actor_tbl_08740658 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeeWalkInit, WaddleDeeWalkEnterState */
u32 gWaddleDeeWalkStates[2] ACTOR_TBL(08740658) = {
    (u32)WaddleDeeWalk,
    (u32)WaddleDeeWalkFall,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeeWalkUpdate */
u32 gWaddleDeeWalkStateUpdates[2] ACTOR_TBL(08740658) = {
    (u32)WaddleDeeWalkState0Update,
    (u32)WaddleDeeWalkFallUpdate,
};

/* ---- 0x08740670-0x08740680: 2 table(s), 4 function pointer(s), section .actor_tbl_08740670 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeePaceInit, WaddleDeePaceEnterState */
u32 gWaddleDeePaceStates[2] ACTOR_TBL(08740670) = {
    (u32)WaddleDeePaceWalk,
    (u32)WaddleDeePaceFall,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeePaceUpdate */
u32 gWaddleDeePaceStateUpdates[2] ACTOR_TBL(08740670) = {
    (u32)WaddleDeePaceWalkUpdate,
    (u32)WaddleDeePaceFallUpdate,
};

/* ---- 0x087406C4-0x087406EC: 4 table(s), 10 function pointer(s), section .actor_tbl_087406c4 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDeeJumpInit, WaddleDeeJumpEnterState */
u32 gWaddleDeeJumpStates[3] ACTOR_TBL(087406c4) = {
    (u32)WaddleDeeJumpWalk,
    (u32)WaddleDeeJump,
    (u32)WaddleDeeJumpFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDeeJumpUpdate */
u32 gWaddleDeeJumpStateUpdates[3] ACTOR_TBL(087406c4) = {
    (u32)WaddleDeeJumpWalkUpdate,
    (u32)WaddleDeeJumpState1Update,
    (u32)WaddleDeeJumpFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeWalkInit, ParasolWaddleDeeWalkEnterState */
u32 gParasolWaddleDeeWalkStates[2] ACTOR_TBL(087406c4) = {
    (u32)ParasolWaddleDeeWalk,
    (u32)ParasolWaddleDeeWalkState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeWalkUpdate */
u32 gParasolWaddleDeeWalkStateUpdates[2] ACTOR_TBL(087406c4) = {
    (u32)ParasolWaddleDeeWalkState0Update,
    (u32)ParasolWaddleDeeWalkState1Update,
};

/* ---- 0x08740700-0x08740720: 5 table(s), 8 function pointer(s), section .actor_tbl_08740700 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in WaddleDeeIdleInit, WaddleDeeIdleEnterState */
u32 gWaddleDeeIdleStates[1] ACTOR_TBL(08740700) = {
    (u32)WaddleDeeIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WaddleDeeIdleUpdate */
u32 gWaddleDeeIdleStateUpdates[1] ACTOR_TBL(08740700) = {
    (u32)WaddleDeeIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeStandInit, ParasolWaddleDeeStandEnterState */
u32 gParasolWaddleDeeStandStates[2] ACTOR_TBL(08740700) = {
    (u32)ParasolWaddleDeeStandState0,
    (u32)ParasolWaddleDeeStandState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeStandUpdate */
u32 gParasolWaddleDeeStandStateUpdates[2] ACTOR_TBL(08740700) = {
    (u32)ParasolWaddleDeeStandState0Update,
    (u32)ParasolWaddleDeeStandState1Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Pengy */
u32 gPengyVariants[2] ACTOR_TBL(08740700) = {
    (u32)PengyInit,
    (u32)PengyIdleInit,
};

/* ---- 0x08740758-0x087407BC: 10 table(s), 25 function pointer(s), section .actor_tbl_08740758 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in PengyInit, PengyEnterState */
u32 gPengyStates[4] ACTOR_TBL(08740758) = {
    (u32)PengyWait,
    (u32)PengyWalk,
    (u32)PengyShoot,
    (u32)PengyFall,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in PengyUpdate */
u32 gPengyStateUpdates[4] ACTOR_TBL(08740758) = {
    (u32)PengyWaitUpdate,
    (u32)PengyWalkUpdate,
    (u32)PengyShootUpdate,
    (u32)PengyFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIdleInit, PengyIdleEnterState */
u32 gPengyIdleStates[1] ACTOR_TBL(08740758) = {
    (u32)PengyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIdleUpdate */
u32 gPengyIdleStateUpdates[1] ACTOR_TBL(08740758) = {
    (u32)PengyIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Bomber */
u32 gBomberVariants[2] ACTOR_TBL(08740758) = {
    (u32)BomberInit,
    (u32)BomberIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in BomberInit, BomberEnterState */
u32 gBomberStates[4] ACTOR_TBL(08740758) = {
    (u32)BomberWalk,
    (u32)BomberState1,
    (u32)BomberState2,
    (u32)BomberExplode,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in BomberUpdate */
u32 gBomberStateUpdates[4] ACTOR_TBL(08740758) = {
    (u32)BomberWalkUpdate,
    (u32)BomberState1Update,
    (u32)BomberState2Update,
    (u32)BomberExplodeUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BomberIdleInit, BomberIdleEnterState */
u32 gBomberIdleStates[1] ACTOR_TBL(08740758) = {
    (u32)BomberIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BomberIdleUpdate */
u32 gBomberIdleStateUpdates[1] ACTOR_TBL(08740758) = {
    (u32)BomberIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Sparky */
u32 gSparkyVariants[3] ACTOR_TBL(08740758) = {
    (u32)SparkyJumpInit,
    (u32)SparkyIdleInit,
    (u32)SparkyStandInit,
};

/* ---- 0x087407E4-0x0874080C: 4 table(s), 10 function pointer(s), section .actor_tbl_087407e4 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in SparkyJumpInit, SparkyJumpEnterState */
u32 gSparkyJumpStates[4] ACTOR_TBL(087407e4) = {
    (u32)SparkyJumpDischarge,
    (u32)SparkyJumpLand,
    (u32)SparkyJump,
    (u32)SparkyJumpState3,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in SparkyJumpUpdate */
u32 gSparkyJumpStateUpdates[4] ACTOR_TBL(087407e4) = {
    (u32)SparkyJumpDischargeUpdate,
    (u32)SparkyJumpLandUpdate,
    (u32)SparkyJumpState2Update,
    (u32)SparkyJumpState3Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SparkyIdleInit, SparkyIdleEnterState */
u32 gSparkyIdleStates[1] ACTOR_TBL(087407e4) = {
    (u32)SparkyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SparkyIdleUpdate */
u32 gSparkyIdleStateUpdates[1] ACTOR_TBL(087407e4) = {
    (u32)SparkyIdleState0Update,
};

/* ---- 0x08740810-0x08740824: 3 table(s), 5 function pointer(s), section .actor_tbl_08740810 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in SparkyStandInit, SparkyStandEnterState */
u32 gSparkyStandStates[2] ACTOR_TBL(08740810) = {
    (u32)SparkyStandDischarge,
    (u32)SparkyStandWait,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in SparkyStandUpdate */
u32 gSparkyStandStateUpdates[2] ACTOR_TBL(08740810) = {
    (u32)SparkyStandDischargeUpdate,
    (u32)SparkyStandWaitUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Scarfy */
u32 gScarfyVariants[1] ACTOR_TBL(08740810) = {
    (u32)ScarfyInit,
};

/* ---- 0x08740960-0x0874099C: 3 table(s), 15 function pointer(s), section .actor_tbl_08740960 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in ScarfyInit, ScarfyEnterState */
u32 gScarfyStates[6] ACTOR_TBL(08740960) = {
    (u32)ScarfyHide,
    (u32)ScarfyState1,
    (u32)ScarfyHover,
    (u32)ScarfyTransform,
    (u32)ScarfyChase,
    (u32)ScarfyExplode,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in ScarfyUpdate */
u32 gScarfyStateUpdates[6] ACTOR_TBL(08740960) = {
    (u32)ScarfyHideUpdate,
    (u32)ScarfyState1Update,
    (u32)ScarfyHoverUpdate,
    (u32)ScarfyTransformUpdate,
    (u32)ScarfyChaseUpdate,
    (u32)ScarfyExplodeUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_SwordKnight, Task_BladeKnight */
u32 gSwordAndBladeKnightVariants[3] ACTOR_TBL(08740960) = {
    (u32)SwordAndBladeKnightWalkInit,
    (u32)SwordAndBladeKnightIdleInit,
    (u32)SwordAndBladeKnightStandInit,
};

/* ---- 0x087409FC-0x08740A80: 8 table(s), 33 function pointer(s), section .actor_tbl_087409fc ---- */
/* include/enemy.h; CallTableEntry(i, 8, ...) in SwordAndBladeKnightWalkInit, SwordAndBladeKnightWalkEnterState */
u32 gSwordAndBladeKnightWalkStates[8] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightWalkState0,
    (u32)SwordAndBladeKnightWalkState1,
    (u32)SwordAndBladeKnightWalkState2,
    (u32)SwordAndBladeKnightWalkState3,
    (u32)SwordAndBladeKnightWalkState4,
    (u32)SwordAndBladeKnightWalkState5,
    (u32)SwordAndBladeKnightWalkState6,
    (u32)SwordAndBladeKnightWalkFall,
};
/* include/enemy.h; CallTableEntry(i, 8, ...) in SwordAndBladeKnightWalkUpdate */
u32 gSwordAndBladeKnightWalkStateUpdates[8] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightWalkState0Update,
    (u32)SwordAndBladeKnightWalkState1Update,
    (u32)SwordAndBladeKnightWalkState2Update,
    (u32)SwordAndBladeKnightWalkState3Update,
    (u32)SwordAndBladeKnightWalkState4Update,
    (u32)SwordAndBladeKnightWalkState5Update,
    (u32)SwordAndBladeKnightWalkState6Update,
    (u32)SwordAndBladeKnightWalkFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SwordAndBladeKnightIdleInit, SwordAndBladeKnightIdleEnterState */
u32 gSwordAndBladeKnightIdleStates[1] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SwordAndBladeKnightIdleUpdate */
u32 gSwordAndBladeKnightIdleStateUpdates[1] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in SwordAndBladeKnightStandInit, SwordAndBladeKnightStandEnterState */
u32 gSwordAndBladeKnightStandStates[6] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightStandState0,
    (u32)SwordAndBladeKnightStandState1,
    (u32)SwordAndBladeKnightStandState2,
    (u32)SwordAndBladeKnightStandState3,
    (u32)SwordAndBladeKnightStandState4,
    (u32)SwordAndBladeKnightStandState5,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in SwordAndBladeKnightStandUpdate */
u32 gSwordAndBladeKnightStandStateUpdates[6] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightStandState0Update,
    (u32)SwordAndBladeKnightStandState1Update,
    (u32)SwordAndBladeKnightStandState2Update,
    (u32)SwordAndBladeKnightStandState3Update,
    (u32)SwordAndBladeKnightStandState4Update,
    (u32)SwordAndBladeKnightStandState5Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_BlockStar */
u32 gBlockStarVariants[1] ACTOR_TBL(087409fc) = {
    (u32)BlockStarInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Needlous */
u32 gNeedlousVariants[2] ACTOR_TBL(087409fc) = {
    (u32)NeedlousInit,
    (u32)NeedlousIdleInit,
};

/* ---- 0x08740AC8-0x08740B08: 5 table(s), 16 function pointer(s), section .actor_tbl_08740ac8 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in NeedlousInit, NeedlousEnterState */
u32 gNeedlousStates[6] ACTOR_TBL(08740ac8) = {
    (u32)NeedlousWalk,
    (u32)NeedlousFall,
    (u32)NeedlousState2,
    (u32)NeedlousState3,
    (u32)NeedlousState4,
    (u32)NeedlousDash,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NeedlousUpdate */
u32 gNeedlousStateUpdates[6] ACTOR_TBL(08740ac8) = {
    (u32)NeedlousWalkUpdate,
    (u32)NeedlousFallUpdate,
    (u32)NeedlousState2Update,
    (u32)NeedlousState3Update,
    (u32)NeedlousState4Update,
    (u32)NeedlousDashUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NeedlousIdleInit, NeedlousIdleEnterState */
u32 gNeedlousIdleStates[1] ACTOR_TBL(08740ac8) = {
    (u32)NeedlousIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NeedlousIdleUpdate */
u32 gNeedlousIdleStateUpdates[1] ACTOR_TBL(08740ac8) = {
    (u32)NeedlousIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_UFO */
u32 gUFOVariants[2] ACTOR_TBL(08740ac8) = {
    (u32)UFOInit,
    (u32)UFOIdleInit,
};

/* ---- 0x08740B84-0x08740BD4: 11 table(s), 20 function pointer(s), section .actor_tbl_08740b84 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in UFOInit, UFOEnterState */
u32 gUFOStates[4] ACTOR_TBL(08740b84) = {
    (u32)UFOZigzag,
    (u32)UFOPickMove,
    (u32)UFOFlyToTarget,
    (u32)UFOShoot,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in UFOUpdate */
u32 gUFOStateUpdates[4] ACTOR_TBL(08740b84) = {
    (u32)UFOZigzagUpdate,
    (u32)UFOPickMoveUpdate,
    (u32)UFOFlyToTargetUpdate,
    (u32)UFOShootUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in UFOIdleInit, UFOIdleEnterState */
u32 gUFOIdleStates[1] ACTOR_TBL(08740b84) = {
    (u32)UFOIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in UFOIdleUpdate */
u32 gUFOIdleStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)UFOIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Parasol */
u32 gParasolVariants[4] ACTOR_TBL(08740b84) = {
    (u32)ParasolRiseInit,
    (u32)ParasolChaseInit,
    (u32)ParasolIdleInit,
    (u32)ParasolVariant3,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolRiseInit, ParasolRiseEnterState */
u32 gParasolRiseStates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolRise,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolRiseUpdate */
u32 gParasolRiseStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolRiseState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolChaseInit, ParasolChaseEnterState */
u32 gParasolChaseStates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolChase,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolChaseUpdate */
u32 gParasolChaseStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolChaseState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolIdleInit, ParasolIdleEnterState */
u32 gParasolIdleStates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolIdleUpdate */
u32 gParasolIdleStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolIdleState0Update,
};

/* ---- 0x08741088-0x08741094: 3 table(s), 3 function pointer(s), section .actor_tbl_08741088 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_PengyIceBreath */
u32 gPengyIceBreathVariants[1] ACTOR_TBL(08741088) = {
    (u32)PengyIceBreathInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIceBreathInit, PengyIceBreathEnterState */
u32 gPengyIceBreathStates[1] ACTOR_TBL(08741088) = {
    (u32)PengyIceBreathState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIceBreathUpdate */
u32 gPengyIceBreathStateUpdates[1] ACTOR_TBL(08741088) = {
    (u32)PengyIceBreathState0Update,
};

/* ---- 0x087410AC-0x087410C0: 3 table(s), 5 function pointer(s), section .actor_tbl_087410ac ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_UFOLaser */
u32 gUFOLaserVariants[1] ACTOR_TBL(087410ac) = {
    (u32)UFOLaserInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in UFOLaserInit, UFOLaserEnterState */
u32 gUFOLaserStates[2] ACTOR_TBL(087410ac) = {
    (u32)UFOLaserState0,
    (u32)UFOLaserVanish,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in UFOLaserUpdate */
u32 gUFOLaserStateUpdates[2] ACTOR_TBL(087410ac) = {
    (u32)UFOLaserState0Update,
    (u32)sub_0807d384,
};

/* ---- 0x087411C0-0x08741214: 7 table(s), 21 function pointer(s), section .actor_tbl_087411c0 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Rocky */
u32 gRockyVariants[3] ACTOR_TBL(087411c0) = {
    (u32)RockyWalkInit,
    (u32)RockyIdleInit,
    (u32)RockyStandInit,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in RockyWalkInit, RockyWalkEnterState */
u32 gRockyWalkStates[5] ACTOR_TBL(087411c0) = {
    (u32)RockyWalk,
    (u32)RockyWalkState1,
    (u32)RockyWalkState2,
    (u32)RockyWalkState3,
    (u32)RockyWalkState4,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in RockyWalkUpdate */
u32 gRockyWalkStateUpdates[5] ACTOR_TBL(087411c0) = {
    (u32)RockyWalkState0Update,
    (u32)RockyWalkState1Update,
    (u32)RockyWalkState2Update,
    (u32)RockyWalkState3Update,
    (u32)RockyWalkState4Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in RockyIdleInit, RockyIdleEnterState */
u32 gRockyIdleStates[1] ACTOR_TBL(087411c0) = {
    (u32)RockyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in RockyIdleUpdate */
u32 gRockyIdleStateUpdates[1] ACTOR_TBL(087411c0) = {
    (u32)RockyIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in RockyStandInit, RockyStandEnterState */
u32 gRockyStandStates[3] ACTOR_TBL(087411c0) = {
    (u32)RockyStandState0,
    (u32)RockyStandState1,
    (u32)RockyStandState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in RockyStandUpdate */
u32 gRockyStandStateUpdates[3] ACTOR_TBL(087411c0) = {
    (u32)RockyStandState0Update,
    (u32)RockyStandState1Update,
    (u32)RockyStandState2Update,
};

/* ---- 0x08741220-0x08741298: 15 table(s), 30 function pointer(s), section .actor_tbl_08741220 ---- */
/* include/enemy.h; read by SirKibbleShootUpdate, SirKibbleJumpUpdate */
u32 gSirKibbleEnterStates[2] ACTOR_TBL(08741220) = {
    (u32)SirKibbleStandEnterState,
    (u32)SirKibbleWalkEnterState,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_SirKibble */
u32 gSirKibbleVariants[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleStandInit,
    (u32)SirKibbleWalkInit,
    (u32)SirKibbleIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleStandInit, SirKibbleStandEnterState */
u32 gSirKibbleStandStates[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleWait,
    (u32)SirKibbleShoot,
    (u32)SirKibbleJump,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleStandUpdate */
u32 gSirKibbleStandStateUpdates[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleWaitUpdate,
    (u32)SirKibbleShootUpdate,
    (u32)SirKibbleJumpUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleWalkInit, SirKibbleWalkEnterState */
u32 gSirKibbleWalkStates[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleWalk,
    (u32)SirKibbleShoot,
    (u32)SirKibbleJump,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleWalkUpdate */
u32 gSirKibbleWalkStateUpdates[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleWalkState0Update,
    (u32)SirKibbleShootUpdate,
    (u32)SirKibbleJumpUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleIdleInit, SirKibbleIdleEnterState */
u32 gSirKibbleIdleStates[1] ACTOR_TBL(08741220) = {
    (u32)SirKibbleIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleIdleUpdate */
u32 gSirKibbleIdleStateUpdates[1] ACTOR_TBL(08741220) = {
    (u32)SirKibbleIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Cappy */
u32 gCappyVariants[3] ACTOR_TBL(08741220) = {
    (u32)CappyCappedInit,
    (u32)CappyCaplessInit,
    (u32)CappyStandInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyCappedInit, CappyCappedEnterState */
u32 gCappyCappedStates[1] ACTOR_TBL(08741220) = {
    (u32)CappyCappedHop,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyCappedUpdate */
u32 gCappyCappedStateUpdates[1] ACTOR_TBL(08741220) = {
    (u32)CappyCappedHopUpdate,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in CappyCaplessInit, CappyCaplessEnterState */
u32 gCappyCaplessStates[2] ACTOR_TBL(08741220) = {
    (u32)CappyCaplessHop,
    (u32)CappyCaplessJump,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in CappyCaplessUpdate */
u32 gCappyCaplessStateUpdates[2] ACTOR_TBL(08741220) = {
    (u32)CappyCaplessHopUpdate,
    (u32)CappyCaplessJumpUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyStandInit, CappyStandEnterState */
u32 gCappyStandStates[1] ACTOR_TBL(08741220) = {
    (u32)CappyStandHop,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyStandUpdate */
u32 gCappyStandStateUpdates[1] ACTOR_TBL(08741220) = {
    (u32)CappyStandHopUpdate,
};

/* ---- 0x087412BC-0x087412EC: 9 table(s), 12 function pointer(s), section .actor_tbl_087412bc ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Gordo */
u32 gGordoVariants[4] ACTOR_TBL(087412bc) = {
    (u32)GordoBobInit,
    (u32)GordoBounceVerticalInit,
    (u32)GordoBounceHorizontalInit,
    (u32)GordoSweepInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBobInit */
u32 gGordoBobStates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBob,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBobUpdate */
u32 gGordoBobStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBobState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceVerticalInit */
u32 gGordoBounceVerticalStates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBounceVertical,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceVerticalUpdate */
u32 gGordoBounceVerticalStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBounceVerticalState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceHorizontalInit */
u32 gGordoBounceHorizontalStates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBounceHorizontal,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceHorizontalUpdate */
u32 gGordoBounceHorizontalStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoBounceHorizontalState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoSweepInit */
u32 gGordoSweepStates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoSweep,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoSweepUpdate */
u32 gGordoSweepStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)GordoSweepState0Update,
};

/* ---- 0x08741300-0x08741318: 5 table(s), 6 function pointer(s), section .actor_tbl_08741300 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_CoolSpook */
u32 gCoolSpookVariants[2] ACTOR_TBL(08741300) = {
    (u32)CoolSpookFlyInit,
    (u32)CoolSpookBobInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoolSpookFlyInit, CoolSpookFlyEnterState */
u32 gCoolSpookFlyStates[1] ACTOR_TBL(08741300) = {
    (u32)CoolSpookFly,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoolSpookFlyUpdate */
u32 gCoolSpookFlyStateUpdates[1] ACTOR_TBL(08741300) = {
    (u32)CoolSpookFlyState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoolSpookBobInit, CoolSpookBobEnterState */
u32 gCoolSpookBobStates[1] ACTOR_TBL(08741300) = {
    (u32)CoolSpookBob,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoolSpookBobUpdate */
u32 gCoolSpookBobStateUpdates[1] ACTOR_TBL(08741300) = {
    (u32)CoolSpookBobState0Update,
};

/* ---- 0x08741380-0x087413D8: 9 table(s), 22 function pointer(s), section .actor_tbl_08741380 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Kabu */
u32 gKabuVariants[4] ACTOR_TBL(08741380) = {
    (u32)KabuJumpInit,
    (u32)KabuTeleportInit,
    (u32)KabuSlideInit,
    (u32)KabuIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuJumpInit, KabuJumpEnterState */
u32 gKabuJumpStates[3] ACTOR_TBL(08741380) = {
    (u32)KabuJumpSpin,
    (u32)KabuJump,
    (u32)KabuJumpFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuJumpUpdate */
u32 gKabuJumpStateUpdates[3] ACTOR_TBL(08741380) = {
    (u32)KabuJumpSpinUpdate,
    (u32)KabuJumpState1Update,
    (u32)KabuJumpFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuTeleportInit, KabuTeleportEnterState */
u32 gKabuTeleportStates[3] ACTOR_TBL(08741380) = {
    (u32)KabuTeleportSpin,
    (u32)KabuTeleport,
    (u32)KabuTeleportState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuTeleportUpdate */
u32 gKabuTeleportStateUpdates[3] ACTOR_TBL(08741380) = {
    (u32)KabuTeleportSpinUpdate,
    (u32)KabuTeleportState1Update,
    (u32)KabuTeleportState2Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KabuSlideInit, KabuSlideEnterState */
u32 gKabuSlideStates[2] ACTOR_TBL(08741380) = {
    (u32)KabuSlide,
    (u32)KabuSlideState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KabuSlideUpdate */
u32 gKabuSlideStateUpdates[2] ACTOR_TBL(08741380) = {
    (u32)KabuSlideState0Update,
    (u32)KabuSlideState1Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KabuIdleInit, KabuIdleEnterState */
u32 gKabuIdleStates[1] ACTOR_TBL(08741380) = {
    (u32)KabuIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KabuIdleUpdate */
u32 gKabuIdleStateUpdates[1] ACTOR_TBL(08741380) = {
    (u32)KabuIdleState0Update,
};

/* ---- 0x08741488-0x087414B0: 5 table(s), 10 function pointer(s), section .actor_tbl_08741488 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Twister */
u32 gTwisterVariants[2] ACTOR_TBL(08741488) = {
    (u32)TwisterInit,
    (u32)TwisterIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwisterInit, TwisterEnterState */
u32 gTwisterStates[3] ACTOR_TBL(08741488) = {
    (u32)TwisterState0,
    (u32)TwisterState1,
    (u32)TwisterState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwisterUpdate */
u32 gTwisterStateUpdates[3] ACTOR_TBL(08741488) = {
    (u32)TwisterState0Update,
    (u32)TwisterState1Update,
    (u32)TwisterState2Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwisterIdleInit, TwisterIdleEnterState */
u32 gTwisterIdleStates[1] ACTOR_TBL(08741488) = {
    (u32)TwisterIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwisterIdleUpdate */
u32 gTwisterIdleStateUpdates[1] ACTOR_TBL(08741488) = {
    (u32)TwisterIdleState0Update,
};

/* ---- 0x087414B4-0x087414F8: 7 table(s), 17 function pointer(s), section .actor_tbl_087414b4 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_HotHead */
u32 gHotHeadVariants[3] ACTOR_TBL(087414b4) = {
    (u32)HotHeadWalkInit,
    (u32)HotHeadIdleInit,
    (u32)HotHeadStandInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadWalkInit, HotHeadWalkEnterState */
u32 gHotHeadWalkStates[3] ACTOR_TBL(087414b4) = {
    (u32)HotHeadWalk,
    (u32)HotHeadWalkShoot,
    (u32)HotHeadWalkFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadWalkUpdate */
u32 gHotHeadWalkStateUpdates[3] ACTOR_TBL(087414b4) = {
    (u32)HotHeadWalkState0Update,
    (u32)HotHeadWalkShootUpdate,
    (u32)HotHeadWalkFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadIdleInit, HotHeadIdleEnterState */
u32 gHotHeadIdleStates[1] ACTOR_TBL(087414b4) = {
    (u32)HotHeadIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadIdleUpdate */
u32 gHotHeadIdleStateUpdates[1] ACTOR_TBL(087414b4) = {
    (u32)HotHeadIdleState0Update,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadStandInit, HotHeadStandEnterState */
u32 gHotHeadStandStates[3] ACTOR_TBL(087414b4) = {
    (u32)HotHeadStandWait,
    (u32)HotHeadStandShoot,
    (u32)HotHeadStandFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadStandUpdate */
u32 gHotHeadStandStateUpdates[3] ACTOR_TBL(087414b4) = {
    (u32)HotHeadStandWaitUpdate,
    (u32)HotHeadStandShootUpdate,
    (u32)HotHeadStandFallUpdate,
};

/* ---- 0x08741544-0x087415AC: 9 table(s), 26 function pointer(s), section .actor_tbl_08741544 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Starman */
u32 gStarmanVariants[4] ACTOR_TBL(08741544) = {
    (u32)StarmanVariant0,
    (u32)StarmanJumpInit,
    (u32)StarmanFlyInit,
    (u32)StarmanIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in StarmanVariant0, sub_08080e40 */
u32 gUnk_08741554[6] ACTOR_TBL(08741544) = {
    (u32)sub_08080ea4,
    (u32)sub_08080f34,
    (u32)sub_08081084,
    (u32)sub_08081140,
    (u32)sub_080812f0,
    (u32)sub_08081274,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in sub_08080e5c */
u32 gUnk_0874156C[6] ACTOR_TBL(08741544) = {
    (u32)sub_08080edc,
    (u32)sub_08080fa4,
    (u32)sub_080810c4,
    (u32)sub_0808124c,
    (u32)sub_080813a4,
    (u32)sub_080812ec,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in StarmanJumpInit, StarmanJumpEnterState */
u32 gStarmanJumpStates[3] ACTOR_TBL(08741544) = {
    (u32)StarmanJumpWalk,
    (u32)StarmanJump,
    (u32)StarmanJumpFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in StarmanJumpUpdate */
u32 gStarmanJumpStateUpdates[3] ACTOR_TBL(08741544) = {
    (u32)StarmanJumpWalkUpdate,
    (u32)StarmanJumpState1Update,
    (u32)StarmanJumpFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanFlyInit, StarmanFlyEnterState */
u32 gStarmanFlyStates[1] ACTOR_TBL(08741544) = {
    (u32)StarmanFly,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanFlyUpdate */
u32 gStarmanFlyStateUpdates[1] ACTOR_TBL(08741544) = {
    (u32)StarmanFlyState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanIdleInit, StarmanIdleEnterState */
u32 gStarmanIdleStates[1] ACTOR_TBL(08741544) = {
    (u32)StarmanIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanIdleUpdate */
u32 gStarmanIdleStateUpdates[1] ACTOR_TBL(08741544) = {
    (u32)StarmanIdleState0Update,
};

/* ---- 0x087415B8-0x087415E4: 5 table(s), 11 function pointer(s), section .actor_tbl_087415b8 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_PoppyBrosJr */
u32 gPoppyBrosJrVariants[3] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrInit,
    (u32)PoppyBrosJrInit,
    (u32)PoppyBrosJrStandInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in PoppyBrosJrInit, PoppyBrosJrEnterState */
u32 gPoppyBrosJrStates[3] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrHop,
    (u32)PoppyBrosJrWalk,
    (u32)PoppyBrosJrJump,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in PoppyBrosJrUpdate */
u32 gPoppyBrosJrStateUpdates[3] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrHopUpdate,
    (u32)PoppyBrosJrWalkUpdate,
    (u32)PoppyBrosJrJumpUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrStandInit, PoppyBrosJrStandEnterState */
u32 gPoppyBrosJrStandStates[1] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrStandHop,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrStandUpdate */
u32 gPoppyBrosJrStandStateUpdates[1] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrStandHopUpdate,
};

/* ---- 0x08741604-0x08741628: 7 table(s), 9 function pointer(s), section .actor_tbl_08741604 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_PoppyBrosJrOnApple, Task_PoppyBrosJrOnMaximTomato */
u32 gPoppyBrosJrRideVariants[3] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrRideInit,
    (u32)PoppyBrosJrDroppedObjectInit,
    (u32)PoppyBrosJrRideIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideInit, PoppyBrosJrRideEnterState */
u32 gPoppyBrosJrRideStates[1] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrRide,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideUpdate */
u32 gPoppyBrosJrRideStateUpdates[1] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrRideState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrDroppedObjectInit, PoppyBrosJrDroppedObjectEnterState */
u32 gPoppyBrosJrDroppedObjectStates[1] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrDroppedObjectState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrDroppedObjectUpdate */
u32 gPoppyBrosJrDroppedObjectStateUpdates[1] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrDroppedObjectState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideIdleInit, PoppyBrosJrRideIdleEnterState */
u32 gPoppyBrosJrRideIdleStates[1] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrRideIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideIdleUpdate */
u32 gPoppyBrosJrRideIdleStateUpdates[1] ACTOR_TBL(08741604) = {
    (u32)sub_08082548,
};

/* ---- 0x08741640-0x08741684: 5 table(s), 17 function pointer(s), section .actor_tbl_08741640 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Wheelie */
u32 gWheelieVariants[3] ACTOR_TBL(08741640) = {
    (u32)WheelieInit,
    (u32)WheelieInit,
    (u32)WheelieIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in WheelieInit, WheelieEnterState */
u32 gWheelieStates[6] ACTOR_TBL(08741640) = {
    (u32)WheelieState0,
    (u32)WheelieState1,
    (u32)WheelieSkid,
    (u32)WheelieWait,
    (u32)WheelieBounceOffWall,
    (u32)WheelieFall,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in WheelieUpdate */
u32 gWheelieStateUpdates[6] ACTOR_TBL(08741640) = {
    (u32)WheelieState0Update,
    (u32)WheelieState1Update,
    (u32)WheelieSkidUpdate,
    (u32)WheelieWaitUpdate,
    (u32)WheelieBounceOffWallUpdate,
    (u32)WheelieFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WheelieIdleInit, WheelieIdleEnterState */
u32 gWheelieIdleStates[1] ACTOR_TBL(08741640) = {
    (u32)WheelieIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WheelieIdleUpdate */
u32 gWheelieIdleStateUpdates[1] ACTOR_TBL(08741640) = {
    (u32)WheelieIdleState0Update,
};

/* ---- 0x0874176C-0x087417B8: 5 table(s), 19 function pointer(s), section .actor_tbl_0874176c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Flamer */
u32 gFlamerVariants[3] ACTOR_TBL(0874176c) = {
    (u32)FlamerInit,
    (u32)FlamerInit,
    (u32)FlamerIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in FlamerInit, FlamerEnterState */
u32 gFlamerStates[7] ACTOR_TBL(0874176c) = {
    (u32)FlamerState0,
    (u32)FlamerCrawl,
    (u32)FlamerFall,
    (u32)FlamerState3,
    (u32)FlamerState4,
    (u32)FlamerState5,
    (u32)FlamerState6,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in FlamerUpdate */
u32 gFlamerStateUpdates[7] ACTOR_TBL(0874176c) = {
    (u32)FlamerState0Update,
    (u32)FlamerCrawlUpdate,
    (u32)FlamerFallUpdate,
    (u32)FlamerState3Update,
    (u32)FlamerState4Update,
    (u32)FlamerState5Update,
    (u32)FlamerState6Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in FlamerIdleInit, FlamerIdleEnterState */
u32 gFlamerIdleStates[1] ACTOR_TBL(0874176c) = {
    (u32)FlamerIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in FlamerIdleUpdate */
u32 gFlamerIdleStateUpdates[1] ACTOR_TBL(0874176c) = {
    (u32)FlamerIdleState0Update,
};

/* ---- 0x08741E64-0x08741E70: 3 table(s), 3 function pointer(s), section .actor_tbl_08741e64 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_SirKibbleCutter */
u32 gSirKibbleCutterVariants[1] ACTOR_TBL(08741e64) = {
    (u32)SirKibbleCutterInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleCutterInit, SirKibbleCutterEnterState */
u32 gSirKibbleCutterStates[1] ACTOR_TBL(08741e64) = {
    (u32)SirKibbleCutterState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleCutterUpdate */
u32 gSirKibbleCutterStateUpdates[1] ACTOR_TBL(08741e64) = {
    (u32)SirKibbleCutterState0Update,
};

/* ---- 0x08741E7C-0x08741E94: 5 table(s), 6 function pointer(s), section .actor_tbl_08741e7c ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_HotHeadFire */
u32 gHotHeadFireVariants[2] ACTOR_TBL(08741e7c) = {
    (u32)HotHeadFireVariant0,
    (u32)HotHeadFireVariant1,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadFireVariant0, sub_080840d4 */
u32 gUnk_08741E84[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_08084114,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080840f0 */
u32 gUnk_08741E88[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_08084248,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadFireVariant1, sub_0808429c */
u32 gUnk_08741E8C[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_08084308,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080842b8 */
u32 gUnk_08741E90[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_080843f8,
};

/* ---- 0x08741F70-0x08741FA8: 3 table(s), 14 function pointer(s), section .actor_tbl_08741f70 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Noddy */
u32 gNoddyVariants[2] ACTOR_TBL(08741f70) = {
    (u32)NoddyInit,
    (u32)NoddyVariant1,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NoddyInit, NoddyEnterState */
u32 gNoddyStates[6] ACTOR_TBL(08741f70) = {
    (u32)NoddyWalk,
    (u32)NoddyFallAsleep,
    (u32)NoddySleep,
    (u32)NoddyWakeUp,
    (u32)NoddySleepFall,
    (u32)NoddyState5,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NoddyUpdate */
u32 gNoddyStateUpdates[6] ACTOR_TBL(08741f70) = {
    (u32)NoddyWalkUpdate,
    (u32)NoddyFallAsleepUpdate,
    (u32)NoddySleepUpdate,
    (u32)NoddyWakeUpUpdate,
    (u32)NoddySleepFallUpdate,
    (u32)NoddyState5Update,
};

/* ---- 0x08741FB8-0x08742010: 6 table(s), 22 function pointer(s), section .actor_tbl_08741fb8 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Chilly */
u32 gChillyVariants[2] ACTOR_TBL(08741fb8) = {
    (u32)ChillyInit,
    (u32)ChillyIdle,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in ChillyInit, ChillyEnterState */
u32 gChillyStates[5] ACTOR_TBL(08741fb8) = {
    (u32)ChillyState0,
    (u32)ChillyWait,
    (u32)ChillySlide,
    (u32)ChillyState3,
    (u32)ChillyFall,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in ChillyUpdate */
u32 gChillyStateUpdates[5] ACTOR_TBL(08741fb8) = {
    (u32)ChillyState0Update,
    (u32)ChillyWaitUpdate,
    (u32)ChillySlideUpdate,
    (u32)ChillyState3Update,
    (u32)ChillyFallUpdate,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_WaddleDoo */
u32 gWaddleDooVariants[4] ACTOR_TBL(08741fb8) = {
    (u32)WaddleDooWalkInit,
    (u32)ParasolWaddleDooInit,
    (u32)WaddleDooIdle,
    (u32)WaddleDooShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDooWalkInit, WaddleDooWalkEnterState */
u32 gWaddleDooWalkStates[3] ACTOR_TBL(08741fb8) = {
    (u32)WaddleDooWalk,
    (u32)WaddleDooWalkJump,
    (u32)WaddleDooWalkShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDooWalkUpdate */
u32 gWaddleDooWalkStateUpdates[3] ACTOR_TBL(08741fb8) = {
    (u32)WaddleDooWalkState0Update,
    (u32)WaddleDooWalkJumpUpdate,
    (u32)WaddleDooWalkShootUpdate,
};

/* ---- 0x08742030-0x08742050: 2 table(s), 8 function pointer(s), section .actor_tbl_08742030 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in ParasolWaddleDooInit, ParasolWaddleDooEnterState */
u32 gParasolWaddleDooStates[4] ACTOR_TBL(08742030) = {
    (u32)ParasolWaddleDooWalk,
    (u32)ParasolWaddleDooJump,
    (u32)ParasolWaddleDooShoot,
    (u32)ParasolWaddleDooDrift,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in ParasolWaddleDooUpdate */
u32 gParasolWaddleDooStateUpdates[4] ACTOR_TBL(08742030) = {
    (u32)ParasolWaddleDooWalkUpdate,
    (u32)ParasolWaddleDooJumpUpdate,
    (u32)ParasolWaddleDooShootUpdate,
    (u32)ParasolWaddleDooDriftUpdate,
};

/* ---- 0x08742064-0x08742088: 3 table(s), 9 function pointer(s), section .actor_tbl_08742064 ---- */
/* include/enemy.h; CallTableEntry(i, 7, ...) in Task_BrontoBurt, BrontoBurtEnterVariant */
u32 gBrontoBurtVariants[7] ACTOR_TBL(08742064) = {
    (u32)BrontoBurtWaveInit,
    (u32)BrontoBurtWeaveInit,
    (u32)BrontoBurtSwoopInit,
    (u32)BrontoBurtDiagonalInit,
    (u32)BrontoBurtChaseInit,
    (u32)BrontoBurtTakeOffInit,
    (u32)BrontoBurtIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWaveInit, BrontoBurtWaveEnterState */
u32 gBrontoBurtWaveStates[1] ACTOR_TBL(08742064) = {
    (u32)BrontoBurtWave,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWaveUpdate */
u32 gBrontoBurtWaveStateUpdates[1] ACTOR_TBL(08742064) = {
    (u32)BrontoBurtWaveState0Update,
};

/* ---- 0x087420A0-0x087420AC: 3 table(s), 3 function pointer(s), section .actor_tbl_087420a0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWeaveInit, BrontoBurtWeaveEnterState */
u32 gBrontoBurtWeaveStates[1] ACTOR_TBL(087420a0) = {
    (u32)BrontoBurtWeave,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWeaveUpdate */
u32 gBrontoBurtWeaveStateUpdates[1] ACTOR_TBL(087420a0) = {
    (u32)sub_08086444,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtSwoopInit */
s32 gBrontoBurtSwoopStates[1] ACTOR_TBL(087420a0) = {
    (s32)BrontoBurtSwoop,
};

/* ---- 0x087420BC-0x087420C0: 1 table(s), 1 function pointer(s), section .actor_tbl_087420bc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtSwoopUpdate */
u32 gBrontoBurtSwoopStateUpdates[1] ACTOR_TBL(087420bc) = {
    (u32)BrontoBurtSwoopState0Update,
};

/* ---- 0x087420F0-0x087420F4: 1 table(s), 1 function pointer(s), section .actor_tbl_087420f0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtDiagonalInit, BrontoBurtDiagonalEnterState */
u32 gBrontoBurtDiagonalStates[1] ACTOR_TBL(087420f0) = {
    (u32)BrontoBurtDiagonal,
};

/* ---- 0x08742100-0x0874210C: 3 table(s), 3 function pointer(s), section .actor_tbl_08742100 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtDiagonalUpdate */
u32 gBrontoBurtDiagonalStateUpdates[1] ACTOR_TBL(08742100) = {
    (u32)BrontoBurtDiagonalState0Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtChaseInit, BrontoBurtChaseEnterState */
u32 gBrontoBurtChaseStates[1] ACTOR_TBL(08742100) = {
    (u32)BrontoBurtChase,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtChaseUpdate */
u32 gBrontoBurtChaseStateUpdates[1] ACTOR_TBL(08742100) = {
    (u32)BrontoBurtChaseState0Update,
};

/* ---- 0x0874212C-0x08742144: 2 table(s), 6 function pointer(s), section .actor_tbl_0874212c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in BrontoBurtTakeOffInit, BrontoBurtTakeOffEnterState */
u32 gBrontoBurtTakeOffStates[3] ACTOR_TBL(0874212c) = {
    (u32)BrontoBurtTakeOffWait,
    (u32)BrontoBurtTakeOff,
    (u32)BrontoBurtTakeOffState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in BrontoBurtTakeOffUpdate */
u32 gBrontoBurtTakeOffStateUpdates[3] ACTOR_TBL(0874212c) = {
    (u32)BrontoBurtTakeOffWaitUpdate,
    (u32)BrontoBurtTakeOffState1Update,
    (u32)BrontoBurtTakeOffState2Update,
};

/* ---- 0x08742570-0x08742598: 1 table(s), 10 function pointer(s), section .actor_tbl_08742570 ---- */
/* include/enemy.h; CallTableEntry(i, 10, ...) in Task_Twizzy, TwizzyEnterVariant */
u32 gTwizzyVariants[10] ACTOR_TBL(08742570) = {
    (u32)TwizzyWaveInit,
    (u32)TwizzyWeaveInit,
    (u32)TwizzySwoopInit,
    (u32)TwizzyDiagonalInit,
    (u32)TwizzyChaseInit,
    (u32)TwizzyTakeOffInit,
    (u32)TwizzyVariant6,
    (u32)TwizzyVariant7,
    (u32)TwizzyHoverInit,
    (u32)TwizzyIdle,
};

/* ---- 0x087425B0-0x087425B8: 2 table(s), 2 function pointer(s), section .actor_tbl_087425b0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWaveInit, TwizzyWaveEnterState */
u32 gTwizzyWaveStates[1] ACTOR_TBL(087425b0) = {
    (u32)TwizzyWave,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWaveUpdate */
u32 gTwizzyWaveStateUpdates[1] ACTOR_TBL(087425b0) = {
    (u32)TwizzyWaveState0Update,
};

/* ---- 0x087425D0-0x087425DC: 3 table(s), 3 function pointer(s), section .actor_tbl_087425d0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWeaveInit, TwizzyWeaveEnterState */
u32 gTwizzyWeaveStates[1] ACTOR_TBL(087425d0) = {
    (u32)TwizzyWeave,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWeaveUpdate */
u32 gTwizzyWeaveStateUpdates[1] ACTOR_TBL(087425d0) = {
    (u32)sub_080873b0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzySwoopInit */
s32 gTwizzySwoopStates[1] ACTOR_TBL(087425d0) = {
    (s32)TwizzySwoop,
};

/* ---- 0x087425EC-0x087425F0: 1 table(s), 1 function pointer(s), section .actor_tbl_087425ec ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzySwoopUpdate */
u32 gTwizzySwoopStateUpdates[1] ACTOR_TBL(087425ec) = {
    (u32)TwizzySwoopState0Update,
};

/* ---- 0x087425F8-0x08742600: 2 table(s), 2 function pointer(s), section .actor_tbl_087425f8 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyDiagonalInit, TwizzyDiagonalEnterState */
u32 gTwizzyDiagonalStates[1] ACTOR_TBL(087425f8) = {
    (u32)TwizzyDiagonal,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyDiagonalUpdate */
u32 gTwizzyDiagonalStateUpdates[1] ACTOR_TBL(087425f8) = {
    (u32)TwizzyDiagonalState0Update,
};

/* ---- 0x0874260C-0x08742614: 2 table(s), 2 function pointer(s), section .actor_tbl_0874260c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyChaseInit, TwizzyChaseEnterState */
u32 gTwizzyChaseStates[1] ACTOR_TBL(0874260c) = {
    (u32)TwizzyChase,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyChaseUpdate */
u32 gTwizzyChaseStateUpdates[1] ACTOR_TBL(0874260c) = {
    (u32)TwizzyChaseState0Update,
};

/* ---- 0x0874263C-0x0874269C: 6 table(s), 24 function pointer(s), section .actor_tbl_0874263c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwizzyTakeOffInit, TwizzyTakeOffEnterState */
u32 gTwizzyTakeOffStates[3] ACTOR_TBL(0874263c) = {
    (u32)TwizzyTakeOffWait,
    (u32)TwizzyTakeOff,
    (u32)TwizzyTakeOffState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwizzyTakeOffUpdate */
u32 gTwizzyTakeOffStateUpdates[3] ACTOR_TBL(0874263c) = {
    (u32)TwizzyTakeOffWaitUpdate,
    (u32)TwizzyTakeOffState1Update,
    (u32)TwizzyTakeOffState2Update,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwizzyVariant6, sub_08087e60 */
u32 gUnk_08742654[3] ACTOR_TBL(0874263c) = {
    (u32)sub_08087eb4,
    (u32)sub_08087f18,
    (u32)sub_08087f8c,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08087e84 */
u32 gUnk_08742660[3] ACTOR_TBL(0874263c) = {
    (u32)sub_08087ef0,
    (u32)sub_08087f88,
    (u32)sub_08087fc8,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in TwizzyVariant7, sub_08088000 */
u32 gUnk_0874266C[6] ACTOR_TBL(0874263c) = {
    (u32)sub_08088054,
    (u32)sub_080881a0,
    (u32)sub_080881e4,
    (u32)sub_08088230,
    (u32)sub_0808827c,
    (u32)sub_08088320,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in sub_08088024 */
u32 gUnk_08742684[6] ACTOR_TBL(0874263c) = {
    (u32)sub_080880fc,
    (u32)sub_080881e0,
    (u32)sub_08088208,
    (u32)sub_08088254,
    (u32)sub_080882f0,
    (u32)sub_0808835c,
};

/* ---- 0x087426AC-0x087426EC: 5 table(s), 16 function pointer(s), section .actor_tbl_087426ac ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyHoverInit, TwizzyHoverEnterState */
u32 gTwizzyHoverStates[1] ACTOR_TBL(087426ac) = {
    (u32)TwizzyHover,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyHoverUpdate */
u32 gTwizzyHoverStateUpdates[1] ACTOR_TBL(087426ac) = {
    (u32)TwizzyHoverState0Update,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Squishy */
u32 gSquishyVariants[4] ACTOR_TBL(087426ac) = {
    (u32)SquishyWalkInit,
    (u32)SquishyVariant1,
    (u32)SquishyVariant2,
    (u32)SquishyIdle,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in SquishyWalkInit, SquishyWalkEnterState, SquishyVariant2 */
u32 gSquishyWalkStates[5] ACTOR_TBL(087426ac) = {
    (u32)SquishyWalk,
    (u32)SquishyWalkState1,
    (u32)SquishyWalkFall,
    (u32)SquishyWalkState3,
    (u32)SquishyWalkState4,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in SquishyWalkUpdate */
u32 gSquishyWalkStateUpdates[5] ACTOR_TBL(087426ac) = {
    (u32)SquishyWalkState0Update,
    (u32)SquishyWalkState1Update,
    (u32)SquishyWalkFallUpdate,
    (u32)SquishyWalkState3Update,
    (u32)SquishyWalkState4Update,
};

/* ---- 0x08742704-0x08742734: 4 table(s), 12 function pointer(s), section .actor_tbl_08742704 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in SquishyVariant1, sub_08088aec */
u32 gUnk_08742704[3] ACTOR_TBL(08742704) = {
    (u32)sub_08088b58,
    (u32)sub_08088bd8,
    (u32)sub_08088ca4,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08088b10 */
u32 gUnk_08742710[3] ACTOR_TBL(08742704) = {
    (u32)sub_08088b98,
    (u32)sub_08088ca0,
    (u32)sub_08088ce4,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SquishyVariant2, sub_08088d58 */
u32 gUnk_0874271C[3] ACTOR_TBL(08742704) = {
    (u32)sub_08088dac,
    (u32)sub_08088efc,
    (u32)sub_08088ea4,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08088d7c */
u32 gUnk_08742728[3] ACTOR_TBL(08742704) = {
    (u32)sub_08088e78,
    (u32)sub_08088f24,
    (u32)sub_08088ed4,
};

/* ---- 0x0874273C-0x0874276C: 3 table(s), 12 function pointer(s), section .actor_tbl_0874273c ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Bubbles */
u32 gBubblesVariants[2] ACTOR_TBL(0874273c) = {
    (u32)BubblesInit,
    (u32)BubblesIdle,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in BubblesInit, BubblesEnterState */
u32 gBubblesStates[5] ACTOR_TBL(0874273c) = {
    (u32)BubblesJump,
    (u32)BubblesBounceOffWall,
    (u32)BubblesBounceOffCeiling,
    (u32)BubblesLand,
    (u32)BubblesFall,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in BubblesUpdate */
u32 gBubblesStateUpdates[5] ACTOR_TBL(0874273c) = {
    (u32)BubblesJumpUpdate,
    (u32)BubblesBounceOffWallUpdate,
    (u32)sub_08089530,
    (u32)BubblesLandUpdate,
    (u32)BubblesFallUpdate,
};

/* ---- 0x08742798-0x087427B0: 3 table(s), 6 function pointer(s), section .actor_tbl_08742798 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Glunk */
u32 gGlunkVariants[2] ACTOR_TBL(08742798) = {
    (u32)GlunkInit,
    (u32)GlunkIdle,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in GlunkInit, GlunkEnterState */
u32 gGlunkStates[2] ACTOR_TBL(08742798) = {
    (u32)GlunkWait,
    (u32)GlunkShoot,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in GlunkUpdate */
u32 gGlunkStateUpdates[2] ACTOR_TBL(08742798) = {
    (u32)GlunkWaitUpdate,
    (u32)GlunkShootUpdate,
};

/* ---- 0x087427B4-0x08742814: 3 table(s), 24 function pointer(s), section .actor_tbl_087427b4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Slippy */
u32 gSlippyVariants[2] ACTOR_TBL(087427b4) = {
    (u32)SlippyInit,
    (u32)SlippyIdle,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in SlippyInit, SlippyEnterState */
u32 gSlippyStates[11] ACTOR_TBL(087427b4) = {
    (u32)SlippyState0,
    (u32)SlippyState1,
    (u32)SlippyState2,
    (u32)SlippyState3,
    (u32)SlippyState4,
    (u32)SlippyState5,
    (u32)SlippyState6,
    (u32)SlippyState7,
    (u32)SlippyState8,
    (u32)SlippyState9,
    (u32)SlippyFall,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in SlippyUpdate */
u32 gSlippyStateUpdates[11] ACTOR_TBL(087427b4) = {
    (u32)SlippyState0Update,
    (u32)SlippyState1Update,
    (u32)SlippyState2Update,
    (u32)SlippyState3Update,
    (u32)SlippyState4Update,
    (u32)SlippyState5Update,
    (u32)SlippyState6Update,
    (u32)SlippyState7Update,
    (u32)SlippyState8Update,
    (u32)SlippyState9Update,
    (u32)SlippyFallUpdate,
};

/* ---- 0x087428C0-0x087428E8: 3 table(s), 10 function pointer(s), section .actor_tbl_087428c0 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in Task_Blipper */
u32 gBlipperVariants[6] ACTOR_TBL(087428c0) = {
    (u32)BlipperChaseInit,
    (u32)BlipperWaveInit,
    (u32)sub_0808af34,
    (u32)sub_0808b1d0,
    (u32)sub_0808b1d0,
    (u32)BlipperIdle,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in BlipperChaseInit, BlipperChaseEnterState */
u32 gBlipperChaseStates[2] ACTOR_TBL(087428c0) = {
    (u32)BlipperChase,
    (u32)sub_0808acdc,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in BlipperChaseUpdate */
u32 gBlipperChaseStateUpdates[2] ACTOR_TBL(087428c0) = {
    (u32)sub_0808abf4,
    (u32)sub_0808ad20,
};

/* ---- 0x08742908-0x08742930: 6 table(s), 10 function pointer(s), section .actor_tbl_08742908 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BlipperWaveInit, BlipperWaveEnterState */
u32 gBlipperWaveStates[1] ACTOR_TBL(08742908) = {
    (u32)BlipperWave,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BlipperWaveUpdate */
u32 gBlipperWaveStateUpdates[1] ACTOR_TBL(08742908) = {
    (u32)sub_0808aeec,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808af34, sub_0808afac */
u32 gUnk_08742910[2] ACTOR_TBL(08742908) = {
    (u32)sub_0808b008,
    (u32)sub_0808b120,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808afd0 */
u32 gUnk_08742918[2] ACTOR_TBL(08742908) = {
    (u32)sub_0808b094,
    (u32)sub_0808b1a8,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808b1d0, sub_0808b210 */
u32 gUnk_08742920[2] ACTOR_TBL(08742908) = {
    (u32)sub_0808b28c,
    (u32)sub_0808b368,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808b234 */
u32 gUnk_08742928[2] ACTOR_TBL(08742908) = {
    (u32)sub_0808b2fc,
    (u32)sub_0808b468,
};

/* ---- 0x08742940-0x08742998: 3 table(s), 22 function pointer(s), section .actor_tbl_08742940 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Gip */
u32 gGipVariants[2] ACTOR_TBL(08742940) = {
    (u32)GipInit,
    (u32)GipIdle,
};
/* include/enemy.h; CallTableEntry(i, 12, ...) in GipInit, GipEnterState */
u32 gGipStates[12] ACTOR_TBL(08742940) = {
    (u32)GipWalk,
    (u32)GipWalk,
    (u32)GipWalk,
    (u32)GipWait,
    (u32)GipState4,
    (u32)GipState5,
    (u32)GipState6,
    (u32)GipState7,
    (u32)GipState8,
    (u32)GipState9,
    (u32)GipJump,
    (u32)GipShoot,
};
/* include/enemy.h; CallTableEntry(i, 8, ...) in GipUpdate */
u32 gGipStateUpdates[8] ACTOR_TBL(08742940) = {
    (u32)GipWalkUpdate,
    (u32)GipWaitUpdate,
    (u32)sub_0808c1d0,
    (u32)sub_0808c3e8,
    (u32)sub_0808c4bc,
    (u32)sub_0808c538,
    (u32)GipJumpUpdate,
    (u32)sub_0808c684,
};

/* ---- 0x08743188-0x087431CC: 7 table(s), 17 function pointer(s), section .actor_tbl_08743188 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_BroomHatter */
u32 gBroomHatterVariants[3] ACTOR_TBL(08743188) = {
    (u32)BroomHatterVariant0,
    (u32)BroomHatterVariant1,
    (u32)BroomHatterIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in BroomHatterVariant0, sub_0808d624 */
u32 gUnk_08743194[3] ACTOR_TBL(08743188) = {
    (u32)sub_0808d640,
    (u32)sub_0808d790,
    (u32)sub_0808d964,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0808d58c */
u32 gUnk_087431A0[3] ACTOR_TBL(08743188) = {
    (u32)sub_0808d764,
    (u32)sub_0808d938,
    (u32)sub_0808d9fc,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in BroomHatterVariant1, sub_0808dacc */
u32 gUnk_087431AC[3] ACTOR_TBL(08743188) = {
    (u32)sub_0808dae8,
    (u32)sub_0808dc94,
    (u32)sub_0808debc,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0808da34 */
u32 gUnk_087431B8[3] ACTOR_TBL(08743188) = {
    (u32)sub_0808dc68,
    (u32)sub_0808de90,
    (u32)sub_0808df54,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BroomHatterIdleInit */
u32 gBroomHatterIdleStates[1] ACTOR_TBL(08743188) = {
    (u32)BroomHatterIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BroomHatterIdleUpdate */
u32 gBroomHatterIdleStateUpdates[1] ACTOR_TBL(08743188) = {
    (u32)BroomHatterIdleState0Update,
};

/* ---- 0x087431E4-0x08743214: 5 table(s), 12 function pointer(s), section .actor_tbl_087431e4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_LaserBall */
u32 gLaserBallVariants[2] ACTOR_TBL(087431e4) = {
    (u32)LaserBallInit,
    (u32)LaserBallIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in LaserBallInit, LaserBallEnterState */
u32 gLaserBallStates[4] ACTOR_TBL(087431e4) = {
    (u32)LaserBallApproach,
    (u32)LaserBallShoot,
    (u32)LaserBallHover,
    (u32)LaserBallRetreat,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in LaserBallUpdate */
u32 gLaserBallStateUpdates[4] ACTOR_TBL(087431e4) = {
    (u32)LaserBallApproachUpdate,
    (u32)LaserBallShootUpdate,
    (u32)LaserBallHoverUpdate,
    (u32)LaserBallRetreatUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in LaserBallIdleInit */
u32 gLaserBallIdleStates[1] ACTOR_TBL(087431e4) = {
    (u32)LaserBallIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in LaserBallIdleUpdate */
u32 gLaserBallIdleStateUpdates[1] ACTOR_TBL(087431e4) = {
    (u32)LaserBallIdleState0Update,
};

/* ---- 0x08743224-0x08743248: 5 table(s), 9 function pointer(s), section .actor_tbl_08743224 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Coconut */
u32 gCoconutVariants[3] ACTOR_TBL(08743224) = {
    (u32)CoconutInit,
    (u32)CoconutInit,
    (u32)CoconutIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in CoconutInit, CoconutEnterState */
u32 gCoconutStates[3] ACTOR_TBL(08743224) = {
    (u32)CoconutWait,
    (u32)CoconutFall,
    (u32)CoconutExplode,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutUpdate */
u32 gCoconutStateUpdates[1] ACTOR_TBL(08743224) = {
    (u32)CoconutWaitUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutIdleInit */
u32 gCoconutIdleStates[1] ACTOR_TBL(08743224) = {
    (u32)CoconutIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutIdleUpdate */
u32 gCoconutIdleStateUpdates[1] ACTOR_TBL(08743224) = {
    (u32)CoconutIdleState0Update,
};

/* ---- 0x08743284-0x087432EC: 9 table(s), 26 function pointer(s), section .actor_tbl_08743284 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in Task_Shotzo */
u32 gShotzoVariants[6] ACTOR_TBL(08743284) = {
    (u32)ShotzoAimInit,
    (u32)ShotzoFixedInit,
    (u32)ShotzoFixedInit,
    (u32)ShotzoFixedInit,
    (u32)ParasolShotzoInit,
    (u32)ShotzoIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoAimInit, ShotzoAimEnterState */
u32 gShotzoAimStates[3] ACTOR_TBL(08743284) = {
    (u32)ShotzoAim,
    (u32)ShotzoAimFall,
    (u32)ShotzoAimShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoAimUpdate */
u32 gShotzoAimStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)ShotzoAimState0Update,
    (u32)ShotzoAimFallUpdate,
    (u32)ShotzoAimShootUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoFixedInit, ShotzoFixedEnterState */
u32 gShotzoFixedStates[3] ACTOR_TBL(08743284) = {
    (u32)ShotzoFixedState0,
    (u32)ShotzoFixedFall,
    (u32)ShotzoFixedShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoFixedUpdate */
u32 gShotzoFixedStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)ShotzoFixedState0Update,
    (u32)ShotzoFixedFallUpdate,
    (u32)ShotzoFixedShootUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ParasolShotzoInit, ParasolShotzoEnterState */
u32 gParasolShotzoStates[3] ACTOR_TBL(08743284) = {
    (u32)ParasolShotzoAim,
    (u32)ParasolShotzoShoot,
    (u32)ParasolShotzoState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ParasolShotzoUpdate */
u32 gParasolShotzoStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)ParasolShotzoAimUpdate,
    (u32)ParasolShotzoShootUpdate,
    (u32)ParasolShotzoState2Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoIdleInit */
u32 gShotzoIdleStates[1] ACTOR_TBL(08743284) = {
    (u32)ShotzoIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoIdleUpdate */
u32 gShotzoIdleStateUpdates[1] ACTOR_TBL(08743284) = {
    (u32)ShotzoIdleState0Update,
};

/* ---- 0x087432F4-0x0874330C: 3 table(s), 6 function pointer(s), section .actor_tbl_087432f4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Coner */
u32 gConerVariants[2] ACTOR_TBL(087432f4) = {
    (u32)ConerInit,
    (u32)ConerIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ConerInit, ConerEnterState */
u32 gConerStates[3] ACTOR_TBL(087432f4) = {
    (u32)ConerWalk,
    (u32)ConerState1,
    (u32)ConerState2,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ConerIdleInit */
u32 gConerIdleStates[1] ACTOR_TBL(087432f4) = {
    (u32)ConerIdle,
};

/* ---- 0x08743600-0x08743614: 3 table(s), 5 function pointer(s), section .actor_tbl_08743600 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_LaserBallLaser */
u32 gLaserBallLaserVariants[1] ACTOR_TBL(08743600) = {
    (u32)LaserBallLaserInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in LaserBallLaserInit, LaserBallLaserEnterState */
u32 gLaserBallLaserStates[2] ACTOR_TBL(08743600) = {
    (u32)LaserBallLaserState0,
    (u32)LaserBallLaserState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in LaserBallLaserUpdate */
u32 gLaserBallLaserStateUpdates[2] ACTOR_TBL(08743600) = {
    (u32)LaserBallLaserState0Update,
    (u32)LaserBallLaserState1Update,
};

/* ---- 0x0874362C-0x08743644: 3 table(s), 6 function pointer(s), section .actor_tbl_0874362c ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_ShotzoCannonball */
u32 gShotzoCannonballVariants[4] ACTOR_TBL(0874362c) = {
    (u32)ShotzoCannonballInit,
    (u32)ShotzoCannonballInit,
    (u32)ShotzoCannonballInit,
    (u32)ShotzoCannonballInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoCannonballInit, ShotzoCannonballEnterState */
u32 gShotzoCannonballStates[1] ACTOR_TBL(0874362c) = {
    (u32)ShotzoCannonballState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoCannonballUpdate */
u32 gShotzoCannonballStateUpdates[1] ACTOR_TBL(0874362c) = {
    (u32)ShotzoCannonballState0Update,
};

/* ---- 0x08743848-0x087438A4: 3 table(s), 23 function pointer(s), section .actor_tbl_08743848 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Bonkers */
u32 gBonkersVariants[1] ACTOR_TBL(08743848) = {
    (u32)BonkersInit,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in BonkersInit, BonkersEnterState */
u32 gBonkersStates[11] ACTOR_TBL(08743848) = {
    (u32)BonkersIntro,
    (u32)BonkersWalk,
    (u32)BonkersJump,
    (u32)BonkersHop,
    (u32)BonkersDash,
    (u32)BonkersThrow,
    (u32)BonkersSlam,
    (u32)BonkersJumpSlam,
    (u32)BonkersTripleSlam,
    (u32)BonkersBounceOffWall,
    (u32)BonkersDefeat,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in BonkersUpdate */
u32 gBonkersStateUpdates[11] ACTOR_TBL(08743848) = {
    (u32)BonkersIntroUpdate,
    (u32)BonkersWalkUpdate,
    (u32)BonkersJumpUpdate,
    (u32)BonkersHopUpdate,
    (u32)BonkersDashUpdate,
    (u32)BonkersThrowUpdate,
    (u32)BonkersSlamUpdate,
    (u32)BonkersJumpSlamUpdate,
    (u32)BonkersTripleSlamUpdate,
    (u32)BonkersBounceOffWallUpdate,
    (u32)BonkersDefeatUpdate,
};

/* ---- 0x08743984-0x087439C0: 3 table(s), 15 function pointer(s), section .actor_tbl_08743984 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_PoppyBrosSr */
u32 gPoppyBrosSrVariants[1] ACTOR_TBL(08743984) = {
    (u32)PoppyBrosSrInit,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in PoppyBrosSrInit, PoppyBrosSrEnterState */
u32 gPoppyBrosSrStates[7] ACTOR_TBL(08743984) = {
    (u32)PoppyBrosSrIntro,
    (u32)PoppyBrosSrState1,
    (u32)PoppyBrosSrState2,
    (u32)PoppyBrosSrState3,
    (u32)PoppyBrosSrState4,
    (u32)PoppyBrosSrState5,
    (u32)PoppyBrosSrDefeat,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in PoppyBrosSrUpdate */
u32 gPoppyBrosSrStateUpdates[7] ACTOR_TBL(08743984) = {
    (u32)PoppyBrosSrIntroUpdate,
    (u32)PoppyBrosSrState1Update,
    (u32)PoppyBrosSrState2Update,
    (u32)PoppyBrosSrState3Update,
    (u32)PoppyBrosSrState4Update,
    (u32)PoppyBrosSrState5Update,
    (u32)PoppyBrosSrDefeatUpdate,
};

/* ---- 0x08743ADC-0x08743B48: 3 table(s), 27 function pointer(s), section .actor_tbl_08743adc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Bugzzy */
u32 gBugzzyVariants[1] ACTOR_TBL(08743adc) = {
    (u32)BugzzyInit,
};
/* include/enemy.h; CallTableEntry(i, 13, ...) in BugzzyInit, BugzzyEnterState */
u32 gBugzzyStates[13] ACTOR_TBL(08743adc) = {
    (u32)BugzzyIntro,
    (u32)BugzzyWalk,
    (u32)BugzzySummon,
    (u32)BugzzyCharge,
    (u32)BugzzyState4,
    (u32)BugzzyState5,
    (u32)BugzzyState6,
    (u32)BugzzyHop,
    (u32)BugzzyFall,
    (u32)BugzzyBounceOffWall,
    (u32)BugzzyBackdrop,
    (u32)BugzzyState11,
    (u32)BugzzyDefeat,
};
/* include/enemy.h; CallTableEntry(i, 13, ...) in BugzzyUpdate */
u32 gBugzzyStateUpdates[13] ACTOR_TBL(08743adc) = {
    (u32)BugzzyIntroUpdate,
    (u32)BugzzyWalkUpdate,
    (u32)BugzzySummonUpdate,
    (u32)BugzzyChargeUpdate,
    (u32)BugzzyState4Update,
    (u32)BugzzyState5Update,
    (u32)BugzzyState6Update,
    (u32)BugzzyHopUpdate,
    (u32)BugzzyFallUpdate,
    (u32)BugzzyBounceOffWallUpdate,
    (u32)BugzzyBackdropUpdate,
    (u32)BugzzyState11Update,
    (u32)BugzzyDefeatUpdate,
};

/* ---- 0x08744170-0x0874417C: 3 table(s), 3 function pointer(s), section .actor_tbl_08744170 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_BonkersNut */
u32 gBonkersNutVariants[1] ACTOR_TBL(08744170) = {
    (u32)BonkersNutInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BonkersNutInit */
u32 gBonkersNutStates[1] ACTOR_TBL(08744170) = {
    (u32)BonkersNutState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BonkersNutUpdate */
u32 gBonkersNutStateUpdates[1] ACTOR_TBL(08744170) = {
    (u32)BonkersNutState0Update,
};

/* ---- 0x087441A4-0x087441BC: 3 table(s), 6 function pointer(s), section .actor_tbl_087441a4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_PoppyBrosSrBomb */
u32 gPoppyBrosSrBombVariants[2] ACTOR_TBL(087441a4) = {
    (u32)PoppyBrosSrBombInit,
    (u32)PoppyBrosSrBombInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in PoppyBrosSrBombInit, PoppyBrosSrBombEnterState */
u32 gPoppyBrosSrBombStates[2] ACTOR_TBL(087441a4) = {
    (u32)PoppyBrosSrBombHeld,
    (u32)PoppyBrosSrBombFlight,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in PoppyBrosSrBombUpdate */
u32 gPoppyBrosSrBombStateUpdates[2] ACTOR_TBL(087441a4) = {
    (u32)PoppyBrosSrBombHeldUpdate,
    (u32)PoppyBrosSrBombFlightUpdate,
};

/* ---- 0x087441CC-0x087441D8: 3 table(s), 3 function pointer(s), section .actor_tbl_087441cc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_BugzzyLadybug */
u32 gBugzzyLadybugVariants[1] ACTOR_TBL(087441cc) = {
    (u32)BugzzyLadybugInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BugzzyLadybugInit */
u32 gBugzzyLadybugStates[1] ACTOR_TBL(087441cc) = {
    (u32)BugzzyLadybugState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BugzzyLadybugUpdate */
u32 gBugzzyLadybugStateUpdates[1] ACTOR_TBL(087441cc) = {
    (u32)BugzzyLadybugState0Update,
};

/* ---- 0x08744440-0x0874449C: 3 table(s), 23 function pointer(s), section .actor_tbl_08744440 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_GrandWheelie */
u32 gGrandWheelieVariants[1] ACTOR_TBL(08744440) = {
    (u32)GrandWheelieInit,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in GrandWheelieInit, GrandWheelieEnterState */
u32 gGrandWheelieStates[11] ACTOR_TBL(08744440) = {
    (u32)GrandWheelieFall,
    (u32)GrandWheelieState1,
    (u32)GrandWheelieState2,
    (u32)GrandWheelieHop,
    (u32)GrandWheelieCharge,
    (u32)GrandWheelieState5,
    (u32)GrandWheelieState6,
    (u32)GrandWheelieState7,
    (u32)GrandWheelieSummon,
    (u32)GrandWheelieBounceOffWall,
    (u32)GrandWheelieDefeat,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in sub_080942b4, GrandWheelieUpdate */
u32 gGrandWheelieStateUpdates[11] ACTOR_TBL(08744440) = {
    (u32)GrandWheelieFallUpdate,
    (u32)GrandWheelieState1Update,
    (u32)GrandWheelieState2Update,
    (u32)GrandWheelieHopUpdate,
    (u32)GrandWheelieChargeUpdate,
    (u32)GrandWheelieState5Update,
    (u32)GrandWheelieState6Update,
    (u32)GrandWheelieState7Update,
    (u32)GrandWheelieSummonUpdate,
    (u32)GrandWheelieBounceOffWallUpdate,
    (u32)GrandWheelieDefeatUpdate,
};

/* ---- 0x087444E4-0x08744510: 1 table(s), 11 function pointer(s), section .actor_tbl_087444e4 ---- */
/* include/enemy.h; CallTableEntry(i, 12, ...) in FireLionEnterState: the bound exceeds the 11 entries, so index 11 would read the next label, gUnk_08744510 */
u32 gFireLionStates[11] ACTOR_TBL(087444e4) = {
    (u32)FireLionDropIn,
    (u32)sub_080959ec,
    (u32)FireLionWait,
    (u32)FireLionHop,
    (u32)FireLionHop,
    (u32)sub_08095be8,
    (u32)sub_08095be8,
    (u32)FireLionSlash,
    (u32)FireLionPounce,
    (u32)FireLionState9,
    (u32)FireLionDefeat,
};

/* ---- 0x08744564-0x08744598: 1 table(s), 13 function pointer(s), section .actor_tbl_08744564 ---- */
/* include/enemy.h; CallTableEntry(i, 13, ...) in sub_0809699c, FireLionUpdate */
u32 gFireLionStateUpdates[13] ACTOR_TBL(08744564) = {
    (u32)FireLionDropInUpdate,
    (u32)sub_08095ad0,
    (u32)FireLionHopUpdate,
    (u32)sub_08095d20,
    (u32)FireLionSlashUpdate,
    (u32)FireLionPounceUpdate,
    (u32)FireLionChargeUpdate,
    (u32)sub_080962ac,
    (u32)sub_0809616c,
    (u32)sub_080963c0,
    (u32)sub_08096920,
    (u32)FireLionDefeatUpdate,
    (u32)sub_08096a28,
};

/* ---- 0x0874489C-0x087448E4: 2 table(s), 18 function pointer(s), section .actor_tbl_0874489c ---- */
/* include/enemy.h; CallTableEntry(i, 9, ...) in PhanPhanEnterState */
u32 gPhanPhanStates[9] ACTOR_TBL(0874489c) = {
    (u32)PhanPhanDropIn,
    (u32)sub_0809773c,
    (u32)PhanPhanHop,
    (u32)PhanPhanCharge,
    (u32)PhanPhanBounceOffWall,
    (u32)PhanPhanJump,
    (u32)PhanPhanThrowPlayer,
    (u32)PhanPhanThrowApple,
    (u32)PhanPhanDefeat,
};
/* include/enemy.h; CallTableEntry(i, 9, ...) in sub_080975c8, PhanPhanUpdate */
u32 gPhanPhanStateUpdates[9] ACTOR_TBL(0874489c) = {
    (u32)PhanPhanDropInUpdate,
    (u32)PhanPhanState1Update,
    (u32)PhanPhanHopUpdate,
    (u32)PhanPhanChargeUpdate,
    (u32)PhanPhanBounceOffWallUpdate,
    (u32)PhanPhanJumpUpdate,
    (u32)PhanPhanThrowPlayerUpdate,
    (u32)PhanPhanThrowAppleUpdate,
    (u32)PhanPhanDefeatUpdate,
};

/* ---- 0x08745630-0x087456CC: 3 table(s), 39 function pointer(s), section .actor_tbl_08745630 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrFrosty */
u32 gMrFrostyVariants[1] ACTOR_TBL(08745630) = {
    (u32)MrFrostyInit,
};
/* include/enemy.h; CallTableEntry(i, 19, ...) in MrFrostyInit, MrFrostyEnterState */
u32 gMrFrostyStates[19] ACTOR_TBL(08745630) = {
    (u32)MrFrostyWait,
    (u32)MrFrostyHop,
    (u32)MrFrostyWalkBack,
    (u32)MrFrostyDash,
    (u32)MrFrostyBounceOffWall,
    (u32)MrFrostyState5,
    (u32)MrFrostyState6,
    (u32)MrFrostyState7,
    (u32)MrFrostySpin,
    (u32)MrFrostyState9,
    (u32)MrFrostyState10,
    (u32)MrFrostyState11,
    (u32)MrFrostyState12,
    (u32)MrFrostyDefeat,
    (u32)MrFrostyState14,
    (u32)MrFrostyState15,
    (u32)MrFrostyState16,
    (u32)MrFrostyState17,
    (u32)MrFrostyDropIn,
};
/* include/enemy.h; CallTableEntry(i, 19, ...) in MrFrostyUpdate */
u32 gMrFrostyStateUpdates[19] ACTOR_TBL(08745630) = {
    (u32)MrFrostyWaitUpdate,
    (u32)MrFrostyHopUpdate,
    (u32)MrFrostyWalkBackUpdate,
    (u32)MrFrostyDashUpdate,
    (u32)MrFrostyBounceOffWallUpdate,
    (u32)MrFrostyState5Update,
    (u32)MrFrostyState6Update,
    (u32)MrFrostyState7Update,
    (u32)MrFrostySpinUpdate,
    (u32)MrFrostyState9Update,
    (u32)MrFrostyState10Update,
    (u32)MrFrostyState11Update,
    (u32)MrFrostyState12Update,
    (u32)MrFrostyDefeatUpdate,
    (u32)MrFrostyState14Update,
    (u32)MrFrostyState15Update,
    (u32)MrFrostyState16Update,
    (u32)MrFrostyState17Update,
    (u32)MrFrostyDropInUpdate,
};

/* ---- 0x0874574C-0x08745810: 3 table(s), 49 function pointer(s), section .actor_tbl_0874574c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTock */
u32 gMrTickTockVariants[1] ACTOR_TBL(0874574c) = {
    (u32)MrTickTockInit,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MrTickTockInit, MrTickTockEnterState */
u32 gMrTickTockStates[24] ACTOR_TBL(0874574c) = {
    (u32)MrTickTockWait,
    (u32)MrTickTockPickMove,
    (u32)MrTickTockHop,
    (u32)MrTickTockWalkBack,
    (u32)MrTickTockJumpForward,
    (u32)MrTickTockState5,
    (u32)MrTickTockDash,
    (u32)MrTickTockState7,
    (u32)MrTickTockState8,
    (u32)MrTickTockState9,
    (u32)MrTickTockJumpBack,
    (u32)MrTickTockShootNotes,
    (u32)MrTickTockState12,
    (u32)MrTickTockState13,
    (u32)MrTickTockState14,
    (u32)MrTickTockBounceOffWall,
    (u32)MrTickTockState16,
    (u32)MrTickTockState17,
    (u32)MrTickTockState18,
    (u32)MrTickTockDefeat,
    (u32)MrTickTockState20,
    (u32)MrTickTockState21,
    (u32)MrTickTockState22,
    (u32)MrTickTockDropIn,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MrTickTockUpdate */
u32 gMrTickTockStateUpdates[24] ACTOR_TBL(0874574c) = {
    (u32)MrTickTockWaitUpdate,
    (u32)MrTickTockPickMoveUpdate,
    (u32)MrTickTockHopUpdate,
    (u32)MrTickTockWalkBackUpdate,
    (u32)MrTickTockJumpForwardUpdate,
    (u32)MrTickTockState5Update,
    (u32)MrTickTockDashUpdate,
    (u32)MrTickTockState7Update,
    (u32)MrTickTockState8Update,
    (u32)MrTickTockState9Update,
    (u32)MrTickTockJumpBackUpdate,
    (u32)MrTickTockShootNotesUpdate,
    (u32)MrTickTockState12Update,
    (u32)MrTickTockState13Update,
    (u32)MrTickTockState14Update,
    (u32)MrTickTockBounceOffWallUpdate,
    (u32)MrTickTockState16Update,
    (u32)MrTickTockState17Update,
    (u32)sub_0809b210,
    (u32)MrTickTockDefeatUpdate,
    (u32)MrTickTockState20Update,
    (u32)MrTickTockState21Update,
    (u32)MrTickTockState22Update,
    (u32)MrTickTockDropInUpdate,
};

/* ---- 0x08745AE4-0x08745B0C: 6 table(s), 10 function pointer(s), section .actor_tbl_08745ae4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrFrostyIceCube */
u32 gMrFrostyIceCubeVariants[1] ACTOR_TBL(08745ae4) = {
    (u32)MrFrostyIceCubeInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in MrFrostyIceCubeInit, MrFrostyIceCubeEnterState */
u32 gMrFrostyIceCubeStates[3] ACTOR_TBL(08745ae4) = {
    (u32)MrFrostyIceCubeState0,
    (u32)MrFrostyIceCubeFlight,
    (u32)MrFrostyIceCubeState2,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in MrFrostyIceCubeUpdate */
u32 gMrFrostyIceCubeStateUpdates[3] ACTOR_TBL(08745ae4) = {
    (u32)MrFrostyIceCubeState0Update,
    (u32)MrFrostyIceCubeFlightUpdate,
    (u32)MrFrostyIceCubeState2Update,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTockRing */
u32 gMrTickTockRingVariants[1] ACTOR_TBL(08745ae4) = {
    (u32)MrTickTockRingInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in MrTickTockRingInit, MrTickTockRingEnterState */
u32 gMrTickTockRingStates[1] ACTOR_TBL(08745ae4) = {
    (u32)MrTickTockRingState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in MrTickTockRingUpdate */
u32 gMrTickTockRingStateUpdates[1] ACTOR_TBL(08745ae4) = {
    (u32)MrTickTockRingState0Update,
};

/* ---- 0x08745B1C-0x08745B30: 3 table(s), 5 function pointer(s), section .actor_tbl_08745b1c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTockNote */
u32 gMrTickTockNoteVariants[1] ACTOR_TBL(08745b1c) = {
    (u32)MrTickTockNoteInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrTickTockNoteInit, MrTickTockNoteEnterState */
u32 gMrTickTockNoteStates[2] ACTOR_TBL(08745b1c) = {
    (u32)MrTickTockNoteFlight,
    (u32)MrTickTockNoteVanish,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrTickTockNoteUpdate */
u32 gMrTickTockNoteStateUpdates[2] ACTOR_TBL(08745b1c) = {
    (u32)MrTickTockNoteFlightUpdate,
    (u32)MrTickTockNoteVanishUpdate,
};

/* ---- 0x08747AA4-0x08747B10: 6 table(s), 27 function pointer(s), section .actor_tbl_08747aa4 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_0809c490 */
u32 gAxeKnightVariants[4] ACTOR_TBL(08747aa4) = {
    (u32)AxeKnightVariant0,
    (u32)AxeKnightVariant1,
    (u32)AxeKnightVariant2,
    (u32)AxeKnightVariant3,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in AxeKnightEnterState */
u32 gAxeKnightStates[5] ACTOR_TBL(08747aa4) = {
    (u32)AxeKnightWalk,
    (u32)AxeKnightSlash,
    (u32)AxeKnightThrow,
    (u32)AxeKnightJumpThrow,
    (u32)AxeKnightState4,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in AxeKnightUpdate */
u32 gAxeKnightStateUpdates[5] ACTOR_TBL(08747aa4) = {
    (u32)AxeKnightWalkUpdate,
    (u32)AxeKnightSlashUpdate,
    (u32)AxeKnightThrowUpdate,
    (u32)AxeKnightJumpThrowUpdate,
    (u32)AxeKnightState4Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0809d13c */
u32 gJavelinKnightVariants[2] ACTOR_TBL(08747aa4) = {
    (u32)JavelinKnightVariant0,
    (u32)JavelinKnightVariant1,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in JavelinKnightEnterState */
u32 gJavelinKnightStates[6] ACTOR_TBL(08747aa4) = {
    (u32)JavelinKnightState0,
    (u32)JavelinKnightHop,
    (u32)JavelinKnightState2,
    (u32)JavelinKnightJumpThrow,
    (u32)JavelinKnightJump,
    (u32)JavelinKnightState5,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in JavelinKnightUpdate */
u32 gJavelinKnightStateUpdates[5] ACTOR_TBL(08747aa4) = {
    (u32)JavelinKnightState0Update,
    (u32)sub_0809d42c,
    (u32)sub_0809d568,
    (u32)sub_0809d608,
    (u32)sub_0809d6b4,
};

/* ---- 0x08747BCC-0x08747C28: 7 table(s), 23 function pointer(s), section .actor_tbl_08747bcc ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0809dc7c */
u32 gMaceKnightVariants[3] ACTOR_TBL(08747bcc) = {
    (u32)MaceKnightVariant0,
    (u32)MaceKnightVariant1,
    (u32)MaceKnightVariant2,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MaceKnightEnterState */
u32 gMaceKnightStates[2] ACTOR_TBL(08747bcc) = {
    (u32)MaceKnightWalk,
    (u32)MaceKnightThrow,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MaceKnightUpdate */
u32 gMaceKnightStateUpdates[2] ACTOR_TBL(08747bcc) = {
    (u32)MaceKnightWalkUpdate,
    (u32)MaceKnightThrowUpdate,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_MaceKnightMace */
u32 gMaceKnightMaceVariants[3] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809e320,
    (u32)sub_0809e320,
    (u32)MaceKnightMaceVariant2,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_0809e824 */
u32 gTridentKnightVariants[4] ACTOR_TBL(08747bcc) = {
    (u32)TridentKnightVariant0,
    (u32)TridentKnightVariant1,
    (u32)TridentKnightVariant2,
    (u32)TridentKnightVariant3,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in TridentKnightEnterState */
u32 gTridentKnightStates[4] ACTOR_TBL(08747bcc) = {
    (u32)TridentKnightWalk,
    (u32)TridentKnightJump,
    (u32)TridentKnightThrow,
    (u32)TridentKnightJumpThrow,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in TridentKnightUpdate */
u32 gTridentKnightStateUpdates[5] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809ea08,
    (u32)TridentKnightJumpUpdate,
    (u32)TridentKnightThrowUpdate,
    (u32)TridentKnightJumpThrowUpdate,
    (u32)sub_0809ec80,
};

/* ---- 0x08747C6C-0x08747C80: 1 table(s), 5 function pointer(s), section .actor_tbl_08747c6c ---- */
/* include/enemy.h; CallTableEntry(i, 5, ...) in Task_TridentKnightTrident */
u32 gTridentKnightTridentVariants[5] ACTOR_TBL(08747c6c) = {
    (u32)TridentKnightTridentVariant0,
    (u32)TridentKnightTridentVariant1,
    (u32)TridentKnightTridentVariant2,
    (u32)TridentKnightTridentVariant3,
    (u32)TridentKnightTridentVariant4,
};

/* ---- 0x08748264-0x08748268: 1 table(s), 1 function pointer(s), section .actor_tbl_08748264 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDedede */
u32 gKingDededeVariants[1] ACTOR_TBL(08748264) = {
    (u32)KingDededeInit,
};

/* ---- 0x0874844C-0x087484A4: 2 table(s), 22 function pointer(s), section .actor_tbl_0874844c ---- */
/* include/enemy.h; CallTableEntry(i, 11, ...) in KingDededeInit, KingDededeEnterState */
u32 gKingDededeStates[11] ACTOR_TBL(0874844c) = {
    (u32)KingDededeIntro,
    (u32)KingDededeWait,
    (u32)KingDededeWalk,
    (u32)KingDededeJump,
    (u32)KingDededeFloat,
    (u32)KingDededeExhale,
    (u32)KingDededeHighJump,
    (u32)KingDededeSlam,
    (u32)KingDededeInhale,
    (u32)KingDededeSpit,
    (u32)KingDededeFall,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in KingDededeUpdate */
u32 gKingDededeStateUpdates[11] ACTOR_TBL(0874844c) = {
    (u32)KingDededeIntroUpdate,
    (u32)KingDededeWaitUpdate,
    (u32)KingDededeWalkUpdate,
    (u32)KingDededeJumpUpdate,
    (u32)KingDededeFloatUpdate,
    (u32)KingDededeExhaleUpdate,
    (u32)KingDededeHighJumpUpdate,
    (u32)KingDededeSlamUpdate,
    (u32)KingDededeInhaleUpdate,
    (u32)KingDededeSpitUpdate,
    (u32)KingDededeFallUpdate,
};

/* ---- 0x087484C4-0x087484E4: 3 table(s), 8 function pointer(s), section .actor_tbl_087484c4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in KingDededeDefeatedInit, KingDededeDefeatedEnterState */
u32 gKingDededeDefeatedStates[2] ACTOR_TBL(087484c4) = {
    (u32)KingDededeDefeatedFall,
    (u32)KingDededeDefeatedHoldBack,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KingDededeDefeatedUpdate */
u32 gKingDededeDefeatedStateUpdates[2] ACTOR_TBL(087484c4) = {
    (u32)KingDededeDefeatedFallUpdate,
    (u32)KingDededeDefeatedHoldBackUpdate,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_MrShineAndMrBright */
u32 gMrShineAndMrBrightVariants[4] ACTOR_TBL(087484c4) = {
    (u32)MrShineAndMrBrightInit,
    (u32)MrShineInit,
    (u32)MrBrightInit,
    (u32)MrShineAndMrBrightVariant3,
};

/* ---- 0x08748624-0x08748764: 6 table(s), 80 function pointer(s), section .actor_tbl_08748624 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in MrShineAndMrBrightInit, MrShineAndMrBrightEnterState */
u32 gMrShineAndMrBrightStates[4] ACTOR_TBL(08748624) = {
    (u32)MrShineAndMrBrightState0,
    (u32)MrShineAndMrBrightState1,
    (u32)MrShineAndMrBrightState2,
    (u32)MrShineAndMrBrightState3,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in MrShineAndMrBrightUpdate */
u32 gMrShineAndMrBrightStateUpdates[4] ACTOR_TBL(08748624) = {
    (u32)MrShineAndMrBrightState0Update,
    (u32)MrShineAndMrBrightState1Update,
    (u32)MrShineAndMrBrightState2Update,
    (u32)MrShineAndMrBrightState3Update,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrShineInit, MrShineEnterState */
u32 gMrShineStates[18] ACTOR_TBL(08748624) = {
    (u32)MrShineFall,
    (u32)MrShineWait,
    (u32)MrShineAscend,
    (u32)MrShineWaitToAttack,
    (u32)MrShineState4,
    (u32)MrShineDropStars,
    (u32)MrShineChase,
    (u32)MrShineWaitToDescend,
    (u32)MrShineDescend,
    (u32)MrShineWalk,
    (u32)MrShineJump,
    (u32)MrShineJumpBack,
    (u32)MrShineDash,
    (u32)MrShineRecoil,
    (u32)MrShineThrow,
    (u32)MrShineState15,
    (u32)MrShineState16,
    (u32)MrShineState17,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrShineUpdate */
u32 gMrShineStateUpdates[18] ACTOR_TBL(08748624) = {
    (u32)MrShineFallUpdate,
    (u32)MrShineWaitUpdate,
    (u32)MrShineAscendUpdate,
    (u32)MrShineWaitToAttackUpdate,
    (u32)MrShineState4Update,
    (u32)MrShineDropStarsUpdate,
    (u32)MrShineChaseUpdate,
    (u32)MrShineWaitToDescendUpdate,
    (u32)MrShineDescendUpdate,
    (u32)MrShineWalkUpdate,
    (u32)MrShineJumpUpdate,
    (u32)MrShineJumpBackUpdate,
    (u32)MrShineDashUpdate,
    (u32)MrShineRecoilUpdate,
    (u32)MrShineThrowUpdate,
    (u32)MrShineState15Update,
    (u32)sub_080a3ba0,
    (u32)sub_080a3c08,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrBrightInit, MrBrightEnterState */
u32 gMrBrightStates[18] ACTOR_TBL(08748624) = {
    (u32)MrBrightFall,
    (u32)MrBrightWait,
    (u32)MrBrightAscend,
    (u32)MrBrightWaitToAttack,
    (u32)MrBrightState4,
    (u32)MrBrightState5,
    (u32)MrBrightChase,
    (u32)MrBrightWaitToDescend,
    (u32)MrBrightDescend,
    (u32)MrBrightHop,
    (u32)MrBrightJump,
    (u32)MrBrightJumpBack,
    (u32)MrBrightDash,
    (u32)MrBrightRecoil,
    (u32)MrBrightThrow,
    (u32)MrBrightState15,
    (u32)MrBrightState16,
    (u32)MrBrightState17,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrBrightUpdate */
u32 gMrBrightStateUpdates[18] ACTOR_TBL(08748624) = {
    (u32)MrBrightFallUpdate,
    (u32)MrBrightWaitUpdate,
    (u32)MrBrightAscendUpdate,
    (u32)MrBrightWaitToAttackUpdate,
    (u32)MrBrightState4Update,
    (u32)MrBrightState5Update,
    (u32)MrBrightChaseUpdate,
    (u32)MrBrightWaitToDescendUpdate,
    (u32)MrBrightDescendUpdate,
    (u32)MrBrightHopUpdate,
    (u32)MrBrightJumpUpdate,
    (u32)MrBrightJumpBackUpdate,
    (u32)MrBrightDashUpdate,
    (u32)MrBrightRecoilUpdate,
    (u32)MrBrightThrowUpdate,
    (u32)MrBrightState15Update,
    (u32)sub_080a4658,
    (u32)sub_080a46c0,
};

/* ---- 0x087489B4-0x087489C0: 3 table(s), 3 function pointer(s), section .actor_tbl_087489b4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDededeStar */
u32 gKingDededeStarVariants[1] ACTOR_TBL(087489b4) = {
    (u32)KingDededeStarInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeStarInit, KingDededeStarEnterState */
u32 gKingDededeStarStates[1] ACTOR_TBL(087489b4) = {
    (u32)sub_080a4ac4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeStarUpdate */
u32 gKingDededeStarStateUpdates[1] ACTOR_TBL(087489b4) = {
    (u32)sub_080a4b1c,
};

/* ---- 0x087489D4-0x08748A00: 6 table(s), 11 function pointer(s), section .actor_tbl_087489d4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDededeAirPuff */
u32 gKingDededeAirPuffVariants[1] ACTOR_TBL(087489d4) = {
    (u32)KingDededeAirPuffInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeAirPuffInit, sub_080a4c20 */
u32 gKingDededeAirPuffStates[1] ACTOR_TBL(087489d4) = {
    (u32)KingDededeAirPuffState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeAirPuffUpdate */
u32 gKingDededeAirPuffStateUpdates[1] ACTOR_TBL(087489d4) = {
    (u32)KingDededeAirPuffState0Update,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_MrShineAndMrBrightAttack */
u32 gMrShineAndMrBrightAttackVariants[4] ACTOR_TBL(087489d4) = {
    (u32)MrShineCrescentInit,
    (u32)MrBrightFireballInit,
    (u32)MrShineFallingStarInit,
    (u32)MrBrightBeamInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrShineCrescentInit, MrShineCrescentEnterState */
u32 gMrShineCrescentStates[2] ACTOR_TBL(087489d4) = {
    (u32)MrShineCrescentState0,
    (u32)MrShineCrescentState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrShineCrescentUpdate */
u32 gMrShineCrescentStateUpdates[2] ACTOR_TBL(087489d4) = {
    (u32)MrShineCrescentState0Update,
    (u32)sub_080a4e10,
};

/* ---- 0x08748A28-0x08748A38: 2 table(s), 4 function pointer(s), section .actor_tbl_08748a28 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrShineFallingStarInit, sub_080a4f24 */
u32 gMrShineFallingStarStates[2] ACTOR_TBL(08748a28) = {
    (u32)MrShineFallingStarState0,
    (u32)MrShineFallingStarState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrShineFallingStarUpdate */
u32 gMrShineFallingStarStateUpdates[2] ACTOR_TBL(08748a28) = {
    (u32)MrShineFallingStarState0Update,
    (u32)sub_080a503c,
};

/* ---- 0x08748A54-0x08748A80: 5 table(s), 11 function pointer(s), section .actor_tbl_08748a54 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrBrightFireballInit, MrBrightFireballEnterState */
u32 gMrBrightFireballStates[2] ACTOR_TBL(08748a54) = {
    (u32)MrBrightFireballState0,
    (u32)MrBrightFireballState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrBrightFireballUpdate */
u32 gMrBrightFireballStateUpdates[2] ACTOR_TBL(08748a54) = {
    (u32)MrBrightFireballState0Update,
    (u32)sub_080a5184,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrBrightBeamInit, sub_080a5220 */
u32 gMrBrightBeamStates[2] ACTOR_TBL(08748a54) = {
    (u32)MrBrightBeamState0,
    (u32)MrBrightBeamState1,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MrBrightBeamUpdate */
u32 gMrBrightBeamStateUpdates[2] ACTOR_TBL(08748a54) = {
    (u32)MrBrightBeamState0Update,
    (u32)sub_080a5320,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_MrBrightBeamEffect */
u32 gMrBrightBeamEffectVariants[3] ACTOR_TBL(08748a54) = {
    (u32)MrBrightBeamEffectVariant0,
    (u32)MrBrightBeamEffectVariant1,
    (u32)MrBrightBeamEffectVariant2,
};

/* ---- 0x08748EB8-0x08748F8C: 5 table(s), 53 function pointer(s), section .actor_tbl_08748eb8 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MetaKnight */
u32 gMetaKnightVariants[1] ACTOR_TBL(08748eb8) = {
    (u32)MetaKnightInit,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MetaKnightInit, MetaKnightEnterState */
u32 gMetaKnightStates[24] ACTOR_TBL(08748eb8) = {
    (u32)MetaKnightIntro,
    (u32)MetaKnightFollow,
    (u32)MetaKnightState2,
    (u32)MetaKnightState3,
    (u32)MetaKnightState4,
    (u32)MetaKnightState5,
    (u32)MetaKnightApproach,
    (u32)MetaKnightRun,
    (u32)MetaKnightState8,
    (u32)MetaKnightState9,
    (u32)MetaKnightLand,
    (u32)MetaKnightState11,
    (u32)MetaKnightState12,
    (u32)MetaKnightSwordSpin,
    (u32)MetaKnightState14,
    (u32)MetaKnightDownThrust,
    (u32)MetaKnightState16,
    (u32)MetaKnightState17,
    (u32)MetaKnightState18,
    (u32)MetaKnightState19,
    (u32)MetaKnightDoubleSlash,
    (u32)MetaKnightState21,
    (u32)MetaKnightState22,
    (u32)MetaKnightState23,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MetaKnightUpdate */
u32 gMetaKnightStateUpdates[24] ACTOR_TBL(08748eb8) = {
    (u32)MetaKnightIntroUpdate,
    (u32)MetaKnightFollowUpdate,
    (u32)MetaKnightState2Update,
    (u32)MetaKnightState3Update,
    (u32)MetaKnightState4Update,
    (u32)MetaKnightState5Update,
    (u32)MetaKnightApproachUpdate,
    (u32)MetaKnightRunUpdate,
    (u32)MetaKnightState8Update,
    (u32)MetaKnightState9Update,
    (u32)MetaKnightLandUpdate,
    (u32)MetaKnightState11Update,
    (u32)MetaKnightState12Update,
    (u32)MetaKnightSwordSpinUpdate,
    (u32)MetaKnightState14Update,
    (u32)MetaKnightDownThrustUpdate,
    (u32)MetaKnightState16Update,
    (u32)MetaKnightState17Update,
    (u32)MetaKnightState18Update,
    (u32)MetaKnightState19Update,
    (u32)MetaKnightDoubleSlashUpdate,
    (u32)MetaKnightState21Update,
    (u32)MetaKnightState22Update,
    (u32)MetaKnightState23Update,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MetaKnightDefeatedInit, MetaKnightDefeatedEnterState */
u32 gMetaKnightDefeatedStates[2] ACTOR_TBL(08748eb8) = {
    (u32)sub_080a7438,
    (u32)sub_080a75c8,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MetaKnightDefeatedUpdate */
u32 gMetaKnightDefeatedStateUpdates[2] ACTOR_TBL(08748eb8) = {
    (u32)sub_080a75a0,
    (u32)sub_080a787c,
};

/* ---- 0x08749150-0x087491A0: 5 table(s), 20 function pointer(s), section .actor_tbl_08749150 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Kracko */
u32 gKrackoVariants[2] ACTOR_TBL(08749150) = {
    (u32)KrackoJrInit,
    (u32)KrackoInit,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KrackoJrInit, KrackoJrEnterState */
u32 gKrackoJrStates[2] ACTOR_TBL(08749150) = {
    (u32)KrackoJrState0,
    (u32)KrackoJrTransform,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KrackoJrUpdate */
u32 gKrackoJrStateUpdates[2] ACTOR_TBL(08749150) = {
    (u32)KrackoJrState0Update,
    (u32)KrackoJrTransformUpdate,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in KrackoInit, KrackoEnterState */
u32 gKrackoStates[7] ACTOR_TBL(08749150) = {
    (u32)KrackoIntro,
    (u32)KrackoPickMove,
    (u32)KrackoWait,
    (u32)KrackoCross,
    (u32)KrackoLightningSweep,
    (u32)KrackoSwoop,
    (u32)KrackoSummon,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in KrackoUpdate */
u32 gKrackoStateUpdates[7] ACTOR_TBL(08749150) = {
    (u32)KrackoIntroUpdate,
    (u32)KrackoPickMoveUpdate,
    (u32)KrackoWaitUpdate,
    (u32)KrackoCrossUpdate,
    (u32)KrackoLightningSweepUpdate,
    (u32)KrackoSwoopUpdate,
    (u32)KrackoSummonUpdate,
};

/* ---- 0x087493F4-0x08749458: 3 table(s), 25 function pointer(s), section .actor_tbl_087493f4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_NightmareWizard */
u32 gNightmareWizardVariants[1] ACTOR_TBL(087493f4) = {
    (u32)NightmareWizardInit,
};
/* include/enemy.h; CallTableEntry(i, 12, ...) in NightmareWizardInit, NightmareWizardEnterState */
u32 gNightmareWizardStates[12] ACTOR_TBL(087493f4) = {
    (u32)NightmareWizardState0,
    (u32)NightmareWizardState1,
    (u32)NightmareWizardState2,
    (u32)NightmareWizardState3,
    (u32)NightmareWizardState4,
    (u32)NightmareWizardOpenCloak,
    (u32)NightmareWizardOpenPalm,
    (u32)NightmareWizardPoint,
    (u32)NightmareWizardTwist,
    (u32)NightmareWizardSwoop,
    (u32)NightmareWizardState10,
    (u32)NightmareWizardHurt,
};
/* include/enemy.h; CallTableEntry(i, 12, ...) in NightmareWizardUpdate */
u32 gNightmareWizardStateUpdates[12] ACTOR_TBL(087493f4) = {
    (u32)NightmareWizardState0Update,
    (u32)NightmareWizardState1Update,
    (u32)NightmareWizardState2Update,
    (u32)NightmareWizardState3Update,
    (u32)NightmareWizardState4Update,
    (u32)NightmareWizardOpenCloakUpdate,
    (u32)NightmareWizardOpenPalmUpdate,
    (u32)NightmareWizardPointUpdate,
    (u32)NightmareWizardTwistUpdate,
    (u32)NightmareWizardSwoopUpdate,
    (u32)NightmareWizardState10Update,
    (u32)NightmareWizardHurtUpdate,
};

/* ---- 0x08749B8C-0x08749BAC: 4 table(s), 8 function pointer(s), section .actor_tbl_08749b8c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_MetaKnightSword */
u32 gUnk_08749B8C[3] ACTOR_TBL(08749b8c) = {
    (u32)sub_080ac868,
    (u32)sub_080ac950,
    (u32)sub_080aca3c,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_080ac72c, sub_080ac82c, sub_080ac84c */
u32 gUnk_08749B98[3] ACTOR_TBL(08749b8c) = {
    (u32)sub_080ac94c,
    (u32)sub_080aca38,
    (u32)sub_080aca60,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KrackoStarman */
u32 gKrackoStarmanStates[1] ACTOR_TBL(08749b8c) = {
    (u32)KrackoStarmanState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KrackoStarmanUpdate */
u32 gKrackoStarmanStateUpdates[1] ACTOR_TBL(08749b8c) = {
    (u32)KrackoStarmanCheckParent,
};

/* ---- 0x08749BE4-0x08749BEC: 2 table(s), 2 function pointer(s), section .actor_tbl_08749be4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_NightmareWizardStar */
u32 gNightmareWizardStarStates[1] ACTOR_TBL(08749be4) = {
    (u32)NightmareWizardStarState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NightmareWizardStarUpdate */
u32 gNightmareWizardStarStateUpdates[1] ACTOR_TBL(08749be4) = {
    (u32)NightmareWizardStarState0Update,
};

/* ---- 0x08749D10-0x08749D1C: 1 table(s), 3 function pointer(s), section .actor_tbl_08749d10 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in PaintRollerEnterState */
u32 gPaintRollerStates[3] ACTOR_TBL(08749d10) = {
    (u32)Task_PaintRoller,
    (u32)PaintRollerRunToNextSpot,
    (u32)PaintRollerSummon,
};

/* ---- 0x0874AD34-0x0874AD44: 1 table(s), 4 function pointer(s), section .actor_tbl_0874ad34 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in NightmarePowerOrbUpdate */
u32 gNightmarePowerOrbStateUpdates[4] ACTOR_TBL(0874ad34) = {
    (u32)sub_080af114,
    (u32)sub_080af144,
    (u32)sub_080af188,
    (u32)sub_080af1c8,
};

/* ---- 0x0874AD74-0x0874B1A8: 7 script(s), section .actor_tbl_0874ad74 ---- */
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 13 words up to its -2 (restart) */
u32 gUnk_0874AD74[13] ACTOR_TBL(0874ad74) = {
    -4, 4, (u32)NightmarePowerOrbGetAnimDelay,
    -4, 5, (u32)NightmarePowerOrbGetAnimDelay,
    -4, 6, (u32)NightmarePowerOrbGetAnimDelay,
    -4, 7, (u32)NightmarePowerOrbGetAnimDelay,
    -2,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 65 words up to its -2 (restart) */
u32 gUnk_0874ADA8[65] ACTOR_TBL(0874ad74) = {
    4, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -3, (u32)sub_080af178,
    5, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -3, (u32)sub_080af178,
    6, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -3, (u32)sub_080af178,
    7, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -3, (u32)sub_080af178,
    -2,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 57 words up to its -2 (restart) */
u32 gUnk_0874AEAC[57] ACTOR_TBL(0874ad74) = {
    4, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    5, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    6, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    7, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -2,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 59 words up to its -1 (stop) */
u32 gUnk_0874AF90[59] ACTOR_TBL(0874ad74) = {
    4, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    5, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    6, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    7, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -3, (u32)sub_080af1b8,
    -1,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 9 words up to its -2 (restart) */
u32 gUnk_0874B07C[9] ACTOR_TBL(0874ad74) = {
    4, 1,
    6, 1,
    5, 1,
    7, 1,
    -2,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 9 words up to its -2 (restart) */
u32 gUnk_0874B0A0[9] ACTOR_TBL(0874ad74) = {
    4, 4,
    5, 4,
    6, 4,
    7, 4,
    -2,
};
/* include/enemy.h; NightmarePowerOrbStartAnim installs it as Actor.animScript, NightmarePowerOrbStepAnim runs it: 57 words up to its -2 (restart) */
u32 gUnk_0874B0C4[57] ACTOR_TBL(0874ad74) = {
    4, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    5, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    6, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    7, 2,
    8, 1,
    9, 1,
    11, 1,
    10, 1,
    9, 1,
    8, 1,
    -2,
};

/* ---- 0x0874B1DC-0x0874B20C: 1 table(s), 12 function pointer(s), section .actor_tbl_0874b1dc ---- */
/* include/enemy.h; CallTableEntry(i, 12, ...) in Task_NightmarePowerOrbStar */
u32 gNightmarePowerOrbStarVariants[12] ACTOR_TBL(0874b1dc) = {
    (u32)NightmarePowerOrbStarVariant0,
    (u32)NightmarePowerOrbStarVariant1,
    (u32)NightmarePowerOrbStarVariant2,
    (u32)NightmarePowerOrbStarVariant3,
    (u32)NightmarePowerOrbStarVariant4,
    (u32)NightmarePowerOrbStarVariant5,
    (u32)NightmarePowerOrbStarVariant6,
    (u32)NightmarePowerOrbStarVariant7,
    (u32)NightmarePowerOrbStarVariant8,
    (u32)NightmarePowerOrbStarVariant9,
    (u32)NightmarePowerOrbStarVariant10,
    (u32)NightmarePowerOrbStarVariant11,
};

/* ---- 0x0874B540-0x0874B560: 1 table(s), 8 function pointer(s), section .actor_tbl_0874b540 ---- */
/* include/enemy.h; CallTableEntry(i, 8, ...) in Task_PaintRollerPainting */
u32 gPaintRollerPaintingStates[8] ACTOR_TBL(0874b540) = {
    (u32)PaintRollerPaintingCar,
    (u32)PaintRollerPaintingKirby,
    (u32)PaintRollerPaintingWaddleDee,
    (u32)PaintRollerPaintingMike,
    (u32)PaintRollerPaintingBaseball,
    (u32)PaintRollerPaintingBomb,
    (u32)PaintRollerPaintingCloud,
    (u32)PaintRollerPaintingParasol,
};

/* ---- 0x0874B5E4-0x0874B614: 2 table(s), 12 function pointer(s), section .actor_tbl_0874b5e4 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in Task_HeavyMoleUpperArm, Task_HeavyMoleLowerArm, HeavyMoleArmEnterState */
u32 gHeavyMoleArmStates[6] ACTOR_TBL(0874b5e4) = {
    (u32)HeavyMoleArmState0,
    (u32)HeavyMoleArmState1,
    (u32)HeavyMoleArmState2,
    (u32)HeavyMoleArmState3,
    (u32)HeavyMoleArmState4,
    (u32)HeavyMoleArmState5,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in HeavyMoleArmUpdate */
u32 gHeavyMoleArmStateUpdates[6] ACTOR_TBL(0874b5e4) = {
    (u32)HeavyMoleArmState0Update,
    (u32)HeavyMoleArmState1Update,
    (u32)HeavyMoleArmState2Update,
    (u32)HeavyMoleArmState3Update,
    (u32)HeavyMoleArmState4Update,
    (u32)HeavyMoleArmState5Update,
};

/* ---- 0x0874C12C-0x0874C158: 5 table(s), 11 function pointer(s), section .actor_tbl_0874c12c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoods */
u32 gWhispyWoodsVariants[1] ACTOR_TBL(0874c12c) = {
    (u32)WhispyWoodsInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsInit, WhispyWoodsEnterState */
u32 gWhispyWoodsStates[4] ACTOR_TBL(0874c12c) = {
    (u32)WhispyWoodsWait,
    (u32)WhispyWoodsBlowTwoPuffs,
    (u32)WhispyWoodsBlowFourPuffs,
    (u32)WhispyWoodsDropApples,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsUpdate */
u32 gWhispyWoodsStateUpdates[4] ACTOR_TBL(0874c12c) = {
    (u32)WhispyWoodsWaitUpdate,
    (u32)WhispyWoodsBlowTwoPuffsUpdate,
    (u32)WhispyWoodsBlowFourPuffsUpdate,
    (u32)WhispyWoodsDropApplesUpdate,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WhispyWoodsDefeatedInit, WhispyWoodsDefeatedEnterState */
u32 gWhispyWoodsDefeatedStates[1] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2e58,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WhispyWoodsDefeatedUpdate */
u32 gWhispyWoodsDefeatedStateUpdates[1] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2f34,
};

/* ---- 0x0874C21C-0x0874C240: 3 table(s), 9 function pointer(s), section .actor_tbl_0874c21c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoodsApple */
u32 gWhispyWoodsAppleVariants[1] ACTOR_TBL(0874c21c) = {
    (u32)WhispyWoodsAppleInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsAppleInit, WhispyWoodsAppleEnterState */
u32 gWhispyWoodsAppleStates[4] ACTOR_TBL(0874c21c) = {
    (u32)WhispyWoodsAppleFall,
    (u32)WhispyWoodsAppleState1,
    (u32)WhispyWoodsAppleState2,
    (u32)WhispyWoodsAppleState3,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsAppleUpdate */
u32 gWhispyWoodsAppleStateUpdates[4] ACTOR_TBL(0874c21c) = {
    (u32)WhispyWoodsAppleFallUpdate,
    (u32)WhispyWoodsAppleState1Update,
    (u32)WhispyWoodsAppleState2Update,
    (u32)WhispyWoodsAppleState3Update,
};

/* ---- 0x0874C254-0x0874C260: 3 table(s), 3 function pointer(s), section .actor_tbl_0874c254 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoodsAirPuff */
u32 gWhispyWoodsAirPuffVariants[1] ACTOR_TBL(0874c254) = {
    (u32)WhispyWoodsAirPuffInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WhispyWoodsAirPuffInit, sub_080b33bc */
u32 gWhispyWoodsAirPuffStates[1] ACTOR_TBL(0874c254) = {
    (u32)WhispyWoodsAirPuffState0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WhispyWoodsAirPuffUpdate */
u32 gWhispyWoodsAirPuffStateUpdates[1] ACTOR_TBL(0874c254) = {
    (u32)WhispyWoodsAirPuffState0Update,
};
