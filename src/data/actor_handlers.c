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
 *   ActorCollideTerrain, sub_080696a0 and sub_08069888 (src/actor_692fc.c)
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
s32 sub_08078984(void);
s32 sub_080789ac(void);
s32 sub_08078a48(void);
s32 sub_08078b08(void);
s32 sub_08078b38(void);
s32 sub_08078b64(void);
s32 sub_08079480(void);
s32 sub_080794a0(void);
s32 sub_080794c0(void);
s32 sub_08079a20(void);
s32 sub_08079a40(void);
s32 sub_08079a60(void);
s32 sub_08079a70(void);
s32 sub_08079a90(void);
s32 sub_08079fd0(void);
s32 sub_0807a008(void);
s32 sub_0807a040(void);
s32 sub_0807a050(void);
void sub_0807a05c(void);
s32 sub_0807afd8(void);
s32 sub_0807b010(void);
s32 sub_0807b048(void);
s32 sub_0807b058(void);
s32 sub_0807b070(void);
s32 sub_0807bdb8(void);
s32 sub_0807be08(void);
s32 sub_0807be9c(void);
s32 sub_0807beac(void);
s32 sub_0807bed4(void);
void sub_0807befc(void);
s32 sub_0807d1bc(void);
s32 sub_0807d1dc(void);
s32 sub_0807dd70(void);
s32 sub_0807dddc(void);
s32 sub_0807de30(void);
s32 sub_0807de64(void);
s32 sub_0807de88(void);
s32 sub_0807e4cc(void);
s32 sub_0807e4fc(void);
s32 sub_0807e514(void);
s32 sub_0807e520(void);
s32 sub_0807e9b4(void);
s32 sub_0807e9c8(void);
s32 sub_0807fbd0(void);
s32 sub_0807fc20(void);
s32 sub_0807fc70(void);
s32 sub_0807fc94(void);
s32 sub_0807fcac(void);
s32 sub_08080398(void);
s32 sub_080803a4(void);
void sub_080803bc(void);
s32 sub_080803cc(void);
s32 sub_08080b70(void);
s32 sub_08080bcc(void);
s32 sub_08080c2c(void);
s32 sub_08080c38(void);
s32 sub_08081884(void);
s32 sub_080818a8(void);
s32 sub_08081900(void);
s32 sub_08081960(void);
s32 sub_08081984(void);
s32 sub_080819a4(void);
void sub_08081f08(void);
s32 sub_08081f18(void);
s32 sub_08081f38(void);
s32 sub_08081f50(void);
s32 sub_08082678(void);
s32 sub_080826a0(void);
s32 sub_080826bc(void);
s32 sub_080826c8(void);
s32 sub_08082d14(void);
s32 sub_08082d4c(void);
s32 sub_08082db0(void);
s32 sub_08082dd4(void);
u8 sub_08083e5c(void);
u8 sub_08084bc0(void);
u8 sub_08084c0c(void);
u8 sub_08084c5c(void);
s32 sub_08084cb8(void);
u8 sub_08085390(void);
u8 sub_080853c8(void);
u8 sub_08085404(void);
s32 sub_0808542c(void);
u8 sub_08085e74(void);
u8 sub_08085ef0(void);
u8 sub_08085fa0(void);
s32 sub_08085fec(void);
s32 sub_08086024(void);
void sub_0808606c(void);
s32 sub_080884e4(void);
s32 sub_08088540(void);
s32 sub_08088590(void);
s32 sub_080885c0(void);
s32 sub_08088fc0(void);
s32 sub_08089024(void);
s32 sub_08089064(void);
s32 sub_080890d4(void);
s32 sub_08089120(void);
s32 sub_0808913c(void);
s32 sub_080896ec(void);
s32 sub_0808972c(void);
s32 sub_0808976c(void);
s32 sub_0808978c(void);
s32 sub_080897d0(void);
s32 sub_08089bf0(void);
s32 sub_08089c0c(void);
s32 sub_08089c30(void);
s32 SlippyLand(void);
s32 SlippyStartFall(void);
s32 SlippyEnterWater(void);
s32 SlippyHitWall(void);
s32 SlippyHitCeiling(void);
s32 sub_0808bb70(void);
s32 sub_0808bc18(void);
s32 sub_0808bc60(void);
s32 sub_0808bd04(void);
s32 sub_0808c71c(void);
s32 sub_0808c74c(void);
s32 sub_0808c77c(void);
s32 sub_0808c79c(void);
s32 sub_0808c7ec(void);
void sub_0808d130(void);
void sub_0808d200(void);
s32 BroomHatterStartFall(void);
s32 BroomHatterLand(void);
s32 BroomHatterEnterWater(void);
s32 sub_0808d388(void);
s32 sub_0808e8a4(void);
s32 sub_0808e8c4(void);
s32 ShotzoStartFall(void);
s32 ShotzoLand(void);
s32 ShotzoHitWall(void);
s32 ShotzoEnterWater(void);
s32 ParasolShotzoReactToDefeat(void);
s32 sub_0808f978(void);
s32 sub_0808f9b8(void);
s32 sub_0808f9d8(void);
s32 sub_0808f9f8(void);
s32 BonkersReactToDamage(void);
s32 BonkersReactToDefeat(void);
s32 sub_08090f4c(void);
s32 PoppyBrosSrReactToDamage(void);
s32 PoppyBrosSrReactToDefeat(void);
s32 sub_08091b60(void);
s32 BugzzyHitWall(void);
void BugzzyHitCeiling(void);
s32 BugzzyReactToDamage(void);
s32 BugzzyReactToDefeat(void);
s32 sub_08093bb0(void);
s32 sub_08093edc(void);
s32 sub_08093f00(void);
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
s32 sub_080988c4(void);
u8 sub_080988f8(void);
u8 sub_08098a04(void);
u8 MrFrostyReactToDefeat(void);
u8 MrFrostyReactToDamage(void);
u8 MrTickTockStartFall(void);
u8 MrTickTockLand(void);
u8 MrTickTockHitWall(void);
u8 MrTickTockReactToDamage(void);
u8 MrTickTockReactToDefeat(void);
u8 sub_0809b454(void);
void sub_0809b4d8(void);
u8 sub_0809b4dc(void);
u8 MrTickTockNoteLand(void);
u8 MrTickTockNoteHitWall(void);
u8 sub_0809d0a0(void);
u8 sub_0809d0dc(void);
u8 sub_0809d138(void);
u8 sub_0809db48(void);
u8 sub_0809dbc4(void);
u8 sub_0809dc3c(void);
u8 sub_0809e7c8(void);
u8 sub_0809e7d4(void);
u8 sub_0809e7e8(void);
u8 sub_0809e820(void);
u8 sub_0809f52c(void);
u8 sub_0809f588(void);
u8 sub_0809f618(void);
void MetaKnightsKnightReactToDamage(void);
u8 sub_0809fd64(void);
u8 sub_0809fe10(void);
u8 KingDededeLand(void);
void KingDededeHitWall(void);
u8 KingDededeHitCeiling(void);
s32 sub_080a1df8(void);
s32 sub_080a1e4c(void);
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
s32 sub_080b1588(void);
s32 sub_080b1594(void);
s32 sub_080b15b4(void);
void sub_080b2550(void);
void sub_080b2574(void);
s32 sub_080b2f38(void);
s32 PickupStartFall(void);
s32 PickupLand(void);
s32 PickupEnterWater(void);
s32 AbilityStarBounceOffFloor(void);
s32 AbilityStarEnterWater(void);
s32 AbilityStarBounceOffWall(void);
s32 sub_080b442c(void);

/* ---- 0x0873F5FC-0x0873F664: 6 record(s), section .actor_tbl_0873f5fc ---- */
/* gAbilityStarDef */
struct ActorHandlers gAbilityStarTerrainHandlers ACTOR_TBL(0873f5fc) = {
    .landCallback = (u32)AbilityStarBounceOffFloor,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)AbilityStarEnterWater,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)AbilityStarBounceOffWall,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_080b442c,
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
/* gUnk_0873F394 */
struct ActorVt gUnk_0873F64C ACTOR_TBL(0873f5fc) = {
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
struct ActorHandlers gUnk_0873F8F4[] ACTOR_TBL(0873f8f4) = { {
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
struct ActorHandlers gUnk_08740E54 ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_08078a48,
    .leaveGroundCallback = (u32)sub_080789ac,
    .enterWaterCallback = (u32)sub_08078b08,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08078b38,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_08078b64,
};
/* gPengyDef */
struct ActorHandlers gPengyTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_080794a0,
    .leaveGroundCallback = (u32)sub_08079480,
    .enterWaterCallback = (u32)sub_080794c0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gBomberDef */
struct ActorHandlers gBomberTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_08079a40,
    .leaveGroundCallback = (u32)sub_08079a20,
    .enterWaterCallback = (u32)sub_08079a60,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08079a90,
    .unk14 = (u32)sub_08079a70,
    .hitCeilingCallback = 0,
};
/* gSparkyDef */
struct ActorHandlers gSparkyTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_0807a008,
    .leaveGroundCallback = (u32)sub_08079fd0,
    .enterWaterCallback = (u32)sub_0807a040,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807a050,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0807a05c,
};
/* gSwordKnightDef, gBladeKnightDef */
struct ActorHandlers gUnk_08740EC4 ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_0807b010,
    .leaveGroundCallback = (u32)sub_0807afd8,
    .enterWaterCallback = (u32)sub_0807b048,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807b070,
    .unk14 = (u32)sub_0807b058,
    .hitCeilingCallback = 0,
};
/* gNeedlousDef */
struct ActorHandlers gNeedlousTerrainHandlers ACTOR_TBL(08740e54) = {
    .landCallback = (u32)sub_0807be08,
    .leaveGroundCallback = (u32)sub_0807bdb8,
    .enterWaterCallback = (u32)sub_0807be9c,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807beac,
    .unk14 = (u32)sub_0807bed4,
    .hitCeilingCallback = (u32)sub_0807befc,
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
    .defeatCallback = (u32)sub_08078984,
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
struct ActorVt gUnk_08740F2C ACTOR_TBL(08740e54) = {
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
struct ActorVt gUnk_08740F50 ACTOR_TBL(08740e54) = {
    .damageKind = 0,
    .defeatKind = 1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gUnk_08740F5C ACTOR_TBL(08740e54) = {
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
struct ActorVt gUnk_08740FA4 ACTOR_TBL(08740e54) = {
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
    .damageCallback = (u32)sub_0807d1bc,
    .defeatCallback = (u32)sub_0807d1dc,
};

/* ---- 0x08741B64-0x08741D64: 28 record(s), section .actor_tbl_08741b64 ---- */
/* gRockyDef */
struct ActorHandlers gRockyTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_0807dd70,
    .leaveGroundCallback = (u32)sub_0807dddc,
    .enterWaterCallback = (u32)sub_0807de88,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807de64,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0807de30,
};
/* gSirKibbleDef */
struct ActorHandlers gSirKibbleTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_0807e514,
    .leaveGroundCallback = (u32)sub_0807e4fc,
    .enterWaterCallback = (u32)sub_0807e520,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807e4cc,
    .unk14 = (u32)sub_0807e4cc,
    .hitCeilingCallback = 0,
};
/* gCappyDef, gUnk_0874183C */
struct ActorHandlers gUnk_08741B9C ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)sub_0807e9c8,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807e9b4,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gKabuDef */
struct ActorHandlers gKabuTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_0807fc70,
    .leaveGroundCallback = (u32)sub_0807fc20,
    .enterWaterCallback = (u32)sub_0807fcac,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0807fbd0,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0807fc94,
};
/* gTwisterDef */
struct ActorHandlers gTwisterTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_08080398,
    .leaveGroundCallback = (u32)sub_080803a4,
    .enterWaterCallback = (u32)sub_080803cc,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_080803bc,
};
/* gStarmanDef */
struct ActorHandlers gStarmanTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_08081960,
    .leaveGroundCallback = (u32)sub_08081900,
    .enterWaterCallback = (u32)sub_080819a4,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08081984,
    .unk14 = (u32)sub_08081884,
    .hitCeilingCallback = (u32)sub_080818a8,
};
/* gHotHeadDef */
struct ActorHandlers gHotHeadTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_08080bcc,
    .leaveGroundCallback = (u32)sub_08080b70,
    .enterWaterCallback = (u32)sub_08080c38,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08080c2c,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosJrDef */
struct ActorHandlers gPoppyBrosJrTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = (u32)sub_08081f18,
    .enterWaterCallback = (u32)sub_08081f50,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08081f38,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_08081f08,
};
/* 4 ActorDefs (gPoppyBrosJrOnAppleDef, gPoppyBrosJrOnMaximTomatoDef, ...) */
struct ActorHandlers gUnk_08741C44 ACTOR_TBL(08741b64) = {
    .landCallback = (u32)sub_080826a0,
    .leaveGroundCallback = (u32)sub_08082678,
    .enterWaterCallback = (u32)sub_080826c8,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_080826bc,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gWheelieDef */
struct ActorHandlers gWheelieTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = (u32)sub_08082d14,
    .enterWaterCallback = (u32)sub_08082dd4,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08082d4c,
    .unk14 = (u32)sub_08082db0,
    .hitCeilingCallback = 0,
};
/* gFlamerDef */
struct ActorHandlers gFlamerTerrainHandlers ACTOR_TBL(08741b64) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)sub_08083e5c,
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
/* gUnk_0874183C */
struct ActorVt gUnk_08741CBC ACTOR_TBL(08741b64) = {
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
struct ActorVt gUnk_08741CE0 ACTOR_TBL(08741b64) = {
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
struct ActorVt gUnk_08741F64 ACTOR_TBL(08741f4c) = {
    .damageKind = 0,
    .defeatKind = 9,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};

/* ---- 0x08742CF0-0x08742EA4: 23 record(s), section .actor_tbl_08742cf0 ---- */
/* gNoddyDef */
struct ActorHandlers gNoddyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_08084bc0,
    .leaveGroundCallback = (u32)sub_08084c0c,
    .enterWaterCallback = (u32)sub_08084c5c,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08084cb8,
    .unk14 = (u32)sub_08084cb8,
    .hitCeilingCallback = 0,
};
/* gChillyDef */
struct ActorHandlers gChillyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_08085390,
    .leaveGroundCallback = (u32)sub_080853c8,
    .enterWaterCallback = (u32)sub_08085404,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808542c,
    .unk14 = (u32)sub_0808542c,
    .hitCeilingCallback = 0,
};
/* gWaddleDooDef, gParasolWaddleDooDef */
struct ActorHandlers gUnk_08742D28 ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_08085e74,
    .leaveGroundCallback = (u32)sub_08085ef0,
    .enterWaterCallback = (u32)sub_08085fa0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08086024,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_08085fec,
};
/* gTwizzyDef */
struct ActorHandlers gTwizzyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_080884e4,
    .leaveGroundCallback = (u32)sub_08088540,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08088590,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_080885c0,
};
/* gSquishyDef */
struct ActorHandlers gSquishyTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_08088fc0,
    .leaveGroundCallback = (u32)sub_08089024,
    .enterWaterCallback = (u32)sub_08089064,
    .leaveWaterCallback = (u32)sub_080890d4,
    .hitWallCallback = (u32)sub_08089120,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0808913c,
};
/* gBubblesDef */
struct ActorHandlers gBubblesTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_080896ec,
    .leaveGroundCallback = (u32)sub_0808972c,
    .enterWaterCallback = (u32)sub_0808976c,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808978c,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_080897d0,
};
/* gGlunkDef */
struct ActorHandlers gGlunkTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_08089bf0,
    .leaveGroundCallback = (u32)sub_08089c0c,
    .enterWaterCallback = (u32)sub_08089c30,
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
    .landCallback = (u32)sub_0808bb70,
    .leaveGroundCallback = (u32)sub_0808bc18,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808bc60,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0808bd04,
};
/* gGipDef */
struct ActorHandlers gGipTerrainHandlers ACTOR_TBL(08742cf0) = {
    .landCallback = (u32)sub_0808c71c,
    .leaveGroundCallback = (u32)sub_0808c74c,
    .enterWaterCallback = (u32)sub_0808c77c,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808c79c,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0808c7ec,
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
    .defeatCallback = (u32)sub_0808606c,
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
struct ActorVt gUnk_08742E50 ACTOR_TBL(08742cf0) = {
    .damageKind = 0,
    .defeatKind = 0,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = 0,
    .defeatCallback = 0,
};
/* src/enemy_88000.c */
struct ActorVt gUnk_08742E5C ACTOR_TBL(08742cf0) = {
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
    .landCallback = (u32)sub_0808d130,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808d130,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0808d130,
};
/* gGlunkShotDef */
struct ActorHandlers gGlunkShotTerrainHandlers ACTOR_TBL(087430ec) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = 0,
    .unk14 = 0,
    .hitCeilingCallback = (u32)sub_0808d200,
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
    .landCallback = (u32)sub_0808e8a4,
    .leaveGroundCallback = 0,
    .enterWaterCallback = (u32)sub_0808e8c4,
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
    .landCallback = (u32)sub_0808f9b8,
    .leaveGroundCallback = (u32)sub_0808f978,
    .enterWaterCallback = (u32)sub_0808f9d8,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0808f9f8,
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
struct ActorVt gUnk_08743558 ACTOR_TBL(087434c4) = {
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
    .hitWallCallback = (u32)sub_08090f4c,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosSrDef */
struct ActorHandlers gPoppyBrosSrTerrainHandlers ACTOR_TBL(0874407c) = {
    .landCallback = 0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08091b60,
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
struct ActorVt gUnk_087440DC ACTOR_TBL(0874407c) = {
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
struct ActorVt gUnk_087440F4 ACTOR_TBL(0874407c) = {
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
    .hitWallCallback = (u32)sub_08093bb0,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gPoppyBrosSrBombDef */
struct ActorHandlers gPoppyBrosSrBombTerrainHandlers ACTOR_TBL(087442c8) = {
    .landCallback = (u32)sub_08093f00,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08093edc,
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
struct ActorVt gUnk_0874430C ACTOR_TBL(087442c8) = {
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
struct ActorVt gUnk_08744324 ACTOR_TBL(087442c8) = {
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
    .landCallback = (u32)sub_080988f8,
    .leaveGroundCallback = (u32)sub_080988c4,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_08098a04,
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
struct ActorVt gUnk_08745A80 ACTOR_TBL(08745a3c) = {
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
    .landCallback = (u32)sub_0809b454,
    .leaveGroundCallback = (u32)sub_0809b4d8,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0809b4dc,
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
    .landCallback = (u32)sub_0809d0a0,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0809d0dc,
    .unk14 = (u32)sub_0809d0dc,
    .hitCeilingCallback = (u32)sub_0809d138,
};
/* gMaceKnightDef */
struct ActorHandlers gMaceKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)sub_0809e7c8,
    .leaveGroundCallback = (u32)sub_0809e7d4,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0809e7e8,
    .unk14 = (u32)sub_0809e7e8,
    .hitCeilingCallback = (u32)sub_0809e820,
};
/* gTridentKnightDef */
struct ActorHandlers gTridentKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)sub_0809f52c,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0809f588,
    .unk14 = (u32)sub_0809f588,
    .hitCeilingCallback = (u32)sub_0809f618,
};
/* gJavelinKnightDef */
struct ActorHandlers gJavelinKnightTerrainHandlers ACTOR_TBL(087481a0) = {
    .landCallback = (u32)sub_0809db48,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_0809dbc4,
    .unk14 = (u32)sub_0809dbc4,
    .hitCeilingCallback = (u32)sub_0809dc3c,
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
    .landCallback = (u32)sub_080a1df8,
    .leaveGroundCallback = 0,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_080a1e4c,
    .unk14 = 0,
    .hitCeilingCallback = 0,
};
/* gKingDededeDef */
struct ActorVt gKingDededeHitReactions ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = -1,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)sub_0809fe10,
    .defeatCallback = (u32)sub_0809fd64,
};
/* src/enemy_9fbd0.c */
struct ActorVt gUnk_08748974 ACTOR_TBL(08748930) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)sub_0809fe10,
    .defeatCallback = (u32)sub_0809fd64,
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
struct ActorVt gUnk_0874898C ACTOR_TBL(08748930) = {
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
/* gUnk_08748B08, gUnk_08748B34 */
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
struct ActorVt gUnk_08749B30 ACTOR_TBL(08749b08) = {
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
struct ActorVt gUnk_08749B48 ACTOR_TBL(08749b08) = {
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
struct ActorVt gUnk_08749B60 ACTOR_TBL(08749b08) = {
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
struct ActorVt gUnk_0874B4EC ACTOR_TBL(0874b4e0) = {
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
struct ActorVt gUnk_0874B504 ACTOR_TBL(0874b4e0) = {
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
struct ActorHandlers gUnk_0874BF58 ACTOR_TBL(0874bf58) = {
    .landCallback = (u32)sub_080b1588,
    .leaveGroundCallback = (u32)sub_080b1594,
    .enterWaterCallback = 0,
    .leaveWaterCallback = 0,
    .hitWallCallback = (u32)sub_080b15b4,
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
struct ActorVt gUnk_0874BFD4 ACTOR_TBL(0874bf58) = {
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
    .damageCallback = (u32)sub_080b2574,
    .defeatCallback = (u32)sub_080b2550,
};
/* src/enemy_ae3bc.c */
struct ActorVt gUnk_0874C210 ACTOR_TBL(0874c204) = {
    .damageKind = -1,
    .defeatKind = 7,
    .filler02 = { 0x00, 0x00 },
    .damageCallback = (u32)sub_080b2574,
    .defeatCallback = (u32)sub_080b2550,
};

/* ---- 0x0874C418-0x0874C44C: 3 record(s), section .actor_tbl_0874c418 ---- */
/* gWhispyWoodsAppleDef */
struct ActorHandlers gWhispyWoodsAppleTerrainHandlers ACTOR_TBL(0874c418) = {
    .landCallback = (u32)sub_080b2f38,
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
