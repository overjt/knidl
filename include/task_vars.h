#ifndef GUARD_TASK_VARS_H
#define GUARD_TASK_VARS_H

/*
 * Per-family names of struct Task's registers (#155 run 5).
 *
 * unk18-unk34, unk46, unk6C-unk70 and unk74 hold a different thing in every
 * task family: a timer, a counter, a child's slot.  Like pret's
 * `#define tState data[0]` over struct Task's data[16], each family names
 * the registers it uses with object-like macros, and its code writes
 * `gCurTask->fireLionHopCount` for `gCurTask->unk6C`.  A macro changes no
 * token after preprocessing; it is used only on a pointer proven to hold a
 * task of its family, and shared helpers keep the plain unkXX.  Each line
 * gives the member, the type the code reads it as (the sites keep their
 * casts) and the role.  Generated and checked by tools/task_alias.py
 * (docs/header-conventions.md, "Per-family registers").
 */

/* Actor - every actor: a task made by CreateActor, sub_08064a78 or
   sub_08064d9c (src/actor_63698.c), which also bind Task.u8C.actor; the room
   objects of kinds 0-6 come through CreateActor */
#define actorAnimDelay18 unk18 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) in unk18 */
#define actorAnimDelay1C unk1C /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) in unk1C */
#define actorAnimDelay24 unk24 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) */
#define actorAnimDelay unk28 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) */
#define actorAnimDelay30 unk30 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) in unk30 */
#define actorSpawnArg unk74 /* u8: the spawn argument (CreateActor's p4, ActorSpawn.spawnArg), set once at creation */

/* Bonkers - Bonkers (task type #49, Task_Bonkers; gBonkersVariants,
   gBonkersStates / gBonkersStateUpdates) */
#define bonkersIgnoreTerrainTimer unk18 /* s32: frames left in which BonkersUpdate skips ActorCollideTerrain (24 if sub_08067060) */
#define bonkersMoveLength unk1C /* s32: the length of the walk, the hop series or the dash (walk cycles, hops, frames) */
#define bonkersDashStepCount unk24 /* s32: animation steps of the dash so far; a dust trail on every odd one */
#define bonkersSequencePhase unk28 /* s32: step 4..0 through the state sequence gUnk_08743744 (BonkersChooseNextState) */
#define bonkersSlamsLeft unk2C /* s32: slams (state 6) left before a dash must end in the triple slam (state 8) */
#define bonkersFlashing unk30 /* s32: 1 after a damage hit until the hit timer ends; BonkersUpdate flashes the palette meanwhile */
#define bonkersDefeatPhase unk34 /* s32: 0 until defeated, 1 once the defeat hook ran, 2 when the defeat's wait is over */
#define bonkersHammerHitBoxSlot unk46 /* s16: the hammer hit box child (task type #177) Task_Bonkers creates; never read */
#define bonkersNutSlot unk46 /* s16: the nut actor (task type #110) BonkersThrow just created; never read */
#define bonkersLoopCount unk6C /* s16: the running state's loop counter (walk cycles, hops, dash frames, swings, slams, shakes) */

/* BonkersHammerHitBox - Bonkers' hammer hit box (task type #177,
   Task_BonkersHammerHitBox; created by Task_Bonkers) */
#define bonkersHammerHitBoxIndex unk28 /* s32: the parent's frame minus 16 (0-6); indexes the hammer box offsets and boxes */

/* BonkersNut - Bonkers' nut (task type #110, Task_BonkersNut;
   gBonkersNutVariants, gBonkersNutStates; spawned by BonkersThrow) */
#define bonkersNutBounced unk28 /* s32: 1 once the nut's two bounces are over; its update then breaks it (ActorDie) */

/* BrontoBurt - Bronto Burt (task type #20, Task_BrontoBurt;
   gBrontoBurtVariants: rows Wave, 1, Swoop, Diagonal, Chase, TakeOff, Idle)
   */
#define brontoBurtSteerTimer unk28 /* s32: frames counted up to 8 between the Chase row's TaskAccelerateTowardNearestPlayer steps */
#define brontoBurtFlightAngle unk2C /* s32: flight angle of the Diagonal row (64 + 128k of 512: one of the four diagonals) */
#define brontoBurtChaseTimer unk30 /* s32: frames the Chase row steers toward the nearest player (384); then it flies off */
#define brontoBurtTurnCount unk30 /* s32: vertical turns left in the Wave row (4, one every 40 frames); then it keeps rising */
#define brontoBurtAtPlayerHeight unk34 /* s32: 1 once the falling Swoop row is within 15 px of the nearest player's height */
#define brontoBurtBounceTimer unk34 /* s32: frames of the bounce animation left after a terrain bounce (20); at 0 the flap restarts */
#define brontoBurtGrounded unk34 /* s32: 1 while the TakeOff row waits on the ground; only then the update runs terrain checks */
#define brontoBurtSteerDirY unk34 /* s32: vertical sign of the last steering step (-1 up, 0, 1 down); picks the flap speed */
#define brontoBurtTurnTimer unk34 /* s32: frames to the next vertical turn of the wave (40) */

/* BroomHatter - Broom Hatter (task type #11, Task_BroomHatter;
   gBroomHatterVariants: two sweeping rows and Idle) */
#define broomHatterSweepDone unk28 /* s32: set when the sweep loop of state 0 (to 2) or state 1 (to 1) ends; its update then re-picks */
#define broomHatterLanded unk30 /* s32: 1 once BroomHatterLand has run; row 1's sweeps move only when it is set */
#define broomHatterMove unk34 /* s32: move picked last (0: state 1, 1: state 0, 2: state 0 turned around); never repeated */
#define broomHatterLoopCount unk6C /* s16: states 0/1 loop counter: motion-and-frame cycles done (2 in state 0, 3 in state 1) */

/* Bubbles - Bubbles (task type #30, Task_Bubbles; gBubblesVariants,
   gBubblesStates) */
#define bubblesRollPhase unk28 /* s32: roll phase 0-15 of the ball: BubblesSetRollFrame's frame and flip index */
#define bubblesJumpKind unk34 /* s32: jump BubblesJump picked (0-2): velY -3/-4/-5 px (gUnk_0874276C), spin per 4/8/no frames */

/* Bugzzy - Bugzzy (task type #54, Task_Bugzzy; gBugzzyVariants, gBugzzyStates
   / gBugzzyStateUpdates) */
#define bugzzyRushing unk18 /* s32: 1 from the charge's rush to a catch, a wall or the defeat; the afterimages follow it */
#define bugzzyHeldPlayerSlot unk1C /* s32: the caught player's task slot (HoldPlayer, SetHeldPlayerState), -1 if none */
#define bugzzyIgnoreTerrainTimer unk20 /* s32: frames left in which BugzzyUpdate skips ActorCollideTerrain (24 if sub_08067060) */
#define bugzzySavedPalette unk24 /* s32: Actor.palette saved while a blink frame draws with palette 0 */
#define bugzzyBackdropMove unk28 /* s32: which Backdrop throw (0-3) BugzzyChooseBackdrop picked; kept to avoid repeating it */
#define bugzzyWalkCount unk2C /* s32: BugzzyWalk runs since the last summon; the third enters state 2 (BugzzySummon) */
#define bugzzyFlashing unk30 /* s32: 1 after a damage hit until the hit timer ends; BugzzyUpdate flashes the palette meanwhile */
#define bugzzyBoxSet unk34 /* s32: index of the box set BugzzyUpdate applies each frame (attack box, two boxes, catch box) */
#define bugzzyLoopCount unk6C /* s16: iterations of the running move's loop (steps, flaps, hops, throws, shakes), counted from 0 */

/* BugzzyAfterimage - Bugzzy's afterimage (task type #200,
   Task_BugzzyAfterimage; created by BugzzyCharge) */
#define bugzzyAfterimageMoveTimer unk28 /* s32: frames until the afterimage jumps to Bugzzy's position again (every 4) */
#define bugzzyAfterimageLifeTimer unk2C /* s32: frames left to show (odd frames hidden); cleared when Bugzzy stops rushing */
#define bugzzyAfterimageSpawnArg unk74 /* u8: Bugzzy's actorSpawnArg, copied in by BugzzyCharge; indexes the life lengths gUnk_08743B48 */

/* BugzzyLadybug - Bugzzy's ladybug (task type #133, Task_BugzzyLadybug;
   gBugzzyLadybugVariants, gBugzzyLadybugStates; spawned by BugzzySummon) */
#define bugzzyLadybugAngle unk28 /* s32: the launch angle (AngleToVector): gUnk_087441BC[variant] by facing, plus 384 */
#define bugzzyLadybugHitsActive unk2C /* s32: 1 once the launch is over; BugzzyLadybugUpdate checks hits only then */
#define bugzzyLadybugSpeed unk30 /* s32: the launch speed (AngleToVector's length): 384, less 48 per launch frame */
#define bugzzyLadybugFrameTimer unk34 /* s32: frames until the sprite toggles frame bit 0 again (every 2) */
#define bugzzyLadybugLoopCount unk6C /* s16: iterations of BugzzyLadybugState0's two loops (launch frames, bobs), counted from 0 */

/* FireLion - Fire Lion (task type #55, Task_FireLion; gFireLionStates /
   gFireLionStateUpdates) */
#define fireLionHeldPlayerSlot unk18 /* s32: the caught player's task slot (HoldPlayer, SetHeldPlayerState), -1 if none */
#define fireLionGlowing unk1C /* s32: 1 while the palette also blends toward gUnk_082B07BC (state 9's leap, the defeat) */
#define fireLionDustTimer unk20 /* s32: frames since the last dust trail while the defeated lion slides (a puff every 16) */
#define fireLionFlameSlots unk24 /* s32: the four flame children's slots, one per byte (cleared after; unreferenced reader) */
#define fireLionWallHit unk2C /* s32: set by the wall hook FireLionHitWall; cleared before each move, then tested */
#define fireLionCatchActive unk30 /* s32: 1 while sub_08096a40 tests the catch boxes (the lunges, state 8's leap) */
#define fireLionLanded unk34 /* s32: set by the land hook FireLionLand; cleared before each jump and waited on */
#define fireLionFlameSlot unk46 /* s16: the flame child (Task_FireLionFlame, task type #214) just created by sub_08095834 */
#define fireLionLoopCount unk6C /* s16: iterations of the running move's loop (hops, lunges, swipes, shakes), counted from 0 */
#define fireLionSequencePhase unk6E /* s16: step 0-7 through the state sequence gUnk_087445E8[actorSpawnArg] (FireLionChooseNextState) */
#define fireLionPalettePhase unk70 /* s16: step 0-7 of the palette blend cycle, indexes the ratios gUnk_087445D8 */

/* GrandWheelie - Grand Wheelie (task type #51, Task_GrandWheelie;
   gGrandWheelieVariants, gGrandWheelieStates / gGrandWheelieStateUpdates) */
#define grandWheelieRushCooldown unk18 /* s32: 0 when the second rush (state 7) and the summon may come; state 7 sets 1, counted to 4 */
#define grandWheelieDustTimer unk1C /* s32: frames since the last dust trail while skidding (every 8) or sliding defeated (every 16) */
#define grandWheelieGroundClass unk1C /* s8: the ground class sub_08094d10 gave last (-1 in the air); a change re-picks the roll speed */
#define grandWheelieFlashTimer unk20 /* s32: frames left of the hit flash after a damage hit (ActorFlashPalette while > 0) */
#define grandWheelieDefeatPhase unk2C /* s32: the defeat's step: 0 knocked into the air, 1 landed and sliding, 2 stopped */
#define grandWheelieHopsLeft unk2C /* s32: jumps left in state 3 (1 or 3, from sub_08094908) */
#define grandWheelieLanded unk2C /* s32: set by the land hook GrandWheelieLand in states 8-10; cleared before a jump and waited on */
#define grandWheelieRushRange unk2C /* s32: during the rush: 1 within 63 px of the player, 0 farther ahead, -1 farther after passing */
#define grandWheelieSavedPixelX unk2C /* s32: the pixel X at the start of state 2, restored when its jitter ends */
#define grandWheelieSkidDir unk2C /* s32: the facing when the skid began; GrandWheelieBrake decelerates against it */
#define grandWheelieDefeatDone unk30 /* s32: 1 once the defeat's 170-frame wait has ended; the update then runs ActorDie */
#define grandWheelieRushPhase unk30 /* s32: state 7's step: 0 skid, 1 rush with the dash flame, 2 done */
#define grandWheelieSkidTimer unk30 /* s32: frames left of state 5's skid before it picks state 4 or 2 */
#define grandWheelieStandTimer unk30 /* s32: frames left in state 2 before it picks state 3 or 4 */
#define grandWheelieSummonPhase unk34 /* s32: step 0-3 of GrandWheelieCheckSummon's count; the summon test runs on 3 */
#define grandWheelieFlameSlot unk46 /* s16: the dash flame child (CreateDashFlame, task type #167) of the rush */
#define grandWheelieMiniWheelieSlot unk46 /* s16: the mini wheelie actor (task type #114) GrandWheelieSummon just created */

/* GrandWheelieMiniWheelie - Grand Wheelie's mini wheelie (task type #114,
   Task_GrandWheelieMiniWheelie; spawned by GrandWheelieSummon) */
#define grandWheelieMiniWheelieGroundClass unk1C /* s8: the ground class sub_08094d10 gave last (-1 after a landing); indexes the roll speeds */
#define grandWheelieMiniWheelieRolling unk2C /* s32: 0 while thrown in the air, 1 once it has landed and rolls */

/* KingDedede - King Dedede (task type #59, Task_KingDedede;
   gKingDededeStates) */
#define kingDededePickCount unk18 /* s32: moves picked since the last Float; the sixth pick is a Float (state 4) */
#define kingDededeNextState unk1C /* s32: the state KingDededeWalk hands over to once close enough (pair of gUnk_087482A8) */
#define kingDededeFloatTimer unk20 /* s32: frames KingDededeFloat drifts after the player (300); -2 while puffing up */
#define kingDededeInhaling unk20 /* s32: nonzero while the inhale is on (sub_080a0098 sets 1, KingDededeInhale clears it) */
#define kingDededeSlamCount unk20 /* s32: slams in this KingDededeSlam (1, or 4 every fourth time); the loop runs unk20 - 1 more */
#define kingDededeWaitTimer unk20 /* s32: frames left in the wait (state 0 / KingDededeWait) before the next move */
#define kingDededeWalkTargetX unk20 /* s32: X KingDededeWalk heads for when its next state is 8 (by the player, inside the room) */
#define kingDededeFloatBumpTimer unk24 /* s32: frames the Float shows the bump frame 23 after hitting floor or ceiling; -2 = none */
#define kingDededeJumpForward unk24 /* s32: 1 when the state-6 jump also moves toward the player, 0 straight up */
#define kingDededeSlamKind unk24 /* s32: KingDededeSlam's kind: 0 one slam, 1 four slams, 2 sub_080a0768 (player high) */
#define kingDededeSlamSetupCount unk30 /* s32: KingDededeSlam setups mod 4 (player low); at 0 the slam is the four-slam kind */
#define kingDededeInhaleTimer unk34 /* s32: frames the inhale goes on catching nothing (90) before it stops */
#define kingDededeWalkStopDist unk34 /* s32: distance to the target at which the walk hands over (46 Jump, 52 hammer, 8 spot, 0) */
#define kingDededeChildSlot unk46 /* s16: slot of the running state's child (air puff, hammer / inhale hit box, dust); -1 none */
#define kingDededeLoopCount unk6C /* s16: iterations of the running state's loop: extra slams, puff-up and step cycles */
#define kingDededeThirdHealth unk70 /* s16: a third of the start health (ActorComputeHealth * 85 >> 8); below it the faster moves */

/* Kracko - Kracko (task type #65, Task_Kracko; gKrackoVariants: Kracko Jr.'s
   gKrackoJrStates, then gKrackoStates on the same task) */
#define krackoJrMoveAxis unk18 /* s32: axis of the sub-step 1-2 pass: 0 horizontal, 1 vertical */
#define krackoJrTargetX unk18 /* s32: X of the spot sub-step 8 flies to (the player's pixelX when the charge began) */
#define krackoLastPick unk18 /* s32: the state state 1 last picked (4 or 5 first); a LightningSweep is not picked twice */
#define krackoJrSteerTimer unk1C /* s32: frames until Kracko Jr. turns its heading toward the player again (every 4) */
#define krackoJrMoveDir unk20 /* s32: side of the player on the pass's axis when it began: 0 negative, 1 positive */
#define krackoJrTargetY unk20 /* s32: Y of the spot sub-step 8 flies to (the player's pixelY, | 15 on ground) */
#define krackoJrAccelTimer unk24 /* s32: frames until sub-step 3 accelerates toward the player again (every 4) */
#define krackoThunderSfxTimer unk24 /* s32: frames until KrackoLightningSweep plays the next thunder sound (every 10) */
#define krackoJrPhase unk28 /* s32: Kracko Jr.'s sub-step: KrackoJrState0Update's switch (0-10), KrackoJrTransformUpdate's */
#define krackoPickCount unk28 /* s32: picks since the last KrackoSummon; 128 right after one (no Summon twice in a row) */
#define krackoJrStepTimer unk2C /* s32: frames counted up in sub-steps 0-2: 32 before a pass, then a stop roll every 64 */
#define krackoPickPhase unk2C /* s32: what state 1 does next: 0 opening moves, 1 pick a move, 2 go to the side spot, 3 idle */
#define krackoSide unk30 /* s32: which half of the view Kracko is on: 0 left, 1 right (index of gUnk_087490B0/E4) */
#define krackoDefeatStage unk34 /* s32: 0 fighting, 1 defeated (KrackoReactToDefeat), 2 star rod piece dropped */
#define krackoJrAirborneTimer unk34 /* s32: frames the target player has stayed in the air; 0x8000 once past 119 (then sub-step 3) */
#define krackoOrbsSlot unk46 /* s16: task slot of Kracko Jr.'s orbs (#195); the defeat sweep spares that task */
#define krackoLoopCount unk6C /* s16: iterations of the running state's loop: shake passes, bob steps, players checked */
#define krackoBoltCount unk6E /* s16: bolts dropped in this pass of KrackoLightningSweep (0-3); index of gUnk_08749100 */
#define krackoBoltFrameCount unk70 /* s16: frames since the last bolt in KrackoLightningSweep (0-3) */

/* LaserBall - Laser Ball (task type #13, Task_LaserBall; gLaserBallVariants,
   gLaserBallStates) */
#define laserBallTargetDir unk18 /* s32: 8-way direction (0-7) to the target point, recomputed every 16 frames */
#define laserBallTurnPending unk1C /* s32: 1 when the nearest player changed sides; the next loop plays the turn frames and clears it */
#define laserBallPlayerSide unk20 /* s32: TaskGetXDirBitToNearestPlayer() (4 or 8) last seen by LaserBallApproach */
#define laserBallPrevTargetDir unk28 /* s32: the target direction before LaserBallReaim's recomputation */
#define laserBallTargetX unk2C /* s32: pixel X 64 px beside the nearest player, on the Laser Ball's side: the point it flies to */
#define laserBallHoverTimer unk30 /* s32: frames hovered; after 5 each frame may start LaserBallShoot */
#define laserBallShotDone unk30 /* s32: 1 once LaserBallShoot has fired its lasers; the update then enters LaserBallRetreat */
#define laserBallSteerTimer unk30 /* s32: frames left in the approach's 16-frame steering period (re-aim at 0, push at 16 and 8) */
#define laserBallLaserDir unk34 /* s32: 0 facing right, 1 facing left: each laser's spawn arg in LaserBallShoot */
#define laserBallMoveDir unk34 /* s32: 8-way direction (0-7) TaskAccelerateInDir steers the approach along */
#define laserBallShotCount unk6C /* s16: lasers fired in LaserBallShoot (1-3, RandomRange(3) + 1 re-drawn each loop) */
#define laserBallWindUpCount unk6E /* s16: wind-up cycles (frames 6, 7, 4) done before the first laser (loop of 8) */

/* MetaKnight - Meta Knight (task type #61, Task_MetaKnight;
   gMetaKnightStates) */
#define metaKnightFollowOffsetX unk18 /* s32: X offset from the target player that state 1 keeps (+-64, gUnk_08748D60) */
#define metaKnightFollowTimer unk1C /* s32: frames left in state 1 (follow) before sub_080a7190 picks the next state */
#define metaKnightWalkAnimFlags unk20 /* s32: for the walk-frame loop sub_080a6e98: bit 1 fast step delays, bit 0 hold the frame */
#define metaKnightFollowPhase unk24 /* s32: state 1's step: 0 walk to the spot by the player, 1 move with the player, 2 stand */
#define metaKnightGuardFlags unk28 /* s32: guard rolls: bit 7 set by each state change, bits 0-1 rolls left after damage */
#define metaKnightTargetPlayerSlot unk2C /* s32: task slot of the player state 1 last followed (TaskFindNearestPlayer); -1 at init */
#define metaKnightAttackBoxIndex unk30 /* s32: index into gUnk_08748D38, the attack box set each frame; -1 = none (intro, defeat) */
#define metaKnightFollowFlags unk34 /* s32: state-1 flags: bit 0 stopped at a room edge, bits 1-2 keep the side (no near-player pick) */
#define metaKnightLoopCount unk6C /* s16: iterations of the running state's loop: frame steps, spins (SwordSpin), speed steps */
#define metaKnightSpinFrameCount unk6E /* s16: frame steps done in one spin of MetaKnightSwordSpin (0-6) */

/* MrFrosty - Mr. Frosty (task type #52, Task_MrFrosty; gMrFrostyVariants,
   gMrFrostyStates / gMrFrostyStateUpdates) */
#define mrFrostyFlashEnabled unk18 /* s32: 1 until the defeat; MrFrostyUpdate runs the damage flash only while it is set */
#define mrFrostyIceCubeSlot unk1C /* s32: the ice cube actor (task type #115) CreateMrFrostyIceCube created */
#define mrFrostyCollideTerrain unk20 /* s32: 1 while MrFrostyUpdate runs ActorCollideTerrain before the state update */
#define mrFrostyStatePhase unk28 /* s32: 0 in a state's first part, 1 after its event (landing, wind-up); updates test it */
#define mrFrostyFlashTimer unk2C /* s32: frames left of the damage flash (ActorFlashPalette while > 0) */
#define mrFrostyDefeatDone unk30 /* s32: 1 once the defeat's 170-frame wait has ended; state 15's update then runs ActorDie */
#define mrFrostyHopsLeft unk30 /* s32: jumps MrFrostyHop makes, from gUnk_0874561F (stored when state 1 is picked) */
#define mrFrostyTimer unk30 /* s32: frames left of the running state's timed part (or of the wait, set by the state before) */
#define mrFrostyIceCubeTurnsLeft unk34 /* s32: times the ice-cube state 8 may still be picked before state 2 or 1 must come (2) */
#define mrFrostyDustTrailSlot unk46 /* s16: the dust trail child (CreateDustTrail, task type #143) of the dash or the defeat slide */
#define mrFrostyLoopCount unk6C /* s16: the running state's loop counter (wind-up cycles), counted from 0 */

/* MrFrostyIceCube - Mr. Frosty's ice cube (task type #115,
   Task_MrFrostyIceCube; gMrFrostyIceCubeVariants, gMrFrostyIceCubeStates;
   spawned by CreateMrFrostyIceCube) */
#define mrFrostyIceCubeArc unk28 /* s32: the flight's arc: 0 a high lob, 1 a flat fast throw (sub_0809b4fc, by the player's dy) */
#define mrFrostyIceCubeReady unk28 /* s32: 1 once state 0's rise and drop are over; its update then waits for Mr. Frosty's state 12 */
#define mrFrostyIceCubeParentState unk2C /* s32: Mr. Frosty's Task.state, copied each frame; at 12 the cube enters its flight */
#define mrFrostyIceCubeSavedTileWord unk70 /* u16: the tileWord the cube was created with, saved before tileWord is cleared (no reader) */

/* MrShineAndMrBright - Mr. Shine & Mr. Bright (task type #63,
   Task_MrShineAndMrBright; the pair's gMrShineAndMrBrightStates, the twins'
   gMrShineStates / gMrBrightStates) */
#define mrShineAndMrBrightTargetScreenX unk18 /* s32: screen X (16-208) above the player that Mr. Bright's state-4 flight stops at */
#define mrShineAndMrBrightFaceTimer unk1C /* s32: frames until Mr. Shine's wait faces the nearest player again (every 8) */
#define mrShineAndMrBrightKnockedOut unk20 /* s32: nonzero once this twin's health ran out while the other fights (flashes, stays up) */
#define mrShineAndMrBrightArrivedAxes unk24 /* s32: axes (X, Y) on which the flight to the pair task has arrived; 2 = there (states 16-17) */
#define mrShineAndMrBrightLastMove unk24 /* s32: index (0-3) of the last ground move picked from gUnk_087484E4; not picked twice */
#define mrShineAndMrBrightBobFrame unk28 /* s32: frame of Mr. Bright's 48-frame bob in his wait (47 down to 0); picks velY */
#define mrShineAndMrBrightPaletteIndex unk28 /* s32: Mr. Shine's palette cycle step (0-3), index of gUnk_0874850C */
#define mrShineAndMrBrightPaletteTimer unk2C /* s32: frames until Mr. Shine's palette cycle steps again (every 12) */
#define mrShineAndMrBrightJumpDir unk30 /* s32: X direction of the jump (+-1: toward the player, 1 in 4 away); negated at a wall */
#define mrShineAndMrBrightSkyTimer unk30 /* s32: frames the twin stays in its sky state (3, 6, 7) before the next sky move (300) */
#define mrShineAndMrBrightStarTimer unk30 /* s32: frames until Mr. Shine drops the next falling star in state 5 (45) */
#define mrShineAndMrBrightStepTimer unk30 /* s32: frames left of a MrShineWalk lunge step (20); picks the step speed from gUnk_0874852C */
#define mrShineAndMrBrightBeamTimer unk34 /* s32: frames Mr. Bright's beam (state 5) lasts (120); under 96 the dropped stars start */
#define mrShineAndMrBrightStarCount unk34 /* s32: falling stars Mr. Shine still drops in state 5 (8); each star's spawnArg */
#define mrShineAndMrBrightTargetSlot unk34 /* s32: task slot of the player the sky twin goes for (found in the screen band / nearest) */
#define mrShineAndMrBrightDashTrailSlot unk46 /* s16: slot of the dash's trail actor (Mr. Shine's dash flame, Mr. Bright's fire trail); -1 none */
#define mrShineAndMrBrightLoopCount unk6C /* s16: the running state's loop counter (MrShineWalk's four steps, MrBrightHop's hops) */
#define mrShineAndMrBrightFrozen unk6E /* s16: 1 when the pair task stopped this twin's state updates (the joint defeat) */
#define mrShineAndMrBrightFlashing unk70 /* s16: nonzero while the palette hook flashes the twin (states 4, 11, 15); MrShineAndMrBrightEndFlash clears */

/* MrTickTock - Mr. Tick-Tock (task type #53, Task_MrTickTock;
   gMrTickTockVariants, gMrTickTockStates / gMrTickTockStateUpdates) */
#define mrTickTockFlashEnabled unk18 /* s32: 1 until the defeat; MrTickTockUpdate runs the damage flash only while it is set */
#define mrTickTockSfxPlayer unk1C /* s32: what PlaySfx(0x219) returned in state 11, for StopSfxOnPlayer */
#define mrTickTockCollideTerrain unk20 /* s32: 1 while MrTickTockUpdate runs ActorCollideTerrain before the state update */
#define mrTickTockStatePhase unk28 /* s32: 0 in a state's first part, 1 after its event (landing, wall, wind-up); updates test it */
#define mrTickTockFlashTimer unk2C /* s32: frames left of the damage flash (ActorFlashPalette while > 0) */
#define mrTickTockDefeatDone unk30 /* s32: 1 once the defeat's 170-frame wait has ended; state 21's update then runs ActorDie */
#define mrTickTockHopsLeft unk30 /* s32: jumps left in state 2 (MrTickTockHop), from gUnk_087456D0 */
#define mrTickTockTimer unk30 /* s32: frames left (or, in state 16, frames since its start) of the running state's timed part */
#define mrTickTockCheckPhase unk34 /* s32: step of sub_08099db0's count; every fourth call enters state 9 */
#define mrTickTockDustTrailSlot unk46 /* s16: the dust trail child (CreateDustTrail, task type #143) of the dash; sub_08098afc frees it */
#define mrTickTockLoopCount unk6C /* s16: iterations of the running state's frame loop, counted from 0 */
#define mrTickTockNoteOffsetX unk6C /* s16: the X offset of the note CreateMrTickTockNote spawns, gUnk_087456CC[unk70] */
#define mrTickTockRingSlot unk6C /* s16: the ring actor (task type #118) state 8 created; stored, never read */
#define mrTickTockNoteOffsetIndex unk70 /* s16: the random index 0-3 into the note offsets gUnk_087456CC */
#define mrTickTockPrevVelX unk70 /* s16: last frame's velX in state 13; the update stops once the speed grows past it */

/* MrTickTockNote - Mr. Tick-Tock's note (task type #119, Task_MrTickTockNote;
   gMrTickTockNoteVariants, gMrTickTockNoteStates; spawned by
   CreateMrTickTockNote) */
#define mrTickTockNoteDir unk2C /* s32: the flight's X direction: the opposite of the parent's facing (1 or -1) */

/* MrTickTockRing - Mr. Tick-Tock's ring (task type #118, Task_MrTickTockRing;
   gMrTickTockRingVariants, gMrTickTockRingStates; spawned by
   CreateMrTickTockRing) */
#define mrTickTockRingParentState unk28 /* s32: the parent's Task.state, copied each frame; the ring ends once it is not 8 or 13 */
#define mrTickTockRingSfxPlayer unk2C /* s32: what PlaySfx(0x219) returned, for StopSfxOnPlayer when the ring ends */

/* NightmareWizard - Nightmare Wizard (task type #67, Task_NightmareWizard;
   gNightmareWizardStates) */
#define nightmareWizardMaxHealth unk1C /* s32: health at the start (ActorComputeHealth); under a third of it the attacks change */
#define nightmareWizardAttackTimer unk20 /* s32: frames state 1 bobs before the spot's attack (60, 18, or 15-60 from gUnk_08749220) */
#define nightmareWizardPalmStarDir unk20 /* s32: direction index the next OpenPalm star gets as its spawnArg (0-4, +1..4 mod 5) */
#define nightmareWizardFrameTimer unk24 /* s32: frames until the next step of the 36-39 robe frames (every 2) in a sweep or OpenPalm */
#define nightmareWizardSfxTimer unk24 /* s32: frames until state 8 plays sound 0x22E again (every 6) */
#define nightmareWizardSpotAttack unk24 /* s32: the current spot's attack code (gUnk_08749224[spot], 0-4), row of gUnk_08749230 */
#define nightmareWizardSpotIndex unk28 /* s32: index into the spot script gUnk_08749224 (0-9, wraps); -1 before the first spot */
#define nightmareWizardWaitIndex unk2C /* s32: index of state 1's wait in gUnk_08749220 (15/30/45/60), stepped by 1-3 mod 4 */
#define nightmareWizardOpenCloakCount unk30 /* s32: OpenCloak attacks made; bit 0 picks the order of the five stars */
#define nightmareWizardVanishCount unk34 /* s32: vanishes made (state 3); every fourth goes to state 10 instead of 2 */
#define nightmareWizardLoopCount unk6C /* s16: iterations of the running state's loop: bob cycles, frame steps, stars shot */

/* NightmareWizardStar - Nightmare Wizard's star (task type #136,
   Task_NightmareWizardStar; gNightmareWizardStarStates) */
#define nightmareWizardStarDone unk28 /* s32: nonzero once the star must die: its 255-frame homing ended or the wizard fell */
#define nightmareWizardStarAngle unk2C /* s32: the row-3 star's flight angle (0-511) from its direction index and facing */
#define nightmareWizardStarHomingTimer unk34 /* s32: frames left of the homing flight (255); it accelerates toward a player every 4th */

/* Noddy - Noddy (task type #10, Task_Noddy; gNoddyVariants, gNoddyStates) */
#define noddySleepTimer unk30 /* s32: frames of the current 224-frame sleep period left (NoddySleep / NoddySleepFall) */
#define noddyWalkTimer unk30 /* s32: frames NoddyWalk walks (gUnk_08741FA8[spawn arg]) before it sits down (state 1) */
#define noddySleepPeriodCount unk34 /* s32: 224-frame sleep periods left (starts at 1); NoddySleep wakes when it and the timer are 0 */
#define noddyBubbleSlot unk46 /* s16: slot of the last Task_NoddyBubble (type 194) the sleeping loop puffed */

/* PhanPhan - Phan Phan (task type #56, Task_PhanPhan; gPhanPhanStates /
   gPhanPhanStateUpdates) */
#define phanPhanHeldPlayerSlot unk18 /* s32: the caught player's task slot (HoldPlayer, SetHeldPlayerState), -1 if none */
#define phanPhanMoveCount unk1C /* s32: moves picked since the last apple throw; the third enters state 7 (PhanPhanThrowApple) */
#define phanPhanLanded unk28 /* s32: set by the land hook PhanPhanLand; cleared before each jump and waited on */
#define phanPhanCatchActive unk2C /* s32: 1 while PhanPhanCheckCatch tests the catch box (states 1, 2, 3 and 5) */
#define phanPhanAppleSlot unk46 /* s16: the apple actor (task type #138) CreatePhanPhanApple just created; never read */
#define phanPhanLoopCount unk6C /* s16: the running state's loop counter (moves, hops, shakes, swings), counted from 0 */

/* PhanPhanApple - Phan Phan's apple (task type #138, Task_PhanPhanApple;
   spawned by CreatePhanPhanApple) */
#define phanPhanAppleLanded unk34 /* s32: set by the land hook PhanPhanAppleLand; cleared before each bounce and waited on */

/* PoppyBrosSr - Poppy Bros. Sr. (task type #50, Task_PoppyBrosSr;
   gPoppyBrosSrVariants, gPoppyBrosSrStates / gPoppyBrosSrStateUpdates) */
#define poppyBrosSrIgnoreTerrainTimer unk18 /* s32: frames left in which PoppyBrosSrUpdate skips ActorCollideTerrain (24 if sub_08067060) */
#define poppyBrosSrHopsLeft unk1C /* s32: hops left in sub_08091954's loop (state 1: gUnk_087438DC, state 3: 2) */
#define poppyBrosSrPlayerNearY unk1C /* s32: state 2: 1 when the nearest player is within 63 px vertically (the bomb's variant) */
#define poppyBrosSrAimTimer unk20 /* s32: state 2: frames left before the hand throws, facing the player meanwhile (-1 outside) */
#define poppyBrosSrHopPhase unk20 /* s32: 0/1, toggled each hop; indexes the X speeds gUnk_087438E4 and the hop frames */
#define poppyBrosSrFrameDelay unk24 /* s32: frames each step of the hop frame sequences lasts (3, 6, 7 or 10) */
#define poppyBrosSrPlayerDistX unk24 /* s32: state 2: abs dx to the nearest player, read once to pick the aim time */
#define poppyBrosSrDefeatPhase unk2C /* s32: 0 until defeated, 1 once the defeat hook ran, 2 when the defeat's wait is over */
#define poppyBrosSrHeadAnimIndex unk30 /* s32: which head script gPoppyBrosSrHeadAnims[] the head child plays (0/1 hops, 2 throw, 3 fall) */
#define poppyBrosSrFlashing unk34 /* s32: 1 after a damage hit until hitTimer reaches 1; PoppyBrosSrUpdate flashes the palette */
#define poppyBrosSrHeadSlot unk46 /* s16: the head child (task type #179, Task_PoppyBrosSrHead); never read */

/* PoppyBrosSrBomb - Poppy Bros. Sr.'s bomb (task type #111,
   Task_PoppyBrosSrBomb; gPoppyBrosSrBombVariants, gPoppyBrosSrBombStates;
   spawned by Task_PoppyBrosSrHand) */
#define poppyBrosSrBombBouncesLeft unk28 /* s32: bounces left (3) before the bomb bursts at a landing */

/* PoppyBrosSrHand - Poppy Bros. Sr.'s hand (task type #180,
   Task_PoppyBrosSrHand; created by PoppyBrosSrState2) */
#define poppyBrosSrHandBombVariant unk18 /* s32: the variant of the bomb it holds: the parent's poppyBrosSrPlayerNearY */
#define poppyBrosSrHandReleased unk1C /* s32: 0 while holding the bomb, 1 once it lets go (the bomb then starts its flight) */
#define poppyBrosSrHandOffsetVelX unk28 /* s32: the X step added to the hand's offset each frame (16.16) */
#define poppyBrosSrHandOffsetX unk2C /* s32: the hand's X offset from the parent (16.16, times the parent's facing) */
#define poppyBrosSrHandOffsetVelY unk30 /* s32: the Y step added to the hand's offset each frame (16.16) */
#define poppyBrosSrHandOffsetY unk34 /* s32: the hand's Y offset from the parent (16.16) */
#define poppyBrosSrHandBombSlot unk46 /* s16: the bomb actor (task type #111) the hand creates; never read */
#define poppyBrosSrHandLoopCount unk6C /* s16: the hand's loop counter (the frames of its three moves), counted from 0 */

/* PoppyBrosSrHead - Poppy Bros. Sr.'s head (task type #179,
   Task_PoppyBrosSrHead; created by PoppyBrosSrInit) */
#define poppyBrosSrHeadStepTimer unk28 /* s32: frames until the head script's next AnimCmd step (0 at the script's end) */
#define poppyBrosSrHeadScriptPos unk2C /* s32: the index of the current AnimCmd in the head script */
#define poppyBrosSrHeadShownIndex unk30 /* s32: the head script it plays: the parent's poppyBrosSrHeadAnimIndex it last saw (-1 at start) */

/* Shotzo - Shotzo (task type #25, Task_Shotzo; gShotzoVariants: rows Aim,
   Fixed x3, ParasolShotzo, Idle) */
#define shotzoPrevBarrelDir unk18 /* s32: barrel direction before ShotzoStepBarrel's last step (picks the in-between frame 6) */
#define shotzoBarrelOnTarget unk1C /* s32: 1 when the barrel direction unk34 equals the target direction unk30 */
#define shotzoTargetInArc unk20 /* s32: 1 unless the nearest player is over 22.5 degrees below the horizontal (out of the arc) */
#define shotzoAimTimer unk28 /* s32: frames to the next aim check (gUnk_08743248[shotzoSpeedLevel]: 80/60/40/20) */
#define shotzoShotDone unk28 /* s32: 1 once the shot (the Fixed row: the first volley) is over; the Shoot update may leave */
#define shotzoArmed unk2C /* s32: 1 after one on-target aim check since the last shot; the next on-target check fires */
#define shotzoFixedRecoilStep unk2C /* s32: recoil step 0-3 of the Fixed row's shot: index into gUnk_0874325A[variant] */
#define shotzoTargetBarrelDir unk30 /* s32: barrel direction 0-4 that points at the nearest player; ShotzoStepBarrel steps unk34 to it */
#define shotzoBarrelDir unk34 /* s32: barrel direction 0-4 (right, up-right, up, up-left, left): frame, muzzle and recoil index */
#define shotzoSmokeRingSlot unk46 /* s16: slot of the Task_SmokeRing (type 172) puffed at the muzzle with each cannonball */
#define shotzoLoopCount unk6C /* s16: the Shoot states' loop counter (aiming rows: 4 recoil steps; Fixed: 3 shots per volley) */
#define shotzoFixedRecoilCount unk6E /* s16: recoil steps done in the Fixed row's shot (loop of 4, 2 frames each) */
#define shotzoRecoilStep unk6E /* s16: recoil step 0-3 of the aiming rows' shot: index into gUnk_0874325A[shotzoBarrelDir] */
#define shotzoRecoilDone unk70 /* s16: 1 while no recoil runs: cleared as a cannonball is fired, set after its recoil */
#define shotzoSpeedLevel unk74 /* u8: speed level 0-3 (the spawn arg; ParasolShotzoAim forces 1): aim period, cannonball speed */

/* ShotzoCannonball - Shotzo's cannonball (task type #109,
   Task_ShotzoCannonball; gShotzoCannonballStates) */
#define shotzoCannonballLifeTimer unk28 /* s32: frames until it dies (gUnk_08743614[spawn arg]: 214/150/107/83, the Fixed row's 38) */
#define shotzoCannonballDir unk30 /* s32: the parent Shotzo's barrel direction 0-4 it is fired along */

/* Slippy - Slippy (task type #34, Task_Slippy; gSlippyVariants,
   gSlippyStates) */
#define slippyCollideTerrain unk28 /* s32: 1 when SlippyUpdate runs ActorCollideTerrain (0 while it starts in or leaves the water) */
#define slippySwimAngle unk2C /* s32: swim heading (512 per turn) the water states pass to AngleToVector */
#define slippyMovePickCount unk34 /* s32: calls of SlippyPickMove; its parity picks one of the two weight tables */
#define slippyLoopCount unk6C /* s16: SlippyState1's loop counter: frame cycles done (gUnk_08742820[spawn arg]: 3 or 2) */

/* Twizzy - Twizzy (task type #24, Task_Twizzy; gTwizzyVariants: rows Wave, 1,
   Swoop, Diagonal, Chase, TakeOff, 6, 7, Hover, Idle) */
#define twizzySteerTimer unk28 /* s32: frames counted up to 8 between the Chase row's TaskAccelerateTowardNearestPlayer steps */
#define twizzyFlightAngle unk2C /* s32: flight angle of the Diagonal row (64 + 128k of 512: one of the four diagonals) */
#define twizzyChaseTimer unk30 /* s32: frames the Chase row steers toward the nearest player (384); then it flies off */
#define twizzyTurnCount unk30 /* s32: vertical turns left in the Wave row (4, one every 40 frames); then it keeps rising */
#define twizzyAtPlayerHeight unk34 /* s32: 1 once the falling Swoop row is within 15 px of the nearest player's height */
#define twizzyBounceTimer unk34 /* s32: frames of the bounce animation left after a terrain bounce (20); at 0 the flap restarts */
#define twizzyFaceTimer unk34 /* s32: frames to the Hover row's next TaskFaceNearestPlayer (8) */
#define twizzyGrounded unk34 /* s32: 1 while the TakeOff row waits on the ground; only then the update runs terrain checks */
#define twizzySteerDirY unk34 /* s32: vertical sign of the last steering step (-1 up, 0, 1 down); picks the flap speed */
#define twizzyTurnTimer unk34 /* s32: frames to the next vertical turn of the wave (40) */
#define twizzyFlapCount unk6C /* s16: wing-flap cycles done in row 6's take-off (loop of 4, 8 frames each) */

/* WaddleDoo - Waddle Doo (task type #17, Task_WaddleDoo; gWaddleDooVariants:
   rows Walk, ParasolWaddleDoo, Idle, Shoot) */
#define waddleDooPickTimer unk28 /* s32: frames to the walk's next random pick (15 at spawn, 80 on re-entry, 30 after a miss) */
#define waddleDooAirborne unk2C /* s32: 1 once WaddleDooStartFall has run (row 0's WaddleDooLand clears it); the pick waits */
#define waddleDooBeamStep unk34 /* s32: beam segments fired so far in this shot; each new Task_WaddleDooBeam reads it */
#define waddleDooBeamSlot unk46 /* s16: slot of the last beam actor (task type 106) the Shoot loop created */
#define waddleDooLoopCount unk6C /* s16: the Shoot states' loop counter (walk-frame cycles, wind-up frames, beam segments) */

/* WaddleDooBeam - Waddle Doo's beam (task type #106, Task_WaddleDooBeam) */
#define waddleDooBeamAngleIndex unk34 /* s32: index into the beam angle table gUnk_08742FAC: the parent's beam step (halved for arg 0-1) */
#define waddleDooBeamLoopCount unk6C /* s16: Task_WaddleDooBeam's loop counter: frame 0/1 cycles shown before ActorDestroy (3) */

#endif // GUARD_TASK_VARS_H
