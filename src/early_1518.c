#include "gba/gba.h"
#include "global.h"

/* Early subsystem code, 0x08001518-0x08001B08 (issue #32, batch A3).
 *
 * - sub_08001518 is the DEFAULT (no-op) IRQ handler: the 14-entry handler
 *   table at 0x080CFDE8 (copied to 0x030004B0 by AgbInit) points at it, as
 *   do the un-hook paths of sub_08001488/sub_080014e8.
 * - sub_0800151c / sub_08001560 (no symbols.csv entries; the census merged
 *   0x1518-0x157C into one "function") set/clear the DISPCNT forced-blank
 *   bit through the gUnk_03001ED8 shadow; 0x151C also restores REG_IE /
 *   REG_DISPSTAT from their shadows.  Neither is referenced anywhere in the
 *   ROM (dead exports kept by whole-object linking, lesson 3.17c).
 * - sub_0800157c (node-list) and sub_080017e4 (direct args) feed the VBlank
 *   transfer ring buffer at 0x03000B80..0x03000F7B (85 12-byte entries
 *   {control, src, dst}; write cursor gUnk_03000FC4, consumer cursor
 *   gUnk_03000F88, consumed by the IWRAM-copied routine at 0x03001F40 =
 *   sub_08001b08).  When DISPCNT forced-blank is set the copy happens
 *   immediately via CpuSet instead.  Control-word low nibble: 0 = 16-bit
 *   CpuSet, 1 = 32-bit, 2/3 = 16/32-bit fill; modes 3/4 copy 0x200-byte
 *   rows to every other 0x200-block (dst stride 0x400); mode 6 fills with
 *   a halfword value; mode 8 = LZ77UnCompVram.
 * - sub_08001a94 pushes a 12-byte record into the table at 0x030004F0
 *   (128 entries, counter gUnk_03001EE8) and appends its index to the
 *   per-lane byte list gUnk_03001680[lane][64] with counts gUnk_03000B30;
 *   sub_08001a34/sub_08001a0c reset those structures (free-list entries at
 *   0x03000050 get 236 = 0xEC in their first word); sub_08001a84 calls the
 *   IWRAM-copied consumer at 0x03001F41 (Thumb).
 *
 * Matching notes (agbcc -O2 -mthumb-interwork, docs/lessons-learned.md):
 * - In sub_0800157c the ONE variable `cmd` is reused as switch operand
 *   (`cmd &= 0xF` compiles to the in-place `ands r2, r0`), chunk size and
 *   control word - that keeps all of them in r2 as in the ROM.  In
 *   sub_080017e4 the `mode` PARAMETER is reused as the control word, which
 *   pins it (and the ctrl computations) to callee-saved r4.
 * - The chunk selection must be the if/else form `if (size <= 0x1FF) cmd =
 *   size; else cmd = 0x200;`: gcc 2.9's jump.c rewrites it to "x = 0x200;
 *   if (cond) x = size;" but the comparison constant 0x1FF was already
 *   forced to a register at expand time, so its pool load lands BEFORE the
 *   0x200 materialization.  Writing the transformed form directly emits the
 *   loads in the wrong order.
 * - The busy-wait `while (gUnk_03000F88 == (u32)q);` reads the NON-volatile
 *   consumer cursor: the ROM's spin loop compares a stale register copy.
 * - In sub_08001a94 the counter cell gUnk_03001EE8 is volatile (five
 *   separate reloads through r5) and the byte-list store needs the
 *   embedded-assignment index `[(cnt = gUnk_03000B30[a]) + a * 64]`: the
 *   destination address of an assignment is expanded first, so the
 *   gUnk_03001680 base pool load precedes the count load as in the ROM
 *   (same rule as the chained-assignment pool order, lesson 3.8).  The
 *   volatile indexed byte store emits the ROM's dead ldrb pre-read
 *   (lesson 3.7).
 * - In sub_08001a0c the fill bounds must come from the gUnk_03000050
 *   symbol: integer-literal pointers let gcc fold `p + 0x100` into a
 *   single pool constant and drop the entry guard the ROM has. */

extern vu16 gUnk_03001ED8; /* REG_DISPCNT shadow */
extern vu16 gUnk_03000018; /* REG_IE shadow */
extern vu16 gUnk_03001E9C; /* REG_DISPSTAT shadow */

extern u32 gUnk_03000FC4;  /* transfer ring write cursor */
extern u32 gUnk_03000F88;  /* transfer ring consumer cursor */
extern vu8 gUnk_03000F7C;  /* transfer ring buffer end (0x03000B80 + 0x3FC) */

extern u32 gUnk_03000050[]; /* 128 8-byte free-list entries */
extern u32 gUnk_03000B04;
extern vs32 gUnk_03001EE8;  /* record counter, -1 = empty */
extern vu16 gUnk_03000B1C;
extern vu16 gUnk_03001A80;
extern u32 gUnk_03000B30[]; /* per-lane counts (16 words, CpuFastSet-cleared) */
extern vu8 gUnk_03001680[]; /* per-lane byte lists [lane][64] */
extern u8 gUnk_030004F0[];  /* 12-byte records, indexed by gUnk_03001EE8 */

extern void sub_08001488(void);
extern void sub_080014e8(void);

struct TransferNode
{
    u32 cmd; /* mode in bits 0-3, byte size in bits 8-31 */
    u32 src;
    u32 dst;
};

void sub_08001518(void)
{
}

void sub_0800151c(void)
{
    gUnk_03001ED8 |= 0x80;
    REG_DISPCNT = gUnk_03001ED8;
    sub_080014e8();
    sub_08001488();
    REG_IE = gUnk_03000018;
    REG_DISPSTAT = gUnk_03001E9C;
}

void sub_08001560(void)
{
    gUnk_03001ED8 &= 0xFF7F;
    REG_DISPCNT = gUnk_03001ED8;
}

void sub_0800157c(struct TransferNode *node)
{
    u32 *q = (u32 *)gUnk_03000FC4;
    u32 cmd;

    while ((cmd = node->cmd) != 0)
    {
        u32 size = cmd >> 8;
        u32 src = node->src;
        u32 dst = node->dst;

        cmd &= 0xF;
        switch (cmd)
        {
        case 1:
            if (REG_DISPCNT & 0x80)
            {
                CpuSet((void *)src, (void *)dst, (size >> 1) & 0x1FFFFF);
            }
            else
            {
                if ((src | dst | size) & 3)
                    cmd = (size >> 1) << 4;
                else if (size & 31)
                    cmd = ((size >> 2) | 0x04000000) << 4;
                else
                    cmd = ((size >> 2) << 4) | 1;
                *q++ = cmd;
                *q++ = src;
                *q++ = dst;
                if (q >= (u32 *)&gUnk_03000F7C)
                    q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
                while (gUnk_03000F88 == (u32)q)
                    ;
            }
            break;
        case 2:
        case 5:
            CpuSet((void *)src, (void *)dst, (size >> 1) & 0x1FFFFF);
            break;
        case 4:
            dst += 0x200;
        case 3:
            while (size != 0)
            {
                if (size <= 0x1FF)
                    cmd = size;
                else
                    cmd = 0x200;
                size -= cmd;
                if (REG_DISPCNT & 0x80)
                {
                    CpuSet((void *)src, (void *)dst, (cmd >> 1) & 0x1FFFFF);
                }
                else
                {
                    if ((src | dst | cmd) & 3)
                        cmd = (cmd >> 1) << 4;
                    else if (cmd & 31)
                        cmd = ((cmd >> 2) | 0x04000000) << 4;
                    else
                        cmd = ((cmd >> 2) << 4) | 1;
                    *q++ = cmd;
                    *q++ = src;
                    *q++ = dst;
                    if (q >= (u32 *)&gUnk_03000F7C)
                        q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
                    while (gUnk_03000F88 == (u32)q)
                        ;
                }
                src += 0x200;
                dst += 0x400;
            }
            break;
        case 6:
            if (REG_DISPCNT & 0x80)
            {
                u16 tmp = src;

                CpuSet(&tmp, (void *)dst, ((size >> 1) & 0x1FFFFF) | 0x01000000);
            }
            else
            {
                if ((dst | size) & 3)
                    cmd = (((size >> 1) | 0x01000000) << 4) | 2;
                else if (size & 31)
                    cmd = (((size >> 2) | 0x05000000) << 4) | 2;
                else
                    cmd = (((size >> 2) | 0x01000000) << 4) | 3;
                *q++ = cmd;
                src &= 0xFFFF;
                *q++ = (src << 16) | src;
                *q++ = dst;
                if (q >= (u32 *)&gUnk_03000F7C)
                    q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
                while (gUnk_03000F88 == (u32)q)
                    ;
            }
            break;
        case 8:
            LZ77UnCompVram((void *)src, (void *)dst);
            break;
        }
        node++;
    }
    gUnk_03000FC4 = (u32)q;
}

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size)
{
    u32 *q = (u32 *)gUnk_03000FC4;

    switch (mode)
    {
    case 1:
        if (REG_DISPCNT & 0x80)
        {
            CpuSet((void *)src, (void *)dst, (size >> 1) & 0x1FFFFF);
        }
        else
        {
            if ((src | dst | size) & 3)
                mode = (size >> 1) << 4;
            else if (size & 31)
                mode = ((size >> 2) | 0x04000000) << 4;
            else
                mode = ((size >> 2) << 4) | 1;
            *q++ = mode;
            *q++ = src;
            *q++ = dst;
            if (q >= (u32 *)&gUnk_03000F7C)
                q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
            while (gUnk_03000F88 == (u32)q)
                ;
        }
        break;
    case 2:
    case 5:
        CpuSet((void *)src, (void *)dst, (size >> 1) & 0x1FFFFF);
        break;
    case 4:
        dst += 0x200;
    case 3:
        while (size != 0)
        {
            u32 chunk;

            if (size <= 0x1FF)
                chunk = size;
            else
                chunk = 0x200;
            size -= chunk;
            if (REG_DISPCNT & 0x80)
            {
                CpuSet((void *)src, (void *)dst, (chunk >> 1) & 0x1FFFFF);
            }
            else
            {
                if ((src | dst | chunk) & 3)
                    mode = (chunk >> 1) << 4;
                else if (chunk & 31)
                    mode = ((chunk >> 2) | 0x04000000) << 4;
                else
                    mode = ((chunk >> 2) << 4) | 1;
                *q++ = mode;
                *q++ = src;
                *q++ = dst;
                if (q >= (u32 *)&gUnk_03000F7C)
                    q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
                while (gUnk_03000F88 == (u32)q)
                    ;
            }
            src += 0x200;
            dst += 0x400;
        }
        break;
    case 6:
        if (REG_DISPCNT & 0x80)
        {
            u16 tmp = src;

            CpuSet(&tmp, (void *)dst, ((size >> 1) & 0x1FFFFF) | 0x01000000);
        }
        else
        {
            if ((dst | size) & 3)
                mode = (((size >> 1) | 0x01000000) << 4) | 2;
            else if (size & 31)
                mode = (((size >> 2) | 0x05000000) << 4) | 2;
            else
                mode = (((size >> 2) | 0x01000000) << 4) | 3;
            *q++ = mode;
            src &= 0xFFFF;
            *q++ = (src << 16) | src;
            *q++ = dst;
            if (q >= (u32 *)&gUnk_03000F7C)
                q = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
            while (gUnk_03000F88 == (u32)q)
                ;
        }
        break;
    case 8:
        LZ77UnCompVram((void *)src, (void *)dst);
        break;
    }
    gUnk_03000FC4 = (u32)q;
}

void sub_08001a34(void);

void sub_08001a0c(void)
{
    u32 *p;
    u32 *end;

    sub_08001a34();
    p = gUnk_03000050;
    end = p + 0x100;
    while (p < end)
    {
        *p = 236;
        p += 2;
    }
    sub_08001a34();
}

void sub_08001a34(void)
{
    u32 zeroWord;
    u32 zero;

    gUnk_03000B04 = 0x03000050;
    gUnk_03001EE8 = -1;
    zero = 0;
    zeroWord = zero;
    CpuFastSet(&zeroWord, (u32 *)0x03000B30, 0x01000010);
    gUnk_03000B1C = gUnk_03001A80 = zero;
}

void sub_08001a84(void)
{
    ((void (*)(void))0x03001F41)();
}

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, u16 f)
{
    u16 *p;
    u32 cnt;

    gUnk_03001EE8 = gUnk_03001EE8 + 1;
    p = (u16 *)(gUnk_03001EE8 * 12 + (u32)gUnk_030004F0);
    if (gUnk_03001EE8 > 127)
        return -1;
    *p++ = c;
    *p++ = d;
    *p++ = e;
    *p++ = f;
    *(u32 *)p = b;
    gUnk_03001680[(cnt = gUnk_03000B30[a]) + a * 64] = gUnk_03001EE8;
    gUnk_03000B30[a] = cnt + 1;
    return gUnk_03001EE8;
}
