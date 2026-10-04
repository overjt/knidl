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
    (u32)CutsceneFountainStart,
};
/* include/cutscene.h; CallTableEntry(i, 63, ...) in Task_CutsceneActor */
u32 gCutsceneActors[63] GAME_TBL(08731fa8) = {
    (u32)CutsceneDuelKirby,
    (u32)CutsceneActorScript1,
    (u32)CutsceneActorScript2,
    (u32)CutsceneActorScript3,
    (u32)CutsceneDuelBladeKnight,
    (u32)CutsceneBeachKirby,
    (u32)CutsceneBeachChair,
    (u32)CutsceneBeachCandyDream,
    (u32)CutsceneBeachDonutDream,
    (u32)CutsceneBeachMeatDream,
    (u32)CutsceneBeachActorScript10,
    (u32)CutsceneBeachWaddleDoo,
    (u32)CutsceneBeachSunglasses,
    (u32)CutsceneBeachQuestionMarks,
    (u32)CutsceneBombKirby,
    (u32)CutsceneBombPoppyBrosSr,
    (u32)CutsceneBombHeldBomb,
    (u32)CutsceneBombThrownBomb,
    (u32)CutsceneBombActorScript18,
    (u32)CutsceneBombActorScript19,
    (u32)CutsceneBombActorScript20,
    (u32)CutsceneBombActorScript21,
    (u32)CutsceneBalloonsKirby,
    (u32)CutsceneBalloonsLooseBalloon,
    (u32)CutsceneBalloonsYellowBalloon,
    (u32)CutsceneBalloonsGreenBalloon,
    (u32)CutsceneBalloonsLastBalloon,
    (u32)CutsceneBalloonsActorScript27,
    (u32)CutsceneBalloonsActorScript28,
    (u32)CutsceneBalloonsActorScript29,
    (u32)CutsceneTomatoKirby,
    (u32)CutsceneTomatoMaximTomato,
    (u32)CutsceneTomatoActorScript32,
    (u32)CutsceneTomatoExclamation,
    (u32)CutsceneShipKirby,
    (u32)CutsceneShipSpyglass,
    (u32)CutsceneShipPirateHat,
    (u32)CutsceneShipShark,
    (u32)CutsceneShipWaves,
    (u32)CutsceneSingingKirby,
    (u32)CutsceneSingingActorScript40,
    (u32)CutsceneSingingRainbowBar,
    (u32)CutsceneSingingActorScript42,
    (u32)CutsceneSingingActorScript43,
    (u32)CutsceneSingingActorScript44,
    (u32)CutsceneSingingActorScript45,
    (u32)CutsceneSingingActorScript46,
    (u32)CutsceneSingingActorScript47,
    (u32)CutsceneSingingActorScript48,
    (u32)CutsceneSingingActorScript49,
    (u32)CutsceneFountainKirby,
    (u32)CutsceneFountainKingDedede,
    (u32)CutsceneFountainActorScript52,
    (u32)CutsceneFountainActorScript53,
    (u32)CutsceneFountainActorScript54,
    (u32)CutsceneFountainNightmarePowerOrb,
    (u32)CutsceneFountainActorScript56,
    (u32)CutsceneFountainStarRod,
    (u32)CutsceneFountainJet,
    (u32)CutsceneFountainActorScript59,
    (u32)CutsceneFountainActorScript60,
    (u32)CutsceneFountainActorScript61,
    (u32)CutsceneFountainActorScript62,
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
    MapEventMidBossFight,
    sub_0802d6cc,
    MapEventBreakTwoBlocks,
    MapEventBreakThreeBlocks,
    sub_0802d96c,
    MapEventStageUnlockPan,
    MapEventBigSwitchUnlockPan,
};

/* ---- 0x087328D8-0x087328F0: 1 table(s), 6 function pointer(s), section .game_tbl_087328d8 ---- */
/* include/camera.h; CallTableEntry(i, 6, ...) in Task_StageEffect */
void (*gStageEffectStates[6])(void) GAME_TBL(087328d8) = {
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
    PlayerObjectLightOrb,
    PlayerObjectUFOShot,
    PlayerObjectStarRodShot,
    PlayerObjectStarRodFlightShot,
};

/* ---- 0x0873B928-0x0873B9EC: 1 table(s), 49 function pointer(s), section .game_tbl_0873b928 ---- */
/* include/effect.h; CallTableEntry(i, 49, ...) in Task_PlayerEffect */
void (*gPlayerEffectVariants[49])(void) GAME_TBL(0873b928) = {
    PlayerEffectInhaleAir,
    sub_08053c48,
    sub_08053d08,
    PlayerEffectAbilityGetSparkle,
    PlayerEffectImpactStar,
    PlayerEffectDeathStar,
    PlayerEffectSkidDust,
    PlayerEffectRunDust,
    PlayerEffectSlideDust,
    PlayerEffectSplash,
    sub_080548f0,
    PlayerEffectBubble,
    PlayerEffectDeathStarRing,
    PlayerEffectMetaKnightDeathBlast,
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
    PlayerEffectUFOLaserTrail,
    sub_08057ad4,
    PlayerEffectHammerDust,
    PlayerEffectParasolSparkle,
    PlayerEffectMikeAttack,
    PlayerEffectSleepBubble,
    sub_08058720,
    PlayerEffectIceBreathCloud,
    PlayerEffectFreezeAura,
    sub_08059570,
    PlayerEffectStonePuff,
    sub_08059d7c,
    PlayerEffectTornadoDust,
    PlayerEffectCrashBlast,
    sub_0805acec,
    PlayerEffectUFOChargeSparkle,
};

/* ---- 0x0873DBE4-0x0873DC3C: 2 table(s), 22 function pointer(s), section .game_tbl_0873dbe4 ---- */
/* include/effect.h; CallTableEntry(i, 11, ...) in PlayerGoalGameInit, PlayerGoalGameEnterState */
u32 gPlayerGoalGameStates[11] GAME_TBL(0873dbe4) = {
    (u32)PlayerGoalGameState0,
    (u32)PlayerGoalGameWaitForPress,
    (u32)PlayerGoalGameState2,
    (u32)PlayerGoalGameWaitForLaunch,
    (u32)PlayerGoalGameLaunch,
    (u32)sub_0805b8f8,
    (u32)PlayerGoalGameFall,
    (u32)PlayerGoalGameLand,
    (u32)PlayerGoalGameWait,
    (u32)PlayerGoalGameDance,
    (u32)PlayerGoalGameFinish,
};
/* include/effect.h; CallTableEntry(i, 11, ...) in PlayerGoalGameUpdate */
u32 gPlayerGoalGameStateUpdates[11] GAME_TBL(0873dbe4) = {
    (u32)PlayerGoalGameState0Update,
    (u32)PlayerGoalGameWaitForPressUpdate,
    (u32)PlayerGoalGameState2Update,
    (u32)PlayerGoalGameWaitForLaunchUpdate,
    (u32)PlayerGoalGameLaunchUpdate,
    (u32)PlayerGoalGameState5Update,
    (u32)PlayerGoalGameFallUpdate,
    (u32)PlayerGoalGameLandUpdate,
    (u32)PlayerGoalGameWaitUpdate,
    (u32)PlayerGoalGameDanceUpdate,
    (u32)PlayerGoalGameFinishUpdate,
};

/* ---- 0x0873DEA0-0x0873DEA8: 1 table(s), 2 function pointer(s), section .game_tbl_0873dea0 ---- */
/* include/effect.h; CallTableEntry(i, 2, ...) in sub_0805dba0, sub_0805dbfc */
u32 gUnk_0873DEA0[2] GAME_TBL(0873dea0) = {
    (u32)sub_0805dc18,
    (u32)sub_0805dd88,
};

/* ---- 0x0873DEDC-0x0873DF14: 1 table(s), 14 function pointer(s), section .game_tbl_0873dedc ---- */
/* include/effect.h; CallTableEntry(i, 14, ...) in PlayerDanceInGoalGame, PlayerDanceAfterStageClear */
u32 gPlayerDances[14] GAME_TBL(0873dedc) = {
    (u32)sub_0805e2d4,
    (u32)PlayerDance1,
    (u32)PlayerDance2,
    (u32)PlayerDance3,
    (u32)PlayerDance4,
    (u32)PlayerDance5,
    (u32)sub_0805e2d4,
    (u32)PlayerDance7,
    (u32)PlayerDance8,
    (u32)PlayerDance9,
    (u32)PlayerDance10,
    (u32)PlayerDance11,
    (u32)PlayerDance12,
    (u32)PlayerDance13,
};

/* ---- 0x0873DF24-0x0873DF38: 1 table(s), 5 function pointer(s), section .game_tbl_0873df24 ---- */
/* include/actor.h; CallTableEntry(i, 5, ...) in Task_PaletteAnim */
u32 gPaletteAnimVariants[5] GAME_TBL(0873df24) = {
    (u32)PaletteAnimVariant0,
    (u32)PaletteAnimCycle,
    (u32)PaletteAnimFlashColor,
    (u32)PaletteAnimVariant3,
    (u32)PaletteAnimBgBlend,
};

/* ---- 0x0873E280-0x0873E284: 1 table(s), 1 function pointer(s), section .game_tbl_0873e280 ---- */
/* include/actor.h; CallTableEntry(i, 1, ...) in Task_InhalableStar */
u32 gInhalableStarStates[1] GAME_TBL(0873e280) = {
    (u32)InhalableStarState0,
};

/* ---- 0x0873E284-0x0873E288: 1 table(s), 1 function pointer(s), section .game_tbl_0873e284 ---- */
/* include/actor.h; CallTableEntry(i, 1, ...) in InhalableStarUpdate: one entry; the 26 words
 * after it, up to the next label, are other data and stay structure-only */
u32 gInhalableStarStateUpdates[1] GAME_TBL(0873e284) = {
    (u32)InhalableStarState0Update,
};

/* ---- 0x0873E2F0-0x0873E348: 2 table(s), 22 function pointer(s), section .game_tbl_0873e2f0 ---- */
/* include/actor.h; CallTableEntry(i, 11, ...) in HeldPlayerInit, HeldPlayerEnterState */
u32 gHeldPlayerStates[11] GAME_TBL(0873e2f0) = {
    (u32)HeldPlayerSwallow,
    (u32)HeldPlayerSpitFlight,
    (u32)HeldPlayerSpitBounceOff,
    (u32)HeldPlayerBackdropHeld,
    (u32)HeldPlayerBackdropBounceOff,
    (u32)HeldPlayerState5,
    (u32)HeldPlayerState6,
    (u32)HeldPlayerThrowHeld,
    (u32)HeldPlayerThrowFlightForward,
    (u32)HeldPlayerThrowFlightBackward,
    (u32)HeldPlayerThrowBounceOff,
};
/* include/actor.h; CallTableEntry(i, 11, ...) in HeldPlayerUpdate */
u32 gHeldPlayerStateUpdates[11] GAME_TBL(0873e2f0) = {
    (u32)HeldPlayerSwallowUpdate,
    (u32)HeldPlayerSpitFlightUpdate,
    (u32)HeldPlayerSpitBounceOffUpdate,
    (u32)HeldPlayerBackdropHeldUpdate,
    (u32)HeldPlayerBackdropBounceOffUpdate,
    (u32)HeldPlayerState5Update,
    (u32)HeldPlayerState6Update,
    (u32)HeldPlayerThrowHeldUpdate,
    (u32)HeldPlayerThrowFlightForwardUpdate,
    (u32)HeldPlayerThrowFlightBackwardUpdate,
    (u32)HeldPlayerThrowBounceOffUpdate,
};

/* ---- 0x0873E5BC-0x0873E5F8: 2 table(s), 15 function pointer(s), section .game_tbl_0873e5bc ---- */
/* include/actor.h; CallTableEntry(i, 11, ...) in ActorDie */
u32 gActorDefeats[11] GAME_TBL(0873e5bc) = {
    (u32)ActorDefeatByEffect,
    (u32)ActorDefeatExplodeByEffect,
    (u32)ActorDefeat2,
    (u32)ActorDefeat3,
    (u32)ActorDefeat4,
    (u32)ActorDefeatAbilityStar,
    (u32)ActorDefeatMidBoss,
    (u32)ActorDefeatBoss,
    (u32)ActorDefeat8,
    (u32)ActorDefeat9,
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
    (u32)ActorDefeatFrozenState2,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDefeatFrozenUpdate */
u32 gActorDefeatFrozenStateUpdates[3] GAME_TBL(0873e670) = {
    (u32)ActorDefeatFrozenShakeUpdate,
    (u32)ActorDefeatFrozenSlideUpdate,
    (u32)sub_0806aa0c,
};
/* include/actor.h; CallTableEntry(i, 4, ...) in ActorDefeatExplodeByEffect */
u32 gActorExplodeDefeatsByEffect[4] GAME_TBL(0873e670) = {
    (u32)ActorDefeatExplode,
    (u32)ActorExplodeDefeat1,
    (u32)ActorExplodeDefeat2,
    (u32)ActorExplodeDefeat3,
};

/* ---- 0x0873E734-0x0873E7A4: 5 table(s), 25 function pointer(s), section .game_tbl_0873e734 ---- */
/* include/actor.h; read by BossRunDefeatHook */
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
    (u32)KingDededeDefeatedInit,
    (u32)PaintRollerDropStarRodPiece,
    (u32)MetaKnightDefeatedInit,
    (u32)sub_080adc44,
    (u32)MrShineAndMrBrightDropStarRodPiece,
    (u32)sub_080b2dd4,
    (u32)KrackoDropStarRodPiece,
    (u32)sub_080af308,
    (u32)NightmareWizardDefeat,
};
/* include/actor.h; CallTableEntry(i, 4, ...) in ActorDefeat8 */
u32 gUnk_0873E77C[4] GAME_TBL(0873e734) = {
    (u32)sub_0806b1a8,
    (u32)sub_0806b1c4,
    (u32)sub_0806b1f4,
    (u32)sub_0806b224,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDrownInit, ActorDrownEnterState */
u32 gActorDrownStates[3] GAME_TBL(0873e734) = {
    (u32)ActorDrownSink,
    (u32)ActorDrownState1,
    (u32)ActorDrownState2,
};
/* include/actor.h; CallTableEntry(i, 3, ...) in ActorDrownUpdate */
u32 gActorDrownStateUpdates[3] GAME_TBL(0873e734) = {
    (u32)ActorDrownSinkUpdate,
    (u32)ActorDrownState1Update,
    (u32)ActorDrownState2Update,
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
    (u32)LandingImpactVariant0,
    (u32)LandingImpactVariant1,
    (u32)LandingImpactVariant2,
};
