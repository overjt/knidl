#ifndef GUARD_ROOM_H
#define GUARD_ROOM_H

#include "gba/types.h"

/* room.h: the RAM cells and ROM tables of the level / room builder, the doors
   and the stage helpers (M07).  One declaration per symbol, with the type its
   consumers prove (issue #36 phase 2, docs/header-conventions.md). */

struct Unk020055D8Entry;

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 width;
    /*0x04*/ u16 height;
    /*0x06*/ u16 unk6[0];
};

/* M08's per-player camera positions (src/camera_28b8c.c) */
struct CamPos { u16 x, y; };

struct CamRect { s16 x0, x1, y0, y1; };

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};

/* The room header gCurRoomDef points at (one entry of the gRoomTable
   room table): unk18/unk28 are length-prefixed palettes, unk30 the BG map
   streamed into 0x06003000, unk40 the room's BG animation script set. */
struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 bgm;
    /*0x05*/ u8 mapsCompressed;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *metatileMap;
    /*0x0C*/ void *blockLayer;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 width;
    /*0x16*/ u16 height;
    /*0x18*/ u16 *bg2Palette;
    /*0x1C*/ void *bg2Tiles;
    /*0x20*/ void *metatileTiles;
    /*0x24*/ u16 borderX;
    /*0x26*/ u16 borderY;
    /*0x28*/ u16 *bg3Palette;
    /*0x2C*/ void *bg3Tiles;
    /*0x30*/ struct BgMap *bg3Map;
    /*0x34*/ u16 bg3BorderX;
    /*0x36*/ u16 bg3BorderY;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 doorCount;
    /*0x3C*/ u16 objectCount;
    /*0x3E*/ u16 objectsSortedByY;
    /*0x40*/ u16 bgAnimSet;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *doors;
    /*0x48*/ void *objects;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 entryX;
    /*0x52*/ u16 entryY;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

struct Unk020055D8
{
    /*0x00*/ s16 count;
    /*0x02*/ s16 sortedByY;
    /*0x04*/ struct Unk020055D8Entry *entries;
};

struct Unk020061F0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ struct MapTile *unk8;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
    /*0x1C*/ s8 unk1C;
    /*0x1D*/ u8 filler1D[3];
};

struct Unk03005670
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
};

struct Unk03005680
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 lockedAxes;
    /*0x02*/ u16 x0;
    /*0x04*/ u16 x1;
    /*0x06*/ u16 y0;
    /*0x08*/ u16 y1;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
};

/* EWRAM */
extern s8 gUnk_02000000;
extern u16 gUnk_0200001C;
extern u8 gUnk_02000020;
extern u16 gUnk_02000030;
extern struct MapCell gRoomMapBuffer[];
extern u16 gPlayerAbilities[];
extern u8 gUnk_02004B64;
extern s16 gObjectSpawnViewRect[4];
extern u16 gUnk_02004B80;
extern struct Unk02004B90 gDoorStates[];
extern s8 gUnk_02004C98;
extern u16 gBg1MetatileMap[];
extern u8 gUnk_02005574[];
extern u8 gUnk_02005578;
extern s16 gMaxHealth;
extern s16 gPlayerHealth[]; /* health per player (M02's HUD) */
extern s8 gUnk_02005590[];
extern u8 gBg3MapShape;
extern s16 gUnk_020055B8[4];
extern u8 gUnk_020055C8;
extern s16 gUnk_020055D4;
extern struct Unk020055D8 gRoomObjectList;
extern s16 gRoomEntryX;
extern u8 gUnk_020055E8;
extern s8 gUnk_02006098[];
extern s8 gSubGameLevel;
extern u8 gRoomEntrySet;
extern struct Unk020061F0 gBreakingBlocks[];
extern u8 gUnk_020069F0;
extern s8 gDoorObjectTasks[][3];
extern struct MapCell gUnk_02006AA0[];
extern u32 gUnk_02007BF0[8][8];
extern u8 gUnk_02007CF0;
extern u8 gUnk_02007D38;
extern s16 gPlayerLives[];
extern u16 gUnk_02007D50;
extern u8 gUsedSubGameDoors[];
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern u8 gUnk_02007E8C;
extern u16 gUnk_02007FA0; /* the block sub_08030b14 broke: x (pixels) */
extern u16 gSavedPlayerAbilityUses[];
extern u16 gUnk_02007FB0;
extern u8 gUnk_02007FB4;
extern u8 gCutscenePending;
extern u16 gUnk_02007FF0;
extern s8 gUnk_02007FF8;
extern u8 gUnk_02008000;
extern u16 gSavedPlayerAbilities[];
extern u16 gUnk_02008050;
extern u16 gUnk_02008054;
extern u16 gUnk_02008060[];
extern u16 gBlockLayer[]; /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern s16 gRoomEntryY;
extern u8 gUnk_0200AF00;
extern u8 gUnk_0200AF04;
extern u8 gUnk_0200AF08;
extern s16 gUnk_0200AF0C;
extern u16 gPlayerAbilityUses[];
extern s16 gUnk_0200AFE0[4];
extern u16 gUnk_0200AFF4;
extern s8 gUnk_0200B02C;
extern s8 gUnk_0200B034;
extern s8 gUnk_0200B038;
extern u8 gHBlankScrollStarted;
extern u8 gUnk_0200B04C;
extern u8 gRoomBgLayout;
extern u8 gUnk_0200B078;
extern u16 gMetatileTiles[];
extern u8 gUnk_0200D080;

/* IWRAM */
extern u16 gUnk_030012B0[];
extern u16 gUnk_03001370[];
extern s16 gCameraAnchorY;
extern u32 gUnk_03001F10;
extern s8 gUnk_03001F20;
extern s32 gUnk_03001F2C; /* boot_091ac.c spelling */
extern u8 gUnk_03001F30; /* set only by the mode list's fifth row
                                       (src/menu_0ca10.c); not link play */
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern u8 gUnk_0300234C;
extern u8 gActivePlayerCount;
extern u16 gMilestoneFlags;
extern s8 gUnk_03002384;
extern s16 gCameraFocusY;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern s8 gUnk_030023B8;
extern u16 gLatchedPressedKeys[]; /* newly-pressed keys, latched per player */
extern u32 gBigSwitchFlags[];
extern s16 gCameraFocusX;
extern u16 gGameState; /* current game state (main dispatch) */
extern s8 gUnk_030023E0;
extern s32 gCurSaveSlot;
extern s8 gStageIndex;
extern u8 gUnk_03002400[8][7];
extern s8 gStageRequest; /* stage request (M02) */
extern s8 gUnk_03002444;
extern s32 gUnk_03002448;
extern u16 gLatchedHeldKeys[]; /* latched state mask per player (M11) */
extern u8 gExtraMode;
extern s8 gRoomIndex;
extern s16 *gCurTileDrifts;
extern u16 gCameraMode;
extern struct CamPos gPlayerCameraPos[4];
extern s16 gRoomMetatileCount;
extern s32 gBg3ParallaxX;
extern struct RoomDef *gCurRoomDef; /* the current room header */
extern s32 gRoomDriftVelX;
extern u16 gPlayerGroupCenter[2];
extern s16 gCameraBounds[4];
extern u16 gRoomBorder[2];
extern u16 gCameraPos[2];
extern u16 gBg3Border[2];
extern u8 gPlayerCameraMode[];
extern s32 gScrollLockSpeedX;
extern s32 gCameraCenterX;
extern s32 gRoomDriftVelY;
extern s16 gRoomHeight; /* map height in metatiles */
extern s16 gRoomWidth; /* map width in metatiles */
extern u16 gRoomUpdateFlags;
extern s16 gRoomBounds[];
extern s32 gBg3ParallaxY;
extern s32 gCameraCenterY;
extern struct CamRect gPlayerBounds[4];
extern struct MapCell *gRoomMap; /* the room's metatile map */
extern s32 gScrollLockSpeedY;
extern u16 gBg3StreamPos[2];
extern u16 gCameraStreamPos[2];
extern struct Unk03005670 gScreenShake;
extern struct Unk03005680 gScrollLock;
extern u16 gBg3Pos[2];

/* ROM */
extern u8 gUnk_08334EB4[];
extern s32 gRoomDriftVelocities[][2];
extern s8 gUnk_08732302[][6];
extern u32 gUnk_0873232C[];
extern u16 gUnk_08732348[][9];
extern u8 gUnk_087323E2[][3][2];
extern u16 *gUnk_0873240C[];
extern s16 gUnk_087325A2[];
extern void (*gRoomTaskVariants[])(void);
extern u8 gUnk_08732630[];
extern u16 gUnk_08732638[][2];
extern u8 gUnk_0873264C[][2];
extern s16 gTileDrifts[];
extern s16 gTileDriftsDoubled[];
extern u32 gUnk_0874CDF8[];
extern struct RoomDef **gRoomTable[][8];

#endif /* GUARD_ROOM_H */
