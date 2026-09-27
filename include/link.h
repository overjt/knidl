#ifndef GUARD_LINK_H
#define GUARD_LINK_H

#include "gba/types.h"

/* link.h: the RAM cells and ROM tables of the SIO multi-play link driver, the
   link-play session code and the SDK MultiBoot library (engine zone).  One
   declaration per symbol, with the type its consumers prove (issue #36 phase
   2, docs/header-conventions.md). */

/* AGB SDK MultiBootParam (0x4C bytes); the live instance is gMultiBootParam. */
struct MultiBootParam
{
    /*0x00*/ u32 system_work[5];
    /*0x14*/ u8 handshake_data;
    /*0x15*/ u8 padding;
    /*0x16*/ u16 handshake_timeout;
    /*0x18*/ u8 probe_count;
    /*0x19*/ u8 client_data[3];
    /*0x1C*/ u8 palette_data;
    /*0x1D*/ u8 response_bit;
    /*0x1E*/ u8 client_bit;
    /*0x1F*/ u8 reserved1;
    /*0x20*/ u8 *boot_srcp;
    /*0x24*/ u8 *boot_endp;
    /*0x28*/ u8 *masterp;   /* plain u8 * (lesson 3.481) */
    /*0x2C*/ u8 *reserved2[3];
    /*0x38*/ u32 system_work2[4];
    /*0x48*/ u8 sendflag;
    /*0x49*/ u8 probe_target_bit;
    /*0x4A*/ u8 check_wait;
    /*0x4B*/ u8 server_type;
};

/* EWRAM */
extern u32 *gLinkBlockSrc;
extern u8 gLinkBlockAcks[4];
extern vu16 gMultiBootDataRecv[4];
/* Link session sequencer state / frame counters (EWRAM). */
extern s32 gLinkBlockTimeout;
extern vs32 gLinkBlockState;
extern vs32 gLinkBlockWords;
extern u8 gLinkBroadcastAcks;
extern u32 *gLinkBlockDst;
extern u32 gUnk_0200EBB8;
extern vs32 gLinkBlockIndex;
extern vu8 gMultiBootStruct[];
extern struct MultiBootParam gMultiBootParam;
extern vu32 gLinkBlockParentChecksum;
extern s32 gLinkBlockFrames;
extern u32 gLinkBlockChecksum;
extern vs32 gLinkSetupMode;
extern u8 gUnk_0200EC4C;

/* IWRAM */
extern u32 gUnk_03001EFC;
extern u16 gLocalPlayer; /* this player's index */
extern s8 gUnk_030023A8[];
extern u16 gPlayerCount; /* number of players */
extern s16 gUnk_0300244C;
extern u8  gLastSendQueueCount;
extern u32 gLinkErrorMask;
extern u32 gLinkRecvVCount;
extern u32 gSendCmdFilled;
extern u8  gUnk_03004D34;
extern vu16 gUnk_03004D38[]; /* receive staging, 4 halfwords */
extern u8 gLastRecvQueueCount;
extern u16 gLinkSavedIme; /* saved REG_IME */
extern u16 gRecvCmds[4][4]; /* [0]=state [1]=keys held [2]=keys pressed */
extern u32 gLinkStatus;
extern u32 gChecksumAvailable;
extern u32 gUnk_03004D78;
extern u32 gSerialIntrCount; /* frame counter */
extern u16 gRecvNonzeroCheck;
extern u16 gSendNonzeroCheck; /* OR of the frame's send words */
extern u16 gShouldAdvanceLinkState[]; /* send/receive mailbox (LinkMain1) */
extern u16 gSendCmd[4]; /* link send buffer */
extern u8 gLink[]; /* link work area */
extern u8  gUnk_03005270;
extern u8  gUnk_03005278;
extern u8 gSendBufferEmpty;
/* Per-client probe response cache (3 halfwords). */
extern u16 gMultiBootClientData[];

/* ROM */
extern vu16 gUnk_04000006; /* REG_VCOUNT */
extern vu16 gUnk_0400010C; /* REG_TM3CNT_L */
extern vu16 gUnk_0400010E; /* REG_TM3CNT_H */
extern vu16 gUnk_04000120; /* REG_SIOMULTI0 */
extern vu16 gUnk_04000128; /* REG_SIOCNT */
extern vu16 gUnk_0400012A; /* REG_SIOMLT_SEND */
extern vu16 gUnk_04000208; /* REG_IME */

#endif /* GUARD_LINK_H */
