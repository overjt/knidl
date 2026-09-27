#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s16 gUnk_08747B88[];

/* Externals */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void sub_0809d994(void)
{
    if (gUnk_08747B88[gCurTask->frame] != -1)
        QueueSprite(gCurTask->layer,
                     gCurTask->frameTable[gUnk_08747B88[gCurTask->frame]],
                     gCurTask->spriteFlags,
                     (gCurTask->tileWord & 0xFFF) | (240 << 8),
                     gCurTask->pixelX - gSpriteCameraX,
                     (s16)(gCurTask->pixelY - gSpriteCameraY));
}
