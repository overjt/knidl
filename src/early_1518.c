#include "gba/gba.h"
#include "global.h"

/* Early subsystem code, 0x08001518-0x08001B08 (issue #32, batch A3).
 *
 * - IntrDummy is the DEFAULT (no-op) IRQ handler: the 14-entry handler
 *   table at 0x080CFDE8 (copied to 0x030004B0 by AgbInit) points at it, as
 *   do the un-hook paths of ClearHBlankIntr/ClearVCountIntr.
 * - EnableForcedBlank / DisableForcedBlank (no symbols.csv entries; the census merged
 *   0x1518-0x157C into one "function") set/clear the DISPCNT forced-blank
 *   bit through the gDispCnt shadow; 0x151C also restores REG_IE /
 *   REG_DISPSTAT from their shadows.  Neither is referenced anywhere in the
 *   ROM (dead exports kept by whole-object linking, lesson 3.17c).
 * - RequestCopyList (node-list) and RequestCopy (direct args) feed the VBlank
 *   transfer ring buffer at 0x03000B80..0x03000F7B (85 12-byte entries
 *   {control, src, dst}; write cursor gCopyQueueWrite, consumer cursor
 *   gCopyQueueRead, consumed by the IWRAM-copied routine at 0x03001F40 =
 *   BuildOam).  When DISPCNT forced-blank is set the copy happens
 *   immediately via CpuSet instead.  Control-word low nibble: 0 = 16-bit
 *   CpuSet, 1 = 32-bit, 2/3 = 16/32-bit fill; modes 3/4 copy 0x200-byte
 *   rows to every other 0x200-block (dst stride 0x400); mode 6 fills with
 *   a halfword value; mode 8 = LZ77UnCompVram.
 * - QueueSprite pushes a 12-byte record into the table at 0x030004F0
 *   (128 entries, counter gSpriteQueueTop) and appends its index to the
 *   per-lane byte list gSpriteLayerLists[lane][64] with counts gSpriteLayerCounts;
 *   ResetSpriteQueue/ResetOamShadow reset those structures (free-list entries at
 *   0x03000050 get 236 = 0xEC in their first word); RunBuildOamInIwram calls the
 *   IWRAM-copied consumer at 0x03001F41 (Thumb).
 *
 * Matching notes (agbcc -O2 -mthumb-interwork, docs/lessons-learned.md):
 * - In RequestCopyList the ONE variable `cmd` is reused as switch operand
 *   (`cmd &= 0xF` compiles to the in-place `ands r2, r0`), chunk size and
 *   control word - that keeps all of them in r2 as in the ROM.  In
 *   RequestCopy the `mode` PARAMETER is reused as the control word, which
 *   pins it (and the ctrl computations) to callee-saved r4.
 * - The chunk selection must be the if/else form `if (size <= 0x1FF) cmd =
 *   size; else cmd = 0x200;`: gcc 2.9's jump.c rewrites it to "x = 0x200;
 *   if (cond) x = size;" but the comparison constant 0x1FF was already
 *   forced to a register at expand time, so its pool load lands BEFORE the
 *   0x200 materialization.  Writing the transformed form directly emits the
 *   loads in the wrong order.
 * - The busy-wait `while (gCopyQueueRead == (u32)q);` reads the NON-volatile
 *   consumer cursor: the ROM's spin loop compares a stale register copy.
 * - In QueueSprite the counter cell gSpriteQueueTop is volatile (five
 *   separate reloads through r5) and the byte-list store needs the
 *   embedded-assignment index `[(cnt = gSpriteLayerCounts[a]) + a * 64]`: the
 *   destination address of an assignment is expanded first, so the
 *   gSpriteLayerLists base pool load precedes the count load as in the ROM
 *   (same rule as the chained-assignment pool order, lesson 3.8).  The
 *   volatile indexed byte store emits the ROM's dead ldrb pre-read
 *   (lesson 3.7).
 * - In ResetOamShadow the fill bounds must come from the gOamBuffer
 *   symbol: integer-literal pointers let gcc fold `p + 0x100` into a
 *   single pool constant and drop the entry guard the ROM has. */

/* Not from main.h: this file's view of gOamBuffer differs (lesson 3.517). */
extern vu16 gDispCnt; /* REG_DISPCNT shadow */
extern vu16 gIntrEnable; /* REG_IE shadow */
extern vu16 gDispStat; /* REG_DISPSTAT shadow */

extern u32 gCopyQueueWrite;  /* transfer ring write cursor */
extern u32 gCopyQueueRead;  /* transfer ring consumer cursor */
extern vu8 gWinOut;  /* transfer ring buffer end (0x03000B80 + 0x3FC) */

extern u32 gOamBuffer[]; /* 128 8-byte free-list entries */
extern u32 gOamBufferCursor;
extern vs32 gSpriteQueueTop;  /* record counter, -1 = empty */
extern vu16 gOamAffineCount;
extern vu16 gAffineSpriteBufferPos;
extern vu32 gSpriteLayerCounts[16]; /* per-lane counts (16 words, CpuFastSet-cleared) */
extern vu8 gSpriteLayerLists[]; /* per-lane byte lists [lane][64] */
extern u16 gSpriteQueue[][6];  /* 12-byte records, indexed by gSpriteQueueTop */

extern void ClearHBlankIntr(void);
extern void ClearVCountIntr(void);

struct TransferNode
{
    u32 cmd; /* mode in bits 0-3, byte size in bits 8-31 */
    u32 src;
    u32 dst;
};

void IntrDummy(void)
{
}

void EnableForcedBlank(void)
{
    gDispCnt |= 0x80;
    REG_DISPCNT = gDispCnt;
    ClearVCountIntr();
    ClearHBlankIntr();
    REG_IE = gIntrEnable;
    REG_DISPSTAT = gDispStat;
}

void DisableForcedBlank(void)
{
    gDispCnt &= 0xFF7F;
    REG_DISPCNT = gDispCnt;
}

void RequestCopyList(struct TransferNode *node)
{
    u32 *q = (u32 *)gCopyQueueWrite;
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
                    cmd = ((size >> 2) | 0x04000000) << 4; /* raw: not an address: CpuSet's 32-bit flag in a copy command */
                else
                    cmd = ((size >> 2) << 4) | 1;
                *q++ = cmd;
                *q++ = src;
                *q++ = dst;
                if (q >= (u32 *)&gWinOut)
                    q = (u32 *)((u32)&gWinOut - 0x3FC);
                while (gCopyQueueRead == (u32)q)
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
                        cmd = ((cmd >> 2) | 0x04000000) << 4; /* raw: not an address: CpuSet's 32-bit flag in a copy command */
                    else
                        cmd = ((cmd >> 2) << 4) | 1;
                    *q++ = cmd;
                    *q++ = src;
                    *q++ = dst;
                    if (q >= (u32 *)&gWinOut)
                        q = (u32 *)((u32)&gWinOut - 0x3FC);
                    while (gCopyQueueRead == (u32)q)
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
                    cmd = (((size >> 2) | 0x05000000) << 4) | 2; /* raw: not an address: CpuSet's fill + 32-bit flags in a copy command */
                else
                    cmd = (((size >> 2) | 0x01000000) << 4) | 3;
                *q++ = cmd;
                src &= 0xFFFF;
                *q++ = (src << 16) | src;
                *q++ = dst;
                if (q >= (u32 *)&gWinOut)
                    q = (u32 *)((u32)&gWinOut - 0x3FC);
                while (gCopyQueueRead == (u32)q)
                    ;
            }
            break;
        case 8:
            LZ77UnCompVram((void *)src, (void *)dst);
            break;
        }
        node++;
    }
    gCopyQueueWrite = (u32)q;
}

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size)
{
    u32 *q = (u32 *)gCopyQueueWrite;

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
                mode = ((size >> 2) | 0x04000000) << 4; /* raw: not an address: CpuSet's 32-bit flag in a copy command */
            else
                mode = ((size >> 2) << 4) | 1;
            *q++ = mode;
            *q++ = src;
            *q++ = dst;
            if (q >= (u32 *)&gWinOut)
                q = (u32 *)((u32)&gWinOut - 0x3FC);
            while (gCopyQueueRead == (u32)q)
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
                    mode = ((chunk >> 2) | 0x04000000) << 4; /* raw: not an address: CpuSet's 32-bit flag in a copy command */
                else
                    mode = ((chunk >> 2) << 4) | 1;
                *q++ = mode;
                *q++ = src;
                *q++ = dst;
                if (q >= (u32 *)&gWinOut)
                    q = (u32 *)((u32)&gWinOut - 0x3FC);
                while (gCopyQueueRead == (u32)q)
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
                mode = (((size >> 2) | 0x05000000) << 4) | 2; /* raw: not an address: CpuSet's fill + 32-bit flags in a copy command */
            else
                mode = (((size >> 2) | 0x01000000) << 4) | 3;
            *q++ = mode;
            src &= 0xFFFF;
            *q++ = (src << 16) | src;
            *q++ = dst;
            if (q >= (u32 *)&gWinOut)
                q = (u32 *)((u32)&gWinOut - 0x3FC);
            while (gCopyQueueRead == (u32)q)
                ;
        }
        break;
    case 8:
        LZ77UnCompVram((void *)src, (void *)dst);
        break;
    }
    gCopyQueueWrite = (u32)q;
}

void ResetSpriteQueue(void);

void ResetOamShadow(void)
{
    u32 *p;
    u32 *end;

    ResetSpriteQueue();
    p = gOamBuffer;
    end = p + 0x100;
    while (p < end)
    {
        *p = 236;
        p += 2;
    }
    ResetSpriteQueue();
}

void ResetSpriteQueue(void)
{
    u32 zeroWord;
    u32 zero;

    gOamBufferCursor = IWRAM_START + 0x50;
    gSpriteQueueTop = -1;
    zero = 0;
    zeroWord = zero;
    CpuFastSet(&zeroWord, (u32 *)(IWRAM_START + 0xB30), 0x01000010);
    gOamAffineCount = gAffineSpriteBufferPos = zero;
}

void RunBuildOamInIwram(void)
{
    ((void (*)(void))(IWRAM_START + 0x1F41))();
}

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u16 f)
{
    u16 *p;
    u32 cnt;

    gSpriteQueueTop = gSpriteQueueTop + 1;
    p = (u16 *)(gSpriteQueueTop * 12 + (u32)gSpriteQueue);
    if (gSpriteQueueTop > 127)
        return -1;
    *p++ = c;
    *p++ = d;
    *p++ = e;
    *p++ = f;
    *(u32 *)p = b;
    gSpriteLayerLists[(cnt = gSpriteLayerCounts[a]) + a * 64] = gSpriteQueueTop;
    gSpriteLayerCounts[a] = cnt + 1;
    return gSpriteQueueTop;
}
