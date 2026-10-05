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
extern u32 gLinkBlockAckCount;
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
extern vu16 gLinkRecvSnapshot[]; /* receive staging, 4 halfwords */
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

/* REG_IME as a symbol.  raw: src/link_block_main.c's GIME needs the symbol, the
 * io_reg.h REG_IME changes its allocation (lesson 3.523) */
extern vu16 gRegIme; /* REG_IME */


/* Functions (defined in the files named above each group). */

/* src/link_sync_random.c */
void LinkSyncRandom(void);

/* src/link_sync_clock.c */
void LinkSyncClock(void);

/* src/link_disconnect.c */
void DisconnectLink(void);

/* src/link_run_frames.c */
void FillSendCmd(void);
void UpdatePlayerKeys(void);
void RunLinkFrame(void);
void RunFrames(s32 count);
void RunFramesNoTasks(s32 count);
void RunLinkFrames(s32 count);
void RunFramesUntilFadeDone(void);
void RunFramesNoTasksUntilFadeDone(void);
void RunLinkFramesUntilFadeDone(void);
void ApplyBgLayout(u16 *p);
void SeedRandom(u32 seed);
u32 Random(void);
void IntToDigits(s16 n);
u16 BlendColor(u16 a, u16 b, u16 ratio);

/* src/link_setup_init.c */
void LinkSetupInit(void);

/* src/link_setup.c */
void LinkSetupStop(void);
void MultiBootSetParams(u8 *start, u8 *end);
void MultiBootInitWithParams(u8 *start, u8 *end);
void LinkSetupRequestStart(void);
void LinkSetupDetect(void);
void LinkSetupMultiCart(void);
void LinkSetupMultiBoot(void);

/* src/link_setup_intr_block.c */
void LinkSetupMain(u16 a);
void LinkSetupIntr(void);
u32 LinkBroadcastWordStep(void);
void LinkBlockAnnounce(u32 *src, u32 *dst, u32 size);
u32 LinkBlockHandshakeStep(void);
void LinkBlockStart(void);
void LinkBlockParentIntr(void);
void LinkBlockChildIntr(void);
u32 IsLinkBlockDone(void);

/* src/link_block_main.c */
void LinkBlockMain(void);
void MultiBootInit(struct MultiBootParam *mp);

/* src/link_multiboot_main.c */
u32 MultiBootMain(struct MultiBootParam *mp);

/* src/link_multiboot.c */
int MultiBootSend(struct MultiBootParam *mp, u16 data);
void MultiBootStartProbe(struct MultiBootParam *mp);
int MultiBootCheckComplete(struct MultiBootParam *mp);
int MultiBootHandShake(struct MultiBootParam *mp);
void MultiBootWaitCycles(s32 cycles);
void MultiBootWaitSendDone(void);

/* src/link_driver.c */
u32 IsInView(s16 x, s16 y);
void TaskFreezeOrThawOthers(u16 val, s32 idx);
void TaskRestoreSkipMask(u32 idx);
void TaskSaveSkipMask(u32 idx);
void InitLinkDriver(void);
void EnableSerial(void);
void DisableSerial(void);
void ResetSerial(void);
void CheckMasterOrSlave(void);
void InitTimer(void);

/* src/link_cmd_queue.c */
void EnqueueSendCmd(u16 *p);
void DequeueRecvCmds(u16 (*p)[4]);

/* src/link_vsync.c */
void LinkVSync(void);

/* src/link_timer3_intr.c */
void Timer3Intr(void);

/* src/link_serial_cb.c */
void SerialCB(void);

/* src/link_start_transfer.c */
void StartTransfer(void);

/* src/link_do_recv.c */
void DoRecv(void);

/* src/link_send_connect.c */
void DoSend(void);
void StopTimer(void);
void SendRecvDone(void);
void ResetSendBuffer(void);
void ResetRecvBuffer(void);
u32 ConnectLink(void);
u32 IsLinkError(void);

#endif /* GUARD_LINK_H */
