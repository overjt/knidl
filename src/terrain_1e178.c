#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/* terrain_1e178.c (0x0801E178-0x0801ECCF, issue #84).
 *
 * The landing probe sub_0801bde0 runs for a box in the air
 * (gTerrainProbeResult.unk6 == 0), the twin of sub_0801ecd0 in src/terrain_1ecd0.c
 * with a wall step at the box's bottom corner in front (the head
 * src/terrain_1c930.c's sub_0801c930 has too).  Its shared epilogue at
 * 0x0801ECBA is reached by a long `bl` from 0x0801E470 as well as by `b.n`s
 * (a far branch inside the function, lesson 4.39). */

/* Landing probe of a box in the air (sub_0801bde0, on-ground flag
   gTerrainProbeResult.unk6 == 0), the twin of sub_0801ecd0: first step the box
   out of a wall it moves into at its bottom corner (the same head as
   sub_0801c930), then the flag gTerrainProbeResult.unkD, then snap the box to
   the floor under its centre; failing that, follow the cell's slope link
   (gUnk_08735018[byte 2]) and test the floor under the box's left and
   right corners, keeping the corner/slope flags in gTerrainProbeResult.unkB. */
void sub_0801e178(void)
{
    s32 side;
    s32 a;
    s32 u;
    s32 odd;
    u32 below;
    u32 cell;
    u32 cellBelow;
    u8 b;
    const s8 *p;
    s32 dir;
    s32 r;

    if (gUnk_02005574[0] == 0 && (gTerrainBoundsClamp & 4)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0
            || ((gTerrainProbeResult.unkB & 1)
                && gTerrainProbeResult.unkC <= ((gTerrainProbeY + gTerrainBoxBottom) >> 4))))
    {
        if (gTerrainVelX == 0)
            dir = (s8)gTerrainFacing;   /* canon.h: u8; the ROM loads it with ldrsb */
        else
        {
            dir = -1;
            if (gTerrainVelX > 0)
                dir = 1;
        }
        if (dir == 1)
        {
            if (sub_08021ab4(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
                goto done;
            gTerrainProbeResult.unk0 = 1;
            /* The ROM places this arm's `u = t` and its steps after the
               left arm (the gotos reproduce that layout, as in
               sub_0801c930). */
            if (gUnk_087336F0[gTerrainTile] == 0)
                goto rflat;
            u = gUnk_087338F0[gTerrainTile];
            goto rmove;
        }
        if (sub_08021ab4(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.unk0 = 2;
            if (gUnk_087336F0[gTerrainTile] != 0)
                u = gUnk_087338F0[gTerrainTile];
            else
                u = gTerrainTile;
            gTerrainProbeX += GetTilePushRight(u);
            if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
            {
                if (gUnk_087336F0[gTerrainTile] != 0)
                    u = gUnk_087338F0[gTerrainTile];
                else
                    u = gTerrainTile;
                gTerrainProbeX += GetTilePushRight(u);
            }
        }
        goto done;
    rflat:
        u = gTerrainTile;
    rmove:
        gTerrainProbeX += GetTilePushLeft(u);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            if (gUnk_087336F0[gTerrainTile] != 0)
                u = gUnk_087338F0[gTerrainTile];
            else
                u = gTerrainTile;
            gTerrainProbeX += GetTilePushLeft(u);
        }
    }
done:
    side = 0;
    r = TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY);
    if (!(gTerrainProbeResult.unkD & 0x10))
    {
        if ((u16)(gTerrainTile - 48) <= 1)
            gTerrainProbeResult.unkD = 3;
        else
            gTerrainProbeResult.unkD = 0;
        /* The no-floor test is written in both arms: the ROM loads the
           floor code's addresses at the end of each arm (3.430). */
        if (r == 0)
            goto floor;
    }
    else
    {
        gTerrainProbeResult.unkB = 1;
        if ((u16)(gTerrainTile - 48) <= 1)
        {
            gTerrainProbeResult.unkD = 3;
            return;
        }
        gTerrainProbeResult.unkD = 0;
        if (r == 0)
            goto floor;
    }
    {
        if (gUnk_087336F0[gTerrainTile] != 0)
        {
            if (gTerrainProbeResult.unkB & 0x30)
            {
                gTerrainProbeResult.unkB = 1;
                if ((gTerrainProbeY & 0xFFF0) < ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0))
                    goto snap0;
            }
            if (!(gTerrainProbeResult.unkB & 1))
            {
                if (gCollisionTileShapeClass[gTerrainTile] != 1)
                    goto walls;
                goto floor;
            }
            if (gCollisionTileShapeClass[gTerrainTile] == 1
                && gTerrainProbeResult.unkC > ((gTerrainProbeY + gTerrainBoxBottom) >> 4))
                goto floor;
        }
    snap0:
        gTerrainProbeResult.onGround = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            if (gUnk_087336F0[gTerrainTile] == 0 || (s8)(a = gCollisionTileShapeClass[gTerrainTile]) != 1
                || ((b = gTerrainProbeResult.unkB) & 0x30)
                || ((a & b) != 0
                    && ((gTerrainProbeY + gTerrainBoxBottom) >> 4) >= gTerrainProbeResult.unkC))
            {
                gTerrainProbeResult.slope = gCollisionTileSlope[gTerrainTile];
                gTerrainProbeY += GetTilePushUp(gTerrainTile);
            }
        }
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
    }
    else
    {
        gTerrainProbeResult.unkB &= 0xFE;
    }
    u = gUnk_08735018[gUnk_03005574];
    if (u != 0)
    {
        odd = gUnk_03005574 & 1;
        side = 2;
        if (odd)
            side = 1;
        p = gCollisionTileShapes[u];
        if (p[gTerrainPixelIndex] != 0)
        {
            if (odd)
            {
                if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0)
                    && (gUnk_087336F0[u] == 0 || (gTerrainProbeResult.unkB & 0x10)))
                    goto snap;
            }
            else if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0)
                     && (gUnk_087336F0[u] == 0 || (gTerrainProbeResult.unkB & 0x20)))
            {
            snap:
                gTerrainProbeResult.onGround = 1;
                gTerrainProbeResult.unk2 = 1;
                gTerrainProbeResult.slope = gCollisionTileSlope[u];
                gTerrainProbeY += GetTilePushUp(u);
                gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
                return;
            }
        }
    }
walls:
    u = gTerrainTile;
    below = gTerrainTileBelow;
    cell = gUnk_03005574;
    cellBelow = gUnk_030055AC;

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
        if (gUnk_087339F0[u] != 0 && (gUnk_087336F0[u] == 0 || (gTerrainProbeResult.unkB & 1)))
            goto right;
        if (gUnk_087336F0[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 0x10))
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
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        goto right;
    }
    if (gCollisionTileSlope[gTerrainTile] == 0)
        goto flatL;
slopeL:
    if (gTerrainVelY >= 0 && gUnk_087336F0[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if ((gUnk_08732DF0[gTerrainTile] & 0xF0) != 0x90)
            goto clr10;
        if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83)
        {
            if (gUnk_08735018[cell] == 0)
                goto clr10;
        }
        else if (gUnk_08735018[cellBelow] == 0)
            goto clr10;
        goto set10;
    }
    goto clr12;
flatL:
    if (gTerrainVelY >= 0 && gUnk_087336F0[gTerrainTileBelow] != 0
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
        if (gUnk_087339F0[u] != 0 && (gUnk_087336F0[u] == 0 || (gTerrainProbeResult.unkB & 1)))
            return;
        if (gUnk_087336F0[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 0x20))
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
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
        return;
    }
    if (gCollisionTileSlope[gTerrainTile] == 0)
        goto flatR;
slopeR:
    if (gTerrainVelY >= 0 && gUnk_087336F0[gTerrainTileBelow] != 0
        && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
    {
        if ((gUnk_08732DF0[gTerrainTile] & 0xF0) != 0xA0)
            goto clr20;
        if ((gUnk_08732DF0[gTerrainTile] & 0xCF) == 0x83)
        {
            if (gUnk_08735018[cell] == 0)
                goto clr20;
        }
        else if (gUnk_08735018[cellBelow] == 0)
            goto clr20;
        goto set20;
    }
    goto clr24;
flatR:
    if (gTerrainVelY >= 0 && gUnk_087336F0[gTerrainTileBelow] != 0
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
