#include "global.h"
#include "task.h"
#include "actor.h"
#include "cutscene.h"
#include "enemy.h"

/* The actor definition tables (0x0873ECEC-0x0873EE9F, issue #36 phase 2):
 * one array of struct ActorDef pointers per actor kind.  ActorBindDefSlot
 * (src/actor_63698.c) binds a task to its definition with
 * table[Task.unk76] picked by Task.actorKind; the lengths are the spans
 * between the consumer-referenced labels (docs/data.md 5.1).  The records
 * stay structure-only data in seg 18 (actor_rodata).  The four
 * records that other files read as u32 arrays are cast.  Carved by
 * tools/carve_data.py. */

/* kind 0: the enemies, by subtype */
struct ActorDef *const gEnemyDefs[] = {
    (struct ActorDef *)gWaddleDeeDef,
    &gRockyDef,
    &gNoddyDef,
    &gBroomHatterDef,
    &gPengyDef,
    &gLaserBallDef,
    &gChillyDef,
    &gSirKibbleDef,
    &gCappyDef,
    (struct ActorDef *)gWaddleDooDef,
    &gGordoDef,
    &gCoolSpookDef,
    &gBrontoBurtDef,
    &gKabuDef,
    &gBomberDef,
    &gCoconutDef,
    &gTwizzyDef,
    (struct ActorDef *)gShotzoDef,
    &gSparkyDef,
    &gTwisterDef,
    &gSquishyDef,
    &gScarfyDef,
    &gBubblesDef,
    &gStarmanDef,
    &gHotHeadDef,
    &gGlunkDef,
    &gSlippyDef,
    &gBlipperDef,
    &gSwordKnightDef,
    &gBladeKnightDef,
    &gPoppyBrosJrDef,
    &gPoppyBrosJrOnAppleDef,
    &gPoppyBrosJrOnMaximTomatoDef,
    &gConerDef,
    &gWheelieDef,
    &gFlamerDef,
    &gNeedlousDef,
    &gUFODef,
    &gParasolDef,
    &gGipDef,
    &gBlockStarDef,
};

/* kinds 1 and 3: the mid-bosses */
struct ActorDef *const gMidBossDefs[] = {
    &gBonkersDef,
    &gPoppyBrosSrDef,
    &gGrandWheelieDef,
    &gMrFrostyDef,
    &gMrTickTockDef,
    &gBugzzyDef,
    &gFireLionDef,
    &gPhanPhanDef,
    NULL,
    NULL,
};

/* kind 2: the bosses (the subtype is also kept in gBossSubtype) */
struct ActorDef *const gBossDefs[] = {
    &gKingDededeDef,
    &gPaintRollerDef,
    &gMetaKnightDef,
    &gHeavyMoleDef,
    &gMrShineAndMrBrightDef,
    &gWhispyWoodsDef,
    &gKrackoDef,
    &gNightmarePowerOrbDef,
    &gNightmareWizardDef,
};

/* kind 4 */
struct ActorDef *const gChildActorDefs[] = {
    &gPengyIceBreathDef,
    &gLaserBallLaserDef,
    &gChillyFreezeDef,
    &gSirKibbleCutterDef,
    &gWaddleDooBeamDef,
    &gGlunkShotDef,
    &gHotHeadFireDef,
    &gShotzoCannonballDef,
    &gBonkersNutDef,
    &gPoppyBrosSrBombDef,
    &gKingDededeStarDef,
    &gKingDededeAirPuffDef,
    &gGrandWheelieMiniWheelieDef,
    &gMrFrostyIceCubeDef,
    &gPaintRollerPaintingDef,
    (struct ActorDef *)gPaintRollerLightningDef,
    &gMrTickTockRingDef,
    &gMrTickTockNoteDef,
    &gMetaKnightSwordDef,
    &gHeavyMoleArmDef,
    &gHeavyMoleYellowMissileDef,
    &gHeavyMoleRedMissileDef,
    &gWhispyWoodsAppleDef,
    &gWhispyWoodsAirPuffDef,
    &gUnk_08748B08,
    &gKrackoStarmanDef,
    &gAxeKnightAxeDef,
    &gMaceKnightMaceDef,
    &gTridentKnightTridentDef,
    &gJavelinKnightJavelinDef,
    &gBugzzyLadybugDef,
    &gUFOLaserDef,
    &gNightmarePowerOrbStarDef,
    &gNightmareWizardStarDef,
    &gGipStarDef,
    &gPhanPhanAppleDef,
    &gInhalableStarDef,
};

/* kind 5 */
struct ActorDef *const gObjectDefs[] = {
    &gWarpStarDef,
    &gCannonDef,
    &gCannonFuseDef,
    &gBigSwitchDef,
    &gStakeDef,
    &gUnk_0873F49C,
};

/* every other kind */
struct ActorDef *const gItemDefs[] = {
    &gAbilityStarDef,
    &gOneUpDef,
    &gMaximTomatoDef,
    &gInvincibleCandyDef,
    &gEnergyDrinkDef,
    &gUnk_0873F394,
};
