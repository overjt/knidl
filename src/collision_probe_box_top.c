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

void sub_0801c8dc(void)
{
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0 && gCollisionTileOneWay[gTerrainTile] == 0)
        gTerrainProbeResult.unk0++;
}
