#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "save.h"

void StoreProgressInSaveSlot(s32 a)
{
    s32 i;
    s32 stage;

    gSaveSlots[a].curLevel[gExtraMode] = gCurLevel;
    gSaveSlots[a].curStage[gExtraMode] = gCurStage;
    gSaveSlots[a].furthestLevel[gExtraMode] = gFurthestLevel;
    gSaveSlots[a].furthestStage[gExtraMode] = gFurthestStage;
    gSaveSlots[a].bigSwitchFlags[gExtraMode] = gBigSwitchFlags[0];
    for (i = 0; i <= 7; i++)
    {
        for (stage = 0; stage <= 6; stage++)
        {
            gSaveSlots[a].stageClearStatus[i][stage] &= 15 << ((gExtraMode ^ 1) * 4);
            gSaveSlots[a].stageClearStatus[i][stage] |= gStageClearStatus[i][stage] << (gExtraMode * 4);
        }
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[a].bossEnduranceBestTime[i] = gBossEnduranceBestTime[i];
        gSaveSlots[a].metaKnightmareBestTime[i] = gMetaKnightmareBestTime[i];
    }
    CalcCompletionPercent(gExtraMode);
    gSaveSlots[a].completionPercent[gExtraMode] = gCompletionPercent;
    gSaveSlots[a].milestoneFlags = gMilestoneFlags;
}
void StoreProgressInBothHalves(s32 a)
{
    s32 i;
    s32 stage;

    for (i = 0; i <= 1; i++)
    {
        gSaveSlots[a].curLevel[i] = gCurLevel;
        gSaveSlots[a].curStage[i] = gCurStage;
        gSaveSlots[a].furthestLevel[i] = gFurthestLevel;
        gSaveSlots[a].furthestStage[i] = gFurthestStage;
        gSaveSlots[a].bigSwitchFlags[i] = gBigSwitchFlags[0];
        CalcCompletionPercent(i);
        gSaveSlots[a].completionPercent[i] = gCompletionPercent;
    }
    gSaveSlots[a].milestoneFlags = gMilestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (stage = 0; stage <= 6; stage++)
            gSaveSlots[a].stageClearStatus[i][stage] = (gStageClearStatus[i][stage] << 4) | gStageClearStatus[i][stage];
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[a].bossEnduranceBestTime[i] = gBossEnduranceBestTime[i];
        gSaveSlots[a].metaKnightmareBestTime[i] = gMetaKnightmareBestTime[i];
    }
}
void LoadSaveSlot(s32 a)
{
    s32 i;
    s32 stage;

    if (a == -1)
        return;
    if (gPlayerCount != 1)
        a = 3;
    gMilestoneFlags = gSaveSlots[a].milestoneFlags;
    gCurLevel = gSaveSlots[a].curLevel[gExtraMode];
    gCurStage = gSaveSlots[a].curStage[gExtraMode];
    gFurthestLevel = gSaveSlots[a].furthestLevel[gExtraMode];
    gFurthestStage = gSaveSlots[a].furthestStage[gExtraMode];
    gBigSwitchFlags[0] = gSaveSlots[a].bigSwitchFlags[gExtraMode];
    for (i = 0; i <= 7; i++)
    {
        for (stage = 0; stage <= 6; stage++)
            gStageClearStatus[i][stage] = (gSaveSlots[a].stageClearStatus[i][stage] >> (gExtraMode * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gBossEnduranceBestTime[i] = gSaveSlots[a].bossEnduranceBestTime[i];
        gMetaKnightmareBestTime[i] = gSaveSlots[a].metaKnightmareBestTime[i];
    }
    CalcCompletionPercent(gExtraMode);
}
void ResetLevelProgress(void)
{
    s32 level;
    s32 stage;

    gCurLevel = 0;
    gCurStage = 0;
    gFurthestLevel = 0;
    gFurthestStage = 0;
    gBigSwitchFlags[0] = 0;
    for (level = 0; level <= 7; level++)
    {
        for (stage = 0; stage <= 6; stage++)
            gStageClearStatus[level][stage] = 0;
    }
    CalcCompletionPercent(0);
}
void ResetProgress(void)
{
    s32 level;
    s32 stage;
    u16 *p;
    u16 *q;

    gExtraMode = 0;
    gMilestoneFlags = 0;
    gCurLevel = 0;
    gCurStage = 0;
    gFurthestLevel = 0;
    gFurthestStage = 0;
    gBigSwitchFlags[0] = 0;
    gCompletionPercent = 0;
    for (level = 0; level <= 7; level++)
    {
        for (stage = 0; stage <= 6; stage++)
            gStageClearStatus[level][stage] = 0;
    }
    p = gBossEnduranceBestTime;
    q = gMetaKnightmareBestTime;
    q[0] = 0;
    p[0] = 0;
    q[1] = 0;
    p[1] = 0;
    q[2] = 0;
    p[2] = 0;
    q[3] = 0;
    p[3] = 0;
}
s32 CheckNewMilestones(void)
{
    s32 r;
    u16 v;

    r = 0;
    v = gMilestoneFlags;
    if (((v >> gExtraMode) & 1) != 0 && (v & (16 << gExtraMode)) == 0)
    {
        v |= 16 << gExtraMode;
        gMilestoneFlags = v;
        r = 1;
    }
    v = gMilestoneFlags;
    if ((v & (4 << gExtraMode)) != 0 && (v & (64 << gExtraMode)) == 0)
    {
        v |= 64 << gExtraMode;
        gMilestoneFlags = v;
        r |= 2;
    }
    if (r == 0)
        goto zero;
    if (gCurSaveSlot == -1)
        return r;
    if (gPlayerCount == 1)
        StoreProgressInSaveSlot(gCurSaveSlot);
    else
        MergeProgressIntoSaveSlot(gCurSaveSlot);
    gSaveSlots[gCurSaveSlot].saveCount++;
    UpdateSaveSlotChecksum(gCurSaveSlot);
    WriteSaveSlot(gCurSaveSlot);
    return r;
zero:
    return 0;
}
u32 ReadInputRecording(void)
{
    if (gSramAvailable == 0)
        return 0;
    ReadSram((u8 *)(SRAM_START + 0x800), gInputRecording, 240 << 7);
}
u32 WriteInputRecording(void)
{
    if (gSramAvailable == 0)
        return 0;
    return WriteSramEx(gInputRecording, (u8 *)(SRAM_START + 0x800), 240 << 7);
}
u32 WriteInputRecordingEntry(u8 *src, s32 i)
{
    return WriteSramEx(src, (u8 *)(i * 2 + (SRAM_START + 0x92C)), 2);
}
void CopySaveSlotToLinkSlot(void)
{
    s32 i;
    s32 j;

    gSaveSlots[3].curLevel[gExtraMode] = gSaveSlots[gCurSaveSlot].curLevel[gExtraMode];
    gSaveSlots[3].curStage[gExtraMode] = gSaveSlots[gCurSaveSlot].curStage[gExtraMode];
    gSaveSlots[3].furthestLevel[gExtraMode] = gSaveSlots[gCurSaveSlot].furthestLevel[gExtraMode];
    gSaveSlots[3].furthestStage[gExtraMode] = gSaveSlots[gCurSaveSlot].furthestStage[gExtraMode];
    gSaveSlots[3].bigSwitchFlags[gExtraMode] = gSaveSlots[gCurSaveSlot].bigSwitchFlags[gExtraMode];
    gSaveSlots[3].completionPercent[gExtraMode] = gSaveSlots[gCurSaveSlot].completionPercent[gExtraMode];
    gSaveSlots[3].milestoneFlags = gSaveSlots[gCurSaveSlot].milestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gSaveSlots[3].stageClearStatus[i][j] = gSaveSlots[gCurSaveSlot].stageClearStatus[i][j];
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[3].bossEnduranceBestTime[i] = gSaveSlots[gCurSaveSlot].bossEnduranceBestTime[i];
        gSaveSlots[3].metaKnightmareBestTime[i] = gSaveSlots[gCurSaveSlot].metaKnightmareBestTime[i];
    }
}
void FillSendCmdWithSaveSlot(void)
{
    s32 q;
    s32 r;

    gSendCmd[0] = gLinkSaveSlotPart | (204 << 7);
    switch (gLinkSaveSlotPart)
    {
    case 0:
        break;
    case 1:
        gSendCmd[1] = ((s8)gSaveSlots[gCurSaveSlot].curLevel[gExtraMode] << 8)
                         | (s8)gSaveSlots[gCurSaveSlot].curStage[gExtraMode];
        gSendCmd[2] = ((s8)gSaveSlots[gCurSaveSlot].furthestLevel[gExtraMode] << 8)
                         | (s8)gSaveSlots[gCurSaveSlot].furthestStage[gExtraMode];
        gSendCmd[3] = gSaveSlots[gCurSaveSlot].completionPercent[gExtraMode];
        break;
    case 2:
        gSendCmd[1] = gSaveSlots[gCurSaveSlot].bigSwitchFlags[gExtraMode] >> 16;
        gSendCmd[2] = gSaveSlots[gCurSaveSlot].bigSwitchFlags[gExtraMode];
        gSendCmd[3] = gSaveSlots[gCurSaveSlot].milestoneFlags;
        break;
    default:
        q = Div(gLinkSaveSlotPart - 3, 3);
        r = Mod(gLinkSaveSlotPart - 3, 3);
        if (q > 7)
            break;
        switch (r)
        {
        case 0:
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][0];
            gSendCmd[2] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][1];
            gSendCmd[3] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][2];
            break;
        case 1:
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][3];
            gSendCmd[2] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][4];
            gSendCmd[3] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][5];
            break;
        case 2:
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].stageClearStatus[q][6];
            break;
        }
        break;
    }
}
