#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_2296c.c (0x0802296C-0x08023617, issue #93).
 *
 * Level resets and room loaders, part 1.  sub_0802296c (M02's
 * ResetScoresAndMaxHealth) and its twin sub_08022c3c clear the level state, rebuild
 * the per-stage door masks gUsedSubGameDoors[] and the cleared-stage mask
 * gUnk_0200B04C from the save flags (gUnk_08732348[level][6] names each
 * stage's flag) and place the player at the matching door of the hub
 * room gRoomTable[8][stage][0]; sub_08022f50 (AgbMain) resets level,
 * stage and room; sub_08022f98/sub_08022f9c are M02's screen-setup hooks;
 * sub_08022fa8 and sub_080233e0 are the loaders of M02's first screen
 * setup sub_0800b648 (see level_242d0.c); CreateRoomTask spawns task type
 * #3 with its variant index. */

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

extern struct RoomDef **gRoomTable[][8];
extern s8 gLevelIndex;
extern s8 gStageIndex;
extern s8 gRoomIndex;
extern s8 gUnk_030023B8;
extern u8 gUnk_03001F20;
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern u16 gUnk_0200001C;
extern s8 gUnk_02004C98;
extern u8 gUnk_02005578;
extern s16 gRoomEntryX;
extern u8 gRoomEntrySet;
extern u8 gUnk_020069F0;
extern u8 gUsedSubGameDoors[];
extern u16 gUnk_02007D60;
extern u8 gCutscenePending;
extern u16 gUnk_02007FF0;
extern s8 gUnk_02007FF8;
extern s16 gRoomEntryY;
extern u8 gUnk_0200AF00;
extern u8 gUnk_0200AF04;
extern u8 gUnk_0200AF08;
extern s8 gUnk_0200B038;
extern u8 gUnk_0200B04C;
extern u8 gUnk_03001F30;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern u16 gUnk_08732348[][9];
extern u8 gUnk_02004B64;
extern s16 gUnk_0200AF0C;
extern u32 gUnk_02007BF0[8][8];
extern s8 gStageRequest;
extern u8 gUnk_08334EB4[];
extern u8 gUnk_020055C8;
extern s8 gUnk_03002444;
extern u16 gCameraMode;
extern struct RoomDef *gCurRoomDef;
extern s8 gUnk_02007D64;
extern s8 gUnk_02000000;
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern s16 gRoomMetatileCount;
extern u16 gRoomBorder[2];
extern u16 gBg3Border[2];
extern struct Unk020055D8 gRoomObjectList;
extern struct MapCell *gRoomMap;
extern struct MapCell gRoomMapBuffer[];
extern u16 gBlockLayer[];
extern u16 gMetatileTiles[];
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
extern u8 gRoomBgLayout;
extern u16 gCameraPos[2];
extern u16 gBg3Pos[2];
extern u16 gUnk_02004B80;
extern u8 gUnk_02007E8C;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetTasksAndOam(void);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSetOthersSkipMask(u16 val, s32 idx);
s32 TaskCreateFrom(u32 type, s32 idx);
void LoadGfxSet(u16 a0);
void HudShowClock(void);
void HudInit(s32 i);
s32 AddPlayerLives(s32 a, u32 b);
void InitRoomBgLayout(void);
void sub_08028130(void);
void sub_08028304(void);
void CalcBg3Parallax(void);
void CalcRoomBounds(void);
void CameraResetBoundsToGroup(void);
void CameraResetBounds(void);
void SetRoomEntryPoint(void);
void sub_080290ac(void);
void CameraInitPos(void);
void sub_08029110(void);
void LoadBg2Gfx(void);
void LoadBg3Gfx(void);
void ClearBg2Bg3Maps(void);
void SelectBg3MapShape(void);
void LoadBg3Map(void);
void SpawnRoomObjectsInView(void);
void InitDoors(void);
void CameraWriteScrollParallax(void);
void DrawBg2View(s32 px, s32 py);
void DrawBg3View(s32 px, s32 py);
void DrawBg23FullRows(s32 py);
void sub_0802b074(s32 px);
void CameraFollowFocus(void);
void CameraFollowScrollLocked(void);
void CameraHoldAnchor(void);
void StopScreenShake(void);
void LoadRoomBgAnims(void);
void ResetBlockAnims(void);
void sub_080307b0(void);
void CreatePlayer(s32 a0);
void sub_0803d1c4(s32 a0);
void sub_08077d38(s32 id);
void sub_080b4e40(void);
void sub_080b4ea8(void);
void sub_080b5024(void);
void CreateRoomTask(s32 a);

void sub_0802296c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gUnk_0200AF08 = 0;
    gUnk_0200001C = 0;
    gUnk_02004B64 = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gUnk_0200B04C = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUnk_02007BF0[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gUnk_03001F30)
            gUsedSubGameDoors[i] = 15;
        else
            gUsedSubGameDoors[i] = 0;
        if (gUnk_08732348[i][6] & 0x100)
        {
            if (gUnk_030023C8[0] & (1 << (gUnk_08732348[i][6] & 0xFF)))
                gUnk_0200B04C |= 1 << i;
        }
        else if (gUnk_03002400[i][gUnk_08732348[i][6]] != 0)
        {
            gUnk_0200B04C |= 1 << i;
        }
    }
    gLevelIndex = 8;
    gStageIndex = gUnk_030023B8;
    gRoomIndex = 0;
    gUnk_0200AF04 = 0;
    gUnk_02005578 = gStageIndex;
    gUnk_02007FF8 = gUnk_03001F20;
    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    d = room->doors;
    if (gUnk_030023E0 == 0 && gUnk_03002384 == 0)
    {
        gRoomEntrySet = 0;
        gUnk_020069F0 = 2;
    }
    else
    {
        if ((s8)gUnk_03001F20 == 16)
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 22;
            gRoomEntryY = d->unk4 * 16 + 5;
            gUnk_020069F0 = 1;
            gUnk_0200AF00 = 0;
        }
        else if ((s8)gUnk_03001F20 == 32)
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 16;
            gRoomEntryY = d->unk4 * 16 + 53;
            gUnk_020069F0 = 0;
        }
        else
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 22;
            gRoomEntryY = d->unk4 * 16 + 5;
            gUnk_020069F0 = 1;
            gUnk_0200AF00 = 0;
        }
        gRoomEntrySet = 1;
    }
    gCutscenePending = 1;
}

void sub_08022c3c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gUnk_0200AF08 = 0;
    gUnk_0200001C = 0;
    gUnk_02004B64 = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gUnk_0200B04C = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUnk_02007BF0[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gUnk_03001F30)
            gUsedSubGameDoors[i] = 15;
        else
            gUsedSubGameDoors[i] = 0;
        if (gUnk_08732348[i][6] & 0x100)
        {
            if (gUnk_030023C8[0] & (1 << (gUnk_08732348[i][6] & 0xFF)))
                gUnk_0200B04C |= 1 << i;
        }
        else if (gUnk_03002400[i][gUnk_08732348[i][6]] != 0)
        {
            gUnk_0200B04C |= 1 << i;
        }
    }
    if (gUnk_030023B8 == 7)
    {
        gLevelIndex = 7;
        gStageIndex = 0;
        gRoomIndex = 0;
        gRoomEntrySet = 0;
        gStageRequest = 2;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
    }
    else
    {
        gLevelIndex = 8;
        gStageIndex = gUnk_02005578;
        gRoomIndex = 0;
        gUnk_0200AF04 = 0;
        if (gUnk_030023E0 == 0 && gUnk_03002384 == 0)
        {
            gRoomEntryX = 70;
            gRoomEntryY = 0x105;
            gUnk_020069F0 = 0;
            gUnk_0200AF00 = 0;
        }
        else
        {
            room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
            d = room->doors;
            if (gUnk_02007FF8 == 16)
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
            }
            else if (gUnk_02007FF8 == 32)
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 16;
                gRoomEntryY = d->unk4 * 16 + 53;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
            }
            else
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == gUnk_02007FF8)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
            }
        }
        gStageRequest = 1;
        gRoomEntrySet = 1;
    }
    gUnk_0200B038 = 0;
    gCutscenePending = 1;
}

void sub_08022f50(void)
{
    gLevelIndex = 0;
    gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
    gRoomIndex = 0;
    gUnk_020069F0 = 0;
    gRoomEntrySet = 0;
    gCutscenePending = 0;
    HudShowClock();
}

void sub_08022f98(void)
{
}

void sub_08022f9c(void)
{
    gUnk_020055C8 = 0;
}

void sub_08022fa8(void)
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
    InitRoomBgLayout();
    sub_080b4e40();
    SetRoomEntryPoint();
    sub_08029110();
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
        sub_0803d1c4(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    sub_080b5024();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
    HudInit(gLocalPlayer);
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

void sub_080233e0(void)
{
    u32 a;

    TaskSetSkipMask(0, gUnk_02007E8C);
    TaskSetOthersSkipMask(15, gUnk_02007E8C);
    sub_08077d38(gUnk_02007E8C);
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
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gRoomUpdateFlags = 0;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    sub_08028130();
    sub_080b4ea8();
    InitDoors();
    sub_080307b0();
    sub_080290ac();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
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
    if (gRoomBgLayout == 2)
        gBg3Pos[0] = gUnk_02004B80;
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    if (gRoomBgLayout == 1)
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

void CreateRoomTask(s32 a)
{
    s32 id = TaskCreateFrom(3, 63);
    struct Task *t;

    if (id != -1)
    {
        t = &gTasks[id];
        t->state = a;
    }
}
