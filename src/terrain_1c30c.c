#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C30C-0x0801C444.
 *
 * Two per-frame entry points that load the actor's terrain box (TerrainProbeBegin),
 * derive the actor's position relative to the room from Task.posX/unk50 and
 * Task.velX/unk58, run the probes and write the results back (TerrainProbeEnd).
 */

void TerrainCollidePointPushOut(const s8 *p)
{

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    if (gTerrainProbeResult.unkB & 0x80)
        TerrainInitOneWayFloor(gTerrainPrevX, gTerrainPrevY);
    TerrainProbePointPushOut();
    TerrainProbeWaterAtPoint();
    TerrainProbeEnd(p);
}

u16 TerrainCollidePointStop(const s8 *p)
{
    u16 r;

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    if (gTerrainProbeResult.unkB & 0x80)
        TerrainInitOneWayFloor(gTerrainPrevX, gTerrainPrevY);
    r = TerrainProbePointStop();
    TerrainProbeWater();
    TerrainProbeEnd(p);
    return r;
}
