#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_5acc.c (0x08005ACC-0x08005C4B, issue #63).
 *
 * Three task helpers of the task system (src/early_58e4.c):
 *   sub_08005acc  is the running task's position (Task.unk48/unk4A) on screen,
 *                 relative to the camera gUnk_03002348/gUnk_030023E4, with a
 *                 63-pixel margin on every side?
 *   sub_08005b20  upload the running task's tile stream (its graphics
 *                 descriptor Task.unk38[Task.unk3C], struct TaskGfx) to OBJ VRAM
 *                 at tile Task.unk40 & 0x7FF (or to the 0x0600FE00 bank when
 *                 `alt` is set), then its palette to palette-buffer bank
 *                 Task.unk40 >> 12; returns the descriptor's first word.  A dead
 *                 export: nothing calls it.
 *   sub_08005bc4  the same without the palette (called by src/early_5d9c.c).
 * The tile stream is a list of (size, data) chunks ended by 0xFFFF, one
 * 1 KiB VRAM block per chunk.
 *
 * Matching notes (issue #63): the chunk walk is `p = (u16 *)((u8 *)q + *p)`
 * and the descriptor is read as `tbl = t->unk38; tbl[t->unk3C]` (table first,
 * lesson 3.79).  Issue #32 had declared both upload functions unreachable
 * (lesson 3.73); plain C matches them. */

extern u16 gUnk_03002348;
extern u16 gUnk_030023E4;
extern u8 gUnk_06010000[];
extern u16 gUnk_03001470[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);

/* Is the running task on screen (with a 63-pixel margin on every side)
 * relative to the camera gUnk_03002348/gUnk_030023E4? */
u32 sub_08005acc(void)
{
    s16 x;
    s16 y;
    u16 t;

    x = gUnk_03002490->unk48 - gUnk_03002348;
    y = gUnk_03002490->unk4A - gUnk_030023E4;
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
u32 sub_08005b20(u32 alt)
{
    u16 attr;
    u32 dst;
    u32 *tbl;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u16 *pal;

    attr = gUnk_03002490->unk40;
    if (alt == 0)
        dst = (attr & 0x7FF) * 32 + (u32)gUnk_06010000;
    else
        dst = (attr & 0x7FF) * 32 + 0x0600FE00;
    tbl = gUnk_03002490->unk38;
    g = (struct TaskGfx *)tbl[gUnk_03002490->unk3C];
    p = g->unk08;
    while (*p != 0xFFFF)
    {
        q = p + 1;
        sub_080017e4(3, (u32)q, dst, *p);
        p = (u16 *)((u8 *)q + *p);
        dst += 0x400;
    }
    pal = g->unk04;
    sub_080017e4(2, (u32)(pal + 1), (attr >> 12) * 32 + (u32)gUnk_03001470, *pal);
    return g->unk00;
}

/* Upload the running task's tile stream. */
u32 sub_08005bc4(u32 alt)
{
    u16 attr;
    u32 dst;
    u32 *tbl;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;

    attr = gUnk_03002490->unk40;
    if (alt == 0)
        dst = (attr & 0x7FF) * 32 + (u32)gUnk_06010000;
    else
        dst = (attr & 0x7FF) * 32 + 0x0600FE00;
    tbl = gUnk_03002490->unk38;
    g = (struct TaskGfx *)tbl[gUnk_03002490->unk3C];
    p = g->unk08;
    while (*p != 0xFFFF)
    {
        q = p + 1;
        sub_080017e4(3, (u32)q, dst, *p);
        p = (u16 *)((u8 *)q + *p);
        dst += 0x400;
    }
    return g->unk00;
}
