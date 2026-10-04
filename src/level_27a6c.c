#include "gba/gba.h"
#include "global.h"

/* level_27a6c.c (0x08027A6C-0x08027E27, issue #93).
 *
 * sub_08027a6c builds the room's map in the second buffer gHubRoomMapBuffer for
 * the room loaders LoadHubRoom and LoadBigSwitchViewRoom (src/level_23948.c).  It
 * decompresses the room's metatile table (RoomDef +0x20) into gMetatileTiles
 * and its metatile map (+0x08) into gHubRoomMapBuffer (or CpuSet-copies it when
 * +0x05 says the map is stored raw), then walks the map from row 1 with the
 * per-cell marker table gUnk_0873240C[gCurLevel][cell]: a cell whose
 * marker names a flag that is still clear (a gBigSwitchFlags bit for markers
 * with bit 8 set, a gStageClearStatus[gCurLevel][] byte otherwise) takes the
 * matching cell of the next room's 2x2 pattern (gRoomTable[level][stage]
 * [room + 1]'s map).  With gHubUnlockFlags set it first clears the BG map at
 * 0x06001800 and gBg1MetatileMap, records up to two cells whose marker is
 * gHubUnlockSource | 0x80 in gHubUnlockBlocks (y then x; src/camtask_2d38c.c
 * reads them as a flat s16[4]) and copies the pattern cell of every
 * gHubUnlockSource marker into gBg1MetatileMap; otherwise it returns early when
 * gCurLevel < gFurthestLevel and gUnk_0873232C[gCurLevel]'s flags
 * are all set (or it has none).
 *
 * Matching notes (issue #93's final campaign, lesson 3.489): parked in #93 at
 * 234 differing bytes; two plain source facts closed it.  The marker is one
 * `u16 v`, so `v & 0x100` is an unsigned-short AND whose constant is a
 * two-insn HImode chain that agbcc's second loop pass hoists (lesson 3.465),
 * and the map store is the plain struct copy `gHubRoomMapBuffer[idx] = *p;`. */

struct MapCell
{
    /*0x00*/ u8 metatileLo;
    /*0x01*/ u8 metatileHi;
    /*0x02*/ u8 slopeIndex;
    /*0x03*/ u8 collisionTile;
};
struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 width;
    /*0x04*/ u16 height;
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
    /*0x04*/ s8 bgm;
    /*0x05*/ u8 mapsCompressed;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *metatileMap;
    /*0x0C*/ void *blockLayer;
    /*0x10*/ void *blockMetatiles;
    /*0x14*/ u16 width;
    /*0x16*/ u16 height;
    /*0x18*/ u16 *bg2Palette;
    /*0x1C*/ void *bg2Tiles;
    /*0x20*/ void *metatileTiles;
    /*0x24*/ u16 borderX;
    /*0x26*/ u16 borderY;
    /*0x28*/ u16 *bg3Palette;
    /*0x2C*/ void *bg3Tiles;
    /*0x30*/ struct BgMap *bg3Map;
    /*0x34*/ u16 bg3BorderX;
    /*0x36*/ u16 bg3BorderY;
    /*0x38*/ u16 driftObjectIndex;
    /*0x3A*/ u16 doorCount;
    /*0x3C*/ u16 objectCount;
    /*0x3E*/ u16 objectsSortedByY;
    /*0x40*/ u16 bgAnimSet;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *doors;
    /*0x48*/ void *objects;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 entryX;
    /*0x52*/ u16 entryY;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 bg3FullShake;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};
/* Not from room.h: this file's view of gHubUnlockBlocks differs (lesson 3.517). */
extern struct RoomDef *gCurRoomDef;
extern u16 gMetatileTiles[];
extern struct MapCell gHubRoomMapBuffer[];
extern s16 gRoomMetatileCount;
extern struct RoomDef **gRoomTable[][8];
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern u8 gHubUnlockFlags;
extern u16 gBg1MetatileMap[];
extern s16 gHubUnlockBlocks[][2];
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern s8 gCurLevel;
extern u16 *gUnk_0873240C[];
extern u16 gHubUnlockSource;
extern u32 gBigSwitchFlags[];
extern u8 gStageClearStatus[8][7];
extern u32 gUnk_0873232C[];
extern s8 gFurthestLevel;
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void sub_08027a6c(void)
{
    s32 y;
    s32 n;
    struct MapCell *alt;
    struct MapCell *p;
    s32 x, idx;
    u16 v;
    u32 mask;
    u32 a, b;

    n = 0;
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    if (gCurRoomDef->mapsCompressed != 0)
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gHubRoomMapBuffer, 0);
    else
        CpuSet(gCurRoomDef->metatileMap, gHubRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
    alt = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1]->metatileMap;
    if (gHubUnlockFlags != 0)
    {
        a = 0;
        CpuFastSet(&a, (u32 *)(BG_VRAM + 0x1800), 0x01000200);
        b = 0;
        CpuFastSet(&b, (u32 *)gBg1MetatileMap, ((gRoomMetatileCount / 2) & 0x1FFFFF) | 0x01000000);
        for (idx = 0; idx < 2; idx++)
        {
            gHubUnlockBlocks[idx][0] = -1;
            gHubUnlockBlocks[idx][1] = -1;
        }
        idx = gRoomWidth;
        for (y = 1; y < gRoomHeight; y++)
        {
            for (x = 0; x < gRoomWidth; x++)
            {
                if (y & 1)
                {
                    if (x & 1)
                        p = &alt[3];
                    else
                        p = &alt[2];
                }
                else
                {
                    if (x & 1)
                        p = &alt[1];
                    else
                        p = &alt[0];
                }
                v = gUnk_0873240C[gCurLevel][idx];
                if (v != 0)
                {
                    if (v == (gHubUnlockSource | 0x80) && n <= 1)
                    {
                        gHubUnlockBlocks[n][1] = y;
                        gHubUnlockBlocks[n][0] = x;
                        n++;
                    }
                    v &= 0xFF7F;
                    if (v & 0x100)
                    {
                        if (gHubUnlockSource == v)
                            gBg1MetatileMap[idx] = *(u16 *)p;
                        if (!(gBigSwitchFlags[0] & (1 << (v & 0xFF7F))))
                            gHubRoomMapBuffer[idx] = *p;
                    }
                    else
                    {
                        if (gHubUnlockSource == v)
                            gBg1MetatileMap[idx] = *(u16 *)p;
                        if (!gStageClearStatus[gCurLevel][v - 1])
                            gHubRoomMapBuffer[idx] = *p;
                    }
                }
                idx++;
            }
        }
    }
    else
    {
        mask = gUnk_0873232C[gCurLevel];
        if (gCurLevel < gFurthestLevel)
        {
            if (mask == 0)
                return;
            if ((gBigSwitchFlags[0] & mask) == mask)
                return;
        }
        idx = gRoomWidth;
        for (y = 1; y < gRoomHeight; y++)
        {
            for (x = 0; x < gRoomWidth; x++)
            {
                if (y & 1)
                {
                    if (x & 1)
                        p = &alt[3];
                    else
                        p = &alt[2];
                }
                else
                {
                    if (x & 1)
                        p = &alt[1];
                    else
                        p = &alt[0];
                }
                v = gUnk_0873240C[gCurLevel][idx];
                v &= 0xFF7F;
                if (v != 0)
                {
                    if (v & 0x100)
                    {
                        if (!(gBigSwitchFlags[0] & (1 << (v & 0xFF))))
                            gHubRoomMapBuffer[idx] = *p;
                    }
                    else
                    {
                        if (!gStageClearStatus[gCurLevel][v - 1])
                            gHubRoomMapBuffer[idx] = *p;
                    }
                }
                idx++;
            }
        }
    }
}
