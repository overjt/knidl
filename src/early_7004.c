#include "gba/gba.h"
#include "global.h"

/* Link (SIO multi-play) interrupt handlers and session bootstrap,
 * 0x08006D18-0x080072FF (issue #32, batch H1) -- the last block of the
 * game_code_early segment; src/main.c's AgbMain starts at 0x08007300.
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix (fnmatch --newpb),
 * the single recipe of this zone (lesson 3.75).  No Makefile override needed.
 *
 * Contents in ROM order.  Two functions have no symbols.csv entry of their
 * own (lesson 2.13 dead exports hidden inside a neighbour's size):
 * DoSend lives inside the declared 0x21C of DoRecv, and
 * SendRecvDone inside the declared 0x4A of StopTimer -- see the report;
 * symbols.csv's sub_08007102/0x22 entry is a mis-split of SendRecvDone.
 *
 *   Timer3Intr  timer-3 IRQ (gIntrTable[1]): stop the timeout timer, re-arm
 *               SIOCNT (pokeruby's Timer3Intr).
 *   SerialCB  serial IRQ (gIntrTable[0]) for the link session: snapshots SIOCNT, then
 *                 either runs the transfer step (state 4) or the connect/ID
 *                 handshake (state 2).            [src/early_6d28.c]
 *   StartTransfer  re-arm the SIOCNT start bit.
 *   DoRecv  per-frame receive step: copies the four SIOMULTI words to
 *                 gUnk_03004D38 and folds them into the per-player buffer.
 *                                                 [src/early_6e9c.c]
 *   DoSend  send step: pushes the next ring slot into SIOMLT_SEND.
 *   StopTimer  stop the link timeout timer (TM3).
 *   SendRecvDone  end-of-round bookkeeping / re-arm.
 *   ResetSendBuffer  clear the 4x30 halfword ring at +0x1C and its two cursors.
 *   ResetRecvBuffer  clear the 4x4x30 halfword buffer at +0x110 and its cursors.
 *   ConnectLink  blocking link bring-up loop; returns 1 on timeout (60
 *                 frames without reaching state 4), 0 on success.
 *   IsLinkError  poll gLinkStatus against the mask in gLinkErrorMask.
 *
 * gLink is the link work area (0x4D2 bytes, ending just below
 * gLinkCommand).  Byte offsets used here:
 *   +0x00 session-active flag      +0x01 state (1,2,3,4)
 *   +0x02 player id (SIOCNT bits 4-5)   +0x03 player count
 *   +0x04 u16 recv[4] (= gUnk_03004DA4, the SIOMULTI snapshot)
 *   +0x0D frame counter (s8)       +0x10 "ids dirty" flag
 *   +0x11 derived slot id + 1      +0x12 SIOCNT error bit (bit 6)
 *   +0x13 payload-mismatch flag    +0x14 ring-overflow flag
 *   +0x16 u16 running checksum     +0x18/+0x19 send/recv round cursors
 *   +0x1C  u16 ring[4][30]         +0x10C/+0x10D ring write/read cursors
 *   +0x110 u16 buf[4][4][30]       +0x4D0/+0x4D1 buf write/read cursors
 */

struct Pair { u32 a, b; };

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

extern u8 gLink[];      /* link work area */
extern vu16 gUnk_03004D38[];    /* receive staging, 4 halfwords */
extern u16 gShouldAdvanceLinkState[];     /* send/receive mailbox (LinkMain1) */
extern u16 gSendCmd[4];
extern u16 gRecvCmds[3][4];
extern u32 gLinkErrorMask;
extern u32 gLinkRecvVCount;
extern u32 gLinkPauseFrames;
extern u32 gLinkStatus;
extern u32 gChecksumAvailable;
extern u32 gUnk_03004D78;
extern u32 gSerialIntrCount;
extern vu16 gRecvNonzeroCheck;
extern u8 gLastRecvQueueCount;
extern vu16 gLocalPlayer;
extern vu16 gLinkIsMaster;
extern vu16 gLinkPlayerCount;
extern vu16 gPlayerCount;
extern vu16 gIntrMasterEnable;
extern vu16 gLinkCommand;
extern u8 gSendBufferEmpty;
extern void (*gIntrTable[])(void);
extern u32 gLinkDriverMode;
extern vu8 gMultiBootStruct[];
extern vu16 gUnk_04000006;      /* REG_VCOUNT */
extern vu16 gUnk_0400010C;      /* REG_TM3CNT_L */
extern vu16 gUnk_0400010E;      /* REG_TM3CNT_H */
extern vu16 gUnk_04000120;      /* REG_SIOMULTI0 */
extern vu16 gUnk_04000128;      /* REG_SIOCNT */
extern vu16 gUnk_0400012A;      /* REG_SIOMLT_SEND */
extern vu16 gUnk_04000208;      /* REG_IME */

void EnableSerial(void);
void DisableSerial(void);
void LinkMain1(u16 *a, u16 *b, u16 *c);
void RunFrame(void);

void Timer3Intr(void);
void SerialCB(void);
void StartTransfer(void);
void DoRecv(void);
void DoSend(void);
void StopTimer(void);
void SendRecvDone(void);

void DoSend(void)
{
    if (gLink[24] == 4) {
        gUnk_0400012A = *(u16 *)&gLink[22];
        if (gSendBufferEmpty == 0) {
            gLink[0x10D]--;
            gLink[0x10C]++;
            if (gLink[0x10C] > 29)
                gLink[0x10C] = 0;
        } else {
            gSendBufferEmpty = 0;
        }
    } else {
        if (gSendBufferEmpty == 0) {
            if (gLink[0x10D] == 0)
                gSendBufferEmpty = 1;
        }
        if (gSendBufferEmpty != 0) {
            gUnk_0400012A = 0;
        } else {
            gUnk_0400012A = *(u16 *)((gLink[0x10C] << 1)
                          + (((gLink[24] << 4) - gLink[24]) << 2)
                          + (u32)&gLink[28]);
        }
        gLink[24]++;
    }
}

void StopTimer(void)
{
    if (gLink[0] != 0) {
        gUnk_0400010E &= 0xFF7F;
        gUnk_0400010C = 0xFF7C;
    }
}

void SendRecvDone(void)
{
    if (gLink[25] == 4) {
        if ((u32)gLink == 0x53F3) {
            gLink[24] = 0;
            gLink[25] = 0;
        }
        gLink[24] = 0;
        gLink[25] = 0;
    } else if (gLink[0] != 0) {
        gUnk_0400010E |= 0x80;
    }
}

void ResetSendBuffer(void)
{
    u8 *buf;
    u16 fill;
    u8 i, j;

    gLink[0x10C] = 0;
    gLink[0x10D] = 0;
    buf = gLink + 28;
    fill = 0xEFFF;
    i = 0;
    do {
        j = 0;
        do {
            *(u16 *)(i * 60 + j * 2 + (u32)buf) = fill;
            j++;
        } while (j <= 29);
        i++;
    } while (i <= 3);
}

void ResetRecvBuffer(void)
{
    u8 *buf;
    u16 fill;
    u8 i, j, k;
    u32 t;

    gLink[0x4D0] = 0;
    gLink[0x4D1] = 0;
    k = 0;
    buf = gLink + 0x110;
    fill = 0xEFFF;
    do {
        i = 0;
        do {
            j = 0;
            do {
                t = j * 2;
                t += i * 60;
                t += k * 240;
                *(u16 *)(t + (u32)buf) = fill;
                j++;
            } while (j <= 29);
            i++;
        } while (i <= 3);
        k++;
    } while (k <= 3);
}

u32 ConnectLink(void)
{
    gUnk_03004D78 = 0;
    DisableSerial();
    gIntrMasterEnable = (gUnk_04000208 &= 0xFFFE, gUnk_04000208);
    gIntrTable[0] = SerialCB;
    gIntrTable[1] = Timer3Intr;
    gUnk_04000208 |= 1;
    gIntrMasterEnable = gUnk_04000208;
    EnableSerial();
    gLink[1] = 2;
    gLinkDriverMode = 1;
    gLinkStatus = 0;
    while (RunFrame(), gLink[1] != 4) {
        switch (gLink[1]) {
        case 1:
            *(u8 *)gShouldAdvanceLinkState = 1;
            break;
        case 2:
            if ((gLinkStatus & 0x20) != 0
             && gLink[3] == gMultiBootStruct[1]
             && (gLinkStatus & 0x40) == 0)
                *(u8 *)gShouldAdvanceLinkState = 1;
            break;
        }
        LinkMain1(gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
        if (++gUnk_03004D78 > 59)
            return 1;
    }
    return 0;
}

u32 IsLinkError(void)
{
    if ((gLinkStatus & gLinkErrorMask) != 0)
        return 1;
    return 0;
}
