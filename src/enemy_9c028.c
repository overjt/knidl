
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02007D00[];
extern u8 gUnk_03002350;
extern s16 gUnk_0300244C;
extern u32 * gUnk_08745CFC[];
extern u32 * gUnk_08745D4C[];

/* Externals */
extern void sub_08063908(u32 def);

void sub_0809c028(void)
{
    struct Task *t;
    struct Task *u;
    register struct Task **tp asm("r0");
    u32 **tab;
    u32 *p;
    s32 i;

    if (gUnk_0300244C != 0)
    {
        tab = gUnk_08745D4C;
        BLOCK_CROSS_JUMP
    }
    else
        tab = gUnk_08745CFC;
    tp = &gUnk_03002490;
    u = *tp;
    i = u->unk73 * 4 + gUnk_03002350 - 1;
    p = tab[i];
    sub_08063908(p[0]);
    gUnk_02007D00[4] = p[1];
    tp = &gUnk_03002490;
    t = *tp;
    t->unk28 = p[2];
    t->unk2C = p[3];
    t->unk30 = p[4];
    t->unk34 = p[5];
    gUnk_02007D00[3] = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[1] = 0;
    gUnk_02007D00[0] = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    gUnk_02007D00[5] = 0;
    t->unk6C = 0;
}
