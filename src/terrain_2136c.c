#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/* terrain_2136c.c (0x0802136C-0x080214DF, issue #84).
 *
 * The room probe of the non-player entry points (src/terrain_1bcac.c,
 * terrain_1c30c.c): src/terrain_21130.c's sub_08021130 without its tile-set
 * special case in front and its gCollisionTileDoor copy behind. */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);

/* Room probe: sub_08021130 without its tile-set special case in front
   and the gCollisionTileDoor copy behind.  Classifies the cells at the probe
   point (bit 7 of the tile set) into the flags gTerrainProbeResult.unk7 and the
   cell boundary gTerrainProbeResult.unk8 (0xFFFF when there is none).  prev2 is
   u32 here: the u8 copy of sub_08021130 lets the 0x80 mask register win
   r8 over &gTerrainProbeY (global-alloc priority 0.1333 vs 0.1324). */
void TerrainProbeWater(void)
{
    u8 prev;
    u32 prev2;
    u32 zero;
    s32 y;
    s32 h;
    u16 f;
    u8 v;

    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY);
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
}
