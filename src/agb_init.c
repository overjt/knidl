#include "gba/gba.h"
#include "global.h"

/* AgbInit (0x08000310-0x080008E7, issue #28): boot-time initializer called
 * from crt0 before main.  Clears EWRAM/IWRAM/VRAM/OAM/palette via CpuFastSet,
 * copies the IRQ handler table and the master ISR into IWRAM via CpuSet, then
 * initializes the I/O register block through IWRAM shadow cells
 * (rom-map.md §4).  The function is ~1KB, so the compiler emits its literal
 * pool behind a pool-skip branch after the epilogue (0x08000700-0x080008E7);
 * the "function" the symbol census saw at 0x08000700 is that branch, not code.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork; see docs/lessons-learned.md):
 *  - The IWRAM/EWRAM cells are unnamed state (gUnk_<address>), defined as
 *    absolute symbols in asm/rom_syms.s via tools/split_config.json
 *    "data_symbols".  They must stay symbolic: gcc derives nearby *constant*
 *    addresses with add/sub chains but never across distinct symbols, which
 *    is how the ROM's pool layout proves these were named globals.
 *  - `REG = shadow = value` chained assignments: the volatile inner cell is
 *    re-read for the outer store; non-volatile inners forward the value.
 *  - Volatile *indexed* stores (array element / struct field) emit a dead
 *    pre-read; volatile pointer-deref and bare-symbol stores do not.
 *  - The zeroA/zeroB/zeroC locals pin the per-region zero registers: each is
 *    block-local (single region, allocated r4 by local-alloc), and re-using
 *    one variable across regions would change the whole allocation. */

extern vu16 gUnk_03001004;
extern vu32 gUnk_03000FA0;
extern vu16 gUnk_03001EA4;
extern vu16 gUnk_03000FAC;
extern vu16 gUnk_03000FD0;
extern vu16 gUnk_03001EC4;
extern u16 gUnk_03000498[4];
extern u32 gUnk_03000F88;
extern u32 gUnk_03000FC4;
extern u32 gUnk_03000020[4];
extern vu16 gUnk_03001EF4;
extern vu16 gUnk_03000B70;
extern vu16 gUnk_03000038;
extern vu16 gUnk_03000FC8;
extern vu16 gUnk_03001ECC;
extern vu16 gUnk_03001670;
extern vu16 gUnk_03001EB8[4];
extern vu16 gUnk_03000F98[4];
extern vu32 gUnk_03000FB4;
extern vu32 gUnk_03000AF4;
extern vu32 gUnk_03000F90;
extern vu32 gUnk_030004A0;
extern vu32 gUnk_03000014;
extern vu16 gUnk_03001ED8;
extern vu16 gUnk_03001E9C;
extern vu16 gUnk_03001188;
extern vu16 gUnk_03000B14;
extern vu16 gUnk_03000B10;
extern vu16 gUnk_03001EB4;
extern u32 gUnk_03000010;
extern u32 gUnk_0300117C;
extern u32 gUnk_03000FC0;
extern u32 gUnk_03001EE0;
extern u32 gUnk_03001E94;
extern u32 gUnk_03000F8C;
extern u32 gUnk_03000FA8;
extern u32 gUnk_03000B78;
extern vu16 gUnk_03000FD4;
extern vu16 gUnk_03001018;
extern vu16 gUnk_03000044;
extern vu16 gUnk_03000AFC;
extern vu8 gUnk_03000B18;
extern vu8 gUnk_03001ED0;
extern vu8 gUnk_03000F7C;
extern vu8 gUnk_03001010;
extern vu8 gUnk_03001EB0;
extern vu8 gUnk_03001000;
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03000040;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03001EAC;
extern vu16 gUnk_03001EEC;
extern vu16 gUnk_0300100C;
extern vu16 gUnk_03000FD8;
extern vu16 gUnk_03001EA0;
extern vu32 gUnk_03001EA8;
extern vu16 gUnk_03001EE4;
extern vu16 gUnk_03000B0C;
extern vu16 gUnk_03000FCC;
extern vu16 gUnk_03000FBC;
extern vs16 gUnk_03000490;
extern vu16 gUnk_03001EDC;
extern vu16 gUnk_03000AF8;
extern u16 gUnk_03000F80[4];
extern vu8 gUnk_03001674[4];
extern vu8 gUnk_03001180[4];
extern vu8 gUnk_0300001C[4];
extern vu32 gUnk_03000FB0;
extern u32 gUnk_03000FA4;
extern u32 gUnk_0300003C;
extern u32 gUnk_03000B74;
extern u16 gUnk_03001014;
extern vu16 gUnk_03001170;
extern vu16 gUnk_03000B7C;
extern vu16 gUnk_03001178;
extern vu16 gUnk_03001020;
extern vu16 gUnk_03000B20;
extern vu16 gUnk_03001008;
extern vu16 gUnk_03000B24;
extern vu16 gUnk_03000018;
extern vu16 gUnk_03001EF8;
extern vu32 gUnk_0200EC48;
extern vu32 gUnk_0200EBA0;
extern u16 gUnk_030023AC;
extern u16 gUnk_0300243C;
struct Unk_030023A8
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
};
extern struct Unk_030023A8 gUnk_030023A8;
extern vs16 gUnk_0300244C;
extern vu32 gUnk_03000B00;

extern void m4aSoundInit(void);
extern void sub_08001a0c(void);
extern void sub_08001b08(void);
extern void sub_08002028(void);
extern void sub_08002268(void);
extern void sub_08002eac(u32 arg);

void AgbInit(void)
{
    u32 zeroWords[5];
    u16 zeroHalf0;
    u16 zeroHalf1;
    u32 zeroA;
    u32 zeroB;
    u16 zeroC;
    u16 zeroHwA;
    u8 zeroByteB;
    u16 *fillA;
    u16 *fillB;
    s32 i;

    zeroA = 0;
    zeroWords[0] = zeroA;
    CpuFastSet(&zeroWords[0], (u32 *)0x02000000, 0x01010000);
    zeroWords[1] = zeroA;
    CpuFastSet(&zeroWords[1], (u32 *)0x03000010, 0x01001EDC);
    zeroWords[2] = zeroA;
    CpuFastSet(&zeroWords[2], (u32 *)0x06000000, 0x01006000);
    zeroWords[3] = zeroA;
    CpuFastSet(&zeroWords[3], (u32 *)0x07000000, 0x01000100);
    zeroWords[4] = zeroA;
    CpuFastSet(&zeroWords[4], (u32 *)0x05000000, 0x01000100);

    gUnk_03001004 |= 0x4014;
    REG_WAITCNT = gUnk_03001004;

    gUnk_03000FA0 = zeroA;

    CpuSet((const void *)0x080CFDE8, (void *)0x030004B0, 28);
    CpuSet((const void *)0x08000108, (void *)0x03001030, 160);
    INTR_VECTOR = (void (*)(void))0x03001030;

    gUnk_03000FAC = gUnk_03001EA4 = zeroA;
    gUnk_03001EC4 = gUnk_03000FD0 = zeroA;

    gUnk_03000498[3] = zeroA;
    gUnk_03000498[2] = zeroA;
    gUnk_03000498[1] = zeroA;
    gUnk_03000498[0] = zeroA;

    gUnk_03000FC4 = gUnk_03000F88 = 0x03000B80;

    gUnk_03000020[3] = zeroA;
    gUnk_03001EF4 = zeroA;
    gUnk_03000B70 = zeroA;
    gUnk_03000038 = zeroA;
    gUnk_03000FC8 = 14;
    gUnk_03001ECC = 4;
    gUnk_03001670 = gUnk_03001ECC;

    /* Computed here (matching the ROM), used for the CpuSet fills below;
     * both live across the calls in r8/r9. */
    fillA = &zeroHalf0;
    fillB = &zeroHalf1;

    zeroHwA = 0;
    for (i = 0; i < 4; i++)
        gUnk_03000F98[i] = gUnk_03001EB8[i] = zeroHwA;

    gUnk_03000FB4 = zeroB = 0;
    gUnk_03000AF4 = zeroB;
    gUnk_03000F90 = zeroB;
    gUnk_030004A0 = zeroB;
    gUnk_03000014 = zeroB;

    gUnk_03001004 = REG_WAITCNT & 0x8000;

    REG_DISPCNT = gUnk_03001ED8 = 128;

    zeroByteB = 0;
    REG_DISPSTAT = gUnk_03001E9C = zeroB;
    REG_BG0CNT = gUnk_03001188 = zeroB;
    REG_BG1CNT = gUnk_03000B14 = zeroB;
    REG_BG2CNT = gUnk_03000B10 = zeroB;
    REG_BG3CNT = gUnk_03001EB4 = zeroB;

    REG_BG0VOFS = gUnk_03000010 = zeroB;
    REG_BG0HOFS = gUnk_0300117C = zeroB;
    REG_BG1VOFS = gUnk_03000FC0 = zeroB;
    REG_BG1HOFS = gUnk_03001EE0 = zeroB;
    REG_BG2VOFS = gUnk_03001E94 = zeroB;
    REG_BG2HOFS = gUnk_03000F8C = zeroB;
    REG_BG3VOFS = gUnk_03000FA8 = zeroB;
    REG_BG3HOFS = gUnk_03000B78 = zeroB;

    REG_WIN0H = gUnk_03000FD4 = zeroB;
    REG_WIN1H = gUnk_03001018 = zeroB;
    REG_WIN0V = gUnk_03000044 = zeroB;
    REG_WIN1V = gUnk_03000AFC = zeroB;

    gUnk_03000B18 = gUnk_03001ED0 = zeroByteB;
    REG_WININ = (gUnk_03001ED0 << 8) | gUnk_03000B18;
    gUnk_03000F7C = gUnk_03001010 = zeroByteB;
    REG_WINOUT = (gUnk_03001010 << 8) | gUnk_03000F7C;
    gUnk_03001EB0 = gUnk_03001000 = zeroByteB;
    REG_MOSAIC = (gUnk_03001000 << 8) | gUnk_03001EB0;
    gUnk_0300118C = gUnk_03000040 = zeroByteB;
    REG_BLDCNT = (gUnk_03000040 << 8) | gUnk_0300118C;
    gUnk_03000B08 = gUnk_03001EAC = zeroByteB;
    REG_BLDALPHA = (gUnk_03001EAC << 8) | gUnk_03000B08;

    REG_BLDY = gUnk_03001EEC = zeroB;

    REG_DMA0CNT_H = zeroB;
    REG_DMA1CNT_H = zeroB;
    REG_DMA2CNT_H = zeroB;
    REG_DMA3CNT_H = zeroB;

    REG_TM0CNT_L = REG_TM0CNT_H = zeroB;
    REG_TM1CNT_L = REG_TM1CNT_H = zeroB;
    REG_TM2CNT_L = REG_TM2CNT_H = zeroB;
    REG_TM3CNT_L = REG_TM3CNT_H = zeroB;

    REG_SIOMULTI0 = zeroB;
    REG_SIOMULTI1 = zeroB;
    REG_SIOMULTI2 = zeroB;
    REG_SIOMULTI3 = zeroB;
    REG_SIOCNT = zeroB;
    REG_SIODATA8 = zeroB;

    REG_KEYINPUT = gUnk_0300100C = zeroB;
    REG_KEYCNT = gUnk_03000FD8 = zeroB;

    gUnk_03001EA0 = zeroB;
    gUnk_03001EA8 = zeroB;

    m4aSoundInit();

    gUnk_03001EE4 = 1;
    gUnk_03000B0C = zeroB;
    gUnk_03000FCC = zeroB;
    gUnk_03000FBC = 0x100;
    gUnk_03000490 = -999;
    gUnk_03000AF8 = gUnk_03001EDC = zeroB;

    /* gUnk_03000F80 must be a plain (non-volatile) array: |= on a volatile
     * indexed element would emit two reads, the ROM has one. */
    for (i = 0; i < 4; i++)
    {
        gUnk_03000F80[i] |= 0xFFFF;
        gUnk_03001674[i] = 0;
        gUnk_03001180[i] = i;
        gUnk_0300001C[i] = i;
    }

    gUnk_03000FB0 = 0x03001270;
    zeroC = 0;
    *fillA = zeroC;
    CpuSet(fillA, (void *)0x03001270, 0x01000200);
    *fillB = zeroC;
    CpuSet(fillB, (void *)0x03001A90, 0x01000200);

    sub_08001a0c();

    CpuSet((const void *)((u32)sub_08001b08 & ~1), (void *)0x03001F40, 0x100);

    sub_08002028();

    /* Dead store, eliminated by the compiler.  It invalidates the compiler's
     * knowledge that zeroC == 0, which is what forces the fresh `movs rN, #0`
     * materializations in the tail below (the ROM has them; without this the
     * tail reuses zeroC's register across the three calls above). */
    zeroC = 2;

    gUnk_0300003C = gUnk_03000FA4 = 0;
    gUnk_03001014 = gUnk_03000B74 = 0;

    gUnk_03000B24 = gUnk_03001008 = gUnk_03000B20 = gUnk_03001020 = gUnk_03001178 = gUnk_03000B7C = gUnk_03001170 = 0;

    REG_DISPSTAT = gUnk_03001E9C = 8;

    REG_IF = 0;
    REG_IE = gUnk_03000018 = 0x2001;
    REG_IME = gUnk_03001EF8 = 1;
    REG_RCNT = 0;

    gUnk_0200EC48 = -1;
    gUnk_0200EBA0 = 0;

    gUnk_0300243C = gUnk_030023AC = 1;

    /* One expression: gUnk_0300244C's address is materialized before the
     * byte stores but written last, exactly as in the ROM. */
    gUnk_0300244C = (gUnk_030023A8.unk3 = 255,
                     gUnk_030023A8.unk2 = -1,
                     gUnk_030023A8.unk1 = -1,
                     gUnk_030023A8.unk0 = -1);

    sub_08002268();

    if (gUnk_03000B00 == 0)
        sub_08002eac(0xDEFBC);
}
