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
extern u8 gUnk_03005550[];
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
extern void sub_080224b0(void);
extern u8 sub_080227a4(struct Task *t);
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

void sub_080a5644(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_080a56b0;
    t->unk28 = 0;
    t->unk2C = -1;
    t->unk30 = -1;
    t->unk34 = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = (s16)sub_08065f5c();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 24, gUnk_08748EBC);
}

void sub_080a5694(void)
{
    CallTableEntry(gCurTask->unk14, 24, gUnk_08748EBC);
}

void sub_080a56b0(void)
{
    struct Task *t;
    u32 rr;
    u32 *q;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 24, gUnk_08748F1C);
    if (gCurTask->unk30 != -1)
    {
        ActorSetAttackBox(gUnk_08748D38[gCurTask->unk30]);
        gUnk_02007D00[1] = gCurTask->unk78;
        sub_08068f68();
        switch (gCurTask->unk7C)
        {
        case 1:
        case 2:
            if ((u8)sub_080a720c() == 1)
            {
                gCurTask->unk7C = 0;
                t = gCurTask;
                t->unk78 = gUnk_02007D00[1];
                t->unk30 = 2;
                ActorSetState(22);
                sub_080a7168();
            }
            break;
        case 6:
            gUnk_02007D00[0] = gCurTask->unk7F;
            switch (gCurTask->unk82)
            {
            case 4:
                gUnk_03001F2C = sub_080a6e1c();
                q = &gUnk_03002448;
                rr = RandomRange(16);
                *q = rr;
                if ((s32)rr < gUnk_08748D6C[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(22);
                    sub_080a7168();
                }
                else if ((s32)rr < gUnk_08748D70[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(16);
                    sub_080a7168();
                }
                break;
            case 2:
            case 3:
                gCurTask->unk30 = 2;
                ActorSetState(23);
                sub_080a7168();
                break;
            }
            break;
        }
        ActorReactToHit();
    }
}

void sub_080a57d4(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    gCurTask->unk15 = 0;
    gCurTask->unk4C = (gViewRect[0] + 216) << 16;
    gCurTask->unk50 = (gViewRect[2] + 44) << 16;
    if (gUnk_03001F30 == 0)
    {
        for (i = 0; i < gUnk_03002350; i++)
        {
            sp.unk00 = 18;
            sp.unk04 = 120;
            sp.unk08 = 0;
            sp.unk09 = i;
            sp.unk0C = gUnk_08748D28[i + (gUnk_03002350 - 1) * 4] + gViewRect[0];
            sp.unk0E = gViewRect[2];
            sp.unk10 = gCurTask->unk8C->unk20;
            sp.unk0A = 0;
            CreateActorFromDesc(&sp, 1);
        }
    }
    else
        gUnk_02007D00[2] = gPlayerCount;
    gCurTask->unk43 = 1;
    TaskSetFrame(4);
    while (gUnk_02007D00[2] != gUnk_03002350)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(1);
    gCurTask->unk30 = 2;
    sub_08066544();
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(185, (s16)(t->unk48 + 8), t->unk4A, t->unk8C->unk20);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(18);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(188, (s16)(t->unk48 - 32), t->unk4A, t->unk8C->unk20 | (240 << 8));
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk60 = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk60 = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk60 = -0x10000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(20);
    gCurTask->unk43 = 255;
    TaskSetFrame(60);
    TaskYieldTrampoline(10);
    CreateChildTaskHere(184, 0);
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(9);
    TaskSleepForever();
}

void sub_080a5a78(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a5aa0(void)
{
    struct Task *t;
    s32 v;

    gCurTask->unk15 = 1;
    t = gCurTask;
    t->unk28 &= ~128;
    t->unk30 = 0;
    v = TaskFindNearestPlayer();
    gUnk_02007D00[5] = v;
    if (gUnk_0300244C != 0)
    {
        if (gCurTask->unk2C != v)
        {
            gCurTask->unk20 = 0;
            gCurTask->unk24 = 0;
            gCurTask->unk2C = gUnk_02007D00[5];
        }
    }
    sub_080a6e98();
}

void sub_080a5af4(void)
{
    struct Task *pt;
    s16 *px;
    vu16 *pv;
    vu16 *f9;
    s32 d;

    gCurTask->unk1C--;
    if (gCurTask->unk1C == 0)
    {
        sub_080a7190();
        return;
    }
    pt = &gTasks[gUnk_02007D00[5]];
    px = &pt->unk48;
    d = (*px + gCurTask->unk18) - gCurTask->unk48;
    switch (gCurTask->unk24)
    {
    case 0:
        if (gCurTask->unk34 & 2)
        {
            if (d < 0)
                gCurTask->unk43 = 255;
            else
                gCurTask->unk43 = 1;
        }
        else
            TaskFaceNearestPlayer();
        if (abs(d) <= 1)
        {
            gCurTask->unk24 = 1;
            gCurTask->unk54 = 0;
            gUnk_02007D00[7] = 0;
        }
        else if (abs(d) <= 27)
        {
            gCurTask->unk20 &= ~2;
            if (d > 0)
                gCurTask->unk54 = 128 << 9;
            else
                gCurTask->unk54 = -0x10000;
        }
        else
        {
            gCurTask->unk20 |= 2;
            if (d > 0)
                gCurTask->unk54 = 128 << 10;
            else
                gCurTask->unk54 = -0x20000;
        }
        gUnk_02007D00[6] = pt->unk48;
        break;
    case 1:
        gCurTask->unk34 &= ~6;
        if ((u8)sub_080a6f38(gCurTask->unk48, gCurTask->unk4A) == 1)
        {
            gCurTask->unk24 = 2;
            gCurTask->unk54 = 0;
        }
        else
        {
            gCurTask->unk48 = *px + gCurTask->unk18;
            gCurTask->unk4C = gCurTask->unk48 << 16;
            TaskFaceNearestPlayer();
            f9 = gPlayerHeldKeys;
            pv = &f9[gUnk_02007D00[5]];
            if ((*pv & 48) && *px != gUnk_02007D00[6])
            {
                if ((u16)(*pv & 16) != 0)
                {
                    if (gUnk_02007D00[7] < 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]++;
                }
                else
                {
                    if (gUnk_02007D00[7] > 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]--;
                }
                if (abs(gUnk_02007D00[7]) == 15)
                    gCurTask->unk24 = 0;
            }
            else if (pt->unk54 == 0)
            {
                gCurTask->unk24 = 2;
                gCurTask->unk54 = 0;
            }
        }
        gUnk_02007D00[6] = pt->unk48;
        break;
    case 2:
        TaskFaceNearestPlayer();
        if (*px != gUnk_02007D00[6])
        {
            if ((u8)sub_080a6f38((s16)(*px + gCurTask->unk18), gCurTask->unk4A) == 0)
            {
                gCurTask->unk24 = 0;
                gCurTask->unk20 &= ~1;
                break;
            }
        }
        if (gCurTask->unk3C == 65)
            gCurTask->unk20 |= 1;
        break;
    }
    if ((u8)sub_080a6f74() == 1)
    {
        gCurTask->unk54 = 0;
        gCurTask->unk24 = 2;
        if (!(gCurTask->unk34 & 1))
        {
            gCurTask->unk34 |= 1;
            gCurTask->unk1C = RandomRange(30) + 30;
        }
    }
    TaskUpdateFlip();
    if (!(gCurTask->unk34 & 6))
    {
        if (abs(TaskGetNearestPlayerDx()) <= 31)
        {
            ActorSetState(gUnk_08748DA8[pt->unk7A][RandomRange(8)]);
            sub_080a7168();
        }
    }
}

void sub_080a5dd0(void)
{
    gCurTask->unk15 = 2;
    if (!(gCurTask->unk34 & 2))
    {
        if (gCurTask->unk43 == 1)
            gCurTask->unk18 = 64;
        else
            gCurTask->unk18 = -64;
    }
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 &= 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e30(void)
{
    sub_080227a4(gCurTask);
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a5e60(void)
{
    gCurTask->unk15 = 3;
    gCurTask->unk18 = -gCurTask->unk18;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 = 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e9c(void)
{
    sub_080227a4(gCurTask);
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a5ecc(void)
{
    gCurTask->unk15 = 4;
    gCurTask->unk1C = 48;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5ef0(void)
{
    sub_080227a4(gCurTask);
    if (gCurTask->unk14 != 4)
        TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a5f20(void)
{
    struct Task *t;

    gCurTask->unk15 = 5;
    gCurTask->unk34 = 4;
    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->unk43 == 1)
        t->unk18 = -64;
    else
        t->unk18 = 64;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5f7c(void)
{
    sub_080227a4(gCurTask);
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a5fac(void)
{
    gCurTask->unk15 = 6;
    if (gCurTask->unk18 < 0)
        gUnk_02007D00[6] = 1;
    else
        gUnk_02007D00[6] = 0;
    {
        struct Task *t2 = &gTasks[TaskFindNearestPlayer()];

        gUnk_02007D00[7] = t2->unk48;
    }
    gCurTask->unk20 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskSetFrame(61);
    sub_080a6e98();
}

void sub_080a6020(void)
{
    s32 d;
    s32 x;

    sub_080227a4(gCurTask);
    x = gCurTask->unk48;
    d = x - gUnk_02007D00[7];
    if (d >= 0 ? d <= 5 : gUnk_02007D00[7] - x <= 5)
    {
        if (abs(TaskGetNearestPlayerDy()) <= 47)
            ActorSetState(gUnk_08748E48[RandomRange(16)]);
        else
            ActorSetState(gUnk_08748E68[RandomRange(16)]);
        if (gCurTask->unk14 == 3)
            gUnk_02007D00[6]++;
        gCurTask->unk18 = gUnk_08748D60[gUnk_02007D00[6]];
        gCurTask->unk34 = 2;
        sub_080a7168();
    }
}

void sub_080a60d8(void)
{
    gCurTask->unk15 = 7;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(224 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(82);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
}

void sub_080a6130(void)
{
    sub_080227a4(gCurTask);
    if (abs(TaskGetNearestPlayerDx()) <= 43)
    {
        gCurTask->unk34 = 0;
        if (gCurTask->unk78 >= gUnk_02007D00[3] >> 1)
            ActorSetState(gUnk_08748E88[RandomRange(8)]);
        else
            ActorSetState(gUnk_08748E98[RandomRange(8)]);
        sub_080a7168();
    }
}

void sub_080a61b4(void)
{
    gCurTask->unk15 = 9;
    gCurTask->unk30 = 0;
    gUnk_02007D00[7] = 0;
    gCurTask->unk7A = 0;
    TaskSetMotionY(-0x26000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk3C++;
    if (gCurTask->unk7A == 0)
    {
        while (gCurTask->unk7A == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6264(void)
{
    sub_080a6fb4();
    if (gCurTask->unk14 != 9)
        sub_080a7168();
}

void sub_080a6280(void)
{
    gCurTask->unk15 = 8;
    gUnk_02007D00[7] = 0;
    gCurTask->unk7A = 0;
    TaskSetMotionY(-0x33000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk3C++;
    if (gCurTask->unk7A == 0)
    {
        while (gCurTask->unk7A == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6330(void)
{
    sub_080a6fb4();
    if (gCurTask->unk14 != 8)
        sub_080a7168();
}

void sub_080a634c(void)
{
    gCurTask->unk15 = 10;
    TaskStop();
    gCurTask->unk30 = 0;
    TaskSetFrame(81);
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a638c(void)
{
    if (gCurTask->unk14 != 10)
        sub_080a7168();
}

void sub_080a63a4(void)
{
    gCurTask->unk15 = 11;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a63d8(void)
{
    if (gCurTask->unk14 != 11)
        sub_080a7168();
}

void sub_080a63f0(void)
{
    gCurTask->unk15 = 12;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6420(void)
{
    if (gCurTask->unk14 != 12)
        sub_080a7168();
}

void sub_080a6438(void)
{
    struct Task *t;
    s32 i;

    gCurTask->unk15 = 13;
    t = gCurTask;
    t->unk60 = 144 << 7;
    t->unk68 = 128 << 11;
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023D4 <= 43)
        gUnk_02007D00[7] = 4;
    else
        gUnk_02007D00[7] = 2;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_02007D00[7])
    {
        PlaySfx(0x225);
        TaskSetFrame(114);
        TaskYieldTrampoline(1);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk6E++;
        } while ((s16)gCurTask->unk6E <= 6);
        gCurTask->unk6C++;
    }
    TaskSetFrame(122);
    TaskYieldTrampoline(2);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a650c(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->unk7A != 0)
    {
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6544(void)
{
    gCurTask->unk15 = 16;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 15;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6574(void)
{
    if (gCurTask->unk14 != 16)
        sub_080a7168();
}

void sub_080a658c(void)
{
    gCurTask->unk15 = 15;
    TaskStop();
    TaskSetFrame(111);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskSetMotionY(128 << 10, 144 << 7, 128 << 11);
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_080261d4(2);
    PlaySfx(504);
    sub_0806e9b4(0, 0, 3);
    gCurTask->unk30 = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6628(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->unk14 != 15)
        sub_080a7168();
}

void sub_080a6650(void)
{
    gCurTask->unk15 = 14;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 17;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6680(void)
{
    if (gCurTask->unk14 != 14)
        sub_080a7168();
}

void sub_080a6698(void)
{
    gCurTask->unk15 = 17;
    sub_080a7080();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a66c8(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->unk7A != 0)
    {
        StopSfxOnPlayer(gUnk_02007D00[4], 137 << 2);
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6710(void)
{
    gCurTask->unk15 = 18;
    TaskStop();
    sub_080a7080();
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6734(void)
{
    sub_080227a4(gCurTask);
    if (gCurTask->unk14 != 18)
        sub_080a7168();
}

void sub_080a6754(void)
{
    struct Task *t;

    gCurTask->unk15 = 19;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(12);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->unk48 - t->unk43 * 8), (s16)(t->unk4A + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk30 = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(24);
    TaskSetFrame(60);
    TaskYieldTrampoline(28);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a680c(void)
{
    s32 v;

    sub_080227a4(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->unk54 = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->unk14 != 19)
        sub_080a7168();
}

void sub_080a6868(void)
{
    struct Task *t;

    gCurTask->unk15 = 20;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->unk48 - t->unk43 * 8), (s16)(t->unk4A + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    TaskSetFrame(97);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->unk48 - t->unk43 * 8), (s16)(t->unk4A + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C--;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    TaskSetFrame(88);
    TaskYieldTrampoline(10);
    ActorSetState(21);
    TaskSleepForever();
}

void sub_080a6988(void)
{
    s32 v;

    sub_080227a4(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->unk54 = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->unk14 != 20)
        sub_080a7168();
}

void sub_080a69e4(void)
{
    struct Task *t;

    gCurTask->unk15 = 21;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(105);
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->unk48 - t->unk43 * 8), (s16)(t->unk4A + 8), 0);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(20);
    gCurTask->unk30 = 0;
    TaskSetFrame(60);
    TaskYieldTrampoline(48);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a6aac(void)
{
    sub_080227a4(gCurTask);
    if (gUnk_02007D00[7] <= 25)
    {
        gUnk_02007D00[7]++;
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2
             && gUnk_02007D00[7] >= gUnk_08748D4C[(s16)gCurTask->unk6C]; gCurTask->unk6C++)
            ;
        TaskSetMotionXFacing(gUnk_08748D50[(s16)gCurTask->unk6C], 0x5A5A5A5A);
    }
    if (gCurTask->unk14 != 21)
        sub_080a7168();
}

void sub_080a6b3c(void)
{
    struct Task *t;

    gCurTask->unk15 = 22;
    gUnk_02007D00[6] = gCurTask->unk7A;
    gUnk_02007D00[7] = 20;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    t = gCurTask;
    if (t->unk7A == 0)
    {
        CreateChildTask(188, (s16)(t->unk48 + t->unk43 * 4), (s16)(t->unk4A - 4),
                     t->unk8C->unk20 | (240 << 8));
        gCurTask->unk60 = 168 << 5;
        gCurTask->unk68 = 192 << 10;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(70);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
    else
    {
        CreateChildTask(188, (s16)(t->unk48 + t->unk43 * 16), (s16)(t->unk4A + 4),
                     t->unk8C->unk20 | (240 << 8));
        TaskStop();
        TaskSetFrame(69);
    }
    TaskSleepForever();
}

void sub_080a6c3c(void)
{
    s32 *pD;
    u8 *b3v;
    s32 *pE;
    s32 w;
    s32 v;

    sub_080227a4(gCurTask);
    pD = gUnk_02007D00;
    v = pD[7];
    if (v == 0)
    {
        gCurTask->unk30 = v;
        if (gCurTask->unk7A != 0)
        {
            if (pD[6] == 0)
            {
                TaskStop();
                ActorSetState(10);
                TaskSetEntry(sub_080a5694, gCurTaskIdx);
            }
            else
            {
                TaskStop();
                gCurTask->unk24 = v;
                sub_080a7190();
            }
        }
    }
    else
    {
        w = v - 1;
        pD[7] = w;
        pD[5] = 0;
        b3v = (u8 *)gUnk_08748D78;
        pE = pD;
        while (w >= *(u8 *)(pD[5] + (u32)b3v))
            pD[5]++;
        TaskSetMotionXFacing(gUnk_08748D80[pE[5]], 0x5A5A5A5A);
        if (gCurTask->unk7A != 0)
        {
            TaskStopY();
            pE[6] = gCurTask->unk7A;
            TaskSetFrame(69);
        }
    }
}

void sub_080a6d08(void)
{
    struct Task *t;

    gCurTask->unk15 = 23;
    TaskStop();
    gUnk_02007D00[6] = gCurTask->unk7A;
    t = gCurTask;
    if (t->unk7A == 0)
    {
        t->unk60 = 168 << 5;
        t->unk68 = 192 << 10;
    }
    gUnk_02007D00[7] = 60;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    TaskSetFrame(69);
    TaskSleepForever();
}

void sub_080a6d60(void)
{
    u16 v;

    sub_080227a4(gCurTask);
    if (gPlayerStates[gUnk_02007D00[0]].unk40 & 4) {
        if (gCurTask->unk7A != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                gUnk_02007D00[6] = gCurTask->unk7A;
            }
            if (--gUnk_02007D00[7] == 0) {
                gCurTask->unk30 = 1;
                v = gUnk_08748EA8[RandomRange(2)];
                ActorSetState(v);
                TaskSetEntry(sub_080a5694, gCurTaskIdx);
            }
        }
    } else {
        if (gCurTask->unk7A != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                v = 10;
            } else {
                v = 2;
            }
            ActorSetState(v);
            TaskSetEntry(sub_080a5694, gCurTaskIdx);
        } else {
            TaskSetFrame(70);
        }
    }
}

s32 sub_080a6e1c(void)
{
    s32 x = gCurTask->unk78;
    s32 q = gUnk_02007D00[3] >> 2;

    if (x < q)
        return 0;
    if (x < gUnk_02007D00[3] >> 1)
        return 1;
    if (x < q * 3)
        return 2;
    return 3;
}

void sub_080a6e58(void)
{
    if (gCurTask->unk78 >= gUnk_02007D00[3] >> 1)
        gCurTask->unk1C = RandomRange(54) + 48;
    else
        gCurTask->unk1C = RandomRange(84) + 96;
}

void sub_080a6e98(void)
{
    struct Task **c = &gCurTask;
    struct Task **d;
    u8 *a = gUnk_08748D98;
    u8 *b = a + 8;
    struct Task *t;
    struct Task *u;
    s32 ix;
    s32 n;

    for (;;)
    {
        t = *c;
        if (t->unk20 & 2)
        {
            ix = t->unk3C - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)b));
        }
        else
        {
            ix = t->unk3C - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)a));
        }
        t = *c;
        n = t->unk20 & 1;
        d = &gCurTask;
        if (n == 0)
        {
            if ((t->unk43 == 1 && t->unk54 >= 0) || (t->unk43 == -1 && t->unk54 <= 0))
            {
                u = *c;
                u->unk3C++;
                if ((s16)u->unk3C > 68)
                    u->unk3C = 61;
            }
            else
            {
                u = *d;
                u->unk3C--;
                if ((s16)u->unk3C <= 60)
                    u->unk3C = 68;
            }
        }
    }
}

s32 sub_080a6f38(s16 x, s16 y)
{
    if ((u16)sub_08021bb4(x, y, 1, 0) == 0 && (u16)sub_08021bb4(x, y, -1, 0) == 0)
        return 0;
    return 1;
}

s32 sub_080a6f74(void)
{
    u8 v = sub_080227a4(gCurTask);

    if (((v & 1) && gCurTask->unk54 < 0) || ((v & 2) && gCurTask->unk54 > 0))
        return 1;
    return 0;
}

void sub_080a6fb4(void)
{
    s32 k;
    s32 g;

    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gUnk_02007D00[7] == 0 && gCurTask->unk7A == 0 && gCurTask->unk58 > 0)
    {
        if (gUnk_02007D00[6] == 0)
        {
            gCurTask->unk34 = 0;
            gUnk_02007D00[7]++;
            gUnk_02007D00[4] = TaskGetNearestPlayerDx();
            if (TaskGetNearestPlayerDy() < 0)
                k = 0;
            else
            {
                g = gUnk_02007D00[4];
                if (abs(g) <= 15)
                    k = 1;
                else if ((g > 0 && gCurTask->unk43 == 1) ||
                         (g < 0 && gCurTask->unk43 == -1))
                    k = 2;
                else
                    k = 3;
            }
            gUnk_02007D00[6] = gUnk_08748E28[k][RandomRange(4)];
            if (gUnk_02007D00[6] == 0)
                return;
        }
        ActorSetState((u16)gUnk_02007D00[6]);
    }
}

void sub_080a7080(void)
{
    gUnk_02007D00[4] = -1;
    TaskSetFrame(98);
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_02007D00[4] = PlaySfx(137 << 2);
    gCurTask->unk7A = 0;
    gCurTask->unk58 = -0x40000;
    gCurTask->unk60 = 128 << 8;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskStopY();
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk58 = 128 << 11;
    gCurTask->unk60 = -0x8000;
    TaskSetFrame(101);
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
}

void sub_080a7168(void)
{
    gCurTask->unk28 |= 128;
    TaskSetEntry(sub_080a5694, gCurTaskIdx);
}

void sub_080a7190(void)
{
    struct Task *pt = &gTasks[TaskFindNearestPlayer()];

    if (gCurTask->unk34 & 1)
        ActorSetState(gUnk_08748DC8[RandomRange(16)]);
    else
        ActorSetState(gUnk_08748DE8[pt->unk7A][RandomRange(16)]);
    gCurTask->unk34 = 0;
    sub_080a7168();
}

s32 sub_080a720c(void)
{
    u8 k;

    if (gCurTask->unk28 & 128)
        gCurTask->unk28++;
    if (gCurTask->unk28 & 3)
    {
        gCurTask->unk28--;
        k = sub_080a6e1c();
        if (RandomRange(16) < gUnk_08748D74[k])
            return 1;
    }
    return 0;
}

s32 sub_080a7260(void)
{
    gBg2Cnt |= 64;
    gBg3Cnt |= 64;
    gCurTask->unk28 |= 2;
    sub_0806619c(13, (u32)sub_080a72b0, (u32)gUnk_082F427C, 16, 1);
    return 0;
}

void sub_080a72b0(void)
{
    gBgMosaic = gUnk_08748EAC[gUnk_02006190[3] >> 1];
    if (gUnk_02006190[3] == 0)
    {
        gBg2Cnt &= 0xFFBF;
        gBg3Cnt &= 0xFFBF;
        sub_0806621c();
    }
}

s32 sub_080a72fc(void)
{
    TaskStop();
    ActorSetHitReactions((u32)gUnk_08749B30);
    gCurTask->unk30 = -1;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 sub_080a7334(void)
{
    struct Task *t;

    switch (gCurTask->unk14)
    {
    case 1:
        t = gCurTask;
        t->unk24 = 1;
        t->unk54 = 0;
        break;
    case 8:
    case 13:
    case 15:
    case 17:
        TaskTurnAroundAndReverseX();
        break;
    }
    return 0;
}

void sub_080a73b4(void)
{
    struct Task *t = gCurTask;

    t->unk0C = (u32)sub_080653ec;
    t->unk04 = (u32)sub_080a73fc;
    t->unk38 = gUnk_08753BB4;
    t->unk42 = 4;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748F7C);
}

void sub_080a73fc(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->unk15, 2, gUnk_08748F84);
}

void sub_080a741c(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08748F7C);
}

void sub_080a7438(void)
{
    struct ActorSpawn sp;

    gCurTask->unk15 = 0;
    TaskStop();
    gCurTask->unk7A = 0;
    gCurTask->unk60 = 168 << 5;
    gCurTask->unk68 = 192 << 10;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    sp.unk00 = 18;
    sp.unk04 = 120;
    sp.unk08 = 1;
    sp.unk09 = 0;
    sp.unk0C = 0;
    sp.unk0E = 0;
    sp.unk10 = gCurTask->unk8C->unk20 + (128 << 5);
    sp.unk0A = 0;
    CreateActorFromDescHere(&sp, 1);
    TaskGetScreenPosSlot(gCurTaskIdx);
    gCurTask->unk30 = gUnk_030023B4;
    TaskGetNearestPlayerScreenPos();
    if ((u32)(gUnk_030023B4 - 88) > 64)
        gCurTask->unk28 = 128;
    else if (gCurTask->unk30 <= 127)
        gCurTask->unk28 = 64;
    else
        gCurTask->unk28 = 176;
    if (gCurTask->unk30 < gCurTask->unk28)
        gCurTask->unk43 = 1;
    else
        gCurTask->unk43 = -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetFrame(21);
    gCurTask->unk2C = abs(gCurTask->unk30 - gCurTask->unk28) >> 1;
    TaskSetMotionY(-((gCurTask->unk2C >> 1) << 13), 128 << 6, 192 << 10);
    if (gCurTask->unk58 == 0)
        gCurTask->unk58 = -0x10000;
    gCurTask->unk7A = 0;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a75a0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080a741c, gCurTaskIdx);
}

void sub_080a75c8(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(14);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    t = gCurTask;
    CreateChildTask(186, (s16)(t->unk48 - t->unk43 * 2), (s16)(t->unk4A - 1), t->unk8C->unk20);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk7A = 0;
    gCurTask->unk58 = -0x30000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk7A = 0;
    gCurTask->unk58 = -0x20000;
    gCurTask->unk60 = 128 << 9;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    TaskYieldTrampoline(5);
    gCurTask->unk7A = 0;
    gCurTask->unk58 = -0x44000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C++;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk3C++;
    TaskYieldTrampoline(9);
    TaskStop();
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetFrame(45);
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    TaskSetFrame(57);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    gCurTask->unk7A = 0;
    gCurTask->unk58 = -0x70000;
    while (gCurTask->unk4A > 10)
        TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(30);
    FadeOutBgm(8);
    TaskYieldTrampoline(32);
    sub_08066fc0(0, 128, 104);
    ActorDestroy();
}

void sub_080a787c(void)
{
}

void sub_080a7880(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080a78a0;
    TaskSleepForever();
}
