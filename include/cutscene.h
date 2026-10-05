#ifndef GUARD_CUTSCENE_H
#define GUARD_CUTSCENE_H

#include "gba/types.h"
#include "task.h"

/* cutscene.h: the RAM cells and ROM tables of the scripted sequences: AgbMain
   state 7, the sequence director (M04) and the cutscene bank (M19).  One
   declaration per symbol, with the type its consumers prove (issue #36 phase
   2, docs/header-conventions.md). */

struct GfxHeader;

/* The 24-entry animation rows at 0x08740320 / 0x087404A0 the credits
   particles walk: `unk00` indexes the gfx pointer table, `unk02` is the
   step's delay. */
struct RoomParticleAnimFrame
{
    /*0x00*/ u8 frame;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The eight 4-byte records at 0x03000FE0 that M19's credits tasks animate:
   a frame table index (unk00), the frame within it (unk01), the timer
   (unk02) and the countdown RoomParticleDrawScrolled draws on (unk03). */
struct RoomParticle
{
    /*0x00*/ u8 animRow;
    /*0x01*/ u8 animStep;
    /*0x02*/ u8 pixelX;
    /*0x03*/ u8 pixelY;
};

/* The 0x087401E4 script records M19's ending-sequence tasks walk: a pointer
   table indexed by Task.unk20, each entry a header plus two `s16` step lists
   (the "forward" list at +6 and the "reverse" one at +18) that
   CannonFuseGetPieceFrame picks between on Task.unk18/unk24. */
struct CannonFusePiece
{
    /*0x00*/ u8 unk00[4];
    /*0x04*/ u16 enterFrame;
    /*0x06*/ u16 unk06[6];
    /*0x12*/ u16 unk12[1];
};

/* EWRAM */
extern u32 gWarpStarFlightSfxPlayer;
extern u32 gWarpStarFlightSfx;

/* IWRAM */
extern struct RoomParticle gRoomParticles[];

/* ROM */
extern u32 gUnk_080D21C8[];
extern u32 gUnk_0824A9CC[];
extern u32 gUnk_0825D2C8[];
extern u32 gUnk_085E6FA4[];
extern u32 gUnk_085E6FE4[];
extern u32 gUnk_085E72D4[];
extern struct GfxHeader *const gCutsceneSheets[];
extern u32 gCutsceneDurations[];
extern u32 gCutsceneStarts[];
extern u32 gCutsceneActors[];
extern s16 gUnk_087320C4[][16];
extern u32 gUnk_08732104[];
extern u32 gUnk_08732134[];
extern u32 gUnk_08732138[];
extern u32 gWarpStarBoardAttackBox[];
extern u32 gWarpStarTerrainBox[];
extern u32 gPlayerCannonTerrainBox[];
extern u32 gUnk_0873FB7C[];
extern s32 gUnk_0873FB94[];
extern u32 gWarpStarStates[];
extern u32 gWarpStarStateUpdates[];
extern u32 gWarpStarFlights[];
extern u32 gWarpStarFlightUpdates[];
extern u32 gWarpStarCameraPaths[];
extern s16 gUnk_0873FCF8[];
extern s16 gUnk_0873FD20[];
extern s16 gUnk_0873FD48[];
extern s16 gUnk_0873FD70[];
extern u32 gUnk_0873FD98[];
extern u32 gUnk_0873FE98[];
extern u32 gUnk_08740098[];
extern u16 gUnk_0874009C[];
extern u16 gUnk_087400A6[];
extern u32 gPlayerCannonStates[];
extern u32 gPlayerCannonStateUpdates[];
extern u32 gCannonVariants[];
extern u16 gUnk_087400E4[];
extern u32 gCannonStates[];
extern u32 gCannonStateUpdates[];
extern u32 gCannonFuseVariants[];
extern u16 gUnk_08740124[];
extern s8 gUnk_087401CC[];
extern struct CannonFusePiece *gCannonFusePieces[];
extern u32 gCannonFuseStates[];
extern u32 gCannonFuseStateUpdates[];
extern u32 gBigSwitchVariants[];
extern u32 gBigSwitchStates[];
extern u32 gBigSwitchStateUpdates[];
extern u32 gStakeVariants[];
extern u32 gStakeStates[];
extern u32 gStakeStateUpdates[];
extern u32 gRoomParticlesVariants[];
extern struct RoomParticleAnimFrame gUnk_08740320[][24];
extern struct RoomParticleAnimFrame gUnk_087404A0[][24];
extern u8 gUnk_08740620[];
extern u32 gWaddleDeeVariants[];
extern struct AnimCmd gWaddleDeeJumpAnim[];
extern u32 gWaddleDeeDef[];
extern u32 gWaddleDeeFrames[];
extern u32 gCannonFrames[];
extern u32 gCannonFuseFrames[];
extern u32 gBigSwitchFrames[];
extern u32 gStakeFrames[];
extern u32 gWarpStarVanishFrames[];
extern u32 gUnk_08752DB8[];
extern u32 gUnk_08752E00[];
extern u32 gCutsceneDuelFrames[];
extern u32 gCutsceneBeachFrames[];
extern u32 gCutsceneBombFrames[];
extern u32 gCutsceneBalloonsFrames[];
extern u32 gCutsceneTomatoFrames[];
extern u32 gCutsceneShipFrames[];
extern u32 gCutsceneSingingFrames[];
extern u32 gNightmarePowerOrbEscapeFrames[];


/* Functions (defined in the files named above each group). */

/* src/cutscene_main.c */
void CutsceneMain(void);
void CutsceneLoadGraphics(void);

/* src/cutscene_director_duel.c */
s32 CreateCutsceneActor(s32 a, s32 b);
void Task_CutsceneDirector(void);
void CutsceneCheckSkip(void);
void Task_CutsceneActor(void);
void CutsceneDuelStart(void);
void CutsceneDuelKirby(void);
void CutsceneActorScript1(void);

/* src/cutscene_actor_particles.c */
void sub_080109c8(void);

/* src/cutscene_scenes.c */
void CutsceneActorDrawPlayerFrame(void);
void CutsceneActorScript2(void);
void CutsceneActorScript3(void);
void CutsceneDuelBladeKnight(void);
void CutsceneBeachStart(void);
void CutsceneBeachKirby(void);
void CutsceneBeachChair(void);
void CutsceneBeachCandyDream(void);
void CutsceneBeachDonutDream(void);
void CutsceneBeachMeatDream(void);
s32 CutsceneBeachThoughtBubble(void);
void CutsceneBeachWaddleDoo(void);
void CutsceneBeachSunglasses(void);
void CutsceneBeachQuestionMarks(void);
void CutsceneBombStart(void);
void CutsceneBombKirby(void);
void CutsceneBombPoppyBrosSr(void);
void CutsceneBombHeldBomb(void);
void CutsceneBombThrownBomb(void);
void CutsceneBombActorScript18(void);
void CutsceneBombHeldBombFuse(void);
void sub_08012fe0(void);
void CutsceneBombThrownBombFuse(void);
void CutsceneBombActorScript21(void);
void CutsceneBalloonsStart(void);
void CutsceneBalloonsKirby(void);
void CutsceneBalloonsLooseBalloon(void);
void CutsceneBalloonsYellowBalloon(void);
void CutsceneBalloonsGreenBalloon(void);
void CutsceneBalloonsLastBalloon(void);
void CutsceneBalloonsActorScript27(void);
void CutsceneBalloonsActorScript28(void);
void CutsceneBalloonsActorScript29(void);
void CutsceneTomatoStart(void);
void CutsceneTomatoKirby(void);
void CutsceneTomatoMaximTomato(void);
void CutsceneTomatoActorScript32(void);
void CutsceneTomatoExclamation(void);
void CutsceneShipStart(void);
void CutsceneShipKirby(void);
void CutsceneShipSpyglass(void);
void CutsceneShipPirateHat(void);
void CutsceneShipShark(void);
void CutsceneShipWaves(void);
void CutsceneSingingStart(void);
void CutsceneSingingKirby(void);
void CutsceneSingingActorScript40(void);
void CutsceneSingingRainbowBar(void);
void CutsceneSingingActorScript42(void);
void CutsceneSingingBeamedNotes(void);
void CutsceneSingingDottedNote(void);
void CutsceneSingingActorScript45(void);
void CutsceneSingingQuarterNote(void);
void CutsceneSingingActorScript47(void);
void CutsceneSingingThoughtBubble(void);
void CutsceneSingingTrebleClef(void);
void CutsceneFountainStart(void);

/* src/cutscene_warp_star.c */
void MetaKnightWarpStarRideDraw(void);
void sub_08070ffc(void);
void Task_WarpStar(void);
void WarpStarUpdate(void);
void WarpStarEnterState(void);
void WarpStarAnimateTiles(void);
void WarpStarBoard(void);
u16 WarpStarCopyTilesToRider(s32 idx);
void CreateWarpStar(int x, int y, int c);
void WarpStarHoverEmpty(void);
void WarpStarHoverEmptyUpdate(void);
void WarpStarHoverBoarded(void);
void WarpStarHoverBoardedUpdate(void);
void WarpStarVanish(void);
void sub_08071774(void);
void WarpStarStartFlight(void);
void WarpStarFlightUpdate(void);
void WarpStarFlightEnterState(void);
void WarpStarSetTrail(int a, int b, int c, int d);
void WarpStarStopTrail(void);
void WarpStarEmitTrailStars(void);
void WarpStarDrawFlight(void);
void WarpStarSetRiderState(u16 a);
void WarpStarSetMetaKnightRiderState(u16 a);
void CreateFlyingWarpStar(int x, int y, int c);
void sub_08071d2c(void);
void WarpStarTakeOff(void);
void WarpStarTakeOffUpdate(void);
void WarpStarDescendSlow(void);
void WarpStarDescendSlowUpdate(void);
void WarpStarFlight5(void);
void WarpStarFlight5Update(void);
void WarpStarFlight6(void);
void WarpStarFlight6Update(void);
void WarpStarFlight8(void);
void WarpStarFlight8Update(void);
void WarpStarFlight10(void);
void WarpStarFlight10Update(void);
void WarpStarFlight11(void);
void WarpStarFlight11Update(void);
void WarpStarFlight12(void);
void WarpStarFlight12Update(void);

/* src/cutscene_warp_star_flights.c */
void WarpStarFlight13(void);
void WarpStarFlight13Update(void);
void WarpStarFlight14(void);
void WarpStarFlight14Update(void);
void WarpStarFlight15(void);
void WarpStarFlight15Update(void);
void WarpStarFlight17(void);
void WarpStarFlight17Update(void);
void WarpStarFlight18(void);
void WarpStarFlight18Update(void);
void WarpStarFlight19(void);
void WarpStarFlight19Update(void);
void WarpStarFlight20(void);
void WarpStarFlight20Update(void);
void WarpStarFlight21(void);
void WarpStarFlight21Update(void);
void WarpStarFlight22(void);
void WarpStarFlight22Update(void);
void WarpStarFlight23(void);
void WarpStarFlight23Update(void);
void WarpStarFlight24(void);
void WarpStarFlight24Update(void);
void sub_080743cc(void);
void WarpStarCheckExit(void);
void WarpStarDescendFast(void);
void WarpStarDescendFastUpdate(void);
void Task_WarpStarCamera(void);
void WarpStarCameraUpdate(void);
void WarpStarCameraFollowPlayer(void);
void sub_080745d0(void);
void WarpStarCameraTakeOff(void);
void sub_08074628(void);
void WarpStarCameraPath5(void);
void WarpStarCameraPath6(void);
void WarpStarCameraPath8(void);
void WarpStarCameraPath10(void);
void WarpStarCameraPath11(void);
void WarpStarCameraPath12(void);
void WarpStarCameraPath13(void);
void WarpStarCameraPath14(void);
void WarpStarCameraPath15(void);
void WarpStarCameraPath17(void);
void WarpStarCameraPath18(void);
void WarpStarCameraPath20(void);
void WarpStarCameraPath21(void);
void WarpStarCameraPath23(void);
void WarpStarCameraPath24(void);
void CreateWarpStarTrailStar(int a, int b, int c);

/* src/cutscene_nightmare_power_orb_escape.c */
void Task_WarpStarTrailStar(void);
void sub_08074e8c(void);
void sub_08074ee0(u32 flag);
void sub_08074f48(u8 a);
void Task_NightmarePowerOrbEscape(void);
void CreateNightmarePowerOrbEscapeStars(s32 a);
void Task_NightmarePowerOrbEscapeStar(void);
void NightmarePowerOrbEscapeStarDraw(void);
void sub_080761b4(void);
void PlayerCannonInit(void);
void PlayerCannonUpdate(void);
void PlayerCannonEnterState(void);

/* src/cutscene_cannon.c */
void PlayerCannonStepPose(void);
void sub_08076454(void);
void sub_080764f8(void);
void PlayerCannonSetLaunchVelocityFacing(u16 a);
void PlayerCannonSetLaunchVelocity(u16 a);
void sub_080766ac(void);
void sub_08076710(void);
void sub_08076798(void);
void PlayerCannonCheckExit(void);
void PlayerCannonStepFlight(s32 a);
void sub_080768c8(void);
void sub_08076958(void);
void PlayerCannonAnimateMetaKnightLaunch(void);
void PlayerCannonAnimateMetaKnightArrival(void);
void sub_08076a58(void);
void sub_08076c00(void);
void PlayerCannonWait(void);
void PlayerCannonWaitUpdate(void);
void PlayerCannonLaunchUp(void);
void PlayerCannonLaunchUpUpdate(void);
void PlayerCannonLaunchUpRight(void);
void PlayerCannonLaunchUpRightUpdate(void);
void PlayerCannonLaunchShort(void);
void PlayerCannonLaunchShortUpdate(void);
void PlayerCannonArrive(void);
void PlayerCannonArriveUpdate(void);
void PlayerCannonState5(void);
void PlayerCannonState5Update(void);
void PlayerEnterCannon(s32 id, s32 v);
void PlayerLeaveCannon(s32 id);
void PlayerEndCannonLaunch(s32 id);
void PlayerJumpOutOfCannon(s32 id);
void CannonLaunchPlayers(int a);
void Task_Cannon(void);
u16 CannonPickLaunchState(void);
void CannonLoadPlayer(s32 id);
void sub_0807717c(void);
void sub_080771b0(void);
void sub_080771c4(void);
void CannonFire(void);
void CannonInit(void);
void CannonUpdate(void);
void CannonEnterState(void);
void CannonWait(void);
void CannonWaitUpdate(void);
void CannonLaunchShort(void);
void CannonLaunchShortUpdate(void);
void CannonLaunchUp(void);
void CannonLaunchUpUpdate(void);
void CannonLaunchUpRight(void);
void CannonLaunchUpRightUpdate(void);
void Task_CannonFuse(void);
void CannonFuseInitBurn(void);
s32 CannonFuseGetPieceFrame(struct CannonFusePiece *p);
void CannonFuseEnterPiece(s32 x, s32 y, s32 d);
void CannonFuseBurnStep(void);
void CannonFuseStepCell(struct CannonFusePiece *p);
void CannonFuseRestoreStep(void);
void CannonFuseMoveSpark(void);
void CreateCannonFuseSpark(void);
void CannonFuseInit(void);
void CannonFuseUpdate(void);
void CannonFuseEnterState(void);

/* src/cutscene_big_switch_room_particles.c */
void CannonFuseWait(void);
void CannonFuseWaitUpdate(void);
void CannonFuseBurn(void);
void CannonFuseBurnUpdate(void);
void CannonFuseRestore(void);
void CannonFuseRestoreUpdate(void);
void Task_BigSwitch(void);
s32 BigSwitchHitterCanPress(void);
void sub_08077cd4(void);
void BigSwitchStartPress(void);
void BigSwitchStartRefill(s32 id);
void BigSwitchRefillHealth(void);
void BigSwitchInit(void);
void BigSwitchUpdate(void);
void BigSwitchEnterState(void);
void BigSwitchWait(void);
void BigSwitchWaitUpdate(void);
void BigSwitchPress(void);
void BigSwitchPressUpdate(void);
void BigSwitchRefill(void);
void BigSwitchRefillUpdate(void);
void Task_Stake(void);
void StakeInit(void);
void StakeUpdate(void);
void StakeState0(void);
void StakeState0Update(void);
void RoomParticlesDrawFixed(void);
void RoomParticlesDrawBelowLine(void);
void RoomParticlesDrawRepeated(void);
void RoomParticleStepX(struct RoomParticle *p);
void RoomParticleInit(struct RoomParticle *p, u8 a, u8 b);
void RoomParticleDrawFixed(struct RoomParticle *p);
void sub_08078258(struct RoomParticle *p);
void RoomParticleDrawScrolled(struct RoomParticle *p);
void RoomParticleDrawRepeated(struct RoomParticle *p);
u8 RoomParticleIsOnScreen(s16 x, s16 y);
void RoomParticleStepY(struct RoomParticle *p, u8 a);
void Task_RoomParticles(void);
void RoomParticlesVariant0(void);
void sub_080786b4(void);
void RoomParticlesVariant1(void);
void sub_08078734(void);
void RoomParticlesVariant2(void);
void sub_080787b8(void);
void RoomParticlesVariant3(void);
void sub_0807883c(void);
void RoomParticlesVariant4(void);
void sub_080788e0(void);
void Task_WaddleDee(void);
s32 ParasolWaddleDeeReactToDefeat(void);
s32 WaddleDeeStartFall(void);
s32 WaddleDeeLand(void);
s32 WaddleDeeEnterWater(void);
s32 WaddleDeeHitWall(void);
s32 WaddleDeeHitCeiling(void);

#endif /* GUARD_CUTSCENE_H */
