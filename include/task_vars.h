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
#define actorAnimDelay20 unk20 /* s32: frames until the next step of the animation script (ActorStartAnim / ActorTickAnim) */
#define actorAnimDelay24 unk24 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) */
#define actorAnimDelay unk28 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) */
#define actorAnimDelay30 unk30 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) in unk30 */
#define actorAnimDelay34 unk34 /* s32: frames until the next step of the animation script (ActorStartAnim / ActorStepAnim) */
#define actorDustTrailSlot unk46 /* s16: the dust trail child (CreateDustTrail, task type #143); TaskFreeDustTrail frees it */
#define actorSpawnArg unk74 /* u8: the spawn argument (CreateActor's p4, ActorSpawn.spawnArg), set once at creation */

/* Blipper - Blipper (task type #35, Task_Blipper; gBlipperVariants: rows 0,
   1, 2, 3/4, Idle) */
#define blipperStroking unk28 /* s32: row 0: nonzero when the last steering step accelerated (stroke frames), 0 = glide frame */
#define blipperCollideTerrain unk2C /* s32: rows 3/4: 1 when their update runs ActorCollideTerrain (0 in row 3's leap) */
#define blipperTurnTimer unk30 /* s32: rows 3/4: frames to the next turn-around while swimming (192) */
#define blipperChaseTimer unk34 /* s32: row 0's frame count: steers every 8th frame, chases until 0x700, then swims away */
#define blipperLeapTimer unk34 /* s32: rows 3/4: frames before the next leap check near a player (90 after a leap, 16 on a miss) */
#define blipperSavedFacing unk34 /* s32: row 2's facing, saved at spawn and restored by both of its states */
#define blipperDropletSlot unk46 /* s16: slot of the last Task_BlipperDroplet (task type 218) CreateBlipperDroplet made */
#define blipperLoopCount unk6C /* s16: row 2's state 0 loop counter: zigzag strokes (3) */
#define blipperLeapSpeedIndex unk74 /* u8: leap X speed index (gUnk_08742930): the spawn arg, then 1 / 0 by a player within 32 px */

/* Bomber - Bomber (task type #22, Task_Bomber; gBomberVariants,
   gBomberStates, gBomberIdleStates) */
#define bomberHasWalked unk28 /* s32: 1 once BomberWalk ran a frame: state 2 then teeters before it turns */
#define bomberLoopCount unk6C /* s16: the running state's loop counter (state 2's 5 teeter cycles; the explosion's 4 flashes) */

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

/* Cappy - Cappy (task type #16, Task_Cappy; gCappyVariants,
   gCappyCappedStates, gCappyCaplessStates, gCappyStandStates) */
#define cappyHopCount unk28 /* s32: hops left before it faces the player again (40) */
#define cappyCaplessSlot unk46 /* s16: slot of the capless Cappy spawned on a hit of kind 3, given its room object */

/* Chilly - Chilly (task type #14, Task_Chilly; gChillyVariants,
   gChillyStates) */
#define chillySavedState unk28 /* s32: Task.state saved by ChillyStartFall; ChillyLand re-enters it */
#define chillyFreezeSlot unk46 /* s16: slot of the Task_ChillyFreeze (task type 104) its freeze attack created */
#define chillyLoopCount unk6C /* s16: the running state's loop counter (looks, slide steps, shake steps) */

/* ChillyFreeze - Chilly's freeze (task type #104, Task_ChillyFreeze) */
#define chillyFreezeSparkleTimer unk30 /* s32: frames into the 21-frame sparkle cycle (a Task_ChillyFreezeSparkle at 0 and at 10) */
#define chillyFreezeSfxTimer unk34 /* s32: frames to the next sound 185 (replayed every 5 frames) */
#define chillyFreezeSparkleSlot unk46 /* s16: slot of the Task_ChillyFreezeSparkle (task type 215) just created */
#define chillyFreezeLoopCount unk6C /* s16: Task_ChillyFreeze's loop counter: 4-frame flash cycles before ActorDestroy (60) */

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

/* Flamer - Flamer (task type #43, Task_Flamer; gFlamerVariants,
   gFlamerStates, gFlamerIdleStates) */
#define flamerSurfaceSlope unk18 /* s32: slope class of the surface tile it crawls on (gUnk_087416A4: 0 none, 1 flat, 2-9 slopes) */
#define flamerCrawlPhase unk1C /* s32: phase of FlamerCrawlUpdate: 0 follow the surface, 1 lost it (wrap the corner), 2 recheck */
#define flamerSteerTimer unk1C /* s32: frames until the next steering step of the homing flight (states 4 and 6) */
#define flamerFlightAngle unk20 /* s32: heading of the homing flight, 0-0x1FF, steered 32 units at a time toward the player */
#define flamerFlameTimer unk24 /* s32: frames of state 5's dash, counted up: every 4th spawns a Task_FlamerFlame */
#define flamerCheckTimer unk28 /* s32: frames until the crawl checks for a player within 64 px (bit 7 set: never, variant 1) */
#define flamerDashCount unk2C /* s32: dashes of state 5 done: the first goes on to state 6, a later one flies off */
#define flamerSurfaceSide unk30 /* s32: direction code of the surface it clings to (0 down, 1 left, 2 up, 3 right) */
#define flamerCrawlDir unk34 /* s32: direction code it crawls in (0 down, 1 left, 2 up, 3 right) */
#define flamerFlameSlot unk46 /* s16: slot of the last Task_FlamerFlame state 5 spawned */
#define flamerLoopCount unk6C /* s16: the running state's loop counter (state 3: the 7 hop steps, also its velY table index) */
#define flamerSpeedLevel unk74 /* u8: speed level 0-2: the spawn argument, +1 (at most 2) for variant 1 */

/* FlamerFlame - Flamer's flame (task type #176, Task_FlamerFlame) */
#define flamerFlameArcIndex unk18 /* s32: index 0-3 into gUnk_08741E94 / A4 (velY -1/-0.5/+1/+0.5 px, accelY): set by Flamer */

/* Gip - Gip (task type #47, Task_Gip; gGipVariants, gGipStates) */
#define gipCollideTerrain unk18 /* s32: 1 when GipUpdate runs ActorCollideTerrain before the state update (0 on the wall) */
#define gipShootTimer unk28 /* s32: frames GipWalk walks before GipShoot (120, reloaded after each shot) */
#define gipTurnTimer unk2C /* s32: frames to the walk's next turn check (16): away from a player within 63 px, else toward */
#define gipStarSlot unk46 /* s16: slot of the last Task_GipStar (task type 137) GipShoot spat */
#define gipLoopCount unk6C /* s16: the wall states' loop counter (3 strokes in states 4-7, 8 shakes in state 10) */

/* Glunk - Glunk (task type #33, Task_Glunk; gGlunkVariants, gGlunkStates) */
#define glunkShotSlot unk46 /* s16: slot of the last shot actor (task type 107, Task_GlunkShot) GlunkShoot fired */
#define glunkLoopCount unk6C /* s16: the running state's loop counter (GlunkWait's cycles, GlunkShoot's shots) */

/* GlunkShot - Glunk's shot (task type #107, Task_GlunkShot) */
#define glunkShotLoopCount unk6C /* s16: Task_GlunkShot's loop counter: frame 4/5 cycles before ActorDestroy (4) */

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

/* HeavyMole - Heavy Mole (task type #62, Task_HeavyMole; a timed move script,
   no state table) */
#define heavyMoleScrollSpeed unk18 /* s32: 16.16 X speed HeavyMoleMove adds to the camera anchor each frame (gUnk_0874ACBC) */
#define heavyMoleCameraX unk1C /* s32: the camera anchor X in 16.16 (gCameraAnchorX = unk1C >> 16), moved by the scroll speed */
#define heavyMoleMoveTimer unk28 /* s32: frames left in the current step of the move script; at 0 HeavyMoleStartNextMove */
#define heavyMoleAnimSpeedTimer unk2C /* s32: frames until sub_080ad788 re-picks the body's frame-delay level (every 120) */
#define heavyMolePatternIndex unk6C /* s16: candidate (0-3) of sub_080ada20's weighted pattern pick; the last one is skipped */

/* HeavyMoleArm - Heavy Mole's arms (task types #121 / #122,
   Task_HeavyMoleUpperArm / Task_HeavyMoleLowerArm; gHeavyMoleArmStates) */
#define heavyMoleArmSpinLevel unk28 /* s32: high half: claw spin speed level 0-3 (gUnk_0874B82A delays); low half: frames to its step */
#define heavyMoleArmPathStep unk2C /* s32: step counted down through the state's path tables (state 4: 24 poses, state 5: 11 moves) */
#define heavyMoleArmStateTimer unk2C /* s32: frames until state 0 goes to state 1, and state 1 to a random attack (gUnk_0874B831) */
#define heavyMoleArmSwingStep unk30 /* s32: the swing's angle step in states 2/3: +-1 (state 2) or +-2 (state 3), 0 = not swinging */
#define heavyMoleArmTimeLimit unk30 /* s32: frames state 4 may run (gUnk_0874B8C4 by phase) before it is cut back to state 0 */
#define heavyMoleArmAngleIndex unk34 /* s32: the arm's angle (0-4) in a swing; frame block unk34 * 6 of the arm frames */
#define heavyMoleArmBaseFrame unk34 /* s32: state 4's pose: the base arm frame from gUnk_0874B8C8 / gUnk_0874B8F8, plus the spin frame */

/* HeavyMoleEye - Heavy Mole's eye (task type #191, Task_HeavyMoleEye) */
#define heavyMoleEyeBlinkTimer unk28 /* s32: frames the eye stays hidden (96) before its three-frame 11-12-11 flash */

/* HeavyMoleRedMissile - Heavy Mole's red missile (task type #124,
   Task_HeavyMoleRedMissile) */
#define heavyMoleRedMissileLoopCount unk6C /* s16: the flight loop's counter: 4-frame cycles flown before the turn (up to unk6E, 1-32) */

/* HeavyMoleSmoke - Heavy Mole's smoke (task type #192, Task_HeavyMoleSmoke)
   */
#define heavyMoleSmokeRow unk28 /* s32: which of the two puff heights (0/1): Y offset gUnk_0874ACFA, column of the puff tables */
#define heavyMoleSmokePuffCount unk2C /* s32: puffs left in this burst (20); its bit 0 alternates the two heights */
#define heavyMoleSmokeStep unk30 /* s32: step of one puff (2 down to 0): index of its frame, velocity and sleep tables */

/* HeavyMoleYellowMissile - Heavy Mole's yellow missile (task type #123,
   Task_HeavyMoleYellowMissile) */
#define heavyMoleYellowMissileLoopCount unk6C /* s16: the flight loop's counter: 4-frame cycles flown before the turn (up to unk6E, 1-32) */

/* HotHead - Hot Head (task type #32, Task_HotHead; gHotHeadVariants,
   gHotHeadWalkStates, gHotHeadStandStates, gHotHeadIdleStates) */
#define hotHeadShotTimer unk28 /* s32: frames until it may shoot (90; 60 again after a check that did not shoot) */
#define hotHeadOddsIndex unk2C /* s32: index of the shot odds gUnk_087414B0 (2, 3): 0 at the first check, 1 after a miss */
#define hotHeadLoopFrame unk30 /* s32: frame of the body loop (4-8 and back, sub_08080b2c); set again when a shot stops */
#define hotHeadFrameStep unk34 /* s32: step of the body loop's frame (-1 or +1), reversed at frames 4 and 8 */
#define hotHeadFireSlot unk46 /* s16: slot of the last Task_HotHeadFire it spawned (task type #108) */
#define hotHeadLoopCount unk6C /* s16: the running state's loop counter (Shoot: 6 rounds of 5 flames, or 6 frame steps) */
#define hotHeadFlameIndex unk6E /* s16: flame of the current round of the fan (0-4); each Task_HotHeadFire reads it */

/* HotHeadFire - Hot Head's fire (task type #108, Task_HotHeadFire;
   gHotHeadFireVariants: rows 0, 1) */
#define hotHeadFireAngle unk28 /* s32: row 0: the flame's heading (0 or 256 by facing, plus its fan offset gUnk_08741E70) */
#define hotHeadFireAimAngle unk2C /* s32: row 1: the heading toward the nearest player, snapped to one of four diagonal sectors */
#define hotHeadFireFanIndex unk2C /* s32: row 0: which flame of the fan (0-4): the parent Hot Head's (s16)unk6E */

/* Kabu - Kabu (task type #21, Task_Kabu; gKabuVariants and its four rows'
   state tables) */
#define kabuBlinkTimer unk28 /* s32: frames left of the teleport's blink out or in (48); indexes the blink masks */
#define kabuFrameTimer unk28 /* s32: frames until the next spin frame while it falls (reloaded from the frame delay) */
#define kabuJumpIndex unk28 /* s32: last jump picked of the five in gUnk_08741328/3C/50 (-1: none yet) */
#define kabuPlayerSlot unk28 /* s32: slot of the nearest player the slide follows (TaskFindNearestPlayer) */
#define kabuTeleportTimer unk28 /* s32: frames until the teleport (gUnk_08741355[spawn arg]; 0 when a player is within 63 px) */
#define kabuBlinkFrame unk2C /* s32: spin frame (4-11) the blink shows, advanced every 5 frames */
#define kabuFrameDelay unk2C /* s32: frames each spin frame is shown */
#define kabuSlideSide unk2C /* s32: side of the player the slide heads for (0: +64 px, 1: -64 px), flipped each pass */
#define kabuBlinkRow unk30 /* s32: start of the blink masks in gUnk_08741357: 7 blinking out, 0 blinking in */
#define kabuPassCount unk30 /* s32: passes left before the burst (3) */
#define kabuSpinCount unk30 /* s32: spin cycles left before the jump (random 0-2) */
#define kabuTurnTimer unk34 /* s32: frames until the slide turns for the next pass (180 at the target, turns at 120) */
#define kabuLoopCount unk6C /* s16: the running state's loop counter (the spin's frame steps; KabuTeleport's spot tries) */

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

/* KingDededeStar - King Dedede's star (task type #112, Task_KingDededeStar)
   */
#define kingDededeStarReleased unk18 /* s32: nonzero once the star hit the room's side and let the held player go; the state then dies */
#define kingDededeStarFirstUpdate unk1C /* s32: 1 until the first update has set the held player's state 1 */

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

/* KrackoCloud - Kracko's cloud (task type #196, Task_KrackoCloud) */
#define krackoCloudSpeedIndex unk28 /* s32: Kracko's mode (gUnk_02007D00[0]) as last seen; row of the delay table gUnk_087491D4 */
#define krackoCloudFrameStep unk2C /* s32: step (0-3) of the cloud's four-frame cycle gUnk_087491BC */
#define krackoCloudFrameTimer unk30 /* s32: frames until the cloud's next frame (p[step] of its gUnk_087491D4 row) */
#define krackoCloudLoopCount unk6C /* s16: the opening animation's loop counter */

/* KrackoJrOrbs - Kracko Jr.'s orbs (task type #195, Task_KrackoJrOrbs) */
#define krackoJrOrbsSpeedIndex unk28 /* s32: Kracko's mode (gUnk_02007D00[0] & 15) as last seen; index of the delay table gUnk_087491A8 */
#define krackoJrOrbsFrameStep unk2C /* s32: step (0-3) of the orbs' four-frame cycle gUnk_087491A0 */
#define krackoJrOrbsFrameTimer unk30 /* s32: frames until the orbs' next frame (gUnk_087491A8[speed index]) */

/* KrackoLightningMiddle - Kracko's lightning, middle segment (task type #198,
   Task_KrackoLightningMiddle) */
#define krackoLightningMiddleBottomSlot unk46 /* s16: task slot of the bottom segment (#199) it makes below itself */

/* KrackoLightningTop - Kracko's lightning, top segment (task type #197,
   Task_KrackoLightningTop) */
#define krackoLightningTopMiddleSlot unk46 /* s16: task slot of the middle segment (#198) it makes below itself */

/* KrackoStarman - Kracko's Starman (task type #128, Task_KrackoStarman) */
#define krackoStarmanFrameTimer unk28 /* s32: frames until the next frame of the 11-13 fall cycle (every 6) */

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

/* MaceKnightMace - the Mace Knight's mace (task type #130,
   Task_MaceKnightMace; gMaceKnightMaceVariants; created by
   CreateMaceKnightMace) */
#define maceKnightMaceDismissed unk2C /* s32: 1 once its knight was hit (hitKind 1, 3 or 4); the mace then lets go and is destroyed */
#define maceKnightMaceLoopCount unk6C /* s16: the mace's loop counter (swing passes), counted from 0 */

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

/* MetaKnightCape - Meta Knight's cape (task type #185, Task_MetaKnightCape)
   */
#define metaKnightCapeLoopCount unk6E /* s16: the frame loop's counter (3 passes) */

/* MetaKnightMask - Meta Knight's mask (task type #186, Task_MetaKnightMask)
   */
#define metaKnightMaskHalfCount unk6C /* s16: halves made so far (0, 1); each half (#187) gets it as its side */

/* MetaKnightMaskHalf - Meta Knight's mask half (task type #187,
   Task_MetaKnightMaskHalf) */
#define metaKnightMaskHalfSide unk18 /* s32: which half (0/1): 0 flies backward with frames 31-34, 1 forward with 27-30 */
#define metaKnightMaskHalfLoopCount unk6C /* s16: the frame loop's counter (5 passes) */

/* MetaKnights - the Meta-Knights group (task type #57, Task_MetaKnights):
   spawns the four knights' queues */
#define metaKnightsQueue0Index unk18 /* s32: the next spawn record of queue 0 (the record list metaKnightsQueue0) */
#define metaKnightsQueue1Index unk1C /* s32: the next spawn record of queue 1 (the record list metaKnightsQueue1) */
#define metaKnightsQueue2Index unk20 /* s32: the next spawn record of queue 2 (the record list metaKnightsQueue2) */
#define metaKnightsQueue3Index unk24 /* s32: the next spawn record of queue 3 (the record list metaKnightsQueue3) */
#define metaKnightsQueue0 unk28 /* s32: queue 0: the 8-byte knight spawn records (kind, variant, x, y) for place 0 */
#define metaKnightsQueue1 unk2C /* s32: queue 1: the 8-byte knight spawn records (kind, variant, x, y) for place 1 */
#define metaKnightsQueue2 unk30 /* s32: queue 2: the 8-byte knight spawn records (kind, variant, x, y) for place 2 */
#define metaKnightsQueue3 unk34 /* s32: queue 3: the 8-byte knight spawn records (kind, variant, x, y) for place 3 */
#define metaKnightsNewKnightSlot unk46 /* s16: the knight (task type #58) sub_0809bc1c just created, while it stores the knight's place */
#define metaKnightsLoopCount unk6C /* s16: the intro's loop counter (frame loops), counted from 0 */
#define metaKnightsPaletteTimer unk6C /* s16: frame 0-80 of the group's palette pulse; the four knight palettes blend while above 63 */

/* MetaKnightsKnight - a knight of the Meta-Knights (task type #58,
   Task_MetaKnightsKnight; the kind in actorSpawnArg: 0 Axe, 1 Javelin, 2
   Mace, 3 Trident) */
#define metaKnightsKnightAxeCaught unk18 /* s32: Axe Knight: 1 once its thrown axe has flown back into it; the throw then catches it */
#define metaKnightsKnightFlashTimer unk24 /* s32: frames left of the damage flash (MetaKnightsKnightFlashPalette while > 0) */
#define metaKnightsKnightJavelinBouncesLeft unk28 /* s32: Javelin Knight row 1: wall bounces left (4) before its vertical direction flips */
#define metaKnightsKnightJavelinHopsLeft unk28 /* s32: Javelin Knight row 0: hops left in state 1 (3) before it chooses its next move */
#define metaKnightsKnightTridentSavedFacing unk28 /* s32: Trident Knight row 1: the facing at the start of its run-in, restored before it throws */
#define metaKnightsKnightTridentWalkBack unk28 /* s32: Trident Knight: 0 walks toward its facing, 1 backs away (TridentKnightWalk's two walks) */
#define metaKnightsKnightAxeWalkTimer unk2C /* s32: Axe Knight: frames left of the walk before it decides its next move (90) */
#define metaKnightsKnightJavelinFallDir unk2C /* s32: Javelin Knight row 1: 0 pushed upward, 1 pushed downward; flips every fourth wall bounce */
#define metaKnightsKnightJavelinJumpedLast unk2C /* s32: Javelin Knight row 0: 1 when the last choice was the high jump (state 4), so it alternates */
#define metaKnightsKnightTridentThrowIndex unk2C /* s32: Trident Knight row 1: the next of the 7 throws gUnk_08747C28 (the trident's variant) */
#define metaKnightsKnightTridentTimer unk2C /* s32: Trident Knight: frames the walk update counts down to its next attack (60, 120 after one) */
#define metaKnightsKnightAxeWaitTimer unk30 /* s32: Axe Knight: frames the walk waits before it may lunge or decide again (90 after each) */
#define metaKnightsKnightJavelinPushOn unk30 /* s32: Javelin Knight row 1: 1 while this frame's vertical push applies (a wall or end clears it) */
#define metaKnightsKnightWeaponSlot unk46 /* s16: its weapon actor: axe #129, mace #130, trident #131 or javelin #132 (-1: mace gone) */
#define metaKnightsKnightLoopCount unk6C /* s16: the running state's loop counter (frame loops), counted from 0 */
#define metaKnightsKnightQueue unk6E /* s16: the group's queue (place) 0-3 it was spawned from; indexes gUnk_02007D00[0..3] */

/* MetaKnightSword - Meta Knight's sword (task type #120,
   Task_MetaKnightSword; gUnk_08749B8C) */
#define metaKnightSwordHopFrameTimer unk28 /* s32: frames until the next frame of the hop's 16-23 spin (every 2) */
#define metaKnightSwordLanded unk28 /* s32: 1 once the thrown sword has landed; only then the update checks hits (the pick-up) */
#define metaKnightSwordFallFrameTimer unk2C /* s32: frames until the next frame of the fall's 4-11 spin (every 2) */
#define metaKnightSwordPickerIndex unk70 /* u16: player index the update searches for the one who touched the sword (gPlayerCount: none) */

/* MetaKnightSwordHitBox - Meta Knight's sword hit box (task type #184,
   Task_MetaKnightSwordHitBox) */
#define metaKnightSwordHitBoxFrameIndex unk28 /* s32: the parent's frame - 88 (his slash frames): index of the box and offset tables */

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

/* MrShineAndMrBrightAttack - Mr. Shine & Mr. Bright's attack objects (task
   type #127, Task_MrShineAndMrBrightAttack;
   gMrShineAndMrBrightAttackVariants) */
#define mrShineAndMrBrightAttackFrameTimer unk18 /* s32: frames until the beam's frame flips between 2 and 3 (every 2) */
#define mrShineAndMrBrightAttackSpinning unk18 /* s32: nonzero while the falling star spins (sub_08064bcc each frame) */
#define mrShineAndMrBrightAttackBeamFrame unk1C /* s32: 0/1: the beam shows frame 2 or 3; toggled every 2 frames */
#define mrShineAndMrBrightAttackStartVelY unk28 /* s32: the falling star's fall speed (3 or 4 px/frame by spawnArg bit 0), set on its start */
#define mrShineAndMrBrightAttackStartVelX unk2C /* s32: the falling star's X speed (gUnk_08748A40, toward the nearest player's side) */
#define mrShineAndMrBrightAttackLoopCount unk6C /* s16: the falling star's blink loop counter (2 passes) */

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

/* Needlous - Needlous (task type #44, Task_Needlous; gNeedlousVariants,
   gNeedlousStates, gNeedlousIdleStates) */
#define needlousCooldownTimer unk18 /* s32: frames after a dash during which the walk ignores the player (150) */
#define needlousLevelTimer unk1C /* s32: frames the player must stay within 9 px of its y before it hops */
#define needlousWallTimer unk1C /* s32: frames it stands at a wall during the dash before turning back (16) */
#define needlousMoveTimer unk20 /* s32: frames left of the walk (gUnk_08740A80) or the dash (60) before the next state */
#define needlousSetupState unk24 /* s32: state whose setup ran last (0 walk, 5 dash, -1 none): re-entry keeps its timers */

/* NightmarePowerOrb - Nightmare Power Orb (task type #66,
   Task_NightmarePowerOrb; a fixed attack script,
   gNightmarePowerOrbStateUpdates) */
#define nightmarePowerOrbScale unk18 /* s32: 16.16 index of the scale table gUnk_0873FF98 for the escape flight's affine draw; 0 plain */
#define nightmarePowerOrbMaxHealth unk24 /* s32: health at the start (ActorComputeHealth); under half of it the stars aim at the player */
#define nightmarePowerOrbScaleSpeed unk28 /* s32: 16.16 change of the escape flight's scale per frame (shrinks: -0xD00) */
#define nightmarePowerOrbAnimTimer unk2C /* s32: frames counted up on the gUnk_0874AD74 animation; at 40 gUnk_0874ADA8 starts */
#define nightmarePowerOrbStarSlot unk46 /* s16: task slot of the last star CreateNightmarePowerOrbStar made (#135) */
#define nightmarePowerOrbLoopCount unk6C /* s16: the attack script's loop counter (two rounds before the last steps) */

/* NightmarePowerOrbEscape - Nightmare Power Orb's escape (task type #98,
   Task_NightmarePowerOrbEscape) */
#define nightmarePowerOrbEscapeLoopCount unk6C /* s16: the running loop's counter (four 9-pass loops) */

/* NightmarePowerOrbEscapeStar - Nightmare Power Orb's escape star (task type
   #99, Task_NightmarePowerOrbEscapeStar) */
#define nightmarePowerOrbEscapeStarScale unk18 /* s32: 16.16 index of the scale table gUnk_0873FF98 for the affine draw (0x3F0000 full) */
#define nightmarePowerOrbEscapeStarAngle unk1C /* s32: rotation angle of the affine draw (0-511, wraps); -1 at start */
#define nightmarePowerOrbEscapeStarScaleSpeed unk28 /* s32: 16.16 change of the scale per frame (grow / shrink) */
#define nightmarePowerOrbEscapeStarSpinSpeed unk2C /* s32: change of the rotation angle per frame (+-16, +-8) */
#define nightmarePowerOrbEscapeStarLoopCount unk6C /* s16: the running loop's counter (the frame-step loops of each row) */

/* NightmarePowerOrbIntroScroll - Nightmare Power Orb's intro scroll (task
   type #80, Task_NightmarePowerOrbIntroScroll) */
#define nightmarePowerOrbIntroScrollSpeed unk28 /* s32: pixels per frame sub_080b08a0 adds to gCameraAnchorY (-4 easing to 0) */

/* NightmarePowerOrbStar - Nightmare Power Orb's star (task type #135,
   Task_NightmarePowerOrbStar; gNightmarePowerOrbStarVariants) */
#define nightmarePowerOrbStarTrailSlot unk46 /* s16: task slot of the last trail the star made (#210-#213) */

/* NightmarePowerOrbStarAfterimage - Nightmare Power Orb star's afterimage
   (task type #213, Task_NightmarePowerOrbStarAfterimage) */
#define nightmarePowerOrbStarAfterimageLoopCount unk6C /* s16: the frame loop's counter */

/* NightmarePowerOrbStarTrail - Nightmare Power Orb star's trail (task type
   #210, Task_NightmarePowerOrbStarTrail) */
#define nightmarePowerOrbStarTrailLoopCount unk6C /* s16: the sparkle loop's counter (frames 19-22 per pass) */

/* NightmarePowerOrbStarTrailDown - Nightmare Power Orb star's downward trail
   (task type #212, Task_NightmarePowerOrbStarTrailDown) */
#define nightmarePowerOrbStarTrailDownLoopCount unk6C /* s16: the sparkle loop's counter (frames 19-22 per pass) */

/* NightmarePowerOrbStarTrailUp - Nightmare Power Orb star's upward trail
   (task type #211, Task_NightmarePowerOrbStarTrailUp) */
#define nightmarePowerOrbStarTrailUpLoopCount unk6C /* s16: the sparkle loop's counter (frames 19-22 per pass) */

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

/* NightmareWizardCloakHands - Nightmare Wizard's cloak hands (task type #205,
   Task_NightmareWizardCloakHands) */
#define nightmareWizardCloakHandsBobY unk28 /* s32: Y offset (-3 to 0) the hands bob by; their update adds it to the parent's Y */
#define nightmareWizardCloakHandsLoopCount unk6C /* s16: the bob loop's counter (3 frames each way) */

/* NightmareWizardHitBox - Nightmare Wizard's hit box (task type #208,
   Task_NightmareWizardHitBox) */
#define nightmareWizardHitBoxIndex unk28 /* s32: row of the box tables gUnk_08749458 / gUnk_08749490 for the wizard's frame; -1 none */

/* NightmareWizardPalmTornado - Nightmare Wizard's palm tornado (task type
   #202, Task_NightmareWizardPalmTornado) */
#define nightmareWizardPalmTornadoFrameTimer unk28 /* s32: frames until the tornado's next frame (0-3 cycle, every 2) */

/* NightmareWizardPointTornado - Nightmare Wizard's point tornado (task type
   #204, Task_NightmareWizardPointTornado) */
#define nightmareWizardPointTornadoFrameTimer unk28 /* s32: frames until the tornado's next frame (0-3, then 4-7 cycles, every 2) */

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

/* PaintRoller - Paint Roller (task type #60, Task_PaintRoller;
   gPaintRollerStates) */
#define paintRollerEnteredState unk28 /* s32: the state PaintRollerEnterState last entered; the update re-enters on a change */
#define paintRollerStepsLeft unk30 /* s32: steps left in the running loop: move-script steps, or the pose repeats after a Summon */
#define paintRollerPaintingSlot unk46 /* s16: task slot of the painting CreatePaintRollerPainting made (#116) */

/* PaintRollerPainting - Paint Roller's painting (task type #116,
   Task_PaintRollerPainting; gPaintRollerPaintingStates) */
#define paintRollerPaintingParasolTimer unk2C /* s32: frames counted up from -32 by the parasol: hits once > 0, chases every 16, gone after 256 */
#define paintRollerPaintingLightningSlot unk46 /* s16: task slot of the last lightning the cloud row made (#117) */
#define paintRollerPaintingLoopCount unk6C /* s16: the running row's loop counter */

/* Parasol - Parasol (task type #46, Task_Parasol: the parasol a carrier
   drops; gParasolVariants) */
#define parasolHittable unk18 /* s32: 1 once it takes hits (the Rise and Chase updates call ActorCheckHits only then) */
#define parasolPlayerSlot unk1C /* s32: slot of the player that knocked it off (or the nearest); it drifts away from it */
#define parasolSteerTimer unk20 /* s32: frames until the next acceleration toward the player (15) */
#define parasolLifeTimer unk24 /* s32: frames left before the chasing parasol dies (255) */
#define parasolHittableAtOnce unk28 /* s32: 1: takes hits from the start (the drop's argument), else only after 20 frames */

/* Pengy - Pengy (task type #12, Task_Pengy; gPengyVariants, gPengyStates,
   gPengyIdleStates) */
#define pengyShotTimer unk28 /* s32: frames before it may shoot again (240 after a shot; 0 or 240 at the start) */
#define pengyNearTimer unk2C /* s32: frames a player has stayed within 63 px (at 33: shoot, or walk 1 in 3; then 3) */
#define pengyWaitTimer unk30 /* s32: frames waited since the last walk or shot (walks at 75/90/105 by chance, at 120) */
#define pengyIceBreathSlot unk46 /* s16: slot of the last Task_PengyIceBreath it breathed (task type #102) */
#define pengyLoopCount unk6C /* s16: the running state's loop counter (PengyShoot's ten breaths) */

/* PengyIceBreath - Pengy's ice breath (task type #102, Task_PengyIceBreath;
   gPengyIceBreathVariants, gPengyIceBreathStates) */
#define pengyIceBreathLoopCount unk6C /* s16: the running state's loop counter (the breath's five 2-frame cycles before ActorDestroy) */

/* PengyIceBreathSparkle - Pengy's ice-breath sparkle (task type #217,
   Task_PengyIceBreathSparkle) */
#define pengyIceBreathSparkleIndex unk74 /* u8: breath direction it follows (0-2, from PengyShoot): indexes gUnk_087410C0 / gUnk_087410D8 */

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

/* Player - the player (task type #5, Task_Player; its actions gPlayerActions
   / gPlayerActionHandlers, Meta Knightmare's gMetaKnightActions /
   gMetaKnightActionHandlers, and the Warp Star, cannon and goal-game rides)
   */
#define playerBallRollFrame unk46 /* s16: Ball: the roll frame index 0-15 into gUnk_0873DB0A (-1 while another animation plays) */
#define playerBaseFrame unk46 /* s16: the running action's base frame, from its per-ability frame table; frames step from it */
#define playerGoalGameMarkerSlot unk46 /* s16: the goal game's player marker child (task type #87, Task_GoalGamePlayerMarker) */
#define playerHiJumpFramePhase unk46 /* s16: Hi-Jump: step 0-3 of the four-frame cycle added to the update's base frame */
#define playerNeedleStep unk46 /* s16: Needle: step 0-10 of the spikes' sprout and retract; indexes gUnk_0873DADE */
#define playerStarRodFrameIndex unk46 /* s16: Star Rod flight: the index into the frame row the update steps through */
#define playerThrowHoldFramePhase unk46 /* s16: Throw (holding): step 0-3 of the held pose's cycle (0/1 one frame set, 2/3 the other) */
#define playerUfoFrameIndex unk46 /* s16: UFO: the frame index the update shows (gUnk_0873DB34, or + 0xF88 / 0xFA8), -1 none */
#define playerLoopCount unk6C /* s16: the running action's loop counter (frame runs, steps, shakes, players), counted from 0 */
#define playerBankBlendRatio unk6E /* s16: Spark: ratio 0-256 (+128 a step) blending the sprite's palette bank to its charged colours */
#define playerActionDone unk70 /* s16: Ball / Star Rod: 1 once the action's body has finished; its update then hands over */
#define playerNextBankBlendRatio unk70 /* s16: Spark: ratio 0-256 (+64 a step) blending the next palette bank to its charged colours */

/* PoppyBrosJr - Poppy Bros. Jr. (task type #38, Task_PoppyBrosJr;
   gPoppyBrosJrVariants, gPoppyBrosJrStates) and its riders (#39
   Task_PoppyBrosJrOnApple, #40 Task_PoppyBrosJrOnMaximTomato;
   gPoppyBrosJrRideVariants) */
#define poppyBrosJrRiderVelY unk18 /* s32: the rider sprite's hop velocity (16.16; -1: standing) */
#define poppyBrosJrDropFrameTimer unk1C /* s32: frames counted up to the dropped object's next frame (every 4th) */
#define poppyBrosJrRiderFrame unk1C /* s32: the rider sprite's frame (Actor.extraFrame): 7, then 6-5-4 and back in a hop */
#define poppyBrosJrRollTimer unk20 /* s32: frames until the ridden object's next rolling frame (9-16) */
#define poppyBrosJrBounceCount unk24 /* s32: bounces of the dropped object done (2: it lies still and takes hits) */
#define poppyBrosJrGravityTimer unk24 /* s32: frames of a hop before gravity starts (6) */
#define poppyBrosJrLandCount unk28 /* s32: landings counted by sub_08081e64 (every 2nd may turn it); 0 at a wall */
#define poppyBrosJrMountKind unk28 /* s32: what the rider rides: 0 an apple, 1 a Maxim Tomato */
#define poppyBrosJrSavedTileWord unk2C /* s32: the spawn tileWord, kept for the rider sprite while the object draws in palette 15 */
#define poppyBrosJrTurnBack unk2C /* s32: 1 after a random turn: the next landing turns it back */
#define poppyBrosJrNoHitTimer unk30 /* s32: frames without hit checks after the variant-1 jump (30), cleared on landing */
#define poppyBrosJrHopPhase unk34 /* s32: -1 while crouching before a hop (the update waits), 0 once it has left the ground */
#define poppyBrosJrRiderOffsetY unk34 /* s32: the rider sprite's hop offset (16.16, at most 0) above its seat */
#define poppyBrosJrSpawnSlot unk46 /* s16: slot of the actor spawned on a hit to take over its room object (TransferRoomObject) */

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

/* Rocky - Rocky (task type #9, Task_Rocky; gRockyVariants, gRockyWalkStates,
   gRockyStandStates, gRockyIdleStates) */
#define rockyLeapTimer unk28 /* s32: frames a player must stay near (49 x 15 px) before the next leap check (60) */
#define rockyPlayerSlot unk2C /* s32: slot of the nearest player it watches (TaskFindNearestPlayer) */
#define rockyLoopCount unk6C /* s16: the running state's loop counter (two step cycles of the walk / the stand) */

/* Scarfy - Scarfy (task type #29, Task_Scarfy; gScarfyVariants,
   gScarfyStates) */
#define scarfyChaseTimer unk28 /* s32: frames left of the chase (gUnk_0874094C[spawn arg]); at 0 it explodes */
#define scarfyShakeTimer unk28 /* s32: frames until the transform's shake flips its velX (2) */
#define scarfyWaveSign unk28 /* s32: sign of the hover's velY wave (+1 or -1), flipped after each half of the wave */
#define scarfyChaseAccel unk2C /* s32: acceleration step of the chase (TaskAccelerateTowardNearestPlayer's step) */
#define scarfyShakePhase unk2C /* s32: which of the two shake velocities gUnk_08740934 the transform applies (0/1) */
#define scarfyChaseSpeedLimit unk30 /* s32: speed limit of the chase (TaskAccelerateTowardNearestPlayer's limit) */
#define scarfyShakeDir unk30 /* s32: sign of the transform shake's velX (-facing) */
#define scarfyLoopCount unk6C /* s16: the running state's loop counter (the transform's two shakes; the explode's six) */

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

/* SirKibble - Sir Kibble (task type #15, Task_SirKibble; gSirKibbleVariants,
   gSirKibbleStandStates, gSirKibbleWalkStates, gSirKibbleIdleStates) */
#define sirKibbleStateTimer unk28 /* s32: frames left of the state: Wait/Walk gUnk_08741216[spawn arg]; Shoot 88 on the ground */
#define sirKibbleSavedFacing unk30 /* s32: facing toward the player; the jump throw faces away and restores it */
#define sirKibbleCutterSlot unk46 /* s16: slot of the last Task_SirKibbleCutter it threw (task type #105) */

/* Slippy - Slippy (task type #34, Task_Slippy; gSlippyVariants,
   gSlippyStates) */
#define slippyCollideTerrain unk28 /* s32: 1 when SlippyUpdate runs ActorCollideTerrain (0 while it starts in or leaves the water) */
#define slippySwimAngle unk2C /* s32: swim heading (512 per turn) the water states pass to AngleToVector */
#define slippyMovePickCount unk34 /* s32: calls of SlippyPickMove; its parity picks one of the two weight tables */
#define slippyLoopCount unk6C /* s16: SlippyState1's loop counter: frame cycles done (gUnk_08742820[spawn arg]: 3 or 2) */

/* Sparky - Sparky (task type #26, Task_Sparky; gSparkyVariants,
   gSparkyJumpStates, gSparkyStandStates, gSparkyIdleStates) */
#define sparkyJumpIndex unk28 /* s32: jump picked (0-2; 0 straight up) for SparkySetJumpMotion's speed tables */
#define sparkyPhase unk2C /* s32: progress of the state: 0 running, 2 discharging (sound ticks), 1 done (pick next) */
#define sparkySoundTimer unk30 /* s32: frames until the next discharge sound (sfx 191; every 3 frames) */
#define sparkyLoopCount unk6C /* s16: the running state's loop counter (the discharge's 8 widened-box cycles) */

/* Squishy - Squishy (task type #28, Task_Squishy; gSquishyVariants: rows
   Walk, 1, 2, Idle) */
#define squishyCollideTerrain unk28 /* s32: 1 when row 1's update runs ActorCollideTerrain (set at the end of its leap) */
#define squishyLanded unk28 /* s32: 1 once SquishyLand has run: the Walk row's jump and fall states wait for it */
#define squishyHopTimer unk34 /* s32: frames of row 2's hop at the player (37 above it, 43 below, 40 level); then the next */
#define squishyWalkTimer unk34 /* s32: frames SquishyWalk walks (100); at 80/60/40/20 a 1-in-4 jump, at 0 always */

/* Starman - Starman (task type #31, Task_Starman; gStarmanVariants: row 0
   gUnk_08741554, gStarmanJumpStates, gStarmanFlyStates, gStarmanIdleStates)
   */
#define starmanDropCount unk28 /* s32: drops of row 0's state 3 done: after the first it walks again, a later one flies off */
#define starmanWalkTimer unk28 /* s32: frames walked in the 217-frame cycle, counted up (216 at the start) */
#define starmanWaveTimer unk28 /* s32: frames until the next step of the flight's velY wave (6 or 8) */
#define starmanHideCount unk2C /* s32: times this leap hid above the view (0/1); the drop after one shakes the screen */
#define starmanJumpTime unk2C /* s32: frame of the walk cycle at which it jumps (gUnk_0874150C[random 16]) */
#define starmanWavePhase unk2C /* s32: step of the flight's velY wave (0-7: index into gUnk_08741524) */
#define starmanFallCount unk30 /* s32: falls StarmanStartFall started (variant 1); once non-zero, sub_08081884 turns it */
#define starmanFrameTimer unk30 /* s32: frames until the next frame of a fall loop (4) */
#define starmanHideTimer unk30 /* s32: frames left hidden above the view (it follows the player's x for the first ones) */
#define starmanLeapFrame unk30 /* s32: frame of the leap (16-18, the next every 6 frames), shown while it moves fast */
#define starmanWaveCount unk30 /* s32: waves left (6); at 0 it rises away (accelY -0x1900) */

/* SwordAndBladeKnight - Sword Knight and Blade Knight (task types #36 / #37,
   Task_SwordKnight / Task_BladeKnight; gSwordAndBladeKnightVariants and its
   rows' state tables) */
#define swordAndBladeKnightWaterChecked unk18 /* s32: 1 after state 0's one-time check for standing in water (ActorStartDrown) */
#define swordAndBladeKnightStopped unk1C /* s32: 1 once a wall or a ledge stopped it: the lunge's speed steps halt */
#define swordAndBladeKnightComboCount unk28 /* s32: extra swings chained after a swing (0, or 1 after a 1-in-4 second swing) */
#define swordAndBladeKnightLungeDir unk2C /* s32: sign of the swing's lunge (the facing when the swing began) */
#define swordAndBladeKnightBackOffTime unk30 /* s32: frames state 5 walks backward after the swings (90) */
#define swordAndBladeKnightLungeTimer unk30 /* s32: frames until the lunge's next speed step (3) */
#define swordAndBladeKnightLungeStep unk34 /* s32: step of the lunge's speed table gUnk_087409E4 (0-5) */
#define swordAndBladeKnightSlashSlot unk46 /* s16: slot of the last Task_SwordAndBladeKnightSlash it spawned (task type #173) */

/* SwordAndBladeKnightSlash - Sword and Blade Knight's slash (task type #173,
   Task_SwordAndBladeKnightSlash) */
#define swordAndBladeKnightSlashStruck unk18 /* s32: 1 after a hit of kind 6 struck it (sfx 243); later frames test the second box */

/* TridentKnightTrident - the Trident Knight's trident (task type #131,
   Task_TridentKnightTrident; gTridentKnightTridentVariants; created by
   CreateTridentKnightTrident) */
#define tridentKnightTridentFrameCount unk28 /* s32: frames since the throw; while its bit 1 is set the sprite alternates frame bit 0 */

/* Twister - Twister (task type #27, Task_Twister; gTwisterVariants,
   gTwisterStates, gTwisterIdleStates) */
#define twisterSoundTimer unk18 /* s32: countdown to the next spin sound (sfx 192), less the anim step each tick */
#define twisterPlayerSlot unk20 /* s32: slot of the nearest player state 1 steers toward (TaskFindNearestPlayer) */
#define twisterTargetSide unk24 /* s32: side of the player it steers to (0: +64 px, 1: -64 px), flipped when level */
#define twisterAscentCount unk28 /* s32: ascents of state 2 done: the first drops back to state 0, a later one keeps rising */
#define twisterPhaseTimer unk2C /* s32: frames left of the current phase (twisterPhase) */
#define twisterPhase unk30 /* s32: step of the current state: state 0 spin-up steps 0-4, state 1 -1 (rise) then 0-2 */
#define twisterLoopCount unk6C /* s16: the running state's loop counter (state 1's 40-frame rise) */

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

/* UFO - UFO (task type #45, Task_UFO; gUFOVariants, gUFOStates,
   gUFOIdleStates) */
#define ufoZigzagCount unk1C /* s32: zigzags state 0 flies before it picks a point (0-2: +1 per point, 0 after terrain) */
#define ufoPointCount unk20 /* s32: target points flown to since the last shot (the 4th is beside the player, then UFOShoot) */
#define ufoPointIndex unk24 /* s32: target point on the ring around the player (0-15; 16/17 beside it), gUnk_08740B3C/60 */
#define ufoTargetY unk28 /* s32: y of the target point (the nearest player's y + the ring offset) */
#define ufoTargetX unk2C /* s32: x of the target point (the nearest player's x + the ring offset) */
#define ufoFlightAngle unk34 /* s32: angle from it to the target point (GetPointAngle), the flight's AngleToVector angle */
#define ufoLaserSlot unk46 /* s16: slot of the last Task_UFOLaser it fired (task type #134) */
#define ufoLoopCount unk6C /* s16: the running state's loop counter (state 0's zigzags) */

/* WaddleDee - Waddle Dee (task type #8, Task_WaddleDee; gWaddleDeeVariants
   and its rows' state tables, the Parasol rows included) */
#define waddleDeeJumpTimer unk28 /* s32: frames until the next jump check (80 at first, then 30; 1 in 4 checks jumps) */
#define waddleDeeTurnTimer unk28 /* s32: frames until the pacing Waddle Dee turns around (gUnk_08740668[spawn arg]) */
#define waddleDeeJumpLaunched unk2C /* s32: 1 once the jump left its crouch; the update waits for the landing only then */
#define waddleDeeLoopCount unk6C /* s16: the running state's loop counter (the jump's two crouch cycles) */

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

/* Wheelie - Wheelie (task type #42, Task_Wheelie; gWheelieVariants,
   gWheelieStates, gWheelieIdleStates) */
#define wheelieSavedPosY unk24 /* s32: posY saved at a wall hit in the air; put back 2 frames later */
#define wheelieCheckTimer unk28 /* s32: frames until the next skid check (30) */
#define wheelieFrameScale unk2C /* s32: multiplier of the roll frames' delays in sub_08082cc4 (1, or 2 in the slow turn) */
#define wheelieFrameIndex unk30 /* s32: step of the wall bounce's frame cycle (0-3, index into gUnk_0874163B) */
#define wheelieRestoreTimer unk30 /* s32: frames until the position saved at a wall hit in the air is put back (2) */
#define wheelieStateTimer unk30 /* s32: frames left of the skid (42) or of the slow turn (gUnk_08741638: 5/35/65) */
#define wheelieFrameTimer unk34 /* s32: frames until the wall bounce's next frame (3) */
#define wheelieSavedPosX unk34 /* s32: posX saved at a wall hit in the air; put back 2 frames later */
#define wheelieSkidStopped unk34 /* s32: 1 once the skid stopped braking (left the ground or hit a wall) */
#define wheelieLoopCount unk6C /* s16: the running state's loop counter (state 0's four frame cycles) */

/* WhispyWoods - Whispy Woods (task type #64, Task_WhispyWoods;
   gWhispyWoodsStates) */
#define whispyWoodsWaitDone unk28 /* s32: nonzero once WhispyWoodsWait's blinks are over; the update then picks the attack */
#define whispyWoodsHurtTimer unk2C /* s32: frames of the hurt animation after a hit (32): frames 9, 7, 4 at 32, 20, 16 */
#define whispyWoodsAppleTimer unk30 /* s32: frames counted down in WhispyWoodsDropApples (150); an apple at 150, 100 and 50 */
#define whispyWoodsAttackPhase unk34 /* s32: step (0-4) of the attack sequence WhispyWoodsPickAttack walks; picks state 1, 2 or 3 */
#define whispyWoodsLoopCount unk6C /* s16: the running state's loop counter (states 1 and 2, WhispyWoodsDropApples) */

/* WhispyWoodsAirPuff - Whispy Woods' air puff (task type #126,
   Task_WhispyWoodsAirPuff; gWhispyWoodsAirPuffStates) */
#define whispyWoodsAirPuffEffectX unk30 /* s32: X of the extra sprite State0Update draws for 6 frames (the spawn X, drifting) */
#define whispyWoodsAirPuffEffectY unk34 /* s32: Y of that extra sprite (the spawn Y) */
#define whispyWoodsAirPuffEffectTimer unk6C /* u16: frames the extra sprite is still drawn (6); index of its frame and drift tables */

/* WhispyWoodsApple - Whispy Woods' apple (task type #125,
   Task_WhispyWoodsApple; gWhispyWoodsAppleStates) */
#define whispyWoodsAppleFirstFall unk28 /* s32: nonzero until the first landing; meanwhile the update checks terrain first */
#define whispyWoodsAppleFloorY unk2C /* s32: pixelY of the first landing; the bounce states check pixelY > unk2C */
#define whispyWoodsAppleRollFrame unk30 /* s32: step (0-7) of the 8-frame cycle gUnk_0874C24C that sub_080b3010 shows */
#define whispyWoodsAppleLoopCount unk6C /* s16: WhispyWoodsAppleFall's blink loop counter (6 passes) */

/* WhispyWoodsLeaves - Whispy Woods' leaves (task type #170,
   Task_WhispyWoodsLeaves) */
#define whispyWoodsLeavesStartX unk18 /* s32: pixelX at creation; the fourth leaf stays drawn there */
#define whispyWoodsLeavesStartY unk1C /* s32: pixelY at creation; the fourth leaf stays drawn there */
#define whispyWoodsLeavesLifeTimer unk6C /* u16: frames the leaves live (66); 65 - unk6C indexes the four leaves' frame tables */
#define whispyWoodsLeavesLoopCount unk6E /* s16: Task_WhispyWoodsLeaves' swaying loop counter (3 passes) */

#endif // GUARD_TASK_VARS_H
