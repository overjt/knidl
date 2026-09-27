#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_2136c.c (0x0802136C-0x080214DF, issue #84).
 *
 * The room probe of the non-player entry points (src/terrain_1bcac.c,
 * terrain_1c30c.c): src/terrain_21130.c's sub_08021130 without its tile-set
 * special case in front and its gCollisionTileDoor copy behind. */

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
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
    /*0x10*/ u8 unk10;
};

extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxTop;           /* box top offset */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern s16 gRoomHeight;           /* map height in cells */

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
    prev = gTerrainProbeResult.unk7;
    prev2 = prev;
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.unk7 = 0;
    zero = 0;
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
        f = gTerrainTile & 0x80;
        if (f)
        {
            gTerrainProbeResult.unk7 = 1;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop);
            if (gTerrainTile & 0x80)
            {
                v = gTerrainProbeResult.unk7 | 2 | zero;
                gTerrainProbeResult.unk7 = v;
                if ((prev & 2) == 0)
                {
                    gTerrainProbeResult.unk7 = v | 0x80;
                    gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxTop) & 0xFFF0;
                }
                else
                {
                    gTerrainProbeResult.unk7 = v | 8;
                }
            }
            else
            {
                gTerrainProbeResult.unk7 |= 0x48;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0) + 16;
            }
        }
        else
        {
            gTerrainProbeResult.unk7 = f;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
            if (gTerrainTile & 0x80)
            {
                gTerrainProbeResult.unk7 |= 0x48;
                gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gTerrainProbeResult.unk7 |= 0x80;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) + 16;
            }
        }
    }
}
