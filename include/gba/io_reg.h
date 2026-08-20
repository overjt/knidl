#ifndef GUARD_GBA_IO_REG_H
#define GUARD_GBA_IO_REG_H

#include "gba/types.h"

#define REG_BASE 0x04000000u

/* Waitstate control: bits 0-1 select the SRAM wait state (3 = 8 clocks). */
#define REG_WAITCNT (*(vu16 *)(REG_BASE + 0x204))

#endif // GUARD_GBA_IO_REG_H
