#include "global.h"
#include "task.h"

/* The task-type table gTaskTypes[266] (0x0872FF30-0x0873077F, issue #36
 * phase 2).  TaskCreate (src/early_5654.c) indexes it by task type:
 * taskClass is the class list the new task joins (0-4) and entry the body
 * it copies into gTaskResumeAddrs[], which the ARM task switcher at
 * 0x08000234 enters with `bx` (docs/analysis/rom-map.md section 6), so every
 * entry is a Thumb function pointer.  The three pad bytes are zero in the
 * ROM.  Carved by tools/carve_data.py; the bodies are declared here with the
 * signature of their definition. */

void Task_BootLogo(void);
void Task_TitlePalette(void);
void Task_TitleSprites(void);
void Task_Room(void);
void Task_MapEvent(void);
void Task_Player(void);
void Task_PlayerObject(void);
void Task_PlayerEffect(void);
void Task_WaddleDee(void);
void Task_Rocky(void);
void Task_Noddy(void);
void Task_BroomHatter(void);
void Task_Pengy(void);
void Task_LaserBall(void);
void Task_Chilly(void);
void Task_SirKibble(void);
void Task_Cappy(void);
void Task_WaddleDoo(void);
void Task_Gordo(void);
void Task_CoolSpook(void);
void Task_BrontoBurt(void);
void Task_Kabu(void);
void Task_Bomber(void);
void Task_Coconut(void);
void Task_Twizzy(void);
void Task_Shotzo(void);
void Task_Sparky(void);
void Task_Twister(void);
void Task_Squishy(void);
void Task_Scarfy(void);
void Task_Bubbles(void);
void Task_Starman(void);
void Task_HotHead(void);
void Task_Glunk(void);
void Task_Slippy(void);
void Task_Blipper(void);
void Task_SwordKnight(void);
void Task_BladeKnight(void);
void Task_PoppyBrosJr(void);
void Task_PoppyBrosJrOnApple(void);
void Task_PoppyBrosJrOnMaximTomato(void);
void Task_Coner(void);
void Task_Wheelie(void);
void Task_Flamer(void);
void Task_Needlous(void);
void Task_UFO(void);
void Task_Parasol(void);
void Task_Gip(void);
void Task_BlockStar(void);
void Task_Bonkers(void);
void Task_PoppyBrosSr(void);
void Task_GrandWheelie(void);
void Task_MrFrosty(void);
void Task_MrTickTock(void);
void Task_Bugzzy(void);
void Task_FireLion(void);
void Task_PhanPhan(void);
void Task_MetaKnights(void);
void Task_MetaKnightsKnight(void);
void Task_KingDedede(void);
void Task_PaintRoller(void);
void Task_MetaKnight(void);
void Task_HeavyMole(void);
void Task_MrShineAndMrBright(void);
void Task_WhispyWoods(void);
void Task_Kracko(void);
void Task_NightmarePowerOrb(void);
void Task_NightmareWizard(void);
void Task_AbilityStar(void);
void Task_OneUp(void);
void Task_MaximTomato(void);
void Task_InvincibleCandy(void);
void Task_EnergyDrink(void);
void Task_StarRodPiece(void);
void Task_WarpStar(void);
void Task_Cannon(void);
void Task_CannonFuse(void);
void Task_BigSwitch(void);
void Task_Stake(void);
void sub_08078598(void);
void Task_NightmarePowerOrbIntroScroll(void);
void Task_GoalGameLaunchStars(void);
void Task_GoalGameBigTrailStar(void);
void Task_GoalGameSmallTrailStar(void);
void Task_GoalGameSpring(void);
void Task_GoalGameSign(void);
void Task_GoalGameHelperKirby(void);
void Task_GoalGamePlayerMarker(void);
void sub_0805d564(void);
void Task_GoalGameOneUp(void);
void Task_GoalGameCamera(void);
void Task_CutsceneDirector(void);
void Task_CutsceneActor(void);
void Task_SubGame(void);
void Task_QuickDrawObject(void);
void Task_BombRallyObject(void);
void Task_AirGrindObject(void);
void Task_WarpStarCamera(void);
void Task_NightmarePowerOrbEscape(void);
void sub_080752f4(void);
void Task_EndingEpilogue(void);
void Task_EndingStarRodReturn(void);
void Task_PengyIceBreath(void);
void Task_LaserBallLaser(void);
void Task_ChillyFreeze(void);
void Task_SirKibbleCutter(void);
void Task_WaddleDooBeam(void);
void Task_GlunkShot(void);
void Task_HotHeadFire(void);
void Task_ShotzoCannonball(void);
void Task_BonkersNut(void);
void Task_PoppyBrosSrBomb(void);
void Task_KingDededeStar(void);
void Task_KingDededeAirPuff(void);
void Task_GrandWheelieMiniWheelie(void);
void Task_MrFrostyIceCube(void);
void Task_PaintRollerPainting(void);
void Task_PaintRollerLightning(void);
void Task_MrTickTockRing(void);
void Task_MrTickTockNote(void);
void Task_MetaKnightSword(void);
void Task_HeavyMoleUpperArm(void);
void Task_HeavyMoleLowerArm(void);
void Task_HeavyMoleYellowMissile(void);
void Task_HeavyMoleRedMissile(void);
void Task_WhispyWoodsApple(void);
void Task_WhispyWoodsAirPuff(void);
void sub_080a4c84(void);
void Task_KrackoStarman(void);
void Task_AxeKnightAxe(void);
void Task_MaceKnightMace(void);
void Task_TridentKnightTrident(void);
void Task_JavelinKnightJavelin(void);
void Task_BugzzyLadybug(void);
void Task_UFOLaser(void);
void Task_NightmarePowerOrbStar(void);
void Task_NightmareWizardStar(void);
void Task_GipStar(void);
void Task_PhanPhanApple(void);
void Task_InhalableStar(void);
void Task_ActorSplash(void);
void Task_StarFlash(void);
void Task_StarFlashOnParent(void);
void Task_DustTrail(void);
void Task_DustPuff(void);
void sub_0806cf70(void);
void Task_LandingDust(void);
void Task_DustBurst(void);
void Task_StarScatter(void);
void Task_RayBurst(void);
void Task_SmallBlast(void);
void Task_StarScatterOnParent(void);
void Task_RayBurstOnParent(void);
void Task_SmallBlastOnParent(void);
void Task_ImpactStar(void);
void Task_RingStar(void);
void Task_CannonSmoke(void);
void Task_CannonFuseSpark(void);
void Task_TrailFlash(void);
void Task_HitFrost(void);
void Task_HitFlames(void);
void Task_HitSparks(void);
void Task_BossScreenFlash(void);
void Task_ExplosionScreenFlash(void);
void Task_WarpStarSparkle(void);
void Task_WarpStarTrailStar(void);
void Task_AbilityReleaseFlash(void);
void Task_DashFlame(void);
void Task_DashFireTrail(void);
void Task_LandingImpact(void);
void Task_WhispyWoodsLeaves(void);
void Task_IceBlock(void);
void Task_SmokeRing(void);
void Task_SwordAndBladeKnightSlash(void);
void Task_PaletteAnim(void);
void Task_HotHeadFlame(void);
void Task_FlamerFlame(void);
void Task_BonkersHammerHitBox(void);
void Task_KingDededeLandingStar(void);
void Task_PoppyBrosSrHead(void);
void Task_PoppyBrosSrHand(void);
void Task_PoppyBrosSrBombSpark(void);
void Task_KingDededeHammerHitBox(void);
void Task_KingDededeInhaleHitBox(void);
void Task_MetaKnightSwordHitBox(void);
void Task_MetaKnightCape(void);
void Task_MetaKnightMask(void);
void Task_MetaKnightMaskHalf(void);
void Task_MetaKnightSparkle(void);
void Task_HeavyMoleMissileHatch(void);
void Task_HeavyMoleTurbines(void);
void Task_HeavyMoleEye(void);
void Task_HeavyMoleSmoke(void);
void sub_080a54e4(void);
void Task_NoddyBubble(void);
void Task_KrackoJrOrbs(void);
void Task_KrackoCloud(void);
void Task_KrackoLightningTop(void);
void Task_KrackoLightningMiddle(void);
void Task_KrackoLightningBottom(void);
void Task_BugzzyAfterimage(void);
void Task_NightmareWizardPalm(void);
void sub_080abf10(void);
void Task_NightmareWizardPointingHand(void);
void sub_080ac124(void);
void Task_NightmareWizardCloakHands(void);
void sub_080ac410(void);
void Task_NightmareWizardPendant(void);
void sub_080ac510(void);
void sub_080abd04(void);
void sub_080afdf0(void);
void sub_080aff40(void);
void sub_080b0144(void);
void sub_080b0338(void);
void Task_FireLionFlame(void);
void Task_ChillyFreezeSparkle(void);
void Task_PengyIceBreathPuff(void);
void Task_PengyIceBreathSparkle(void);
void Task_BlipperDroplet(void);
void Task_GlunkShotSpray(void);
void sub_080b08e4(void);
void Task_ArenaDoorSign(void);
void Task_BossDoorSign(void);
void Task_DoorOpening(void);
void Task_StageClearFlag(void);
void Task_QuickDrawDoorSign(void);
void Task_BombRallyDoorSign(void);
void Task_AirGrindDoorSign(void);
void Task_MuseumDoorSign(void);
void Task_StageDoorSign(void);
void Task_WarpStarStationDoorSign(void);
void Task_WarpStarStationDoorSparkle(void);
void Task_LevelDoorSign(void);
void Task_WarpStarStationNumber(void);
void Task_WarpStarStationLevelSign(void);
void Task_MuseumAbilitySign(void);
void Task_StageEffect(void);
void Task_IntroStoryPicture(void);
void Task_FileSelectSlotLabel(void);
void Task_FileSelectSlot(void);
void Task_FileSelectCursor(void);
void Task_FileMenuSlot(void);
void Task_FileMenuHighlight(void);
void Task_EraseConfirmDialog(void);
void Task_EraseFileWipe(void);
void Task_NormalExtraPanel(void);
void Task_PlayerCountPanel(void);
void Task_ModeListCursor(void);
void Task_ModePlayerCountPanel(void);
void sub_0800ef30(void);
void sub_0800f084(void);
void Task_LinkPlayPlayerList(void);
void Task_LinkPlayConsole(void);
void Task_LinkPlayCable(void);
void Task_SoundTestCursors(void);
void Task_SoundTestPulse(void);
void Task_MenuScreenTitle(void);
void Task_BgScroll(void);
void Task_MenuBackground(void);
void Task_MenuBgPaletteCycle(void);
void Task_GameOverSprite(void);
void Task_GameOverCursor(void);
void Task_GameOverPalette(void);
void Task_HalveScore(void);
void Task_GameOverObject(void);
void Task_ExtraModeTitleSprite(void);

/* data-policy: functional - the task-type table: one {class, body} pair per
   task type, as TaskCreate reads it (266 entries x 4 numbers). */
const struct TaskType gTaskTypes[] = {
    /*   0 */ { 0, { 0, 0, 0 }, (u32)Task_BootLogo },
    /*   1 */ { 0, { 0, 0, 0 }, (u32)Task_TitlePalette },
    /*   2 */ { 0, { 0, 0, 0 }, (u32)Task_TitleSprites },
    /*   3 */ { 4, { 0, 0, 0 }, (u32)Task_Room },
    /*   4 */ { 4, { 0, 0, 0 }, (u32)Task_MapEvent },
    /*   5 */ { 1, { 0, 0, 0 }, (u32)Task_Player },
    /*   6 */ { 1, { 0, 0, 0 }, (u32)Task_PlayerObject },
    /*   7 */ { 1, { 0, 0, 0 }, (u32)Task_PlayerEffect },
    /*   8 */ { 3, { 0, 0, 0 }, (u32)Task_WaddleDee },
    /*   9 */ { 3, { 0, 0, 0 }, (u32)Task_Rocky },
    /*  10 */ { 3, { 0, 0, 0 }, (u32)Task_Noddy },
    /*  11 */ { 3, { 0, 0, 0 }, (u32)Task_BroomHatter },
    /*  12 */ { 3, { 0, 0, 0 }, (u32)Task_Pengy },
    /*  13 */ { 3, { 0, 0, 0 }, (u32)Task_LaserBall },
    /*  14 */ { 3, { 0, 0, 0 }, (u32)Task_Chilly },
    /*  15 */ { 3, { 0, 0, 0 }, (u32)Task_SirKibble },
    /*  16 */ { 3, { 0, 0, 0 }, (u32)Task_Cappy },
    /*  17 */ { 3, { 0, 0, 0 }, (u32)Task_WaddleDoo },
    /*  18 */ { 3, { 0, 0, 0 }, (u32)Task_Gordo },
    /*  19 */ { 3, { 0, 0, 0 }, (u32)Task_CoolSpook },
    /*  20 */ { 3, { 0, 0, 0 }, (u32)Task_BrontoBurt },
    /*  21 */ { 3, { 0, 0, 0 }, (u32)Task_Kabu },
    /*  22 */ { 3, { 0, 0, 0 }, (u32)Task_Bomber },
    /*  23 */ { 3, { 0, 0, 0 }, (u32)Task_Coconut },
    /*  24 */ { 3, { 0, 0, 0 }, (u32)Task_Twizzy },
    /*  25 */ { 3, { 0, 0, 0 }, (u32)Task_Shotzo },
    /*  26 */ { 3, { 0, 0, 0 }, (u32)Task_Sparky },
    /*  27 */ { 3, { 0, 0, 0 }, (u32)Task_Twister },
    /*  28 */ { 3, { 0, 0, 0 }, (u32)Task_Squishy },
    /*  29 */ { 3, { 0, 0, 0 }, (u32)Task_Scarfy },
    /*  30 */ { 3, { 0, 0, 0 }, (u32)Task_Bubbles },
    /*  31 */ { 3, { 0, 0, 0 }, (u32)Task_Starman },
    /*  32 */ { 3, { 0, 0, 0 }, (u32)Task_HotHead },
    /*  33 */ { 3, { 0, 0, 0 }, (u32)Task_Glunk },
    /*  34 */ { 3, { 0, 0, 0 }, (u32)Task_Slippy },
    /*  35 */ { 3, { 0, 0, 0 }, (u32)Task_Blipper },
    /*  36 */ { 3, { 0, 0, 0 }, (u32)Task_SwordKnight },
    /*  37 */ { 3, { 0, 0, 0 }, (u32)Task_BladeKnight },
    /*  38 */ { 3, { 0, 0, 0 }, (u32)Task_PoppyBrosJr },
    /*  39 */ { 3, { 0, 0, 0 }, (u32)Task_PoppyBrosJrOnApple },
    /*  40 */ { 3, { 0, 0, 0 }, (u32)Task_PoppyBrosJrOnMaximTomato },
    /*  41 */ { 3, { 0, 0, 0 }, (u32)Task_Coner },
    /*  42 */ { 3, { 0, 0, 0 }, (u32)Task_Wheelie },
    /*  43 */ { 3, { 0, 0, 0 }, (u32)Task_Flamer },
    /*  44 */ { 3, { 0, 0, 0 }, (u32)Task_Needlous },
    /*  45 */ { 3, { 0, 0, 0 }, (u32)Task_UFO },
    /*  46 */ { 3, { 0, 0, 0 }, (u32)Task_Parasol },
    /*  47 */ { 3, { 0, 0, 0 }, (u32)Task_Gip },
    /*  48 */ { 3, { 0, 0, 0 }, (u32)Task_BlockStar },
    /*  49 */ { 3, { 0, 0, 0 }, (u32)Task_Bonkers },
    /*  50 */ { 3, { 0, 0, 0 }, (u32)Task_PoppyBrosSr },
    /*  51 */ { 3, { 0, 0, 0 }, (u32)Task_GrandWheelie },
    /*  52 */ { 3, { 0, 0, 0 }, (u32)Task_MrFrosty },
    /*  53 */ { 3, { 0, 0, 0 }, (u32)Task_MrTickTock },
    /*  54 */ { 3, { 0, 0, 0 }, (u32)Task_Bugzzy },
    /*  55 */ { 3, { 0, 0, 0 }, (u32)Task_FireLion },
    /*  56 */ { 3, { 0, 0, 0 }, (u32)Task_PhanPhan },
    /*  57 */ { 3, { 0, 0, 0 }, (u32)Task_MetaKnights },
    /*  58 */ { 3, { 0, 0, 0 }, (u32)Task_MetaKnightsKnight },
    /*  59 */ { 3, { 0, 0, 0 }, (u32)Task_KingDedede },
    /*  60 */ { 3, { 0, 0, 0 }, (u32)Task_PaintRoller },
    /*  61 */ { 3, { 0, 0, 0 }, (u32)Task_MetaKnight },
    /*  62 */ { 3, { 0, 0, 0 }, (u32)Task_HeavyMole },
    /*  63 */ { 3, { 0, 0, 0 }, (u32)Task_MrShineAndMrBright },
    /*  64 */ { 3, { 0, 0, 0 }, (u32)Task_WhispyWoods },
    /*  65 */ { 3, { 0, 0, 0 }, (u32)Task_Kracko },
    /*  66 */ { 3, { 0, 0, 0 }, (u32)Task_NightmarePowerOrb },
    /*  67 */ { 3, { 0, 0, 0 }, (u32)Task_NightmareWizard },
    /*  68 */ { 4, { 0, 0, 0 }, (u32)Task_AbilityStar },
    /*  69 */ { 4, { 0, 0, 0 }, (u32)Task_OneUp },
    /*  70 */ { 4, { 0, 0, 0 }, (u32)Task_MaximTomato },
    /*  71 */ { 4, { 0, 0, 0 }, (u32)Task_InvincibleCandy },
    /*  72 */ { 4, { 0, 0, 0 }, (u32)Task_EnergyDrink },
    /*  73 */ { 3, { 0, 0, 0 }, (u32)Task_StarRodPiece },
    /*  74 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStar },
    /*  75 */ { 3, { 0, 0, 0 }, (u32)Task_Cannon },
    /*  76 */ { 3, { 0, 0, 0 }, (u32)Task_CannonFuse },
    /*  77 */ { 3, { 0, 0, 0 }, (u32)Task_BigSwitch },
    /*  78 */ { 3, { 0, 0, 0 }, (u32)Task_Stake },
    /*  79 */ { 3, { 0, 0, 0 }, (u32)sub_08078598 },
    /*  80 */ { 3, { 0, 0, 0 }, (u32)Task_NightmarePowerOrbIntroScroll },
    /*  81 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameLaunchStars },
    /*  82 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameBigTrailStar },
    /*  83 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameSmallTrailStar },
    /*  84 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameSpring },
    /*  85 */ { 4, { 0, 0, 0 }, (u32)Task_GoalGameSign },
    /*  86 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameHelperKirby },
    /*  87 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGamePlayerMarker },
    /*  88 */ { 3, { 0, 0, 0 }, (u32)sub_0805d564 },
    /*  89 */ { 3, { 0, 0, 0 }, (u32)Task_GoalGameOneUp },
    /*  90 */ { 4, { 0, 0, 0 }, (u32)Task_GoalGameCamera },
    /*  91 */ { 1, { 0, 0, 0 }, (u32)Task_CutsceneDirector },
    /*  92 */ { 2, { 0, 0, 0 }, (u32)Task_CutsceneActor },
    /*  93 */ { 3, { 0, 0, 0 }, (u32)Task_SubGame },
    /*  94 */ { 3, { 0, 0, 0 }, (u32)Task_QuickDrawObject },
    /*  95 */ { 4, { 0, 0, 0 }, (u32)Task_BombRallyObject },
    /*  96 */ { 3, { 0, 0, 0 }, (u32)Task_AirGrindObject },
    /*  97 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarCamera },
    /*  98 */ { 3, { 0, 0, 0 }, (u32)Task_NightmarePowerOrbEscape },
    /*  99 */ { 3, { 0, 0, 0 }, (u32)sub_080752f4 },
    /* 100 */ { 3, { 0, 0, 0 }, (u32)Task_EndingEpilogue },
    /* 101 */ { 3, { 0, 0, 0 }, (u32)Task_EndingStarRodReturn },
    /* 102 */ { 2, { 0, 0, 0 }, (u32)Task_PengyIceBreath },
    /* 103 */ { 2, { 0, 0, 0 }, (u32)Task_LaserBallLaser },
    /* 104 */ { 2, { 0, 0, 0 }, (u32)Task_ChillyFreeze },
    /* 105 */ { 2, { 0, 0, 0 }, (u32)Task_SirKibbleCutter },
    /* 106 */ { 2, { 0, 0, 0 }, (u32)Task_WaddleDooBeam },
    /* 107 */ { 2, { 0, 0, 0 }, (u32)Task_GlunkShot },
    /* 108 */ { 2, { 0, 0, 0 }, (u32)Task_HotHeadFire },
    /* 109 */ { 2, { 0, 0, 0 }, (u32)Task_ShotzoCannonball },
    /* 110 */ { 2, { 0, 0, 0 }, (u32)Task_BonkersNut },
    /* 111 */ { 2, { 0, 0, 0 }, (u32)Task_PoppyBrosSrBomb },
    /* 112 */ { 2, { 0, 0, 0 }, (u32)Task_KingDededeStar },
    /* 113 */ { 2, { 0, 0, 0 }, (u32)Task_KingDededeAirPuff },
    /* 114 */ { 2, { 0, 0, 0 }, (u32)Task_GrandWheelieMiniWheelie },
    /* 115 */ { 2, { 0, 0, 0 }, (u32)Task_MrFrostyIceCube },
    /* 116 */ { 2, { 0, 0, 0 }, (u32)Task_PaintRollerPainting },
    /* 117 */ { 2, { 0, 0, 0 }, (u32)Task_PaintRollerLightning },
    /* 118 */ { 2, { 0, 0, 0 }, (u32)Task_MrTickTockRing },
    /* 119 */ { 2, { 0, 0, 0 }, (u32)Task_MrTickTockNote },
    /* 120 */ { 2, { 0, 0, 0 }, (u32)Task_MetaKnightSword },
    /* 121 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleUpperArm },
    /* 122 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleLowerArm },
    /* 123 */ { 2, { 0, 0, 0 }, (u32)Task_HeavyMoleYellowMissile },
    /* 124 */ { 2, { 0, 0, 0 }, (u32)Task_HeavyMoleRedMissile },
    /* 125 */ { 2, { 0, 0, 0 }, (u32)Task_WhispyWoodsApple },
    /* 126 */ { 2, { 0, 0, 0 }, (u32)Task_WhispyWoodsAirPuff },
    /* 127 */ { 4, { 0, 0, 0 }, (u32)sub_080a4c84 },
    /* 128 */ { 2, { 0, 0, 0 }, (u32)Task_KrackoStarman },
    /* 129 */ { 2, { 0, 0, 0 }, (u32)Task_AxeKnightAxe },
    /* 130 */ { 4, { 0, 0, 0 }, (u32)Task_MaceKnightMace },
    /* 131 */ { 2, { 0, 0, 0 }, (u32)Task_TridentKnightTrident },
    /* 132 */ { 2, { 0, 0, 0 }, (u32)Task_JavelinKnightJavelin },
    /* 133 */ { 2, { 0, 0, 0 }, (u32)Task_BugzzyLadybug },
    /* 134 */ { 2, { 0, 0, 0 }, (u32)Task_UFOLaser },
    /* 135 */ { 2, { 0, 0, 0 }, (u32)Task_NightmarePowerOrbStar },
    /* 136 */ { 2, { 0, 0, 0 }, (u32)Task_NightmareWizardStar },
    /* 137 */ { 2, { 0, 0, 0 }, (u32)Task_GipStar },
    /* 138 */ { 2, { 0, 0, 0 }, (u32)Task_PhanPhanApple },
    /* 139 */ { 2, { 0, 0, 0 }, (u32)Task_InhalableStar },
    /* 140 */ { 1, { 0, 0, 0 }, (u32)Task_ActorSplash },
    /* 141 */ { 1, { 0, 0, 0 }, (u32)Task_StarFlash },
    /* 142 */ { 1, { 0, 0, 0 }, (u32)Task_StarFlashOnParent },
    /* 143 */ { 1, { 0, 0, 0 }, (u32)Task_DustTrail },
    /* 144 */ { 1, { 0, 0, 0 }, (u32)Task_DustPuff },
    /* 145 */ { 1, { 0, 0, 0 }, (u32)sub_0806cf70 },
    /* 146 */ { 1, { 0, 0, 0 }, (u32)Task_LandingDust },
    /* 147 */ { 3, { 0, 0, 0 }, (u32)Task_DustBurst },
    /* 148 */ { 1, { 0, 0, 0 }, (u32)Task_StarScatter },
    /* 149 */ { 1, { 0, 0, 0 }, (u32)Task_RayBurst },
    /* 150 */ { 1, { 0, 0, 0 }, (u32)Task_SmallBlast },
    /* 151 */ { 1, { 0, 0, 0 }, (u32)Task_StarScatterOnParent },
    /* 152 */ { 1, { 0, 0, 0 }, (u32)Task_RayBurstOnParent },
    /* 153 */ { 1, { 0, 0, 0 }, (u32)Task_SmallBlastOnParent },
    /* 154 */ { 1, { 0, 0, 0 }, (u32)Task_ImpactStar },
    /* 155 */ { 1, { 0, 0, 0 }, (u32)Task_RingStar },
    /* 156 */ { 1, { 0, 0, 0 }, (u32)Task_CannonSmoke },
    /* 157 */ { 1, { 0, 0, 0 }, (u32)Task_CannonFuseSpark },
    /* 158 */ { 4, { 0, 0, 0 }, (u32)Task_TrailFlash },
    /* 159 */ { 4, { 0, 0, 0 }, (u32)Task_HitFrost },
    /* 160 */ { 4, { 0, 0, 0 }, (u32)Task_HitFlames },
    /* 161 */ { 4, { 0, 0, 0 }, (u32)Task_HitSparks },
    /* 162 */ { 1, { 0, 0, 0 }, (u32)Task_BossScreenFlash },
    /* 163 */ { 1, { 0, 0, 0 }, (u32)Task_ExplosionScreenFlash },
    /* 164 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarSparkle },
    /* 165 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarTrailStar },
    /* 166 */ { 3, { 0, 0, 0 }, (u32)Task_AbilityReleaseFlash },
    /* 167 */ { 3, { 0, 0, 0 }, (u32)Task_DashFlame },
    /* 168 */ { 3, { 0, 0, 0 }, (u32)Task_DashFireTrail },
    /* 169 */ { 3, { 0, 0, 0 }, (u32)Task_LandingImpact },
    /* 170 */ { 3, { 0, 0, 0 }, (u32)Task_WhispyWoodsLeaves },
    /* 171 */ { 4, { 0, 0, 0 }, (u32)Task_IceBlock },
    /* 172 */ { 3, { 0, 0, 0 }, (u32)Task_SmokeRing },
    /* 173 */ { 4, { 0, 0, 0 }, (u32)Task_SwordAndBladeKnightSlash },
    /* 174 */ { 4, { 0, 0, 0 }, (u32)Task_PaletteAnim },
    /* 175 */ { 4, { 0, 0, 0 }, (u32)Task_HotHeadFlame },
    /* 176 */ { 2, { 0, 0, 0 }, (u32)Task_FlamerFlame },
    /* 177 */ { 3, { 0, 0, 0 }, (u32)Task_BonkersHammerHitBox },
    /* 178 */ { 1, { 0, 0, 0 }, (u32)Task_KingDededeLandingStar },
    /* 179 */ { 4, { 0, 0, 0 }, (u32)Task_PoppyBrosSrHead },
    /* 180 */ { 4, { 0, 0, 0 }, (u32)Task_PoppyBrosSrHand },
    /* 181 */ { 2, { 0, 0, 0 }, (u32)Task_PoppyBrosSrBombSpark },
    /* 182 */ { 3, { 0, 0, 0 }, (u32)Task_KingDededeHammerHitBox },
    /* 183 */ { 3, { 0, 0, 0 }, (u32)Task_KingDededeInhaleHitBox },
    /* 184 */ { 3, { 0, 0, 0 }, (u32)Task_MetaKnightSwordHitBox },
    /* 185 */ { 4, { 0, 0, 0 }, (u32)Task_MetaKnightCape },
    /* 186 */ { 4, { 0, 0, 0 }, (u32)Task_MetaKnightMask },
    /* 187 */ { 4, { 0, 0, 0 }, (u32)Task_MetaKnightMaskHalf },
    /* 188 */ { 2, { 0, 0, 0 }, (u32)Task_MetaKnightSparkle },
    /* 189 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleMissileHatch },
    /* 190 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleTurbines },
    /* 191 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleEye },
    /* 192 */ { 4, { 0, 0, 0 }, (u32)Task_HeavyMoleSmoke },
    /* 193 */ { 4, { 0, 0, 0 }, (u32)sub_080a54e4 },
    /* 194 */ { 4, { 0, 0, 0 }, (u32)Task_NoddyBubble },
    /* 195 */ { 4, { 0, 0, 0 }, (u32)Task_KrackoJrOrbs },
    /* 196 */ { 4, { 0, 0, 0 }, (u32)Task_KrackoCloud },
    /* 197 */ { 4, { 0, 0, 0 }, (u32)Task_KrackoLightningTop },
    /* 198 */ { 4, { 0, 0, 0 }, (u32)Task_KrackoLightningMiddle },
    /* 199 */ { 4, { 0, 0, 0 }, (u32)Task_KrackoLightningBottom },
    /* 200 */ { 4, { 0, 0, 0 }, (u32)Task_BugzzyAfterimage },
    /* 201 */ { 3, { 0, 0, 0 }, (u32)Task_NightmareWizardPalm },
    /* 202 */ { 3, { 0, 0, 0 }, (u32)sub_080abf10 },
    /* 203 */ { 3, { 0, 0, 0 }, (u32)Task_NightmareWizardPointingHand },
    /* 204 */ { 3, { 0, 0, 0 }, (u32)sub_080ac124 },
    /* 205 */ { 3, { 0, 0, 0 }, (u32)Task_NightmareWizardCloakHands },
    /* 206 */ { 3, { 0, 0, 0 }, (u32)sub_080ac410 },
    /* 207 */ { 3, { 0, 0, 0 }, (u32)Task_NightmareWizardPendant },
    /* 208 */ { 3, { 0, 0, 0 }, (u32)sub_080ac510 },
    /* 209 */ { 3, { 0, 0, 0 }, (u32)sub_080abd04 },
    /* 210 */ { 3, { 0, 0, 0 }, (u32)sub_080afdf0 },
    /* 211 */ { 3, { 0, 0, 0 }, (u32)sub_080aff40 },
    /* 212 */ { 3, { 0, 0, 0 }, (u32)sub_080b0144 },
    /* 213 */ { 3, { 0, 0, 0 }, (u32)sub_080b0338 },
    /* 214 */ { 3, { 0, 0, 0 }, (u32)Task_FireLionFlame },
    /* 215 */ { 3, { 0, 0, 0 }, (u32)Task_ChillyFreezeSparkle },
    /* 216 */ { 3, { 0, 0, 0 }, (u32)Task_PengyIceBreathPuff },
    /* 217 */ { 3, { 0, 0, 0 }, (u32)Task_PengyIceBreathSparkle },
    /* 218 */ { 3, { 0, 0, 0 }, (u32)Task_BlipperDroplet },
    /* 219 */ { 3, { 0, 0, 0 }, (u32)Task_GlunkShotSpray },
    /* 220 */ { 3, { 0, 0, 0 }, (u32)sub_080b08e4 },
    /* 221 */ { 3, { 0, 0, 0 }, (u32)Task_ArenaDoorSign },
    /* 222 */ { 3, { 0, 0, 0 }, (u32)Task_BossDoorSign },
    /* 223 */ { 3, { 0, 0, 0 }, (u32)Task_DoorOpening },
    /* 224 */ { 3, { 0, 0, 0 }, (u32)Task_StageClearFlag },
    /* 225 */ { 3, { 0, 0, 0 }, (u32)Task_QuickDrawDoorSign },
    /* 226 */ { 3, { 0, 0, 0 }, (u32)Task_BombRallyDoorSign },
    /* 227 */ { 3, { 0, 0, 0 }, (u32)Task_AirGrindDoorSign },
    /* 228 */ { 3, { 0, 0, 0 }, (u32)Task_MuseumDoorSign },
    /* 229 */ { 3, { 0, 0, 0 }, (u32)Task_StageDoorSign },
    /* 230 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarStationDoorSign },
    /* 231 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarStationDoorSparkle },
    /* 232 */ { 3, { 0, 0, 0 }, (u32)Task_LevelDoorSign },
    /* 233 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarStationNumber },
    /* 234 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarStationLevelSign },
    /* 235 */ { 3, { 0, 0, 0 }, (u32)Task_MuseumAbilitySign },
    /* 236 */ { 4, { 0, 0, 0 }, (u32)Task_StageEffect },
    /* 237 */ { 4, { 0, 0, 0 }, (u32)Task_IntroStoryPicture },
    /* 238 */ { 4, { 0, 0, 0 }, (u32)Task_FileSelectSlotLabel },
    /* 239 */ { 4, { 0, 0, 0 }, (u32)Task_FileSelectSlot },
    /* 240 */ { 4, { 0, 0, 0 }, (u32)Task_FileSelectCursor },
    /* 241 */ { 4, { 0, 0, 0 }, (u32)Task_FileMenuSlot },
    /* 242 */ { 4, { 0, 0, 0 }, (u32)Task_FileMenuHighlight },
    /* 243 */ { 4, { 0, 0, 0 }, (u32)Task_EraseConfirmDialog },
    /* 244 */ { 4, { 0, 0, 0 }, (u32)Task_EraseFileWipe },
    /* 245 */ { 4, { 0, 0, 0 }, (u32)Task_NormalExtraPanel },
    /* 246 */ { 4, { 0, 0, 0 }, (u32)Task_PlayerCountPanel },
    /* 247 */ { 4, { 0, 0, 0 }, (u32)Task_ModeListCursor },
    /* 248 */ { 4, { 0, 0, 0 }, (u32)Task_ModePlayerCountPanel },
    /* 249 */ { 4, { 0, 0, 0 }, (u32)sub_0800ef30 },
    /* 250 */ { 4, { 0, 0, 0 }, (u32)sub_0800f084 },
    /* 251 */ { 4, { 0, 0, 0 }, (u32)Task_LinkPlayPlayerList },
    /* 252 */ { 4, { 0, 0, 0 }, (u32)Task_LinkPlayConsole },
    /* 253 */ { 4, { 0, 0, 0 }, (u32)Task_LinkPlayCable },
    /* 254 */ { 4, { 0, 0, 0 }, (u32)Task_SoundTestCursors },
    /* 255 */ { 4, { 0, 0, 0 }, (u32)Task_SoundTestPulse },
    /* 256 */ { 4, { 0, 0, 0 }, (u32)Task_MenuScreenTitle },
    /* 257 */ { 4, { 0, 0, 0 }, (u32)Task_BgScroll },
    /* 258 */ { 4, { 0, 0, 0 }, (u32)Task_MenuBackground },
    /* 259 */ { 4, { 0, 0, 0 }, (u32)Task_MenuBgPaletteCycle },
    /* 260 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverSprite },
    /* 261 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverCursor },
    /* 262 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverPalette },
    /* 263 */ { 4, { 0, 0, 0 }, (u32)Task_HalveScore },
    /* 264 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverObject },
    /* 265 */ { 4, { 0, 0, 0 }, (u32)Task_ExtraModeTitleSprite },
};
