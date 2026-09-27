#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"

/* bgmap_2b2f0.c (0x0802B2F0-0x0802B4BB, issue #86).
 *
 * sub_0802b2f0, sub_0802b368 and sub_0802b3e4 (called from M09) compute
 * the tile rectangle gUnk_020055B8[4] (x0, x1, y0, y1) around the last
 * streamed camera position, clamped to the room; the three differ only in
 * the window width.  SetBg23ScreenSize and SetBg3ScreenSize set the screen-size
 * bits (15:14) of the BG2CNT/BG3CNT shadows gBg2Cnt/gBg3Cnt. */

void sub_0802b2f0(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFD;
    gUnk_020055B8[1] = (gCameraStreamPos[0] >> 3) + 32;
    gUnk_020055B8[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gRoomWidth * 2)
        gUnk_020055B8[1] = gRoomWidth * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gRoomHeight * 2)
        gUnk_020055B8[3] = gRoomHeight * 2 - 1;
}

void sub_0802b368(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFF;
    gUnk_020055B8[1] = (gCameraStreamPos[0] >> 3) + 30;
    gUnk_020055B8[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gRoomWidth * 2)
        gUnk_020055B8[1] = gRoomWidth * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gRoomHeight * 2)
        gUnk_020055B8[3] = gRoomHeight * 2 - 1;
}

void sub_0802b3e4(void)
{
    s16 x0;
    gUnk_020055B8[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFF;
    gUnk_020055B8[1] = (gCameraStreamPos[0] >> 3) + 30;
    gUnk_020055B8[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gUnk_020055B8[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gUnk_020055B8[0] = 0;
    if (gUnk_020055B8[1] >= gRoomWidth * 2)
        gUnk_020055B8[1] = gRoomWidth * 2 - 1;
    if (gUnk_020055B8[2] < 0)
        gUnk_020055B8[2] = 0;
    if (gUnk_020055B8[3] >= gRoomHeight * 2)
        gUnk_020055B8[3] = gRoomHeight * 2 - 1;
}

void SetBg23ScreenSize(u16 a)
{
    gBg2Cnt &= 0x3FFF;
    gBg2Cnt |= a;
    gBg3Cnt &= 0x3FFF;
    gBg3Cnt |= a;
}

void SetBg3ScreenSize(u16 a)
{
    gBg3Cnt &= 0x3FFF;
    gBg3Cnt |= a;
}
