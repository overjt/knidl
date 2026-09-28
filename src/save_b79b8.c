#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "room.h"
#include "save.h"

s32 CalcCompletionPercent(s32 a)
{
    s32 i;

    if (gMilestoneFlags & (4 << a))
    {
        gCompletionPercent = 100;
        return;
    }
    if (gFurthestLevel > 1)
    {
        gCompletionPercent = gFurthestStage * 2 + ((gFurthestLevel - 2) * 12 + (gFurthestLevel + 14));
        if ((gMilestoneFlags >> a) & 1)
            gCompletionPercent += 3;
    }
    else if (gFurthestLevel == 1)
        gCompletionPercent = gFurthestStage * 2 + (gFurthestLevel + 4);
    else if (gFurthestLevel == 0)
        gCompletionPercent = gFurthestStage;
    for (i = 0; i <= 16; i++)
    {
        if (gBigSwitchFlags[0] & (1 << i))
            gCompletionPercent++;
    }
    if (gCompletionPercent == 100)
        gMilestoneFlags |= 4 << a;
}
