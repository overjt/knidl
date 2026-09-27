#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern u16 gHudClock[];
extern struct SaveSlot gSaveSlots[];
extern s32 gSramAvailable;
extern u16 gPlayerCount;
extern s32 gCurSaveSlot;
extern u8 gUnk_080CFE20[];

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
        if (gSaveSlots[i].unk08 > best)
            best = gSaveSlots[i].unk08;
    }
    gSaveSlots[a].unk08 = best + 1;
    gSaveSlots[a].unk0C++;
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
        if (gSaveSlots[i].unk08 > best)
            best = gSaveSlots[i].unk08;
    }
    gSaveSlots[a].unk08 = best + 1;
    gSaveSlots[a].unk0C++;
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
        if (gSaveSlots[i].unk08 > best)
            best = gSaveSlots[i].unk08;
    }
    gSaveSlots[a].unk08 = best + 1;
    gSaveSlots[a].unk0C++;
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
        if (gSaveSlots[i].unk08 > best)
            best = gSaveSlots[i].unk08;
    }
    gSaveSlots[a].unk08 = best + 1;
    gSaveSlots[a].unk0C++;
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
    end = (u32 *)&gSaveSlots[a].unk70;
    while (p != end)
        *p++ = 0x99999999;
    gSaveSlots[a].unk08 = 0;
    UpdateSaveSlotChecksum(a);
}
u32 CalcSaveSlotChecksum(s32 a)
{
    u32 *p;
    u32 *end;
    u32 sum;

    p = (u32 *)&gSaveSlots[a];
    end = (u32 *)&gSaveSlots[a].unk70;
    sum = 0x97538642;
    while (p != end)
        sum += *p++;
    return sum;
}
