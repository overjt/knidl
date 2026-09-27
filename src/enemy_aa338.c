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
extern u32 gBigSwitchFlags[];
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
extern u32 gMrShineAndMrBrightStates[];
extern u32 gMrShineAndMrBrightStateUpdates[];
extern u32 gMrShineStates[];
extern u32 gMrShineStateUpdates[];
extern u32 gMrBrightStates[];
extern u32 gMrBrightStateUpdates[];
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
extern u32 gMetaKnightStates[];
extern u32 gMetaKnightStateUpdates[];
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
extern u32 gNightmareWizardStates[];
extern u32 gNightmareWizardStateUpdates[];
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
extern u32 gWhispyWoodsStates[];
extern u32 gWhispyWoodsStateUpdates[];
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
extern u32 gUnk_0874C260[];
extern u32 gUnk_0874C2A6[];
extern u32 gUnk_0874C2EC[];
extern u32 gUnk_0874C332[];
extern u32 gUnk_0874C44C[];
extern u32 gUnk_0874C500[];
extern u32 gUnk_0874C568[];
extern u32 gAbilityStarFrames[];
extern u32 gOneUpFrames[];
extern u32 gMaximTomatoFrames[];
extern u32 gInvincibleCandyFrames[];
extern u32 gEnergyDrinkFrames[];
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
extern u32 gWhispyWoodsAppleFrames[];
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
extern u32 gPickupVariants[];
extern u32 gUnk_08756084[];
extern u32 gPickupStates[];
extern u32 gPickupStateUpdates[];
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
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
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
extern s16 CreateStarFlash(u8 kind, s32 dx, s32 dy);
extern s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d);
extern void CreateBurstEffect(u32 a, s32 b);
extern void PlayExplosionAnim(void);
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
void CreateMrShineAndMrBright();
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
void MrShineAndMrBrightInit();
void MrShineAndMrBrightUpdate();
void MrShineAndMrBrightEnterState();
void sub_080a3184();
void sub_080a31a4();
void sub_080a31d0();
void sub_080a31f0();
void sub_080a31f4();
void sub_080a3238();
void sub_080a3250();
void sub_080a3268();
void sub_080a3280();
void MrShineInit();
void MrShineUpdate();
void MrShineEnterState();
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
void MrBrightInit();
void MrBrightUpdate();
void MrBrightEnterState();
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
void Task_KingDededeStar();
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
void MetaKnightInit();
void MetaKnightEnterState();
void MetaKnightUpdate();
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
void NightmareWizardInit();
void NightmareWizardEnterState();
void NightmareWizardUpdate();
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
void WhispyWoodsInit();
void WhispyWoodsUpdate();
void WhispyWoodsEnterState();
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
void Task_WhispyWoodsApple();
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
void Task_WhispyWoodsAirPuff();
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
void Task_OneUp();
void Task_MaximTomato();
void Task_InvincibleCandy();
void Task_EnergyDrink();
void sub_080b3f54();
void sub_080b3fcc();
void sub_080b3ffc();
s32 sub_080b404c();
s32 sub_080b406c();
s32 sub_080b408c();
void PickupInit();
void PickupUpdate();
void PickupEnterState();
void sub_080b4174();
void sub_080b4190();
void sub_080b4194();
void sub_080b41c8();
void sub_080b41cc();
void sub_080b4200();
s32 sub_080b4204();
void PickupHeal();
void MaximTomatoHeal();
void EnergyDrinkHeal();
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
void Task_AbilityStar();
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

void NightmareWizardInit(void)
{
    gCurTask->updateCallback = (u32)NightmareWizardUpdate;
    gCurTask->unk28 = -1;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 0;
    gCurTask->unk34 = 0;
    gCurTask->unk1C = (s16)ActorComputeHealth();
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 12, gNightmareWizardStates);
}

void NightmareWizardEnterState(void)
{
    CallTableEntry(gCurTask->state, 12, gNightmareWizardStates);
}

void NightmareWizardUpdate(void)
{
    s32 n;

    CallTableEntry(gCurTask->updateState, 12, gNightmareWizardStateUpdates);
    if ((u16)gCurTask->frame <= 35)
    {
        gUnk_03001F2C = gUnk_08749380[gCurTask->frame];
        ActorSetAttackBox(gUnk_08749358[gUnk_03001F2C]);
        sub_08063a00(gUnk_0874936C[gUnk_03001F2C]);
        sub_08068f68();
    }
    else
    {
        n = gUnk_02007D00[0] & 1;
        if (n != 0)
        {
            if (gUnk_02007D00[1] == 0)
            {
                ActorSetAttackBox((u32)gUnk_08749870);
                sub_080639f0((struct ActorAux *)gUnk_08749AF8);
            }
            else
            {
                ActorSetAttackBox((u32)gUnk_0874988C);
                sub_080639f0((struct ActorAux *)gUnk_08749B00);
            }
            ActorCheckHits();
        }
        else
        {
            gCurTask->hitKind = n;
            gCurTask->hitTimer = n;
            gCurTask->hitterSlot = 255;
            gCurTask->hitterPlayer = -1;
        }
    }
    ActorReactToHit();
}

void sub_080aa47c(void)
{
    gCurTask->updateState = 0;
    ActorStopAnim();
    sub_080ab4a0();
    TaskStop();
    sub_080ab670(1);
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    sub_08066544();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(15);
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 7;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(15);
        gCurTask->velY = 128 << 7;
        TaskYieldTrampoline(15);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080aa52c(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 0)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aa560(void)
{
    s32 v;
    struct Task *t;
    s32 r1v;
    s32 r2v;

    gCurTask->updateState = 1;
    r1v = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    t = gCurTask;
    t->unk18 = r1v;
    t->unk24 = gUnk_08749224[t->unk28];
    if (t->unk24 == 0)
        goto is0;
    if (t->unk24 == 3)
    {
        v = 18;
        goto setv;
    }
    goto els;
is0:
    v = 60;
    goto setv;
els:
    r2v = RandomRange(3);
    t = gCurTask;
    t->unk2C = (t->unk2C + r2v + 1) & 3;
    v = gUnk_08749220[t->unk2C];
setv:
    t->unk20 = v;
    if (gCurTask->unk24 != 4)
    {
        for (;;)
        {
            gCurTask->velY = -0x8000;
            TaskYieldTrampoline(10);
            gCurTask->velY = -0x10000;
            TaskYieldTrampoline(10);
            gCurTask->velY = -0x8000;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 9;
            TaskYieldTrampoline(10);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(10);
        }
    }
    TaskSleepForever();
}

void sub_080aa62c(void)
{
    s32 w;

    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    w = gCurTask->unk20 - 1;
    gCurTask->unk20 = w;
    if (w == 0)
    {
        TaskStop();
        ActorSetState(gUnk_08749230[gCurTask->unk24][0]);
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
    }
}

void sub_080aa67c(void)
{
    gCurTask->updateState = 2;
    sub_080ab4a0();
    TaskStop();
    sub_080ab670(1);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080aa6a8(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aa6d0(void)
{
    s32 n;

    gCurTask->updateState = 3;
    TaskStop();
    sub_080ab728(1);
    n = gCurTask->unk34 + 1;
    gCurTask->unk34 = n;
    if ((n & 3) != 0)
    {
        TaskYieldTrampoline(RandomRange(31) + 30);
        ActorSetState(2);
    }
    else
        ActorSetState(10);
    TaskSleepForever();
}

void sub_080aa71c(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aa744(void)
{
    struct Task **c;
    struct Task *u;
    s32 *pb;
    s32 w;

    gCurTask->updateState = 4;
    TaskGetNearestPlayerScreenPos();
    if (gUnk_030023B4 > 128)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    TaskSetFrame(36);
    gCurTask->unk24 = 2;
    c = &gCurTask;
    pb = &gUnk_030023B4;
top:
    u = *c;
    w = u->unk24 - 1;
    u->unk24 = w;
    if (w == 0)
    {
        u->frame++;
        if ((s16)u->frame > 39)
            u->frame = 36;
        (*c)->unk24 = 2;
    }
    TaskGetScreenPos();
    if (((*c)->facing == 1 && *pb <= 31)
        || ((*c)->facing == -1 && *pb > 208))
        goto out;
    TaskYieldTrampoline(1);
    goto top;
out:
    if (abs(TaskGetNearestPlayerDx()) <= 111)
    {
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        gCurTask->unk6C = 0;
        do
        {
            sub_080ab7cc();
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
        TaskSetMotionXFacing(176 << 11, 0x5A5A5A5A);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 23);
        TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
        TaskSetFrame(56);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->facing *= -1;
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        sub_080ab7cc();
        TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    }
    else
    {
        TaskStop();
        gCurTask->facing *= -1;
        TaskSetFrame(56);
        TaskYieldTrampoline(8);
        gCurTask->frame++;
        TaskYieldTrampoline(8);
        gCurTask->facing *= -1;
        sub_080ab7cc();
    }
    gCurTask->unk6C = 0;
    do
    {
        sub_080ab810();
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskStop();
    if (gUnk_08749224[gCurTask->unk28] == 2)
        ActorSetState(7);
    else
        ActorSetState(6);
    TaskSleepForever();
}

void sub_080aa970(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aa998(void)
{
    struct ActorSpawn sp;
    s32 d;

    gCurTask->updateState = 7;
    gUnk_02007D00[7] = 0;
    ActorStopAnim();
    TaskSetFrame(52);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    CreateChildTask(203, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->unk8C->savedTileWord | (128 << 4));
    CreateChildTask(204, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    PlaySfx(0x235);
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_087492C0);
    if (gUnk_02007D00[7] == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gUnk_02007D00[7] == 0);
    }
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_087492D4);
    PlaySfx(140 << 2);
    gCurTask->unk6C = 0;
    do
    {
        sp.subtype = 33;
        sp.taskType = 136;
        sp.spawnArg = 0;
        sp.x = 48;
        sp.y = 16;
        sp.tileWord = gCurTask->unk8C->savedTileWord;
        sp.checkTerrain = 1;
        d = (s16)gCurTask->health;
        if (d <= Div(gCurTask->unk1C, 3) && (gCurTask->unk6C & 1))
        {
            sp.variant = 1;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
            sp.variant = 2;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
        }
        else
        {
            sp.variant = 0;
            CreateActorFromDescAtOffsetFacing(&sp, 1);
        }
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskYieldTrampoline(31);
    gUnk_02007D00[7] = 2;
    ActorStopAnim();
    gCurTask->unk6C = 0;
    do
    {
        sub_080ab810();
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080aab4c(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 7)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aab80(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 6;
    ActorStopAnim();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(36);
    gCurTask->unk24 = 2;
    while (1)
    {
        if (--gCurTask->unk24 == 0)
        {
            if ((s16)++gCurTask->frame > 39)
                gCurTask->frame = 36;
            gCurTask->unk24 = 2;
        }
        TaskGetScreenPos();
        if (gUnk_030023D4 > 64)
            break;
        if (TaskGetYDirBitToNearestPlayer() != 1)
            break;
        if (abs(TaskGetNearestPlayerDy()) <= 7)
            break;
        TaskYieldTrampoline(1);
    }
    TaskStop();
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_087492AC);
    gUnk_03001F2C = (s16)gCurTask->health > Div(gCurTask->unk1C, 3);
    gUnk_02007D00[6] = gUnk_0874921C[gUnk_03001F2C];
    gUnk_02007D00[7] = gUnk_0874921E[gUnk_03001F2C];
    gUnk_02007D00[5] = 0;
    CreateChildTask(201, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->unk8C->savedTileWord | 0x800);
    CreateChildTask(202, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    gCurTask->unk20 = 0;
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gUnk_02007D00[7]; gCurTask->unk6C++)
    {
        TaskYieldTrampoline(gUnk_02007D00[6]);
        gCurTask->unk20 += RandomRange(4) + 1;
        if (gCurTask->unk20 > 4)
            gCurTask->unk20 -= 5;
        sp.subtype = 33;
        sp.taskType = 136;
        sp.variant = 3;
        sp.x = 0;
        sp.y = 8;
        sp.tileWord = gCurTask->unk8C->savedTileWord;
        sp.checkTerrain = 1;
        sp.spawnArg = gCurTask->unk20;
        CreateActorFromDescAtOffsetFacing(&sp, 1);
    }
    TaskYieldTrampoline(4);
    ActorStopAnim();
    gUnk_02007D00[5] = 1;
    gCurTask->unk6C = 0;
    do
    {
        sub_080ab810();
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080aad64(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 6)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aad98(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 5;
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749270);
    gCurTask->velY = 0;
    gCurTask->accelY = -0x2000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 128 << 6;
    TaskYieldTrampoline(16);
    TaskStopY();
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749298);
    CreateChildTask(207, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->unk8C->savedTileWord | (128 << 4));
    CreateChildTask(205, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->unk8C->savedTileWord | (128 << 4));
    CreateChildTask(206, gCurTask->pixelX, gCurTask->pixelY, 0xD310);
    sp.subtype = 33;
    sp.taskType = 136;
    sp.variant = 4;
    sp.tileWord = gCurTask->unk8C->savedTileWord;
    sp.checkTerrain = 1;
    if (gCurTask->unk30 & 1)
    {
        TaskYieldTrampoline(20);
        sp.spawnArg = 0;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 1;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 2;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 3;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 4;
        CreateActorFromDescHere(&sp, 1);
    }
    else
    {
        TaskYieldTrampoline(20);
        sp.spawnArg = 4;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 2;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 3;
        CreateActorFromDescHere(&sp, 1);
        TaskYieldTrampoline(16);
        sp.spawnArg = 0;
        CreateActorFromDescHere(&sp, 1);
        sp.spawnArg = 1;
        CreateActorFromDescHere(&sp, 1);
    }
    gCurTask->unk30++;
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080aaf38(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 5)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080aaf6c(void)
{
    s32 d;

    gCurTask->updateState = 8;
    gCurTask->unk24 = 1;
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_087492E8);
    d = (s16)gCurTask->health;
    if (d > Div(gCurTask->unk1C, 3))
    {
        TaskYieldTrampoline(60);
        gCurTask->velY = 160 << 8;
        gCurTask->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 9);
        gCurTask->velY = 0;
        gCurTask->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        gCurTask->velY = -0xA000;
        gCurTask->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 9);
    }
    else
    {
        gCurTask->accelY = -0x1900;
        if (gCurTask->pixelY - gViewRect[2] >= -40)
        {
            do
                TaskYieldTrampoline(1);
            while (gCurTask->pixelY - gViewRect[2] >= -40);
        }
        TaskStopY();
        ActorStopAnim();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(30);
        gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_0874930C);
        gUnk_02007D00[1] = 1;
        gCurTask->velY = 128 << 12;
        TaskYieldTrampoline(12);
        gCurTask->velY = 128 << 11;
        TaskYieldTrampoline(10);
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(5);
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(5);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(5);
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(5);
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(10);
        gCurTask->velY = -0x80000;
        TaskYieldTrampoline(6);
        gUnk_02007D00[1] = 0;
    }
    TaskStop();
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080ab158(void)
{
    s32 w;

    w = gCurTask->unk24 - 1;
    gCurTask->unk24 = w;
    if (w == 0)
    {
        PlaySfx(0x22E);
        gCurTask->unk24 = 6;
    }
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 8)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080ab1a8(void)
{
    gCurTask->updateState = 9;
    TaskYieldTrampoline(24);
    PlaySfx(0x231);
    TaskFaceNearestPlayer();
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = 144 << 9;
    TaskYieldTrampoline(24);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    ActorStopAnim();
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskSetFrame(56);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749284);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(24);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(8);
    ActorStopAnim();
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    TaskSetFrame(56);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->facing *= -1;
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    sub_080ab7cc();
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    gCurTask->unk6C = 0;
    do
    {
        sub_080ab810();
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080ab394(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 9)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080ab3c8(void)
{
    gCurTask->updateState = 10;
    gCurTask->unk6C = 0;
    do
    {
        sub_080ab4d0(5);
        sub_080ab670(1);
        sub_080ab728(1);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080ab418(void)
{
    if (gCurTask->state != 10)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080ab440(void)
{
    gCurTask->updateState = 11;
    sub_080ab5c0();
    gUnk_02007D00[0] = 0;
    ActorSetState(3);
    TaskSleepForever();
}

void sub_080ab46c(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
    if (gCurTask->state != 11)
        TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
}

void sub_080ab4a0(void)
{
    s32 n;

    n = gCurTask->unk28 + 1;
    gCurTask->unk28 = n;
    if (n == 10)
        gCurTask->unk28 = 0;
    sub_080ab4d0(gUnk_08749224[gCurTask->unk28]);
}

void sub_080ab4d0(s32 a)
{
    gCurTask->pixelX = gViewRect[0] + 128;
    if (gUnk_08749244[a] != 0)
        gCurTask->pixelX += RandomRange(gUnk_08749244[a] + 1) - (gUnk_08749244[a] >> 1);
    gCurTask->pixelY = gUnk_08749252[a] + gViewRect[2];
    if (gUnk_08749260[a] != 0)
        gCurTask->pixelY += RandomRange(gUnk_08749260[a] + 1) - (gUnk_08749260[a] >> 1);
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
}

void sub_080ab570(void)
{
    s32 v;

    if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
    {
        v = gCurTask->velX + (128 << 7);
        gCurTask->velX = v;
        if (v > 192 << 9)
            gCurTask->velX = 192 << 9;
    }
    else
    {
        v = gCurTask->velX - 0x4000;
        gCurTask->velX = v;
        if (v < -0x18000)
            gCurTask->velX = -0x18000;
    }
}

void sub_080ab5c0(void)
{
    TaskStop();
    if (gUnk_02007D00[1] == 0)
        gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749330);
    else
    {
        gCurTask->unk18 = ActorStartAnim((struct AnimCmd *)gUnk_08749344);
        gUnk_02007D00[1] = 0;
    }
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x4000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    gCurTask->velX = 0;
}

void sub_080ab670(s32 a)
{
    gCurTask->drawCallback = (u32)sub_08065438;
    if (a != 0)
        PlaySfx(141 << 2);
    TaskSetFrame(67);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->drawCallback = (u32)sub_080a9ed8;
    TaskSetFrame(50);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(10);
}

void sub_080ab728(s32 a)
{
    TaskStop();
    gCurTask->drawCallback = (u32)sub_08065438;
    if (a != 0)
        PlaySfx(0x233);
    TaskSetFrame(60);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
}

void sub_080ab7cc(void)
{
    TaskSetFrame(44);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
}

void sub_080ab810(void)
{
    TaskSetFrame(36);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
}

s32 sub_080ab854(void)
{
    gUnk_02007D00[0] |= 1;
    gCurTask->facing = TaskGetFacingToward(gCurTask->hitterPlayer);
    CreateStarFlash(1, 0, 0);
    ActorSetState(11);
    TaskSetEntry(NightmareWizardEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_080ab8a8(void)
{
    TaskStop();
    gUnk_02007FB8[0] = 0;
    gCurTask->facing = TaskGetFacingToward(gCurTask->hitterPlayer);
    ActorSetHitReactions((u32)gUnk_08749B60);
    gUnk_02007D00[0] |= 2;
    if (gGameState == 20)
    {
        sub_0800a698();
        sub_080b7cb4(gCurSaveSlot[0]);
    }
    if (gUnk_02007D00[1] != 0)
    {
        gCurTask->frameTable = gUnk_08754568;
        TaskFaceNearestPlayer();
        TaskSetFrame(0);
    }
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_080ab93c(void)
{
    gUnk_02007FB8[0] = 0;
    gCurTask->drawCallback = (u32)sub_080a9ed8;
    gCurTask->updateCallback = (u32)sub_080abcb4;
    gCurTask->frameTable = gNightmareWizardFrames;
    gCurTask->layer = 11;
    sub_080ab5c0();
    ActorStopAnim();
    sub_080ab728(0);
    sub_080ab4d0(6);
    TaskStop();
    sub_080ab670(0);
    gCurTask->drawCallback = (u32)sub_08065438;
    PlaySfx(143 << 2);
    RequestScreenShake(7);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 76;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 78;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 79;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    CreateChildTask(209, gCurTask->pixelX, gCurTask->pixelY,
                 gCurTask->unk8C->savedTileWord);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 80;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 81;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    RequestScreenShake(6);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 82;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 85;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 14);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 83;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 84;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    RequestScreenShake(5);
    gCurTask->frame = 86;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(2);
    gCurTask->frame = 87;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    gCurTask->frame = 88;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    RequestScreenShake(2);
    TaskYieldTrampoline(180);
    if (gGameState == 20)
        goto far;
    {
        RequestScreenShake(7);
        sub_080abcd0(gViewRect[0] + 80, gViewRect[2] + 136, 30);
        sub_080abcd0(gViewRect[0] + 224, gViewRect[2] + 136, 15);
        sub_080abcd0(gViewRect[0] + 208, gViewRect[2] + 136, 0);
        sub_080abcd0(gViewRect[0] + 48, gViewRect[2] + 136, 30);
        sub_080abcd0(gViewRect[0] + 192, gViewRect[2] + 136, 10);
        sub_080abcd0(gViewRect[0] + 112, gViewRect[2] + 136, 10);
        sub_080abcd0(gViewRect[0] + 32, gViewRect[2] + 136, 10);
        sub_080abcd0(gViewRect[0] + 216, gViewRect[2] + 136, 45);
        sub_080abcd0(gViewRect[0] + 80, gViewRect[2] + 136, 0);
        sub_080abcd0(gViewRect[0] + 200, gViewRect[2] + 136, 30);
        sub_080abcd0(gViewRect[0] + 224, gViewRect[2] + 136, 0);
        RequestScreenShake(0);
    }
    goto fin;
far:
    TaskYieldTrampoline(180);
fin:
    sub_08025b5c();
    TaskExitTrampoline();
}

void sub_080abcb4(void)
{
    gCurTask->unk18 = ActorTickAnim(gCurTask->unk18);
}

void sub_080abcd0(s32 x, s32 y, s32 d)
{
    PlaySfx(189);
    CreateChildTaskAt(148, (s16)x, (s16)y, 0);
    if (d != 0)
        TaskYieldTrampoline(d);
}

void sub_080abd04(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 10;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 16;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080abe38(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)sub_080abe7c;
    TaskFaceLikeParent();
    TaskSetFrame(9);
    TaskSleepForever();
}

void sub_080abe7c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0 && gUnk_02007D00[5] == 0)
        {
            t->posX = o->pixelX << 16;
            t->posY = o->pixelY << 16;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080abf10(void)
{
    s32 w;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)sub_080aa16c;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_08754708;
    gCurTask->updateCallback = (u32)sub_080abf94;
    TaskFaceLikeParent();
    gCurTask->unk28 = 2;
    TaskSetFrame(0);
    while (gUnk_02007D00[5] == 0)
    {
        w = gCurTask->unk28 - 1;
        gCurTask->unk28 = w;
        if (w == 0)
        {
            gCurTask->unk28 = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 3)
                gCurTask->frame = w;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_080abf94(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080ac020(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)sub_080ac08c;
    TaskFaceLikeParent();
    TaskSetFrame(10);
    TaskYieldTrampoline(85);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gUnk_02007D00[7] = 1;
    TaskSetFrame(12);
    TaskSleepForever();
}

void sub_080ac08c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            if (gUnk_02007D00[7] == 1)
            {
                t->posX = o->pixelX << 16;
                t->posY = o->pixelY << 16;
            }
            else if (gUnk_02007D00[7] == 2)
                TaskFree(gCurTaskIdx);
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080ac124(void)
{
    s32 w;
    s32 w2;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)sub_080aa16c;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_08754718;
    gCurTask->updateCallback = (u32)sub_080ac1f0;
    TaskFaceLikeParent();
    gCurTask->unk28 = 2;
    TaskSetFrame(0);
    while (gUnk_02007D00[7] == 0)
    {
        w = gCurTask->unk28 - 1;
        gCurTask->unk28 = w;
        if (w == 0)
        {
            gCurTask->unk28 = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 3)
                gCurTask->frame = w;
        }
        TaskYieldTrampoline(1);
    }
    gCurTask->unk28 = 2;
    TaskSetFrame(4);
    while (gUnk_02007D00[7] == 1)
    {
        w2 = gCurTask->unk28 - 1;
        gCurTask->unk28 = w2;
        if (w2 == 0)
        {
            gCurTask->unk28 = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 7)
                gCurTask->frame = 4;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_080ac1f0(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080ac27c(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 7;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)sub_080ac30c;
    TaskFaceLikeParent();
    TaskSetFrame(4);
    gCurTask->unk28 = 0;
    for (;;)
    {
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk28--;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk28++;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
    }
}

void sub_080ac30c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && o->state == 5 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY + t->unk28;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080ac3a4(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)sub_080ac30c;
    gCurTask->unk28 = 0;
    TaskFaceLikeParent();
    for (;;)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
}

void sub_080ac410(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)sub_080aa16c;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_087546F8;
    gCurTask->updateCallback = (u32)sub_080ac47c;
    TaskFaceLikeParent();
    for (;;)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
}

void sub_080ac47c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 8 && o->state == 5 && gUnk_02007D00[0] == 0)
        {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

void sub_080ac510(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_080ac530;
    TaskSleepForever();
}

void sub_080ac530(void)
{
    vs16 *arr;
    struct Task *t;
    struct Task *o;
    s16 i;
    struct Task *u;
    s32 v;
    s32 w;
    s32 wv0;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1)
    {
        o = &gTasks[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] != 2)
        {
            if (o->frame == -1)
                return;
            t->pixelX = o->pixelX;
            wv0 = *(u16 *)((u8 *)o + 74);
    t->pixelY = wv0;
            t->facing = o->facing;
            gCurTask->unk28 = gUnk_087494C8[o->frame];
            if (gCurTask->unk28 == -1)
                return;
            ActorCheckHitsWithBox((s32)gUnk_08749458[gCurTask->unk28]);
            u = gCurTask;
            v = u->hitKind;
            if (v == 6 && u->unk82 == 9)
            {
                o = &gTasks[u->hitterSlot];
                o->hitKind = v;
            }
            else
            {
                w = gUnk_08749490[gCurTask->unk28];
                if (w == 0)
                    return;
                ActorCheckHitsWithBox(w);
                u = gCurTask;
                v = u->hitKind;
                if (v == 6 && u->unk82 == 9)
                {
                    o = &gTasks[u->hitterSlot];
                    o->hitKind = v;
                }
            }
        }
        else
            TaskFree(gCurTaskIdx);
    }
    else
        TaskFree(gCurTaskIdx);
}

s32 sub_080ac678(void)
{
    TaskSetFrame(0);
}

void sub_080ac684(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_08753DA0;
    TaskFaceLikeParent();
    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        t->layer = 12;
        gCurTask->updateCallback = (u32)sub_080ac72c;
        ActorSetState(0);
        break;
    case 1:
        t->layer = 3;
        gCurTask->updateCallback = (u32)sub_080ac82c;
        ActorSetState(1);
        break;
    case 2:
        t->layer = 12;
        gCurTask->updateCallback = (u32)sub_080ac84c;
        ActorSetState(2);
        break;
    }
    CallTableEntry(gCurTask->state, 3, gUnk_08749B8C);
}

void sub_080ac72c(void)
{
    s32 d;
    s32 k;

    sub_0806956c();
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
    if (gCurTask->unk28 > 0)
        ActorCheckHits();
    if (gCurTask->hitKind == 7)
    {
        for (gCurTask->unk70 = 0; (s16)gCurTask->unk70 < gPlayerCount; gCurTask->unk70++)
        {
            d = gCurTask->hitterPlayer;
            k = (s16)gCurTask->unk70;
            if ((d >> k) & 1)
            {
                if (gPlayerStates[k].ability == 24)
                {
                    if (gTasks[k].variant > 6)
                    {
                        sub_08040858(k);
                        gUnk_02007D00[2]++;
                        break;
                    }
                }
                else if (!(gPlayerStates[k].unk42 & 2) && gPlayerStates[k].mode != 13
                         && gPlayerStates[k].mode != 10)
                {
                    sub_08040858(k);
                    gUnk_02007D00[2]++;
                    break;
                }
            }
        }
        if ((s16)gCurTask->unk70 != gPlayerCount)
            ActorDestroy();
    }
}

void sub_080ac82c(void)
{
    sub_0806956c();
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
}

void sub_080ac84c(void)
{
    CallTableEntry(gCurTask->updateState, 3, gUnk_08749B98);
}

void sub_080ac868(void)
{
    struct ActorSpawn sp;
    s32 w;
    u8 v74;

    gCurTask->updateState = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    gCurTask->unk28 = 0;
    gCurTask->onGround = 0;
    gCurTask->unk2C = 2;
    TaskSetFrame(10);
    while (gCurTask->onGround == 0)
    {
        TaskYieldTrampoline(1);
        w = gCurTask->unk2C - 1;
        gCurTask->unk2C = w;
        if (w == 0)
        {
            gCurTask->unk2C = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 11)
                gCurTask->frame = 4;
        }
    }
    TaskStop();
    TaskSetFrame(12);
    gCurTask->unk28 = 1;
    v74 = gCurTask->unk74;
    if (v74 == 0)
    {
        sp.subtype = 18;
        sp.taskType = 120;
        sp.variant = 2;
        sp.spawnArg = v74;
        sp.x = gViewRect[0] + 120;
        sp.y = gViewRect[2] + 72;
        sp.tileWord = (128 << 5) + gCurTask->tileWord;
        sp.checkTerrain = 0;
        CreateActorFromDesc(&sp, 1);
    }
    TaskSleepForever();
}

void sub_080ac94c(void)
{
}

void sub_080ac950(void)
{
    s32 w;

    gCurTask->updateState = 1;
    if (abs(TaskGetNearestPlayerDx()) <= 15)
    {
        TaskGetNearestPlayerScreenPos();
        if (gUnk_030023B4 <= 119)
            gCurTask->velX = 128 << 8;
        else
            gCurTask->velX = -0x8000;
    }
    TaskSetMotionY(-0x40000, 192 << 6, 192 << 10);
    gCurTask->onGround = 0;
    gCurTask->unk28 = 2;
    TaskSetFrame(22);
    while (gCurTask->onGround == 0)
    {
        TaskYieldTrampoline(1);
        w = gCurTask->unk28 - 1;
        gCurTask->unk28 = w;
        if (w == 0)
        {
            gCurTask->unk28 = 2;
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 23)
                gCurTask->frame = 16;
        }
        if (gCurTask->velY > 0)
            gCurTask->layer = 12;
    }
    TaskStop();
    TaskSetFrame(24);
    TaskSleepForever();
}

void sub_080aca38(void)
{
}

void sub_080aca3c(void)
{
    gCurTask->updateState = 2;
    gCurTask->frame = 35;
    TaskYieldTrampoline(216);
    TaskExitTrampoline();
}

void sub_080aca60(void)
{
    if ((s16)gTaskSlotTypes[gCurTask->parent] == -1)
        ActorDestroy();
}

void sub_080aca90(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0875447C;
    TaskFaceNearestPlayer();
    gUnk_02007D00[2]++;
    gCurTask->updateCallback = (u32)sub_080acaf0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08749BA4);
}

void sub_080acaf0(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_08749BA8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080acb20(void)
{
    s32 w;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(0, 148 << 10, 192 << 10);
    TaskSetMotionXFacing(192 << 8, 0x5A5A5A5A);
    gCurTask->unk28 = 6;
    TaskSetFrame(11);
    while (gCurTask->onGround == 0)
    {
        w = gCurTask->unk28 - 1;
        gCurTask->unk28 = w;
        if (w == 0)
        {
            gCurTask->frame++;
            if ((s16)gCurTask->frame > 13)
                gCurTask->frame = 11;
            gCurTask->unk28 = 6;
        }
        TaskYieldTrampoline(1);
    }
    TaskStopY();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(8);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(10);
        TaskSetFrame(9);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
    }
}

void sub_080acc18(void)
{
    vs16 *arr;
    s16 i;

    arr = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 6 && o->unk34 == 0)
            return;
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_080acc8c(void)
{
    gUnk_02007D00[2]--;
}

void sub_080acc9c(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 9;
    gCurTask->frameTable = gUnk_08754738;
    gCurTask->updateCallback = (u32)sub_080accf0;
    gCurTask->unk28 = 0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08749BE4);
}

void sub_080accf0(void)
{
    if ((u8)sub_0806951c() == 1)
        TaskSetEntry(ActorDie, gCurTaskIdx);
    else
    {
        CallTableEntry(gCurTask->updateState, 1, gUnk_08749BE8);
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_080acd38(void)
{
    struct Task *t;
    s32 w;
    s32 a2;
    struct Task *u;
    struct Task *u2;
    struct Task **c;
    struct Task **c2;
    struct Task **c3;

    gCurTask->updateState = 0;
    TaskFaceLikeParent();
    gCurTask->unk30 = ActorStartAnim((struct AnimCmd *)gUnk_08749BD0);
    gCurTask->onGround = 0;
    t = gCurTask;
    if (t->variant == 3)
    {
        a2 = gUnk_08749BB1[t->unk74] + (160 << 2);
        t->unk2C = (a2 - (t->facing << 7)) & 0x1FF;
        AngleToVector(t->unk2C, 128 << 3);
        u = gCurTask;
        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
        PlaySfx(0x22F);
    }
    else if (t->variant == 4)
    {
        AngleToVector(gUnk_08749BAC[t->unk74], 128 << 3);
        u = gCurTask;
        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
        PlaySfx(0x22F);
    }
    else
    {
        TaskSetMotionXFacing(gUnk_08749BB8[t->variant], 0x5A5A5A5A);
        u2 = gCurTask;
        u2->velY = gUnk_08749BC4[u2->variant];
    }
    TaskYieldTrampoline(255);
    c = &gCurTask;
    u = *c;
    c2 = c;
    u->unk34 = 255;
    c3 = c2;
    do
    {
        if (((*c3)->unk34 & 3) == 0)
            TaskAccelerateTowardNearestPlayer(154 << 7, 0x18100);
        TaskYieldTrampoline(1);
        u = *c2;
        w = u->unk34 - 1;
        u->unk34 = w;
    } while (w != 0);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_080ace60(void)
{
    gCurTask->unk30 = ActorTickAnim(gCurTask->unk30);
    if (gUnk_02007D00[0] == 2)
        gCurTask->unk28++;
    if (gCurTask->unk28 != 0)
        TaskSetEntry(ActorDie, gCurTaskIdx);
}

void Task_PaintRoller(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)sub_08065438;
    gCurTask->frameTable = gPaintRollerFrames;
    gCurTask->layer = 11;
    sub_08066088(0);
    sub_08063a00((u32)gUnk_0874B3A8);
    gCurTask->unk34 = 0;
    gUnk_02007D00[0] = 1;
    gUnk_02007D00[2] = 1;
    gUnk_02007D00[3] = 0x10001;
    gUnk_02007D00[4] = 0;
    gUnk_02007D00[5] = -1;
    gUnk_02007D00[9] = ActorComputeHealth();
    sub_080ad32c();
    ActorSetState(0);
    gCurTask->unk28 = 0;
    gCurTask->updateCallback = (u32)sub_080acf3c;
    sub_080664e0((struct AnimCmd *)gUnk_08749CEC);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080acf3c(void)
{
    sub_080ad0f8();
}

void sub_080acf48(void)
{
    gCurTask->unk28 = gCurTask->state;
    CallTableEntry(gCurTask->state, 3, gUnk_08749D10);
}

void sub_080acf68(void)
{
    gCurTask->updateCallback = (u32)sub_080ad0f8;
    gUnk_02007D00[5] = -1;
    sub_080ad32c();
    gUnk_02007D00[1] = 0;
    gCurTask->unk30 = 16;
    CreateDustTrail(1, 3, 8, 10);
    while (gCurTask->unk30 > 0)
        sub_080ad278();
    sub_080ad170();
    while (gCurTask->unk30 > 0)
        sub_080ad278();
    TaskStop();
    sub_080ad32c();
    if (sub_080ad37c() != 0)
    {
        gUnk_02007D00[5] = 2;
        sub_080ad08c();
    }
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080acffc(void)
{
    s32 w;

    gUnk_02007D00[5] = -1;
    gUnk_02007D00[1] = 7;
    gCurTask->unk30 = 10;
    while (gCurTask->unk30 > 0)
        sub_080ad278();
    if (sub_080ad37c() != 0)
        TaskYieldTrampoline(32);
    sub_080ad3e8();
    gUnk_02007D00[1] = 8;
    gCurTask->unk30 = 27;
    while (gCurTask->unk30 > 0)
        sub_080ad278();
    sub_080ad3a0();
    gUnk_02007D00[5] = 1;
    if (gCurTask->unk30 > 0)
    {
        do
        {
            sub_080ad08c();
            w = gCurTask->unk30 - 1;
            gCurTask->unk30 = w;
        } while (w > 0);
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080ad08c(void)
{
    TaskStop();
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(24);
    TaskYieldTrampoline(4);
    TaskSetFrame(26);
    TaskYieldTrampoline(11);
    TaskSetFrame(24);
    TaskYieldTrampoline(4);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    TaskSetFrame(18);
    TaskYieldTrampoline(11);
}

void sub_080ad0f8(void)
{
    struct Task *t;

    sub_08068f68();
    ActorReactToHit();
    t = gCurTask;
    if (t->unk28 != t->state)
        TaskSetEntry(sub_080acf48, gCurTaskIdx);
}

void sub_080ad128(void)
{
    CreateChildTaskHere(142, 0);
    sub_080ad458();
}

s32 sub_080ad13c(void)
{
    ActorSetHitReactions((u32)gUnk_0874B4EC);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_080ad160(void)
{
    sub_08066fc0(0, 128, 104);
}

void sub_080ad170(void)
{
    s32 v3;
    u8 v5;
    s32 w;
    s32 w2;
    s32 w3;

    v3 = gUnk_02007D00[0];
    v5 = gUnk_02007D00[0];
    if (gUnk_02007D00[4] > 1)
    {
        gUnk_02007D00[4] = 0;
        gUnk_030023D4 = RandomRange(2);
        v5 = v5 + 1;
        v5 &= 1;
        v5 = v5 + gUnk_030023D4 * 2;
        gUnk_030023D4 = gUnk_08749D1C[gUnk_02007D00[0] * 4 + v5];
    }
    else if (gUnk_02007D00[4] <= -2)
    {
        gUnk_02007D00[4] = 0;
        v5 += 2;
        v5 &= 3;
        gUnk_030023D4 = gUnk_08749D1C[v3 * 4 + v5];
    }
    else
    {
        w2 = RandomRange(3);
        w3 = v5 + 1;
        v5 = w3 + w2;
        v5 &= 3;
        gUnk_030023D4 = gUnk_08749D1C[gUnk_02007D00[0] * 4 + v5];
        w = gUnk_08749D38[gUnk_030023D4];
        if (gUnk_02007D00[4] != w)
            gUnk_02007D00[4] = w;
        else
            gUnk_02007D00[4] <<= 1;
    }
    gUnk_02007D00[0] = v5;
    gUnk_02007D00[1] = gUnk_08749D2C[gUnk_030023D4];
    gCurTask->unk30 = gUnk_08749D44[gUnk_02007D00[1]];
}

void sub_080ad278(void)
{
    s32 w;

    w = gCurTask->unk30 - 1;
    gCurTask->unk30 = w;
    gUnk_030023D4 = gUnk_08749D4C[gUnk_02007D00[1]];
    TaskSetFrame(*(s16 *)(gUnk_030023D4 + w * 2));
    gUnk_030023D4 = gUnk_08749D70[gUnk_02007D00[1]];
    TaskSetMotionXFacing(*(s32 *)(gUnk_030023D4 + gCurTask->unk30 * 4), 0x5A5A5A5A);
    gUnk_030023D4 = gUnk_08749D94[gUnk_02007D00[1]];
    gCurTask->velY = *(s32 *)(gUnk_030023D4 + gCurTask->unk30 * 4);
    gUnk_030023D4 = gUnk_08749DB8[gUnk_02007D00[1]];
    PlaySfx(*(s32 *)(gUnk_030023D4 + gCurTask->unk30 * 4));
    gUnk_030023D4 = gUnk_08749DDC[gUnk_02007D00[1]];
    TaskYieldTrampoline(*(u8 *)(gUnk_030023D4 + gCurTask->unk30));
}

void sub_080ad32c(void)
{
    struct Task *t;

    gCurTask->facing = gUnk_0874AAD0[gUnk_02007D00[0]];
    t = gCurTask;
    t->posX = gUnk_0874AAD4[gUnk_02007D00[0]] << 16;
    t->posY = gUnk_0874AADC[gUnk_02007D00[0]] << 16;
}

s32 sub_080ad37c(void)
{
    s32 r = 0;

    if ((s16)gCurTask->health >= gUnk_02007D00[9] >> 1)
        r = 1;
    return r;
}

void sub_080ad3a0(void)
{
    s32 r;

    r = sub_080ad37c();
    if (r != 0)
        gUnk_030023D4 = 2;
    else
        gUnk_030023D4 = r;
    gUnk_02007D00[2] = (gUnk_02007D00[2] + 1) & 1;
    gCurTask->unk30 = gUnk_02007D00[2] + gUnk_030023D4;
}

void sub_080ad3e8(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    sp.subtype = 14;
    sp.taskType = 116;
    sp.variant = t->variant;
    sp.spawnArg = t->unk74;
    sp.x = gUnk_0874AAE4[gUnk_02007D00[0]];
    sp.y = gUnk_0874AAEC[gUnk_02007D00[0]];
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 0;
    gCurTask->unk46 = CreateActorFromDesc(&sp, 1);
}

void sub_080ad458(void)
{
    sub_0806619c(13, (u32)sub_080ad47c, (u32)gUnk_082DFFA8, 32, 1);
}

void sub_080ad47c(void)
{
    if (gUnk_02006190[3] == 0)
    {
        sub_0806621c();
        if (gUnk_02007D00[5] >= 0)
        {
            ActorSetState((u16)gUnk_02007D00[5]);
            TaskSetEntry(sub_080acf48, gCurTaskIdx);
        }
    }
}

void Task_HeavyMole(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    gCurTask->moveCallback = (u32)sub_080ad650;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->frameTable = gHeavyMoleFrames;
    gCurTask->layer = 10;
    sub_08066088(0);
    gCurTask->tileWord |= 128 << 4;
    sub_08066144();
    gCurTask->facing = 1;
    sub_08063a00((u32)gUnk_0874B3FC);
    t = gCurTask;
    t->unk1C = gCameraAnchorX << 16;
    t->posX = 176 << 16;
    t->pixelX = gCameraAnchorX + 56;
    t->posY = gCameraAnchorY << 16;
    t->pixelY = t->posY >> 16;
    t->unk28 = 120;
    t->unk2C = 120;
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = 0;
    gUnk_02007D00[2] = 128 << 10;
    gUnk_02007D00[3] = 0;
    gUnk_02007D00[4] = 0;
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 240;
    gUnk_02007D00[7] = 3;
    gUnk_02007D00[9] = ActorComputeHealth();
    sp.subtype = 19;
    sp.taskType = 121;
    sp.variant = gCurTask->variant;
    sp.spawnArg = gCurTask->unk74;
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 0);
    sp.subtype = 19;
    sp.taskType = 122;
    sp.variant = gCurTask->variant;
    sp.spawnArg = gCurTask->unk74;
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 0);
    CreateChildTaskHere(190, 1);
    CreateChildTaskHere(189, 1);
    gUnk_02007D00[8] = CreateChildTaskHere(191, 1);
    CreateChildTaskHere(192, 1);
    gCurTask->unk8C->unk3C = (u32)sub_080ae380;
    sub_08066544();
    gCurTask->updateCallback = (u32)sub_080ad710;
    for (;;)
    {
        gCurTask->frame = 4;
        sub_080ad630();
        gCurTask->frame = 5;
        sub_080ad630();
        gCurTask->frame = 6;
        sub_080ad630();
        gCurTask->frame = 7;
        sub_080ad630();
    }
}

void sub_080ad630(void)
{
    u8 *tb = gUnk_0874AAF4;
    s16 *p = (s16 *)gUnk_02007D00;

    TaskYieldTrampoline(tb[p[5]]);
}

void sub_080ad650(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    s16 *sp5;
    s32 v;
    s32 w;
    s32 v2;
    s32 w2;

    if (gUnk_02006190[3] <= 0)
        TaskIntegrateMotion();
    else
        gUnk_030023D4 = (s16)gCurTask->health;
    ta = gCurTask;
    v = ta->posY >> 16;
    sp5 = gRoomBounds;
    w = sp5[2] + 90;
    if (v < w)
        ta->posY = (w << 16) + (128 << 8);
    tb = gCurTask;
    v2 = tb->posY >> 16;
    w2 = sp5[3] - 90;
    if (v2 >= w2)
        tb->posY = (w2 << 16) + -0x8000;
    t = gCurTask;
    t->unk1C = (u16)t->unk1C + (gCameraAnchorX << 16) + t->unk18;
    gCameraAnchorX = t->unk1C >> 16;
    gCameraAnchorY = t->posY >> 16;
    t->pixelX = gCameraAnchorX + (t->posX >> 16) - 120;
    t->pixelY = t->posY >> 16;
    gUnk_02006190[0] = t->pixelX;
    gUnk_02006190[1] = t->pixelY;
}

void sub_080ad710(void)
{
    s32 w;
    s32 w2;

    if (gUnk_0200AFF8 != 0)
        sub_08066564();
    sub_08068f68();
    ActorReactToHit();
    TaskBreakBlocksNoPlayer((u32)gUnk_0874B538);
    w = gCurTask->unk28 - 1;
    gCurTask->unk28 = w;
    if (w <= 0)
        sub_080ad7f0();
    w2 = gCurTask->unk2C - 1;
    gCurTask->unk2C = w2;
    if (w2 == 0)
    {
        sub_080ad788();
        gCurTask->unk2C = 120;
    }
    if (gUnk_02007D00[6] > 0)
    {
        gUnk_02007D00[6]--;
        if (gUnk_02007D00[6] <= 0)
        {
            gUnk_02007D00[5]--;
            if (gUnk_02007D00[5] < 0)
                gUnk_02007D00[5] = 2;
        }
    }
}

void sub_080ad788(void)
{
    gUnk_030023D4 = gUnk_02007D00[2] >> 16;
    if (gCurTask->velX > 0)
    {
        gUnk_030023D4++;
        if (gUnk_030023D4 > 2)
            gUnk_030023D4 = 2;
    }
    else if (gCurTask->velX < 0)
    {
        gUnk_030023D4--;
        if (gUnk_030023D4 < 0)
            gUnk_030023D4 = 0;
    }
    else if (gUnk_030023D4 != 1)
        gUnk_030023D4 = 1;
    else if ((gUnk_02007D00[2] & 255) == 0)
        gUnk_030023D4 = 2;
    else
        gUnk_030023D4 = 0;
    gUnk_02007D00[2] = ((s16 *)gUnk_02007D00)[5] + (gUnk_030023D4 << 16);
}

void sub_080ad7f0(void)
{
    struct Task *t;
    struct Task **c8;
    s32 d;
    s32 va;
    s32 r;
    struct Task *u2;

    gUnk_030023D4 = gUnk_0874AAF7[gUnk_02007D00[1]];
    gCurTask->unk28 = gUnk_0874AB26[gUnk_030023D4];
    gUnk_030023B4 = gUnk_0874AB50[gUnk_030023D4] + gUnk_02007D00[3];
    gCurTask->velX = gUnk_0874ABA0[gUnk_030023B4];
    gCurTask->velY = gUnk_0874AC24[gUnk_030023B4];
    switch (gUnk_030023D4)
    {
    case 0:
        sub_080ad9dc();
        c8 = &gCurTask;
        t = *c8;
        t->unk18 = gUnk_0874ACBC[gUnk_02007D00[3]];
        gUnk_030023D4 = va = gUnk_0874ACA8[gUnk_02007D00[3]];
        gUnk_030023B4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        d = (gUnk_030023B4 << 16) - t->posX;
        if (d < 0)
            d = t->posX - (gUnk_030023B4 << 16);
        r = Div(d, va);
        t = *c8;
        t->unk28 = r;
        if (t->posX >> 16 > gUnk_030023B4)
            gUnk_030023D4 = -gUnk_030023D4;
        t->velX = gUnk_030023D4;
        gUnk_02007D00[1]++;
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 1:
        TaskStop();
        gUnk_030023D4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        gCurTask->posX = (gUnk_030023D4 << 16) + (128 << 8);
        sub_080ada20();
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 4:
    case 5:
        TaskStop();
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        gUnk_02007D00[1]++;
        break;
    case 2:
    case 3:
        if (gUnk_030023D4 == 3)
            gUnk_030023B4 = 1;
        else
            gUnk_030023B4 = 0;
        gUnk_030023D4 = sub_080adaf8(gUnk_030023B4);
        switch (gUnk_030023D4)
        {
        case 1:
            break;
        case 0:
        case 3:
            if (RandomRange(2) == 0)
                break;
        case 2:
            gCurTask->velY = -gCurTask->velY;
            break;
        }
        gUnk_02007D00[1]++;
        break;
    default:
        gUnk_02007D00[1]++;
        break;
    }
}

void sub_080ad9dc(void)
{
    if (gCurTask->health < gUnk_02007D00[9] >> 1)
        gUnk_02007D00[3] = 2;
    else if (gCurTask->health < Div(gUnk_02007D00[9] * 5, 6) + 1)
        gUnk_02007D00[3] = 1;
    else
        gUnk_02007D00[3] = 0;
}

void sub_080ada20(void)
{
    s16 v;

    gUnk_030023D4 = 255 - gUnk_0874ACD4[gUnk_02007D00[3] * 4 + (gUnk_02007D00[0] & 255)];
    gUnk_030023B4 = RandomRange(gUnk_030023D4);
    gUnk_030023D4 = 0;
    gCurTask->unk6C = 0;
    while (1)
    {
        if ((s16)gCurTask->unk6C != gUnk_02007D00[0])
        {
            gUnk_030023D4 += gUnk_0874ACD4[gUnk_02007D00[3] * 4 + (s16)gCurTask->unk6C];
            if (gUnk_030023D4 >= gUnk_030023B4)
                break;
            if ((s16)gCurTask->unk6C == 3)
                break;
        }
        gCurTask->unk6C++;
    }
    v = gCurTask->unk6C;
    gUnk_030023D4 = v;
    gUnk_02007D00[0] = v;
    gUnk_02007D00[1] = gUnk_0874ACE4[(gUnk_030023B4 & gUnk_0874ACE0[v]) + v * 2];
}

s32 sub_080adaf8(s32 arg)
{
    s32 *pd;
    u16 *a;
    s16 *p5;
    u16 *tt;
    s32 v4;

    pd = &gUnk_030023D4;
    *pd = 0;
    v4 = gCurTask->posY >> 16;
    p5 = gRoomBounds;
    tt = gUnk_0874ACEE;
    a = &tt[gUnk_02007D00[3] * 2 + arg];
    if (v4 < p5[2] + a[0])
        *pd = 1;
    if (v4 > p5[3] - a[0])
        *pd |= 2;
    return *pd;
}

void sub_080adb58(void)
{
    CreateChildTaskHere(142, 0);
    sub_0806619c(23, (u32)sub_080adb90, (u32)gUnk_082F65D4, 32, 0);
    TaskSetSkipMask(4, gCurTaskIdx);
}

void sub_080adb90(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 *p;

    TaskBreakBlocksNoPlayer(gUnk_0874B538);
    c = &gCurTask;
    t = *c;
    t->unk2C--;
    if (t->unk2C == 0)
    {
        sub_080ad788();
        u = *c;
        u->unk2C = 120;
    }
    p = gUnk_02007D00;
    if (p[6] > 0)
    {
        p[6]--;
        if (p[6] <= 0)
        {
            p[5]--;
            if (p[5] < 0)
                p[5] = 2;
        }
    }
    if (gUnk_02006190[3] == 0)
        sub_0806621c();
}

s32 sub_080adbf0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *tb;

    t = gCurTask;
    t->posX = t->pixelX << 16;
    ActorSetHitReactions((u32)gUnk_0874B504);
    tb = gTasks;
    u = &tb[gUnk_02007D00[8]];
    u->frame = 13;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_080adc44(void)
{
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(150);
    if (gGameState == 8 && gUnk_03001F30 == 0)
    {
        if (sub_08066394() != 0)
        {
            sub_080670ac(15);
            sub_08025acc();
        }
    }
    else
    {
        if (sub_08066394() != 0)
        {
            sub_080670ac(15);
            ExitClearedStage();
        }
    }
    TaskSleepForever();
}

void sub_080adca4(void)
{
    struct Task **c;
    s32 *p;
    s32 *p2;
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 9;
    u = gCurTask;
    u->lateUpdateCallback = (u32)sub_080add48;
    c = &gCurTask;
    p = gUnk_02007D00;
top:
    (*c)->frame = 14;
    if (p[7] > 0)
    {
        p2 = gUnk_02007D00;
        do
        {
            TaskYieldTrampoline(1);
        } while (p2[7] > 0);
    }
    PlaySfx(131 << 2);
    (*c)->frame++;
    TaskYieldTrampoline(4);
    (*c)->frame++;
    TaskYieldTrampoline(48);
    sub_080addf8();
    TaskYieldTrampoline(64);
    (*c)->frame--;
    TaskYieldTrampoline(4);
    (*c)->frame--;
    TaskYieldTrampoline(2);
    p[7] = 3;
    goto top;
}

void sub_080add48(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
        TaskSetEntry(sub_080ade98, gCurTaskIdx);
    else if (gUnk_02006190[3] != 0)
        TaskSetSkipMask(1, gCurTaskIdx);
    else
        TaskSetSkipMask(0, gCurTaskIdx);
}

void sub_080addf8(void)
{
    struct ActorSpawn sp;
    s32 n;

    PlaySfx(0x20B);
    if (gUnk_02007D00[4] == 0)
        n = RandomRange(4);
    else if (gUnk_02007D00[4] == 1)
    {
        n = 1;
        gUnk_02007D00[4] = 2;
    }
    else
        n = RandomRange(3);
    if (n == 0)
    {
        gUnk_02007D00[4] = 1;
        sp.subtype = 21;
        sp.taskType = 124;
        sp.variant = gCurTask->variant;
        sp.spawnArg = gCurTask->unk74;
        sp.checkTerrain = 1;
        CreateActorFromDescHere(&sp, 0);
    }
    else
    {
        sp.subtype = 20;
        sp.taskType = 123;
        sp.variant = gCurTask->variant;
        sp.spawnArg = gCurTask->unk74;
        CreateActorFromDescHere(&sp, 0);
    }
}

void sub_080ade98(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080adebc;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080adebc(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->parent].frame == -1)
        t->drawCallback = 0;
    else
        t->drawCallback = (u32)TaskDrawWorld;
}

void sub_080adefc(void)
{
    struct Task **c;
    s32 k;
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 7;
    u = gCurTask;
    u->lateUpdateCallback = (u32)sub_080adf50;
    c = &gCurTask;
    k = 8;
top:
    (*c)->frame = k;
    sub_080ad630();
    (*c)->frame = 9;
    sub_080ad630();
    (*c)->frame = 10;
    sub_080ad630();
    goto top;
}

void sub_080adf50(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
        TaskSetEntry(sub_080adfd4, gCurTaskIdx);
}

void sub_080adfd4(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080adff8;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080adff8(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->parent].frame == -1)
        t->drawCallback = 0;
    else
        t->drawCallback = (u32)TaskDrawWorld;
}

void sub_080ae038(void)
{
    struct Task **c;
    s32 k;
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    s32 w;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gHeavyMoleFrames;
    t->layer = 9;
    u = gCurTask;
    u->lateUpdateCallback = (u32)sub_080ae0b4;
    u->unk28 = 96;
    c = &gCurTask;
    k = 11;
top:
    t2 = *c;
    w = t2->unk28;
    if (w >= 0)
    {
        t2->unk28 = w - 1;
        t2->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        goto top;
    }
    t2->frame = k;
    TaskYieldTrampoline(4);
    u2 = *c;
    u2->frame = 12;
    TaskYieldTrampoline(4);
    u3 = *c;
    u3->frame = k;
    TaskYieldTrampoline(4);
    u4 = *c;
    u4->unk28 = 96;
    goto top;
}

void sub_080ae0b4(void)
{
    struct Task *t;
    s16 *a;
    s32 w;
    s32 i;
    s32 k1;
    s32 k2;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gCurTask;
    a = &t->parent;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    w = tt->pixelX << 16;
    t->posX = w;
    t->pixelX = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    w2 = tt2->pixelY << 16;
    t->posY = w2;
    t->pixelY = w2 >> 16;
    if (gUnk_0200D120[*a - 32].hitState == 2)
    {
        k1 = 13;
        t->frame = k1;
        TaskSetEntry(sub_080ae174, gCurTaskIdx);
    }
    else if (gUnk_02006190[3] != 0)
    {
        k2 = 13;
        t->frame = k2;
        TaskSetSkipMask(1, gCurTaskIdx);
    }
    else
        TaskSetSkipMask(0, gCurTaskIdx);
}

void sub_080ae174(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080ae198;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080ae198(void)
{
    vs16 *arr;
    struct Task *t;
    s32 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((u16)arr[i] != 62)
        ActorDestroy();
    else
    {
        if (gTasks[i].frame == -1)
            t->drawCallback = 0;
        else
            t->drawCallback = (u32)TaskDrawWorld;
    }
}

void sub_080ae1f0(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->frameTable = gHeavyMoleFrames;
    gCurTask->layer = 7;
    gCurTask->lateUpdateCallback = (u32)sub_080ae37c;
    while (1)
    {
        TaskStop();
        gCurTask->frame = 0xFFFF;
        while (gUnk_02007D00[7] <= 0)
            TaskYieldTrampoline(1);
        while (gUnk_02007D00[6] > 0)
            TaskYieldTrampoline(1);
        gCurTask->unk2C = 20;
        while (gUnk_02007D00[7] > 0)
        {
            if (gUnk_02007D00[5] == 0)
                gUnk_030023D4 = (gCurTask->unk2C & 1) + 1;
            else
                gUnk_030023D4 = gUnk_02007D00[5];
            gCurTask->unk28 = gUnk_030023D4 >> 1;
            gCurTask->posX = (gTasks[gCurTask->parent].pixelX - 24) << 16;
            gCurTask->posY = (gTasks[gCurTask->parent].pixelY
                                    + gUnk_0874ACFA[gCurTask->unk28]) << 16;
            gCurTask->unk30 = 3;
            do
            {
                gCurTask->unk30--;
                gCurTask->frame = gUnk_0874ACFE[gCurTask->unk30 * 2 + gCurTask->unk28];
                gCurTask->velX = gUnk_0874AD0C[gCurTask->unk30];
                gCurTask->velY = gUnk_0874AD18[gCurTask->unk30 * 2 + gCurTask->unk28];
                TaskYieldTrampoline(gUnk_0874AD30[gCurTask->unk30]);
            } while (gCurTask->unk30 > 0);
            if (--gCurTask->unk2C <= 0)
                break;
        }
        gUnk_02007D00[6] = 240;
    }
}

void sub_080ae37c(void)
{
}

s32 sub_080ae380(s32 i)
{
    s32 v;

    v = (s16)(u16)gTaskSlotTypes[i];
    switch (v)
    {
    case 121 ... 122:
        return 1;
    case 62:
        return 1;
    case 123 ... 124:
        return 1;
    case 189 ... 191:
        return 1;
    case 192:
        return 1;
    default:
        return 0;
    }
}
