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
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern u16 gHudClock[];
extern struct SaveSlot gSaveSlots[];
extern s32 gSramAvailable;
extern u16 gPlayerCount;

s32 WriteSaveSlot(s32 a);
void ClearSaveSlot(s32 a);
u32 CalcSaveSlotChecksum(s32 a);
u32 UpdateSaveSlotChecksum(s32 a);

u32 UpdateSaveSlotChecksum(s32 a)
{
    gSaveSlots[a].unk70 = CalcSaveSlotChecksum(a);
}
