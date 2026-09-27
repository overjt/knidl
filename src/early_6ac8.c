#include "gba/gba.h"
#include "global.h"

/* early_6ac8.c (0x08006AC8-0x08006CD3, issue #63).
 *
 * The send and receive queues of the SIO multi-play link driver
 * (src/early_6464.c, src/early_6cd4.c, src/early_6d18.c): the per-frame link
 * step sub_08006914 calls sub_08006ac8 to queue the frame's four command
 * words into the send ring gUnk_03004DA0.ring[4][30] (the words are ORed into
 * gUnk_03004D84 first; an all-zero frame is not queued, a full ring sets the
 * overflow flag unk14) and sub_08006bb4 to take the oldest four-player frame
 * out of the receive ring gUnk_03004DA0.buf[4][4][30] (or clear the caller's
 * 4x4 matrix and set the "received nothing" flag unk0C).  Both run with the
 * interrupts masked down to HBlank (REG_IE = 2) and restore REG_IE and
 * REG_IME afterwards.
 *
 * The driver is the SIO multi-play library pokeruby ships as src/link.c, in
 * an older revision (four command words per frame, 30-entry queues, the
 * REG_IE save): these are EnqueueSendCmd and DequeueRecvCmds, and struct Link
 * is pokeruby's struct Link with this revision's layout.
 *
 * Matching note (issue #63): REG_IME and REG_IE are the io_reg.h macros; with
 * a symbol for REG_IME, gcse keeps its address in a register to the end of
 * the function (lesson 3.482). */

/* The link work area gUnk_03004DA0 (0x4D2 bytes; layout as in
 * src/early_6d18.c).  It is the SIO multi-play library pokeruby ships as
 * src/link.c (struct Link there), in an earlier revision: four command
 * words per frame and 30-entry queues. */
struct Link {
    /*0x000*/ u8 unk00, unk01, unk02, count;
    /*0x004*/ u16 recv[4];
    /*0x00C*/ u8 unk0C, unk0D, unk0E, unk0F;
    /*0x010*/ u8 unk10, unk11, unk12, unk13, unk14, unk15;
    /*0x016*/ u16 chk;
    /*0x018*/ u8 unk18, unk19, unk1A, unk1B;
    /*0x01C*/ u16 ring[4][30];
    /*0x10C*/ u8 unk10C, unk10D, unk10E, unk10F;
    /*0x110*/ u16 buf[4][4][30];
    /*0x4D0*/ u8 unk4D0, unk4D1;
};

extern struct Link gUnk_03004DA0;
extern u16 gUnk_03004D44;       /* saved REG_IME */
extern u16 gUnk_03004D84;       /* OR of the frame's send words */
extern u8 gUnk_03004D20;

/* Queue one 4-halfword send frame into the send ring (pokeruby's
 * EnqueueSendCmd) and clear the caller's buffer.  REG_IME is the io_reg.h
 * macro, as in sub_08006bb4: a symbol's address is kept alive in r9 by
 * gcse to the final restore instead of being re-loaded there. */
void sub_08006ac8(u16 *p)
{
    u16 ie;
    u32 n;
    u32 i;

    gUnk_03004D44 = REG_IME;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 2;
    REG_IME = 1;
    if (gUnk_03004DA0.unk10D < 30)
    {
        n = gUnk_03004DA0.unk10C + gUnk_03004DA0.unk10D;
        if (n >= 30)
            n -= 30;
        for (i = 0; i < 4; i++)
        {
            gUnk_03004D84 |= *p;
            gUnk_03004DA0.ring[i][n] = *p;
            *p = 0;
            p++;
        }
    }
    else
    {
        gUnk_03004DA0.unk14 = 1;
    }
    if (gUnk_03004D84)
    {
        gUnk_03004DA0.unk10D++;
        gUnk_03004D84 = 0;
    }
    REG_IME = 0;
    REG_IE = ie;
    REG_IME = gUnk_03004D44;
    gUnk_03004D20 = gUnk_03004DA0.unk10D;
}

/* Dequeue one 4x4 receive frame out of the receive ring (pokeruby's
 * DequeueRecvCmds); when nothing is pending the caller's matrix is zeroed
 * and the "received nothing" flag is set.  REG_IME is the plain io_reg.h
 * macro here: as a symbol, gcse's PRE keeps its address alive to the end of
 * the function and the first reload register moves from r2 to r4. */
void sub_08006bb4(u16 (*p)[4])
{
    u16 ie;
    u32 i;
    u32 j;

    gUnk_03004D44 = REG_IME;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 2;
    REG_IME = 1;
    if (gUnk_03004DA0.unk4D1 == 0)
    {
        for (i = 0; i < 4; i++)
            for (j = 0; j < gUnk_03004DA0.count; j++)
                p[i][j] = 0;
        gUnk_03004DA0.unk0C = 1;
    }
    else
    {
        for (i = 0; i < 4; i++)
            for (j = 0; j < gUnk_03004DA0.count; j++)
                p[i][j] = gUnk_03004DA0.buf[j][i][gUnk_03004DA0.unk4D0];
        gUnk_03004DA0.unk4D1--;
        gUnk_03004DA0.unk4D0++;
        if (gUnk_03004DA0.unk4D0 >= 30)
            gUnk_03004DA0.unk4D0 = 0;
        gUnk_03004DA0.unk0C = 0;
    }
    REG_IME = 0;
    REG_IE = ie;
    REG_IME = gUnk_03004D44;
}
