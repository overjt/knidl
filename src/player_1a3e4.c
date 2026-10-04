#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"

/* player_1a3e4.c (0x0801A3E4-0x0801A76B, issue #125).
 *
 * Draw callback of the ending's big scripted sprites, installed by M05's
 * scripts CutsceneActorScript58/CutsceneActorScript59/CutsceneActorScript60 and by variants 6-8 of task
 * type #101 (src/ending_c9004.c).  It copies the current animation frame's
 * tiles from the decompressed sheet in gUnk_02020000 to the task's OBJ tiles
 * (Task.tileWord & 0xFFF, less 16) and draws the frame at the task's position
 * less the BG3 scroll, choosing the layout by the task's animation table
 * Task.frameTable: gUnk_08755440 is a body plus four side pieces at -64/+64 and
 * -96/+96 pixels (frames gUnk_087321C0/gUnk_087321EC), gUnk_0875546C and
 * gUnk_08755484 one sprite each (gUnk_085E24D8, gUnk_085E26E8).
 *
 * Matching note (issue #125): the second layout's attribute bits are a u16
 * variable; its OR is `movs r3, #128; lsls r3, #4; orrs r3, r4` where the
 * literal `0x800 | Task.tileWord` of the other two needs a reload register and a
 * HImode copy, and that one missing copy is what keeps the ROM's cross-jump
 * to four instructions (lesson 3.475). */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s16 f);
s32 IsOnScreen(s16 x, s16 y);

void sub_0801a3e4(void)
{
    struct Task *t;
    struct Task *u;
    u32 *tbl;
    u32 base;
    u32 dst;
    u16 dx;
    u16 dy;
    s16 xb;
    s16 yb;
    s32 x;
    s32 x2;
    s32 x3;
    s32 x4;
    u16 c;

    t = gCurTask;
    tbl = t->frameTable;
    if (tbl == NULL)
        return;
    if (t->frame == -1)
        return;
    base = ((t->tileWord - 16) & 0xFFF) << 5;
    dst = OBJ_VRAM0 + base;
    dx = t->pixelX - (gBg3ScrollX >> 16);
    dy = t->pixelY - (gBg3ScrollY >> 16);
    if (tbl == gUnk_08755440)
    {
        RequestCopy(4, (u32)gUnk_02020000 + (gUnk_08732190[t->frame] << 5), dst,
                     128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_08732190[gCurTask->frame] + 16) << 5),
                     (OBJ_VRAM0 + 0x400) + base, 224 << 1);
        xb = dx;
        x = xb - 64;
        yb = dy;
        if (IsOnScreen(x, yb) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_087321C0[u->frame], u->spriteFlags,
                         0x800 | u->tileWord, x, yb);
        }
        x2 = xb + 64;
        if (IsOnScreen(x2, yb) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_087321C0[u->frame], u->spriteFlags,
                         0x800 | u->tileWord, x2, yb);
        }
        x3 = xb - 96;
        if (IsOnScreen(x3, yb - 7) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_087321EC[u->frame], u->spriteFlags,
                         0x800 | u->tileWord, x3, yb - 7);
        }
        x4 = xb + 96;
        if (IsOnScreen(x4, yb - 7) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_087321EC[u->frame], u->spriteFlags,
                         0x800 | u->tileWord, x4, yb - 7);
        }
    }
    else if (tbl == gUnk_0875546C)
    {
        RequestCopy(4, (u32)gUnk_02020000 + ((gUnk_087321A6[t->frame] + 14) << 5),
                     (OBJ_VRAM0 + 0x5C0) + base, 64);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321A6[gCurTask->frame] + 16) << 5),
                     (OBJ_VRAM0 + 0x800) + base, 128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321A6[gCurTask->frame] + 32) << 5),
                     (OBJ_VRAM0 + 0xC00) + base, 64);
        xb = dx;
        yb = dy;
        /* this arm's attribute bits are a u16 variable: the ROM builds its
           OR as `movs r3, #128; lsls r3, #4; orrs r3, r4`, where the literal
           `0x800 | u->unk40` of the other arms needs a reload register and a
           HImode copy (lesson 3.475) */
        c = 0x800;
        if (IsOnScreen(xb, yb) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, (u32)gUnk_085E24D8, u->spriteFlags,
                         c | u->tileWord, xb, yb);
        }
    }
    else if (tbl == gUnk_08755484)
    {
        RequestCopy(4, (u32)gUnk_02020000 + ((gUnk_087321B2[t->frame] + 2) << 5),
                     (OBJ_VRAM0 + 0xC40) + base, 224 << 1);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321B2[gCurTask->frame] + 16) << 5),
                     (OBJ_VRAM0 + 0x1000) + base, 128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321B2[gCurTask->frame] + 32) << 5),
                     (OBJ_VRAM0 + 0x1400) + base, 128 << 2);
        xb = dx;
        yb = dy;
        if (IsOnScreen(xb, yb) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, (u32)gUnk_085E26E8, u->spriteFlags,
                         0x800 | u->tileWord, xb, yb);
        }
    }
}
