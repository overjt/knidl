#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "camera.h"

/* camera_block_anim_clip.c (0x0802B2F0-0x0802B4BB, issue #86).
 *
 * SetBlockAnimClipRect, SetBg1BlockAnimClipRect and SetBlockAnimClipRectWithEdges (called from M09) compute
 * the tile rectangle gBlockAnimClipRect[4] (x0, x1, y0, y1) around the last
 * streamed camera position, clamped to the room; the three differ only in
 * the window width.  SetBg23ScreenSize and SetBg3ScreenSize set the screen-size
 * bits (15:14) of the BG2CNT/BG3CNT shadows gBg2Cnt/gBg3Cnt. */

void SetBlockAnimClipRect(void)
{
    s16 x0;
    gBlockAnimClipRect[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFD;
    gBlockAnimClipRect[1] = (gCameraStreamPos[0] >> 3) + 32;
    gBlockAnimClipRect[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gBlockAnimClipRect[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gBlockAnimClipRect[0] = 0;
    if (gBlockAnimClipRect[1] >= gRoomWidth * 2)
        gBlockAnimClipRect[1] = gRoomWidth * 2 - 1;
    if (gBlockAnimClipRect[2] < 0)
        gBlockAnimClipRect[2] = 0;
    if (gBlockAnimClipRect[3] >= gRoomHeight * 2)
        gBlockAnimClipRect[3] = gRoomHeight * 2 - 1;
}

void SetBg1BlockAnimClipRect(void)
{
    s16 x0;
    gBlockAnimClipRect[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFF;
    gBlockAnimClipRect[1] = (gCameraStreamPos[0] >> 3) + 30;
    gBlockAnimClipRect[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gBlockAnimClipRect[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gBlockAnimClipRect[0] = 0;
    if (gBlockAnimClipRect[1] >= gRoomWidth * 2)
        gBlockAnimClipRect[1] = gRoomWidth * 2 - 1;
    if (gBlockAnimClipRect[2] < 0)
        gBlockAnimClipRect[2] = 0;
    if (gBlockAnimClipRect[3] >= gRoomHeight * 2)
        gBlockAnimClipRect[3] = gRoomHeight * 2 - 1;
}

void SetBlockAnimClipRectWithEdges(void)
{
    s16 x0;
    gBlockAnimClipRect[0] = x0 = (gCameraStreamPos[0] >> 3) + 0xFFFF;
    gBlockAnimClipRect[1] = (gCameraStreamPos[0] >> 3) + 30;
    gBlockAnimClipRect[2] = (gCameraStreamPos[1] >> 3) + 0xFFFD;
    gBlockAnimClipRect[3] = (gCameraStreamPos[1] >> 3) + 22;
    if (x0 < 0)
        gBlockAnimClipRect[0] = 0;
    if (gBlockAnimClipRect[1] >= gRoomWidth * 2)
        gBlockAnimClipRect[1] = gRoomWidth * 2 - 1;
    if (gBlockAnimClipRect[2] < 0)
        gBlockAnimClipRect[2] = 0;
    if (gBlockAnimClipRect[3] >= gRoomHeight * 2)
        gBlockAnimClipRect[3] = gRoomHeight * 2 - 1;
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
