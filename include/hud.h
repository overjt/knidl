#ifndef GUARD_HUD_H
#define GUARD_HUD_H

#include "gba/types.h"

/* hud.h: the RAM cells and ROM tables of the HUD (M02) and the HUD/overlay
   effects (M33).  One declaration per symbol, with the type its consumers
   prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxHeader;

struct HudBar
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
};

struct Unk020060A0
{
    /*0x00*/ s8 unk0;
    /*0x01*/ s8 unk1;
    /*0x02*/ s16 unk2;
};

struct Unk0200D120
{
    /*0x00*/ u8 filler00[0x05];
    /*0x05*/ u8 hitState;
    /*0x06*/ u8 filler06[0x1A];
    /*0x20*/ u16 savedTileWord;
    /*0x22*/ u8 filler22[0x26];
    /*0x48*/ s8 *attackBox;
    /*0x4C*/ u8 filler4C[0x24];
};

/* EWRAM */
extern u16 gUnk_0200000C;
extern u8 gHudTilemapDirty;
extern u8 gUnk_02000034;
extern s8 gHudHpBarIndex;
extern s16 gHudHpBarMaxHp;
extern s8 gUnk_020055D0;
extern s8 gUnk_020055F0[];
extern u16 gHudTilemap[];
extern u32 gUnk_02005F10[];
extern u8 gHudMode;
extern s32 gPlayerScores[]; /* score per player */
extern u8 gHudShowsClock;
extern u16 gHudClock[]; /* clock (four fields) */
extern struct Unk020060A0 gUnk_020060A0[];
extern s8 gUnk_02006130[];
extern s8 gUnk_0200617C;
extern s8 gHudAbilityPanelState;
extern struct HudBar gHudHpBars[];
extern s16 gHudHpBarLength;
extern u16 gUnk_02007D40;
extern s16 gHudHpBarValues[];
extern s16 gUnk_02008014[];
extern s16 gUnk_0200801C;
extern u8 gUnk_02008020[];
extern u8 gHudShowsHpBar;
extern struct Unk0200D120 gUnk_0200D120[];

/* IWRAM */
extern u32 gUnk_030015B0[];
extern u32 gUnk_030027A8[];

/* ROM */
extern u16 gUnk_085A5654[];
extern u8 gUnk_085A6714[];
extern u16 gHudDigitTiles[2][10]; /* digit tiles, top and bottom rows */
extern u16 gUnk_085A6F5C[]; /* the clock's colon tiles */
extern u16 gUnk_085A6F60[];
extern u16 gUnk_085A6F64[];
extern u16 gUnk_085A6F68[][5];
extern u16 gHudPlayerIconTiles[2][4][2];
extern u16 gUnk_085A6FC4[];
extern u16 gUnk_085A6FC8[];
extern u16 gUnk_085A6FF0[];
extern u16 gUnk_085A6FF8[];
extern u16 gUnk_085A6FFC[];
extern u16 gUnk_08731CE6[];
extern u32 gUnk_087555D8[];
extern struct GfxHeader *gUnk_087555FC[];


/* Functions (defined in the files named above each group). */

/* src/hud_099fc.c */
void Task_IntroStoryPicture(void);
void HudShowScore(void);
void HudShowClock(void);
void HudReset(void);
void HudInit(s32 i);
void HudRedraw(s32 i);
void sub_08009e14(void);
void sub_08009e20(void);
s32 AddPlayerLives(s32 a, u32 b);
s32 AddPlayerLivesNoHud(s32 a, u32 b);
s32 AddPlayerHealth(s32 a, u32 b);
s32 SetPlayerAbilityNoHud(s32 a, s32 b, u32 c);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);
void AddPlayerScore(s32 a, u32 b);
void AddPlayerScoreNoHud(s32 a, u32 b);
void HudShowAbilityAnimated(s32 a, s32 b);

/* src/hud_0a130.c */
void HudShowAbility(s32 a, s32 id);
void sub_0800a178(s32 a, s32 id);
void sub_0800a19c(s32 id);
void sub_0800a21c(s32 id);
void HudShowHpBar(void);
void HudAnimateTaskHpBar(void);
void HudSetTaskHpBar(void);
void HudRemoveHpBar(void);
void sub_0800a698(void);
void sub_0800a6a4(void);
void HudUpdateAbilityPanel(void);
void HudUpdateHpBars(void);
void HudAnimateHpBar(s32 from, s32 to, s32 i);
s32 sub_0800aa18(s32 from, s32 to);
void HudSetHpBar(s32 x, s32 i);
void HudResetHpBar(s32 i);
void sub_0800aaac(s32 i);

/* src/hud_0aad0.c */
void sub_0800aad0(void);
void HudRedrawClock(void);
void sub_0800ab3c(void);
void HudDrawPlayerIcon(s32 a);
void HudDrawLives(s32 n);
void HudDrawHealth(s32 n);
void sub_0800acbc(s32 a, s32 d);
void HudDrawScore(s32 v);
void HudDrawClock(u16 *time);
void HudDrawAbilityPanel(s32 n);
void sub_0800b0fc(void);
void HudDrawHpBar(s32 x);
void sub_0800b190(s32 from, s32 to);
void sub_0800b230(s32 a, s32 b);

/* src/hud_0b318.c */
void HudClearTiles(s32 x, s32 y, s32 n);
void HudClearWholeTilemap(void);
void HudClearTilemap(void);
void HudFlushTilemap(void);
void HudLoadGfx(void);

/* src/hud_b2fe8.c */
void sub_080b2fe8(void);
void sub_080b3010(u8 a);
void Task_WhispyWoodsApple(void);
void sub_080b3090(void);
void sub_080b30c8(void);
void sub_080b3110(void);
void sub_080b312c(void);
void sub_080b319c(void);
void sub_080b31a0(void);
void sub_080b31e0(void);
void sub_080b3214(void);
void sub_080b3258(void);
void sub_080b328c(void);
void sub_080b32d0(void);
void Task_WhispyWoodsAirPuff(void);
void sub_080b3368(void);
void sub_080b3398(void);
void sub_080b33bc(void);
void sub_080b33d8(void);
void sub_080b3758(void);
void sub_080b37ec(void);
void sub_080b38f0(void);
void sub_080b3a00(void);
void WhispyWoodsLeavesDraw(void);
void Task_WhispyWoodsLeaves(void);
void WhispyWoodsLeavesUpdate(void);
void Task_OneUp(void);
void Task_MaximTomato(void);
void Task_InvincibleCandy(void);
void Task_EnergyDrink(void);
void sub_080b3f54(void);
void sub_080b3fcc(void);
void sub_080b3ffc(void);
s32 sub_080b404c(void);
s32 sub_080b406c(void);
s32 sub_080b408c(void);
void PickupInit(void);
void PickupUpdate(void);
void PickupEnterState(void);
void sub_080b4174(void);
void sub_080b4190(void);
void sub_080b4194(void);
void sub_080b41c8(void);
void sub_080b41cc(void);
void sub_080b4200(void);
s32 sub_080b4204(u32 a);
void PickupHeal(void);
void MaximTomatoHeal(void);
void EnergyDrinkHeal(void);
s32 sub_080b4390(void);
s32 sub_080b43d4(void);
s32 sub_080b43f4(void);
s32 sub_080b442c(void);
void sub_080b447c(void);
void sub_080b44f0(void);
s32 sub_080b4524(void);
s32 sub_080b45c0(void);
void sub_080b460c(void);
void sub_080b4648(void);
void Task_AbilityStar(void);
void sub_080b4714(void);
void sub_080b4754(void);
void sub_080b4770(void);
void sub_080b4788(void);
void sub_080b4794(void);
void sub_080b47c0(void);
void Task_StarRodPiece(void);
void sub_080b480c(void);
void sub_080b4878(void);
void sub_080b48e0(void);
void sub_080b48f8(void);
void sub_080b4968(void);
void sub_080b4a34(void);
void sub_080b4a5c(void);
void sub_080b4a8c(void);
void sub_080b4afc(void);
void sub_080b4b18(void);
void sub_080b4b94(void);
void sub_080b4bb0(void);
void sub_080b4bd8(void);
void sub_080b4be4(void);
void sub_080b4c14(void);
void sub_080b4c84(void);
void sub_080b4ca0(void);
void sub_080b4d1c(void);
void sub_080b4d50(void);
void sub_080b4db4(void);
void sub_080b4dd0(void);
void sub_080b4df8(void);
void sub_080b4e04(void);
void sub_080b4e40(void);

/* src/hud_b4ea8.c */
void sub_080b4ea8(void);

/* src/hud_b5024.c */
void sub_080b5024(void);
s32 sub_080b5338(s32 i);
void sub_080b54d0(s32 a);
void sub_080b5540(s32 a, s32 b);
void sub_080b5558(void);
s32 sub_080b55d8(u32 a, u32 b);
s32 sub_080b5628(u32 a);
s32 sub_080b5654(u32 a);

/* src/hud_b5840.c */
void HBlankScrollVBlankCallback(void);
void UpdateHBlankScroll(void);

#endif /* GUARD_HUD_H */
