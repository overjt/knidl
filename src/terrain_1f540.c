#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1f540.c (0x0801F540-0x0801FF83, issue #84).
 *
 * The probe sets of three more entry points in src/terrain_1bcac.c:
 * sub_0801bf1c's right/left wall probes sub_0801f540/sub_0801f6b0 (the
 * box's top corner through a gUnk_08732DF0 wall class, then the middle and
 * bottom corners), sub_0801c030's ceiling probe sub_0801f800 (sub_0801dee8
 * without the slope tiles) and landing probe sub_0801f9b8 (not while moving
 * up; it tracks passable floor tiles in gTerrainProbeResult.unkB bits 0-2), and
 * sub_0801c12c's pair sub_0801fc48 (on the ground: follow the floor or
 * drop off it) / sub_0801fe2c (in the air: land). */

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

extern u8 gCollisionTileSlope[];
extern u8 gUnk_08732DF0[];
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087339F0[];
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY;           /* Task.unk58 */
extern s16 gTerrainBoxLeft;           /* box left offset */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxTop;           /* box top offset */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern u16 gTerrainTileBelow;           /* cell below: tile set */
extern s16 gTerrainPrevBoxRight;           /* box right (room-relative) */
extern u16 gTerrainTileLeft;           /* cell to the left: tile set */
extern s16 gTerrainBoxRight;           /* box right offset */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 GetTileFloorSnap(u16 a);
s32 GetTilePushDown(u16 a);
s32 GetTilePushUp(u16 a);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);

/* Right wall probe of the third entry point (sub_0801bf1c). */
void sub_0801f540(void)
{
    s32 d;
    s32 x;
    s32 a;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0
        && (gUnk_087336F0[gTerrainTile] == 0
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
        && gUnk_087336F0[gTerrainTile] != 0)
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
        && gUnk_087336F0[gTerrainTile] != 0)
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
        && (gUnk_087336F0[gTerrainTile] == 0
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
        && gUnk_087336F0[gTerrainTile] != 0)
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
        && gUnk_087336F0[gTerrainTile] != 0)
    {
        d = GetTilePushRight(gTerrainTile);
        if (d != 0)
        {
            gTerrainProbeX += d;
            gTerrainProbeResult.unk0 = 2;
        }
    }
}

/* Ceiling probe of the fourth entry point (sub_0801c030): sub_0801dee8
   without the slope-tile case. */
void sub_0801f800(void)
{
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop) != 0
        && gUnk_087336F0[gTerrainTile] == 0)
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
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (gUnk_087339F0[gTerrainTileRight] == 0 || gUnk_087336F0[gTerrainTileRight] != 0))
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.unk1++;
        return;
    }
    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxTop) != 0
        && gCollisionTileSlope[gTerrainTile] == 0
        && gUnk_087336F0[gTerrainTile] == 0
        && (gUnk_087339F0[gTerrainTileLeft] == 0 || gUnk_087336F0[gTerrainTileLeft] != 0))
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.unk1++;
    }
}

/* Floor probe (none while gTerrainVelY, Task.unk58, is negative): land
   the box's bottom on the floor cell under it, else on a slope under one
   of its bottom corners.  Bits 0-2 of gTerrainProbeResult.unkB record, for the
   centre and the two corners, whether the cell below is a
   gUnk_087336F0 tile, and let such a cell be landed on next time. */
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
        && (gUnk_087336F0[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1)))
    {
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
        return;
    }
    if (gUnk_087336F0[gTerrainTileBelow] != 0)
        gTerrainProbeResult.unkB |= 1;
    else
        gTerrainProbeResult.unkB &= ~1;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        if (gUnk_087336F0[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 2))
        {
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
            gTerrainProbeResult.unk6 = 1;
            gTerrainProbeResult.unk2 = 1;
        }
    }
    else
    {
        if (gUnk_087336F0[gTerrainTileBelow] != 0 && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
            gTerrainProbeResult.unkB |= 2;
        else
            gTerrainProbeResult.unkB &= ~2;
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        if (gUnk_087336F0[gTerrainTile] != 0 && !(gTerrainProbeResult.unkB & 4))
            return;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeResult.unk2 = 1;
    }
    if (gUnk_087336F0[gTerrainTileBelow] != 0 && gCollisionTileShapeClass[gTerrainTileBelow] == 1)
        gTerrainProbeResult.unkB |= 4;
    else
        gTerrainProbeResult.unkB &= ~4;
}

/* Floor probe of the fifth entry point (sub_0801c12c) for a box standing on
   the ground: keep it on the floor cell under it (or the cell below), else
   count the floor cells under its two bottom corners and clear the
   on-ground flags when there are none (the box starts to fall). */
void sub_0801fc48(void)
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
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTileBelow];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTileBelow) + 16;
    }
    else
    {
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
    }
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
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
        gTerrainProbeResult.unk2 = gTerrainProbeResult.unk3 = gTerrainProbeResult.unk6 = gTerrainProbeResult.unk5 = 0;
}

/* Landing probe of the fifth entry point (sub_0801c12c) for a box in the
   air: land the box's bottom on the floor cell under it, else a bottom
   corner on a floor cell, and set the on-ground flags. */
void sub_0801fe2c(void)
{
    s32 a;
    s32 a2;

    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainBoxBottom + gTerrainProbeY) != 0)
    {
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
        return;
    }
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    }
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (a2 = gCollisionTileShapeClass[gTerrainTile]) == 1)
    {
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeResult.unk2 = 1;
    }
}
