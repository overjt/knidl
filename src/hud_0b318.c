#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_0b318.c (0x0800B318-0x0800B44B, issue #96).
 *
 * HUD tilemap buffer helpers. */

extern u8 gUnk_0200002C;
extern u16 gUnk_02005600[];
extern u8 gUnk_03001F30;
extern s8 gUnk_03002444;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08008c64(u16 a0);

/* Copy n tiles from src into the HUD tilemap buffer at column x, row y.
 * The walking `pos` (not `pos + i`) is what keeps src incremented in place
 * and after the destination. */
void sub_0800b318(u16 *src, s32 x, s32 y, s32 n)
{
    s32 i;
    s32 pos = x + (y << 5);

    for (i = 0; i < n; i++) {
        gUnk_02005600[pos] = *src;
        pos++;
        src++;
    }
    gUnk_0200002C = 1;
}

/* Clear n tiles of the HUD tilemap buffer at column x, row y. */
void sub_0800b34c(s32 x, s32 y, s32 n)
{
    s32 i;
    s32 pos = x + (y << 5);

    for (i = 0; i < n; i++) {
        gUnk_02005600[pos] = 0;
        pos++;
    }
    gUnk_0200002C = 1;
}

/* The fill source must be volatile: a plain u16 merges the ROM's two
 * `mov rX, sp` into one. */
void sub_0800b37c(void)
{
    vu16 zero = 0;

    CpuSet((void *)&zero, gUnk_02005600, 0x01000400);
    gUnk_0200002C = 1;
}

void sub_0800b3a8(void)
{
    vu16 zero;

    if (gUnk_03002444 != 0) {
        zero = 0;
        CpuSet((void *)&zero, &gUnk_02005600[64], 0x010003C0);
    } else {
        zero = 0;
        CpuSet((void *)&zero, gUnk_02005600, 0x01000400);
    }
    gUnk_0200002C = 1;
}

/* Flush the HUD tilemap buffer to VRAM when it is dirty. */
void sub_0800b3f8(void)
{
    if (gUnk_0200002C != 0) {
        sub_080017e4(1, (u32)gUnk_02005600, 0x06001000, 0x800);
        gUnk_0200002C = 0;
    }
}

void sub_0800b428(void)
{
    if (gUnk_03001F30 == 0)
        sub_08008c64(4);
    else
        sub_08008c64(5);
}
