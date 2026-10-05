#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void JavelinKnightDrawOverlay(void)
{
    if (gUnk_08747B88[gCurTask->frame] != -1)
        QueueSprite(gCurTask->layer,
                     gCurTask->frameTable[gUnk_08747B88[gCurTask->frame]],
                     gCurTask->spriteFlags,
                     (gCurTask->tileWord & 0xFFF) | (240 << 8),
                     gCurTask->pixelX - gSpriteCameraX,
                     (s16)(gCurTask->pixelY - gSpriteCameraY));
}
