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
    s32 j;

    gSaveSlots[a].unk16[gExtraMode] = gUnk_030023B8;
    gSaveSlots[a].unk18[gExtraMode] = gUnk_03001F20;
    gSaveSlots[a].unk1A[gExtraMode] = gUnk_030023E0;
    gSaveSlots[a].unk1C[gExtraMode] = gUnk_03002384;
    gSaveSlots[a].unk20[gExtraMode] = gBigSwitchFlags[0];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            gSaveSlots[a].unk28[i][j] &= 15 << ((gExtraMode ^ 1) * 4);
            gSaveSlots[a].unk28[i][j] |= gUnk_03002400[i][j] << (gExtraMode * 4);
        }
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[a].unk60[i] = gUnk_03001F18[i];
        gSaveSlots[a].unk68[i] = gUnk_03002378[i];
    }
    CalcCompletionPercent(gExtraMode);
    gSaveSlots[a].completionPercent[gExtraMode] = gCompletionPercent;
    gSaveSlots[a].milestoneFlags = gMilestoneFlags;
}
void StoreProgressInBothHalves(s32 a)
{
    s32 i;
    s32 j;

    for (i = 0; i <= 1; i++)
    {
        gSaveSlots[a].unk16[i] = gUnk_030023B8;
        gSaveSlots[a].unk18[i] = gUnk_03001F20;
        gSaveSlots[a].unk1A[i] = gUnk_030023E0;
        gSaveSlots[a].unk1C[i] = gUnk_03002384;
        gSaveSlots[a].unk20[i] = gBigSwitchFlags[0];
        CalcCompletionPercent(i);
        gSaveSlots[a].completionPercent[i] = gCompletionPercent;
    }
    gSaveSlots[a].milestoneFlags = gMilestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gSaveSlots[a].unk28[i][j] = (gUnk_03002400[i][j] << 4) | gUnk_03002400[i][j];
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[a].unk60[i] = gUnk_03001F18[i];
        gSaveSlots[a].unk68[i] = gUnk_03002378[i];
    }
}
void LoadSaveSlot(s32 a)
{
    s32 i;
    s32 j;

    if (a == -1)
        return;
    if (gPlayerCount != 1)
        a = 3;
    gMilestoneFlags = gSaveSlots[a].milestoneFlags;
    gUnk_030023B8 = gSaveSlots[a].unk16[gExtraMode];
    gUnk_03001F20 = gSaveSlots[a].unk18[gExtraMode];
    gUnk_030023E0 = gSaveSlots[a].unk1A[gExtraMode];
    gUnk_03002384 = gSaveSlots[a].unk1C[gExtraMode];
    gBigSwitchFlags[0] = gSaveSlots[a].unk20[gExtraMode];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = (gSaveSlots[a].unk28[i][j] >> (gExtraMode * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_03001F18[i] = gSaveSlots[a].unk60[i];
        gUnk_03002378[i] = gSaveSlots[a].unk68[i];
    }
    CalcCompletionPercent(gExtraMode);
}
void sub_080b81a0(void)
{
    s32 i;
    s32 j;

    gUnk_030023B8 = 0;
    gUnk_03001F20 = 0;
    gUnk_030023E0 = 0;
    gUnk_03002384 = 0;
    gBigSwitchFlags[0] = 0;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = 0;
    }
    CalcCompletionPercent(0);
}
void ResetProgress(void)
{
    s32 i;
    s32 j;
    u16 *p;
    u16 *q;

    gExtraMode = 0;
    gMilestoneFlags = 0;
    gUnk_030023B8 = 0;
    gUnk_03001F20 = 0;
    gUnk_030023E0 = 0;
    gUnk_03002384 = 0;
    gBigSwitchFlags[0] = 0;
    gCompletionPercent = 0;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = 0;
    }
    p = gUnk_03001F18;
    q = gUnk_03002378;
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
    ReadSram((u8 *)0x0E000800, gInputRecording, 240 << 7);
}
u32 WriteInputRecording(void)
{
    if (gSramAvailable == 0)
        return 0;
    return WriteSramEx(gInputRecording, (u8 *)0x0E000800, 240 << 7);
}
u32 WriteInputRecordingEntry(u8 *src, s32 i)
{
    return WriteSramEx(src, (u8 *)(i * 2 + 0x0E00092C), 2);
}
void CopySaveSlotToLinkSlot(void)
{
    s32 i;
    s32 j;

    gSaveSlots[3].unk16[gExtraMode] = gSaveSlots[gCurSaveSlot].unk16[gExtraMode];
    gSaveSlots[3].unk18[gExtraMode] = gSaveSlots[gCurSaveSlot].unk18[gExtraMode];
    gSaveSlots[3].unk1A[gExtraMode] = gSaveSlots[gCurSaveSlot].unk1A[gExtraMode];
    gSaveSlots[3].unk1C[gExtraMode] = gSaveSlots[gCurSaveSlot].unk1C[gExtraMode];
    gSaveSlots[3].unk20[gExtraMode] = gSaveSlots[gCurSaveSlot].unk20[gExtraMode];
    gSaveSlots[3].completionPercent[gExtraMode] = gSaveSlots[gCurSaveSlot].completionPercent[gExtraMode];
    gSaveSlots[3].milestoneFlags = gSaveSlots[gCurSaveSlot].milestoneFlags;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gSaveSlots[3].unk28[i][j] = gSaveSlots[gCurSaveSlot].unk28[i][j];
    }
    for (i = 0; i <= 3; i++)
    {
        gSaveSlots[3].unk60[i] = gSaveSlots[gCurSaveSlot].unk60[i];
        gSaveSlots[3].unk68[i] = gSaveSlots[gCurSaveSlot].unk68[i];
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
        gSendCmd[1] = ((s8)gSaveSlots[gCurSaveSlot].unk16[gExtraMode] << 8)
                         | (s8)gSaveSlots[gCurSaveSlot].unk18[gExtraMode];
        gSendCmd[2] = ((s8)gSaveSlots[gCurSaveSlot].unk1A[gExtraMode] << 8)
                         | (s8)gSaveSlots[gCurSaveSlot].unk1C[gExtraMode];
        gSendCmd[3] = gSaveSlots[gCurSaveSlot].completionPercent[gExtraMode];
        break;
    case 2:
        gSendCmd[1] = gSaveSlots[gCurSaveSlot].unk20[gExtraMode] >> 16;
        gSendCmd[2] = gSaveSlots[gCurSaveSlot].unk20[gExtraMode];
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
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].unk28[q][0];
            gSendCmd[2] = gSaveSlots[gCurSaveSlot].unk28[q][1];
            gSendCmd[3] = gSaveSlots[gCurSaveSlot].unk28[q][2];
            break;
        case 1:
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].unk28[q][3];
            gSendCmd[2] = gSaveSlots[gCurSaveSlot].unk28[q][4];
            gSendCmd[3] = gSaveSlots[gCurSaveSlot].unk28[q][5];
            break;
        case 2:
            gSendCmd[1] = gSaveSlots[gCurSaveSlot].unk28[q][6];
            break;
        }
        break;
    }
}
