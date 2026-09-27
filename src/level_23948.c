#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_23948.c (0x08023948-0x080242CF, issue #93).
 *
 * Room loaders, part 2: sub_08023948 and sub_08023ca0 (M02's screen
 * setups sub_0800b788 and sub_0800b87c) and sub_08023fd4, plus the
 * room-task variants 1 and 2 of task type #3 (sub_08023e34, sub_08023e78)
 * and their per-frame bodies (sub_08023ea0 ... sub_08023fa0).
 * sub_08023948 and sub_08023ca0 build their map in the second buffer
 * gUnk_02006AA0 through sub_08027a6c instead of gRoomMapBuffer, spawn the
 * door objects and set up the multi-player cameras; sub_08023fd4 loads
 * the fixed room gRoomTable[8][7][0] with the player at (136, 928) and
 * BGM 1.  Every loader ends with the per-player loop that refills health,
 * rebuilds the player mask gActivePlayerMask and restarts the player tasks
 * (CreatePlayer and InitPlayerState/sub_0803d1c4). */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

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

extern s8 gUnk_03002444;
extern s8 gUnk_030023B8;
extern s8 gStageIndex;
extern u8 gUnk_03001F20;
extern u8 gUnk_0200AF08;
extern u8 gUnk_020069F0;
extern u16 gCameraMode;
extern struct RoomDef *gCurRoomDef;
extern struct RoomDef **gRoomTable[][8];
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern s8 gUnk_02007D64;
extern s8 gUnk_02000000;
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern s16 gRoomMetatileCount;
extern u16 gRoomBorder[2];
extern u16 gBg3Border[2];
extern struct MapCell *gRoomMap;
extern struct MapCell gUnk_02006AA0[];
extern u8 gRoomBgLayout;
extern s16 *gUnk_0300558C;
extern s16 gUnk_0873A318[];
extern u8 gUnk_02005574[];
extern u8 gUnk_02000020;
extern u8 gUnk_0200B078;
extern u8 gHBlankScrollStarted;
extern u16 gRoomUpdateFlags;
extern u8 gActivePlayerMask;
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
extern u16 gLocalPlayer;
extern u16 gCameraPos[2];
extern struct MapCell gRoomMapBuffer[];
extern u16 gMetatileTiles[];
extern u16 gUnk_02007FB0;
extern s16 gCameraFocusX;
extern s16 gCameraFocusY;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetTasksAndOam(void);
s32 PlayBgm(s32 songId);
void TaskSleepForever(void);
void LoadGfxSet(u16 a0);
void HudInit(s32 i);
s32 AddPlayerLives(s32 a, u32 b);
void HudUpdateAbilityPanel(void);
void CreateRoomTask(s32 a);
void RoomTaskDraw(void);
s32 sub_08026834(void);
void sub_08026b60(void);
void UpdateDoors(void);
void sub_08027a6c(void);
void SpawnDoorObjects(void);
void CalcBg3Parallax(void);
void CalcRoomBounds(void);
void CameraResetBounds(void);
void SetRoomEntryPoint(void);
void sub_08029034(void);
void CameraInitPos(void);
void sub_08029194(void);
void LoadBg2Gfx(void);
void LoadBg3Gfx(void);
void ClearBg2Bg3Maps(void);
void SelectBg3MapShape(void);
void InitDoors(void);
void CameraUpdatePosNoParallax(void);
void StreamBg123Maps(void);
void StreamBg23Maps(void);
void CameraWriteScrollParallax(void);
void CameraWriteScrollBg23(void);
void CameraWriteScrollBg123(void);
void DrawBg123View(s32 px, s32 py);
void DrawBg23View(s32 px, s32 py);
void sub_0802c550(void);
void sub_0802c680(void);
void sub_0802c7f4(void);
void CameraSnapBoundsToAnchor(void);
void CameraSnapPlayersToAnchor(void);
void CameraSnapToFocus(void);
void StopScreenShake(void);
void UpdateScreenShake(void);
void LoadRoomBgAnims(void);
void ResetBlockAnims(void);
void CreatePlayer(s32 a0);
void InitPlayerState(s32 a0);
void sub_0803d1c4(s32 a0);
void LatchPlayerKeys(void);
void sub_08023ea0(void);
void sub_08023efc(void);
void sub_08023f18(void);
void sub_08023f5c(void);
void sub_08023fa0(void);

void sub_08023948(void)
{
    s32 i;
    u32 a;

    ResetTasksAndOam();
    gUnk_03002444 = 1;
    gUnk_030023B8 = gStageIndex;
    gUnk_03001F20 = 16;
    if (gUnk_0200AF08 != 0 || gUnk_020069F0 == 2)
        gCameraMode = 4;
    else
        gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(1);
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
    gRoomMap = gUnk_02006AA0;
    sub_08027a6c();
    gRoomBgLayout = 0;
    gUnk_0300558C = gUnk_0873A318;
    *gUnk_02005574 = 0;
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitDoors();
    SetRoomEntryPoint();
    SpawnDoorObjects();
    sub_08029194();
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
        sub_0803d1c4(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    CameraResetBounds();
    HudInit(gLocalPlayer);
    switch (gCameraMode)
    {
    default:
    case 0:
    case 3:
        sub_0802c550();
        break;
    case 2:
    case 4:
        CameraSnapPlayersToAnchor();
        break;
    case 1:
        sub_0802c680();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    if (gUnk_0200AF08 != 0)
        DrawBg123View(gCameraPos[0], gCameraPos[1]);
    else
        DrawBg23View(gCameraPos[0], gCameraPos[1]);
}

void sub_08023ca0(void)
{
    u8 z;
    u32 w;
    u32 zero;

    gUnk_03002444 = 1;
    gUnk_030023B8 = gStageIndex;
    gUnk_03001F20 = 16;
    gCameraMode = 2;
    LoadGfxSet(1);
    CreateRoomTask(2);
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
    gRoomMap = gUnk_02006AA0;
    z = 0;
    w = 0;
    sub_08027a6c();
    gRoomBgLayout = z;
    gUnk_0300558C = gUnk_0873A318;
    gUnk_02005574[0] = z;
    gUnk_02000020 = z;
    gUnk_0200B078 = z;
    gHBlankScrollStarted = z;
    gRoomUpdateFlags = 3;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitDoors();
    SpawnDoorObjects();
    sub_08029034();
    sub_08026b60();
    sub_08026834();
    CameraResetBounds();
    CameraSnapBoundsToAnchor();
    CameraInitPos();
    CameraWriteScrollParallax();
    zero = w;
    CpuFastSet(&zero, (void *)0x06002000, 0x01000400);
    DrawBg123View(gCameraPos[0], gCameraPos[1]);
}

void sub_08023e34(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = (u32)RoomTaskDraw;
    t->unk04 = (u32)sub_08023ea0;
    if (gUnk_0200AF08 != 0)
        t->unk08 = (u32)sub_08023f5c;
    else
        t->unk08 = (u32)sub_08023f18;
    TaskSleepForever();
}

void sub_08023e78(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08023efc;
    t->unk08 = (u32)sub_08023fa0;
    TaskSleepForever();
}

void sub_08023ea0(void)
{
    if (gRoomUpdateFlags & 1)
    {
        switch (gCameraMode)
        {
        default:
        case 0:
            sub_0802c550();
            break;
        case 2:
        case 4:
            CameraSnapPlayersToAnchor();
            break;
        case 3:
            sub_0802c7f4();
            break;
        case 1:
            sub_0802c680();
            break;
        }
    }
}

void sub_08023efc(void)
{
    if (gRoomUpdateFlags & 1)
        CameraSnapBoundsToAnchor();
}

void sub_08023f18(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg23Maps();
    }
    CameraWriteScrollBg23();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    HudUpdateAbilityPanel();
}

void sub_08023f5c(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg123Maps();
    }
    CameraWriteScrollBg123();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    HudUpdateAbilityPanel();
}

void sub_08023fa0(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg123Maps();
    }
    CameraWriteScrollBg123();
}

void sub_08023fd4(void)
{
    s32 i;
    u32 a;

    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(4);
    gCurRoomDef = gRoomTable[8][7][0];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gUnk_02000000 = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gUnk_03002444 = 0;
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
    gUnk_02007FB0 = 0;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    LoadRoomBgAnims();
    gCameraFocusX = 136;
    gCameraFocusY = 928;
    PlayBgm(1);
    gRoomBgLayout = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 1;
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
        }
        else
        {
            gPlayerCameraMode[i] = 0;
        }
        CreatePlayer(i);
        InitPlayerState(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    LatchPlayerKeys();
    CameraResetBounds();
    HudInit(gLocalPlayer);
    CameraSnapToFocus();
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    DrawBg23View(gCameraPos[0], gCameraPos[1]);
}
