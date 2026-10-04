#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "gba/types.h"

/* save.h: the RAM cells and ROM tables of the save file, the SRAM records,
   the wavy-scroll effect, the input recorder and the link-play results screen
   (M34).  One declaration per symbol, with the type its consumers prove
   (issue #36 phase 2, docs/header-conventions.md). */

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

struct LinkSave
{
    /*0x000*/ u32 unk00;
    /*0x004*/ u16 unk04;
    /*0x006*/ u16 unk06;
    /*0x008*/ u8 unk08;
    /*0x009*/ u8 unk09;
    /*0x00A*/ u8 unk0A;
    /*0x00B*/ u8 unk0B;
    /*0x00C*/ u16 unk0C;
    /*0x00E*/ u16 unk0E;
    /*0x010*/ u16 unk10;
    /*0x012*/ u8 unk12;
    /*0x013*/ u8 unk13;
    /*0x014*/ u16 unk14;
    /*0x016*/ u16 unk16[4];
    /*0x01E*/ u16 unk1E[4];
    /*0x026*/ u16 unk26[4];
    /*0x02E*/ u16 unk2E[4];
    /*0x036*/ u16 unk36;
    /*0x038*/ u8 unk38[8];
    /*0x040*/ u16 unk40[8][8];
    /*0x0C0*/ u16 unkC0;
    /*0x0C2*/ u16 unkC2;
    /*0x0C4*/ u8 unkC4[2];
    /*0x0C6*/ u8 unkC6[2];
    /*0x0C8*/ u8 unkC8[2];
    /*0x0CA*/ u8 unkCA[2];
    /*0x0CC*/ u32 unkCC[2];
    /*0x0D4*/ u8 unkD4[8][7];
    /*0x10C*/ u16 unk10C[4];
    /*0x114*/ u16 unk114[4];
    /*0x11C*/ u16 unk11C;
    /*0x11E*/ u8 unk11E;
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 unk12C[0x3B6A];
};

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 generation;
    /*0x0C*/ s32 saveCount;
    /*0x10*/ u16 milestoneFlags;
    /*0x12*/ u16 completionPercent[2];
    /*0x16*/ u8 curLevel[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 furthestLevel[2];
    /*0x1C*/ u8 furthestStage[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 bigSwitchFlags[2];
    /*0x28*/ u8 stageClearStatus[8][7];
    /*0x60*/ u16 bossEnduranceBestTime[4];
    /*0x68*/ u16 metaKnightmareBestTime[4];
    /*0x70*/ u32 checksum;
    /*0x74*/ u8 filler74[0x8C];
};

/* EWRAM */
extern u16 gUnk_02000010[];
extern struct SaveSlot gSaveSlots[];
extern u32 gUnk_0200E900[];
extern struct LinkRec gLinkSaveSlots[];
extern u8 gLinkSaveSlotPart;
extern u32 gInputRecorderDemo;
extern u8 gInputRecorderRunning;
extern s16 gInputRecorderMode;
extern u16 gInputRecorderKeys[];
extern u8 gInputRecorderEntryFrames[];
extern struct LinkSave *gInputRecordingPtr;
extern u16 gInputRecorderEndPos[];
extern u16 gInputRecorderNextPos[];
extern u8 gInputRecording[];
extern u16 gInputRecorderCurPos[];
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern s16 gHBlankScrollEffect;
extern u16 gHBlankScrollDmaTable[];

/* IWRAM */
extern u32 gHBlankDmaDest;
extern u32 gHBlankDmaCnt[];
extern s32 gSramAvailable;
extern u32 gHBlankDmaSrc[];
extern u16 gBossEnduranceBestTime[];
extern u16 gCompletionPercent;
extern u16 gMetaKnightmareBestTime[];

/* ROM */
extern u8 gUnk_080CFE20[];
extern u8 gUnk_085ADD1C[];
extern u8 gUnk_085B09BC[];
extern u8 gUnk_085B09DC[];
extern u8 gUnk_085B0A10[];
extern u8 gUnk_085B0A30[];
extern u8 gUnk_085B0A64[];
extern u8 gUnk_085B0AB0[];
extern u8 gUnk_085B0AD0[];
extern u8 gUnk_085B0AD4[];
extern u8 gUnk_085B0AFC[];
extern u8 gUnk_085B0B00[];
extern u8 gUnk_085B0B08[];
extern u8 gUnk_085B0B10[];
extern u8 gUnk_085B0B28[];
extern u8 gUnk_085B0B5C[];
extern u8 gUnk_085B0BB4[];
extern u8 gUnk_085B0BD4[];
extern u8 gUnk_085B0C2C[];
extern u8 gUnk_087561C4[];
extern s8 gUnk_087561CC[];
extern u32 gUnk_0875625C[];
extern u16 gUnk_08756268[];
extern u32 gPlayerLifeRequestStates[];
extern u32 gPlayerLifeRequestStateUpdates[];


/* Functions (defined in the files named above each group). */

/* src/save_b6154.c */
s32 sub_080b6154(void);

/* src/save_b6290.c */
s32 sub_080b6290(void);

/* src/save_b63a4.c */
s32 sub_080b63a4(void);

/* src/save_b6474.c */
s32 sub_080b6474(void);
s32 sub_080b6570(void);
s32 sub_080b67dc(void);

/* src/save_b6a90.c */
void UpdateRoomHBlankScroll(void);

/* src/save_b6b08.c */
s32 sub_080b6b08(void);

/* src/save_b6c40.c */
s32 sub_080b6c40(void);

/* src/save_b6d04.c */
s32 sub_080b6d04(void);

/* src/save_b6e44.c */
void ResetHBlankScroll(void);
void StopHBlankScroll(void);
void StartHBlankScroll(s32 a);
void StartRoomHBlankScroll(s32 a);
void HoldHBlankScroll(void);
void SuspendHBlankScroll(void);
void RestoreRoomHBlankScroll(void);
void ResumeHBlankScroll(void);

/* src/save_b6f38.c */
void InputRecorderStart(void);

/* src/save_b72bc.c */
void InputRecorderRestoreState(void);

/* src/save_b75a4.c */
void InputRecorderRecordFrame(void);
void InputRecorderPlayFrame(void);

/* src/save_b77d4.c */
void InputRecorderUpdate(void);
void InitSaveSlots(void);
void SelectLatestSaveSlot(void);
s32 ReadSaveSlot(s32 a, s32 b);
void InitNewSaveFile(s32 a);

/* src/save_b79b8.c */
s32 CalcCompletionPercent(s32 a);

/* src/save_b7a9c.c */
s32 WriteSaveSlot(s32 a);
u32 WriteSramSignature(void);
void WriteNewSaveFile(s32 a);
void SaveProgress(s32 a);
void SaveMetaKnightmareBestTime(s32 a);
void SaveBossEnduranceBestTime(s32 a);
void EraseSaveSlot(s32 a);
void ClearSaveSlot(s32 a);
u32 CalcSaveSlotChecksum(s32 a);

/* src/save_b7df4.c */
u32 UpdateSaveSlotChecksum(s32 a);

/* src/save_b7e14.c */
void StoreProgressInSaveSlot(s32 a);
void StoreProgressInBothHalves(s32 a);
void LoadSaveSlot(s32 a);
void ResetProgress(void);
s32 CheckNewMilestones(void);
u32 ReadInputRecording(void);
u32 WriteInputRecording(void);
u32 WriteInputRecordingEntry(u8 *src, s32 i);
void CopySaveSlotToLinkSlot(void);
void FillSendCmdWithSaveSlot(void);

/* src/save_b8694.c */
void ReceiveLinkSaveSlots(void);

/* src/save_b8888.c */
void ExchangeLinkSaveSlots(void);

/* src/save_b8918.c */
void MergeLinkSaveSlots(void);
void MergeProgressIntoSaveSlot(s32 a);

/* src/save_b8ea0.c */
void sub_080b8ea0(void);
void sub_080b8ebc(void);
void sub_080b8ef4(void);
void sub_080b8f8c(s32 a);
void sub_080b8ff0(void);
void sub_080b902c(void);
void sub_080b9064(void);
void sub_080b9090(void);
void sub_080b90c8(void);
void sub_080b90f8(void);
void sub_080b9108(void);
void sub_080b9118(void);
void sub_080b9140(void);
void sub_080b9198(void);
void sub_080b91fc(void);
void sub_080b927c(void);
void sub_080b9344(void);
void sub_080b938c(void);
void sub_080b93d8(void);
s32 sub_080b9424(void);
void sub_080b94b4(s32 a, s32 b);
void sub_080b9578(void);
void sub_080b95ac(void);
void sub_080b95ec(void);
void PlayerLifeRequestInit(void);
void PlayerLifeRequestUpdate(void);
void PlayerLifeRequestEnterState(void);
void sub_080b9674(void);
void sub_080b9690(void);
void sub_080b96a0(void);
void sub_080b96bc(void);
void PlayerLifeRequestWait(void);
void PlayerLifeRequestWaitUpdate(void);
void PlayerLifeRequestReceive(void);
void PlayerLifeRequestReceiveUpdate(void);
void sub_080b9770(void);
void sub_080b9798(void);
void sub_080b97a4(void);
void sub_080b97d0(void);
void sub_080b97dc(void);
void sub_080b97f8(void);
void sub_080b97fc(s32 a);
void sub_080b9878(void);
void sub_080b98c0(void);
void sub_080b9968(s32 a, s32 b);
void sub_080b99e8(s32 a, s32 b, s32 c);
void sub_080b9a88(s32 a);
void sub_080b9b08(s32 a);
void sub_080b9b98(s32 a);
void sub_080b9c28(void);
void sub_080b9c74(void);
void sub_080b9cc0(void);

#endif /* GUARD_SAVE_H */
