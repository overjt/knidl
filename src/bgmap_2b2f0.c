#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* bgmap_2b2f0.c (0x0802B2F0-0x0802B4BB, issue #86).
 *
 * sub_0802b2f0, sub_0802b368 and sub_0802b3e4 (called from M09) compute
 * the tile rectangle gUnk_020055B8[4] (x0, x1, y0, y1) around the last
 * streamed camera position, clamped to the room; the three differ only in
 * the window width.  sub_0802b460 and sub_0802b49c set the screen-size
 * bits (15:14) of the BG2CNT/BG3CNT shadows gUnk_03000B10/gUnk_03001EB4. */

extern s16 gUnk_020055B8[4];
extern u16 gUnk_0300566C[2];
extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;
extern vu16 gUnk_03000B10;
extern vu16 gUnk_03001EB4;

void sub_0802b2f0(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gUnk_0300566C[0] >> 3) + 0xFFFD;
    gUnk_020055B8[1] = (gUnk_0300566C[0] >> 3) + 32;
    gUnk_020055B8[2] = (gUnk_0300566C[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gUnk_0300566C[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gUnk_03005620 * 2)
        gUnk_020055B8[1] = gUnk_03005620 * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gUnk_0300561C * 2)
        gUnk_020055B8[3] = gUnk_0300561C * 2 - 1;
}

void sub_0802b368(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gUnk_0300566C[0] >> 3) + 0xFFFF;
    gUnk_020055B8[1] = (gUnk_0300566C[0] >> 3) + 30;
    gUnk_020055B8[2] = (gUnk_0300566C[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gUnk_0300566C[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gUnk_03005620 * 2)
        gUnk_020055B8[1] = gUnk_03005620 * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gUnk_0300561C * 2)
        gUnk_020055B8[3] = gUnk_0300561C * 2 - 1;
}

void sub_0802b3e4(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gUnk_0300566C[0] >> 3) + 0xFFFF;
    gUnk_020055B8[1] = (gUnk_0300566C[0] >> 3) + 30;
    gUnk_020055B8[2] = (gUnk_0300566C[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gUnk_0300566C[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gUnk_03005620 * 2)
        gUnk_020055B8[1] = gUnk_03005620 * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gUnk_0300561C * 2)
        gUnk_020055B8[3] = gUnk_0300561C * 2 - 1;
}

void sub_0802b460(u16 a)
{
    gUnk_03000B10 &= 0x3FFF;
    gUnk_03000B10 |= a;
    gUnk_03001EB4 &= 0x3FFF;
    gUnk_03001EB4 |= a;
}

void sub_0802b49c(u16 a)
{
    gUnk_03001EB4 &= 0x3FFF;
    gUnk_03001EB4 |= a;
}
