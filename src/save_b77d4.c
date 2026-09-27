#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 generation;
    /*0x0C*/ s32 saveCount;
    /*0x10*/ u16 milestoneFlags;
    /*0x12*/ u16 completionPercent[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 checksum;
    /*0x74*/ u8 filler74[0x8C];
};

extern struct SaveSlot gSaveSlots[];
extern u32 gUnk_0200E900[];
extern s16 gInputRecorderMode;
extern s32 gSramAvailable;
extern s16 gCompletionPercent;
extern u16 gMilestoneFlags;
extern s8 gUnk_03002384;
extern s32 gBigSwitchFlags[];
extern s8 gUnk_030023E0;
extern s32 gCurSaveSlot;

void InputRecorderRecordFrame(void);
void InputRecorderPlayFrame(void);
void SelectLatestSaveSlot(void);
s32 ReadSaveSlot(s32 a, s32 b);
void WriteNewSaveFile(s32 a);
void ClearSaveSlot(s32 a);
u32 CalcSaveSlotChecksum(s32 a);
void ResetProgress(void);

void InputRecorderUpdate(void)
{
    switch (gInputRecorderMode)
    {
    default:
        break;
    case 1:
        InputRecorderRecordFrame();
        break;
    case 2:
    case 3:
        InputRecorderPlayFrame();
        break;
    }
}
void InitSaveSlots(void)
{
    s32 i;
    s32 j;
    s32 off;
    s32 r;
    u32 mask;
    u32 *p;

    gSramAvailable = 1;
    for (i = 0; i <= 2; i++)
        ClearSaveSlot(i);
    mask = 0;
    i = 0;
    p = gUnk_0200E900;
    for (; i <= 2; i++)
    {
        r = ReadSaveSlot(3, i);
        if (*p != 0x99999999 || r != 0)
            mask |= 1 << i;
    }
    if (mask != 0 && gSramAvailable != 0)
    {
        ClearSaveSlot(3);
        for (i = 0, off = 0; i <= 3; i++)
        {
            if (((mask >> i) & 1) != 0)
            {
                for (j = 0; j < 2; j++)
                    WriteSramEx((u8 *)gUnk_0200E900, (u8 *)(off + j * 256 + 0x0E000200), 256);
            }
            off += 512;
        }
    }
    mask = 0;
    for (i = 0; i <= 2; i++)
    {
        if (ReadSaveSlot(i, i) != 0)
        {
            ClearSaveSlot(i);
            mask += 1;
        }
    }
    if (mask != 0)
        gSramAvailable = 0;
    SelectLatestSaveSlot();
}
void SelectLatestSaveSlot(void)
{
    s32 i;
    u32 best;

    best = 0;
    gCurSaveSlot = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gSaveSlots[i].generation > best)
        {
            best = gSaveSlots[i].generation;
            gCurSaveSlot = i;
        }
    }
}
s32 ReadSaveSlot(s32 a, s32 b)
{
    s32 i;

    if (gSramAvailable != 0)
    {
        for (i = 0; i <= 1; i++)
        {
            ReadSram((u8 *)((((b * 2) + i) << 8) + 0x0E000200), (u8 *)&gSaveSlots[a], 256);
            if (CalcSaveSlotChecksum(a) == gSaveSlots[a].checksum)
                break;
        }
        if (i == 2)
            goto one;
    }
    return 0;
one:
    return 1;
}
void InitNewSaveFile(s32 a)
{
    gSaveSlots[a].unk04 = a;
    gSaveSlots[a].saveCount = 0;
    ResetProgress();
    WriteNewSaveFile(a);
}
