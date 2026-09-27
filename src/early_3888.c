#include "gba/gba.h"
#include "global.h"

/* early_3888.c (0x08003888-0x08003963, issue #63).
 *
 * Cold link init, called once from the main menu's link-play screen
 * (src/menu_0d450.c): it clears the link driver work area gUnk_0200EBC0
 * (0x30 bytes), resets the session state (gUnk_0200EBA8, gUnk_0200EC48, the
 * per-player bytes gUnk_030023A8 and their minimum gUnk_0300244C), puts SIO in
 * multi-play mode (115200 bps, IRQ on) and installs sub_08004068 as both link
 * IRQ handlers (serial and timer 3, gIntrTable[0]/[1]) before enabling the
 * serial interrupt.
 *
 * Matching note (issue #63): the per-player bytes and their minimum are one
 * chained assignment, `gUnk_0300244C = ... = gUnk_030023A8.unk03 = 0xFF;`:
 * the u8 byte gets 0xFF, the s8 bytes and the s16 cell -1, and the chain loads
 * the cell's address first (lesson 3.69).  Issue #32 had read the resulting
 * HImode store as a missing movhi scratch. */

/* Link (SIO multi-play + multiboot) driver work area at 0x0200EBC0. */
struct SioWork
{
    /*0x00*/ vu8 unk00;
    /*0x01*/ vu8 unk01;
    /*0x02*/ vu8 unk02;
    /*0x03*/ vu8 unk03;
    /*0x04*/ vu8 unk04;
    /*0x05*/ vu8 unk05;
    /*0x06*/ vu16 unk06;
    /*0x08*/ vu16 unk08;
    /*0x0A*/ vu16 unk0A;
    /*0x0C*/ vu16 unk0C;
    /*0x0E*/ vu16 unk0E;
    /*0x10*/ u32 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 unk18;
    /*0x1C*/ vu16 unk1C;
    /*0x1E*/ vu16 unk1E[3];
    /*0x24*/ vu8 unk24;
    /*0x25*/ vu8 unk25;
    /*0x26*/ vu8 unk26;
    /*0x27*/ vu8 unk27;
    /*0x28*/ vu16 unk28;
    /*0x2A*/ vu8 unk2A;
    /*0x2B*/ vu8 unk2B;
    /*0x2C*/ vu8 unk2C;
    /*0x2D*/ vu8 unk2D;
    /*0x2E*/ vu16 unk2E;
};

struct Unk030023A8
{
    /*0x00*/ s8 unk00[3];
    /*0x03*/ u8 unk03;
};

extern struct SioWork gUnk_0200EBC0;
extern u32 gUnk_0200EBA8;
extern u32 gUnk_0200EC48;
extern struct Unk030023A8 gUnk_030023A8;
extern vu16 gUnk_0300244C;
extern u32 gIntrTable[];
extern vu16 gIntrEnable;
extern vu16 gIntrMasterEnable;

void sub_08004068(void);

/* Cold link init: clear the driver work area, reset the session state,
 * put SIO in multi-play mode and install sub_08004068 as both link IRQ
 * handlers. */
void sub_08003888(void)
{
    vu16 zero;

    gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
    zero = 0;
    CpuSet((void *)&zero, &gUnk_0200EBC0, 0x01000018);
    gUnk_0200EBC0.unk06 = gUnk_0200EBC0.unk08 = 0x100;
    gUnk_0200EBA8 = 0;
    gUnk_0200EC48 = 0;
    gUnk_0300244C = gUnk_030023A8.unk00[0] = gUnk_030023A8.unk00[1]
        = gUnk_030023A8.unk00[2] = gUnk_030023A8.unk03 = 0xFF;
    REG_SIOCNT = 0x2000;
    REG_SIOCNT |= 0x4003;
    REG_SIOMLT_SEND = 0;
    gIntrTable[1] = gIntrTable[0] = (u32)sub_08004068;
    REG_IE = gIntrEnable = gIntrEnable | 0x80;
    gIntrMasterEnable = REG_IME = REG_IME | 1;
}
