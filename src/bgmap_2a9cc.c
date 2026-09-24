#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* bgmap_2a9cc.c (0x0802A9CC-0x0802B2EF, issue #86).
 *
 * Tilemap streaming.  A room is gUnk_03005620 x gUnk_0300561C metatiles
 * (gUnk_03005660, 4 bytes each); a metatile is 2x2 tile entries in
 * gUnk_0200B080.  sub_0802afc8 writes one tile of the 64x32-tile BG map
 * at 0x06002000, sub_0802af6c one of the 32x32 map at 0x06001800 (from
 * the u16 metatile map gUnk_02004CA0) and sub_0802b030 one of the 64x32
 * map at 0x06003000 (straight from the BG map gUnk_030055EC->unk30).  The
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

struct Unk030055EC
{
    /*0x00*/ u8 filler00[0x30];
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u8 filler34[0xC];
    /*0x40*/ u16 unk40;
};

struct MapTile
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;
extern struct Unk030055EC *gUnk_030055EC;
extern u16 gUnk_02004CA0[];
extern u16 gUnk_0200B080[];
extern struct MapTile *gUnk_03005660;
extern u16 gUnk_080D71A0[];
extern u16 gUnk_02008160[];

void sub_0802af6c(s32 x, s32 y);
void sub_0802afc8(s32 x, s32 y);
void sub_0802b030(s32 x, s32 y);
void sub_0802b168(s32 x, s32 y);
void sub_0802b29c(s32 x, s32 y);

void sub_0802a9cc(s32 px, s32 py)
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
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            sub_0802afc8(x, y);
}

void sub_0802aa4c(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gUnk_0300561C * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    for (x = x0; x <= x1; x++)
        sub_0802afc8(x, y);
}

void sub_0802aa9c(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gUnk_03005620 * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
        sub_0802afc8(x, y);
}

void sub_0802aae8(s32 x)
{
    s32 y;

    if (x < 0 || x >= gUnk_03005620 * 2)
        return;
    for (y = 0; y < gUnk_0300561C * 2; y++)
        sub_0802b168(x, y);
}

void sub_0802ab30(s32 px, s32 py)
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
    m = gUnk_030055EC->unk30;
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
            sub_0802b030(x, y);
}

void sub_0802aba8(s32 x0, s32 x1, s32 y)
{
    s32 x;
    struct BgMap *m;
    s32 w;

    if (y < 0 || y >= (m = gUnk_030055EC->unk30)->unk4)
        return;
    w = m->unk2;
    if (x0 < 0)
        x0 = 0;
    if (w < x1)
        x1 = w;
    for (x = x0; x <= x1; x++)
        sub_0802b030(x, y);
}

void sub_0802abec(s32 x, s32 y0, s32 y1)
{
    s32 y;
    struct BgMap *m;
    s32 h;

    if (x < 0 || x >= (m = gUnk_030055EC->unk30)->unk2)
        return;
    h = m->unk4;
    if (y0 < 0)
        y0 = 0;
    if (h < y1)
        y1 = h;
    for (y = y0; y <= y1; y++)
        sub_0802b030(x, y);
}

void sub_0802ac30(s32 px, s32 py)
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
    if (gUnk_03005620 * 2 < x1)
        x1 = gUnk_03005620 * 2;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 < y1)
        y1 = gUnk_0300561C * 2;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
        {
            sub_0802af6c(x, y);
            sub_0802afc8(x, y);
            sub_0802b030(x, y);
        }
}

void sub_0802acbc(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gUnk_0300561C * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    for (x = x0; x <= x1; x++)
    {
        sub_0802af6c(x, y);
        sub_0802afc8(x, y);
        sub_0802b030(x, y);
    }
}

void sub_0802ad1c(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gUnk_03005620 * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
    {
        sub_0802af6c(x, y);
        sub_0802afc8(x, y);
        sub_0802b030(x, y);
    }
}

void sub_0802ad78(s32 px, s32 py)
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
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
        {
            sub_0802afc8(x, y);
            sub_0802b030(x, y);
        }
}

void sub_0802ae00(s32 x0, s32 x1, s32 y)
{
    s32 x;

    if (y < 0 || y >= gUnk_0300561C * 2)
        return;
    if (x0 < 0)
        x0 = 0;
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    for (x = x0; x <= x1; x++)
    {
        sub_0802afc8(x, y);
        sub_0802b030(x, y);
    }
}

void sub_0802ae58(s32 x, s32 y0, s32 y1)
{
    s32 y;

    if (x < 0 || x >= gUnk_03005620 * 2)
        return;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
    {
        sub_0802afc8(x, y);
        sub_0802b030(x, y);
    }
}

void sub_0802aeac(s32 py)
{
    s32 y0, y1;
    s32 x, y;

    py >>= 3;
    y0 = py - 3;
    y1 = py + 22;
    if (y0 < 0)
        y0 = 0;
    if (gUnk_0300561C * 2 <= y1)
        y1 = gUnk_0300561C * 2 - 1;
    for (y = y0; y <= y1; y++)
        for (x = 0; x < gUnk_03005620 * 2; x++)
        {
            sub_0802afc8(x, y);
            sub_0802b030(x, y);
        }
}

void sub_0802af1c(s32 y)
{
    s32 x;

    if (y < 0 || y >= gUnk_0300561C * 2)
        return;
    for (x = 0; x < gUnk_03005620 * 2; x++)
    {
        sub_0802afc8(x, y);
        sub_0802b030(x, y);
    }
}

void sub_0802af6c(s32 x, s32 y)
{
    u16 *src;
    s32 i;

    src = &gUnk_0200B080[(gUnk_02004CA0[(x >> 1) + (y >> 1) * gUnk_03005620] << 2)
                         + (x & 1) + ((y & 1) << 1)];
    i = (x & 31) + ((y & 31) << 5);
    ((u16 *)0x06001800)[i] = *src;
}

void sub_0802afc8(s32 x, s32 y)
{
    u16 *src;
    s32 i;
    src = &gUnk_0200B080[((&gUnk_03005660[x >> 1])[(y >> 1) * gUnk_03005620].unk0 << 2)
                         + (x & 1) + ((y & 1) << 1)];
    i = (x & 31) + ((y & 31) << 5) + ((x & 32) << 5);
    ((u16 *)0x06002000)[i] = *src;
}

void sub_0802b030(s32 x, s32 y)
{
    u16 *src;
    s32 i;

    src = &gUnk_030055EC->unk30->unk6[x] + gUnk_030055EC->unk30->unk2 * y;
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
    if (gUnk_03005620 * 2 <= x1)
        x1 = gUnk_03005620 * 2 - 1;
    for (i = 0; i < gUnk_0300561C * 2; i++)
        for (x = x0; x <= x1; x++)
            ((u16 *)0x06002000)[(x & 31) + ((i & 63) << 5)]
                = *(gUnk_0200B080 + ((&gUnk_03005660[x >> 1])[(i >> 1) * gUnk_03005620].unk0 << 2) + (x & 1) + ((i & 1) << 1));
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
    if ((&gUnk_03005660[x])[y * gUnk_03005620].unk3 == 0)
    {
        xn = (q & 1) ? x + 1 : x - 1;
        yn = (q & 2) ? y + 1 : y - 1;
        a = (&gUnk_03005660[x])[yn * gUnk_03005620].unk3 != 0;
        b = (&gUnk_03005660[xn])[y * gUnk_03005620].unk3 != 0;
        c = (&gUnk_03005660[xn])[yn * gUnk_03005620].unk3 != 0;
        if (a || b)
            idx = a + (b << 1);
        else
            idx = c << 2;
        *dst = gUnk_080D71A0[(idx << 2) + q];
    }
    else
    {
        *dst = *(gUnk_0200B080 + ((&gUnk_03005660[x])[y * gUnk_03005620].unk0 << 2) + q);
    }
}

void sub_0802b25c(s32 x)
{
    s32 y;

    if (x < 0 || x >= gUnk_03005620)
        return;
    for (y = 0; y < gUnk_0300561C; y++)
        sub_0802b29c(x, y);
}

void sub_0802b29c(s32 x, s32 y)
{
    s32 i;

    i = x + y * gUnk_03005620;
    gUnk_03005660[i].unk0 = gUnk_03005660[i + 2048].unk0;
    gUnk_03005660[i].unk2 = gUnk_03005660[i + 2048].unk2;
    gUnk_03005660[i].unk3 = gUnk_03005660[i + 2048].unk3;
    gUnk_02008160[i] = gUnk_02008160[i + 2048];
}
