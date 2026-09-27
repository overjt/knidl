#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_1a3e4.c (0x0801A3E4-0x0801A76B, issue #125).
 *
 * Draw callback of the ending's big scripted sprites, installed by M05's
 * scripts sub_08019b30/sub_08019c44/sub_08019d30 and by variants 6-8 of task
 * type #101 (src/ending_c9004.c).  It copies the current animation frame's
 * tiles from the decompressed sheet in gUnk_02020000 to the task's OBJ tiles
 * (Task.unk40 & 0xFFF, less 16) and draws the frame at the task's position
 * less the BG3 scroll, choosing the layout by the task's animation table
 * Task.unk38: gUnk_08755440 is a body plus four side pieces at -64/+64 and
 * -96/+96 pixels (frames gUnk_087321C0/gUnk_087321EC), gUnk_0875546C and
 * gUnk_08755484 one sprite each (gUnk_085E24D8, gUnk_085E26E8).
 *
 * Matching note (issue #125): the second layout's attribute bits are a u16
 * variable; its OR is `movs r3, #128; lsls r3, #4; orrs r3, r4` where the
 * literal `0x800 | Task.unk40` of the other two needs a reload register and a
 * HImode copy, and that one missing copy is what keeps the ROM's cross-jump
 * to four instructions (lesson 3.475). */

extern u32 gUnk_08755440[];
extern u32 gUnk_0875546C[];
extern u32 gUnk_08755484[];
extern u32 gUnk_02020000[];
extern vs32 gUnk_03000B78; /* BG3HOFS shadow (16.16) */
extern vs32 gUnk_03000FA8; /* BG3VOFS shadow (16.16) */
extern s16 gUnk_08732190[];
extern s16 gUnk_087321A6[];
extern s16 gUnk_087321B2[];
extern u32 gUnk_087321C0[];
extern u32 gUnk_087321EC[];
extern u32 gUnk_085E24D8[];
extern u32 gUnk_085E26E8[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
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

    t = gUnk_03002490;
    tbl = t->unk38;
    if (tbl == NULL)
        return;
    if (t->unk3C == -1)
        return;
    base = ((t->unk40 - 16) & 0xFFF) << 5;
    dst = 0x06010000 + base;
    dx = t->unk48 - (gUnk_03000B78 >> 16);
    dy = t->unk4A - (gUnk_03000FA8 >> 16);
    if (tbl == gUnk_08755440)
    {
        RequestCopy(4, (u32)gUnk_02020000 + (gUnk_08732190[t->unk3C] << 5), dst,
                     128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_08732190[gUnk_03002490->unk3C] + 16) << 5),
                     0x06010400 + base, 224 << 1);
        xb = dx;
        x = xb - 64;
        yb = dy;
        if (IsOnScreen(x, yb) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, gUnk_087321C0[u->unk3C], u->unk3E,
                         0x800 | u->unk40, x, yb);
        }
        x2 = xb + 64;
        if (IsOnScreen(x2, yb) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, gUnk_087321C0[u->unk3C], u->unk3E,
                         0x800 | u->unk40, x2, yb);
        }
        x3 = xb - 96;
        if (IsOnScreen(x3, yb - 7) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, gUnk_087321EC[u->unk3C], u->unk3E,
                         0x800 | u->unk40, x3, yb - 7);
        }
        x4 = xb + 96;
        if (IsOnScreen(x4, yb - 7) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, gUnk_087321EC[u->unk3C], u->unk3E,
                         0x800 | u->unk40, x4, yb - 7);
        }
    }
    else if (tbl == gUnk_0875546C)
    {
        RequestCopy(4, (u32)gUnk_02020000 + ((gUnk_087321A6[t->unk3C] + 14) << 5),
                     0x060105C0 + base, 64);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321A6[gUnk_03002490->unk3C] + 16) << 5),
                     0x06010800 + base, 128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321A6[gUnk_03002490->unk3C] + 32) << 5),
                     0x06010C00 + base, 64);
        xb = dx;
        yb = dy;
        /* this arm's attribute bits are a u16 variable: the ROM builds its
           OR as `movs r3, #128; lsls r3, #4; orrs r3, r4`, where the literal
           `0x800 | u->unk40` of the other arms needs a reload register and a
           HImode copy (lesson 3.475) */
        c = 0x800;
        if (IsOnScreen(xb, yb) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, (u32)gUnk_085E24D8, u->unk3E,
                         c | u->unk40, xb, yb);
        }
    }
    else if (tbl == gUnk_08755484)
    {
        RequestCopy(4, (u32)gUnk_02020000 + ((gUnk_087321B2[t->unk3C] + 2) << 5),
                     0x06010C40 + base, 224 << 1);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321B2[gUnk_03002490->unk3C] + 16) << 5),
                     0x06011000 + base, 128 << 2);
        RequestCopy(4, (u32)gUnk_02020000
                        + ((gUnk_087321B2[gUnk_03002490->unk3C] + 32) << 5),
                     0x06011400 + base, 128 << 2);
        xb = dx;
        yb = dy;
        if (IsOnScreen(xb, yb) != 0)
        {
            u = gUnk_03002490;
            QueueSprite(u->unk42, (u32)gUnk_085E26E8, u->unk3E,
                         0x800 | u->unk40, xb, yb);
        }
    }
}
