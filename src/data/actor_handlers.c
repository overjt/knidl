#include "global.h"
#include "task.h"
#include "actor.h"

/* The actor handler records of actor_rodata (issue #167): 62 struct
 * ActorHandlers tables and 136 struct ActorVt records (include/actor.h),
 * 261 function pointers, in 28 runs of adjacent records (one of them round 1's
 * row of gActorDrownTerrainHandlers, moved here from actor_tables.c), each run in address
 * order.
 *
 * - struct ActorHandlers (7 words) is what Actor.terrainHandlers points at:
 *   ActorCollideTerrain, ActorCollideTerrainInCameraBounds and ActorCollideTerrainFloor (src/actor_692fc.c)
 *   read all seven entries and call the first that claims the frame.
 * - struct ActorVt (12 bytes) is what Actor.hitReactions points at:
 *   ActorReactToDamage and ActorReactToDefeat (src/actor_692fc.c) read the
 *   two s8 modes and call unk04 / unk08 when the mode is -1.
 *
 * Every record here is pointed at by an ActorDef's terrainHandlers or
 * hitReactions field (src/data/actor_records.c; ActorLoadDef copies both
 * into the Actor, src/actor_63698.c) or by the code (ActorSetTerrainHandlers,
 * ActorSetHitReactions, enemy_88000.c's direct stores), the line above each
 * says which, and its span up to the next label is the record (two ActorVt
 * labels are followed by 12 unreferenced bytes, which stay structure-only).
 * Every word keeps its state: a handler is its function, 0 stays 0, the
 * ActorVt mode word is its bytes.  The four records include/actor.h declares
 * are one-element arrays, the view their callers pass by name.
 *
 * Each run is a named section .actor_tbl_<address>, listed by linker.ld
 * between the data pieces of actor_rodata inside ONE output section
 * (tools/ldgroup.py, docs/data.md 5.2).  The records are not const (the
 * qualifier would reach the setters' parameters); the section attribute is
 * what places them in ROM. */

#define ACTOR_TBL(addr) __attribute__((section(".actor_tbl_" #addr)))

/* Handlers whose prototype lives in a header this file does not include
 * (enemy.h, player.h, ...; enemy.h declares 29 of these records as
 * u32 [], the view its callers pass to ActorSetHitReactions, lesson
 * 3.517). */
s32 ParasolWaddleDeeReactToDefeat(void);
s32 WaddleDeeStartFall(void);
s32 WaddleDeeLand(void);
s32 WaddleDeeEnterWater(void);
s32 WaddleDeeHitWall(void);
s32 WaddleDeeHitCeiling(void);
s32 PengyStartFall(void);
s32 PengyLand(void);
s32 PengyEnterWater(void);
s32 BomberStartFall(void);
s32 BomberLand(void);
s32 BomberEnterWater(void);
s32 sub_08079a70(void);
s32 BomberBounceOffWall(void);
s32 SparkyStartFall(void);
s32 SparkyLand(void);
s32 SparkyEnterWater(void);
s32 SparkyBounceOffWall(void);
void SparkyHitCeiling(void);
s32 SwordAndBladeKnightStartFall(void);
s32 SwordAndBladeKnightLand(void);
s32 SwordAndBladeKnightEnterWater(void);
s32 sub_0807b058(void);
s32 SwordAndBladeKnightHitWall(void);
s32 NeedlousStartFall(void);
s32 NeedlousLand(void);
s32 NeedlousEnterWater(void);
s32 NeedlousHitWall(void);
s32 sub_0807bed4(void);
void NeedlousHitCeiling(void);
s32 UFOLaserReactToDamage(void);
s32 UFOLaserReactToDefeat(void);
s32 RockyLand(void);
s32 RockyStartFall(void);
s32 RockyHitCeiling(void);
s32 RockyHitWall(void);
s32 RockyEnterWater(void);
s32 sub_0807e4cc(void);
s32 SirKibbleStartFall(void);
s32 SirKibbleLand(void);
s32 SirKibbleEnterWater(void);
s32 CappyBounceOffWall(void);
s32 CappyEnterWater(void);
s32 KabuHitWall(void);
s32 KabuStartFall(void);
s32 KabuLand(void);
s32 KabuHitCeiling(void);
s32 KabuEnterWater(void);
s32 TwisterLand(void);
s32 TwisterStartFall(void);
void TwisterHitCeiling(void);
s32 TwisterEnterWater(void);
s32 HotHeadStartFall(void);
s32 HotHeadLand(void);
s32 HotHeadBounceOffWall(void);
s32 HotHeadEnterWater(void);
s32 sub_08081884(void);
s32 StarmanHitCeiling(void);
s32 StarmanStartFall(void);
s32 StarmanLand(void);
s32 StarmanHitWall(void);
s32 StarmanEnterWater(void);
void PoppyBrosJrHitCeiling(void);
s32 PoppyBrosJrStartFall(void);
s32 PoppyBrosJrHitWall(void);
s32 PoppyBrosJrEnterWater(void);
s32 PoppyBrosJrRiderStartFall(void);
s32 PoppyBrosJrRiderLand(void);
s32 PoppyBrosJrRiderBounceOffWall(void);
s32 PoppyBrosJrRiderEnterWater(void);
s32 WheelieStartFall(void);
s32 WheelieHitWall(void);
s32 sub_08082db0(void);
s32 WheelieEnterWater(void);
u8 FlamerEnterWater(void);
u8 NoddyLand(void);
u8 NoddyStartFall(void);
u8 NoddyEnterWater(void);
s32 NoddyBounceOffWall(void);
u8 ChillyLand(void);
u8 ChillyStartFall(void);
u8 ChillyEnterWater(void);
s32 ChillyHitWall(void);
u8 WaddleDooLand(void);
u8 WaddleDooStartFall(void);
u8 WaddleDooEnterWater(void);
s32 WaddleDooHitCeiling(void);
s32 WaddleDooHitWall(void);
void ParasolWaddleDooReactToDefeat(void);
s32 TwizzyLand(void);
s32 TwizzyStartFall(void);
s32 TwizzyHitWall(void);
s32 TwizzyHitCeiling(void);
s32 SquishyLand(void);
s32 SquishyStartFall(void);
s32 SquishyEnterWater(void);
s32 SquishyLeaveWater(void);
s32 SquishyBounceOffWall(void);
s32 SquishyHitCeiling(void);
s32 sub_080896ec(void);
s32 BubblesStartFall(void);
s32 BubblesEnterWater(void);
s32 BubblesHitWall(void);
s32 BubblesHitCeiling(void);
s32 GlunkLand(void);
s32 GlunkStartFall(void);
s32 GlunkEnterWater(void);
s32 SlippyLand(void);
s32 SlippyStartFall(void);
s32 SlippyEnterWater(void);
s32 SlippyHitWall(void);
s32 SlippyHitCeiling(void);
s32 BlipperLand(void);
s32 BlipperStartFall(void);
s32 BlipperHitWall(void);
s32 BlipperHitCeiling(void);
s32 GipLand(void);
s32 GipStartFall(void);
s32 GipEnterWater(void);
s32 GipHitWall(void);
s32 GipHitCeiling(void);
void WaddleDooBeamHitTerrain(void);
void GlunkShotHitCeiling(void);
s32 BroomHatterStartFall(void);
s32 BroomHatterLand(void);
s32 BroomHatterEnterWater(void);
s32 sub_0808d388(void);
s32 CoconutLand(void);
s32 CoconutEnterWater(void);
s32 ShotzoStartFall(void);
s32 ShotzoLand(void);
s32 ShotzoHitWall(void);
s32 ShotzoEnterWater(void);
s32 ParasolShotzoReactToDefeat(void);
s32 ConerStartFall(void);
s32 ConerLand(void);
s32 ConerEnterWater(void);
s32 ConerBounceOffWall(void);
s32 BonkersReactToDamage(void);
s32 BonkersReactToDefeat(void);
s32 BonkersHitWall(void);
s32 PoppyBrosSrReactToDamage(void);
s32 PoppyBrosSrReactToDefeat(void);
s32 PoppyBrosSrHitWall(void);
s32 BugzzyHitWall(void);
void BugzzyHitCeiling(void);
s32 BugzzyReactToDamage(void);
s32 BugzzyReactToDefeat(void);
s32 BonkersNutHitWall(void);
s32 PoppyBrosSrBombHitWall(void);
s32 PoppyBrosSrBombLand(void);
s32 GrandWheelieLand(void);
s32 GrandWheelieHitWall(void);
void GrandWheelieReactToDamage(void);
void GrandWheelieReactToDefeat(void);
s32 FireLionLand(void);
s32 FireLionHitWall(void);
s32 FireLionReactToDamage(void);
s32 FireLionReactToDefeat(void);
s32 PhanPhanLand(void);
s32 PhanPhanHitWall(void);
s32 PhanPhanReactToDamage(void);
s32 PhanPhanReactToDefeat(void);
void GrandWheelieMiniWheelieLand(void);
void GrandWheelieMiniWheelieEnterWater(void);
void GrandWheelieMiniWheelieHitWall(void);
void GrandWheelieMiniWheelieHitCeiling(void);
s32 PhanPhanAppleLand(void);
s32 PhanPhanAppleHitWall(void);
s32 MrFrostyStartFall(void);
u8 MrFrostyLand(void);
u8 MrFrostyHitWall(void);
u8 MrFrostyReactToDefeat(void);
u8 MrFrostyReactToDamage(void);
u8 MrTickTockStartFall(void);
u8 MrTickTockLand(void);
u8 MrTickTockHitWall(void);
u8 MrTickTockReactToDamage(void);
u8 MrTickTockReactToDefeat(void);
u8 MrFrostyIceCubeLand(void);
void MrFrostyIceCubeStartFall(void);
u8 MrFrostyIceCubeHitWall(void);
u8 MrTickTockNoteLand(void);
u8 MrTickTockNoteHitWall(void);
u8 AxeKnightLand(void);
u8 AxeKnightHitWall(void);
u8 AxeKnightHitCeiling(void);
u8 JavelinKnightLand(void);
u8 JavelinKnightHitWall(void);
u8 JavelinKnightHitCeiling(void);
u8 MaceKnightLand(void);
u8 MaceKnightStartFall(void);
u8 MaceKnightHitWall(void);
u8 MaceKnightHitCeiling(void);
u8 TridentKnightLand(void);
u8 TridentKnightHitWall(void);
u8 TridentKnightHitCeiling(void);
void MetaKnightsKnightReactToDamage(void);
u8 KingDededeReactToDefeat(void);
u8 KingDededeReactToDamage(void);
u8 KingDededeLand(void);
void KingDededeHitWall(void);
u8 KingDededeHitCeiling(void);
s32 MrShineAndMrBrightLand(void);
s32 MrShineAndMrBrightHitWall(void);
s32 MrShineAndMrBrightReactToDefeat(void);
s32 MrShineAndMrBrightReactToDamage(void);
s32 MetaKnightReactToDamage(void);
s32 MetaKnightReactToDefeat(void);
s32 MetaKnightHitWall(void);
s32 KrackoReactToDamage(void);
s32 KrackoReactToDefeat(void);
s32 NightmareWizardReactToDamage(void);
s32 NightmareWizardReactToDefeat(void);
void PaintRollerReactToDamage(void);
s32 PaintRollerReactToDefeat(void);
void HeavyMoleReactToDamage(void);
s32 HeavyMoleReactToDefeat(void);
void NightmarePowerOrbReactToDamage(void);
void NightmarePowerOrbReactToDefeat(void);
s32 PaintRollerPaintingLand(void);
s32 PaintRollerPaintingStartFall(void);
s32 PaintRollerPaintingHitWall(void);
void WhispyWoodsReactToDefeat(void);
void WhispyWoodsReactToDamage(void);
s32 WhispyWoodsAppleLand(void);
s32 PickupStartFall(void);
s32 PickupLand(void);
s32 PickupEnterWater(void);
s32 AbilityStarBounceOffFloor(void);
s32 AbilityStarEnterWater(void);
s32 AbilityStarBounceOffWall(void);
s32 AbilityStarHitCeiling(void);

/* ---- 0x0873F5FC-0x0873F664: 6 record(s), section .actor_tbl_0873f5fc ---- */
/* gAbilityStarDef */
struct ActorHandlers gAbilityStarTerrainHandlers ACTOR_TBL(0873f5fc) = {
    .landCallback = (u32)AbilityStarBounceOffFloor,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)AbilityStarEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)AbilityStarBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)AbilityStarHitCeiling,
};
/* 4 ActorDefs (gOneUpDef, gMaximTomatoDef, ...) */
struct ActorHandlers gPickupTerrainHandlers ACTOR_TBL(0873f5fc) = {
    .landCallback = (u32)PickupLand,
    .leaveGroundCallback = (u32)PickupStartFall,
    .enterWaterCallback = (u32)PickupEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gAbilityStarDef */
struct ActorVt gAbilityStarHitReactions ACTOR_TBL(0873f5fc) = {
    .damageKind = 0,
    .defeatKind = 5,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* 4 ActorDefs (gOneUpDef, gMaximTomatoDef, ...) */
struct ActorVt gUnk_0873F640 ACTOR_TBL(0873f5fc) = {
    .damageKind = 0,
    .defeatKind = 10,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gItemDef5 */
struct ActorVt gItemDef5HitReactions ACTOR_TBL(0873f5fc) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBigSwitchDef */
struct ActorVt gBigSwitchHitReactions ACTOR_TBL(0873f5fc) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x0873F8F4-0x0873F910: 1 record(s), section .actor_tbl_0873f8f4 ---- */
/* 19 ActorDefs (gCannonDef, gCannonFuseDef, ...); src/actor_6b2e4.c, src/actor_6c2a4.c */
struct ActorHandlers gNullTerrainHandlers[] ACTOR_TBL(0873f8f4) = { {
        .landCallback = 0,
        .leaveGroundCallback = 0,
        .enterWaterCallback = 0,
        .leaveWaterCallback = 0,
        .hitWallCallback = 0,
        .unk14 = 0,
        .hitCeilingCallback = 0,
} };

/* ---- 0x0873F910-0x0873F92C: 1 record(s), section .actor_tbl_0873f910 ---- */
/* src/actor_692fc.c */
struct ActorHandlers gActorDrownTerrainHandlers[] ACTOR_TBL(0873f910) = { {
        .landCallback = (u32)ActorDrownLand,
        .leaveGroundCallback = 0,
        .enterWaterCallback = 0,
        .leaveWaterCallback = 0,
        .hitWallCallback = 0,
        .unk14 = 0,
        .hitCeilingCallback = 0,
} };

/* ---- 0x0873F92C-0x0873F950: 3 record(s), section .actor_tbl_0873f92c ---- */
/* src/actor_6b2e4.c */
struct ActorVt gUnk_0873F92C[] ACTOR_TBL(0873f92c) = { {
        .damageKind = 0,
        .defeatKind = 4,
        .filler02 = { 0x00, 0x00 },
        .damageCallback = 0,
        .defeatCallback = 0,
} };
/* src/actor_6b2e4.c */
struct ActorVt gUnk_0873F938[] ACTOR_TBL(0873f92c) = { {
        .damageKind = 0,
        .defeatKind = 0,
        .filler02 = { 0x00, 0x00 },
        .damageCallback = 0,
        .defeatCallback = 0,
} };
/* gInhalableStarDef, gUnk_0873F690 */
struct ActorVt gUnk_0873F944 ACTOR_TBL(0873f92c) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08740E54-0x08740FB0: 21 record(s), section .actor_tbl_08740e54 ---- */
/* gWaddleDeeDef, gParasolWaddleDeeDef */
struct ActorHandlers gWaddleDeeTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)WaddleDeeLand,
    .leaveGroundCallback = (u32)WaddleDeeStartFall,
    .enterWaterCallback = (u32)WaddleDeeEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)WaddleDeeHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)WaddleDeeHitCeiling,
};
/* gPengyDef */
struct ActorHandlers gPengyTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)PengyLand,
    .leaveGroundCallback = (u32)PengyStartFall,
    .enterWaterCallback = (u32)PengyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gBomberDef */
struct ActorHandlers gBomberTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)BomberLand,
    .leaveGroundCallback = (u32)BomberStartFall,
    .enterWaterCallback = (u32)BomberEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BomberBounceOffWall,
    .unk14 = (u32)sub_08079a70,
    .hitCeilingCallback = 0,
};
/* gSparkyDef */
struct ActorHandlers gSparkyTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)SparkyLand,
    .leaveGroundCallback = (u32)SparkyStartFall,
    .enterWaterCallback = (u32)SparkyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)SparkyBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)SparkyHitCeiling,
};
/* gSwordKnightDef, gBladeKnightDef */
struct ActorHandlers gSwordAndBladeKnightTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)SwordAndBladeKnightLand,
    .leaveGroundCallback = (u32)SwordAndBladeKnightStartFall,
    .enterWaterCallback = (u32)SwordAndBladeKnightEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)SwordAndBladeKnightHitWall,
    .unk14 = (u32)sub_0807b058,
    .hitCeilingCallback = 0,
};
/* gNeedlousDef */
struct ActorHandlers gNeedlousTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)NeedlousLand,
    .leaveGroundCallback = (u32)NeedlousStartFall,
    .enterWaterCallback = (u32)NeedlousEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)NeedlousHitWall,
    .unk14 = (u32)sub_0807bed4,
    .hitCeilingCallback = (u32)NeedlousHitCeiling,
};
/* gWaddleDeeDef */
struct ActorVt gWaddleDeeHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gParasolWaddleDeeDef */
struct ActorVt gParasolWaddleDeeHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = (u32)ParasolWaddleDeeReactToDefeat,
};
/* gPengyDef */
struct ActorVt gPengyHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBomberDef */
struct ActorVt gBomberHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_78b68.c */
struct ActorVt gBomberExplodeHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 2,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gSparkyDef */
struct ActorVt gSparkyHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gScarfyDef */
struct ActorVt gScarfyHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gScarfyTransformHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gScarfyExplodeHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gSwordKnightDef, gBladeKnightDef */
struct ActorVt gUnk_08740F68 ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBlockStarDef */
struct ActorVt gBlockStarHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gNeedlousDef */
struct ActorVt gNeedlousHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUFODef */
struct ActorVt gUFOHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gParasolDef */
struct ActorVt gParasolHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gParasolChaseHitReactions ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x087411B4-0x087411C0: 1 record(s), section .actor_tbl_087411b4 ---- */
/* gUFOLaserDef */
struct ActorVt gUFOLaserHitReactions ACTOR_TBL(087411b4) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)UFOLaserReactToDamage,
    .defeatCallback = (u32)UFOLaserReactToDefeat,
};

/* ---- 0x08741B64-0x08741D64: 28 record(s), section .actor_tbl_08741b64 ---- */
/* gRockyDef */
struct ActorHandlers gRockyTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)RockyLand,
    .leaveGroundCallback = (u32)RockyStartFall,
    .enterWaterCallback = (u32)RockyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)RockyHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)RockyHitCeiling,
};
/* gSirKibbleDef */
struct ActorHandlers gSirKibbleTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)SirKibbleLand,
    .leaveGroundCallback = (u32)SirKibbleStartFall,
    .enterWaterCallback = (u32)SirKibbleEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807e4cc,
    .unk14 = (u32)sub_0807e4cc,
    .hitCeilingCallback = 0,
};
/* gCappyDef, gCappyCaplessDef */
struct ActorHandlers gCappyTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)CappyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)CappyBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gKabuDef */
struct ActorHandlers gKabuTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)KabuLand,
    .leaveGroundCallback = (u32)KabuStartFall,
    .enterWaterCallback = (u32)KabuEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)KabuHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)KabuHitCeiling,
};
/* gTwisterDef */
struct ActorHandlers gTwisterTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)TwisterLand,
    .leaveGroundCallback = (u32)TwisterStartFall,
    .enterWaterCallback = (u32)TwisterEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = (u32)TwisterHitCeiling,
};
/* gStarmanDef */
struct ActorHandlers gStarmanTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)StarmanLand,
    .leaveGroundCallback = (u32)StarmanStartFall,
    .enterWaterCallback = (u32)StarmanEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)StarmanHitWall,
    .unk14 = (u32)sub_08081884,
    .hitCeilingCallback = (u32)StarmanHitCeiling,
};
/* gHotHeadDef */
struct ActorHandlers gHotHeadTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)HotHeadLand,
    .leaveGroundCallback = (u32)HotHeadStartFall,
    .enterWaterCallback = (u32)HotHeadEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)HotHeadBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosJrDef */
struct ActorHandlers gPoppyBrosJrTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = (u32)PoppyBrosJrStartFall,
    .enterWaterCallback = (u32)PoppyBrosJrEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PoppyBrosJrHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)PoppyBrosJrHitCeiling,
};
/* 4 ActorDefs (gPoppyBrosJrOnAppleDef, gPoppyBrosJrOnMaximTomatoDef, ...) */
struct ActorHandlers gPoppyBrosJrRiderTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)PoppyBrosJrRiderLand,
    .leaveGroundCallback = (u32)PoppyBrosJrRiderStartFall,
    .enterWaterCallback = (u32)PoppyBrosJrRiderEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PoppyBrosJrRiderBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gWheelieDef */
struct ActorHandlers gWheelieTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = (u32)WheelieStartFall,
    .enterWaterCallback = (u32)WheelieEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)WheelieHitWall,
    .unk14 = (u32)sub_08082db0,
    .hitCeilingCallback = 0,
};
/* gFlamerDef */
struct ActorHandlers gFlamerTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)FlamerEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gRockyDef */
struct ActorVt gRockyHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gSirKibbleDef */
struct ActorVt gSirKibbleHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gCappyDef */
struct ActorVt gCappyHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gCappyCaplessDef */
struct ActorVt gCappyCaplessHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gCoolSpookDef */
struct ActorVt gCoolSpookHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gKabuDef */
struct ActorVt gKabuHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_7f044.c */
struct ActorVt gKabuTeleportHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gTwisterDef */
struct ActorVt gTwisterHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gStarmanDef */
struct ActorVt gStarmanHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gHotHeadDef */
struct ActorVt gHotHeadHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosJrDef */
struct ActorVt gPoppyBrosJrHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosJrOnAppleDef */
struct ActorVt gPoppyBrosJrOnAppleHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosJrOnMaximTomatoDef */
struct ActorVt gPoppyBrosJrOnMaximTomatoHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosJrAppleDef */
struct ActorVt gPoppyBrosJrAppleHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosJrMaximTomatoDef */
struct ActorVt gPoppyBrosJrMaximTomatoHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gWheelieDef */
struct ActorVt gWheelieHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gFlamerDef */
struct ActorVt gFlamerHitReactions ACTOR_TBL(08741b64) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08741F4C-0x08741F70: 3 record(s), section .actor_tbl_08741f4c ---- */
/* gSirKibbleCutterDef */
struct ActorVt gSirKibbleCutterHitReactions ACTOR_TBL(08741f4c) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gHotHeadFireDef */
struct ActorVt gHotHeadFireHitReactions ACTOR_TBL(08741f4c) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_82e68.c */
struct ActorVt gHotHeadFireBallUpdateHitReactions ACTOR_TBL(08741f4c) = {
    .damageKind = 0,
    .defeatKind = 9,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08742CF0-0x08742EA4: 23 record(s), section .actor_tbl_08742cf0 ---- */
/* gNoddyDef */
struct ActorHandlers gNoddyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)NoddyLand,
    .leaveGroundCallback = (u32)NoddyStartFall,
    .enterWaterCallback = (u32)NoddyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)NoddyBounceOffWall,
    .unk14 = (u32)NoddyBounceOffWall,
    .hitCeilingCallback = 0,
};
/* gChillyDef */
struct ActorHandlers gChillyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)ChillyLand,
    .leaveGroundCallback = (u32)ChillyStartFall,
    .enterWaterCallback = (u32)ChillyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)ChillyHitWall,
    .unk14 = (u32)ChillyHitWall,
    .hitCeilingCallback = 0,
};
/* gWaddleDooDef, gParasolWaddleDooDef */
struct ActorHandlers gWaddleDooTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)WaddleDooLand,
    .leaveGroundCallback = (u32)WaddleDooStartFall,
    .enterWaterCallback = (u32)WaddleDooEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)WaddleDooHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)WaddleDooHitCeiling,
};
/* gTwizzyDef */
struct ActorHandlers gTwizzyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)TwizzyLand,
    .leaveGroundCallback = (u32)TwizzyStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)TwizzyHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)TwizzyHitCeiling,
};
/* gSquishyDef */
struct ActorHandlers gSquishyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)SquishyLand,
    .leaveGroundCallback = (u32)SquishyStartFall,
    .enterWaterCallback = (u32)SquishyEnterWater,
    .leaveWaterCallback = (u32)SquishyLeaveWater,
    .hitWallCallback = (u32)SquishyBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)SquishyHitCeiling,
};
/* gBubblesDef */
struct ActorHandlers gBubblesTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_080896ec,
    .leaveGroundCallback = (u32)BubblesStartFall,
    .enterWaterCallback = (u32)BubblesEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BubblesHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)BubblesHitCeiling,
};
/* gGlunkDef */
struct ActorHandlers gGlunkTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)GlunkLand,
    .leaveGroundCallback = (u32)GlunkStartFall,
    .enterWaterCallback = (u32)GlunkEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gSlippyDef */
struct ActorHandlers gSlippyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)SlippyLand,
    .leaveGroundCallback = (u32)SlippyStartFall,
    .enterWaterCallback = (u32)SlippyEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)SlippyHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)SlippyHitCeiling,
};
/* gBlipperDef */
struct ActorHandlers gBlipperTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)BlipperLand,
    .leaveGroundCallback = (u32)BlipperStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BlipperHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)BlipperHitCeiling,
};
/* gGipDef */
struct ActorHandlers gGipTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)GipLand,
    .leaveGroundCallback = (u32)GipStartFall,
    .enterWaterCallback = (u32)GipEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)GipHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)GipHitCeiling,
};
/* gNoddyDef */
struct ActorVt gNoddyHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gChillyDef */
struct ActorVt gChillyHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gWaddleDooDef */
struct ActorVt gWaddleDooHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gParasolWaddleDooDef */
struct ActorVt gParasolWaddleDooHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = (u32)ParasolWaddleDooReactToDefeat,
};
/* gBrontoBurtDef */
struct ActorVt gBrontoBurtHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gTwizzyDef */
struct ActorVt gTwizzyHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gSquishyDef; src/enemy_88000.c */
struct ActorVt gSquishyHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_88000.c */
struct ActorVt gSquishyInWaterHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBubblesDef */
struct ActorVt gBubblesHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gGlunkDef */
struct ActorVt gGlunkHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gSlippyDef */
struct ActorVt gSlippyHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBlipperDef */
struct ActorVt gBlipperHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gGipDef */
struct ActorVt gGipHitReactions ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x087430EC-0x0874313C: 4 record(s), section .actor_tbl_087430ec ---- */
/* gWaddleDooBeamDef */
struct ActorHandlers gWaddleDooBeamTerrainHandlers ACTOR_TBL(087430ec) = {
    .landCallback = (u32)WaddleDooBeamHitTerrain,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)WaddleDooBeamHitTerrain,
    .unk14 = 0,
    .hitCeilingCallback = (u32)WaddleDooBeamHitTerrain,
};
/* gGlunkShotDef */
struct ActorHandlers gGlunkShotTerrainHandlers ACTOR_TBL(087430ec) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = (u32)GlunkShotHitCeiling,
};
/* gGlunkShotDef */
struct ActorVt gGlunkShotHitReactions ACTOR_TBL(087430ec) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gGipStarDef */
struct ActorVt gGipStarHitReactions ACTOR_TBL(087430ec) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x087434C4-0x08743588: 11 record(s), section .actor_tbl_087434c4 ---- */
/* gBroomHatterDef */
struct ActorHandlers gBroomHatterTerrainHandlers ACTOR_TBL(087434c4) = {
    .landCallback = (u32)BroomHatterLand,
    .leaveGroundCallback = (u32)BroomHatterStartFall,
    .enterWaterCallback = (u32)BroomHatterEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = (u32)sub_0808d388,
    .hitCeilingCallback = 0,
};
/* gCoconutDef */
struct ActorHandlers gCoconutTerrainHandlers ACTOR_TBL(087434c4) = {
    .landCallback = (u32)CoconutLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)CoconutEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gShotzoDef, gParasolShotzoDef */
struct ActorHandlers gShotzoTerrainHandlers ACTOR_TBL(087434c4) = {
    .landCallback = (u32)ShotzoLand,
    .leaveGroundCallback = (u32)ShotzoStartFall,
    .enterWaterCallback = (u32)ShotzoEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)ShotzoHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gConerDef */
struct ActorHandlers gConerTerrainHandlers ACTOR_TBL(087434c4) = {
    .landCallback = (u32)ConerLand,
    .leaveGroundCallback = (u32)ConerStartFall,
    .enterWaterCallback = (u32)ConerEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)ConerBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gBroomHatterDef */
struct ActorVt gBroomHatterHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gLaserBallDef */
struct ActorVt gLaserBallHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gCoconutDef */
struct ActorVt gCoconutHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_8e404.c */
struct ActorVt gCoconutExplodeHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gShotzoDef */
struct ActorVt gShotzoHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gConerDef */
struct ActorVt gConerHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gParasolShotzoDef */
struct ActorVt gParasolShotzoHitReactions ACTOR_TBL(087434c4) = {
    .damageKind = 0,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = (u32)ParasolShotzoReactToDefeat,
};

/* ---- 0x087436E4-0x08743734: 4 record(s), section .actor_tbl_087436e4 ---- */
/* gLaserBallLaserDef */
struct ActorHandlers gLaserBallLaserTerrainHandlers ACTOR_TBL(087436e4) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gShotzoCannonballDef */
struct ActorHandlers gShotzoCannonballTerrainHandlers ACTOR_TBL(087436e4) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gLaserBallLaserDef */
struct ActorVt gLaserBallLaserHitReactions ACTOR_TBL(087436e4) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gShotzoCannonballDef */
struct ActorVt gShotzoCannonballHitReactions ACTOR_TBL(087436e4) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x0874407C-0x08744118: 9 record(s), section .actor_tbl_0874407c ---- */
/* gBonkersDef */
struct ActorHandlers gBonkersTerrainHandlers ACTOR_TBL(0874407c) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BonkersHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosSrDef */
struct ActorHandlers gPoppyBrosSrTerrainHandlers ACTOR_TBL(0874407c) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PoppyBrosSrHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gBugzzyDef */
struct ActorHandlers gBugzzyTerrainHandlers ACTOR_TBL(0874407c) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BugzzyHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)BugzzyHitCeiling,
};
/* gBonkersDef */
struct ActorVt gBonkersHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)BonkersReactToDamage,
    .defeatCallback = (u32)BonkersReactToDefeat,
};
/* src/enemy_9000c.c */
struct ActorVt gBonkersDefeatedHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)BonkersReactToDamage,
    .defeatCallback = 0,
};
/* gPoppyBrosSrDef */
struct ActorVt gPoppyBrosSrHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)PoppyBrosSrReactToDamage,
    .defeatCallback = (u32)PoppyBrosSrReactToDefeat,
};
/* src/enemy_9113c.c */
struct ActorVt gPoppyBrosSrDefeatedHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)PoppyBrosSrReactToDamage,
    .defeatCallback = 0,
};
/* gBugzzyDef */
struct ActorVt gBugzzyHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)BugzzyReactToDamage,
    .defeatCallback = (u32)BugzzyReactToDefeat,
};
/* src/enemy_91f9c.c */
struct ActorVt gBugzzyDefeatedHitReactions ACTOR_TBL(0874407c) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)BugzzyReactToDamage,
    .defeatCallback = 0,
};

/* ---- 0x087442C8-0x0874433C: 7 record(s), section .actor_tbl_087442c8 ---- */
/* gBonkersNutDef */
struct ActorHandlers gBonkersNutTerrainHandlers ACTOR_TBL(087442c8) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)BonkersNutHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosSrBombDef */
struct ActorHandlers gPoppyBrosSrBombTerrainHandlers ACTOR_TBL(087442c8) = {
    .landCallback = (u32)PoppyBrosSrBombLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PoppyBrosSrBombHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gBonkersNutDef */
struct ActorVt gBonkersNutHitReactions ACTOR_TBL(087442c8) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_91f9c.c */
struct ActorVt gBonkersNutDieHitReactions ACTOR_TBL(087442c8) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPoppyBrosSrBombDef */
struct ActorVt gPoppyBrosSrBombHitReactions ACTOR_TBL(087442c8) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_91f9c.c */
struct ActorVt gPoppyBrosSrBombDieHitReactions ACTOR_TBL(087442c8) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gBugzzyLadybugDef */
struct ActorVt gBugzzyLadybugHitReactions ACTOR_TBL(087442c8) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x087453BC-0x08745458: 9 record(s), section .actor_tbl_087453bc ---- */
/* gGrandWheelieDef */
struct ActorHandlers gGrandWheelieTerrainHandlers ACTOR_TBL(087453bc) = {
    .landCallback = (u32)GrandWheelieLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)GrandWheelieHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gFireLionDef */
struct ActorHandlers gFireLionTerrainHandlers ACTOR_TBL(087453bc) = {
    .landCallback = (u32)FireLionLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)FireLionHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPhanPhanDef */
struct ActorHandlers gPhanPhanTerrainHandlers ACTOR_TBL(087453bc) = {
    .landCallback = (u32)PhanPhanLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PhanPhanHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gGrandWheelieDef */
struct ActorVt gGrandWheelieHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)GrandWheelieReactToDamage,
    .defeatCallback = (u32)GrandWheelieReactToDefeat,
};
/* src/enemy_93f64.c */
struct ActorVt gGrandWheelieDefeatedHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)GrandWheelieReactToDamage,
    .defeatCallback = 0,
};
/* gFireLionDef */
struct ActorVt gFireLionHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)FireLionReactToDamage,
    .defeatCallback = (u32)FireLionReactToDefeat,
};
/* src/enemy_957bc.c */
struct ActorVt gFireLionDefeatedHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)FireLionReactToDamage,
    .defeatCallback = 0,
};
/* gPhanPhanDef */
struct ActorVt gPhanPhanHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)PhanPhanReactToDamage,
    .defeatCallback = (u32)PhanPhanReactToDefeat,
};
/* src/enemy_974c8.c */
struct ActorVt gPhanPhanDefeatedHitReactions ACTOR_TBL(087453bc) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)PhanPhanReactToDamage,
    .defeatCallback = 0,
};

/* ---- 0x087455C8-0x08745618: 4 record(s), section .actor_tbl_087455c8 ---- */
/* gGrandWheelieMiniWheelieDef */
struct ActorHandlers gGrandWheelieMiniWheelieTerrainHandlers ACTOR_TBL(087455c8) = {
    .landCallback = (u32)GrandWheelieMiniWheelieLand,
    .leaveGroundCallback = (u32)GrandWheelieMiniWheelieLand,
    .enterWaterCallback = (u32)GrandWheelieMiniWheelieEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)GrandWheelieMiniWheelieHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)GrandWheelieMiniWheelieHitCeiling,
};
/* gPhanPhanAppleDef */
struct ActorHandlers gPhanPhanAppleTerrainHandlers ACTOR_TBL(087455c8) = {
    .landCallback = (u32)PhanPhanAppleLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PhanPhanAppleHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gGrandWheelieMiniWheelieDef */
struct ActorVt gGrandWheelieMiniWheelieHitReactions ACTOR_TBL(087455c8) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gPhanPhanAppleDef */
struct ActorVt gPhanPhanAppleHitReactions ACTOR_TBL(087455c8) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08745A3C-0x08745AA4: 6 record(s), section .actor_tbl_08745a3c ---- */
/* gMrFrostyDef */
struct ActorHandlers gMrFrostyTerrainHandlers ACTOR_TBL(08745a3c) = {
    .landCallback = (u32)MrFrostyLand,
    .leaveGroundCallback = (u32)MrFrostyStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MrFrostyHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gMrTickTockDef */
struct ActorHandlers gMrTickTockTerrainHandlers ACTOR_TBL(08745a3c) = {
    .landCallback = (u32)MrTickTockLand,
    .leaveGroundCallback = (u32)MrTickTockStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MrTickTockHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gMrFrostyDef */
struct ActorVt gMrFrostyHitReactions ACTOR_TBL(08745a3c) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrFrostyReactToDamage,
    .defeatCallback = (u32)MrFrostyReactToDefeat,
};
/* src/enemy_988f8.c */
struct ActorVt gMrFrostyDefeatedHitReactions ACTOR_TBL(08745a3c) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrFrostyReactToDamage,
    .defeatCallback = 0,
};
/* gMrTickTockDef */
struct ActorVt gMrTickTockHitReactions ACTOR_TBL(08745a3c) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrTickTockReactToDamage,
    .defeatCallback = (u32)MrTickTockReactToDefeat,
};
/* src/enemy_99b20.c */
struct ActorVt gMrTickTockDefeatedHitReactions ACTOR_TBL(08745a3c) = {
    .damageKind = -1,
    .defeatKind = 6,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrTickTockReactToDamage,
    .defeatCallback = 0,
};

/* ---- 0x08745C90-0x08745CD4: 3 record(s), section .actor_tbl_08745c90 ---- */
/* gMrFrostyIceCubeDef */
struct ActorHandlers gMrFrostyIceCubeTerrainHandlers ACTOR_TBL(08745c90) = {
    .landCallback = (u32)MrFrostyIceCubeLand,
    .leaveGroundCallback = (u32)MrFrostyIceCubeStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MrFrostyIceCubeHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gMrTickTockNoteDef */
struct ActorHandlers gMrTickTockNoteTerrainHandlers ACTOR_TBL(08745c90) = {
    .landCallback = (u32)MrTickTockNoteLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MrTickTockNoteHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gMrFrostyIceCubeDef; the 12 bytes after it, up to the next label, have no reference and stay structure-only data */
struct ActorVt gMrFrostyIceCubeHitReactions ACTOR_TBL(08745c90) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08745CE0-0x08745CEC: 1 record(s), section .actor_tbl_08745ce0 ---- */
/* gMrTickTockNoteDef */
struct ActorVt gMrTickTockNoteHitReactions ACTOR_TBL(08745ce0) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x087481A0-0x08748264: 11 record(s), section .actor_tbl_087481a0 ---- */
/* gAxeKnightDef */
struct ActorHandlers gAxeKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)AxeKnightLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)AxeKnightHitWall,
    .unk14 = (u32)AxeKnightHitWall,
    .hitCeilingCallback = (u32)AxeKnightHitCeiling,
};
/* gMaceKnightDef */
struct ActorHandlers gMaceKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)MaceKnightLand,
    .leaveGroundCallback = (u32)MaceKnightStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MaceKnightHitWall,
    .unk14 = (u32)MaceKnightHitWall,
    .hitCeilingCallback = (u32)MaceKnightHitCeiling,
};
/* gTridentKnightDef */
struct ActorHandlers gTridentKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)TridentKnightLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)TridentKnightHitWall,
    .unk14 = (u32)TridentKnightHitWall,
    .hitCeilingCallback = (u32)TridentKnightHitCeiling,
};
/* gJavelinKnightDef */
struct ActorHandlers gJavelinKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)JavelinKnightLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)JavelinKnightHitWall,
    .unk14 = (u32)JavelinKnightHitWall,
    .hitCeilingCallback = (u32)JavelinKnightHitCeiling,
};
/* gAxeKnightDef */
struct ActorVt gAxeKnightHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = -1,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MetaKnightsKnightReactToDamage,
    .defeatCallback = 0,
};
/* gAxeKnightAxeDef */
struct ActorVt gAxeKnightAxeHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gMaceKnightDef */
struct ActorVt gMaceKnightHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = -1,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MetaKnightsKnightReactToDamage,
    .defeatCallback = 0,
};
/* gTridentKnightDef */
struct ActorVt gTridentKnightHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = -1,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MetaKnightsKnightReactToDamage,
    .defeatCallback = 0,
};
/* gTridentKnightTridentDef */
struct ActorVt gTridentKnightTridentHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gJavelinKnightDef */
struct ActorVt gJavelinKnightHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = -1,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MetaKnightsKnightReactToDamage,
    .defeatCallback = 0,
};
/* gJavelinKnightJavelinDef */
struct ActorVt gJavelinKnightJavelinHitReactions ACTOR_TBL(087481a0) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08748930-0x087489A4: 7 record(s), section .actor_tbl_08748930 ---- */
/* gKingDededeDef */
struct ActorHandlers gKingDededeTerrainHandlers ACTOR_TBL(08748930) = {
    .landCallback = (u32)KingDededeLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)KingDededeHitWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)KingDededeHitCeiling,
};
/* gMrShineAndMrBrightDef, gUnk_087487BC */
struct ActorHandlers gUnk_0874894C ACTOR_TBL(08748930) = {
    .landCallback = (u32)MrShineAndMrBrightLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MrShineAndMrBrightHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gKingDededeDef */
struct ActorVt gKingDededeHitReactions ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)KingDededeReactToDamage,
    .defeatCallback = (u32)KingDededeReactToDefeat,
};
/* src/enemy_9fbd0.c */
struct ActorVt gKingDededeDefeatedHitReactions ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)KingDededeReactToDamage,
    .defeatCallback = (u32)KingDededeReactToDefeat,
};
/* gMrShineAndMrBrightDef */
struct ActorVt gMrShineAndMrBrightHitReactions ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrShineAndMrBrightReactToDamage,
    .defeatCallback = (u32)MrShineAndMrBrightReactToDefeat,
};
/* src/enemy_a1590.c */
struct ActorVt gMrShineAndMrBrightDefeatedHitReactions ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_087487BC */
struct ActorVt gUnk_08748998 ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MrShineAndMrBrightReactToDamage,
    .defeatCallback = (u32)MrShineAndMrBrightReactToDefeat,
};

/* ---- 0x08748CF8-0x08748D28: 4 record(s), section .actor_tbl_08748cf8 ---- */
/* gKingDededeStarDef */
struct ActorVt gKingDededeStarHitReactions ACTOR_TBL(08748cf8) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_a1590.c */
struct ActorVt gUnk_08748D04 ACTOR_TBL(08748cf8) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gChildActorDef24, gUnk_08748B34 */
struct ActorVt gUnk_08748D10 ACTOR_TBL(08748cf8) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_08748B60, gUnk_08748B8C */
struct ActorVt gUnk_08748D1C ACTOR_TBL(08748cf8) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08749B08-0x08749B6C: 7 record(s), section .actor_tbl_08749b08 ---- */
/* gMetaKnightDef */
struct ActorHandlers gMetaKnightTerrainHandlers ACTOR_TBL(08749b08) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)MetaKnightHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gMetaKnightDef */
struct ActorVt gMetaKnightHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)MetaKnightReactToDamage,
    .defeatCallback = (u32)MetaKnightReactToDefeat,
};
/* src/enemy_a5644.c */
struct ActorVt gMetaKnightReactToDefeatHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gKrackoDef */
struct ActorVt gKrackoHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)KrackoReactToDamage,
    .defeatCallback = (u32)KrackoReactToDefeat,
};
/* src/enemy_a93ec.c */
struct ActorVt gKrackoReactToDefeatHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gNightmareWizardDef */
struct ActorVt gNightmareWizardHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)NightmareWizardReactToDamage,
    .defeatCallback = (u32)NightmareWizardReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gNightmareWizardReactToDefeatHitReactions ACTOR_TBL(08749b08) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08749CD4-0x08749CEC: 2 record(s), section .actor_tbl_08749cd4 ---- */
/* gKrackoStarmanDef */
struct ActorVt gKrackoStarmanHitReactions ACTOR_TBL(08749cd4) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gNightmareWizardStarDef */
struct ActorVt gNightmareWizardStarHitReactions ACTOR_TBL(08749cd4) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x0874B4E0-0x0874B51C: 5 record(s), section .actor_tbl_0874b4e0 ---- */
/* gPaintRollerDef */
struct ActorVt gPaintRollerHitReactions ACTOR_TBL(0874b4e0) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)PaintRollerReactToDamage,
    .defeatCallback = (u32)PaintRollerReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gPaintRollerReactToDefeatHitReactions ACTOR_TBL(0874b4e0) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gHeavyMoleDef */
struct ActorVt gHeavyMoleHitReactions ACTOR_TBL(0874b4e0) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)HeavyMoleReactToDamage,
    .defeatCallback = (u32)HeavyMoleReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gHeavyMoleReactToDefeatHitReactions ACTOR_TBL(0874b4e0) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gNightmarePowerOrbDef; the 12 bytes after it, up to the next label, have no reference and stay structure-only data */
struct ActorVt gNightmarePowerOrbHitReactions ACTOR_TBL(0874b4e0) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)NightmarePowerOrbReactToDamage,
    .defeatCallback = (u32)NightmarePowerOrbReactToDefeat,
};

/* ---- 0x0874BF58-0x0874BFF8: 12 record(s), section .actor_tbl_0874bf58 ---- */
/* 6 ActorDefs (gUnk_0874B96C, gUnk_0874B998, ...) */
struct ActorHandlers gPaintRollerPaintingTerrainHandlers ACTOR_TBL(0874bf58) = {
    .landCallback = (u32)PaintRollerPaintingLand,
    .leaveGroundCallback = (u32)PaintRollerPaintingStartFall,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)PaintRollerPaintingHitWall,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gUnk_0874B96C */
struct ActorVt gUnk_0874BF74 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874B998 */
struct ActorVt gUnk_0874BF80 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874B9C4 */
struct ActorVt gUnk_0874BF8C ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874B9F0 */
struct ActorVt gUnk_0874BF98 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874BA1C */
struct ActorVt gUnk_0874BFA4 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874BA48 */
struct ActorVt gUnk_0874BFB0 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874BA74 */
struct ActorVt gUnk_0874BFBC ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gUnk_0874BAA0 */
struct ActorVt gUnk_0874BFC8 ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_ae3bc.c */
struct ActorVt gPaintRollerPaintingParasolUpdateHitReactions ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 3,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gHeavyMoleYellowMissileDef */
struct ActorVt gHeavyMoleYellowMissileHitReactions ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gHeavyMoleRedMissileDef */
struct ActorVt gHeavyMoleRedMissileHitReactions ACTOR_TBL(0874bf58) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x0874C204-0x0874C21C: 2 record(s), section .actor_tbl_0874c204 ---- */
/* gWhispyWoodsDef */
struct ActorVt gWhispyWoodsHitReactions ACTOR_TBL(0874c204) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)WhispyWoodsReactToDamage,
    .defeatCallback = (u32)WhispyWoodsReactToDefeat,
};
/* src/enemy_ae3bc.c */
struct ActorVt gWhispyWoodsReactToDefeatHitReactions ACTOR_TBL(0874c204) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)WhispyWoodsReactToDamage,
    .defeatCallback = (u32)WhispyWoodsReactToDefeat,
};

/* ---- 0x0874C418-0x0874C44C: 3 record(s), section .actor_tbl_0874c418 ---- */
/* gWhispyWoodsAppleDef */
struct ActorHandlers gWhispyWoodsAppleTerrainHandlers ACTOR_TBL(0874c418) = {
    .landCallback = (u32)WhispyWoodsAppleLand,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gWhispyWoodsAppleDef */
struct ActorVt gWhispyWoodsAppleHitReactions ACTOR_TBL(0874c418) = {
    .damageKind = 0,
    .defeatKind = 8,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* gWhispyWoodsAirPuffDef */
struct ActorVt gWhispyWoodsAirPuffHitReactions ACTOR_TBL(0874c418) = {
    .damageKind = 0,
    .defeatKind = 4,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
