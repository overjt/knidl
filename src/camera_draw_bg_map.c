#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_draw_bg_map.c (0x0802A9CC-0x0802B2EF, issue #86).
 *
 * Tilemap streaming.  A room is gRoomWidth x gRoomHeight metatiles
 * (gRoomMap, 4 bytes each); a metatile is 2x2 tile entries in
 * gMetatileTiles.  DrawBg2Tile writes one tile of the 64x32-tile BG map
 * at 0x06002000, DrawBg1Tile one of the 32x32 map at 0x06001800 (from
 * the u16 metatile map gBg1MetatileMap) and DrawBg3Tile one of the 64x32
 * map at 0x06003000 (straight from the BG map gCurRoomDef->unk30).  The
 * rest loop them over a row, a column or the whole 36x26-tile window
 * around a pixel position, clamped to the room.  DrawBg2ViewLooping fills the
 * 0x06002000 map as 32x64 tiles instead, and DrawBg2EdgeTile builds such a
 * tile at a metatile edge from the solid flags (MapTile.collisionTile) of the three
 * neighbours it touches (table gUnk_080D71A0).  RestoreMapColumn/RestoreMapCell
 * restore a metatile column from the backup copy 2048 cells further on. */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 width;
    /*0x04*/ u16 height;
    /*0x06*/ u16 unk6[0];
};

/* The room header gCurRoomDef points at (one entry of the gRoomTable
   room table): unk18/unk28 are length-prefixed palettes, unk30 the BG map
   streamed into 0x06003000, unk40 the room's BG animation script set. */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x18];
    /*0x18*/ u16 *bg2Palette;
    /*0x1C*/ u8 filler1C[0xC];
    /*0x28*/ u16 *bg3Palette;
    /*0x2C*/ u8 filler2C[4];
    /*0x30*/ struct BgMap *bg3Map;
    /*0x34*/ u8 filler34[0xC];
    /*0x40*/ u16 bgAnimSet;
};

struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 slopeIndex;
    /*0x03*/ u8 collisionTile;
};

/* Not from room.h: this file's view of gRoomMap differs (lesson 3.517). */
extern s16 gRoomHeight;
extern s16 gRoomWidth;
extern struct RoomDef *gCurRoomDef;
extern u16 gBg1MetatileMap[];
extern u16 gMetatileTiles[];
extern struct MapTile *gRoomMap;
extern u16 gUnk_080D71A0[];
extern u16 gBlockLayer[];

void DrawBg1Tile(s32 x, s32 y);
void DrawBg2Tile(s32 x, s32 y);
void DrawBg3Tile(s32 x, s32 y);
void DrawBg2EdgeTile(s32 x, s32 y);
void RestoreMapCell(s32 x, s32 y);

void DrawBg2View(s32 px, s32 py)
{
    s32 x0, x1, y0, y1;
    s32 x, y;

    px >>= 3;
    x0 = px - 3;
    x1 = px + 32;
    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            DrawBg2Tile(x, y);
}

void DrawBg2Row(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gRoomHeight * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    for (x = x0; x <= x1; x++)
        DrawBg2Tile(x, y);
}

void DrawBg2Column(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
        DrawBg2Tile(x, y);
}

void DrawBg2EdgeColumn(s32 x)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth * 2)
        return;
    for (y = 0; y < gRoomHeight * 2; y++)
        DrawBg2EdgeTile(x, y);
}

void DrawBg3View(s32 px, s32 py)
{
    s32 x0, x1, y0, y1;
    s32 x, y;
    struct BgMap *m;
    s32 w, h;

    px >>= 3;
    x0 = px - 3;
    x1 = px + 32;
    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    m = gCurRoomDef->bg3Map;
    w = m->width;
    if (x0 < 0)
        x0 = 0;
    if (w < x1)
        x1 = w;
    h = m->height;
    if (y0 < 0)
        y0 = 0;
    if (h < y1)
        y1 = h;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            DrawBg3Tile(x, y);
}

void DrawBg3Row(s32 x0, s32 x1, s32 y)
{
    s32 x;
    struct BgMap *m;
    s32 w;

    if (y < 0 || y >= (m = gCurRoomDef->bg3Map)->height)
        return;
    w = m->width;
    if (x0 < 0)
        x0 = 0;
    if (w < x1)
        x1 = w;
    for (x = x0; x <= x1; x++)
        DrawBg3Tile(x, y);
}

void DrawBg3Column(s32 x, s32 y0, s32 y1)
{
    s32 y;
    struct BgMap *m;
    s32 h;

    if (x < 0 || x >= (m = gCurRoomDef->bg3Map)->width)
        return;
    h = m->height;
    if (y0 < 0)
        y0 = 0;
    if (h < y1)
        y1 = h;
    for (y = y0; y <= y1; y++)
        DrawBg3Tile(x, y);
}

void DrawBg123View(s32 px, s32 py)
{
    s32 x0, x1, y0, y1;
    s32 x, y;

    px >>= 3;
    x0 = px - 1;
    x1 = px + 30;
    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 < x1)
        x1 = gRoomWidth * 2;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 < y1)
        y1 = gRoomHeight * 2;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
        {
            DrawBg1Tile(x, y);
            DrawBg2Tile(x, y);
            DrawBg3Tile(x, y);
        }
}

void DrawBg123Row(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gRoomHeight * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    for (x = x0; x <= x1; x++)
    {
        DrawBg1Tile(x, y);
        DrawBg2Tile(x, y);
        DrawBg3Tile(x, y);
    }
}

void DrawBg123Column(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
    {
        DrawBg1Tile(x, y);
        DrawBg2Tile(x, y);
        DrawBg3Tile(x, y);
    }
}

void DrawBg23View(s32 px, s32 py)
{
    s32 x0, x1, y0, y1;
    s32 x, y;

    px >>= 3;
    x0 = px - 3;
    x1 = px + 32;
    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
        {
            DrawBg2Tile(x, y);
            DrawBg3Tile(x, y);
        }
}

void DrawBg23Row(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gRoomHeight * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    for (x = x0; x <= x1; x++)
    {
        DrawBg2Tile(x, y);
        DrawBg3Tile(x, y);
    }
}

void DrawBg23Column(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
    {
        DrawBg2Tile(x, y);
        DrawBg3Tile(x, y);
    }
}

void DrawBg23FullRows(s32 py)
{
    s32 y0, y1;
    s32 x, y;

    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    if (y0 < 0)
        y0 = 0;
    if (gRoomHeight * 2 <= y1)
        y1 = gRoomHeight * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = 0; x < gRoomWidth * 2; x++)
        {
            DrawBg2Tile(x, y);
            DrawBg3Tile(x, y);
        }
}

void DrawBg23FullRow(s32 y)
{
    s32 x;

    if (y < 0 || y >= gRoomHeight * 2)
        return;
    for (x = 0; x < gRoomWidth * 2; x++)
    {
        DrawBg2Tile(x, y);
        DrawBg3Tile(x, y);
    }
}

void DrawBg1Tile(s32 x, s32 y)
{
    u16 *src;
    s32 i;

    src = &gMetatileTiles[(gBg1MetatileMap[(x >> 1) + (y >> 1) * gRoomWidth] << 2)
                         + (x & 1) + ((y & 1) << 1)];
    i = (x & 31) + ((y & 31) << 5);
    ((u16 *)(BG_VRAM + 0x1800))[i] = *src;
}

void DrawBg2Tile(s32 x, s32 y)
{
    u16 *src;
    s32 i;
    src = &gMetatileTiles[((&gRoomMap[x >> 1])[(y >> 1) * gRoomWidth].metatile << 2)
                         + (x & 1) + ((y & 1) << 1)];
    i = (x & 31) + ((y & 31) << 5) + ((x & 32) << 5);
    ((u16 *)(BG_VRAM + 0x2000))[i] = *src;
}

void DrawBg3Tile(s32 x, s32 y)
{
    u16 *src;
    s32 i;

    src = &gCurRoomDef->bg3Map->unk6[x] + gCurRoomDef->bg3Map->width * y;
    i = (x & 31) + ((y & 31) << 5) + ((x & 32) << 5);
    ((u16 *)(BG_VRAM + 0x3000))[i] = *src;
}

void DrawBg2ViewLooping(s32 px)
{
    s32 x0, x1;
    s32 x, i;

    px >>= 3;
    x0 = px - 1;
    x1 = px + 30;
    if (x0 < 0)
        x0 = 0;
    if (gRoomWidth * 2 <= x1)
        x1 = gRoomWidth * 2 - 1;
    for (i = 0; i < gRoomHeight * 2; i++)
        for (x = x0; x <= x1; x++)
            ((u16 *)(BG_VRAM + 0x2000))[(x & 31) + ((i & 63) << 5)]
                = *(gMetatileTiles + ((&gRoomMap[x >> 1])[(i >> 1) * gRoomWidth].metatile << 2) + (x & 1) + ((i & 1) << 1));
    for (i = x0; i <= x1; i++)
    {
        DrawBg2EdgeTile(i, 26);
        DrawBg2EdgeTile(i, 37);
    }
}

void DrawBg2EdgeTile(s32 x, s32 y)
{
    u16 *dst;
    s32 q;
    s32 xn, yn;
    s32 a, b, c;
    s32 idx;

    dst = (u16 *)(BG_VRAM + 0x2000) + ((x & 31) + ((y & 63) << 5));
    q = (x & 1) + ((y & 1) << 1);
    x >>= 1;
    y >>= 1;
    if ((&gRoomMap[x])[y * gRoomWidth].collisionTile == 0)
    {
        xn = (q & 1) ? x + 1 : x - 1;
        yn = (q & 2) ? y + 1 : y - 1;
        a = (&gRoomMap[x])[yn * gRoomWidth].collisionTile != 0;
        b = (&gRoomMap[xn])[y * gRoomWidth].collisionTile != 0;
        c = (&gRoomMap[xn])[yn * gRoomWidth].collisionTile != 0;
        if (a || b)
            idx = a + (b << 1);
        else
            idx = c << 2;
        *dst = gUnk_080D71A0[(idx << 2) + q];
    }
    else
    {
        *dst = *(gMetatileTiles + ((&gRoomMap[x])[y * gRoomWidth].metatile << 2) + q);
    }
}

void RestoreMapColumn(s32 x)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth)
        return;
    for (y = 0; y < gRoomHeight; y++)
        RestoreMapCell(x, y);
}

void RestoreMapCell(s32 x, s32 y)
{
    s32 i;

    i = x + y * gRoomWidth;
    gRoomMap[i].metatile = gRoomMap[i + 2048].metatile;
    gRoomMap[i].slopeIndex = gRoomMap[i + 2048].slopeIndex;
    gRoomMap[i].collisionTile = gRoomMap[i + 2048].collisionTile;
    gBlockLayer[i] = gBlockLayer[i + 2048];
}
