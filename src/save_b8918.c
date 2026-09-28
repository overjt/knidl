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
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((s8)gSaveSlots[3].unk1A[gExtraMode] > (s8)gLinkSaveSlots[i].unk0A[gExtraMode])
        {
            gSaveSlots[3].unk1A[gExtraMode] = gLinkSaveSlots[i].unk0A[gExtraMode];
            gSaveSlots[3].unk1C[gExtraMode] = gLinkSaveSlots[i].unk0C[gExtraMode];
            gSaveSlots[3].unk16[gExtraMode] = gLinkSaveSlots[i].unk06[gExtraMode];
            gSaveSlots[3].unk18[gExtraMode] = gLinkSaveSlots[i].unk08[gExtraMode];
        }
        else if ((s8)gSaveSlots[3].unk1A[gExtraMode] == (s8)gLinkSaveSlots[i].unk0A[gExtraMode])
        {
            if ((s8)gSaveSlots[3].unk1C[gExtraMode] > (s8)gLinkSaveSlots[i].unk0C[gExtraMode])
            {
                gSaveSlots[3].unk1C[gExtraMode] = gLinkSaveSlots[i].unk0C[gExtraMode];
                gSaveSlots[3].unk16[gExtraMode] = gLinkSaveSlots[i].unk06[gExtraMode];
                gSaveSlots[3].unk18[gExtraMode] = gLinkSaveSlots[i].unk08[gExtraMode];
            }
            else if ((s8)gSaveSlots[3].unk1C[gExtraMode] == (s8)gLinkSaveSlots[i].unk0C[gExtraMode])
            {
                if ((s8)gSaveSlots[3].unk16[gExtraMode] > (s8)gLinkSaveSlots[i].unk06[gExtraMode])
                {
                    gSaveSlots[3].unk16[gExtraMode] = gLinkSaveSlots[i].unk06[gExtraMode];
                    gSaveSlots[3].unk18[gExtraMode] = gLinkSaveSlots[i].unk08[gExtraMode];
                }
                else if ((s8)gSaveSlots[3].unk16[gExtraMode] == (s8)gLinkSaveSlots[i].unk06[gExtraMode])
                {
                    if ((s8)gSaveSlots[3].unk18[gExtraMode] > (s8)gLinkSaveSlots[i].unk08[gExtraMode])
                        gSaveSlots[3].unk18[gExtraMode] = gLinkSaveSlots[i].unk08[gExtraMode];
                }
            }
        }
        if (gSaveSlots[3].completionPercent[gExtraMode] > gLinkSaveSlots[i].unk02[gExtraMode])
            gSaveSlots[3].completionPercent[gExtraMode] = gLinkSaveSlots[i].unk02[gExtraMode];
        gSaveSlots[3].unk20[gExtraMode] &= gLinkSaveSlots[i].unk10[gExtraMode];
        gSaveSlots[3].milestoneFlags &= gLinkSaveSlots[i].unk00;
        for (j = 0; j <= 7; j++)
        {
            for (k = 0; k <= 6; k++)
            {
                if (gSaveSlots[3].unk28[j][k] > gLinkSaveSlots[i].unk18[j][k])
                    gSaveSlots[3].unk28[j][k] = gLinkSaveSlots[i].unk18[j][k];
            }
        }
    }
}
void MergeProgressIntoSaveSlot(s32 a)
{
    s32 i;
    s32 j;

    gSaveSlots[3].unk16[gExtraMode] = gCurLevel;
    gSaveSlots[3].unk18[gExtraMode] = gUnk_03001F20;
    gSaveSlots[3].unk1A[gExtraMode] = gFurthestLevel;
    gSaveSlots[3].unk1C[gExtraMode] = gFurthestStage;
    gSaveSlots[3].unk20[gExtraMode] = gBigSwitchFlags[0];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            gSaveSlots[3].unk28[i][j] &= 15 << ((gExtraMode ^ 1) * 4);
            gSaveSlots[3].unk28[i][j] |= gStageClearStatus[i][j] << (gExtraMode * 4);
        }
    }
    CalcCompletionPercent(gExtraMode);
    gSaveSlots[3].completionPercent[gExtraMode] = gCompletionPercent;
    gSaveSlots[3].milestoneFlags = gMilestoneFlags;
    gSaveSlots[a].unk16[gExtraMode] = gSaveSlots[3].unk16[gExtraMode];
    gSaveSlots[a].unk18[gExtraMode] = gSaveSlots[3].unk18[gExtraMode];
    if ((s8)gSaveSlots[a].unk1A[gExtraMode] < (s8)gSaveSlots[3].unk1A[gExtraMode])
    {
        gSaveSlots[a].unk1A[gExtraMode] = gSaveSlots[3].unk1A[gExtraMode];
        gSaveSlots[a].unk1C[gExtraMode] = gSaveSlots[3].unk1C[gExtraMode];
    }
    else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == (s8)gSaveSlots[3].unk1A[gExtraMode])
    {
        if ((s8)gSaveSlots[a].unk1C[gExtraMode] < (s8)gSaveSlots[3].unk1C[gExtraMode])
            gSaveSlots[a].unk1C[gExtraMode] = gSaveSlots[3].unk1C[gExtraMode];
    }
    gSaveSlots[a].unk20[gExtraMode] |= gSaveSlots[3].unk20[gExtraMode];
    gSaveSlots[a].milestoneFlags |= gSaveSlots[3].milestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            if (gSaveSlots[a].unk28[i][j] < gSaveSlots[3].unk28[i][j])
                gSaveSlots[a].unk28[i][j] = gSaveSlots[3].unk28[i][j];
        }
    }
    if ((gSaveSlots[a].milestoneFlags & (4 << gExtraMode)) != 0)
    {
        gSaveSlots[a].completionPercent[gExtraMode] = 100;
    }
    else
    {
        if ((s8)gSaveSlots[a].unk1A[gExtraMode] > 1)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode] * 2;
            gSaveSlots[a].completionPercent[gExtraMode] += (s8)gSaveSlots[a].unk1A[gExtraMode]
                + ((((s8)gSaveSlots[a].unk1A[gExtraMode] - 2) * 3) * 4 + 14);
            if (((gSaveSlots[a].milestoneFlags >> gExtraMode) & 1) != 0)
                gSaveSlots[a].completionPercent[gExtraMode] += 3;
        }
        else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == 1)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode] * 2;
            gSaveSlots[a].completionPercent[gExtraMode] += (s8)gSaveSlots[a].unk1A[gExtraMode] + 4;
        }
        else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == 0)
        {
            gSaveSlots[a].completionPercent[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode];
        }
        for (i = 0; i <= 16; i++)
        {
            if ((gSaveSlots[a].unk20[gExtraMode] & (1 << i)) != 0)
                gSaveSlots[a].completionPercent[gExtraMode]++;
        }
        if (gSaveSlots[a].completionPercent[gExtraMode] == 100)
            gSaveSlots[a].milestoneFlags |= 4 << gExtraMode;
    }
}
