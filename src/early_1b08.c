#include "gba/gba.h"
#include "global.h"

/* OAM shadow builder (0x08001B08-0x08001CC7, issue #32 batch B1).
 *
 * Walks 16 priority buckets (gUnk_03000B30[i] = entry count, gUnk_03001680[i]
 * = 64 slot indices per bucket).  Each slot indexes a 12-byte sprite record in
 * gUnk_030004F0 holding:
 *      +0  flags   bit15 = "wide" OAM source stride, bits14-13 = priority,
 *                  bits11-10 = OBJ mode bits OR'd into attr0
 *      +2  flags   bits15-11 = palette bank / blend control,
 *                  bits10-0  = base tile number added to attr2
 *      +4  y bias, +6 x bias, +8 pointer to the OAM template stream
 * The template stream is copied into the OAM shadow at gUnk_03000050 with the
 * biases added (attr0 y in bits 7-0, attr1 x in bits 8-0, both wrapping in
 * their own field width) until a template entry has bit12 set ("last") or the
 * shadow fills up.  Afterwards gUnk_03000B04 keeps the write cursor,
 * gUnk_03001EC8 the number of entries used, and every unused OAM slot gets
 * attr0 = 236 (off-screen y) to hide it.
 *
 * NOTE (IWRAM): AgbInit copies these 0x1C0 bytes to 0x03001F40 with a
 * 256-halfword CpuSet and the game executes the RAM copy.  That is safe and
 * needs no special source treatment: the function makes no BL calls at all,
 * every branch is a PC-relative Thumb b/bcc, and every datum it touches is an
 * absolute IWRAM address loaded from the function's own literal pool -- which
 * sits inside the copied 0x1C0 bytes.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork; see docs/lessons-learned.md):
 *  - `hoist`/`hoistZ` are dead stores (they compile to nothing).  They exist
 *    so the constants they carry are a *different expression* from the literal
 *    spelled at the second use site, which stops CSE from unifying the two
 *    uses -- the ROM materializes both separately.
 *  - `u = t + hoistZ` (hoistZ == 0) survives cprop as `add rD,rS,#0`, i.e. a
 *    plain register COPY that CSE will not propagate away.  That copy is what
 *    keeps the ROM's two separate `v + <pal offset>` adds; writing `u = t`
 *    lets copy propagation collapse them into one.
 *  - `{ u32 w = v + tile; *dst = w; }`: storing straight to the u16 `*dst`
 *    narrows the addition to HImode, which introduces a `(u16)v` temp with a
 *    higher pseudo number than `tile`, and agbcc canonicalizes commutative
 *    operands lowest-pseudo-first -- giving `adds r0,r2,r3` instead of the
 *    ROM's `adds r0,r3,r2`.  The u32 temp keeps both operands as the original
 *    pseudos and restores the ROM operand order.
 *  - The dead store `b = 0;` late in the loop body extends `b`'s pseudo past
 *    the end of the CSE block, which makes cse.c pick `b` (not `c`) as the
 *    canonical register of the shared "255" quantity.  Without it CSE rewrites
 *    `b &= v` into `b = v & c` and the ROM's second `movs rX,#255` disappears.
 *  - `register u16 flags asm("r3")`: the two record-header loads land in r3 in
 *    the ROM (sharing the register with `v`, which never overlaps them).  Left
 *    to itself global alloc puts them in r1/r0 because the mask temporaries in
 *    their live range already own r0.  Same idiom as FadeOutBody in
 *    src/m4a_c1.c.
 *  - `{ u32 t2 = v & 0x1000; last = t2; }` (NOT `last = v & 0x1000;`, NOT a
 *    long-lived `mask` variable) decides the reload register of `flip` at
 *    0x08001BC0.  agbcc's reload picks a spill register per insn by rotating
 *    round-robin from the previously allocated one (reload1.c
 *    allocate_reload_reg / last_spill_reg).  If the 0x1000 mask lives in a
 *    function-scope variable, that variable is spilled with a REG_EQUIV
 *    constant and *rematerialized by reload*, which consumes one rotation step
 *    and pushes `ldr [sp,#12]` from r0 onto r1.  Written as a block-local u32
 *    temp the constant is an ordinary short-lived pseudo (no reload), the
 *    rotation still points at r0, and the ROM's `ldr r0,[sp,#12]` comes out.
 *    Writing `last = v & 0x1000;` directly instead narrows the AND to HImode
 *    and costs the extra `adds rD,rS,#0` movhi copy (lessons-learned 3.24).
 *  - `u16 last;` is declared AFTER `u16 mode, pal, tile;`.  With the u32 temp
 *    above, `tile` and `last` end up with *identical* global-alloc priorities
 *    (global.c: floor_log2(refs)*refs/live_length -> 3*12/156 == 2*6/52), and
 *    allocno_compare breaks that tie by pseudo number.  Declaring `last` last
 *    makes `tile` the lower pseudo, so `tile` takes r8 and `last` r9 as in the
 *    ROM; the other order swaps every `mov rX,r8` / `mov rX,r9` in the body.
 *
 * STATUS: byte-exact (448/448). */

extern u16 *gUnk_03000B04;       /* OAM shadow write cursor */
extern vu32 gUnk_03000B30[16];   /* per-bucket sprite counts */
extern u8 gUnk_03001680[16][64]; /* per-bucket sprite slot indices */
extern u16 gUnk_030004F0[][6];   /* 12-byte sprite records */
extern u16 gUnk_03000050[];      /* OAM shadow (128 entries * 4 halfwords) */
extern vu16 gUnk_03001EC8;       /* number of OAM entries used */

void sub_08001b08(void)
{
    u16 *dst;
    s32 i;
    u32 j;
    u16 *p;
    u32 v;
    register u16 flags asm("r3");
    u16 mode, pal, tile;
    u16 last;
    u16 x, y, flip;
    vu32 *cnt;
    u32 prio;
    u32 hoist;
    u32 hoistZ;

    dst = gUnk_03000B04;
    for (i = 0, cnt = gUnk_03000B30; i < 16; cnt++, i++)
    {
        hoist = 0xFF00;
        hoistZ = 0;
        if (*cnt != 0)
        {
            for (j = 0; j < *cnt; j++)
            {
                p = gUnk_030004F0[gUnk_03001680[i][j]];
                flags = *p++;
                flip = flags & 0x8000;
                prio = (flags & 0x6000) >> 3;
                mode = 0xC00;
                mode &= flags;
                flags = *p++;
                pal = 0xF800;
                pal &= flags;
                tile = 0x7FF;
                tile &= flags;
                x = *p++;
                y = *p++;
                p = *(u16 **)p;

            next_oam:
                {
                    u32 b, c;

                    v = mode | *p++;
                    {
                        u32 a0;

                        a0 = hoist;
                        a0 &= v;
                        c = 0xFF;
                        b = 0xFF;
                        b &= v;
                        *dst++ = a0 | ((y + b) & c);
                    }
                    { u32 t2 = v & 0x1000; last = t2; }

                    if (flip)
                    {
                        p++;
                        v = *p++;
                    }
                    else
                    {
                        v = p[0];
                        p += 2;
                    }
                    {
                        u32 a1, d;

                        d = 0x1FF;
                        a1 = 0xFE00;
                        a1 &= v;
                        v &= d;
                        *dst++ = a1 | ((x + v) & d);
                    }

                    v = *p++;
                    if (pal != 0)
                    {
                        if (pal & 0x800)
                        {
                            s32 t = pal & 0xF000;
                            s32 u = t + hoistZ;

                            if ((s32)((v + u) & 0xF000) >= u)
                                v = (u16)(v + t);
                        }
                        else
                        {
                            v = (v & 0xFFF) | pal;
                        }
                    }
                    if (prio != 0)
                        v = (v & 0xF3FF) | prio;
                    b = 0;
                    { u32 w = v + tile; *dst = w; }
                    dst += 2;

                    if (dst >= gUnk_03000050 + 512)
                        goto finish;
                    if (!last)
                        goto next_oam;
                }
            }
        }
    }

finish:
    gUnk_03000B04 = dst;
    gUnk_03001EC8 = ((u32)dst - (u32)gUnk_03000050) >> 3;

    {
        u32 *q = (u32 *)dst;

        while (q < (u32 *)(gUnk_03000050 + 512))
        {
            *q = 236;
            q += 2;
        }
    }
}
