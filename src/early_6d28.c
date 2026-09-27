#include "gba/gba.h"
#include "global.h"
#include "main.h"

/* early_6d28.c (0x08006D28-0x08006E8B, issues #32/#63).
 *
 * The serial interrupt of the SIO multi-play link driver (pokeruby's SerialCB
 * with DoHandshake written inline; this ROM carries an older revision of the
 * library pokeruby ships as src/link.c, see src/early_6ac8.c), installed in
 * gIntrTable[0] by src/early_6464.c and src/early_7004.c.  It records the
 * player id from SIOCNT, then by link state gLink.unk01: in state 4
 * (connected) it records the SIOCNT error bit and runs the receive step
 * DoRecv, the send step DoSend and SendRecvDone; in state 2 it
 * runs the handshake: it sends 0x8FFF (master, unk10 == 1) or 0xCFF0, copies
 * the four SIOMULTI words into recv[], and on a master's 0x8FFF publishes the
 * player id, the master flag and the player count (gLocalPlayer,
 * gLinkIsMaster, gLinkPlayerCount/gPlayerCount) and moves to state 3 or 4;
 * otherwise it counts the players answering 0xCFF0-0xCFF3, stores the count
 * and derives the slot id unk11 from the lowest answer.  Every call bumps the
 * frame counter unk0D and gSerialIntrCount, and on the fourth frame copies the
 * receive-queue count unk4D1 to gLastRecvQueueCount.
 *
 * Matching notes (final campaign, lessons 3.490/3.491): the handshake reads
 * the snapshot through the struct, as pokeruby does (a `u16 *recv` local was
 * #63's shape), and the loop's else arm ends with a zero-code STAND-IN, a
 * dead `i = 4;` before its `break` (see the comment at the site): flow
 * deletes it, but until then it keeps jump.c's else-arm swap out of the
 * jump passes before register allocation, which is what the ROM's
 * allocation shows. */

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

struct Pair { u32 a, b; };

/* SIOCNT in multi-play mode, as pokeruby's SIO_MULTI_CNT. */
struct SioMultiCnt
{
    u16 baudRate:2;
    u16 si:1;
    u16 sd:1;
    u16 id:2;
    u16 error:1;
    u16 enable:1;
    u16 unused_11_8:4;
    u16 mode:2;
    u16 intrEnable:1;
    u16 unused_15:1;
    u16 data;
};

/* Not from link.h: this file's view of gLink differs (lesson 3.517). */
extern struct Link gLink;
extern vu16 gUnk_04000120;      /* REG_SIOMULTI0 */
extern vu16 gUnk_04000128;      /* REG_SIOCNT */
extern vu16 gUnk_0400012A;      /* REG_SIOMLT_SEND */
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u32 gSerialIntrCount;
extern u8 gLastRecvQueueCount;
void DoRecv(void);
void DoSend(void);
void SendRecvDone(void);

/* The serial interrupt of the link session (pokeruby's SerialCB, with
 * DoHandshake written inline): state 4 runs the receive/send step, state 2
 * the connect handshake. */
void SerialCB(void)
{
    u8 i;
    u8 playerCount = 0;
    u16 minRecv = 0xFFFF;

    gLink.unk02 = ((struct SioMultiCnt *)&gUnk_04000128)->id;

    switch (gLink.unk01)
    {
    case 4:
        gLink.unk12 = ((struct SioMultiCnt *)&gUnk_04000128)->error;
        DoRecv();
        DoSend();
        SendRecvDone();
        break;
    case 2:
        if (gLink.unk10 == 1)
            gUnk_0400012A = 0x8FFF;
        else
            gUnk_0400012A = 0xCFF0;

        *(struct Pair *)gLink.recv = *(struct Pair *)&gUnk_04000120;
        gLink.unk10 = 0;

        if (gLink.recv[0] == 0x8FFF)
        {
            gLocalPlayer = gLink.unk02;
            gLinkIsMaster = gLink.unk00;
            gLinkPlayerCount = gLink.count;
            gPlayerCount = gLinkPlayerCount;
            if (gLink.unk00)
                gLink.unk01 = 3;
            else
                gLink.unk01 = 4;
            break;
        }

        for (i = 0; i < 4; i++)
        {
            if ((gLink.recv[i] & ~3) == 0xCFF0)
            {
                playerCount++;
                if (minRecv > gLink.recv[i] && gLink.recv[i] != 0)
                    minRecv = gLink.recv[i];
            }
            else
            {
                if (gLink.recv[i] != 0xFFFF)
                    playerCount = 0;
                /* Dead store (flow deletes it, zero bytes), but it keeps
                 * this arm's skip label alive until reload, so jump.c's
                 * "if (foo) bar; else break;" swap only happens in jump2:
                 * global alloc sees the ++ before this reset and
                 * update_equiv_regs doubles playerCount's live length once,
                 * not twice (the ROM's r4/r5/r6 order). */
                i = 4;
                break;
            }
        }

        if (gLink.unk10 == 0)
            gLink.count = playerCount;

        if (gLink.count > 1)
            gLink.unk11 = (minRecv & 3) + 1;
        else
            gLink.unk11 = 0;

        gLink.unk10 = 0;
        break;
    }

    gLink.unk0D++;
    gSerialIntrCount++;

    if ((s8)gLink.unk0D == 4)
        gLastRecvQueueCount = gLink.unk4D1;
}
