#include "gba/gba.h"
#include "global.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0802069C-0x080207A0.
 *
 * Tile-attribute lookups on the current room: the room descriptor cells at
 * 0x030055xx hold the map size (gRoomWidth x gRoomHeight cells of 16x16
 * pixels), the cell array pointer (gRoomMap, 4 bytes per cell, byte 3 is
 * the tile-set index) and the last query results; the 0x100-stride ROM index
 * tables at 0x087328F0.. map a tile-set index to its per-pixel attribute
 * table.  All query functions take pixel coordinates and return the signed
 * attribute byte for that pixel, or 0 when the coordinate is outside the map.
 */

u32 sub_0802069c(void)
{
    u32 result = 0;
    s8 v;

    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        if (gUnk_087336F0[gTerrainTileBelow] != 0)
        {
            gTerrainProbeResult.unkB |= 1;
            if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
                gTerrainProbeResult.unkC = (u16)gTerrainProbeY >> 4;
            else
                gTerrainProbeResult.unkC = (gTerrainProbeY + 16) >> 4;
        }
        else
        {
            gTerrainProbeResult.unkB &= 0xFE;
        }
    }
    else
    {
        v = gUnk_087336F0[gTerrainTile];
        if (v == 0 || ((gTerrainProbeResult.unkB & 1) && gTerrainProbeResult.unkC <= gTerrainProbeY >> 4))
        {
            gTerrainProbeResult.unk2 = 1;
            gTerrainProbeResult.unk0 = 3;
            gTerrainProbeResult.ceilingHits = 1;
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeX = gTerrainPrevX;
            gTerrainProbeY = gTerrainPrevY;
            result = 1;
        }
    }
    return result;
}
