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

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
void TerrainProbeBegin(const s8 *p);
void sub_080207a0(void);
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 GetTilePushDown(u16 a);
s32 GetTilePushRight(u16 a);
s32 TerrainQueryPixel(u32 x, u32 y);

void TerrainProbeBegin(const s8 *p)
{
    gTerrainProbeX = (gCurTask->posX >> 16) + p[0];
    gTerrainProbeY = (gCurTask->posY >> 16) + p[1];
    gTerrainBoxTop = p[2];
    gTerrainBoxBottom = p[3];
    gTerrainBoxLeft = p[4];
    gTerrainBoxRight = p[5];
    gTerrainFacing = gCurTask->facing;
    gTerrainProbeResult.unk0 = gTerrainProbeResult.ceilingHits = gTerrainProbeResult.unk2 = gTerrainProbeResult.unk3 = gTerrainProbeResult.slope = gTerrainProbeResult.unk5 = gTerrainProbeResult.onSlipperyFloor = gTerrainProbeResult.unkF = gTerrainProbeResult.unk10 = 0;
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
    gTerrainResult.unk8 = gTerrainProbeResult.unk8;
    gTerrainResult.onSlipperyFloor = gTerrainProbeResult.onSlipperyFloor;
    gTerrainResult.unkC = gTerrainProbeResult.unkF;
    gTerrainResult.unkD = gTerrainProbeResult.unk10;
    gCurTask->unk84 = (gTerrainProbeResult.unkC << 8) | gTerrainProbeResult.unkB;
}
