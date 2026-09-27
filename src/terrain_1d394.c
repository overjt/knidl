#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1d394.c (0x0801D394-0x0801D9C7, issue #84).
 *
 * The floor probe sub_0801bcac runs for a box standing on the ground
 * (gTerrainProbeResult.unk6 != 0), the simpler sibling of sub_0801c930 in
 * src/terrain_1c930.c. */

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
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087336F0[];
extern u16 gUnk_08735018[];         /* indexed by the cell's byte 2 */
extern u16 gTerrainPixelIndex;           /* pixel offset inside the queried cell */
extern s16 gTerrainBoxLeft;           /* box left offset */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gUnk_03005574;           /* queried cell: byte 2 */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern u16 gTerrainTileBelow;           /* cell below: tile set */
extern s32 gTerrainVelX;           /* Task.unk54 */
extern s16 gTerrainBoxRight;           /* box right offset */
extern u16 gUnk_030055AC;           /* cell below: byte 2 */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 GetTileFloorSnap(u16 a);
s32 sub_08021ab4(u32 x, u32 y);

/* Floor probe of a box standing on the ground (gTerrainProbeResult.unk6 != 0). */
void sub_0801d394(void)
{
    s32 tile;
    s32 tile2;
    s32 hit;
    s32 dir;

    gTerrainProbeResult.unk2++;
    gTerrainProbeResult.unkB = 0;
    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (gCollisionTileShapeClass[gTerrainTile] != 0)
    {
        if (gUnk_087336F0[gTerrainTile] == 0)
            goto floor;
        gTerrainProbeResult.unkB |= 1;
        if (gCollisionTileSlope[gTerrainTile] != 0)
            goto floor;
        if (gTerrainProbeResult.unkC <= (gTerrainProbeY + gTerrainBoxBottom) >> 4)
            goto floor;
    }
    if (gUnk_08735018[gUnk_03005574] != 0)
    {
        hit = 0;
        if (gUnk_03005574 & 1)
        {
            dir = 1;
            if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                hit = 1;
        }
        if (hit != 0 || gTerrainPixelIndex + 16 <= 255)
            goto slope;
    }
    if (gCollisionTileShapeClass[gTerrainTileBelow] != 0)
    {
        if (gUnk_087336F0[gTerrainTileBelow] != 0)
            gTerrainProbeResult.unkB |= 1;
    }
    else
    {
        if (gUnk_08735018[gUnk_030055AC] == 0)
            goto edges;
        hit = 0;
        if (gUnk_030055AC & 1)
        {
            dir = 1;
            if (((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0) < (gTerrainProbeX & 0xFFF0))
                hit = 1;
        }
        else
        {
            dir = 2;
            if ((gTerrainProbeX & 0xFFF0) < ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                hit = 1;
        }
        if (hit == 0)
            goto edges;
        goto slope_below;
    }
    gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTileBelow];
    gTerrainProbeY += GetTileFloorSnap(gTerrainTileBelow) + 16;
    goto check;

floor:
    gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
    gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
check:
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || gTerrainProbeResult.unkC <= (gTerrainProbeY + gTerrainBoxBottom) >> 4))
    {
        gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
    }
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

slope:
    tile = gUnk_08735018[gUnk_03005574];
    if (dir == 1)
    {
        if (hit != 0)
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[tile];
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        if (hit != 0)
        {
            gTerrainProbeResult.unk4 = gCollisionTileSlope[tile];
            gTerrainProbeY += GetTileFloorSnap(tile);
        }
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
    goto sides;

slope_below:
    tile = gUnk_08735018[gUnk_030055AC];
    if (dir == 1)
    {
        gTerrainProbeResult.unk4 = gCollisionTileSlope[tile];
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX > 0)
            gTerrainProbeResult.unk3 = dir;
    }
    else if (dir == 2)
    {
        gTerrainProbeResult.unk4 = gCollisionTileSlope[tile];
        gTerrainProbeY += GetTileFloorSnap(tile) + 16;
        if (gTerrainVelX < 0)
            gTerrainProbeResult.unk3 = 1;
    }
sides:
    if (sub_08021ab4(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~1;
    if (sub_08021ab4(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
        dir &= ~2;
    if (dir == 0)
        goto clear;
    if (gUnk_087336F0[tile] != 0)
    {
        if (dir == 1)
            gTerrainProbeResult.unkB = 16;
        else
            gTerrainProbeResult.unkB = 32;
    }
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

edges:
    dir = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.unkC))
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        tile = gTerrainTile;
    }
    else
    {
        tile = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[tile] != 0)
        dir += 2;
    if (gUnk_087336F0[tile] != 0)
        gTerrainProbeResult.unkB |= 2;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0
        && (gUnk_087336F0[gTerrainTile] == 0 || gCollisionTileSlope[gTerrainTile] != 0
            || (gTerrainProbeY + gTerrainBoxBottom) >> 4 >= gTerrainProbeResult.unkC))
    {
        gTerrainProbeY += GetTileFloorSnap(gTerrainTile);
        tile2 = gTerrainTile;
    }
    else
    {
        tile2 = gTerrainTileBelow;
    }
    if (gCollisionTileShapeClass[tile2] != 0)
        dir++;
    if (gUnk_087336F0[tile2] != 0)
        gTerrainProbeResult.unkB |= 4;
    if (dir == 0)
        goto clear;
    if (dir <= 2 && gTerrainVelX != 0)
    {
        if (gTerrainVelX < 0)
        {
            if (dir == 1)
                gTerrainProbeResult.unk3++;
        }
        else if (gTerrainVelX > 0 && dir == 2)
        {
            gTerrainProbeResult.unk3++;
        }
    }
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    return;

clear:
    gTerrainProbeResult.unk5 = 0;
    gTerrainProbeResult.unk6 = 0;
    gTerrainProbeResult.unk3 = 0;
    gTerrainProbeResult.unk2 = 0;
}
