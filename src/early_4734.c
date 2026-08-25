#include "gba/gba.h"
#include "global.h"

/* Link boot sequencer + the AGB SDK MultiBoot client library
 * (0x08004734-0x08004FEB, issue #32 batch E2).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaves sub_08004968 / sub_08004e9c / sub_08004f98 end in a bare `bx lr`;
 * agbcc unconditionally emits `push {lr}` / `pop {r0}; bx r0` even for leaves
 * (docs/lessons-learned.md 3.18).
 *
 * 0x08004968-0x08004FEB is the AGB SDK multiboot library (the same code
 * pokeemerald ships as src/multiboot.c).  Semantic names, in ROM order:
 *   sub_08004968  MultiBootInit
 *   sub_08004984  MultiBootMain
 *   sub_08004d6c  MultiBootSend           (static)
 *   sub_08004db4  MultiBootStartProbe
 *   sub_08004dd8  MultiBootStartMaster
 *   sub_08004e9c  MultiBootCheckComplete
 *   sub_08004eac  MultiBootHandShake      (static)
 *   sub_08004f98  MultiBootWaitCycles     (static)
 *   sub_08004fb0  MultiBootWaitSendDone   (static)
 * sub_08004734 is game code: the 5-step link/multiboot session sequencer
 * driven by the counter at 0x0200EBA8.
 *
 * STATUS: 9 of the 10 functions are byte-exact.  sub_08004984
 * (MultiBootMain) is NOT matched: same size (1000 bytes) and the same
 * instruction sequence, but 534 bytes differ on register naming.  The whole
 * function's allocation is shifted by exactly one hard register (ROM has
 * t=r5 / mp=r7 / &check_wait=sl, this candidate has t=r4 / mp=r6 /
 * &check_wait=r9) because gcc keeps the 0x04000120 base in a callee-saved
 * register across case 0's two loops while the ROM re-loads the pool word at
 * every mention.  See the batch report for the full analysis.
 */

/* AGB SDK MultiBootParam (0x4C bytes); the live instance is gUnk_0200EBF0. */
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
    /*0x28*/ vu8 *masterp;
    /*0x2C*/ u8 *reserved2[3];
    /*0x38*/ u32 system_work2[4];
    /*0x48*/ u8 sendflag;
    /*0x49*/ u8 probe_target_bit;
    /*0x4A*/ u8 check_wait;
    /*0x4B*/ u8 server_type;
};

/* REG_SIOMULTI0..3 as an array.  UNRESOLVED (see the report): sub_08004eac
 * only reproduces the ROM through the cast literal (gcc rematerialises the
 * pool word at every mention), while sub_08004984 only reproduces the ROM's
 * instruction *count* through a symbol reference.  Both spell 0x04000120. */
#define SIOMULTI  ((vu16 *)REG_ADDR_SIOMULTI0)
extern vu16 gUnk_04000120[];
#define SIOMULTI2 gUnk_04000120

/* Per-client probe response cache (3 halfwords). */
extern u16 gUnk_03006920[];

/* Link session sequencer state / frame counters (EWRAM). */
extern s32 gUnk_0200EBA0;
extern s32 gUnk_0200EBA4;
extern vs32 gUnk_0200EBA8;
extern s32 gUnk_0200EBAC;
extern s32 gUnk_0200EBBC;
extern s32 gUnk_0200EC40;
extern vu16 gUnk_03000018;      /* REG_IE shadow */
extern vu16 gUnk_03001EF8;      /* REG_IME shadow */
extern vu16 gUnk_03001F38;      /* link-mode flag */

/* REG_IME must be reached through a SYMBOL here, not the io_reg.h cast
 * literal: with the literal, cse.c derives 0x04000208 from the still-live
 * 0x0400010C (REG_TM3CNT_L) as `adds r1,#252`, which removes one address
 * pseudo and shifts the whole register allocation by one.  The ROM pools
 * 0x04000208 on its own at every mention. */
extern vu16 gUnk_04000208;
#define GIME gUnk_04000208

/* The MultiBoot SWI thunk returns an error code; syscall.h declares it u8,
 * but the ROM keeps the value untruncated, i.e. the original prototype was
 * int-returning.  Alias it rather than fight the header (see report). */
extern int MultiBootSvc(struct MultiBootParam *mp) asm("MultiBoot");

int sub_08004d6c(struct MultiBootParam *mp, u16 data);
int sub_08004eac(struct MultiBootParam *mp);
void sub_08004f98(s32 cycles);
void sub_08004fb0(void);
void sub_08004968(struct MultiBootParam *mp);
void sub_08004db4(struct MultiBootParam *mp);
int sub_08004e9c(struct MultiBootParam *mp);


/*FN sub_08004734*/
void sub_08004734(void)
{
    switch (gUnk_0200EBA8)
    {
    case 0:
        gUnk_0200EBA4 = ((((gUnk_0200EBAC << 2) >> 2) * 0x10B3) >> 18) + 9;
        gUnk_0200EBA8++;
        break;
    case 1:
        if (gUnk_03001F38 != 0)
        {
            if (gUnk_0200EC40 <= 5)
                break;
        }
        else
        {
            REG_SIOCNT = 0x1000;
        }
        REG_SIODATA32 = 0;
        REG_IF |= 0xC0;
        if (gUnk_03001F38 != 0)
        {
            REG_SIOCNT |= 0x80;
            REG_TM3CNT_L = 0xF318;
            REG_TM3CNT_H = 0xC0;
            gUnk_03001EF8 = GIME = GIME & 0xFFFE;
            REG_IE = gUnk_03000018 = gUnk_03000018 | 0x40;
        }
        else
        {
            REG_SIOCNT |= 0x4080;
            gUnk_03001EF8 = GIME = GIME & 0xFFFE;
            REG_IE = gUnk_03000018 = gUnk_03000018 | 0x80;
        }
        gUnk_03001EF8 = GIME = GIME | 1;
        gUnk_0200EC40 = 0;
        gUnk_0200EBA8++;
        break;
    case 2:
        if (gUnk_0200EBBC < gUnk_0200EBAC && gUnk_0200EC40 < gUnk_0200EBA4)
            break;
        gUnk_0200EBA8++;
        break;
    case 3:
        gUnk_0200EBA8++;
        break;
    case 4:
        gUnk_03001EF8 = GIME = GIME & 0xFFFE;
        REG_IE = gUnk_03000018 = gUnk_03000018 & 0xFF3F;
        REG_SIOCNT = 0x1000;
        REG_SIOCNT = 0x2000;
        REG_SIOCNT |= 0x4003;
        REG_TM3CNT_H = 0;
        REG_IF |= 0xC0;
        gUnk_03001EF8 = GIME = GIME | 1;
        gUnk_0200EBA8 = 0x9999;
        gUnk_0200EBA0 = 0;
        break;
    }
    gUnk_0200EC40++;
}


/*FN sub_08004968*/
void sub_08004968(struct MultiBootParam *mp)
{
    mp->client_bit = 0;
    mp->probe_count = 0;
    mp->response_bit = 0;
    mp->check_wait = 15;
    mp->sendflag = 0;
    mp->handshake_timeout = 0;
}

