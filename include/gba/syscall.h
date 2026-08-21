#ifndef GUARD_GBA_SYSCALL_H
#define GUARD_GBA_SYSCALL_H

/*
 * GBA BIOS syscall interface (issue #27).
 *
 * IMPORTANT — SWI numbering:
 *
 * This ROM's SDK thunks use the "SDK/libagbsyscall" SWI order, NOT the
 * retail-BIOS order documented in GBATEK's main table.  The two agree for
 * 0x00-0x07, 0x10-0x1F and 0x25+, but differ in the arithmetic/memory band:
 *
 *   number   SDK order (this ROM)      retail BIOS order
 *   0x08     Sqrt                      CpuSet
 *   0x09     ArcTan                    CpuFastSet
 *   0x0A     ArcTan2                   GetBiosChecksum
 *   0x0B     CpuSet                    Sqrt
 *   0x0C     CpuFastSet                ArcTan
 *   0x0D     GetBiosChecksum           ArcTan2
 *   0x0E     BgAffineSet               (unused)
 *   0x0F     ObjAffineSet              (unused)
 *
 * The SDK-order column was verified against this ROM: CpuFastSet is
 * svc 0x0C at 0x080CFA54 and CpuSet is svc 0x0B at 0x080CFA58 (AgbInit
 * boot fills/copies, rom-map.md §4), Div is svc 0x06 (0x080CFA5C),
 * MultiBoot is svc 0x25 with mode hardcoded to 1 (0x080CFA74), and
 * SoundDriverVSyncOff is svc 0x28 (0x080CFA7C, called on the soft-reset
 * path after clearing IME).  Entries marked "unverified" below are filled
 * in from the same SDK-order table (GBATEK alternate order; mirrored in
 * rust-console/gba issue #27) and have no thunk in this ROM.
 *
 * ROM thunk map (SDK syscall wrappers, 0x080CFA40-0x080CFA80, Thumb,
 * each `svc N; bx lr`); caller counts from a full-ROM BL census:
 *
 *   0x080CFA50  svc 0x0A               ArcTan2              (11 callers)
 *   0x080CFA54  svc 0x0C               CpuFastSet           (21)
 *   0x080CFA58  svc 0x0B               CpuSet               (61)
 *   0x080CFA5C  svc 0x06               Div                  (41)
 *   0x080CFA60  svc 0x06 + mov r0,r1   Mod (remainder)      (7)
 *   0x080CFA68  svc 0x13               HuffUnComp           (6)
 *   0x080CFA6C  svc 0x12               LZ77UnCompVram       (29)
 *   0x080CFA70  svc 0x11               LZ77UnCompWram       (19)
 *   0x080CFA74  mov r1,#1; svc 0x25    MultiBoot            (1)
 *   0x080CFA7C  svc 0x28               SoundDriverVSyncOff  (1)
 *
 *   0x080CFA80  (svc 0x01; svc 0x00)   compound reset helper
 *               (clears IME + high byte of INTR_CHECK, sets sp=0x03007F00,
 *               RegisterRamReset(r0) then SoftReset); 2 callers.  It and
 *               the non-syscall helpers at 0x080CFA40 / 0x080CFA4C get
 *               named with the SDK wrappers in issue #29.
 *
 * When the wrappers themselves are decompiled (issue #29), implement them
 * in one place and include this header; do not hand-code `svc` numbers in
 * game code.
 */

#include "gba/types.h"

struct MultiBootParam; /* full definition lands with the multiboot module */

/* SWI numbers, SDK order used by this ROM (see table above). */
#define SWI_SOFT_RESET            0x00 /* verified: 0x080CFA92 */
#define SWI_REGISTER_RAM_RESET    0x01 /* verified: 0x080CFA90 */
#define SWI_HALT                  0x02 /* unverified */
#define SWI_STOP                  0x03 /* unverified */
#define SWI_INTR_WAIT             0x04 /* unverified */
#define SWI_VBLANK_INTR_WAIT      0x05 /* unverified */
#define SWI_DIV                   0x06 /* verified: 0x080CFA5C / 0x080CFA60 */
#define SWI_DIV_ARM               0x07 /* unverified */
#define SWI_SQRT                  0x08 /* unverified */
#define SWI_ARC_TAN               0x09 /* unverified */
#define SWI_ARC_TAN_2             0x0A /* verified: 0x080CFA50 */
#define SWI_CPU_SET               0x0B /* verified: 0x080CFA58 */
#define SWI_CPU_FAST_SET          0x0C /* verified: 0x080CFA54 */
#define SWI_GET_BIOS_CHECKSUM     0x0D /* unverified */
#define SWI_BG_AFFINE_SET         0x0E /* unverified */
#define SWI_OBJ_AFFINE_SET        0x0F /* unverified */
#define SWI_BIT_UNPACK            0x10 /* unverified */
#define SWI_LZ77_UNCOMP_WRAM      0x11 /* verified: 0x080CFA70 */
#define SWI_LZ77_UNCOMP_VRAM      0x12 /* verified: 0x080CFA6C */
#define SWI_HUFF_UNCOMP           0x13 /* verified: 0x080CFA68 */
#define SWI_RLE_UNCOMP_WRAM       0x14 /* unverified */
#define SWI_RLE_UNCOMP_VRAM       0x15 /* unverified */
#define SWI_DIFF8_UNFILTER_WRAM   0x16 /* unverified */
#define SWI_DIFF8_UNFILTER_VRAM   0x17 /* unverified */
#define SWI_DIFF16_UNFILTER       0x18 /* unverified */
#define SWI_SOUND_BIAS            0x19 /* unverified */
#define SWI_SOUND_DRIVER_INIT     0x1A /* unverified */
#define SWI_SOUND_DRIVER_MODE     0x1B /* unverified */
#define SWI_SOUND_DRIVER_MAIN     0x1C /* unverified */
#define SWI_SOUND_DRIVER_VSYNC    0x1D /* unverified */
#define SWI_SOUND_CHANNEL_CLEAR   0x1E /* unverified */
#define SWI_MIDI_KEY2_FREQ        0x1F /* unverified */
#define SWI_SOUND_MISC_0          0x20 /* unverified, purpose unknown */
#define SWI_SOUND_MISC_1          0x21 /* unverified, purpose unknown */
#define SWI_SOUND_MISC_2          0x22 /* unverified, purpose unknown */
#define SWI_SOUND_MISC_3          0x23 /* unverified, purpose unknown */
#define SWI_SOUND_MISC_4          0x24 /* unverified, purpose unknown */
#define SWI_MULTI_BOOT            0x25 /* verified: 0x080CFA74 */
#define SWI_HARD_RESET            0x26 /* unverified */
#define SWI_CUSTOM_HALT           0x27 /* unverified */
#define SWI_SOUND_DRIVER_VSYNC_OFF 0x28 /* verified: 0x080CFA7C */
#define SWI_SOUND_DRIVER_VSYNC_ON  0x29 /* unverified */
#define SWI_SOUND_GET_JUMP_LIST    0x2A /* unverified */

/*
 * Prototypes for the ROM's syscall thunks (addresses pinned above; all
 * Thumb entry points, i.e. actual addresses are +1).
 *
 * CpuSet/CpuFastSet control word: bits 0-20 hold the transfer count
 * (words/halfwords for CpuSet, words for CpuFastSet — always a multiple
 * of 8 for CpuFastSet); bit 24 (0x01000000) selects fill mode (source is
 * a single pattern unit) instead of copy.
 */
void CpuSet(const void *src, void *dest, u32 control);         /* 0x080CFA58 */
void CpuFastSet(const u32 *src, u32 *dest, u32 control);       /* 0x080CFA54 */

/* Div: signed division; r0 = quotient, r1 = remainder, r3 = abs(r0_in).
 * Mod is the ROM's second wrapper around the same SWI (svc 0x06 followed
 * by mov r0, r1), returning the remainder instead of the quotient. */
s32 Div(s32 numerator, s32 denominator);                       /* 0x080CFA5C */
s32 Mod(s32 numerator, s32 denominator);                       /* 0x080CFA60 */

/* ArcTan2(x, y): angle of the vector (x, y), returned as a 16-bit
 * fraction of a full turn (0x0000-0xFFFF = 0-360 degrees).  Inputs are
 * used as 16-bit signed values by the BIOS. */
s32 ArcTan2(s16 x, s16 y);                                     /* 0x080CFA50 */

/* Decompression (BIOS header-tagged streams; src is word-aligned). */
void LZ77UnCompWram(const void *src, void *dest);              /* 0x080CFA70 */
void LZ77UnCompVram(const void *src, void *dest);              /* 0x080CFA6C */
void HuffUnComp(const void *src, void *dest);                  /* 0x080CFA68 */

/* MultiBoot: the thunk hardcodes mode = 1 (master).  Returns 0 on
 * success, a nonzero error code otherwise.  struct MultiBootParam is
 * defined with the multiboot/communication module (see also the helper
 * at 0x080CFA40). */
u8 MultiBoot(struct MultiBootParam *param);                    /* 0x080CFA74 */

/* SoundDriverVSyncOff: shuts down the sound DMA/vsync path; the ROM calls
 * it on the soft-reset path after clearing IME (0x08000FE4). */
void SoundDriverVSyncOff(void);                                /* 0x080CFA7C */

#endif // GUARD_GBA_SYSCALL_H
