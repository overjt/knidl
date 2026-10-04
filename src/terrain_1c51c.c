#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C51C-0x0801C690.
 *
 * TerrainProbeBegin copies the actor's six signed box offsets and the task fields
 * into the room-descriptor cells and clears the probe result block at
 * gTerrainProbeResult; TerrainProbeEnd writes the probe results back into the task
 * (re-seating Task.posX/posY when the probe moved the actor) and mirrors the
 * result block into gTerrainResult.
 */

void TerrainProbeBegin(const s8 *p)
{
    gTerrainProbeX = (gCurTask->posX >> 16) + p[0];
    gTerrainProbeY = (gCurTask->posY >> 16) + p[1];
    gTerrainBoxTop = p[2];
    gTerrainBoxBottom = p[3];
    gTerrainBoxLeft = p[4];
    gTerrainBoxRight = p[5];
    gTerrainFacing = gCurTask->facing;
    gTerrainProbeResult.unk0 = gTerrainProbeResult.ceilingHits = gTerrainProbeResult.unk2 = gTerrainProbeResult.unk3 = gTerrainProbeResult.slope = gTerrainProbeResult.unk5 = gTerrainProbeResult.onSlipperyFloor = gTerrainProbeResult.damage = gTerrainProbeResult.unk10 = 0;
    gTerrainProbeResult.onGround = gCurTask->onGround;
    gTerrainProbeResult.waterFlags = gCurTask->waterFlags;
    gTerrainProbeResult.unkB = gCurTask->unk84;
    gTerrainProbeResult.unkC = gCurTask->unk84 >> 8;
}

void TerrainProbeEnd(const s8 *p)
{
    gCurTask->onGround = gTerrainProbeResult.onGround;
    gCurTask->waterFlags = gTerrainProbeResult.waterFlags;
    if (gCurTask->posX >> 16 != gTerrainProbeX - p[0])
    {
        gCurTask->posX = ((gTerrainProbeX - p[0]) << 16) + 0x8000;
        gCurTask->pixelX = gTerrainProbeX - p[0];
    }
    if (gCurTask->posY >> 16 != gTerrainProbeY - p[1])
    {
        gCurTask->posY = ((gTerrainProbeY - p[1]) << 16) + 0x8000;
        gCurTask->pixelY = gTerrainProbeY - p[1];
    }
    gTerrainResult.unk0 = gTerrainProbeResult.unk0;
    gTerrainResult.ceilingHits = gTerrainProbeResult.ceilingHits;
    gTerrainResult.unk2 = gTerrainProbeResult.unk2;
    gTerrainResult.unk3 = gTerrainProbeResult.unk3;
    gTerrainResult.slope = gTerrainProbeResult.slope;
    gTerrainResult.unk5 = gTerrainProbeResult.unk5;
    gTerrainResult.waterSurfaceY = gTerrainProbeResult.waterSurfaceY;
    gTerrainResult.onSlipperyFloor = gTerrainProbeResult.onSlipperyFloor;
    gTerrainResult.damage = gTerrainProbeResult.damage;
    gTerrainResult.unkD = gTerrainProbeResult.unk10;
    gCurTask->unk84 = (gTerrainProbeResult.unkC << 8) | gTerrainProbeResult.unkB;
}
