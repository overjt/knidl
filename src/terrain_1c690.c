#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1c690.c (0x0801C690-0x0801C8DB, issue #84).
 *
 * The wall probes src/terrain_1bcac.c's entry points run for a box
 * standing on the ground (gTerrainProbeResult.unk6 != 0): sub_0801c690 (right
 * edge) and sub_0801c7cc (left edge) push the probe x out of a wall cell
 * (gCollisionTileShapeClass attribute 1) at the box's top, else its middle corner.
 * 
 * Matching note: cse records `attribute == 1` after the test and then swaps
 * the operands of a later AND with the attribute, so the ROM's source ANDs
 * the constant (`!(gCollisionTileSlope[t] & 1)`, two table reads) or compares a
 * cast (`(s8)a == 1`), and stores the constant `1`/`2` (cse substitutes the
 * attribute register). */

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
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087336F0[];
extern u16 gTerrainTileRight;
extern s16 gTerrainBoxLeft;           /* box left offset */
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;           /* probe x */
extern s16 gTerrainProbeY;           /* probe y */
extern u16 gTerrainTile;           /* queried cell: tile set */
extern s16 gTerrainBoxTop;           /* box top offset */
extern u16 gTerrainTileLeft;           /* cell to the left: tile set */
extern s16 gTerrainBoxRight;           /* box right offset */

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);

/* Right wall probe of a box standing on the ground (gTerrainProbeResult.unk6 !=
   0), the mirror image of sub_0801c7cc: step the probe x out of a wall
   cell at the box's top-right, else its middle-right corner. */
void sub_0801c690(void)
{
    s32 a;
    s32 d;
    s32 a2;
    s8 b;
    s32 d2;

    if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) != 0)
    {
        if ((a = gCollisionTileShapeClass[gTerrainTile]) == 1 && gUnk_087336F0[gTerrainTile] == 0
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
        if ((a2 = gCollisionTileShapeClass[gTerrainTile]) == 1 && gUnk_087336F0[gTerrainTile] == 0)
        {
            b = gCollisionTileShapeClass[gTerrainTileLeft];
            if (b == 0 || (gUnk_087336F0[gTerrainTileLeft] != 0 && b == 1))
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
   0), the mirror image of sub_0801c690: step the probe x out of a wall
   cell at the box's top-left, else its middle-left corner. */
void sub_0801c7cc(void)
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
        if ((s8)a == 1 && gUnk_087336F0[gTerrainTile] == 0
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
        if (a2 == 1 && gUnk_087336F0[gTerrainTile] == 0)
        {
            b = gCollisionTileShapeClass[gTerrainTileRight];
            if (b == 0 || (gUnk_087336F0[gTerrainTileRight] != 0 && b == 1))
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
