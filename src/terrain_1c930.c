#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/* terrain_1c930.c (0x0801C930-0x0801D393, issue #84).
 *
 * The floor probe TerrainCollideBoxInCameraBounds runs for a box standing on the ground
 * (gTerrainProbeResult.unk6 != 0): a wall step in the moving direction first,
 * then the floor under the box (the step and slope attribute tables
 * gCollisionTileStepTile/gUnk_087337F0/gCollisionTileSlippery and gSlopeIndexTiles), the result
 * flags gTerrainProbeResult.unkD/unk5/unkE, and the ledge counter
 * gTerrainProbeResult.unk10 when the probe finds no floor. */

/* Floor probe of a box standing on the ground (gTerrainProbeResult.unk6 != 0),
   TerrainCollideBoxInCameraBounds's counterpart of TerrainProbeFloor: a wall step into the moving
   direction first, the flags gTerrainProbeResult.unkD/unk5/unkE on top, and the
   ledge counter gTerrainProbeResult.unk10 when the probe finds no floor. */
void TerrainProbeFloorInCameraBounds(void)
{
    s32 d;
    s32 hit;
    s32 dir;
    s32 tile;
    s32 tile2;

    gTerrainProbeResult.unk2++;
    gTerrainProbeResult.unkB = 0;
    if (gUnk_02005574[0] == 0 && (gTerrainBoundsClamp & 4)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        if (gTerrainVelX == 0)
            d = (s8)gTerrainFacing;
        else
        {
            d = -1;
            if (gTerrainVelX > 0)
                d = 1;
        }
        if (d == 1)
        {
            if (GetTileShapeAtPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
                goto next;
            gTerrainProbeResult.unk0 = d;
            if (gCollisionTileOneWay[gTerrainTile] != 0)
            {
                tile = gCollisionTileStepTile[gTerrainTile];
                goto right;
            }
            /* The ROM places this arm's `tile = t` and its steps after
               the left arm (the gotos reproduce that layout). */
            goto right_t;
        }
        if (GetTileShapeAtPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.unk0 = 2;
            if (gCollisionTileOneWay[gTerrainTile] != 0)
                tile = gCollisionTileStepTile[gTerrainTile];
            else
                tile = gTerrainTile;
            gTerrainProbeX += GetTilePushRight(tile);
            if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
            {
                if (gCollisionTileOneWay[gTerrainTile] != 0)
                    tile = gCollisionTileStepTile[gTerrainTile];
                else
                    tile = gTerrainTile;
                gTerrainProbeX += GetTilePushRight(tile);
            }
        }
        goto next;
    right_t:
        tile = gTerrainTile;
    right:
        gTerrainProbeX += GetTilePushLeft(tile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            if (gCollisionTileOneWay[gTerrainTile] != 0)
                tile = gCollisionTileStepTile[gTerrainTile];
            else
                tile = gTerrainTile;
            gTerrainProbeX += GetTilePushLeft(tile);
        }
    }
next:
    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (!(gTerrainProbeResult.unkD & 0x10))
    {
        if (gTerrainTile == 48 || gTerrainTile == 49)
            gTerrainProbeResult.unkD = 1;
        if (gTerrainTileBelow == 49)
            gTerrainProbeResult.unkD = 2;
    }
    else
    {
        gTerrainProbeResult.unkD |= 1;
        if (gTerrainTile == 48 || gTerrainTile == 49)
        {
            gTerrainProbeResult.unkD |= 2;
            goto clear;
        }
        gTerrainProbeResult.unkD = 0;
    }
    if (gCollisionTileShapeClass[gTerrainTile] != 0)
    {
        if (gCollisionTileOneWay[gTerrainTile] == 0)
            goto floor;
        gTerrainProbeResult.unkB |= 1;
        if (gCollisionTileSlope[gTerrainTile] != 0)
            goto floor;
        if (gTerrainProbeResult.floorRow <= (gTerrainProbeY + gTerrainBoxBottom) >> 4)
            goto floor;
    }
    if (gSlopeIndexTiles[gTerrainSlopeIndex] != 0)
    {
        hit = 0;
        if (gTerrainSlopeIndex & 1)
        {
            dir = 1;
            if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                hit = 1;
        }
        if (hit != 0 || gTerrainPixelIndex + 16 <= 255)
            goto slope;
    }
    if (gCollisionTileShapeClass[gTerrainTileBelow] != 0)
    {
        if (gCollisionTileOneWay[gTerrainTileBelow] != 0)
            gTerrainProbeResult.unkB |= 1;
    }
    else
    {
        if (gSlopeIndexTiles[gTerrainSlopeIndexBelow] == 0)
            goto edges;
        hit = 0;
        if (gTerrainSlopeIndexBelow & 1)
        {
            dir = 1;
            if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                hit = 1;
        }
        if (hit == 0)
            goto edges;
        goto slope_below;
    }
    TerrainLoadFloorAttributes(gTerrainTileBelow);
    gTerrainProbeY += GetTileFloorSnap(gTerrainTileBelow) + 16;
    goto check;

floor:
    TerrainLoadFloorAttributes(gTerrainTile);
    gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
check:
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || gTerrainProbeResult.floorRow <= (gTerrainProbeY + gTerrainBoxBottom) >> 4))
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        TerrainLoadFloorAttributes(gTerrainTile);
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0)
        hit = (s8)gUnk_087337F0[gTerrainTile];
    else
        hit = (s8)gUnk_087337F0[gTerrainTileBelow];
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0)
        hit &= (s8)gUnk_087337F0[gTerrainTile];
    else
        hit &= (s8)gUnk_087337F0[gTerrainTileBelow];
    gTerrainProbeResult.unk5 = hit;
    gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

slope:
    tile = gSlopeIndexTiles[gTerrainSlopeIndex];
    if (dir == 1)
    {
        if (hit != 0)
        {
            TerrainLoadFloorAttributes(tile);
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        if (hit != 0)
        {
            TerrainLoadFloorAttributes(tile);
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
    goto sides;

slope_below:
    tile = gSlopeIndexTiles[gTerrainSlopeIndexBelow];
    if (dir == 1)
    {
        TerrainLoadFloorAttributes(tile);
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        TerrainLoadFloorAttributes(tile);
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
sides:
    if (GetTileShapeAtPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~1;
    if (GetTileShapeAtPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~2;
    if (dir == 0)
        goto clear;
    if (gCollisionTileOneWay[tile] != 0)
    {
        if (dir == 1)
            gTerrainProbeResult.unkB = 16;
        else
            gTerrainProbeResult.unkB = 32;
    }
    gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

edges:
    dir = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.floorRow))
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        tile = gTerrainTile;
    }
    else
    {
        tile = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[tile] != 0)
        dir += 2;
    hit = (s8)gUnk_087337F0[tile];
    if (gCollisionTileOneWay[tile] != 0)
        gTerrainProbeResult.unkB |= 2;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.floorRow))
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        tile2 = gTerrainTile;
    }
    else
    {
        tile2 = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[tile2] != 0)
        dir++;
    hit &= (s8)gUnk_087337F0[tile2];
    if (gCollisionTileOneWay[tile2] != 0)
        gTerrainProbeResult.unkB |= 4;
    gTerrainProbeResult.unk5 = hit;
    if (dir == 0)
        goto clear;
    if (dir <= 2 && gTerrainVelX != 0)
    {
        if (gTerrainVelX < 0)
        {
            if (dir == 1)
                gTerrainProbeResult.unk3++;
        }
        else if (gTerrainVelX > 0 && dir == 2)
        {
            gTerrainProbeResult.unk3++;
        }
    }
    gTerrainProbeResult.onSlipperyFloor = gCollisionTileSlippery[tile] | gCollisionTileSlippery[tile2];
    gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

clear:
    gTerrainProbeResult.unk5 = 0;
    gTerrainProbeResult.onGround = 0;
    gTerrainProbeResult.unk3 = 0;
    gTerrainProbeResult.unk2 = 0;
    TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (gCollisionTileShapeClass[gTerrainTile] == 0 && gTerrainSlopeIndex == 0 && gTerrainVelX != 0)
    {
        if (gTerrainVelX > 0)
        {
            if (gCollisionTileShapeClass[gTerrainTileRight] == 0)
            {
                tile = GetCollisionTileAtPixel(gTerrainProbeX + 16, gTerrainProbeY + gTerrainBoxBottom + 1);
                if (gCollisionTileShapeClass[tile] == 1)
                    gTerrainProbeResult.overGap++;
            }
        }
        else
        {
            if (gCollisionTileShapeClass[gTerrainTileLeft] == 0)
            {
                tile = GetCollisionTileAtPixel(gTerrainProbeX - 16, gTerrainProbeY + gTerrainBoxBottom + 1);
                if (gCollisionTileShapeClass[tile] == 1)
                    gTerrainProbeResult.overGap++;
            }
        }
    }
}
