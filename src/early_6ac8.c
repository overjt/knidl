#include "gba/gba.h"
#include "global.h"

/* early_6ac8.c (0x08006AC8-0x08006CD3, issue #63).
 *
 * The send and receive queues of the SIO multi-play link driver
 * (src/early_6464.c, src/early_6cd4.c, src/early_6d18.c): the per-frame link
 * step LinkMain1 calls EnqueueSendCmd to queue the frame's four command
 * words into the send ring gLink.ring[4][30] (the words are ORed into
 * gSendNonzeroCheck first; an all-zero frame is not queued, a full ring sets the
 * overflow flag unk14) and DequeueRecvCmds to take the oldest four-player frame
 * out of the receive ring gLink.buf[4][4][30] (or clear the caller's
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

/* The link work area gLink (0x4D2 bytes; layout as in
 * src/early_6d18.c).  It is the SIO multi-play library pokeruby ships as
 * src/link.c (struct Link there), in an earlier revision: four command
 * words per frame and 30-entry queues. */
struct Link {
    /*0x000*/ u8 isMaster, state, localId, count;
    /*0x004*/ u16 recv[4];
    /*0x00C*/ u8 receivedNothing, serialIntrCounter, unk0E, unk0F;
    /*0x010*/ u8 handshakeAsMaster, unk11, hardwareError, badChecksum, queueFull, lag;
    /*0x016*/ u16 chk;
    /*0x018*/ u8 sendCmdIndex, recvCmdIndex, unk1A, unk1B;
    /*0x01C*/ u16 ring[4][30];
    /*0x10C*/ u8 sendQueuePos, sendQueueCount, unk10E, unk10F;
    /*0x110*/ u16 buf[4][4][30];
    /*0x4D0*/ u8 recvQueuePos, recvQueueCount;
};

/* Not from link.h: this file's view of gLink differs (lesson 3.517). */
extern struct Link gLink;
extern u16 gLinkSavedIme;       /* saved REG_IME */
extern u16 gSendNonzeroCheck;       /* OR of the frame's send words */
extern u8 gLastSendQueueCount;

/* Queue one 4-halfword send frame into the send ring (pokeruby's
 * EnqueueSendCmd) and clear the caller's buffer.  REG_IME is the io_reg.h
 * macro, as in DequeueRecvCmds: a symbol's address is kept alive in r9 by
 * gcse to the final restore instead of being re-loaded there. */
void EnqueueSendCmd(u16 *p)
{
    u16 ie;
    u32 n;
    u32 i;

    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 2;
    REG_IME = 1;
    if (gLink.sendQueueCount < 30)
    {
        n = gLink.sendQueuePos + gLink.sendQueueCount;
        if (n >= 30)
            n -= 30;
        for (i = 0; i < 4; i++)
        {
            gSendNonzeroCheck |= *p;
            gLink.ring[i][n] = *p;
            *p = 0;
            p++;
        }
    }
    else
    {
        gLink.queueFull = 1;
    }
    if (gSendNonzeroCheck)
    {
        gLink.sendQueueCount++;
        gSendNonzeroCheck = 0;
    }
    REG_IME = 0;
    REG_IE = ie;
    REG_IME = gLinkSavedIme;
    gLastSendQueueCount = gLink.sendQueueCount;
}

/* Dequeue one 4x4 receive frame out of the receive ring (pokeruby's
 * DequeueRecvCmds); when nothing is pending the caller's matrix is zeroed
 * and the "received nothing" flag is set.  REG_IME is the plain io_reg.h
 * macro here: as a symbol, gcse's PRE keeps its address alive to the end of
 * the function and the first reload register moves from r2 to r4. */
void DequeueRecvCmds(u16 (*p)[4])
{
    u16 ie;
    u32 i;
    u32 j;

    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 2;
    REG_IME = 1;
    if (gLink.recvQueueCount == 0)
    {
        for (i = 0; i < 4; i++)
            for (j = 0; j < gLink.count; j++)
                p[i][j] = 0;
        gLink.receivedNothing = 1;
    }
    else
    {
        for (i = 0; i < 4; i++)
            for (j = 0; j < gLink.count; j++)
                p[i][j] = gLink.buf[j][i][gLink.recvQueuePos];
        gLink.recvQueueCount--;
        gLink.recvQueuePos++;
        if (gLink.recvQueuePos >= 30)
            gLink.recvQueuePos = 0;
        gLink.receivedNothing = 0;
    }
    REG_IME = 0;
    REG_IE = ie;
    REG_IME = gLinkSavedIme;
}
