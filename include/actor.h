#ifndef GUARD_ACTOR_H
#define GUARD_ACTOR_H

#include "gba/types.h"
#include "task.h"

/* actor.h: the RAM cells and ROM tables of the actor core: the struct Task
   field API (M17) and the player-state task bodies (M18).  One declaration
   per symbol, with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct ActorDef;

/* The 0x0824A9E4 record sub_08070648 uploads from (two tile-count halfwords
   plus two source pointers). */
struct GfxSrc
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02;
    /*0x04*/ u32 unk04;
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
};

struct Unk0873EAC0
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
};

/* EWRAM */
extern u16 gNextActorSerial;
extern u32 gUnk_02004C90;
extern u16 gUnk_020055C0;
extern s32 gUnk_02006040[];
extern s8 gCannonFuseState;
extern u8 gUnk_02006178;
extern s32 gUnk_02006190[];
extern s8 gPaletteAnimTasks[];
extern u8 gUnk_02007CF4[];
extern s32 gUnk_02007D00[];
extern u32 gUnk_02007F50;
extern s8 gUnk_02007FB8[];
extern u32 gUnk_0200AEF4;
extern u8 gUnk_0200AFF8;
extern u8 gUnk_0200B030;
extern struct Actor gActors[];

/* IWRAM */
extern u32 gUnk_03001610[];
extern u8 gUnk_03001F34;
extern struct PlayerState gPlayerStates[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern u8 gUnk_030023F0;

/* ROM */
extern struct GfxSrc gUnk_0824A9E4;
extern u8 gUnk_0825088C[];
extern u8 gUnk_082530C8[];
extern u8 gUnk_0825CA44[];
extern u32 gUnk_08334DC0[];
extern s16 gUnk_0873D384[];
extern s16 gUnk_0873D420[][3];
extern s32 gUnk_0873DF14[];
extern u32 gPaletteAnimVariants[];
extern u32 gUnk_0873DF38[][4];
extern u8 gUnk_0873DF78[];
extern u32 gUnk_0873DF7C[][3];
extern u8 gUnk_0873DFAC[];
extern u16 gUnk_0873E16C[];
extern s16 gUnk_0873E184[];
extern s8 gUnk_0873E1B4[];
extern s32 gUnk_0873E1B8[];
extern s16 gUnk_0873E1E8[];
extern s8 gUnk_0873E1F8[];
extern s16 gUnk_0873E220[];
extern u32 gUnk_0873E264[];
extern u32 gUnk_0873E280[];
extern u32 gUnk_0873E284[];
extern u32 gUnk_0873E2F0[];
extern u32 gUnk_0873E31C[];
extern s32 gUnk_0873E348[];
extern s32 gUnk_0873E388[];
extern u16 gUnk_0873E3C8[];
extern u16 gUnk_0873E58C[];
extern s16 gUnk_0873E5A4[];
extern u32 gUnk_0873E5BC[];
extern u32 gActorDefeatsByEffect[];
extern s16 gUnk_0873E5F8[];
extern u16 gUnk_0873E610[];
extern u16 gUnk_0873E620[];
extern u16 gUnk_0873E634[];
extern u16 gUnk_0873E640[];
extern u32 gUnk_0873E670[];
extern u32 gUnk_0873E67C[];
extern u32 gUnk_0873E688[];
extern u16 gUnk_0873E698[];
extern u16 gUnk_0873E69A;
extern u16 gUnk_0873E69C;
extern u32 gUnk_0873E6A0[];
extern u32 gUnk_0873E6D0[];
extern u16 gUnk_0873E700[];
extern u16 gUnk_0873E72E;
extern u16 gUnk_0873E730;
extern u32 gUnk_0873E734[];
extern u8 gUnk_0873E758[];
extern u8 gUnk_0873E77C[];
extern u32 gUnk_0873E78C[];
extern u8 gUnk_0873E798[];
extern s16 gUnk_0873E7A4[];
extern s16 gUnk_0873E7C4[];
extern s16 gUnk_0873E864[];
extern u32 gActorAttachedStates[];
extern struct Unk0873EAC0 gUnk_0873EAC0[];
extern u16 gUnk_0873EAD8[][4];
extern s16 gUnk_0873EAF0[];
extern s8 gUnk_0873EB30[];
extern s8 gUnk_0873EB38[];
extern s16 gUnk_0873EB40[];
extern s16 gUnk_0873EB60[];
extern s16 gUnk_0873EB80[];
extern u32 gUnk_0873EBA0[];
extern u32 gUnk_0873EC20[];
extern s32 gUnk_0873ECA0[];
extern s32 gUnk_0873ECC0[];
extern u16 gUnk_0873ECD0[];
extern u32 gUnk_0873ECE0[];
extern struct ActorDef *gUnk_0873ECEC[];
extern struct ActorDef *gMidBossDefs[];
extern struct ActorDef *gBossDefs[];
extern struct ActorDef *gUnk_0873EDDC[];
extern struct ActorDef *gUnk_0873EE70[];
extern struct ActorDef *gUnk_0873EE88[];
extern u32 *gUnk_0873EF74[];
extern u32 gUnk_0873F01C[];
extern u32 *gUnk_0873F0C4[];
extern u32 *gUnk_0873F118[];
extern u32 *gUnk_0873F138[];
extern u32 gUnk_0873F198[];
extern u32 gMidBossTaskTypes[];
extern u32 gBossTaskTypes[];
extern u32 gUnk_0873F288[];
extern u32 gUnk_0873F2A0[];
extern u8 gUnk_0873F5D4[];
extern struct ActorDef gUnk_0873F690;
extern struct ActorDef gUnk_0873F6BC;
extern struct ActorDef gUnk_0873F6E8;
extern struct ActorDef gUnk_0873F704;
extern u8 gUnk_0873F7E4[];
extern u8 gUnk_0873F81C[];
extern u32 gUnk_0873F830[];
extern u32 gUnk_0873F844[];
extern u32 gUnk_0873F858[];
extern u32 gUnk_0873F86C[];
extern u8 gUnk_0873F880[];
extern u32 gUnk_0873F894[];
extern u32 gUnk_0873F8B4[];
extern u32 gUnk_0873F8BC[];
extern u32 gUnk_0873F8CC[];
extern u32 gUnk_0873F8DC[];
extern u32 gUnk_0873F8F4[];
extern u32 gUnk_0873F910[];
extern u32 gUnk_0873F92C[];
extern u32 gUnk_0873F938[];
extern s16 *gUnk_0873F950[];
extern u16 gUnk_0873FAB4[];
extern u8 gUnk_0873FAE8[];
extern u32 gUnk_0873FB04[];
extern u32 gUnk_0873FB24[];
extern u32 gUnk_0873FB44[];
extern u32 gUnk_0873FB60[];
extern u32 gUnk_0874C520[];
extern u32 gUnk_0874C9D8[];
extern u32 gUnk_0874CA1C[];
extern u32 gUnk_0874CA78[];
extern u32 gUnk_0874CAD8[];
extern u32 gUnk_0874CB3C[];
extern u32 gUnk_0874CB7C[];
extern u32 gUnk_0874CB90[];
extern u32 gUnk_0874CBC8[];
extern u32 gUnk_0874CBD0[];
extern u32 gUnk_0874CC38[];
extern u32 gUnk_0874CC48[];
extern u32 gUnk_0874CC60[];
extern u32 gUnk_0874CC84[];
extern u32 gUnk_0874CCA4[];
extern u32 gUnk_0874CCBC[];
extern u32 gParasolFrames[];
extern u32 gUnk_08752D20[];
extern u32 gWarpStarFrames[];
extern u32 gUnk_08752E48[];

#endif /* GUARD_ACTOR_H */
