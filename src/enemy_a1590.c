#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u32 gUnk_0200000C[];
extern u32 gUnk_02000034[];
extern u32 gUnk_02004C90;
extern u32 gUnk_02005580[];
extern s16 gUnk_02005588[];
extern u32 gUnk_02005590[];
extern u32 gUnk_020055D0[];
extern u32 gUnk_020055D8[];
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
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[0x1A];
    /*0x20*/ u16 unk20;
    /*0x22*/ u8 filler22[0x26];
    /*0x48*/ s8 *unk48;
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
extern u32 gUnk_03000FA4;
extern u32 gHBlankDmaDest[];
extern u32 gHBlankDmaCnt[];
extern u8 gObjPalette[];
extern u32 gUnk_03001570[];
extern u32 gUnk_030015B0[];
extern u16 gFrameCount;
extern vu8 gBgMosaic;
extern vu16 gBg3Cnt;
extern u32 gHBlankDmaSrc[];
extern s16 gUnk_03001F00;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern struct PlayerState gPlayerStates[];
extern u8 gUnk_03002340;
extern s32 gUnk_03002344;
extern s16 gSpriteCameraX;
extern u8 gUnk_03002350;
extern u16 gLocalPlayer;
extern s8 gUnk_0300238C;
extern s16 gUnk_03002398;
extern u16 gPlayerCount;
extern u8 gUnk_030023B0;
extern s32 gUnk_030023B4;
extern u32 gUnk_030023C8[];
extern s32 gUnk_030023D4;
extern u16 gGameState;
extern s16 gSpriteCameraY;
extern u32 gCurSaveSlot[];
extern u32 gUnk_030023EC[];
extern u32 gUnk_03002448;
extern s16 gUnk_0300244C;
extern u32 gExtraMode[];
extern u32 gUnk_03002468[];
extern u32 gUnk_030027A8[];
extern vs16 gTaskSlotTypes[];
extern u8 gTerrainResult[];
extern u32 gUnk_03005568[];
extern s16 gUnk_03005628[];
extern u32 gUnk_03005680[];
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
extern u32 *gUnk_0873F0E4[];
extern u32 gUnk_0873F104[];
extern u32 *gUnk_0873F15C[];
extern u32 gUnk_0873F180[];
extern s16 gUnk_0873FF98[];
extern struct AnimCmd gUnk_087484A4[];
extern u32 gUnk_087484D4[];
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
extern u32 gUnk_08748EB8[];
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
extern u32 gUnk_08749150[];
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
extern u32 gUnk_087493F4[];
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
extern u32 gUnk_0874C12C[];
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
extern u32 gUnk_0874C260[];
extern u32 gUnk_0874C2A6[];
extern u32 gUnk_0874C2EC[];
extern u32 gUnk_0874C332[];
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
extern u32 gUnk_08753994[];
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
extern u32 gUnk_08753BB4[];
extern u32 gUnk_08753DA0[];
extern u32 gUnk_08753E3C[];
extern u32 gUnk_08753E8C[];
extern u32 gUnk_087540AC[];
extern u32 gUnk_087540EC[];
extern u32 gUnk_0875412C[];
extern u32 gUnk_08754180[];
extern u32 gUnk_087541B0[];
extern u32 gUnk_087541E0[];
extern u32 gUnk_08754210[];
extern u32 gUnk_08754260[];
extern u32 gUnk_08754280[];
extern u32 gUnk_08754290[];
extern u32 gUnk_087542A8[];
extern u32 gUnk_087542C0[];
extern u32 gUnk_08754308[];
extern u32 gUnk_08754358[];
extern u32 gUnk_087543A0[];
extern u32 gUnk_087543E8[];
extern u32 gUnk_08754418[];
extern u32 gUnk_08754448[];
extern u32 gUnk_0875447C[];
extern u32 gUnk_087544B4[];
extern u32 gUnk_08754504[];
extern u32 gUnk_08754560[];
extern u32 gUnk_08754568[];
extern u32 gUnk_0875456C[];
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
extern s32 sub_08009ee8(s32 a, s32 b);
extern void sub_08009fcc();
extern void sub_0800a280(void);
extern void sub_0800a294();
extern void sub_0800a4c0();
extern void sub_0800a554(void);
extern void sub_0800a698(void);
extern s32 sub_08021bb4(s16 x, s16 y, s32 c, s32 d);
extern void TaskInitWaterFlags(void);
extern u8 ClampTaskToRoom(struct Task *t);
extern u32 sub_0802294c();
extern void sub_080258e0();
extern void sub_08025a30();
extern void sub_08025acc();
extern void sub_08025b5c();
extern void sub_080261d4(u32 a);
extern void sub_080275cc();
extern void sub_0802cda0();
extern s32 sub_0802d344();
extern void sub_0802ffe8();
extern void sub_080308e8();
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
extern void sub_0806406c(void);
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
extern s32 sub_08064758(u16 lo, u16 hi);
extern s32 sub_080647fc(u16 lo, u16 hi);
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
extern void sub_080657a4(void);
extern void sub_080657cc(u32 v);
extern void sub_080657f8(u32 a, u32 b);
extern void sub_08065dbc(u32 slot, u32 sub);
extern void sub_08065dd0(u32 slot, u32 i);
extern void sub_08065dfc(u32 slot);
extern u8 TaskHasSameSerial(u32 i);
extern s16 sub_08065f5c(void);
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
extern u32 sub_0806a25c(void);
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
void sub_080a19cc();
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
void sub_080a55ec();
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
void sub_080a7d2c();
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
void sub_080aa288();
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
void sub_080acea8();
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
void sub_080ad4b8();
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
void sub_080ae3bc();
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
void sub_080b0840();
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
void sub_080b2a00();
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
void sub_080b60e8();
extern void CpuSet(const void *src, void *dst, u32 control);

/* sub_080a1590 (0x080A1590-0x080A15F0) */
void sub_080a1590(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(9);
    t = gCurTask;
    t->unk60 = 160 << 6;
    t->unk68 = 160 << 11;
    if (!(t->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a15f0 (0x080A15F0-0x080A1618) */
void sub_080a15f0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080a1570, gCurTaskIdx);
}

/* sub_080a1618 (0x080A1618-0x080A1624) */
s32 sub_080a1618(void)
{
    TaskSetFrame(0);
}

/* sub_080a1624 (0x080A1624-0x080A1668) */
void sub_080a1624(void)
{
    s32 i;
    struct PlayerState *p;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
            gPlayerStates[i].unk42 = 64;
    }
}

/* sub_080a1668 (0x080A1668-0x080A1704) */
void sub_080a1668(s32 a)
{
    struct Task *t = &gTasks[a];

    if (gCurTask->unk43 == -1)
    {
        if (t->unk48 <= gUnk_03005628[0] - 95)
            t->unk43 = 255;
        else
            t->unk43 = 1;
    }
    else
    {
        if (t->unk48 <= gUnk_03005628[1] + 95)
            t->unk43 = 255;
        else
            t->unk43 = 1;
    }
    gPlayerStates[a].unk01 = 18;
    gPlayerStates[a].unk42 = 64;
    gUnk_02007D00[a] = -1;
}

/* sub_080a1704 (0x080A1704-0x080A1740) */
void sub_080a1704(s32 a)
{
    if (TaskGetFacingToward(a) == gCurTask->unk43)
        sub_080a1740(a);
    else
        gUnk_02007D00[a] = -1;
}

/* sub_080a1740 (0x080A1740-0x080A1790) */
void sub_080a1740(s32 a)
{
    if (abs(TaskGetDxTo(a)) <= 48)
        sub_080a1668(a);
    else if (gCurTask->unk54 == 0)
        gUnk_02007D00[a] = -1;
}

/* sub_080a1790 (0x080A1790-0x080A180C) */
void sub_080a1790(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            switch (gUnk_02007D00[i])
            {
            case 1:
                sub_080a1704(i);
                break;
            case 0:
                if (gPlayerStates[i].unk04 <= 2)
                    sub_080a1740(i);
                break;
            default:
                if (gPlayerStates[i].unk04 <= 2)
                    gUnk_02007D00[i + 3]--;
                break;
            }
        }
    }
}

/* sub_080a180c (0x080A180C-0x080A1864) */
void sub_080a180c(void)
{
    s32 i;
    s32 sum;

    i = 0;
    sum = 0;
    for (; i < gPlayerCount; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
            sum += gUnk_02007D00[i + 3];
    }
    if (sum == 0)
    {
        gCurTask->unk1C = 1;
        sub_080a1624();
    }
}

/* sub_080a1864 (0x080A1864-0x080A18D4) */
void sub_080a1864(void)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            if (gPlayerStates[i].unk04 <= 2)
                gUnk_02007D00[i] = 1;
            else
                gUnk_02007D00[i] = 0;
            gUnk_02007D00[i + 3] = 8;
        }
    }
    t = gCurTask;
    t->unk1C = 0;
    t->unk20 = 120;
    t->unk28 = 0;
    t->unk2C = 0;
}

/* sub_080a18d4 (0x080A18D4-0x080A1980) */
void sub_080a18d4(void)
{
    s32 i;
    s32 v;

    gCurTask->unk15 = 1;
    TaskStop();
    sub_080a1864();
    TaskYieldTrampoline(16);
    sub_080261d4(4);
    gCurTask->unk2C = 1;
    TaskSetFrame(10);
    v = 128 << 10;
    for (i = 2; i >= 0; i--)
    {
        TaskSetMotionXFacing(v, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        v >>= 1;
    }
    TaskStop();
    gCurTask->unk24 = ActorStartAnim(gUnk_087484A4);
    gCurTask->unk28 = 1;
    while (gCurTask->unk1C == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk2C = 0;
    if (gCurTask->unk1C != 2)
        TaskYieldTrampoline(150);
    TaskYieldTrampoline(24);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = 0;
    sub_08025a30();
    TaskSleepForever();
}

/* sub_080a1980 (0x080A1980-0x080A19CC) */
void sub_080a1980(void)
{
    if (gCurTask->unk28 != 0)
        gCurTask->unk24 = ActorTickAnim(gCurTask->unk24);
    if (gCurTask->unk1C == 0)
    {
        if (gCurTask->unk20 <= 0)
        {
            gCurTask->unk1C = 2;
            sub_080a1624();
        }
        else
        {
            gCurTask->unk20--;
            if (gCurTask->unk2C != 0)
            {
                sub_080a1790();
                sub_080a180c();
            }
        }
    }
}

/* sub_080a19cc (0x080A19CC-0x080A19EC) */
void sub_080a19cc(void)
{
    CallTableEntry(gCurTask->unk73, 4, gUnk_087484D4);
}

/* sub_080a19ec (0x080A19EC-0x080A1AD0) */
void sub_080a19ec(void)
{
    struct Task *tb;
    struct Task *ta;
    struct Actor *ab;
    struct Actor *aa;

    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    ab = tb->unk8C;
    aa = ta->unk8C;
    ActorSetAttackBoxSlot(gUnk_02007D00[1], ab->unk60->unk04);
    ActorSetAttackBoxSlot(gUnk_02007D00[0], aa->unk60->unk04);
    gUnk_0200AFF8 = 0;
    sub_0800a280();
    sub_0800a294(tb->unk78 * 2, tb->unk78);
    while (gUnk_0200AFF8 == 0)
        TaskYieldTrampoline(1);
    gUnk_0200AFF8 = 0;
    sub_0800a280();
    sub_0800a294(ta->unk78 * 2, ta->unk78);
    while (gUnk_0200AFF8 == 0)
        TaskYieldTrampoline(1);
    if (gUnk_030023B0 != 0)
        TaskYieldTrampoline(120);
    ActorSetAttackBoxSlot(gUnk_02007D00[1], (u32)gUnk_0874883C);
    sub_08063a14(gUnk_02007D00[1], (u32)gUnk_08748874);
    ActorSetAttackBoxSlot(gUnk_02007D00[0], (u32)gUnk_087488AC);
    sub_08063a14(gUnk_02007D00[0], (u32)gUnk_087488E4);
}

/* sub_080a1ad0 (0x080A1AD0-0x080A1B94) */
void sub_080a1ad0(void)
{
    struct Actor *a;
    u32 v;
    u32 w;
    s32 r;

    a = gCurTask->unk8C;
    gUnk_02007D00[1] = sub_0806505c(1, 0, gViewRect[0] + 192, gViewRect[2] + 48, a->unk20);
    v = a->unk20 & 0xFFF;
    w = gCurTask->unk40 & 0xF000;
    gUnk_02007D00[0] = sub_0806505c(2, 0, gViewRect[0] + 32, gViewRect[2] + 64, v | w);
    r = sub_0806505c(3, 0, 0, 0, gCurTask->unk40);
    gUnk_02006040[5] = r;
    if (r != -1)
    {
        struct Task *t = &gTasks[r];

        t->unk44 = gUnk_02007D00[0];
    }
    ActorLoadDefSlot(gUnk_02007D00[0], gUnk_087487BC);
    gUnk_02007D00[6] = 1;
    gUnk_02007D00[8] = sub_08065f5c() >> 1;
}

/* sub_080a1b94 (0x080A1B94-0x080A1BD8) */
void sub_080a1b94(void)
{
    s32 a;
    s32 b;

    if (RandomRange(2) != 0)
    {
        b = 1;
        a = 2;
        gUnk_02007D00[2] = 0;
        sub_080275cc(2);
    }
    else
    {
        b = 2;
        a = 1;
        gUnk_02007D00[2] = 1;
        sub_080275cc(0);
    }
    sub_080a1bd8(a, b);
}

/* sub_080a1bd8 (0x080A1BD8-0x080A1C1C) */
void sub_080a1bd8(u16 a, u16 b)
{
    ActorSetStateSlot(gUnk_02007D00[1], a);
    TaskSetEntry(sub_080a332c, gUnk_02007D00[1]);
    ActorSetStateSlot(gUnk_02007D00[0], b);
    TaskSetEntry(sub_080a3cbc, gUnk_02007D00[0]);
}

/* sub_080a1c1c (0x080A1C1C-0x080A1C90) */
void sub_080a1c1c(void)
{
    struct Task *tb;
    struct Task *ta;
    struct Task *t;

    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    t = gCurTask;
    t->unk48 = (tb->unk48 + ta->unk48) >> 1;
    t->unk4A = (tb->unk4A + ta->unk4A) >> 1;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
}

/* sub_080a1c90 (0x080A1C90-0x080A1D2C) */
void sub_080a1c90(void)
{
    struct Task *tb;
    struct Task *ta;

    gCurTask->unk78 = 0;
    gUnk_02007D00[7] = 0;
    gUnk_02007D00[6] = 0;
    sub_080a1c1c();
    ActorSetState(2);
    TaskSetEntry(sub_080a3168, gCurTaskIdx);
    if (gUnk_02006040[6] != -1)
    {
        StopSfxOnPlayer(gUnk_02006040[6], 0x216);
        gUnk_02006040[6] = -1;
    }
    sub_080a1bd8(16, 16);
    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    tb->unk6E = 1;
    ta->unk6E = 1;
    gCurTask->unk08 = 0;
    sub_080275cc(1);
}

/* sub_080a1d2c (0x080A1D2C-0x080A1D84) */
void sub_080a1d2c(void)
{
    struct Task *tb;
    struct Task *ta;

    gUnk_02007D00[7] = 0;
    ActorSetState(3);
    TaskSetEntry(sub_080a3168, gCurTaskIdx);
    sub_080a1bd8(17, 17);
    tb = &gTasks[gUnk_02007D00[1]];
    ta = &gTasks[gUnk_02007D00[0]];
    tb->unk6E = 1;
    ta->unk6E = 1;
}

/* sub_080a1d84 (0x080A1D84-0x080A1DBC) */
void sub_080a1d84(void)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    ActorSetHitReactions((u32)gUnk_0874898C);
    a->unk3C = (u32)sub_080a1dbc;
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

/* sub_080a1dbc (0x080A1DBC-0x080A1DD4) */
s32 sub_080a1dbc(s32 a)
{
    if ((u16)gTaskSlotTypes[a] == 63)
        return 0;
    return 1;
}

/* sub_080a1dd4 (0x080A1DD4-0x080A1DF8) */
void sub_080a1dd4(void)
{
    if (gUnk_02007D00[5] == 0 && gUnk_02007D00[3] != 0)
    {
        gUnk_02007D00[3] = 3;
        ActorSetState(2);
    }
}

/* sub_080a1df8 (0x080A1DF8-0x080A1E4C) */
s32 sub_080a1df8(void)
{
    if (gCurTask->unk14 == 8)
    {
        ActorSetState(1);
        if (gCurTask->unk73 == 2)
            TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
        else
            TaskSetEntry(sub_080a332c, gCurTaskIdx);
        return 1;
    }
    return 0;
}

/* sub_080a1e4c (0x080A1E4C-0x080A1EC4) */
s32 sub_080a1e4c(void)
{
    struct Task *t = gCurTask;

    switch (t->unk14)
    {
    default:
        t->unk54 = 0;
        return 0;
    case 11:
        TaskSetMotionX(-t->unk54, -t->unk5C, t->unk64);
        return 0;
    case 12:
        t->unk54 = 0;
        sub_080a2c90();
        ActorSetState(13);
        if (gCurTask->unk73 == 2)
            TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
        else
            TaskSetEntry(sub_080a332c, gCurTaskIdx);
        return 1;
    }
}

/* sub_080a1ec4 (0x080A1EC4-0x080A1F90) */
s32 sub_080a1ec4(void)
{
    sub_080261d4(4);
    sub_0806d4e4(1, 0);
    sub_0800a4c0();
    PlaySfx(510);
    CreateChildTaskHere(162, 1);
    if (gCurTask->unk46 != -1)
    {
        ActorDestroySlot(gCurTask->unk46);
        gCurTask->unk46 = 0xFFFF;
    }
    if (gCurTask->unk73 == 2)
        sub_080a30d0();
    if (gUnk_02007D00[5] != 0)
        ActorSetState(15);
    else
        ActorSetState(2);
    gCurTask->unk20++;
    gUnk_02007D00[5]++;
    if (gCurTask->unk73 == 2)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    else
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    TaskStop();
    return 1;
}

/* sub_080a1f90 (0x080A1F90-0x080A1FC8) */
void sub_080a1f90(void)
{
    if (gCurTask->unk73 == 2)
        sub_080a2b2c(gUnk_02004C90, gUnk_02006190[4]);
    if (gUnk_02006190[3] <= 0)
        sub_0806621c();
}

/* sub_080a1fc8 (0x080A1FC8-0x080A2020) */
s32 sub_080a1fc8(void)
{
    if (gCurTask->unk73 == 2)
        sub_0806619c(13, (u32)sub_080a1f90, (u32)gUnk_082FB230, 16, 2);
    else
        sub_0806619c(13, (u32)sub_080a1f90, (u32)gUnk_082FB210, 16, 0);
    sub_0806caa0(1, 0, 0);
    return 0;
}

/* sub_080a2020 (0x080A2020-0x080A2030) */
void sub_080a2020(void)
{
    sub_08068f68();
    ActorReactToHit();
}

/* sub_080a2030 (0x080A2030-0x080A2090) */
void sub_080a2030(void)
{
    sub_0800a4c0();
    if (gCurTask->unk4A - gViewRect[2] <= 31)
        TaskSetMotionY(128 << 9, 168 << 5, 192 << 11);
    else
        TaskSetMotionY(-0x30000, 168 << 5, 192 << 11);
    sub_080a2c90();
    gCurTask->unk42 = 10;
}

/* sub_080a2090 (0x080A2090-0x080A2164) */
void sub_080a2090(void)
{
    struct Task *t;
    s32 v;

    v = RandomRange(24) + 16;
    gUnk_02006040[4] = v;
    t = gCurTask;
    if (t->unk48 - gViewRect[0] <= 99)
        t->unk43 = 1;
    else
        t->unk43 = 255;
    if (gCurTask->unk73 == 2)
    {
        gUnk_02007D00[2] = 1;
        ActorSetAttackBox((u32)gUnk_087488C8);
        sub_080275cc(0);
    }
    else
    {
        gUnk_02007D00[2] = 0;
        ActorSetAttackBox((u32)gUnk_08748858);
        sub_080275cc(2);
    }
    gUnk_02007D00[4] = 0;
    gCurTask->unk7A = 0;
    TaskStop();
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    TaskSetMotionY(-0x60000, 148 << 6, 192 << 10);
    sub_080a2c90();
    gCurTask->unk42 = 12;
}

/* sub_080a2164 (0x080A2164-0x080A21A0) */
void sub_080a2164(void)
{
    struct Task *t = gCurTask;
    u16 d = t->unk4A - gViewRect[2];

    if ((s16)d <= 15)
    {
        t->unk4A = gViewRect[2] + 17;
        t->unk50 = t->unk4A << 16;
        t->unk58 = 0;
    }
}

/* sub_080a21a0 (0x080A21A0-0x080A2224) */
void sub_080a21a0(void *a)
{
    struct Task *t = gCurTask;
    u16 d = t->unk4A - gViewRect[2];

    if (t->unk58 < 0)
    {
        if ((s16)d > 16)
            return;
        t->unk4A = gViewRect[2] + 17;
        t->unk50 = t->unk4A << 16;
        t->unk58 = 0;
    }
    else if ((s16)d <= (s32)gUnk_02006040[4])
    {
        if ((s16)d > 16)
            return;
        t->unk4A = gViewRect[2] + 17;
        t->unk50 = t->unk4A << 16;
        t->unk58 = 0;
    }
    else
    {
        t->unk4A = gViewRect[2] + gUnk_02006040[4];
        t->unk50 = t->unk4A << 16;
        TaskStop();
        ActorSetState(3);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* sub_080a2224 (0x080A2224-0x080A2274) */
s32 sub_080a2224(void *a)
{
    s32 v;

    if (gUnk_02007D00[5] != 0)
    {
        if (gUnk_02007D00[5] == 2 && gCurTask->unk14 != 16)
            v = 15;
        else
        {
            if (gCurTask->unk20 != 0)
                return 0;
            v = 8;
        }
        ActorSetState(v);
        TaskSetEntry(a, gCurTaskIdx);
        return 1;
    }
    return 0;
}

/* sub_080a2274 (0x080A2274-0x080A22B0) */
void sub_080a2274(void *a)
{
    if (sub_080647fc(16, 64) != 0)
    {
        gCurTask->unk34 = gUnk_030023D4;
        ActorSetState(6);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* sub_080a22b0 (0x080A22B0-0x080A22D4) */
s32 sub_080a22b0(void)
{
    if (gCurTask->unk78 < gUnk_02007D00[8])
        return 60;
    return 120;
}

/* sub_080a22d4 (0x080A22D4-0x080A2390) */
void sub_080a22d4(void)
{
    struct Task *t;
    s32 ofs;
    s32 r;
    s32 acc;
    s32 i;

    t = gCurTask;
    ofs = 0;
    if (t->unk78 >= gUnk_02007D00[8])
        ofs = 4;
    r = 8 - gUnk_087484EC[t->unk24 + ofs];
    r = RandomRange(r);
    acc = 0;
    i = 3;
    if (gCurTask->unk24 != 3)
    {
        acc = gUnk_087484EC[ofs + 3];
        if (acc >= r)
            goto sel;
    }
dec:
    /* Zero-code stand-in: the loop note of this do/while (0) weights the
       refs of acc inside it, so global allocation ranks acc (r2) above r
       (r3); acc's constant `= 0` set halves its priority otherwise. */
    do
    {
        i--;
        if (i < 0)
            goto sel;
        if (gCurTask->unk24 == i)
            goto dec;
        acc += gUnk_087484EC[i + ofs];
        if (acc < r)
            goto dec;
    } while (0);
sel:
    if (gUnk_02007D00[4] != 0 && !(i & 1))
    {
        if (RandomRange(2) != 0)
            i++;
        else
            i--;
    }
    i &= 3;
    gCurTask->unk24 = i;
    ActorSetState(gUnk_087484E4[i]);
}

/* sub_080a2390 (0x080A2390-0x080A23C0) */
s32 sub_080a2390(void)
{
    s32 ret = 0;
    u8 v = ClampTaskToRoom(gCurTask);

    if ((v & 1) || (v & 2))
        ret = 1;
    return ret;
}

/* sub_080a23c0 (0x080A23C0-0x080A2400) */
void sub_080a23c0(void)
{
    TaskFaceNearestPlayer();
    if (RandomRange(4) == 0)
        gCurTask->unk30 = -gCurTask->unk43;
    else
        gCurTask->unk30 = gCurTask->unk43;
}

/* sub_080a2400 (0x080A2400-0x080A2448) */
void sub_080a2400(void *a)
{
    gUnk_02007D00[4] = 0;
    if (gCurTask->unk20 != 0)
        ActorSetState(3);
    else
    {
        gUnk_02007D00[7] = 0;
        gCurTask->unk30 = 300;
        ActorSetState(7);
    }
    TaskSetEntry(a, gCurTaskIdx);
}

/* sub_080a2448 (0x080A2448-0x080A2488) */
void sub_080a2448(void *a)
{
    if (gUnk_02007D00[7] == 0)
    {
        if (gUnk_02007D00[3] == 0)
            gUnk_02007D00[3] = 1;
        sub_080a2590(a);
    }
    else
    {
        ActorSetState(4);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* sub_080a2488 (0x080A2488-0x080A24F8) */
void sub_080a2488(void *a)
{
    if ((u8)sub_080a24f8() != 0)
    {
        AngleToVector((s16)TaskGetAngleTo(gCurTask->unk34, 3), 64);
        gCurTask->unk54 = gUnk_030023B4;
        gCurTask->unk58 = gUnk_030023D4;
    }
    else
    {
        if (gUnk_02007D00[7] == 0)
            ActorSetState(7);
        else
            ActorSetState(3);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* sub_080a24f8 (0x080A24F8-0x080A2590) */
s32 sub_080a24f8(void)
{
    s32 ret = 0;
    s32 d2;
    s32 d = gCurTask->unk4A - gViewRect[2];

    if (d >= 16 && d <= 63)
    {
        if ((u8)sub_080a2390() == 0 && sub_080647fc(16, 64) != 0 && sub_08064758(16, 208) != 0)
        {
            gCurTask->unk34 = gUnk_030023D4;
            ret = 1;
        }
    }
    else
    {
        if (d <= 15)
            gCurTask->unk4A = gViewRect[2] + 17;
        d2 = gCurTask->unk4A - gViewRect[2];
        if (d2 > 63)
            gCurTask->unk4A = gViewRect[2] + 64;
        gCurTask->unk50 = gCurTask->unk4A << 16;
    }
    return ret;
}

/* sub_080a2590 (0x080A2590-0x080A25C4) */
void sub_080a2590(void *a)
{
    if ((gUnk_02007D00[3] >> 1) & 1)
    {
        gUnk_02007D00[3] = 0;
        ActorSetState(8);
        TaskSetEntry(a, gCurTaskIdx);
    }
}

/* sub_080a25c4 (0x080A25C4-0x080A2608) */
void sub_080a25c4(void *a)
{
    if ((u8)sub_080a2224(a) == 0)
    {
        gCurTask->unk30--;
        if (gCurTask->unk30 <= 0)
        {
            ActorSetState(4);
            TaskSetEntry(a, gCurTaskIdx);
        }
        else
            sub_080a2274(a);
    }
}

/* sub_080a2608 (0x080A2608-0x080A263C) */
void sub_080a2608(void *a)
{
    if ((u8)sub_080a2224(a) == 0)
    {
        gCurTask->unk30--;
        if (gCurTask->unk30 <= 0)
            sub_080a2448(a);
        else
            sub_080a2488(a);
    }
}

/* sub_080a263c (0x080A263C-0x080A268C) */
void sub_080a263c(void *a)
{
    if ((u8)sub_080a2224(a) == 0)
    {
        if (gUnk_02007D00[3] == 0)
        {
            gCurTask->unk30--;
            if (gCurTask->unk30 > 0)
                sub_080a2274(a);
            else
            {
                gUnk_02007D00[3]++;
                sub_080a2590(a);
            }
        }
        else
            sub_080a2590(a);
    }
}

/* sub_080a268c (0x080A268C-0x080A2754) */
void sub_080a268c(s32 a)
{
    if (gCurTask->unk54 != 0)
    {
        if (abs(TaskGetDxTo(gCurTask->unk44)) <= a)
        {
            gCurTask->unk54 = 0;
            gCurTask->unk24++;
        }
    }
    if (gCurTask->unk58 != 0)
    {
        if (abs(TaskGetDyTo(gCurTask->unk44)) <= a)
        {
            gCurTask->unk58 = 0;
            gCurTask->unk24++;
        }
    }
    if (gCurTask->unk24 == 2)
        gUnk_02007D00[7]++;
}

/* sub_080a2754 (0x080A2754-0x080A27B8) */
void sub_080a2754(void)
{
    TaskStop();
    gCurTask->unk24 = 0;
    gCurTask->unk6E = 0;
    AngleToVector((s16)TaskGetAngleTo(gCurTask->unk44, 3), 256);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    if (gCurTask->unk54 == 0)
        gCurTask->unk24++;
    if (gCurTask->unk58 == 0)
        gCurTask->unk24++;
}

/* sub_080a27b8 (0x080A27B8-0x080A2814) */
void sub_080a27b8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk42 = 11;
    t = gCurTask;
    t->unk38 = gUnk_08754210;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    t = gCurTask;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->unk70 = 0;
    t->unk6E = 0;
    t->unk2C = 0;
    t->unk28 = 0;
}

/* sub_080a2814 (0x080A2814-0x080A28D0) */
void sub_080a2814(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk8C->unk0B & 1))
    {
        if ((s16)t->unk70 != 0 || t->unk20 != 0)
        {
            u32 v = 2;

            if (t->unk20 != 0)
                v = 4;
            v &= gFrameCount;
            if (v == 0)
                v = (u32)gUnk_082FB210;
            else
                v = (u32)gUnk_082FB190;
            RequestCopy(2, v, (u32)(gObjPalette + ((t->unk40 >> 12) << 5)), 32);
        }
        else
        {
            if (t->unk2C <= 0)
            {
                RequestCopy(2, gUnk_0874850C[t->unk28],
                             (u32)(gObjPalette + ((t->unk40 >> 12) << 5)), 32);
                t = gCurTask;
                t->unk2C = 12;
                t->unk28 = (t->unk28 + 1) & 3;
            }
            gCurTask->unk2C--;
        }
    }
}

/* sub_080a28d0 (0x080A28D0-0x080A291C) */
void sub_080a28d0(void)
{
    s32 i;
    s32 *p;

    gCurTask->unk30--;
    i = 0;
    p = gUnk_0874851C;
    if (*p > gCurTask->unk30)
    {
        do
        {
            p++;
            i++;
            if (i > 3)
                break;
        } while (*p > gCurTask->unk30);
    }
    TaskSetMotionXFacing(gUnk_0874852C[i], 0x5A5A5A5A);
}

/* sub_080a291c (0x080A291C-0x080A2954) */
void sub_080a291c(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 24;
    sp.unk04 = 127;
    sp.unk08 = 0;
    sp.unk09 = 0;
    sp.unk0C = 24;
    sp.unk0E = 0;
    sp.unk0A = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    PlaySfx(0x226);
}

/* sub_080a2954 (0x080A2954-0x080A2994) */
void sub_080a2954(void)
{
    struct ActorSpawn sp;
    struct Task *t = gCurTask;

    t->unk34--;
    t->unk30 = 45;
    sp.unk00 = 24;
    sp.unk04 = 127;
    sp.unk08 = 2;
    sp.unk09 = t->unk34;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk0A = 0;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
}

/* sub_080a2994 (0x080A2994-0x080A29CC) */
void sub_080a2994(void)
{
    struct Task *t = gCurTask;

    if (t->unk43 == -1)
        t->unk3C = 4;
    else
        t->unk3C = 8;
    if (gCurTask->unk20 != 0)
        TaskSetFrame(19);
}

/* sub_080a29cc (0x080A29CC-0x080A2A00) */
void sub_080a29cc(void)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    TaskStop();
    sub_080a2994();
    if (a->unk1C != 6)
        gCurTask->unk30 = 300;
    gUnk_02007D00[7] = 1;
}

/* sub_080a2a00 (0x080A2A00-0x080A2A24) */
void sub_080a2a00(void)
{
    TaskStop();
    gUnk_02007D00[4] = 1;
    gCurTask->unk70 = 1;
    sub_080a2994();
}

/* sub_080a2a24 (0x080A2A24-0x080A2A44) */
void sub_080a2a24(void)
{
    TaskStop();
    gCurTask->unk30 = 45;
    gCurTask->unk34 = 8;
    sub_080a2994();
}

/* sub_080a2a44 (0x080A2A44-0x080A2A94) */
void sub_080a2a44(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk42 = 11;
    t = gCurTask;
    t->unk38 = gUnk_0875412C;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    gCurTask->unk34 = 0;
    gCurTask->unk30 = 0;
    gCurTask->unk2C = 0;
    gCurTask->unk28 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk20 = 0;
    gCurTask->unk1C = 0;
    gCurTask->unk18 = 0;
}

/* sub_080a2a94 (0x080A2A94-0x080A2AF4) */
void sub_080a2a94(void)
{
    s32 i;
    s32 *p;

    gCurTask->unk28--;
    if (gCurTask->unk28 < 0)
        gCurTask->unk28 = 47;
    gCurTask->unk7A = 0;
    i = 0;
    p = gUnk_08748604;
    if (*p > gCurTask->unk28)
    {
        do
        {
            p++;
            i++;
            if (i > 7)
                break;
        } while (*p > gCurTask->unk28);
    }
    gCurTask->unk58 = gUnk_08748614[i];
}

/* sub_080a2af4 (0x080A2AF4-0x080A2B14) */
void sub_080a2af4(void)
{
    TaskStop();
    sub_080a23c0();
    ActorStopAnim();
    TaskSetFrame(4);
    TaskYieldTrampoline(12);
}

/* sub_080a2b14 (0x080A2B14-0x080A2B2C) */
void sub_080a2b14(void)
{
    TaskStop();
    TaskSetFrame(10);
    TaskYieldTrampoline(12);
}

/* sub_080a2b2c (0x080A2B2C-0x080A2BC4) */
void sub_080a2b2c(void *a, u32 b)
{
    struct Task *t = gCurTask;
    struct TaskGfx *g;
    u32 *gt;
    s16 fr;

    if (t->unk38 != 0 && t->unk3C != -1 && !(t->unk8C->unk0B & 1))
    {
        t->unk8C->unk0A |= 1;
        gt = gTasks[gUnk_02006040[5]].unk38;
        fr = gTasks[gUnk_02006040[5]].unk3C;
        if (fr != -1)
            g = (struct TaskGfx *)gt[fr];
        else
            g = (struct TaskGfx *)gt[0];
        if (!(gFrameCount & 2))
            ActorLoadPalette((void *)((u32)g->unk04 + 2), g->unk04[0], 0);
        else
            ActorLoadPalette(a, b << 1, 1);
    }
}

/* sub_080a2bc4 (0x080A2BC4-0x080A2C90) */
void sub_080a2bc4(void)
{
    struct Task *t = gCurTask;
    struct Actor *act = t->unk8C;
    struct TaskGfx *g;
    u32 *gt;
    struct Task *o;
    u32 v;

    if (!(act->unk0B & 1))
    {
        if ((s16)t->unk70 != 0 || t->unk20 != 0)
        {
            v = 2;
            if (t->unk20 != 0)
                v = 4;
            act->unk0A |= 1;
            v &= gFrameCount;
            if (v == 0)
                RequestCopy(2, (u32)gUnk_082FB230,
                             (u32)(gObjPalette + ((gCurTask->unk40 >> 12) << 5)), 32);
            else
            {
                gt = gTasks[gUnk_02006040[5]].unk38;
                o = &gTasks[gUnk_02006040[5]];
                if (o->unk3C != -1)
                    g = (struct TaskGfx *)gt[o->unk3C];
                else
                    g = (struct TaskGfx *)gt[0];
                RequestCopy(2, (u32)g->unk04 + 2,
                             (u32)(gObjPalette + ((gCurTask->unk40 >> 12) << 5)),
                             g->unk04[0]);
            }
        }
    }
}

/* sub_080a2c90 (0x080A2C90-0x080A2CA8) */
void sub_080a2c90(void)
{
    gCurTask->unk8C->unk0A = 0;
    gCurTask->unk70 = 0;
}

/* sub_080a2ca8 (0x080A2CA8-0x080A2CE8) */
void sub_080a2ca8(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 24;
    sp.unk04 = 127;
    sp.unk08 = 1;
    sp.unk09 = 0;
    sp.unk0C = 24;
    sp.unk0E = 0;
    sp.unk10 = sub_0806660c(1);
    sp.unk0A = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
    PlaySfx(0x227);
}

/* sub_080a2ce8 (0x080A2CE8-0x080A2D38) */
void sub_080a2ce8(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 24;
    sp.unk04 = 127;
    sp.unk08 = 3;
    sp.unk09 = 0;
    sp.unk0C = 0;
    sp.unk0E = 64;
    sp.unk10 = sub_0806660c(2);
    sp.unk0A = 0;
    gUnk_02006040[0] = CreateActorFromDesc(&sp, 1);
    gUnk_02006040[6] = PlaySfx(0x216);
}

/* sub_080a2d38 (0x080A2D38-0x080A2DE0) */
void sub_080a2d38(void)
{
    s32 r;
    struct Task *t;

    r = CreateChildTask(193, 0, 96, (u16)sub_0806660c(2));
    gUnk_02006040[3] = r;
    if (r != -1)
    {
        t = &gTasks[r];
        t->unk73 = 0;
    }
    r = CreateChildTask(193, 0, 16, (u16)sub_0806660c(2));
    gUnk_02006040[1] = r;
    if (r != -1)
    {
        t = &gTasks[r];
        t->unk73 = 1;
    }
    r = CreateChildTask(193, 0, 8, (u16)sub_0806660c(2));
    gUnk_02006040[2] = r;
    if (r != -1)
    {
        t = &gTasks[r];
        t->unk73 = 2;
    }
}

/* sub_080a2de0 (0x080A2DE0-0x080A2E88) */
s32 sub_080a2de0(void)
{
    s32 ret = 0;
    struct Task *t = gCurTask;

    if (t->unk54 == 0 && t->unk58 == 0)
        ret = 1;
    else
    {
        t = gCurTask;
        if (t->unk43 == 1)
        {
            if (t->unk48 - gViewRect[0] >= t->unk18)
                t->unk54 = 0;
        }
        else
        {
            if (t->unk48 - gViewRect[0] <= t->unk18)
                t->unk54 = 0;
        }
        t = gCurTask;
        if (t->unk58 >= 0)
        {
            if (t->unk4A >= (s32)gUnk_02006040[4])
                t->unk58 = 0;
        }
        else
        {
            if (t->unk4A <= (s32)gUnk_02006040[4])
                t->unk58 = 0;
        }
    }
    return ret;
}

/* sub_080a2e88 (0x080A2E88-0x080A2EDC) */
void sub_080a2e88(u8 a)
{
    TaskStop();
    if (a == 1)
        gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_0874859C);
    else if (gCurTask->unk20 != 0)
    {
        ActorStopAnim();
        TaskSetFrame(20);
    }
    else
        gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_08748588);
}

/* sub_080a2edc (0x080A2EDC-0x080A2F38) */
void sub_080a2edc(void)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    TaskStop();
    if (gCurTask->unk20 != 0)
    {
        ActorStopAnim();
        TaskSetFrame(20);
    }
    else
        gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_08748588);
    if (a->unk1C != 6)
        gCurTask->unk30 = 300;
    gUnk_02007D00[7] = 1;
}

/* sub_080a2f38 (0x080A2F38-0x080A3000) */
void sub_080a2f38(void)
{
    TaskStop();
    gUnk_02007D00[4] = 1;
    gCurTask->unk70 = 1;
    sub_080a2e88(1);
    gCurTask->unk34 = TaskGetNearestPlayerScreenPos();
    if (gUnk_030023B4 <= 15)
        gUnk_030023B4 = 16;
    if (gUnk_030023B4 > 207)
        gUnk_030023B4 = 208;
    gCurTask->unk18 = gUnk_030023B4;
    gUnk_02006040[4] = 48;
    switch ((u8)TaskGetXDirBitTo(gCurTask->unk34))
    {
    case 4:
        gCurTask->unk43 = 1;
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        break;
    case 8:
        gCurTask->unk43 = 255;
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        break;
    }
    if ((s32)gUnk_02006040[4] < gCurTask->unk4A)
        gCurTask->unk58 = -0x10000;
    else
        gCurTask->unk58 = 128 << 9;
}

/* sub_080a3000 (0x080A3000-0x080A301C) */
void sub_080a3000(void)
{
    TaskStop();
    gCurTask->unk34 = 120;
    sub_080a2e88(1);
}

/* sub_080a301c (0x080A301C-0x080A306C) */
void sub_080a301c(void)
{
    switch ((u8)TaskGetXDirBitTo(TaskGetNearestPlayerScreenPos()))
    {
    case 4:
        gCurTask->unk43 = 1;
        gCurTask->unk54 = 152 << 7;
        break;
    case 8:
        gCurTask->unk43 = 255;
        TaskSetMotionXFacing(152 << 7, 0x5A5A5A5A);
        break;
    }
}

/* sub_080a306c (0x080A306C-0x080A30D0) */
void sub_080a306c(void)
{
    if (gUnk_02006040[0] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[0]);
    if (gUnk_02006040[3] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[3]);
    if (gUnk_02006040[1] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[1]);
    if (gUnk_02006040[2] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[2]);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[3] = -1;
    gUnk_02006040[0] = -1;
    if (gUnk_02006040[6] != -1)
    {
        StopSfxOnPlayer(gUnk_02006040[6], 0x216);
        gUnk_02006040[6] = -1;
    }
}

/* sub_080a30d0 (0x080A30D0-0x080A3114) */
void sub_080a30d0(void)
{
    if (gUnk_02006040[0] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[0]);
    if (gUnk_02006040[1] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[1]);
    if (gUnk_02006040[2] + 1 > 1)
        ActorDestroySlot(gUnk_02006040[2]);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[0] = -1;
}

/* sub_080a3114 (0x080A3114-0x080A314C) */
void sub_080a3114(void)
{
    sub_08066088(0);
    sub_080a1ad0();
    gCurTask->unk04 = (u32)sub_080a314c;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 4, gUnk_08748624);
}

/* sub_080a314c (0x080A314C-0x080A3168) */
void sub_080a314c(void)
{
    CallTableEntry(gCurTask->unk15, 4, gUnk_08748634);
}

/* sub_080a3168 (0x080A3168-0x080A3184) */
void sub_080a3168(void)
{
    CallTableEntry(gCurTask->unk14, 4, gUnk_08748624);
}

/* sub_080a3184 (0x080A3184-0x080A31A4) */
void sub_080a3184(void)
{
    gCurTask->unk15 = 0;
    sub_080a19ec();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a31a4 (0x080A31A4-0x080A31D0) */
void sub_080a31a4(void)
{
    if (gCurTask->unk14 != 0)
    {
        TaskSetEntry(sub_080a3168, gCurTaskIdx);
        sub_080a1b94();
    }
}

/* sub_080a31d0 (0x080A31D0-0x080A31F0) */
void sub_080a31d0(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk08 = (u32)sub_080a31f4;
    TaskSleepForever();
}

/* sub_080a31f0 (0x080A31F0-0x080A31F4) */
void sub_080a31f0(void)
{
}

/* sub_080a31f4 (0x080A31F4-0x080A3238) */
void sub_080a31f4(void)
{
    struct Task *t;

    if (gUnk_02007D00[5] == 2)
        sub_080a1c90();
    else
    {
        t = &gTasks[gUnk_02007D00[2] != 0 ? gUnk_02007D00[1] : gUnk_02007D00[0]];
        gCurTask->unk78 = t->unk78;
    }
}

/* sub_080a3238 (0x080A3238-0x080A3250) */
void sub_080a3238(void)
{
    gCurTask->unk15 = 2;
    TaskSleepForever();
}

/* sub_080a3250 (0x080A3250-0x080A3268) */
void sub_080a3250(void)
{
    if (gUnk_02007D00[7] == 2)
        sub_080a1d2c();
}

/* sub_080a3268 (0x080A3268-0x080A3280) */
void sub_080a3268(void)
{
    gCurTask->unk15 = 3;
    TaskSleepForever();
}

/* sub_080a3280 (0x080A3280-0x080A3298) */
void sub_080a3280(void)
{
    if (gUnk_02007D00[7] == 2)
        sub_080a1d84();
}

/* sub_080a3298 (0x080A3298-0x080A32C4) */
void sub_080a3298(void)
{
    gCurTask->unk04 = (u32)sub_080a32c4;
    sub_080a27b8();
    CallTableEntry(gCurTask->unk14, 18, gUnk_08748644);
}

/* sub_080a32c4 (0x080A32C4-0x080A332C) */
void sub_080a32c4(void)
{
    if ((u8)ActorCollideTerrain() == 0 && gCurTask->unk6E == 0)
        CallTableEntry(gCurTask->unk15, 18, gUnk_0874868C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        sub_080a2814();
        if (gUnk_02007D00[6] != 0)
            sub_080a2020();
    }
}

/* sub_080a332c (0x080A332C-0x080A3348) */
void sub_080a332c(void)
{
    CallTableEntry(gCurTask->unk14, 18, gUnk_08748644);
}

/* sub_080a3348 (0x080A3348-0x080A33A4) */
void sub_080a3348(void)
{
    gCurTask->unk15 = 0;
    TaskSetFrame(10);
    gCurTask->unk7A = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSleepForever();
}

/* sub_080a33a4 (0x080A33A4-0x080A33DC) */
void sub_080a33a4(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 0)
    {
        t->unk24 = 3;
        t->unk20 = 0;
        t->unk46 = 0xFFFF;
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    }
}

/* sub_080a33dc (0x080A33DC-0x080A3424) */
void sub_080a33dc(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    sub_080a1dd4();
    gCurTask->unk1C = 8;
    ActorSetAttackBox((u32)gUnk_0874883C);
    TaskSetFrame(10);
    TaskYieldTrampoline((s16)sub_080a22b0());
    sub_080a22d4();
    TaskSleepForever();
}

/* sub_080a3424 (0x080A3424-0x080A346C) */
void sub_080a3424(void)
{
    if (gCurTask->unk1C <= 0)
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(10);
        gCurTask->unk1C = 8;
    }
    gCurTask->unk1C--;
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
}

/* sub_080a346c (0x080A346C-0x080A348C) */
void sub_080a346c(void)
{
    gCurTask->unk15 = 2;
    TaskSetFrame(13);
    sub_080a2090();
    TaskSleepForever();
}

/* sub_080a348c (0x080A348C-0x080A349C) */
void sub_080a348c(void)
{
    sub_080a21a0(sub_080a332c);
}

/* sub_080a349c (0x080A349C-0x080A34B8) */
void sub_080a349c(void)
{
    gCurTask->unk15 = 3;
    sub_080a29cc();
    TaskSleepForever();
}

/* sub_080a34b8 (0x080A34B8-0x080A34C8) */
void sub_080a34b8(void)
{
    sub_080a25c4(sub_080a332c);
}

/* sub_080a34c8 (0x080A34C8-0x080A34F0) */
void sub_080a34c8(void)
{
    gCurTask->unk15 = 4;
    sub_080a2a00();
    TaskYieldTrampoline(60);
    ActorSetState(5);
    TaskSleepForever();
}

/* sub_080a34f0 (0x080A34F0-0x080A352C) */
void sub_080a34f0(void)
{
    void *f = sub_080a332c;

    if ((u8)sub_080a2224(f) == 0 && gCurTask->unk14 != 4)
    {
        sub_080a2c90();
        TaskSetEntry(f, gCurTaskIdx);
    }
}

/* sub_080a352c (0x080A352C-0x080A3548) */
void sub_080a352c(void)
{
    gCurTask->unk15 = 5;
    sub_080a2a24();
    TaskSleepForever();
}

/* sub_080a3548 (0x080A3548-0x080A3588) */
void sub_080a3548(void)
{
    void *f = sub_080a332c;

    if ((u8)sub_080a2224(f) == 0)
    {
        gCurTask->unk30--;
        if (gCurTask->unk30 <= 0)
        {
            if (gCurTask->unk34 > 0)
                sub_080a2954();
            else
                sub_080a2400(f);
        }
    }
}

/* sub_080a3588 (0x080A3588-0x080A35A8) */
void sub_080a3588(void)
{
    gCurTask->unk15 = 6;
    TaskStop();
    sub_080a2994();
    TaskSleepForever();
}

/* sub_080a35a8 (0x080A35A8-0x080A35B8) */
void sub_080a35a8(void)
{
    sub_080a2608(sub_080a332c);
}

/* sub_080a35b8 (0x080A35B8-0x080A35D8) */
void sub_080a35b8(void)
{
    gCurTask->unk15 = 7;
    TaskStop();
    sub_080a2994();
    TaskSleepForever();
}

/* sub_080a35d8 (0x080A35D8-0x080A35E8) */
void sub_080a35d8(void)
{
    sub_080a263c(sub_080a332c);
}

/* sub_080a35e8 (0x080A35E8-0x080A361C) */
void sub_080a35e8(void)
{
    gCurTask->unk15 = 8;
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    sub_080a2030();
    TaskSleepForever();
}

/* sub_080a361c (0x080A361C-0x080A3628) */
void sub_080a361c(void)
{
    sub_080a2164();
}

/* sub_080a3628 (0x080A3628-0x080A368C) */
void sub_080a3628(void)
{
    gCurTask->unk15 = 9;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk30 = 20;
        TaskSetFrame(10);
        TaskYieldTrampoline(10);
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a368c (0x080A368C-0x080A36C8) */
void sub_080a368c(void)
{
    if (gCurTask->unk14 != 9)
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    else
    {
        sub_080a28d0();
        if ((u8)sub_080a2390() != 0)
            TaskTurnAroundAndReverseX();
    }
}

/* sub_080a36c8 (0x080A36C8-0x080A3768) */
void sub_080a36c8(void)
{
    gCurTask->unk15 = 10;
    TaskStop();
    sub_080a23c0();
    TaskSetFrame(10);
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk7A = 0;
    TaskSetFrame(13);
    gCurTask->unk54 = gCurTask->unk30 << 16;
    TaskSetMotionY(-0x60000, 128 << 7, 192 << 11);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a3768 (0x080A3768-0x080A37AC) */
void sub_080a3768(void)
{
    if (gCurTask->unk14 != 10)
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
    {
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk30 = -gCurTask->unk30;
    }
}

/* sub_080a37ac (0x080A37AC-0x080A3840) */
void sub_080a37ac(void)
{
    gCurTask->unk15 = 11;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk70 = 1;
    gCurTask->unk7A = 0;
    TaskSetFrame(13);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(10);
    ActorSetState(12);
    TaskSleepForever();
}

/* sub_080a3840 (0x080A3840-0x080A387C) */
void sub_080a3840(void)
{
    if (gCurTask->unk14 != 11)
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
        gCurTask->unk54 = 0;
}

/* sub_080a387c (0x080A387C-0x080A38EC) */
void sub_080a387c(void)
{
    gCurTask->unk15 = 12;
    TaskStop();
    PlaySfx(500);
    TaskSetMotionXFacing(144 << 11, 0x5A5A5A5A);
    gCurTask->unk46 = sub_0806e6f8(-16, 7);
    gCurTask->unk3E &= 0x7FFF;
    if (gCurTask->unk43 == 1)
        gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_0874853C);
    else
        gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_08748558);
    TaskSleepForever();
}

/* sub_080a38ec (0x080A38EC-0x080A3954) */
void sub_080a38ec(void)
{
    if ((u8)sub_080a2390() != 0)
    {
        gCurTask->unk54 = 0;
        sub_080a2c90();
        ActorDestroySlot(gCurTask->unk46);
        gCurTask->unk46 = 0xFFFF;
        ActorSetState(13);
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    }
    else
        gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
}

/* sub_080a3954 (0x080A3954-0x080A3A10) */
void sub_080a3954(void)
{
    gCurTask->unk15 = 13;
    TaskStop();
    PlaySfx(0x1F7);
    sub_080261d4(4);
    sub_08063a00((u32)gUnk_08748890);
    gCurTask->unk7A = 0;
    TaskSetFrame(14);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSetFrame(12);
    TaskYieldTrampoline(5);
    TaskSetFrame(10);
    TaskYieldTrampoline(5);
    TaskSetFrame(12);
    TaskYieldTrampoline(5);
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a3a10 (0x080A3A10-0x080A3A40) */
void sub_080a3a10(void)
{
    if (gCurTask->unk14 != 13)
    {
        sub_08063a00((u32)gUnk_08748874);
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    }
}

/* sub_080a3a40 (0x080A3A40-0x080A3B10) */
void sub_080a3a40(void)
{
    gCurTask->unk15 = 14;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(11);
    TaskYieldTrampoline(15);
    TaskSetFrame(15);
    TaskYieldTrampoline(20);
    gCurTask->unk3C++;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    sub_080a291c();
    gCurTask->unk3C++;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(20);
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a3b10 (0x080A3B10-0x080A3B4C) */
void sub_080a3b10(void)
{
    if (gCurTask->unk14 != 14)
        TaskSetEntry(sub_080a332c, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
        gCurTask->unk54 = 0;
}

/* sub_080a3b4c (0x080A3B4C-0x080A3B78) */
void sub_080a3b4c(void)
{
    gCurTask->unk15 = 15;
    TaskStop();
    gCurTask->unk70 = 1;
    TaskSetFrame(19);
    TaskSleepForever();
}

/* sub_080a3b78 (0x080A3B78-0x080A3B7C) */
void sub_080a3b78(void)
{
}

/* sub_080a3b7c (0x080A3B7C-0x080A3BA0) */
void sub_080a3b7c(void)
{
    gCurTask->unk04 = (u32)sub_080a3ba0;
    sub_080a2754();
    TaskSetFrame(19);
    TaskSleepForever();
}

/* sub_080a3ba0 (0x080A3BA0-0x080A3BC0) */
void sub_080a3ba0(void)
{
    if (gCurTask->unk24 != 2)
        sub_080a268c(8);
    sub_080a2814();
}

/* sub_080a3bc0 (0x080A3BC0-0x080A3C08) */
void sub_080a3bc0(void)
{
    gCurTask->unk04 = (u32)sub_080a3c08;
    sub_080a2754();
    TaskSetFrame(19);
    for (;;)
    {
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk58 = -gCurTask->unk58;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk58 = -gCurTask->unk58;
    }
}

/* sub_080a3c08 (0x080A3C08-0x080A3C28) */
void sub_080a3c08(void)
{
    if (gCurTask->unk24 != 2)
        sub_080a268c(3);
    sub_080a2814();
}

/* sub_080a3c28 (0x080A3C28-0x080A3C54) */
void sub_080a3c28(void)
{
    gCurTask->unk04 = (u32)sub_080a3c54;
    sub_080a2a44();
    CallTableEntry(gCurTask->unk14, 18, gUnk_087486D4);
}

/* sub_080a3c54 (0x080A3C54-0x080A3CBC) */
void sub_080a3c54(void)
{
    if ((u8)ActorCollideTerrain() == 0 && gCurTask->unk6E == 0)
        CallTableEntry(gCurTask->unk15, 18, gUnk_0874871C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        sub_080a2bc4();
        if (gUnk_02007D00[6] != 0)
            sub_080a2020();
    }
}

/* sub_080a3cbc (0x080A3CBC-0x080A3CD8) */
void sub_080a3cbc(void)
{
    CallTableEntry(gCurTask->unk14, 18, gUnk_087486D4);
}

/* sub_080a3cd8 (0x080A3CD8-0x080A3D3C) */
void sub_080a3cd8(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk1C = ActorStartAnim(gUnk_08748574);
    gCurTask->unk7A = 0;
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    TaskSleepForever();
}

/* sub_080a3d3c (0x080A3D3C-0x080A3D84) */
void sub_080a3d3c(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 0)
    {
        t->unk24 = 3;
        t->unk20 = 0;
        t->unk46 = 0xFFFF;
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    }
    gCurTask->unk1C = ActorTickAnimFacingNearestPlayer(gCurTask->unk1C);
}

/* sub_080a3d84 (0x080A3D84-0x080A3DD0) */
void sub_080a3d84(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    sub_080a1dd4();
    ActorSetAttackBox((u32)gUnk_087488AC);
    gCurTask->unk1C = ActorStartAnim(gUnk_08748574);
    gCurTask->unk28 = 48;
    TaskYieldTrampoline((s16)sub_080a22b0());
    sub_080a22d4();
    TaskSleepForever();
}

/* sub_080a3dd0 (0x080A3DD0-0x080A3E10) */
void sub_080a3dd0(void)
{
    struct Task *t;

    gCurTask->unk1C = ActorTickAnimFacingNearestPlayer(gCurTask->unk1C);
    sub_080a2a94();
    t = gCurTask;
    if (t->unk14 != 1 && t->unk28 == 47)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
}

/* sub_080a3e10 (0x080A3E10-0x080A3E3C) */
void sub_080a3e10(void)
{
    gCurTask->unk15 = 2;
    sub_080a2090();
    gCurTask->unk1C = ActorStartAnim(gUnk_087485EC);
    TaskSleepForever();
}

/* sub_080a3e3c (0x080A3E3C-0x080A3E60) */
void sub_080a3e3c(void)
{
    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    sub_080a21a0(sub_080a3cbc);
}

/* sub_080a3e60 (0x080A3E60-0x080A3E7C) */
void sub_080a3e60(void)
{
    gCurTask->unk15 = 3;
    sub_080a2edc();
    TaskSleepForever();
}

/* sub_080a3e7c (0x080A3E7C-0x080A3EA0) */
void sub_080a3e7c(void)
{
    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    sub_080a25c4(sub_080a3cbc);
}

/* sub_080a3ea0 (0x080A3EA0-0x080A3EDC) */
void sub_080a3ea0(void)
{
    gCurTask->unk15 = 4;
    sub_080a2f38();
    while ((u8)sub_080a2de0() == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(60);
    ActorSetState(5);
    TaskSleepForever();
}

/* sub_080a3edc (0x080A3EDC-0x080A3F24) */
void sub_080a3edc(void)
{
    void *f;

    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    f = sub_080a3cbc;
    if ((u8)sub_080a2224(f) == 0 && gCurTask->unk14 != 4)
    {
        sub_080a2c90();
        TaskSetEntry(f, gCurTaskIdx);
    }
}

/* sub_080a3f24 (0x080A3F24-0x080A3F54) */
void sub_080a3f24(void)
{
    gCurTask->unk15 = 5;
    sub_080a3000();
    TaskYieldTrampoline(6);
    sub_080a2ce8();
    sub_080a2d38();
    sub_080657f8(10, 80);
    TaskSleepForever();
}

/* sub_080a3f54 (0x080A3F54-0x080A3FB8) */
void sub_080a3f54(void)
{
    void *f;

    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    f = sub_080a3cbc;
    if ((u8)sub_080a2224(f) == 0)
    {
        if (gCurTask->unk34 <= 95)
            sub_080a301c();
        if (gCurTask->unk34 <= 0)
        {
            sub_080a2400(f);
            sub_080657cc(16);
            sub_080a306c();
        }
        gCurTask->unk34--;
    }
    else
    {
        sub_080657a4();
        sub_080a306c();
    }
}

/* sub_080a3fb8 (0x080A3FB8-0x080A3FD4) */
void sub_080a3fb8(void)
{
    gCurTask->unk15 = 6;
    sub_080a2e88(0);
    TaskSleepForever();
}

/* sub_080a3fd4 (0x080A3FD4-0x080A3FF8) */
void sub_080a3fd4(void)
{
    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    sub_080a2608(sub_080a3cbc);
}

/* sub_080a3ff8 (0x080A3FF8-0x080A4018) */
void sub_080a3ff8(void)
{
    gCurTask->unk15 = 7;
    TaskStop();
    sub_080a2e88(0);
    TaskSleepForever();
}

/* sub_080a4018 (0x080A4018-0x080A403C) */
void sub_080a4018(void)
{
    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    sub_080a263c(sub_080a3cbc);
}

/* sub_080a403c (0x080A403C-0x080A406C) */
void sub_080a403c(void)
{
    gCurTask->unk15 = 8;
    TaskStop();
    gCurTask->unk1C = ActorStartAnimNoFlip(gUnk_087485FC);
    sub_080a2030();
    TaskSleepForever();
}

/* sub_080a406c (0x080A406C-0x080A408C) */
void sub_080a406c(void)
{
    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    sub_080a2164();
}

/* sub_080a408c (0x080A408C-0x080A412C) */
void sub_080a408c(void)
{
    gCurTask->unk15 = 9;
    sub_080a2af4();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk7A = 0;
        TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
        TaskSetMotionY(-0x20000, 128 << 7, 128 << 10);
        gCurTask->unk1C = ActorStartAnim(gUnk_087485B0);
        if (!(gCurTask->unk7A & 1))
        {
            while (!(gCurTask->unk7A & 1))
                TaskYieldTrampoline(1);
        }
        sub_080a2b14();
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a412c (0x080A412C-0x080A4180) */
void sub_080a412c(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk7A & 1))
        gCurTask->unk1C = ActorTickAnim(t->unk1C);
    if (gCurTask->unk14 != 9)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
        TaskTurnAroundAndReverseX();
}

/* sub_080a4180 (0x080A4180-0x080A4200) */
void sub_080a4180(void)
{
    struct Task *t;

    gCurTask->unk15 = 10;
    sub_080a2af4();
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk54 = t->unk30 * 9 << 13;
    TaskSetMotionY(-0x60000, 128 << 7, 192 << 11);
    gCurTask->unk1C = ActorStartAnim(gUnk_087485C0);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    sub_080a2b14();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a4200 (0x080A4200-0x080A4260) */
void sub_080a4200(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk7A & 1))
        gCurTask->unk1C = ActorTickAnim(t->unk1C);
    if (gCurTask->unk14 != 10)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
    {
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk30 = -gCurTask->unk30;
    }
}

/* sub_080a4260 (0x080A4260-0x080A42F8) */
void sub_080a4260(void)
{
    gCurTask->unk15 = 11;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk70 = 1;
    ActorStopAnim();
    sub_080a2b14();
    gCurTask->unk7A = 0;
    gCurTask->unk1C = ActorStartAnim(gUnk_087485D0);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(12);
    TaskSleepForever();
}

/* sub_080a42f8 (0x080A42F8-0x080A4350) */
void sub_080a42f8(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk7A & 1))
        gCurTask->unk1C = ActorTickAnim(t->unk1C);
    if (gCurTask->unk14 != 11)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
        gCurTask->unk54 = 0;
}

/* sub_080a4350 (0x080A4350-0x080A43D8) */
void sub_080a4350(void)
{
    gCurTask->unk15 = 12;
    TaskStop();
    gCurTask->unk46 = sub_0806e808(24, 5);
    gUnk_02006040[2] = -1;
    gUnk_02006040[1] = -1;
    gUnk_02006040[0] = -1;
    PlaySfx(500);
    TaskSetMotionXFacing(144 << 11, 0x5A5A5A5A);
    TaskSetFrame(12);
    TaskYieldTrampoline(3);
    gUnk_02006040[0] = sub_0806e808(24, 5);
    TaskYieldTrampoline(3);
    gUnk_02006040[1] = sub_0806e808(24, 5);
    TaskYieldTrampoline(3);
    gUnk_02006040[2] = sub_0806e808(24, 5);
    TaskSleepForever();
}

/* sub_080a43d8 (0x080A43D8-0x080A4430) */
void sub_080a43d8(void)
{
    if ((u8)sub_080a2390() != 0)
    {
        gCurTask->unk54 = 0;
        sub_080a2c90();
        ActorDestroySlot(gCurTask->unk46);
        gCurTask->unk46 = 0xFFFF;
        sub_080a30d0();
        ActorSetState(13);
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    }
}

/* sub_080a4430 (0x080A4430-0x080A44C4) */
void sub_080a4430(void)
{
    gCurTask->unk15 = 13;
    TaskStop();
    PlaySfx(0x1F7);
    sub_080261d4(4);
    gCurTask->unk7A = 0;
    gCurTask->unk1C = ActorStartAnim(gUnk_087485DC);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 168 << 5, 192 << 10);
    if (!(gCurTask->unk7A & 1))
    {
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a44c4 (0x080A44C4-0x080A4508) */
void sub_080a44c4(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk7A & 1))
        gCurTask->unk1C = ActorTickAnim(t->unk1C);
    if (gCurTask->unk14 != 13)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
}

/* sub_080a4508 (0x080A4508-0x080A45BC) */
void sub_080a4508(void)
{
    gCurTask->unk15 = 14;
    TaskStop();
    TaskFaceNearestPlayer();
    gCurTask->unk1C = ActorStartAnim(gUnk_087485E4);
    TaskYieldTrampoline(64);
    sub_080a2ca8();
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(20);
    ActorSetState(1);
    TaskSleepForever();
}

/* sub_080a45bc (0x080A45BC-0x080A4604) */
void sub_080a45bc(void)
{
    struct Task *t;

    gCurTask->unk1C = ActorTickAnim(gCurTask->unk1C);
    if (gCurTask->unk14 != 14)
        TaskSetEntry(sub_080a3cbc, gCurTaskIdx);
    else if ((u8)sub_080a2390() != 0)
        gCurTask->unk54 = 0;
}

/* sub_080a4604 (0x080A4604-0x080A4630) */
void sub_080a4604(void)
{
    gCurTask->unk15 = 15;
    TaskStop();
    gCurTask->unk70 = 1;
    TaskSetFrame(20);
    TaskSleepForever();
}

/* sub_080a4630 (0x080A4630-0x080A4634) */
void sub_080a4630(void)
{
}

/* sub_080a4634 (0x080A4634-0x080A4658) */
void sub_080a4634(void)
{
    gCurTask->unk04 = (u32)sub_080a4658;
    sub_080a2754();
    TaskSetFrame(20);
    TaskSleepForever();
}

/* sub_080a4658 (0x080A4658-0x080A4678) */
void sub_080a4658(void)
{
    if (gCurTask->unk24 != 2)
        sub_080a268c(8);
    sub_080a2bc4();
}

/* sub_080a4678 (0x080A4678-0x080A46C0) */
void sub_080a4678(void)
{
    gCurTask->unk04 = (u32)sub_080a46c0;
    sub_080a2754();
    TaskSetFrame(20);
    for (;;)
    {
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk58 = -gCurTask->unk58;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = -gCurTask->unk54;
        gCurTask->unk58 = -gCurTask->unk58;
    }
}

/* sub_080a46c0 (0x080A46C0-0x080A46E0) */
void sub_080a46c0(void)
{
    if (gCurTask->unk24 != 2)
        sub_080a268c(3);
    sub_080a2bc4();
}

/* sub_080a46e0 (0x080A46E0-0x080A4708) */
void sub_080a46e0(void)
{
    ActorDestroySlot(gUnk_02007D00[1]);
    ActorDestroySlot(gUnk_02007D00[0]);
    ActorDestroySlot(gUnk_02006040[5]);
}

/* sub_080a4708 (0x080A4708-0x080A472C) */
void sub_080a4708(void)
{
    sub_08066fc0(0, gCurTask->unk48, gCurTask->unk4A);
}

/* sub_080a472c (0x080A472C-0x080A4808) */
void sub_080a472c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk0C = (u32)sub_080a488c;
    t->unk04 = (u32)sub_080a4808;
    t->unk38 = gUnk_08754180;
    t->unk42 = 11;
    for (;;)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    }
}

/* sub_080a4808 (0x080A4808-0x080A4814) */
void sub_080a4808(void)
{
    sub_080a4814();
}

/* sub_080a4814 (0x080A4814-0x080A488C) */
void sub_080a4814(void)
{
    struct Task *pt = &gTasks[gUnk_02007D00[0]];
    struct Task *t = gCurTask;
    struct Actor *myact = t->unk8C;
    struct Actor *pact = gTasks[gUnk_02007D00[0]].unk8C;

    switch (pt->unk3C)
    {
    case 13:
        t->unk38 = gUnk_087541E0;
        break;
    case 12:
        t->unk38 = gUnk_087541B0;
        break;
    default:
        t->unk38 = gUnk_08754180;
        break;
    }
    if (pt->unk3C > 15)
        gCurTask->unk3C = 0xFFFF;
    gCurTask->unk43 = pt->unk43;
    myact->unk0A = pact->unk0A;
}

/* sub_080a488c (0x080A488C-0x080A498C) */
void sub_080a488c(void)
{
    struct Task *t;
    struct Actor *act;
    struct TaskGfx *g;
    u16 *src;
    u16 *nx;
    u32 *gt;
    u32 dst;
    u32 x;

    if ((u8)sub_08066a6c() != 0 && TaskIsOnScreen() != 0)
    {
        t = gCurTask;
        x = t->unk40;
        x &= 0x7FF;
        dst = (x << 5) + 0x0600FE00;
        act = t->unk8C;
        gt = t->unk38;
        if (t->unk3C != -1)
        {
            g = (struct TaskGfx *)gt[t->unk3C];
            src = g->unk08;
            while (src[0] != 0xFFFF)
            {
                nx = src + 1;
                RequestCopy(4, (u32)nx, dst, src[0]);
                src = (u16 *)((u32)nx + src[0]);
                dst += 128 << 3;
            }
            QueueSprite(gCurTask->unk42, (u32)g->unk00, gCurTask->unk3E,
                         gCurTask->unk40 | (128 << 4),
                         gCurTask->unk48 - gSpriteCameraX,
                         (s16)(gCurTask->unk4A - gSpriteCameraY));
        }
        else
            g = (struct TaskGfx *)gt[0];
        if (!(act->unk0A & 1))
            ActorLoadPalette((void *)((u32)g->unk04 + 2), g->unk04[0], 0);
    }
}

/* sub_080a498c (0x080A498C-0x080A49CC) */
void sub_080a498c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 9;
    gCurTask->unk38 = gUnk_0874C44C;
    CallTableEntry(gCurTask->unk73, 1, gUnk_087489B4);
}

/* sub_080a49cc (0x080A49CC-0x080A4A1C) */
void sub_080a49cc(void)
{
    struct Task *t;

    gUnk_02007D00[8] = -1;
    sub_08068920(gUnk_02007D00[1], 2);
    t = gCurTask;
    sub_08068950(t->unk48, t->unk4A, -t->unk43);
    sub_080261d4(4);
    gCurTask->unk18 = 1;
}

/* sub_080a4a1c (0x080A4A1C-0x080A4A60) */
void sub_080a4a1c(void)
{
    gCurTask->unk0C = (u32)ActorDrawWorldInView;
    gCurTask->unk04 = (u32)sub_080a4a60;
    TaskFaceLikeParent();
    ActorSetAttackBox((u32)gUnk_08748BD4);
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087489B8);
}

/* sub_080a4a60 (0x080A4A60-0x080A4AA8) */
void sub_080a4a60(void)
{
    sub_08064bcc();
    CallTableEntry(gCurTask->unk15, 1, gUnk_087489BC);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a4aa8 (0x080A4AA8-0x080A4AC4) */
void sub_080a4aa8(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087489B8);
}

/* sub_080a4ac4 (0x080A4AC4-0x080A4B1C) */
void sub_080a4ac4(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk24 = 2;
    t->unk1C = 1;
    t->unk3C = 4;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    gCurTask->unk18 = 0;
    while (gCurTask->unk18 == 0)
        TaskYieldTrampoline(1);
    ActorSetHitReactions((u32)gUnk_08748D04);
    ActorDie();
}

/* sub_080a4b1c (0x080A4B1C-0x080A4B68) */
void sub_080a4b1c(void)
{
    u8 v;

    if (gCurTask->unk1C != 0)
    {
        sub_08068920(gUnk_02007D00[1], 1);
        gCurTask->unk1C = 0;
    }
    else
    {
        v = ClampTaskToRoom(gCurTask);
        if ((v & 1) || (v & 2))
            sub_080a49cc();
    }
}

/* sub_080a4b68 (0x080A4B68-0x080A4BA8) */
void sub_080a4b68(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 9;
    gCurTask->unk38 = gUnk_08753990;
    CallTableEntry(gCurTask->unk73, 1, gUnk_087489D4);
}

/* sub_080a4ba8 (0x080A4BA8-0x080A4BDC) */
void sub_080a4ba8(void)
{
    gCurTask->unk04 = (u32)sub_080a4bdc;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087489D8);
}

/* sub_080a4bdc (0x080A4BDC-0x080A4C20) */
void sub_080a4bdc(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087489DC);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a4c20 (0x080A4C20-0x080A4C3C) */
void sub_080a4c20(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087489D8);
}

/* sub_080a4c3c (0x080A4C3C-0x080A4C80) */
void sub_080a4c3c(void)
{
    s32 i;
    u32 *p;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskSetFrame(0);
    p = gUnk_087489C0;
    for (i = 4; i >= 0; i--)
    {
        TaskSetMotionXFacing(*p++, 0x5A5A5A5A);
        TaskYieldTrampoline(6);
    }
    ActorDestroy();
}

/* sub_080a4c80 (0x080A4C80-0x080A4C84) */
void sub_080a4c80(void)
{
}

/* sub_080a4c84 (0x080A4C84-0x080A4CC4) */
void sub_080a4c84(void)
{
    struct Task *t = gCurTask;
    struct Actor *a = t->unk8C;

    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 9;
    a->unk38 = -2;
    CallTableEntry(gCurTask->unk73, 4, gUnk_087489E0);
}

/* sub_080a4cc4 (0x080A4CC4-0x080A4D00) */
void sub_080a4cc4(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080a4d00;
    t->unk38 = gUnk_08754260;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_087489F0);
}

/* sub_080a4d00 (0x080A4D00-0x080A4D6C) */
void sub_080a4d00(void)
{
    if ((u8)sub_080695bc() != 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_080a4d6c, gCurTaskIdx);
    }
    else
        CallTableEntry(gCurTask->unk15, 2, gUnk_087489F8);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a4d6c (0x080A4D6C-0x080A4D88) */
void sub_080a4d6c(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_087489F0);
}

/* sub_080a4d88 (0x080A4D88-0x080A4DD8) */
void sub_080a4d88(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    if (gCurTask->unk43 == 1)
        gCurTask->unk28 = ActorStartAnimNoFlip(gUnk_08748A14);
    else
        gCurTask->unk28 = ActorStartAnimNoFlip(gUnk_08748A00);
    TaskSleepForever();
}

/* sub_080a4dd8 (0x080A4DD8-0x080A4DF4) */
void sub_080a4dd8(void)
{
    gCurTask->unk28 = ActorTickAnim(gCurTask->unk28);
}

/* sub_080a4df4 (0x080A4DF4-0x080A4E10) */
void sub_080a4df4(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a4e10 (0x080A4E10-0x080A4E14) */
void sub_080a4e10(void)
{
}

/* sub_080a4e14 (0x080A4E14-0x080A4E9C) */
void sub_080a4e14(void)
{
    s32 r;
    s32 m;

    m = RandomRange(32) + 16;
    gCurTask->unk50 = ((s16)m + gViewRect[2]) << 16;
    gCurTask->unk4C = (gUnk_08748A38[gCurTask->unk74 & 3] + gViewRect[0]) << 16;
    r = RandomRange(3);
    gCurTask->unk28 = ((gCurTask->unk74 & 1) + 3) << 16;
    if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
        r += 2;
    gCurTask->unk2C = gUnk_08748A40[r];
}

/* sub_080a4e9c (0x080A4E9C-0x080A4EE0) */
void sub_080a4e9c(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080a4ee0;
    t->unk38 = gUnk_087542A8;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B34);
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A28);
}

/* sub_080a4ee0 (0x080A4EE0-0x080A4F24) */
void sub_080a4ee0(void)
{
    CallTableEntry(gCurTask->unk15, 2, gUnk_08748A30);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a4f24 (0x080A4F24-0x080A4F40) */
void sub_080a4f24(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A28);
}

/* sub_080a4f40 (0x080A4F40-0x080A5008) */
void sub_080a4f40(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk7A = 0;
    gCurTask->unk18 = 0;
    sub_080a4e14();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk40 = 0;
    gCurTask->unk54 = gCurTask->unk2C;
    gCurTask->unk58 = gCurTask->unk28;
    switch ((s32)RandomRange(3))
    {
    case 0:
        PlaySfx(0x213);
        break;
    case 1:
        PlaySfx(133 << 2);
        break;
    case 2:
        PlaySfx(0x215);
        break;
    }
    t = gCurTask;
    t->unk38 = gUnk_0874C44C;
    t->unk3C = 4;
    t->unk18 = 1;
    t->unk24 = 2;
    TaskSleepForever();
}

/* sub_080a5008 (0x080A5008-0x080A5020) */
void sub_080a5008(void)
{
    if (gCurTask->unk18 != 0)
        sub_08064bcc();
}

/* sub_080a5020 (0x080A5020-0x080A503C) */
void sub_080a5020(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a503c (0x080A503C-0x080A5040) */
void sub_080a503c(void)
{
}

/* sub_080a5040 (0x080A5040-0x080A5084) */
void sub_080a5040(void)
{
    struct Task *t = gCurTask;

    t->unk04 = (u32)sub_080a5084;
    t->unk38 = gUnk_08754280;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B60);
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A54);
}

/* sub_080a5084 (0x080A5084-0x080A50F0) */
void sub_080a5084(void)
{
    if ((u8)sub_080695bc() != 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_080a50f0, gCurTaskIdx);
    }
    else
        CallTableEntry(gCurTask->unk15, 2, gUnk_08748A5C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a50f0 (0x080A50F0-0x080A510C) */
void sub_080a50f0(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A54);
}

/* sub_080a510c (0x080A510C-0x080A5164) */
void sub_080a510c(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(2);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    }
}

/* sub_080a5164 (0x080A5164-0x080A5168) */
void sub_080a5164(void)
{
}

/* sub_080a5168 (0x080A5168-0x080A5184) */
void sub_080a5168(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    ActorDie();
}

/* sub_080a5184 (0x080A5184-0x080A5188) */
void sub_080a5184(void)
{
}

/* sub_080a5188 (0x080A5188-0x080A51DC) */
void sub_080a5188(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk42 = 12;
    t = gCurTask;
    t->unk04 = (u32)sub_080a51dc;
    t->unk38 = gUnk_08754290;
    TaskFaceLikeParent();
    ActorLoadDef(gUnk_08748B8C);
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A64);
}

/* sub_080a51dc (0x080A51DC-0x080A5220) */
void sub_080a51dc(void)
{
    CallTableEntry(gCurTask->unk15, 2, gUnk_08748A6C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* sub_080a5220 (0x080A5220-0x080A523C) */
void sub_080a5220(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748A64);
}

/* sub_080a523c (0x080A523C-0x080A528C) */
void sub_080a523c(void)
{
    struct Task *t;
    s16 x;
    s16 y;

    t = gCurTask;
    x = t->unk48 + 16;
    y = t->unk4A + 32;
    sub_08067120(x, y, 1, 1);
    t = gCurTask;
    x = t->unk48 - 16;
    y = t->unk4A + 32;
    sub_08067120(x, y, -1, 1);
}

/* sub_080a528c (0x080A528C-0x080A52C8) */
void sub_080a528c(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk7A = 0;
    gCurTask->unk18 = 0;
    gCurTask->unk1C = 0;
    TaskYieldTrampoline(2);
    sub_080a523c();
    TaskYieldTrampoline(84);
    sub_080a523c();
    TaskSleepForever();
}

/* sub_080a52c8 (0x080A52C8-0x080A5304) */
void sub_080a52c8(void)
{
    struct Task *t = gCurTask;

    if (t->unk18 <= 0)
    {
        if (t->unk1C != 0)
            t->unk3C = 3;
        else
            t->unk3C = 2;
        gCurTask->unk18 = 2;
        gCurTask->unk1C ^= 1;
    }
    gCurTask->unk18--;
}

/* sub_080a5304 (0x080A5304-0x080A5320) */
void sub_080a5304(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    ActorDestroy();
}

/* sub_080a5320 (0x080A5320-0x080A5324) */
void sub_080a5320(void)
{
}

/* sub_080a5324 (0x080A5324-0x080A5388) */
void sub_080a5324(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 10;
    t = gCurTask;
    t->unk38 = gUnk_0874C500;
    TaskFaceLikeParent();
    gCurTask->unk3C = 0;
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

/* sub_080a5388 (0x080A5388-0x080A53A8) */
void sub_080a5388(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080a53a8;
    TaskSleepForever();
}

/* sub_080a53a8 (0x080A53A8-0x080A5484) */
void sub_080a53a8(void)
{
    vs16 *arr;
    s16 i;
    s16 dx;
    s16 dy;
    u32 *tbl;
    s32 ok;

    arr = gTaskSlotTypes;
    i = gCurTask->unk44;
    if ((s16)arr[i] != -1) {
        if ((u8)TaskHasSameSerial(i) == 1) {
            struct Task *o = &gTasks[gCurTask->unk44];

            switch (o->unk3C) {
            case 30:
            case 31:
            case 32:
            case 33:
                dx = -20;
                dy = -30;
                ok = 1;
                tbl = gUnk_08748C0C;
                break;
            case 35:
                dx = 40;
                dy = 8;
                ok = 1;
                tbl = gUnk_08748C28;
                break;
            case 34:
                dx = 40;
                dy = 16;
                ok = 1;
                tbl = gUnk_08748C0C;
                break;
            default:
                ok = 0;
                break;
            }
            if (ok != 0) {
                gCurTask->unk48 = o->unk48 + dx * o->unk43;
                gCurTask->unk4A = dy + o->unk4A;
                ActorCheckHitsWithBox((s32)tbl);
            }
        }
    }
}

/* sub_080a5484 (0x080A5484-0x080A54A4) */
void sub_080a5484(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080a54a4;
    TaskSleepForever();
}

/* sub_080a54a4 (0x080A54A4-0x080A54E4) */
void sub_080a54a4(void)
{
    struct Task **c = &gCurTask;

    if (ActorCheckHitsWithBox((s32)gUnk_08748C44) != 0)
    {
        gUnk_02007D00[8] = (*c)->unk7E;
        sub_080685ec((*c)->unk7E, (*c)->unk44, 0);
    }
}

/* sub_080a54e4 (0x080A54E4-0x080A5524) */
void sub_080a54e4(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk42 = 10;
    gCurTask->unk38 = gUnk_08754290;
    CallTableEntry(gCurTask->unk73, 3, gUnk_08748A74);
}

/* sub_080a5524 (0x080A5524-0x080A556C) */
void sub_080a5524(void)
{
    struct Task **c;
    u16 z;
    u16 x;

    TaskStop();
    c = &gCurTask;
    z = 0;
    for (;;) {
        (*c)->unk3C = 1;
        TaskYieldTrampoline(1);
        x = (*c)->unk3C;
        (*c)->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(2);
        (*c)->unk3C = z;
        TaskYieldTrampoline(1);
        x = (*c)->unk3C;
        (*c)->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(3);
    }
}

/* sub_080a556c (0x080A556C-0x080A55AC) */
void sub_080a556c(void)
{
    struct Task **c;
    u32 *base;
    u32 *p;
    s32 v;
    s32 i;

    v = gCurTask->unk50;
    gCurTask->unk3C = 4;
    gCurTask->unk58 = 128 << 12;
    c = &gCurTask;
    base = gUnk_08748A80;
    for (;;)
    {
        p = base;
        for (i = 4; i >= 0; i--)
        {
            (*c)->unk50 = v;
            (*c)->unk4C = *p++;
            TaskYieldTrampoline(10);
        }
    }
}

/* sub_080a55ac (0x080A55AC-0x080A55EC) */
void sub_080a55ac(void)
{
    struct Task **c;
    u32 *base;
    u32 *p;
    s32 v;
    s32 i;

    v = gCurTask->unk50;
    gCurTask->unk3C = 5;
    gCurTask->unk58 = 128 << 13;
    c = &gCurTask;
    base = gUnk_08748A94;
    for (;;)
    {
        p = base;
        for (i = 6; i >= 0; i--)
        {
            (*c)->unk50 = v;
            (*c)->unk4C = *p++;
            TaskYieldTrampoline(5);
        }
    }
}

/* sub_080a55ec (0x080A55EC-0x080A5644) */
void sub_080a55ec(void)
{
    struct Task *t;

    sub_08066088(0);
    sub_08066144();
    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)sub_080653ec;
    t->unk42 = 11;
    gCurTask->unk38 = gUnk_08753BB4;
    sub_08063a00((u32)gUnk_087495EC);
    CallTableEntry(gCurTask->unk73, 1, gUnk_08748EB8);
}
