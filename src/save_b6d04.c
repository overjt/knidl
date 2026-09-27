#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern s32 gUnk_02016C30;
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vu32 gUnk_03001E94;
extern u16 gUnk_030023E4;
extern s8 gUnk_087561CC[];

/* The fixed BG2VOFS value `w` is a plain u32 local that global allocation
   spills (the ROM's `str r0, [sp]` ... `mov r0, sp; ldrh`, lesson 3.467), and
   the tail store goes through the pointer local `pc`, assigned with the other
   three before the loop: global allocation drops it and reload re-loads the
   address right before the store, after the value (lesson 3.258).  */

s32 sub_080b6d04(void)
{
    u16 i;
    s32 n;
    s32 vt1;
    s32 vt2;
    u16 v;
    u32 w;
    u16 *p;
    u16 *base;
    vs32 *pf;
    vs32 *pb;
    s32 *pc2;
    s32 *pc3;
    s8 *pt2;
    s8 *pt;
    s32 d;
    u32 *pc;

    if (gUnk_02016490 == 3)
    {
        gUnk_03000F8C = gUnk_020164A0[0] << 16;
        gUnk_03000B78 = gUnk_020164A0[2] << 16;
        gUnk_0300101C = 0x04000018;
        return 3;
    }
    if (gUnk_02016490 == 2)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
    }
    else
    {
        if (gUnk_02016494 > 0x1FF)
            gUnk_02016494 = 0;
        d = 512 - gUnk_02016494;
        i = gUnk_030023E4;
        w = gUnk_03001E94 >> 16;
        pt = gUnk_087561CC;
        v = ((d * pt[i >> 3]) >> 1) + *(pc2 = &gUnk_02016C30);
        pf = &gUnk_03000F8C;
        base = gUnk_020164A0;
        pb = &gUnk_03000B78;
        pc = &gUnk_0300101C;
        pc3 = pc2;
        pt2 = pt;
        p = base;
        n = 159;
        do
        {
            if ((i & 7) == 0)
                v = ((d * pt2[i >> 3]) >> 1) + *pc3;
            p[0] = v;
            p[1] = w;
            p[2] = v;
            i++;
            p += 3;
        } while (--n >= 0);
        vt1 = base[0] << 16;
        *pf = vt1;
        vt2 = base[2] << 16;
        *pb = vt2;
        *pc = 0x04000018;
        return 3;
    }
}
