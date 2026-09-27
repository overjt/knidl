#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_5acc.c (0x08005ACC-0x08005C4B, issue #63).
 *
 * Three task helpers of the task system (src/early_58e4.c):
 *   TaskIsOnScreen  is the running task's position (Task.pixelX/unk4A) on screen,
 *                 relative to the camera gSpriteCameraX/gSpriteCameraY, with a
 *                 63-pixel margin on every side?
 *   TaskLoadFrameTilesAndPalette  upload the running task's tile stream (its graphics
 *                 descriptor Task.frameTable[Task.frame], struct TaskGfx) to OBJ VRAM
 *                 at tile Task.tileWord & 0x7FF (or to the 0x0600FE00 bank when
 *                 `alt` is set), then its palette to palette-buffer bank
 *                 Task.tileWord >> 12; returns the descriptor's first word.  A dead
 *                 export: nothing calls it.
 *   TaskLoadFrameTiles  the same without the palette (called by src/early_5d9c.c).
 * The tile stream is a list of (size, data) chunks ended by 0xFFFF, one
 * 1 KiB VRAM block per chunk.
 *
 * Matching notes (issue #63): the chunk walk is `p = (u16 *)((u8 *)q + *p)`
 * and the descriptor is read as `tbl = t->unk38; tbl[t->unk3C]` (table first,
 * lesson 3.79).  Issue #32 had declared both upload functions unreachable
 * (lesson 3.73); plain C matches them. */

extern u16 gSpriteCameraX;
extern u16 gSpriteCameraY;
extern u8 gObjVram[];
extern u16 gObjPalette[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

/* Is the running task on screen (with a 63-pixel margin on every side)
 * relative to the camera gSpriteCameraX/gSpriteCameraY? */
u32 TaskIsOnScreen(void)
{
    s16 x;
    s16 y;
    u16 t;

    x = gCurTask->pixelX - gSpriteCameraX;
    y = gCurTask->pixelY - gSpriteCameraY;
    t = x + 63;
    if (t > 366)
        return 0;
    if (y <= -64)
        return 0;
    if (y > 223)
        return 0;
    return 1;
}

/* Hidden (unreferenced) export: upload the running task's tile stream plus
 * its palette. */
u32 TaskLoadFrameTilesAndPalette(u32 alt)
{
    u16 attr;
    u32 dst;
    u32 *tbl;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u16 *pal;

    attr = gCurTask->tileWord;
    if (alt == 0)
        dst = (attr & 0x7FF) * 32 + (u32)gObjVram;
    else
        dst = (attr & 0x7FF) * 32 + 0x0600FE00;
    tbl = gCurTask->frameTable;
    g = (struct TaskGfx *)tbl[gCurTask->frame];
    p = g->tiles;
    while (*p != 0xFFFF)
    {
        q = p + 1;
        RequestCopy(3, (u32)q, dst, *p);
        p = (u16 *)((u8 *)q + *p);
        dst += 0x400;
    }
    pal = g->palette;
    RequestCopy(2, (u32)(pal + 1), (attr >> 12) * 32 + (u32)gObjPalette, *pal);
    return g->oamTemplate;
}

/* Upload the running task's tile stream. */
u32 TaskLoadFrameTiles(u32 alt)
{
    u16 attr;
    u32 dst;
    u32 *tbl;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;

    attr = gCurTask->tileWord;
    if (alt == 0)
        dst = (attr & 0x7FF) * 32 + (u32)gObjVram;
    else
        dst = (attr & 0x7FF) * 32 + 0x0600FE00;
    tbl = gCurTask->frameTable;
    g = (struct TaskGfx *)tbl[gCurTask->frame];
    p = g->tiles;
    while (*p != 0xFFFF)
    {
        q = p + 1;
        RequestCopy(3, (u32)q, dst, *p);
        p = (u16 *)((u8 *)q + *p);
        dst += 0x400;
    }
    return g->oamTemplate;
}
