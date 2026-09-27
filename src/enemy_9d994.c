#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s16 gUnk_08747B88[];

/* Externals */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void sub_0809d994(void)
{
    if (gUnk_08747B88[gUnk_03002490->unk3C] != -1)
        QueueSprite(gUnk_03002490->unk42,
                     gUnk_03002490->unk38[gUnk_08747B88[gUnk_03002490->unk3C]],
                     gUnk_03002490->unk3E,
                     (gUnk_03002490->unk40 & 0xFFF) | (240 << 8),
                     gUnk_03002490->unk48 - gUnk_03002348,
                     (s16)(gUnk_03002490->unk4A - gUnk_030023E4));
}
