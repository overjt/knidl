#ifndef GUARD_GBA_DEFINES_H
#define GUARD_GBA_DEFINES_H

#include <stddef.h>

#define TRUE  1
#define FALSE 0

/* ---------- Section-placement attributes --------------------------------- */
#define IWRAM_DATA __attribute__((section("iwram_data")))
#define EWRAM_DATA __attribute__((section("ewram_data")))

/* ---------- Common compiler attributes ------------------------------------ */
#define NAKED     __attribute__((naked))
#define UNUSED    __attribute__((unused))
#define PACKED    __attribute__((packed))
#define ALIGN(n)  __attribute__((aligned(n)))

/* ---------- Memory map ---------------------------------------------------- */
#define EWRAM_START 0x02000000u
#define EWRAM_END   (EWRAM_START + 0x40000u)
#define IWRAM_START 0x03000000u
#define IWRAM_END   (IWRAM_START + 0x8000u)

#define ROM_START   0x08000000u

/* Common hardware addresses */
#define PLTT        0x05000000u
#define VRAM        0x06000000u
#define OAM         0x07000000u

/* GBA interrupt vector / check / sound-info pointers (conventional) */
#define INTR_CHECK   (*(volatile unsigned short *)0x03007FF8u)
#define INTR_VECTOR  (*(void (*volatile *)(void))0x03007FFCu)

/* ---------- Display constants --------------------------------------------- */
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 160

/* ---------- Colour helper ------------------------------------------------- */
#define RGB(r, g, b) ((r) | ((g) << 5) | ((b) << 10))

#endif // GUARD_GBA_DEFINES_H
