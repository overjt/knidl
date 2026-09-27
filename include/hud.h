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
extern s16 gUnk_02007D30;
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

#endif /* GUARD_HUD_H */
