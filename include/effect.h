#ifndef GUARD_EFFECT_H
#define GUARD_EFFECT_H

#include "gba/types.h"

/* effect.h: the RAM cells and ROM tables of the player effect objects (task
   type #7, M15) and the effect spawner (M16).  One declaration per symbol,
   with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

/* EWRAM */
extern u32 gStageClearDanceBgmPlayed[];
extern u8 gUnk_02006A14[];
extern s8 gDanceId;
extern u16 gObjPaletteBlendBase[];
extern u16 gScreenAttackTasks[]; /* 20 task indices, 0xFFFF = empty; non-volatile, signed reads cast (s16) (variant 37 in effect_mike_attack.c) */

/* IWRAM */
extern u16 gObjPaletteBank8[]; /* palette buffer */

/* ROM */
extern u8 gUnk_081FD870[];
extern u8 gUnk_082030D8[];
extern u32 gUnk_085B9B2C[];
extern u32 gUnk_085B9B6C[];
extern void (*gPlayerEffectVariants[])(void); /* task type #7's 49 variants, indexed by Task.unk18 >> 24 */
extern s16 gPlayerEffectStarTrajectories[]; /* [6][8]: s16 x, y offsets, 8.8 velocities */
extern u16 gPlayerEffectDeathStarRingMotions[][4]; /* [8 + 4][4]: 8.8 velocity x, y, acceleration x, y */
extern s16 gUnk_0873BA8C[][2][3]; /* {base, scale, amount} rows for RandomSpreadFacing */
extern u16 gUnk_0873BAB0[][3]; /* per sub-state: 8.8 x velocity, 8.8 y acceleration, frame */
extern u16 gUnk_0873BAE6[];
extern u16 gUnk_0873BAEE[];
extern u8 gUnk_0873BAFA[];
extern u16 gUnk_0873BAFC[];
extern u16 gUnk_0873BB0E[][3]; /* per sub-state: 8.8 x velocity, 8.8 y velocity, frame */
extern s16 gUnk_0873BB26[];
extern u8 gUnk_0873BB3E[];
extern u16 gUnk_0873BB7E[];
extern u16 gUnk_0873BC3E[];
extern u32 gUnk_0873C038[];
extern u8 gUnk_0873C04C[];
extern u32 gUnk_0873C23C[];
extern u32 gUnk_0873C250[];
extern u8 gUnk_0873C2B4[];
extern s8 gUnk_0873CB74[]; /* collision box passed to TerrainCollidePointStop */
extern u32 gUnk_0873CC94[];
extern u32 gUnk_0873CF8C[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern s16 gGoalGameLaneX[];
extern s16 gGoalGameLayerHeights[];
extern u32 gPlayerGoalGameStates[];
extern u32 gPlayerGoalGameStateUpdates[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gGoalGameLayerScores[];
extern u32 gUnk_0873DCA8[];
extern u32 gGoalGameTrailStarOffsetX[];
extern u32 gGoalGameTrailStarOffsetY[];
extern u32 gGoalGameSpringDepths[];
extern u32 gGoalGameCameraRiseDurations[];
extern u32 gGoalGameCameraPauseDurations[];
extern u32 gUnk_0873DD4C[];
extern u32 gUnk_0873DD5C[];
extern u32 gUnk_0873DD64[];
extern u32 gUnk_0873DD80[];
extern u32 gUnk_0873DDA2[];
extern u32 gUnk_0873DDB4[];
extern u32 gUnk_0873DDBE[];
extern u32 gUnk_0873DDE8[];
extern u32 gUnk_0873DEA0[];
extern u16 gUnk_0873DEA8[];
extern u32 gPlayerDances[];
extern u32 gUnk_0874C600[];
extern u32 gPlayerEffectBubbleFrames[];
extern u32 gUnk_0874C67C[];
extern u32 gPlayerEffectDeathStarRingFrames[];
extern u32 gUnk_0874C718[];
extern u32 gPlayerEffectLocalPlayerArrowFrames[];
extern u32 gUnk_0874C784[];
extern u32 gPlayerEffectHurtSparksFrames[];
extern u32 gUnk_0874C7B4[];
extern u32 gStarBurstFrames[];
extern u32 gUnk_0874C828[];
extern u32 gGoalGameStarFrames[];
extern u32 gPlayerEffectHurtBurstFrames[];
extern u32 gUnk_0874C960[];
extern u32 gUnk_0874C980[];
extern u32 gInhaleAirFrames[];
extern u32 gUnk_08751C74[];
extern u32 gPlayerEffectSparkAuraFrames[];
extern u32 gSparkleFrames[];
extern u32 gPlayerEffectBurningFlamesFrames[];
extern u32 gUnk_08751D50[];
extern u32 gUnk_08751D80[];
extern u32 gPlayerEffectHammerDustFrames[];
extern u32 gPlayerEffectParasolSparkleFrames[];
extern u32 gUnk_08751DBC[];
extern u32 gPlayerEffectSleepBubbleFrames[];
extern u32 gUnk_08751E00[];
extern u32 gPlayerEffectFreezeAuraFrames[];
extern u32 gPlayerEffectStonePuffFrames[];
extern u32 gUnk_08751F0C[];
extern u32 gUnk_08751F84[];
extern u32 gPlayerEffectTornadoDustFrames[];
extern u32 gPlayerEffectCrashBlastFrames[];
extern u32 gPlayerEffectUFOChargeSparkleFrames[];
extern u32 gGoalGameSpringFrames[];
extern u32 gGoalGamePlayerMarkerFrames[];
extern u32 gUnk_087548A0[];


/* Functions (defined in the files named above each group). */

/* src/effect_skid_dust.c */
void Task_PlayerEffect(void);
void PlayerEffectInhaleAir(void);
void PlayerEffectInhaleAirUpdate(void);
void PlayerEffectInhaleAirDraw(void);
void PlayerEffectCatchDust(void);
void PlayerEffectSpitDust(void);
void PlayerEffectAbilityGetSparkle(void);
void PlayerEffectAbilityGetSparkleUpdate(void);
void PlayerEffectImpactStar(void);
void PlayerEffectDeathStar(void);
void PlayerEffectSkidDust(void);
void PlayerEffectSkidDustUpdate(void);

/* src/effect_slide_dust.c */
void PlayerEffectRunDust(void);
void PlayerEffectRunDustUpdate(void);
void PlayerEffectSlideDust(void);
void PlayerEffectSlideDustUpdate(void);
void PlayerEffectSplash(void);
void PlayerEffectLeaveWaterSplash(void);
void PlayerEffectBubble(void);
void PlayerEffectBubbleUpdate(void);

/* src/effect_death_star_ring_ability.c */
void PlayerEffectDeathStarRing(void);
void PlayerEffectDeathStarRingLateUpdate(void);
void PlayerEffectMetaKnightDeathBlast(void);
void sub_08054de8(void);
void sub_08054fe4(void);
void sub_080552fc(void);
void sub_080553d4(void);

/* src/effect_dance_star_burst.c */
void PlayerEffectAbilityLoss(void);
void sub_08055520(void);
void PlayerEffectDanceStarBurst(void);
void sub_0805574c(void);
void sub_080557d4(void);
void sub_0805587c(void);
void PlayerEffectLocalPlayerArrow(void);
void PlayerEffectLocalPlayerArrowUpdate(void);

/* src/effect_hurt_flames_sparks.c */
void PlayerEffectHurtFlames(void);
void PlayerEffectHurtFlamesUpdate(void);
void PlayerEffectHurtSparks(void);
void PlayerEffectHurtSparksUpdate(void);
void sub_0805614c(void);
void sub_08056300(void);
void sub_08056320(void);
void sub_08056428(void);

/* src/effect_ability_puffs.c */
void PlayerEffectHurtBurst(void);
void sub_080564ac(void);
void sub_08056770(void);
void sub_08056da8(void);

/* src/effect_fire_breath_spark_aura.c */
void PlayerEffectFireBreathFlames(void);
void PlayerEffectFireBreathFlamesUpdate(void);
void PlayerEffectSparkAura(void);
void PlayerEffectSparkAuraUpdate(void);
void PlayerEffectSwordSparkle(void);

/* src/effect_burning_flames_wheel.c */
void PlayerEffectBurningFlames(void);
void PlayerEffectBurningFlamesUpdate(void);
void PlayerEffectUFOLaserTrail(void);
void sub_08057ad4(void);
void sub_08057c98(void);

/* src/effect_mike_attack.c */
void PlayerEffectHammerDust(void);
void PlayerEffectParasolSparkle(void);
void PlayerEffectMikeAttack(void);
void PlayerEffectMikeAttackUpdate(void);
void PlayerEffectSleepBubble(void);
void PlayerEffectSleepBubbleUpdate(void);
void sub_08058720(void);

/* src/effect_ice_breath_freeze_aura.c */
void PlayerEffectIceBreathCloud(void);
void PlayerEffectIceBreathCloudUpdate(void);
void PlayerEffectFreezeAura(void);
void PlayerEffectFreezeAuraUpdate(void);

/* src/effect_hi_jump_ball.c */
void sub_08059570(void);
void sub_08059aac(void);
void sub_08059b18(void);
void PlayerEffectStonePuff(void);
void sub_08059d7c(void);
void sub_0805a320(void);

/* src/effect_crash_blast.c */
void PlayerEffectTornadoDust(void);
void PlayerEffectTornadoDustUpdate(void);
void PlayerEffectCrashBlast(void);
void PlayerEffectCrashBlastUpdate(void);
void PlayerEffectCrashBlastDraw(void);
void sub_0805acec(void);
void sub_0805ae00(void);
void PlayerEffectUFOChargeSparkle(void);
void PlayerEffectUFOChargeSparkleUpdate(void);
void sub_0805af80(void);

/* src/effect_goal_game_dance.c */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2);
void GoalGameMain(void);
void GoalGameInit(void);
void PlayerGoalGameInit(void);
void PlayerGoalGameEnterState(void);
void sub_0805b370(void);
s32 PlayerGoalGameUpdate(void);
void PlayerGoalGameState0(void);
void PlayerGoalGameState0Update(void);
void PlayerGoalGameWaitForPress(void);
void PlayerGoalGameWaitForPressUpdate(void);
s32 PlayerGoalGameCheckPress(void);
void PlayerGoalGameSetLaunchPower(void);
void PlayerGoalGameState2(void);
void PlayerGoalGameState2Update(void);
void PlayerGoalGameWaitForLaunch(void);
void PlayerGoalGameWaitForLaunchUpdate(void);
void PlayerGoalGameLaunch(void);
void PlayerGoalGameLaunchUpdate(void);
void sub_0805b83c(void);
void PlayerGoalGameAddToLayerSign(void);
void sub_0805b8f8(void);
void PlayerGoalGameState5Update(void);
void PlayerGoalGameFall(void);
void PlayerGoalGameFallUpdate(void);
void PlayerGoalGameLand(void);
void PlayerGoalGameLandUpdate(void);
void PlayerGoalGameWalkToSpotUpdate(void);
void PlayerGoalGameWait(void);
void PlayerGoalGameWaitUpdate(void);
void PlayerGoalGameWaitLateUpdate(void);
void PlayerGoalGameRemoveFromLayerSign(void);
void PlayerGoalGameDance(void);
void PlayerGoalGameDanceUpdate(void);
void PlayerGoalGameFinish(void);
void PlayerGoalGameFinishUpdate(void);
void PlayerGoalGameDrawPressPrompt(void);
void Task_GoalGameLaunchStars(void);
void GoalGameLaunchStarsFall(void);
void GoalGameLaunchStarsUpdate(void);
void GoalGameLaunchStarsDraw(void);
void Task_GoalGameBigTrailStar(void);
void Task_GoalGameSmallTrailStar(void);
void PlayerGoalGameRideSpring(void);
void Task_GoalGameCamera(void);
void GoalGameCameraFollowPlayer(void);
void GoalGameCameraUpdate(void);
void Task_GoalGameSpring(void);
void Task_GoalGamePlayerMarker(void);
s32 GoalGamePlayerMarkerFollowParent(void);
void Task_GoalGameSign(void);
void GoalGameSignFollowHelperKirby(void);
void Task_GoalGameHelperKirby(void);
void GoalGameHelperKirbyUpdate(void);
void sub_0805d564(void);
void sub_0805d5fc(void);
void Task_GoalGameOneUp(void);
void TaskStartFrameScript(s32 a0);
void TaskStartFrameScriptId(s32 a0);
void TaskUpdateFrameScript(void);
void TaskAdvanceFrameScript(void);
void GoalGameHelperKirbyInitSprite(s32 a0, s32 a1);
void GoalGameHelperKirbyDraw(void);
void sub_0805dba0(void);
void sub_0805dbfc(void);
void sub_0805dc18(void);
void sub_0805dd4c(void);
void sub_0805dd88(void);
void sub_0805ddb0(s32 a0);
void StartAllPlayersDance(void);
void sub_0805df9c(void);
void PlayerWalkToDanceSpotUpdate(void);
void PlayerSetDanceSpot(s32 a0);
void PlayerWalkToDanceSpot(s32 a0);
void PlayerDance(void);
void PlayerDanceInGoalGame(void);
void PlayerDanceAfterStageClear(void);
void sub_0805e2d4(void);
void PlayerDance1(void);
void PlayerDance2(s32 a0, s32 a1, s32 a2);
void PlayerDance3(void);
void PlayerDance4(void);
void PlayerDance5(void);
void PlayerDance7(void);
void PlayerDance8(void);
void PlayerDance9(void);
void PlayerDance10(void);
void PlayerDance11(void);

#endif /* GUARD_EFFECT_H */
