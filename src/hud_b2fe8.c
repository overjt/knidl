#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u32 gUnk_0200000C[];
extern u32 gUnk_02000034[];
extern u32 gUnk_02004C90;
extern u32 gMaxHealth[];
extern s16 gPlayerHealth[];
extern u32 gUnk_02005590[];
extern u32 gUnk_020055D0[];
extern u32 gRoomObjectList[];
extern u8 gUnk_02005E10[];
extern u32 gUnk_02005F10[];
extern u32 gUnk_02006040[];
extern u32 gUnk_020060A0[];
extern u32 gUnk_02006130[];
extern s32 gUnk_02006190[];
extern u8 gUnk_020069F0;
extern u32 gUnk_02007BF0[];
extern s32 gUnk_02007D00[];
extern u32 gUnk_02007D40[];
extern s16 gPlayerLives[];
extern u16 gUnk_02007D60;
extern u32 gUnk_02007D64[];
extern s8 gUnk_02007FB8[];
extern u32 gUnk_02008014[];
extern u32 gUnk_02008020[];
extern u32 gUnk_0200AF0C[];
extern u8 gUnk_0200AFF8;
extern u8 gUnk_0200B030;
extern u32 gUnk_0200B04C[];
extern u32 gUnk_0200B078[];
extern u8 gUnk_0200D080;
struct Unk0200D120
{
    /*0x00*/ u8 filler00[0x05];
    /*0x05*/ u8 hitState;
    /*0x06*/ u8 filler06[0x1A];
    /*0x20*/ u16 savedTileWord;
    /*0x22*/ u8 filler22[0x26];
    /*0x48*/ s8 *attackBox;
    /*0x4C*/ u8 filler4C[0x24];
};
extern struct Unk0200D120 gUnk_0200D120[];
extern u32 gHBlankScrollTimer[];
extern u32 gHBlankScrollTable[];
extern u32 gHBlankScrollEffect[];
extern u32 gHBlankScrollDmaTable[];
extern u32 gUnk_02020000[];
extern u32 gFrameCallback;
extern vu16 gBg2Cnt;
extern u32 gUnk_03000B74;
extern u16 gPlayerHeldKeys[];
extern u32 gVBlankCallback;
extern u32 gHBlankDmaDest[];
extern u32 gHBlankDmaCnt[];
extern u8 gObjPalette[];
extern u32 gUnk_03001570[];
extern u32 gUnk_030015B0[];
extern u16 gFrameCount;
extern vu8 gBgMosaic;
extern vu16 gBg3Cnt;
extern u32 gHBlankDmaSrc[];
extern s16 gCameraAnchorY;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern struct PlayerState gPlayerStates[];
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern s16 gSpriteCameraX;
extern u8 gActivePlayerCount;
extern u16 gLocalPlayer;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern u16 gPlayerCount;
extern u8 gUnk_030023B0;
extern s32 gUnk_030023B4;
extern u32 gUnk_030023C8[];
extern s32 gUnk_030023D4;
extern u16 gGameState;
extern s16 gSpriteCameraY;
extern u32 gCurSaveSlot[];
extern u32 gStageIndex[];
extern u32 gUnk_03002448;
extern s16 gUnk_0300244C;
extern u32 gExtraMode[];
extern u32 gRoomIndex[];
extern u32 gUnk_030027A8[];
extern vs16 gTaskSlotTypes[];
extern u8 gTerrainResult[];
extern u32 gTerrainBoundsClamp[];
extern s16 gRoomBounds[];
extern u32 gScrollLock[];
extern u32 gUnk_080A7358[];
extern u32 gUnk_080A81B8[];
extern u32 gUnk_080AD86C[];
extern u32 gUnk_080B0464[];
extern u32 gUnk_080B1694[];
extern u32 gUnk_080B2684[];
extern u32 gUnk_080B2820[];
extern u32 gUnk_080B4FDC[];
extern u32 gUnk_080B5184[];
extern u32 gUnk_080B5378[];
extern u32 gUnk_080B5E3C[];
extern u32 gUnk_082DFFA8[];
extern u32 gUnk_082F427C[];
extern u32 gUnk_082F65D4[];
extern u32 gUnk_082FB190[];
extern u32 gUnk_082FB210[];
extern u32 gUnk_082FB230[];
extern u32 gUnk_082FD438[];
extern u32 gUnk_082FE0E4[];
extern u32 gUnk_082FE104[];
extern u32 gUnk_082FEFF4[];
extern u32 gUnk_082FFDF0[];
extern u16 gUnk_08334480[];
extern u32 gUnk_083344C0[];
extern u32 gUnk_087324DA[];
extern u32 gPlayerDefaultTerrainBox[];
extern u32 gUnk_0873EEA0[];
extern u32 gUnk_0873EF48[];
extern u32 *gMidBossGfx[];
extern u32 gMetaKnightsGfx[];
extern u32 *gBossGfx[];
extern u32 gUnk_0873F180[];
extern s16 gUnk_0873FF98[];
extern struct AnimCmd gUnk_087484A4[];
extern u32 gMrShineAndMrBrightVariants[];
extern u16 gUnk_087484E4[];
extern s32 gUnk_087484EC[];
extern u32 gUnk_0874850C[];
extern s32 gUnk_0874851C[];
extern s32 gUnk_0874852C[];
extern struct AnimCmd gUnk_0874853C[];
extern struct AnimCmd gUnk_08748558[];
extern struct AnimCmd gUnk_08748574[];
extern struct AnimCmd gUnk_08748588[];
extern struct AnimCmd gUnk_0874859C[];
extern struct AnimCmd gUnk_087485B0[];
extern struct AnimCmd gUnk_087485C0[];
extern struct AnimCmd gUnk_087485D0[];
extern struct AnimCmd gUnk_087485DC[];
extern struct AnimCmd gUnk_087485E4[];
extern struct AnimCmd gUnk_087485EC[];
extern struct AnimCmd gUnk_087485FC[];
extern s32 gUnk_08748604[];
extern s32 gUnk_08748614[];
extern u32 gUnk_08748624[];
extern u32 gUnk_08748634[];
extern u32 gUnk_08748644[];
extern u32 gUnk_0874868C[];
extern u32 gUnk_087486D4[];
extern u32 gUnk_0874871C[];
extern struct ActorDef gUnk_087487BC[];
extern u32 gUnk_0874883C[];
extern u32 gUnk_08748858[];
extern u32 gUnk_08748874[];
extern u32 gUnk_08748890[];
extern u32 gUnk_087488AC[];
extern u32 gUnk_087488C8[];
extern u32 gUnk_087488E4[];
extern u32 gUnk_0874898C[];
extern u32 gUnk_087489B4[];
extern u32 gUnk_087489B8[];
extern u32 gUnk_087489BC[];
extern u32 gUnk_087489C0[];
extern u32 gUnk_087489D4[];
extern u32 gUnk_087489D8[];
extern u32 gUnk_087489DC[];
extern u32 gUnk_087489E0[];
extern u32 gUnk_087489F0[];
extern u32 gUnk_087489F8[];
extern struct AnimCmd gUnk_08748A00[];
extern struct AnimCmd gUnk_08748A14[];
extern u32 gUnk_08748A28[];
extern u32 gUnk_08748A30[];
extern s16 gUnk_08748A38[];
extern s32 gUnk_08748A40[];
extern u32 gUnk_08748A54[];
extern u32 gUnk_08748A5C[];
extern u32 gUnk_08748A64[];
extern u32 gUnk_08748A6C[];
extern u32 gUnk_08748A74[];
extern u32 gUnk_08748A80[];
extern u32 gUnk_08748A94[];
extern struct ActorDef gUnk_08748B34[];
extern struct ActorDef gUnk_08748B60[];
extern struct ActorDef gUnk_08748B8C[];
extern struct ActorDef gUnk_08748BD4[];
extern u32 gUnk_08748C0C[];
extern u32 gUnk_08748C28[];
extern u32 gUnk_08748C44[];
extern u32 gUnk_08748D04[];
extern u8 gUnk_08748D28[];
extern u32 gUnk_08748D38[];
extern u32 gUnk_08748D44[];
extern u8 gUnk_08748D4C[];
extern u32 gUnk_08748D50[];
extern s32 gUnk_08748D60[];
extern u8 gUnk_08748D6C[];
extern u8 gUnk_08748D70[];
extern u8 gUnk_08748D74[];
extern u8 gUnk_08748D78[];
extern u32 gUnk_08748D80[];
extern u8 gUnk_08748D98[];
extern u16 gUnk_08748DA8[][8];
extern u16 gUnk_08748DC8[];
extern u16 gUnk_08748DE8[][16];
extern u16 gUnk_08748E28[][4];
extern u16 gUnk_08748E48[];
extern u16 gUnk_08748E68[];
extern u16 gUnk_08748E88[];
extern u16 gUnk_08748E98[];
extern u16 gUnk_08748EA8[];
extern u8 gUnk_08748EAC[];
extern u32 gMetaKnightVariants[];
extern u32 gUnk_08748EBC[];
extern u32 gUnk_08748F1C[];
extern u32 gUnk_08748F7C[];
extern u32 gUnk_08748F84[];
extern u32 gUnk_08748F8C[];
extern u16 gUnk_08749014[];
extern u16 gUnk_08749058[];
extern s16 gUnk_0874909C[];
extern u8 gUnk_087490A4[];
extern s16 gUnk_087490A8[];
extern u8 gUnk_087490B0[];
extern u8 gUnk_087490B2[];
extern u8 gUnk_087490B4[][8];
extern u8 gUnk_087490DC[];
extern u8 gUnk_087490E4[];
extern s32 gUnk_087490E8[];
extern s32 gUnk_08749100[];
extern u32 gKrackoVariants[];
extern u32 gUnk_08749158[];
extern u32 gUnk_08749160[];
extern u32 gUnk_08749168[];
extern u32 gUnk_08749184[];
extern u16 gUnk_087491A0[];
extern u32 gUnk_087491A8[];
extern u16 gUnk_087491BC[];
extern s16 *gUnk_087491D4[];
extern u16 gUnk_087491E4[];
extern s16 gUnk_087491FC[];
extern s16 gUnk_08749214[];
extern u8 gUnk_0874921C[];
extern u8 gUnk_0874921E[];
extern u8 gUnk_08749220[];
extern u8 gUnk_08749224[];
extern u16 gUnk_08749230[][2];
extern s16 gUnk_08749244[];
extern u16 gUnk_08749252[];
extern s16 gUnk_08749260[];
extern u32 gUnk_08749270[];
extern u32 gUnk_08749284[];
extern u32 gUnk_08749298[];
extern u32 gUnk_087492AC[];
extern u32 gUnk_087492C0[];
extern u32 gUnk_087492D4[];
extern u32 gUnk_087492E8[];
extern u32 gUnk_0874930C[];
extern u32 gUnk_08749330[];
extern u32 gUnk_08749344[];
extern u32 gUnk_08749358[];
extern u32 gUnk_0874936C[];
extern s8 gUnk_08749380[];
extern s8 gUnk_087493A4[];
extern u32 gNightmareWizardVariants[];
extern u32 gUnk_087493F8[];
extern u32 gUnk_08749428[];
extern u32 gUnk_08749458[];
extern u32 gUnk_08749490[];
extern s8 gUnk_087494C8[];
extern u32 gUnk_087495EC[];
extern u32 gUnk_08749704[];
extern u32 gUnk_08749720[];
extern u32 gUnk_08749758[];
extern u32 gUnk_08749774[];
extern u32 gUnk_08749870[];
extern u32 gUnk_0874988C[];
extern u32 gUnk_08749AF8[];
extern u32 gUnk_08749B00[];
extern u32 gUnk_08749B30[];
extern u32 gUnk_08749B48[];
extern u32 gUnk_08749B60[];
extern u32 gUnk_08749B84[];
extern u32 gUnk_08749B8C[];
extern u32 gUnk_08749B98[];
extern u32 gUnk_08749BA4[];
extern u32 gUnk_08749BA8[];
extern u8 gUnk_08749BAC[];
extern s8 gUnk_08749BB1[];
extern u32 gUnk_08749BB8[];
extern u32 gUnk_08749BC4[];
extern u32 gUnk_08749BD0[];
extern u32 gUnk_08749BE4[];
extern u32 gUnk_08749BE8[];
extern u32 gUnk_08749CEC[];
extern u32 gUnk_08749D10[];
extern s8 gUnk_08749D1C[];
extern u8 gUnk_08749D2C[];
extern s8 gUnk_08749D38[];
extern u8 gUnk_08749D44[];
extern u32 gUnk_08749D4C[];
extern u32 gUnk_08749D70[];
extern u32 gUnk_08749D94[];
extern u32 gUnk_08749DB8[];
extern u32 gUnk_08749DDC[];
extern u8 gUnk_0874AAD0[];
extern s16 gUnk_0874AAD4[];
extern s16 gUnk_0874AADC[];
extern u16 gUnk_0874AAE4[];
extern u16 gUnk_0874AAEC[];
extern u8 gUnk_0874AAF4[];
extern u8 gUnk_0874AAF7[];
extern u16 gUnk_0874AB26[];
extern u32 gUnk_0874AB50[];
extern u32 gUnk_0874ABA0[];
extern u32 gUnk_0874AC24[];
extern u32 gUnk_0874ACA8[];
extern s16 gUnk_0874ACB4[];
extern u32 gUnk_0874ACBC[];
extern u8 gUnk_0874ACD4[];
extern u8 gUnk_0874ACE0[];
extern u8 gUnk_0874ACE4[];
extern u16 gUnk_0874ACEE[];
extern s16 gUnk_0874ACFA[];
extern u16 gUnk_0874ACFE[];
extern s32 gUnk_0874AD0C[];
extern s32 gUnk_0874AD18[];
extern u8 gUnk_0874AD30[];
extern u32 gUnk_0874AD34[];
extern u16 gUnk_0874AD44[];
extern u32 gUnk_0874AD74[];
extern u32 gUnk_0874ADA8[];
extern u32 gUnk_0874AEAC[];
extern u32 gUnk_0874AF90[];
extern u32 gUnk_0874B07C[];
extern u32 gUnk_0874B0A0[];
extern u32 gUnk_0874B0C4[];
extern u16 gUnk_0874B1A8[];
extern u32 gUnk_0874B1DC[];
extern u32 gUnk_0874B20C[];
extern u32 gUnk_0874B220[];
extern u32 gUnk_0874B234[];
extern u32 gUnk_0874B240[];
extern u32 gUnk_0874B254[];
extern u32 gUnk_0874B2C8[];
extern u32 gUnk_0874B3A8[];
extern u32 gUnk_0874B3FC[];
extern u32 gUnk_0874B450[];
extern u32 gUnk_0874B488[];
extern u32 gUnk_0874B4A4[];
extern u32 gUnk_0874B4EC[];
extern u32 gUnk_0874B504[];
extern u32 gUnk_0874B538[];
extern u32 gUnk_0874B540[];
extern u32 gUnk_0874B560[];
extern u32 gUnk_0874B568[];
extern u32 gUnk_0874B574[];
extern u32 gUnk_0874B590[];
extern u32 gUnk_0874B5A4[];
extern u32 gUnk_0874B5E4[];
extern u32 gUnk_0874B5FC[];
extern u32 gUnk_0874B614[];
extern u32 gUnk_0874B63E[];
extern u32 gUnk_0874B6BC[];
extern u32 gUnk_0874B6D4[];
extern u32 gUnk_0874B77C[];
extern u32 gUnk_0874B824[];
extern u32 gUnk_0874B82A[];
extern u32 gUnk_0874B82E[];
extern u32 gUnk_0874B831[];
extern u32 gUnk_0874B835[];
extern u32 gUnk_0874B838[];
extern u32 gUnk_0874B83B[];
extern u32 gUnk_0874B840[];
extern u32 gUnk_0874B86C[];
extern u32 gUnk_0874B898[];
extern u32 gUnk_0874B8C4[];
extern u32 gUnk_0874B8C8[];
extern u32 gUnk_0874B8F8[];
extern u32 gUnk_0874B928[];
extern u32 gUnk_0874B96C[];
extern u32 gUnk_0874B998[];
extern u32 gUnk_0874B9C4[];
extern u32 gUnk_0874B9F0[];
extern u32 gUnk_0874BA1C[];
extern u32 gUnk_0874BA48[];
extern u32 gUnk_0874BA74[];
extern u32 gUnk_0874BAA0[];
extern u32 gUnk_0874BACC[];
extern u32 gUnk_0874BB98[];
extern u32 gUnk_0874BFD4[];
extern u32 gUnk_0874C108[];
extern u32 gUnk_0874C110[];
extern u32 gWhispyWoodsVariants[];
extern u32 gUnk_0874C130[];
extern u32 gUnk_0874C140[];
extern u32 gUnk_0874C150[];
extern u32 gUnk_0874C154[];
extern u32 gUnk_0874C210[];
extern u32 gUnk_0874C21C[];
extern u32 gUnk_0874C220[];
extern u32 gUnk_0874C230[];
extern u32 gUnk_0874C240[];
extern u32 gUnk_0874C246[];
extern u32 gUnk_0874C24C[];
extern u32 gUnk_0874C254[];
extern u32 gUnk_0874C258[];
extern u32 gUnk_0874C25C[];
extern s8 gUnk_0874C260[];
extern s8 gUnk_0874C2A6[];
extern s8 gUnk_0874C2EC[];
extern s8 gUnk_0874C332[];
extern u32 gUnk_0874C44C[];
extern u32 gUnk_0874C500[];
extern u32 gUnk_0874C568[];
extern u32 gUnk_0874CAEC[];
extern u32 gUnk_0874CCF4[];
extern u32 gUnk_0874CD0C[];
extern u32 gUnk_0874CD24[];
extern u32 gUnk_0874CD3C[];
extern u32 gUnk_0874CE68[];
extern u32 gUnk_08753990[];
extern u32 gPaintRollerFrames[];
extern u32 gUnk_08753A8C[];
extern u32 gUnk_08753AAC[];
extern u32 gUnk_08753AC8[];
extern u32 gUnk_08753AE8[];
extern u32 gUnk_08753B04[];
extern u32 gUnk_08753B20[];
extern u32 gUnk_08753B48[];
extern u32 gUnk_08753B68[];
extern u32 gUnk_08753B88[];
extern u32 gUnk_08753BA4[];
extern u32 gMetaKnightFrames[];
extern u32 gUnk_08753DA0[];
extern u32 gHeavyMoleFrames[];
extern u32 gUnk_08753E8C[];
extern u32 gUnk_087540AC[];
extern u32 gUnk_087540EC[];
extern u32 gMrBrightFrames[];
extern u32 gUnk_08754180[];
extern u32 gUnk_087541B0[];
extern u32 gUnk_087541E0[];
extern u32 gMrShineFrames[];
extern u32 gUnk_08754260[];
extern u32 gUnk_08754280[];
extern u32 gUnk_08754290[];
extern u32 gUnk_087542A8[];
extern u32 gWhispyWoodsFrames[];
extern u32 gUnk_08754308[];
extern u32 gUnk_08754358[];
extern u32 gUnk_087543A0[];
extern u32 gKrackoFrames[];
extern u32 gUnk_08754418[];
extern u32 gUnk_08754448[];
extern u32 gUnk_0875447C[];
extern u32 gNightmarePowerOrbFrames[];
extern u32 gUnk_08754504[];
extern u32 gUnk_08754560[];
extern u32 gUnk_08754568[];
extern u32 gNightmareWizardFrames[];
extern u32 gUnk_087546D0[];
extern u32 gUnk_087546F8[];
extern u32 gUnk_08754708[];
extern u32 gUnk_08754718[];
extern u32 gUnk_08754738[];
extern u32 gUnk_08754780[];
extern u32 gUnk_0875607C[];
extern u32 gUnk_08756084[];
extern u32 gUnk_087560A0[];
extern u32 gUnk_087560AC[];
extern u32 gUnk_087560B8[];
extern u32 gUnk_087560C0[];
extern u32 gUnk_087560D0[];
extern u32 gUnk_087560D8[];
extern u32 gUnk_087560E0[];
extern u32 gUnk_087560EC[];
extern u32 gUnk_08756150[];
extern u32 gUnk_08756158[];
extern u32 gUnk_08756160[];
extern u32 gUnk_0875616C[];
extern u32 gUnk_08756178[];
extern u32 gUnk_08756184[];
extern u32 gHBlankScrollEffects[];

/* External functions */
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void ResetFadeAndBlend(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlayBgm(s32 songId);
extern void sub_08003184(void);
extern s32 PlaySfx(s32 id);
extern void StopBgm(void);
extern void StopSfxOnPlayer(s32 player, s32 songId);
extern void FadeOutBgm(s32 speed);
extern void SetBgmVolume(u16 volume);
extern void TaskSetSkipMask(u8 val, s32 idx);
extern void TaskFree(s32 id);
extern void TaskIntegrateMotion(void);
extern void TaskMove(void);
extern void TaskMoveRelativeToParent(void);
extern u32 TaskIsOnScreen(void);
extern void TaskDrawWorld(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetMotionX(s32 a, s32 b, s32 c);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern s32 AddPlayerHealth(s32 a, s32 b);
extern void SetPlayerAbilityNoHud();
extern void HudShowHpBar(void);
extern void HudStartHpBar();
extern void HudSetTaskHpBar();
extern void HudRemoveHpBar(void);
extern void sub_0800a698(void);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void TaskInitWaterFlags(void);
extern u8 ClampTaskToRoom(struct Task *t);
extern u32 sub_0802294c();
extern void ExitClearedStage();
extern void sub_08025a30();
extern void sub_08025acc();
extern void sub_08025b5c();
extern void RequestScreenShake(u32 a);
extern void sub_080275cc();
extern void StartScrollLock();
extern s32 CreateMapEvent();
extern void sub_0802ffe8();
extern void TaskBreakBlocksNoPlayer();
extern void sub_08030db8();
extern void sub_0803e68c();
extern void sub_08040858();
extern void sub_08040894();
extern void sub_0805ddb0();
extern void sub_0805deac();
extern void sub_0805e110();
extern s32 sub_08063698(u32 type, s32 start);
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorLoadDefSlot(u32 i, struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAttackBoxSlot(u32 i, u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
extern void sub_08063a14(u32 i, u32 v);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetDyTo(u32 i);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskGetPosSlot(u32 i);
extern void TaskGetNearestPlayerPos(void);
extern s32 TaskGetFacingToward(u32 i);
extern void TaskFaceNearestPlayer(void);
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
extern void ActorDestroySlot(s32 i);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskToggleFacingAndReverseX(void);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void ActorStopAnim(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern s32 ActorTickAnim(s32 n);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleTo(u32 i, s32 prec);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u8 TaskGetYDirBitTo(u32 i);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern u8 TaskGetXDirBitTo(u32 i);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern s32 TaskFindNearestPlayerInScreenXBand(u16 lo, u16 hi);
extern s32 TaskFindNearestPlayerInScreenYBand(u16 lo, u16 hi);
extern void TaskGetScreenPosSlot(u32 i);
extern s32 TaskGetNearestPlayerScreenPos(void);
extern void TaskGetScreenPos(void);
extern void TaskFaceLikeParent(void);
extern s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio);
extern void sub_08064bcc(void);
extern s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_0806505c(u8 p3, u8 p4, u32 x, u32 y, u16 prio);
extern u8 ActorIsInView(void);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_080653ec(void);
extern void sub_08065438(void);
extern void sub_08065640(void);
extern void ActorMove(void);
extern void TaskMoveRelativeToView(void);
extern void ResetBgPaletteBlend(void);
extern void EndBgPaletteBlend(u32 v);
extern void StartBgPaletteBlend(u32 a, u32 b);
extern void sub_08065dbc(u32 slot, u32 sub);
extern void sub_08065dd0(u32 slot, u32 i);
extern void sub_08065dfc(u32 slot);
extern u8 TaskHasSameSerial(u32 i);
extern s16 ActorComputeHealth(void);
extern u16 sub_08066088(u32 mode);
extern void sub_08066144(void);
extern void sub_0806619c(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4);
extern void sub_0806621c(void);
extern s32 sub_08066394(void);
extern void sub_080664e0(struct AnimCmd *p);
extern void sub_08066544(void);
extern void sub_08066564(void);
extern void sub_08066580(void);
extern u16 sub_0806660c(u16 a);
extern u16 sub_08066630(u16 a);
extern void sub_080666f8(struct AnimCmd *p);
extern u32 sub_08066718(void);
extern void ActorLoadPalette(void *src, u32 size, u8 force);
extern u8 sub_08066a6c(void);
extern void sub_08066f50(s32 x, s32 y);
extern void sub_08066fc0(u8 p3, s16 x, s16 y);
extern void sub_0806704c(void);
extern void sub_080670ac(u16 a);
extern void sub_080670d4(void);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern void sub_08068920(s32 i, u8 c);
extern void sub_08068950(s16 x, s16 y, s16 d);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_0806956c(void);
extern u32 sub_080695bc(void);
extern u32 sub_080696a0(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);
extern u32 sub_08069b84(void);
extern u32 sub_08069bbc(void);
extern u32 ActorReactToDefeat(void);
extern void ActorDie(void);
extern void sub_0806b05c(void);
extern void sub_0806b098(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0806d730(void);
extern s32 sub_0806e6f8(s16 x, s16 y);
extern s32 sub_0806e808(s16 x, s16 y);
extern s32 sub_0806e9b4(u8 a, s16 x, s16 y);
extern void sub_080a1570(void);
extern void sub_080b7cb4();

/* Module functions */
void sub_080a1590();
void sub_080a15f0();
s32 sub_080a1618();
void sub_080a1624();
void sub_080a1668();
void sub_080a1704();
void sub_080a1740();
void sub_080a1790();
void sub_080a180c();
void sub_080a1864();
void sub_080a18d4();
void sub_080a1980();
void Task_MrShineAndMrBright();
void sub_080a19ec();
void sub_080a1ad0();
void sub_080a1b94();
void sub_080a1bd8(u16 a, u16 b);
void sub_080a1c1c();
void sub_080a1c90();
void sub_080a1d2c();
void sub_080a1d84();
s32 sub_080a1dbc(s32 a);
void sub_080a1dd4();
s32 sub_080a1df8();
s32 sub_080a1e4c();
s32 sub_080a1ec4();
void sub_080a1f90();
s32 sub_080a1fc8();
void sub_080a2020();
void sub_080a2030();
void sub_080a2090();
void sub_080a2164();
void sub_080a21a0();
s32 sub_080a2224();
void sub_080a2274();
s32 sub_080a22b0(void);
void sub_080a22d4();
s32 sub_080a2390();
void sub_080a23c0();
void sub_080a2400();
void sub_080a2448();
void sub_080a2488();
s32 sub_080a24f8(void);
void sub_080a2590();
void sub_080a25c4();
void sub_080a2608();
void sub_080a263c();
void sub_080a268c();
void sub_080a2754();
void sub_080a27b8();
void sub_080a2814();
void sub_080a28d0();
void sub_080a291c();
void sub_080a2954();
void sub_080a2994();
void sub_080a29cc();
void sub_080a2a00();
void sub_080a2a24();
void sub_080a2a44();
void sub_080a2a94();
void sub_080a2af4();
void sub_080a2b14();
void sub_080a2b2c();
void sub_080a2bc4();
void sub_080a2c90();
void sub_080a2ca8();
void sub_080a2ce8();
void sub_080a2d38();
s32 sub_080a2de0();
void sub_080a2e88(u8 a);
void sub_080a2edc();
void sub_080a2f38();
void sub_080a3000();
void sub_080a301c();
void sub_080a306c();
void sub_080a30d0();
void sub_080a3114();
void sub_080a314c();
void sub_080a3168();
void sub_080a3184();
void sub_080a31a4();
void sub_080a31d0();
void sub_080a31f0();
void sub_080a31f4();
void sub_080a3238();
void sub_080a3250();
void sub_080a3268();
void sub_080a3280();
void sub_080a3298();
void sub_080a32c4();
void sub_080a332c();
void sub_080a3348();
void sub_080a33a4();
void sub_080a33dc();
void sub_080a3424();
void sub_080a346c();
void sub_080a348c();
void sub_080a349c();
void sub_080a34b8();
void sub_080a34c8();
void sub_080a34f0();
void sub_080a352c();
void sub_080a3548();
void sub_080a3588();
void sub_080a35a8();
void sub_080a35b8();
void sub_080a35d8();
void sub_080a35e8();
void sub_080a361c();
void sub_080a3628();
void sub_080a368c();
void sub_080a36c8();
void sub_080a3768();
void sub_080a37ac();
void sub_080a3840();
void sub_080a387c();
void sub_080a38ec();
void sub_080a3954();
void sub_080a3a10();
void sub_080a3a40();
void sub_080a3b10();
void sub_080a3b4c();
void sub_080a3b78();
void sub_080a3b7c();
void sub_080a3ba0();
void sub_080a3bc0();
void sub_080a3c08();
void sub_080a3c28();
void sub_080a3c54();
void sub_080a3cbc();
void sub_080a3cd8();
void sub_080a3d3c();
void sub_080a3d84();
void sub_080a3dd0();
void sub_080a3e10();
void sub_080a3e3c();
void sub_080a3e60();
void sub_080a3e7c();
void sub_080a3ea0();
void sub_080a3edc();
void sub_080a3f24();
void sub_080a3f54();
void sub_080a3fb8();
void sub_080a3fd4();
void sub_080a3ff8();
void sub_080a4018();
void sub_080a403c();
void sub_080a406c();
void sub_080a408c();
void sub_080a412c();
void sub_080a4180();
void sub_080a4200();
void sub_080a4260();
void sub_080a42f8();
void sub_080a4350();
void sub_080a43d8();
void sub_080a4430();
void sub_080a44c4();
void sub_080a4508();
void sub_080a45bc();
void sub_080a4604();
void sub_080a4630();
void sub_080a4634();
void sub_080a4658();
void sub_080a4678();
void sub_080a46c0();
void sub_080a46e0();
void sub_080a4708();
void sub_080a472c();
void sub_080a4808();
void sub_080a4814();
void sub_080a488c();
void sub_080a498c();
void sub_080a49cc();
void sub_080a4a1c();
void sub_080a4a60();
void sub_080a4aa8();
void sub_080a4ac4();
void sub_080a4b1c();
void sub_080a4b68();
void sub_080a4ba8();
void sub_080a4bdc();
void sub_080a4c20();
void sub_080a4c3c();
void sub_080a4c80();
void sub_080a4c84();
void sub_080a4cc4();
void sub_080a4d00();
void sub_080a4d6c();
void sub_080a4d88();
void sub_080a4dd8();
void sub_080a4df4();
void sub_080a4e10();
void sub_080a4e14();
void sub_080a4e9c();
void sub_080a4ee0();
void sub_080a4f24();
void sub_080a4f40();
void sub_080a5008();
void sub_080a5020();
void sub_080a503c();
void sub_080a5040();
void sub_080a5084();
void sub_080a50f0();
void sub_080a510c();
void sub_080a5164();
void sub_080a5168();
void sub_080a5184();
void sub_080a5188();
void sub_080a51dc();
void sub_080a5220();
void sub_080a523c();
void sub_080a528c();
void sub_080a52c8();
void sub_080a5304();
void sub_080a5320();
void sub_080a5324();
void sub_080a5388();
void sub_080a53a8();
void sub_080a5484();
void sub_080a54a4();
void sub_080a54e4();
void sub_080a5524();
void sub_080a556c();
void sub_080a55ac();
void Task_MetaKnight();
void sub_080a5644();
void sub_080a5694();
void sub_080a56b0();
void sub_080a57d4();
void sub_080a5a78();
void sub_080a5aa0();
void sub_080a5af4();
void sub_080a5dd0();
void sub_080a5e30();
void sub_080a5e60();
void sub_080a5e9c();
void sub_080a5ecc();
void sub_080a5ef0();
void sub_080a5f20();
void sub_080a5f7c();
void sub_080a5fac();
void sub_080a6020();
void sub_080a60d8();
void sub_080a6130();
void sub_080a61b4();
void sub_080a6264();
void sub_080a6280();
void sub_080a6330();
void sub_080a634c();
void sub_080a638c();
void sub_080a63a4();
void sub_080a63d8();
void sub_080a63f0();
void sub_080a6420();
void sub_080a6438();
void sub_080a650c();
void sub_080a6544();
void sub_080a6574();
void sub_080a658c();
void sub_080a6628();
void sub_080a6650();
void sub_080a6680();
void sub_080a6698();
void sub_080a66c8();
void sub_080a6710();
void sub_080a6734();
void sub_080a6754();
void sub_080a680c();
void sub_080a6868();
void sub_080a6988();
void sub_080a69e4();
void sub_080a6aac();
void sub_080a6b3c();
void sub_080a6c3c();
void sub_080a6d08();
void sub_080a6d60();
s32 sub_080a6e1c(void);
void sub_080a6e58();
void sub_080a6e98();
s32 sub_080a6f38(s16 x, s16 y);
s32 sub_080a6f74();
void sub_080a6fb4();
void sub_080a7080();
void sub_080a7168();
void sub_080a7190();
s32 sub_080a720c();
s32 sub_080a7260();
void sub_080a72b0();
s32 sub_080a72fc();
s32 sub_080a7334();
void sub_080a73b4();
void sub_080a73fc();
void sub_080a741c();
void sub_080a7438();
void sub_080a75a0();
void sub_080a75c8();
void sub_080a787c();
void sub_080a7880();
void sub_080a78a0();
void sub_080a7998();
void sub_080a7ae4();
void sub_080a7b88();
void sub_080a7c9c();
s32 sub_080a7d20();
void Task_Kracko();
void sub_080a7d98();
void sub_080a7df4();
void sub_080a7e10();
void sub_080a7e44();
void sub_080a8038();
void sub_080a85e4();
void sub_080a860c();
void sub_080a87c8();
void sub_080a8834();
void sub_080a8850();
void sub_080a8878();
void sub_080a8948();
void sub_080a8970();
void sub_080a8b44();
void sub_080a8b6c();
void sub_080a8bcc();
void sub_080a8bf4();
void sub_080a8c84();
void sub_080a8d1c();
void sub_080a8f18();
void sub_080a8f40();
void sub_080a8fb4();
void sub_080a8fdc();
void sub_080a9304();
void sub_080a932c();
s32 sub_080a93ec();
void sub_080a9434();
void sub_080a94d4();
s32 sub_080a95dc();
void sub_080a96a4();
void sub_080a96dc();
void sub_080a9738();
s32 sub_080a9760();
void sub_080a9794();
s32 sub_080a97d8();
void sub_080a9814();
void sub_080a983c();
void sub_080a9910();
void sub_080a99a0();
void sub_080a9ba0();
void sub_080a9c28();
void sub_080a9cf0();
void sub_080a9da4();
void sub_080a9e14();
s32 sub_080a9e88(s32 a);
void sub_080a9ea4();
void sub_080a9ed8();
void sub_080a9ef4();
void sub_080aa16c();
void sub_080aa188();
void Task_NightmareWizard();
void sub_080aa338();
void sub_080aa38c();
void sub_080aa3a8();
void sub_080aa47c();
void sub_080aa52c();
void sub_080aa560();
void sub_080aa62c();
void sub_080aa67c();
void sub_080aa6a8();
void sub_080aa6d0();
void sub_080aa71c();
void sub_080aa744();
void sub_080aa970();
void sub_080aa998();
void sub_080aab4c();
void sub_080aab80();
void sub_080aad64();
void sub_080aad98();
void sub_080aaf38();
void sub_080aaf6c();
void sub_080ab158();
void sub_080ab1a8();
void sub_080ab394();
void sub_080ab3c8();
void sub_080ab418();
void sub_080ab440();
void sub_080ab46c();
void sub_080ab4a0();
void sub_080ab4d0(s32 a);
void sub_080ab570();
void sub_080ab5c0();
void sub_080ab670(s32 a);
void sub_080ab728(s32 a);
void sub_080ab7cc();
void sub_080ab810();
s32 sub_080ab854();
s32 sub_080ab8a8();
void sub_080ab93c();
void sub_080abcb4();
void sub_080abcd0(s32 x, s32 y, s32 d);
void sub_080abd04();
void sub_080abe38();
void sub_080abe7c();
void sub_080abf10();
void sub_080abf94();
void sub_080ac020();
void sub_080ac08c();
void sub_080ac124();
void sub_080ac1f0();
void sub_080ac27c();
void sub_080ac30c();
void sub_080ac3a4();
void sub_080ac410();
void sub_080ac47c();
void sub_080ac510();
void sub_080ac530();
s32 sub_080ac678();
void sub_080ac684();
void sub_080ac72c();
void sub_080ac82c();
void sub_080ac84c();
void sub_080ac868();
void sub_080ac94c();
void sub_080ac950();
void sub_080aca38();
void sub_080aca3c();
void sub_080aca60();
void sub_080aca90();
void sub_080acaf0();
void sub_080acb20();
void sub_080acc18();
void sub_080acc8c();
void sub_080acc9c();
void sub_080accf0();
void sub_080acd38();
void sub_080ace60();
void Task_PaintRoller();
void sub_080acf3c();
void sub_080acf48();
void sub_080acf68();
void sub_080acffc();
void sub_080ad08c();
void sub_080ad0f8();
void sub_080ad128();
s32 sub_080ad13c();
void sub_080ad160();
void sub_080ad170();
void sub_080ad278();
void sub_080ad32c();
s32 sub_080ad37c(void);
void sub_080ad3a0();
void sub_080ad3e8();
void sub_080ad458();
void sub_080ad47c();
void Task_HeavyMole();
void sub_080ad630();
void sub_080ad650();
void sub_080ad710();
void sub_080ad788();
void sub_080ad7f0();
void sub_080ad9dc();
void sub_080ada20();
s32 sub_080adaf8();
void sub_080adb58();
void sub_080adb90();
s32 sub_080adbf0();
void sub_080adc44();
void sub_080adca4();
void sub_080add48();
void sub_080addf8();
void sub_080ade98();
void sub_080adebc();
void sub_080adefc();
void sub_080adf50();
void sub_080adfd4();
void sub_080adff8();
void sub_080ae038();
void sub_080ae0b4();
void sub_080ae174();
void sub_080ae198();
void sub_080ae1f0();
void sub_080ae37c();
s32 sub_080ae380(s32 i);
void Task_NightmarePowerOrb();
void sub_080ae470();
void sub_080ae4c4();
void sub_080ae548();
void sub_080ae628();
void sub_080ae79c();
void sub_080ae7ec();
void sub_080ae870();
void sub_080ae8e0();
void sub_080aec00();
void sub_080aed5c();
s32 sub_080aeeb0();
void sub_080aeef8();
void sub_080aef30();
void sub_080aef50();
void sub_080aef5c();
void sub_080aefd4();
void sub_080aeff8();
void sub_080af020();
s32 sub_080af100(void);
void sub_080af114();
void sub_080af144();
void sub_080af178();
void sub_080af188();
void sub_080af1b8();
void sub_080af1c8();
void sub_080af1d4();
void sub_080af20c(u8 a);
void sub_080af26c();
void sub_080af278();
void sub_080af294();
void sub_080af308();
void sub_080af30c();
void sub_080af358();
void sub_080af38c();
void sub_080af4a4();
void sub_080af5bc();
void sub_080af6c8();
void sub_080af7d4();
void sub_080af844();
void sub_080af938();
void sub_080af9e4();
void sub_080afaa4();
void sub_080afb50();
void sub_080afc10();
void sub_080afcd4();
void sub_080afd9c();
void sub_080afdf0();
void sub_080aff40();
void sub_080b0144();
void sub_080b0338();
void sub_080b0570();
void sub_080b05e8();
void sub_080b07d8();
void Task_NightmarePowerOrbIntroScroll();
void sub_080b08a0();
void sub_080b08e4();
void sub_080b09ac();
void sub_080b0b04();
void sub_080b0b50();
void sub_080b0cc8();
void sub_080b0de4();
void sub_080b0e80();
void sub_080b0f04();
void sub_080b0f98();
void sub_080b102c();
void sub_080b10c8();
void sub_080b123c();
void sub_080b1264();
void sub_080b1398();
void sub_080b13bc();
void sub_080b14ac();
void sub_080b14fc();
void sub_080b1564();
void sub_080b1578();
s32 sub_080b1588();
s32 sub_080b1594();
s32 sub_080b15b4();
void sub_080b15c0();
void sub_080b1710();
void sub_080b1770();
void sub_080b17d0();
void sub_080b1830();
void sub_080b1890();
void sub_080b1910();
void sub_080b199c();
void sub_080b1a00();
void sub_080b1a1c();
void sub_080b1b2c();
void sub_080b1c04();
void sub_080b1d2c();
void sub_080b1d98();
void sub_080b1e20();
void sub_080b1ef8();
void sub_080b1f80();
void sub_080b2058(u8 a);
void sub_080b20d4();
void sub_080b214c();
void sub_080b21a0();
void sub_080b2228();
void sub_080b2294();
void sub_080b22b8();
void sub_080b22f8();
void sub_080b2418();
void sub_080b2538();
void sub_080b2550();
void sub_080b2574();
void sub_080b25a4();
void sub_080b25e8();
void sub_080b2664();
void sub_080b2768();
void sub_080b27b0();
s32 sub_080b2804();
void sub_080b2884();
void sub_080b2890();
void Task_WhispyWoods();
void sub_080b2a74();
void sub_080b2aa4();
void sub_080b2ac8();
void sub_080b2ae4();
void sub_080b2b28();
void sub_080b2b40();
void sub_080b2bf0();
void sub_080b2c18();
void sub_080b2cc8();
void sub_080b2cf0();
void sub_080b2d80();
void sub_080b2dd4();
void sub_080b2e20();
void sub_080b2e3c();
void sub_080b2e58();
void sub_080b2f34();
s32 sub_080b2f38();
void sub_080b2f78();
void sub_080b2fb0();
void sub_080b2fe8();
void sub_080b3010(u8 a);
void sub_080b3050();
void sub_080b3090();
void sub_080b30c8();
void sub_080b3110();
void sub_080b312c();
void sub_080b319c();
void sub_080b31a0();
void sub_080b31e0();
void sub_080b3214();
void sub_080b3258();
void sub_080b328c();
void sub_080b32d0();
void sub_080b3318();
void sub_080b3368();
void sub_080b3398();
void sub_080b33bc();
void sub_080b33d8();
void sub_080b3758();
void sub_080b37ec();
void sub_080b38f0();
void sub_080b3a00();
void sub_080b3a64();
void sub_080b3c68();
void sub_080b3e30();
void sub_080b3e54();
void sub_080b3e94();
void sub_080b3ed4();
void sub_080b3f14();
void sub_080b3f54();
void sub_080b3fcc();
void sub_080b3ffc();
s32 sub_080b404c();
s32 sub_080b406c();
s32 sub_080b408c();
void sub_080b40a4();
void sub_080b4100();
void sub_080b4158();
void sub_080b4174();
void sub_080b4190();
void sub_080b4194();
void sub_080b41c8();
void sub_080b41cc();
void sub_080b4200();
s32 sub_080b4204();
void sub_080b4240();
void sub_080b429c();
void sub_080b42f8();
s32 sub_080b4390();
s32 sub_080b43d4();
s32 sub_080b43f4();
s32 sub_080b442c();
void sub_080b447c();
void sub_080b44f0();
s32 sub_080b4524();
s32 sub_080b45c0();
void sub_080b460c();
void sub_080b4648();
void sub_080b469c();
void sub_080b4714();
void sub_080b4754();
void sub_080b4770();
void sub_080b4788();
void sub_080b4794();
void sub_080b47c0();
void sub_080b47cc();
void sub_080b480c();
void sub_080b4878();
void sub_080b48e0();
void sub_080b48f8();
void sub_080b4968();
void sub_080b4a34();
void sub_080b4a5c();
void sub_080b4a8c();
void sub_080b4afc();
void sub_080b4b18();
void sub_080b4b94();
void sub_080b4bb0();
void sub_080b4bd8();
void sub_080b4be4();
void sub_080b4c14();
void sub_080b4c84();
void sub_080b4ca0();
void sub_080b4d1c();
void sub_080b4d50();
void sub_080b4db4();
void sub_080b4dd0();
void sub_080b4df8();
void sub_080b4e04();
void sub_080b4e40();
void sub_080b4ea8();
void sub_080b5024();
s32 sub_080b5338();
void sub_080b54a4();
void sub_080b54d0();
void sub_080b5540();
void sub_080b5558();
s32 sub_080b55d8();
s32 sub_080b5628();
s32 sub_080b5654();
s32 sub_080b5670();
s32 sub_080b5840();
s32 sub_080b590c();
void sub_080b59d8();
s32 sub_080b5a94();
s32 sub_080b5bdc();
s32 sub_080b5d84();
void HBlankScrollVBlankCallback();
void UpdateHBlankScroll();
extern void CpuSet(const void *src, void *dst, u32 control);

void sub_080b2fe8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk6C = 6;
    t->unk30 = t->pixelX;
    t->unk34 = t->pixelY;
}

void sub_080b3010(u8 a)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    u8 *tb;

    c = &gCurTask;
    t = *c;
    if (t->unk30 > 7)
        t->unk30 = 0;
    tb = (u8 *)gUnk_0874C24C;
    TaskSetFrame(tb[(*c)->unk30]);
    u = *c;
    u->unk30++;
    TaskYieldTrampoline(a);
}

void sub_080b3050(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = *c;
    u->frameTable = gUnk_08754308;
    u->unk28 = 1;
    CallTableEntry(u->variant, 1, gUnk_0874C21C);
}

void sub_080b3090(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->updateCallback = (u32)sub_080b30c8;
    t->facing = t->unk74;
    ActorSetState(0);
    u = *c;
    CallTableEntry(u->state, 4, gUnk_0874C220);
}

void sub_080b30c8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    if (t->unk28 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
        {
            u = *c;
            CallTableEntry(u->updateState, 4, gUnk_0874C230);
        }
    }
    else
        CallTableEntry(t->updateState, 4, gUnk_0874C230);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080b3110(void)
{
    CallTableEntry(gCurTask->state, 4, gUnk_0874C220);
}

void sub_080b312c(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *u3;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = z;
    TaskFaceNearestPlayer();
    u = *c;
    u->onGround = z;
    u2 = *c;
    u2->unk6C = z;
    do
    {
        q1 = *c;
        q1->frame = 4;
        TaskYieldTrampoline(4);
        q2 = *c;
        q2->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        q3 = *c;
        q3->unk6C++;
    } while ((s16)q3->unk6C <= 5);
    u3 = gCurTask;
    u3->accelY = 148 << 6;
    u3->speedLimitY = 128 << 11;
    for (;;)
    {
        sub_080b3010(15);
        sub_080b3010(8);
        sub_080b3010(4);
    }
}

void sub_080b319c(void)
{
}

void sub_080b31a0(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 1;
    TaskFaceNearestPlayer();
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFE0000;
    u2->accelY = 168 << 5;
    for (;;)
        sub_080b3010(4);
}

void sub_080b31e0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->pixelY > t->unk2C)
    {
        ActorSetState(2);
        TaskSetEntry(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b3214(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 2;
    TaskSetFrameNoFlip(4);
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(160 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFF0000;
    u2->accelY = 192 << 4;
    for (;;)
        sub_080b3010(8);
}

void sub_080b3258(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->pixelY > t->unk2C)
    {
        ActorSetState(3);
        TaskSetEntry(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b328c(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 3;
    TaskSetFrameNoFlip(4);
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFFC000;
    u2->accelY = 224 << 3;
    for (;;)
        sub_080b3010(8);
}

void sub_080b32d0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    if (t->pixelY > t->unk2C)
    {
        t->onGround = 0;
        TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
        u = *c;
        u->velY = 0xFFFFC000;
        u->accelY = 224 << 3;
    }
}

void sub_080b3318(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    b42 = &t->layer;
    z = 0;
    *b42 = 9;
    u = *c;
    u->frameTable = gUnk_0874C568;
    u->tileWord = z;
    u->facing = 255;
    u2 = *c;
    CallTableEntry(u2->variant, 1, gUnk_0874C254);
}

void sub_080b3368(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b3398;
    ActorSetState(0);
    u = *c;
    CallTableEntry(u->state, 1, gUnk_0874C258);
}

void sub_080b3398(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0874C25C);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080b33bc(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874C258);
}

void sub_080b33d8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 v5;
    s32 v6;
    s32 z;
    struct Task *u0;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;

    t = gCurTask;
    z = 0;
    t->updateState = z;
    u = gCurTask;
    u->onGround = z;
    c = &gCurTask;
    v5 = 128 << 9;
    v6 = 0xFFFD0000;
top:
    u0 = *c;
    u0->velX = 0xFFFC0000;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u1 = *c;
    u1->velY = v5;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u2 = *c;
    u2->velX = v6;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u3 = *c;
    u3->velY = 128 << 8;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    u4 = *c;
    u4->velX = v6;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u5 = *c;
    u5->velY = v5;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u6 = *c;
    u6->velX = 0xFFFE0000;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    u7 = *c;
    u7->velY = 0;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u8 = *c;
    u8->velX = 0xFFFF0000;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u9 = *c;
    u9->velY = v5;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    goto top;
}

void sub_080b3758(void)
{
    struct Task *t;
    struct Task *t6;
    s16 *a;
    u32 h;
    s32 w;
    u8 *t46;
    u32 *t68;
    u8 *t40;
    s32 d;
    u8 dv;
    s32 a0v;

    t = gCurTask;
    a = (s16 *)((u8 *)t + 108);
    h = *(u16 *)a;
    if (*a != 0)
    {
        *a = h - 1;
        t46 = (u8 *)gUnk_0874C246;
        dv = t46[*a];
        d = (s8)dv;
        t6 = t;
        w = t6->unk30 - d;
        t6->unk30 = w;
        a0v = t->layer + 1;
        t68 = (u32 *)gUnk_0874C568;
        t40 = (u8 *)gUnk_0874C240;
        QueueSprite(a0v, t68[t40[*a]], t->spriteFlags, t->tileWord,
                     w - gSpriteCameraX,
                     (s16)(t->unk34 - (u16)gSpriteCameraY));
    }
}

void sub_080b37ec(void)
{
    gCurTask->health = gCurTask->unk80;
    gCurTask->unk80 = gCurTask->unk46;
    gCurTask->unk46 = gCurTask->unk70;
    gCurTask->unk70 = (u32)gCurTask->unk24 >> 16;
    gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF) + (((u32)gCurTask->unk20 >> 16) << 16);
    gCurTask->unk20 = (gCurTask->unk20 & 0xFFFF) + (((u32)gCurTask->unk34 >> 16) << 16);
    gCurTask->unk34 = (gCurTask->unk34 & 0xFFFF) + (((u32)gCurTask->unk30 >> 16) << 16);
    gCurTask->unk30 = (gCurTask->unk30 & 0xFFFF) + (((u32)gCurTask->unk2C >> 16) << 16);
    /* the redundant mask is what loads 0xFFFF0000 here for the ANDs below
       (combine drops the AND before the shift, the constant stays) */
    gCurTask->unk2C = (gCurTask->unk2C & 0xFFFF) + (((u32)(gCurTask->unk28 & 0xFFFF0000) >> 16) << 16);
    gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF) + (gCurTask->pixelX << 16);
    gCurTask->unk84 = gCurTask->hitterPlayer;
    gCurTask->hitterPlayer = gCurTask->parent;
    gCurTask->parent = gCurTask->facing;
    gCurTask->facing = gCurTask->unk24;
    gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) + (gCurTask->unk20 & 0xFFFF);
    gCurTask->unk20 = (gCurTask->unk20 & 0xFFFF0000) + (gCurTask->unk34 & 0xFFFF);
    gCurTask->unk34 = (gCurTask->unk34 & 0xFFFF0000) + (gCurTask->unk30 & 0xFFFF);
    gCurTask->unk30 = (gCurTask->unk30 & 0xFFFF0000) + (gCurTask->unk2C & 0xFFFF);
    gCurTask->unk2C = (gCurTask->unk2C & 0xFFFF0000) + (gCurTask->unk28 & 0xFFFF);
    gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF0000) + gCurTask->pixelY;
}

void sub_080b38f0(void)
{
    register struct Task **c asm("r4");
    struct Task *t;
    struct Task *q;
    struct Task *t2;
    struct Task *t3;
    register u16 *a70 asm("r3");
    register s16 *a48 asm("r2");
    s32 w24c;
    s32 wv;
    s16 *a4A;
    u8 *aw;
    register u8 *ab asm("r1");
    register u8 *ac asm("r0");
    register s32 k6 asm("r2");
    s32 w;
    s32 w2;
    register s32 w3 asm("r0");
    register s32 w4 asm("r0");
    s32 w24b;
    register s32 w5 asm("r2");
    register s32 w6 asm("r2");

    c = &gCurTask;
    t = *c;
    a70 = (u16 *)((u8 *)t + 112);
    q = t;
    w24c = *(u16 *)&q->unk24;
    a48 = (s16 *)((u8 *)q + 72);
    t->unk24 = w24c + (*a48 << 16);
    t->unk20 = *(u16 *)&t->unk20 + (*a48 << 16);
    t->unk34 = *(u16 *)&t->unk34 + (*a48 << 16);
    t->unk30 = *(u16 *)&t->unk30 + (*a48 << 16);
    t->unk2C = *(u16 *)&t->unk2C + (*a48 << 16);
    t->unk28 = *(u16 *)&t->unk28 + (*a48 << 16);
    w = *a48;
    t->unk18 = w;
    *a70 = w;
    aw = (u8 *)t + 70;
    *(u16 *)aw = w;
    aw += 58;
    *aw = w;
    w2 = (s8)(u8)w;
    aw -= 8;
    *(s16 *)aw = w2;
    t2 = *c;
    wv = *(u16 *)((u8 *)t2 + 74);
    *(u8 *)((u8 *)t2 + 67) = wv;
    t3 = *c;
    w24b = t3->unk24;
    k6 = 0xFFFF0000;
    w24b &= k6;
    a4A = (s16 *)((u8 *)t3 + 74);
    t3->unk24 = w24b + *a4A;
    asm("" ::: "memory");
    t3->unk20 = (t3->unk20 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk34 = (t3->unk34 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk30 = (t3->unk30 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk2C = (t3->unk2C & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk28 = (t3->unk28 & k6) + *a4A;
    asm("" ::: "memory");
    w3 = *a4A;
    t3->unk1C = w3;
    ab = (u8 *)t3 + 67;
    *ab = w3;
    w4 = (s8)(u8)w3;
    w5 = w4;
    *(s16 *)(ab + 1) = w4;
    ac = (u8 *)t3 + 127;
    *ac = w5;
    w6 = (s8)(u8)w5;
    ac += 5;
    *(s16 *)ac = w6;
}

void sub_080b3a00(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    s32 r;
    s32 r2;
    s32 r3;
    s32 r4;
    s32 r5;
    s32 r6;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;

    r = RandomRange(30);
    c = &gCurTask;
    u1 = *c;
    u1->hitTimer = r - 10;
    r2 = RandomRange(30);
    u2 = *c;
    u2->onGround = r2 - 10;
    r3 = RandomRange(30);
    u3 = *c;
    w3 = r3 - 10;
    *(u8 *)((u8 *)u3 + 123) = w3;
    r4 = RandomRange(30);
    u4 = *c;
    w4 = r4 - 10;
    *(u8 *)((u8 *)u4 + 124) = w4;
    r5 = RandomRange(30);
    u5 = *c;
    w5 = r5 - 10;
    *(u8 *)((u8 *)u5 + 125) = w5;
    r6 = RandomRange(30);
    u6 = *c;
    w6 = r6 - 10;
    *(u16 *)((u8 *)u6 + 130) = w6;
}

void sub_080b3a64(void)
{
    if (gUnk_0874C260[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C260[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->pixelX - gSpriteCameraX + gCurTask->hitTimer,
                     gCurTask->pixelY - gSpriteCameraY + gCurTask->hitKind);
    if (gUnk_0874C2A6[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C2A6[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     ((u32)gCurTask->unk20 >> 16) - gSpriteCameraX + gCurTask->onGround,
                     gCurTask->unk20 - gSpriteCameraY + (s8)gCurTask->hitDirection);
    if (gUnk_0874C2EC[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C2EC[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->health - gSpriteCameraX + gCurTask->waterFlags,
                     gCurTask->unk84 - gSpriteCameraY + (s8)gCurTask->unk82);
    if (gUnk_0874C332[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C332[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->unk18 - gSpriteCameraX,
                     gCurTask->unk1C - gSpriteCameraY);
}

void sub_080b3c68(void)
{
    s32 m;
    s32 z;

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->tileWord = 0;
    gCurTask->drawCallback = (u32)sub_080b3a64;
    gCurTask->layer = 8;
    gCurTask->frameTable = (u32 *)gUnk_0874CE68;
    gCurTask->updateCallback = (u32)sub_080b3e30;
    sub_080b38f0();
    sub_080b3a00();
    gCurTask->unk6C = 66;
    gCurTask->unk6E = 0;
    /* the two constants the loop keeps in r5/r8 are variables (lesson 3.471) */
    m = 0x8000;
    z = 0;
    do
    {
        gCurTask->velY = z;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x18000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x10000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = m;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x4000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(2);
        gCurTask->velX = z;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x18000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x4000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(2);
    } while (++gCurTask->unk6E <= 2);
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x18000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x10000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x8000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x4000;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x18000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x10000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x8000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x4000;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080b3e30(void)
{
    struct Task **c;
    u8 *su;
    s32 w;

    sub_080b37ec();
    c = &gCurTask;
    su = (u8 *)*c + 108;
    w = *(u16 *)su - 1;
    *(u16 *)su = w;
    if ((u16)w == 0)
        ActorDestroy();
}

void sub_080b3e54(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_0874CCF4;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3e94(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_0874CD0C;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3ed4(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_0874CD24;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3f14(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_0874CD3C;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3f54(void)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    if (a->animScript == NULL)
        gCurTask->unk34 = ActorStartAnim((struct AnimCmd *)gUnk_08756084);
    gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
    if (gUnk_03001F30 == 1 && gCurTask->unk76 == 1)
        gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xE000;
    a->extraLayerOffset = 1;
    a->extraTileWord = 0xF000;
    a->extraOffsetY = 0;
    a->unk16 = 0;
}

void sub_080b3fcc(void)
{
    struct Task **c;
    struct Task *t;
    struct Actor *a;
    s32 u;

    c = &gCurTask;
    t = *c;
    a = *(struct Actor **)((u8 *)t + 140);
    if (t->frame == -1)
    {
        *(u16 *)((u8 *)a + 26) = 5;
        t->frame = 4;
    }
    else
    {
        u = 0xFFFF;
        *(u16 *)((u8 *)a + 26) = u;
    }
}

void sub_080b3ffc(void)
{
    if (gCurTask->frame != 5)
    {
        if (gUnk_03001F30 == 1 && gCurTask->unk76 == 1)
            gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xE000;
    }
    else
        gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
}

s32 sub_080b404c(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_080b4158, gCurTaskIdx);
    return 1;
}

s32 sub_080b406c(void)
{
    ActorSetState(0);
    TaskSetEntry(sub_080b4158, gCurTaskIdx);
    return 1;
}

s32 sub_080b408c(void)
{
    struct Task *t;

    t = gCurTask;
    t->accelY = 128 << 5;
    t->speedLimitY = 160 << 9;
    return 0;
}

void sub_080b40a4(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->updateCallback = (u32)sub_080b4100;
    *(u8 *)((u8 *)ta + 122) = 0;
    TaskInitWaterFlags();
    tb = *c;
    if ((s8)*(u8 *)((u8 *)tb + 123) == 3)
        ActorSetState(2);
    else
        ActorSetState(1);
    sub_080b3f54();
    CallTableEntry(gCurTask->state, 3, gUnk_087560A0);
}

void sub_080b4100(void)
{
    s32 v;

    if (gCurTask->variant == 0 && (u8)sub_08069888() == 0)
        CallTableEntry(gCurTask->updateState, 3, gUnk_087560AC);
    v = gCurTask->unk34;
    gCurTask->unk34 = ActorTickAnim(v);
    if (v <= 0)
    {
        sub_080b3fcc();
        sub_080b3ffc();
    }
    ActorCheckHits();
    sub_08069bbc();
}

void sub_080b4158(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_087560A0);
}

void sub_080b4174(void)
{
    gCurTask->updateState = 0;
    TaskStopY();
    TaskSleepForever();
}

void sub_080b4190(void)
{
}

void sub_080b4194(void)
{
    struct Task **c;
    struct Task *t;

    c = &gCurTask;
    (*c)->updateState = 1;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->accelY = 128 << 6;
        t->speedLimitY = 160 << 10;
    }
    else
    {
        TaskStop();
    }
    TaskSleepForever();
}

void sub_080b41c8(void)
{
}

void sub_080b41cc(void)
{
    struct Task **c;
    struct Task *t;

    c = &gCurTask;
    (*c)->updateState = 2;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->accelY = 128 << 5;
        t->speedLimitY = 160 << 9;
    }
    else
    {
        TaskStop();
    }
    TaskSleepForever();
}

void sub_080b4200(void)
{
}

s32 sub_080b4204(u32 a)
{
    s32 r;

    if ((gActivePlayerMask >> a) & 1)
    {
        r = AddPlayerHealth(8, a);
        if (r <= *(s16 *)gMaxHealth - 1)
            return 0;
    }
    return 1;
}

void sub_080b4240(void)
{
    struct Task **c;
    struct Task *t;
    s16 *h;
    s32 w;
    u8 *e;

    c = &gCurTask;
    t = *c;
    t->moveCallback = 0;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    w = (s8)*(u8 *)((u8 *)t + 126);
    h = (s16 *)((u8 *)t + 68);
    *h = w;
    e = (u8 *)gPlayerHealth;
    if (*(s16 *)((*h << 1) + (u32)e) != 0)
    {
        sub_080670ac(15);
        CallTableEntry((*c)->state, 2, gUnk_087560B8);
        sub_080670d4();
    }
    ActorDestroy();
}

void sub_080b429c(void)
{
    struct Task **c;
    u8 k4;

    sub_08067108();
    c = &gCurTask;
    do
    {
        if (gLocalPlayer == *(s16 *)((u8 *)*c + 68))
            PlaySfx(221);
        k4 = sub_080b4204(*(s16 *)((u8 *)*c + 68));
        TaskYieldTrampoline(8);
    } while (k4 == 0);
    sub_08040894(*(s16 *)((u8 *)gCurTask + 68), 1);
    sub_08067114();
}

void sub_080b42f8(void)
{
    struct Task **c;
    struct Task **c2;
    struct Task *t;
    struct Task *t2;
    u8 *h;
    s32 w;
    s32 w0;
    s32 n;
    u8 k4;
    s32 z;
    u8 *h0;

    w0 = *(u8 *)gExtraMode;
    n = 1;
    if (w0 == 0 && gUnk_03001F30 == 0)
        n = 2;
    sub_08067108();
    c2 = &gCurTask;
    t = *c2;
    h0 = (u8 *)t + 108;
    z = 0;
    *(u16 *)h0 = z;
    if (z >= n)
        goto xend;
    c = c2;
xbody:
    if (gLocalPlayer == *(s16 *)((u8 *)*c + 68))
        PlaySfx(221);
    k4 = sub_080b4204(*(s16 *)((u8 *)*c + 68));
    TaskYieldTrampoline(8);
    if (k4 != 0)
        goto xend;
    t2 = *c;
    h = (u8 *)t2 + 108;
    w = *(u16 *)h + 1;
    *(u16 *)h = w;
    if (*(s16 *)h < n)
        goto xbody;
xend:
    sub_08040894(*(s16 *)((u8 *)gCurTask + 68), 2);
    sub_08067114();
}

s32 sub_080b4390(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    t = *c;
    if (t->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        *(u8 *)((u8 *)t + 122) = 0;
        (*c)->velY = 0xFFFD0000;
        PlaySfx(157);
        r = 0;
    }
    return r;
}

s32 sub_080b43d4(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_080b4754, gCurTaskIdx);
    return 1;
}

s32 sub_080b43f4(void)
{
    s32 r;

    if (gCurTask->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        TaskToggleFacingAndReverseX();
        PlaySfx(157);
        r = 0;
    }
    return r;
}

s32 sub_080b442c(void)
{
    struct Task *t;
    s32 r;

    t = gCurTask;
    if (t->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->velY = 0;
        if ((u8)(gTerrainResult[4] - 5) <= 3)
            TaskToggleFacingAndReverseX();
        PlaySfx(157);
        r = 0;
    }
    return r;
}

void sub_080b447c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    u8 *b3;
    u8 *e;
    s32 i;
    s32 o;
    s32 w;
    s32 z;

    c = &gCurTask;
    t = *c;
    b3 = (u8 *)gTasks;
    i = *(s16 *)((u8 *)t + 68);
    o = i * 144;
    w = *(u8 *)(b3 + o + 67);
    w = -w;
    *(u8 *)((u8 *)t + 67) = w;
    e = (u8 *)gUnk_087560C0;
    TaskSetMotionXFacing(*(s32 *)((*(u8 *)((u8 *)*c + 116) << 2) + (u32)e), 0x5A5A5A5A);
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    t2 = *c;
    t2->unk28 = 130 << 1;
    t2->unk2C = 2;
    t2->unk30 = 48;
    z = 0;
    t2->frame = 4;
    *(u8 *)((u8 *)t2 + 122) = z;
}

void sub_080b44f0(void)
{
    struct Task **c;
    struct Task **c2;
    struct Task *t;
    struct Task *t3;
    s32 w0;
    s32 w;

    c = &gCurTask;
    t = *c;
    w0 = t->unk2C;
    c2 = c;
    if (w0 <= 0)
    {
        w = *(u16 *)((u8 *)t + 60) + 1;
        *(u16 *)((u8 *)t + 60) = w;
        if ((s16)w > 19)
            t->frame = 4;
        (*c2)->unk2C = 2;
    }
    t3 = *c2;
    t3->unk2C = t3->unk2C - 1;
}

s32 sub_080b4524(void)
{
    struct PointPair box;
    struct Task *t = gCurTask;
    s32 r;

    box.x0 = t->pixelX - 640;
    box.y0 = t->pixelY - 640;
    box.x1 = t->pixelX + 640;
    box.y1 = t->pixelY + 640;
    if (TaskIsInRectSlot(&box, t->parent) != 0)
        r = 0;
    else
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_080b45c0(void)
{
    struct Task *t;
    u8 *p;
    s32 r;

    t = gCurTask;
    p = *(u8 **)((u8 *)t + 136);
    if (((gActivePlayerMask >> *(s16 *)((u8 *)t + 68)) & 1) && *(s8 *)(p + 13) == 0)
        r = 0;
    else
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    return r;
}

void sub_080b460c(void)
{
    u8 *q0;
    s32 w;
    s32 w2;

    q0 = (u8 *)gTerrainBoundsClamp;
    w = *q0;
    if (1 & w)
        goto docall;
    w2 = 2;
    w2 &= w;
    if (w2 == 0)
        goto skip;
docall:
    TaskToggleFacingAndReverseX();
skip:
    if (*(u8 *)gTerrainBoundsClamp & 4)
        gCurTask->velY = 0;
}

void sub_080b4648(void)
{
    struct Task *t;
    s32 w;
    s32 wl;

    if (sub_080b45c0() != 0)
        return;
    if (sub_080b4524() != 0)
        return;
    t = gCurTask;
    wl = t->unk28;
    w = wl;
    wl = wl - 1;
    t->unk28 = wl;
    if (w <= 0)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        return;
    }
    if (sub_0802294c(t) != 0)
    {
        ActorDestroy();
        return;
    }
    sub_080b44f0();
}

void sub_080b469c(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;
    s32 w;
    s32 w2;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)ActorDrawWorldInView;
    *(u8 *)((u8 *)ta + 66) = 5;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_0874CAEC;
    tb->updateCallback = (u32)sub_080b4714;
    sub_080b447c();
    w = *(u8 *)((u8 *)*c + 123);
    w2 = 1;
    w2 &= w;
    if (w2 == 0)
        goto elsecall;
    w2 = 64;
    w2 &= w;
    if (w2 != 0)
        goto elsecall;
    ActorSetState(1);
    goto after;
elsecall:
    ActorSetState(0);
after:
    CallTableEntry(gCurTask->state, 2, gUnk_087560D0);
}

void sub_080b4714(void)
{
    struct Task *t;
    s32 w;

    if ((u8)sub_080696a0() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_087560D8);
    t = gCurTask;
    w = t->unk30;
    if (w <= 0)
        ActorCheckHits();
    else
        t->unk30 = w - 1;
    sub_08069b84();
}

void sub_080b4754(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_087560D0);
}

void sub_080b4770(void)
{
    gCurTask->updateState = 0;
    TaskSleepForever();
}

void sub_080b4788(void)
{
    sub_080b4648();
}

void sub_080b4794(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 1;
    TaskStop();
    (*c)->accelY = 128 << 3;
    TaskYieldTrampoline(60);
    ActorDie();
}

void sub_080b47c0(void)
{
    sub_080b4648();
}

void sub_080b47cc(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)ActorMove;
    ta->drawCallback = (u32)ActorDrawWorldInView;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gUnk_08754780;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 3, gUnk_087560E0);
}

void sub_080b480c(void)
{
    s32 i;
    s32 o;
    s32 n;
    vu16 *e;
    struct Task *t;

    i = 32;
    n = -1;
    o = 144 << 5;
    do
    {
        e = (vu16 *)gTaskSlotTypes;
        e = (vu16 *)((i << 1) + (u32)e);
        if ((s16)*e != n && *e == 68)
        {
            t = (struct Task *)((u8 *)gTasks + o);
            if (*(u8 *)(*(u8 **)((u8 *)t + 140) + 4) != 0)
            {
                t->posX = *(s16 *)((u8 *)t + 72) << 16;
                t->posY = *(s16 *)((u8 *)t + 74) << 16;
            }
            TaskSetEntry(ActorDie, i);
        }
        o += 144;
        i += 1;
    } while (i <= 62);
}

void sub_080b4878(void)
{
    s32 i;
    u8 *b5;
    s32 w;

    i = 0;
    if (i < gPlayerCount)
    {
        b5 = (u8 *)gPlayerStates;
        do
        {
            if ((gActivePlayerMask >> i) & 1)
            {
                sub_0803e68c(i);
                w = (s8)*(u8 *)(b5 + 116 * i + 13);
                if (w == 24 || w == 11)
                    SetPlayerAbilityNoHud(0, -1, i);
            }
            i++;
        } while (i < gPlayerCount);
    }
    sub_080b480c();
    sub_08067108();
}

void sub_080b48e0(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->unk1C = 0;
    t->unk18 = 0;
}

void sub_080b48f8(void)
{
    s32 i;
    s32 k;
    s32 n;
    u16 *pa;
    s32 n2;
    s32 one;
    u16 *pb;
    s32 m5;
    u8 *p2;
    u8 *pc7;

    i = 0;
    k = 0;
    n2 = gPlayerCount;
    pa = &gPlayerCount;
    pc7 = &gActivePlayerCount;
    if (k < n2)
    {
        m5 = gActivePlayerMask;
        n = n2;
        one = 1;
        p2 = (u8 *)gPlayerStates;
        do
        {
            if (((m5 >> i) & one) && *(s8 *)(p2 + 22) == 2)
                k++;
            p2 += 116;
            i++;
        } while (i < n);
    }
    if (k == *pc7)
    {
        pb = pa;
        if (*pb == 1)
            sub_0805ddb0(0);
        else
            sub_0805deac();
        ActorDestroy();
    }
}

void sub_080b4968(void)
{
    s32 i;
    s32 o;
    s32 one;
    s32 b;
    struct Task **c;
    struct Task *t;
    struct Task *t3;
    u8 *p1;
    s32 w1;
    s32 w2;

    i = 0;
    if (i >= gPlayerCount)
        return;
    o = i;
    one = 1;
    do
    {
        if ((gActivePlayerMask >> i) & one)
        {
            b = one << i;
            c = &gCurTask;
            if (!((*c)->unk18 & b))
            {
                t3 = (struct Task *)((u8 *)gTasks + o);
                w1 = i * 116;
                p1 = (u8 *)((u32)gPlayerStates + w1);
                w2 = p1[4];
                if (w2 == 0)
                {
                    if (*(s16 *)((u8 *)t3 + 74) <= 116)
                    {
                        p1[1] = 22;
                        *(u32 *)(p1 + 104) = w2;
                        *(u8 *)((u8 *)t3 + 122) = w2;
                    }
                    else
                    {
                        sub_0805e110(i);
                        t = *c;
                        t->unk18 |= b;
                        t->unk1C++;
                    }
                }
                else if (*(u32 *)(p1 + 104) == 0 && *(s16 *)((u8 *)t3 + 74) > 116)
                {
                    *(u32 *)(p1 + 104) = (u32)gPlayerDefaultTerrainBox;
                }
            }
        }
        o += 144;
        i++;
    } while (i < gPlayerCount);
}

void sub_080b4a34(void)
{
    if (gCurTask->unk1C == gActivePlayerCount)
        sub_080b48f8();
    else
        sub_080b4968();
}

void sub_080b4a5c(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b4a8c;
    ActorSetState(0);
    CallTableEntry((*c)->state, 2, gUnk_08756150);
}

void sub_080b4a8c(void)
{
    struct Task **c;

    c = &gCurTask;
    CallTableEntry((*c)->updateState, 2, gUnk_08756158);
    if ((*c)->state == 0 && ActorCheckHits() != 0)
    {
        sub_080b4878();
        sub_0806d4e4(0, 0);
        if (gLocalPlayer == (s8)*(u8 *)((u8 *)*c + 126))
            PlaySfx(198);
        ActorSetState(1);
        TaskSetEntry(sub_080b4afc, gCurTaskIdx);
    }
}

void sub_080b4afc(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08756150);
}

void sub_080b4b18(void)
{
    struct Task **c;
    struct Task *t;
    s32 v6;
    s32 v5;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 0;
    *(u8 *)((u8 *)*c + 67) = 1;
    TaskStop();
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
        (*c)->velY = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4b94(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4bb0(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 1;
    sub_080b48e0();
    (*c)->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_080b4bd8(void)
{
    sub_080b4a34();
}

void sub_080b4be4(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b4c14;
    ActorSetState(0);
    CallTableEntry((*c)->state, 3, gUnk_08756160);
}

void sub_080b4c14(void)
{
    struct Task **c;

    c = &gCurTask;
    CallTableEntry((*c)->updateState, 3, gUnk_0875616C);
    if ((*c)->state == 1 && ActorCheckHits() != 0)
    {
        sub_080b4878();
        sub_0806d4e4(0, 0);
        if (gLocalPlayer == (s8)*(u8 *)((u8 *)*c + 126))
            PlaySfx(198);
        ActorSetState(2);
        TaskSetEntry(sub_080b4c84, gCurTaskIdx);
    }
}

void sub_080b4c84(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_08756160);
}

void sub_080b4ca0(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 0;
    *(u8 *)((u8 *)*c + 67) = 255;
    TaskStop();
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    t->velX = 0xFFFC0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFE0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFF0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFF8000;
    TaskYieldTrampoline(8);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080b4d1c(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
    if (t->state != 0)
        TaskSetEntry(sub_080b4c84, gCurTaskIdx);
}

void sub_080b4d50(void)
{
    struct Task **c;
    struct Task *t;
    s32 v6;
    s32 v5;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 1;
    TaskStop();
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
        (*c)->velY = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4db4(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4dd0(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 2;
    sub_080b48e0();
    (*c)->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_080b4df8(void)
{
    sub_080b4a34();
}

void sub_080b4e04(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = 0;
    if (*(u8 *)((u8 *)t + 116) != 2)
        TaskYieldTrampoline(60);
    if (sub_08066394() != 0)
    {
        sub_080670ac(15);
        ExitClearedStage();
    }
    TaskSleepForever();
}

void sub_080b4e40(void)
{
    u8 *p1;
    u8 *q;
    s32 w;
    s32 m5;
    s32 z4;
    s32 z3;
    s32 k2;
    s32 i2;

    m5 = 255;
    z4 = 0;
    z3 = 0;
    p1 = (u8 *)gUnk_020060A0;
    k2 = 9;
    do
    {
        w = *p1;
        w |= m5;
        *p1 = w;
        *(u16 *)(p1 + 2) = z3;
        *(u8 *)(p1 + 1) = z4;
        p1 += 4;
        k2 -= 1;
    } while (k2 >= 0);
    i2 = 0;
    do
    {
        ((u8 *)gUnk_02008020)[i2] = 0;
        q = (u8 *)gUnk_02006130 + i2;
        w = *q;
        w |= 255;
        *q = w;
        i2++;
    } while (i2 <= 47);
    i2 = 0;
    do
    {
        q = (u8 *)gUnk_02005590 + i2;
        w = *q;
        w |= 255;
        *q = w;
        i2++;
    } while (i2 <= 30);
    sub_080b4ea8();
}
