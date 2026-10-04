#ifndef GUARD_CAMERA_H
#define GUARD_CAMERA_H

#include "gba/types.h"
#include "room.h"

/* camera.h: the RAM cells and ROM tables of the camera, the BG map streaming,
   the map events and the stage objects #221-#236 (M08).  One declaration per
   symbol, with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct BgAnim
{
    /*0x00*/ u16 cmdIndex;
    /*0x02*/ s16 waitFrames;
    /*0x04*/ struct BgAnimCmd *script;
    /*0x08*/ u16 fadeFrame;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 *fadeSrc;
    /*0x10*/ u16 *fadeDst;
    /*0x14*/ u16 fadeColorIndex;
    /*0x16*/ u16 fadeColorCount;
    /*0x18*/ u32 fadeRate;
};

struct BgAnimCmd
{
    /*0x00*/ u16 op;
    /*0x02*/ u16 arg;
    /*0x04*/ void *ptr;
};

/* A BG animation palette fade (op 1's pointer; src/data/bg_anim_scripts.c):
   BgAnimStartPaletteFade copies it into a gBgAnims[] slot and
   BgAnimStepPaletteFade blends colorCount colours from src to dst into the
   palette buffer at colour colorIndex, at rate/256 per frame. */
struct BgAnimPaletteFade
{
    /*0x00*/ u16 *src;
    /*0x04*/ u16 *dst;
    /*0x08*/ u16 colorIndex;
    /*0x0A*/ u16 colorCount;
    /*0x0C*/ u32 rate;
};

struct Unk03004B00
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u32 unk4;
    /*0x08*/ u32 unk8;
};

/* EWRAM */
extern u8 gBlockAnimHookId;
extern u8 gUnk_02005E10[];
extern struct BgAnim gBgAnims[];
extern u8 gWarpStarStationDoorRevealed;
extern struct BreakingBlock gBg1BreakingBlocks[];
extern s32 gHBlankScrollBaseX;

/* IWRAM */
/* Actor-vs-player hit test cells (M17's src/actor_673ec.c widths). */
extern s16 gViewRect[]; /* camera rectangle: left, right, top, bottom */
extern struct Unk03004B00 gUnk_03004B00;

/* ROM */
extern u16 gUnk_080D71A0[];
extern u8 gUnk_085A0638[];
extern u8 gUnk_085A0C38[][32];
extern u8 gUnk_085A12F8[];
extern u8 gUnk_085A1BF8[];
extern u8 gUnk_085A24F8[];
extern u8 gUnk_085A2DF8[][32];
extern u16 gWarpStarStationDoorCells[][2];
extern s8 gUnk_08732428[][3];
extern s8 gUnk_087324A6[][3];
extern u16 *gScreenShakePatterns[];
extern void (*gMapEventVariants[])(void);
extern u8 gUnk_087328BC[];
extern s16 gMuseumAbilitySignX[][2];
extern void (*gStageEffectStates[])(void);
extern u32 gUnk_0874CD54[];
extern u32 gUnk_0874CD68[];
extern u32 gUnk_0874CDE0[];
extern u32 gSmokeRingFrames[];
extern u32 gArenaDoorSignFrames[];
extern u32 gBossDoorSignFrames[];
extern u32 gUnk_087558C4[];
extern u32 gDoorMarkerFrames[];
extern u32 gUnk_087558DC[];
extern u32 gDoorOpeningFrames[];
extern u32 gStageClearFlagFrames[];
extern u32 gQuickDrawDoorSignFrames[];
extern u32 gBombRallyDoorSignFrames[];
extern u32 gAirGrindDoorSignFrames[];
extern u32 gMuseumDoorSignFrames[];
extern u32 gStageDoorSignFrames[];
extern u32 gWarpStarStationDoorSignFrames[];
extern u32 gWarpStarStationDoorSparkleFrames[];
extern u32 gLevelDoorSignFrames[];
extern u32 gWarpStarStationNumberFrames[];
extern u32 gWarpStarStationLevelSignFrames[];
extern u32 gMuseumAbilitySignFrames[];
extern struct BgAnimCmd *const *const gRoomBgAnimScripts[];
/* The BG animation scripts (src/data/bg_anim_scripts.c), in address order. */
extern struct BgAnimCmd gRoomBgAnimSet1Script0[];
extern struct BgAnimCmd gRoomBgAnimSet1Script1[];
extern struct BgAnimCmd gRoomBgAnimSet1Script2[];
extern struct BgAnimCmd gRoomBgAnimSet1Script3[];
extern struct BgAnimCmd gRoomBgAnimSet1Script4[];
extern struct BgAnimCmd gRoomBgAnimSet1Script5[];
extern struct BgAnimCmd gRoomBgAnimSet1Script6[];
extern struct BgAnimCmd gRoomBgAnimSet1Script7[];
extern struct BgAnimCmd gRoomBgAnimSet1Script8[];
extern struct BgAnimCmd gRoomBgAnimSet1Script9[];
extern struct BgAnimCmd gRoomBgAnimSet2Script0[];
extern struct BgAnimCmd gRoomBgAnimSet3Script0[];
extern struct BgAnimCmd gRoomBgAnimSet4Script0[];
extern struct BgAnimCmd gRoomBgAnimSet5Script2[];
extern struct BgAnimCmd gRoomBgAnimSet6Script0[];
extern struct BgAnimCmd gRoomBgAnimSet5Script0[];
extern struct BgAnimCmd gRoomBgAnimSet5Script1[];
extern struct BgAnimCmd gRoomBgAnimSet6Script1[];
extern struct BgAnimCmd gRoomBgAnimSet6Script2[];
extern struct BgAnimCmd gRoomBgAnimSet7Script0[];
extern struct BgAnimCmd gRoomBgAnimSet8Script0[];
extern struct BgAnimCmd gRoomBgAnimSet10Script0[];
extern struct BgAnimCmd gRoomBgAnimSet10Script1[];
extern struct BgAnimCmd gRoomBgAnimSet10Script2[];
extern struct BgAnimCmd gRoomBgAnimSet12Script0[];
extern struct BgAnimCmd gRoomBgAnimSet9Script0[];
extern struct BgAnimCmd gRoomBgAnimSet9Script1[];
extern struct BgAnimCmd gRoomBgAnimSet9Script2[];
extern struct BgAnimCmd gRoomBgAnimSet11Script1[];
extern struct BgAnimCmd gRoomBgAnimSet11Script0[];


/* Functions (defined in the files named above each group). */

struct BgAnimTileFrame;

/* src/camera_296a0.c */
void StreamBg2MapLooping(void);
void StreamBg123Maps(void);
void StreamBg23Maps(void);
void StreamBg23Rows(void);
void CameraWriteScrollParallax(void);
void CameraWriteScrollHBlank(void);
void CameraWriteScrollBg23(void);
void CameraWriteScrollBg123(void);
void sub_08029b30(void);
void SpawnRoomObjectsScrolledIn(void);

/* src/camera_29c74.c */
void UpdatePlayerGroupCenter(void);
void SetCameraBoundsToGroup(void);
void SetCameraBoundsToGroupInScrollLock(void);
void UpdatePlayerCameras(void);
void UpdatePlayerCamerasInScrollLock(void);
void SetPlayerGroupCenterFromTasks(void);
void SetPlayerBoundsFromCamera(void);
void SetPlayerBoundsFromCameraInScrollLock(void);
void SetViewRectToPlayers(void);
void SetViewRectToPlayersInScrollLock(void);
void LockPlayersPastScrollLine(void);
void SpawnRoomObjectsInRect(s32 x0, s32 x1, s32 y0, s32 y1);

/* src/bgmap_2a9cc.c */
void DrawBg2View(s32 px, s32 py);
void DrawBg2Row(s32 x0, s32 x1, s32 y);
void DrawBg2Column(s32 x, s32 y0, s32 y1);
void DrawBg2EdgeColumn(s32 x);
void DrawBg3View(s32 px, s32 py);
void DrawBg3Row(s32 x0, s32 x1, s32 y);
void DrawBg3Column(s32 x, s32 y0, s32 y1);
void DrawBg123View(s32 px, s32 py);
void DrawBg123Row(s32 x0, s32 x1, s32 y);
void DrawBg123Column(s32 x, s32 y0, s32 y1);
void DrawBg23View(s32 px, s32 py);
void DrawBg23Row(s32 x0, s32 x1, s32 y);
void DrawBg23Column(s32 x, s32 y0, s32 y1);
void DrawBg23FullRows(s32 py);
void DrawBg23FullRow(s32 y);
void DrawBg1Tile(s32 x, s32 y);
void DrawBg2Tile(s32 x, s32 y);
void DrawBg3Tile(s32 x, s32 y);
void DrawBg2ViewLooping(s32 px);
void DrawBg2EdgeTile(s32 x, s32 y);
void RestoreMapColumn(s32 x);
void RestoreMapCell(s32 x, s32 y);

/* src/bgmap_2b2f0.c */
void SetBlockAnimClipRect(void);
void SetBg1BlockAnimClipRect(void);
void SetBlockAnimClipRectWithEdges(void);
void SetBg23ScreenSize(u16 a);
void SetBg3ScreenSize(u16 a);

/* src/camera_2b4bc.c */
void CameraFollowFocus(void);
void CameraSlideToScrollLock(void);
void CameraFollowScrollLocked(void);
void CameraSlideFromScrollLock(void);

/* src/camera_2c42c.c */
void CameraHoldAnchor(void);
void HubCameraFollowFocus(void);
void HubCameraFollowFocusPlayer(void);
void HubCameraGlideToPlayers(void);
void CameraSnapBoundsToAnchor(void);
void CameraSnapPlayersToAnchor(void);
void CameraSnapToFocus(void);
void StopScreenShake(void);
void UpdateScreenShake(void);
void StartScrollLock(s32 x0, s32 x1, s32 y0, s32 y1);

/* src/camera_2d01c.c */
void CameraLeaveScrollLock(void);
void CameraStartHoldAnchor(void);
void sub_0802d0c4(void);
void LoadRoomBgAnims(void);
void UpdateBgAnims(void);
void BgAnimCopyTiles(struct BgAnimTileFrame *a);
void BgAnimStartPaletteFade(struct BgAnim *p, struct BgAnimPaletteFade *q);
void BgAnimStepPaletteFade(struct BgAnim *p);
void SetCollisionTile(u32 x, u32 y, u32 v);
void BgAnimStop(struct BgAnim *p);
s32 CreateMapEvent(s32 a);
void Task_MapEvent(void);

/* src/camtask_2d38c.c */
void MapEventMidBossFight(void);
s32 CreateMapEventBreakTwoBlocks(s32 x, s32 y);
void MapEventBreakTwoBlocks(void);
s32 CreateMapEventBreakThreeBlocks(s32 x, s32 y);
void MapEventBreakThreeBlocks(void);
void MapEventFadeToNextRoomPalettes(void);
void sub_0802d96c(void);
void sub_0802da8c(void);
void MapEventStageUnlockPan(void);
void MapEventBigSwitchUnlockPan(void);

/* src/obj_2eac8.c */
s32 CreateArenaDoorSign(s32 x, s32 y, s32 a);
void Task_ArenaDoorSign(void);
void ArenaDoorSignUpdate(void);
s32 CreateBossDoorSign(s32 x, s32 y, s32 a);
void Task_BossDoorSign(void);
void BossDoorSignUpdate(void);
s32 CreateDoorOpening(s32 x, s32 y, s32 a);
void Task_DoorOpening(void);
void DoorOpeningUpdate(void);
s32 CreateStageClearFlag(s32 x, s32 y, s32 a, s32 b);
void Task_StageClearFlag(void);
s32 CreateQuickDrawDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_QuickDrawDoorSign(void);
void QuickDrawDoorSignAnimate(void);
void QuickDrawDoorSignShowStill(void);
s32 CreateBombRallyDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_BombRallyDoorSign(void);
void BombRallyDoorSignAnimate(void);
void BombRallyDoorSignShowStill(void);
s32 CreateAirGrindDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_AirGrindDoorSign(void);
void AirGrindDoorSignAnimate(void);
void AirGrindDoorSignShowStill(void);
s32 CreateMuseumDoorSign(s32 x, s32 y, s32 a);
void Task_MuseumDoorSign(void);
s32 CreateStageDoorSign(s32 x, s32 y, s32 a, s32 b);
s32 CreateClearedStageDoorSign(s32 x, s32 y, s32 a, s32 b);
s32 CreateCompletedStageDoorSign(s32 x, s32 y, s32 a, s32 b);

/* src/obj_2f62c.c */
void Task_StageDoorSign(void);
void StageDoorSignBlinkSignAndDoor(void);
void StageDoorSignBlinkDoor(void);
void StageDoorSignShowStill(void);
void DoorSignDrawWithDoor(void);
s32 CreateWarpStarStationDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_WarpStarStationDoorSign(void);
void WarpStarStationDoorSignUpdate(void);
void sub_0802f93c(void);
s32 CreateWarpStarStationDoorSparkle(s32 x, s32 y, s32 a, s32 b);
void Task_WarpStarStationDoorSparkle(void);
void sub_0802fd98(void);
s32 CreateLevelDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_LevelDoorSign(void);
void DoorObjectDraw(void);
void SubGameDoorSignDrawUsed(void);
s32 CreateWarpStarStationNumber(s32 a, s32 x, s32 y);
void Task_WarpStarStationNumber(void);
s32 CreateWarpStarStationLevelSign(s32 a);
void Task_WarpStarStationLevelSign(void);
s32 CreateMuseumAbilitySigns(u8 a);
s32 CreateMuseumAbilitySign(u8 a, s32 b);
void Task_MuseumAbilitySign(void);
s32 CreateStageEffect(s32 a, s32 x, s32 y);

/* src/obj_30238.c */
void Task_StageEffect(void);
void sub_08030254(void);
void sub_080302cc(void);
void sub_08030404(void);
void sub_080304ec(void);
void sub_08030580(void);
void sub_08030604(void);

/* src/obj_306b4.c */
s32 QueueWorldSprite(u8 a, s32 b, u16 c, u16 d, s16 x, s16 y);
void ResetBlockAnims(void);
void ResumeBlockAnims(void);
void PauseBlockAnims(void);
void StartBlockAnims(void);
void StartBlockAnimsWithEdges(void);
void StartBg1BlockAnims(void);

#endif /* GUARD_CAMERA_H */
