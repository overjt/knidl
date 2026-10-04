#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "mode.h"
#include "room.h"
#include "camera.h"
#include "save.h"

/* room_27e28.c (0x08027E28-0x0802831F, issue #93).
 *
 * Room start-up, part 1.  InitRoomBgLayout (the loaders' second step) sets up
 * the scroll speeds, the BG layout gRoomBgLayout/gUnk_0200B078 for the
 * room (a 7-way switch on RoomDef.unk54, table 0x08027F28), the
 * metatile-map edits of the special rooms and the bottom bound
 * (sub_08025e0c); sub_08028130 is its reduced form for sub_080233e0,
 * InitEndingRoomBgLayout its layout-only form for LoadEndingRoom, and
 * StartRoomBlockAnims picks the tile-upload routine for the layout.
 * sub_08027a6c, the first function of the range (the second map buffer
 * gHubRoomMapBuffer for LoadHubRoom/LoadBigSwitchViewRoom), landed separately as
 * src/level_27a6c.c. */

struct RoomObjectEntry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void InitRoomBgLayout(void)
{
    s8 *e;
    struct MapCell *p;
    s32 i, j;
    s16 idx;

    gScrollLockSpeedX = 0;
    gScrollLockSpeedY = 0;
    gRoomDriftVelX = 0;
    gRoomDriftVelY = 0;
    gRoomBgLayout = 0;
    gCurTileDrifts = gTileDrifts;
    gUnk_02005574[0] = 0;
    if ((s16)gCurRoomDef->driftObjectIndex != -1)
    {
        e = (s8 *)&gRoomObjectList.entries[(s16)gCurRoomDef->driftObjectIndex];
        if (e[1] == 7)
        {
            gRoomDriftVelX = gRoomDriftVelocities[e[2]][0];
            gRoomDriftVelY = gRoomDriftVelocities[e[2]][1];
        }
        else if (e[1] == 8)
        {
            gCurTileDrifts = gTileDriftsDoubled;
        }
    }
    if (gCurRoomDef->unk56 != 0)
        gRoomBgLayout = 2;
    else if (gBg3MapShape == 2)
        gRoomBgLayout = 1;
    else if (gBg3MapShape == 1)
        SetBg3ScreenSize(0x8000);
    switch (gCurRoomDef->unk54)
    {
    case 1:
        gUnk_0200B078 = 1;
        gRoomBgLayout = 3;
        *gUnk_02005574 = 1;
        SetBg23ScreenSize(0x8000);
        gBg3ParallaxX = 0;
        i = 13;
        p = gRoomMapBuffer;
        for (; i <= 18; i++)
        {
            for (j = 0; j <= 16; j++)
            {
                idx = gRoomWidth * i + j;
                ((struct MapTile *)gRoomMap)[idx].metatile = 0;
                ((struct MapTile *)gRoomMap)[idx].slopeIndex = 0;
                ((struct MapTile *)gRoomMap)[idx].collisionTile = 0;
                gBlockLayer[idx] = 0;
            }
        }
        CpuSet(p, p + 2048, (gRoomMetatileCount * 2) & 0x1FFFFF);
        CpuSet(gBlockLayer, gBlockLayer + 2048, gRoomMetatileCount & 0x1FFFFF);
        gCameraMode = CAMERA_MODE_HOLD_ANCHOR;
        break;
    case 2:
        gUnk_0200B078 = 2;
        break;
    case 3:
        gUnk_0200B078 = 3;
        gRoomBounds[2] = gRoomHeight * 16 - gRoomBorder[1] - 80;
        LoadGfxSet(3);
        CreateWarpStarStationLevelSign(gCurLevel);
        break;
    case 4:
        gUnk_0200B078 = 6;
        gRoomBgLayout = 4;
        *gUnk_02005574 = 1;
        SetBg23ScreenSize(0);
        StartRoomHBlankScroll(8);
        gUnk_02000020 = 2;
        gCameraMode = CAMERA_MODE_HOLD_ANCHOR;
        break;
    case 5:
        gUnk_0200B078 = 7;
        gRoomBgLayout = 5;
        gRoomBounds[1] = gRoomBorder[0] + 120;
        StartRoomHBlankScroll(9);
        gUnk_02000020 = 3;
        if (gRoomEntryMode != 2)
            sub_08025e0c();
        break;
    case 6:
        gUnk_0200B078 = 4;
        gUnk_02006098[0] = 0;
        break;
    case 7:
        gUnk_0200B078 = 5;
        gUnk_02006098[0] = 1;
        break;
    }
    if (gUnk_02007D64 == 3)
        CreateMuseumAbilitySigns(gCurLevel);
}

void sub_08028130(void)
{
    s8 *e;
    struct RoomDef *next;

    gScrollLockSpeedX = 0;
    gScrollLockSpeedY = 0;
    gRoomDriftVelX = 0;
    gRoomDriftVelY = 0;
    gRoomBgLayout = 0;
    gUnk_02005574[0] = 0;
    if ((s16)gCurRoomDef->driftObjectIndex != -1)
    {
        e = (s8 *)&gRoomObjectList.entries[(s16)gCurRoomDef->driftObjectIndex];
        if (e[1] == 7)
        {
            gRoomDriftVelX = gRoomDriftVelocities[e[2]][0];
            gRoomDriftVelY = gRoomDriftVelocities[e[2]][1];
        }
    }
    if (gCurRoomDef->unk56 != 0)
        gRoomBgLayout = 2;
    else if (gBg3MapShape == 2)
        gRoomBgLayout = 1;
    else if (gBg3MapShape == 1)
        SetBg3ScreenSize(0x8000);
    if (gCurRoomDef->unk54 != 0)
    {
        gUnk_0200B078 = 4;
        if (gUnk_02006098[0] == 1)
        {
            next = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1];
            RequestCopy(2, (u32)(next->bg2Palette + 1), (u32)gBgPaletteBank2, *gCurRoomDef->bg2Palette);
            RequestCopy(2, (u32)(next->bg3Palette + 1), (u32)gBgPaletteBank2 + 0x1C0 - *gCurRoomDef->bg3Palette, *gCurRoomDef->bg3Palette);
        }
    }
}

void InitEndingRoomBgLayout(s32 a)
{
    gScrollLockSpeedX = 0;
    gScrollLockSpeedY = 0;
    gRoomDriftVelX = 0;
    gRoomDriftVelY = 0;
    gRoomBgLayout = 0;
    gUnk_02005574[0] = 0;
    if (a == 0)
    {
        gUnk_0200B078 = 6;
        gRoomBgLayout = 4;
        SetBg23ScreenSize(0);
        StartRoomHBlankScroll(10);
    }
    else if (gBg3MapShape == 2)
    {
        gRoomBgLayout = 1;
    }
    else if (gBg3MapShape == 1)
    {
        SetBg3ScreenSize(0x8000);
    }
    gCameraMode = CAMERA_MODE_HOLD_ANCHOR;
}

void StartRoomBlockAnims(void)
{
    if (gUnk_0200B078 == 1)
        StartBlockAnimsWithEdges();
    else
        StartBlockAnims();
}
