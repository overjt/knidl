#include "gba/gba.h"
#include "global.h"
#include "collision.h"
#include "room.h"

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
 *    re-extending it (TerrainProbePointStop, TerrainProbeWallRightOnGround); the sign extension comes
 *    from the s8 element type (lesson 3.356);
 *  - `p = table[i]; return p[j];` orders the pointer load before the index
 *    load; writing table[i][j] in one expression loads the index first;
 *  - the row/column cell access is `(&gRoomMap[idx])[x]` with
 *    `idx = y * w` in its own statement: a `row` local hoists the map base
 *    load above the multiply and shifts the whole register allocation;
 *  - GetTileShapeAtPixel is GetShapeAtPixelIgnoringOneWay without the attribute guard, but its second
 *    range check is written positively (the fail path sits before the pool).
 */

void TerrainProbeWaterAtPoint(void)
{
    s32 y;
    s32 h;

    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY);
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.waterFlags = 0;
    y = gTerrainProbeY;
    h = gRoomHeight << 4;
    if (y >= h)
    {
        TerrainQueryPixel(gTerrainProbeX, h - 16);
        if (gTerrainTile & 0x80)
            gTerrainProbeResult.waterFlags = 11;
    }
    else
    {
        TerrainQueryPixel(gTerrainProbeX, y);
        if (gTerrainTile > 127)
            gTerrainProbeResult.waterFlags = 129;
        else
            gTerrainProbeResult.waterFlags = 0;
    }
}

void TerrainProbeDamage(void)
{
    gTerrainProbeResult.damage = 0;
    gTerrainProbeResult.damage = TerrainQueryDamage(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) | gTerrainProbeResult.damage;
    gTerrainProbeResult.damage = TerrainQueryDamage(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) | gTerrainProbeResult.damage;
    gTerrainProbeResult.damage = TerrainQueryDamage(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) | gTerrainProbeResult.damage;
    gTerrainProbeResult.damage = TerrainQueryDamage(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) | gTerrainProbeResult.damage;
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
        gTerrainSlopeIndex = gRoomMap[idx].slopeIndex;
        gTerrainTileShape = gCollisionTileShapes[gTerrainTile];
        return gTerrainTileShape[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gTerrainSlopeIndex = 0;
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
            gTerrainSlopeIndexBelow = (&gRoomMap[idx])[gRoomWidth].slopeIndex;
        }
        else
        {
            gTerrainTileBelow = gTerrainSlopeIndexBelow = 0;
        }
        gTerrainTile = gRoomMap[idx].collisionTile;
        gTerrainSlopeIndex = gRoomMap[idx].slopeIndex;
        p = gCollisionTileShapes[gTerrainTile];
        return p[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gTerrainSlopeIndex = gTerrainTileBelow = gTerrainSlopeIndexBelow = 0;
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
        gTerrainSlopeIndexLeft = (&gRoomMap[idx])[-1].slopeIndex;
        if (x + 1 < gRoomWidth)
        {
            gTerrainTileRight = (&gRoomMap[idx])[1].collisionTile;
            gTerrainSlopeIndexRight = (&gRoomMap[idx])[1].slopeIndex;
        }
        else
        {
            gTerrainTileRight = gTerrainSlopeIndexRight = 0;
        }
        gTerrainTile = gRoomMap[idx].collisionTile;
        gTerrainSlopeIndex = gRoomMap[idx].slopeIndex;
        p = gCollisionTileShapes[gTerrainTile];
        return p[gTerrainPixelIndex];
    }
    gTerrainPixelIndex = gTerrainTile = gTerrainSlopeIndex = gTerrainTileLeft = gTerrainSlopeIndexLeft = gTerrainTileRight = gTerrainSlopeIndexRight = 0;
    return 0;
}

s32 TerrainQueryDamage(u32 x, u32 y)
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
    if (gCollisionTileDamaging[tile] == 0)
        return 0;
    return gCollisionTileDamageShapes[tile][off];
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

void TerrainLoadFloorAttributes(u16 a)
{
    gTerrainProbeResult.slope = gCollisionTileSlope[a];
    gTerrainProbeResult.unk5 = gUnk_087337F0[a];
    gTerrainProbeResult.onSlipperyFloor = gCollisionTileSlippery[a];
}

s32 GetShapeAtPixelIgnoringOneWay(u32 x, u32 y)
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
    if (gCollisionTileOneWay[tile] != 0)
        return 0;
    p = gCollisionTileShapes[tile];
    return p[((y & 15) << 4) + (x & 15)];
}

s32 GetTileShapeAtPixel(u32 x, u32 y)
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
