#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u16 gCompletionPercent;
extern u16 gUnk_03002364;
extern s8 gUnk_03002384;
extern u32 gUnk_030023C8[];
extern s8 gUnk_030023E0;

s32 CalcCompletionPercent(s32 a)
{
    s32 i;

    if (gUnk_03002364 & (4 << a))
    {
        gCompletionPercent = 100;
        return;
    }
    if (gUnk_030023E0 > 1)
    {
        gCompletionPercent = gUnk_03002384 * 2 + ((gUnk_030023E0 - 2) * 12 + (gUnk_030023E0 + 14));
        if ((gUnk_03002364 >> a) & 1)
            gCompletionPercent += 3;
    }
    else if (gUnk_030023E0 == 1)
        gCompletionPercent = gUnk_03002384 * 2 + (gUnk_030023E0 + 4);
    else if (gUnk_030023E0 == 0)
        gCompletionPercent = gUnk_03002384;
    for (i = 0; i <= 16; i++)
    {
        if (gUnk_030023C8[0] & (1 << i))
            gCompletionPercent++;
    }
    if (gCompletionPercent == 100)
        gUnk_03002364 |= 4 << a;
}
