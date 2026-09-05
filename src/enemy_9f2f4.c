#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s16 gUnk_08747C30[];

/* Externals */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void sub_0809f2f4(void)
{
    struct Task *t;
    register struct Task *u asm("r6");
    s32 idx;
    s32 e;
    s32 k;
    register s16 *pp asm("r2");
    s16 *bp;
    BLOCK_CROSS_JUMP
    bp = gUnk_08747C30;
    t = gUnk_03002490;
    k = t->unk3C * 2;
    pp = (s16 *)(k + (s32)bp);
    asm("" :: "r"(k), "r"(bp));
    idx = *pp;
    if (idx != -1)
        sub_08001a94(t->unk42, (u = t)->unk38[idx], u->unk3E,
                     (t->unk40 & 0xFFF) | (240 << 8),
                     e = t->unk48 - gUnk_03002348,
                     (s16)(t->unk4A - gUnk_030023E4));
}
