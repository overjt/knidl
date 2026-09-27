#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_242d0.c (0x080242D0-0x080261BF, issue #93).
 *
 * Room loaders, part 3: sub_08024300 (M02's stage sequence state,
 * CutsceneMain), sub_08024698 (entered through sub_08024610 and
 * sub_08024654, which preset the level/stage/room and the player
 * position) and sub_0802497c, plus the room-task variants 3-6 of task type
 * #3 (sub_080242d0, sub_08024540, sub_08024904, sub_08024da4) and their
 * per-frame bodies.  A loader looks the room header up in
 * gRoomTable[level][stage][room] and stores it in gCurRoomDef, copies
 * its size, origin, BG3 origin and object list into the camera cells,
 * decompresses (or CpuSet-copies) its metatile map into gRoomMapBuffer and
 * its metatile table into gMetatileTiles, resets the camera (M08) and the
 * players, streams the whole view and spawns task type #3.  sub_0802497c is
 * the same shape as sub_08022fa8.
 * 
 * The doors, from FindDoorAt on (one translation unit with the loaders:
 * split at 0x08024E40, EnterDoor and ExitClearedStage swap hoisted address
 * registers, lesson 4.79).  FindDoorAt(x, y) finds an enterable door at a
 * pixel - the door metatiles 16/144, 54/182 and 55/183, the door records
 * RoomDef.doors and their locks gUsedSubGameDoors[]/gUnk_0200B04C - and records
 * it in gUnk_02000030 (type << 8 | index); EnterDoor enters it, a 9-way
 * switch on the door kind (RoomDef door byte +6) that sets the next
 * level/stage/room, the arrival position gRoomEntryX/gRoomEntryY and
 * the stage request gStageRequest for M02's state bodies.  ExitClearedStage
 * (a stage cleared: the hub's next stage door, or on to the next level),
 * sub_08025a30, sub_08025acc, sub_08025b0c, sub_08025b5c, sub_08025bc8 and
 * sub_08025dc4 are the other exits, each setting the level/stage/room and a
 * stage request; sub_08025e0c lowers the room's bottom bound, sub_08025e88
 * reads an object-list entry's parameter, and sub_08025f00, sub_080260b0
 * and sub_0802610c pick the position the player arrives at. */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

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
    /*0x3E*/ u16 unk3E;
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

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

struct Unk020055D8
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk020055D8Entry *unk4;
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

struct CamRect { s16 x0, x1, y0, y1; };

extern struct RoomDef **gRoomTable[][8];
extern struct RoomDef *gCurRoomDef;
extern s8 gLevelIndex;
extern s8 gStageIndex;
extern s8 gUnk_030023B8;
extern u8 gUnk_03001F20;
extern struct MapCell *gRoomMap;
extern struct MapCell gRoomMapBuffer[];
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern s16 gRoomMetatileCount;
extern u16 gMetatileTiles[];
extern s16 *gUnk_0300558C;
extern s16 gUnk_0873A318[];
extern u16 gCameraMode;
extern u16 gRoomUpdateFlags;
extern u16 gRoomBorder[2];
extern u16 gCameraPos[2];
extern u16 gBg3Border[2];
extern u16 gBg3Pos[2];
extern u8 gActivePlayerMask;
extern s16 gCameraFocusX;
extern s16 gCameraFocusY;
extern s8 gUnk_02000000;
extern u8 gUnk_02000020;
extern u8 gUnk_02005574[];
extern u8 gBg3MapShape;
extern s8 gUnk_02007D64;
extern u8 gHBlankScrollStarted;
extern u8 gRoomBgLayout;
extern u8 gUnk_0200B078;
extern u8 gUnk_08732630[];
extern s8 gRoomIndex;
extern s16 gRoomEntryX;
extern s16 gRoomEntryY;
extern u8 gRoomEntrySet;
extern s8 gUnk_03002444;
extern struct Unk020055D8 gRoomObjectList;
extern u16 gBlockLayer[];
extern u8 gUnk_020069F0;
extern u8 gActivePlayerCount;
extern u8 gUnk_0300234C;
extern u16 gPlayerCount;
extern s16 gPlayerLives[];
extern s16 gPlayerHealth[];
extern s16 gMaxHealth;
extern u16 gSavedPlayerAbilities[];
extern u16 gUnk_02007FA8[];
extern u16 gPlayerAbilities[];
extern u16 gUnk_0200AF18[];
extern u8 gPlayerCameraMode[];
extern vu16 gPlayerHeldKeys[];
extern vu16 gPlayerPressedKeys[];
extern u16 gLatchedHeldKeys[];
extern u16 gLatchedPressedKeys[];
extern struct Unk02004B90 gDoorStates[];
extern u8 gUsedSubGameDoors[];
extern s8 gUnk_0200B038;
extern u8 gUnk_0200B04C;
extern u16 gUnk_02000030;
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern s8 gStageRequest;
extern u8 gUnk_08334EB4[];
extern u16 gGameState;
extern u8 gUnk_03001F30;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern u16 gUnk_08732348[][9];
extern s8 gUnk_08732302[][6];
extern u8 gUnk_087323E2[][3][2];
extern u16 gUnk_0200001C;
extern u8 gUnk_02005578;
extern u16 gUnk_02007D60;
extern s8 gUnk_02007FF8;
extern u16 gUnk_02008050;
extern u16 gUnk_02008054;
extern u8 gUnk_0200AF00;
extern u8 gUnk_0200AF08;
extern u16 gUnk_0200AFF4;
extern s8 gUnk_0200B034;
extern s8 gSubGameLevel;
extern u8 gCutscenePending;
extern u8 gExtraMode;
extern u16 gUnk_02007FF0;
extern u16 gUnk_02007FB0;
extern s8 gUnk_02004C98;
extern u16 gMilestoneFlags;
extern s8 gUnk_02005590[];
extern u8 gUnk_02008000;
extern u8 gUnk_02007FB4;
extern u16 gUnk_02007D50;
extern u8 gUnk_02007E8C;
extern u16 gUnk_02004B80;
extern s16 gRoomBounds[4];
extern s16 gCameraBounds[4];
extern s16 gViewRect[4];
extern struct CamRect gPlayerBounds[4];
extern s8 gUnk_0200B02C;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetTasksAndOam(void);
void TaskSleepForever(void);
void LoadGfxSet(u16 a0);
void HudReset(void);
s32 AddPlayerLives(s32 a, u32 b);
void HudUpdateAbilityPanel(void);
s32 GetCollisionTileAtPixel(u16 x, u16 y);
void CreateRoomTask(s32 a);
void RoomTaskDraw(void);
void RoomTaskUpdateCamera(void);
void sub_08023748(void);
void sub_080237a4(void);
void sub_080237fc(void);
void sub_0802385c(void);
void sub_080238a4(void);
void sub_080238ec(void);
void sub_0802695c(void);
void InitRoomBgLayout(void);
void sub_08028280(s32 a);
void sub_08028304(void);
void CalcBg3Parallax(void);
void CalcRoomBounds(void);
void CameraResetBoundsToGroup(void);
void CameraResetBounds(void);
void sub_08028b8c(void);
void SetRoomEntryPoint(void);
void CameraInitPos(void);
void LoadBg2Gfx(void);
void LoadBg3Gfx(void);
void ClearBg2Bg3Maps(void);
void SelectBg3MapShape(void);
void LoadBg3Map(void);
void SpawnRoomObjectsInView(void);
void InitDoors(void);
void CameraUpdatePos(void);
void StreamBg2Map(void);
void StreamBg3Map(void);
void CameraWriteScrollParallax(void);
void SetViewRectToPlayers(void);
void DrawBg2View(s32 px, s32 py);
void DrawBg3View(s32 px, s32 py);
void DrawBg23FullRows(s32 py);
void sub_0802b074(s32 px);
void CameraFollowFocus(void);
void CameraFollowScrollLocked(void);
void CameraHoldAnchor(void);
void CameraSnapToFocus(void);
void StopScreenShake(void);
void UpdateScreenShake(void);
void LoadRoomBgAnims(void);
void UpdateBgAnims(void);
void ResetBlockAnims(void);
void CreatePlayer(s32 a0);
void InitPlayerState(s32 a0);
void sub_080b4e40(void);
void sub_080b5024(void);
void sub_0802457c(void);
void sub_08024598(void);
void sub_080245d0(void);
void sub_08024698(s32 a0);

void sub_080242d0(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = (u32)RoomTaskDraw;
    t->unk04 = (u32)sub_0802457c;
    t->unk08 = (u32)sub_080245d0;
    TaskSleepForever();
}

void sub_08024300(void)
{
    u32 a;

    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(3);
    if (gLevelIndex == 8)
    {
        gUnk_030023B8 = gStageIndex;
        gUnk_03001F20 = 48;
    }
    else
    {
        gUnk_030023B8 = gLevelIndex;
        gUnk_03001F20 = 48;
    }
    gCurRoomDef = gRoomTable[8][gUnk_030023B8][gUnk_08732630[gUnk_030023B8]];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gUnk_02000000 = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomMap = gRoomMapBuffer;
    if (gCurRoomDef->mapsCompressed != 0)
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gRoomMapBuffer, 0);
    else
        CpuSet(gCurRoomDef->metatileMap, gRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    gRoomBgLayout = 0;
    gUnk_0300558C = gUnk_0873A318;
    *gUnk_02005574 = 0;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    InitRoomBgLayout();
    CalcRoomBounds();
    LoadRoomBgAnims();
    gCameraFocusX = gBg3Border[0] + 120;
    gCameraFocusY = gBg3Border[1] + 80;
    gActivePlayerMask = 0;
    gRoomBgLayout = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 0;
    CameraResetBounds();
    CameraSnapToFocus();
    CameraInitPos();
    CameraWriteScrollParallax();
    SetViewRectToPlayers();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    HudReset();
    if (gBg3MapShape != 0)
    {
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
    }
    else
    {
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
    }
}

void sub_08024540(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0802457c;
    if (gBg3MapShape != 0)
        t->unk08 = (u32)sub_080245d0;
    else
        t->unk08 = (u32)sub_08024598;
    TaskSleepForever();
}

void sub_0802457c(void)
{
    if (gRoomUpdateFlags & 1)
        CameraSnapToFocus();
}

void sub_08024598(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
    }
    CameraWriteScrollParallax();
}

void sub_080245d0(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
        StreamBg3Map();
    }
    CameraWriteScrollParallax();
    HudUpdateAbilityPanel();
}

void sub_08024610(s32 x, s32 y)
{
    gLevelIndex = 7;
    gStageIndex = 0;
    gRoomIndex = 2;
    gRoomEntryX = x;
    gRoomEntryY = y;
    gRoomEntrySet = 1;
    sub_08024698(0);
}

void sub_08024654(s32 x, s32 y)
{
    gLevelIndex = 6;
    gStageIndex = 6;
    gRoomIndex = 0;
    gRoomEntryX = x;
    gRoomEntryY = y;
    gRoomEntrySet = 1;
    sub_08024698(1);
}

void sub_08024698(s32 a0)
{
    u32 a;

    ResetTasksAndOam();
    gUnk_03002444 = 0;
    gUnk_030023B8 = gLevelIndex;
    gUnk_03001F20 = gStageIndex;
    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(0);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gUnk_02000000 = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomObjectList.unk0 = gCurRoomDef->objectCount;
    gRoomObjectList.unk2 = gCurRoomDef->unk3E;
    gRoomObjectList.unk4 = gCurRoomDef->objects;
    gRoomMap = gRoomMapBuffer;
    if (gCurRoomDef->mapsCompressed != 0)
    {
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gRoomMapBuffer, 0);
        RequestCopy(8, (u32)gCurRoomDef->blockLayer, (u32)gBlockLayer, 0);
    }
    else
    {
        CpuSet(gCurRoomDef->metatileMap, gRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
        CpuSet(gCurRoomDef->blockLayer, gBlockLayer, gRoomMetatileCount & 0x1FFFFF);
    }
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    sub_08028280(a0);
    SetRoomEntryPoint();
    LoadRoomBgAnims();
    sub_08028304();
    CameraResetBounds();
    HudReset();
    switch (gCameraMode)
    {
    default:
    case 0:
    case 1:
    case 3:
        CameraFollowFocus();
        break;
    case 5:
        CameraHoldAnchor();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    switch (gRoomBgLayout)
    {
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
        break;
    case 1:
    case 4:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
        break;
    }
}

void sub_08024904(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0802457c;
    switch (gRoomBgLayout)
    {
    case 1:
        gCurTask->unk08 = (u32)sub_080237fc;
        break;
    case 4:
        gCurTask->unk08 = (u32)sub_0802385c;
        break;
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        gCurTask->unk08 = (u32)sub_08023748;
        break;
    }
    TaskSleepForever();
}

void sub_0802497c(void)
{
    s32 i;
    u32 a;

    ResetTasksAndOam();
    gUnk_03002444 = 0;
    if (gLevelIndex == 8)
    {
        gUnk_030023B8 = gStageIndex;
        *(s8 *)&gUnk_03001F20 = -1;
    }
    else
    {
        gUnk_030023B8 = gLevelIndex;
        gUnk_03001F20 = gStageIndex;
    }
    if (gUnk_020069F0 == 2)
        gCameraMode = 5;
    else
        gCameraMode = 0;
    CreateRoomTask(6);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gUnk_02000000 = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomObjectList.unk0 = gCurRoomDef->objectCount;
    gRoomObjectList.unk2 = gCurRoomDef->unk3E;
    gRoomObjectList.unk4 = gCurRoomDef->objects;
    gRoomMap = gRoomMapBuffer;
    if (gCurRoomDef->mapsCompressed != 0)
    {
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gRoomMapBuffer, 0);
        RequestCopy(8, (u32)gCurRoomDef->blockLayer, (u32)gBlockLayer, 0);
    }
    else
    {
        CpuSet(gCurRoomDef->metatileMap, gRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
        CpuSet(gCurRoomDef->blockLayer, gBlockLayer, gRoomMetatileCount & 0x1FFFFF);
    }
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitRoomBgLayout();
    sub_080b4e40();
    SetRoomEntryPoint();
    LoadRoomBgAnims();
    InitDoors();
    sub_08028304();
    gActivePlayerMask = 0;
    gActivePlayerCount = 0;
    gUnk_0300234C = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0)
        {
            if (gPlayerHealth[i] == 0)
            {
                gPlayerHealth[i] = gMaxHealth;
                gSavedPlayerAbilities[i] = 0;
                gUnk_02007FA8[i] = 0xFFFF;
                AddPlayerLives(-1, i);
            }
            if ((s16)gSavedPlayerAbilities[i] != 0)
            {
                gPlayerAbilities[i] = gSavedPlayerAbilities[i];
                gUnk_0200AF18[i] = gUnk_02007FA8[i];
                gSavedPlayerAbilities[i] = 0;
                gUnk_02007FA8[i] = 0xFFFF;
            }
            gActivePlayerMask |= 1 << i;
            gActivePlayerCount++;
            gUnk_0300234C++;
            gPlayerCameraMode[i] = 0;
        }
        else
        {
            gPlayerCameraMode[i] = 3;
        }
        CreatePlayer(i);
        InitPlayerState(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    sub_080b5024();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
    HudReset();
    switch (gCameraMode)
    {
    default:
    case 0:
    case 1:
        CameraFollowFocus();
        break;
    case 3:
        CameraFollowScrollLocked();
        break;
    case 5:
        CameraHoldAnchor();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    SpawnRoomObjectsInView();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    switch (gRoomBgLayout)
    {
    default:
    case 0:
    case 2:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
        break;
    case 3:
        sub_0802b074(gCameraPos[0]);
        LoadBg3Map();
        break;
    case 5:
        DrawBg23FullRows(gCameraPos[1]);
        break;
    case 1:
    case 4:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
        break;
    }
}

void sub_08024da4(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)RoomTaskUpdateCamera;
    switch (gRoomBgLayout)
    {
    default:
    case 0:
        gCurTask->unk08 = (u32)sub_08023748;
        break;
    case 1:
        gCurTask->unk08 = (u32)sub_080237fc;
        break;
    case 2:
        gCurTask->unk08 = (u32)sub_080238ec;
        break;
    case 3:
        gCurTask->unk08 = (u32)sub_080237a4;
        break;
    case 4:
        gCurTask->unk08 = (u32)sub_0802385c;
        break;
    case 5:
        gCurTask->unk08 = (u32)sub_080238a4;
        break;
    }
    TaskSleepForever();
}

s32 FindDoorAt(s32 x, s32 y)
{
    s32 type;
    s32 i;
    struct Door *d;

    gUnk_02000030 = 0xFF;
    if (gCurRoomDef->doorCount == 0)
        return 0;
    switch (GetCollisionTileAtPixel(x, y))
    {
    case 55:
    case 183:
        x -= 16;
        type = 1;
        break;
    case 54:
    case 182:
        type = 2;
        break;
    case 16:
    case 144:
        type = 1;
        break;
    default:
        return 0;
    }
    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        if (gUnk_03002444 != 0)
        {
            if (d->unk0 != 0x270F)
                continue;
        }
        else
        {
            if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61)
                continue;
        }
        if (d->unk2 == (x >> 4) && d->unk4 == (y >> 4))
        {
            if (d->unk0 == 0x15B3)
            {
                gUnk_0200B038 = 1;
                d++;
                i++;
            }
            else
            {
                gUnk_0200B038 = 0;
            }
            break;
        }
    }
    if (gDoorStates[i].unk1 == 0)
        return 0;
    if (gUnk_03002444 != 0)
    {
        switch ((u8)d->unk6)
        {
        case 3:
            if (gUsedSubGameDoors[gUnk_030023B8] & 1)
                return 0;
            break;
        case 4:
            if (gUsedSubGameDoors[gUnk_030023B8] & 2)
                return 0;
            break;
        case 5:
            if (gUsedSubGameDoors[gUnk_030023B8] & 4)
                return 0;
            break;
        case 6:
            if ((gUnk_0200B04C & ~(1 << gUnk_030023B8)) == 0)
                return 0;
            break;
        }
    }
    gUnk_02000030 = (type << 8) | i;
    return 1;
}

s32 EnterDoor(void)
{
    struct RoomDef *room;
    struct Door *d;
    struct Door *e;
    s32 i;
    s32 idx;
    s32 t;

    idx = gUnk_02000030 & 0xFF;
    if (idx == 0xFF)
        return 0;
    d = &gCurRoomDef->doors[idx];
    gRoomEntrySet = 0;
    gUnk_0200AF08 = 0;
    if (gUnk_03002444 != 0)
    {
        if (d->unk0 == 0x270F)
        {
            gUnk_0200B034 = idx;
            switch (d->unk6 & 0xFF)
            {
            case 0:
                gLevelIndex = gStageIndex;
                gStageIndex = d->unk8;
                gRoomIndex = 0;
                gStageRequest = 2;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
                break;
            case 1:
                if (--gStageIndex < 0)
                    gStageIndex = 0;
                gRoomIndex = 0;
                room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
                e = room->doors;
                for (i = 0; i < room->doorCount; e++, i++)
                {
                    if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 2)
                        break;
                }
                gRoomEntryX = e->unk2 * 16 + 22;
                gRoomEntryY = e->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 1;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
                gCutscenePending = 0;
                break;
            case 2:
                if (gUnk_030023B8 >= gUnk_030023E0)
                {
                    gLevelIndex = gStageIndex;
                    gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
                    gRoomIndex = 0;
                    gUnk_02008054 = 0x100;
                    gUnk_0200B038 = 1;
                    gStageRequest = 2;
                    gUnk_020069F0 = 0;
                }
                else
                {
                    if (++gStageIndex > 7)
                        gStageIndex = 7;
                    gRoomIndex = 0;
                    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
                    e = room->doors;
                    for (i = 0; i < room->doorCount; e++, i++)
                    {
                        if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 1)
                            break;
                    }
                    gRoomEntryX = e->unk2 * 16 + 22;
                    gRoomEntryY = e->unk4 * 16 + 5;
                    gRoomEntrySet = 1;
                    gStageRequest = 1;
                    gUnk_020069F0 = 1;
                    gCutscenePending = 0;
                }
                gUnk_0200AF00 = 0;
                break;
            case 3:
                gSubGameLevel = gUnk_087323E2[gUnk_030023B8][0][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 1;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 10;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 4:
                gSubGameLevel = gUnk_087323E2[gUnk_030023B8][1][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 2;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 11;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 5:
                gSubGameLevel = gUnk_087323E2[gUnk_030023B8][2][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 4;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 9;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 6:
            case 7:
            case 8:
                gStageRequest = *(u8 *)&d->unk6 + 6;
                d++;
                gRoomIndex = d->unk0;
                gRoomEntryX = d->unk6;
                gRoomEntryY = d->unk8 + 0xFFFD;
                gRoomEntrySet = 1;
                gUnk_02008054 = 0x200;
                gUnk_0200B038 = 1;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
                break;
            default:
                return 0;
            }
        }
    }
    else if (gUnk_02007D64 != 0)
    {
        gRoomIndex = d->unk0;
        t = gGameState - 11;
        room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
        e = room->doors;
        for (i = 0; i < room->doorCount; e++, i++)
        {
            if (e->unk0 == 0x270F && (e->unk6 & 0xFF) == t)
                break;
        }
        gRoomEntryX = e->unk2 * 16 + 22;
        gRoomEntryY = e->unk4 * 16 + 5;
        gRoomEntrySet = 1;
        gStageRequest = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
    }
    else
    {
        if (d->unk0 == 0x22B8)
        {
            switch (gUnk_03002400[gLevelIndex][gStageIndex])
            {
            case 0:
                gUnk_0200AF00 = 1;
                gUnk_0200AF08 = 1;
                gUnk_0200001C = gStageIndex + 1;
                if (gUnk_08732348[gLevelIndex][6] == (s8)gUnk_03001F20)
                {
                    gUnk_0200AF08 = 17;
                    gUnk_0200B04C |= 1 << gLevelIndex;
                }
                if (gUnk_08732302[gLevelIndex][gStageIndex] == -1
                    || (gUnk_030023C8[0] & (1 << gUnk_08732302[gLevelIndex][gStageIndex])))
                    gUnk_03002400[gLevelIndex][gStageIndex] = 2;
                else
                    gUnk_03002400[gLevelIndex][gStageIndex] = 1;
                if (gUnk_030023E0 <= gLevelIndex && gUnk_03002384 <= gStageIndex)
                    gUnk_03002384 = gStageIndex + 1;
                break;
            case 1:
                if (gUnk_08732302[gLevelIndex][gStageIndex] == -1
                    || (gUnk_030023C8[0] & (1 << gUnk_08732302[gLevelIndex][gStageIndex])))
                    gUnk_03002400[gLevelIndex][gStageIndex] = 2;
            case 2:
            default:
                gUnk_0200AF00 = 0;
                break;
            }
            gLevelIndex = 8;
            gStageIndex = gUnk_030023B8;
            gRoomIndex = 0;
            room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
            e = room->doors;
            for (i = 0; i < room->doorCount; e++, i++)
            {
                if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 0 && e->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gRoomEntryX = e->unk2 * 16 + 22;
            gRoomEntryY = e->unk4 * 16 + 5;
            gRoomEntrySet = 1;
            if (gUnk_03001F30 == 0)
            {
                gStageRequest = 8;
            }
            else
            {
                gStageRequest = 1;
                gUnk_02005578 = gUnk_030023B8;
                gUnk_02007FF8 = gUnk_03001F20;
            }
            gUnk_020069F0 = 1;
        }
        else
        {
            if (gUnk_0200B038 != 0)
            {
                gUnk_02008054 = gRoomIndex;
                gUnk_0200AFF4 = d->unk2 * 16 + 16;
                gUnk_02008050 = d->unk4 * 16 + 5;
            }
            gRoomIndex = d->unk0;
            gRoomEntryX = d->unk6;
            gRoomEntryY = d->unk8 + 0xFFFD;
            gRoomEntrySet = 1;
            gStageRequest = 3;
            gUnk_0200AF00 = 0;
            gUnk_020069F0 = 0;
        }
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    return gUnk_02000030 >> 8;
}

void ExitClearedStage(void)
{
    if (gGameState == 8)
    {
        struct RoomDef *room;
        struct Door *d;
        s32 i;

        gStageIndex = gUnk_030023B8;
        gLevelIndex = 8;
        gStageIndex++;
        if (gStageIndex > 7)
            gStageIndex = 7;
        gRoomIndex = 0;
        room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
        d = room->doors;
        for (i = 0; i < room->doorCount; i++)
        {
            if (d->unk0 == 0x270F && (u8)d->unk6 == 1)
                break;
            d++;
        }
        gRoomEntryX = d->unk2 * 16 + 22;
        gRoomEntryY = d->unk4 * 16 + 5;
        gRoomEntrySet = 1;
        gStageRequest = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
        gCutscenePending = 1;
        sub_0802695c();
        gUnk_02007FF0 = 0;
    }
    else
    {
        gLevelIndex = gUnk_030023B8 + 1;
        gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
        gRoomIndex = 0;
        gUnk_020069F0 = 0;
        gRoomEntrySet = 0;
        gStageRequest = 1;
    }
    gUnk_0200B038 = 0;
}

void sub_08025a30(void)
{
    if (gUnk_03001F30 == 0)
    {
        gLevelIndex = 7;
        gStageIndex = 0;
        gRoomIndex = 0;
        gRoomEntrySet = 0;
        if (gGameState == 8)
        {
            gCutscenePending = 1;
            gStageRequest = 2;
        }
        else
        {
            gStageRequest = 1;
        }
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
        gUnk_0200B038 = 0;
    }
    else
    {
        gStageRequest = 7;
    }
    gUnk_02007FF0 = 0;
    gUnk_02007FB0 |= 2;
}

void sub_08025acc(void)
{
    gRoomIndex++;
    gRoomEntrySet = 0;
    gStageRequest = 3;
    gUnk_0200AF00 = 0;
    gUnk_020069F0 = 0;
    gUnk_02007FB0 |= 2;
}

void sub_08025b0c(void)
{
    gRoomIndex++;
    gRoomEntrySet = 0;
    gStageRequest = 3;
    gUnk_0200AF00 = 0;
    gUnk_02004C98 = 0;
    gUnk_020069F0 = 2;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
}

void sub_08025b5c(void)
{
    if (gGameState == 8)
    {
        if (gExtraMode != 0)
            gMilestoneFlags |= 2;
        else
            gMilestoneFlags |= 1;
        gUnk_030023B8 = 6;
        gUnk_03001F20 = 32;
        gUnk_02007FB0 |= 1;
    }
    gUnk_02007FF0 = 0;
    gStageRequest = 7;
}

void sub_08025bc8(s32 id)
{
    struct RoomDef *room;
    struct Door *d;
    s32 i;
    s8 v;
    s32 lvl;

    if (gUnk_02005590[id - 32] == -1)
        return;
    gUnk_02008000 = gLevelIndex;
    gUnk_02007FB4 = gStageIndex;
    gUnk_02007D50 = gRoomIndex;
    gLevelIndex = 8;
    gStageIndex = gUnk_030023B8;
    gRoomIndex = 0;
    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    d = room->doors;
    for (i = 0; i < room->doorCount; d++, i++)
    {
        if (d->unk0 == 0x270F && (u8)d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
            break;
    }
    gUnk_02007E8C = id;
    gUnk_0200AF08 = 1;
    v = ((s8 *)gRoomObjectList.unk4[gUnk_02005590[id - 32]].filler0)[2];
    gUnk_0200001C = v | 0x100;
    gUnk_030023C8[0] |= 1 << v;
    if (gUnk_03002400[gUnk_030023B8][(s8)gUnk_03001F20] == 1)
        gUnk_03002400[gUnk_030023B8][(s8)gUnk_03001F20] = 2;
    lvl = gUnk_030023B8;
    if (gUnk_08732348[lvl][6] == gUnk_0200001C)
    {
        gUnk_0200AF08 |= 16;
        gUnk_0200B04C |= 1 << lvl;
    }
    gUnk_020069F0 = 4;
    gUnk_0200AF00 = 3;
    gRoomEntryX = d->unk2 * 16 + 22;
    gRoomEntryY = d->unk4 * 16 + 5;
    gRoomEntrySet = 1;
    if (gRoomBgLayout == 2)
        gUnk_02004B80 = gBg3Pos[0];
    gStageRequest = 4;
    gUnk_02007FB0 |= 5;
}

void sub_08025dc4(void)
{
    gLevelIndex = gUnk_02008000;
    gStageIndex = gUnk_02007FB4;
    gRoomIndex = gUnk_02007D50;
    gStageRequest = 3;
}

void sub_08025e00(void)
{
    sub_08028b8c();
}

void sub_08025e0c(void)
{
    s32 i;

    gRoomBounds[2] = gRoomHeight * 16 - gRoomBorder[1] - 80;
    gCameraBounds[2] = gRoomBounds[2];
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            gPlayerBounds[i].y0 = gCameraBounds[2] - 76;
    }
    gViewRect[2] = gRoomBounds[2] - 80;
}

s32 sub_08025e88(s32 i)
{
    struct Unk020055D8Entry *e = &gRoomObjectList.unk4[gUnk_02005590[i - 32]];

    if (gUnk_0200B078 == 3)
    {
        gUnk_0200B02C = e->filler0[3] - 1;
        if (gUnk_0200B02C < gStageIndex)
        {
            gUnk_02004C98 = 1;
            return 2;
        }
        gUnk_02004C98 = 2;
        return 0;
    }
    gUnk_02004C98 = 0;
    return ((s8 *)e->filler0)[2];
}

s32 sub_08025f00(void)
{
    s32 i;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gCurRoomDef->doors;

        for (i = 0; i < gCurRoomDef->doorCount; i++)
        {
            if (d->unk0 == 0x1E61 || d->unk0 == 0x1A0A)
                break;
            d++;
        }
        gRoomIndex = d->unk2;
        gRoomEntryX = d->unk6;
        gRoomEntryY = d->unk8;
        gRoomEntrySet = 1;
        gStageRequest = 3;
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    else
    {
        gStageIndex = gUnk_0200B02C;
        gRoomIndex = 0;
        for (i = 0; i < 32; i++)
        {
            if (gRoomTable[gLevelIndex][gStageIndex][i]->unk54 == 3)
            {
                gRoomIndex = i;
                break;
            }
        }
        if (gUnk_02004C98 == 2)
            gRoomEntryX = 0;
        else
            gRoomEntryX = gRoomWidth * 16;
        gRoomEntryY = gRoomHeight * 16 - gRoomBorder[1] - 168;
        gRoomEntrySet = 1;
        gStageRequest = 12;
        gCutscenePending = 0;
    }
    gUnk_020069F0 = 2;
    gUnk_0200B038 = 0;
    return 1;
}

s32 sub_080260b0(void)
{
    s32 r;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gCurRoomDef->doors;
        s32 i;

        for (i = 0; i < gCurRoomDef->doorCount; i++)
        {
            if (d->unk0 == 0x1A0A)
                break;
            d++;
        }
        r = d->unk4;
    }
    else
    {
        r = 3;
        if (gUnk_02004C98 == 2)
            r = 1;
    }
    gUnk_02004C98 = 0;
    return r;
}

s32 sub_0802610c(void)
{
    struct Door *d = gCurRoomDef->doors;
    s32 i;

    for (i = 0; i < gCurRoomDef->doorCount; i++)
    {
        if (d->unk0 == 0x1E61)
            break;
        d++;
    }
    gRoomIndex = d->unk2;
    gRoomEntryX = d->unk6;
    gRoomEntryY = d->unk8;
    gRoomEntrySet = 1;
    gStageRequest = 3;
    gUnk_020069F0 = 3;
    gUnk_0200B038 = 0;
    if (gUnk_02007D60 & 0x8000)
        gUnk_02007D60 = 0;
    return 1;
}
