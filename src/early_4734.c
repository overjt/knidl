#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "link.h"

/* The link block-transfer step + the AGB SDK MultiBoot client library
 * (0x08004734-0x08004FEB, issue #32 batch E2).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaves MultiBootInit / MultiBootCheckComplete / MultiBootWaitCycles end in a
 * bare `bx lr`;
 * agbcc unconditionally emits `push {lr}` / `pop {r0}; bx r0` even for leaves
 * (docs/lessons-learned.md 3.18).
 *
 * 0x08004968-0x08004FEB is the AGB SDK multiboot library (the same code
 * pokeemerald ships as src/multiboot.c).  Semantic names, in ROM order:
 *   0x08004968  MultiBootInit
 *   0x08004984  MultiBootMain
 *   0x08004D6C  MultiBootSend           (static)
 *   0x08004DB4  MultiBootStartProbe
 *   0x08004DD8  MultiBootStartMaster
 *   0x08004E9C  MultiBootCheckComplete
 *   0x08004EAC  MultiBootHandShake      (static)
 *   0x08004F98  MultiBootWaitCycles     (static)
 *   0x08004FB0  MultiBootWaitSendDone   (static)
 * LinkBlockMain is game code: the 5-step block-transfer state machine
 * (32-bit normal-mode SIO after the 0x5500/0x5501 handshake, run from
 * EndFrame while gLinkDriverMode is 2), not multiboot, driven by the
 * state gLinkBlockState (0x0200EBA8).
 *
 * STATUS: 9 of the 10 functions are byte-exact.
 * MultiBootMain is NOT matched: same size (1000 bytes) and the same
 * instruction sequence, but 534 bytes differ on register naming.  The whole
 * function's allocation is shifted by exactly one hard register (ROM has
 * t=r5 / mp=r7 / &check_wait=sl, this candidate has t=r4 / mp=r6 /
 * &check_wait=r9) because gcc keeps the 0x04000120 base in a callee-saved
 * register across case 0's two loops while the ROM re-loads the pool word at
 * every mention.  See the batch report for the full analysis.
 */

/* REG_SIOMULTI0..3 as an array: the io_reg.h constant (gcc rematerialises
 * the pool word at every mention, lesson 3.482). */
#define SIOMULTI  ((vu16 *)REG_ADDR_SIOMULTI0)

/* REG_IME must be reached through a SYMBOL here, not the io_reg.h cast
 * literal: with the literal, cse.c derives 0x04000208 from the still-live
 * 0x0400010C (REG_TM3CNT_L) as `adds r1,#252`, which removes one address
 * pseudo and shifts the whole register allocation by one.  The ROM pools
 * 0x04000208 on its own at every mention.
 * raw: stays a symbol, REG_IME changes this file's allocation (lesson 3.523) */
#define GIME gUnk_04000208

/* The MultiBoot SWI thunk returns an error code; syscall.h declares it u8,
 * but the ROM keeps the value untruncated, i.e. the original prototype was
 * int-returning.  Alias it rather than fight the header (see report). */
extern int MultiBootSvc(struct MultiBootParam *mp) asm("MultiBoot");

/*FN LinkBlockMain*/
void LinkBlockMain(void)
{
    switch (gLinkBlockState)
    {
    case 0:
        gLinkBlockTimeout = ((((gLinkBlockWords << 2) >> 2) * 0x10B3) >> 18) + 9;
        gLinkBlockState++;
        break;
    case 1:
        if (gLinkIsMaster != 0)
        {
            if (gLinkBlockFrames <= 5)
                break;
        }
        else
        {
            REG_SIOCNT = 0x1000;
        }
        REG_SIODATA32 = 0;
        REG_IF |= 0xC0;
        if (gLinkIsMaster != 0)
        {
            REG_SIOCNT |= 0x80;
            REG_TM3CNT_L = 0xF318;
            REG_TM3CNT_H = 0xC0;
            gIntrMasterEnable = GIME = GIME & 0xFFFE;
            REG_IE = gIntrEnable = gIntrEnable | 0x40;
        }
        else
        {
            REG_SIOCNT |= 0x4080;
            gIntrMasterEnable = GIME = GIME & 0xFFFE;
            REG_IE = gIntrEnable = gIntrEnable | 0x80;
        }
        gIntrMasterEnable = GIME = GIME | 1;
        gLinkBlockFrames = 0;
        gLinkBlockState++;
        break;
    case 2:
        if (gLinkBlockIndex < gLinkBlockWords && gLinkBlockFrames < gLinkBlockTimeout)
            break;
        gLinkBlockState++;
        break;
    case 3:
        gLinkBlockState++;
        break;
    case 4:
        gIntrMasterEnable = GIME = GIME & 0xFFFE;
        REG_IE = gIntrEnable = gIntrEnable & 0xFF3F;
        REG_SIOCNT = 0x1000;
        REG_SIOCNT = 0x2000;
        REG_SIOCNT |= 0x4003;
        REG_TM3CNT_H = 0;
        REG_IF |= 0xC0;
        gIntrMasterEnable = GIME = GIME | 1;
        gLinkBlockState = 0x9999;
        gLinkDriverMode = 0;
        break;
    }
    gLinkBlockFrames++;
}

/*FN MultiBootInit*/
void MultiBootInit(struct MultiBootParam *mp)
{
    mp->client_bit = 0;
    mp->probe_count = 0;
    mp->response_bit = 0;
    mp->check_wait = 15;
    mp->sendflag = 0;
    mp->handshake_timeout = 0;
}
