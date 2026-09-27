#include "gba/gba.h"
#include "global.h"

/* Link boot sequencer + the AGB SDK MultiBoot client library
 * (0x08004734-0x08004FEB, issue #32 batch E2).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaves MultiBootInit / MultiBootCheckComplete / MultiBootWaitCycles end in a bare `bx lr`;
 * agbcc unconditionally emits `push {lr}` / `pop {r0}; bx r0` even for leaves
 * (docs/lessons-learned.md 3.18).
 *
 * 0x08004968-0x08004FEB is the AGB SDK multiboot library (the same code
 * pokeemerald ships as src/multiboot.c).  Semantic names, in ROM order:
 *   MultiBootInit  MultiBootInit
 *   MultiBootMain  MultiBootMain
 *   MultiBootSend  MultiBootSend           (static)
 *   MultiBootStartProbe  MultiBootStartProbe
 *   MultiBootStartMaster  MultiBootStartMaster
 *   MultiBootCheckComplete  MultiBootCheckComplete
 *   MultiBootHandShake  MultiBootHandShake      (static)
 *   MultiBootWaitCycles  MultiBootWaitCycles     (static)
 *   MultiBootWaitSendDone  MultiBootWaitSendDone   (static)
 * sub_08004734 is game code: the 5-step link/multiboot session sequencer
 * driven by the counter at 0x0200EBA8.
 *
 * STATUS: 9 of the 10 functions are byte-exact.  MultiBootMain
 * (MultiBootMain) is NOT matched: same size (1000 bytes) and the same
 * instruction sequence, but 534 bytes differ on register naming.  The whole
 * function's allocation is shifted by exactly one hard register (ROM has
 * t=r5 / mp=r7 / &check_wait=sl, this candidate has t=r4 / mp=r6 /
 * &check_wait=r9) because gcc keeps the 0x04000120 base in a callee-saved
 * register across case 0's two loops while the ROM re-loads the pool word at
 * every mention.  See the batch report for the full analysis.
 */

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

/* REG_SIOMULTI0..3 as an array: MultiBootHandShake reads the io_reg.h constant
 * (gcc rematerialises the pool word at every mention, lesson 3.482). */
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
extern vu16 gIntrEnable;      /* REG_IE shadow */
extern vu16 gIntrMasterEnable;      /* REG_IME shadow */
extern vu16 gLinkIsMaster;      /* link-mode flag */

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

int MultiBootSend(struct MultiBootParam *mp, u16 data);
int MultiBootHandShake(struct MultiBootParam *mp);
void MultiBootWaitCycles(s32 cycles);
void MultiBootWaitSendDone(void);
void MultiBootInit(struct MultiBootParam *mp);
void MultiBootStartProbe(struct MultiBootParam *mp);
int MultiBootCheckComplete(struct MultiBootParam *mp);


/*FN MultiBootSend*/
int MultiBootSend(struct MultiBootParam *mp, u16 data)
{
    u32 t;

    t = REG_SIOCNT & 0x8C;
    if (t != 8)
    {
        MultiBootInit(mp);
        t ^= 8;
        return t;
    }
    REG_SIOMLT_SEND = data;
    REG_SIOCNT |= 0x80;
    mp->sendflag = 1;
    return 0;
}


/*FN MultiBootStartProbe*/
void MultiBootStartProbe(struct MultiBootParam *mp)
{
    if (mp->probe_count != 0)
    {
        MultiBootInit(mp);
        return;
    }
    mp->check_wait = 0;
    mp->client_bit = 0;
    mp->probe_count = 1;
}


/*FN MultiBootStartMaster*/
void MultiBootStartMaster(struct MultiBootParam *mp, u8 *srcp, int length,
                          u8 palette_color, s8 palette_speed)
{
    int n;

    if (mp->probe_count != 0)
    {
        MultiBootInit(mp);
        return;
    }
    if (mp->client_bit == 0)
    {
        MultiBootInit(mp);
        return;
    }
    if (mp->check_wait != 0)
    {
        MultiBootInit(mp);
        return;
    }
    mp->boot_srcp = srcp;
    length = (length + 15) & ~15;
    if (length < 0x100 || length > 0x40000)
    {
        MultiBootInit(mp);
        return;
    }
    mp->boot_endp = srcp + length;
    switch (palette_speed)
    {
    case -4:
    case -3:
    case -2:
    case -1:
        n = (palette_color << 3) | (3 - palette_speed);
        break;
    case 0:
        n = palette_color | 0x38;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        n = (palette_color << 3) | (palette_speed - 1);
        break;
    }
    n &= 0x3F;
    mp->palette_data = (n << 1) | 0x81;
    mp->probe_count = 0xD0;
}


/*FN MultiBootCheckComplete*/
int MultiBootCheckComplete(struct MultiBootParam *mp)
{
    if (mp->probe_count == 0xE9)
        return 1;
    return 0;
}


/*FN MultiBootHandShake*/
int MultiBootHandShake(struct MultiBootParam *mp)
{
    int i;
    u32 v;

    switch (mp->probe_count)
    {
    case 0xE0:
      reset:
        mp->probe_count = 0xE1;
        mp->system_work[1] = 0;
        mp->system_work[0] = 0x100000;
        return MultiBootSend(mp, 0);
    default:
        for (i = 3; i != 0; i--)
        {
            v = SIOMULTI[i];
            if ((mp->client_bit >> i) & 1)
            {
                if (v != mp->system_work[1])
                    goto reset;
            }
        }
        mp->probe_count++;
        mp->system_work[1] = (u16)mp->system_work[0];
        if (mp->system_work[0] == 0)
        {
            mp->system_work[1] = mp->masterp[0xAC] | (mp->masterp[0xAD] << 8);
            mp->system_work[0] = mp->system_work[1] << 5;
        }
        mp->system_work[0] >>= 5;
      send:
        return MultiBootSend(mp, *(u16 *)&mp->system_work[0]);
    case 0xE7:
    case 0xE8:
        for (i = 3; i != 0; i--)
        {
            v = SIOMULTI[i];
            if ((mp->client_bit >> i) & 1)
            {
                if (v != mp->system_work[1])
                    goto fail;
            }
        }
        mp->probe_count++;
        if (mp->probe_count == 0xE9)
            goto done;
        mp->system_work[1] = mp->system_work[0] = mp->masterp[0xAE] | (mp->masterp[0xAF] << 8);
        goto send;
    }
  fail:
    MultiBootInit(mp);
    return 0x71;
  done:
    return 0;
}


/*FN MultiBootWaitCycles*/
void MultiBootWaitCycles(s32 cycles)
{
    asm("mov r2, pc");
    asm("lsr r2, #24");
    asm("mov r1, #12");
    asm("cmp r2, #0x02");
    asm("beq MultiBootWaitCyclesLoop");

    asm("mov r1, #13");
    asm("cmp r2, #0x08");
    asm("beq MultiBootWaitCyclesLoop");

    asm("mov r1, #4");

    asm("MultiBootWaitCyclesLoop:");
    asm("sub r0, r1");
    asm("bgt MultiBootWaitCyclesLoop");
}


/*FN MultiBootWaitSendDone*/
void MultiBootWaitSendDone(void)
{
    s32 i;

    i = 0;
    if (REG_SIOCNT & 0x80) {
        do {
            if (++i > 31068)
                break;
        } while (REG_SIOCNT & 0x80);
    }
    MultiBootWaitCycles(600);
}

