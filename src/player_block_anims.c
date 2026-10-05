#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "player.h"

/* player_block_anims.c (0x080318B4-0x08032687, issue #92).
 *
 * Breakable blocks, part 2: the three per-frame stage hooks M08's
 * src/camera_world_sprite_block_anims.c stores into gBlockAnimHook, and their record helpers.
 * Each hook steps the animation script of every live record once a frame
 * (struct BreakingBlock: {op, arg} pairs - draw a frame, break the four
 * neighbours, wait, free) and then clears the "stepped" bit 15 of unk6.
 * UpdateBlockAnims draws the replacement metatiles into the BG map
 * (BlockAnimDrawColumn/BlockAnimWriteAndDrawColumn for a whole column of n blocks, through the
 * scratch record gBlockAnimScratchRecord) and chains to the neighbours
 * (BlockAnimBreakNeighbors); UpdateBlockAnimsWithEdges also rebuilds the 3x3 edge tiles around
 * the block (BlockAnimDrawWithEdges) for rooms whose BG map has edge tiles; and
 * UpdateBg1BlockAnims animates the blocks of the second layer gBg1MetatileMap in
 * the BG map at 0x06001800, with its own records gBg1BreakingBlocks[] and
 * the probe/spawner pair CanBreakBg1Block/BreakBg1BlockAtCursor that M08's map
 * events call.  FreeBlockAnimAndBlock/FreeBlockAnim/FreeBg1BlockAnimAndBlock free a record,
 * BlockAnimWriteMetatile/BlockAnimWriteMetatileWrapped write its metatile back into the map, and
 * BlockAnimDrawTiles/Bg1BlockAnimDrawTiles draw its 2x2 tiles inside the visible
 * window gBlockAnimClipRect. */

/* M08's view of a map cell (src/camera_draw_bg_map.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 slopeIndex;
    /*0x03*/ u8 collisionTile;
};

/* gBreakingBlocks[64] (and gBg1BreakingBlocks[64] for the second block layer
   gBg1MetatileMap): one 32-byte record per block being broken.  unk0/unk2 =
   metatile x/y, unk4 = its map index, unk6 = the position in the animation
   script (0x7FFF = free slot; bit 15 = already stepped this frame), unk8 =
   the metatile the cell shows next (RoomDef.blockMetatiles[] + the layer's low byte,
   advanced by one per drawn frame), unkC = its tile entry in the BG map,
   unk10 = the script (gBlockBreakScripts[kind], {op, arg} pairs: 1 and 2 draw a
   frame, 3 breaks the four neighbours, 4 waits arg frames, 0x8000/0x8001
   free the record), unk14 = the frames left to wait, unk16/unk18 = the
   metatile index and collision byte written back to the map, unk1A = the
   block kind, unk1C = the player that broke it (-1 = none). */
struct BreakingBlock
{
    /*0x00*/ u16 cellX;
    /*0x02*/ u16 cellY;
    /*0x04*/ u16 mapIndex;
    /*0x06*/ u16 scriptPos;
    /*0x08*/ struct MapTile *metatileCursor;
    /*0x0C*/ u16 *bgMapEntry;
    /*0x10*/ u16 *script;
    /*0x14*/ u16 waitFrames;
    /*0x16*/ u16 metatile;
    /*0x18*/ u16 collisionTile;
    /*0x1A*/ u16 chainAttack;
    /*0x1C*/ s8 breakerPlayer;
    /*0x1D*/ u8 filler1D[3];
};

/* M09's view of the room header (M07's struct RoomDef, src/room_*.c): the
   only field read here is unk10, the table of replacement metatiles the low
   byte of gBlockLayer[] indexes (M07 types it void *). */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ struct MapTile *blockMetatiles;
};

/* Not from room.h: this file's view of gRoomMap differs (lesson 3.517). */
extern struct BreakingBlock gBreakingBlocks[];
extern u16 gBlockLayer[];             /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern s16 gRoomHeight;               /* map height in metatiles */
extern s16 gRoomWidth;               /* map width in metatiles */
extern struct RoomDef *gCurRoomDef;   /* the current room header */
extern struct MapTile *gRoomMap;   /* the room's metatile map */
extern s16 gBlockAnimClipRect[4];
extern u16 gMetatileTiles[];
extern u16 gUnk_080D71A0[];
extern s32 gCameraCenterX;
extern s32 gCameraCenterY;
extern u16 gBg1MetatileMap[];
extern struct BreakingBlock gBg1BreakingBlocks[];
extern struct RoomDef **gRoomTable[][8];
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;

s32 PlaySfx(s32 id);
void WrapLoopingRoom(void);
void SetBlockAnimClipRect(void);
void SetBg1BlockAnimClipRect(void);
void SetBlockAnimClipRectWithEdges(void);

void UpdateBlockAnims(void)
{
    s32 i;
    struct BreakingBlock *b;
    u16 *p;

    SetBlockAnimClipRect();
    for (i = 0; i < 64; i++)
    {
        b = &gBreakingBlocks[i];
        if (b->scriptPos == 0x7FFF || (b->scriptPos & 0x8000))
            continue;
    again:
        if ((s16)--b->waitFrames > 0)
            continue;
        p = &b->script[b->scriptPos * 2];
        switch (p[0])
        {
        case 1:
            BlockAnimDrawColumn(b, (s16)p[1]);
            b->scriptPos++;
            goto again;
        case 2:
            BlockAnimWriteAndDrawColumn(b, (s16)p[1]);
            b->scriptPos++;
            goto again;
        case 3:
            BlockAnimBreakNeighbors(b);
            b->scriptPos++;
            goto again;
        case 4:
            b->waitFrames = p[1];
            b->scriptPos++;
            continue;
        case 0x8000:
        default:
            FreeBlockAnimAndBlock(b);
            continue;
        case 0x8001:
            FreeBlockAnim(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gBreakingBlocks[i].scriptPos &= 0x7FFF;
}

void FreeBlockAnimAndBlock(struct BreakingBlock *b)
{
    b->scriptPos = 0x7FFF;
    b->waitFrames = 0;
    gBlockLayer[b->mapIndex] = 0;
}

void FreeBlockAnim(struct BreakingBlock *b)
{
    b->scriptPos = 0x7FFF;
    b->waitFrames = 0;
    gBlockLayer[b->mapIndex] &= 0x7FFF;
}

void BlockAnimWriteAndDrawColumn(struct BreakingBlock *b, s32 n)
{
    s32 i;
    struct BreakingBlock *s;

    if (n <= 0)
        return;
    BlockAnimWriteMetatile(b);
    BlockAnimDrawTiles(b);
    if (n != 1)
    {
        s = &gBlockAnimScratchRecord;
        for (i = 1; i < n; i++)
        {
            s->cellX = b->cellX;
            s->cellY = b->cellY + i;
            if (s->cellY >= gRoomHeight)
                return;
            s->mapIndex = gRoomWidth * i + b->mapIndex;
            if (gBlockLayer[s->mapIndex] != 0)
            {
                s->metatileCursor = &gCurRoomDef->blockMetatiles[gBlockLayer[s->mapIndex] & 0xFF];
                s->bgMapEntry = (u16 *)(BG_VRAM + 0x2000) + (((s->cellX * 2) & 31) + ((((s->cellY * 2) & 31) + (u16)(s->cellX & 16) * 2) << 5));
                s->metatile = s->metatileCursor->metatile;
                s->collisionTile = s->metatileCursor->collisionTile;
                BlockAnimWriteMetatile(s);
                BlockAnimDrawTiles(s);
                gBlockLayer[s->mapIndex] = 0;
            }
        }
    }
    b->metatileCursor++;
}

void BlockAnimWriteColumn(struct BreakingBlock *b, s32 n)
{
    s32 i;
    struct BreakingBlock *s;

    if (n <= 0)
        return;
    BlockAnimWriteMetatile(b);
    if (n == 1)
        return;
    s = &gBlockAnimScratchRecord;
    for (i = 1; i < n; i++)
    {
        if (b->cellY + i >= gRoomHeight)
            return;
        s->mapIndex = gRoomWidth * i + b->mapIndex;
        if (gBlockLayer[s->mapIndex] != 0)
        {
            s->metatileCursor = &gCurRoomDef->blockMetatiles[gBlockLayer[s->mapIndex] & 0xFF];
            BlockAnimWriteMetatile(s);
            gBlockLayer[s->mapIndex] |= 0x8000;
        }
    }
}

void BlockAnimDrawColumn(struct BreakingBlock *b, s32 n)
{
    s32 i;
    struct BreakingBlock *s;

    if (n <= 0)
        return;

    BlockAnimDrawTiles(b);
    if (n != 1)
    {
        s = &gBlockAnimScratchRecord;
        for (i = 1; i < n; i++)
        {
            s->cellX = b->cellX;
            s->cellY = b->cellY + i;
            if (s->cellY >= gRoomHeight)
                return;
            s->mapIndex = gRoomWidth * i + b->mapIndex;
            if (gBlockLayer[s->mapIndex] != 0)
            {
                s->metatileCursor = &gCurRoomDef->blockMetatiles[gBlockLayer[s->mapIndex] & 0xFF] - 1;
                s->bgMapEntry = (u16 *)(BG_VRAM + 0x2000) + (((s->cellX * 2) & 31) + ((((s->cellY * 2) & 31) + (u16)(s->cellX & 16) * 2) << 5));
                s->metatile = s->metatileCursor->metatile;
                s->collisionTile = s->metatileCursor->collisionTile;

                BlockAnimDrawTiles(s);
                gBlockLayer[s->mapIndex] = 0;
            }
        }
    }
    b->metatileCursor++;
}

void BlockAnimWriteMetatile(struct BreakingBlock *b)
{
    b->metatile = b->metatileCursor->metatile;
    b->collisionTile = b->metatileCursor->collisionTile;
    gRoomMap[b->mapIndex].metatile = b->metatile;
    gRoomMap[b->mapIndex].collisionTile = b->collisionTile;
    gBlockLayer[b->mapIndex] = ((u8)gBlockLayer[b->mapIndex] + 1) | 0x8000;
}

void BlockAnimDrawTiles(struct BreakingBlock *b)
{
    s32 i, j;

    for (i = 0; i <= 1; i++)
    {
        s32 ty = b->cellY * 2 + i;

        if (ty >= gBlockAnimClipRect[2] && ty <= gBlockAnimClipRect[3])
        {
            for (j = 0; j <= 1; j++)
            {
                s32 tx = b->cellX * 2 + j;

                if (tx >= gBlockAnimClipRect[0] && tx <= gBlockAnimClipRect[1])
                    (&b->bgMapEntry[j])[i * 32] = gMetatileTiles[b->metatile * 4 + j + i * 2];
            }
        }
    }
}

void BlockAnimBreakNeighbors(struct BreakingBlock *b)
{
    s32 slot;

    if (CanBreakBlock(b->cellX, b->cellY - 1, b->chainAttack, b->breakerPlayer) != 0)
    {
        slot = BreakBlockAtCursor();
        if (slot != -1)
            gBreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBlock(b->cellX - 1, b->cellY, b->chainAttack, b->breakerPlayer) != 0)
    {
        slot = BreakBlockAtCursor();
        if (slot != -1)
            gBreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBlock(b->cellX + 1, b->cellY, b->chainAttack, b->breakerPlayer) != 0)
    {
        slot = BreakBlockAtCursor();
        if (slot != -1)
            gBreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBlock(b->cellX, b->cellY + 1, b->chainAttack, b->breakerPlayer) != 0)
    {
        slot = BreakBlockAtCursor();
        if (slot != -1)
            gBreakingBlocks[slot].scriptPos |= 0x8000;
    }
}

void UpdateBlockAnimsWithEdges(void)
{
    s32 i;
    struct BreakingBlock *b;
    u16 *p;

    SetBlockAnimClipRectWithEdges();
    for (i = 0; i < 64; i++)
    {
        b = &gBreakingBlocks[i];
        if (b->scriptPos == 0x7FFF || (b->scriptPos & 0x8000))
            continue;
    again:
        if ((s16)--b->waitFrames > 0)
            continue;
        p = &b->script[b->scriptPos * 2];
        switch (p[0])
        {
        case 2:
            BlockAnimWriteMetatileWrapped(b);
        case 1:
            BlockAnimDrawWithEdges(b);
            b->scriptPos++;
            goto again;
        case 3:
            b->scriptPos++;
            goto again;
        case 4:
            b->waitFrames = p[1];
            b->scriptPos++;
            continue;
        case 0x8000:
        default:
            FreeBlockAnimAndBlock(b);
            continue;
        case 0x8001:
            FreeBlockAnim(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gBreakingBlocks[i].scriptPos &= 0x7FFF;
    WrapLoopingRoom();
}

void BlockAnimWriteMetatileWrapped(struct BreakingBlock *b)
{
    u32 x;
    u32 m;

    b->metatile = b->metatileCursor->metatile;
    b->collisionTile = b->metatileCursor->collisionTile;
    gRoomMap[b->mapIndex].metatile = b->metatile;
    gRoomMap[b->mapIndex].collisionTile = b->collisionTile;
    x = b->cellX;
    if (x > 39)
    {
        m = 31;
        m &= x;
        (&gRoomMap[m])[b->cellY * gRoomWidth].metatile = b->metatile;
        (&gRoomMap[m])[b->cellY * gRoomWidth].collisionTile = b->collisionTile;
    }
    gBlockLayer[b->mapIndex] = ((u8)gBlockLayer[b->mapIndex] + 1) | 0x8000;
}

void BlockAnimDrawWithEdges(struct BreakingBlock *b)
{
    s32 j, i, l, n;
    s32 x, y, v;
    u16 *p;
    s32 k;

    if (b->collisionTile != 0)
    {
        for (i = 0; i <= 1; i++)
        {
            y = b->cellY * 2 + i;
            if (y >= gBlockAnimClipRect[2] && y <= gBlockAnimClipRect[3])
            {
                for (j = 0; j <= 1; j++)
                {
                    x = b->cellX * 2 + j;
                    if (x >= gBlockAnimClipRect[0] && x <= gBlockAnimClipRect[1])
                        (&b->bgMapEntry[j])[i * 32] = gMetatileTiles[b->metatile * 4 + j + i * 2];
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
            y += b->cellY;
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
                    x += b->cellX;
                    if (x < 0 || gRoomWidth <= x)
                        gUnk_0200B060[j + (k + i)] = 0;
                    else if ((&gRoomMap[x])[y * gRoomWidth].collisionTile != 0)
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
                x = b->cellX * 2 + j;
                y = b->cellY * 2 + i;
                if (y >= 0 && y < gRoomHeight * 2 && gBlockAnimClipRect[0] <= x && x <= gBlockAnimClipRect[1])
                {
                    v = gUnk_0200B060[gUnk_0873A6D4[n][0]] + gUnk_0200B060[gUnk_0873A6D4[n][1]] * 2;
                    if (v == 0)
                        v = gUnk_0200B060[gUnk_0873A6D4[n][2]] * 4;
                    p = (u16 *)(BG_VRAM + 0x2000) + ((x & 31) + ((y & 63) << 5));
                    *p = gUnk_080D71A0[v * 4 + n];
                }
                for (l = 0; l <= 2; l++)
                {
                    x = b->cellX * 2 + j + gUnk_0873A734[gUnk_0873A6D4[n][l]][0];
                    y = b->cellY * 2 + i + gUnk_0873A734[gUnk_0873A6D4[n][l]][1];

                    if (y >= 0 && gRoomHeight * 2 > y && x >= gBlockAnimClipRect[0] && gBlockAnimClipRect[1] >= x)
                    {
                        if ((&gRoomMap[x >> 1])[(y >> 1) * gRoomWidth].collisionTile == 0)
                        {
                            v = gUnk_0200B060[gUnk_0873A6EC[n][l][0]] + gUnk_0200B060[gUnk_0873A6EC[n][l][1]] * 2;
                            if (v == 0)
                                v = gUnk_0200B060[gUnk_0873A6EC[n][l][2]] * 4;
                            p = (u16 *)(BG_VRAM + 0x2000) + ((x & 31) + ((y & 63) << 5));
                            *p = gUnk_080D71A0[v * 4 + (x & 1) + (y & 1) * 2];
                        }
                    }
                }
                n++;
            }
        }
    }
    b->metatileCursor++;
}

s32 CanBreakBg1Block(s32 x, s32 y)
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

s16 BreakBg1BlockAtCursor(void)
{
    s32 i;
    struct BreakingBlock *b;

    i = 0;
    while (gBg1BreakingBlocks[i].scriptPos != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gBg1BreakingBlocks[i];
    b->mapIndex = gBlockCursorIndex;
    b->metatileCursor = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1]->blockMetatiles + 1;
    b->cellX = gBlockCursorX;
    b->cellY = gBlockCursorY;
    b->bgMapEntry = (u16 *)(BG_VRAM + 0x1800) + (((gBlockCursorX * 2) & 31) + (((gBlockCursorY * 2) & 31) << 5));
    b->scriptPos = 0;
    b->waitFrames = 0;
    b->script = gUnk_0873A458;
    gBg1MetatileMap[gBlockCursorIndex] |= 0x8000;
    PlaySfx(224);
    if (b->script[0] == 1)
        Bg1BlockAnimWriteMetatile(b);
    return i;
}

void UpdateBg1BlockAnims(void)
{
    s32 i;
    struct BreakingBlock *b;
    u16 *p;

    SetBg1BlockAnimClipRect();
    for (i = 0; i < 64; i++)
    {
        b = &gBg1BreakingBlocks[i];
        if (b->scriptPos == 0x7FFF || (b->scriptPos & 0x8000))
            continue;
    again:
        if ((s16)--b->waitFrames > 0)
            continue;
        p = &b->script[b->scriptPos * 2];
        switch (p[0])
        {
        case 1:
            Bg1BlockAnimDrawTiles(b);
            b->scriptPos++;
            goto again;
        case 2:
            Bg1BlockAnimWriteMetatile(b);
            Bg1BlockAnimDrawTiles(b);
            b->scriptPos++;
            goto again;
        case 3:
            Bg1BlockAnimBreakNeighbors(b);
            b->scriptPos++;
            goto again;
        case 4:
            b->waitFrames = p[1];
            b->scriptPos++;
            continue;
        case 0x8000:
        default:
            FreeBg1BlockAnimAndBlock(b);
            continue;
        }
    }
    for (i = 0; i < 64; i++)
        gBg1BreakingBlocks[i].scriptPos &= 0x7FFF;
}

void FreeBg1BlockAnimAndBlock(struct BreakingBlock *b)
{
    b->scriptPos = 0x7FFF;
    b->waitFrames = 0;
    gBg1MetatileMap[b->mapIndex] = 0;
}

void Bg1BlockAnimWriteMetatile(struct BreakingBlock *b)
{
    b->metatile = b->metatileCursor->metatile;
    gBg1MetatileMap[b->mapIndex] = b->metatile | 0x8000;
}

void Bg1BlockAnimDrawTiles(struct BreakingBlock *b)
{
    s32 i, j;

    for (i = 0; i <= 1; i++)
    {
        s32 ty = b->cellY * 2 + i;

        if (ty >= gBlockAnimClipRect[2] && ty <= gBlockAnimClipRect[3])
        {
            for (j = 0; j <= 1; j++)
            {
                s32 tx = b->cellX * 2 + j;

                if (tx >= gBlockAnimClipRect[0] && tx <= gBlockAnimClipRect[1])
                    (&b->bgMapEntry[j])[i * 32] = gMetatileTiles[b->metatile * 4 + j + i * 2];
            }
        }
    }
    b->metatileCursor++;
}

void Bg1BlockAnimBreakNeighbors(struct BreakingBlock *b)
{
    s16 slot;

    if (CanBreakBg1Block(b->cellX, b->cellY - 1) != 0)
    {
        slot = BreakBg1BlockAtCursor();
        if (slot != -1)
            gBg1BreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBg1Block(b->cellX - 1, b->cellY) != 0)
    {
        slot = BreakBg1BlockAtCursor();
        if (slot != -1)
            gBg1BreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBg1Block(b->cellX + 1, b->cellY) != 0)
    {
        slot = BreakBg1BlockAtCursor();
        if (slot != -1)
            gBg1BreakingBlocks[slot].scriptPos |= 0x8000;
    }
    if (CanBreakBg1Block(b->cellX, b->cellY + 1) != 0)
    {
        slot = BreakBg1BlockAtCursor();
        if (slot != -1)
            gBg1BreakingBlocks[slot].scriptPos |= 0x8000;
    }
}
