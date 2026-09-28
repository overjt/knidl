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
void sub_080b47cc(void);
void Task_WarpStar(void);
void Task_Cannon(void);
void Task_CannonFuse(void);
void Task_BigSwitch(void);
void Task_Stake(void);
void sub_08078598(void);
void Task_NightmarePowerOrbIntroScroll(void);
void sub_0805beb0(void);
void sub_0805c204(void);
void sub_0805c410(void);
void sub_0805cb30(void);
void sub_0805cca0(void);
void sub_0805cf3c(void);
void sub_0805cbec(void);
void sub_0805d564(void);
void sub_0805d668(void);
void sub_0805c5fc(void);
void Task_CutsceneDirector(void);
void Task_CutsceneActor(void);
void Task_SubGame(void);
void Task_QuickDrawObject(void);
void Task_BombRallyObject(void);
void Task_AirGrindObject(void);
void sub_0807450c(void);
void sub_08075000(void);
void sub_080752f4(void);
void sub_080c6c64(void);
void sub_080c9004(void);
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
void sub_080b0cc8(void);
void sub_080b13bc(void);
void Task_MrTickTockRing(void);
void Task_MrTickTockNote(void);
void Task_MetaKnightSword(void);
void sub_080b1710(void);
void sub_080b1770(void);
void sub_080b22f8(void);
void sub_080b2418(void);
void Task_WhispyWoodsApple(void);
void Task_WhispyWoodsAirPuff(void);
void sub_080a4c84(void);
void sub_080aca90(void);
void Task_AxeKnightAxe(void);
void Task_MaceKnightMace(void);
void Task_TridentKnightTrident(void);
void Task_JavelinKnightJavelin(void);
void Task_BugzzyLadybug(void);
void Task_UFOLaser(void);
void sub_080af30c(void);
void sub_080acc9c(void);
void Task_GipStar(void);
void Task_PhanPhanApple(void);
void sub_080671c0(void);
void Task_ActorSplash(void);
void Task_StarFlash(void);
void Task_StarFlashOnParent(void);
void Task_DustTrail(void);
void Task_DustPuff(void);
void sub_0806cf70(void);
void sub_0806d148(void);
void Task_DustBurst(void);
void Task_StarScatter(void);
void Task_RayBurst(void);
void Task_SmallBlast(void);
void Task_StarScatterOnParent(void);
void Task_RayBurstOnParent(void);
void Task_SmallBlastOnParent(void);
void sub_0806d7ec(void);
void Task_RingStar(void);
void sub_0806daec(void);
void sub_0806dd90(void);
void Task_TrailFlash(void);
void Task_HitFrost(void);
void Task_HitFlames(void);
void Task_HitSparks(void);
void sub_0806da04(void);
void sub_0806da20(void);
void Task_WarpStarSparkle(void);
void sub_08074c0c(void);
void Task_AbilityReleaseFlash(void);
void sub_0806e73c(void);
void sub_0806e84c(void);
void sub_0806e9fc(void);
void sub_080b3c68(void);
void Task_IceBlock(void);
void sub_0806ee1c(void);
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
void sub_080adca4(void);
void sub_080adefc(void);
void sub_080ae038(void);
void sub_080ae1f0(void);
void sub_080a54e4(void);
void Task_NoddyBubble(void);
void sub_080a983c(void);
void sub_080a99a0(void);
void sub_080a9c28(void);
void sub_080a9cf0(void);
void sub_080a9da4(void);
void Task_BugzzyAfterimage(void);
void sub_080abe38(void);
void sub_080abf10(void);
void sub_080ac020(void);
void sub_080ac124(void);
void sub_080ac27c(void);
void sub_080ac410(void);
void sub_080ac3a4(void);
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
void sub_0802eb28(void);
void sub_0802ec7c(void);
void sub_0802ede4(void);
void sub_0802eff8(void);
void sub_0802f0cc(void);
void sub_0802f26c(void);
void sub_0802f38c(void);
void sub_0802f480(void);
void sub_0802f62c(void);
void sub_0802f84c(void);
void sub_0802faa8(void);
void sub_0802fe64(void);
void sub_08030034(void);
void sub_080300c0(void);
void sub_080301a4(void);
void Task_StageEffect(void);
void Task_IntroStoryPicture(void);
void sub_0800daf8(void);
void sub_0800db64(void);
void Task_FileSelectCursor(void);
void sub_0800de6c(void);
void Task_FileMenuHighlight(void);
void sub_0800e9a4(void);
void sub_0800eae4(void);
void sub_0800e314(void);
void sub_0800e46c(void);
void Task_ModeListCursor(void);
void sub_0800e81c(void);
void sub_0800ef30(void);
void sub_0800f084(void);
void Task_LinkPlayPlayerList(void);
void sub_0800f390(void);
void sub_0800f5ec(void);
void Task_SoundTestCursors(void);
void Task_SoundTestPulse(void);
void Task_MenuScreenTitle(void);
void Task_BgScroll(void);
void sub_0800fb94(void);
void sub_0800fa30(void);
void Task_GameOverSprite(void);
void Task_GameOverCursor(void);
void Task_GameOverPalette(void);
void Task_HalveScore(void);
void Task_GameOverObject(void);
void sub_08008348(void);

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
    /*  73 */ { 3, { 0, 0, 0 }, (u32)sub_080b47cc },
    /*  74 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStar },
    /*  75 */ { 3, { 0, 0, 0 }, (u32)Task_Cannon },
    /*  76 */ { 3, { 0, 0, 0 }, (u32)Task_CannonFuse },
    /*  77 */ { 3, { 0, 0, 0 }, (u32)Task_BigSwitch },
    /*  78 */ { 3, { 0, 0, 0 }, (u32)Task_Stake },
    /*  79 */ { 3, { 0, 0, 0 }, (u32)sub_08078598 },
    /*  80 */ { 3, { 0, 0, 0 }, (u32)Task_NightmarePowerOrbIntroScroll },
    /*  81 */ { 3, { 0, 0, 0 }, (u32)sub_0805beb0 },
    /*  82 */ { 3, { 0, 0, 0 }, (u32)sub_0805c204 },
    /*  83 */ { 3, { 0, 0, 0 }, (u32)sub_0805c410 },
    /*  84 */ { 3, { 0, 0, 0 }, (u32)sub_0805cb30 },
    /*  85 */ { 4, { 0, 0, 0 }, (u32)sub_0805cca0 },
    /*  86 */ { 3, { 0, 0, 0 }, (u32)sub_0805cf3c },
    /*  87 */ { 3, { 0, 0, 0 }, (u32)sub_0805cbec },
    /*  88 */ { 3, { 0, 0, 0 }, (u32)sub_0805d564 },
    /*  89 */ { 3, { 0, 0, 0 }, (u32)sub_0805d668 },
    /*  90 */ { 4, { 0, 0, 0 }, (u32)sub_0805c5fc },
    /*  91 */ { 1, { 0, 0, 0 }, (u32)Task_CutsceneDirector },
    /*  92 */ { 2, { 0, 0, 0 }, (u32)Task_CutsceneActor },
    /*  93 */ { 3, { 0, 0, 0 }, (u32)Task_SubGame },
    /*  94 */ { 3, { 0, 0, 0 }, (u32)Task_QuickDrawObject },
    /*  95 */ { 4, { 0, 0, 0 }, (u32)Task_BombRallyObject },
    /*  96 */ { 3, { 0, 0, 0 }, (u32)Task_AirGrindObject },
    /*  97 */ { 3, { 0, 0, 0 }, (u32)sub_0807450c },
    /*  98 */ { 3, { 0, 0, 0 }, (u32)sub_08075000 },
    /*  99 */ { 3, { 0, 0, 0 }, (u32)sub_080752f4 },
    /* 100 */ { 3, { 0, 0, 0 }, (u32)sub_080c6c64 },
    /* 101 */ { 3, { 0, 0, 0 }, (u32)sub_080c9004 },
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
    /* 116 */ { 2, { 0, 0, 0 }, (u32)sub_080b0cc8 },
    /* 117 */ { 2, { 0, 0, 0 }, (u32)sub_080b13bc },
    /* 118 */ { 2, { 0, 0, 0 }, (u32)Task_MrTickTockRing },
    /* 119 */ { 2, { 0, 0, 0 }, (u32)Task_MrTickTockNote },
    /* 120 */ { 2, { 0, 0, 0 }, (u32)Task_MetaKnightSword },
    /* 121 */ { 4, { 0, 0, 0 }, (u32)sub_080b1710 },
    /* 122 */ { 4, { 0, 0, 0 }, (u32)sub_080b1770 },
    /* 123 */ { 2, { 0, 0, 0 }, (u32)sub_080b22f8 },
    /* 124 */ { 2, { 0, 0, 0 }, (u32)sub_080b2418 },
    /* 125 */ { 2, { 0, 0, 0 }, (u32)Task_WhispyWoodsApple },
    /* 126 */ { 2, { 0, 0, 0 }, (u32)Task_WhispyWoodsAirPuff },
    /* 127 */ { 4, { 0, 0, 0 }, (u32)sub_080a4c84 },
    /* 128 */ { 2, { 0, 0, 0 }, (u32)sub_080aca90 },
    /* 129 */ { 2, { 0, 0, 0 }, (u32)Task_AxeKnightAxe },
    /* 130 */ { 4, { 0, 0, 0 }, (u32)Task_MaceKnightMace },
    /* 131 */ { 2, { 0, 0, 0 }, (u32)Task_TridentKnightTrident },
    /* 132 */ { 2, { 0, 0, 0 }, (u32)Task_JavelinKnightJavelin },
    /* 133 */ { 2, { 0, 0, 0 }, (u32)Task_BugzzyLadybug },
    /* 134 */ { 2, { 0, 0, 0 }, (u32)Task_UFOLaser },
    /* 135 */ { 2, { 0, 0, 0 }, (u32)sub_080af30c },
    /* 136 */ { 2, { 0, 0, 0 }, (u32)sub_080acc9c },
    /* 137 */ { 2, { 0, 0, 0 }, (u32)Task_GipStar },
    /* 138 */ { 2, { 0, 0, 0 }, (u32)Task_PhanPhanApple },
    /* 139 */ { 2, { 0, 0, 0 }, (u32)sub_080671c0 },
    /* 140 */ { 1, { 0, 0, 0 }, (u32)Task_ActorSplash },
    /* 141 */ { 1, { 0, 0, 0 }, (u32)Task_StarFlash },
    /* 142 */ { 1, { 0, 0, 0 }, (u32)Task_StarFlashOnParent },
    /* 143 */ { 1, { 0, 0, 0 }, (u32)Task_DustTrail },
    /* 144 */ { 1, { 0, 0, 0 }, (u32)Task_DustPuff },
    /* 145 */ { 1, { 0, 0, 0 }, (u32)sub_0806cf70 },
    /* 146 */ { 1, { 0, 0, 0 }, (u32)sub_0806d148 },
    /* 147 */ { 3, { 0, 0, 0 }, (u32)Task_DustBurst },
    /* 148 */ { 1, { 0, 0, 0 }, (u32)Task_StarScatter },
    /* 149 */ { 1, { 0, 0, 0 }, (u32)Task_RayBurst },
    /* 150 */ { 1, { 0, 0, 0 }, (u32)Task_SmallBlast },
    /* 151 */ { 1, { 0, 0, 0 }, (u32)Task_StarScatterOnParent },
    /* 152 */ { 1, { 0, 0, 0 }, (u32)Task_RayBurstOnParent },
    /* 153 */ { 1, { 0, 0, 0 }, (u32)Task_SmallBlastOnParent },
    /* 154 */ { 1, { 0, 0, 0 }, (u32)sub_0806d7ec },
    /* 155 */ { 1, { 0, 0, 0 }, (u32)Task_RingStar },
    /* 156 */ { 1, { 0, 0, 0 }, (u32)sub_0806daec },
    /* 157 */ { 1, { 0, 0, 0 }, (u32)sub_0806dd90 },
    /* 158 */ { 4, { 0, 0, 0 }, (u32)Task_TrailFlash },
    /* 159 */ { 4, { 0, 0, 0 }, (u32)Task_HitFrost },
    /* 160 */ { 4, { 0, 0, 0 }, (u32)Task_HitFlames },
    /* 161 */ { 4, { 0, 0, 0 }, (u32)Task_HitSparks },
    /* 162 */ { 1, { 0, 0, 0 }, (u32)sub_0806da04 },
    /* 163 */ { 1, { 0, 0, 0 }, (u32)sub_0806da20 },
    /* 164 */ { 3, { 0, 0, 0 }, (u32)Task_WarpStarSparkle },
    /* 165 */ { 3, { 0, 0, 0 }, (u32)sub_08074c0c },
    /* 166 */ { 3, { 0, 0, 0 }, (u32)Task_AbilityReleaseFlash },
    /* 167 */ { 3, { 0, 0, 0 }, (u32)sub_0806e73c },
    /* 168 */ { 3, { 0, 0, 0 }, (u32)sub_0806e84c },
    /* 169 */ { 3, { 0, 0, 0 }, (u32)sub_0806e9fc },
    /* 170 */ { 3, { 0, 0, 0 }, (u32)sub_080b3c68 },
    /* 171 */ { 4, { 0, 0, 0 }, (u32)Task_IceBlock },
    /* 172 */ { 3, { 0, 0, 0 }, (u32)sub_0806ee1c },
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
    /* 189 */ { 4, { 0, 0, 0 }, (u32)sub_080adca4 },
    /* 190 */ { 4, { 0, 0, 0 }, (u32)sub_080adefc },
    /* 191 */ { 4, { 0, 0, 0 }, (u32)sub_080ae038 },
    /* 192 */ { 4, { 0, 0, 0 }, (u32)sub_080ae1f0 },
    /* 193 */ { 4, { 0, 0, 0 }, (u32)sub_080a54e4 },
    /* 194 */ { 4, { 0, 0, 0 }, (u32)Task_NoddyBubble },
    /* 195 */ { 4, { 0, 0, 0 }, (u32)sub_080a983c },
    /* 196 */ { 4, { 0, 0, 0 }, (u32)sub_080a99a0 },
    /* 197 */ { 4, { 0, 0, 0 }, (u32)sub_080a9c28 },
    /* 198 */ { 4, { 0, 0, 0 }, (u32)sub_080a9cf0 },
    /* 199 */ { 4, { 0, 0, 0 }, (u32)sub_080a9da4 },
    /* 200 */ { 4, { 0, 0, 0 }, (u32)Task_BugzzyAfterimage },
    /* 201 */ { 3, { 0, 0, 0 }, (u32)sub_080abe38 },
    /* 202 */ { 3, { 0, 0, 0 }, (u32)sub_080abf10 },
    /* 203 */ { 3, { 0, 0, 0 }, (u32)sub_080ac020 },
    /* 204 */ { 3, { 0, 0, 0 }, (u32)sub_080ac124 },
    /* 205 */ { 3, { 0, 0, 0 }, (u32)sub_080ac27c },
    /* 206 */ { 3, { 0, 0, 0 }, (u32)sub_080ac410 },
    /* 207 */ { 3, { 0, 0, 0 }, (u32)sub_080ac3a4 },
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
    /* 221 */ { 3, { 0, 0, 0 }, (u32)sub_0802eb28 },
    /* 222 */ { 3, { 0, 0, 0 }, (u32)sub_0802ec7c },
    /* 223 */ { 3, { 0, 0, 0 }, (u32)sub_0802ede4 },
    /* 224 */ { 3, { 0, 0, 0 }, (u32)sub_0802eff8 },
    /* 225 */ { 3, { 0, 0, 0 }, (u32)sub_0802f0cc },
    /* 226 */ { 3, { 0, 0, 0 }, (u32)sub_0802f26c },
    /* 227 */ { 3, { 0, 0, 0 }, (u32)sub_0802f38c },
    /* 228 */ { 3, { 0, 0, 0 }, (u32)sub_0802f480 },
    /* 229 */ { 3, { 0, 0, 0 }, (u32)sub_0802f62c },
    /* 230 */ { 3, { 0, 0, 0 }, (u32)sub_0802f84c },
    /* 231 */ { 3, { 0, 0, 0 }, (u32)sub_0802faa8 },
    /* 232 */ { 3, { 0, 0, 0 }, (u32)sub_0802fe64 },
    /* 233 */ { 3, { 0, 0, 0 }, (u32)sub_08030034 },
    /* 234 */ { 3, { 0, 0, 0 }, (u32)sub_080300c0 },
    /* 235 */ { 3, { 0, 0, 0 }, (u32)sub_080301a4 },
    /* 236 */ { 4, { 0, 0, 0 }, (u32)Task_StageEffect },
    /* 237 */ { 4, { 0, 0, 0 }, (u32)Task_IntroStoryPicture },
    /* 238 */ { 4, { 0, 0, 0 }, (u32)sub_0800daf8 },
    /* 239 */ { 4, { 0, 0, 0 }, (u32)sub_0800db64 },
    /* 240 */ { 4, { 0, 0, 0 }, (u32)Task_FileSelectCursor },
    /* 241 */ { 4, { 0, 0, 0 }, (u32)sub_0800de6c },
    /* 242 */ { 4, { 0, 0, 0 }, (u32)Task_FileMenuHighlight },
    /* 243 */ { 4, { 0, 0, 0 }, (u32)sub_0800e9a4 },
    /* 244 */ { 4, { 0, 0, 0 }, (u32)sub_0800eae4 },
    /* 245 */ { 4, { 0, 0, 0 }, (u32)sub_0800e314 },
    /* 246 */ { 4, { 0, 0, 0 }, (u32)sub_0800e46c },
    /* 247 */ { 4, { 0, 0, 0 }, (u32)Task_ModeListCursor },
    /* 248 */ { 4, { 0, 0, 0 }, (u32)sub_0800e81c },
    /* 249 */ { 4, { 0, 0, 0 }, (u32)sub_0800ef30 },
    /* 250 */ { 4, { 0, 0, 0 }, (u32)sub_0800f084 },
    /* 251 */ { 4, { 0, 0, 0 }, (u32)Task_LinkPlayPlayerList },
    /* 252 */ { 4, { 0, 0, 0 }, (u32)sub_0800f390 },
    /* 253 */ { 4, { 0, 0, 0 }, (u32)sub_0800f5ec },
    /* 254 */ { 4, { 0, 0, 0 }, (u32)Task_SoundTestCursors },
    /* 255 */ { 4, { 0, 0, 0 }, (u32)Task_SoundTestPulse },
    /* 256 */ { 4, { 0, 0, 0 }, (u32)Task_MenuScreenTitle },
    /* 257 */ { 4, { 0, 0, 0 }, (u32)Task_BgScroll },
    /* 258 */ { 4, { 0, 0, 0 }, (u32)sub_0800fb94 },
    /* 259 */ { 4, { 0, 0, 0 }, (u32)sub_0800fa30 },
    /* 260 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverSprite },
    /* 261 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverCursor },
    /* 262 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverPalette },
    /* 263 */ { 4, { 0, 0, 0 }, (u32)Task_HalveScore },
    /* 264 */ { 4, { 0, 0, 0 }, (u32)Task_GameOverObject },
    /* 265 */ { 4, { 0, 0, 0 }, (u32)sub_08008348 },
};
