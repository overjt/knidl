#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_207a0.c (0x080207A0-0x0802112F, issue #84).
 *
 * The probes of two more entry points: sub_080207a0, the slope-follow probe
 * of src/terrain_1c30c.c's sub_0801c30c, and sub_08020b38, the tile-edge
 * probe of src/terrain_1c444.c's sub_0801c444.
 * 
 * Matching note: both are a goto dispatch - the velocity-sign tests first,
 * then the bodies in the order right, left, up, down - which is the only
 * spelling that gives the ROM's layout (the first jump pass moves the first
 * block of each body reached by an unconditional goto into its place). */

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
extern s8 gUnk_087336F0[];
extern s8 gUnk_087338F0[];
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY;           /* Task.velY */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern u16 gTerrainTileBelow;           /* cell below: tile set */
extern u16 gTerrainTileLeft;           /* cell to the left: tile set */
extern s32 gTerrainVelX;           /* Task.velX */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 GetTilePushDown(u16 a);
s32 GetTilePushUp(u16 a);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);

/* Slope-follow probe of sub_0801c30c (src/terrain_1c30c.c): when the pixel
   under the probe point is solid, step the point along its cell -
   horizontally by the sign of Task.velX (gTerrainVelX), else vertically
   by the sign of Task.velY (gTerrainVelY) - up to two cells, mapping a
   passable cell to its step tile (gUnk_087338F0) while unkB bit 0 is set;
   otherwise remember in unkB bit 0 whether the cell below is passable. */
void sub_080207a0(void)
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
    if (gUnk_087336F0[gTerrainTileBelow] != 0)
        gTerrainProbeResult.unkB |= 1;
    else
        gTerrainProbeResult.unkB &= 0xFE;
    return;
right:
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 1;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushLeft(t);
    if (gCollisionTileSlope[gTerrainTile] != 0 && (gCollisionTileSlope[gTerrainTile] & 1))
        return;
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk4 = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushLeft(t);
    return;
left:
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 2;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushRight(t);
    if (gCollisionTileSlope[gTerrainTile] != 0 && !(gCollisionTileSlope[gTerrainTile] & 1))
        return;
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if (!(gTerrainProbeResult.unkB & 1))
            return;
        t = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        t = gTerrainTile;
    }
    gTerrainProbeResult.unk4 = gCollisionTileSlope[t];
    gTerrainProbeX += GetTilePushRight(t);
    return;
up:
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gUnk_087336F0[gTerrainTile] != 0)
        return;
    gTerrainProbeResult.unk1 = 1;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
    gTerrainProbeY += GetTilePushDown(gTerrainTile);
    if (!(gUnk_08732DF0[gTerrainTile] & 0x40)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
    }
    return;
down:
    if (gUnk_087336F0[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1))
    {
        gTerrainProbeResult.unk2 = 1;
        gTerrainProbeResult.unk6 = 1;
        gTerrainProbeY += GetTilePushUp(gTerrainTile);
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        if (!(gUnk_08732DF0[gTerrainTile] & 0x80)
            && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0
            && (gUnk_087336F0[gTerrainTile] == 0 || (gTerrainProbeResult.unkB & 1)))
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeY += GetTilePushUp(gTerrainTile);
        }
    }
}

/* The tile-edge probe of sub_0801c444 (src/terrain_1c444.c): with the box
   moving sideways, a wall probe at the probe point that steps onto
   gUnk_087338F0's step tiles and remembers passable wall tiles in
   gTerrainProbeResult.unkB bits 1/2; with it moving vertically, a ceiling or
   floor probe that sets unkB to the side (2 = left, 4 = right) on which
   the tile edge it stopped at continues. */
void sub_08020b38(void)
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
        if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
            && (gCollisionTileSlope[gTerrainTile] & 1) != 0)
        {
            gTerrainProbeResult.unkB |= 4;
            return;
        }
        if (gUnk_087336F0[gTerrainTileRight] == 0)
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
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if ((gTerrainProbeResult.unkB & 4) == 0)
            return;
        u = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        u = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 1;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[u];
    gTerrainProbeX += GetTilePushLeft(u);
    if ((gCollisionTileSlope[gTerrainTile] == 0 || (gCollisionTileSlope[gTerrainTile] & 1) == 0)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        if (gUnk_087336F0[gTerrainTile] != 0)
        {
            if ((gTerrainProbeResult.unkB & 4) == 0)
                return;
            u = gUnk_087338F0[gTerrainTile];
        }
        else
        {
            u = gTerrainTile;
        }
        gTerrainProbeResult.unk4 = gCollisionTileSlope[u];
        gTerrainProbeX += GetTilePushLeft(u);
    }
    gTerrainProbeResult.unkB = 0;
    if (gTerrainProbeResult.unk4 != 0)
        gTerrainProbeX++;
    return;
left:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        gTerrainProbeResult.unkB &= 0xFD;
        if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
            && (gCollisionTileSlope[gTerrainTile] & 1) == 0)
        {
            gTerrainProbeResult.unkB |= 2;
            return;
        }
        if (gUnk_087336F0[gTerrainTileLeft] == 0)
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
    if (gUnk_087336F0[gTerrainTile] != 0)
    {
        if ((gTerrainProbeResult.unkB & 2) == 0)
            return;
        u = gUnk_087338F0[gTerrainTile];
    }
    else
    {
        u = gTerrainTile;
    }
    gTerrainProbeResult.unk0 = 2;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[u];
    gTerrainProbeX += GetTilePushRight(u);
    if ((gCollisionTileSlope[gTerrainTile] == 0 || (gCollisionTileSlope[gTerrainTile] & 1) != 0)
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
    {
        if (gUnk_087336F0[gTerrainTile] != 0)
        {
            if ((gTerrainProbeResult.unkB & 2) == 0)
                return;
            u = gUnk_087338F0[gTerrainTile];
        }
        else
        {
            u = gTerrainTile;
        }
        gTerrainProbeResult.unk4 = gCollisionTileSlope[u];
        gTerrainProbeX += GetTilePushRight(u);
    }
    gTerrainProbeResult.unkB = 0;
    if (gTerrainProbeResult.unk4 != 0)
        gTerrainProbeX--;
    return;
up:
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
        return;
    if (gUnk_087336F0[gTerrainTile] != 0)
        return;
    gTerrainProbeResult.unk1 = 1;
    gTerrainProbeY += GetTilePushDown(gTerrainTile);
    if ((gUnk_08732DF0[gTerrainTile] & 0x40) == 0
        && TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY) != 0)
        gTerrainProbeY += GetTilePushDown(gTerrainTile);
    gTerrainProbeResult.unkB = 0;
    gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
    if (gTerrainProbeResult.unk4 == 0)
        return;
    gTerrainProbeY--;
    if ((gTerrainProbeResult.unk4 & 1) != 0)
    {
        if (gUnk_087336F0[gTerrainTileLeft] == 0)
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
    if (gUnk_087336F0[gTerrainTileRight] == 0)
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
    gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
    if (gTerrainProbeResult.unk4 == 0)
        return;
    gTerrainProbeY++;
    if ((gTerrainProbeResult.unk4 & 1) != 0)
    {
        if (gUnk_087336F0[gTerrainTileLeft] == 0)
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
    if (gUnk_087336F0[gTerrainTileRight] == 0)
        return;
    if (gCollisionTileSlope[gTerrainTileRight] == 0)
        return;
    if ((gCollisionTileSlope[gTerrainTileRight] & 1) == 0)
        return;
    if ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) == 0x83 && (gTerrainProbeY & 15) > 7)
        return;
    gTerrainProbeResult.unkB = 4;
}
