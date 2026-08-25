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
 * sub_08007004 lives inside the declared 0x21C of sub_08006e9c, and
 * sub_080070e8 inside the declared 0x4A of sub_080070b8 -- see the report;
 * symbols.csv's sub_08007102/0x22 entry is a mis-split of sub_080070e8.
 *
 *   sub_08006d18  serial IRQ: stop the timeout timer, re-arm SIOCNT.
 *   sub_08006d28  VBlank IRQ for the link session: snapshots SIOCNT, then
 *                 either runs the transfer step (state 4) or the connect/ID
 *                 handshake (state 2).            [NOT MATCHING - see report]
 *   sub_08006e8c  re-arm the SIOCNT start bit.
 *   sub_08006e9c  per-frame receive step: copies the four SIOMULTI words to
 *                 gUnk_03004D38 and folds them into the per-player buffer.
 *                                                 [NOT MATCHING - see report]
 *   sub_08007004  send step: pushes the next ring slot into SIOMLT_SEND.
 *   sub_080070b8  stop the link timeout timer (TM3).
 *   sub_080070e8  end-of-round bookkeeping / re-arm.
 *   sub_08007124  clear the 4x30 halfword ring at +0x1C and its two cursors.
 *   sub_08007174  clear the 4x4x30 halfword buffer at +0x110 and its cursors.
 *   sub_080071dc  blocking link bring-up loop; returns 1 on timeout (60
 *                 frames without reaching state 4), 0 on success.
 *   sub_080072e0  poll gUnk_03004D70 against the mask in gUnk_03004D24.
 *
 * gUnk_03004DA0 is the link work area (0x4D2 bytes, ending just below
 * gUnk_03005274).  Byte offsets used here:
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

extern u8 gUnk_03004DA0[];      /* link work area */
extern vu16 gUnk_03004D38[];    /* receive staging, 4 halfwords */
extern u16 gUnk_03004D88[];     /* send/receive mailbox (sub_08006914) */
extern u16 gUnk_03004D90[4];
extern u16 gUnk_03004D50[3][4];
extern u32 gUnk_03004D24;
extern u32 gUnk_03004D28;
extern u32 gUnk_03004D30;
extern u32 gUnk_03004D70;
extern u32 gUnk_03004D74;
extern u32 gUnk_03004D78;
extern u32 gUnk_03004D7C;
extern vu16 gUnk_03004D80;
extern u8 gUnk_03004D40;
extern vu16 gUnk_03002360;
extern vu16 gUnk_03001F38;
extern vu16 gUnk_0300243C;
extern vu16 gUnk_030023AC;
extern vu16 gUnk_03001EF8;
extern vu16 gUnk_03005274;
extern u8 gUnk_0300527C;
extern void (*gUnk_030004B0[])(void);
extern u32 gUnk_0200EBA0;
extern vu8 gUnk_0200EBC0[];
extern vu16 gUnk_04000006;      /* REG_VCOUNT */
extern vu16 gUnk_0400010C;      /* REG_TM3CNT_L */
extern vu16 gUnk_0400010E;      /* REG_TM3CNT_H */
extern vu16 gUnk_04000120;      /* REG_SIOMULTI0 */
extern vu16 gUnk_04000128;      /* REG_SIOCNT */
extern vu16 gUnk_0400012A;      /* REG_SIOMLT_SEND */
extern vu16 gUnk_04000208;      /* REG_IME */

void sub_08006724(void);
void sub_08006868(void);
void sub_08006914(u16 *a, u16 *b, u16 *c);
void sub_080022e4(void);

void sub_08006d18(void);
void sub_08006d28(void);
void sub_08006e8c(void);
void sub_08006e9c(void);
void sub_08007004(void);
void sub_080070b8(void);
void sub_080070e8(void);

void sub_08006d18(void)
{
    sub_080070b8();
    sub_08006e8c();
}

