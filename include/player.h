#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include "gba/types.h"

/* player.h: the RAM cells and ROM tables of the player: animation bank and
   collision registry (M05), breakable blocks and the player task (M09), the
   action machine and its objects (M10-M14).  One declaration per symbol, with
   the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct BreakingBlock;

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to CanBreakBlock; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 offsetX;
    /*0x03*/ s8 offsetY;
    /*0x04*/ s8 (*boxes)[4];
};

/* 16-byte per-slot record, 3 slots per player, at gUnk_02007E90.
   unk00/unk04 are 16.16 (x,y) offsets whose high halves are read directly,
   unk08 is the 16.16 y-delta, unk0C a down-counter, unk0D a frame id. */
struct M04Spark
{
    /*0x00*/ s32 offsetX;
    /*0x04*/ s32 offsetY;
    /*0x08*/ s32 velX;
    /*0x0C*/ u8 frameTimer;
    /*0x0D*/ u8 frame;
    /*0x0E*/ u16 unk0E;
};

struct M11Buf { u8 unk00[4]; u8 unk04[4]; };

struct PlayerBodyBox { u32 w[5]; };

/* M11's per-player records */
struct PlayerHitBoxSet { u8 unk00; u8 unk01; u8 offsetX; u8 offsetY; u8 *boxes; };

/* gUnk_0873B510[]: a palette fade, src/dst palettes and the blend step */
struct BurningPaletteFade
{
    /*0x00*/ u16 *src;
    /*0x04*/ u16 *dst;
    /*0x08*/ s32 rate;
};

struct LifeRequests
{
    /*0x00*/ s32 timeout;
    /*0x04*/ u8 requests[4];
    /*0x08*/ u8 gameOver[4];
};

/* one step of a player's knock-back script: {dx, dy, flags} with
   flags & 15 = frames to hold, & 64 = mirror dx with the facing,
   & 128 = sound; a zero flags byte ends the script */
struct OffsetScriptRow
{
    /*0x00*/ u16 offsetX;
    /*0x02*/ u16 offsetY;
    /*0x04*/ u8 flags;
    /*0x05*/ u8 filler5[3];
};

/* EWRAM */
extern u16 gEndingLocalPlayer;
extern u8 gBlockCursorPlayer; /*   the player that hit it */
extern u8 gBlockCursorShake; /*   hit-box id bit 11 */
extern s16 gBrokenBlockY[]; /*   y (pixels) */
extern struct PlayerHitBoxSet gPlayerHitBoxSets[];
extern u8 gUnk_020055C4;
extern struct LifeRequests gLifeRequests;
extern u16 gBlockCursorIndex; /*   map index */
extern struct PlayerBodyBox gPlayerBodyBoxes[];
extern u16 gBlockCursorAttack; /*   the block kind (hit-box id low byte) */
extern s16 gBlockCursorTile; /*   the metatile's collision byte */
extern u8 gUnk_020061E0;
extern struct M11Buf gPlayerHitBoxLists[];
extern u16 gBlockCursorX; /* the block CanBreakBlock accepted: x */
extern struct M04Spark gUnk_02007E90[][3];
extern u16 gUnk_02007F60[];
extern struct BreakingBlock gBlockAnimScratchRecord;
extern u16 gBlockCursorY; /*   y */
extern u16 gPlayerBubbleTimers[];
extern u16 gUnk_0200B060[];
extern u32 gUnk_02020000[]; /* decompression buffer */

/* IWRAM */
extern u16 gObjPaletteBank1[];
extern u8 gCreditsDemoSet;

/* ROM */
extern u8 gUnk_080D07C8[];
extern u32 gUnk_080D2148[];
extern u16 gPlayerPalettes[][16]; /* per-player palettes (M03 spelling) */
extern u32 gUnk_080DC728[];
extern u8 gUnk_080DCA28[];
extern u32 gUnk_080DCC28[];
extern u32 gUnk_080DCC48[];
extern u8 gUnk_080DCC68[];
extern u32 gUnk_081AC358[];
extern u8 gUnk_081AC378[];
extern u8 gUnk_081BBD70[];
extern u8 gUnk_081BC050[];
extern u8 gUnk_081BE45C[];
extern u8 gUnk_081BE6BC[]; /* per-player palettes, 128 bytes each */
extern u8 gUnk_081BFE38[];
extern u8 gUnk_081CC328[];
extern u8 gUnk_081CF260[];
extern u8 gUnk_081D5B04[];
extern u8 gUnk_081DCDFC[];
extern u8 gUnk_081E1D0C[];
extern u8 gUnk_081E43B4[];
extern u8 gUnk_081EFD60[];
extern u8 gUnk_081F1AE0[];
extern u16 gUnk_081F59F0[];
extern u8 gUnk_081F6CEC[];
extern u8 gUnk_08200D08[];
extern u16 gUnk_08203098[];
extern u16 gUnk_082030B8[];
extern u8 gUnk_082036D8[];
extern u8 gUnk_08204B78[];
extern u8 gUnk_08204B98[];
extern u8 gUnk_082181F0[];
extern u16 gUnk_08226254[];
extern u32 gNightmarePowerOrbGfx[];
extern u32 gUnk_085E24D8[];
extern u32 gUnk_085E26E8[];
extern u16 gUnk_085E2920[];
extern u16 gUnk_085E2A20[];
extern u16 gUnk_085E2B20[];
extern s16 gUnk_08732150[][16];
extern s16 gUnk_08732190[];
extern s16 gUnk_087321A6[];
extern s16 gUnk_087321B2[];
extern u32 gUnk_087321C0[];
extern u32 gUnk_087321EC[];
extern u16 gUnk_0873A458[];
extern u16 *gUnk_0873A47C[]; /* animation script per block kind */
extern s8 gUnk_0873A494[];
extern s8 gUnk_0873A5D4[];
extern u16 gUnk_0873A6D4[][3];
extern u16 gUnk_0873A6EC[][3][3];
extern s8 gUnk_0873A734[][2];
extern void (*gPlayerActions[])(void);
extern void (*gPlayerActionHandlers[])(void);
extern s16 gUnk_0873A924[][16];
extern u32 gUnk_0873A964[];
extern struct OffsetScriptRow *gUnk_0873A994[];
extern u16 gUnk_0873AEBC[][2];
extern u16 gUnk_0873AF0C[][2];
extern s8 gUnk_0873AF20[][2];
extern u8 gUnk_0873AF30[][2];
extern u8 gUnk_0873AF3A[][2];
extern u8 gUnk_0873AF42[];
extern u16 gUnk_0873AF58[][2];
extern u32 gUnk_0873AF6C[];
extern u32 gPlayerMotionXPresets[];
extern u32 gPlayerMotionYPresets[];
extern void (*gMetaKnightActions[])(void);
extern void (*gMetaKnightActionHandlers[])(void);
extern struct BurningPaletteFade gUnk_0873B510[];
extern u16 gUnk_0873B534[][32];
extern u8 gUnk_0873B634[];
extern s16 gUnk_0873B654[];
extern u8 gUnk_0873B65E[];
extern void (*gPlayerBallVariants[])(void); /* enter 49's sub-actions [9], indexed by Task.variant */
extern void (*gPlayerBallVariantUpdates[])(void); /* handler 46's per-frame sub-handlers [9] */
extern void (*gPlayerStarRodFlightVariants[])(void); /* enter 58's sub-actions [4] */
extern void (*gPlayerStarRodFlightVariantUpdates[])(void); /* handler 55's per-frame sub-handlers [4] */
extern u16 gUnk_0873B6CC[][2];
extern u8 gUnk_0873B6DC[];
extern u16 gUnk_0873B6E8[][6];
extern u16 gUnk_0873B724[][4];
extern void (*gPlayerObjectVariants[])(void); /* task type #6's 13 variants, indexed by Task.unk18 >> 24 */
extern u16 gUnk_0873B7B0[][2];
extern u16 gUnk_0873B7C0[][2][6];
extern u8 gUnk_0873B808[][6][5];
extern u16 gUnk_0873B862[][2];
extern u8 gUnk_0873B872[][8];
extern u8 gUnk_0873B88A[][5];
extern u16 gUnk_0873B8C6[][2][8];
extern u32 gPlayerDefaultBodyBox[]; /* stored to PlayerState.bodyBox as (u32)gPlayerDefaultBodyBox */
extern u32 gUnk_0873BD14[];
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873BD3C[];
extern u32 gUnk_0873BD50[];
extern u32 gUnk_0873BD64[]; /* collider row passed to RegisterCollider (4th arg) */
extern u32 gUnk_0873BD78[];
extern u32 gUnk_0873BD8C[];
extern u32 gUnk_0873BDA0[];
extern u32 gUnk_0873BDB4[];
extern u32 gUnk_0873BDD4[];
extern u32 gUnk_0873BDE8[];
extern u32 gUnk_0873BDFC[];
extern u32 gUnk_0873BE10[];
extern u32 gUnk_0873BE24[];
extern u32 gUnk_0873BE38[];
extern u32 gUnk_0873BE4C[];
extern u32 gUnk_0873BE60[];
extern u32 gUnk_0873BE74[];
extern u32 gUnk_0873BE88[];
extern u8 gUnk_0873BE9C[];
extern u8 gUnk_0873BEB0[];
extern u8 gUnk_0873BEC4[];
extern u32 gUnk_0873BED8[]; /* collider row passed to RegisterCollider (4th arg) */
extern u8 gUnk_0873BEEC[];
extern u32 gUnk_0873BF00[];
extern u32 gUnk_0873BF14[];
extern u32 gUnk_0873BF28[];
extern u32 gUnk_0873BF3C[];
extern u32 gUnk_0873BF64[];
extern u32 gUnk_0873BF84[];
extern u32 gUnk_0873BF98[];
extern u32 gUnk_0873BFD8[];
extern u32 gUnk_0873C060[];
extern u32 gUnk_0873C074[];
extern u32 gUnk_0873C0C0[];
extern u32 gUnk_0873C128[];
extern u32 gUnk_0873C1B0[];
extern u32 gUnk_0873C1C4[];
extern u32 gUnk_0873C1EC[];
extern u32 gUnk_0873C214[]; /* collider row passed to RegisterCollider (4th arg) */
extern u32 gUnk_0873C228[];
extern u32 gUnk_0873C264[];
extern u32 gUnk_0873C278[];
extern u32 gUnk_0873C28C[];
extern u32 gUnk_0873C2A0[];
extern u32 gUnk_0873C2C8[];
extern u32 gUnk_0873C2DC[];
extern u32 gUnk_0873C304[];
extern u32 gUnk_0873C318[];
extern u32 gUnk_0873C358[];
extern u32 gUnk_0873C36C[];
extern u32 gUnk_0873CA54[];
extern u32 gUnk_0873CA68[];
extern u32 gUnk_0873CA7C[];
extern u32 gUnk_0873CA90[];
extern u32 gUnk_0873CAA4[];
extern u32 gPlayerDefaultTerrainBox[];
extern u32 gUnk_0873CB24[];
extern u32 gUnk_0873CB2C[];
extern u32 gUnk_0873CB34[];
extern u32 gUnk_0873CB3C[];
extern s8 gUnk_0873CB44[]; /* collision box passed to sub_0802205c / TerrainCollideBoxAlongVelocity */
extern s8 gUnk_0873CB4C[];
extern s8 gUnk_0873CB54[];
extern s8 gUnk_0873CB5C[];
extern s8 gUnk_0873CB64[];
extern s8 gUnk_0873CB6C[];
extern u32 gUnk_0873CB84[];
extern u32 gUnk_0873CB94[];
extern u32 gUnk_0873CBA4[];
extern u32 gUnk_0873CBAC[];
extern u32 gUnk_0873CBB4[];
extern u32 gUnk_0873CBDC[];
extern u32 gUnk_0873CBEC[];
extern u32 gUnk_0873CBFC[];
extern u32 gUnk_0873CC0C[];
extern u32 gUnk_0873CC1C[];
extern u32 gUnk_0873CC2C[];
extern u32 gUnk_0873CC3C[];
extern u32 gUnk_0873CC44[];
extern struct HitBoxSet gUnk_0873CC54;
extern u32 gUnk_0873CC64[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern u32 gUnk_0873CC74[];
extern u32 gUnk_0873CC84[];
extern u32 gUnk_0873CCA4[];
extern u32 gUnk_0873CCAC[];
extern u32 gUnk_0873CCB4[];
extern u32 gUnk_0873CCFC[];
extern u32 gUnk_0873CD04[];
extern u32 gUnk_0873CD44[];
extern u32 gUnk_0873CDAC[];
extern u32 gUnk_0873CDB4[];
extern u32 gUnk_0873CDBC[];
extern u32 gUnk_0873CDF4[];
extern u32 gUnk_0873CDFC[];
extern u32 gUnk_0873CE64[];
extern u32 gUnk_0873CEEC[];
extern u32 gUnk_0873CEF4[];
extern u32 gUnk_0873CF1C[];
extern u32 gUnk_0873CF4C[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern u32 gUnk_0873CF5C[];
extern u32 gUnk_0873CF6C[];
extern u32 gUnk_0873CF7C[];
extern u32 gUnk_0873CF94[];
extern u32 gUnk_0873CF9C[];
extern u32 gUnk_0873D03C[];
extern u32 gUnk_0873D044[];
extern u32 gUnk_0873D04C[];
extern u8 gAbilityBButtonActions[];
extern u16 gUnk_0873D0F8[][5];
extern s16 gUnk_0873D206[];
extern s16 gUnk_0873D210[];
extern s16 gUnk_0873D2E0[];
extern u16 gUnk_0873D2E8[];
extern u16 gUnk_0873D31C[];
extern u16 gUnk_0873D350[];
extern s16 gUnk_0873D3B8[][2];
extern u16 gUnk_0873D4BC[][5];
extern s16 gUnk_0873D5C0[];
extern s16 gUnk_0873D5CA[][2];
extern u16 gPlayerDoorAnims[][7];
extern u16 gUnk_0873D79E[];
extern s16 gUnk_0873D7E4[][3];
extern s16 gUnk_0873D880[];
extern u16 gUnk_0873D8B4[];
extern u16 gUnk_0873D908[];
extern u32 gUnk_0873D986[];
extern s16 gUnk_0873D9DA[4][4];
extern u16 gUnk_0873D9FA[][2];
extern u16 gUnk_0873DA62[][2];
extern s16 gUnk_0873DACA[][2];
extern u16 gUnk_0873DADE[][2];
extern u16 gUnk_0873DB0A[];
extern s16 gUnk_0873DB34[];
extern u16 gUnk_0873DB44[][2];
extern u32 gUnk_0874C478[];
extern u32 gUnk_0874C4E4[];
extern u32 gUnk_0874C650[];
extern u32 gUnk_0874C7CC[];
extern u32 gUnk_0874CE90[];
extern u32 gPlayerFrames[];
extern u32 gUnk_08751990[];
extern u32 gUnk_087519CC[];
extern u32 gUnk_087519E8[];
extern u32 gUnk_08751A28[];
extern u32 gUnk_08751A98[];
extern u32 gUnk_08751AF8[];
extern u32 gUnk_08751B40[];
extern u32 gUnk_08751BB0[];
extern u32 gUnk_08751BF4[];
extern u32 gUnk_08751CA4[];
extern u32 gUnk_08751E5C[];
extern u32 gUnk_0875204C[];
extern u32 gUnk_087520A8[];
extern u32 gUnk_08755068[];
extern u32 gUnk_08755378[];
extern u32 gUnk_087553DC[];
extern u32 gUnk_087553FC[];
extern u32 gUnk_08755440[];
extern u32 gUnk_0875546C[];
extern u32 gUnk_08755484[];
extern u8 gUnk_08757368[];


/* Functions (defined in the files named above each group). */

/* src/player_17668.c */
void CutsceneFountainKirby(void);
void CutsceneFountainKirbyUpdate(void);
void CutsceneFountainKingDedede(void);

/* src/player_18b84.c */
void CutsceneFountainKingDededeUpdate(void);
void CutsceneFountainActorScript53(void);
void CutsceneFountainActorScript54(void);

/* src/player_18e14.c */
void CutsceneFountainActorScript52(void);

/* src/player_19000.c */
void CutsceneFountainNightmarePowerOrb(void);
void CutsceneFountainNightmarePowerOrbUpdate(void);
void CutsceneFountainStarRod(void);
void CutsceneFountainJet(void);
void CutsceneFountainActorScript59(void);
void CutsceneFountainActorScript60(void);
void CutsceneFountainActorScript61(void);
void sub_08019ecc(void);

/* src/player_19eec.c */
void CutsceneFountainActorScript62(void);

/* src/player_1a07c.c */
void CutsceneFountainActorScript56(void);
void CutsceneFountainKirbyDraw(void);
void sub_0801a310(void);

/* src/player_1a3e4.c */
void sub_0801a3e4(void);

/* src/player_1a76c.c */
void sub_0801a76c(s32 i);
void ClearColliderLists(void);

/* src/block_30804.c */
u16 TaskBreakBlocksAtNoPlayer(struct HitBoxSet *p, s32 x, s32 y);
u16 BreakBlocksInHitBoxes(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 BreakFirstBlockInHitBox(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 BreakTopBlockRow(struct HitBoxSet *p, s32 x, s32 y, s32 dir);
s32 IsUnbrokenBlockAt(u32 x, u32 y);
s32 BreakBlockAt(u32 x, u32 y);
s32 CanBreakBlock(s32 x, s32 y, s32 id, s32 e);
s32 sub_08031310(s32 x, s32 y);
s32 BreakBlockAtCursor(void);

/* src/block_318b4.c */
void UpdateBlockAnims(void);
void FreeBlockAnimAndBlock(struct BreakingBlock *b);
void FreeBlockAnim(struct BreakingBlock *b);
void BlockAnimWriteAndDrawColumn(struct BreakingBlock *b, s32 n);
void BlockAnimWriteColumn(struct BreakingBlock *b, s32 n);
void BlockAnimDrawColumn(struct BreakingBlock *b, s32 n);
void BlockAnimWriteMetatile(struct BreakingBlock *b);
void BlockAnimDrawTiles(struct BreakingBlock *b);
void BlockAnimBreakNeighbors(struct BreakingBlock *b);
void UpdateBlockAnimsWithEdges(void);
void BlockAnimWriteMetatileWrapped(struct BreakingBlock *b);
void BlockAnimDrawWithEdges(struct BreakingBlock *b);
s32 CanBreakBg1Block(s32 x, s32 y);
s16 BreakBg1BlockAtCursor(void);
void UpdateBg1BlockAnims(void);
void FreeBg1BlockAnimAndBlock(struct BreakingBlock *b);
void Bg1BlockAnimWriteMetatile(struct BreakingBlock *b);
void Bg1BlockAnimDrawTiles(struct BreakingBlock *b);
void Bg1BlockAnimBreakNeighbors(struct BreakingBlock *b);

/* src/player_32688.c */
void Task_Player(void);
void PlayerStartRequestedAction(void);
void PlayerUpdate(void);
void sub_0803332c(void);
void sub_08033414(void);

/* src/player_337f4.c */
void sub_080337f4(void);
void sub_080337f8(void);
void sub_080337fc(void);
void sub_08033800(void);
void sub_08033804(void);
void sub_08033808(void);
void PlayerActionStand(void);
void PlayerActionStandUpdate(void);
void PlayerActionWalk(void);
void PlayerActionWalkUpdate(void);

/* src/player_343c0.c */
void PlayerActionRun(void);
void PlayerActionRunUpdate(void);
void PlayerActionSkid(void);
void PlayerActionSkidUpdate(void);
void PlayerActionJump(void);
void PlayerActionJumpUpdate(void);
void PlayerActionReleaseJump(void);
void PlayerActionReleaseJumpUpdate(void);
void sub_08034f70(void);

/* src/player_34f8c.c */
void PlayerActionFall(void);
void PlayerActionFallUpdate(void);
void PlayerActionHighFall(void);
void PlayerActionHighFallUpdate(void);
void PlayerActionFloat(void);

/* src/player_36280.c */
void PlayerActionFloatUpdate(void);
void PlayerActionDuck(void);
void PlayerActionDuckUpdate(void);
void PlayerActionSlide(void);
void PlayerActionSlideUpdate(void);

/* src/player_36c94.c */
void PlayerActionLadder(void);
void PlayerActionLadderUpdate(void);
void PlayerActionInhale(void);
void PlayerActionInhaleUpdate(void);
void PlayerActionSpit(void);
void PlayerActionSpitUpdate(void);
void PlayerActionSwallow(void);
void PlayerActionSwallowUpdate(void);

/* src/player_37ed8.c */
void PlayerActionHurt(void);
void PlayerActionHurtUpdate(void);

/* src/player_3919c.c */
void PlayerActionDie(void);
void sub_080396a4(void);
void PlayerActionEnterDoor(void);

/* src/player_39c24.c */
void PlayerActionExitDoor(void);
void PlayerActionExitDoorUpdate(void);

/* src/player_3aa64.c */
void PlayerActionSwim(void);
void PlayerActionSwimUpdate(void);
void PlayerActionStandInWater(void);
void PlayerActionStandInWaterUpdate(void);
void PlayerActionWalkInWater(void);
void PlayerActionWalkInWaterUpdate(void);
void PlayerActionSwallowInWater(void);
void PlayerActionSwallowInWaterUpdate(void);
void PlayerActionSpitInWater(void);
void PlayerActionSpitInWaterUpdate(void);
void PlayerActionWaterShot(void);
void PlayerActionWaterShotUpdate(void);
void PlayerActionRecoil(void);
void PlayerActionRecoilUpdate(void);

/* src/player_3bde8.c */
void PlayerActionShareItem(void);
void PlayerActionShareItemUpdate(void);
void sub_0803c9b4(s32 a);
void sub_0803cbd8(void);
void sub_0803ccd8(s32 a);

/* src/stage_3cd60.c */
void PlayerPlayBump(void);
void sub_0803ce98(void);
s32 sub_0803d010(void);
void CreatePlayer(s32 a0);
void InitPlayerState(s32 a0);
void InitPlayerStateKeepInvincibility(s32 a0);
void InitPlayerStateKeepMouth(s32 a0);
void PlayerAccelerateAxis(s32 a0, s32 a1, s32 a2);
void PlayerMove(void);
s32 PlayerLoadFrameTilesAndPalette(s32 a0);
void sub_0803d710(void);
void sub_0803d7c4(void);
void sub_0803d824(void);
s32 sub_0803d870(void);
void sub_0803db74(void);
void sub_0803ddc0(void);
void sub_0803dfc8(void);
void PlayerStopAxes(s32 axes);
void sub_0803e080(void);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void sub_0803e28c(s32 a0);
void PlayerUpdateInvulnerability(void);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
void FreezeOtherTasks(s32 a0);
void PlayerUpdateFlip(void);
s32 PlayerFaceHeldDirection(void);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
s32 sub_0803e55c(void);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void PlayerStartOffsetScript(s32 a0);
void sub_0803e68c(s32 a0);
s32 PlayerUpdateInvincibility(void);
void PlayerEndInvincibility(void);
void sub_0803e8ec(void);
s32 sub_0803eaf8(s32 a0);
void FreePlayerEffectsAndObjects(s8 a0);
void sub_0803f6e0(void);
u16 sub_0803f7e0(u16 a0);
void sub_0803f834(u16 a0, void *src);
void PlayerTurnToHeldDirection(void);
s32 PlayerGetHeldDirection(void);
void PlayerCheckBump(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerCheckDie(void);
void sub_0803fb54(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerGetFacingSlope(s32 a0);
s32 PlayerCheckSkid(void);
s32 PlayerCheckJump(void);
s32 PlayerCheckFallOrWater(void);
s32 PlayerCheckDuckOrSwallow(void);
s32 PlayerCheckLadder(void);
s32 PlayerCheckFloat(void);
s32 PlayerCheckAirFloat(void);
s32 PlayerCheckBButton(void);
s32 PlayerCheckEnterWater(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 PlayerCheckStartSwim(void);
s32 PlayerRequestLocomotion(void);
s32 sub_080404e4(void);
s32 PlayerCheckShareItem(void);
void PlayerRequestStandOrFall(void);
void LatchPlayerKeys(void);
void sub_08040808(s32 a0);
void sub_08040858(s32 a0);
void PlayerGiveInvincibleCandy(s32 a0);
void PlayerStartItemShare(s32 a0, u8 a1);
s32 sub_080408e4(void);
void sub_08040934(s32 a0);
void sub_080409b8(s32 a0);
void sub_08040a44(s16 p0, s16 p1);

/* src/stage_40b40.c */
void PlayerSetMotionXPreset(s32 a0, s32 a1);

/* src/stage_413a4.c */
void PlayerSetMotionYPreset(s32 a0);
void MetaKnightActionStand(void);
void MetaKnightActionWalk(void);
void MetaKnightActionWalkUpdate(void);
void MetaKnightActionRun(void);
void MetaKnightActionRunUpdate(void);
void MetaKnightActionSkid(void);
void MetaKnightActionJump(void);
void MetaKnightActionJumpUpdate(void);
void MetaKnightActionReleaseJump(void);
void MetaKnightActionFall(void);
void MetaKnightActionFallUpdate(void);
void MetaKnightActionFloat(void);
void MetaKnightActionFloatUpdate(void);
void MetaKnightActionDuck(void);
void MetaKnightActionSlide(void);
void MetaKnightActionSlideUpdate(void);
void MetaKnightActionLadder(void);
void MetaKnightActionLadderUpdate(void);
void MetaKnightActionHurt(void);
void MetaKnightActionHurtUpdate(void);
void MetaKnightActionDie(void);
void sub_08042c50(void);
void MetaKnightActionRecoil(void);
void MetaKnightActionRecoilUpdate(void);
void MetaKnightActionEnterDoor(void);
void MetaKnightActionExitDoor(void);
void MetaKnightActionSwim(void);

/* src/stage_4335c.c */
void MetaKnightActionSwimUpdate(void);

/* src/stage_43654.c */
void MetaKnightActionStandInWater(void);
void MetaKnightActionWalkInWater(void);
void MetaKnightActionSlash(void);
void MetaKnightActionSlashUpdate(void);
void MetaKnightActionDashSlash(void);
void MetaKnightActionDashSlashUpdate(void);
void MetaKnightActionUpwardSlash(void);
void MetaKnightActionUpwardSlashUpdate(void);
void MetaKnightActionDownThrust(void);
void MetaKnightActionDownThrustUpdate(void);
void PlayerActionFire(void);
void PlayerActionFireUpdate(void);
void PlayerActionSpark(void);

/* src/player_449c8.c */
void PlayerActionSparkUpdate(void);
void PlayerActionCutter(void);
void PlayerActionCutterUpdate(void);

/* src/player_44d04.c */
void PlayerActionSword(void);
void PlayerActionSwordUpdate(void);

/* src/player_455c8.c */
void PlayerActionBurning(void);
void PlayerActionBurningUpdate(void);
void PlayerActionLaser(void);
void PlayerActionLaserUpdate(void);

/* src/player_45d34.c */
void PlayerActionMike(void);
void PlayerActionMikeUpdate(void);

/* src/player_46330.c */
void PlayerActionWheel(void);
void PlayerActionWheelUpdate(void);

/* src/player_46c00.c */
void PlayerActionHammer(void);
void PlayerActionHammerUpdate(void);

/* src/player_474e8.c */
void PlayerActionParasol(void);
void PlayerActionParasolUpdate(void);
void PlayerActionSleep(void);
void PlayerActionSleepUpdate(void);
void PlayerActionNeedle(void);
void PlayerActionNeedleUpdate(void);

/* src/player_47fe8.c */
void PlayerActionGetAbility(void);
void PlayerActionGetAbilityUpdate(void);

/* src/player_49738.c */
void LoadAbilityTiles(void);
void PlayerLoadSparkTiles(void);

/* src/player_49b48.c */
void PlayerActionIce(void);
void PlayerActionIceUpdate(void);
void PlayerActionFreeze(void);
void PlayerActionFreezeUpdate(void);

/* src/player_49f98.c */
void PlayerActionHiJump(void);
void PlayerActionHiJumpUpdate(void);

/* src/player_4a54c.c */
void PlayerActionBeam(void);
void PlayerActionBeamUpdate(void);
void PlayerActionStone(void);
void PlayerActionStoneUpdate(void);

/* src/player_4ab70.c */
void PlayerActionTornado(void);
void PlayerActionTornadoUpdate(void);
void PlayerActionCrash(void);
void PlayerActionCrashUpdate(void);

/* src/player_4b5b4.c */
void PlayerActionLight(void);
void PlayerActionLightUpdate(void);

/* src/player_4b858.c */
void PlayerActionBackdropHold(void);
void PlayerActionBackdropHoldUpdate(void);

/* src/player_4c64c.c */
void PlayerActionThrowHold(void);
void PlayerActionThrowHoldUpdate(void);

/* src/player_4cc7c.c */
void PlayerActionUFO(void);
void PlayerActionUFOUpdate(void);

/* src/player_4dc08.c */
void PlayerActionBackdrop(void);
void PlayerActionBackdropUpdate(void);
void PlayerActionThrow(void);

/* src/sub_0804e3a0.c */
void PlayerActionThrowUpdate(void);

/* src/player_4e5a4.c */
void PlayerActionBall(void);
void PlayerActionBallEnterVariant(void);
void PlayerActionBallUpdate(void);

/* src/player_4e78c.c */
void PlayerBallTransform(void);
void PlayerBallTransformUpdate(void);
void PlayerBallStand(void);
void PlayerBallStandUpdate(void);
void PlayerBallRoll(void);
void PlayerBallRollUpdate(void);
void PlayerBallSkid(void);
void PlayerBallSkidUpdate(void);

/* src/player_4ee08.c */
void PlayerBallJump(void);
void PlayerBallJumpUpdate(void);
void PlayerBallBounce(void);
void PlayerBallBounceUpdate(void);
void PlayerBallFall(void);
void PlayerBallFallUpdate(void);
void PlayerBallLand(void);
void PlayerBallLandUpdate(void);
void PlayerBallRevert(void);
void PlayerBallRevertUpdate(void);

/* src/player_4f614.c */
s32 PlayerBallPlayBump(void);
s32 PlayerBallGetRollDelay(void);
void PlayerBallStepRoll(void);
s32 PlayerBallCheckVariant(s32 a);
s32 PlayerBallCheckLanding(s32 a0);

/* src/player_4f948.c */
void PlayerActionStarRod(void);
void PlayerActionStarRodUpdate(void);
void PlayerActionStarRodJump(void);
void PlayerActionStarRodJumpUpdate(void);
void PlayerActionStarRodFlight(void);
void PlayerActionStarRodFlightEnterVariant(void);
void PlayerActionStarRodFlightUpdate(void);

/* src/player_4ffdc.c */
void PlayerStarRodFlightIntro(void);
void PlayerStarRodFlightIntroUpdate(void);
void PlayerStarRodFlightFly(void);
void PlayerStarRodFlightFlyUpdate(void);
void PlayerStarRodFlightShoot(void);
void PlayerStarRodFlightShootUpdate(void);
void PlayerStarRodFlightHurt(void);
void PlayerStarRodFlightHurtUpdate(void);
s32 PlayerStarRodFlightCheckShoot(void);
void PlayerStarRodFlightSteer(void);

/* src/plobj_507bc.c */
void Task_PlayerObject(void);
void sub_08050814(void);
void sub_0805091c(void);

/* src/plobj_509ec.c */
void PlayerObjectAirPuff(void);
void PlayerObjectAirPuffUpdate(void);
void PlayerObjectSpitStar(void);
void PlayerObjectSpitStarUpdate(void);
void sub_08050f80(void);
void PlayerObjectSpitMultiStar(void);
void PlayerObjectSpitMultiStarUpdate(void);
void sub_080513d4(void);

/* src/plobj_514f8.c */
void PlayerObjectWaterShot(void);
void PlayerObjectWaterShotUpdate(void);
void PlayerObjectFireBreath(void);
void PlayerObjectFireBreathUpdate(void);
void PlayerObjectCutterBlade(void);
void PlayerObjectCutterBladeUpdate(void);
void PlayerObjectLaserBeam(void);
void PlayerObjectLaserBeamUpdate(void);

/* src/plobj_5239c.c */
void PlayerObjectIceBreath(void);
void PlayerObjectIceBreathUpdate(void);
s32 PlayerObjectBeamOrb(void);
s32 sub_08052b08(void);
void PlayerObjectLightOrb(void);

/* src/plobj_52f6c.c */
void PlayerObjectUFOShot(void);
void PlayerObjectUFOShotUpdate(void);
void PlayerObjectUFOShotLateUpdate(void);
void PlayerObjectStarRodShot(void);
void PlayerObjectStarRodShotUpdate(void);
void PlayerObjectStarRodFlightShot(void);
void PlayerObjectStarRodFlightShotUpdate(void);

#endif /* GUARD_PLAYER_H */
