#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "save.h"

void MergeLinkSaveSlots(void)
{
    s32 player;
    s32 level;
    s32 stage;

    for (player = 0; player < gPlayerCount; player++)
    {
        if ((s8)gSaveSlots[3].furthestLevel[gExtraMode] > (s8)gLinkSaveSlots[player].furthestLevel[gExtraMode])
        {
            gSaveSlots[3].furthestLevel[gExtraMode] = gLinkSaveSlots[player].furthestLevel[gExtraMode];
            gSaveSlots[3].furthestStage[gExtraMode] = gLinkSaveSlots[player].furthestStage[gExtraMode];
            gSaveSlots[3].curLevel[gExtraMode] = gLinkSaveSlots[player].curLevel[gExtraMode];
            gSaveSlots[3].curStage[gExtraMode] = gLinkSaveSlots[player].curStage[gExtraMode];
        }
        else if ((s8)gSaveSlots[3].furthestLevel[gExtraMode] == (s8)gLinkSaveSlots[player].furthestLevel[gExtraMode])
        {
            if ((s8)gSaveSlots[3].furthestStage[gExtraMode] > (s8)gLinkSaveSlots[player].furthestStage[gExtraMode])
            {
                gSaveSlots[3].furthestStage[gExtraMode] = gLinkSaveSlots[player].furthestStage[gExtraMode];
                gSaveSlots[3].curLevel[gExtraMode] = gLinkSaveSlots[player].curLevel[gExtraMode];
                gSaveSlots[3].curStage[gExtraMode] = gLinkSaveSlots[player].curStage[gExtraMode];
            }
            else if ((s8)gSaveSlots[3].furthestStage[gExtraMode] == (s8)gLinkSaveSlots[player].furthestStage[gExtraMode])
            {
                if ((s8)gSaveSlots[3].curLevel[gExtraMode] > (s8)gLinkSaveSlots[player].curLevel[gExtraMode])
                {
                    gSaveSlots[3].curLevel[gExtraMode] = gLinkSaveSlots[player].curLevel[gExtraMode];
                    gSaveSlots[3].curStage[gExtraMode] = gLinkSaveSlots[player].curStage[gExtraMode];
                }
                else if ((s8)gSaveSlots[3].curLevel[gExtraMode] == (s8)gLinkSaveSlots[player].curLevel[gExtraMode])
                {
                    if ((s8)gSaveSlots[3].curStage[gExtraMode] > (s8)gLinkSaveSlots[player].curStage[gExtraMode])
                        gSaveSlots[3].curStage[gExtraMode] = gLinkSaveSlots[player].curStage[gExtraMode];
                }
            }
        }
        if (gSaveSlots[3].completionPercent[gExtraMode] > gLinkSaveSlots[player].completionPercent[gExtraMode])
            gSaveSlots[3].completionPercent[gExtraMode] = gLinkSaveSlots[player].completionPercent[gExtraMode];
        gSaveSlots[3].bigSwitchFlags[gExtraMode] &= gLinkSaveSlots[player].bigSwitchFlags[gExtraMode];
        gSaveSlots[3].milestoneFlags &= gLinkSaveSlots[player].milestoneFlags;
        for (level = 0; level <= 7; level++)
        {
            for (stage = 0; stage <= 6; stage++)
            {
                if (gSaveSlots[3].stageClearStatus[level][stage] > gLinkSaveSlots[player].stageClearStatus[level][stage])
                    gSaveSlots[3].stageClearStatus[level][stage] = gLinkSaveSlots[player].stageClearStatus[level][stage];
            }
        }
    }
}
void MergeProgressIntoSaveSlot(s32 a)
{
    s32 i;
    s32 stage;

    gSaveSlots[3].curLevel[gExtraMode] = gCurLevel;
    gSaveSlots[3].curStage[gExtraMode] = gCurStage;
    gSaveSlots[3].furthestLevel[gExtraMode] = gFurthestLevel;
    gSaveSlots[3].furthestStage[gExtraMode] = gFurthestStage;
    gSaveSlots[3].bigSwitchFlags[gExtraMode] = gBigSwitchFlags[0];
    for (i = 0; i <= 7; i++)
    {
        for (stage = 0; stage <= 6; stage++)
        {
            gSaveSlots[3].stageClearStatus[i][stage] &= 15 << ((gExtraMode ^ 1) * 4);
            gSaveSlots[3].stageClearStatus[i][stage] |= gStageClearStatus[i][stage] << (gExtraMode * 4);
        }
    }
    CalcCompletionPercent(gExtraMode);
    gSaveSlots[3].completionPercent[gExtraMode] = gCompletionPercent;
    gSaveSlots[3].milestoneFlags = gMilestoneFlags;
    gSaveSlots[a].curLevel[gExtraMode] = gSaveSlots[3].curLevel[gExtraMode];
    gSaveSlots[a].curStage[gExtraMode] = gSaveSlots[3].curStage[gExtraMode];
    if ((s8)gSaveSlots[a].furthestLevel[gExtraMode] < (s8)gSaveSlots[3].furthestLevel[gExtraMode])
    {
        gSaveSlots[a].furthestLevel[gExtraMode] = gSaveSlots[3].furthestLevel[gExtraMode];
        gSaveSlots[a].furthestStage[gExtraMode] = gSaveSlots[3].furthestStage[gExtraMode];
    }
    else if ((s8)gSaveSlots[a].furthestLevel[gExtraMode] == (s8)gSaveSlots[3].furthestLevel[gExtraMode])
    {
        if ((s8)gSaveSlots[a].furthestStage[gExtraMode] < (s8)gSaveSlots[3].furthestStage[gExtraMode])
            gSaveSlots[a].furthestStage[gExtraMode] = gSaveSlots[3].furthestStage[gExtraMode];
    }
    gSaveSlots[a].bigSwitchFlags[gExtraMode] |= gSaveSlots[3].bigSwitchFlags[gExtraMode];
    gSaveSlots[a].milestoneFlags |= gSaveSlots[3].milestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (stage = 0; stage <= 6; stage++)
        {
            if (gSaveSlots[a].stageClearStatus[i][stage] < gSaveSlots[3].stageClearStatus[i][stage])
                gSaveSlots[a].stageClearStatus[i][stage] = gSaveSlots[3].stageClearStatus[i][stage];
        }
    }
    if ((gSaveSlots[a].milestoneFlags & (4 << gExtraMode)) != 0)
    {
        gSaveSlots[a].completionPercent[gExtraMode] = 100;
    }
    else
    {
        if ((s8)gSaveSlots[a].furthestLevel[gExtraMode] > 1)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].furthestStage[gExtraMode] * 2;
            gSaveSlots[a].completionPercent[gExtraMode] += (s8)gSaveSlots[a].furthestLevel[gExtraMode]
                + ((((s8)gSaveSlots[a].furthestLevel[gExtraMode] - 2) * 3) * 4 + 14);
            if (((gSaveSlots[a].milestoneFlags >> gExtraMode) & 1) != 0)
                gSaveSlots[a].completionPercent[gExtraMode] += 3;
        }
        else if ((s8)gSaveSlots[a].furthestLevel[gExtraMode] == 1)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].furthestStage[gExtraMode] * 2;
            gSaveSlots[a].completionPercent[gExtraMode] += (s8)gSaveSlots[a].furthestLevel[gExtraMode] + 4;
        }
        else if ((s8)gSaveSlots[a].furthestLevel[gExtraMode] == 0)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].furthestStage[gExtraMode];
        }
        for (i = 0; i <= 16; i++)
        {
            if ((gSaveSlots[a].bigSwitchFlags[gExtraMode] & (1 << i)) != 0)
                gSaveSlots[a].completionPercent[gExtraMode]++;
        }
        if (gSaveSlots[a].completionPercent[gExtraMode] == 100)
            gSaveSlots[a].milestoneFlags |= 4 << gExtraMode;
    }
}
