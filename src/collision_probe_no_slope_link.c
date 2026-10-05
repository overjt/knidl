#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* collision_probe_no_slope_link.c (0x0801F540-0x0801FF83, issue #84).
 *
 * The probe sets of three more entry points in src/collision_collide_box.c:
 * TerrainCollideBoxWalls's right/left wall probes sub_0801f540/sub_0801f6b0 (the
 * box's top corner through a gUnk_08732DF0 wall class, then the middle and
 * bottom corners), TerrainCollideBoxCeilingAndFloor's ceiling probe TerrainProbeCeilingNoSlopeLink (TerrainProbeCeiling
 * without the slope tiles) and landing probe sub_0801f9b8 (not while moving
 * up; it tracks passable floor tiles in gTerrainProbeResult.unkB bits 0-2), and
 * TerrainCollideBoxFloor's pair TerrainProbeFloorNoSlopeLink (on the ground: follow the floor or
 * drop off it) / TerrainProbeLandingNoSlopeLink (in the air: land). */

/* Right wall probe of the third entry point (TerrainCollideBoxWalls). */
void sub_0801f540(void)
{
    s32 d;
    s32 x;
    s32 a;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0
            || (gCollisionTileSlope[gTerrainTile] != 0 && (gUnk_08732DF0[gTerrainTile] & 0xF0) == 0xA0)))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
            return;
        }
    }
    x = gTerrainProbeX + gTerrainBoxRight;
    if ((x & 0xFFF0) == (gTerrainPrevBoxRight & 0xFFF0))
        return;
    if (TerrainQueryPixelAndSides(x, gTerrainProbeY + gTerrainBoxTop) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && gCollisionTileOneWay[gTerrainTile] != 0)
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = a;
            return;
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && gCollisionTileOneWay[gTerrainTile] != 0)
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = a;
        }
    }
}

/* Left wall probe of the third entry point, the mirror image of
   sub_0801f540. */
void sub_0801f6b0(void)
{
    s32 d;
    s32 x;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0
            || (gCollisionTileSlope[gTerrainTile] != 0 && (gUnk_08732DF0[gTerrainTile] & 0xF0) == 0x90)))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
            return;
        }
    }
    x = gTerrainProbeX + gTerrainBoxLeft;
    if ((x & 0xFFF0) == (gTerrainPrevBoxRight & 0xFFF0))
        return;
    if (TerrainQueryPixelAndSides(x, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && gCollisionTileOneWay[gTerrainTile] != 0)
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
            return;
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && gCollisionTileOneWay[gTerrainTile] != 0)
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
        }
    }
}

/* Ceiling probe of the fourth entry point (TerrainCollideBoxCeilingAndFloor): TerrainProbeCeiling
   without the slope-tile case. */
void TerrainProbeCeilingNoSlopeLink(void)
{
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileOneWay[gTerrainTile] == 0)
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
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (gCollisionTileCollides[gTerrainTileRight] == 0 || gCollisionTileOneWay[gTerrainTileRight] != 0))
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.ceilingHits++;
        return;
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gCollisionTileOneWay[gTerrainTile] == 0
        && (gCollisionTileCollides[gTerrainTileLeft] == 0 || gCollisionTileOneWay[gTerrainTileLeft] != 0))
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.ceilingHits++;
    }
}

/* Floor probe (none while gTerrainVelY, Task.velY, is negative): land
   the box's bottom on the floor cell under it, else on a slope under one
   of its bottom corners.  Bits 0-2 of gTerrainProbeResult.unkB record, for the
   centre and the two corners, whether the cell below is a
   gCollisionTileOneWay tile, and let such a cell be landed on next time. */
void sub_0801f9b8(void)
{
    s32 a;
    s32 a2;

    if (gTerrainVelY < 0)
    {
        gTerrainProbeResult.unkB = 0;
        return;
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY) != 0
        && (gCollisionTileOneWay[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1)))
    {
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
        return;
    }
    if (gCollisionTileOneWay[gTerrainTileBelow] != 0)
        gTerrainProbeResult.unkB |= 1;
    else
        gTerrainProbeResult.unkB &= ~1;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        if (gCollisionTileOneWay[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 2))
        {
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
            gTerrainProbeResult.onGround = 1;
            gTerrainProbeResult.unk2 = 1;
        }
    }
    else
    {
        if (gCollisionTileOneWay[gTerrainTileBelow] != 0 && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
            gTerrainProbeResult.unkB |= 2;
        else
            gTerrainProbeResult.unkB &= ~2;
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        if (gCollisionTileOneWay[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 4))
            return;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
    }
    if (gCollisionTileOneWay[gTerrainTileBelow] != 0 && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
        gTerrainProbeResult.unkB |= 4;
    else
        gTerrainProbeResult.unkB &= ~4;
}

/* Floor probe of the fifth entry point (TerrainCollideBoxFloor) for a box standing on
   the ground: keep it on the floor cell under it (or the cell below), else
   count the floor cells under its two bottom corners and clear the
   on-ground flags when there are none (the box starts to fall). */
void TerrainProbeFloorNoSlopeLink(void)
{
    s32 n;
    u16 t;

    gTerrainProbeResult.unk2++;
    gTerrainProbeResult.unkB = 0;
    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (gCollisionTileShapeClass[gTerrainTile] == 0)
    {
        if (gCollisionTileShapeClass[gTerrainTileBelow] == 0)
            goto count;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTileBelow];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTileBelow) + 16;
    }
    else
    {
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
    }
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
    }
    return;
count:
    n = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gCollisionTileSlope[gTerrainTile] == 0)
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        t = gTerrainTile;
    }
    else
    {
        t = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[t] != 0)
        n += 2;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gCollisionTileSlope[gTerrainTile] == 0)
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        t = gTerrainTile;
    }
    else
    {
        t = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[t] != 0)
        n += 1;
    if (n == 0)
        gTerrainProbeResult.unk2 = gTerrainProbeResult.unk3 = gTerrainProbeResult.onGround = gTerrainProbeResult.unk5 = 0;
}

/* Landing probe of the fifth entry point (TerrainCollideBoxFloor) for a box in the
   air: land the box's bottom on the floor cell under it, else a bottom
   corner on a floor cell, and set the on-ground flags. */
void TerrainProbeLandingNoSlopeLink(void)
{
    s32 a;
    s32 a2;

    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY) != 0)
    {
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
        return;
    }
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    }
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
    }
}
