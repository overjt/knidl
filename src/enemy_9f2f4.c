#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s16 gUnk_08747C30[];

/* Externals */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void sub_0809f2f4(void)
{
    if (gUnk_08747C30[gCurTask->frame] != -1)
        QueueSprite(gCurTask->layer,
                     gCurTask->unk38[gUnk_08747C30[gCurTask->frame]],
                     gCurTask->unk3E,
                     (gCurTask->unk40 & 0xFFF) | (240 << 8),
                     gCurTask->unk48 - gSpriteCameraX,
                     (s16)(gCurTask->unk4A - gSpriteCameraY));
}
