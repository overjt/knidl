#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x08021130-0x0802136C.
 *
 * The room probe for the player: sub_08021130 is TerrainProbeWater (still in asm)
 * with a tile-set special case in front (tile sets 64..79 and 192..207 pick a
 * pair of signed offsets from the gCurTileDrifts table into gTerrainDriftX /
 * gTerrainDriftY) and the gCollisionTileDoor attribute copied to unkA behind.
 */

void sub_08021130(void)
{
    u8 prev;
    u8 prev2;
    u32 zero;
    s32 y;
    s32 h;
    u16 f;
    u8 v;
    u16 tile;
    u16 t;

    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY);
    tile = gTerrainTile;
    t = tile - 64;
    if (t <= 15)
    {
        gTerrainDriftX = gCurTileDrifts[(tile - 48) * 2];
        gTerrainDriftY = gCurTileDrifts[(gTerrainTile - 48) * 2 + 1];
    }
    else
    {
        t = tile - 192;
        if (t <= 15)
        {
            gTerrainDriftX = gCurTileDrifts[(tile - 192) * 2];
            gTerrainDriftY = gCurTileDrifts[(gTerrainTile - 192) * 2 + 1];
        }
    }
    prev = gTerrainProbeResult.waterFlags;
    prev2 = prev;
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.waterFlags = 0;
    zero = 0;
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
        f = gTerrainTile & 0x80;
        if (f)
        {
            gTerrainProbeResult.waterFlags = 1;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop);
            if (gTerrainTile & 0x80)
            {
                v = gTerrainProbeResult.waterFlags | 2 | zero;
                gTerrainProbeResult.waterFlags = v;
                if ((prev & 2) == 0)
                {
                    gTerrainProbeResult.waterFlags = v | 0x80;
                    gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxTop) & 0xFFF0;
                }
                else
                {
                    gTerrainProbeResult.waterFlags = v | 8;
                }
            }
            else
            {
                gTerrainProbeResult.waterFlags |= 0x48;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0) + 16;
            }
        }
        else
        {
            gTerrainProbeResult.waterFlags = f;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
            if (gTerrainTile & 0x80)
            {
                gTerrainProbeResult.waterFlags |= 0x48;
                gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gTerrainProbeResult.waterFlags |= 0x80;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) + 16;
            }
        }
    }
    gTerrainProbeResult.atDoor = 0;
    if (gCollisionTileDoor[tile] != 0)
        gTerrainProbeResult.atDoor = 1;
}
