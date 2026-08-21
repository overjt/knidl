#ifndef GUARD_GBA_GBA_H
#define GUARD_GBA_GBA_H

/*
 * Umbrella header for the GBA platform headers (issue #27).  Game code
 * normally includes "global.h" (which may include this) or the specific
 * gba/*.h header it needs.  New headers added under include/gba/ must be
 * wired in here and touched by tools/header_smoke.c (`make check-headers`).
 */

#include "gba/defines.h"
#include "gba/types.h"
#include "gba/io_reg.h"
#include "gba/interrupts.h"
#include "gba/syscall.h"
#include "gba/agb_sram.h"

#endif // GUARD_GBA_GBA_H
