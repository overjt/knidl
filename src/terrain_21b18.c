#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_21b18.c (0x08021B18-0x0802296B, issue #93).
 *
 * Map and collision queries, the continuation of M06's terrain probes
 * (src/terrain_*.c).  gRoomMap is the room's metatile map,
 * gRoomWidth x gRoomHeight cells of 16x16 pixels.  GetCollisionTileAtPixel,
 * sub_08021b2c, sub_08021b70 and sub_08021bb4 read a cell's tile-set byte
 * (or unk2) at pixel or metatile coordinates; sub_08021c14, IsWaterAtPixel
 * and IsFullBlockAtPixel test a pixel for a wall through M06's TerrainQueryPixel and
 * the per-tile-set tables; sub_08022540 and sub_0802259c read the second
 * map layer gBlockLayer and a pixel's attribute.  sub_08021c74 resolves
 * a task's hit box against the floor and slopes (position
 * gTerrainProbeX/gTerrainProbeY, box offsets gTerrainBoxTop/84/1C/9C, results
 * in gTerrainProbeResult) and writes the corrected position back;
 * sub_0802205c and sub_0802233c do the same for walls, sub_080222b0 probes
 * the ground and TaskInitWaterFlags/sub_080224f8 set a task's in-wall state
 * (Task.unk7B).  The rest clamp a body or a task to the per-player bounds
 * gPlayerBounds, the camera bounds gCameraBounds or the room bounds
 * gRoomBounds and return which edges were hit. */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

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
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 *unk18;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u16 *unk28;
    /*0x2C*/ void *unk2C;
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u16 unk34;
    /*0x36*/ u16 unk36;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ u16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *unk44;
    /*0x48*/ void *unk48;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 unk50;
    /*0x52*/ u16 unk52;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct CamRect { s16 x0, x1, y0, y1; };

extern struct MapCell *gRoomMap;
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern u16 gTerrainTile;
extern s8 gUnk_087339F0[];
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gTerrainProbeX;
extern s16 gTerrainProbeY;
extern s16 gTerrainBoxTop;
extern s16 gTerrainBoxBottom;
extern s16 gTerrainBoxLeft;
extern s16 gTerrainBoxRight;
extern s8 gCollisionTileShapeClass[];
extern u16 gUnk_03005574;
extern u16 gTerrainTileBelow;
extern u16 gUnk_030055AC;
extern u16 gTerrainPixelIndex;
extern u16 gUnk_08735018[];
extern s8 gUnk_087336F0[];
extern u8 gCollisionTileSlope[];
extern u16 gTerrainTileRight;
extern u8 gUnk_08732DF0[];
extern u16 gTerrainTileLeft;
extern u16 gBlockLayer[];
extern struct RoomDef *gCurRoomDef;
extern s8 *const gCollisionTileShapes[];
extern s8 gUnk_087335F0[];
extern u8 gUnk_03005568;
extern u16 gUnk_03005544;
extern struct CamRect gPlayerBounds[4];
extern u8 gUnk_02005574[];
extern s16 gRoomBounds[4];
extern s16 gCameraBounds[4];

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 sub_08021970(u16 a);
s32 sub_08021b2c(u32 x, u32 y);

s32 GetCollisionTileAtPixel(u16 x, u16 y)
{
    return sub_08021b2c(x >> 4, y >> 4);
}

s32 sub_08021b2c(u32 x, u32 y)
{
    s16 w = gRoomWidth;
    u32 idx;

    if (x >= w || y >= gRoomHeight)
        return 0;
    idx = y * w;
    return (&gRoomMap[idx])[x].unk3;
}

s32 sub_08021b70(u32 x, u32 y)
{
    s16 w;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x >= w || y >= gRoomHeight)
        return 0;
    idx = y * w;
    return (&gRoomMap[idx])[x].unk2;
}

s32 sub_08021bb4(s16 x, s16 y, s16 dx, s16 dy)
{
    s32 cx = (x >> 4) + dx;
    s32 cy = (y >> 4) + dy;
    s32 w;
    s32 idx;

    if (cx <= 0 || cx >= (w = gRoomWidth) - 1 || cy <= 0 || cy >= gRoomHeight - 1)
        return -1;
    idx = cy * w;
    return (&gRoomMap[idx])[cx].unk3;
}

u16 sub_08021c14(s16 x, s16 y)
{
    TerrainQueryPixel(x, y);
    if (gTerrainTile <= 127 && gUnk_087339F0[gTerrainTile] == 0)
        return 0;
    return 1;
}

u8 IsWaterAtPixel(s16 x, s16 y)
{
    TerrainQueryPixel(x, y);
    if (gTerrainTile > 127)
        return 1;
    return 0;
}

void sub_08021c74(s8 *box, s32 id)
{
    struct Task *t = &gTasks[id];
    s32 tile;
    s32 flags;
    s32 n;

    gTerrainProbeX = (t->unk4C >> 16) + box[0];
    gTerrainProbeY = (t->unk50 >> 16) + box[1];
    gTerrainBoxTop = box[2];
    gTerrainBoxBottom = box[3];
    gTerrainBoxLeft = box[4];
    gTerrainBoxRight = box[5];
    gTerrainProbeResult.unk2++;
    gTerrainProbeResult.unk6 = 1;
    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
    if (gCollisionTileShapeClass[gTerrainTile] == 0)
    {
        tile = gUnk_08735018[gUnk_03005574];
        if (tile != 0)
        {
            flags = 0;
            if (gUnk_03005574 & 1)
            {
                if ((gTerrainProbeX & 0xFFF0) != ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0))
                    flags = 1;
            }
            else
            {
                if ((gTerrainProbeX & 0xFFF0) != ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                    flags = 2;
            }
            if (flags != 0 || gTerrainPixelIndex + 16 <= 255)
                goto slope;
        }
        if (gCollisionTileShapeClass[gTerrainTileBelow] == 0)
        {
            tile = gUnk_08735018[gUnk_030055AC];
            if (tile == 0)
                goto edges;
            if (gUnk_030055AC & 1)
            {
                if ((gTerrainProbeX & 0xFFF0) != ((gTerrainProbeX + gTerrainBoxLeft) & 0xFFF0))
                    flags = 1;
            }
            else
            {
                if ((gTerrainProbeX & 0xFFF0) != ((gTerrainProbeX + gTerrainBoxRight) & 0xFFF0))
                    flags = 2;
            }
            if (flags == 0)
                goto edges;
            goto slope;
        }
        gTerrainProbeY += sub_08021970(gTerrainTileBelow) + 16;
    }
    else
    {
        gTerrainProbeY += sub_08021970(gTerrainTile);
    }
    if (TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) != 0)
        gTerrainProbeY += sub_08021970(gTerrainTile);
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    goto done;

slope:
    /* Two identical arms: jump2 cross-jumps them into the ROM's
       `cmp #1; beq; cmp #2; bne` (one `flags == 1 || flags == 2` test
       folds to a `subs; cmp #1; bhi` range check). */
    if (flags == 1)
        gTerrainProbeY += sub_08021970(tile);
    else if (flags == 2)
        gTerrainProbeY += sub_08021970(tile);
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) == 0)
        flags &= ~1;
    if (TerrainQueryPixel(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) == 0)
        flags &= ~2;
    if (flags == 0)
        goto clear;
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    goto done;

edges:
    n = 0;
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        gTerrainProbeY += sub_08021970(gTerrainTile);
        n = 2;
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY + gTerrainBoxBottom) != 0)
    {
        gTerrainProbeY += sub_08021970(gTerrainTile);
        n++;
    }
    if (n == 0)
        goto clear;
    gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 1) >> 4;
    goto done;

clear:
    gTerrainProbeResult.unk6 = 0;
    gTerrainProbeResult.unk2 = 0;
    gTerrainProbeResult.unkB = 0;

done:
    t->unk7A = gTerrainProbeResult.unk6;
    t->unk84 = (gTerrainProbeResult.unkC << 8) | gTerrainProbeResult.unkB;
    n = gTerrainProbeX - box[0];
    if ((t->unk4C >> 16) != n)
    {
        t->unk4C = (n << 16) + 0x8000;
        t->unk48 = n;
    }
    n = gTerrainProbeY - box[1];
    if ((t->unk50 >> 16) != n)
    {
        t->unk50 = (n << 16) + 0x8000;
        t->unk4A = n;
    }
}

void sub_0802205c(s8 *box)
{
    gTerrainProbeX = (gCurTask->unk4C >> 16) + box[0];
    gTerrainProbeY = (gCurTask->unk50 >> 16) + box[1];
    gTerrainBoxTop = box[2];
    gTerrainBoxBottom = box[3];
    gTerrainBoxLeft = box[4];
    gTerrainBoxRight = box[5];
    gTerrainProbeResult.unkB = 0;
    if (gCurTask->unk43 != -1)
    {
        if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxRight, gTerrainProbeY) == 0)
        {
            if ((gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
                 && (gCollisionTileSlope[gTerrainTile] & 1))
                || (gUnk_087336F0[gTerrainTileRight] != 0 && gCollisionTileSlope[gTerrainTileRight] != 0
                    && (gCollisionTileSlope[gTerrainTileRight] & 1)
                    && ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) != 0x83 || (gTerrainProbeY & 15) <= 7)))
                gTerrainProbeResult.unkB |= 4;
            else
                goto end;
        }
    }
    else
    {
        if (TerrainQueryPixelAndSides(gTerrainProbeX + gTerrainBoxLeft, gTerrainProbeY) == 0)
        {
            if ((gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
                 && !(gCollisionTileSlope[gTerrainTile] & 1))
                || (gUnk_087336F0[gTerrainTileLeft] != 0 && gCollisionTileSlope[gTerrainTileLeft] != 0
                    && !(gCollisionTileSlope[gTerrainTileLeft] & 1)
                    && ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) != 0x83 || (gTerrainProbeY & 15) <= 7)))
                gTerrainProbeResult.unkB |= 2;
            else
                goto end;
        }
    }
    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom) == 0 && gUnk_087336F0[gTerrainTileBelow] != 0)
    {
        gTerrainProbeResult.unkB |= 1;
        if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
            gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom) >> 4;
        else
            gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
end:
    gCurTask->unk84 = gTerrainProbeResult.unkB;
}

void sub_080222b0(s32 x, s32 y)
{
    gTerrainProbeResult.unkB = 0;
    gTerrainProbeResult.unkC = 0;
    if (TerrainQueryPixelAndBelow(x, y) == 0 && gUnk_087336F0[gTerrainTileBelow] != 0)
    {
        gTerrainProbeResult.unkB |= 1;
        if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
            gTerrainProbeResult.unkC = y >> 4;
        else
            gTerrainProbeResult.unkC = (y + 16) >> 4;
    }
    else
    {
        gTerrainProbeResult.unkB &= 0xFE;
        gTerrainProbeResult.unkC = 0;
    }
}

void sub_0802233c(s8 *off)
{
    gTerrainProbeX = (gCurTask->unk4C >> 16) + off[0];
    gTerrainProbeY = (gCurTask->unk50 >> 16) + off[1];
    gTerrainProbeResult.unkB = 0;
    if (TerrainQueryPixelAndSides(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        if (gCurTask->unk43 != -1)
        {
            if ((gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
                 && (gCollisionTileSlope[gTerrainTile] & 1))
                || (gUnk_087336F0[gTerrainTileRight] != 0 && gCollisionTileSlope[gTerrainTileRight] != 0
                    && (gCollisionTileSlope[gTerrainTileRight] & 1)
                    && ((gUnk_08732DF0[gTerrainTileRight] & 0xCF) != 0x83 || (gTerrainProbeY & 15) <= 7)))
                gTerrainProbeResult.unkB |= 4;
        }
        else
        {
            if ((gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0
                 && !(gCollisionTileSlope[gTerrainTile] & 1))
                || (gUnk_087336F0[gTerrainTileLeft] != 0 && gCollisionTileSlope[gTerrainTileLeft] != 0
                    && !(gCollisionTileSlope[gTerrainTileLeft] & 1)
                    && ((gUnk_08732DF0[gTerrainTileLeft] & 0xCF) != 0x83 || (gTerrainProbeY & 15) <= 7)))
                gTerrainProbeResult.unkB |= 2;
        }
    }
    gCurTask->unk84 = gTerrainProbeResult.unkB;
}

void TaskInitWaterFlags(void)
{
    TerrainQueryPixel(gCurTask->unk4C >> 16, gCurTask->unk50 >> 16);
    if (gTerrainTile > 127)
        gCurTask->unk7B = 3;
    else
        gCurTask->unk7B = 0;
    gCurTask->unk84 = 128;
}

void sub_080224f8(s32 id)
{
    struct Task *t = &gTasks[id];

    TerrainQueryPixel(t->unk4C >> 16, t->unk50 >> 16);
    if (gTerrainTile > 127)
        t->unk7B = 3;
    else
        t->unk7B = 0;
    t->unk84 = 128;
}

s32 sub_08022540(u32 x, u32 y)
{
    s16 w;
    u16 *p;
    s32 v;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x < w && y < gRoomHeight)
    {
        idx = y * w + x;
        if (gBlockLayer[idx] != 0)
        {
            v = ((u8 *)gCurRoomDef->unk10)[gBlockLayer[idx] * 4 + 7] - 56;
            if ((u32)v <= 5)
                return v;
        }
    }
    return -1;
}

u16 sub_0802259c(u16 x, u16 y)
{
    s16 w;
    u32 idx;
    s32 tile;
    s32 r;

    x >>= 4;
    y >>= 4;
    w = gRoomWidth;
    if (x >= w || y >= gRoomHeight)
        return 0;
    idx = y * w;
    tile = (&gRoomMap[idx])[x].unk3;
    r = 0;
    if (tile > 127)
        r = 256;
    if (gCollisionTileShapes[tile][((y & 15) << 4) + (x & 15)] != 0)
        r |= gUnk_087335F0[tile];
    return r;
}

s32 IsFullBlockAtPixel(u16 x, u16 y)
{
    if (gCollisionTileShapeClass[GetCollisionTileAtPixel(x, y)] == 1)
        return 1;
    return 0;
}

void sub_08022650(void)
{
    gUnk_03005568 = 0;
    gUnk_03005544 = 0;
    if (gPlayerBounds[gCurTaskIdx].x0 > gTerrainProbeX + gTerrainBoxLeft)
    {
        gTerrainProbeX = gPlayerBounds[gCurTaskIdx].x0 - gTerrainBoxLeft;
        gUnk_03005568 = 1;
    }
    else if (gPlayerBounds[gCurTaskIdx].x1 < gTerrainProbeX + gTerrainBoxRight)
    {
        gTerrainProbeX = gPlayerBounds[gCurTaskIdx].x1 - gTerrainBoxRight;
        gUnk_03005568 = 2;
    }
    if (gPlayerBounds[gCurTaskIdx].y0 > gTerrainProbeY + gTerrainBoxTop)
    {
        gTerrainProbeY = gPlayerBounds[gCurTaskIdx].y0 - gTerrainBoxTop;
        gUnk_03005568 |= 4;
        if (gUnk_02005574[0] == 0)
            gUnk_03005544 = gPlayerBounds[gCurTaskIdx].y0;
    }
}

s32 sub_08022760(struct Task *t)
{
    if (gPlayerBounds[gCurTaskIdx].y1 < t->unk4A)
        return 1;
    return 0;
}

s32 sub_08022788(s32 y, s32 i)
{
    if (gPlayerBounds[i].y0 == y)
        return 1;
    return 0;
}

s32 ClampTaskToRoom(struct Task *t)
{
    s32 r = 0;
    s32 lo = gRoomBounds[0] - 111;
    s32 hi = gRoomBounds[1] + 111;

    if (lo > t->unk48)
    {
        t->unk4C = lo << 16;
        t->unk48 = lo;
        r = 1;
    }
    else if (hi < t->unk48)
    {
        t->unk4C = hi << 16;
        t->unk48 = hi;
        r = 2;
    }
    lo = gRoomBounds[2] - 72;
    if (lo > t->unk4A)
    {
        t->unk50 = lo << 16;
        t->unk4A = lo;
        r |= 4;
    }
    return r;
}

s32 sub_08022810(void)
{
    s32 lo;
    s32 hi;

    gUnk_03005568 = 0;
    lo = gCameraBounds[0] - 117;
    hi = gCameraBounds[1] + 117;
    if (lo > gTerrainProbeX + gTerrainBoxLeft)
    {
        gTerrainProbeX = lo - gTerrainBoxLeft;
        gUnk_03005568 = 1;
    }
    else if (hi < gTerrainProbeX + gTerrainBoxRight)
    {
        gTerrainProbeX = hi - gTerrainBoxRight;
        gUnk_03005568 = 2;
    }
    lo = gCameraBounds[2] - 76;
    if (lo > gTerrainProbeY + gTerrainBoxTop)
    {
        gTerrainProbeY = lo - gTerrainBoxTop;
        gUnk_03005568 |= 4;
    }
}

s32 sub_080228c4(struct Task *t)
{
    s32 r = 0;
    s32 lo = gCameraBounds[0] - 111;
    s32 hi = gCameraBounds[1] + 111;

    if (t->unk48 < lo)
    {
        if (lo - t->unk48 <= 11)
        {
            t->unk4C = lo << 16;
            t->unk48 = lo;
            r = 1;
        }
    }
    else if (hi < t->unk48)
    {
        if (t->unk48 - lo > 12)
        {
            t->unk4C = hi << 16;
            t->unk48 = hi;
            r = 2;
        }
    }
    lo = gCameraBounds[2] - 72;
    if (lo > t->unk4A)
    {
        if (lo - t->unk48 <= 11)
        {
            t->unk50 = lo << 16;
            t->unk4A = lo;
            r |= 4;
        }
    }
    return r;
}

s32 sub_0802294c(struct Task *t)
{
    if (gRoomBounds[3] + 104 < t->unk4A)
        return 1;
    return 0;
}
