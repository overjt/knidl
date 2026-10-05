#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "gba/types.h"
#include "constants/rooms.h"

/* save.h: the RAM cells and ROM tables of the save file, the SRAM records,
   the wavy-scroll effect, the input recorder and the link-play results screen
   (M34).  One declaration per symbol, with the type its consumers prove
   (issue #36 phase 2, docs/header-conventions.md). */

struct LinkRec
{
    /*0x00*/ u16 milestoneFlags;
    /*0x02*/ u16 completionPercent[2];
    /*0x06*/ u8 curLevel[2];
    /*0x08*/ u8 curStage[2];
    /*0x0A*/ u8 furthestLevel[2];
    /*0x0C*/ u8 furthestStage[2];
    /*0x0E*/ u16 pad0E;
    /*0x10*/ u32 bigSwitchFlags[2];
    /*0x18*/ u8 stageClearStatus[8][7];
    /*0x50*/ u8 filler50[0x10];
};

struct InputRecording
{
    /*0x000*/ u32 rngValue;
    /*0x004*/ u16 vblankCount;
    /*0x006*/ u16 frameCount;
    /*0x008*/ u8 levelIndex;
    /*0x009*/ u8 stageIndex;
    /*0x00A*/ u8 roomIndex;
    /*0x00B*/ u8 extraMode;
    /*0x00C*/ u16 playerCount;
    /*0x00E*/ u16 roomEntryX;
    /*0x010*/ u16 roomEntryY;
    /*0x012*/ u8 roomEntryMode;
    /*0x013*/ u8 roomPlayerMode;
    /*0x014*/ u16 maxHealth;
    /*0x016*/ u16 playerLives[4];
    /*0x01E*/ u16 playerHealth[4];
    /*0x026*/ u16 playerAbilities[4];
    /*0x02E*/ u16 playerAbilityUses[4];
    /*0x036*/ u16 warpStarStationLevels;
    /*0x038*/ u8 usedSubGameDoors[8];
    /*0x040*/ u16 usedRoomObjects[8][8];
    /*0x0C0*/ u16 milestoneFlags;
    /*0x0C2*/ u16 completionPercent;
    /*0x0C4*/ u8 curLevel[2];
    /*0x0C6*/ u8 curStage[2];
    /*0x0C8*/ u8 furthestLevel[2];
    /*0x0CA*/ u8 furthestStage[2];
    /*0x0CC*/ u32 bigSwitchFlags[2];
    /*0x0D4*/ u8 stageClearStatus[8][7];
    /*0x10C*/ u16 bossEnduranceBestTime[4];
    /*0x114*/ u16 metaKnightmareBestTime[4];
    /*0x11C*/ u16 localPlayer;
    /*0x11E*/ u8 metaKnightmareMode;
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 keyLog[0x3B6A];
};

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 slotIndex;
    /*0x08*/ u32 generation;
    /*0x0C*/ s32 saveCount;
    /*0x10*/ u16 milestoneFlags;
    /*0x12*/ u16 completionPercent[2];
    /*0x16*/ u8 curLevel[2];
    /*0x18*/ u8 curStage[2];
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
extern u16 gLifeRequestShownLives[];
extern struct SaveSlot gSaveSlots[];
extern u32 gUnk_0200E900[];
extern struct LinkRec gLinkSaveSlots[];
extern u8 gLinkSaveSlotPart;
extern u32 gInputRecorderDemo;
extern u8 gInputRecorderRunning;
extern s16 gInputRecorderMode;
extern u16 gInputRecorderKeys[];
extern u8 gInputRecorderEntryFrames[];
extern struct InputRecording *gInputRecordingPtr;
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
void PlayerLifeRequestClear(void);
void PlayerClearOwnLifeRequests(void);
void PlayerLifeRequestPickStartState(void);
void PlayerLifeRequestLoadGfx(s32 a);
void PlayerLifeRequestOpenMenu(void);
void PlayerLifeRequestOpenGiverList(void);
void PlayerLifeRequestStartAsking(void);
void PlayerLifeRequestTakeLife(void);
void PlayerLifeRequestFinish(void);
void PlayerLifeRequestStartFail(void);
void PlayerLifeRequestStartNoGiver(void);
void PlayerLifeRequestShowGameOver(void);
void PlayerLifeRequestMoveMenuCursor(void);
void PlayerLifeRequestSelectChoice(void);
void PlayerLifeRequestMoveListCursor(void);
void PlayerLifeRequestSelectGiver(void);
void PlayerLifeRequestReceiveCheckPress(void);
void PlayerLifeRequestFailCheckPress(void);
void PlayerLifeRequestNoGiverCheckPress(void);
s32 PlayerLifeRequestCountGivers(void);
void PlayerLifeRequestRefreshGiverList(s32 a, s32 b);
void PlayerLifeRequestCheckGiven(void);
void PlayerLifeRequestCheckTimeout(void);
void PlayerLifeRequestCheckGiverLives(void);
void PlayerLifeRequestInit(void);
void PlayerLifeRequestUpdate(void);
void PlayerLifeRequestEnterState(void);
void PlayerLifeRequestChoose(void);
void PlayerLifeRequestChooseUpdate(void);
void PlayerLifeRequestPickGiver(void);
void PlayerLifeRequestPickGiverUpdate(void);
void PlayerLifeRequestWait(void);
void PlayerLifeRequestWaitUpdate(void);
void PlayerLifeRequestReceive(void);
void PlayerLifeRequestReceiveUpdate(void);
void PlayerLifeRequestFail(void);
void PlayerLifeRequestFailUpdate(void);
void PlayerLifeRequestNoGiver(void);
void PlayerLifeRequestNoGiverUpdate(void);
void PlayerLifeRequestGameOver(void);
void PlayerLifeRequestGameOverUpdate(void);
void PlayerLifeRequestDrawMenu(s32 a);
void PlayerLifeRequestDrawListTitle(void);
void PlayerLifeRequestDrawGiverList(void);
void PlayerLifeRequestDrawGiverIcon(s32 a, s32 b);
void PlayerLifeRequestDrawLives(s32 a, s32 b, s32 c);
void PlayerLifeRequestDrawListCursor(s32 a);
void PlayerLifeRequestDrawAsking(s32 a);
void PlayerLifeRequestDrawBorrowed(s32 a);
void PlayerLifeRequestDrawCannotBorrow(void);
void PlayerLifeRequestDrawGotNothing(void);
void PlayerLifeRequestDrawGameOver(void);

#endif /* GUARD_SAVE_H */
