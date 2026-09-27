#include "gba/gba.h"
#include "global.h"
#include "task.h"

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

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
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
extern s8 gUnk_087339F0[];
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY;           /* Task.velY */
extern s16 gTerrainBoxLeft;           /* box left offset */
extern s16 gTerrainPrevY;           /* actor y (room-relative) */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxTop;           /* box top offset */
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern u16 gTerrainTileBelow;           /* cell below: tile set */
extern u16 gTerrainTileLeft;           /* cell to the left: tile set */
extern s32 gTerrainVelX;           /* Task.velX */
extern s16 gTerrainBoxRight;           /* box right offset */

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 GetTilePushDown(u16 a);
s32 GetTilePushUp(u16 a);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);

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
            gTerrainProbeResult.unk1 = 1;
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
