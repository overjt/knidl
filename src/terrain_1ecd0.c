#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1ecd0.c (0x0801ECD0-0x0801F53F, issue #84).
 *
 * The landing probe sub_0801bcac runs for a box in the air
 * (gTerrainProbeResult.unk6 == 0): land on the floor under the box's centre, else
 * follow the cell's slope link gUnk_08735018[byte 2] and test the floor
 * under the box's left and right bottom corners, keeping the corner/slope
 * flags in gTerrainProbeResult.unkB. */

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 waterFlags;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 onSlipperyFloor;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

/* ROM tables indexed by tile set (0x100 entries each). */
extern s8 *const gCollisionTileShapes[];   /* per-tile-set pixel attribute tables */
extern u8 gCollisionTileSlope[];
extern u8 gUnk_08732DF0[];
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087339F0[];
extern u16 gUnk_08735018[];         /* indexed by the cell's byte 2 */
extern u16 gTerrainPixelIndex;           /* pixel offset inside the queried cell */
extern s16 gTerrainPrevBoxLeft;           /* box left (room-relative) */
extern s32 gTerrainVelY;           /* Task.velY */
extern s16 gTerrainBoxLeft;           /* box left offset */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern u16 gTerrainTileBelow;           /* cell below: tile set */
extern s16 gTerrainPrevBoxRight;           /* box right (room-relative) */
extern s16 gTerrainBoxRight;           /* box right offset */
extern u16 gUnk_030055AC;           /* cell below: byte 2 */
extern s16 gTerrainPrevBoxBottom;           /* box bottom (room-relative) */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 GetTilePushUp(u16 a);

/* Landing probe sub_0801bcac runs for a box in the air
   (gTerrainProbeResult.unk6 == 0): snap to the floor under the box's centre;
   failing that, follow the cell's slope link (gUnk_08735018[byte 2]) and
   test the floor under the box's left and right corners, keeping the
   corner/slope flags in gTerrainProbeResult.unkB.  The corner results share their flag updates through
   the labels at the end of each half. */
void sub_0801ecd0(void)
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
        if (gUnk_087336F0[gTerrainTile] != 0)
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
                     && gTerrainProbeResult.unkC > ((gTerrainProbeY + gTerrainBoxBottom) >> 4))
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
