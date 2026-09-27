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

    if (gUnk_03002364 & (4 << a))
    {
        gUnk_0300235C = 100;
        return;
    }
    if (gUnk_030023E0 > 1)
    {
        gUnk_0300235C = gUnk_03002384 * 2 + ((gUnk_030023E0 - 2) * 12 + (gUnk_030023E0 + 14));
        if ((gUnk_03002364 >> a) & 1)
            gUnk_0300235C += 3;
    }
    else if (gUnk_030023E0 == 1)
        gUnk_0300235C = gUnk_03002384 * 2 + (gUnk_030023E0 + 4);
    else if (gUnk_030023E0 == 0)
        gUnk_0300235C = gUnk_03002384;
    for (i = 0; i <= 16; i++)
    {
        if (gUnk_030023C8[0] & (1 << i))
            gUnk_0300235C++;
    }
    if (gUnk_0300235C == 100)
        gUnk_03002364 |= 4 << a;
}
