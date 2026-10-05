#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* collision_probe_wall_on_ground.c (0x0801C690-0x0801C8DB, issue #84).
 *
 * The wall probes src/collision_collide_box.c's entry points run for a box
 * standing on the ground (gTerrainProbeResult.unk6 != 0): TerrainProbeWallRightOnGround (right
 * edge) and TerrainProbeWallLeftOnGround (left edge) push the probe x out of a wall cell
 * (gCollisionTileShapeClass attribute 1) at the box's top, else its middle corner.
 * 
 * Matching note: cse records `attribute == 1` after the test and then swaps
 * the operands of a later AND with the attribute, so the ROM's source ANDs
 * the constant (`!(gCollisionTileSlope[t] & 1)`, two table reads) or compares a
 * cast (`(s8)a == 1`), and stores the constant `1`/`2` (cse substitutes the
 * attribute register). */

/* Right wall probe of a box standing on the ground (gTerrainProbeResult.unk6 !=
   0), the mirror image of TerrainProbeWallLeftOnGround: step the probe x out of a wall
   cell at the box's top-right, else its middle-right corner. */
void TerrainProbeWallRightOnGround(void)
{
    s32 a;
    s32 d;
    s32 a2;
    s8 b;
    s32 d2;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0)
    {
        if ((a = gCollisionTileShapeClass[gTerrainTile]) == 1 && gCollisionTileOneWay[gTerrainTile] == 0
            && (gCollisionTileShapeClass[gTerrainTileLeft] == 0
                || (gCollisionTileSlope[gTerrainTileLeft] != 0 && !(gCollisionTileSlope[gTerrainTileLeft] & 1))))
        {
            d = GetTilePushLeft(gTerrainTile);
            if (d != 0)
            {
                gTerrainProbeX += d;
                gTerrainProbeResult.unk0 = 1;
                return;
            }
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0)
    {
        if ((a2 = gCollisionTileShapeClass[gTerrainTile]) == 1 && gCollisionTileOneWay[gTerrainTile] == 0)
        {
            b = gCollisionTileShapeClass[gTerrainTileLeft];
            if (b == 0 || (gCollisionTileOneWay[gTerrainTileLeft] != 0 && b == 1))
            {
                d2 = GetTilePushLeft(gTerrainTile);
                if (d2 != 0)
                {
                    gTerrainProbeX += d2;
                    gTerrainProbeResult.unk0 = 1;
                }
            }
        }
    }
}

/* Left wall probe of a box standing on the ground (gTerrainProbeResult.unk6 !=
   0), the mirror image of TerrainProbeWallRightOnGround: step the probe x out of a wall
   cell at the box's top-left, else its middle-left corner. */
void TerrainProbeWallLeftOnGround(void)
{
    s32 a;
    u8 m;
    s32 d;
    s8 a2;
    s8 b;
    s32 d2;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) != 0)
    {
        a = gCollisionTileShapeClass[gTerrainTile];
        if ((s8)a == 1 && gCollisionTileOneWay[gTerrainTile] == 0
            && (gCollisionTileShapeClass[gTerrainTileRight] == 0
                || ((m = gCollisionTileSlope[gTerrainTileRight]) != 0 && (a & m) != 0)))
        {
            d = GetTilePushRight(gTerrainTile);
            if (d != 0)
            {
                gTerrainProbeX += d;
                gTerrainProbeResult.unk0 = 2;
                return;
            }
        }
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0)
    {
        a2 = gCollisionTileShapeClass[gTerrainTile];
        if (a2 == 1 && gCollisionTileOneWay[gTerrainTile] == 0)
        {
            b = gCollisionTileShapeClass[gTerrainTileRight];
            if (b == 0 || (gCollisionTileOneWay[gTerrainTileRight] != 0 && b == 1))
            {
                d2 = GetTilePushRight(gTerrainTile);
                if (d2 != 0)
                {
                    gTerrainProbeX += d2;
                    gTerrainProbeResult.unk0 = 2;
                }
            }
        }
    }
}
