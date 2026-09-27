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

struct LinkRec
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02[2];
    /*0x06*/ u8 unk06[2];
    /*0x08*/ u8 unk08[2];
    /*0x0A*/ u8 unk0A[2];
    /*0x0C*/ u8 unk0C[2];
    /*0x0E*/ u16 pad0E;
    /*0x10*/ u32 unk10[2];
    /*0x18*/ u8 unk18[8][7];
    /*0x50*/ u8 filler50[0x10];
};

extern s32 Div(s32 a, s32 b);
extern s32 Mod(s32 a, s32 b);
extern u16 gHudClock[];
extern struct SaveSlot gSaveSlots[];
extern struct LinkRec gLinkSaveSlots[];
extern u8 gLinkSaveSlotPart;
extern u8 gInputRecording[];
extern s32 gSramAvailable;
extern u16 gUnk_03001F18[];
extern u8 gUnk_03001F20;
extern u16 gCompletionPercent;
extern u16 gMilestoneFlags;
extern u16 gUnk_03002378[];
extern s8 gUnk_03002384;
extern u16 gPlayerCount;
extern u8 gUnk_030023B8;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern s32 gCurSaveSlot;
extern u8 gUnk_03002400[8][7];
extern u8 gExtraMode;
extern u16 gRecvCmds[];
extern u16 gSendCmd[];
extern void LinkStopKeyExchange(void);
extern void LinkStartRecordExchange(void);
extern void RunLinkFrame(void);

s32 CalcCompletionPercent(s32 a);
s32 WriteSaveSlot(s32 a);
u32 UpdateSaveSlotChecksum(s32 a);
void StoreProgressInSaveSlot(s32 a);
void MergeProgressIntoSaveSlot(s32 a);

void sub_080b8888(void)
{
    s32 n;
    s32 i;

    LinkStartRecordExchange();
    gLinkSaveSlotPart = 0;
    do
    {
        RunLinkFrame();
        n = 0;
        if (gLinkSaveSlotPart <= 26)
        {
            gLinkSaveSlotPart++;
        }
        else
        {
            for (i = 0; i < gPlayerCount; i++)
            {
                if ((gRecvCmds[i] & 0xFF00) == (204 << 7) && (gRecvCmds[i] & 255) == 27)
                    n++;
            }
        }
    } while (n != gPlayerCount);
    LinkStopKeyExchange();
}
