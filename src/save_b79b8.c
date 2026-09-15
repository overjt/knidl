#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u16 gUnk_0300235C;
extern u16 gUnk_03002364;
extern s8 gUnk_03002384;
extern u32 gUnk_030023C8[];
extern s8 gUnk_030023E0;

s32 sub_080b79b8(s32 a)
{
    s32 i;
    s32 one;
    u32 *pmask;
    u16 *cp;
    s32 v;
    s32 t;
    s32 u;
    s32 u2;
    register u32 m asm("r3");
    register u16 *p asm("r5");
    register u16 *q asm("r3");

    if (gUnk_03002364 & (4 << a))
    {
        gUnk_0300235C = 100;
        return;
    }
    if (gUnk_030023E0 > 1)
    {
        p = &gUnk_0300235C;
        u = gUnk_03002384 * 2;
        u2 = (gUnk_030023E0 - 2) * 12 + 14;
        t = gUnk_030023E0 + u2;
        v = u + t;
        *p = v;
        if ((gUnk_03002364 >> a) & 1)
            *p = v + 3;
    }
    else if (gUnk_030023E0 == 1)
    {
        q = &gUnk_0300235C;
        *q = gUnk_03002384 * 2 + (gUnk_030023E0 + 4);
        p = q;
    }
    else
    {
        p = &gUnk_0300235C;
        if (gUnk_030023E0 == 0)
            *p = gUnk_03002384;
    }
    i = 0;
    pmask = gUnk_030023C8;
    one = 1;
    m = *pmask;
    cp = &gUnk_0300235C;
    do
    {
        if (m & (one << i))
            (*cp)++;
        i++;
    } while (i <= 16);
    if (*p == 100)
        gUnk_03002364 |= 4 << a;
}
