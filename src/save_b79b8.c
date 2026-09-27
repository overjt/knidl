#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u16 gCompletionPercent;
extern u16 gMilestoneFlags;
extern s8 gUnk_03002384;
extern u32 gBigSwitchFlags[];
extern s8 gUnk_030023E0;

s32 CalcCompletionPercent(s32 a)
{
    s32 i;

    if (gMilestoneFlags & (4 << a))
    {
        gCompletionPercent = 100;
        return;
    }
    if (gUnk_030023E0 > 1)
    {
        gCompletionPercent = gUnk_03002384 * 2 + ((gUnk_030023E0 - 2) * 12 + (gUnk_030023E0 + 14));
        if ((gMilestoneFlags >> a) & 1)
            gCompletionPercent += 3;
    }
    else if (gUnk_030023E0 == 1)
        gCompletionPercent = gUnk_03002384 * 2 + (gUnk_030023E0 + 4);
    else if (gUnk_030023E0 == 0)
        gCompletionPercent = gUnk_03002384;
    for (i = 0; i <= 16; i++)
    {
        if (gBigSwitchFlags[0] & (1 << i))
            gCompletionPercent++;
    }
    if (gCompletionPercent == 100)
        gMilestoneFlags |= 4 << a;
}
