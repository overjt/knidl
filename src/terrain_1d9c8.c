#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1d9c8.c (0x0801D9C8-0x0801E177, issue #84).
 *
 * The probes src/terrain_1bcac.c's entry points run for a box in the air
 * (gTerrainProbeResult.unk6 == 0) before its landing probe: the right and left wall probes sub_0801d9c8 /
 * sub_0801dc88 (top, middle and, in the moving direction, bottom corner of
 * the box edge) and the ceiling probe sub_0801dee8.
 * 
 * Matching notes: a tile attribute the ROM tests with `cmp #1` and then ANDs
 * with a flag is `(flags & 1)`, the constant: cse knows the attribute
 * register holds 1 and substitutes it for the constant, which keeps the AND
 * in place on that register; writing `a & flags` lets cse swap the operands
 * instead.  The ceiling probe's two side tests each end with their own
 * `gTerrainProbeY += ...; gTerrainProbeResult.unk1++;` (lesson 3.430). */

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

/* ROM tables indexed by tile set (0x100 entries each). */
extern s8 *const gCollisionTileShapes[];   /* per-tile-set pixel attribute tables */
extern u8 gCollisionTileSlope[];
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087339F0[];
extern u16 gTerrainPixelIndex;           /* pixel offset inside the queried cell */
extern s16 gUnk_0300550C;           /* box left (room-relative) */
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY;           /* Task.unk58 */
extern s16 gTerrainBoxLeft;           /* box left offset */
extern s16 gTerrainPrevY;           /* actor y (room-relative) */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxTop;           /* box top offset */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern s16 gUnk_03005590;           /* box right (room-relative) */
extern u16 gTerrainTileLeft;           /* cell to the left: tile set */
extern s32 gTerrainVelX;           /* Task.unk54 */
extern s16 gTerrainBoxRight;           /* box right offset */
extern s16 gUnk_030055A4;           /* box top (room-relative) */
extern s16 gUnk_030055B0;           /* box bottom (room-relative) */
extern u16 gUnk_08735098[];

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 GetTilePushDown(u16 a);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);

/* Right wall probe of a box in the air (gTerrainProbeResult.unk6 == 0), the
   mirror image of sub_0801dc88: the wall cell's attribute (1) goes to
   gTerrainProbeResult.unk0. */
void sub_0801d9c8(void)
{
    s32 d;
    s32 a;
    s32 a2;
    s32 a3;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gUnk_087339F0[gTerrainTileLeft] == 0 || gUnk_087336F0[gTerrainTileLeft] != 0))
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
        && gUnk_087336F0[gTerrainTile] == 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gUnk_087339F0[gTerrainTileLeft] == 0 || gUnk_087336F0[gTerrainTileLeft] != 0))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0
            && (gUnk_030055A4 & 0xFFF0) <= ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0)
            && ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0) > (gUnk_03005590 & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
            return;
        }
    }
    if (gTerrainVelX > 0
        && TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (a3 = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gUnk_087339F0[gTerrainTileLeft] == 0
            || (gUnk_087336F0[gTerrainTileLeft] != 0 && (gTerrainProbeResult.unkB & 1) == 0)))
    {
        d = GetTilePushLeft(gTerrainTile);
        if (d != 0
            && ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0) > (gUnk_03005590 & 0xFFF0)
            && (gUnk_030055B0 & 0xFFF0) >= ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 1;
        }
    }
}

/* Left wall probe of a box in the air (gTerrainProbeResult.unk6 == 0): the top,
   middle and (when moving left) bottom of the box's left edge, each pushing
   the probe x out of a wall cell and setting gTerrainProbeResult.unk0 = 2. */
void sub_0801dc88(void)
{
    s32 d;
    s32 a;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) != 0
        && gUnk_087336F0[gTerrainTile] == 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && (gUnk_087339F0[gTerrainTileRight] == 0 || gUnk_087336F0[gTerrainTileRight] != 0))
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
        && gUnk_087336F0[gTerrainTile] == 0
        && gCollisionTileShapeClass[gTerrainTile] == 1
        && (gUnk_087339F0[gTerrainTileRight] == 0 || gUnk_087336F0[gTerrainTileRight] != 0))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0
            && (gUnk_030055A4 & 0xFFF0) <= ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0)
            && ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gUnk_0300550C & 0xFFF0))
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
            return;
        }
    }
    if (gTerrainVelX < 0
        && TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1
        && (gUnk_087339F0[gTerrainTileRight] == 0
            || (gUnk_087336F0[gTerrainTileRight] != 0 && (gTerrainProbeResult.unkB & 1) == 0)))
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0
            && ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gUnk_0300550C & 0xFFF0)
            && (gUnk_030055B0 & 0xFFF0) >= ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0))
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
void sub_0801dee8(void)
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
        p = &tbl[gUnk_03005574];
        if (*p == 0)
            goto side;
        slope = (gUnk_03005574 & 1) ? 1 : 2;
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
        gTerrainProbeResult.unk1++;
        gTerrainProbeResult.unk4 = gCollisionTileSlope[t];
        gTerrainProbeY += GetTilePushDown(t);
        return;
    }
    if (gUnk_087336F0[gTerrainTile] == 0)
    {
        gTerrainProbeResult.unk1++;
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0)
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushDown(gTerrainTile);
        }
        return;
    }
side:
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (gUnk_087339F0[gTerrainTileRight] == 0 || gUnk_087336F0[gTerrainTileRight] != 0)
        && slope == 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.unk1++;
        return;
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (gUnk_087339F0[gTerrainTileLeft] == 0 || gUnk_087336F0[gTerrainTileLeft] != 0)
        && slope == 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.unk1++;
    }
}
