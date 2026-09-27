#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* room_27e28.c (0x08027E28-0x0802831F, issue #93).
 *
 * Room start-up, part 1.  InitRoomBgLayout (the loaders' second step) sets up
 * the scroll speeds, the BG layout gRoomBgLayout/gUnk_0200B078 for the
 * room (a 7-way switch on RoomDef.unk54, table 0x08027F28), the
 * metatile-map edits of the special rooms and the bottom bound
 * (sub_08025e0c); sub_08028130 is its reduced form for sub_080233e0,
 * sub_08028280 its layout-only form for sub_08024698, and
 * sub_08028304 picks the tile-upload routine for the layout.
 * sub_08027a6c, the first function of the range (the second map buffer
 * gUnk_02006AA0 for sub_08023948/sub_08023ca0), landed separately as
 * src/level_27a6c.c. */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 width;
    /*0x04*/ u16 height;
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

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

struct Unk020055D8
{
    /*0x00*/ s16 count;
    /*0x02*/ s16 sortedByY;
    /*0x04*/ struct Unk020055D8Entry *entries;
};

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};

extern s32 gScrollLockSpeedX;
extern s32 gScrollLockSpeedY;
extern s32 gRoomDriftVelX;
extern s32 gRoomDriftVelY;
extern u8 gRoomBgLayout;
extern s16 *gCurTileDrifts;
extern s16 gTileDrifts[];
extern s16 gTileDriftsDoubled[];
extern u8 gUnk_02005574[];
extern struct RoomDef *gCurRoomDef;
extern struct Unk020055D8 gRoomObjectList;
extern s32 gRoomDriftVelocities[][2];
extern u8 gBg3MapShape;
extern u8 gUnk_0200B078;
extern s32 gBg3ParallaxX;
extern struct MapCell gRoomMapBuffer[];
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern struct MapCell *gRoomMap;
extern u16 gBlockLayer[];
extern s16 gRoomMetatileCount;
extern s16 gRoomBounds[4];
extern u16 gRoomBorder[2];
extern s8 gUnk_030023B8;
extern u8 gUnk_02000020;
extern u16 gCameraMode;
extern u8 gUnk_020069F0;
extern s8 gUnk_02006098[];
extern s8 gUnk_02007D64;
extern struct RoomDef **gRoomTable[][8];
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern u16 gUnk_030012B0[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void LoadGfxSet(u16 a0);
void sub_08025e0c(void);
void SetBg23ScreenSize(u16 a);
void SetBg3ScreenSize(u16 a);
s32 sub_08030074(s32 a);
s32 sub_08030100(u8 a);
void sub_080307b0(void);
void sub_080307cc(void);
void StartRoomHBlankScroll(s32 a);

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
    if ((s16)gCurRoomDef->unk38 != -1)
    {
        e = (s8 *)&gRoomObjectList.entries[(s16)gCurRoomDef->unk38];
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
                ((struct MapTile *)gRoomMap)[idx].unk2 = 0;
                ((struct MapTile *)gRoomMap)[idx].collisionTile = 0;
                gBlockLayer[idx] = 0;
            }
        }
        CpuSet(p, p + 2048, (gRoomMetatileCount * 2) & 0x1FFFFF);
        CpuSet(gBlockLayer, gBlockLayer + 2048, gRoomMetatileCount & 0x1FFFFF);
        gCameraMode = 5;
        break;
    case 2:
        gUnk_0200B078 = 2;
        break;
    case 3:
        gUnk_0200B078 = 3;
        gRoomBounds[2] = gRoomHeight * 16 - gRoomBorder[1] - 80;
        LoadGfxSet(3);
        sub_08030074(gUnk_030023B8);
        break;
    case 4:
        gUnk_0200B078 = 6;
        gRoomBgLayout = 4;
        *gUnk_02005574 = 1;
        SetBg23ScreenSize(0);
        StartRoomHBlankScroll(8);
        gUnk_02000020 = 2;
        gCameraMode = 5;
        break;
    case 5:
        gUnk_0200B078 = 7;
        gRoomBgLayout = 5;
        gRoomBounds[1] = gRoomBorder[0] + 120;
        StartRoomHBlankScroll(9);
        gUnk_02000020 = 3;
        if (gUnk_020069F0 != 2)
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
        sub_08030100(gUnk_030023B8);
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
    if ((s16)gCurRoomDef->unk38 != -1)
    {
        e = (s8 *)&gRoomObjectList.entries[(s16)gCurRoomDef->unk38];
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
            RequestCopy(2, (u32)(next->bg2Palette + 1), (u32)gUnk_030012B0, *gCurRoomDef->bg2Palette);
            RequestCopy(2, (u32)(next->bg3Palette + 1), (u32)gUnk_030012B0 + 0x1C0 - *gCurRoomDef->bg3Palette, *gCurRoomDef->bg3Palette);
        }
    }
}

void sub_08028280(s32 a)
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
    gCameraMode = 5;
}

void sub_08028304(void)
{
    if (gUnk_0200B078 == 1)
        sub_080307cc();
    else
        sub_080307b0();
}
