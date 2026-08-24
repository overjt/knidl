#include "gba/gba.h"
#include "global.h"

/* Early subsystem: link-play frame driver, wait helpers, RNG, decimal digit
 * split and 15bpp colour blending (0x08002B04-0x0800310F, issue #32 batch C2).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaf sub_08002e38 ends in a bare `bx lr`; agbcc unconditionally emits
 * `push {lr}` / `pop {r0}; bx r0` even for leaves.
 *
 * Matching notes (see docs/lessons-learned.md §3):
 *  - The switch dispatch trees in sub_08002b04 / sub_08002b8c are NOT what
 *    gcc's balance_case_nodes produces for the same case values (it puts the
 *    middle case at the root).  The ROM's tree is "root = lowest case, empty
 *    left subtree", which only comes out of an explicit
 *    `if (t != K) { if (t > K) {...} } else {...}` nest — that also puts the
 *    K body last, exactly where the ROM has it.
 *  - gUnk_03004D50 must be a 2-D array: `g[1][i]` materialises the row base
 *    (`adds r0,r6,#0; adds r0,#8; adds r0,r5,r0`) while a flat `g[i + 4]`
 *    folds into the walking pointer as `[r4, #8]`.  The 2-D form is also what
 *    keeps the base register live, which is what pushes 0x8800 into r8.
 *  - gUnk_03000F98[] / gUnk_03001EB8[] are `vu16` arrays: the ROM's dead
 *    pre-read `ldrh` before every store is the volatile *indexed* store idiom
 *    (§3.7), and `gUnk_03000F98[i] = gUnk_03001EB8[i] = 0` is the chained
 *    assignment idiom (§3.8) — outer address materialised first, inner cell
 *    re-read for the outer store.
 *  - sub_08002e38's first parameter read must be volatile (`*(vu16 *)p`);
 *    without it the address/temporary pseudos swap r1<->r2.  The remaining
 *    reads p[1..4] are plain (a volatile pointer re-reads them).
 *  - sub_08002f14 keeps the digit buffer in a function-scope `u8 *b` that
 *    every branch assigns: that is what pins it to r4 in the zero branch too.
 *    `zero`/`e` in the zero branch reproduce the ROM's preheader order
 *    (base, bound copy, zero, base+5) and its signed pointer compare.
 *  - sub_08003014's ratio/count parameters are 32-bit; the u16 truncations
 *    belong to the LOCALS (`u16 r = ratio; u16 n = count; n--;`), which is
 *    what places `ldr r7,[sp,#32]` before them and keeps ratio in r6.
 *  - sub_080030b8 is a dead export hidden inside symbols.csv's 0xFC size for
 *    sub_08003014 (lesson 2.13 / zone lesson 14): nothing in ROM calls it.
 */

extern vu16 gUnk_0300243C;      /* number of linked players */
extern u32 gUnk_03004D30;
extern vu16 gUnk_03005274;      /* link session state, high byte = command */
extern u16 gUnk_03004D90[4];    /* link send buffer */
extern vu16 gUnk_03001EF4;      /* keys held last frame */
extern vu16 gUnk_03000038;      /* keys newly pressed */
extern u8 gUnk_03004D40;

extern vu16 gUnk_03000F98[];    /* per-player keys held */
extern vu16 gUnk_03001EB8[];    /* per-player keys pressed */
extern u16 gUnk_03004D88[];
extern u16 gUnk_03004D50[3][4]; /* [0]=state [1]=keys held [2]=keys pressed */
extern u8 gUnk_03004DA0[];
extern u32 gUnk_03004D28;
extern vu16 gUnk_03001EC4;      /* VBlank wait flag */
extern u32 gUnk_03004D7C;       /* frame counter */
extern u32 gUnk_03004D2C;

extern vu16 gUnk_03001E90;      /* frames left to wait */
extern u16 gUnk_03000048;
extern vu16 gUnk_03001ED8;      /* display/mode flags */
extern u16 gUnk_03001188;
extern u16 gUnk_03000B14;
extern u16 gUnk_03000B10;
extern u16 gUnk_03001EB4;
extern vu32 gUnk_03000FB4;      /* RNG state */
extern u8 gUnk_03001F08[6];     /* decimal digit buffer, [5] = sign/flag */

void sub_080b84f0(void);
void sub_080b8694(void);
void sub_08006914(u16 *a, u16 *b, u16 *c);
void sub_08008b8c(void);
void sub_08005228(void);
void sub_08001a84(void);
void sub_08000de4(void);
void sub_08001a34(void);
u32 sub_080072e0(void);
void sub_080022e4(void);
void sub_080022d0(void);

void sub_08002b04(void)
{
    s32 t;

    if (gUnk_0300243C > 1 && gUnk_03004D30 == 0) {
        t = gUnk_03005274 & 0xFF00;
        if (t != 0x6600) {
            if (t > 0x6600) {
                switch (t) {
                case 0x8800:
                    gUnk_03004D90[0] = t;
                    gUnk_03004D90[1] = gUnk_03001EF4;
                    gUnk_03004D90[2] = gUnk_03000038;
                    gUnk_03004D90[3] = gUnk_03004D40;
                    break;
                case 0x9900:
                    gUnk_03004D90[3] = 0;
                    gUnk_03004D90[2] = 0;
                    gUnk_03004D90[1] = 0;
                    gUnk_03004D90[0] = 0;
                    break;
                }
            }
        } else {
            sub_080b84f0();
        }
    }
}

void sub_08002b8c(void)
{
    s32 i;
    s32 t;
    u32 tries;
    u32 old;

    if (gUnk_0300243C <= 1) {
        gUnk_03000F98[0] = gUnk_03001EF4;
        gUnk_03001EB8[0] = gUnk_03000038;
        return;
    }

    sub_08006914(gUnk_03004D88, gUnk_03004D90, gUnk_03004D50[0]);
    if ((gUnk_03005274 & 0xFF00) == 0x8800) {
        tries = 0;
        if (gUnk_03004DA0[12] != 0) {
            u32 v;

            do {
                v = REG_VCOUNT;
                if (v < gUnk_03004D28)
                    v += 228;
            } while (v - gUnk_03004D28 <= 38);

            while (gUnk_03004DA0[12] != 0) {
                old = gUnk_03004D7C;
                gUnk_03001EC4 = 1;
                if (REG_IME & 1) {
                    while (gUnk_03001EC4 != 0)
                        ;
                }
                if (old == gUnk_03004D7C) {
                    tries++;
                    if (tries > 29)
                        sub_08008b8c();
                }
                sub_08002b04();
                sub_08006914(gUnk_03004D88, gUnk_03004D90, gUnk_03004D50[0]);
            }
            gUnk_03004D2C = 1;
        }
    }

    for (i = 0; i < 4; i++) {
        t = gUnk_03004D50[0][i] & 0xFF00;
        if (t != 0x6600) {
            if (t > 0x6600) {
                if (t == 0x8800) {
                    gUnk_03000F98[i] = gUnk_03004D50[1][i];
                    gUnk_03001EB8[i] = gUnk_03004D50[2][i];
                }
            }
        } else {
            sub_080b8694();
        }
        if ((gUnk_03004D50[0][i] & 0xFF00) != 0x8800)
            gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
    }
}

void sub_08002d18(void)
{
    sub_08005228();
    sub_08001a84();
    if (gUnk_03004D2C == 0)
        sub_08002b04();
    gUnk_03004D2C = 0;
    sub_08000de4();
    sub_08001a34();
    sub_08002b8c();
    if (sub_080072e0() != 0)
        sub_08008b8c();
}

void sub_08002d54(s32 count)
{
    while (--count != -1)
        sub_080022e4();
}

void sub_08002d74(s32 count)
{
    while (--count != -1)
        sub_080022d0();
}

void sub_08002d94(s32 count)
{
    while (--count != -1)
        sub_08002d18();
}

void sub_08002db4(void)
{
    while (gUnk_03001E90 != 0)
        sub_080022e4();
    gUnk_03000048 = 0;
}

void sub_08002de0(void)
{
    while (gUnk_03001E90 != 0)
        sub_080022d0();
    gUnk_03000048 = 0;
}

void sub_08002e0c(void)
{
    while (gUnk_03001E90 != 0)
        sub_08002d18();
    gUnk_03000048 = 0;
}

void sub_08002e38(u16 *p)
{
    gUnk_03001ED8 &= 0xFF80;
    gUnk_03001ED8 |= *(vu16 *)p;
    if ((gUnk_03001ED8 & 7) <= 2) {
        if (p[1] != 0)
            gUnk_03001188 = p[1];
        if (p[2] != 0)
            gUnk_03000B14 = p[2];
        if (p[3] != 0)
            gUnk_03000B10 = p[3];
        if (p[4] != 0)
            gUnk_03001EB4 = p[4];
    }
}

void sub_08002e98(u32 idx, u32 count, void (**fns)(void))
{
    if (idx < count)
        fns[idx]();
}

void sub_08002eac(u32 seed)
{
    gUnk_03000FB4 = seed & 0xFFF;
}

u32 sub_08002ec0(void)
{
    gUnk_03000FB4 = (gUnk_03000FB4 * 61 + 0x579) & 0xFFF;
    return gUnk_03000FB4;
}

u32 sub_08002ee8(u32 range)
{
    gUnk_03000FB4 = (gUnk_03000FB4 * 61 + 0x579) & 0xFFF;
    return (range * gUnk_03000FB4) >> 12;
}

void sub_08002f14(s16 n)
{
    u8 *b;
    s32 d;
    s16 v = n;

    if (n > 0) {
        gUnk_03001F08[5] = 18;
        b = gUnk_03001F08;
    } else if (n < 0) {
        gUnk_03001F08[5] = 16;
        v = -n;
        b = gUnk_03001F08;
    } else {
        u8 *e;
        u8 zero;
        u8 *p;

        b = gUnk_03001F08;
        e = gUnk_03001F08;
        zero = 0;
        p = b + 5;
        do {
            *p = zero;
            p--;
        } while ((s32)p >= (s32)e);
        b[5] = 17;
        return;
    }

    d = -1;
    while (v >= 0) { v -= 10000; d++; }
    v += 10000;
    b[4] = d;

    d = -1;
    while (v >= 0) { v -= 1000; d++; }
    v += 1000;
    b[3] = d;

    d = -1;
    while (v >= 0) { v -= 100; d++; }
    v += 100;
    b[2] = d;

    d = -1;
    while (v >= 0) { v -= 10; d++; }
    v += 10;
    b[1] = d;

    b[0] = v;
}

void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out)
{
    u16 *p = out;
    u16 r = ratio;
    u16 n = count;

    n--;
    while (n != 0xFFFF) {
        *p = (((*src & 31) + ((((*dst & 31) - (*src & 31)) * r) >> 8)) & 31)
           | (((*src & 0x3E0) + ((((*dst & 0x3E0) - (*src & 0x3E0)) * r) >> 8)) & 0x3E0)
           | (((*src & 0x7C00) + ((((*dst & 0x7C00) - (*src & 0x7C00)) * r) >> 8)) & 0x7C00);
        p++;
        src++;
        dst++;
        n--;
    }
}

u16 sub_080030b8(u16 a, u16 b, u16 ratio)
{
    return (((a & 31) + ((((b & 31) - (a & 31)) * ratio) >> 8)) & 31)
         | (((a & 0x3E0) + ((((b & 0x3E0) - (a & 0x3E0)) * ratio) >> 8)) & 0x3E0)
         | (((a & 0x7C00) + ((((b & 0x7C00) - (a & 0x7C00)) * ratio) >> 8)) & 0x7C00);
}
