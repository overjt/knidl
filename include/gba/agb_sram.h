#ifndef GUARD_GBA_AGB_SRAM_H
#define GUARD_GBA_AGB_SRAM_H

#include "gba/types.h"

void ReadSram(const u8 *src, u8 *dest, u32 size);
void WriteSram(const u8 *src, u8 *dest, u32 size);
u32 VerifySram(const u8 *src, u8 *dest, u32 size);
u32 WriteSramEx(const u8 *src, u8 *dest, u32 size);

#endif // GUARD_GBA_AGB_SRAM_H
