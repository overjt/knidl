#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_1cc8.c (0x08001CC8-0x08001FCF, issue #63).
 *
 * Affine sprite emitter, called by the actors, enemies, effects, HUD, menus,
 * sub-games and ending scenes that draw a rotated or scaled sprite.  The OAM
 * template stream at p (4 halfwords per entry, the last one flagged by bit 12
 * of attr0) is copied into the affine staging buffer gUnk_03001190 at
 * gUnk_03001A80 with every entry's position scaled by 256/sx and 256/sy
 * around the sprite's centre (the half sizes come from the shape/size table
 * gUnk_0872EB14[shape][size]), double-size unless both scales are at least
 * 256, and pointed at affine slot gUnk_03000B1C; the slot's matrix (sx, sy
 * and the rotation rot through the trig table gUnk_0872FB30) goes into the
 * OAM shadow gUnk_03000050.  Returns the address of the first staged entry.
 *
 * The definition's parameters are (u16 *, s16, s16, s16): the ROM truncates
 * all three at entry and sign-extends them at every use (lesson 3.27).  The
 * callers in src/ declare `s32 sub_08001cc8(s32, s16, s16, s32)`; those
 * declarations only decide how a caller extends its arguments, the callers
 * match their own ROM bytes as written, and they were left alone (lesson
 * 3.428).
 *
 * Matching notes (issue #63): a goto loop (lesson 3.21), in-place arithmetic
 * on v (`v >>= 8; v -= h; v &= 0xFF;`), a real 3-D size table, and the attr0
 * store as `(s16)dbl | (s16)(...)`; lesson 3.479.  Issue #32 had called the
 * last 8 bytes an unreachable regmove tie (lesson 3.35). */

extern vs16 gUnk_03001190[];       /* affine OBJ staging buffer */
extern vu16 gUnk_03001A80;         /* staging buffer write index */
extern vu16 gUnk_03000050[];       /* OAM shadow (attrs + affine params) */
extern vu16 gUnk_03000B1C;         /* affine matrix index */
extern const u8 gUnk_0872EB14[4][4][2];   /* [shape][size] -> {w,h} */
extern s16 gUnk_0872FB30[];  /* trig table (mid pointer) */

/* Emit an affine sprite: copy the OAM template stream at `p` into the
 * staging buffer with every entry scaled by 256/sx, 256/sy around its
 * centre (double-size unless both scales are >= 256), then write the
 * rotation/scale matrix for `rot` into affine slot gUnk_03000B1C.  Returns
 * the address of the first staged entry. */
s32 sub_08001cc8(u16 *p, s16 sx, s16 sy, s16 rot)
{
    vs16 *first;
    u16 a0;
    u16 a1;
    s32 w;
    s32 h;
    u16 dbl;
    s32 inv;
    s32 v;
    s32 half;

    first = &gUnk_03001190[(s16)gUnk_03001A80];
    /* A goto loop, not do/while: the ROM re-loads the pool words and
     * re-extends sx/sy every pass, which loop.c would hoist (lesson 3.21). */
loop:
    {
        a0 = *p++;
        a1 = *p++;
        w = gUnk_0872EB14[a0 >> 14][a1 >> 14][0];
        h = gUnk_0872EB14[a0 >> 14][a1 >> 14][1];
        if ((sx & 0xFF00) && (sy & 0xFF00))
            dbl = 0;
        else
            dbl = 0x200;

        inv = 0;
        if (sy != 0)
            inv = Div(0x1000000, abs(sy));
        inv = (u16)(inv >> 8);
        v = a0 & 0xFF;
        if (v & 0x80)
            v |= 0xFFFFFF00;
        if (dbl)
        {
            h = h - (((h >> 1) * inv) >> 8);
        }
        else
        {
            half = h >> 1;
            h = h - ((half * inv) >> 8) - half;
        }
        v *= inv;
        if (v < 0)
            v += 128;
        else
            v -= 128;
        v >>= 8;
        v -= h;
        v &= 0xFF;
        /* Both (s16) casts are in the ROM: the sign-extension of dbl
         * survives only this spelling (u16 dbl is what tests it without one). */
        gUnk_03001190[(s16)gUnk_03001A80++] = (s16)dbl | (s16)((a0 & 0xFF00) | v | 0x100);

        inv = 0;
        if (sx != 0)
            inv = Div(0x1000000, abs(sx));
        inv = (u16)(inv >> 8);
        v = a1 & 0x1FF;
        if (v & 0x100)
            v |= 0xFFFFFF00;
        if (dbl)
        {
            w = w - (((w >> 1) * inv) >> 8);
        }
        else
        {
            half = w >> 1;
            w = w - ((half * inv) >> 8) - half;
        }
        v *= inv;
        if (v < 0)
            v += 128;
        else
            v -= 128;
        v >>= 8;
        v -= w;
        v &= 0x1FF;
        gUnk_03001190[(s16)gUnk_03001A80++] = (a1 & 0xC000) | v | (gUnk_03000B1C << 9);
        gUnk_03001190[(s16)gUnk_03001A80++] = 0;
        p++;
        gUnk_03001190[(s16)gUnk_03001A80++] = *p++;
    }
    if (!(a0 & 0x1000))
        goto loop;

    if (rot != 0)
    {
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 3] = (sx * *(gUnk_0872FB30 + rot)) >> 8;
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 7] = -((sx * *(gUnk_0872FB30 - 128 + rot)) >> 8);
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 11] = (sy * *(gUnk_0872FB30 - 128 + rot)) >> 8;
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 15] = (sy * *(gUnk_0872FB30 + rot)) >> 8;
    }
    else
    {
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 3] = sx;
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 7] = 0;
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 11] = 0;
        gUnk_03000050[((s16)gUnk_03000B1C << 4) + 15] = sy;
    }
    gUnk_03000B1C++;
    return (s32)first;
}
