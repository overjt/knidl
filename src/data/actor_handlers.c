#include "global.h"
#include "task.h"
#include "actor.h"

/* The actor handler records of actor_rodata (issue #167): 62 struct
 * ActorHandlers tables and 136 struct ActorVt records (include/actor.h),
 * 261 function pointers, in 28 runs of adjacent records (one of them round 1's
 * row of gUnk_0873F910, moved here from actor_tables.c), each run in address
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
s32 sub_0808a8d4(void);
s32 sub_0808a964(void);
s32 sub_0808a9a8(void);
s32 sub_0808a9d8(void);
s32 sub_0808aa28(void);
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
s32 sub_0808d2b8(void);
s32 sub_0808d304(void);
s32 sub_0808d354(void);
s32 sub_0808d388(void);
s32 sub_0808e8a4(void);
s32 sub_0808e8c4(void);
s32 sub_0808ebe0(void);
s32 sub_0808ec34(void);
s32 sub_0808ec90(void);
s32 sub_0808ecb4(void);
s32 sub_0808ece0(void);
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
s32 sub_080937d0(void);
void sub_08093858(void);
s32 BugzzyReactToDamage(void);
s32 BugzzyReactToDefeat(void);
s32 sub_08093bb0(void);
s32 sub_08093edc(void);
s32 sub_08093f00(void);
s32 sub_080954f0(void);
s32 sub_080955a8(void);
void GrandWheelieReactToDamage(void);
void GrandWheelieReactToDefeat(void);
s32 sub_08096d64(void);
s32 sub_08096df4(void);
s32 FireLionReactToDamage(void);
s32 FireLionReactToDefeat(void);
s32 sub_08098528(void);
s32 sub_08098540(void);
s32 PhanPhanReactToDamage(void);
s32 PhanPhanReactToDefeat(void);
void sub_080986ec(void);
void sub_08098718(void);
void sub_08098728(void);
void sub_08098738(void);
s32 sub_080988a4(void);
s32 sub_080988b4(void);
s32 sub_080988c4(void);
u8 sub_080988f8(void);
u8 sub_08098a04(void);
u8 MrFrostyReactToDefeat(void);
u8 MrFrostyReactToDamage(void);
u8 sub_08099aec(void);
u8 sub_08099b20(void);
u8 sub_08099c4c(void);
u8 MrTickTockReactToDamage(void);
u8 MrTickTockReactToDefeat(void);
u8 sub_0809b454(void);
void sub_0809b4d8(void);
u8 sub_0809b4dc(void);
u8 sub_0809b9c0(void);
u8 sub_0809b9e0(void);
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
void sub_0809f7f8(void);
u8 sub_0809fd64(void);
u8 sub_0809fe10(void);
u8 sub_080a0538(void);
void sub_080a0588(void);
u8 sub_080a0598(void);
s32 sub_080a1df8(void);
s32 sub_080a1e4c(void);
s32 MrShineAndMrBrightReactToDefeat(void);
s32 MrShineAndMrBrightReactToDamage(void);
s32 MetaKnightReactToDamage(void);
s32 MetaKnightReactToDefeat(void);
s32 sub_080a7334(void);
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
s32 sub_080b404c(void);
s32 sub_080b406c(void);
s32 sub_080b408c(void);
s32 sub_080b4390(void);
s32 sub_080b43d4(void);
s32 sub_080b43f4(void);
s32 sub_080b442c(void);

/* ---- 0x0873F5FC-0x0873F664: 6 record(s), section .actor_tbl_0873f5fc ---- */
/* gAbilityStarDef */
struct ActorHandlers gUnk_0873F5FC ACTOR_TBL(0873f5fc) = {
    .unk00 = (u32)sub_080b4390,
    .unk04 = 0,
    .unk08 = (u32)sub_080b43d4,
    .unk0C = 0,
    .unk10 = (u32)sub_080b43f4,
    .unk14 = 0,
    .unk18 = (u32)sub_080b442c,
};
/* 4 ActorDefs (gOneUpDef, gMaximTomatoDef, ...) */
struct ActorHandlers gUnk_0873F618 ACTOR_TBL(0873f5fc) = {
    .unk00 = (u32)sub_080b406c,
    .unk04 = (u32)sub_080b404c,
    .unk08 = (u32)sub_080b408c,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gAbilityStarDef */
struct ActorVt gUnk_0873F634 ACTOR_TBL(0873f5fc) = {
    .unk00 = 0,
    .unk01 = 5,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* 4 ActorDefs (gOneUpDef, gMaximTomatoDef, ...) */
struct ActorVt gUnk_0873F640 ACTOR_TBL(0873f5fc) = {
    .unk00 = 0,
    .unk01 = 10,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0873F394 */
struct ActorVt gUnk_0873F64C ACTOR_TBL(0873f5fc) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBigSwitchDef */
struct ActorVt gUnk_0873F658 ACTOR_TBL(0873f5fc) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x0873F8F4-0x0873F910: 1 record(s), section .actor_tbl_0873f8f4 ---- */
/* 19 ActorDefs (gCannonDef, gCannonFuseDef, ...); src/actor_6b2e4.c, src/actor_6c2a4.c */
struct ActorHandlers gUnk_0873F8F4[] ACTOR_TBL(0873f8f4) = { {
        .unk00 = 0,
        .unk04 = 0,
        .unk08 = 0,
        .unk0C = 0,
        .unk10 = 0,
        .unk14 = 0,
        .unk18 = 0,
} };

/* ---- 0x0873F910-0x0873F92C: 1 record(s), section .actor_tbl_0873f910 ---- */
/* src/actor_692fc.c */
struct ActorHandlers gUnk_0873F910[] ACTOR_TBL(0873f910) = { {
        .unk00 = (u32)sub_0806b24c,
        .unk04 = 0,
        .unk08 = 0,
        .unk0C = 0,
        .unk10 = 0,
        .unk14 = 0,
        .unk18 = 0,
} };

/* ---- 0x0873F92C-0x0873F950: 3 record(s), section .actor_tbl_0873f92c ---- */
/* src/actor_6b2e4.c */
struct ActorVt gUnk_0873F92C[] ACTOR_TBL(0873f92c) = { {
        .unk00 = 0,
        .unk01 = 4,
        .filler02 = { 0x00, 0x00 },
        .unk04 = 0,
        .unk08 = 0,
} };
/* src/actor_6b2e4.c */
struct ActorVt gUnk_0873F938[] ACTOR_TBL(0873f92c) = { {
        .unk00 = 0,
        .unk01 = 0,
        .filler02 = { 0x00, 0x00 },
        .unk04 = 0,
        .unk08 = 0,
} };
/* gInhalableStarDef, gUnk_0873F690 */
struct ActorVt gUnk_0873F944 ACTOR_TBL(0873f92c) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08740E54-0x08740FB0: 21 record(s), section .actor_tbl_08740e54 ---- */
/* gWaddleDeeDef, gParasolWaddleDeeDef */
struct ActorHandlers gUnk_08740E54 ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_08078a48,
    .unk04 = (u32)sub_080789ac,
    .unk08 = (u32)sub_08078b08,
    .unk0C = 0,
    .unk10 = (u32)sub_08078b38,
    .unk14 = 0,
    .unk18 = (u32)sub_08078b64,
};
/* gPengyDef */
struct ActorHandlers gUnk_08740E70 ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_080794a0,
    .unk04 = (u32)sub_08079480,
    .unk08 = (u32)sub_080794c0,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gBomberDef */
struct ActorHandlers gUnk_08740E8C ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_08079a40,
    .unk04 = (u32)sub_08079a20,
    .unk08 = (u32)sub_08079a60,
    .unk0C = 0,
    .unk10 = (u32)sub_08079a90,
    .unk14 = (u32)sub_08079a70,
    .unk18 = 0,
};
/* gSparkyDef */
struct ActorHandlers gUnk_08740EA8 ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_0807a008,
    .unk04 = (u32)sub_08079fd0,
    .unk08 = (u32)sub_0807a040,
    .unk0C = 0,
    .unk10 = (u32)sub_0807a050,
    .unk14 = 0,
    .unk18 = (u32)sub_0807a05c,
};
/* gSwordKnightDef, gBladeKnightDef */
struct ActorHandlers gUnk_08740EC4 ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_0807b010,
    .unk04 = (u32)sub_0807afd8,
    .unk08 = (u32)sub_0807b048,
    .unk0C = 0,
    .unk10 = (u32)sub_0807b070,
    .unk14 = (u32)sub_0807b058,
    .unk18 = 0,
};
/* gNeedlousDef */
struct ActorHandlers gUnk_08740EE0 ACTOR_TBL(08740e54) = {
    .unk00 = (u32)sub_0807be08,
    .unk04 = (u32)sub_0807bdb8,
    .unk08 = (u32)sub_0807be9c,
    .unk0C = 0,
    .unk10 = (u32)sub_0807beac,
    .unk14 = (u32)sub_0807bed4,
    .unk18 = (u32)sub_0807befc,
};
/* gWaddleDeeDef */
struct ActorVt gUnk_08740EFC ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gParasolWaddleDeeDef */
struct ActorVt gUnk_08740F08 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = (u32)sub_08078984,
};
/* gPengyDef */
struct ActorVt gUnk_08740F14 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBomberDef */
struct ActorVt gUnk_08740F20 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_78b68.c */
struct ActorVt gUnk_08740F2C ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 2,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gSparkyDef */
struct ActorVt gUnk_08740F38 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gScarfyDef */
struct ActorVt gUnk_08740F44 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gUnk_08740F50 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gUnk_08740F5C ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gSwordKnightDef, gBladeKnightDef */
struct ActorVt gUnk_08740F68 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBlockStarDef */
struct ActorVt gUnk_08740F74 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gNeedlousDef */
struct ActorVt gUnk_08740F80 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUFODef */
struct ActorVt gUnk_08740F8C ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gParasolDef */
struct ActorVt gUnk_08740F98 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_7aa5c.c */
struct ActorVt gUnk_08740FA4 ACTOR_TBL(08740e54) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x087411B4-0x087411C0: 1 record(s), section .actor_tbl_087411b4 ---- */
/* gUFOLaserDef */
struct ActorVt gUnk_087411B4 ACTOR_TBL(087411b4) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0807d1bc,
    .unk08 = (u32)sub_0807d1dc,
};

/* ---- 0x08741B64-0x08741D64: 28 record(s), section .actor_tbl_08741b64 ---- */
/* gRockyDef */
struct ActorHandlers gUnk_08741B64 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_0807dd70,
    .unk04 = (u32)sub_0807dddc,
    .unk08 = (u32)sub_0807de88,
    .unk0C = 0,
    .unk10 = (u32)sub_0807de64,
    .unk14 = 0,
    .unk18 = (u32)sub_0807de30,
};
/* gSirKibbleDef */
struct ActorHandlers gUnk_08741B80 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_0807e514,
    .unk04 = (u32)sub_0807e4fc,
    .unk08 = (u32)sub_0807e520,
    .unk0C = 0,
    .unk10 = (u32)sub_0807e4cc,
    .unk14 = (u32)sub_0807e4cc,
    .unk18 = 0,
};
/* gCappyDef, gUnk_0874183C */
struct ActorHandlers gUnk_08741B9C ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = (u32)sub_0807e9c8,
    .unk0C = 0,
    .unk10 = (u32)sub_0807e9b4,
    .unk14 = 0,
    .unk18 = 0,
};
/* gKabuDef */
struct ActorHandlers gUnk_08741BB8 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_0807fc70,
    .unk04 = (u32)sub_0807fc20,
    .unk08 = (u32)sub_0807fcac,
    .unk0C = 0,
    .unk10 = (u32)sub_0807fbd0,
    .unk14 = 0,
    .unk18 = (u32)sub_0807fc94,
};
/* gTwisterDef */
struct ActorHandlers gUnk_08741BD4 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_08080398,
    .unk04 = (u32)sub_080803a4,
    .unk08 = (u32)sub_080803cc,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = (u32)sub_080803bc,
};
/* gStarmanDef */
struct ActorHandlers gUnk_08741BF0 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_08081960,
    .unk04 = (u32)sub_08081900,
    .unk08 = (u32)sub_080819a4,
    .unk0C = 0,
    .unk10 = (u32)sub_08081984,
    .unk14 = (u32)sub_08081884,
    .unk18 = (u32)sub_080818a8,
};
/* gHotHeadDef */
struct ActorHandlers gUnk_08741C0C ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_08080bcc,
    .unk04 = (u32)sub_08080b70,
    .unk08 = (u32)sub_08080c38,
    .unk0C = 0,
    .unk10 = (u32)sub_08080c2c,
    .unk14 = 0,
    .unk18 = 0,
};
/* gPoppyBrosJrDef */
struct ActorHandlers gUnk_08741C28 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk04 = (u32)sub_08081f18,
    .unk08 = (u32)sub_08081f50,
    .unk0C = 0,
    .unk10 = (u32)sub_08081f38,
    .unk14 = 0,
    .unk18 = (u32)sub_08081f08,
};
/* 4 ActorDefs (gPoppyBrosJrOnAppleDef, gPoppyBrosJrOnMaximTomatoDef, ...) */
struct ActorHandlers gUnk_08741C44 ACTOR_TBL(08741b64) = {
    .unk00 = (u32)sub_080826a0,
    .unk04 = (u32)sub_08082678,
    .unk08 = (u32)sub_080826c8,
    .unk0C = 0,
    .unk10 = (u32)sub_080826bc,
    .unk14 = 0,
    .unk18 = 0,
};
/* gWheelieDef */
struct ActorHandlers gUnk_08741C60 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk04 = (u32)sub_08082d14,
    .unk08 = (u32)sub_08082dd4,
    .unk0C = 0,
    .unk10 = (u32)sub_08082d4c,
    .unk14 = (u32)sub_08082db0,
    .unk18 = 0,
};
/* gFlamerDef */
struct ActorHandlers gUnk_08741C7C ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = (u32)sub_08083e5c,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gRockyDef */
struct ActorVt gUnk_08741C98 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gSirKibbleDef */
struct ActorVt gUnk_08741CA4 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gCappyDef */
struct ActorVt gUnk_08741CB0 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874183C */
struct ActorVt gUnk_08741CBC ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gCoolSpookDef */
struct ActorVt gUnk_08741CC8 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gKabuDef */
struct ActorVt gUnk_08741CD4 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_7f044.c */
struct ActorVt gUnk_08741CE0 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gTwisterDef */
struct ActorVt gUnk_08741CEC ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gStarmanDef */
struct ActorVt gUnk_08741CF8 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gHotHeadDef */
struct ActorVt gUnk_08741D04 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosJrDef */
struct ActorVt gUnk_08741D10 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosJrOnAppleDef */
struct ActorVt gUnk_08741D1C ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosJrOnMaximTomatoDef */
struct ActorVt gUnk_08741D28 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosJrAppleDef */
struct ActorVt gUnk_08741D34 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosJrMaximTomatoDef */
struct ActorVt gUnk_08741D40 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gWheelieDef */
struct ActorVt gUnk_08741D4C ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gFlamerDef */
struct ActorVt gUnk_08741D58 ACTOR_TBL(08741b64) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08741F4C-0x08741F70: 3 record(s), section .actor_tbl_08741f4c ---- */
/* gSirKibbleCutterDef */
struct ActorVt gUnk_08741F4C ACTOR_TBL(08741f4c) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gHotHeadFireDef */
struct ActorVt gUnk_08741F58 ACTOR_TBL(08741f4c) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_82e68.c */
struct ActorVt gUnk_08741F64 ACTOR_TBL(08741f4c) = {
    .unk00 = 0,
    .unk01 = 9,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08742CF0-0x08742EA4: 23 record(s), section .actor_tbl_08742cf0 ---- */
/* gNoddyDef */
struct ActorHandlers gUnk_08742CF0 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_08084bc0,
    .unk04 = (u32)sub_08084c0c,
    .unk08 = (u32)sub_08084c5c,
    .unk0C = 0,
    .unk10 = (u32)sub_08084cb8,
    .unk14 = (u32)sub_08084cb8,
    .unk18 = 0,
};
/* gChillyDef */
struct ActorHandlers gUnk_08742D0C ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_08085390,
    .unk04 = (u32)sub_080853c8,
    .unk08 = (u32)sub_08085404,
    .unk0C = 0,
    .unk10 = (u32)sub_0808542c,
    .unk14 = (u32)sub_0808542c,
    .unk18 = 0,
};
/* gWaddleDooDef, gParasolWaddleDooDef */
struct ActorHandlers gUnk_08742D28 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_08085e74,
    .unk04 = (u32)sub_08085ef0,
    .unk08 = (u32)sub_08085fa0,
    .unk0C = 0,
    .unk10 = (u32)sub_08086024,
    .unk14 = 0,
    .unk18 = (u32)sub_08085fec,
};
/* gTwizzyDef */
struct ActorHandlers gUnk_08742D44 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_080884e4,
    .unk04 = (u32)sub_08088540,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08088590,
    .unk14 = 0,
    .unk18 = (u32)sub_080885c0,
};
/* gSquishyDef */
struct ActorHandlers gUnk_08742D60 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_08088fc0,
    .unk04 = (u32)sub_08089024,
    .unk08 = (u32)sub_08089064,
    .unk0C = (u32)sub_080890d4,
    .unk10 = (u32)sub_08089120,
    .unk14 = 0,
    .unk18 = (u32)sub_0808913c,
};
/* gBubblesDef */
struct ActorHandlers gUnk_08742D7C ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_080896ec,
    .unk04 = (u32)sub_0808972c,
    .unk08 = (u32)sub_0808976c,
    .unk0C = 0,
    .unk10 = (u32)sub_0808978c,
    .unk14 = 0,
    .unk18 = (u32)sub_080897d0,
};
/* gGlunkDef */
struct ActorHandlers gUnk_08742D98 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_08089bf0,
    .unk04 = (u32)sub_08089c0c,
    .unk08 = (u32)sub_08089c30,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gSlippyDef */
struct ActorHandlers gUnk_08742DB4 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_0808a8d4,
    .unk04 = (u32)sub_0808a964,
    .unk08 = (u32)sub_0808a9a8,
    .unk0C = 0,
    .unk10 = (u32)sub_0808a9d8,
    .unk14 = 0,
    .unk18 = (u32)sub_0808aa28,
};
/* gBlipperDef */
struct ActorHandlers gUnk_08742DD0 ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_0808bb70,
    .unk04 = (u32)sub_0808bc18,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0808bc60,
    .unk14 = 0,
    .unk18 = (u32)sub_0808bd04,
};
/* gGipDef */
struct ActorHandlers gUnk_08742DEC ACTOR_TBL(08742cf0) = {
    .unk00 = (u32)sub_0808c71c,
    .unk04 = (u32)sub_0808c74c,
    .unk08 = (u32)sub_0808c77c,
    .unk0C = 0,
    .unk10 = (u32)sub_0808c79c,
    .unk14 = 0,
    .unk18 = (u32)sub_0808c7ec,
};
/* gNoddyDef */
struct ActorVt gUnk_08742E08 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gChillyDef */
struct ActorVt gUnk_08742E14 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gWaddleDooDef */
struct ActorVt gUnk_08742E20 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gParasolWaddleDooDef */
struct ActorVt gUnk_08742E2C ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = (u32)sub_0808606c,
};
/* gBrontoBurtDef */
struct ActorVt gUnk_08742E38 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gTwizzyDef */
struct ActorVt gUnk_08742E44 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gSquishyDef; src/enemy_88000.c */
struct ActorVt gUnk_08742E50 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_88000.c */
struct ActorVt gUnk_08742E5C ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBubblesDef */
struct ActorVt gUnk_08742E68 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gGlunkDef */
struct ActorVt gUnk_08742E74 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gSlippyDef */
struct ActorVt gUnk_08742E80 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBlipperDef */
struct ActorVt gUnk_08742E8C ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gGipDef */
struct ActorVt gUnk_08742E98 ACTOR_TBL(08742cf0) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x087430EC-0x0874313C: 4 record(s), section .actor_tbl_087430ec ---- */
/* gWaddleDooBeamDef */
struct ActorHandlers gUnk_087430EC ACTOR_TBL(087430ec) = {
    .unk00 = (u32)sub_0808d130,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0808d130,
    .unk14 = 0,
    .unk18 = (u32)sub_0808d130,
};
/* gGlunkShotDef */
struct ActorHandlers gUnk_08743108 ACTOR_TBL(087430ec) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = (u32)sub_0808d200,
};
/* gGlunkShotDef */
struct ActorVt gUnk_08743124 ACTOR_TBL(087430ec) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gGipStarDef */
struct ActorVt gUnk_08743130 ACTOR_TBL(087430ec) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x087434C4-0x08743588: 11 record(s), section .actor_tbl_087434c4 ---- */
/* gBroomHatterDef */
struct ActorHandlers gUnk_087434C4 ACTOR_TBL(087434c4) = {
    .unk00 = (u32)sub_0808d304,
    .unk04 = (u32)sub_0808d2b8,
    .unk08 = (u32)sub_0808d354,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = (u32)sub_0808d388,
    .unk18 = 0,
};
/* gCoconutDef */
struct ActorHandlers gUnk_087434E0 ACTOR_TBL(087434c4) = {
    .unk00 = (u32)sub_0808e8a4,
    .unk04 = 0,
    .unk08 = (u32)sub_0808e8c4,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gShotzoDef, gParasolShotzoDef */
struct ActorHandlers gUnk_087434FC ACTOR_TBL(087434c4) = {
    .unk00 = (u32)sub_0808ec34,
    .unk04 = (u32)sub_0808ebe0,
    .unk08 = (u32)sub_0808ecb4,
    .unk0C = 0,
    .unk10 = (u32)sub_0808ec90,
    .unk14 = 0,
    .unk18 = 0,
};
/* gConerDef */
struct ActorHandlers gUnk_08743518 ACTOR_TBL(087434c4) = {
    .unk00 = (u32)sub_0808f9b8,
    .unk04 = (u32)sub_0808f978,
    .unk08 = (u32)sub_0808f9d8,
    .unk0C = 0,
    .unk10 = (u32)sub_0808f9f8,
    .unk14 = 0,
    .unk18 = 0,
};
/* gBroomHatterDef */
struct ActorVt gUnk_08743534 ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gLaserBallDef */
struct ActorVt gUnk_08743540 ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gCoconutDef */
struct ActorVt gUnk_0874354C ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_8e404.c */
struct ActorVt gUnk_08743558 ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gShotzoDef */
struct ActorVt gUnk_08743564 ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gConerDef */
struct ActorVt gUnk_08743570 ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gParasolShotzoDef */
struct ActorVt gUnk_0874357C ACTOR_TBL(087434c4) = {
    .unk00 = 0,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = (u32)sub_0808ece0,
};

/* ---- 0x087436E4-0x08743734: 4 record(s), section .actor_tbl_087436e4 ---- */
/* gLaserBallLaserDef */
struct ActorHandlers gUnk_087436E4 ACTOR_TBL(087436e4) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gShotzoCannonballDef */
struct ActorHandlers gUnk_08743700 ACTOR_TBL(087436e4) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gLaserBallLaserDef */
struct ActorVt gUnk_0874371C ACTOR_TBL(087436e4) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gShotzoCannonballDef */
struct ActorVt gUnk_08743728 ACTOR_TBL(087436e4) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x0874407C-0x08744118: 9 record(s), section .actor_tbl_0874407c ---- */
/* gBonkersDef */
struct ActorHandlers gUnk_0874407C ACTOR_TBL(0874407c) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08090f4c,
    .unk14 = 0,
    .unk18 = 0,
};
/* gPoppyBrosSrDef */
struct ActorHandlers gUnk_08744098 ACTOR_TBL(0874407c) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08091b60,
    .unk14 = 0,
    .unk18 = 0,
};
/* gBugzzyDef */
struct ActorHandlers gUnk_087440B4 ACTOR_TBL(0874407c) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080937d0,
    .unk14 = 0,
    .unk18 = (u32)sub_08093858,
};
/* gBonkersDef */
struct ActorVt gUnk_087440D0 ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)BonkersReactToDamage,
    .unk08 = (u32)BonkersReactToDefeat,
};
/* src/enemy_9000c.c */
struct ActorVt gUnk_087440DC ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)BonkersReactToDamage,
    .unk08 = 0,
};
/* gPoppyBrosSrDef */
struct ActorVt gUnk_087440E8 ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)PoppyBrosSrReactToDamage,
    .unk08 = (u32)PoppyBrosSrReactToDefeat,
};
/* src/enemy_9113c.c */
struct ActorVt gUnk_087440F4 ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)PoppyBrosSrReactToDamage,
    .unk08 = 0,
};
/* gBugzzyDef */
struct ActorVt gUnk_08744100 ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)BugzzyReactToDamage,
    .unk08 = (u32)BugzzyReactToDefeat,
};
/* src/enemy_91f9c.c */
struct ActorVt gUnk_0874410C ACTOR_TBL(0874407c) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)BugzzyReactToDamage,
    .unk08 = 0,
};

/* ---- 0x087442C8-0x0874433C: 7 record(s), section .actor_tbl_087442c8 ---- */
/* gBonkersNutDef */
struct ActorHandlers gUnk_087442C8 ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08093bb0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gPoppyBrosSrBombDef */
struct ActorHandlers gUnk_087442E4 ACTOR_TBL(087442c8) = {
    .unk00 = (u32)sub_08093f00,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08093edc,
    .unk14 = 0,
    .unk18 = 0,
};
/* gBonkersNutDef */
struct ActorVt gUnk_08744300 ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk01 = 1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_91f9c.c */
struct ActorVt gUnk_0874430C ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPoppyBrosSrBombDef */
struct ActorVt gUnk_08744318 ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk01 = 1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_91f9c.c */
struct ActorVt gUnk_08744324 ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gBugzzyLadybugDef */
struct ActorVt gUnk_08744330 ACTOR_TBL(087442c8) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x087453BC-0x08745458: 9 record(s), section .actor_tbl_087453bc ---- */
/* gGrandWheelieDef */
struct ActorHandlers gUnk_087453BC ACTOR_TBL(087453bc) = {
    .unk00 = (u32)sub_080954f0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080955a8,
    .unk14 = 0,
    .unk18 = 0,
};
/* gFireLionDef */
struct ActorHandlers gUnk_087453D8 ACTOR_TBL(087453bc) = {
    .unk00 = (u32)sub_08096d64,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08096df4,
    .unk14 = 0,
    .unk18 = 0,
};
/* gPhanPhanDef */
struct ActorHandlers gUnk_087453F4 ACTOR_TBL(087453bc) = {
    .unk00 = (u32)sub_08098528,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08098540,
    .unk14 = 0,
    .unk18 = 0,
};
/* gGrandWheelieDef */
struct ActorVt gUnk_08745410 ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)GrandWheelieReactToDamage,
    .unk08 = (u32)GrandWheelieReactToDefeat,
};
/* src/enemy_93f64.c */
struct ActorVt gUnk_0874541C ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)GrandWheelieReactToDamage,
    .unk08 = 0,
};
/* gFireLionDef */
struct ActorVt gUnk_08745428 ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)FireLionReactToDamage,
    .unk08 = (u32)FireLionReactToDefeat,
};
/* src/enemy_957bc.c */
struct ActorVt gUnk_08745434 ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)FireLionReactToDamage,
    .unk08 = 0,
};
/* gPhanPhanDef */
struct ActorVt gUnk_08745440 ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)PhanPhanReactToDamage,
    .unk08 = (u32)PhanPhanReactToDefeat,
};
/* src/enemy_974c8.c */
struct ActorVt gUnk_0874544C ACTOR_TBL(087453bc) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)PhanPhanReactToDamage,
    .unk08 = 0,
};

/* ---- 0x087455C8-0x08745618: 4 record(s), section .actor_tbl_087455c8 ---- */
/* gGrandWheelieMiniWheelieDef */
struct ActorHandlers gUnk_087455C8 ACTOR_TBL(087455c8) = {
    .unk00 = (u32)sub_080986ec,
    .unk04 = (u32)sub_080986ec,
    .unk08 = (u32)sub_08098718,
    .unk0C = 0,
    .unk10 = (u32)sub_08098728,
    .unk14 = 0,
    .unk18 = (u32)sub_08098738,
};
/* gPhanPhanAppleDef */
struct ActorHandlers gUnk_087455E4 ACTOR_TBL(087455c8) = {
    .unk00 = (u32)sub_080988a4,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080988b4,
    .unk14 = 0,
    .unk18 = 0,
};
/* gGrandWheelieMiniWheelieDef */
struct ActorVt gUnk_08745600 ACTOR_TBL(087455c8) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gPhanPhanAppleDef */
struct ActorVt gUnk_0874560C ACTOR_TBL(087455c8) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08745A3C-0x08745AA4: 6 record(s), section .actor_tbl_08745a3c ---- */
/* gMrFrostyDef */
struct ActorHandlers gUnk_08745A3C ACTOR_TBL(08745a3c) = {
    .unk00 = (u32)sub_080988f8,
    .unk04 = (u32)sub_080988c4,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08098a04,
    .unk14 = 0,
    .unk18 = 0,
};
/* gMrTickTockDef */
struct ActorHandlers gUnk_08745A58 ACTOR_TBL(08745a3c) = {
    .unk00 = (u32)sub_08099b20,
    .unk04 = (u32)sub_08099aec,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_08099c4c,
    .unk14 = 0,
    .unk18 = 0,
};
/* gMrFrostyDef */
struct ActorVt gUnk_08745A74 ACTOR_TBL(08745a3c) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrFrostyReactToDamage,
    .unk08 = (u32)MrFrostyReactToDefeat,
};
/* src/enemy_988f8.c */
struct ActorVt gUnk_08745A80 ACTOR_TBL(08745a3c) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrFrostyReactToDamage,
    .unk08 = 0,
};
/* gMrTickTockDef */
struct ActorVt gUnk_08745A8C ACTOR_TBL(08745a3c) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrTickTockReactToDamage,
    .unk08 = (u32)MrTickTockReactToDefeat,
};
/* src/enemy_99b20.c */
struct ActorVt gUnk_08745A98 ACTOR_TBL(08745a3c) = {
    .unk00 = -1,
    .unk01 = 6,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrTickTockReactToDamage,
    .unk08 = 0,
};

/* ---- 0x08745C90-0x08745CD4: 3 record(s), section .actor_tbl_08745c90 ---- */
/* gMrFrostyIceCubeDef */
struct ActorHandlers gUnk_08745C90 ACTOR_TBL(08745c90) = {
    .unk00 = (u32)sub_0809b454,
    .unk04 = (u32)sub_0809b4d8,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809b4dc,
    .unk14 = 0,
    .unk18 = 0,
};
/* gMrTickTockNoteDef */
struct ActorHandlers gUnk_08745CAC ACTOR_TBL(08745c90) = {
    .unk00 = (u32)sub_0809b9c0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809b9e0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gMrFrostyIceCubeDef; the 12 bytes after it, up to the next label, have no reference and stay structure-only data */
struct ActorVt gUnk_08745CC8 ACTOR_TBL(08745c90) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08745CE0-0x08745CEC: 1 record(s), section .actor_tbl_08745ce0 ---- */
/* gMrTickTockNoteDef */
struct ActorVt gUnk_08745CE0 ACTOR_TBL(08745ce0) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x087481A0-0x08748264: 11 record(s), section .actor_tbl_087481a0 ---- */
/* gUnk_08747D5C */
struct ActorHandlers gUnk_087481A0 ACTOR_TBL(087481a0) = {
    .unk00 = (u32)sub_0809d0a0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809d0dc,
    .unk14 = (u32)sub_0809d0dc,
    .unk18 = (u32)sub_0809d138,
};
/* gUnk_08747DB4 */
struct ActorHandlers gUnk_087481BC ACTOR_TBL(087481a0) = {
    .unk00 = (u32)sub_0809e7c8,
    .unk04 = (u32)sub_0809e7d4,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809e7e8,
    .unk14 = (u32)sub_0809e7e8,
    .unk18 = (u32)sub_0809e820,
};
/* gUnk_08747E0C */
struct ActorHandlers gUnk_087481D8 ACTOR_TBL(087481a0) = {
    .unk00 = (u32)sub_0809f52c,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809f588,
    .unk14 = (u32)sub_0809f588,
    .unk18 = (u32)sub_0809f618,
};
/* gUnk_08747E64 */
struct ActorHandlers gUnk_087481F4 ACTOR_TBL(087481a0) = {
    .unk00 = (u32)sub_0809db48,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_0809dbc4,
    .unk14 = (u32)sub_0809dbc4,
    .unk18 = (u32)sub_0809dc3c,
};
/* gUnk_08747D5C */
struct ActorVt gUnk_08748210 ACTOR_TBL(087481a0) = {
    .unk00 = -1,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809f7f8,
    .unk08 = 0,
};
/* gAxeKnightAxeDef */
struct ActorVt gUnk_0874821C ACTOR_TBL(087481a0) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_08747DB4 */
struct ActorVt gUnk_08748228 ACTOR_TBL(087481a0) = {
    .unk00 = -1,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809f7f8,
    .unk08 = 0,
};
/* gUnk_08747E0C */
struct ActorVt gUnk_08748234 ACTOR_TBL(087481a0) = {
    .unk00 = -1,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809f7f8,
    .unk08 = 0,
};
/* gTridentKnightTridentDef */
struct ActorVt gUnk_08748240 ACTOR_TBL(087481a0) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_08747E64 */
struct ActorVt gUnk_0874824C ACTOR_TBL(087481a0) = {
    .unk00 = -1,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809f7f8,
    .unk08 = 0,
};
/* gJavelinKnightJavelinDef */
struct ActorVt gUnk_08748258 ACTOR_TBL(087481a0) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08748930-0x087489A4: 7 record(s), section .actor_tbl_08748930 ---- */
/* gKingDededeDef */
struct ActorHandlers gUnk_08748930 ACTOR_TBL(08748930) = {
    .unk00 = (u32)sub_080a0538,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080a0588,
    .unk14 = 0,
    .unk18 = (u32)sub_080a0598,
};
/* gMrShineAndMrBrightDef, gUnk_087487BC */
struct ActorHandlers gUnk_0874894C ACTOR_TBL(08748930) = {
    .unk00 = (u32)sub_080a1df8,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080a1e4c,
    .unk14 = 0,
    .unk18 = 0,
};
/* gKingDededeDef */
struct ActorVt gUnk_08748968 ACTOR_TBL(08748930) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809fe10,
    .unk08 = (u32)sub_0809fd64,
};
/* src/enemy_9fbd0.c */
struct ActorVt gUnk_08748974 ACTOR_TBL(08748930) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_0809fe10,
    .unk08 = (u32)sub_0809fd64,
};
/* gMrShineAndMrBrightDef */
struct ActorVt gUnk_08748980 ACTOR_TBL(08748930) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrShineAndMrBrightReactToDamage,
    .unk08 = (u32)MrShineAndMrBrightReactToDefeat,
};
/* src/enemy_a1590.c */
struct ActorVt gUnk_0874898C ACTOR_TBL(08748930) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_087487BC */
struct ActorVt gUnk_08748998 ACTOR_TBL(08748930) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MrShineAndMrBrightReactToDamage,
    .unk08 = (u32)MrShineAndMrBrightReactToDefeat,
};

/* ---- 0x08748CF8-0x08748D28: 4 record(s), section .actor_tbl_08748cf8 ---- */
/* gKingDededeStarDef */
struct ActorVt gUnk_08748CF8 ACTOR_TBL(08748cf8) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_a1590.c */
struct ActorVt gUnk_08748D04 ACTOR_TBL(08748cf8) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_08748B08, gUnk_08748B34 */
struct ActorVt gUnk_08748D10 ACTOR_TBL(08748cf8) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_08748B60, gUnk_08748B8C */
struct ActorVt gUnk_08748D1C ACTOR_TBL(08748cf8) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08749B08-0x08749B6C: 7 record(s), section .actor_tbl_08749b08 ---- */
/* gMetaKnightDef */
struct ActorHandlers gUnk_08749B08 ACTOR_TBL(08749b08) = {
    .unk00 = 0,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080a7334,
    .unk14 = 0,
    .unk18 = 0,
};
/* gMetaKnightDef */
struct ActorVt gUnk_08749B24 ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)MetaKnightReactToDamage,
    .unk08 = (u32)MetaKnightReactToDefeat,
};
/* src/enemy_a5644.c */
struct ActorVt gUnk_08749B30 ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gKrackoDef */
struct ActorVt gUnk_08749B3C ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)KrackoReactToDamage,
    .unk08 = (u32)KrackoReactToDefeat,
};
/* src/enemy_a93ec.c */
struct ActorVt gUnk_08749B48 ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gNightmareWizardDef */
struct ActorVt gUnk_08749B54 ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)NightmareWizardReactToDamage,
    .unk08 = (u32)NightmareWizardReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gUnk_08749B60 ACTOR_TBL(08749b08) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x08749CD4-0x08749CEC: 2 record(s), section .actor_tbl_08749cd4 ---- */
/* gKrackoStarmanDef */
struct ActorVt gUnk_08749CD4 ACTOR_TBL(08749cd4) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gNightmareWizardStarDef */
struct ActorVt gUnk_08749CE0 ACTOR_TBL(08749cd4) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x0874B4E0-0x0874B51C: 5 record(s), section .actor_tbl_0874b4e0 ---- */
/* gPaintRollerDef */
struct ActorVt gUnk_0874B4E0 ACTOR_TBL(0874b4e0) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)PaintRollerReactToDamage,
    .unk08 = (u32)PaintRollerReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gUnk_0874B4EC ACTOR_TBL(0874b4e0) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gHeavyMoleDef */
struct ActorVt gUnk_0874B4F8 ACTOR_TBL(0874b4e0) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)HeavyMoleReactToDamage,
    .unk08 = (u32)HeavyMoleReactToDefeat,
};
/* src/enemy_aa338.c */
struct ActorVt gUnk_0874B504 ACTOR_TBL(0874b4e0) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gNightmarePowerOrbDef; the 12 bytes after it, up to the next label, have no reference and stay structure-only data */
struct ActorVt gUnk_0874B510 ACTOR_TBL(0874b4e0) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)NightmarePowerOrbReactToDamage,
    .unk08 = (u32)NightmarePowerOrbReactToDefeat,
};

/* ---- 0x0874BF58-0x0874BFF8: 12 record(s), section .actor_tbl_0874bf58 ---- */
/* 6 ActorDefs (gUnk_0874B96C, gUnk_0874B998, ...) */
struct ActorHandlers gUnk_0874BF58 ACTOR_TBL(0874bf58) = {
    .unk00 = (u32)sub_080b1588,
    .unk04 = (u32)sub_080b1594,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = (u32)sub_080b15b4,
    .unk14 = 0,
    .unk18 = 0,
};
/* gUnk_0874B96C */
struct ActorVt gUnk_0874BF74 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874B998 */
struct ActorVt gUnk_0874BF80 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874B9C4 */
struct ActorVt gUnk_0874BF8C ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874B9F0 */
struct ActorVt gUnk_0874BF98 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874BA1C */
struct ActorVt gUnk_0874BFA4 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874BA48 */
struct ActorVt gUnk_0874BFB0 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874BA74 */
struct ActorVt gUnk_0874BFBC ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gUnk_0874BAA0 */
struct ActorVt gUnk_0874BFC8 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 0,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* src/enemy_ae3bc.c */
struct ActorVt gUnk_0874BFD4 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 3,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gHeavyMoleYellowMissileDef */
struct ActorVt gUnk_0874BFE0 ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gHeavyMoleRedMissileDef */
struct ActorVt gUnk_0874BFEC ACTOR_TBL(0874bf58) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};

/* ---- 0x0874C204-0x0874C21C: 2 record(s), section .actor_tbl_0874c204 ---- */
/* gWhispyWoodsDef */
struct ActorVt gUnk_0874C204 ACTOR_TBL(0874c204) = {
    .unk00 = -1,
    .unk01 = -1,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_080b2574,
    .unk08 = (u32)sub_080b2550,
};
/* src/enemy_ae3bc.c */
struct ActorVt gUnk_0874C210 ACTOR_TBL(0874c204) = {
    .unk00 = -1,
    .unk01 = 7,
    .filler02 = { 0x00, 0x00 },
    .unk04 = (u32)sub_080b2574,
    .unk08 = (u32)sub_080b2550,
};

/* ---- 0x0874C418-0x0874C44C: 3 record(s), section .actor_tbl_0874c418 ---- */
/* gWhispyWoodsAppleDef */
struct ActorHandlers gUnk_0874C418 ACTOR_TBL(0874c418) = {
    .unk00 = (u32)sub_080b2f38,
    .unk04 = 0,
    .unk08 = 0,
    .unk0C = 0,
    .unk10 = 0,
    .unk14 = 0,
    .unk18 = 0,
};
/* gWhispyWoodsAppleDef */
struct ActorVt gUnk_0874C434 ACTOR_TBL(0874c418) = {
    .unk00 = 0,
    .unk01 = 8,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
/* gWhispyWoodsAirPuffDef */
struct ActorVt gUnk_0874C440 ACTOR_TBL(0874c418) = {
    .unk00 = 0,
    .unk01 = 4,
    .filler02 = { 0x00, 0x00 },
    .unk04 = 0,
    .unk08 = 0,
};
