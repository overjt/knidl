#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* collision_probe_wall_in_air_ceiling.c (0x0801D9C8-0x0801E177, issue #84).
 *
 * The probes src/collision_collide_box.c's entry points run for a box in the air
 * (gTerrainProbeResult.unk6 == 0) before its landing probe: the right and left wall probes TerrainProbeWallRightInAir /
 * TerrainProbeWallLeftInAir (top, middle and, in the moving direction, bottom corner of
 * the box edge) and the ceiling probe TerrainProbeCeiling.
 * 
 * Matching notes: a tile attribute the ROM tests with `cmp #1` and then ANDs
 * with a flag is `(flags & 1)`, the constant: cse knows the attribute
 * register holds 1 and substitutes it for the constant, which keeps the AND
 * in place on that register; writing `a & flags` lets cse swap the operands
 * instead.  The ceiling probe's two side tests each end with their own
 * `gTerrainProbeY += ...; gTerrainProbeResult.unk1++;` (lesson 3.430). */

/* Right wall probe of a box in the air (gTerrainProbeResult.unk6 == 0), the
   mirror image of TerrainProbeWallLeftInAir: the wall cell's attribute (1) goes to
   gTerrainProbeResult.unk0. */
void TerrainProbeWallRightInAir(void)
{
    s32 d;
    s32 a;
    s32 a2;
    s32 a3;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gCollisionTileCollides[gTerrainTileLeft] == 0 || gCollisionTileOneWay[gTerrainTileLeft] != 0))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0 && (gTerrainPrevY & 0xFFF0) == (gTerrainProbeY & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
            return;
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gCollisionTileCollides[gTerrainTileLeft] == 0 || gCollisionTileOneWay[gTerrainTileLeft] != 0))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0
            && (gTerrainPrevBoxTop & 0xFFF0) <= ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0)
            && ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0) > (gTerrainPrevBoxRight & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
            return;
        }
    }
    if (gTerrainVelX > 0
        && TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (a3 = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gCollisionTileCollides[gTerrainTileLeft] == 0
            || (gCollisionTileOneWay[gTerrainTileLeft] != 0 && (gTerrainProbeResult.unkB & 1) == 0)))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0
            && ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0) > (gTerrainPrevBoxRight & 0xFFF0)
            && (gTerrainPrevBoxBottom & 0xFFF0) >= ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
        }
    }
}

/* Left wall probe of a box in the air (gTerrainProbeResult.unk6 == 0): the top,
   middle and (when moving left) bottom of the box's left edge, each pushing
   the probe x out of a wall cell and setting gTerrainProbeResult.unk0 = 2. */
void TerrainProbeWallLeftInAir(void)
{
    s32 d;
    s32 a;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && (gCollisionTileCollides[gTerrainTileRight] == 0 || gCollisionTileOneWay[gTerrainTileRight] != 0))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0 && (gTerrainPrevY & 0xFFF0) == (gTerrainProbeY & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
            return;
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && (gCollisionTileCollides[gTerrainTileRight] == 0 || gCollisionTileOneWay[gTerrainTileRight] != 0))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0
            && (gTerrainPrevBoxTop & 0xFFF0) <= ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0)
            && ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainPrevBoxLeft & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
            return;
        }
    }
    if (gTerrainVelX < 0
        && TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gCollisionTileCollides[gTerrainTileRight] == 0
            || (gCollisionTileOneWay[gTerrainTileRight] != 0 && (gTerrainProbeResult.unkB & 1) == 0)))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0
            && ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainPrevBoxLeft & 0xFFF0)
            && (gTerrainPrevBoxBottom & 0xFFF0) >= ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
        }
    }
}

/* Ceiling probe of a box in the air (gTerrainProbeResult.unk6 == 0): when the
   box's top hits a solid cell (or, moving up, the slope tile gUnk_08735098
   maps the cell's byte 2 to), push the probe y down out of it and count
   the hit in gTerrainProbeResult.unk1, else test the box's two top corners
   against a ceiling edge. */
void TerrainProbeCeiling(void)
{
    s32 slope;
    u16 *p;
    u16 *tbl;
    u16 t;
    s8 *q;

    slope = 0;
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) == 0)
    {
        tbl = gUnk_08735098;
        p = &tbl[gTerrainSlopeIndex];
        if (*p == 0)
            goto side;
        slope = (gTerrainSlopeIndex & 1) ? 1 : 2;
        t = *p;
        q = gCollisionTileShapes[t];
        if (q[gTerrainPixelIndex] == 0)
            goto side;
        if (gTerrainVelY >= 0)
            return;
        if (slope == 1)
        {
            if ((gTerrainProbeX & 0xFFF0) == ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0))
                return;
        }
        else if (slope == 2)
        {
            if ((gTerrainProbeX & 0xFFF0) == ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                return;
        }
        gTerrainProbeResult.ceilingHits++;
        gTerrainProbeResult.slope = gCollisionTileSlope[t];
        gTerrainProbeY += GetTilePushDown(t);
        return;
    }
    if (gCollisionTileOneWay[gTerrainTile] == 0)
    {
        gTerrainProbeResult.ceilingHits++;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0)
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushDown(gTerrainTile);
        }
        return;
    }
side:
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (gCollisionTileCollides[gTerrainTileRight] == 0 || gCollisionTileOneWay[gTerrainTileRight] != 0)
        && slope == 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.ceilingHits++;
        return;
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (gCollisionTileCollides[gTerrainTileLeft] == 0 || gCollisionTileOneWay[gTerrainTileLeft] != 0)
        && slope == 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.ceilingHits++;
    }
}
