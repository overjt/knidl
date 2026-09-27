#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"

/* terrain_1ff84.c (0x0801FF84-0x0802069B, issue #84).
 *
 * The probe set of the sixth entry point (src/terrain_1bcac.c's
 * sub_0801c230): a wall probe in the moving direction that steps the box up
 * gUnk_087338F0's step tiles and remembers passable wall tiles in
 * gTerrainProbeResult.unkB bits 1/2, a ceiling and a floor probe while the box
 * moves vertically, and the on-floor bookkeeping (unkB bit 0, the floor row
 * unkC); plus sub_08020698, an empty dead export.
 * 
 * Matching note: a tile attribute read for a test (`gCollisionTileSlope[t] != 0 &&
 * (gCollisionTileSlope[t] & 1)`) is written as two table reads, not a local: the
 * local's AND is tied in place by regmove, the cse'd read is not. */

/* The probe set of the sixth entry point (sub_0801c230): a wall probe in
   the moving direction that steps the box onto gUnk_087338F0's step tiles
   and remembers passable wall tiles in gTerrainProbeResult.unkB bits 1 and 2,
   then a ceiling probe and a floor probe while the box moves vertically,
   and the on-floor bookkeeping (unkB bit 0, the floor row unkC). */
void sub_0801ff84(void)
{
    u8 b;
    u16 t;
    s32 u;

    if (gTerrainVelX != 0)
    {
        if (gTerrainVelX > 0)
        {
            if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) == 0)
            {
                gTerrainProbeResult.unkB &= 0xFB;
                if (gUnk_087336F0[gTerrainTile] != 0
                    && gCollisionTileSlope[gTerrainTile] != 0 && (gCollisionTileSlope[gTerrainTile] & 1) != 0)
                {
                    gTerrainProbeResult.unkB |= 4;
                    goto vertical;
                }
                if (gUnk_087336F0[gTerrainTileRight] == 0)
                    goto vertical;
                if (gCollisionTileSlope[gTerrainTileRight] == 0)
                    goto vertical;
                if ((gCollisionTileSlope[gTerrainTileRight] & 1) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                    goto vertical;
                gTerrainProbeResult.unkB |= 4;
                goto vertical;
            }
            if (gUnk_087339F0[gTerrainTile] == 1 && (gTerrainPrevY & 0xFFF0) != (gTerrainProbeY & 0xFFF0))
                goto vertical;
            if (gUnk_087336F0[gTerrainTile] != 0)
            {
                if (gCollisionTileSlope[gTerrainTile] == 0)
                    goto vertical;
                if ((gTerrainProbeResult.unkB & 4) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                    goto vertical;
                u = gUnk_087338F0[gTerrainTile];
            }
            else
            {
                u = gTerrainTile;
            }
            gTerrainProbeResult.unk0 = 1;
            gTerrainProbeX += GetTilePushLeft(u);
            if ((gCollisionTileSlope[gTerrainTile] == 0 || (gCollisionTileSlope[gTerrainTile] & 1) == 0)
                && TerrainQueryPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0)
            {
                    if (gUnk_087336F0[gTerrainTile] != 0)
                {
                    if (gCollisionTileSlope[gTerrainTile] == 0)
                        goto vertical;
                    if ((gTerrainProbeResult.unkB & 4) == 0)
                        goto vertical;
                    if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                        goto vertical;
                    u = gUnk_087338F0[gTerrainTile];
                }
                else
                {
                    u = gTerrainTile;
                }
                gTerrainProbeX += GetTilePushLeft(u);
            }
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        }
        else
        {
            if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) == 0)
            {
                gTerrainProbeResult.unkB &= 0xFD;
                if (gUnk_087336F0[gTerrainTile] != 0
                    && gCollisionTileSlope[gTerrainTile] != 0 && (gCollisionTileSlope[gTerrainTile] & 1) == 0)
                {
                    gTerrainProbeResult.unkB |= 2;
                    goto vertical;
                }
                if (gUnk_087336F0[gTerrainTileLeft] == 0)
                    goto vertical;
                if (gCollisionTileSlope[gTerrainTileLeft] == 0)
                    goto vertical;
                if ((gCollisionTileSlope[gTerrainTileLeft] & 1) != 0)
                    goto vertical;
                if ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                    goto vertical;
                gTerrainProbeResult.unkB |= 2;
                goto vertical;
            }
            if (gUnk_087339F0[gTerrainTile] == 1 && (gTerrainPrevY & 0xFFF0) != (gTerrainProbeY & 0xFFF0))
                goto vertical;
            if (gUnk_087336F0[gTerrainTile] != 0)
            {
                if (gCollisionTileSlope[gTerrainTile] == 0)
                    goto vertical;
                if ((gTerrainProbeResult.unkB & 2) == 0)
                    goto vertical;
                if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                    goto vertical;
                u = gUnk_087338F0[gTerrainTile];
            }
            else
            {
                u = gTerrainTile;
            }
            gTerrainProbeResult.unk0 = 2;
            gTerrainProbeX += GetTilePushRight(u);
            if ((gCollisionTileSlope[gTerrainTile] != 0 || (gCollisionTileSlope[gTerrainTile] & 1) == 0)
                && TerrainQueryPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) != 0)
            {
                    if (gUnk_087336F0[gTerrainTile] != 0)
                {
                    if (gCollisionTileSlope[gTerrainTile] == 0)
                        goto vertical;
                    if ((gTerrainProbeResult.unkB & 2) == 0)
                        goto vertical;
                    if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
                        goto vertical;
                    u = gUnk_087338F0[gTerrainTile];
                }
                else
                {
                    u = gTerrainTile;
                }
                gTerrainProbeX += GetTilePushRight(u);
            }
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        }
    }
vertical:
    if (gTerrainVelY != 0)
    {
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0
            && gUnk_087336F0[gTerrainTile] == 0)
        {
            gTerrainProbeResult.ceilingHits = 1;
            gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushDown(gTerrainTile);
            if ((gUnk_08732DF0[gTerrainTile] & 0x40) == 0
                && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0)
            {
                gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
                gTerrainProbeY += GetTilePushDown(gTerrainTile);
            }
            gTerrainProbeResult.unkB = 0;
        }
        if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY) == 0)
            goto floor;
        if (gUnk_087336F0[gTerrainTile] != 0)
        {
            if ((gTerrainProbeResult.unkB & 1) == 0)
                goto off;
            if (gTerrainProbeResult.unkC > (gTerrainProbeY + gTerrainBoxBottom) >> 4)
                goto off;
        }
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if ((gUnk_08732DF0[gTerrainTile] & 0x80) == 0
            && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            t = gTerrainTile;
            if (gUnk_087336F0[t] != 0)
            {
                if ((gTerrainProbeResult.unkB & 1) == 0)
                    goto done;
                if ((gTerrainProbeY + gTerrainBoxBottom) >> 4 < gTerrainProbeResult.unkC)
                    goto done;
            }
            gTerrainProbeResult.slope = gCollisionTileSlope[t];
            gTerrainProbeY += GetTilePushUp(t);
        }
    done:
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        return;
    }
floor:
    if (gUnk_087336F0[gTerrainTileBelow] != 0)
    {
        gTerrainProbeResult.unkB |= 1;
        if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
            gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom) >> 4;
        else
            gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
        return;
    }
off:
    gTerrainProbeResult.unkB &= 0xFE;
}

/* An empty function: nothing in the ROM calls it or points at it. */
void sub_08020698(void)
{
}
