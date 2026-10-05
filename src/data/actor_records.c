#include "global.h"
#include "task.h"
#include "actor.h"

/* The actor records of seg 18 (0x0873F2B8-0x0874C3CF, issue #36 phase 2
 * run 3): 136 struct ActorDef (0x2C bytes) and 43 struct ActorAux (8 bytes)
 * records in 34 runs, each run in address order.  ActorInitFromDefSlot and
 * ActorLoadDefSlot (src/actor_63698.c) read an ActorDef when a task binds
 * it (Actor.def); ActorDef.unk10 becomes Actor.unk60, the ActorAux whose
 * altAttackBox becomes gAttackBox (src/actor_673ec.c).  The six pointer
 * tables are src/data/actor_defs.c; since #167 the behaviour tables and
 * scripts between the runs are src/data/actor_tables.c and the terrain
 * handlers and hit reactions the records point at src/data/
 * actor_handlers.c, while the hit boxes and other value data stay
 * structure-only data in data/actor_rodata.s.
 *
 * Each run is a named section .actor_rec_<address>: linker.ld lists them
 * between the data pieces of actor_rodata inside ONE output section
 * (docs/data.md 5.2).  The records are not const (the qualifier would
 * reach ActorLoadDef's parameter and Actor.def under -Werror); the section
 * attribute is what places them in ROM.  A word the data file did not
 * symbolize would stay a number here, so the shift test sees the same
 * relocations; since ActorDef.unk10 is a pointer everywhere (#36 run 3)
 * there is none. */

#define ACTOR_REC(addr) __attribute__((section(".actor_rec_" #addr)))

/* Functions whose prototype lives in a header this file does not include
 * (enemy.h and cutscene.h declare 31 of these records as u32 [], a view
 * that conflicts with the definitions below, lesson 3.517). */
void SparkyTeardown(void);
void UFOTeardown(void);
void CoolSpookTeardown(void);
void LaserBallTeardown(void);
void MetaKnightsKnightTeardown(void);
void sub_080acc8c(void);

/* Record targets no header declares: labels of the data files. */
extern const u8 gAbilityStarAttackBox[];
extern const u8 gUnk_0873F4E4[];
extern const u8 gUnk_0873F51C[];
extern const u8 gWarpStarAttackBox[];
extern const u8 gCannonAttackBox[];
extern const u8 gCannonFuseAttackBox[];
extern const u8 gBigSwitchAttackBox[];
extern const u8 gUnk_0873F5C4[];
extern const u8 gCannonTerrainBox[];
extern const u8 gCannonFuseTerrainBox[];
extern const u8 gBigSwitchTerrainBox[];
extern const u8 gUnk_0873F720[];
extern const u8 gUnk_0873F73C[];
extern const u8 gUnk_0873F774[];
extern const u8 gUnk_0873F7AC[];
extern const u8 gUnk_0873F7C8[];
extern const u8 gUnk_0873F800[];
extern const u8 gUnk_0873F89C[];
extern const u8 gUnk_0873F8A4[];
extern const u8 gUnk_0873F8E4[];
extern const u8 gScarfyAttackBox[];
extern const u8 gPengyIceBreathAttackBox[];
extern const u8 gUFOLaserAttackBox[];
extern const u8 gUFOLaserTerrainBox[];
extern const u8 gGordoAttackBox[];
extern const u8 gCoolSpookAttackBox[];
extern const u8 gUnk_08741AF8[];
extern const u8 gRockyTerrainBox[];
extern const u8 gSirKibbleTerrainBox[];
extern const u8 gUnk_08741B24[];
extern const u8 gGordoTerrainBox[];
extern const u8 gCoolSpookTerrainBox[];
extern const u8 gUnk_08741B3C[];
extern const u8 gHotHeadTerrainBox[];
extern const u8 gWheelieTerrainBox[];
extern const u8 gFlamerTerrainBox[];
extern const u8 gSirKibbleCutterAttackBox[];
extern const u8 gHotHeadFireAttackBox[];
extern const u8 gHotHeadFireTerrainBox[];
extern const u8 gChillyAttackBox[];
extern const u8 gUnk_08742C14[];
extern const u8 gChillyTerrainBox[];
extern const u8 gBubblesTerrainBox[];
extern const u8 gGipTerrainBox[];
extern const u8 gChillyFreezeAttackBox[];
extern const u8 gWaddleDooBeamAttackBox[];
extern const u8 gGlunkShotAttackBox[];
extern const u8 gGipStarAttackBox[];
extern const u8 gWaddleDooBeamTerrainBox[];
extern const u8 gGlunkShotTerrainBox[];
extern const u8 gUnk_08743414[];
extern const u8 gShotzoAttackBox[];
extern const u8 gParasolShotzoAttackBox[];
extern const u8 gLaserBallLaserAttackBox[];
extern const u8 gShotzoCannonballAttackBox[];
extern const u8 gLaserBallLaserTerrainBox[];
extern const u8 gShotzoCannonballTerrainBox[];
extern const u8 gUnk_08743BD0[];
extern const u8 gUnk_08743BEC[];
extern const u8 gUnk_08743C40[];
extern const u8 gUnk_08743C94[];
extern const u8 gUnk_08743CE8[];
extern const u8 gUnk_08743D3C[];
extern const u8 gUnk_08743DE4[];
extern const u8 gUnk_08743E00[];
extern const u8 gUnk_08743E54[];
extern const u8 gUnk_08743E8C[];
extern const u8 gUnk_08743EFC[];
extern const u8 gUnk_08743F18[];
extern const u8 gUnk_08743F6C[];
extern const u8 gBonkersTerrainBox[];
extern const u8 gPoppyBrosSrTerrainBox[];
extern const u8 gBugzzyTerrainBox[];
extern const u8 gBonkersNutAttackBox[];
extern const u8 gPoppyBrosSrBombAttackBox[];
extern const u8 gBugzzyLadybugAttackBox[];
extern const u8 gBonkersNutTerrainBox[];
extern const u8 gPoppyBrosSrBombTerrainBox[];
extern const u8 gBugzzyLadybugTerrainBox[];
extern const u8 gUnk_087449E8[];
extern const u8 gUnk_08744A20[];
extern const u8 gUnk_08744A74[];
extern const u8 gUnk_08744AC8[];
extern const u8 gUnk_08744B54[];
extern const u8 gUnk_08744BA8[];
extern const u8 gUnk_08744C34[];
extern const u8 gUnk_08744C88[];
extern const u8 gUnk_08744CDC[];
extern const u8 gUnk_08744D14[];
extern const u8 gUnk_08744D68[];
extern const u8 gUnk_08744DBC[];
extern const u8 gUnk_08744E10[];
extern const u8 gUnk_08744E64[];
extern const u8 gUnk_08744EB8[];
extern const u8 gUnk_08744F0C[];
extern const u8 gUnk_08744F60[];
extern const u8 gUnk_08744FB4[];
extern const u8 gUnk_08745008[];
extern const u8 gUnk_0874505C[];
extern const u8 gUnk_087450B0[];
extern const u8 gUnk_0874521C[];
extern const u8 gUnk_08745254[];
extern const u8 gGrandWheelieTerrainBox[];
extern const u8 gUnk_08745304[];
extern const u8 gPhanPhanTerrainBox[];
extern const u8 gGrandWheelieMiniWheelieAttackBox[];
extern const u8 gPhanPhanAppleAttackBox[];
extern const u8 gGrandWheelieMiniWheelieTerrainBox[];
extern const u8 gUnk_08745868[];
extern const u8 gUnk_087458D8[];
extern const u8 gUnk_08745964[];
extern const u8 gUnk_08745A0C[];
extern const u8 gUnk_08745A1C[];
extern const u8 gMrFrostyIceCubeAttackBox[];
extern const u8 gUnk_08745BEC[];
extern const u8 gMrTickTockNoteAttackBox[];
extern const u8 gMrFrostyIceCubeTerrainBox[];
extern const u8 gMrTickTockNoteTerrainBox[];
extern const u8 gUnk_08747EBC[];
extern const u8 gUnk_08747ED8[];
extern const u8 gAxeKnightAxeAttackBox[];
extern const u8 gMaceKnightMaceAttackBox[];
extern const u8 gTridentKnightTridentAttackBox[];
extern const u8 gJavelinKnightJavelinAttackBox[];
extern const u8 gAxeKnightTerrainBox[];
extern const u8 gMaceKnightTerrainBox[];
extern const u8 gTridentKnightTerrainBox[];
extern const u8 gJavelinKnightTerrainBox[];
extern const u8 gKingDededeAttackBox[];
extern const u8 gUnk_08748804[];
extern const u8 gUnk_0874883C[];
extern const u8 gUnk_08748858[];
extern const u8 gUnk_087488AC[];
extern const u8 gUnk_087488C8[];
extern const u8 gKingDededeTerrainBox[];
extern const u8 gMrShineAndMrBrightTerrainBox[];
extern const u8 gUnk_08748910[];
extern const u8 gKingDededeStarAttackBox[];
extern const u8 gKingDededeAirPuffAttackBox[];
extern const u8 gUnk_08748C60[];
extern const u8 gUnk_08748C7C[];
extern const u8 gUnk_08748C98[];
extern const u8 gUnk_08748CB4[];
extern const u8 gKingDededeStarTerrainBox[];
extern const u8 gKingDededeAirPuffTerrainBox[];
extern const u8 gUnk_08748CE0[];
extern const u8 gUnk_08748CE8[];
extern const u8 gUnk_08748CF0[];
extern const u8 gUnk_08749598[];
extern const u8 gUnk_08749608[];
extern const u8 gKrackoAttackBox[];
extern const u8 gUnk_0874973C[];
extern const u8 gUnk_08749790[];
extern const u8 gUnk_08749870[];
extern const u8 gUnk_0874988C[];
extern const u8 gMetaKnightTerrainBox[];
extern const u8 gKrackoTerrainBox[];
extern const u8 gMetaKnightSwordAttackBox[];
extern const u8 gKrackoStarmanAttackBox[];
extern const u8 gNightmareWizardStarAttackBox[];
extern const u8 gMetaKnightSwordTerrainBox[];
extern const u8 gNightmareWizardStarTerrainBox[];
extern const u8 gPaintRollerAttackBox[];
extern const u8 gUnk_0874B3C4[];
extern const u8 gHeavyMoleAttackBox[];
extern const u8 gUnk_0874B418[];
extern const u8 gNightmarePowerOrbAttackBox[];
extern const u8 gPaintRollerTerrainBox[];
extern const u8 gUnk_0874BB7C[];
extern const u8 gPaintRollerLightningAttackBox[];
extern const u8 gUnk_0874BF34[];
extern const u8 gUnk_0874BF50[];
extern const u8 gWhispyWoodsAttackBox[];
extern const u8 gUnk_0874C1BC[];
extern const u8 gWhispyWoodsTerrainBox[];
extern const u8 gWhispyWoodsAppleAttackBox[];
extern const u8 gWhispyWoodsAirPuffAttackBox[];
extern const u8 gWhispyWoodsAppleTerrainBox[];
extern const u8 gWhispyWoodsAirPuffTerrainBox[];

/* Actor.terrainHandlers and Actor.hitReactions targets: src/data/actor_handlers.c. */
extern struct ActorHandlers gAbilityStarTerrainHandlers;
extern struct ActorHandlers gPickupTerrainHandlers;
extern struct ActorHandlers gWaddleDeeTerrainHandlers;
extern struct ActorHandlers gPengyTerrainHandlers;
extern struct ActorHandlers gBomberTerrainHandlers;
extern struct ActorHandlers gSparkyTerrainHandlers;
extern struct ActorHandlers gSwordAndBladeKnightTerrainHandlers;
extern struct ActorHandlers gNeedlousTerrainHandlers;
extern struct ActorHandlers gRockyTerrainHandlers;
extern struct ActorHandlers gSirKibbleTerrainHandlers;
extern struct ActorHandlers gCappyTerrainHandlers;
extern struct ActorHandlers gKabuTerrainHandlers;
extern struct ActorHandlers gTwisterTerrainHandlers;
extern struct ActorHandlers gStarmanTerrainHandlers;
extern struct ActorHandlers gHotHeadTerrainHandlers;
extern struct ActorHandlers gPoppyBrosJrTerrainHandlers;
extern struct ActorHandlers gPoppyBrosJrRiderTerrainHandlers;
extern struct ActorHandlers gWheelieTerrainHandlers;
extern struct ActorHandlers gFlamerTerrainHandlers;
extern struct ActorHandlers gNoddyTerrainHandlers;
extern struct ActorHandlers gChillyTerrainHandlers;
extern struct ActorHandlers gWaddleDooTerrainHandlers;
extern struct ActorHandlers gTwizzyTerrainHandlers;
extern struct ActorHandlers gSquishyTerrainHandlers;
extern struct ActorHandlers gBubblesTerrainHandlers;
extern struct ActorHandlers gGlunkTerrainHandlers;
extern struct ActorHandlers gSlippyTerrainHandlers;
extern struct ActorHandlers gBlipperTerrainHandlers;
extern struct ActorHandlers gGipTerrainHandlers;
extern struct ActorHandlers gWaddleDooBeamTerrainHandlers;
extern struct ActorHandlers gGlunkShotTerrainHandlers;
extern struct ActorHandlers gBroomHatterTerrainHandlers;
extern struct ActorHandlers gCoconutTerrainHandlers;
extern struct ActorHandlers gShotzoTerrainHandlers;
extern struct ActorHandlers gConerTerrainHandlers;
extern struct ActorHandlers gLaserBallLaserTerrainHandlers;
extern struct ActorHandlers gShotzoCannonballTerrainHandlers;
extern struct ActorHandlers gBonkersTerrainHandlers;
extern struct ActorHandlers gPoppyBrosSrTerrainHandlers;
extern struct ActorHandlers gBugzzyTerrainHandlers;
extern struct ActorHandlers gBonkersNutTerrainHandlers;
extern struct ActorHandlers gPoppyBrosSrBombTerrainHandlers;
extern struct ActorHandlers gGrandWheelieTerrainHandlers;
extern struct ActorHandlers gFireLionTerrainHandlers;
extern struct ActorHandlers gPhanPhanTerrainHandlers;
extern struct ActorHandlers gGrandWheelieMiniWheelieTerrainHandlers;
extern struct ActorHandlers gPhanPhanAppleTerrainHandlers;
extern struct ActorHandlers gMrFrostyTerrainHandlers;
extern struct ActorHandlers gMrTickTockTerrainHandlers;
extern struct ActorHandlers gMrFrostyIceCubeTerrainHandlers;
extern struct ActorHandlers gMrTickTockNoteTerrainHandlers;
extern struct ActorHandlers gAxeKnightTerrainHandlers;
extern struct ActorHandlers gMaceKnightTerrainHandlers;
extern struct ActorHandlers gTridentKnightTerrainHandlers;
extern struct ActorHandlers gJavelinKnightTerrainHandlers;
extern struct ActorHandlers gKingDededeTerrainHandlers;
extern struct ActorHandlers gUnk_0874894C;
extern struct ActorHandlers gMetaKnightTerrainHandlers;
extern struct ActorHandlers gUnk_0874BF58;
extern struct ActorHandlers gWhispyWoodsAppleTerrainHandlers;
extern struct ActorVt gAbilityStarHitReactions;
extern struct ActorVt gUnk_0873F640;
extern struct ActorVt gUnk_0873F64C;
extern struct ActorVt gBigSwitchHitReactions;
extern struct ActorVt gUnk_0873F944;
extern struct ActorVt gWaddleDeeHitReactions;
extern struct ActorVt gParasolWaddleDeeHitReactions;
extern struct ActorVt gPengyHitReactions;
extern struct ActorVt gBomberHitReactions;
extern struct ActorVt gSparkyHitReactions;
extern struct ActorVt gScarfyHitReactions;
extern struct ActorVt gUnk_08740F68;
extern struct ActorVt gBlockStarHitReactions;
extern struct ActorVt gNeedlousHitReactions;
extern struct ActorVt gUFOHitReactions;
extern struct ActorVt gParasolHitReactions;
extern struct ActorVt gUFOLaserHitReactions;
extern struct ActorVt gRockyHitReactions;
extern struct ActorVt gSirKibbleHitReactions;
extern struct ActorVt gCappyHitReactions;
extern struct ActorVt gCappyCaplessHitReactions;
extern struct ActorVt gCoolSpookHitReactions;
extern struct ActorVt gKabuHitReactions;
extern struct ActorVt gTwisterHitReactions;
extern struct ActorVt gStarmanHitReactions;
extern struct ActorVt gHotHeadHitReactions;
extern struct ActorVt gPoppyBrosJrHitReactions;
extern struct ActorVt gPoppyBrosJrOnAppleHitReactions;
extern struct ActorVt gPoppyBrosJrOnMaximTomatoHitReactions;
extern struct ActorVt gPoppyBrosJrAppleHitReactions;
extern struct ActorVt gPoppyBrosJrMaximTomatoHitReactions;
extern struct ActorVt gWheelieHitReactions;
extern struct ActorVt gFlamerHitReactions;
extern struct ActorVt gSirKibbleCutterHitReactions;
extern struct ActorVt gHotHeadFireHitReactions;
extern struct ActorVt gNoddyHitReactions;
extern struct ActorVt gChillyHitReactions;
extern struct ActorVt gWaddleDooHitReactions;
extern struct ActorVt gParasolWaddleDooHitReactions;
extern struct ActorVt gBrontoBurtHitReactions;
extern struct ActorVt gTwizzyHitReactions;
extern struct ActorVt gSquishyHitReactions;
extern struct ActorVt gBubblesHitReactions;
extern struct ActorVt gGlunkHitReactions;
extern struct ActorVt gSlippyHitReactions;
extern struct ActorVt gBlipperHitReactions;
extern struct ActorVt gGipHitReactions;
extern struct ActorVt gGlunkShotHitReactions;
extern struct ActorVt gGipStarHitReactions;
extern struct ActorVt gBroomHatterHitReactions;
extern struct ActorVt gLaserBallHitReactions;
extern struct ActorVt gCoconutHitReactions;
extern struct ActorVt gShotzoHitReactions;
extern struct ActorVt gConerHitReactions;
extern struct ActorVt gParasolShotzoHitReactions;
extern struct ActorVt gLaserBallLaserHitReactions;
extern struct ActorVt gShotzoCannonballHitReactions;
extern struct ActorVt gBonkersHitReactions;
extern struct ActorVt gPoppyBrosSrHitReactions;
extern struct ActorVt gBugzzyHitReactions;
extern struct ActorVt gBonkersNutHitReactions;
extern struct ActorVt gPoppyBrosSrBombHitReactions;
extern struct ActorVt gBugzzyLadybugHitReactions;
extern struct ActorVt gGrandWheelieHitReactions;
extern struct ActorVt gFireLionHitReactions;
extern struct ActorVt gPhanPhanHitReactions;
extern struct ActorVt gGrandWheelieMiniWheelieHitReactions;
extern struct ActorVt gPhanPhanAppleHitReactions;
extern struct ActorVt gMrFrostyHitReactions;
extern struct ActorVt gMrTickTockHitReactions;
extern struct ActorVt gMrFrostyIceCubeHitReactions;
extern struct ActorVt gMrTickTockNoteHitReactions;
extern struct ActorVt gAxeKnightHitReactions;
extern struct ActorVt gAxeKnightAxeHitReactions;
extern struct ActorVt gMaceKnightHitReactions;
extern struct ActorVt gTridentKnightHitReactions;
extern struct ActorVt gTridentKnightTridentHitReactions;
extern struct ActorVt gJavelinKnightHitReactions;
extern struct ActorVt gJavelinKnightJavelinHitReactions;
extern struct ActorVt gKingDededeHitReactions;
extern struct ActorVt gMrShineAndMrBrightHitReactions;
extern struct ActorVt gUnk_08748998;
extern struct ActorVt gKingDededeStarHitReactions;
extern struct ActorVt gUnk_08748D10;
extern struct ActorVt gUnk_08748D1C;
extern struct ActorVt gMetaKnightHitReactions;
extern struct ActorVt gKrackoHitReactions;
extern struct ActorVt gNightmareWizardHitReactions;
extern struct ActorVt gKrackoStarmanHitReactions;
extern struct ActorVt gNightmareWizardStarHitReactions;
extern struct ActorVt gPaintRollerHitReactions;
extern struct ActorVt gHeavyMoleHitReactions;
extern struct ActorVt gNightmarePowerOrbHitReactions;
extern struct ActorVt gUnk_0874BF74;
extern struct ActorVt gUnk_0874BF80;
extern struct ActorVt gUnk_0874BF8C;
extern struct ActorVt gUnk_0874BF98;
extern struct ActorVt gUnk_0874BFA4;
extern struct ActorVt gUnk_0874BFB0;
extern struct ActorVt gUnk_0874BFBC;
extern struct ActorVt gUnk_0874BFC8;
extern struct ActorVt gHeavyMoleYellowMissileHitReactions;
extern struct ActorVt gHeavyMoleRedMissileHitReactions;
extern struct ActorVt gWhispyWoodsHitReactions;
extern struct ActorVt gWhispyWoodsAppleHitReactions;
extern struct ActorVt gWhispyWoodsAirPuffHitReactions;

/* ActorAux records defined further down. */
extern struct ActorAux gUnk_0873F8EC;
extern struct ActorAux gUnk_0874402C;
extern struct ActorAux gUnk_08744054;
extern struct ActorAux gUnk_0874406C;
extern struct ActorAux gUnk_0874531C;
extern struct ActorAux gUnk_08745354;
extern struct ActorAux gUnk_087453B4;
extern struct ActorAux gUnk_08745A2C;
extern struct ActorAux gUnk_08748198;
extern struct ActorAux gUnk_08748918;
extern struct ActorAux gUnk_08748920;
extern struct ActorAux gUnk_08748928;
extern struct ActorAux gUnk_08749AE8;
extern struct ActorAux gUnk_08749AF0;
extern struct ActorAux gUnk_08749AF8;
extern struct ActorAux gUnk_0874B4C8;
extern struct ActorAux gUnk_0874B4D0;
extern struct ActorAux gUnk_0874C1FC;

/* ---- 0x0873F2B8-0x0873F4C8: 12 record(s), section .actor_rec_0873f2b8 ---- */
struct ActorDef gAbilityStarDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gAbilityStarAttackBox,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gAbilityStarTerrainHandlers,
    .hitReactions = (u32)&gAbilityStarHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gOneUpDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)&gPickupTerrainHandlers,
    .hitReactions = (u32)&gUnk_0873F640,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMaximTomatoDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)&gPickupTerrainHandlers,
    .hitReactions = (u32)&gUnk_0873F640,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gInvincibleCandyDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)&gPickupTerrainHandlers,
    .hitReactions = (u32)&gUnk_0873F640,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gEnergyDrinkDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)&gPickupTerrainHandlers,
    .hitReactions = (u32)&gUnk_0873F640,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F394 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F51C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0873F64C,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gWarpStarDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gWarpStarAttackBox,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gCannonDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gCannonAttackBox,
    .terrainBox = (s32)gCannonTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gCannonFuseDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gCannonFuseAttackBox,
    .terrainBox = (s32)gCannonFuseTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBigSwitchDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gBigSwitchAttackBox,
    .terrainBox = (s32)gBigSwitchTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gBigSwitchHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gStakeDef ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F49C ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0873F664-0x0873F6E8: 3 record(s), section .actor_rec_0873f664 ---- */
struct ActorDef gInhalableStarDef ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F7C8,
    .terrainBox = (s32)gUnk_0873F8E4,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0873F944,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F690 ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_FIRE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F7C8,
    .terrainBox = (s32)gUnk_0873F8E4,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0873F944,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F6BC ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F800,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0873F8EC-0x0873F8F4: 1 record(s), section .actor_rec_0873f8ec ---- */
struct ActorAux gUnk_0873F8EC ACTOR_REC(0873f8ec) = {
    .hitDuration = 30,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0873F7AC,
};

/* ---- 0x08740BD4-0x08740DE4: 12 record(s), section .actor_rec_08740bd4 ---- */
struct ActorDef gWaddleDeeDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)&gWaddleDeeTerrainHandlers,
    .hitReactions = (u32)&gWaddleDeeHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gParasolWaddleDeeDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_PARASOL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F73C,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)&gWaddleDeeTerrainHandlers,
    .hitReactions = (u32)&gParasolWaddleDeeHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPengyDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_ICE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)&gPengyTerrainHandlers,
    .hitReactions = (u32)&gPengyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBomberDef ACTOR_REC(08740bd4) = {
    .health1Player = 1,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 100,
    .ability = ABILITY_CRASH,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)&gBomberTerrainHandlers,
    .hitReactions = (u32)&gBomberHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gSparkyDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_SPARK,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F774,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gSparkyTerrainHandlers,
    .hitReactions = (u32)&gSparkyHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))SparkyTeardown,
};
struct ActorDef gScarfyDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_SPARK,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gScarfyAttackBox,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gScarfyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gSwordKnightDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_SWORD,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gSwordAndBladeKnightTerrainHandlers,
    .hitReactions = (u32)&gUnk_08740F68,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBladeKnightDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_SWORD,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gSwordAndBladeKnightTerrainHandlers,
    .hitReactions = (u32)&gUnk_08740F68,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBlockStarDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F7AC,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gBlockStarHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gNeedlousDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 700,
    .ability = ABILITY_NEEDLE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gNeedlousTerrainHandlers,
    .hitReactions = (u32)&gNeedlousHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUFODef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 700,
    .ability = ABILITY_UFO,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUFOHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))UFOTeardown,
};
struct ActorDef gParasolDef ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_PARASOL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gParasolHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x087410E4-0x0874113C: 2 record(s), section .actor_rec_087410e4 ---- */
struct ActorDef gPengyIceBreathDef ACTOR_REC(087410e4) = {
    .health1Player = 1,
    .health2Players = 1,
    .health3Players = 1,
    .health4Players = 1,
    .score = 0,
    .ability = ABILITY_ICE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gPengyIceBreathAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUFOLaserDef ACTOR_REC(087410e4) = {
    .health1Player = 1,
    .health2Players = 1,
    .health3Players = 1,
    .health4Players = 1,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUFOLaserAttackBox,
    .terrainBox = (s32)gUFOLaserTerrainBox,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUFOLaserHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x087417B8-0x08741AA4: 17 record(s), section .actor_rec_087417b8 ---- */
struct ActorDef gRockyDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_STONE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gRockyTerrainBox,
    .terrainHandlers = (u32)&gRockyTerrainHandlers,
    .hitReactions = (u32)&gRockyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gSirKibbleDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_CUTTER,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gSirKibbleTerrainBox,
    .terrainHandlers = (u32)&gSirKibbleTerrainHandlers,
    .hitReactions = (u32)&gSirKibbleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gCappyDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B24,
    .terrainHandlers = (u32)&gCappyTerrainHandlers,
    .hitReactions = (u32)&gCappyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gCappyCaplessDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B24,
    .terrainHandlers = (u32)&gCappyTerrainHandlers,
    .hitReactions = (u32)&gCappyCaplessHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gGordoDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gGordoAttackBox,
    .terrainBox = (s32)gGordoTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gCoolSpookDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = ABILITY_LIGHT,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gCoolSpookAttackBox,
    .terrainBox = (s32)gCoolSpookTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gCoolSpookHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))CoolSpookTeardown,
};
struct ActorDef gKabuDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gKabuTerrainHandlers,
    .hitReactions = (u32)&gKabuHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gTwisterDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_TORNADO,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gTwisterTerrainHandlers,
    .hitReactions = (u32)&gTwisterHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gStarmanDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_HI_JUMP,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B3C,
    .terrainHandlers = (u32)&gStarmanTerrainHandlers,
    .hitReactions = (u32)&gStarmanHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHotHeadDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_FIRE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gHotHeadTerrainBox,
    .terrainHandlers = (u32)&gHotHeadTerrainHandlers,
    .hitReactions = (u32)&gHotHeadHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosJrDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPoppyBrosJrTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosJrHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosJrOnAppleDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AF8,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPoppyBrosJrRiderTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosJrOnAppleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosJrOnMaximTomatoDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AF8,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPoppyBrosJrRiderTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosJrOnMaximTomatoHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosJrAppleDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPoppyBrosJrRiderTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosJrAppleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosJrMaximTomatoDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPoppyBrosJrRiderTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosJrMaximTomatoHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gWheelieDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 800,
    .ability = ABILITY_WHEEL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gWheelieTerrainBox,
    .terrainHandlers = (u32)&gWheelieTerrainHandlers,
    .hitReactions = (u32)&gWheelieHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gFlamerDef ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_BURNING,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gFlamerTerrainBox,
    .terrainHandlers = (u32)&gFlamerTerrainHandlers,
    .hitReactions = (u32)&gFlamerHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08741EB4-0x08741F0C: 2 record(s), section .actor_rec_08741eb4 ---- */
struct ActorDef gSirKibbleCutterDef ACTOR_REC(08741eb4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gSirKibbleCutterAttackBox,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gSirKibbleCutterHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHotHeadFireDef ACTOR_REC(08741eb4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_FIRE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gHotHeadFireAttackBox,
    .terrainBox = (s32)gHotHeadFireTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gHotHeadFireHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x087429E8-0x08742BF8: 12 record(s), section .actor_rec_087429e8 ---- */
struct ActorDef gNoddyDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_SLEEP,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)&gNoddyTerrainHandlers,
    .hitReactions = (u32)&gNoddyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gChillyDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = ABILITY_FREEZE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gChillyAttackBox,
    .terrainBox = (s32)gChillyTerrainBox,
    .terrainHandlers = (u32)&gChillyTerrainHandlers,
    .hitReactions = (u32)&gChillyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gWaddleDooDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_BEAM,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)&gWaddleDooTerrainHandlers,
    .hitReactions = (u32)&gWaddleDooHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gParasolWaddleDooDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_PARASOL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F73C,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)&gWaddleDooTerrainHandlers,
    .hitReactions = (u32)&gParasolWaddleDooHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBrontoBurtDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gBrontoBurtHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gTwizzyDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gTwizzyTerrainHandlers,
    .hitReactions = (u32)&gTwizzyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gSquishyDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)&gSquishyTerrainHandlers,
    .hitReactions = (u32)&gSquishyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBubblesDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_BALL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08742C14,
    .terrainBox = (s32)gBubblesTerrainBox,
    .terrainHandlers = (u32)&gBubblesTerrainHandlers,
    .hitReactions = (u32)&gBubblesHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gGlunkDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gGlunkTerrainHandlers,
    .hitReactions = (u32)&gGlunkHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gSlippyDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gSlippyTerrainHandlers,
    .hitReactions = (u32)&gSlippyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBlipperDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)&gBlipperTerrainHandlers,
    .hitReactions = (u32)&gBlipperHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gGipDef ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gGipTerrainBox,
    .terrainHandlers = (u32)&gGipTerrainHandlers,
    .hitReactions = (u32)&gGipHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08742FBC-0x0874306C: 4 record(s), section .actor_rec_08742fbc ---- */
struct ActorDef gChillyFreezeDef ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gChillyFreezeAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gWaddleDooBeamDef ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gWaddleDooBeamAttackBox,
    .terrainBox = (s32)gWaddleDooBeamTerrainBox,
    .terrainHandlers = (u32)&gWaddleDooBeamTerrainHandlers,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gGlunkShotDef ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gGlunkShotAttackBox,
    .terrainBox = (s32)gGlunkShotTerrainBox,
    .terrainHandlers = (u32)&gGlunkShotTerrainHandlers,
    .hitReactions = (u32)&gGlunkShotHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gGipStarDef ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gGipStarAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gGipStarHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874330C-0x08743414: 6 record(s), section .actor_rec_0874330c ---- */
struct ActorDef gBroomHatterDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gBroomHatterTerrainHandlers,
    .hitReactions = (u32)&gBroomHatterHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gLaserBallDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_LASER,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gLaserBallHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))LaserBallTeardown,
};
struct ActorDef gCoconutDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gCoconutTerrainHandlers,
    .hitReactions = (u32)&gCoconutHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gShotzoDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gShotzoAttackBox,
    .terrainBox = (s32)gUnk_08743414,
    .terrainHandlers = (u32)&gShotzoTerrainHandlers,
    .hitReactions = (u32)&gShotzoHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gConerDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)&gConerTerrainHandlers,
    .hitReactions = (u32)&gConerHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gParasolShotzoDef ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = ABILITY_PARASOL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gParasolShotzoAttackBox,
    .terrainBox = (s32)gUnk_08743414,
    .terrainHandlers = (u32)&gShotzoTerrainHandlers,
    .hitReactions = (u32)&gParasolShotzoHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08743644-0x0874369C: 2 record(s), section .actor_rec_08743644 ---- */
struct ActorDef gLaserBallLaserDef ACTOR_REC(08743644) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gLaserBallLaserAttackBox,
    .terrainBox = (s32)gLaserBallLaserTerrainBox,
    .terrainHandlers = (u32)&gLaserBallLaserTerrainHandlers,
    .hitReactions = (u32)&gLaserBallLaserHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gShotzoCannonballDef ACTOR_REC(08743644) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gShotzoCannonballAttackBox,
    .terrainBox = (s32)gShotzoCannonballTerrainBox,
    .terrainHandlers = (u32)&gShotzoCannonballTerrainHandlers,
    .hitReactions = (u32)&gShotzoCannonballHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08743B4C-0x08743BD0: 3 record(s), section .actor_rec_08743b4c ---- */
struct ActorDef gBonkersDef ACTOR_REC(08743b4c) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 1200,
    .ability = ABILITY_HAMMER,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874402C,
    .attackBox = (u32)gUnk_08743BD0,
    .terrainBox = (s32)gBonkersTerrainBox,
    .terrainHandlers = (u32)&gBonkersTerrainHandlers,
    .hitReactions = (u32)&gBonkersHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosSrDef ACTOR_REC(08743b4c) = {
    .health1Player = 24,
    .health2Players = 32,
    .health3Players = 38,
    .health4Players = 43,
    .score = 1000,
    .ability = ABILITY_CRASH,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08744054,
    .attackBox = (u32)gUnk_08743DE4,
    .terrainBox = (s32)gPoppyBrosSrTerrainBox,
    .terrainHandlers = (u32)&gPoppyBrosSrTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosSrHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBugzzyDef ACTOR_REC(08743b4c) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 2000,
    .ability = ABILITY_BACKDROP,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874406C,
    .attackBox = (u32)gUnk_08743EFC,
    .terrainBox = (s32)gBugzzyTerrainBox,
    .terrainHandlers = (u32)&gBugzzyTerrainHandlers,
    .hitReactions = (u32)&gBugzzyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874402C-0x0874407C: 10 record(s), section .actor_rec_0874402c ---- */
struct ActorAux gUnk_0874402C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743BEC,
};
struct ActorAux gUnk_08744034 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743C40,
};
struct ActorAux gUnk_0874403C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743C94,
};
struct ActorAux gUnk_08744044 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743CE8,
};
struct ActorAux gUnk_0874404C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743D3C,
};
struct ActorAux gUnk_08744054 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E00,
};
struct ActorAux gUnk_0874405C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E54,
};
struct ActorAux gUnk_08744064 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E8C,
};
struct ActorAux gUnk_0874406C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743F18,
};
struct ActorAux gUnk_08744074 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743F6C,
};

/* ---- 0x087441D8-0x0874425C: 3 record(s), section .actor_rec_087441d8 ---- */
struct ActorDef gBonkersNutDef ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gBonkersNutAttackBox,
    .terrainBox = (s32)gBonkersNutTerrainBox,
    .terrainHandlers = (u32)&gBonkersNutTerrainHandlers,
    .hitReactions = (u32)&gBonkersNutHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosSrBombDef ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gPoppyBrosSrBombAttackBox,
    .terrainBox = (s32)gPoppyBrosSrBombTerrainBox,
    .terrainHandlers = (u32)&gPoppyBrosSrBombTerrainHandlers,
    .hitReactions = (u32)&gPoppyBrosSrBombHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gBugzzyLadybugDef ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gBugzzyLadybugAttackBox,
    .terrainBox = (s32)gBugzzyLadybugTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gBugzzyLadybugHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08744964-0x087449E8: 3 record(s), section .actor_rec_08744964 ---- */
struct ActorDef gGrandWheelieDef ACTOR_REC(08744964) = {
    .health1Player = 25,
    .health2Players = 33,
    .health3Players = 40,
    .health4Players = 45,
    .score = 1000,
    .ability = ABILITY_WHEEL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874531C,
    .attackBox = (u32)gUnk_087449E8,
    .terrainBox = (s32)gGrandWheelieTerrainBox,
    .terrainHandlers = (u32)&gGrandWheelieTerrainHandlers,
    .hitReactions = (u32)&gGrandWheelieHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gFireLionDef ACTOR_REC(08744964) = {
    .health1Player = 27,
    .health2Players = 36,
    .health3Players = 43,
    .health4Players = 48,
    .score = 1500,
    .ability = ABILITY_BURNING,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745354,
    .attackBox = (u32)gUnk_08744CDC,
    .terrainBox = (s32)gUnk_08745304,
    .terrainHandlers = (u32)&gFireLionTerrainHandlers,
    .hitReactions = (u32)&gFireLionHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPhanPhanDef ACTOR_REC(08744964) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 1500,
    .ability = ABILITY_THROW,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_087453B4,
    .attackBox = (u32)gUnk_0874521C,
    .terrainBox = (s32)gPhanPhanTerrainBox,
    .terrainHandlers = (u32)&gPhanPhanTerrainHandlers,
    .hitReactions = (u32)&gPhanPhanHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874531C-0x087453BC: 20 record(s), section .actor_rec_0874531c ---- */
struct ActorAux gUnk_0874531C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744A20,
};
struct ActorAux gUnk_08745324 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744A74,
};
struct ActorAux gUnk_0874532C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744AC8,
};
struct ActorAux gUnk_08745334 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744B54,
};
struct ActorAux gUnk_0874533C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744BA8,
};
struct ActorAux gUnk_08745344 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744C34,
};
struct ActorAux gUnk_0874534C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744C88,
};
struct ActorAux gUnk_08745354 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744D14,
};
struct ActorAux gUnk_0874535C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744D68,
};
struct ActorAux gUnk_08745364 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744DBC,
};
struct ActorAux gUnk_0874536C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744E10,
};
struct ActorAux gUnk_08745374 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744E64,
};
struct ActorAux gUnk_0874537C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744EB8,
};
struct ActorAux gUnk_08745384 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744F0C,
};
struct ActorAux gUnk_0874538C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744F60,
};
struct ActorAux gUnk_08745394 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744FB4,
};
struct ActorAux gUnk_0874539C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08745008,
};
struct ActorAux gUnk_087453A4 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874505C,
};
struct ActorAux gUnk_087453AC ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087450B0,
};
struct ActorAux gUnk_087453B4 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08745254,
};

/* ---- 0x08745530-0x08745588: 2 record(s), section .actor_rec_08745530 ---- */
struct ActorDef gGrandWheelieMiniWheelieDef ACTOR_REC(08745530) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gGrandWheelieMiniWheelieAttackBox,
    .terrainBox = (s32)gGrandWheelieMiniWheelieTerrainBox,
    .terrainHandlers = (u32)&gGrandWheelieMiniWheelieTerrainHandlers,
    .hitReactions = (u32)&gGrandWheelieMiniWheelieHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPhanPhanAppleDef ACTOR_REC(08745530) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gPhanPhanAppleAttackBox,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)&gPhanPhanAppleTerrainHandlers,
    .hitReactions = (u32)&gPhanPhanAppleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08745810-0x08745868: 2 record(s), section .actor_rec_08745810 ---- */
struct ActorDef gMrFrostyDef ACTOR_REC(08745810) = {
    .health1Player = 22,
    .health2Players = 29,
    .health3Players = 35,
    .health4Players = 39,
    .score = 1500,
    .ability = ABILITY_FREEZE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745A2C,
    .attackBox = (u32)gUnk_08745868,
    .terrainBox = (s32)gUnk_08745A0C,
    .terrainHandlers = (u32)&gMrFrostyTerrainHandlers,
    .hitReactions = (u32)&gMrFrostyHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMrTickTockDef ACTOR_REC(08745810) = {
    .health1Player = 26,
    .health2Players = 35,
    .health3Players = 41,
    .health4Players = 46,
    .score = 1500,
    .ability = ABILITY_MIKE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745A2C,
    .attackBox = (u32)gUnk_08745964,
    .terrainBox = (s32)gUnk_08745A1C,
    .terrainHandlers = (u32)&gMrTickTockTerrainHandlers,
    .hitReactions = (u32)&gMrTickTockHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08745A2C-0x08745A34: 1 record(s), section .actor_rec_08745a2c ---- */
struct ActorAux gUnk_08745A2C ACTOR_REC(08745a2c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087458D8,
};

/* ---- 0x08745B30-0x08745BB4: 3 record(s), section .actor_rec_08745b30 ---- */
struct ActorDef gMrFrostyIceCubeDef ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gMrFrostyIceCubeAttackBox,
    .terrainBox = (s32)gMrFrostyIceCubeTerrainBox,
    .terrainHandlers = (u32)&gMrFrostyIceCubeTerrainHandlers,
    .hitReactions = (u32)&gMrFrostyIceCubeHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMrTickTockRingDef ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08745BEC,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMrTickTockNoteDef ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gMrTickTockNoteAttackBox,
    .terrainBox = (s32)gMrTickTockNoteTerrainBox,
    .terrainHandlers = (u32)&gMrTickTockNoteTerrainHandlers,
    .hitReactions = (u32)&gMrTickTockNoteHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08747C80-0x08747EBC: 13 record(s), section .actor_rec_08747c80 ---- */
struct ActorDef gUnk_08747C80 ACTOR_REC(08747c80) = {
    .health1Player = 55,
    .health2Players = 72,
    .health3Players = 48,
    .health4Players = 60,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747CAC ACTOR_REC(08747c80) = {
    .health1Player = 64,
    .health2Players = 60,
    .health3Players = 64,
    .health4Players = 66,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747CD8 ACTOR_REC(08747c80) = {
    .health1Player = 54,
    .health2Players = 52,
    .health3Players = 72,
    .health4Players = 48,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747D04 ACTOR_REC(08747c80) = {
    .health1Player = 55,
    .health2Players = 64,
    .health3Players = 66,
    .health4Players = 56,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747D30 ACTOR_REC(08747c80) = {
    .health1Player = 60,
    .health2Players = 54,
    .health3Players = 48,
    .health4Players = 60,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gAxeKnightDef ACTOR_REC(08747c80) = {
    .health1Player = 6,
    .health2Players = 6,
    .health3Players = 6,
    .health4Players = 6,
    .score = 500,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gAxeKnightTerrainBox,
    .terrainHandlers = (u32)&gAxeKnightTerrainHandlers,
    .hitReactions = (u32)&gAxeKnightHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))MetaKnightsKnightTeardown,
};
struct ActorDef gAxeKnightAxeDef ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gAxeKnightAxeAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gAxeKnightAxeHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMaceKnightDef ACTOR_REC(08747c80) = {
    .health1Player = 8,
    .health2Players = 8,
    .health3Players = 8,
    .health4Players = 8,
    .score = 500,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gMaceKnightTerrainBox,
    .terrainHandlers = (u32)&gMaceKnightTerrainHandlers,
    .hitReactions = (u32)&gMaceKnightHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))MetaKnightsKnightTeardown,
};
struct ActorDef gMaceKnightMaceDef ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gMaceKnightMaceAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gTridentKnightDef ACTOR_REC(08747c80) = {
    .health1Player = 6,
    .health2Players = 6,
    .health3Players = 6,
    .health4Players = 6,
    .score = 500,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gTridentKnightTerrainBox,
    .terrainHandlers = (u32)&gTridentKnightTerrainHandlers,
    .hitReactions = (u32)&gTridentKnightHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))MetaKnightsKnightTeardown,
};
struct ActorDef gTridentKnightTridentDef ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gTridentKnightTridentAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gTridentKnightTridentHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gJavelinKnightDef ACTOR_REC(08747c80) = {
    .health1Player = 4,
    .health2Players = 4,
    .health3Players = 4,
    .health4Players = 4,
    .score = 500,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gJavelinKnightTerrainBox,
    .terrainHandlers = (u32)&gJavelinKnightTerrainHandlers,
    .hitReactions = (u32)&gJavelinKnightHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))MetaKnightsKnightTeardown,
};
struct ActorDef gJavelinKnightJavelinDef ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gJavelinKnightJavelinAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gJavelinKnightJavelinHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08748198-0x087481A0: 1 record(s), section .actor_rec_08748198 ---- */
struct ActorAux gUnk_08748198 ACTOR_REC(08748198) = {
    .hitDuration = 18,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08747ED8,
};

/* ---- 0x08748764-0x087487E8: 3 record(s), section .actor_rec_08748764 ---- */
struct ActorDef gKingDededeDef ACTOR_REC(08748764) = {
    .health1Player = 100,
    .health2Players = 135,
    .health3Players = 160,
    .health4Players = 180,
    .score = 80000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748918,
    .attackBox = (u32)gKingDededeAttackBox,
    .terrainBox = (s32)gKingDededeTerrainBox,
    .terrainHandlers = (u32)&gKingDededeTerrainHandlers,
    .hitReactions = (u32)&gKingDededeHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gMrShineAndMrBrightDef ACTOR_REC(08748764) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 15000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748920,
    .attackBox = (u32)gUnk_0874883C,
    .terrainBox = (s32)gMrShineAndMrBrightTerrainBox,
    .terrainHandlers = (u32)&gUnk_0874894C,
    .hitReactions = (u32)&gMrShineAndMrBrightHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087487BC ACTOR_REC(08748764) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 15000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748928,
    .attackBox = (u32)gUnk_087488AC,
    .terrainBox = (s32)gUnk_08748910,
    .terrainHandlers = (u32)&gUnk_0874894C,
    .hitReactions = (u32)&gUnk_08748998,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08748918-0x08748930: 3 record(s), section .actor_rec_08748918 ---- */
struct ActorAux gUnk_08748918 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08748804,
};
struct ActorAux gUnk_08748920 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08748858,
};
struct ActorAux gUnk_08748928 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087488C8,
};

/* ---- 0x08748AB0-0x08748BB8: 6 record(s), section .actor_rec_08748ab0 ---- */
struct ActorDef gKingDededeStarDef ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gKingDededeStarAttackBox,
    .terrainBox = (s32)gKingDededeStarTerrainBox,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gKingDededeStarHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gKingDededeAirPuffDef ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gKingDededeAirPuffAttackBox,
    .terrainBox = (s32)gKingDededeAirPuffTerrainBox,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B08 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C60,
    .terrainBox = (s32)gUnk_08748CE0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_08748D10,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B34 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_CUTTER,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_08748D10,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B60 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C98,
    .terrainBox = (s32)gUnk_08748CE8,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_08748D1C,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B8C ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748CB4,
    .terrainBox = (s32)gUnk_08748CF0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_08748D1C,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08749514-0x08749598: 3 record(s), section .actor_rec_08749514 ---- */
struct ActorDef gMetaKnightDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 60000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AE8,
    .attackBox = (u32)gUnk_08749598,
    .terrainBox = (s32)gMetaKnightTerrainBox,
    .terrainHandlers = (u32)&gMetaKnightTerrainHandlers,
    .hitReactions = (u32)&gMetaKnightHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gKrackoDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 40000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AF0,
    .attackBox = (u32)gKrackoAttackBox,
    .terrainBox = (s32)gKrackoTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gKrackoHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gNightmareWizardDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 200000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AF8,
    .attackBox = (u32)gUnk_08749790,
    .terrainBox = 0,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gNightmareWizardHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x08749AE8-0x08749B08: 4 record(s), section .actor_rec_08749ae8 ---- */
struct ActorAux gUnk_08749AE8 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08749608,
};
struct ActorAux gUnk_08749AF0 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874973C,
};
struct ActorAux gUnk_08749AF8 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08749870,
};
struct ActorAux gUnk_08749B00 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874988C,
};

/* ---- 0x08749BEC-0x08749C70: 3 record(s), section .actor_rec_08749bec ---- */
struct ActorDef gMetaKnightSwordDef ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gMetaKnightSwordAttackBox,
    .terrainBox = (s32)gMetaKnightSwordTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gKrackoStarmanDef ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = ABILITY_HI_JUMP,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gKrackoStarmanAttackBox,
    .terrainBox = (s32)gUnk_08741B3C,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gKrackoStarmanHitReactions,
    .initCallback = NULL,
    .teardown = (void (*)(void))sub_080acc8c,
};
struct ActorDef gNightmareWizardStarDef ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gNightmareWizardStarAttackBox,
    .terrainBox = (s32)gNightmareWizardStarTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gNightmareWizardStarHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874B2DC-0x0874B38C: 4 record(s), section .actor_rec_0874b2dc ---- */
struct ActorDef gPaintRollerDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 30000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874B4C8,
    .attackBox = (u32)gPaintRollerAttackBox,
    .terrainBox = (s32)gPaintRollerTerrainBox,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gPaintRollerHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHeavyMoleDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 50000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874B4D0,
    .attackBox = (u32)gHeavyMoleAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gHeavyMoleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gNightmarePowerOrbDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 50000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gNightmarePowerOrbAttackBox,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gNightmarePowerOrbHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gNightmarePowerOrbStarDef ACTOR_REC(0874b2dc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874B4C8-0x0874B4D8: 2 record(s), section .actor_rec_0874b4c8 ---- */
struct ActorAux gUnk_0874B4C8 ACTOR_REC(0874b4c8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874B3C4,
};
struct ActorAux gUnk_0874B4D0 ACTOR_REC(0874b4c8) = {
    .hitDuration = 54,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874B418,
};

/* ---- 0x0874B940-0x0874BB7C: 13 record(s), section .actor_rec_0874b940 ---- */
struct ActorDef gPaintRollerPaintingDef ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B96C ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_WHEEL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = (u32)&gUnk_0874BF74,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B998 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = (u32)&gUnk_0874BF80,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B9C4 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = (u32)&gUnk_0874BF8C,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B9F0 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_MIKE,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = (u32)&gUnk_0874BF98,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA1C ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_BALL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0874BFA4,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA48 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_CRASH,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = (u32)&gUnk_0874BFB0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA74 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_SPARK,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0874BFBC,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BAA0 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_PARASOL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gUnk_0874BFC8,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gPaintRollerLightningDef ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gPaintRollerLightningAttackBox,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)&gUnk_0874BF58,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHeavyMoleArmDef ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHeavyMoleYellowMissileDef ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_HAMMER,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BF34,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gHeavyMoleYellowMissileHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gHeavyMoleRedMissileDef ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_SLEEP,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BF34,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)&gHeavyMoleRedMissileHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874C158-0x0874C184: 1 record(s), section .actor_rec_0874c158 ---- */
struct ActorDef gWhispyWoodsDef ACTOR_REC(0874c158) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 10000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874C1FC,
    .attackBox = (u32)gWhispyWoodsAttackBox,
    .terrainBox = (s32)gWhispyWoodsTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gWhispyWoodsHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};

/* ---- 0x0874C1FC-0x0874C204: 1 record(s), section .actor_rec_0874c1fc ---- */
struct ActorAux gUnk_0874C1FC ACTOR_REC(0874c1fc) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874C1BC,
};

/* ---- 0x0874C378-0x0874C3D0: 2 record(s), section .actor_rec_0874c378 ---- */
struct ActorDef gWhispyWoodsAppleDef ACTOR_REC(0874c378) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 1000,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gWhispyWoodsAppleAttackBox,
    .terrainBox = (s32)gWhispyWoodsAppleTerrainBox,
    .terrainHandlers = (u32)&gWhispyWoodsAppleTerrainHandlers,
    .hitReactions = (u32)&gWhispyWoodsAppleHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
struct ActorDef gWhispyWoodsAirPuffDef ACTOR_REC(0874c378) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = ABILITY_NORMAL,
    .isItem = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gWhispyWoodsAirPuffAttackBox,
    .terrainBox = (s32)gWhispyWoodsAirPuffTerrainBox,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)&gWhispyWoodsAirPuffHitReactions,
    .initCallback = NULL,
    .teardown = NULL,
};
