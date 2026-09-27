#include "gba/gba.h"
#include "global.h"

/*
 * M06 terrain / collision query (issue #84), range 0x080214E0-0x08021B18.
 *
 * Tile-attribute lookups on the current room: the room descriptor cells at
 * 0x030055xx hold the map size (gRoomWidth x gRoomHeight cells of 16x16
 * pixels), the cell array pointer (gRoomMap, 4 bytes per cell, byte 3 is
 * the tile-set index) and the last query results; the 0x100-stride ROM index
 * tables at 0x087328F0.. map a tile-set index to its per-pixel attribute
 * table.  All query functions take pixel coordinates and return the signed
 * attribute byte for that pixel, or 0 when the coordinate is outside the map.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork -fprologue-bugfix):
 *  - the helpers return int, not s8: callers compare the result without
 *    re-extending it (sub_0802069c, sub_0801c690); the sign extension comes
 *    from the s8 element type (lesson 3.356);
 *  - `p = table[i]; return p[j];` orders the pointer load before the index
 *    load; writing table[i][j] in one expression loads the index first;
 *  - the row/column cell access is `(&gRoomMap[idx])[x]` with
 *    `idx = y * w` in its own statement: a `row` local hoists the map base
 *    load above the multiply and shifts the whole register allocation;
 *  - sub_08021ab4 is sub_08021a40 without the attribute guard, but its second
 *    range check is written positively (the fail path sits before the pool).
 */


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern s8 *const gCollisionTileFloorSnap[];
extern s8 *const gCollisionTilePushDown[];
extern s8 *const gCollisionTilePushUp[];
extern s8 *const gCollisionTilePushRight[];
extern s8 *const gCollisionTilePushLeft[];
extern s8 *const gUnk_087330F0[];
extern s8 *const gCollisionTileShapes[];

/* ROM byte tables indexed by tile set. */
extern u8 gCollisionTileSlope[];
extern u8 gUnk_087337F0[];
extern u8 gCollisionTileSlippery[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern u16 gTerrainPixelIndex;
extern u16 gTerrainTileRight;
extern u16 gTerrainPrevX;
extern u16 gTerrainPrevY;
extern s16 gTerrainBoxLeft;
extern s16 gTerrainProbeX;
extern u16 gUnk_0300556C;
extern s16 gTerrainProbeY;
extern u16 gUnk_03005574;
extern u16 gTerrainTile;
extern s16 gTerrainBoxTop;
extern s16 gTerrainBoxBottom;
extern u16 gTerrainTileBelow;
extern u16 gTerrainTileLeft;
extern s16 gTerrainBoxRight;
extern s8 *gTerrainTileShape;
extern u16 gUnk_030055AC;
extern s16 gRoomMetatileCount;
extern s16 gRoomHeight;
extern s16 gRoomWidth;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};
extern struct MapCell *gRoomMap;

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
};
extern struct Unk03005530 gTerrainProbeResult;

s32 sub_080218f8(u32 x, u32 y);

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 sub_080218f8(u32 x, u32 y);

void sub_080214e0(void)
{
    s32 y;
    s32 h;

    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY);
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.unk7 = 0;
    y = gTerrainProbeY;
    h = gRoomHeight << 4;
    if (y >= h)
    {
        TerrainQueryPixel(gTerrainProbeX, h - 16);
        if (gTerrainTile & 0x80)
            gTerrainProbeResult.unk7 = 11;
    }
    else
    {
        TerrainQueryPixel(gTerrainProbeX, y);
        if (gTerrainTile > 127)
            gTerrainProbeResult.unk7 = 129;
        else
            gTerrainProbeResult.unk7 = 0;
    }
}

void sub_08021564(void)
{
    gTerrainProbeResult.unkF = 0;
    gTerrainProbeResult.unkF = sub_080218f8(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) | gTerrainProbeResult.unkF;
    gTerrainProbeResult.unkF = sub_080218f8(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) | gTerrainProbeResult.unkF;
    gTerrainProbeResult.unkF = sub_080218f8(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) | gTerrainProbeResult.unkF;
    gTerrainProbeResult.unkF = sub_080218f8(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) | gTerrainProbeResult.unkF;
}

s32 TerrainQueryPixel(u32 x, u32 y)
{
    s16 w;
    u32 idx;

    gTerrainPixelIndex = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x < w && y < gRoomHeight)
    {
        idx = y * w + x;
        gTerrainTile = gRoomMap[idx].collisionTile;
        gUnk_03005574 = gRoomMap[idx].unk2;
        gTerrainTileShape = gCollisionTileShapes[gTerrainTile];
        return gTerrainTileShape[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gUnk_03005574 = 0;
    return 0;
}

s32 TerrainQueryPixelAndBelow(u32 x, u32 y)
{
    s16 w;
    s32 idx;
    s8 *p;

    gTerrainPixelIndex = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x < w && y < gRoomHeight)
    {
        idx = y * w + x;
        if (idx + w <= gRoomMetatileCount)
        {
            gTerrainTileBelow = (&gRoomMap[idx])[w].collisionTile;
            gUnk_030055AC = (&gRoomMap[idx])[gRoomWidth].unk2;
        }
        else
        {
            gTerrainTileBelow = gUnk_030055AC = 0;
        }
        gTerrainTile = gRoomMap[idx].collisionTile;
        gUnk_03005574 = gRoomMap[idx].unk2;
        p = gCollisionTileShapes[gTerrainTile];
        return p[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gUnk_03005574 = gTerrainTileBelow = gUnk_030055AC = 0;
    return 0;
}

s32 TerrainQueryPixelAndSides(u32 x, u32 y)
{
    s16 w;
    u32 idx;
    s8 *p;

    gTerrainPixelIndex = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x < w && y < gRoomHeight)
    {
        idx = y * w + x;
        gTerrainTileLeft = (&gRoomMap[idx])[-1].collisionTile;
        gUnk_03005504 = (&gRoomMap[idx])[-1].unk2;
        if (x + 1 < gRoomWidth)
        {
            gTerrainTileRight = (&gRoomMap[idx])[1].collisionTile;
            gUnk_0300556C = (&gRoomMap[idx])[1].unk2;
        }
        else
        {
            gTerrainTileRight = gUnk_0300556C = 0;
        }
        gTerrainTile = gRoomMap[idx].collisionTile;
        gUnk_03005574 = gRoomMap[idx].unk2;
        p = gCollisionTileShapes[gTerrainTile];
        return p[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gUnk_03005574 = gTerrainTileLeft = gUnk_03005504 = gTerrainTileRight = gUnk_0300556C = 0;
    return 0;
}

s32 sub_080218f8(u32 x, u32 y)
{
    u32 off = ((y & 15) << 4) + (x & 15);
    s16 w;
    struct MapCell *row;
    u8 tile;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x >= w)
        return 0;
    if (y >= gRoomHeight)
        return 0;
    idx = y * w;
    tile = (&gRoomMap[idx])[x].collisionTile;
    if (gUnk_08732FF0[tile] == 0)
        return 0;
    return gUnk_087330F0[tile][off];
}

s32 GetTileFloorSnap(u16 a)
{
    s8 *p = gCollisionTileFloorSnap[a];
    return p[gTerrainPixelIndex];
}

s32 GetTilePushDown(u16 a)
{
    s8 *p = gCollisionTilePushDown[a];
    return p[gTerrainPixelIndex];
}

s32 GetTilePushUp(u16 a)
{
    s8 *p = gCollisionTilePushUp[a];
    return p[gTerrainPixelIndex];
}

s32 GetTilePushRight(u16 a)
{
    s8 *p = gCollisionTilePushRight[a];
    return p[gTerrainPixelIndex];
}

s32 GetTilePushLeft(u16 a)
{
    s8 *p = gCollisionTilePushLeft[a];
    return p[gTerrainPixelIndex];
}

void sub_08021a10(u16 a)
{
    gTerrainProbeResult.slope = gCollisionTileSlope[a];
    gTerrainProbeResult.unk5 = gUnk_087337F0[a];
    gTerrainProbeResult.unkE = gCollisionTileSlippery[a];
}

s32 sub_08021a40(u32 x, u32 y)
{
    u32 cx = x >> 4;
    u32 cy;
    s16 w = gRoomWidth;
    u8 tile;
    u32 idx;
    s8 *p;

    if (cx >= w)
        return 0;
    cy = y >> 4;
    if (cy >= gRoomHeight)
        return 0;
    idx = cy * w;
    tile = (&gRoomMap[idx])[cx].collisionTile;
    if (gUnk_087336F0[tile] != 0)
        return 0;
    p = gCollisionTileShapes[tile];
    return p[((y & 15) << 4) + (x & 15)];
}

s32 sub_08021ab4(u32 x, u32 y)
{
    u32 cx = x >> 4;
    u32 cy;
    s16 w = gRoomWidth;
    u8 tile;
    u32 idx;
    s8 *p;

    if (cx >= w)
        return 0;
    cy = y >> 4;
    if (cy < gRoomHeight)
    {
        idx = cy * w;
        tile = (&gRoomMap[idx])[cx].collisionTile;
        p = gCollisionTileShapes[tile];
        return p[((y & 15) << 4) + (x & 15)];
    }
    return 0;
}
