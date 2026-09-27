#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C8DC-0x0801C930.
 *
 * sub_0801c8dc: count a step when the cell below the actor's box is solid.
 */

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
void TerrainProbeBegin(const s8 *p);
void sub_080207a0(void);
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 GetTilePushDown(u16 a);
s32 GetTilePushRight(u16 a);
s32 TerrainQueryPixel(u32 x, u32 y);

s32 TerrainQueryPixel(u32 x, u32 y);

void sub_0801c8dc(void)
{
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0 && gUnk_087336F0[gTerrainTile] == 0)
        gTerrainProbeResult.unk0++;
}
