#ifndef GUARD_ROOM_H
#define GUARD_ROOM_H

#include "gba/types.h"
#include "constants/camera.h"
#include "constants/game_states.h"
#include "constants/rooms.h"

/* room.h: the RAM cells and ROM tables of the level / room builder, the doors
   and the stage helpers (M07).  One declaration per symbol, with the type its
   consumers prove (issue #36 phase 2, docs/header-conventions.md). */

struct RoomObjectEntry;

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
    /*0x00*/ u8 metatileLo;
    /*0x01*/ u8 metatileHi;
    /*0x02*/ u8 slopeIndex;
    /*0x03*/ u8 collisionTile;
};

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 slopeIndex;
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
    /*0x10*/ void *blockMetatiles;
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
    /*0x38*/ u16 driftObjectIndex;
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
    /*0x55*/ u8 bg3FullShake;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct DoorState
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

struct RoomObjectList
{
    /*0x00*/ s16 count;
    /*0x02*/ s16 sortedByY;
    /*0x04*/ struct RoomObjectEntry *entries;
};

struct BreakingBlock
{
    /*0x00*/ u16 cellX;
    /*0x02*/ u16 cellY;
    /*0x04*/ u16 mapIndex;
    /*0x06*/ u16 scriptPos;
    /*0x08*/ struct MapTile *metatileCursor;
    /*0x0C*/ u16 *bgMapEntry;
    /*0x10*/ u16 *script;
    /*0x14*/ u16 waitFrames;
    /*0x16*/ u16 metatile;
    /*0x18*/ u16 collisionTile;
    /*0x1A*/ u16 chainAttack;
    /*0x1C*/ s8 breakerPlayer;
    /*0x1D*/ u8 filler1D[3];
};

struct ScreenShake
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
};

struct ScrollLock
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
extern s8 gRoomBg3FullShake;
extern u16 gHubUnlockSource;
extern u8 gRoomPlayerMode;
extern u16 gFoundDoor;
extern struct MapCell gRoomMapBuffer[];
extern u16 gPlayerAbilities[];
extern u8 gBigSwitchPressActive;
extern s16 gObjectSpawnViewRect[4];
extern u16 gBigSwitchReturnBg3X;
extern struct DoorState gDoorStates[];
extern s8 gWarpStarStationDirection;
extern u16 gBg1MetatileMap[];
extern u8 gUnk_02005574[];
extern u8 gContinueLevel;
extern s16 gMaxHealth;
extern s16 gPlayerHealth[]; /* health per player (M02's HUD) */
extern s8 gUnk_02005590[];
extern u8 gBg3MapShape;
extern s16 gBlockAnimClipRect[4];
extern u8 gRoomBgmStarted;
extern s16 gUnk_020055D4;
extern struct RoomObjectList gRoomObjectList;
extern s16 gRoomEntryX;
extern u8 gCameraPanDone;
extern s8 gUnk_02006098[];
extern s8 gSubGameLevel;
extern u8 gRoomEntrySet;
extern struct BreakingBlock gBreakingBlocks[];
extern u8 gRoomEntryMode;
extern s8 gDoorObjectTasks[][3];
extern struct MapCell gHubRoomMapBuffer[];
extern u32 gUsedRoomObjects[8][8];
extern u8 gRoomExitKind;
extern u8 gCameraFocusPlayer;
extern s16 gPlayerLives[];
extern u16 gBigSwitchReturnRoom;
extern u8 gUsedSubGameDoors[];
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern u8 gPressedBigSwitchSlot;
extern u16 gBrokenBlockX; /* the block BreakFirstBlockInHitBox broke: x (pixels) */
extern u16 gSavedPlayerAbilityUses[];
extern u16 gStageExitFlags;
extern u8 gBigSwitchReturnStage;
extern u8 gCutscenePending;
extern u16 gUnk_02007FF0;
extern s8 gContinueStage;
extern u8 gBigSwitchReturnLevel;
extern u16 gSavedPlayerAbilities[];
extern u16 gUnk_02008050;
extern u16 gUnk_02008054;
extern u16 gPauseSavedBgPalette[];
extern u16 gBlockLayer[]; /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern s16 gRoomEntryY;
extern u8 gEntryDoorEvent;
extern u8 gSkipNextHubBgm;
extern u8 gHubUnlockFlags;
extern s16 gUnk_0200AF0C;
extern u16 gPlayerAbilityUses[];
extern s16 gHubUnlockBlocks[4];
extern u16 gUnk_0200AFF4;
extern s8 gWarpStarStationDest;
extern s8 gEntryDoorIndex;
extern s8 gUnk_0200B038;
extern u8 gHBlankScrollStarted;
extern u8 gWarpStarStationLevels;
extern u8 gRoomBgLayout;
extern u8 gUnk_0200B078;
extern u16 gMetatileTiles[];
extern u8 gMidBossFightState;

/* IWRAM */
extern u16 gBgPaletteBank2[];
extern u16 gBgPaletteBank8[];
extern s16 gCameraAnchorY;
extern u32 gUnk_03001F10;
extern s8 gCurStage;
extern s32 gUnk_03001F2C; /* boot_091ac.c spelling */
extern u8 gMetaKnightmareMode; /* set only by the mode list's fifth row
                                       (src/menu_0ca10.c); not link play */
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern u8 gLivingPlayerCount;
extern u8 gActivePlayerCount;
extern u16 gMilestoneFlags;
extern s8 gFurthestStage;
extern s16 gCameraFocusY;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern s8 gCurLevel;
extern u16 gLatchedPressedKeys[]; /* newly-pressed keys, latched per player */
extern u32 gBigSwitchFlags[];
extern s16 gCameraFocusX;
extern u16 gGameState; /* current game state (main dispatch) */
extern s8 gFurthestLevel;
extern s32 gCurSaveSlot;
extern s8 gStageIndex;
extern u8 gStageClearStatus[8][7];
extern s8 gStageRequest; /* stage request (M02) */
extern s8 gInHub;
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
extern struct ScreenShake gScreenShake;
extern struct ScrollLock gScrollLock;
extern u16 gBg3Pos[2];

/* ROM */
extern u8 gLevelStageCounts[];
extern struct RoomDef gLevel7Stage1Room0;
extern struct RoomDef gLevel7Stage1Room1;
extern struct RoomDef gLevel7Stage1Room2;
extern struct RoomDef gLevel0Stage0Room0;
extern struct RoomDef gLevel0Stage0Room1;
extern struct RoomDef gLevel0Stage0Room2;
extern struct RoomDef gLevel0Stage0Room3;
extern struct RoomDef gLevel0Stage1Room0;
extern struct RoomDef gLevel0Stage1Room1;
extern struct RoomDef gLevel0Stage1Room2;
extern struct RoomDef gLevel0Stage1Room3;
extern struct RoomDef gLevel0Stage1Room4;
extern struct RoomDef gLevel0Stage1Room5;
extern struct RoomDef gLevel0Stage2Room0;
extern struct RoomDef gLevel0Stage2Room1;
extern struct RoomDef gLevel0Stage2Room2;
extern struct RoomDef gLevel0Stage2Room3;
extern struct RoomDef gLevel0Stage3Room0;
extern struct RoomDef gLevel0Stage3Room1;
extern struct RoomDef gLevel0Stage3Room2;
extern struct RoomDef gLevel0Stage3Room3;
extern struct RoomDef gLevel0Stage4Room0;
extern struct RoomDef gLevel1Stage0Room0;
extern struct RoomDef gLevel1Stage0Room1;
extern struct RoomDef gLevel1Stage0Room2;
extern struct RoomDef gLevel1Stage0Room3;
extern struct RoomDef gLevel1Stage1Room0;
extern struct RoomDef gLevel1Stage1Room1;
extern struct RoomDef gLevel1Stage1Room2;
extern struct RoomDef gLevel1Stage1Room3;
extern struct RoomDef gLevel1Stage1Room4;
extern struct RoomDef gLevel1Stage2Room0;
extern struct RoomDef gLevel1Stage2Room1;
extern struct RoomDef gLevel1Stage2Room2;
extern struct RoomDef gLevel1Stage2Room3;
extern struct RoomDef gLevel1Stage2Room4;
extern struct RoomDef gLevel1Stage2Room5;
extern struct RoomDef gLevel1Stage3Room0;
extern struct RoomDef gLevel1Stage3Room1;
extern struct RoomDef gLevel1Stage3Room2;
extern struct RoomDef gLevel1Stage3Room3;
extern struct RoomDef gLevel1Stage3Room4;
extern struct RoomDef gLevel1Stage3Room5;
extern struct RoomDef gLevel1Stage3Room6;
extern struct RoomDef gLevel1Stage4Room0;
extern struct RoomDef gLevel1Stage4Room1;
extern struct RoomDef gLevel1Stage4Room2;
extern struct RoomDef gLevel1Stage4Room3;
extern struct RoomDef gLevel1Stage4Room4;
extern struct RoomDef gLevel1Stage4Room5;
extern struct RoomDef gLevel1Stage4Room6;
extern struct RoomDef gLevel1Stage5Room0;
extern struct RoomDef gLevel2Stage0Room0;
extern struct RoomDef gLevel2Stage0Room1;
extern struct RoomDef gLevel2Stage0Room2;
extern struct RoomDef gLevel2Stage0Room3;
extern struct RoomDef gLevel2Stage0Room4;
extern struct RoomDef gLevel2Stage0Room5;
extern struct RoomDef gLevel2Stage1Room0;
extern struct RoomDef gLevel2Stage1Room1;
extern struct RoomDef gLevel2Stage1Room2;
extern struct RoomDef gLevel2Stage1Room3;
extern struct RoomDef gLevel2Stage1Room4;
extern struct RoomDef gLevel2Stage1Room5;
extern struct RoomDef gLevel2Stage1Room6;
extern struct RoomDef gLevel2Stage1Room7;
extern struct RoomDef gLevel2Stage2Room0;
extern struct RoomDef gLevel2Stage2Room1;
extern struct RoomDef gLevel2Stage2Room2;
extern struct RoomDef gLevel2Stage2Room3;
extern struct RoomDef gLevel2Stage2Room4;
extern struct RoomDef gLevel2Stage3Room0;
extern struct RoomDef gLevel2Stage3Room1;
extern struct RoomDef gLevel2Stage3Room2;
extern struct RoomDef gLevel2Stage3Room3;
extern struct RoomDef gLevel2Stage3Room4;
extern struct RoomDef gLevel2Stage3Room5;
extern struct RoomDef gLevel2Stage4Room0;
extern struct RoomDef gLevel2Stage4Room1;
extern struct RoomDef gLevel2Stage4Room2;
extern struct RoomDef gLevel2Stage4Room3;
extern struct RoomDef gLevel2Stage4Room4;
extern struct RoomDef gLevel2Stage5Room0;
extern struct RoomDef gLevel2Stage5Room1;
extern struct RoomDef gLevel2Stage5Room2;
extern struct RoomDef gLevel2Stage5Room3;
extern struct RoomDef gLevel2Stage5Room4;
extern struct RoomDef gLevel2Stage5Room5;
extern struct RoomDef gLevel2Stage5Room6;
extern struct RoomDef gLevel2Stage5Room7;
extern struct RoomDef gLevel2Stage5Room8;
extern struct RoomDef gLevel2Stage5Room9;
extern struct RoomDef gLevel2Stage6Room0;
extern struct RoomDef gLevel2Stage6Room1;
extern struct RoomDef gLevel2Stage6Room2;
extern struct RoomDef gLevel3Stage0Room0;
extern struct RoomDef gLevel3Stage0Room1;
extern struct RoomDef gLevel3Stage0Room2;
extern struct RoomDef gLevel3Stage0Room3;
extern struct RoomDef gLevel3Stage0Room4;
extern struct RoomDef gLevel3Stage0Room5;
extern struct RoomDef gLevel3Stage1Room0;
extern struct RoomDef gLevel3Stage1Room1;
extern struct RoomDef gLevel3Stage1Room2;
extern struct RoomDef gLevel3Stage1Room3;
extern struct RoomDef gLevel3Stage1Room4;
extern struct RoomDef gLevel3Stage1Room5;
extern struct RoomDef gLevel3Stage2Room0;
extern struct RoomDef gLevel3Stage2Room1;
extern struct RoomDef gLevel3Stage2Room2;
extern struct RoomDef gLevel3Stage2Room3;
extern struct RoomDef gLevel3Stage2Room4;
extern struct RoomDef gLevel3Stage2Room5;
extern struct RoomDef gLevel3Stage2Room6;
extern struct RoomDef gLevel3Stage3Room0;
extern struct RoomDef gLevel3Stage3Room1;
extern struct RoomDef gLevel3Stage3Room2;
extern struct RoomDef gLevel3Stage3Room3;
extern struct RoomDef gLevel3Stage3Room4;
extern struct RoomDef gLevel3Stage4Room0;
extern struct RoomDef gLevel3Stage4Room1;
extern struct RoomDef gLevel3Stage4Room2;
extern struct RoomDef gLevel3Stage4Room3;
extern struct RoomDef gLevel3Stage4Room4;
extern struct RoomDef gLevel3Stage4Room5;
extern struct RoomDef gLevel3Stage4Room6;
extern struct RoomDef gLevel3Stage5Room0;
extern struct RoomDef gLevel3Stage5Room1;
extern struct RoomDef gLevel3Stage5Room2;
extern struct RoomDef gLevel3Stage5Room3;
extern struct RoomDef gLevel3Stage5Room4;
extern struct RoomDef gLevel3Stage5Room5;
extern struct RoomDef gLevel3Stage5Room6;
extern struct RoomDef gLevel3Stage5Room7;
extern struct RoomDef gLevel3Stage6Room0;
extern struct RoomDef gLevel4Stage0Room0;
extern struct RoomDef gLevel4Stage0Room1;
extern struct RoomDef gLevel4Stage0Room2;
extern struct RoomDef gLevel4Stage0Room3;
extern struct RoomDef gLevel4Stage0Room4;
extern struct RoomDef gLevel4Stage1Room0;
extern struct RoomDef gLevel4Stage1Room1;
extern struct RoomDef gLevel4Stage1Room2;
extern struct RoomDef gLevel4Stage1Room3;
extern struct RoomDef gLevel4Stage1Room4;
extern struct RoomDef gLevel4Stage1Room5;
extern struct RoomDef gLevel4Stage1Room6;
extern struct RoomDef gLevel4Stage2Room0;
extern struct RoomDef gLevel4Stage2Room1;
extern struct RoomDef gLevel4Stage2Room2;
extern struct RoomDef gLevel4Stage2Room3;
extern struct RoomDef gLevel4Stage2Room4;
extern struct RoomDef gLevel4Stage2Room5;
extern struct RoomDef gLevel4Stage3Room0;
extern struct RoomDef gLevel4Stage3Room1;
extern struct RoomDef gLevel4Stage3Room2;
extern struct RoomDef gLevel4Stage3Room3;
extern struct RoomDef gLevel4Stage3Room4;
extern struct RoomDef gLevel4Stage3Room5;
extern struct RoomDef gLevel4Stage3Room6;
extern struct RoomDef gLevel4Stage4Room0;
extern struct RoomDef gLevel4Stage4Room1;
extern struct RoomDef gLevel4Stage4Room2;
extern struct RoomDef gLevel4Stage4Room3;
extern struct RoomDef gLevel4Stage4Room4;
extern struct RoomDef gLevel4Stage4Room5;
extern struct RoomDef gLevel4Stage4Room6;
extern struct RoomDef gLevel4Stage4Room7;
extern struct RoomDef gLevel4Stage5Room0;
extern struct RoomDef gLevel4Stage5Room1;
extern struct RoomDef gLevel4Stage5Room2;
extern struct RoomDef gLevel4Stage5Room3;
extern struct RoomDef gLevel4Stage5Room4;
extern struct RoomDef gLevel4Stage6Room0;
extern struct RoomDef gLevel4Stage6Room1;
extern struct RoomDef gLevel5Stage0Room0;
extern struct RoomDef gLevel5Stage0Room1;
extern struct RoomDef gLevel5Stage0Room2;
extern struct RoomDef gLevel5Stage0Room3;
extern struct RoomDef gLevel5Stage0Room4;
extern struct RoomDef gLevel5Stage0Room5;
extern struct RoomDef gLevel5Stage1Room0;
extern struct RoomDef gLevel5Stage1Room1;
extern struct RoomDef gLevel5Stage1Room2;
extern struct RoomDef gLevel5Stage1Room3;
extern struct RoomDef gLevel5Stage1Room4;
extern struct RoomDef gLevel5Stage1Room5;
extern struct RoomDef gLevel5Stage1Room6;
extern struct RoomDef gLevel5Stage2Room0;
extern struct RoomDef gLevel5Stage2Room1;
extern struct RoomDef gLevel5Stage2Room2;
extern struct RoomDef gLevel5Stage2Room3;
extern struct RoomDef gLevel5Stage2Room4;
extern struct RoomDef gLevel5Stage2Room5;
extern struct RoomDef gLevel5Stage2Room6;
extern struct RoomDef gLevel5Stage2Room7;
extern struct RoomDef gLevel5Stage2Room8;
extern struct RoomDef gLevel5Stage2Room9;
extern struct RoomDef gLevel5Stage2Room10;
extern struct RoomDef gLevel5Stage2Room11;
extern struct RoomDef gLevel5Stage3Room0;
extern struct RoomDef gLevel5Stage3Room1;
extern struct RoomDef gLevel5Stage3Room2;
extern struct RoomDef gLevel5Stage3Room3;
extern struct RoomDef gLevel5Stage3Room4;
extern struct RoomDef gLevel5Stage3Room5;
extern struct RoomDef gLevel5Stage4Room0;
extern struct RoomDef gLevel5Stage4Room1;
extern struct RoomDef gLevel5Stage4Room2;
extern struct RoomDef gLevel5Stage4Room3;
extern struct RoomDef gLevel5Stage4Room4;
extern struct RoomDef gLevel5Stage5Room0;
extern struct RoomDef gLevel5Stage5Room1;
extern struct RoomDef gLevel5Stage5Room2;
extern struct RoomDef gLevel5Stage5Room3;
extern struct RoomDef gLevel5Stage5Room4;
extern struct RoomDef gLevel5Stage5Room5;
extern struct RoomDef gLevel5Stage5Room6;
extern struct RoomDef gLevel5Stage5Room7;
extern struct RoomDef gLevel5Stage5Room8;
extern struct RoomDef gLevel5Stage5Room9;
extern struct RoomDef gLevel5Stage5Room10;
extern struct RoomDef gLevel5Stage6Room0;
extern struct RoomDef gLevel6Stage0Room0;
extern struct RoomDef gLevel6Stage0Room1;
extern struct RoomDef gLevel6Stage0Room2;
extern struct RoomDef gLevel6Stage0Room3;
extern struct RoomDef gLevel6Stage1Room0;
extern struct RoomDef gLevel6Stage1Room1;
extern struct RoomDef gLevel6Stage1Room2;
extern struct RoomDef gLevel6Stage1Room3;
extern struct RoomDef gLevel6Stage1Room4;
extern struct RoomDef gLevel6Stage1Room5;
extern struct RoomDef gLevel6Stage1Room6;
extern struct RoomDef gLevel6Stage1Room7;
extern struct RoomDef gLevel6Stage1Room8;
extern struct RoomDef gLevel6Stage1Room9;
extern struct RoomDef gLevel6Stage1Room10;
extern struct RoomDef gLevel6Stage1Room11;
extern struct RoomDef gLevel6Stage1Room12;
extern struct RoomDef gLevel6Stage1Room13;
extern struct RoomDef gLevel6Stage1Room14;
extern struct RoomDef gLevel6Stage1Room15;
extern struct RoomDef gLevel6Stage1Room16;
extern struct RoomDef gLevel6Stage1Room17;
extern struct RoomDef gLevel6Stage1Room18;
extern struct RoomDef gLevel6Stage1Room19;
extern struct RoomDef gLevel6Stage1Room20;
extern struct RoomDef gLevel6Stage1Room21;
extern struct RoomDef gLevel6Stage1Room22;
extern struct RoomDef gLevel6Stage1Room23;
extern struct RoomDef gLevel6Stage1Room24;
extern struct RoomDef gLevel6Stage2Room0;
extern struct RoomDef gLevel6Stage2Room1;
extern struct RoomDef gLevel6Stage2Room2;
extern struct RoomDef gLevel6Stage2Room3;
extern struct RoomDef gLevel6Stage3Room0;
extern struct RoomDef gLevel6Stage3Room1;
extern struct RoomDef gLevel6Stage3Room2;
extern struct RoomDef gLevel6Stage3Room3;
extern struct RoomDef gLevel6Stage3Room4;
extern struct RoomDef gLevel6Stage3Room5;
extern struct RoomDef gLevel6Stage3Room6;
extern struct RoomDef gLevel6Stage4Room0;
extern struct RoomDef gLevel6Stage4Room1;
extern struct RoomDef gLevel6Stage4Room2;
extern struct RoomDef gLevel6Stage4Room3;
extern struct RoomDef gLevel6Stage4Room4;
extern struct RoomDef gLevel6Stage5Room0;
extern struct RoomDef gLevel6Stage5Room1;
extern struct RoomDef gLevel6Stage5Room2;
extern struct RoomDef gLevel6Stage5Room3;
extern struct RoomDef gLevel6Stage5Room4;
extern struct RoomDef gLevel6Stage5Room5;
extern struct RoomDef gLevel6Stage5Room6;
extern struct RoomDef gLevel6Stage5Room7;
extern struct RoomDef gLevel6Stage5Room8;
extern struct RoomDef gLevel6Stage6Room0;
extern struct RoomDef gLevel7Stage0Room0;
extern struct RoomDef gLevel7Stage0Room1;
extern struct RoomDef gLevel7Stage0Room2;
extern struct RoomDef gLevel7Stage2Room0;
extern struct RoomDef gLevel7Stage2Room1;
extern struct RoomDef gLevel7Stage2Room2;
extern struct RoomDef gLevel7Stage2Room3;
extern struct RoomDef gLevel7Stage2Room4;
extern struct RoomDef gLevel7Stage2Room5;
extern struct RoomDef gLevel7Stage2Room6;
extern struct RoomDef gLevel7Stage2Room7;
extern struct RoomDef gLevel7Stage2Room8;
extern struct RoomDef gLevel7Stage2Room9;
extern struct RoomDef gLevel7Stage2Room10;
extern struct RoomDef gLevel8Stage0Room0;
extern struct RoomDef gLevel8Stage0Room1;
extern struct RoomDef gLevel8Stage0Room2;
extern struct RoomDef gLevel8Stage0Room3;
extern struct RoomDef gLevel8Stage0Room4;
extern struct RoomDef gLevel8Stage1Room0;
extern struct RoomDef gLevel8Stage1Room1;
extern struct RoomDef gLevel8Stage1Room2;
extern struct RoomDef gLevel8Stage1Room3;
extern struct RoomDef gLevel8Stage1Room4;
extern struct RoomDef gLevel8Stage1Room5;
extern struct RoomDef gLevel8Stage2Room0;
extern struct RoomDef gLevel8Stage2Room1;
extern struct RoomDef gLevel8Stage2Room2;
extern struct RoomDef gLevel8Stage2Room3;
extern struct RoomDef gLevel8Stage2Room4;
extern struct RoomDef gLevel8Stage2Room5;
extern struct RoomDef gLevel8Stage3Room0;
extern struct RoomDef gLevel8Stage3Room1;
extern struct RoomDef gLevel8Stage3Room2;
extern struct RoomDef gLevel8Stage3Room3;
extern struct RoomDef gLevel8Stage3Room4;
extern struct RoomDef gLevel8Stage3Room5;
extern struct RoomDef gLevel8Stage4Room0;
extern struct RoomDef gLevel8Stage4Room1;
extern struct RoomDef gLevel8Stage4Room2;
extern struct RoomDef gLevel8Stage4Room3;
extern struct RoomDef gLevel8Stage4Room4;
extern struct RoomDef gLevel8Stage4Room5;
extern struct RoomDef gLevel8Stage5Room0;
extern struct RoomDef gLevel8Stage5Room1;
extern struct RoomDef gLevel8Stage5Room2;
extern struct RoomDef gLevel8Stage5Room3;
extern struct RoomDef gLevel8Stage5Room4;
extern struct RoomDef gLevel8Stage5Room5;
extern struct RoomDef gLevel8Stage6Room0;
extern struct RoomDef gLevel8Stage6Room1;
extern struct RoomDef gLevel8Stage6Room2;
extern struct RoomDef gLevel8Stage6Room3;
extern struct RoomDef gLevel8Stage7Room0;
extern struct RoomDef gLevel8Stage7Room1;
extern s32 gRoomDriftVelocities[][2];
extern s8 gUnk_08732302[][6];
extern u32 gUnk_0873232C[];
extern u16 gHubDoorUnlocks[][9];
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
extern struct RoomDef *const *const gRoomTable[][8];
extern struct RoomDef *const gLevel7Stage1Rooms[];
extern struct RoomDef *const gLevel0Stage0Rooms[];
extern struct RoomDef *const gLevel0Stage1Rooms[];
extern struct RoomDef *const gLevel0Stage2Rooms[];
extern struct RoomDef *const gLevel0Stage3Rooms[];
extern struct RoomDef *const gLevel0Stage4Rooms[];
extern struct RoomDef *const gLevel1Stage0Rooms[];
extern struct RoomDef *const gLevel1Stage1Rooms[];
extern struct RoomDef *const gLevel1Stage2Rooms[];
extern struct RoomDef *const gLevel1Stage3Rooms[];
extern struct RoomDef *const gLevel1Stage4Rooms[];
extern struct RoomDef *const gLevel1Stage5Rooms[];
extern struct RoomDef *const gLevel2Stage0Rooms[];
extern struct RoomDef *const gLevel2Stage1Rooms[];
extern struct RoomDef *const gLevel2Stage2Rooms[];
extern struct RoomDef *const gLevel2Stage3Rooms[];
extern struct RoomDef *const gLevel2Stage4Rooms[];
extern struct RoomDef *const gLevel2Stage5Rooms[];
extern struct RoomDef *const gLevel2Stage6Rooms[];
extern struct RoomDef *const gLevel3Stage0Rooms[];
extern struct RoomDef *const gLevel3Stage1Rooms[];
extern struct RoomDef *const gLevel3Stage2Rooms[];
extern struct RoomDef *const gLevel3Stage3Rooms[];
extern struct RoomDef *const gLevel3Stage4Rooms[];
extern struct RoomDef *const gLevel3Stage5Rooms[];
extern struct RoomDef *const gLevel3Stage6Rooms[];
extern struct RoomDef *const gLevel4Stage0Rooms[];
extern struct RoomDef *const gLevel4Stage1Rooms[];
extern struct RoomDef *const gLevel4Stage2Rooms[];
extern struct RoomDef *const gLevel4Stage3Rooms[];
extern struct RoomDef *const gLevel4Stage4Rooms[];
extern struct RoomDef *const gLevel4Stage5Rooms[];
extern struct RoomDef *const gLevel4Stage6Rooms[];
extern struct RoomDef *const gLevel5Stage0Rooms[];
extern struct RoomDef *const gLevel5Stage1Rooms[];
extern struct RoomDef *const gLevel5Stage2Rooms[];
extern struct RoomDef *const gLevel5Stage3Rooms[];
extern struct RoomDef *const gLevel5Stage4Rooms[];
extern struct RoomDef *const gLevel5Stage5Rooms[];
extern struct RoomDef *const gLevel5Stage6Rooms[];
extern struct RoomDef *const gLevel6Stage0Rooms[];
extern struct RoomDef *const gLevel6Stage1Rooms[];
extern struct RoomDef *const gLevel6Stage2Rooms[];
extern struct RoomDef *const gLevel6Stage3Rooms[];
extern struct RoomDef *const gLevel6Stage4Rooms[];
extern struct RoomDef *const gLevel6Stage5Rooms[];
extern struct RoomDef *const gLevel6Stage6Rooms[];
extern struct RoomDef *const gLevel7Stage0Rooms[];
extern struct RoomDef *const gLevel7Stage2Rooms[];
extern struct RoomDef *const gLevel8Stage0Rooms[];
extern struct RoomDef *const gLevel8Stage1Rooms[];
extern struct RoomDef *const gLevel8Stage2Rooms[];
extern struct RoomDef *const gLevel8Stage3Rooms[];
extern struct RoomDef *const gLevel8Stage4Rooms[];
extern struct RoomDef *const gLevel8Stage5Rooms[];
extern struct RoomDef *const gLevel8Stage6Rooms[];
extern struct RoomDef *const gLevel8Stage7Rooms[];


/* Functions (defined in the files named above each group). */

/* src/level_2296c.c */
void ResetLevelStateAtHub(void);
void ResetLevelStateForContinue(void);
void BossEnduranceSetStart(void);
void sub_08022f98(void);
void ClearRoomBgmStarted(void);
void LoadRoom(void);
void sub_080233e0(void);
void CreateRoomTask(s32 a);

/* src/roomtask_23618.c */
void Task_Room(void);
void RoomTaskStageInit(void);
void RoomTaskDraw(void);
void RoomTaskUpdateCamera(void);
void RoomTaskLateUpdateBg2(void);
void RoomTaskLateUpdateLooping(void);
void RoomTaskLateUpdateBg2Bg3(void);
void RoomTaskLateUpdateBg23HBlank(void);
void RoomTaskLateUpdateBg23RowsHBlank(void);
void RoomTaskLateUpdateBg3AutoScroll(void);

/* src/level_23948.c */
void LoadHubRoom(void);
void LoadBigSwitchViewRoom(void);
void RoomTaskHubInit(void);
void RoomTaskBigSwitchViewInit(void);
void RoomTaskHubUpdateCamera(void);
void RoomTaskBigSwitchViewUpdateCamera(void);
void RoomTaskHubLateUpdateBg23(void);
void RoomTaskHubLateUpdateBg123(void);
void RoomTaskBigSwitchViewLateUpdate(void);
void LoadGoalGameRoom(void);

/* src/level_242d0.c */
void RoomTaskGoalGameInit(void);
void LoadCutsceneRoom(void);
void RoomTaskCutsceneInit(void);
void RoomTaskSnapCameraToFocus(void);
void RoomTaskLateUpdateBg2NoObjects(void);
void RoomTaskLateUpdateBg2Bg3NoObjects(void);
void LoadEndingEpilogueRoom(s32 x, s32 y);
void LoadEndingStarRodReturnRoom(s32 x, s32 y);
void LoadEndingRoom(s32 a0);
void sub_08024904(void);
void LoadCreditsRoom(void);
void RoomTaskCreditsInit(void);
s32 FindDoorAt(s32 x, s32 y);
s32 EnterDoor(void);
void ExitClearedStage(void);
void ExitKingDededeStage(void);
void ExitToNextRoom(void);
void ExitToNextRoomOnWarpStar(void);
void ExitToEnding(void);
void PressBigSwitch(s32 id);
void ReturnFromBigSwitchView(void);
void sub_08025e00(void);
void sub_08025e0c(void);
s32 WarpStarPickFlightSlot(s32 i);
s32 ExitOnWarpStar(void);
s32 PickWarpStarArrivalFlight(void);
s32 ExitByCannon(void);

/* src/stage_261c0.c */
s32 CreateBlockBreakEffect(s32 x, s32 y);
s32 TaskCreateHighSlot(s32 type);
void SetCameraFocus(s32 x, s32 y);
void SetCameraFocusOrAnchor(s32 x, s32 y);
void EndMidBossFight(void);
void sub_080262e8(s32 a);
void WrapLoopingRoom(void);
s32 CreateEntryDoorOpening(void);
void CloseDoorOpening(s32 i);
s32 CreateEntryDoorStageClearFlag(void);
s32 sub_08026584(void);
void sub_08026704(s32 i);
s32 CreateStageUnlockPan(void);
s32 CreateBigSwitchUnlockPan(void);
void ClampCameraFocusToRoom(void);
void UnlockNextLevel(void);
void sub_08026994(void);
void SaveAndSetContinuePoint(void);
void sub_080269e8(void);
u32 WhispyWoodsCheckScrollLock(void);
u32 KrackoCheckScrollLock(void);
u32 KingDededeCheckScrollLock(void);

/* src/door_26b60.c */
void sub_08026b60(void);
void UpdateDoors(void);
void DrawDoors(void);

/* src/stage_270d0.c */
void sub_080270d0(void);
void StopRoomAndApplyExitFlags(void);
void StopRoom(void);
void FreeRoomAndDoorObjects(void);
void PauseRoom(void);
void SetRoomUpdateFlags(u32 a);
void ResumeRoom(void);
void PauseSaveBgPalette(void);
void PauseRestoreRoomGraphics(void);
void ReturnToHubStageDoor(void);

/* src/stage_273a0.c */
void ReturnToRestartPoint(void);
void sub_08027548(void);
s32 sub_08027588(void);
s32 sub_080275cc(s32 a);
s32 HoldPlayerCamera(s32 a);
s32 ReleaseDeadPlayerView(s32 i);
s32 AreInactivePlayerCamerasParked(void);
void CameraStartHoldAnchorAt(s32 x, s32 y);
void CameraStartFollowFocusAt(s32 x, s32 y);
void CameraStartFollowingPlayer(s32 a);
void CameraStartPlayersAtAnchor(void);
s32 ArePlayerCamerasDoneGliding(void);
void CameraResumeFollowFocus(void);

/* src/level_27a6c.c */
void sub_08027a6c(void);

/* src/room_27e28.c */
void InitRoomBgLayout(void);
void sub_08028130(void);
void InitEndingRoomBgLayout(s32 a);
void StartRoomBlockAnims(void);

/* src/room_28320.c */
void SpawnDoorObjects(void);
void CalcBg3Parallax(void);
void CalcRoomBounds(void);
void CameraResetBoundsToGroup(void);
void CameraResetBounds(void);

/* src/camera_28b8c.c */
void CameraResetRoomView(void);
void CalcRoomAndCameraBounds(void);
void SetRoomEntryPoint(void);
void sub_08029034(void);
void CameraSetFocusToLocalPlayer(void);
void CameraInitPos(void);
void PlayRoomBgm(void);
void PlayHubRoomBgm(void);
void LoadBg2Gfx(void);
void LoadBg3Gfx(void);
void ClearBg2Bg3Maps(void);
void SelectBg3MapShape(void);
void LoadBg3Map(void);
void SpawnRoomObjectsInView(void);
void InitDoors(void);
void CameraUpdatePos(void);
void CameraUpdatePosNoParallax(void);
void CameraUpdatePosBg3AutoScrollX(void);
void StreamBg2Map(void);
void StreamBg3Map(void);

#endif /* GUARD_ROOM_H */
