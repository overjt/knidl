#ifndef GUARD_GBA_INTERRUPTS_H
#define GUARD_GBA_INTERRUPTS_H

/*
 * GBA interrupt IDs (issue #27).
 *
 * Bit positions in IE (0x04000200) / IF (0x04000202) / KEYCNT-style masks;
 * see io_reg.h for the registers themselves and defines.h for the BIOS
 * handshake cells INTR_CHECK (0x03007FF8) and INTR_VECTOR (0x03007FFC).
 */

/* Interrupt source flags (IE/IF bits, GBATEK). */
#define INTR_FLAG_VBLANK  (1 <<  0) // 0x0001
#define INTR_FLAG_HBLANK  (1 <<  1) // 0x0002
#define INTR_FLAG_VCOUNT  (1 <<  2) // 0x0004
#define INTR_FLAG_TIMER0  (1 <<  3) // 0x0008
#define INTR_FLAG_TIMER1  (1 <<  4) // 0x0010
#define INTR_FLAG_TIMER2  (1 <<  5) // 0x0020
#define INTR_FLAG_TIMER3  (1 <<  6) // 0x0040
#define INTR_FLAG_SERIAL  (1 <<  7) // 0x0080
#define INTR_FLAG_DMA0    (1 <<  8) // 0x0100
#define INTR_FLAG_DMA1    (1 <<  9) // 0x0200
#define INTR_FLAG_DMA2    (1 << 10) // 0x0400
#define INTR_FLAG_DMA3    (1 << 11) // 0x0800
#define INTR_FLAG_KEYPAD  (1 << 12) // 0x1000
#define INTR_FLAG_GAMEPAK (1 << 13) // 0x2000

/*
 * Master ISR dispatch order (docs/analysis/rom-map.md §4).
 *
 * The ROM's master ISR (source 0x08000108, copied by AgbInit to IWRAM
 * 0x03001030 and installed at 0x03007FFC) reads the interrupt block at
 * 0x04000200 and dispatches through the 14-entry handler table at
 * 0x030004B0 (copied from ROM 0x080CFDE8 by AgbInit).
 *
 * Priority bit tests, highest first, with the handler-table index each
 * selects (ip = index * 4):
 *
 *   INTR_FLAG_SERIAL  (0x80)   -> index 0
 *   INTR_FLAG_TIMER3  (0x40)   -> index 1
 *   INTR_FLAG_VBLANK  (0x01)   -> index 2
 *   INTR_FLAG_HBLANK  (0x02)   -> index 3
 *   INTR_FLAG_TIMER1  (0x10)   -> index 6
 *   INTR_FLAG_GAMEPAK (0x2000) -> index 13
 *
 * When decompiling IRQ/task code (issue #32), preserve this fixed priority
 * order; it is a hardware-latency choice baked into the master ISR, not an
 * arbitrary ordering.
 */

#endif // GUARD_GBA_INTERRUPTS_H
