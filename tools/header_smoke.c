/*
 * tools/header_smoke.c — compile-only smoke test for include/gba/*.h
 * (issue #27).  Never linked into the ROM; built by `make check-headers`
 * with both validated compilers (agbcc -O2, old_agbcc -O1) inside Docker.
 *
 * Every header under include/gba/ is included directly (so a header missing
 * from the umbrella gba.h fails here) and a representative use of every
 * register group, field set, interrupt flag, and syscall prototype is
 * forced through the compiler.
 */

#include "gba/gba.h"

/* Direct includes: fail here if a header is missing from the umbrella. */
#include "gba/agb_sram.h"
#include "gba/defines.h"
#include "gba/gba.h"
#include "gba/interrupts.h"
#include "gba/io_reg.h"
#include "gba/syscall.h"
#include "gba/types.h"

/* ROM-verified constants (include/gba/syscall.h, docs/analysis/rom-map.md). */
#if REG_BASE != 0x04000000
#error "REG_BASE must stay 0x04000000 (src/agb_sram.c depends on it)"
#endif
#if SWI_CPU_SET != 0x0B || SWI_CPU_FAST_SET != 0x0C || SWI_DIV != 0x06
#error "SWI numbering no longer matches this ROM's SDK thunks"
#endif
#if SWI_MULTI_BOOT != 0x25 || SWI_SOUND_DRIVER_VSYNC_OFF != 0x28
#error "SWI numbering no longer matches this ROM's SDK thunks"
#endif
#if INTR_FLAG_SERIAL != 0x0080 || INTR_FLAG_GAMEPAK != 0x2000
#error "interrupt IDs drifted from the master-ISR priority bits"
#endif

static vu16 sink16;
static vu32 sink32;
static vu64 sink64;

void header_smoke_display(void)
{
    sink16 = REG_DISPCNT | REG_GREENSWAP | REG_DISPSTAT | REG_VCOUNT;
    sink16 = REG_BG0CNT | REG_BG1CNT | REG_BG2CNT | REG_BG3CNT;
    sink16 = REG_BG0HOFS | REG_BG1VOFS | REG_BG2HOFS | REG_BG3VOFS;
    sink16 = REG_BG2PA | REG_BG2PB | REG_BG2PC | REG_BG2PD | REG_BG2X_L | REG_BG2Y_H;
    sink32 = REG_BG2X | REG_BG3Y;
    sink16 = REG_WIN0H | REG_WIN1H | REG_WIN0V | REG_WIN1V | REG_WININ | REG_WINOUT;
    sink16 = REG_MOSAIC | REG_BLDCNT | REG_BLDALPHA | REG_BLDY;

    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR | DISPSTAT_VCOUNT_INTR;
    REG_BG0CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x256;
    REG_BG2CNT = BGCNT_AFF256x256 | BGCNT_WRAP;
    REG_WIN0H = WIN_RANGE(0, DISPLAY_WIDTH);
    REG_WININ = WININ_WIN0_ALL | WININ_WIN1_OBJ;
    REG_WINOUT = WINOUT_WIN01_ALL | WINOUT_WINOBJ_CLR;
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BD;
    REG_BLDALPHA = BLDALPHA_BLEND(12, 4);
    REG_BLDY = 8;
}

void header_smoke_sound(void)
{
    sink16 = REG_SOUND1CNT_L | REG_SOUND1CNT_H | REG_SOUND1CNT_X;
    sink16 = REG_SOUND2CNT_L | REG_SOUND2CNT_H;
    sink16 = REG_SOUND3CNT_L | REG_SOUND3CNT_H | REG_SOUND3CNT_X;
    sink16 = REG_SOUND4CNT_L | REG_SOUND4CNT_H;
    sink16 = REG_SOUNDCNT_L | REG_SOUNDCNT_H | REG_SOUNDCNT_X | REG_SOUNDBIAS;
    sink32 = REG_WAVE_RAM0 | REG_WAVE_RAM1 | REG_WAVE_RAM2 | REG_WAVE_RAM3 | REG_FIFO_A | REG_FIFO_B;

    REG_SOUNDCNT_H = SOUND_ALL_MIX_FULL | SOUND_A_LEFT_OUTPUT | SOUND_A_TIMER_0 | SOUND_B_FIFO_RESET;
    REG_SOUNDCNT_X = SOUND_MASTER_ENABLE | SOUND_1_ON | SOUND_4_ON;
}

void header_smoke_dma_timer_serial_keys(void)
{
    sink32 = REG_DMA0SAD | REG_DMA0DAD | REG_DMA3SAD | REG_DMA3DAD;
    sink16 = REG_DMA0CNT_L | REG_DMA1CNT_H | REG_DMA2CNT_H | REG_DMA3CNT_L;

    REG_DMA3CNT = DMA_ENABLE | DMA_32BIT | DMA_SRC_INC | DMA_DEST_INC | DMA_START_NOW;
    REG_DMA1CNT_H = DMA_INTR_ENABLE | DMA_REPEAT | DMA_START_HBLANK;

    sink32 = REG_TM0CNT | REG_TM3CNT;
    REG_TM0CNT_L = 0;
    REG_TM0CNT_H = TIMER_64CLK | TIMER_INTR_ENABLE | TIMER_ENABLE;
    REG_TMCNT_L(2) = 0;
    REG_TMCNT_H(2) = TIMER_1024CLK | TIMER_ENABLE;
    sink16 = REG_TM3CNT_H;

    sink16 = REG_SIOCNT | REG_SIODATA8 | REG_SIOMLT_SEND | REG_SIOMULTI0 | REG_SIOMULTI3;
    sink32 = REG_SIODATA32;
    sink64 = REG_SIOMLT_RECV;
    REG_SIOCNT = SIO_MULTI_MODE | SIO_115200_BPS | SIO_INTR_ENABLE;

    sink16 = REG_KEYINPUT | REG_KEYCNT | REG_RCNT;
    sink16 = A_BUTTON | B_BUTTON | SELECT_BUTTON | START_BUTTON | DPAD_ANY | R_BUTTON | L_BUTTON | KEYS_MASK;
    REG_KEYCNT = KEY_INTR_ENABLE | KEY_OR_INTR;
}

void header_smoke_interrupts_waits(void)
{
    REG_IE = INTR_FLAG_VBLANK | INTR_FLAG_HBLANK | INTR_FLAG_VCOUNT | INTR_FLAG_TIMER0
           | INTR_FLAG_TIMER1 | INTR_FLAG_TIMER2 | INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL
           | INTR_FLAG_DMA0 | INTR_FLAG_DMA1 | INTR_FLAG_DMA2 | INTR_FLAG_DMA3
           | INTR_FLAG_KEYPAD | INTR_FLAG_GAMEPAK;
    REG_IF = INTR_FLAG_VBLANK;
    REG_IME = 1;
    INTR_CHECK = INTR_FLAG_VBLANK;
    INTR_VECTOR = (void (*)(void))0;

    REG_WAITCNT = (REG_WAITCNT & 0xFFFC) | WAITCNT_SRAM_8;
    REG_WAITCNT = WAITCNT_WS0_N_3 | WAITCNT_WS0_S_1 | WAITCNT_WS1_N_8 | WAITCNT_WS1_S_4
                | WAITCNT_WS2_N_8 | WAITCNT_WS2_S_8 | WAITCNT_PHI_OUT_4MHZ
                | WAITCNT_PREFETCH_ENABLE | WAITCNT_AGB;
}

void header_smoke_syscall(void)
{
    CpuFastSet((const u32 *)ROM_START, (u32 *)EWRAM_START, 8);
    CpuSet((const void *)ROM_START, (void *)IWRAM_START, 28);
    sink32 = (u32)Div(-100, 7);
    sink32 = (u32)Mod(-100, 7);
    sink32 = (u32)ArcTan2(64, -32);
    LZ77UnCompWram((const void *)ROM_START, (void *)EWRAM_START);
    LZ77UnCompVram((const void *)ROM_START, (void *)VRAM);
    HuffUnComp((const void *)ROM_START, (void *)EWRAM_START);
    sink16 = MultiBoot((struct MultiBootParam *)0);
    SoundDriverVSyncOff();

    ReadSram((const u8 *)ROM_START, (u8 *)EWRAM_START, 16);
    WriteSram((const u8 *)ROM_START, (u8 *)EWRAM_START, 16);
    sink32 = VerifySram((const u8 *)ROM_START, (u8 *)EWRAM_START, 16);
    sink32 = WriteSramEx((const u8 *)ROM_START, (u8 *)EWRAM_START, 16);
}

void header_smoke_types_defines(void)
{
    u8 a = 1;
    u16 b = 2;
    u32 c = 3;
    u64 d = 4;
    s8 e = -1;
    s16 f = -2;
    s32 g = -3;
    s64 h = -4;
    bool8 t = TRUE;
    bool16 u = FALSE;
    bool32 w = TRUE;
    f32 x = 1.0;
    f64 y = 2.0;

    sink32 = (u32)(a + b + c + (u32)d + (u32)e + f + g + (u32)h + t + u + w)
           + (u32)(sizeof(x) + sizeof(y));
    sink32 = EWRAM_START | EWRAM_END | IWRAM_START | IWRAM_END | ROM_START | PLTT | VRAM | OAM;
    sink16 = (u16)(RGB(31, 31, 31) + DISPLAY_WIDTH + DISPLAY_HEIGHT);
    sink16 = (u16)sizeof(sink64);
}
