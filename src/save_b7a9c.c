#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "save.h"

s32 WriteSaveSlot(s32 a);
u32 WriteSramSignature(void);
void ClearSaveSlot(s32 a);
u32 CalcSaveSlotChecksum(s32 a);
u32 UpdateSaveSlotChecksum(s32 a);
void StoreProgressInSaveSlot(s32 a);
void StoreProgressInBothHalves(s32 a);
void MergeProgressIntoSaveSlot(s32 a);

s32 WriteSaveSlot(s32 a)
{
    s32 i;
    s32 n;

    if (gSramAvailable == 0)
        return 0;
    n = 0;
    for (i = 0; i < 2; i++)
    {
        if (WriteSramEx((u8 *)&gSaveSlots[a], (u8 *)((a << 9) + 0x0E000200 + i * 256), 256) != 0)
            n++;
    }
    return n;
}
u32 WriteSramSignature(void)
{
    if (gSramAvailable == 0)
        return 0;
    return WriteSramEx(gUnk_080CFE20, (u8 *)(224 << 20), 10);
}
void WriteNewSaveFile(s32 a)
{
    u32 best;
    u32 i;

    StoreProgressInBothHalves(a);
    best = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gSaveSlots[i].generation > best)
            best = gSaveSlots[i].generation;
    }
    gSaveSlots[a].generation = best + 1;
    gSaveSlots[a].saveCount++;
    UpdateSaveSlotChecksum(a);
    WriteSaveSlot(a);
    WriteSramSignature();
}
void SaveProgress(s32 a)
{
    u32 best;
    u32 i;

    if (a == -1)
        return;
    if (gPlayerCount == 1)
        StoreProgressInSaveSlot(gCurSaveSlot);
    else
        MergeProgressIntoSaveSlot(gCurSaveSlot);
    best = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gSaveSlots[i].generation > best)
            best = gSaveSlots[i].generation;
    }
    gSaveSlots[a].generation = best + 1;
    gSaveSlots[a].saveCount++;
    UpdateSaveSlotChecksum(a);
    WriteSaveSlot(a);
}
void sub_080b7c00(s32 a)
{
    u32 m;
    u32 s;
    u32 t;
    s32 i;
    u32 best;

    if (a == -1)
        return;
    m = 1;
    t = 0;
    s = 0;
    for (i = 0; i <= 3; i++)
    {
        s += gSaveSlots[a].unk68[i] * m;
        t += gHudClock[i] * m;
        m = ((m << 4) - m) << 2;
    }
    if (s != 0 && s < t)
        return;
    for (i = 0; i < 4; i++)
        gSaveSlots[a].unk68[i] = gHudClock[i];
    best = 0;
    for (i = 0; i < 3; i++)
    {
        if (gSaveSlots[i].generation > best)
            best = gSaveSlots[i].generation;
    }
    gSaveSlots[a].generation = best + 1;
    gSaveSlots[a].saveCount++;
    UpdateSaveSlotChecksum(a);
    WriteSaveSlot(a);
}
void sub_080b7cb4(s32 a)
{
    u32 m;
    u32 s;
    u32 t;
    s32 i;
    u32 best;

    if (a == -1)
        return;
    if (gPlayerCount != 1)
        return;
    m = 1;
    t = 0;
    s = 0;
    for (i = 0; i <= 3; i++)
    {
        s += gSaveSlots[a].unk60[i] * m;
        t += gHudClock[i] * m;
        m = ((m << 4) - m) << 2;
    }
    if (s != 0 && s < t)
        return;
    for (i = 0; i < 4; i++)
        gSaveSlots[a].unk60[i] = gHudClock[i];
    best = 0;
    for (i = 0; i < 3; i++)
    {
        if (gSaveSlots[i].generation > best)
            best = gSaveSlots[i].generation;
    }
    gSaveSlots[a].generation = best + 1;
    gSaveSlots[a].saveCount++;
    UpdateSaveSlotChecksum(a);
    WriteSaveSlot(a);
}
void EraseSaveSlot(s32 a)
{
    ClearSaveSlot(a);
    if (gSramAvailable != 0)
        WriteSaveSlot(a);
}
void ClearSaveSlot(s32 a)
{
    u32 *p;
    u32 *end;

    p = (u32 *)&gSaveSlots[a];
    end = (u32 *)&gSaveSlots[a].checksum;
    while (p != end)
        *p++ = 0x99999999;
    gSaveSlots[a].generation = 0;
    UpdateSaveSlotChecksum(a);
}
u32 CalcSaveSlotChecksum(s32 a)
{
    u32 *p;
    u32 *end;
    u32 sum;

    p = (u32 *)&gSaveSlots[a];
    end = (u32 *)&gSaveSlots[a].checksum;
    sum = 0x97538642;
    while (p != end)
        sum += *p++;
    return sum;
}
