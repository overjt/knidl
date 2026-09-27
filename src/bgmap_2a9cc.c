#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* bgmap_2a9cc.c (0x0802A9CC-0x0802B2EF, issue #86).
 *
 * Tilemap streaming.  A room is gRoomWidth x gRoomHeight metatiles
 * (gRoomMap, 4 bytes each); a metatile is 2x2 tile entries in
 * gMetatileTiles.  DrawBg2Tile writes one tile of the 64x32-tile BG map
 * at 0x06002000, DrawBg1Tile one of the 32x32 map at 0x06001800 (from
 * the u16 metatile map gBg1MetatileMap) and DrawBg3Tile one of the 64x32
 * map at 0x06003000 (straight from the BG map gCurRoomDef->unk30).  The
 * rest loop them over a row, a column or the whole 36x26-tile window
 * around a pixel position, clamped to the room.  sub_0802b074 fills the
 * 0x06002000 map as 32x64 tiles instead, and sub_0802b168 builds such a
 * tile at a metatile edge from the solid flags (MapTile.unk3) of the three
 * neighbours it touches (table gUnk_080D71A0).  sub_0802b25c/sub_0802b29c
 * restore a metatile column from the backup copy 2048 cells further on. */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
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
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

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
void sub_0802b168(s32 x, s32 y);
void sub_0802b29c(s32 x, s32 y);

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

void sub_0802aae8(s32 x)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth * 2)
        return;
    for (y = 0; y < gRoomHeight * 2; y++)
        sub_0802b168(x, y);
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
    w = m->unk2;
    if (x0 < 0)
        x0 = 0;
    if (w < x1)
        x1 = w;
    h = m->unk4;
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

    if (y < 0 || y >= (m = gCurRoomDef->bg3Map)->unk4)
        return;
    w = m->unk2;
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

    if (x < 0 || x >= (m = gCurRoomDef->bg3Map)->unk2)
        return;
    h = m->unk4;
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
    ((u16 *)0x06001800)[i] = *src;
}

void DrawBg2Tile(s32 x, s32 y)
{
    u16 *src;
    s32 i;
    src = &gMetatileTiles[((&gRoomMap[x >> 1])[(y >> 1) * gRoomWidth].unk0 << 2)
                         + (x & 1) + ((y & 1) << 1)];
    i = (x & 31) + ((y & 31) << 5) + ((x & 32) << 5);
    ((u16 *)0x06002000)[i] = *src;
}

void DrawBg3Tile(s32 x, s32 y)
{
    u16 *src;
    s32 i;

    src = &gCurRoomDef->bg3Map->unk6[x] + gCurRoomDef->bg3Map->unk2 * y;
    i = (x & 31) + ((y & 31) << 5) + ((x & 32) << 5);
    ((u16 *)0x06003000)[i] = *src;
}

void sub_0802b074(s32 px)
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
            ((u16 *)0x06002000)[(x & 31) + ((i & 63) << 5)]
                = *(gMetatileTiles + ((&gRoomMap[x >> 1])[(i >> 1) * gRoomWidth].unk0 << 2) + (x & 1) + ((i & 1) << 1));
    for (i = x0; i <= x1; i++)
    {
        sub_0802b168(i, 26);
        sub_0802b168(i, 37);
    }
}

void sub_0802b168(s32 x, s32 y)
{
    u16 *dst;
    s32 q;
    s32 xn, yn;
    s32 a, b, c;
    s32 idx;

    dst = (u16 *)0x06002000 + ((x & 31) + ((y & 63) << 5));
    q = (x & 1) + ((y & 1) << 1);
    x >>= 1;
    y >>= 1;
    if ((&gRoomMap[x])[y * gRoomWidth].unk3 == 0)
    {
        xn = (q & 1) ? x + 1 : x - 1;
        yn = (q & 2) ? y + 1 : y - 1;
        a = (&gRoomMap[x])[yn * gRoomWidth].unk3 != 0;
        b = (&gRoomMap[xn])[y * gRoomWidth].unk3 != 0;
        c = (&gRoomMap[xn])[yn * gRoomWidth].unk3 != 0;
        if (a || b)
            idx = a + (b << 1);
        else
            idx = c << 2;
        *dst = gUnk_080D71A0[(idx << 2) + q];
    }
    else
    {
        *dst = *(gMetatileTiles + ((&gRoomMap[x])[y * gRoomWidth].unk0 << 2) + q);
    }
}

void sub_0802b25c(s32 x)
{
    s32 y;

    if (x < 0 || x >= gRoomWidth)
        return;
    for (y = 0; y < gRoomHeight; y++)
        sub_0802b29c(x, y);
}

void sub_0802b29c(s32 x, s32 y)
{
    s32 i;

    i = x + y * gRoomWidth;
    gRoomMap[i].unk0 = gRoomMap[i + 2048].unk0;
    gRoomMap[i].unk2 = gRoomMap[i + 2048].unk2;
    gRoomMap[i].unk3 = gRoomMap[i + 2048].unk3;
    gBlockLayer[i] = gBlockLayer[i + 2048];
}
