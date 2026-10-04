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
struct M19Frame
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The eight 4-byte records at 0x03000FE0 that M19's credits tasks animate:
   a frame table index (unk00), the frame within it (unk01), the timer
   (unk02) and the countdown RoomParticleDrawScrolled draws on (unk03). */
struct M19Particle
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The 0x087401E4 script records M19's ending-sequence tasks walk: a pointer
   table indexed by Task.unk20, each entry a header plus two `s16` step lists
   (the "forward" list at +6 and the "reverse" one at +18) that
   CannonFuseGetPieceFrame picks between on Task.unk18/unk24. */
struct M19Script
{
    /*0x00*/ u8 unk00[4];
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 unk06[6];
    /*0x12*/ u16 unk12[1];
};

/* EWRAM */
extern u32 gUnk_02004B4C;
extern u32 gUnk_02005584;

/* IWRAM */
extern struct M19Particle gRoomParticles[];

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
extern u32 gUnk_0873F554[];
extern u32 gUnk_0873F5CC[];
extern u32 gUnk_0873F5E4[];
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
extern struct M19Script *gUnk_087401E4[];
extern u32 gCannonFuseStates[];
extern u32 gCannonFuseStateUpdates[];
extern u32 gBigSwitchVariants[];
extern u32 gBigSwitchStates[];
extern u32 gBigSwitchStateUpdates[];
extern u32 gStakeVariants[];
extern u32 gStakeStates[];
extern u32 gStakeStateUpdates[];
extern u32 gRoomParticlesVariants[];
extern struct M19Frame gUnk_08740320[][24];
extern struct M19Frame gUnk_087404A0[][24];
extern u8 gUnk_08740620[];
extern u32 gWaddleDeeVariants[];
extern struct AnimCmd gUnk_087406A0[];
extern u32 gWaddleDeeDef[];
extern u32 gWaddleDeeFrames[];
extern u32 gCannonFrames[];
extern u32 gCannonFuseFrames[];
extern u32 gBigSwitchFrames[];
extern u32 gStakeFrames[];
extern u32 gUnk_08752D8C[];
extern u32 gUnk_08752DB8[];
extern u32 gUnk_08752E00[];
extern u32 gUnk_08754A14[];
extern u32 gUnk_08754ABC[];
extern u32 gUnk_08754B80[];
extern u32 gUnk_08754C90[];
extern u32 gUnk_08754D60[];
extern u32 gUnk_08754E7C[];
extern u32 gUnk_08754F68[];
extern u32 gUnk_0875549C[];


/* Functions (defined in the files named above each group). */

/* src/mode_100ac.c */
void CutsceneMain(void);
void CutsceneLoadGraphics(void);

/* src/player_10358.c */
s32 CreateCutsceneActor(s32 a, s32 b);
void Task_CutsceneDirector(void);
void CutsceneCheckSkip(void);
void Task_CutsceneActor(void);
void CutsceneDuelStart(void);
void CutsceneDuelKirby(void);
void sub_08010834(void);

/* src/player_109c8.c */
void sub_080109c8(void);

/* src/player_10b38.c */
void sub_08010b38(void);
void sub_08010bac(void);
void sub_08010cb4(void);
void CutsceneDuelBladeKnight(void);
void CutsceneBeachStart(void);
void CutsceneBeachKirby(void);
void CutsceneBeachChair(void);
void CutsceneBeachCandyDream(void);
void CutsceneBeachDonutDream(void);
void CutsceneBeachMeatDream(void);
s32 sub_08011880(void);
void CutsceneBeachWaddleDoo(void);
void CutsceneBeachSunglasses(void);
void CutsceneBeachQuestionMarks(void);
void CutsceneBombStart(void);
void CutsceneBombKirby(void);
void CutsceneBombPoppyBrosSr(void);
void CutsceneBombHeldBomb(void);
void CutsceneBombThrownBomb(void);
void sub_08012df8(void);
void sub_08012e6c(void);
void sub_08012fe0(void);
void sub_08013058(void);
void sub_08013348(void);
void CutsceneBalloonsStart(void);
void CutsceneBalloonsKirby(void);
void CutsceneBalloonsLooseBalloon(void);
void CutsceneBalloonsYellowBalloon(void);
void CutsceneBalloonsGreenBalloon(void);
void CutsceneBalloonsLastBalloon(void);
void sub_08014184(void);
void sub_080142a0(void);
void sub_080143b4(void);
void CutsceneTomatoStart(void);
void CutsceneTomatoKirby(void);
void CutsceneTomatoMaximTomato(void);
void sub_08014c08(void);
void CutsceneTomatoExclamation(void);
void CutsceneShipStart(void);
void CutsceneShipKirby(void);
void CutsceneShipSpyglass(void);
void CutsceneShipPirateHat(void);
void CutsceneShipShark(void);
void CutsceneShipWaves(void);
void CutsceneSingingStart(void);
void CutsceneSingingKirby(void);
void sub_08015f18(void);
void CutsceneSingingRainbowBar(void);
void sub_080162a0(void);
void sub_08016514(void);
void sub_080167dc(void);
void sub_08016ac4(void);
void sub_08016dd4(void);
void sub_080170e4(void);
void sub_080173f4(void);
void sub_0801757c(void);
void sub_0801761c(void);

/* src/actor_70ec0.c */
void sub_08070ec0(void);
void sub_08070ffc(void);
void Task_WarpStar(void);
void WarpStarUpdate(void);
void WarpStarEnterState(void);
void WarpStarAnimateTiles(void);
void WarpStarBoard(void);
u16 WarpStarCopyTilesToRider(s32 idx);
void CreateWarpStar(int x, int y, int c);
void WarpStarState0(void);
void WarpStarState0Update(void);
void WarpStarState1(void);
void WarpStarState1Update(void);
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
void sub_08071d60(void);
void sub_08071e74(void);
void sub_08071e80(void);
void sub_08071ebc(void);
void sub_08071f54(void);
void sub_080720dc(void);
void sub_080720e8(void);
void sub_0807237c(void);
void sub_08072388(void);
void sub_08072678(void);
void sub_08072684(void);
void sub_080728a4(void);
void sub_080728b0(void);
void sub_08072af4(void);
void sub_08072b00(void);
void sub_08072d80(void);

/* src/actor_72d8c.c */
void sub_08072d8c(void);
void sub_080731c4(void);
void sub_080731d0(void);
void sub_0807328c(void);
void sub_08073298(void);
void sub_08073578(void);
void sub_08073584(void);
void sub_080737f8(void);
void sub_08073804(void);
void sub_0807395c(void);
void sub_08073968(void);
void sub_080739bc(void);
void sub_08073a54(void);
void sub_08073cd4(void);
void sub_08073ce0(void);
void sub_08073e00(void);
void sub_08073e0c(void);
void sub_08073e80(void);
void sub_08073f18(void);
void sub_0807409c(void);
void sub_080740bc(void);
void sub_080743c8(void);
void sub_080743cc(void);
void sub_080743f0(void);
void sub_08074420(void);
void sub_0807447c(void);
void Task_WarpStarCamera(void);
void WarpStarCameraUpdate(void);
void WarpStarCameraFollowPlayer(void);
void sub_080745d0(void);
void sub_080745dc(void);
void sub_08074628(void);
void sub_08074638(void);
void sub_080746c0(void);
void sub_0807470c(void);
void sub_08074784(void);
void sub_08074794(void);
void sub_080747dc(void);
void sub_080747ec(void);
void sub_08074880(void);
void sub_080748a8(void);
void sub_08074904(void);
void sub_08074974(void);
void sub_08074988(void);
void sub_08074ab8(void);
void sub_08074ac8(void);
void sub_08074b60(void);
void CreateWarpStarTrailStar(int a, int b, int c);

/* src/actor_74c0c.c */
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

/* src/actor_763e8.c */
void sub_080763e8(void);
void sub_08076454(void);
void sub_080764f8(void);
void sub_08076558(u16 a);
void sub_080765f4(u16 a);
void sub_080766ac(void);
void sub_08076710(void);
void sub_08076798(void);
void sub_08076828(void);
void sub_0807685c(s32 a);
void sub_080768c8(void);
void sub_08076958(void);
void sub_080769d0(void);
void sub_08076a14(void);
void sub_08076a58(void);
void sub_08076c00(void);
void PlayerCannonState0(void);
void PlayerCannonState0Update(void);
void PlayerCannonState1(void);
void PlayerCannonState1Update(void);
void PlayerCannonState2(void);
void PlayerCannonState2Update(void);
void PlayerCannonState3(void);
void PlayerCannonState3Update(void);
void PlayerCannonState4(void);
void PlayerCannonState4Update(void);
void PlayerCannonState5(void);
void PlayerCannonState5Update(void);
void PlayerEnterCannon(s32 id, s32 v);
void PlayerLeaveCannon(s32 id);
void sub_08076f04(s32 id);
void sub_08076f50(s32 id);
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
void CannonState1(void);
void CannonState1Update(void);
void CannonState2(void);
void CannonState2Update(void);
void CannonState3(void);
void CannonState3Update(void);
void Task_CannonFuse(void);
void CannonFuseInitBurn(void);
s32 CannonFuseGetPieceFrame(struct M19Script *p);
void CannonFuseEnterPiece(s32 x, s32 y, s32 d);
void CannonFuseBurnStep(void);
void CannonFuseStepCell(struct M19Script *p);
void sub_08077980(void);
void CannonFuseMoveSpark(void);
void CreateCannonFuseSpark(void);
void CannonFuseInit(void);
void CannonFuseUpdate(void);
void CannonFuseEnterState(void);

/* src/actor_77ae0.c */
void CannonFuseWait(void);
void CannonFuseWaitUpdate(void);
void CannonFuseBurn(void);
void CannonFuseBurnUpdate(void);
void CannonFuseState2(void);
void CannonFuseState2Update(void);
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
void BigSwitchState1(void);
void BigSwitchState1Update(void);
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
void RoomParticleStepX(struct M19Particle *p);
void RoomParticleInit(struct M19Particle *p, u8 a, u8 b);
void RoomParticleDrawFixed(struct M19Particle *p);
void sub_08078258(struct M19Particle *p);
void RoomParticleDrawScrolled(struct M19Particle *p);
void RoomParticleDrawRepeated(struct M19Particle *p);
u8 RoomParticleIsOnScreen(s16 x, s16 y);
void RoomParticleStepY(struct M19Particle *p, u8 a);
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
