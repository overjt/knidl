#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "hud.h"
#include "room.h"

/* hud_0b318.c (0x0800B318-0x0800B44B, issue #96).
 *
 * The HUD tilemap buffer gHudTilemap (32 tiles per row): tile copy and
 * clear at (x, y), whole-buffer clears, and the flush to 0x06001000 when
 * the dirty flag gHudTilemapDirty is set. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void LoadGfxSet(u16 a0);

/* Copy n tiles from src into the HUD tilemap buffer at column x, row y.
 * The walking `pos` (not `pos + i`) is what keeps src incremented in place
 * and after the destination. */
void HudDrawTiles(u16 *src, s32 x, s32 y, s32 n)
{
    s32 i;
    s32 pos = x + (y << 5);

    for (i = 0; i < n; i++) {
        gHudTilemap[pos] = *src;
        pos++;
        src++;
    }
    gHudTilemapDirty = 1;
}

/* Clear n tiles of the HUD tilemap buffer at column x, row y. */
void HudClearTiles(s32 x, s32 y, s32 n)
{
    s32 i;
    s32 pos = x + (y << 5);

    for (i = 0; i < n; i++) {
        gHudTilemap[pos] = 0;
        pos++;
    }
    gHudTilemapDirty = 1;
}

/* The fill source must be volatile: a plain u16 merges the ROM's two
 * `mov rX, sp` into one. */
void HudClearWholeTilemap(void)
{
    vu16 zero = 0;

    CpuSet((void *)&zero, gHudTilemap, 0x01000400);
    gHudTilemapDirty = 1;
}

void HudClearTilemap(void)
{
    vu16 zero;

    if (gUnk_03002444 != 0) {
        zero = 0;
        CpuSet((void *)&zero, &gHudTilemap[64], 0x010003C0);
    } else {
        zero = 0;
        CpuSet((void *)&zero, gHudTilemap, 0x01000400);
    }
    gHudTilemapDirty = 1;
}

/* Flush the HUD tilemap buffer to VRAM when it is dirty. */
void HudFlushTilemap(void)
{
    if (gHudTilemapDirty != 0) {
        RequestCopy(1, (u32)gHudTilemap, 0x06001000, 0x800);
        gHudTilemapDirty = 0;
    }
}

void HudLoadGfx(void)
{
    if (gUnk_03001F30 == 0)
        LoadGfxSet(4);
    else
        LoadGfxSet(5);
}
