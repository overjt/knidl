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

/* The behaviour tables of game_rodata (0x08730780-0x0873ECEB, the rodata of
 * M02-M18; issue #167): 38 state/handler tables, 472 function pointers, in
 * 22 runs of adjacent tables, each run in address order.  Every table is
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
 * Each run is a named section .game_tbl_<address>: linker.ld lists the runs
 * between the data pieces of game_rodata inside ONE output section
 * (tools/ldgroup.py, docs/data.md 5.2).  The tables are const only where
 * their declaration already is: the qualifier would reach CallTableEntry's
 * table parameter (u32 * / void (**)(void)) under -Werror, and a named
 * section holds objects of one kind, so a const table gets a run of its
 * own.  The section attribute is what places them in ROM.  A function whose
 * prototype differs from the declared element type is cast to it, as its
 * consumers call it. */

#define GAME_TBL(addr) __attribute__((section(".game_tbl_" #addr)))

/* ---- 0x0873078C-0x0873079C: 2 table(s), 4 function pointer(s), section .game_tbl_0873078c ---- */
/* include/mode.h; CallTableEntry(i, 2, ...) in Task_ExtraModeTitleSprite */
void (*gExtraModeTitleSpriteStates[2])(void) GAME_TBL(0873078c) = {
    ExtraModeTitleTransferIcon,
    ExtraModeTitleLevelBar,
};
/* include/mode.h; CallTableEntry(i, 2, ...) in ExtraModeTitleSpriteUpdate */
void (*gExtraModeTitleSpriteStateUpdates[2])(void) GAME_TBL(0873078c) = {
    ExtraModeTitleTransferIconUpdate,
    ExtraModeTitleLevelBarUpdate,
};

/* ---- 0x08731FA8-0x087320C4: 2 table(s), 71 function pointer(s), section .game_tbl_08731fa8 ---- */
/* include/cutscene.h; CallTableEntry(i, 9, ...) in Task_CutsceneDirector: the bound exceeds the 8 entries, so index 8 would read the next label, gCutsceneActors */
u32 gCutsceneStarts[8] GAME_TBL(08731fa8) = {
    (u32)CutsceneDuelStart,
    (u32)CutsceneBeachStart,
    (u32)CutsceneBombStart,
    (u32)CutsceneBalloonsStart,
    (u32)CutsceneTomatoStart,
    (u32)CutsceneShipStart,
    (u32)CutsceneSingingStart,
    (u32)sub_0801761c,
};
/* include/cutscene.h; CallTableEntry(i, 63, ...) in Task_CutsceneActor */
u32 gCutsceneActors[63] GAME_TBL(08731fa8) = {
    (u32)CutsceneDuelKirby,
    (u32)sub_08010834,
    (u32)sub_08010bac,
    (u32)sub_08010cb4,
    (u32)CutsceneDuelBladeKnight,
    (u32)CutsceneBeachKirby,
    (u32)CutsceneBeachChair,
    (u32)CutsceneBeachCandyDream,
    (u32)CutsceneBeachDonutDream,
    (u32)CutsceneBeachMeatDream,
    (u32)sub_08011880,
    (u32)CutsceneBeachWaddleDoo,
    (u32)CutsceneBeachSunglasses,
    (u32)CutsceneBeachQuestionMarks,
    (u32)CutsceneBombKirby,
    (u32)CutsceneBombPoppyBrosSr,
    (u32)CutsceneBombHeldBomb,
    (u32)CutsceneBombThrownBomb,
    (u32)sub_08012df8,
    (u32)sub_08012e6c,
    (u32)sub_08013058,
    (u32)sub_08013348,
    (u32)CutsceneBalloonsKirby,
    (u32)CutsceneBalloonsLooseBalloon,
    (u32)CutsceneBalloonsYellowBalloon,
    (u32)CutsceneBalloonsGreenBalloon,
    (u32)CutsceneBalloonsLastBalloon,
    (u32)sub_08014184,
    (u32)sub_080142a0,
    (u32)sub_080143b4,
    (u32)CutsceneTomatoKirby,
    (u32)CutsceneTomatoMaximTomato,
    (u32)sub_08014c08,
    (u32)CutsceneTomatoExclamation,
    (u32)CutsceneShipKirby,
    (u32)CutsceneShipSpyglass,
    (u32)CutsceneShipPirateHat,
    (u32)CutsceneShipShark,
    (u32)CutsceneShipWaves,
    (u32)CutsceneSingingKirby,
    (u32)sub_08015f18,
    (u32)CutsceneSingingRainbowBar,
    (u32)sub_080162a0,
    (u32)sub_08016514,
    (u32)sub_080167dc,
    (u32)sub_08016ac4,
    (u32)sub_08016dd4,
    (u32)sub_080170e4,
    (u32)sub_080173f4,
    (u32)sub_0801757c,
    (u32)sub_08017668,
    (u32)sub_08018498,
    (u32)sub_08018e14,
    (u32)sub_08018bb8,
    (u32)sub_08018d7c,
    (u32)sub_08019000,
    (u32)sub_0801a07c,
    (u32)sub_080195ec,
    (u32)sub_08019b30,
    (u32)sub_08019c44,
    (u32)sub_08019d30,
    (u32)sub_08019e48,
    (u32)sub_08019eec,
};

/* ---- 0x08732614-0x08732630: 1 table(s), 7 function pointer(s), section .game_tbl_08732614 ---- */
/* include/room.h; CallTableEntry(i, 7, ...) in Task_Room */
void (*gRoomTaskVariants[7])(void) GAME_TBL(08732614) = {
    RoomTaskStageInit,
    RoomTaskHubInit,
    RoomTaskBigSwitchViewInit,
    RoomTaskCutsceneInit,
    RoomTaskGoalGameInit,
    sub_08024904,
    RoomTaskCreditsInit,
};

/* ---- 0x087328A0-0x087328BC: 1 table(s), 7 function pointer(s), section .game_tbl_087328a0 ---- */
/* include/camera.h; CallTableEntry(i, 7, ...) in Task_MapEvent */
void (*gMapEventVariants[7])(void) GAME_TBL(087328a0) = {
    sub_0802d38c,
    sub_0802d6cc,
    sub_0802d4bc,
    sub_0802d5f8,
    sub_0802d96c,
    MapEventStageUnlockPan,
    MapEventBigSwitchUnlockPan,
};

/* ---- 0x087328D8-0x087328F0: 1 table(s), 6 function pointer(s), section .game_tbl_087328d8 ---- */
/* include/camera.h; CallTableEntry(i, 6, ...) in Task_StageEffect */
void (*gUnk_087328D8[6])(void) GAME_TBL(087328d8) = {
    sub_08030254,
    sub_080302cc,
    sub_08030404,
    sub_08030580,
    sub_08030604,
    sub_080304ec,
};

/* ---- 0x0873A748-0x0873A924: 2 table(s), 117 function pointer(s), section .game_tbl_0873a748 ---- */
/* include/player.h; CallTableEntry(i, 62, ...) in Task_Player, PlayerStartRequestedAction */
void (*gPlayerActions[62])(void) GAME_TBL(0873a748) = {
    NULL,
    PlayerActionStand,
    PlayerActionWalk,
    PlayerActionRun,
    PlayerActionSkid,
    PlayerActionJump,
    PlayerActionReleaseJump,
    PlayerActionFall,
    PlayerActionHighFall,
    PlayerActionFloat,
    PlayerActionDuck,
    PlayerActionSlide,
    PlayerActionLadder,
    PlayerActionInhale,
    PlayerActionSpit,
    PlayerActionSwallow,
    PlayerActionHurt,
    PlayerActionDie,
    PlayerActionRecoil,
    PlayerActionShareItem,
    PlayerActionEnterDoor,
    PlayerActionExitDoor,
    sub_08034f70,
    PlayerActionSwim,
    PlayerActionStandInWater,
    PlayerActionWalkInWater,
    PlayerActionSwallowInWater,
    PlayerActionSpitInWater,
    PlayerActionWaterShot,
    PlayerActionGetAbility,
    PlayerActionBackdrop,
    PlayerActionThrow,
    PlayerActionFire,
    PlayerActionSpark,
    PlayerActionCutter,
    PlayerActionSword,
    PlayerActionBurning,
    PlayerActionLaser,
    PlayerActionMike,
    PlayerActionWheel,
    PlayerActionHammer,
    PlayerActionParasol,
    PlayerActionSleep,
    PlayerActionNeedle,
    PlayerActionIce,
    PlayerActionFreeze,
    PlayerActionHiJump,
    PlayerActionBeam,
    PlayerActionStone,
    PlayerActionBall,
    PlayerActionTornado,
    PlayerActionCrash,
    PlayerActionLight,
    PlayerActionBackdropHold,
    PlayerActionThrowHold,
    PlayerActionUFO,
    PlayerActionStarRod,
    PlayerActionStarRodJump,
    PlayerActionStarRodFlight,
    sub_080337f4,
    sub_080337fc,
    sub_08033804,
};
/* include/player.h; CallTableEntry(i, 57, ...) in PlayerUpdate */
void (*gPlayerActionHandlers[57])(void) GAME_TBL(0873a748) = {
    NULL,
    PlayerActionStandUpdate,
    PlayerActionWalkUpdate,
    PlayerActionRunUpdate,
    PlayerActionSkidUpdate,
    PlayerActionJumpUpdate,
    PlayerActionReleaseJumpUpdate,
    PlayerActionFallUpdate,
    PlayerActionHighFallUpdate,
    PlayerActionFloatUpdate,
    PlayerActionDuckUpdate,
    PlayerActionSlideUpdate,
    PlayerActionLadderUpdate,
    PlayerActionInhaleUpdate,
    PlayerActionSpitUpdate,
    PlayerActionSwallowUpdate,
    PlayerActionHurtUpdate,
    PlayerActionRecoilUpdate,
    PlayerActionShareItemUpdate,
    PlayerActionExitDoorUpdate,
    PlayerActionSwimUpdate,
    PlayerActionStandInWaterUpdate,
    PlayerActionWalkInWaterUpdate,
    PlayerActionWaterShotUpdate,
    PlayerActionSpitInWaterUpdate,
    PlayerActionSwallowInWaterUpdate,
    PlayerActionGetAbilityUpdate,
    PlayerActionBackdropUpdate,
    PlayerActionThrowUpdate,
    PlayerActionFireUpdate,
    PlayerActionSparkUpdate,
    PlayerActionCutterUpdate,
    PlayerActionSwordUpdate,
    PlayerActionBurningUpdate,
    PlayerActionLaserUpdate,
    PlayerActionMikeUpdate,
    PlayerActionWheelUpdate,
    PlayerActionHammerUpdate,
    PlayerActionParasolUpdate,
    PlayerActionSleepUpdate,
    PlayerActionNeedleUpdate,
    PlayerActionIceUpdate,
    PlayerActionFreezeUpdate,
    PlayerActionHiJumpUpdate,
    PlayerActionBeamUpdate,
    PlayerActionStoneUpdate,
    PlayerActionBallUpdate,
    PlayerActionTornadoUpdate,
    PlayerActionCrashUpdate,
    PlayerActionLightUpdate,
    PlayerActionBackdropHoldUpdate,
    PlayerActionThrowHoldUpdate,
    PlayerActionUFOUpdate,
    PlayerActionStarRodUpdate,
    PlayerActionStarRodJumpUpdate,
    PlayerActionStarRodFlightUpdate,
    sub_08033808,
};

/* ---- 0x0873B42C-0x0873B510: 2 table(s), 44 function pointer(s), section .game_tbl_0873b42c ---- */
/* include/player.h; CallTableEntry(i, 30, ...) in Task_Player, PlayerStartRequestedAction */
void (*gMetaKnightActions[30])(void) GAME_TBL(0873b42c) = {
    NULL,
    MetaKnightActionStand,
    MetaKnightActionWalk,
    MetaKnightActionRun,
    MetaKnightActionSkid,
    MetaKnightActionJump,
    MetaKnightActionReleaseJump,
    MetaKnightActionFall,
    NULL,
    MetaKnightActionFloat,
    MetaKnightActionDuck,
    MetaKnightActionSlide,
    MetaKnightActionLadder,
    NULL,
    NULL,
    NULL,
    MetaKnightActionHurt,
    MetaKnightActionDie,
    MetaKnightActionRecoil,
    NULL,
    MetaKnightActionEnterDoor,
    MetaKnightActionExitDoor,
    NULL,
    MetaKnightActionSwim,
    MetaKnightActionStandInWater,
    MetaKnightActionWalkInWater,
    MetaKnightActionSlash,
    MetaKnightActionDashSlash,
    MetaKnightActionUpwardSlash,
    MetaKnightActionDownThrust,
};
/* include/player.h; CallTableEntry(i, 27, ...) in PlayerUpdate */
void (*gMetaKnightActionHandlers[27])(void) GAME_TBL(0873b42c) = {
    NULL,
    PlayerActionStandUpdate,
    MetaKnightActionWalkUpdate,
    MetaKnightActionRunUpdate,
    PlayerActionSkidUpdate,
    MetaKnightActionJumpUpdate,
    MetaKnightActionJumpUpdate,
    MetaKnightActionFallUpdate,
    NULL,
    MetaKnightActionFloatUpdate,
    PlayerActionDuckUpdate,
    MetaKnightActionSlideUpdate,
    MetaKnightActionLadderUpdate,
    NULL,
    NULL,
    NULL,
    MetaKnightActionHurtUpdate,
    MetaKnightActionRecoilUpdate,
    NULL,
    PlayerActionExitDoorUpdate,
    MetaKnightActionSwimUpdate,
    PlayerActionStandInWaterUpdate,
    PlayerActionWalkInWaterUpdate,
    MetaKnightActionSlashUpdate,
    MetaKnightActionDashSlashUpdate,
    MetaKnightActionUpwardSlashUpdate,
    MetaKnightActionDownThrustUpdate,
};

/* ---- 0x0873B664-0x0873B6CC: 4 table(s), 26 function pointer(s), section .game_tbl_0873b664 ---- */
/* include/player.h; CallTableEntry(i, 9, ...) in PlayerActionBall, PlayerActionBallEnterVariant */
void (*gPlayerBallVariants[9])(void) GAME_TBL(0873b664) = {
    PlayerBallTransform,
    PlayerBallStand,
    PlayerBallRoll,
    PlayerBallSkid,
    PlayerBallJump,
    PlayerBallBounce,
    PlayerBallFall,
    PlayerBallLand,
    PlayerBallRevert,
};
/* include/player.h; CallTableEntry(i, 9, ...) in PlayerActionBallUpdate */
void (*gPlayerBallVariantUpdates[9])(void) GAME_TBL(0873b664) = {
    PlayerBallTransformUpdate,
    PlayerBallStandUpdate,
    PlayerBallRollUpdate,
    PlayerBallSkidUpdate,
    PlayerBallJumpUpdate,
    PlayerBallBounceUpdate,
    PlayerBallFallUpdate,
    PlayerBallLandUpdate,
    PlayerBallRevertUpdate,
};
/* include/player.h; CallTableEntry(i, 4, ...) in PlayerActionStarRodFlight, PlayerActionStarRodFlightEnterVariant */
void (*gPlayerStarRodFlightVariants[4])(void) GAME_TBL(0873b664) = {
    PlayerStarRodFlightIntro,
    PlayerStarRodFlightFly,
    PlayerStarRodFlightShoot,
    PlayerStarRodFlightHurt,
};
/* include/player.h; CallTableEntry(i, 4, ...) in PlayerActionStarRodFlightUpdate */
void (*gPlayerStarRodFlightVariantUpdates[4])(void) GAME_TBL(0873b664) = {
    PlayerStarRodFlightIntroUpdate,
    PlayerStarRodFlightFlyUpdate,
    PlayerStarRodFlightShootUpdate,
    PlayerStarRodFlightHurtUpdate,
};

/* ---- 0x0873B77C-0x0873B7B0: 1 table(s), 13 function pointer(s), section .game_tbl_0873b77c ---- */
/* include/player.h; CallTableEntry(i, 13, ...) in Task_PlayerObject */
void (*gPlayerObjectVariants[13])(void) GAME_TBL(0873b77c) = {
    PlayerObjectAirPuff,
    PlayerObjectSpitStar,
    PlayerObjectSpitMultiStar,
    PlayerObjectWaterShot,
    PlayerObjectFireBreath,
    PlayerObjectCutterBlade,
    PlayerObjectLaserBeam,
    PlayerObjectIceBreath,
    (void (*)(void))PlayerObjectBeamOrb,
    sub_08052b88,
    sub_08052f6c,
    PlayerObjectStarRodShot,
    PlayerObjectStarRodFlightShot,
};

/* ---- 0x0873B928-0x0873B9EC: 1 table(s), 49 function pointer(s), section .game_tbl_0873b928 ---- */
/* include/effect.h; CallTableEntry(i, 49, ...) in Task_PlayerEffect */
void (*gPlayerEffectVariants[49])(void) GAME_TBL(0873b928) = {
    sub_08053b40,
    sub_08053c48,
    sub_08053d08,
    PlayerEffectAbilityGetSparkle,
    sub_08053e38,
    PlayerEffectDeathStar,
    sub_080540d0,
    sub_08054330,
    sub_08054538,
    PlayerEffectSplash,
    sub_080548f0,
    PlayerEffectBubble,
    sub_08054a80,
    sub_08054d94,
    sub_08054de8,
    sub_08054fe4,
    sub_0805569c,
    sub_08055460,
    sub_08055520,
    sub_0805574c,
    sub_0805587c,
    sub_08055a40,
    sub_08055b24,
    sub_08055d74,
    sub_0805614c,
    sub_08056320,
    PlayerEffectHurtBurst,
    sub_080564ac,
    sub_08056770,
    PlayerEffectFireBreathFlames,
    PlayerEffectSparkAura,
    PlayerEffectSwordSparkle,
    PlayerEffectBurningFlames,
    sub_08057a48,
    sub_08057ad4,
    sub_08057ce0,
    sub_08057e90,
    PlayerEffectMikeAttack,
    PlayerEffectSleepBubble,
    sub_08058720,
    PlayerEffectIceBreathCloud,
    PlayerEffectFreezeAura,
    sub_08059570,
    sub_08059c28,
    sub_08059d7c,
    PlayerEffectTornadoDust,
    PlayerEffectCrashBlast,
    sub_0805acec,
    sub_0805ae94,
};

/* ---- 0x0873DBE4-0x0873DC3C: 2 table(s), 22 function pointer(s), section .game_tbl_0873dbe4 ---- */
/* include/effect.h; CallTableEntry(i, 11, ...) in PlayerGoalGameInit, PlayerGoalGameEnterState */
u32 gPlayerGoalGameStates[11] GAME_TBL(0873dbe4) = {
    (u32)sub_0805b4d8,
    (u32)sub_0805b514,
    (u32)sub_0805b644,
    (u32)sub_0805b670,
    (u32)sub_0805b6c0,
    (u32)sub_0805b8f8,
    (u32)sub_0805b9a4,
    (u32)sub_0805ba08,
    (u32)sub_0805bc1c,
    (u32)sub_0805bce0,
    (u32)sub_0805bd34,
};
/* include/effect.h; CallTableEntry(i, 11, ...) in PlayerGoalGameUpdate */
u32 gPlayerGoalGameStateUpdates[11] GAME_TBL(0873dbe4) = {
    (u32)sub_0805b508,
    (u32)sub_0805b534,
    (u32)sub_0805b660,
    (u32)sub_0805b688,
    (u32)sub_0805b788,
    (u32)sub_0805b998,
    (u32)sub_0805b9c0,
    (u32)sub_0805bb84,
    (u32)sub_0805bc50,
    (u32)sub_0805bd28,
    (u32)sub_0805be3c,
};

/* ---- 0x0873DEA0-0x0873DEA8: 1 table(s), 2 function pointer(s), section .game_tbl_0873dea0 ---- */
/* include/effect.h; CallTableEntry(i, 2, ...) in sub_0805dba0, sub_0805dbfc */
u32 gUnk_0873DEA0[2] GAME_TBL(0873dea0) = {
    (u32)sub_0805dc18,
    (u32)sub_0805dd88,
};

/* ---- 0x0873DEDC-0x0873DF14: 1 table(s), 14 function pointer(s), section .game_tbl_0873dedc ---- */
/* include/effect.h; CallTableEntry(i, 14, ...) in sub_0805e1bc, sub_0805e24c */
u32 gUnk_0873DEDC[14] GAME_TBL(0873dedc) = {
    (u32)sub_0805e2d4,
    (u32)sub_0805e7b4,
    (u32)sub_0805eb2c,
    (u32)sub_0805ee90,
    (u32)sub_0805f1bc,
    (u32)sub_0805f778,
    (u32)sub_0805e2d4,
    (u32)sub_0805fb88,
    (u32)sub_08060308,
    (u32)sub_08060c2c,
    (u32)sub_080613e4,
    (u32)sub_08061cac,
    (u32)sub_08062584,
    (u32)sub_08062f88,
};

/* ---- 0x0873DF24-0x0873DF38: 1 table(s), 5 function pointer(s), section .game_tbl_0873df24 ---- */
/* include/actor.h; CallTableEntry(i, 5, ...) in Task_PaletteAnim */
u32 gPaletteAnimVariants[5] GAME_TBL(0873df24) = {
    (u32)sub_080658d8,
    (u32)sub_080659b4,
    (u32)sub_08065a68,
    (u32)sub_08065b14,
    (u32)PaletteAnimBgBlend,
};

/* ---- 0x0873E280-0x0873E284: 1 table(s), 1 function pointer(s), section .game_tbl_0873e280 ---- */
/* include/actor.h; CallTableEntry(i, 1, ...) in Task_InhalableStar */
u32 gUnk_0873E280[1] GAME_TBL(0873e280) = {
    (u32)sub_08067258,
};

/* ---- 0x0873E284-0x0873E288: 1 table(s), 1 function pointer(s), section .game_tbl_0873e284 ---- */
/* include/actor.h; CallTableEntry(i, 1, ...) in InhalableStarUpdate: one entry; the 26 words
 * after it, up to the next label, are other data and stay structure-only */
u32 gUnk_0873E284[1] GAME_TBL(0873e284) = {
    (u32)sub_08067378,
};

/* ---- 0x0873E2F0-0x0873E348: 2 table(s), 22 function pointer(s), section .game_tbl_0873e2f0 ---- */
/* include/actor.h; CallTableEntry(i, 11, ...) in sub_0806737c, sub_080673ec */
u32 gUnk_0873E2F0[11] GAME_TBL(0873e2f0) = {
    (u32)sub_08067470,
    (u32)sub_080674d8,
    (u32)sub_080674f8,
    (u32)sub_08067550,
    (u32)sub_080676c0,
    (u32)sub_08067950,
    (u32)sub_08067b24,
    (u32)sub_08067d78,
    (u32)sub_08067ea4,
    (u32)sub_080680ac,
    (u32)sub_080682a8,
};
/* include/actor.h; CallTableEntry(i, 11, ...) in sub_08067408 */
u32 gUnk_0873E31C[11] GAME_TBL(0873e2f0) = {
    (u32)sub_080674a8,
    (u32)sub_080674f4,
    (u32)sub_08067520,
    (u32)sub_080675d8,
    (u32)sub_08067908,
    (u32)sub_08067a48,
    (u32)sub_08067d30,
    (u32)sub_08067db0,
    (u32)sub_08068028,
    (u32)sub_08068224,
    (u32)sub_08068460,
};

/* ---- 0x0873E5BC-0x0873E5F8: 2 table(s), 15 function pointer(s), section .game_tbl_0873e5bc ---- */
/* include/actor.h; CallTableEntry(i, 11, ...) in ActorDie */
u32 gActorDefeats[11] GAME_TBL(0873e5bc) = {
    (u32)ActorDefeatByEffect,
    (u32)ActorDefeatExplodeByEffect,
    (u32)sub_0806ab34,
    (u32)sub_0806abec,
    (u32)sub_0806ac6c,
    (u32)ActorDefeatAbilityStar,
    (u32)ActorDefeatMidBoss,
    (u32)ActorDefeatBoss,
    (u32)sub_0806b178,
    (u32)sub_0806b230,
    (u32)ActorDefeatPickup,
};
/* include/actor.h; CallTableEntry(i, 4, ...) in ActorDefeatByEffect */
u32 gActorDefeatsByEffect[4] GAME_TBL(0873e5bc) = {
    (u32)ActorDefeatPlain,
    (u32)ActorDefeatBurning,
    (u32)ActorDefeatShocked,
    (u32)ActorDefeatFrozen,
};

/* ---- 0x0873E670-0x0873E698: 3 table(s), 10 function pointer(s), section .game_tbl_0873e670 ---- */
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDefeatFrozen, ActorDefeatFrozenEnterState */
u32 gActorDefeatFrozenStates[3] GAME_TBL(0873e670) = {
    (u32)ActorDefeatFrozenShake,
    (u32)ActorDefeatFrozenSlide,
    (u32)sub_0806a9d8,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDefeatFrozenUpdate */
u32 gActorDefeatFrozenStateUpdates[3] GAME_TBL(0873e670) = {
    (u32)ActorDefeatFrozenShakeUpdate,
    (u32)sub_0806a9d4,
    (u32)sub_0806aa0c,
};
/* include/actor.h; CallTableEntry(i, 4, ...) in ActorDefeatExplodeByEffect */
u32 gActorExplodeDefeatsByEffect[4] GAME_TBL(0873e670) = {
    (u32)ActorDefeatExplode,
    (u32)sub_0806aa80,
    (u32)sub_0806aa8c,
    (u32)sub_0806aa98,
};

/* ---- 0x0873E734-0x0873E7A4: 5 table(s), 25 function pointer(s), section .game_tbl_0873e734 ---- */
/* include/actor.h; read by sub_0806b070 */
u32 gUnk_0873E734[9] GAME_TBL(0873e734) = {
    (u32)sub_080a1618,
    0,
    (u32)sub_080a7d20,
    0,
    (u32)sub_080a46e0,
    (u32)sub_080b2884,
    (u32)sub_080a9ea4,
    0,
    (u32)sub_080ac678,
};
/* include/actor.h; CallTableEntry(i, 9, ...) in ActorDefeatBoss */
u32 gUnk_0873E758[9] GAME_TBL(0873e734) = {
    (u32)sub_080a150c,
    (u32)PaintRollerDropStarRodPiece,
    (u32)sub_080a73b4,
    (u32)sub_080adc44,
    (u32)MrShineAndMrBrightDropStarRodPiece,
    (u32)sub_080b2dd4,
    (u32)KrackoDropStarRodPiece,
    (u32)sub_080af308,
    (u32)sub_080ab93c,
};
/* include/actor.h; CallTableEntry(i, 4, ...) in sub_0806b178 */
u32 gUnk_0873E77C[4] GAME_TBL(0873e734) = {
    (u32)sub_0806b1a8,
    (u32)sub_0806b1c4,
    (u32)sub_0806b1f4,
    (u32)sub_0806b224,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDrownInit, ActorDrownEnterState */
u32 gActorDrownStates[3] GAME_TBL(0873e734) = {
    (u32)ActorDrownSink,
    (u32)sub_0806b334,
    (u32)sub_0806b390,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDrownUpdate */
u32 gActorDrownStateUpdates[3] GAME_TBL(0873e734) = {
    (u32)sub_0806b330,
    (u32)sub_0806b368,
    (u32)sub_0806b3c0,
};

/* ---- 0x0873EAA0-0x0873EAC0: 1 table(s), 8 function pointer(s), section .game_tbl_0873eaa0 ---- */
/* include/actor.h; CallTableEntry(i, 8, ...) in ActorAttachedEnterState, ActorAttachedRunState */
u32 gActorAttachedStates[8] GAME_TBL(0873eaa0) = {
    (u32)ActorAttachedSwallow,
    (u32)ActorAttachedBackdropHeld,
    (u32)ActorAttachedBackdropFlight,
    (u32)ActorAttachedBackdropBounceOff,
    (u32)ActorAttachedPullIn,
    (u32)ActorAttachedThrowHeld,
    (u32)ActorAttachedThrowFlight,
    (u32)ActorAttachedThrowBounceOff,
};

/* ---- 0x0873ECE0-0x0873ECEC: 1 table(s), 3 function pointer(s), section .game_tbl_0873ece0 ---- */
/* include/actor.h; CallTableEntry(i, 3, ...) in Task_LandingImpact */
u32 gLandingImpactVariants[3] GAME_TBL(0873ece0) = {
    (u32)sub_0806ea70,
    (u32)sub_0806eb04,
    (u32)sub_0806eba4,
};
