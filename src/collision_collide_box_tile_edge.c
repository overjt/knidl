#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C444-0x0801C51C.
 *
 * TerrainCollideBoxTileEdge: the same entry-point body as src/collision_collide_box.c, clearing
 * the pending-collision flag and running the tile-edge probe TerrainProbeTileEdge.
 */

void TerrainClampBoxToCameraBounds(void);

void TerrainCollideBoxTileEdge(const s8 *p)
{
    TerrainProbeBegin(p);
    if (gTerrainProbeResult.unkB & 0x80)
        gTerrainProbeResult.unkB = 0;
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    TerrainProbeTileEdge();
    TerrainProbeEnd(p);
}
