#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* terrain_1d394.c (0x0801D394-0x0801D9C7, issue #84).
 *
 * The floor probe sub_0801bcac runs for a box standing on the ground
 * (gTerrainProbeResult.unk6 != 0), the simpler sibling of sub_0801c930 in
 * src/terrain_1c930.c. */

/* Floor probe of a box standing on the ground (gTerrainProbeResult.unk6 != 0). */
void sub_0801d394(void)
{
    s32 tile;
    s32 tile2;
    s32 hit;
    s32 dir;

    gTerrainProbeResult.unk2++;
    gTerrainProbeResult.unkB = 0;
    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (gCollisionTileShapeClass[gTerrainTile] != 0)
    {
        if (gUnk_087336F0[gTerrainTile] == 0)
            goto floor;
        gTerrainProbeResult.unkB |= 1;
        if (gCollisionTileSlope[gTerrainTile] != 0)
            goto floor;
        if (gTerrainProbeResult.unkC <= (gTerrainProbeY + gTerrainBoxBottom) >> 4)
            goto floor;
    }
    if (gUnk_08735018[gUnk_03005574] != 0)
    {
        hit = 0;
        if (gUnk_03005574 & 1)
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
        if (gUnk_087336F0[gTerrainTileBelow] != 0)
            gTerrainProbeResult.unkB |= 1;
    }
    else
    {
        if (gUnk_08735018[gUnk_030055AC] == 0)
            goto edges;
        hit = 0;
        if (gUnk_030055AC & 1)
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
    gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTileBelow];
    gTerrainProbeY += GetTileFloorSnap(gTerrainTileBelow) + 16;
    goto check;

floor:
    gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
    gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
check:
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || gTerrainProbeResult.unkC <= (gTerrainProbeY + gTerrainBoxBottom) >> 4))
    {
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
    }
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

slope:
    tile = gUnk_08735018[gUnk_03005574];
    if (dir == 1)
    {
        if (hit != 0)
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[tile];
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        if (hit != 0)
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[tile];
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
    goto sides;

slope_below:
    tile = gUnk_08735018[gUnk_030055AC];
    if (dir == 1)
    {
        gTerrainProbeResult.slope = gCollisionTileSlope[tile];
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        gTerrainProbeResult.slope = gCollisionTileSlope[tile];
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
sides:
    if (sub_08021ab4(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~1;
    if (sub_08021ab4(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~2;
    if (dir == 0)
        goto clear;
    if (gUnk_087336F0[tile] != 0)
    {
        if (dir == 1)
            gTerrainProbeResult.unkB = 16;
        else
            gTerrainProbeResult.unkB = 32;
    }
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

edges:
    dir = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.unkC))
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
    if (gUnk_087336F0[tile] != 0)
        gTerrainProbeResult.unkB |= 2;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.unkC))
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
    if (gUnk_087336F0[tile2] != 0)
        gTerrainProbeResult.unkB |= 4;
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
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

clear:
    gTerrainProbeResult.unk5 = 0;
    gTerrainProbeResult.onGround = 0;
    gTerrainProbeResult.unk3 = 0;
    gTerrainProbeResult.unk2 = 0;
}
