#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* block_318b4.c (0x080318B4-0x08032687, issue #92).
 *
 * Breakable blocks, part 2: the three per-frame stage hooks M08's
 * src/obj_306b4.c stores into gUnk_030004A0, and their record helpers.
 * Each hook steps the animation script of every live record once a frame
 * (struct Unk020061F0: {op, arg} pairs - draw a frame, break the four
 * neighbours, wait, free) and then clears the "stepped" bit 15 of unk6.
 * sub_080318b4 draws the replacement metatiles into the BG map
 * (sub_08031b58/sub_080319d0 for a whole column of n blocks, through the
 * scratch record gUnk_02007FD0) and chains to the neighbours
 * (sub_08031d04); sub_08031de4 also rebuilds the 3x3 edge tiles around
 * the block (sub_08031f3c) for rooms whose BG map has edge tiles; and
 * sub_08032428 animates the blocks of the second layer gBg1MetatileMap in
 * the BG map at 0x06001800, with its own records gUnk_0200A6F0[] and
 * the probe/spawner pair sub_08032288/sub_08032338 that M08's map
 * events call.  sub_08031994/sub_080319b0/sub_080324e4 free a record,
 * sub_08031c38/sub_08031ebc write its metatile back into the map, and
 * sub_08031c7c/sub_08032520 draw its 2x2 tiles inside the visible
 * window gUnk_020055B8. */

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

/* gBreakingBlocks[64] (and gUnk_0200A6F0[64] for the second block layer
   gBg1MetatileMap): one 32-byte record per block being broken.  unk0/unk2 =
   metatile x/y, unk4 = its map index, unk6 = the position in the animation
   script (0x7FFF = free slot; bit 15 = already stepped this frame), unk8 =
   the metatile the cell shows next (RoomDef.unk10[] + the layer's low byte,
   advanced by one per drawn frame), unkC = its tile entry in the BG map,
   unk10 = the script (gUnk_0873A47C[kind], {op, arg} pairs: 1 and 2 draw a
   frame, 3 breaks the four neighbours, 4 waits arg frames, 0x8000/0x8001
   free the record), unk14 = the frames left to wait, unk16/unk18 = the
   metatile index and collision byte written back to the map, unk1A = the
   block kind, unk1C = the player that broke it (-1 = none). */
struct Unk020061F0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ struct MapTile *unk8;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
    /*0x1C*/ s8 unk1C;
    /*0x1D*/ u8 filler1D[3];
};

/* M09's view of the room header (M07's struct RoomDef, src/level_*.c): the
   only field read here is unk10, the table of replacement metatiles the low
   byte of gBlockLayer[] indexes (M07 types it void *). */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ struct MapTile *unk10;
};

extern struct Unk020061F0 gBreakingBlocks[];
extern u16 gBlockLayer[];             /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern s16 gRoomHeight;               /* map height in metatiles */
extern s16 gRoomWidth;               /* map width in metatiles */
extern struct RoomDef *gCurRoomDef;   /* the current room header */
extern struct Unk020061F0 gUnk_02007FD0;
extern struct MapTile *gRoomMap;   /* the room's metatile map */
extern s16 gUnk_020055B8[4];
extern u16 gMetatileTiles[];
extern u16 gUnk_0200B060[];
extern u16 gUnk_0873A6D4[][3];
extern u16 gUnk_0873A6EC[][3][3];
extern s8 gUnk_0873A734[][2];
extern u16 gUnk_080D71A0[];
extern u16 gBlockCursorX;               /* the block CanBreakBlock accepted: x */
extern u16 gBlockCursorY;               /*   y */
extern u16 gBlockCursorIndex;               /*   map index */
extern s32 gCameraCenterX;
extern s32 gCameraCenterY;
extern u16 gBg1MetatileMap[];
extern struct Unk020061F0 gUnk_0200A6F0[];
extern struct RoomDef **gRoomTable[][8];
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern u16 gUnk_0873A458[];

s32 PlaySfx(s32 id);
void sub_08026308(void);
void sub_0802b2f0(void);
void sub_0802b368(void);
void sub_0802b3e4(void);
s32 CanBreakBlock(s32 x, s32 y, s32 id, s32 e);
s32 sub_08031374(void);
void sub_08031994(struct Unk020061F0 *b);
void sub_080319b0(struct Unk020061F0 *b);
void sub_080319d0(struct Unk020061F0 *b, s32 n);
void sub_08031b58(struct Unk020061F0 *b, s32 n);
void sub_08031c38(struct Unk020061F0 *b);
void sub_08031c7c(struct Unk020061F0 *b);
void sub_08031d04(struct Unk020061F0 *b);
void sub_08031ebc(struct Unk020061F0 *b);
void sub_08031f3c(struct Unk020061F0 *b);
void sub_080324e4(struct Unk020061F0 *b);
void sub_08032500(struct Unk020061F0 *b);
void sub_08032520(struct Unk020061F0 *b);
void sub_080325b8(struct Unk020061F0 *b);

void sub_080318b4(void)
{
    s32 i;
    struct Unk020061F0 *b;
    u16 *p;

    sub_0802b2f0();
    for (i = 0; i < 64; i++)
    {
        b = &gBreakingBlocks[i];
        if (b->unk6 == 0x7FFF || (b->unk6 & 0x8000))
            continue;
    again:
        if ((s16)--b->unk14 > 0)
            continue;
        p = &b->unk10[b->unk6 * 2];
        switch (p[0])
        {
        case 1:
            sub_08031b58(b, (s16)p[1]);
            b->unk6++;
            goto again;
        case 2:
            sub_080319d0(b, (s16)p[1]);
            b->unk6++;
            goto again;
        case 3:
            sub_08031d04(b);
            b->unk6++;
            goto again;
        case 4:
            b->unk14 = p[1];
            b->unk6++;
            continue;
        case 0x8000:
        default:
            sub_08031994(b);
            continue;
        case 0x8001:
            sub_080319b0(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gBreakingBlocks[i].unk6 &= 0x7FFF;
}

void sub_08031994(struct Unk020061F0 *b)
{
    b->unk6 = 0x7FFF;
    b->unk14 = 0;
    gBlockLayer[b->unk4] = 0;
}

void sub_080319b0(struct Unk020061F0 *b)
{
    b->unk6 = 0x7FFF;
    b->unk14 = 0;
    gBlockLayer[b->unk4] &= 0x7FFF;
}

void sub_080319d0(struct Unk020061F0 *b, s32 n)
{
    s32 i;
    struct Unk020061F0 *s;

    if (n <= 0)
        return;
    sub_08031c38(b);
    sub_08031c7c(b);
    if (n != 1)
    {
        s = &gUnk_02007FD0;
        for (i = 1; i < n; i++)
        {
            s->unk0 = b->unk0;
            s->unk2 = b->unk2 + i;
            if (s->unk2 >= gRoomHeight)
                return;
            s->unk4 = gRoomWidth * i + b->unk4;
            if (gBlockLayer[s->unk4] != 0)
            {
                s->unk8 = &gCurRoomDef->unk10[gBlockLayer[s->unk4] & 0xFF];
                s->unkC = (u16 *)0x06002000 + (((s->unk0 * 2) & 31) + ((((s->unk2 * 2) & 31) + (u16)(s->unk0 & 16) * 2) << 5));
                s->unk16 = s->unk8->unk0;
                s->unk18 = s->unk8->unk3;
                sub_08031c38(s);
                sub_08031c7c(s);
                gBlockLayer[s->unk4] = 0;
            }
        }
    }
    b->unk8++;
}

void sub_08031ab8(struct Unk020061F0 *b, s32 n)
{
    s32 i;
    struct Unk020061F0 *s;

    if (n <= 0)
        return;
    sub_08031c38(b);
    if (n == 1)
        return;
    s = &gUnk_02007FD0;
    for (i = 1; i < n; i++)
    {
        if (b->unk2 + i >= gRoomHeight)
            return;
        s->unk4 = gRoomWidth * i + b->unk4;
        if (gBlockLayer[s->unk4] != 0)
        {
            s->unk8 = &gCurRoomDef->unk10[gBlockLayer[s->unk4] & 0xFF];
            sub_08031c38(s);
            gBlockLayer[s->unk4] |= 0x8000;
        }
    }
}

void sub_08031b58(struct Unk020061F0 *b, s32 n)
{
    s32 i;
    struct Unk020061F0 *s;

    if (n <= 0)
        return;

    sub_08031c7c(b);
    if (n != 1)
    {
        s = &gUnk_02007FD0;
        for (i = 1; i < n; i++)
        {
            s->unk0 = b->unk0;
            s->unk2 = b->unk2 + i;
            if (s->unk2 >= gRoomHeight)
                return;
            s->unk4 = gRoomWidth * i + b->unk4;
            if (gBlockLayer[s->unk4] != 0)
            {
                s->unk8 = &gCurRoomDef->unk10[gBlockLayer[s->unk4] & 0xFF] - 1;
                s->unkC = (u16 *)0x06002000 + (((s->unk0 * 2) & 31) + ((((s->unk2 * 2) & 31) + (u16)(s->unk0 & 16) * 2) << 5));
                s->unk16 = s->unk8->unk0;
                s->unk18 = s->unk8->unk3;

                sub_08031c7c(s);
                gBlockLayer[s->unk4] = 0;
            }
        }
    }
    b->unk8++;
}

void sub_08031c38(struct Unk020061F0 *b)
{
    b->unk16 = b->unk8->unk0;
    b->unk18 = b->unk8->unk3;
    gRoomMap[b->unk4].unk0 = b->unk16;
    gRoomMap[b->unk4].unk3 = b->unk18;
    gBlockLayer[b->unk4] = ((u8)gBlockLayer[b->unk4] + 1) | 0x8000;
}

void sub_08031c7c(struct Unk020061F0 *b)
{
    s32 i, j;

    for (i = 0; i <= 1; i++)
    {
        s32 ty = b->unk2 * 2 + i;

        if (ty >= gUnk_020055B8[2] && ty <= gUnk_020055B8[3])
        {
            for (j = 0; j <= 1; j++)
            {
                s32 tx = b->unk0 * 2 + j;

                if (tx >= gUnk_020055B8[0] && tx <= gUnk_020055B8[1])
                    (&b->unkC[j])[i * 32] = gMetatileTiles[b->unk16 * 4 + j + i * 2];
            }
        }
    }
}

void sub_08031d04(struct Unk020061F0 *b)
{
    s32 slot;

    if (CanBreakBlock(b->unk0, b->unk2 - 1, b->unk1A, b->unk1C) != 0)
    {
        slot = sub_08031374();
        if (slot != -1)
            gBreakingBlocks[slot].unk6 |= 0x8000;
    }
    if (CanBreakBlock(b->unk0 - 1, b->unk2, b->unk1A, b->unk1C) != 0)
    {
        slot = sub_08031374();
        if (slot != -1)
            gBreakingBlocks[slot].unk6 |= 0x8000;
    }
    if (CanBreakBlock(b->unk0 + 1, b->unk2, b->unk1A, b->unk1C) != 0)
    {
        slot = sub_08031374();
        if (slot != -1)
            gBreakingBlocks[slot].unk6 |= 0x8000;
    }
    if (CanBreakBlock(b->unk0, b->unk2 + 1, b->unk1A, b->unk1C) != 0)
    {
        slot = sub_08031374();
        if (slot != -1)
            gBreakingBlocks[slot].unk6 |= 0x8000;
    }
}

void sub_08031de4(void)
{
    s32 i;
    struct Unk020061F0 *b;
    u16 *p;

    sub_0802b3e4();
    for (i = 0; i < 64; i++)
    {
        b = &gBreakingBlocks[i];
        if (b->unk6 == 0x7FFF || (b->unk6 & 0x8000))
            continue;
    again:
        if ((s16)--b->unk14 > 0)
            continue;
        p = &b->unk10[b->unk6 * 2];
        switch (p[0])
        {
        case 2:
            sub_08031ebc(b);
        case 1:
            sub_08031f3c(b);
            b->unk6++;
            goto again;
        case 3:
            b->unk6++;
            goto again;
        case 4:
            b->unk14 = p[1];
            b->unk6++;
            continue;
        case 0x8000:
        default:
            sub_08031994(b);
            continue;
        case 0x8001:
            sub_080319b0(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gBreakingBlocks[i].unk6 &= 0x7FFF;
    sub_08026308();
}

void sub_08031ebc(struct Unk020061F0 *b)
{
    u32 x;
    u32 m;

    b->unk16 = b->unk8->unk0;
    b->unk18 = b->unk8->unk3;
    gRoomMap[b->unk4].unk0 = b->unk16;
    gRoomMap[b->unk4].unk3 = b->unk18;
    x = b->unk0;
    if (x > 39)
    {
        m = 31;
        m &= x;
        (&gRoomMap[m])[b->unk2 * gRoomWidth].unk0 = b->unk16;
        (&gRoomMap[m])[b->unk2 * gRoomWidth].unk3 = b->unk18;
    }
    gBlockLayer[b->unk4] = ((u8)gBlockLayer[b->unk4] + 1) | 0x8000;
}

void sub_08031f3c(struct Unk020061F0 *b)
{
    s32 j, i, l, n;
    s32 x, y, v;
    u16 *p;
    s32 k;

    if (b->unk18 != 0)
    {
        for (i = 0; i <= 1; i++)
        {
            y = b->unk2 * 2 + i;
            if (y >= gUnk_020055B8[2] && y <= gUnk_020055B8[3])
            {
                for (j = 0; j <= 1; j++)
                {
                    x = b->unk0 * 2 + j;
                    if (x >= gUnk_020055B8[0] && x <= gUnk_020055B8[1])
                        (&b->unkC[j])[i * 32] = gMetatileTiles[b->unk16 * 4 + j + i * 2];
                }
            }
        }
    }
    else
    {
        for (j = 0; j <= 2; j++)
        {
            k = j * 2;
            y = j - 1;
            y += b->unk2;
            if (y < 0 || gRoomHeight <= y)
            {
                for (i = 0; i <= 2; i++)
                    gUnk_0200B060[j * 3 + i] = 0;
            }
            else
            {
                for (i = 0; i <= 2; i++)
                {
                    x = i - 1;
                    x += b->unk0;
                    if (x < 0 || gRoomWidth <= x)
                        gUnk_0200B060[j + (k + i)] = 0;
                    else if ((&gRoomMap[x])[y * gRoomWidth].unk3 != 0)
                        gUnk_0200B060[j + (k + i)] = 1;
                    else
                        gUnk_0200B060[j + (k + i)] = 0;
                }
            }
        }
        n = 0;
        for (i = 0; i <= 1; i++)
        {
            for (j = 0; j <= 1; j++)
            {
                x = b->unk0 * 2 + j;
                y = b->unk2 * 2 + i;
                if (y >= 0 && y < gRoomHeight * 2 && gUnk_020055B8[0] <= x && x <= gUnk_020055B8[1])
                {
                    v = gUnk_0200B060[gUnk_0873A6D4[n][0]] + gUnk_0200B060[gUnk_0873A6D4[n][1]] * 2;
                    if (v == 0)
                        v = gUnk_0200B060[gUnk_0873A6D4[n][2]] * 4;
                    p = (u16 *)0x06002000 + ((x & 31) + ((y & 63) << 5));
                    *p = gUnk_080D71A0[v * 4 + n];
                }
                for (l = 0; l <= 2; l++)
                {
                    x = b->unk0 * 2 + j + gUnk_0873A734[gUnk_0873A6D4[n][l]][0];
                    y = b->unk2 * 2 + i + gUnk_0873A734[gUnk_0873A6D4[n][l]][1];

                    if (y >= 0 && gRoomHeight * 2 > y && x >= gUnk_020055B8[0] && gUnk_020055B8[1] >= x)
                    {
                        if ((&gRoomMap[x >> 1])[(y >> 1) * gRoomWidth].unk3 == 0)
                        {
                            v = gUnk_0200B060[gUnk_0873A6EC[n][l][0]] + gUnk_0200B060[gUnk_0873A6EC[n][l][1]] * 2;
                            if (v == 0)
                                v = gUnk_0200B060[gUnk_0873A6EC[n][l][2]] * 4;
                            p = (u16 *)0x06002000 + ((x & 31) + ((y & 63) << 5));
                            *p = gUnk_080D71A0[v * 4 + (x & 1) + (y & 1) * 2];
                        }
                    }
                }
                n++;
            }
        }
    }
    b->unk8++;
}

s32 sub_08032288(s32 x, s32 y)
{
    s32 x0, x1, y0, y1;

    gBlockCursorX = x;
    gBlockCursorY = y;
    if (gBlockCursorX >= gRoomWidth)
        return 0;
    if (gBlockCursorY >= gRoomHeight)
        return 0;
    x0 = (gCameraCenterX >> 20) - 9;
    x1 = (gCameraCenterX >> 20) + 10;
    y0 = (gCameraCenterY >> 20) - 7;
    y1 = (gCameraCenterY >> 20) + 7;
    if (x0 < 0)
        x0 = 0;
    if (x1 > gRoomWidth)
        x1 = gRoomWidth;
    if (y0 < 0)
        y0 = 0;
    if (y1 > gRoomHeight)
        y1 = gRoomHeight;
    if (gBlockCursorX < x0 || gBlockCursorX > x1 || gBlockCursorY < y0 || gBlockCursorY > y1)
        return 0;
    gBlockCursorIndex = gBlockCursorY * gRoomWidth + gBlockCursorX;
    if (gBg1MetatileMap[gBlockCursorIndex] != 0 && !(gBg1MetatileMap[gBlockCursorIndex] & 0x8000))
        return 1;
    return 0;
}

s16 sub_08032338(void)
{
    s32 i;
    struct Unk020061F0 *b;

    i = 0;
    while (gUnk_0200A6F0[i].unk6 != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gUnk_0200A6F0[i];
    b->unk4 = gBlockCursorIndex;
    b->unk8 = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1]->unk10 + 1;
    b->unk0 = gBlockCursorX;
    b->unk2 = gBlockCursorY;
    b->unkC = (u16 *)0x06001800 + (((gBlockCursorX * 2) & 31) + (((gBlockCursorY * 2) & 31) << 5));
    b->unk6 = 0;
    b->unk14 = 0;
    b->unk10 = gUnk_0873A458;
    gBg1MetatileMap[gBlockCursorIndex] |= 0x8000;
    PlaySfx(224);
    if (b->unk10[0] == 1)
        sub_08032500(b);
    return i;
}

void sub_08032428(void)
{
    s32 i;
    struct Unk020061F0 *b;
    u16 *p;

    sub_0802b368();
    for (i = 0; i < 64; i++)
    {
        b = &gUnk_0200A6F0[i];
        if (b->unk6 == 0x7FFF || (b->unk6 & 0x8000))
            continue;
    again:
        if ((s16)--b->unk14 > 0)
            continue;
        p = &b->unk10[b->unk6 * 2];
        switch (p[0])
        {
        case 1:
            sub_08032520(b);
            b->unk6++;
            goto again;
        case 2:
            sub_08032500(b);
            sub_08032520(b);
            b->unk6++;
            goto again;
        case 3:
            sub_080325b8(b);
            b->unk6++;
            goto again;
        case 4:
            b->unk14 = p[1];
            b->unk6++;
            continue;
        case 0x8000:
        default:
            sub_080324e4(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gUnk_0200A6F0[i].unk6 &= 0x7FFF;
}

void sub_080324e4(struct Unk020061F0 *b)
{
    b->unk6 = 0x7FFF;
    b->unk14 = 0;
    gBg1MetatileMap[b->unk4] = 0;
}

void sub_08032500(struct Unk020061F0 *b)
{
    b->unk16 = b->unk8->unk0;
    gBg1MetatileMap[b->unk4] = b->unk16 | 0x8000;
}

void sub_08032520(struct Unk020061F0 *b)
{
    s32 i, j;

    for (i = 0; i <= 1; i++)
    {
        s32 ty = b->unk2 * 2 + i;

        if (ty >= gUnk_020055B8[2] && ty <= gUnk_020055B8[3])
        {
            for (j = 0; j <= 1; j++)
            {
                s32 tx = b->unk0 * 2 + j;

                if (tx >= gUnk_020055B8[0] && tx <= gUnk_020055B8[1])
                    (&b->unkC[j])[i * 32] = gMetatileTiles[b->unk16 * 4 + j + i * 2];
            }
        }
    }
    b->unk8++;
}

void sub_080325b8(struct Unk020061F0 *b)
{
    s16 slot;

    if (sub_08032288(b->unk0, b->unk2 - 1) != 0)
    {
        slot = sub_08032338();
        if (slot != -1)
            gUnk_0200A6F0[slot].unk6 |= 0x8000;
    }
    if (sub_08032288(b->unk0 - 1, b->unk2) != 0)
    {
        slot = sub_08032338();
        if (slot != -1)
            gUnk_0200A6F0[slot].unk6 |= 0x8000;
    }
    if (sub_08032288(b->unk0 + 1, b->unk2) != 0)
    {
        slot = sub_08032338();
        if (slot != -1)
            gUnk_0200A6F0[slot].unk6 |= 0x8000;
    }
    if (sub_08032288(b->unk0, b->unk2 + 1) != 0)
    {
        slot = sub_08032338();
        if (slot != -1)
            gUnk_0200A6F0[slot].unk6 |= 0x8000;
    }
}
