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
/* include/actor.h; CallTableEntry(i, 8, ...) in sub_0806ee30, sub_0806ef38 */
u32 gUnk_0873FB04[8] ACTOR_TBL(0873fb04) = {
    0,
    (u32)sub_0806efec,
    (u32)sub_0806ef5c,
    (u32)sub_0806f1e0,
    (u32)sub_0806f3d8,
    (u32)sub_0806f638,
    (u32)sub_0806fb0c,
    (u32)sub_0806fd04,
};
/* include/actor.h; CallTableEntry(i, 8, ...) in sub_0806ef1c */
u32 gUnk_0873FB24[8] ACTOR_TBL(0873fb04) = {
    0,
    (u32)sub_0806f174,
    (u32)sub_0806efe8,
    (u32)sub_0806f36c,
    (u32)sub_0806f5c4,
    (u32)sub_0806faac,
    (u32)sub_0806fc98,
    (u32)sub_0806ff24,
};
/* include/actor.h; CallTableEntry(i, 7, ...) in sub_08070648, sub_08070758 */
u32 gUnk_0873FB44[7] ACTOR_TBL(0873fb04) = {
    0,
    (u32)sub_0807079c,
    (u32)sub_0807077c,
    (u32)sub_08070930,
    (u32)sub_08070ac8,
    (u32)sub_08070c54,
    (u32)sub_08070d90,
};
/* include/actor.h; CallTableEntry(i, 7, ...) in sub_0807073c */
u32 gUnk_0873FB60[7] ACTOR_TBL(0873fb04) = {
    0,
    (u32)sub_080708ec,
    (u32)sub_08070798,
    (u32)sub_08070a84,
    (u32)sub_08070c0c,
    (u32)sub_08070d48,
    (u32)sub_08070e7c,
};

/* ---- 0x0873FBAC-0x0873FCF8: 5 table(s), 83 function pointer(s), section .actor_tbl_0873fbac ---- */
/* include/cutscene.h; CallTableEntry(i, 3, ...) in Task_WarpStar, WarpStarEnterState */
u32 gWarpStarStates[3] ACTOR_TBL(0873fbac) = {
    (u32)sub_08071418,
    (u32)sub_0807160c,
    (u32)sub_08071694,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in WarpStarUpdate */
u32 gWarpStarStateUpdates[3] ACTOR_TBL(0873fbac) = {
    (u32)sub_0807156c,
    (u32)sub_08071640,
    (u32)sub_08071774,
};
/* include/cutscene.h; CallTableEntry(i, 26, ...) in WarpStarStartFlight, WarpStarFlightEnterState */
u32 gWarpStarFlights[26] ACTOR_TBL(0873fbac) = {
    (u32)sub_08071d60,
    (u32)sub_08071e80,
    (u32)sub_08071d60,
    (u32)sub_08071e80,
    (u32)sub_08074420,
    (u32)sub_08071f54,
    (u32)sub_080720e8,
    (u32)sub_08071e80,
    (u32)sub_08072388,
    (u32)sub_08071e80,
    (u32)sub_08072684,
    (u32)sub_080728b0,
    (u32)sub_08072b00,
    (u32)sub_08072d8c,
    (u32)sub_080731d0,
    (u32)sub_08073298,
    (u32)sub_08071e80,
    (u32)sub_08073584,
    (u32)sub_08073804,
    (u32)sub_08073968,
    (u32)sub_08073a54,
    (u32)sub_08073ce0,
    (u32)sub_08073e0c,
    (u32)sub_08073f18,
    (u32)sub_080740bc,
    (u32)sub_08074420,
};
/* include/cutscene.h; CallTableEntry(i, 26, ...) in WarpStarFlightUpdate */
u32 gWarpStarFlightUpdates[26] ACTOR_TBL(0873fbac) = {
    (u32)sub_08071e74,
    (u32)sub_08071ebc,
    (u32)sub_08071e74,
    (u32)sub_08071ebc,
    (u32)sub_0807447c,
    (u32)sub_080720dc,
    (u32)sub_0807237c,
    (u32)sub_08071ebc,
    (u32)sub_08072678,
    (u32)sub_08071ebc,
    (u32)sub_080728a4,
    (u32)sub_08072af4,
    (u32)sub_08072d80,
    (u32)sub_080731c4,
    (u32)sub_0807328c,
    (u32)sub_08073578,
    (u32)sub_08071ebc,
    (u32)sub_080737f8,
    (u32)sub_0807395c,
    (u32)sub_080739bc,
    (u32)sub_08073cd4,
    (u32)sub_08073e00,
    (u32)sub_08073e80,
    (u32)sub_0807409c,
    (u32)sub_080743c8,
    (u32)sub_080743f0,
};
/* include/cutscene.h; CallTableEntry(i, 25, ...) in Task_WarpStarCamera */
u32 gWarpStarCameraPaths[25] ACTOR_TBL(0873fbac) = {
    (u32)sub_080745dc,
    (u32)sub_08074628,
    (u32)sub_080745dc,
    (u32)sub_08074628,
    (u32)sub_08074588,
    (u32)sub_08074638,
    (u32)sub_080746c0,
    (u32)sub_080745d0,
    (u32)sub_0807470c,
    (u32)sub_080745d0,
    (u32)sub_08074784,
    (u32)sub_08074794,
    (u32)sub_080747dc,
    (u32)sub_080747ec,
    (u32)sub_08074880,
    (u32)sub_080748a8,
    (u32)sub_080745d0,
    (u32)sub_08074904,
    (u32)sub_08074974,
    (u32)sub_080745d0,
    (u32)sub_08074988,
    (u32)sub_08074ab8,
    (u32)sub_080745d0,
    (u32)sub_08074ac8,
    (u32)sub_08074b60,
};

/* ---- 0x087400B0-0x087400E4: 3 table(s), 13 function pointer(s), section .actor_tbl_087400b0 ---- */
/* include/cutscene.h; CallTableEntry(i, 6, ...) in sub_08076318, sub_080763c4 */
u32 gUnk_087400B0[6] ACTOR_TBL(087400b0) = {
    (u32)sub_08076c88,
    (u32)sub_08076cd4,
    (u32)sub_08076d0c,
    (u32)sub_08076d44,
    (u32)sub_08076dac,
    (u32)sub_08076e30,
};
/* include/cutscene.h; CallTableEntry(i, 6, ...) in sub_0807637c */
u32 gUnk_087400C8[6] ACTOR_TBL(087400b0) = {
    (u32)sub_08076cac,
    (u32)sub_08076d00,
    (u32)sub_08076d38,
    (u32)sub_08076d70,
    (u32)sub_08076ddc,
    (u32)sub_08076e48,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_Cannon */
u32 gCannonVariants[1] ACTOR_TBL(087400b0) = {
    (u32)CannonInit,
};

/* ---- 0x08740100-0x08740124: 3 table(s), 9 function pointer(s), section .actor_tbl_08740100 ---- */
/* include/cutscene.h; CallTableEntry(i, 4, ...) in CannonInit, CannonEnterState */
u32 gCannonStates[4] ACTOR_TBL(08740100) = {
    (u32)sub_0807728c,
    (u32)sub_08077308,
    (u32)sub_080773d0,
    (u32)sub_08077568,
};
/* include/cutscene.h; CallTableEntry(i, 4, ...) in CannonUpdate */
u32 gCannonStateUpdates[4] ACTOR_TBL(08740100) = {
    (u32)sub_080772b0,
    (u32)sub_080773a8,
    (u32)sub_08077564,
    (u32)sub_08077718,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_CannonFuse */
u32 gCannonFuseVariants[1] ACTOR_TBL(08740100) = {
    (u32)CannonFuseInit,
};

/* ---- 0x087402BC-0x08740320: 9 table(s), 25 function pointer(s), section .actor_tbl_087402bc ---- */
/* include/cutscene.h; CallTableEntry(i, 3, ...) in CannonFuseInit, CannonFuseEnterState */
u32 gCannonFuseStates[3] ACTOR_TBL(087402bc) = {
    (u32)sub_08077ae0,
    (u32)sub_08077b60,
    (u32)sub_08077bf0,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in CannonFuseUpdate */
u32 gCannonFuseStateUpdates[3] ACTOR_TBL(087402bc) = {
    (u32)sub_08077b24,
    (u32)sub_08077ba8,
    (u32)sub_08077c2c,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_BigSwitch */
u32 gBigSwitchVariants[1] ACTOR_TBL(087402bc) = {
    (u32)BigSwitchInit,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in BigSwitchInit, BigSwitchEnterState */
u32 gBigSwitchStates[3] ACTOR_TBL(087402bc) = {
    (u32)sub_08077e4c,
    (u32)sub_08077e9c,
    (u32)sub_08077ed0,
};
/* include/cutscene.h; CallTableEntry(i, 3, ...) in BigSwitchUpdate */
u32 gBigSwitchStateUpdates[3] ACTOR_TBL(087402bc) = {
    (u32)sub_08077e74,
    (u32)sub_08077ecc,
    (u32)sub_08077f08,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in Task_Stake */
u32 gStakeVariants[1] ACTOR_TBL(087402bc) = {
    (u32)StakeInit,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in StakeInit */
u32 gStakeStates[1] ACTOR_TBL(087402bc) = {
    (u32)sub_08077f98,
};
/* include/cutscene.h; CallTableEntry(i, 1, ...) in StakeUpdate */
u32 gStakeStateUpdates[1] ACTOR_TBL(087402bc) = {
    (u32)sub_08077ff4,
};
/* include/cutscene.h; CallTableEntry(i, 5, ...) in sub_08078598: entries 5-8 lie past that bound; the extent is the span up to the next label (docs/data.md 5.1) */
u32 gUnk_087402FC[9] ACTOR_TBL(087402bc) = {
    (u32)sub_08078670,
    (u32)sub_080786e8,
    (u32)sub_0807876c,
    (u32)sub_080787f0,
    (u32)sub_08078874,
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
    (u32)sub_08078cb8,
    (u32)sub_08078ce4,
};

/* ---- 0x08740670-0x08740680: 2 table(s), 4 function pointer(s), section .actor_tbl_08740670 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeePaceInit, WaddleDeePaceEnterState */
u32 gWaddleDeePaceStates[2] ACTOR_TBL(08740670) = {
    (u32)WaddleDeePaceWalk,
    (u32)WaddleDeePaceFall,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in WaddleDeePaceUpdate */
u32 gWaddleDeePaceStateUpdates[2] ACTOR_TBL(08740670) = {
    (u32)sub_08078dd4,
    (u32)sub_08078e38,
};

/* ---- 0x087406C4-0x087406EC: 4 table(s), 10 function pointer(s), section .actor_tbl_087406c4 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDeeJumpInit, WaddleDeeJumpEnterState */
u32 gWaddleDeeJumpStates[3] ACTOR_TBL(087406c4) = {
    (u32)sub_08078eec,
    (u32)WaddleDeeJump,
    (u32)WaddleDeeJumpFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in WaddleDeeJumpUpdate */
u32 gWaddleDeeJumpStateUpdates[3] ACTOR_TBL(087406c4) = {
    (u32)sub_08078f24,
    (u32)sub_0807906c,
    (u32)sub_080790e8,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeWalkInit, ParasolWaddleDeeWalkEnterState */
u32 gParasolWaddleDeeWalkStates[2] ACTOR_TBL(087406c4) = {
    (u32)ParasolWaddleDeeWalk,
    (u32)sub_080791c0,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeWalkUpdate */
u32 gParasolWaddleDeeWalkStateUpdates[2] ACTOR_TBL(087406c4) = {
    (u32)sub_080791bc,
    (u32)sub_0807921c,
};

/* ---- 0x08740700-0x08740720: 5 table(s), 8 function pointer(s), section .actor_tbl_08740700 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in WaddleDeeIdleInit, sub_0807927c */
u32 gWaddleDeeIdleStates[1] ACTOR_TBL(08740700) = {
    (u32)WaddleDeeIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WaddleDeeIdleUpdate */
u32 gWaddleDeeIdleStateUpdates[1] ACTOR_TBL(08740700) = {
    (u32)sub_080792f4,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeStandInit, ParasolWaddleDeeStandEnterState */
u32 gParasolWaddleDeeStandStates[2] ACTOR_TBL(08740700) = {
    (u32)sub_080793a8,
    (u32)sub_080793d0,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in ParasolWaddleDeeStandUpdate */
u32 gParasolWaddleDeeStandStateUpdates[2] ACTOR_TBL(08740700) = {
    (u32)sub_080793c4,
    (u32)sub_08079424,
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
    (u32)sub_080795d8,
    (u32)sub_080797b4,
    (u32)sub_080798b8,
    (u32)sub_08079914,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIdleInit, sub_080799a0 */
u32 gPengyIdleStates[1] ACTOR_TBL(08740758) = {
    (u32)PengyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PengyIdleUpdate */
u32 gPengyIdleStateUpdates[1] ACTOR_TBL(08740758) = {
    (u32)sub_080799dc,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Bomber */
u32 gBomberVariants[2] ACTOR_TBL(08740758) = {
    (u32)BomberInit,
    (u32)BomberIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in BomberInit, BomberEnterState */
u32 gBomberStates[4] ACTOR_TBL(08740758) = {
    (u32)BomberWalk,
    (u32)sub_08079bac,
    (u32)sub_08079be4,
    (u32)sub_08079cac,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in BomberUpdate */
u32 gBomberStateUpdates[4] ACTOR_TBL(08740758) = {
    (u32)sub_08079b98,
    (u32)sub_08079be0,
    (u32)sub_08079c84,
    (u32)sub_08079d2c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BomberIdleInit, sub_08079db8 */
u32 gBomberIdleStates[1] ACTOR_TBL(08740758) = {
    (u32)BomberIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BomberIdleUpdate */
u32 gBomberIdleStateUpdates[1] ACTOR_TBL(08740758) = {
    (u32)sub_08079e20,
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
    (u32)sub_0807a1c0,
    (u32)sub_0807a3bc,
    (u32)SparkyJump,
    (u32)sub_0807a424,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in SparkyJumpUpdate */
u32 gSparkyJumpStateUpdates[4] ACTOR_TBL(087407e4) = {
    (u32)sub_0807a380,
    (u32)sub_0807a408,
    (u32)sub_0807a1bc,
    (u32)sub_0807a458,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SparkyIdleInit, sub_0807a4e4 */
u32 gSparkyIdleStates[1] ACTOR_TBL(087407e4) = {
    (u32)SparkyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SparkyIdleUpdate */
u32 gSparkyIdleStateUpdates[1] ACTOR_TBL(087407e4) = {
    (u32)sub_0807a574,
};

/* ---- 0x08740810-0x08740824: 3 table(s), 5 function pointer(s), section .actor_tbl_08740810 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in SparkyStandInit, SparkyStandEnterState */
u32 gSparkyStandStates[2] ACTOR_TBL(08740810) = {
    (u32)sub_0807a634,
    (u32)sub_0807a830,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in SparkyStandUpdate */
u32 gSparkyStandStateUpdates[2] ACTOR_TBL(08740810) = {
    (u32)sub_0807a7f4,
    (u32)sub_0807a87c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Scarfy */
u32 gUnk_08740820[1] ACTOR_TBL(08740810) = {
    (u32)ScarfyInit,
};

/* ---- 0x08740960-0x0874099C: 3 table(s), 15 function pointer(s), section .actor_tbl_08740960 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in ScarfyInit, ScarfyEnterState */
u32 gScarfyStates[6] ACTOR_TBL(08740960) = {
    (u32)sub_0807ab8c,
    (u32)sub_0807abdc,
    (u32)sub_0807ab54,
    (u32)ScarfyTransform,
    (u32)ScarfyChase,
    (u32)sub_0807aecc,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in ScarfyUpdate */
u32 gScarfyStateUpdates[6] ACTOR_TBL(08740960) = {
    (u32)sub_0807abb4,
    (u32)sub_0807ac08,
    (u32)sub_0807ab70,
    (u32)sub_0807adcc,
    (u32)ScarfyChaseUpdate,
    (u32)sub_0807af3c,
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
    (u32)sub_0807b3f8,
    (u32)sub_0807b49c,
    (u32)sub_0807b584,
    (u32)sub_0807b4f0,
    (u32)sub_0807b5d8,
    (u32)sub_0807b66c,
    (u32)sub_0807b6e8,
    (u32)SwordAndBladeKnightWalkFall,
};
/* include/enemy.h; CallTableEntry(i, 8, ...) in SwordAndBladeKnightWalkUpdate */
u32 gSwordAndBladeKnightWalkStateUpdates[8] ACTOR_TBL(087409fc) = {
    (u32)sub_0807b430,
    (u32)sub_0807b4c8,
    (u32)sub_0807b5b0,
    (u32)sub_0807b558,
    (u32)sub_0807b640,
    (u32)sub_0807b6b4,
    (u32)sub_0807b7a8,
    (u32)sub_0807b7fc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SwordAndBladeKnightIdleInit, sub_0807b888 */
u32 gSwordAndBladeKnightIdleStates[1] ACTOR_TBL(087409fc) = {
    (u32)SwordAndBladeKnightIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SwordAndBladeKnightIdleUpdate */
u32 gSwordAndBladeKnightIdleStateUpdates[1] ACTOR_TBL(087409fc) = {
    (u32)sub_0807b8d0,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in SwordAndBladeKnightStandInit, SwordAndBladeKnightStandEnterState */
u32 gSwordAndBladeKnightStandStates[6] ACTOR_TBL(087409fc) = {
    (u32)sub_0807b9ec,
    (u32)sub_0807ba7c,
    (u32)sub_0807bb60,
    (u32)sub_0807bad0,
    (u32)sub_0807bbb4,
    (u32)sub_0807bc44,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in SwordAndBladeKnightStandUpdate */
u32 gSwordAndBladeKnightStandStateUpdates[6] ACTOR_TBL(087409fc) = {
    (u32)sub_0807ba18,
    (u32)sub_0807baa8,
    (u32)sub_0807bb8c,
    (u32)sub_0807bb38,
    (u32)sub_0807bc1c,
    (u32)sub_0807bc78,
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
    (u32)sub_0807c118,
    (u32)NeedlousFall,
    (u32)sub_0807c210,
    (u32)sub_0807c248,
    (u32)sub_0807c280,
    (u32)NeedlousDash,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NeedlousUpdate */
u32 gNeedlousStateUpdates[6] ACTOR_TBL(08740ac8) = {
    (u32)sub_0807c138,
    (u32)sub_0807c1f4,
    (u32)sub_0807c22c,
    (u32)sub_0807c264,
    (u32)sub_0807c29c,
    (u32)sub_0807c2e0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NeedlousIdleInit, sub_0807c3d8 */
u32 gNeedlousIdleStates[1] ACTOR_TBL(08740ac8) = {
    (u32)NeedlousIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NeedlousIdleUpdate */
u32 gNeedlousIdleStateUpdates[1] ACTOR_TBL(08740ac8) = {
    (u32)sub_0807c440,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_UFO */
u32 gUFOVariants[2] ACTOR_TBL(08740ac8) = {
    (u32)UFOInit,
    (u32)UFOIdleInit,
};

/* ---- 0x08740B84-0x08740BD4: 11 table(s), 20 function pointer(s), section .actor_tbl_08740b84 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in UFOInit, UFOEnterState */
u32 gUFOStates[4] ACTOR_TBL(08740b84) = {
    (u32)sub_0807c710,
    (u32)sub_0807c80c,
    (u32)sub_0807c8b0,
    (u32)UFOShoot,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in UFOUpdate */
u32 gUFOStateUpdates[4] ACTOR_TBL(08740b84) = {
    (u32)sub_0807c7d8,
    (u32)sub_0807c828,
    (u32)sub_0807c8d0,
    (u32)sub_0807ca18,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in UFOIdleInit, sub_0807cadc */
u32 gUFOIdleStates[1] ACTOR_TBL(08740b84) = {
    (u32)UFOIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in UFOIdleUpdate */
u32 gUFOIdleStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807cbb0,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Parasol */
u32 gParasolVariants[4] ACTOR_TBL(08740b84) = {
    (u32)ParasolRiseInit,
    (u32)ParasolChaseInit,
    (u32)ParasolIdleInit,
    (u32)sub_0807cfd0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolRiseInit, sub_0807ccec */
u32 gParasolRiseStates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807cd08,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolRiseUpdate */
u32 gParasolRiseStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807cd60,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolChaseInit, sub_0807cdec */
u32 gParasolChaseStates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807ce08,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolChaseUpdate */
u32 gParasolChaseStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807ce68,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolIdleInit, sub_0807cf64 */
u32 gParasolIdleStates[1] ACTOR_TBL(08740b84) = {
    (u32)ParasolIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ParasolIdleUpdate */
u32 gParasolIdleStateUpdates[1] ACTOR_TBL(08740b84) = {
    (u32)sub_0807cfcc,
};

/* ---- 0x08741088-0x08741094: 3 table(s), 3 function pointer(s), section .actor_tbl_08741088 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_PengyIceBreath */
u32 gUnk_08741088[1] ACTOR_TBL(08741088) = {
    (u32)sub_0807d060,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807d060, sub_0807d0d8 */
u32 gUnk_0874108C[1] ACTOR_TBL(08741088) = {
    (u32)sub_0807d0f4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807d094 */
u32 gUnk_08741090[1] ACTOR_TBL(08741088) = {
    (u32)sub_0807d178,
};

/* ---- 0x087410AC-0x087410C0: 3 table(s), 5 function pointer(s), section .actor_tbl_087410ac ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_UFOLaser */
u32 gUnk_087410AC[1] ACTOR_TBL(087410ac) = {
    (u32)sub_0807d1fc,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0807d1fc, sub_0807d29c */
u32 gUnk_087410B0[2] ACTOR_TBL(087410ac) = {
    (u32)sub_0807d2b8,
    (u32)sub_0807d320,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0807d230 */
u32 gUnk_087410B8[2] ACTOR_TBL(087410ac) = {
    (u32)sub_0807d2ec,
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
    (u32)sub_0807d748,
    (u32)sub_0807d8f8,
    (u32)sub_0807d9ac,
    (u32)sub_0807da30,
    (u32)sub_0807da84,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in RockyWalkUpdate */
u32 gRockyWalkStateUpdates[5] ACTOR_TBL(087411c0) = {
    (u32)sub_0807d82c,
    (u32)sub_0807d918,
    (u32)sub_0807da08,
    (u32)sub_0807da5c,
    (u32)sub_0807daa8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in RockyIdleInit, sub_0807daf0 */
u32 gRockyIdleStates[1] ACTOR_TBL(087411c0) = {
    (u32)RockyIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in RockyIdleUpdate */
u32 gRockyIdleStateUpdates[1] ACTOR_TBL(087411c0) = {
    (u32)sub_0807db44,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in RockyStandInit, RockyStandEnterState */
u32 gRockyStandStates[3] ACTOR_TBL(087411c0) = {
    (u32)sub_0807dbcc,
    (u32)sub_0807dca0,
    (u32)sub_0807dce8,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in RockyStandUpdate */
u32 gRockyStandStateUpdates[3] ACTOR_TBL(087411c0) = {
    (u32)sub_0807dc78,
    (u32)sub_0807dcc0,
    (u32)sub_0807dd0c,
};

/* ---- 0x08741220-0x08741298: 15 table(s), 30 function pointer(s), section .actor_tbl_08741220 ---- */
/* include/enemy.h; read by sub_0807e244, sub_0807e3b0 */
u32 gUnk_08741220[2] ACTOR_TBL(08741220) = {
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
    (u32)sub_0807df8c,
    (u32)sub_0807e244,
    (u32)sub_0807e3b0,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleWalkInit, SirKibbleWalkEnterState */
u32 gSirKibbleWalkStates[3] ACTOR_TBL(08741220) = {
    (u32)SirKibbleWalk,
    (u32)SirKibbleShoot,
    (u32)SirKibbleJump,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in SirKibbleWalkUpdate */
u32 gSirKibbleWalkStateUpdates[3] ACTOR_TBL(08741220) = {
    (u32)sub_0807e100,
    (u32)sub_0807e244,
    (u32)sub_0807e3b0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleIdleInit, sub_0807e428 */
u32 gSirKibbleIdleStates[1] ACTOR_TBL(08741220) = {
    (u32)SirKibbleIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in SirKibbleIdleUpdate */
u32 gSirKibbleIdleStateUpdates[1] ACTOR_TBL(08741220) = {
    (u32)sub_0807e480,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_Cappy */
u32 gCappyVariants[3] ACTOR_TBL(08741220) = {
    (u32)CappyCappedInit,
    (u32)CappyCaplessInit,
    (u32)sub_0807e8b8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyCappedInit, sub_0807e5a0 */
u32 gCappyCappedStates[1] ACTOR_TBL(08741220) = {
    (u32)sub_0807e640,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CappyCappedUpdate */
u32 gCappyCappedStateUpdates[1] ACTOR_TBL(08741220) = {
    (u32)sub_0807e6d0,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in CappyCaplessInit, CappyCaplessEnterState */
u32 gCappyCaplessStates[2] ACTOR_TBL(08741220) = {
    (u32)sub_0807e768,
    (u32)sub_0807e814,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in CappyCaplessUpdate */
u32 gCappyCaplessStateUpdates[2] ACTOR_TBL(08741220) = {
    (u32)sub_0807e810,
    (u32)sub_0807e884,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807e8b8, sub_0807e904 */
u32 gUnk_08741290[1] ACTOR_TBL(08741220) = {
    (u32)sub_0807e950,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807e920 */
u32 gUnk_08741294[1] ACTOR_TBL(08741220) = {
    (u32)sub_0807e9b0,
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
    (u32)sub_0807ea84,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBobUpdate */
u32 gGordoBobStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ead4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceVerticalInit */
u32 gGordoBounceVerticalStates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807eb60,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceVerticalUpdate */
u32 gGordoBounceVerticalStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ebc4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceHorizontalInit */
u32 gGordoBounceHorizontalStates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ec4c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoBounceHorizontalUpdate */
u32 gGordoBounceHorizontalStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ecb0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoSweepInit */
u32 gGordoSweepStates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ed20,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in GordoSweepUpdate */
u32 gGordoSweepStateUpdates[1] ACTOR_TBL(087412bc) = {
    (u32)sub_0807ed98,
};

/* ---- 0x08741300-0x08741318: 5 table(s), 6 function pointer(s), section .actor_tbl_08741300 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_CoolSpook */
u32 gCoolSpookVariants[2] ACTOR_TBL(08741300) = {
    (u32)sub_0807ee14,
    (u32)sub_0807eec4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807ee14, sub_0807ee44 */
u32 gUnk_08741308[1] ACTOR_TBL(08741300) = {
    (u32)sub_0807ee84,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807ee60 */
u32 gUnk_0874130C[1] ACTOR_TBL(08741300) = {
    (u32)sub_0807eea8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807eec4, sub_0807ef08 */
u32 gUnk_08741310[1] ACTOR_TBL(08741300) = {
    (u32)sub_0807ef48,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0807ef24 */
u32 gUnk_08741314[1] ACTOR_TBL(08741300) = {
    (u32)sub_0807ef60,
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
    (u32)sub_0807f0c4,
    (u32)KabuJump,
    (u32)KabuJumpFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuJumpUpdate */
u32 gKabuJumpStateUpdates[3] ACTOR_TBL(08741380) = {
    (u32)sub_0807f1f0,
    (u32)sub_0807f2b4,
    (u32)sub_0807f348,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuTeleportInit, KabuTeleportEnterState */
u32 gKabuTeleportStates[3] ACTOR_TBL(08741380) = {
    (u32)sub_0807f42c,
    (u32)KabuTeleport,
    (u32)sub_0807f78c,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in KabuTeleportUpdate */
u32 gKabuTeleportStateUpdates[3] ACTOR_TBL(08741380) = {
    (u32)sub_0807f488,
    (u32)sub_0807f634,
    (u32)sub_0807f888,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KabuSlideInit, KabuSlideEnterState */
u32 gKabuSlideStates[2] ACTOR_TBL(08741380) = {
    (u32)sub_0807f920,
    (u32)sub_0807fa98,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KabuSlideUpdate */
u32 gKabuSlideStateUpdates[2] ACTOR_TBL(08741380) = {
    (u32)sub_0807f9a0,
    (u32)sub_0807fafc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KabuIdleInit, sub_0807fb44 */
u32 gKabuIdleStates[1] ACTOR_TBL(08741380) = {
    (u32)KabuIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KabuIdleUpdate */
u32 gKabuIdleStateUpdates[1] ACTOR_TBL(08741380) = {
    (u32)sub_0807fbcc,
};

/* ---- 0x08741488-0x087414B0: 5 table(s), 10 function pointer(s), section .actor_tbl_08741488 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Twister */
u32 gTwisterVariants[2] ACTOR_TBL(08741488) = {
    (u32)TwisterInit,
    (u32)TwisterIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwisterInit, TwisterEnterState */
u32 gTwisterStates[3] ACTOR_TBL(08741488) = {
    (u32)sub_0807fdc8,
    (u32)sub_0807ffa0,
    (u32)sub_080801cc,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwisterUpdate */
u32 gTwisterStateUpdates[3] ACTOR_TBL(08741488) = {
    (u32)sub_0807fe18,
    (u32)sub_0808003c,
    (u32)sub_08080278,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwisterIdleInit, sub_08080300 */
u32 gTwisterIdleStates[1] ACTOR_TBL(08741488) = {
    (u32)TwisterIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwisterIdleUpdate */
u32 gTwisterIdleStateUpdates[1] ACTOR_TBL(08741488) = {
    (u32)sub_08080358,
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
    (u32)sub_0808051c,
    (u32)sub_080806e8,
    (u32)sub_08080768,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadIdleInit, sub_080807bc */
u32 gHotHeadIdleStates[1] ACTOR_TBL(087414b4) = {
    (u32)HotHeadIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in HotHeadIdleUpdate */
u32 gHotHeadIdleStateUpdates[1] ACTOR_TBL(087414b4) = {
    (u32)sub_08080814,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadStandInit, HotHeadStandEnterState */
u32 gHotHeadStandStates[3] ACTOR_TBL(087414b4) = {
    (u32)sub_080808bc,
    (u32)HotHeadStandShoot,
    (u32)HotHeadStandFall,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in HotHeadStandUpdate */
u32 gHotHeadStandStateUpdates[3] ACTOR_TBL(087414b4) = {
    (u32)sub_080808dc,
    (u32)sub_08080aa8,
    (u32)sub_08080b28,
};

/* ---- 0x08741544-0x087415AC: 9 table(s), 26 function pointer(s), section .actor_tbl_08741544 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Starman */
u32 gStarmanVariants[4] ACTOR_TBL(08741544) = {
    (u32)sub_08080e10,
    (u32)StarmanJumpInit,
    (u32)StarmanFlyInit,
    (u32)StarmanIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in sub_08080e10, sub_08080e40 */
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
    (u32)sub_080814b4,
    (u32)sub_08081560,
    (u32)sub_080815dc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanFlyInit, sub_0808164c */
u32 gStarmanFlyStates[1] ACTOR_TBL(08741544) = {
    (u32)sub_0808168c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanFlyUpdate */
u32 gStarmanFlyStateUpdates[1] ACTOR_TBL(08741544) = {
    (u32)sub_080816e8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanIdleInit, sub_080817b8 */
u32 gStarmanIdleStates[1] ACTOR_TBL(08741544) = {
    (u32)StarmanIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in StarmanIdleUpdate */
u32 gStarmanIdleStateUpdates[1] ACTOR_TBL(08741544) = {
    (u32)sub_08081810,
};

/* ---- 0x087415B8-0x087415E4: 5 table(s), 11 function pointer(s), section .actor_tbl_087415b8 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_PoppyBrosJr */
u32 gPoppyBrosJrVariants[3] ACTOR_TBL(087415b8) = {
    (u32)PoppyBrosJrInit,
    (u32)PoppyBrosJrInit,
    (u32)sub_08081d24,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in PoppyBrosJrInit, PoppyBrosJrEnterState */
u32 gPoppyBrosJrStates[3] ACTOR_TBL(087415b8) = {
    (u32)sub_08081aac,
    (u32)PoppyBrosJrWalk,
    (u32)PoppyBrosJrJump,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in PoppyBrosJrUpdate */
u32 gPoppyBrosJrStateUpdates[3] ACTOR_TBL(087415b8) = {
    (u32)sub_08081b5c,
    (u32)sub_08081c3c,
    (u32)sub_08081ce0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08081d24, sub_08081d68 */
u32 gUnk_087415DC[1] ACTOR_TBL(087415b8) = {
    (u32)sub_08081db4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08081d84 */
u32 gUnk_087415E0[1] ACTOR_TBL(087415b8) = {
    (u32)sub_08081e40,
};

/* ---- 0x08741604-0x08741628: 7 table(s), 9 function pointer(s), section .actor_tbl_08741604 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_PoppyBrosJrOnApple, Task_PoppyBrosJrOnMaximTomato */
u32 gPoppyBrosJrRideVariants[3] ACTOR_TBL(08741604) = {
    (u32)PoppyBrosJrRideInit,
    (u32)PoppyBrosJrDroppedObjectInit,
    (u32)PoppyBrosJrRideIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideInit, sub_080820ec */
u32 gPoppyBrosJrRideStates[1] ACTOR_TBL(08741604) = {
    (u32)sub_08082270,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideUpdate */
u32 gPoppyBrosJrRideStateUpdates[1] ACTOR_TBL(08741604) = {
    (u32)sub_080822a4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrDroppedObjectInit, sub_080822e4 */
u32 gPoppyBrosJrDroppedObjectStates[1] ACTOR_TBL(08741604) = {
    (u32)sub_08082338,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrDroppedObjectUpdate */
u32 gPoppyBrosJrDroppedObjectStateUpdates[1] ACTOR_TBL(08741604) = {
    (u32)sub_08082458,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in PoppyBrosJrRideIdleInit, sub_080824d0 */
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
    (u32)sub_0808279c,
    (u32)sub_08082844,
    (u32)sub_080828a8,
    (u32)sub_08082980,
    (u32)sub_08082a08,
    (u32)sub_08082b14,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in WheelieUpdate */
u32 gWheelieStateUpdates[6] ACTOR_TBL(08741640) = {
    (u32)sub_08082818,
    (u32)sub_0808287c,
    (u32)sub_08082908,
    (u32)sub_080829d4,
    (u32)sub_08082aec,
    (u32)sub_08082b48,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WheelieIdleInit, sub_08082bfc */
u32 gWheelieIdleStates[1] ACTOR_TBL(08741640) = {
    (u32)WheelieIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in WheelieIdleUpdate */
u32 gWheelieIdleStateUpdates[1] ACTOR_TBL(08741640) = {
    (u32)sub_08082c58,
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
    (u32)sub_08082f04,
    (u32)sub_08082fdc,
    (u32)FlamerFall,
    (u32)sub_08083370,
    (u32)sub_08083428,
    (u32)sub_08083614,
    (u32)sub_080837d0,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in FlamerUpdate */
u32 gFlamerStateUpdates[7] ACTOR_TBL(0874176c) = {
    (u32)sub_08082fb4,
    (u32)sub_08083020,
    (u32)sub_0808330c,
    (u32)sub_08083400,
    (u32)sub_08083488,
    (u32)sub_0808379c,
    (u32)sub_080838bc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in FlamerIdleInit, sub_080839d0 */
u32 gFlamerIdleStates[1] ACTOR_TBL(0874176c) = {
    (u32)FlamerIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in FlamerIdleUpdate */
u32 gFlamerIdleStateUpdates[1] ACTOR_TBL(0874176c) = {
    (u32)sub_08083a44,
};

/* ---- 0x08741E64-0x08741E70: 3 table(s), 3 function pointer(s), section .actor_tbl_08741e64 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_SirKibbleCutter */
u32 gUnk_08741E64[1] ACTOR_TBL(08741e64) = {
    (u32)sub_08083eb4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08083eb4, sub_08083ee8 */
u32 gUnk_08741E68[1] ACTOR_TBL(08741e64) = {
    (u32)sub_08083f48,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08083f04 */
u32 gUnk_08741E6C[1] ACTOR_TBL(08741e64) = {
    (u32)sub_08083fbc,
};

/* ---- 0x08741E7C-0x08741E94: 5 table(s), 6 function pointer(s), section .actor_tbl_08741e7c ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_HotHeadFire */
u32 gHotHeadFireVariants[2] ACTOR_TBL(08741e7c) = {
    (u32)sub_080840a4,
    (u32)sub_0808424c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080840a4, sub_080840d4 */
u32 gUnk_08741E84[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_08084114,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080840f0 */
u32 gUnk_08741E88[1] ACTOR_TBL(08741e7c) = {
    (u32)sub_08084248,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808424c, sub_0808429c */
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
    (u32)sub_08084a74,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NoddyInit, NoddyEnterState */
u32 gNoddyStates[6] ACTOR_TBL(08741f70) = {
    (u32)NoddyWalk,
    (u32)sub_080847fc,
    (u32)sub_0808487c,
    (u32)sub_08084960,
    (u32)sub_080849dc,
    (u32)sub_080846f4,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in NoddyUpdate */
u32 gNoddyStateUpdates[6] ACTOR_TBL(08741f70) = {
    (u32)sub_080846c4,
    (u32)sub_08084854,
    (u32)sub_080848e4,
    (u32)sub_080849b4,
    (u32)sub_08084a50,
    (u32)sub_080847f8,
};

/* ---- 0x08741FB8-0x08742010: 6 table(s), 22 function pointer(s), section .actor_tbl_08741fb8 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Chilly */
u32 gChillyVariants[2] ACTOR_TBL(08741fb8) = {
    (u32)ChillyInit,
    (u32)ChillyIdle,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in ChillyInit, ChillyEnterState */
u32 gChillyStates[5] ACTOR_TBL(08741fb8) = {
    (u32)sub_08084dc0,
    (u32)sub_08084e9c,
    (u32)sub_08084f78,
    (u32)sub_08085180,
    (u32)ChillyFall,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in ChillyUpdate */
u32 gChillyStateUpdates[5] ACTOR_TBL(08741fb8) = {
    (u32)sub_08084e74,
    (u32)sub_08084f50,
    (u32)sub_08085158,
    (u32)sub_08085274,
    (u32)sub_080852c8,
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
    (u32)sub_08085608,
    (u32)sub_080856dc,
    (u32)sub_0808582c,
};

/* ---- 0x08742030-0x08742050: 2 table(s), 8 function pointer(s), section .actor_tbl_08742030 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in ParasolWaddleDooInit, ParasolWaddleDooEnterState */
u32 gParasolWaddleDooStates[4] ACTOR_TBL(08742030) = {
    (u32)ParasolWaddleDooWalk,
    (u32)ParasolWaddleDooJump,
    (u32)ParasolWaddleDooShoot,
    (u32)sub_08085be4,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in ParasolWaddleDooUpdate */
u32 gParasolWaddleDooStateUpdates[4] ACTOR_TBL(08742030) = {
    (u32)sub_08085998,
    (u32)sub_08085a80,
    (u32)sub_08085bb8,
    (u32)sub_08085c10,
};

/* ---- 0x08742064-0x08742088: 3 table(s), 9 function pointer(s), section .actor_tbl_08742064 ---- */
/* include/enemy.h; CallTableEntry(i, 7, ...) in Task_BrontoBurt, sub_080860d8 */
u32 gBrontoBurtVariants[7] ACTOR_TBL(08742064) = {
    (u32)BrontoBurtWaveInit,
    (u32)sub_080862cc,
    (u32)sub_080864ec,
    (u32)BrontoBurtDiagonalInit,
    (u32)BrontoBurtChaseInit,
    (u32)BrontoBurtTakeOffInit,
    (u32)BrontoBurtIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWaveInit, sub_08086128 */
u32 gBrontoBurtWaveStates[1] ACTOR_TBL(08742064) = {
    (u32)sub_08086170,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtWaveUpdate */
u32 gBrontoBurtWaveStateUpdates[1] ACTOR_TBL(08742064) = {
    (u32)sub_08086274,
};

/* ---- 0x087420A0-0x087420AC: 3 table(s), 3 function pointer(s), section .actor_tbl_087420a0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080862cc, sub_080862fc */
u32 gUnk_087420A0[1] ACTOR_TBL(087420a0) = {
    (u32)sub_08086344,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08086320 */
u32 gUnk_087420A4[1] ACTOR_TBL(087420a0) = {
    (u32)sub_08086444,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080864ec */
s32 gUnk_087420A8[1] ACTOR_TBL(087420a0) = {
    (s32)sub_080865c0,
};

/* ---- 0x087420BC-0x087420C0: 1 table(s), 1 function pointer(s), section .actor_tbl_087420bc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808659c */
u32 gUnk_087420BC[1] ACTOR_TBL(087420bc) = {
    (u32)sub_080867b8,
};

/* ---- 0x087420F0-0x087420F4: 1 table(s), 1 function pointer(s), section .actor_tbl_087420f0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtDiagonalInit, sub_080868b8 */
u32 gBrontoBurtDiagonalStates[1] ACTOR_TBL(087420f0) = {
    (u32)sub_08086984,
};

/* ---- 0x08742100-0x0874210C: 3 table(s), 3 function pointer(s), section .actor_tbl_08742100 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtDiagonalUpdate */
u32 gBrontoBurtDiagonalStateUpdates[1] ACTOR_TBL(08742100) = {
    (u32)sub_080869b8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtChaseInit, sub_08086a20 */
u32 gBrontoBurtChaseStates[1] ACTOR_TBL(08742100) = {
    (u32)sub_08086a68,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BrontoBurtChaseUpdate */
u32 gBrontoBurtChaseStateUpdates[1] ACTOR_TBL(08742100) = {
    (u32)sub_08086b68,
};

/* ---- 0x0874212C-0x08742144: 2 table(s), 6 function pointer(s), section .actor_tbl_0874212c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in BrontoBurtTakeOffInit, BrontoBurtTakeOffEnterState */
u32 gBrontoBurtTakeOffStates[3] ACTOR_TBL(0874212c) = {
    (u32)sub_08086c5c,
    (u32)sub_08086d18,
    (u32)sub_08086df0,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in BrontoBurtTakeOffUpdate */
u32 gBrontoBurtTakeOffStateUpdates[3] ACTOR_TBL(0874212c) = {
    (u32)sub_08086ccc,
    (u32)sub_08086da4,
    (u32)sub_08086ec8,
};

/* ---- 0x08742570-0x08742598: 1 table(s), 10 function pointer(s), section .actor_tbl_08742570 ---- */
/* include/enemy.h; CallTableEntry(i, 10, ...) in Task_Twizzy, sub_080870a4 */
u32 gTwizzyVariants[10] ACTOR_TBL(08742570) = {
    (u32)TwizzyWaveInit,
    (u32)sub_08087268,
    (u32)sub_08087458,
    (u32)TwizzyDiagonalInit,
    (u32)TwizzyChaseInit,
    (u32)TwizzyTakeOffInit,
    (u32)sub_08087e2c,
    (u32)sub_08087fcc,
    (u32)sub_08088360,
    (u32)TwizzyIdle,
};

/* ---- 0x087425B0-0x087425B8: 2 table(s), 2 function pointer(s), section .actor_tbl_087425b0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWaveInit, sub_080870f4 */
u32 gTwizzyWaveStates[1] ACTOR_TBL(087425b0) = {
    (u32)sub_0808713c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyWaveUpdate */
u32 gTwizzyWaveStateUpdates[1] ACTOR_TBL(087425b0) = {
    (u32)sub_08087210,
};

/* ---- 0x087425D0-0x087425DC: 3 table(s), 3 function pointer(s), section .actor_tbl_087425d0 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08087268, sub_08087298 */
u32 gUnk_087425D0[1] ACTOR_TBL(087425d0) = {
    (u32)sub_080872e0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080872bc */
u32 gUnk_087425D4[1] ACTOR_TBL(087425d0) = {
    (u32)sub_080873b0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08087458 */
s32 gUnk_087425D8[1] ACTOR_TBL(087425d0) = {
    (s32)sub_0808752c,
};

/* ---- 0x087425EC-0x087425F0: 1 table(s), 1 function pointer(s), section .actor_tbl_087425ec ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08087508 */
u32 gUnk_087425EC[1] ACTOR_TBL(087425ec) = {
    (u32)sub_08087724,
};

/* ---- 0x087425F8-0x08742600: 2 table(s), 2 function pointer(s), section .actor_tbl_087425f8 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyDiagonalInit, sub_08087824 */
u32 gTwizzyDiagonalStates[1] ACTOR_TBL(087425f8) = {
    (u32)sub_080878f0,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyDiagonalUpdate */
u32 gTwizzyDiagonalStateUpdates[1] ACTOR_TBL(087425f8) = {
    (u32)sub_08087924,
};

/* ---- 0x0874260C-0x08742614: 2 table(s), 2 function pointer(s), section .actor_tbl_0874260c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyChaseInit, sub_0808798c */
u32 gTwizzyChaseStates[1] ACTOR_TBL(0874260c) = {
    (u32)sub_080879d4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in TwizzyChaseUpdate */
u32 gTwizzyChaseStateUpdates[1] ACTOR_TBL(0874260c) = {
    (u32)sub_08087a98,
};

/* ---- 0x0874263C-0x0874269C: 6 table(s), 24 function pointer(s), section .actor_tbl_0874263c ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwizzyTakeOffInit, TwizzyTakeOffEnterState */
u32 gTwizzyTakeOffStates[3] ACTOR_TBL(0874263c) = {
    (u32)sub_08087b8c,
    (u32)sub_08087c48,
    (u32)sub_08087d20,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in TwizzyTakeOffUpdate */
u32 gTwizzyTakeOffStateUpdates[3] ACTOR_TBL(0874263c) = {
    (u32)sub_08087bfc,
    (u32)sub_08087cd4,
    (u32)sub_08087df8,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08087e2c, sub_08087e60 */
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
/* include/enemy.h; CallTableEntry(i, 6, ...) in sub_08087fcc, sub_08088000 */
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
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08088360, sub_08088394 */
u32 gUnk_087426AC[1] ACTOR_TBL(087426ac) = {
    (u32)sub_080883dc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080883b8 */
u32 gUnk_087426B0[1] ACTOR_TBL(087426ac) = {
    (u32)sub_08088478,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_Squishy */
u32 gSquishyVariants[4] ACTOR_TBL(087426ac) = {
    (u32)SquishyWalkInit,
    (u32)sub_08088a64,
    (u32)sub_08088ce8,
    (u32)SquishyIdle,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in SquishyWalkInit, SquishyWalkEnterState, sub_08088ce8 */
u32 gSquishyWalkStates[5] ACTOR_TBL(087426ac) = {
    (u32)SquishyWalk,
    (u32)sub_0808880c,
    (u32)sub_080888c8,
    (u32)sub_08088948,
    (u32)sub_080889cc,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in SquishyWalkUpdate */
u32 gSquishyWalkStateUpdates[5] ACTOR_TBL(087426ac) = {
    (u32)sub_080887a0,
    (u32)sub_080888a0,
    (u32)sub_08088920,
    (u32)sub_080889c8,
    (u32)sub_08088a3c,
};

/* ---- 0x08742704-0x08742734: 4 table(s), 12 function pointer(s), section .actor_tbl_08742704 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08088a64, sub_08088aec */
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
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_08088ce8, sub_08088d58 */
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
    (u32)sub_0808924c,
    (u32)sub_08089334,
    (u32)sub_08089460,
    (u32)sub_08089544,
    (u32)sub_080895ec,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in BubblesUpdate */
u32 gBubblesStateUpdates[5] ACTOR_TBL(0874273c) = {
    (u32)sub_08089330,
    (u32)sub_0808945c,
    (u32)sub_08089530,
    (u32)sub_080895c4,
    (u32)sub_0808967c,
};

/* ---- 0x08742798-0x087427B0: 3 table(s), 6 function pointer(s), section .actor_tbl_08742798 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Glunk */
u32 gGlunkVariants[2] ACTOR_TBL(08742798) = {
    (u32)GlunkInit,
    (u32)GlunkIdle,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in GlunkInit, GlunkEnterState */
u32 gGlunkStates[2] ACTOR_TBL(08742798) = {
    (u32)sub_0808990c,
    (u32)GlunkShoot,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in GlunkUpdate */
u32 gGlunkStateUpdates[2] ACTOR_TBL(08742798) = {
    (u32)sub_080899d4,
    (u32)sub_08089aac,
};

/* ---- 0x087427B4-0x08742814: 3 table(s), 24 function pointer(s), section .actor_tbl_087427b4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_Slippy */
u32 gSlippyVariants[2] ACTOR_TBL(087427b4) = {
    (u32)SlippyInit,
    (u32)SlippyIdle,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in SlippyInit, SlippyEnterState */
u32 gSlippyStates[11] ACTOR_TBL(087427b4) = {
    (u32)sub_08089da8,
    (u32)sub_08089ec8,
    (u32)sub_0808a048,
    (u32)sub_0808a0ac,
    (u32)sub_0808a110,
    (u32)sub_0808a204,
    (u32)sub_0808a298,
    (u32)sub_0808a3c4,
    (u32)sub_0808a4d0,
    (u32)sub_0808a610,
    (u32)SlippyFall,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in SlippyUpdate */
u32 gSlippyStateUpdates[11] ACTOR_TBL(087427b4) = {
    (u32)sub_08089ea0,
    (u32)sub_0808a020,
    (u32)sub_0808a0a8,
    (u32)sub_0808a10c,
    (u32)sub_0808a1d0,
    (u32)sub_0808a270,
    (u32)sub_0808a36c,
    (u32)sub_0808a478,
    (u32)sub_0808a5b8,
    (u32)sub_0808a710,
    (u32)sub_0808a7a4,
};

/* ---- 0x087428C0-0x087428E8: 3 table(s), 10 function pointer(s), section .actor_tbl_087428c0 ---- */
/* include/enemy.h; CallTableEntry(i, 6, ...) in Task_Blipper */
u32 gBlipperVariants[6] ACTOR_TBL(087428c0) = {
    (u32)sub_0808aad8,
    (u32)sub_0808adec,
    (u32)sub_0808af34,
    (u32)sub_0808b1d0,
    (u32)sub_0808b1d0,
    (u32)BlipperIdle,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808aad8, sub_0808ab14 */
u32 gUnk_087428D8[2] ACTOR_TBL(087428c0) = {
    (u32)sub_0808ab70,
    (u32)sub_0808acdc,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808ab38 */
u32 gUnk_087428E0[2] ACTOR_TBL(087428c0) = {
    (u32)sub_0808abf4,
    (u32)sub_0808ad20,
};

/* ---- 0x08742908-0x08742930: 6 table(s), 10 function pointer(s), section .actor_tbl_08742908 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808adec, sub_0808ae24 */
u32 gUnk_08742908[1] ACTOR_TBL(08742908) = {
    (u32)sub_0808ae80,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808ae48 */
u32 gUnk_0874290C[1] ACTOR_TBL(08742908) = {
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
    (u32)sub_0808bfc4,
    (u32)sub_0808c02c,
    (u32)sub_0808c260,
    (u32)sub_0808c0fc,
    (u32)sub_0808c32c,
    (u32)sub_0808c478,
    (u32)sub_0808c4e4,
    (u32)sub_0808c53c,
    (u32)GipShoot,
};
/* include/enemy.h; CallTableEntry(i, 8, ...) in GipUpdate */
u32 gGipStateUpdates[8] ACTOR_TBL(08742940) = {
    (u32)sub_0808bf1c,
    (u32)sub_0808c004,
    (u32)sub_0808c1d0,
    (u32)sub_0808c3e8,
    (u32)sub_0808c4bc,
    (u32)sub_0808c538,
    (u32)sub_0808c5e8,
    (u32)sub_0808c684,
};

/* ---- 0x08743188-0x087431CC: 7 table(s), 17 function pointer(s), section .actor_tbl_08743188 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_BroomHatter */
u32 gBroomHatterVariants[3] ACTOR_TBL(08743188) = {
    (u32)sub_0808d558,
    (u32)sub_0808da00,
    (u32)BroomHatterIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0808d558, sub_0808d624 */
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
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0808da00, sub_0808dacc */
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
    (u32)sub_0808e050,
};

/* ---- 0x087431E4-0x08743214: 5 table(s), 12 function pointer(s), section .actor_tbl_087431e4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_LaserBall */
u32 gLaserBallVariants[2] ACTOR_TBL(087431e4) = {
    (u32)LaserBallInit,
    (u32)LaserBallIdleInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in LaserBallInit, LaserBallEnterState */
u32 gLaserBallStates[4] ACTOR_TBL(087431e4) = {
    (u32)sub_0808e480,
    (u32)LaserBallShoot,
    (u32)sub_0808e54c,
    (u32)sub_0808e730,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in LaserBallUpdate */
u32 gLaserBallStateUpdates[4] ACTOR_TBL(087431e4) = {
    (u32)sub_0808e510,
    (u32)sub_0808e704,
    (u32)sub_0808e5cc,
    (u32)sub_0808e800,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in LaserBallIdleInit */
u32 gLaserBallIdleStates[1] ACTOR_TBL(087431e4) = {
    (u32)LaserBallIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in LaserBallIdleUpdate */
u32 gLaserBallIdleStateUpdates[1] ACTOR_TBL(087431e4) = {
    (u32)sub_0808e8a0,
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
    (u32)sub_0808eb10,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutUpdate */
u32 gCoconutStateUpdates[1] ACTOR_TBL(08743224) = {
    (u32)sub_0808e9d4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutIdleInit */
u32 gCoconutIdleStates[1] ACTOR_TBL(08743224) = {
    (u32)CoconutIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in CoconutIdleUpdate */
u32 gCoconutIdleStateUpdates[1] ACTOR_TBL(08743224) = {
    (u32)sub_0808ebdc,
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
    (u32)sub_0808f3d4,
    (u32)ShotzoAimFall,
    (u32)ShotzoAimShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoAimUpdate */
u32 gShotzoAimStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)sub_0808f400,
    (u32)sub_0808f51c,
    (u32)sub_0808f4b4,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoFixedInit, ShotzoFixedEnterState */
u32 gShotzoFixedStates[3] ACTOR_TBL(08743284) = {
    (u32)sub_0808f528,
    (u32)ShotzoFixedFall,
    (u32)ShotzoFixedShoot,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ShotzoFixedUpdate */
u32 gShotzoFixedStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)sub_0808f578,
    (u32)sub_0808f71c,
    (u32)sub_0808f678,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ParasolShotzoInit, ParasolShotzoEnterState */
u32 gParasolShotzoStates[3] ACTOR_TBL(08743284) = {
    (u32)sub_0808f728,
    (u32)ParasolShotzoShoot,
    (u32)sub_0808f888,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in ParasolShotzoUpdate */
u32 gParasolShotzoStateUpdates[3] ACTOR_TBL(08743284) = {
    (u32)sub_0808f75c,
    (u32)sub_0808f844,
    (u32)sub_0808f8dc,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoIdleInit */
u32 gShotzoIdleStates[1] ACTOR_TBL(08743284) = {
    (u32)ShotzoIdle,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ShotzoIdleUpdate */
u32 gShotzoIdleStateUpdates[1] ACTOR_TBL(08743284) = {
    (u32)sub_0808f974,
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
    (u32)sub_0808fb50,
    (u32)sub_0808fb80,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in ConerIdleInit */
u32 gConerIdleStates[1] ACTOR_TBL(087432f4) = {
    (u32)ConerIdle,
};

/* ---- 0x08743600-0x08743614: 3 table(s), 5 function pointer(s), section .actor_tbl_08743600 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_LaserBallLaser */
u32 gUnk_08743600[1] ACTOR_TBL(08743600) = {
    (u32)sub_0808fc90,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808fc90, sub_0808fd1c */
u32 gUnk_08743604[2] ACTOR_TBL(08743600) = {
    (u32)sub_0808fd60,
    (u32)sub_0808fd38,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0808fcd4 */
u32 gUnk_0874360C[2] ACTOR_TBL(08743600) = {
    (u32)sub_0808fdb4,
    (u32)sub_0808fd5c,
};

/* ---- 0x0874362C-0x08743644: 3 table(s), 6 function pointer(s), section .actor_tbl_0874362c ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_ShotzoCannonball */
u32 gUnk_0874362C[4] ACTOR_TBL(0874362c) = {
    (u32)sub_0808fdf8,
    (u32)sub_0808fdf8,
    (u32)sub_0808fdf8,
    (u32)sub_0808fdf8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808fdf8, sub_0808fe6c */
u32 gUnk_0874363C[1] ACTOR_TBL(0874362c) = {
    (u32)sub_0808fe88,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0808fe28 */
u32 gUnk_08743640[1] ACTOR_TBL(0874362c) = {
    (u32)sub_0808ffe0,
};

/* ---- 0x08743848-0x087438A4: 3 table(s), 23 function pointer(s), section .actor_tbl_08743848 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Bonkers */
u32 gBonkersVariants[1] ACTOR_TBL(08743848) = {
    (u32)BonkersInit,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in BonkersInit, BonkersEnterState */
u32 gBonkersStates[11] ACTOR_TBL(08743848) = {
    (u32)sub_080901e0,
    (u32)BonkersWalk,
    (u32)BonkersJump,
    (u32)BonkersHop,
    (u32)BonkersDash,
    (u32)BonkersThrow,
    (u32)BonkersSlam,
    (u32)BonkersJumpSlam,
    (u32)BonkersTripleSlam,
    (u32)sub_08090c18,
    (u32)BonkersDefeat,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in BonkersUpdate */
u32 gBonkersStateUpdates[11] ACTOR_TBL(08743848) = {
    (u32)sub_08090270,
    (u32)BonkersWalkUpdate,
    (u32)BonkersJumpUpdate,
    (u32)BonkersHopUpdate,
    (u32)BonkersDashUpdate,
    (u32)BonkersThrowUpdate,
    (u32)BonkersSlamUpdate,
    (u32)BonkersJumpSlamUpdate,
    (u32)BonkersTripleSlamUpdate,
    (u32)sub_08090ca8,
    (u32)BonkersDefeatUpdate,
};

/* ---- 0x08743984-0x087439C0: 3 table(s), 15 function pointer(s), section .actor_tbl_08743984 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_PoppyBrosSr */
u32 gPoppyBrosSrVariants[1] ACTOR_TBL(08743984) = {
    (u32)PoppyBrosSrInit,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in PoppyBrosSrInit, PoppyBrosSrEnterState */
u32 gPoppyBrosSrStates[7] ACTOR_TBL(08743984) = {
    (u32)sub_0809128c,
    (u32)sub_08091320,
    (u32)sub_08091390,
    (u32)sub_080915a4,
    (u32)sub_080915f8,
    (u32)sub_080916ec,
    (u32)PoppyBrosSrDefeat,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in PoppyBrosSrUpdate */
u32 gPoppyBrosSrStateUpdates[7] ACTOR_TBL(08743984) = {
    (u32)sub_080912f8,
    (u32)sub_08091368,
    (u32)sub_08091558,
    (u32)sub_080915d0,
    (u32)sub_080916c4,
    (u32)sub_080917fc,
    (u32)PoppyBrosSrDefeatUpdate,
};

/* ---- 0x08743ADC-0x08743B48: 3 table(s), 27 function pointer(s), section .actor_tbl_08743adc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_Bugzzy */
u32 gBugzzyVariants[1] ACTOR_TBL(08743adc) = {
    (u32)BugzzyInit,
};
/* include/enemy.h; CallTableEntry(i, 13, ...) in BugzzyInit, BugzzyEnterState */
u32 gBugzzyStates[13] ACTOR_TBL(08743adc) = {
    (u32)sub_08092198,
    (u32)sub_08092250,
    (u32)BugzzySummon,
    (u32)sub_080926fc,
    (u32)sub_08092a14,
    (u32)sub_08092b58,
    (u32)sub_08092c00,
    (u32)sub_08092cdc,
    (u32)sub_08092e68,
    (u32)sub_08092f2c,
    (u32)sub_080930ac,
    (u32)sub_0809301c,
    (u32)BugzzyDefeat,
};
/* include/enemy.h; CallTableEntry(i, 13, ...) in BugzzyUpdate */
u32 gBugzzyStateUpdates[13] ACTOR_TBL(08743adc) = {
    (u32)sub_08092228,
    (u32)sub_08092590,
    (u32)BugzzySummonUpdate,
    (u32)sub_080929ec,
    (u32)sub_08092b30,
    (u32)sub_08092bd8,
    (u32)sub_08092cb4,
    (u32)sub_08092e40,
    (u32)sub_08092f04,
    (u32)sub_08092ff4,
    (u32)sub_08093354,
    (u32)sub_08093084,
    (u32)BugzzyDefeatUpdate,
};

/* ---- 0x08744170-0x0874417C: 3 table(s), 3 function pointer(s), section .actor_tbl_08744170 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_BonkersNut */
u32 gUnk_08744170[1] ACTOR_TBL(08744170) = {
    (u32)sub_08093a64,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08093a64 */
u32 gUnk_08744174[1] ACTOR_TBL(08744170) = {
    (u32)sub_08093ac8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_08093a98 */
u32 gUnk_08744178[1] ACTOR_TBL(08744170) = {
    (u32)sub_08093b80,
};

/* ---- 0x087441A4-0x087441BC: 3 table(s), 6 function pointer(s), section .actor_tbl_087441a4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in Task_PoppyBrosSrBomb */
u32 gUnk_087441A4[2] ACTOR_TBL(087441a4) = {
    (u32)sub_08093c30,
    (u32)sub_08093c30,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_08093c30, sub_08093c60 */
u32 gUnk_087441AC[2] ACTOR_TBL(087441a4) = {
    (u32)sub_08093ccc,
    (u32)sub_08093dcc,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_08093c7c */
u32 gUnk_087441B4[2] ACTOR_TBL(087441a4) = {
    (u32)sub_08093cf8,
    (u32)sub_08093e54,
};

/* ---- 0x087441CC-0x087441D8: 3 table(s), 3 function pointer(s), section .actor_tbl_087441cc ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_BugzzyLadybug */
u32 gBugzzyLadybugVariants[1] ACTOR_TBL(087441cc) = {
    (u32)BugzzyLadybugInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BugzzyLadybugInit */
u32 gBugzzyLadybugStates[1] ACTOR_TBL(087441cc) = {
    (u32)sub_08094040,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in BugzzyLadybugUpdate */
u32 gBugzzyLadybugStateUpdates[1] ACTOR_TBL(087441cc) = {
    (u32)sub_08094144,
};

/* ---- 0x08744440-0x0874449C: 3 table(s), 23 function pointer(s), section .actor_tbl_08744440 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_GrandWheelie */
u32 gGrandWheelieVariants[1] ACTOR_TBL(08744440) = {
    (u32)GrandWheelieInit,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in GrandWheelieInit, GrandWheelieEnterState */
u32 gGrandWheelieStates[11] ACTOR_TBL(08744440) = {
    (u32)sub_080945fc,
    (u32)sub_0809465c,
    (u32)sub_080946b0,
    (u32)sub_08094844,
    (u32)GrandWheelieCharge,
    (u32)sub_08094da4,
    (u32)sub_08094f28,
    (u32)sub_08094fb0,
    (u32)GrandWheelieSummon,
    (u32)sub_08095254,
    (u32)GrandWheelieDefeat,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in sub_080942b4, GrandWheelieUpdate */
u32 gGrandWheelieStateUpdates[11] ACTOR_TBL(08744440) = {
    (u32)sub_08094640,
    (u32)sub_0809467c,
    (u32)sub_08094758,
    (u32)sub_080948d4,
    (u32)sub_080949e0,
    (u32)sub_08094dec,
    (u32)sub_08094f68,
    (u32)sub_080950b4,
    (u32)GrandWheelieSummonUpdate,
    (u32)sub_0809532c,
    (u32)sub_08095484,
};

/* ---- 0x087444E4-0x08744510: 1 table(s), 11 function pointer(s), section .actor_tbl_087444e4 ---- */
/* include/enemy.h; CallTableEntry(i, 12, ...) in sub_08095940: the bound exceeds the 11 entries, so index 11 would read the next label, gUnk_08744510 */
u32 gUnk_087444E4[11] ACTOR_TBL(087444e4) = {
    (u32)sub_0809595c,
    (u32)sub_080959ec,
    (u32)sub_08095a54,
    (u32)sub_08095aec,
    (u32)sub_08095aec,
    (u32)sub_08095be8,
    (u32)sub_08095be8,
    (u32)sub_08095eac,
    (u32)sub_080963dc,
    (u32)sub_080960bc,
    (u32)sub_08096b7c,
};

/* ---- 0x08744564-0x08744598: 1 table(s), 13 function pointer(s), section .actor_tbl_08744564 ---- */
/* include/enemy.h; CallTableEntry(i, 13, ...) in sub_0809699c, sub_080969c8 */
u32 gUnk_08744564[13] ACTOR_TBL(08744564) = {
    (u32)sub_080959e8,
    (u32)sub_08095ad0,
    (u32)sub_08095be4,
    (u32)sub_08095d20,
    (u32)sub_08096058,
    (u32)sub_08096640,
    (u32)sub_08096278,
    (u32)sub_080962ac,
    (u32)sub_0809616c,
    (u32)sub_080963c0,
    (u32)sub_08096920,
    (u32)sub_08096d20,
    (u32)sub_08096a28,
};

/* ---- 0x0874489C-0x087448E4: 2 table(s), 18 function pointer(s), section .actor_tbl_0874489c ---- */
/* include/enemy.h; CallTableEntry(i, 9, ...) in sub_080975ac */
u32 gUnk_0874489C[9] ACTOR_TBL(0874489c) = {
    (u32)sub_08097694,
    (u32)sub_0809773c,
    (u32)sub_08097a7c,
    (u32)sub_08097b74,
    (u32)sub_08097c78,
    (u32)sub_08097da4,
    (u32)sub_08097e90,
    (u32)sub_08098194,
    (u32)sub_0809829c,
};
/* include/enemy.h; CallTableEntry(i, 9, ...) in sub_080975c8, sub_080975fc */
u32 gUnk_087448C0[9] ACTOR_TBL(0874489c) = {
    (u32)sub_08097714,
    (u32)sub_08097a54,
    (u32)sub_08097b4c,
    (u32)sub_08097c44,
    (u32)sub_08097d7c,
    (u32)sub_08097e68,
    (u32)sub_0809816c,
    (u32)sub_08098268,
    (u32)sub_080983a0,
};

/* ---- 0x08745630-0x087456CC: 3 table(s), 39 function pointer(s), section .actor_tbl_08745630 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrFrosty */
u32 gMrFrostyVariants[1] ACTOR_TBL(08745630) = {
    (u32)MrFrostyInit,
};
/* include/enemy.h; CallTableEntry(i, 19, ...) in MrFrostyInit, MrFrostyEnterState */
u32 gMrFrostyStates[19] ACTOR_TBL(08745630) = {
    (u32)sub_08098fd0,
    (u32)sub_08099020,
    (u32)sub_080990d4,
    (u32)MrFrostyDash,
    (u32)sub_08099244,
    (u32)sub_080992ac,
    (u32)sub_08099350,
    (u32)sub_080993dc,
    (u32)sub_08099474,
    (u32)sub_08099524,
    (u32)sub_080995f4,
    (u32)sub_080996d0,
    (u32)sub_08099770,
    (u32)MrFrostyDefeat,
    (u32)sub_080998a4,
    (u32)sub_08099944,
    (u32)sub_080999c4,
    (u32)sub_08099a10,
    (u32)sub_08099a7c,
};
/* include/enemy.h; CallTableEntry(i, 19, ...) in MrFrostyUpdate */
u32 gMrFrostyStateUpdates[19] ACTOR_TBL(08745630) = {
    (u32)sub_08099004,
    (u32)sub_08099080,
    (u32)sub_08099180,
    (u32)sub_08099238,
    (u32)sub_080992a8,
    (u32)sub_0809931c,
    (u32)sub_08099394,
    (u32)sub_08099448,
    (u32)sub_08099508,
    (u32)sub_080995b8,
    (u32)sub_08099690,
    (u32)sub_08099734,
    (u32)sub_080997e4,
    (u32)MrFrostyDefeatUpdate,
    (u32)sub_08099908,
    (u32)sub_0809998c,
    (u32)sub_08099a0c,
    (u32)sub_08099a54,
    (u32)sub_08099ad0,
};

/* ---- 0x0874574C-0x08745810: 3 table(s), 49 function pointer(s), section .actor_tbl_0874574c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTock */
u32 gMrTickTockVariants[1] ACTOR_TBL(0874574c) = {
    (u32)MrTickTockInit,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MrTickTockInit, MrTickTockEnterState */
u32 gMrTickTockStates[24] ACTOR_TBL(0874574c) = {
    (u32)sub_0809a214,
    (u32)sub_0809a2a0,
    (u32)sub_0809a2e8,
    (u32)sub_0809a36c,
    (u32)sub_0809a464,
    (u32)sub_0809a528,
    (u32)MrTickTockDash,
    (u32)sub_0809a798,
    (u32)sub_0809a7dc,
    (u32)sub_0809a868,
    (u32)sub_0809a8b8,
    (u32)sub_0809a91c,
    (u32)sub_0809aa24,
    (u32)sub_0809ab70,
    (u32)sub_0809acbc,
    (u32)sub_0809adf4,
    (u32)sub_0809ae40,
    (u32)sub_0809af4c,
    (u32)sub_0809b104,
    (u32)MrTickTockDefeat,
    (u32)sub_0809b2ac,
    (u32)sub_0809b34c,
    (u32)sub_0809b3d4,
    (u32)sub_0809b408,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MrTickTockUpdate */
u32 gMrTickTockStateUpdates[24] ACTOR_TBL(0874574c) = {
    (u32)sub_0809a270,
    (u32)sub_0809a2c0,
    (u32)sub_0809a32c,
    (u32)sub_0809a434,
    (u32)sub_0809a4f0,
    (u32)sub_0809a624,
    (u32)MrTickTockDashUpdate,
    (u32)sub_0809a7d8,
    (u32)sub_0809a82c,
    (u32)sub_0809a8b4,
    (u32)sub_0809a918,
    (u32)sub_0809a974,
    (u32)sub_0809aaf0,
    (u32)sub_0809ac54,
    (u32)sub_0809ad6c,
    (u32)sub_0809ae3c,
    (u32)sub_0809aefc,
    (u32)sub_0809b09c,
    (u32)sub_0809b210,
    (u32)MrTickTockDefeatUpdate,
    (u32)sub_0809b310,
    (u32)sub_0809b394,
    (u32)sub_0809b404,
    (u32)sub_0809b438,
};

/* ---- 0x08745AE4-0x08745B0C: 6 table(s), 10 function pointer(s), section .actor_tbl_08745ae4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrFrostyIceCube */
u32 gUnk_08745AE4[1] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b57c,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0809b57c, sub_0809b5ec */
u32 gUnk_08745AE8[3] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b608,
    (u32)sub_0809b6f8,
    (u32)sub_0809b794,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0809b5b4 */
u32 gUnk_08745AF4[3] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b6ac,
    (u32)sub_0809b790,
    (u32)sub_0809b7ec,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTockRing */
u32 gUnk_08745B00[1] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b830,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0809b830, sub_0809b8ac */
u32 gUnk_08745B04[1] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b8c8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_0809b868 */
u32 gUnk_08745B08[1] ACTOR_TBL(08745ae4) = {
    (u32)sub_0809b964,
};

/* ---- 0x08745B1C-0x08745B30: 3 table(s), 5 function pointer(s), section .actor_tbl_08745b1c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MrTickTockNote */
u32 gUnk_08745B1C[1] ACTOR_TBL(08745b1c) = {
    (u32)sub_0809ba44,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0809ba44, sub_0809baec */
u32 gUnk_08745B20[2] ACTOR_TBL(08745b1c) = {
    (u32)sub_0809bb08,
    (u32)sub_0809bb6c,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0809ba94 */
u32 gUnk_08745B28[2] ACTOR_TBL(08745b1c) = {
    (u32)sub_0809bb68,
    (u32)sub_0809bbd4,
};

/* ---- 0x08747AA4-0x08747B10: 6 table(s), 27 function pointer(s), section .actor_tbl_08747aa4 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_0809c490 */
u32 gAxeKnightVariants[4] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809c4d0,
    (u32)sub_0809cab0,
    (u32)sub_0809cc24,
    (u32)sub_0809cd8c,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in AxeKnightEnterState */
u32 gAxeKnightStates[5] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809c570,
    (u32)sub_0809c74c,
    (u32)sub_0809c880,
    (u32)sub_0809c984,
    (u32)sub_0809ca10,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in AxeKnightUpdate */
u32 gAxeKnightStateUpdates[5] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809c638,
    (u32)sub_0809c840,
    (u32)sub_0809c8f8,
    (u32)sub_0809ca0c,
    (u32)sub_0809caac,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_0809d13c */
u32 gJavelinKnightVariants[2] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809d18c,
    (u32)sub_0809d7a4,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in JavelinKnightEnterState */
u32 gJavelinKnightStates[6] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809d25c,
    (u32)sub_0809d280,
    (u32)sub_0809d30c,
    (u32)sub_0809d4a0,
    (u32)sub_0809d56c,
    (u32)sub_0809d638,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in JavelinKnightUpdate */
u32 gJavelinKnightStateUpdates[5] ACTOR_TBL(08747aa4) = {
    (u32)sub_0809d308,
    (u32)sub_0809d42c,
    (u32)sub_0809d568,
    (u32)sub_0809d608,
    (u32)sub_0809d6b4,
};

/* ---- 0x08747BCC-0x08747C28: 7 table(s), 23 function pointer(s), section .actor_tbl_08747bcc ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_0809dc7c */
u32 gMaceKnightVariants[3] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809dcbc,
    (u32)sub_0809dd7c,
    (u32)sub_0809df54,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MaceKnightEnterState */
u32 gMaceKnightStates[2] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809de54,
    (u32)sub_0809df08,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in MaceKnightUpdate */
u32 gMaceKnightStateUpdates[2] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809dee0,
    (u32)sub_0809df2c,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in Task_MaceKnightMace */
u32 gUnk_08747BE8[3] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809e320,
    (u32)sub_0809e320,
    (u32)sub_0809e670,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_0809e824 */
u32 gTridentKnightVariants[4] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809e874,
    (u32)sub_0809eddc,
    (u32)sub_0809efc8,
    (u32)sub_0809f120,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in TridentKnightEnterState */
u32 gTridentKnightStates[4] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809e914,
    (u32)sub_0809eab8,
    (u32)sub_0809eb14,
    (u32)sub_0809eb7c,
};
/* include/enemy.h; CallTableEntry(i, 5, ...) in TridentKnightUpdate */
u32 gTridentKnightStateUpdates[5] ACTOR_TBL(08747bcc) = {
    (u32)sub_0809ea08,
    (u32)sub_0809eb10,
    (u32)sub_0809eb50,
    (u32)sub_0809ebbc,
    (u32)sub_0809ec80,
};

/* ---- 0x08747C6C-0x08747C80: 1 table(s), 5 function pointer(s), section .actor_tbl_08747c6c ---- */
/* include/enemy.h; CallTableEntry(i, 5, ...) in Task_TridentKnightTrident */
u32 gUnk_08747C6C[5] ACTOR_TBL(08747c6c) = {
    (u32)sub_0809f478,
    (u32)sub_0809f49c,
    (u32)sub_0809f4c0,
    (u32)sub_0809f4e4,
    (u32)sub_0809f508,
};

/* ---- 0x08748264-0x08748268: 1 table(s), 1 function pointer(s), section .actor_tbl_08748264 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDedede */
u32 gKingDededeVariants[1] ACTOR_TBL(08748264) = {
    (u32)KingDededeInit,
};

/* ---- 0x0874844C-0x087484A4: 2 table(s), 22 function pointer(s), section .actor_tbl_0874844c ---- */
/* include/enemy.h; CallTableEntry(i, 11, ...) in KingDededeInit, KingDededeEnterState */
u32 gKingDededeStates[11] ACTOR_TBL(0874844c) = {
    (u32)sub_080a0b30,
    (u32)sub_080a0bb4,
    (u32)sub_080a0c08,
    (u32)KingDededeJump,
    (u32)KingDededeFloat,
    (u32)KingDededeExhale,
    (u32)sub_080a1058,
    (u32)sub_080a1168,
    (u32)KingDededeInhale,
    (u32)KingDededeSpit,
    (u32)KingDededeFall,
};
/* include/enemy.h; CallTableEntry(i, 11, ...) in KingDededeUpdate */
u32 gKingDededeStateUpdates[11] ACTOR_TBL(0874844c) = {
    (u32)sub_080a0b74,
    (u32)sub_080a0bdc,
    (u32)sub_080a0c28,
    (u32)KingDededeJumpUpdate,
    (u32)KingDededeFloatUpdate,
    (u32)KingDededeExhaleUpdate,
    (u32)sub_080a1140,
    (u32)sub_080a11a0,
    (u32)sub_080a12e0,
    (u32)sub_080a1400,
    (u32)KingDededeFallUpdate,
};

/* ---- 0x087484C4-0x087484E4: 3 table(s), 8 function pointer(s), section .actor_tbl_087484c4 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a150c, sub_080a1570 */
u32 gUnk_087484C4[2] ACTOR_TBL(087484c4) = {
    (u32)sub_080a1590,
    (u32)sub_080a18d4,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a1550 */
u32 gUnk_087484CC[2] ACTOR_TBL(087484c4) = {
    (u32)sub_080a15f0,
    (u32)sub_080a1980,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in Task_MrShineAndMrBright */
u32 gMrShineAndMrBrightVariants[4] ACTOR_TBL(087484c4) = {
    (u32)MrShineAndMrBrightInit,
    (u32)MrShineInit,
    (u32)MrBrightInit,
    (u32)sub_080a472c,
};

/* ---- 0x08748624-0x08748764: 6 table(s), 80 function pointer(s), section .actor_tbl_08748624 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in MrShineAndMrBrightInit, MrShineAndMrBrightEnterState */
u32 gMrShineAndMrBrightStates[4] ACTOR_TBL(08748624) = {
    (u32)sub_080a3184,
    (u32)sub_080a31d0,
    (u32)sub_080a3238,
    (u32)sub_080a3268,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in MrShineAndMrBrightUpdate */
u32 gMrShineAndMrBrightStateUpdates[4] ACTOR_TBL(08748624) = {
    (u32)sub_080a31a4,
    (u32)sub_080a31f0,
    (u32)sub_080a3250,
    (u32)sub_080a3280,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrShineInit, MrShineEnterState */
u32 gMrShineStates[18] ACTOR_TBL(08748624) = {
    (u32)MrShineFall,
    (u32)MrShineWait,
    (u32)MrShineAscend,
    (u32)sub_080a349c,
    (u32)sub_080a34c8,
    (u32)sub_080a352c,
    (u32)MrShineChase,
    (u32)sub_080a35b8,
    (u32)MrShineDescend,
    (u32)MrShineWalk,
    (u32)MrShineJump,
    (u32)sub_080a37ac,
    (u32)MrShineDash,
    (u32)MrShineRecoil,
    (u32)MrShineThrow,
    (u32)sub_080a3b4c,
    (u32)sub_080a3b7c,
    (u32)sub_080a3bc0,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrShineUpdate */
u32 gMrShineStateUpdates[18] ACTOR_TBL(08748624) = {
    (u32)MrShineFallUpdate,
    (u32)MrShineWaitUpdate,
    (u32)MrShineAscendUpdate,
    (u32)sub_080a34b8,
    (u32)sub_080a34f0,
    (u32)sub_080a3548,
    (u32)MrShineChaseUpdate,
    (u32)sub_080a35d8,
    (u32)MrShineDescendUpdate,
    (u32)MrShineWalkUpdate,
    (u32)MrShineJumpUpdate,
    (u32)sub_080a3840,
    (u32)MrShineDashUpdate,
    (u32)MrShineRecoilUpdate,
    (u32)MrShineThrowUpdate,
    (u32)sub_080a3b78,
    (u32)sub_080a3ba0,
    (u32)sub_080a3c08,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrBrightInit, MrBrightEnterState */
u32 gMrBrightStates[18] ACTOR_TBL(08748624) = {
    (u32)MrBrightFall,
    (u32)MrBrightWait,
    (u32)MrBrightAscend,
    (u32)sub_080a3e60,
    (u32)sub_080a3ea0,
    (u32)sub_080a3f24,
    (u32)MrBrightChase,
    (u32)sub_080a3ff8,
    (u32)MrBrightDescend,
    (u32)sub_080a408c,
    (u32)MrBrightJump,
    (u32)sub_080a4260,
    (u32)MrBrightDash,
    (u32)MrBrightRecoil,
    (u32)MrBrightThrow,
    (u32)sub_080a4604,
    (u32)sub_080a4634,
    (u32)sub_080a4678,
};
/* include/enemy.h; CallTableEntry(i, 18, ...) in MrBrightUpdate */
u32 gMrBrightStateUpdates[18] ACTOR_TBL(08748624) = {
    (u32)MrBrightFallUpdate,
    (u32)MrBrightWaitUpdate,
    (u32)MrBrightAscendUpdate,
    (u32)sub_080a3e7c,
    (u32)sub_080a3edc,
    (u32)sub_080a3f54,
    (u32)MrBrightChaseUpdate,
    (u32)sub_080a4018,
    (u32)MrBrightDescendUpdate,
    (u32)sub_080a412c,
    (u32)MrBrightJumpUpdate,
    (u32)sub_080a42f8,
    (u32)MrBrightDashUpdate,
    (u32)MrBrightRecoilUpdate,
    (u32)MrBrightThrowUpdate,
    (u32)sub_080a4630,
    (u32)sub_080a4658,
    (u32)sub_080a46c0,
};

/* ---- 0x087489B4-0x087489C0: 3 table(s), 3 function pointer(s), section .actor_tbl_087489b4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDededeStar */
u32 gUnk_087489B4[1] ACTOR_TBL(087489b4) = {
    (u32)sub_080a4a1c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080a4a1c, sub_080a4aa8 */
u32 gUnk_087489B8[1] ACTOR_TBL(087489b4) = {
    (u32)sub_080a4ac4,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080a4a60 */
u32 gUnk_087489BC[1] ACTOR_TBL(087489b4) = {
    (u32)sub_080a4b1c,
};

/* ---- 0x087489D4-0x08748A00: 6 table(s), 11 function pointer(s), section .actor_tbl_087489d4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_KingDededeAirPuff */
u32 gKingDededeAirPuffVariants[1] ACTOR_TBL(087489d4) = {
    (u32)KingDededeAirPuffInit,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeAirPuffInit, sub_080a4c20 */
u32 gKingDededeAirPuffStates[1] ACTOR_TBL(087489d4) = {
    (u32)sub_080a4c3c,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KingDededeAirPuffUpdate */
u32 gKingDededeAirPuffStateUpdates[1] ACTOR_TBL(087489d4) = {
    (u32)sub_080a4c80,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_080a4c84 */
u32 gUnk_087489E0[4] ACTOR_TBL(087489d4) = {
    (u32)sub_080a4cc4,
    (u32)sub_080a5040,
    (u32)sub_080a4e9c,
    (u32)sub_080a5188,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a4cc4, sub_080a4d6c */
u32 gUnk_087489F0[2] ACTOR_TBL(087489d4) = {
    (u32)sub_080a4d88,
    (u32)sub_080a4df4,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a4d00 */
u32 gUnk_087489F8[2] ACTOR_TBL(087489d4) = {
    (u32)sub_080a4dd8,
    (u32)sub_080a4e10,
};

/* ---- 0x08748A28-0x08748A38: 2 table(s), 4 function pointer(s), section .actor_tbl_08748a28 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a4e9c, sub_080a4f24 */
u32 gUnk_08748A28[2] ACTOR_TBL(08748a28) = {
    (u32)sub_080a4f40,
    (u32)sub_080a5020,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a4ee0 */
u32 gUnk_08748A30[2] ACTOR_TBL(08748a28) = {
    (u32)sub_080a5008,
    (u32)sub_080a503c,
};

/* ---- 0x08748A54-0x08748A80: 5 table(s), 11 function pointer(s), section .actor_tbl_08748a54 ---- */
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a5040, sub_080a50f0 */
u32 gUnk_08748A54[2] ACTOR_TBL(08748a54) = {
    (u32)sub_080a510c,
    (u32)sub_080a5168,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a5084 */
u32 gUnk_08748A5C[2] ACTOR_TBL(08748a54) = {
    (u32)sub_080a5164,
    (u32)sub_080a5184,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a5188, sub_080a5220 */
u32 gUnk_08748A64[2] ACTOR_TBL(08748a54) = {
    (u32)sub_080a528c,
    (u32)sub_080a5304,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a51dc */
u32 gUnk_08748A6C[2] ACTOR_TBL(08748a54) = {
    (u32)sub_080a52c8,
    (u32)sub_080a5320,
};
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_080a54e4 */
u32 gUnk_08748A74[3] ACTOR_TBL(08748a54) = {
    (u32)sub_080a5524,
    (u32)sub_080a556c,
    (u32)sub_080a55ac,
};

/* ---- 0x08748EB8-0x08748F8C: 5 table(s), 53 function pointer(s), section .actor_tbl_08748eb8 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_MetaKnight */
u32 gMetaKnightVariants[1] ACTOR_TBL(08748eb8) = {
    (u32)MetaKnightInit,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MetaKnightInit, MetaKnightEnterState */
u32 gMetaKnightStates[24] ACTOR_TBL(08748eb8) = {
    (u32)sub_080a57d4,
    (u32)sub_080a5aa0,
    (u32)sub_080a5dd0,
    (u32)sub_080a5e60,
    (u32)sub_080a5ecc,
    (u32)sub_080a5f20,
    (u32)sub_080a5fac,
    (u32)MetaKnightRun,
    (u32)sub_080a6280,
    (u32)sub_080a61b4,
    (u32)MetaKnightLand,
    (u32)sub_080a63a4,
    (u32)sub_080a63f0,
    (u32)MetaKnightSwordSpin,
    (u32)sub_080a6650,
    (u32)MetaKnightDownThrust,
    (u32)sub_080a6544,
    (u32)sub_080a6698,
    (u32)sub_080a6710,
    (u32)sub_080a6754,
    (u32)MetaKnightDoubleSlash,
    (u32)sub_080a69e4,
    (u32)sub_080a6b3c,
    (u32)sub_080a6d08,
};
/* include/enemy.h; CallTableEntry(i, 24, ...) in MetaKnightUpdate */
u32 gMetaKnightStateUpdates[24] ACTOR_TBL(08748eb8) = {
    (u32)sub_080a5a78,
    (u32)sub_080a5af4,
    (u32)sub_080a5e30,
    (u32)sub_080a5e9c,
    (u32)sub_080a5ef0,
    (u32)sub_080a5f7c,
    (u32)sub_080a6020,
    (u32)MetaKnightRunUpdate,
    (u32)sub_080a6330,
    (u32)sub_080a6264,
    (u32)MetaKnightLandUpdate,
    (u32)sub_080a63d8,
    (u32)sub_080a6420,
    (u32)MetaKnightSwordSpinUpdate,
    (u32)sub_080a6680,
    (u32)MetaKnightDownThrustUpdate,
    (u32)sub_080a6574,
    (u32)sub_080a66c8,
    (u32)sub_080a6734,
    (u32)sub_080a680c,
    (u32)MetaKnightDoubleSlashUpdate,
    (u32)sub_080a6aac,
    (u32)sub_080a6c3c,
    (u32)sub_080a6d60,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a73b4, sub_080a741c */
u32 gUnk_08748F7C[2] ACTOR_TBL(08748eb8) = {
    (u32)sub_080a7438,
    (u32)sub_080a75c8,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in sub_080a73fc */
u32 gUnk_08748F84[2] ACTOR_TBL(08748eb8) = {
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
    (u32)sub_080a7e44,
    (u32)sub_080a85e4,
};
/* include/enemy.h; CallTableEntry(i, 2, ...) in KrackoJrUpdate */
u32 gKrackoJrStateUpdates[2] ACTOR_TBL(08749150) = {
    (u32)sub_080a8038,
    (u32)sub_080a860c,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in KrackoInit, KrackoEnterState */
u32 gKrackoStates[7] ACTOR_TBL(08749150) = {
    (u32)sub_080a8878,
    (u32)sub_080a8970,
    (u32)sub_080a8b6c,
    (u32)sub_080a8bf4,
    (u32)sub_080a8d1c,
    (u32)sub_080a8fdc,
    (u32)KrackoSummon,
};
/* include/enemy.h; CallTableEntry(i, 7, ...) in KrackoUpdate */
u32 gKrackoStateUpdates[7] ACTOR_TBL(08749150) = {
    (u32)sub_080a8948,
    (u32)sub_080a8b44,
    (u32)sub_080a8bcc,
    (u32)sub_080a8c84,
    (u32)sub_080a8f18,
    (u32)sub_080a9304,
    (u32)KrackoSummonUpdate,
};

/* ---- 0x087493F4-0x08749458: 3 table(s), 25 function pointer(s), section .actor_tbl_087493f4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_NightmareWizard */
u32 gNightmareWizardVariants[1] ACTOR_TBL(087493f4) = {
    (u32)NightmareWizardInit,
};
/* include/enemy.h; CallTableEntry(i, 12, ...) in NightmareWizardInit, NightmareWizardEnterState */
u32 gNightmareWizardStates[12] ACTOR_TBL(087493f4) = {
    (u32)sub_080aa47c,
    (u32)sub_080aa560,
    (u32)sub_080aa67c,
    (u32)sub_080aa6d0,
    (u32)sub_080aa744,
    (u32)NightmareWizardOpenCloak,
    (u32)NightmareWizardOpenPalm,
    (u32)NightmareWizardPoint,
    (u32)sub_080aaf6c,
    (u32)sub_080ab1a8,
    (u32)sub_080ab3c8,
    (u32)NightmareWizardHurt,
};
/* include/enemy.h; CallTableEntry(i, 12, ...) in NightmareWizardUpdate */
u32 gNightmareWizardStateUpdates[12] ACTOR_TBL(087493f4) = {
    (u32)sub_080aa52c,
    (u32)sub_080aa62c,
    (u32)sub_080aa6a8,
    (u32)sub_080aa71c,
    (u32)sub_080aa970,
    (u32)NightmareWizardOpenCloakUpdate,
    (u32)NightmareWizardOpenPalmUpdate,
    (u32)NightmareWizardPointUpdate,
    (u32)sub_080ab158,
    (u32)sub_080ab394,
    (u32)sub_080ab418,
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
    (u32)sub_080acb20,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in KrackoStarmanUpdate */
u32 gKrackoStarmanStateUpdates[1] ACTOR_TBL(08749b8c) = {
    (u32)KrackoStarmanCheckParent,
};

/* ---- 0x08749BE4-0x08749BEC: 2 table(s), 2 function pointer(s), section .actor_tbl_08749be4 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_NightmareWizardStar */
u32 gNightmareWizardStarStates[1] ACTOR_TBL(08749be4) = {
    (u32)sub_080acd38,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in NightmareWizardStarUpdate */
u32 gNightmareWizardStarStateUpdates[1] ACTOR_TBL(08749be4) = {
    (u32)sub_080ace60,
};

/* ---- 0x08749D10-0x08749D1C: 1 table(s), 3 function pointer(s), section .actor_tbl_08749d10 ---- */
/* include/enemy.h; CallTableEntry(i, 3, ...) in sub_080acf48 */
u32 gUnk_08749D10[3] ACTOR_TBL(08749d10) = {
    (u32)Task_PaintRoller,
    (u32)sub_080acf68,
    (u32)sub_080acffc,
};

/* ---- 0x0874AD34-0x0874AD44: 1 table(s), 4 function pointer(s), section .actor_tbl_0874ad34 ---- */
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_080ae470 */
u32 gUnk_0874AD34[4] ACTOR_TBL(0874ad34) = {
    (u32)sub_080af114,
    (u32)sub_080af144,
    (u32)sub_080af188,
    (u32)sub_080af1c8,
};

/* ---- 0x0874AD74-0x0874B1A8: 7 script(s), section .actor_tbl_0874ad74 ---- */
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 13 words up to its -2 (restart) */
u32 gUnk_0874AD74[13] ACTOR_TBL(0874ad74) = {
    -4, 4, (u32)sub_080af100,
    -4, 5, (u32)sub_080af100,
    -4, 6, (u32)sub_080af100,
    -4, 7, (u32)sub_080af100,
    -2,
};
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 65 words up to its -2 (restart) */
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
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 57 words up to its -2 (restart) */
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
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 59 words up to its -1 (stop) */
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
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 9 words up to its -2 (restart) */
u32 gUnk_0874B07C[9] ACTOR_TBL(0874ad74) = {
    4, 1,
    6, 1,
    5, 1,
    7, 1,
    -2,
};
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 9 words up to its -2 (restart) */
u32 gUnk_0874B0A0[9] ACTOR_TBL(0874ad74) = {
    4, 4,
    5, 4,
    6, 4,
    7, 4,
    -2,
};
/* include/enemy.h; sub_080aefd4 installs it as Actor.animScript, sub_080af020 runs it: 57 words up to its -2 (restart) */
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
    (u32)sub_080af38c,
    (u32)sub_080af4a4,
    (u32)sub_080af5bc,
    (u32)sub_080af6c8,
    (u32)sub_080af844,
    (u32)sub_080af938,
    (u32)sub_080af9e4,
    (u32)sub_080afaa4,
    (u32)sub_080afb50,
    (u32)sub_080afc10,
    (u32)sub_080afcd4,
    (u32)sub_080afd9c,
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
/* include/enemy.h; CallTableEntry(i, 6, ...) in Task_HeavyMoleUpperArm, Task_HeavyMoleLowerArm, sub_080b1a00 */
u32 gUnk_0874B5E4[6] ACTOR_TBL(0874b5e4) = {
    (u32)sub_080b1a1c,
    (u32)sub_080b1c04,
    (u32)sub_080b1d98,
    (u32)sub_080b1ef8,
    (u32)sub_080b21a0,
    (u32)sub_080b20d4,
};
/* include/enemy.h; CallTableEntry(i, 6, ...) in sub_080b1910 */
u32 gUnk_0874B5FC[6] ACTOR_TBL(0874b5e4) = {
    (u32)sub_080b1b2c,
    (u32)sub_080b1d2c,
    (u32)sub_080b1e20,
    (u32)sub_080b1f80,
    (u32)sub_080b2228,
    (u32)sub_080b214c,
};

/* ---- 0x0874C12C-0x0874C158: 5 table(s), 11 function pointer(s), section .actor_tbl_0874c12c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoods */
u32 gWhispyWoodsVariants[1] ACTOR_TBL(0874c12c) = {
    (u32)WhispyWoodsInit,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsInit, WhispyWoodsEnterState */
u32 gWhispyWoodsStates[4] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2ae4,
    (u32)sub_080b2b40,
    (u32)sub_080b2c18,
    (u32)sub_080b2cf0,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in WhispyWoodsUpdate */
u32 gWhispyWoodsStateUpdates[4] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2b28,
    (u32)sub_080b2bf0,
    (u32)sub_080b2cc8,
    (u32)sub_080b2d80,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080b2dd4, sub_080b2e3c */
u32 gUnk_0874C150[1] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2e58,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080b2e20 */
u32 gUnk_0874C154[1] ACTOR_TBL(0874c12c) = {
    (u32)sub_080b2f34,
};

/* ---- 0x0874C21C-0x0874C240: 3 table(s), 9 function pointer(s), section .actor_tbl_0874c21c ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoodsApple */
u32 gUnk_0874C21C[1] ACTOR_TBL(0874c21c) = {
    (u32)sub_080b3090,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_080b3090, sub_080b3110 */
u32 gUnk_0874C220[4] ACTOR_TBL(0874c21c) = {
    (u32)sub_080b312c,
    (u32)sub_080b31a0,
    (u32)sub_080b3214,
    (u32)sub_080b328c,
};
/* include/enemy.h; CallTableEntry(i, 4, ...) in sub_080b30c8 */
u32 gUnk_0874C230[4] ACTOR_TBL(0874c21c) = {
    (u32)sub_080b319c,
    (u32)sub_080b31e0,
    (u32)sub_080b3258,
    (u32)sub_080b32d0,
};

/* ---- 0x0874C254-0x0874C260: 3 table(s), 3 function pointer(s), section .actor_tbl_0874c254 ---- */
/* include/enemy.h; CallTableEntry(i, 1, ...) in Task_WhispyWoodsAirPuff */
u32 gUnk_0874C254[1] ACTOR_TBL(0874c254) = {
    (u32)sub_080b3368,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080b3368, sub_080b33bc */
u32 gUnk_0874C258[1] ACTOR_TBL(0874c254) = {
    (u32)sub_080b33d8,
};
/* include/enemy.h; CallTableEntry(i, 1, ...) in sub_080b3398 */
u32 gUnk_0874C25C[1] ACTOR_TBL(0874c254) = {
    (u32)sub_080b3758,
};
