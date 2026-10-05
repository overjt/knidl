#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* terrain_1ecd0.c (0x0801ECD0-0x0801F53F, issue #84).
 *
 * The landing probe TerrainCollideBox runs for a box in the air
 * (gTerrainProbeResult.unk6 == 0): land on the floor under the box's centre, else
 * follow the cell's slope link gSlopeIndexTiles[byte 2] and test the floor
 * under the box's left and right bottom corners, keeping the corner/slope
 * flags in gTerrainProbeResult.unkB. */

/* Landing probe TerrainCollideBox runs for a box in the air
   (gTerrainProbeResult.unk6 == 0): snap to the floor under the box's centre;
   failing that, follow the cell's slope link (gSlopeIndexTiles[byte 2]) and
   test the floor under the box's left and right corners, keeping the
   corner/slope flags in gTerrainProbeResult.unkB.  The corner results share their flag updates through
   the labels at the end of each half. */
void TerrainProbeLanding(void)
{
    s32 side;
    s32 a;
    u16 u;
    s32 odd;
    u32 below;
    u32 cell;
    u32 cellBelow;
    u8 b;
    const s8 *p;

    side = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY) != 0)
    {
        if (gCollisionTileOneWay[gTerrainTile] != 0)
        {
            if (gTerrainProbeResult.unkB & 0x30)
            {
                gTerrainProbeResult.unkB = 1;
            }
            else if (!(gTerrainProbeResult.unkB & 1))
            {
                if (gCollisionTileShapeClass[gTerrainTile] != 1)
                    goto walls;
                goto floor;
            }
            else if (gCollisionTileShapeClass[gTerrainTile] == 1
                     && gTerrainProbeResult.floorRow > ((gTerrainProbeY + gTerrainBoxBottom) >> 4))
            {
                goto floor;
            }
        }
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            if (gCollisionTileOneWay[gTerrainTile] == 0 || (s8)(a = gCollisionTileShapeClass[gTerrainTile]) != 1
                || ((b = gTerrainProbeResult.unkB) & 0x30)
                || ((a & b) != 0
                    && ((gTerrainProbeY + gTerrainBoxBottom) >> 4) >= gTerrainProbeResult.floorRow))
            {
                gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
                gTerrainProbeY += GetTilePushUp(gTerrainTile);
            }
        }
        gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        return;
    }
floor:
    if (gCollisionTileOneWay[gTerrainTileBelow] != 0)
    {
        gTerrainProbeResult.unkB |= 1;
        if (gCollisionTileOneWay[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
            gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom) >> 4;
        else
            gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    else
    {
        gTerrainProbeResult.unkB &= 0xFE;
    }
    u = gSlopeIndexTiles[gTerrainSlopeIndex];
    if (u != 0)
    {
        odd = gTerrainSlopeIndex & 1;
        side = 2;
        if (odd)
            side = 1;
        p = gCollisionTileShapes[u];
        if (p[gTerrainPixelIndex] != 0)
        {
            if (odd)
            {
                if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0)
                    && (gCollisionTileOneWay[u] == 0 || (gTerrainProbeResult.unkB & 0x10)))
                    goto snap;
            }
            else if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0)
                     && (gCollisionTileOneWay[u] == 0 || (gTerrainProbeResult.unkB & 0x20)))
            {
            snap:
                gTerrainProbeResult.onGround = 1;
                gTerrainProbeResult.unk2 = 1;
                gTerrainProbeResult.slope = gCollisionTileSlope[u];
                gTerrainProbeY += GetTilePushUp(u);
                gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
                return;
            }
        }
    }
walls:
    u = gTerrainTile;
    below = gTerrainTileBelow;
    cell = gTerrainSlopeIndex;
    cellBelow = gTerrainSlopeIndexBelow;

    /* Left corner. */
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        if (gCollisionTileSlope[gTerrainTile] != 0)
        {
            if (gUnk_08732DF0[gTerrainTile] != 0x93)
                goto slopeL;
            if (((gTerrainProbeY + gTerrainBoxBottom) & 15) > 7)
                goto right;
            goto slopeL;
        }
        if (side == 1)
            goto right;
        if (gCollisionTileCollides[u] != 0 && (gCollisionTileOneWay[u] == 0 || (gTerrainProbeResult.unkB & 1)))
            goto right;
        if (gCollisionTileOneWay[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 0x10))
        {
            if (!(gTerrainProbeResult.unkB & 2))
                goto flatL;
            if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) != (gTerrainPrevBoxLeft & 0xFFF0))
                goto flatL;
            if (((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) <= (gTerrainPrevBoxBottom & 0xFFF0))
                goto flatL;
        }
        gTerrainProbeResult.unkB |= 1;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        goto right;
    }
    if (gCollisionTileSlope[gTerrainTile] == 0)
        goto flatL;
slopeL:
    if (gTerrainVelY >= 0 && gCollisionTileOneWay[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if ((gUnk_08732DF0[gTerrainTile] & 0xF0) != 0x90)
            goto clr10;
        if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83)
        {
            if (gSlopeIndexTiles[cell] == 0)
                goto clr10;
        }
        else if (gSlopeIndexTiles[cellBelow] == 0)
            goto clr10;
        goto set10;
    }
    goto clr12;
flatL:
    if (gTerrainVelY >= 0 && gCollisionTileOneWay[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if (gCollisionTileShapeClass[below] == 0)
        {
            gTerrainProbeResult.unkB |= 2;
            goto right;
        }
        goto clr2;
    }
clr12:
    gTerrainProbeResult.unkB &= 0xED;
    goto right;
clr2:
    gTerrainProbeResult.unkB &= 0xFD;
    goto right;
set10:
    gTerrainProbeResult.unkB |= 0x10;
    goto right;
clr10:
    gTerrainProbeResult.unkB &= 0xEF;
right:
    /* Right corner. */
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        if (gCollisionTileSlope[gTerrainTile] != 0)
        {
            if (gUnk_08732DF0[gTerrainTile] != 0xA3)
                goto slopeR;
            if (((gTerrainProbeY + gTerrainBoxBottom) & 15) > 7)
                return;
            goto slopeR;
        }
        if (side == 2)
            return;
        if (gCollisionTileCollides[u] != 0 && (gCollisionTileOneWay[u] == 0 || (gTerrainProbeResult.unkB & 1)))
            return;
        if (gCollisionTileOneWay[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 0x20))
        {
            if (!(gTerrainProbeResult.unkB & 4))
                goto flatR;
            if (((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0) != (gTerrainPrevBoxRight & 0xFFF0))
                return;
            if (((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) <= (gTerrainPrevBoxBottom & 0xFFF0))
                goto flatR;
        }
        gTerrainProbeResult.unkB |= 1;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        return;
    }
    if (gCollisionTileSlope[gTerrainTile] == 0)
        goto flatR;
slopeR:
    if (gTerrainVelY >= 0 && gCollisionTileOneWay[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if ((gUnk_08732DF0[gTerrainTile] & 0xF0) != 0xA0)
            goto clr20;
        if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83)
        {
            if (gSlopeIndexTiles[cell] == 0)
                goto clr20;
        }
        else if (gSlopeIndexTiles[cellBelow] == 0)
            goto clr20;
        goto set20;
    }
    goto clr24;
flatR:
    if (gTerrainVelY >= 0 && gCollisionTileOneWay[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if (gCollisionTileShapeClass[below] == 0)
        {
            gTerrainProbeResult.unkB |= 4;
            return;
        }
        goto clr4;
    }
clr24:
    gTerrainProbeResult.unkB &= 0xDB;
    return;
clr4:
    gTerrainProbeResult.unkB &= 0xFB;
    return;
set20:
    gTerrainProbeResult.unkB |= 0x20;
    return;
clr20:
    gTerrainProbeResult.unkB &= 0xDF;
}
