#ifndef GUARD_CAMERA_H
#define GUARD_CAMERA_H

#include "gba/types.h"
#include "room.h"

/* camera.h: the RAM cells and ROM tables of the camera, the BG map streaming,
   the map events and the stage objects #221-#236 (M08).  One declaration per
   symbol, with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct Unk02007D70
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk02007D70Cmd *unk4;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u32 unk18;
};

struct Unk02007D70Cmd
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ void *unk4;
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
extern struct Unk02007D70 gBgAnims[];
extern u8 gUnk_02007FC4;
extern struct Unk020061F0 gBg1BreakingBlocks[];
extern s32 gUnk_02016C30;

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
extern u16 gUnk_087323C6[][2];
extern s8 gUnk_08732428[][3];
extern s8 gUnk_087324A6[][3];
extern u16 *gScreenShakePatterns[];
extern void (*gMapEventVariants[])(void);
extern u8 gUnk_087328BC[];
extern s16 gUnk_087328C0[][2];
extern void (*gUnk_087328D8[])(void);
extern u32 gUnk_0874CD54[];
extern u32 gUnk_0874CD68[];
extern u32 gUnk_0874CDE0[];
extern u32 gSmokeRingFrames[];
extern u32 gUnk_087558BC[];
extern u32 gUnk_087558C0[];
extern u32 gUnk_087558C4[];
extern u32 gUnk_087558D0[];
extern u32 gUnk_087558DC[];
extern u32 gUnk_087558E8[];
extern u32 gUnk_087558EC[];
extern u32 gUnk_087558FC[];
extern u32 gUnk_08755930[];
extern u32 gUnk_0875593C[];
extern u32 gUnk_08755944[];
extern u32 gUnk_08755948[];
extern u32 gUnk_08755978[];
extern u32 gUnk_0875597C[];
extern u32 gUnk_0875599C[];
extern u32 gUnk_087559A4[];
extern u32 gUnk_087559C0[];
extern u32 gUnk_087559DC[];
extern struct Unk02007D70Cmd **gRoomBgAnimScripts[];


/* Functions (defined in the files named above each group). */

struct Unk0802D25C;
struct Unk0802D278;

/* src/camera_296a0.c */
void sub_080296a0(void);
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
void sub_08029ef4(void);
void UpdatePlayerCameras(void);
void sub_0802a260(void);
void sub_0802a340(void);
void SetPlayerBoundsFromCamera(void);
void sub_0802a484(void);
void SetViewRectToPlayers(void);
void sub_0802a568(void);
void sub_0802a63c(void);
void SpawnRoomObjectsInRect(s32 x0, s32 x1, s32 y0, s32 y1);

/* src/bgmap_2a9cc.c */
void DrawBg2View(s32 px, s32 py);
void DrawBg2Row(s32 x0, s32 x1, s32 y);
void DrawBg2Column(s32 x, s32 y0, s32 y1);
void sub_0802aae8(s32 x);
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
void sub_0802b074(s32 px);
void sub_0802b168(s32 x, s32 y);
void sub_0802b25c(s32 x);
void sub_0802b29c(s32 x, s32 y);

/* src/bgmap_2b2f0.c */
void sub_0802b2f0(void);
void sub_0802b368(void);
void sub_0802b3e4(void);
void SetBg23ScreenSize(u16 a);
void SetBg3ScreenSize(u16 a);

/* src/camera_2b4bc.c */
void CameraFollowFocus(void);
void CameraSlideToScrollLock(void);
void CameraFollowScrollLocked(void);
void CameraSlideFromScrollLock(void);

/* src/camera_2c42c.c */
void CameraHoldAnchor(void);
void sub_0802c550(void);
void sub_0802c680(void);
void sub_0802c7f4(void);
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
void sub_0802d25c(struct Unk0802D25C *a);
void sub_0802d278(struct Unk02007D70 *p, struct Unk0802D278 *q);
void sub_0802d294(struct Unk02007D70 *p);
void sub_0802d2f0(u32 x, u32 y, u32 v);
void sub_0802d32c(struct Unk02007D70 *p);
s32 CreateMapEvent(s32 a);
void Task_MapEvent(void);

/* src/camtask_2d38c.c */
void sub_0802d38c(void);
s32 sub_0802d478(s32 x, s32 y);
void sub_0802d4bc(void);
s32 sub_0802d5b4(s32 x, s32 y);
void sub_0802d5f8(void);
void sub_0802d6cc(void);
void sub_0802d96c(void);
void sub_0802da8c(void);
void sub_0802dcb4(void);
void sub_0802e3ac(void);

/* src/obj_2eac8.c */
s32 sub_0802eac8(s32 x, s32 y, s32 a);
void sub_0802eb28(void);
void sub_0802eba4(void);
s32 sub_0802ec1c(s32 x, s32 y, s32 a);
void sub_0802ec7c(void);
void sub_0802ed20(void);
s32 CreateDoorOpening(s32 x, s32 y, s32 a);
void Task_DoorOpening(void);
void sub_0802ee88(void);
s32 CreateStageClearFlag(s32 x, s32 y, s32 a, s32 b);
void Task_StageClearFlag(void);
s32 CreateQuickDrawDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_QuickDrawDoorSign(void);
void sub_0802f110(void);
void sub_0802f1dc(void);
s32 CreateBombRallyDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_BombRallyDoorSign(void);
void sub_0802f2b0(void);
void sub_0802f2fc(void);
s32 CreateAirGrindDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_AirGrindDoorSign(void);
void sub_0802f3d0(void);
void sub_0802f400(void);
s32 sub_0802f420(s32 x, s32 y, s32 a);
void sub_0802f480(void);
s32 CreateStageDoorSign(s32 x, s32 y, s32 a, s32 b);
s32 CreateClearedStageDoorSign(s32 x, s32 y, s32 a, s32 b);
s32 CreateCompletedStageDoorSign(s32 x, s32 y, s32 a, s32 b);

/* src/obj_2f62c.c */
void Task_StageDoorSign(void);
void sub_0802f684(void);
void sub_0802f6c0(void);
void sub_0802f6f4(void);
void sub_0802f718(void);
s32 sub_0802f7dc(s32 x, s32 y, s32 a, s32 b);
void sub_0802f84c(void);
void sub_0802f8c8(void);
void sub_0802f93c(void);
s32 sub_0802fa3c(s32 x, s32 y, s32 a, s32 b);
void sub_0802faa8(void);
void sub_0802fd98(void);
s32 CreateLevelDoorSign(s32 x, s32 y, s32 a, s32 b);
void Task_LevelDoorSign(void);
void DoorObjectDraw(void);
void sub_0802ff70(void);
s32 sub_0802ffe8(s32 a, s32 x, s32 y);
void sub_08030034(void);
s32 sub_08030074(s32 a);
void sub_080300c0(void);
s32 sub_08030100(u8 a);
s32 sub_08030140(u8 a, s32 b);
void sub_080301a4(void);
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
void sub_080307b0(void);
void sub_080307cc(void);
void sub_080307e8(void);

#endif /* GUARD_CAMERA_H */
