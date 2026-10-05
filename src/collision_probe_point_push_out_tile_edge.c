#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* collision_probe_point_push_out_tile_edge.c (0x080207A0-0x0802112F, issue #84).
 *
 * The probes of two more entry points: TerrainProbePointPushOut, the slope-follow probe
 * of src/collision_collide_point.c's TerrainCollidePointPushOut, and TerrainProbeTileEdge, the tile-edge
 * probe of src/collision_collide_box_tile_edge.c's TerrainCollideBoxTileEdge.
 * 
 * Matching note: both are a goto dispatch - the velocity-sign tests first,
 * then the bodies in the order right, left, up, down - which is the only
 * spelling that gives the ROM's layout (the first jump pass moves the first
 * block of each body reached by an unconditional goto into its place). */

/* Slope-follow probe of TerrainCollidePointPushOut (src/collision_collide_point.c): when the pixel
   under the probe point is solid, step the point along its cell -
   horizontally by the sign of Task.velX (gTerrainVelX), else vertically
   by the sign of Task.velY (gTerrainVelY) - up to two cells, mapping a
   passable cell to its step tile (gCollisionTileStepTile) while unkB bit 0 is set;
   otherwise remember in unkB bit 0 whether the cell below is passable. */
void TerrainProbePointPushOut(void)
{
    s32 t;

    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY) == 0)
        goto none;
    if (gTerrainVelX == 0)
        goto vert;
    if (gTerrainVelX > 0)
        goto right;
    goto left;
vert:
    if (gTerrainVelY == 0)
        return;
    if (gTerrainVelY > 0)
        goto down;
    goto up;
none:
    if (gCollisionTileOneWay[gTerrainTileBelow] != 0)
        gTerrainProbeResult.unkB |= 1;
    else
        gTerrainProbeResult.unkB &= 0xFE;
    return;
right:
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 1;
    gTerrainProbeResult.slope = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushLeft(t);
    if (gCollisionTileSlope[gTerrainTile] != 0 && (gCollisionTileSlope[gTerrainTile] & 1))
        return;
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.slope = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushLeft(t);
    return;
left:
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 2;
    gTerrainProbeResult.slope = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushRight(t);
    if (gCollisionTileSlope[gTerrainTile] != 0 && !(gCollisionTileSlope[gTerrainTile] & 1))
        return;
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.slope = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushRight(t);
    return;
up:
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gCollisionTileOneWay[gTerrainTile] != 0)
        return;
    gTerrainProbeResult.ceilingHits = 1;
    gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
    gTerrainProbeY += GetTilePushDown(gTerrainTile);
    if (!(gUnk_08732DF0[gTerrainTile] & 0x40)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
    }
    return;
down:
    if (gCollisionTileOneWay[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1))
    {
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        if (!(gUnk_08732DF0[gTerrainTile] & 0x80)
            && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0
            && (gCollisionTileOneWay[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1)))
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
    }
}

/* The tile-edge probe of TerrainCollideBoxTileEdge (src/collision_collide_box_tile_edge.c): with the box
   moving sideways, a wall probe at the probe point that steps onto
   gCollisionTileStepTile's step tiles and remembers passable wall tiles in
   gTerrainProbeResult.unkB bits 1/2; with it moving vertically, a ceiling or
   floor probe that sets unkB to the side (2 = left, 4 = right) on which
   the tile edge it stopped at continues. */
void TerrainProbeTileEdge(void)
{
    s32 u;

    if (gTerrainVelX == 0)
        goto vert;
    if (gTerrainVelX > 0)
        goto right;
    goto left;
vert:
    if (gTerrainVelY == 0)
        return;
    if (gTerrainVelY > 0)
        goto down;
    goto up;
right:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        gTerrainProbeResult.unkB &= 0xF9;
        if (gCollisionTileOneWay[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
            && (gCollisionTileSlope[gTerrainTile] & 1) != 0)
        {
            gTerrainProbeResult.unkB |= 4;
            return;
        }
        if (gCollisionTileOneWay[gTerrainTileRight] == 0)
            return;
        if (gCollisionTileSlope[gTerrainTileRight] == 0)
            return;
        if ((gCollisionTileSlope[gTerrainTileRight] & 1) == 0)
            return;
        if ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
            return;
        gTerrainProbeResult.unkB |= 4;
        return;
    }
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if ((gTerrainProbeResult.unkB & 4) == 0)
            return;
        u = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        u = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 1;
    gTerrainProbeResult.slope = gCollisionTileSlope[u];
    gTerrainProbeX += GetTilePushLeft(u);
    if ((gCollisionTileSlope[gTerrainTile] == 0 || (gCollisionTileSlope[gTerrainTile] & 1) == 0)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        if (gCollisionTileOneWay[gTerrainTile] != 0)
        {
            if ((gTerrainProbeResult.unkB & 4) == 0)
                return;
            u = gCollisionTileStepTile[gTerrainTile];
        }
        else
        {
            u = gTerrainTile;
        }
        gTerrainProbeResult.slope = gCollisionTileSlope[u];
        gTerrainProbeX += GetTilePushLeft(u);
    }
    gTerrainProbeResult.unkB = 0;
    if (gTerrainProbeResult.slope != 0)
        gTerrainProbeX++;
    return;
left:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        gTerrainProbeResult.unkB &= 0xFD;
        if (gCollisionTileOneWay[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
            && (gCollisionTileSlope[gTerrainTile] & 1) == 0)
        {
            gTerrainProbeResult.unkB |= 2;
            return;
        }
        if (gCollisionTileOneWay[gTerrainTileLeft] == 0)
            return;
        if (gCollisionTileSlope[gTerrainTileLeft] == 0)
            return;
        if ((gCollisionTileSlope[gTerrainTileLeft] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
            return;
        gTerrainProbeResult.unkB |= 2;
        return;
    }
    if (gCollisionTileOneWay[gTerrainTile] != 0)
    {
        if ((gTerrainProbeResult.unkB & 2) == 0)
            return;
        u = gCollisionTileStepTile[gTerrainTile];
    }
    else
    {
        u = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 2;
    gTerrainProbeResult.slope = gCollisionTileSlope[u];
    gTerrainProbeX += GetTilePushRight(u);
    if ((gCollisionTileSlope[gTerrainTile] == 0 || (gCollisionTileSlope[gTerrainTile] & 1) != 0)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        if (gCollisionTileOneWay[gTerrainTile] != 0)
        {
            if ((gTerrainProbeResult.unkB & 2) == 0)
                return;
            u = gCollisionTileStepTile[gTerrainTile];
        }
        else
        {
            u = gTerrainTile;
        }
        gTerrainProbeResult.slope = gCollisionTileSlope[u];
        gTerrainProbeX += GetTilePushRight(u);
    }
    gTerrainProbeResult.unkB = 0;
    if (gTerrainProbeResult.slope != 0)
        gTerrainProbeX--;
    return;
up:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gCollisionTileOneWay[gTerrainTile] != 0)
        return;
    gTerrainProbeResult.ceilingHits = 1;
    gTerrainProbeY += GetTilePushDown(gTerrainTile);
    if ((gUnk_08732DF0[gTerrainTile] & 0x40) == 0
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
    gTerrainProbeResult.unkB = 0;
    gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
    if (gTerrainProbeResult.slope == 0)
        return;
    gTerrainProbeY--;
    if ((gTerrainProbeResult.slope & 1) != 0)
    {
        if (gCollisionTileOneWay[gTerrainTileLeft] == 0)
            return;
        if (gCollisionTileSlope[gTerrainTileLeft] == 0)
            return;
        if ((gCollisionTileSlope[gTerrainTileLeft] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
            return;
        gTerrainProbeResult.unkB = 2;
        return;
    }
    if (gCollisionTileOneWay[gTerrainTileRight] == 0)
        return;
    if (gCollisionTileSlope[gTerrainTileRight] == 0)
        return;
    if ((gCollisionTileSlope[gTerrainTileRight] & 1) == 0)
        return;
    if ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
        return;
    gTerrainProbeResult.unkB = 4;
    return;
down:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    gTerrainProbeResult.unk2 = 1;
    gTerrainProbeY += GetTilePushUp(gTerrainTile);
    if ((gUnk_08732DF0[gTerrainTile] & 0x80) == 0
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
    gTerrainProbeResult.unkB = 0;
    gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
    if (gTerrainProbeResult.slope == 0)
        return;
    gTerrainProbeY++;
    if ((gTerrainProbeResult.slope & 1) != 0)
    {
        if (gCollisionTileOneWay[gTerrainTileLeft] == 0)
            return;
        if (gCollisionTileSlope[gTerrainTileLeft] == 0)
            return;
        if ((gCollisionTileSlope[gTerrainTileLeft] & 1) != 0)
            return;
        if ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 6)
            return;
        gTerrainProbeResult.unkB = 2;
        return;
    }
    if (gCollisionTileOneWay[gTerrainTileRight] == 0)
        return;
    if (gCollisionTileSlope[gTerrainTileRight] == 0)
        return;
    if ((gCollisionTileSlope[gTerrainTileRight] & 1) == 0)
        return;
    if ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
        return;
    gTerrainProbeResult.unkB = 4;
}
